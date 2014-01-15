/*
 *  linux/arch/arm/kernel/traps.c
 *
 *  Copyright (C) 1995-2009 Russell King
 *  Fragments that appear the same as linux/arch/i386/kernel/traps.c (C) Linus Torvalds
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  'traps.c' handles hardware exceptions after we have saved some state in
 *  'linux/arch/arm/lib/traps.S'.  Mostly a debugging aid, but will probably
 *  kill the offending process.
 */
#include <linux/signal.h>
#include <linux/personality.h>
#include <linux/kallsyms.h>
#include <linux/spinlock.h>
#include <linux/uaccess.h>
#include <linux/hardirq.h>
#include <linux/kdebug.h>
#include <linux/module.h>
#include <linux/kexec.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/sched.h>
#if defined(CONFIG_MP_DEBUG_TOOL_CODEDUMP)
#include <asm/atomic.h>
#endif /* CONFIG_MP_DEBUG_TOOL_CODEDUMP */
#include <asm/cacheflush.h>
#include <asm/system.h>
#include <asm/unistd.h>
#include <asm/traps.h>
#include <asm/unwind.h>
#include <asm/tls.h>

#include "signal.h"
#if defined(CONFIG_MP_DEBUG_TOOL_CODEDUMP)
#include <linux/fs.h>
#endif /* CONFIG_MP_DEBUG_TOOL_CODEDUMP */
static const char *handler[]= { "prefetch abort", "data abort", "address exception", "interrupt" };

void *vectors_page;

#if defined(CONFIG_MP_DEBUG_TOOL_CODEDUMP)
#ifdef CONFIG_DEBUG_USER
#ifdef CONFIG_SHOW_FAULT_TRACE_INFO
unsigned int user_debug = 0xff;
#else
unsigned int user_debug;
#endif
#endif /* CONFIG_MP_DEBUG_TOOL_CODEDUMP */

#if defined(CONFIG_MP_DEBUG_TOOL_KDEBUG)
#ifdef CONFIG_KDEBUGD
#include <linux/bootmem.h>
#include <linux/vmalloc.h>
#endif
#endif /* CONFIG_MP_DEBUG_TOOL_KDEBUG */

static int __init user_debug_setup(char *str)
{
	get_option(&str, &user_debug);
	return 1;
}
__setup("user_debug=", user_debug_setup);
#endif

static void dump_mem(const char *, const char *, unsigned long, unsigned long);

#if defined(CONFIG_MP_DEBUG_TOOL_CODEDUMP)
#ifdef CONFIG_SHOW_FAULT_TRACE_INFO
void show_pid_maps(struct task_struct *task)
{
    struct vm_area_struct *vma;
    struct file *file;
    char path_buf[256];
    unsigned long ino = 0;
    dev_t dev = 0;
    
    printk(KERN_CONT "-----------------------------------------------------------\n");
    printk(KERN_CONT "* dump maps on pid (%d)\n", task->pid);
    printk(KERN_CONT "-----------------------------------------------------------\n");
	
    if (!down_read_trylock(&task->mm->mmap_sem))
    {
        printk(KERN_ERR "Error in getting try lock");
	return;
    }
    
    vma = task->mm->mmap;
    while(vma) {
        file = vma->vm_file;
        if (file) {
            struct inode *inode = file->f_path.dentry->d_inode;
			
			//struct path	*tt = (struct path	*)&file->f_path;

            dev = inode->i_sb->s_dev;
            ino = inode->i_ino;
        }
   
        printk(KERN_CONT "%08lx-%08lx %c%c%c%c %08lx %02x:%02x %lu ",
                            vma->vm_start,
                            vma->vm_end,
                            vma->vm_flags & VM_READ ? 'r' : '-',
                            vma->vm_flags & VM_WRITE ? 'w' : '-',
                            vma->vm_flags & VM_EXEC ? 'x' : '-',
                            vma->vm_flags & VM_MAYSHARE ? 's' : 'p',
                            vma->vm_pgoff << PAGE_SHIFT,
                            MAJOR(dev), MINOR(dev), ino);
        if (file) {
            char* p = d_path((&file->f_path), path_buf, 256);
            
            if (!IS_ERR(p)) 
                printk(KERN_CONT "%s", p);
        }
        printk(KERN_CONT "\n");

        vma = vma->vm_next;
    }
    up_read(&task->mm->mmap_sem);

    printk(KERN_CONT "-----------------------------------------------------------\n\n");
}


void __show_user_stack(struct task_struct *task, unsigned long sp)
{
    struct vm_area_struct *vma;
    vma = find_vma(task->mm, task->user_ssp);
    if (!vma) {
        printk(KERN_CONT "pid(%d) : printing user stack failed.\n", (int)task->pid);
        return;
    }
    
    if (sp < vma->vm_start) {
        printk(KERN_CONT "pid(%d) : seems stack overflow.\n"
                "  sp(%lx), stack vma (0x%08lx ~ 0x%08lx)\n",
                (int)task->pid, sp, vma->vm_start, vma->vm_end);
        return;
    }

    printk(KERN_CONT "pid(%d) stack vma (0x%08lx ~ 0x%08lx)\n", 
            (int)task->pid, vma->vm_start, vma->vm_end);
    dump_mem(KERN_CONT, "User Stack: ", sp, task->user_ssp);
}

#ifdef CONFIG_SHOW_THREAD_GROUP_STACK
void __show_user_stack_tg(struct task_struct *task)
{
    struct task_struct *g, *p;
    unsigned long *user_sp = 0;
    unsigned long offset = 0;
    
    printk(KERN_CONT "--------------------------------------------------------\n");
    printk(KERN_CONT "* dump all user stack of pid(%d) thread group\n", (int)task->pid);
    printk(KERN_CONT "--------------------------------------------------------\n");

    offset = THREAD_START_SP - sizeof(struct pt_regs) + offsetof(struct pt_regs, ARM_sp);

    read_lock(&tasklist_lock);
    do_each_thread(g, p) {
        if (task->mm != p->mm)
            continue;
        if (task->pid == p->pid)
            continue;

        user_sp = (unsigned long*)((unsigned long)task_stack_page(p) + offset);

        __show_user_stack(p, *user_sp);
        printk(KERN_CONT "\n");
    } while_each_thread(g, p);
    read_unlock(&tasklist_lock);

    printk(KERN_CONT "--------------------------------------------------------\n\n");
}
#else
#define __show_user_stack_tg(t)
#endif /* CONFIG_SHOW_THREAD_GROUP_STACK */

/*
 *  Assumes that user program uses frame pointer
 *  TODO : consider context safety
 */
void show_user_stack(struct task_struct *task, struct pt_regs *regs)
{
    struct vm_area_struct *vma;
    
    vma = find_vma(task->mm, task->user_ssp);
    if (vma) {
        printk(KERN_CONT "task stack info : pid(%d) stack area (0x%08lx ~ 0x%08lx)\n",
                    (int)task->pid, vma->vm_start, vma->vm_end);
    }
    
    printk(KERN_CONT "-----------------------------------------------------------\n");
    printk(KERN_CONT "* dump user stack\n");
    printk(KERN_CONT "-----------------------------------------------------------\n");

    __show_user_stack(task, regs->ARM_sp);
    printk(KERN_CONT "-----------------------------------------------------------\n\n");
    
    __show_user_stack_tg(task);
}

#ifdef CONFIG_SHOW_PC_LR_INFO
static void dump_mem_kernel(const char *str, unsigned long bottom, unsigned long top)
{
    unsigned long p = bottom & ~31;
    int i;

    printk(KERN_CONT "%s(0x%08lx to 0x%08lx)\n", str, bottom, top);

    for (p = bottom & ~31; p <= top;) {
        printk(KERN_CONT "%04lx: ", p & 0xffff);

        for (i = 0; i < 8; i++, p += 4) {
            if (p < bottom || p > top)
                printk(KERN_CONT "         ");
            else
                printk(KERN_CONT "%08lx ", *(unsigned long*)p);
        }
        printk(KERN_CONT "\n");
    }
}

void show_pc_lr(struct task_struct *task, struct pt_regs *regs)
{
    unsigned long addr_pc_start, addr_lr_start;
    unsigned long addr_pc_end, addr_lr_end;
    struct vm_area_struct *vma; 
    
    printk(KERN_CONT "\n");
    printk(KERN_CONT "--------------------------------------------------------------------------------------\n");
    printk(KERN_CONT "PC, LR MEMINFO\n");
    printk(KERN_CONT "--------------------------------------------------------------------------------------\n");
    printk(KERN_CONT "PC:%lx, LR:%lx\n", regs->ARM_pc, regs->ARM_lr);

    //Basic error handling
    if(regs->ARM_pc > 0x400)
        addr_pc_start = regs->ARM_pc - 0x400;   // pc - 1024 byte
    else
        addr_pc_start = 0;

    if(regs->ARM_pc < 0xfffffC00)
        addr_pc_end = regs->ARM_pc + 0x400;     // pc + 1024 byte
    else
        addr_pc_end = 0xffffffff;

    if(regs->ARM_lr > 0x800)
        addr_lr_start = regs->ARM_lr - 0x800;   // lr - 2048 byte
    else
        addr_lr_start = 0;

    if(regs->ARM_lr < 0xfffffC00)
        addr_lr_end = regs->ARM_lr + 0x400;     // lr + 1024 byte
    else
        addr_lr_end = 0xffffffff;

    //Calculate vma print range according which contain PC, LR
    if(((regs->ARM_pc & 0xfff) < 0x400) && !find_vma(task->mm, addr_pc_start))
        addr_pc_start = regs->ARM_pc & (~0xfff);
    if(((regs->ARM_pc & 0xfff) > 0xBFF) && !find_vma(task->mm, addr_pc_end))
        addr_pc_end = (regs->ARM_pc & (~0xfff)) + 0xfff;
    if(((regs->ARM_lr & 0xfff) < 0x800) && !find_vma(task->mm, addr_lr_start))
        addr_lr_start = regs->ARM_lr & (~0xfff);
    if(((regs->ARM_lr & 0xfff) > 0xBFF) && !find_vma(task->mm, addr_lr_end))
        addr_lr_end = (regs->ARM_lr & (~0xfff)) + 0xfff;

    //Find a duplicated address range
    if((addr_lr_start < addr_pc_start) && (addr_lr_end > addr_pc_end))
        addr_pc_start = addr_pc_end;
    else if((addr_pc_start <= addr_lr_start) && (addr_pc_end >= addr_lr_end))
        addr_lr_start = addr_lr_end;
    else if((addr_lr_start <= addr_pc_end) && (addr_lr_end > addr_pc_end))
        addr_lr_start = addr_pc_end + 0x4;
    else if((addr_pc_start <= addr_lr_end) && (addr_pc_end > addr_lr_end))
        addr_pc_start = addr_lr_end + 0x4;

    printk(KERN_CONT "--------------------------------------------------------------------------------------\n");
    if((vma=find_vma(task->mm, regs->ARM_pc)) && (regs->ARM_pc >= vma->vm_start))
        dump_mem(KERN_CONT, "PC meminfo ", addr_pc_start, addr_pc_end);
    else
        printk(KERN_CONT "No VMA for ADDR PC\n");
    printk(KERN_CONT "--------------------------------------------------------------------------------------\n");
    if((vma=find_vma(task->mm, regs->ARM_lr)) && (regs->ARM_lr >= vma->vm_start))
        dump_mem(KERN_CONT, "LR meminfo ", addr_lr_start, addr_lr_end);
    else
        printk(KERN_CONT "No VMA for ADDR LR\n");
    printk(KERN_CONT "--------------------------------------------------------------------------------------\n");
    printk(KERN_CONT "\n");
}

static void show_pc_lr_kernel(const struct pt_regs *regs)
{
    unsigned long addr_pc, addr_lr;
    int valid_pc, valid_lr; 
	int valid_pc_mod, valid_lr_mod;
	struct module *mod;

    addr_pc = regs->ARM_pc - 0x400;   // for 1024 byte
    addr_lr = regs->ARM_lr - 0x800;   // for 2048 byte

	valid_pc_mod=((regs->ARM_pc >= VMALLOC_START && regs->ARM_pc < VMALLOC_END) ||
		      (regs->ARM_pc >= MODULES_VADDR && regs->ARM_pc < MODULES_END));
	valid_lr_mod=((regs->ARM_lr >= VMALLOC_START && regs->ARM_lr < VMALLOC_END) ||
		      (regs->ARM_lr >= MODULES_VADDR && regs->ARM_lr < MODULES_END));
    
	valid_pc = (TASK_SIZE <= regs->ARM_pc && regs->ARM_pc < (unsigned long)high_memory) || valid_pc_mod;
	valid_lr = (TASK_SIZE <= regs->ARM_lr && regs->ARM_lr < (unsigned long)high_memory) || valid_lr_mod;

	/* Adjust the addr_pc according to the correct module virtual memory range. */
	if(valid_pc) {
		if (addr_pc < TASK_SIZE)
			addr_pc = TASK_SIZE;
		else if (valid_pc_mod) {
			mod = __module_address(regs->ARM_pc);

			if (!within_module_init(addr_pc, mod) &&
			    !within_module_core(addr_pc, mod))
				addr_pc = regs->ARM_pc & PAGE_MASK;
		}
	}

	/* Adjust the addr_lr according to the correct module virtual memory range. */
	if(valid_lr) {
		if (addr_lr < TASK_SIZE)
			addr_lr = TASK_SIZE;
		else if (valid_lr_mod) {
			mod = __module_address(regs->ARM_lr);

			if (!within_module_init(addr_lr, mod) &&
			    !within_module_core(addr_lr, mod))
				addr_lr = regs->ARM_lr & PAGE_MASK;
		}
	}

    if( valid_pc && valid_lr ){
    	// find a duplicated address range case1
    	if( (addr_lr<=regs->ARM_pc) && (regs->ARM_pc<regs->ARM_lr) ){
       	addr_lr = regs->ARM_pc + 0x4;
   	}
	// find a duplicated address rage case2
       else if( (addr_pc<=regs->ARM_lr) && (regs->ARM_lr<regs->ARM_pc) ){
		addr_pc = regs->ARM_lr + 0x4;}
    }

    printk("--------------------------------------------------------------------------------------\n");
    printk("[VDLP] DISPLAY PC, LR in KERNEL Level\n");
    printk("pc:%lx, ra:%lx\n", regs->ARM_pc, regs->ARM_lr);
    printk("--------------------------------------------------------------------------------------\n");
    if( valid_pc ){
    	dump_mem_kernel("PC meminfo in kernel", addr_pc, regs->ARM_pc );
    }else{
    	printk("[VDLP] Invalid pc addr\n");}
    printk("--------------------------------------------------------------------------------------\n");
    if( valid_lr )
	dump_mem_kernel("LR meminfo in kernel", addr_lr, regs->ARM_lr);
    else
       printk("[VDLP] Invalid lr addr\n");
    printk("--------------------------------------------------------------------------------------\n");
    printk("\n");
}

#ifdef CONFIG_DUMP_RANGE_BASED_ON_REGISTER
int is_valid_kernel_addr(unsigned long register_value)
{
        if(register_value < PAGE_OFFSET || !virt_addr_valid((void*)register_value)){ //includes checking NULL and user address
                return 0;
        }
        else{
                return 1;
        }
}

void show_register_memory_kernel(struct pt_regs * regs) 
{
	unsigned long start_addr_for_printing = 0, end_addr_for_printing = 0;
	int register_num;

	printk("--------------------------------------------------------------------------------------\n");
	printk("REGISTER MEMORY INFO\n");
	printk("--------------------------------------------------------------------------------------\n");

	for(register_num = 0; register_num<sizeof(struct pt_regs)/sizeof(long); register_num++) {

		printk("\n\n* REGISTER : r%d\n",register_num);

		start_addr_for_printing = (regs->uregs[register_num] & PAGE_MASK) - 0x1000;               //-4kbyte
		if(regs->uregs[register_num] >= 0xfffff000){    // if virtual address is 0xffffffff, skip dump address to prevent overflow
                        end_addr_for_printing = 0xffffffff;
               }else{
			end_addr_for_printing = (regs->uregs[register_num] & PAGE_MASK) + PAGE_SIZE + 0xfff;}     //+about 8kbyte
		if(!is_valid_kernel_addr(regs->uregs[register_num])) {

			printk("# Register value 0x%lx is wrong address.\n", regs->uregs[register_num]);
			printk("# We can't do anything.\n"); 
			printk("# So, we search next register.\n");

			continue;
		}
		if(!is_valid_kernel_addr(start_addr_for_printing)) {

			printk("# 'start_addr_for_printing' is wrong address.\n");
			printk("# So, we use just 'regs->uregs[register_num] & PAGE_MASK)'\n");

			start_addr_for_printing = (regs->uregs[register_num] & PAGE_MASK);
		}
		if(!is_valid_kernel_addr(end_addr_for_printing)) {

			printk("# 'end_addr_for_printing' is wrong address.\n");
			printk("# So, we use 'PAGE_ALIGN(regs->uregs[register_num]) + PAGE_SIZE-1'\n");

			end_addr_for_printing = (regs->uregs[register_num] & PAGE_MASK) + PAGE_SIZE-1;
		}

		// dump
		printk("# r%d register :0x%lx, start_addr : 0x%lx, end_addr : 0x%lx\n", register_num, regs->uregs[register_num], start_addr_for_printing, end_addr_for_printing);
		printk("--------------------------------------------------------------------------------------\n");
		dump_mem_kernel("meminfo ", start_addr_for_printing, end_addr_for_printing );
		printk("--------------------------------------------------------------------------------------\n");
		printk("\n");
	}
}
#endif
#endif /* #ifdef CONFIG_SHOW_PC_LR_INFO */

#ifndef CONFIG_SEPARATE_PRINTK_FROM_USER
#define sep_printk_start
#define sep_printk_end
#else
extern void _sep_printk_start(void);
extern void _sep_printk_end(void);
#define sep_printk_start  _sep_printk_start
#define sep_printk_end _sep_printk_end
#endif

#ifdef CONFIG_RUN_TIMER_DEBUG
extern void show_timer_list(void);
#endif

void show_info(struct task_struct *task, struct pt_regs *regs, unsigned long addr)
{
    static atomic_t prn_once = ATOMIC_INIT(0);

    if(atomic_cmpxchg(&prn_once, 0, 1))
    {
        return;
    }

#ifdef CONFIG_SEPARATE_PRINTK_FROM_USER
    sep_printk_start();
#endif

    console_verbose();      /* BSP patch : enable console while show_info */

    preempt_disable();

#ifdef CONFIG_VDLP_VERSION_INFO
    printk(KERN_ALERT"================================================================================\n");
    printk(KERN_ALERT" KERNEL Version : %s\n", DTV_KERNEL_VERSION);
    printk(KERN_ALERT"================================================================================\n");
#endif
#ifdef CONFIG_RUN_TIMER_DEBUG
   show_timer_list();
#endif

#ifdef CONFIG_SHOW_PC_LR_INFO
    show_pc_lr(task, regs);
#endif
    if(addr)
    {
        show_pte(task->mm, addr);
    }
    show_regs(regs);
   show_pid_maps(task);
    show_user_stack(task, regs);
    preempt_enable();

#ifdef CONFIG_SEPARATE_PRINTK_FROM_USER
    sep_printk_end();
#endif
}
#endif /* CONFIG_SHOW_FAULT_TRACE_INFO */
#endif /* CONFIG_MP_DEBUG_TOOL_CODEDUMP */

void dump_backtrace_entry(unsigned long where, unsigned long from, unsigned long frame)
{
#ifdef CONFIG_KALLSYMS
	printk("[<%08lx>] (%pS) from [<%08lx>] (%pS)\n", where, (void *)where, from, (void *)from);
#else
	printk("Function entered at [<%08lx>] from [<%08lx>]\n", where, from);
#endif

	if (in_exception_text(where))
		dump_mem("", "Exception stack", frame + 4, frame + 4 + sizeof(struct pt_regs));
}

#ifndef CONFIG_ARM_UNWIND
/*
 * Stack pointers should always be within the kernels view of
 * physical memory.  If it is not there, then we can't dump
 * out any information relating to the stack.
 */
static int verify_stack(unsigned long sp)
{
	if (sp < PAGE_OFFSET ||
	    (sp > (unsigned long)high_memory && high_memory != NULL))
		return -EFAULT;

	return 0;
}
#endif

/*
 * Dump out the contents of some memory nicely...
 */
static void dump_mem(const char *lvl, const char *str, unsigned long bottom,
		     unsigned long top)
{
	unsigned long first;
	mm_segment_t fs;
	int i;

	/*
	 * We need to switch to kernel mode so that we can use __get_user
	 * to safely read from kernel space.  Note that we now dump the
	 * code first, just in case the backtrace kills us.
	 */
	fs = get_fs();
	set_fs(KERNEL_DS);

	printk("%s%s(0x%08lx to 0x%08lx)\n", lvl, str, bottom, top);

	for (first = bottom & ~31; first < top; first += 32) {
		unsigned long p;
		char str[sizeof(" 12345678") * 8 + 1];

		memset(str, ' ', sizeof(str));
		str[sizeof(str) - 1] = '\0';

		for (p = first, i = 0; i < 8 && p < top; i++, p += 4) {
			if (p >= bottom && p < top) {
				unsigned long val;
				if (__get_user(val, (unsigned long *)p) == 0)
					sprintf(str + i * 9, " %08lx", val);
				else
					sprintf(str + i * 9, " ????????");
			}
		}
		printk("%s%04lx:%s\n", lvl, first & 0xffff, str);
	}

	set_fs(fs);
}

static void dump_instr(const char *lvl, struct pt_regs *regs)
{
	unsigned long addr = instruction_pointer(regs);
	const int thumb = thumb_mode(regs);
	const int width = thumb ? 4 : 8;
	mm_segment_t fs;
	char str[sizeof("00000000 ") * 5 + 2 + 1], *p = str;
	int i;

	/*
	 * We need to switch to kernel mode so that we can use __get_user
	 * to safely read from kernel space.  Note that we now dump the
	 * code first, just in case the backtrace kills us.
	 */
	fs = get_fs();
	set_fs(KERNEL_DS);

	for (i = -4; i < 1 + !!thumb; i++) {
		unsigned int val, bad;

		if (thumb)
			bad = __get_user(val, &((u16 *)addr)[i]);
		else
			bad = __get_user(val, &((u32 *)addr)[i]);

		if (!bad)
			p += sprintf(p, i == 0 ? "(%0*x) " : "%0*x ",
					width, val);
		else {
			p += sprintf(p, "bad PC value");
			break;
		}
	}
	printk("%sCode: %s\n", lvl, str);

	set_fs(fs);
}

#ifdef CONFIG_ARM_UNWIND
static inline void dump_backtrace(struct pt_regs *regs, struct task_struct *tsk)
{
	unwind_backtrace(regs, tsk);
}
#else
static void dump_backtrace(struct pt_regs *regs, struct task_struct *tsk)
{
	unsigned int fp, mode;
	int ok = 1;

	printk("Backtrace: ");

	if (!tsk)
		tsk = current;

	if (regs) {
		fp = regs->ARM_fp;
		mode = processor_mode(regs);
	} else if (tsk != current) {
		fp = thread_saved_fp(tsk);
		mode = 0x10;
	} else {
		asm("mov %0, fp" : "=r" (fp) : : "cc");
		mode = 0x10;
	}

	if (!fp) {
		printk("no frame pointer");
		ok = 0;
	} else if (verify_stack(fp)) {
		printk("invalid frame pointer 0x%08x", fp);
		ok = 0;
	} else if (fp < (unsigned long)end_of_stack(tsk))
		printk("frame pointer underflow");
	printk("\n");

	if (ok)
		c_backtrace(fp, mode);
}
#endif

void dump_stack(void)
{
	dump_backtrace(NULL, NULL);
}

EXPORT_SYMBOL(dump_stack);

void show_stack(struct task_struct *tsk, unsigned long *sp)
{
	dump_backtrace(NULL, tsk);
	barrier();
}

#ifdef CONFIG_PREEMPT
#define S_PREEMPT " PREEMPT"
#else
#define S_PREEMPT ""
#endif
#ifdef CONFIG_SMP
#define S_SMP " SMP"
#else
#define S_SMP ""
#endif

static int __die(const char *str, int err, struct thread_info *thread, struct pt_regs *regs)
{
	struct task_struct *tsk = thread->task;
	static int die_counter;
	int ret;

	printk(KERN_EMERG "Internal error: %s: %x [#%d]" S_PREEMPT S_SMP "\n",
	       str, err, ++die_counter);

	/* trap and error numbers are mostly meaningless on ARM */
	ret = notify_die(DIE_OOPS, str, regs, err, tsk->thread.trap_no, SIGSEGV);
	if (ret == NOTIFY_STOP)
		return ret;

	print_modules();
	__show_regs(regs);
	printk(KERN_EMERG "Process %.*s (pid: %d, stack limit = 0x%p)\n",
		TASK_COMM_LEN, tsk->comm, task_pid_nr(tsk), thread + 1);

	if (!user_mode(regs) || in_interrupt()) {
		dump_mem(KERN_EMERG, "Stack: ", regs->ARM_sp,
			 THREAD_SIZE + (unsigned long)task_stack_page(tsk));
		dump_backtrace(regs, tsk);
		dump_instr(KERN_EMERG, regs);
	}

	return ret;
}

static DEFINE_SPINLOCK(die_lock);

#if defined(CONFIG_MP_DEBUG_TOOL_KDEBUG)
#ifdef CONFIG_KDEBUGD
/*
 * SELP 3.2.x Chelsea Kernel Patch
 * get user stack
 * : assumes that user program uses frame pointer
 * : TODO : consider context safety
 * */
int get_user_stack(struct task_struct *task,
	unsigned int **us_content, unsigned long *start, unsigned long *end)
{
	struct pt_regs *regs;
	struct vm_area_struct *vma;
	int no_of_us_value = 0;
	struct mm_struct *mm = NULL;

	regs = task_pt_regs(task);

	mm = get_task_mm(task);
	vma = find_vma(task->mm, task->user_ssp);
	if (vma) {

		unsigned long bottom = regs->ARM_sp;
		unsigned long top = task->user_ssp;
		unsigned long p = bottom & ~31;
		mm_segment_t fs;
		unsigned int *tmp_user_stack;

		*start = bottom;
		*end = top;

		*us_content = (unsigned int *)vmalloc(
				(top - bottom) * sizeof (unsigned int));

		if (!*us_content) {
			printk ("%s %d> No memory to build user stack\n",
					__FUNCTION__, __LINE__);
			mmput(mm);
			return no_of_us_value;
		}

		printk("stack area (0x%08lx ~ 0x%08lx)\n", vma->vm_start, vma->vm_end);
		printk("User stack area (0x%08lx ~ 0x%08lx)\n",
				regs->ARM_sp, vma->vm_end);

		/*
		 * We need to switch to kernel mode so that we can use __get_user
		 * to safely read from kernel space.  Note that we now dump the
		 * code first, just in case the backtrace kills us.
		 */
		fs = get_fs();
		set_fs(KERNEL_DS);

		/* copy the pointer  to tmp, to fill the app cookie value */
		tmp_user_stack = *us_content;

		for (p = bottom & ~31; p < top; p += 4) {
			if (!(p < bottom || p >= top)) {
				unsigned int val;
				__get_user(val, (unsigned long *)p);
				if (val) {
					*(tmp_user_stack+no_of_us_value++) = p & 0xffff;
					*(tmp_user_stack+no_of_us_value++) = val;
					/*printk("<%08x - %08x> ",
					 *(tmp_user_stack+no_of_us_value++), val);*/
				}
			}
		}

		set_fs(fs);

	}
	mmput(mm);
	return no_of_us_value;
}

EXPORT_SYMBOL(get_user_stack);
#endif
#endif /* CONFIG_MP_DEBUG_TOOL_KDEBUG*/

/*
 * This function is protected against re-entrancy.
 */
void die(const char *str, struct pt_regs *regs, int err)
{
	struct thread_info *thread = current_thread_info();
	int ret;

	oops_enter();

	spin_lock_irq(&die_lock);
	console_verbose();
	bust_spinlocks(1);
	ret = __die(str, err, thread, regs);

	if (regs && kexec_should_crash(thread->task))
		crash_kexec(regs);

	bust_spinlocks(0);
	add_taint(TAINT_DIE);
	spin_unlock_irq(&die_lock);
	oops_exit();

	if (in_interrupt())
		panic("Fatal exception in interrupt");
	if (panic_on_oops)
		panic("Fatal exception");
	if (ret != NOTIFY_STOP)
		do_exit(SIGSEGV);
}

void arm_notify_die(const char *str, struct pt_regs *regs,
		struct siginfo *info, unsigned long err, unsigned long trap)
{
	if (user_mode(regs)) {
		current->thread.error_code = err;
		current->thread.trap_no = trap;

#if defined(CONFIG_MP_DEBUG_TOOL_CODEDUMP)
#ifdef CONFIG_SHOW_FAULT_TRACE_INFO
        show_info(current, regs, 0);
#endif
#endif /* CONFIG_MP_DEBUG_TOOL_CODEDUMP */

		force_sig_info(info->si_signo, info, current);
	} else {
		die(str, regs, err);
	}
}

static LIST_HEAD(undef_hook);
static DEFINE_SPINLOCK(undef_lock);

void register_undef_hook(struct undef_hook *hook)
{
	unsigned long flags;

	spin_lock_irqsave(&undef_lock, flags);
	list_add(&hook->node, &undef_hook);
	spin_unlock_irqrestore(&undef_lock, flags);
}

void unregister_undef_hook(struct undef_hook *hook)
{
	unsigned long flags;

	spin_lock_irqsave(&undef_lock, flags);
	list_del(&hook->node);
	spin_unlock_irqrestore(&undef_lock, flags);
}

static int call_undef_hook(struct pt_regs *regs, unsigned int instr)
{
	struct undef_hook *hook;
	unsigned long flags;
	int (*fn)(struct pt_regs *regs, unsigned int instr) = NULL;

	spin_lock_irqsave(&undef_lock, flags);
	list_for_each_entry(hook, &undef_hook, node)
		if ((instr & hook->instr_mask) == hook->instr_val &&
		    (regs->ARM_cpsr & hook->cpsr_mask) == hook->cpsr_val)
			fn = hook->fn;
	spin_unlock_irqrestore(&undef_lock, flags);

	return fn ? fn(regs, instr) : 1;
}

asmlinkage void __exception do_undefinstr(struct pt_regs *regs)
{
	unsigned int correction = thumb_mode(regs) ? 2 : 4;
	unsigned int instr;
	siginfo_t info;
	void __user *pc;

	/*
	 * According to the ARM ARM, PC is 2 or 4 bytes ahead,
	 * depending whether we're in Thumb mode or not.
	 * Correct this offset.
	 */
	regs->ARM_pc -= correction;

	pc = (void __user *)instruction_pointer(regs);

	if (processor_mode(regs) == SVC_MODE) {
#ifdef CONFIG_THUMB2_KERNEL
		if (thumb_mode(regs)) {
			instr = ((u16 *)pc)[0];
			if (is_wide_instruction(instr)) {
				instr <<= 16;
				instr |= ((u16 *)pc)[1];
			}
		} else
#endif
			instr = *(u32 *) pc;
	} else if (thumb_mode(regs)) {
		get_user(instr, (u16 __user *)pc);
		if (is_wide_instruction(instr)) {
			unsigned int instr2;
			get_user(instr2, (u16 __user *)pc+1);
			instr <<= 16;
			instr |= instr2;
		}
	} else {
		get_user(instr, (u32 __user *)pc);
	}

	if (call_undef_hook(regs, instr) == 0)
		return;

#ifdef CONFIG_DEBUG_USER
	if (user_debug & UDBG_UNDEFINED) {
		printk(KERN_INFO "%s (%d): undefined instruction: pc=%p\n",
			current->comm, task_pid_nr(current), pc);
		dump_instr(KERN_INFO, regs);
	}
#endif

	info.si_signo = SIGILL;
	info.si_errno = 0;
	info.si_code  = ILL_ILLOPC;
	info.si_addr  = pc;

	arm_notify_die("Oops - undefined instruction", regs, &info, 0, 6);
}

asmlinkage void do_unexp_fiq (struct pt_regs *regs)
{
	printk("Hmm.  Unexpected FIQ received, but trying to continue\n");
	printk("You may have a hardware problem...\n");
}

/*
 * bad_mode handles the impossible case in the vectors.  If you see one of
 * these, then it's extremely serious, and could mean you have buggy hardware.
 * It never returns, and never tries to sync.  We hope that we can at least
 * dump out some state information...
 */
asmlinkage void bad_mode(struct pt_regs *regs, int reason)
{
	console_verbose();

	printk(KERN_CRIT "Bad mode in %s handler detected\n", handler[reason]);

	die("Oops - bad mode", regs, 0);
	local_irq_disable();
	panic("bad mode");
}

static int bad_syscall(int n, struct pt_regs *regs)
{
	struct thread_info *thread = current_thread_info();
	siginfo_t info;

	if ((current->personality & PER_MASK) != PER_LINUX &&
	    thread->exec_domain->handler) {
		thread->exec_domain->handler(n, regs);
		return regs->ARM_r0;
	}

#ifdef CONFIG_DEBUG_USER
	if (user_debug & UDBG_SYSCALL) {
		printk(KERN_ERR "[%d] %s: obsolete system call %08x.\n",
			task_pid_nr(current), current->comm, n);
		dump_instr(KERN_ERR, regs);
	}
#endif

	info.si_signo = SIGILL;
	info.si_errno = 0;
	info.si_code  = ILL_ILLTRP;
	info.si_addr  = (void __user *)instruction_pointer(regs) -
			 (thumb_mode(regs) ? 2 : 4);

	arm_notify_die("Oops - bad syscall", regs, &info, n, 0);

	return regs->ARM_r0;
}

static inline void
do_cache_op(unsigned long start, unsigned long end, int flags)
{
	struct mm_struct *mm = current->active_mm;
	struct vm_area_struct *vma;

	if (end < start || flags)
		return;

	down_read(&mm->mmap_sem);
	vma = find_vma(mm, start);
	if (vma && vma->vm_start < end) {
		if (start < vma->vm_start)
			start = vma->vm_start;
		if (end > vma->vm_end)
			end = vma->vm_end;

		up_read(&mm->mmap_sem);
		flush_cache_user_range(start, end);
		return;
	}
	up_read(&mm->mmap_sem);
}

/*
 * Handle all unrecognised system calls.
 *  0x9f0000 - 0x9fffff are some more esoteric system calls
 */
#define NR(x) ((__ARM_NR_##x) - __ARM_NR_BASE)
asmlinkage int arm_syscall(int no, struct pt_regs *regs)
{
	struct thread_info *thread = current_thread_info();
	siginfo_t info;

	if ((no >> 16) != (__ARM_NR_BASE>> 16))
		return bad_syscall(no, regs);

	switch (no & 0xffff) {
	case 0: /* branch through 0 */
		info.si_signo = SIGSEGV;
		info.si_errno = 0;
		info.si_code  = SEGV_MAPERR;
		info.si_addr  = NULL;

		arm_notify_die("branch through zero", regs, &info, 0, 0);
		return 0;

	case NR(breakpoint): /* SWI BREAK_POINT */
		regs->ARM_pc -= thumb_mode(regs) ? 2 : 4;
		ptrace_break(current, regs);
		return regs->ARM_r0;

	/*
	 * Flush a region from virtual address 'r0' to virtual address 'r1'
	 * _exclusive_.  There is no alignment requirement on either address;
	 * user space does not need to know the hardware cache layout.
	 *
	 * r2 contains flags.  It should ALWAYS be passed as ZERO until it
	 * is defined to be something else.  For now we ignore it, but may
	 * the fires of hell burn in your belly if you break this rule. ;)
	 *
	 * (at a later date, we may want to allow this call to not flush
	 * various aspects of the cache.  Passing '0' will guarantee that
	 * everything necessary gets flushed to maintain consistency in
	 * the specified region).
	 */
	case NR(cacheflush):
		do_cache_op(regs->ARM_r0, regs->ARM_r1, regs->ARM_r2);
		return 0;

	case NR(usr26):
		if (!(elf_hwcap & HWCAP_26BIT))
			break;
		regs->ARM_cpsr &= ~MODE32_BIT;
		return regs->ARM_r0;

	case NR(usr32):
		if (!(elf_hwcap & HWCAP_26BIT))
			break;
		regs->ARM_cpsr |= MODE32_BIT;
		return regs->ARM_r0;

	case NR(set_tls):
		thread->tp_value = regs->ARM_r0;
		if (tls_emu)
			return 0;
		if (has_tls_reg) {
			asm ("mcr p15, 0, %0, c13, c0, 3"
				: : "r" (regs->ARM_r0));
		} else {
			/*
			 * User space must never try to access this directly.
			 * Expect your app to break eventually if you do so.
			 * The user helper at 0xffff0fe0 must be used instead.
			 * (see entry-armv.S for details)
			 */
			*((unsigned int *)0xffff0ff0) = regs->ARM_r0;
		}
		return 0;

#ifdef CONFIG_NEEDS_SYSCALL_FOR_CMPXCHG
	/*
	 * Atomically store r1 in *r2 if *r2 is equal to r0 for user space.
	 * Return zero in r0 if *MEM was changed or non-zero if no exchange
	 * happened.  Also set the user C flag accordingly.
	 * If access permissions have to be fixed up then non-zero is
	 * returned and the operation has to be re-attempted.
	 *
	 * *NOTE*: This is a ghost syscall private to the kernel.  Only the
	 * __kuser_cmpxchg code in entry-armv.S should be aware of its
	 * existence.  Don't ever use this from user code.
	 */
	case NR(cmpxchg):
	for (;;) {
		extern void do_DataAbort(unsigned long addr, unsigned int fsr,
					 struct pt_regs *regs);
		unsigned long val;
		unsigned long addr = regs->ARM_r2;
		struct mm_struct *mm = current->mm;
		pgd_t *pgd; pmd_t *pmd; pte_t *pte;
		spinlock_t *ptl;

		regs->ARM_cpsr &= ~PSR_C_BIT;
		down_read(&mm->mmap_sem);
		pgd = pgd_offset(mm, addr);
		if (!pgd_present(*pgd))
			goto bad_access;
		pmd = pmd_offset(pgd, addr);
		if (!pmd_present(*pmd))
			goto bad_access;
		pte = pte_offset_map_lock(mm, pmd, addr, &ptl);
		if (!pte_present(*pte) || !pte_write(*pte) || !pte_dirty(*pte)) {
			pte_unmap_unlock(pte, ptl);
			goto bad_access;
		}
		val = *(unsigned long *)addr;
		val -= regs->ARM_r0;
		if (val == 0) {
			*(unsigned long *)addr = regs->ARM_r1;
			regs->ARM_cpsr |= PSR_C_BIT;
		}
		pte_unmap_unlock(pte, ptl);
		up_read(&mm->mmap_sem);
		return val;

		bad_access:
		up_read(&mm->mmap_sem);
		/* simulate a write access fault */
		do_DataAbort(addr, 15 + (1 << 11), regs);
	}
#endif

	default:
		/* Calls 9f00xx..9f07ff are defined to return -ENOSYS
		   if not implemented, rather than raising SIGILL.  This
		   way the calling program can gracefully determine whether
		   a feature is supported.  */
		if ((no & 0xffff) <= 0x7ff)
			return -ENOSYS;
		break;
	}
#ifdef CONFIG_DEBUG_USER
	/*
	 * experience shows that these seem to indicate that
	 * something catastrophic has happened
	 */
	if (user_debug & UDBG_SYSCALL) {
		printk("[%d] %s: arm syscall %d\n",
		       task_pid_nr(current), current->comm, no);
		dump_instr("", regs);
		if (user_mode(regs)) {
			__show_regs(regs);
			c_backtrace(regs->ARM_fp, processor_mode(regs));
		}
	}
#endif
	info.si_signo = SIGILL;
	info.si_errno = 0;
	info.si_code  = ILL_ILLTRP;
	info.si_addr  = (void __user *)instruction_pointer(regs) -
			 (thumb_mode(regs) ? 2 : 4);

	arm_notify_die("Oops - bad syscall(2)", regs, &info, no, 0);
	return 0;
}

#ifdef CONFIG_TLS_REG_EMUL

/*
 * We might be running on an ARMv6+ processor which should have the TLS
 * register but for some reason we can't use it, or maybe an SMP system
 * using a pre-ARMv6 processor (there are apparently a few prototypes like
 * that in existence) and therefore access to that register must be
 * emulated.
 */

static int get_tp_trap(struct pt_regs *regs, unsigned int instr)
{
	int reg = (instr >> 12) & 15;
	if (reg == 15)
		return 1;
	regs->uregs[reg] = current_thread_info()->tp_value;
	regs->ARM_pc += 4;
	return 0;
}

static struct undef_hook arm_mrc_hook = {
	.instr_mask	= 0x0fff0fff,
	.instr_val	= 0x0e1d0f70,
	.cpsr_mask	= PSR_T_BIT,
	.cpsr_val	= 0,
	.fn		= get_tp_trap,
};

static int __init arm_mrc_hook_init(void)
{
	register_undef_hook(&arm_mrc_hook);
	return 0;
}

late_initcall(arm_mrc_hook_init);

#endif

void __bad_xchg(volatile void *ptr, int size)
{
	printk("xchg: bad data size: pc 0x%p, ptr 0x%p, size %d\n",
		__builtin_return_address(0), ptr, size);
	BUG();
}
EXPORT_SYMBOL(__bad_xchg);

/*
 * A data abort trap was taken, but we did not handle the instruction.
 * Try to abort the user program, or panic if it was the kernel.
 */
asmlinkage void
baddataabort(int code, unsigned long instr, struct pt_regs *regs)
{
	unsigned long addr = instruction_pointer(regs);
	siginfo_t info;

#ifdef CONFIG_DEBUG_USER
	if (user_debug & UDBG_BADABORT) {
		printk(KERN_ERR "[%d] %s: bad data abort: code %d instr 0x%08lx\n",
			task_pid_nr(current), current->comm, code, instr);
		dump_instr(KERN_ERR, regs);
		show_pte(current->mm, addr);
	}
#endif

	info.si_signo = SIGILL;
	info.si_errno = 0;
	info.si_code  = ILL_ILLOPC;
	info.si_addr  = (void __user *)addr;

	arm_notify_die("unknown data abort code", regs, &info, instr, 0);
}

void __attribute__((noreturn)) __bug(const char *file, int line)
{
	printk(KERN_CRIT"kernel BUG at %s:%d!\n", file, line);
	*(int *)0 = 0;

	/* Avoid "noreturn function does return" */
	for (;;);
}
EXPORT_SYMBOL(__bug);

void __readwrite_bug(const char *fn)
{
	printk("%s called, but not implemented\n", fn);
	BUG();
}
EXPORT_SYMBOL(__readwrite_bug);

void __pte_error(const char *file, int line, pte_t pte)
{
	printk("%s:%d: bad pte %08llx.\n", file, line, (long long)pte_val(pte));
}

void __pmd_error(const char *file, int line, pmd_t pmd)
{
	printk("%s:%d: bad pmd %08llx.\n", file, line, (long long)pmd_val(pmd));
}

void __pgd_error(const char *file, int line, pgd_t pgd)
{
	printk("%s:%d: bad pgd %08llx.\n", file, line, (long long)pgd_val(pgd));
}

asmlinkage void __div0(void)
{
	printk("Division by zero in kernel.\n");
	dump_stack();
}
EXPORT_SYMBOL(__div0);

void abort(void)
{
	BUG();

	/* if that doesn't kill us, halt */
	panic("Oops failed to kill thread");
}
EXPORT_SYMBOL(abort);

void __init trap_init(void)
{
	return;
}

static void __init kuser_get_tls_init(unsigned long vectors)
{
	/*
	 * vectors + 0xfe0 = __kuser_get_tls
	 * vectors + 0xfe8 = hardware TLS instruction at 0xffff0fe8
	 */
	if (tls_emu || has_tls_reg)
		memcpy((void *)vectors + 0xfe0, (void *)vectors + 0xfe8, 4);
}

void __init early_trap_init(void)
{
#if defined(CONFIG_CPU_USE_DOMAINS)
	unsigned long vectors = CONFIG_VECTORS_BASE;
#else
	unsigned long vectors = (unsigned long)vectors_page;
#endif
	extern char __stubs_start[], __stubs_end[];
	extern char __vectors_start[], __vectors_end[];
	extern char __kuser_helper_start[], __kuser_helper_end[];
	int kuser_sz = __kuser_helper_end - __kuser_helper_start;

	/*
	 * Copy the vectors, stubs and kuser helpers (in entry-armv.S)
	 * into the vector page, mapped at 0xffff0000, and ensure these
	 * are visible to the instruction stream.
	 */
	memcpy((void *)vectors, __vectors_start, __vectors_end - __vectors_start);
	memcpy((void *)vectors + 0x200, __stubs_start, __stubs_end - __stubs_start);
	memcpy((void *)vectors + 0x1000 - kuser_sz, __kuser_helper_start, kuser_sz);

	/*
	 * Do processor specific fixups for the kuser helpers
	 */
	kuser_get_tls_init(vectors);

	/*
	 * Copy signal return handlers into the vector page, and
	 * set sigreturn to be a pointer to these.
	 */
	memcpy((void *)(vectors + KERN_SIGRETURN_CODE - CONFIG_VECTORS_BASE),
	       sigreturn_codes, sizeof(sigreturn_codes));
	memcpy((void *)(vectors + KERN_RESTART_CODE - CONFIG_VECTORS_BASE),
	       syscall_restart_code, sizeof(syscall_restart_code));

	flush_icache_range(vectors, vectors + PAGE_SIZE);
	modify_domain(DOMAIN_USER, DOMAIN_CLIENT);
}
