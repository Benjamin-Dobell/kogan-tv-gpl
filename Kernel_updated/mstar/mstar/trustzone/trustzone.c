/*
 * linux/driver/mstar/trustzone/trustzone.c
 *
 * Copyright (C) 1992, 1998-2004 Linus Torvalds, Ingo Molnar
 *
 * This file contains the /proc/irq/ handling code.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <asm/uaccess.h>
#include <asm/pgtable.h>
#include <linux/mm.h>
#include <linux/smp.h>

#define MAX_NAMELEN 10

static struct proc_dir_entry *proc_entry;
static struct proc_dir_entry *root_tz_dir;


#define L_PTE_PRESENT           (1 << 0)
#define L_PTE_YOUNG             (1 << 1)
#define L_PTE_FILE              (1 << 2)        /* only when !PRESENT */
#define L_PTE_DIRTY             (1 << 6)
#define L_PTE_WRITE             (1 << 7)
#define L_PTE_USER              (1 << 8)
#define L_PTE_EXEC              (1 << 9)
#define L_PTE_SHARED            (1 << 10)       /* shared(v6), coherent(xsc3) */

struct mem_type {
        unsigned int prot_pte;
        unsigned int prot_l1;
        unsigned int prot_sect;
        unsigned int domain;
};
void Chip_Flush_Cache_All();
 
#define CPU_MODE_FIQ 0x11
#define CPU_MODE_IRQ 0x12
#define CPU_MODE_SVC 0x13
#define CPU_MODE_ABT 0x17
#define CPU_MODE_UND 0x1b
#define CPU_MODE_SYS 0x1f
#define CPU_MODE_MON 0x16

void __smc()
{
    __asm__ volatile(
       
        "mov r3, r13\n"
        "mov r6, lr\n"
        "ldr r0, =0x0\n"
        "smc 0\n"
        "mov r13, r3\n"
        "mov lr, r6\n"
    );
}

void __smc_wrapper()
{
    unsigned int cpu = smp_processor_id();
    if(cpu ==1)
    {
       printk("smc call from cpu1\n");
       __smc();
    }
    else 
    {
       printk("smc call from cpu0\n");
    }
}



int tz_write( struct file *filp, const char __user *buff,
                        unsigned long len, void *data )
{
//  unsigned int cpu = smp_processor_id();
  
//  if(cpu ==0)
//  {
//     printk("reject smc from core0\n");  
//     return len;
//  }
//  printk("cpu %d\n", cpu);

  //printk("tz_write\n");
  //local_irq_disable();
 // __asm("smc 0");
  smp_call_function(__smc_wrapper, NULL, 1);
  //for_each_cpu(__smc_wrapper, 0x3);
  //local_irq_enable();
  //_smc(1,2,3,4);
  printk("switch back to normal world!\n");
  return len;
}

int tz_read( char *page, char **start, off_t off,
                   int count, int *eof, void *data )
{

  printk("tz_read\n");
  return 1;
}
extern void iotable_init(struct map_desc *, int);
int __init init_kirq_proc( void )
{
    int ret = 0;
    char name [MAX_NAMELEN];
    
    printk("Init tz proc\n");
    memset(name, 0, MAX_NAMELEN);
    sprintf(name, "%s", "prompt");

 	  root_tz_dir = proc_mkdir("tz", NULL);
  
    proc_entry = create_proc_entry( "prompt", 0644, root_tz_dir );
    if (proc_entry == NULL) {
      ret = -ENOMEM;
      printk(KERN_INFO "tz_proc: Couldn't create proc entry\n");
      
    } 
    else 
    {
      proc_entry->read_proc = tz_read;
      proc_entry->write_proc = tz_write;
      //proc_entry->owner = THIS_MODULE;
      printk(KERN_INFO "tz: Module loaded.\n");
    }

  return ret;
}

int __exit exit_kirq_proc(void)
{
    printk("exit!\n");
    return 1;
}

MODULE_DESCRIPTION("TZ driver support");
MODULE_AUTHOR("MStar");
MODULE_LICENSE("GPL");
module_init(init_kirq_proc);
module_exit(exit_kirq_proc);


