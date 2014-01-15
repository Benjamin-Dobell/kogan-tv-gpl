////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2007 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (¡§MStar Confidential Information¡¨) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    msglog.c
/// @brief  This used to save console message to RAM area
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/tty.h>
#include <linux/tty_driver.h>
#include <linux/console.h>
#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/nmi.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/smp.h>
#include <linux/security.h>
#include <linux/bootmem.h>
#include <linux/syscalls.h>

#include <linux/ctype.h>
#include <linux/namei.h>
#include <linux/vfs.h>
#include <linux/fcntl.h>
#include <asm/system.h>
#include <asm/io.h>

#include <asm/uaccess.h>


#define MSG_LOG_CONF_FILE   "/etc/msglog.conf"
#define MAX_LOG_FILE_SZIE (1024*1024) // 1MB

/* Control Preemption */

#define MSGLOG_PREEMPT_COUNT_SAVE(cnt) \
	do { 				\
		cnt = preempt_count();	\
		while(preempt_count())	\
		preempt_enable_no_resched(); \
	} while (0)

#define MSGLOG_PREEMPT_COUNT_RESTORE(cnt) \
	do { 				\
		int i;			\
		for(i=0; i<cnt; i++)	\
		preempt_disable(); \
	} while (0)

/* Control Option */

#define MSGLOG_VERSION	"2.2.5 (Mstar)"

int opt_msglog = 0;
int opt_interval = 5;

/* Buffer Size Config */
#define CONFIG_MSGLOG_BUF_SHIFT     17  /* 128KB */

/* Dtvlogd main Log Buffer */
#define __MSGLOG_BUF_LEN	(1 << CONFIG_MSGLOG_BUF_SHIFT)
#define MSGLOG_BUF_MASK	(msglog_buf_len - 1)
#define MSGLOG_BUF(idx)	msglog_buf[(idx) & MSGLOG_BUF_MASK]
#define MSGLOG_INDEX(idx)	((idx) & MSGLOG_BUF_MASK)

static char __msglog_buf[__MSGLOG_BUF_LEN];
static char *msglog_buf = __msglog_buf;
static int msglog_buf_len = __MSGLOG_BUF_LEN;

static int msglogbuf_start = 0; // msg buff start index
static int msglogbuf_end = 0;   // msg buff end undex
static int msglogged_count = 0; // msg buff count in ring buffer
static int msglogbuf_writeok = 0;
static int msglogbuf_writefail = 0;
static DEFINE_SPINLOCK(msglogbuf_lock);
static DECLARE_MUTEX(msglogbuf_sem);
static DECLARE_MUTEX(msglogbuf_complete_sem);

wait_queue_head_t msglogbuf_wq;
int msglogbuf_complete=0;

/* Uninitialized memory buffer */
/*
 * Emergency Log Memory Map
 *
 * +------------------+ 0x0032 0000 (3MB + 128KB)
 * | Emeg Log Buffer  |
 * |    (64KB)        |
 * +------------------+ 0x0031 0000 (3MB + 64KB)
 * | Mgmt Data (16B)  |
 * +------------------+ 0x0030 FFF0
 * |  Reserved Area   |
 * |   (63.9KB)       |
 * +------------------+ 0x0030 0000 (3MB)
 */

#ifdef CONFIG_MSTAR_MSGLOG_EMERGENCY_LOG
#define __DRAM_BUF_LEN    64 * 1024       /* Current 64Kb */
#define DRAM_BUF_MASK (__DRAM_BUF_LEN - 1)
#define DRAM_INDEX(idx) 	((idx) & DRAM_BUF_MASK)

#define RAM_BUF_ADDR   0x310000
#define RAM_DATA_ADDR  (RAM_BUF_ADDR - 0x10)

#define RAM_INFO_ADDR(base)    (base + 0x0)
#define RAM_START_ADDR(base)   (base + 0x4)
#define RAM_END_ADDR(base) (base + 0x8)
#define RAM_CHARS_ADDR(base)   (base + 0xC)

static int dram_buf_len = __DRAM_BUF_LEN;

volatile void __iomem *ram_char_buf;
volatile void __iomem *ram_info_buf;
static int ramwrite_start = 0;
static int ram_clear_flush = 0;
static unsigned int ram_log_magic = 0;

static int drambuf_start, drambuf_end, drambuf_chars;
#endif

/*
 * Uninitialized memory operation
 */
#ifdef CONFIG_MSTAR_MSGLOG_EMERGENCY_LOG
static void write_ram_info(void)
{
    writel(drambuf_start, (void __iomem *)RAM_START_ADDR(ram_info_buf));
    writel(drambuf_end, (void __iomem *)RAM_END_ADDR(ram_info_buf));
    writel(drambuf_chars, (void __iomem *)RAM_CHARS_ADDR(ram_info_buf));
}
#endif


/*
 * rambuf_init()
 */
#ifdef CONFIG_MSTAR_MSGLOG_EMERGENCY_LOG
static void rambuf_init(void)
{
    unsigned int magic = 0xbabe;
    unsigned int temp;

    ram_char_buf = ioremap(RAM_BUF_ADDR, dram_buf_len);
    ram_info_buf = ioremap(RAM_DATA_ADDR, sizeof(u32) * 4);

    temp = (unsigned int)readl(ram_info_buf);

    ram_log_magic = temp;
    if (magic == temp)
    {
        ram_clear_flush = 1;
    }
    else
    {
        ramwrite_start = 1;
        drambuf_start = drambuf_end = drambuf_chars = 0;
        writel(magic, ram_info_buf);
    }
}
#endif

/*
 *	EXTERN FUNCTIONS
 */

extern int msglog_record_print_msg(const char __user *str, int count, int type);

/*
 *	USB I/O FUNCTIONS
 */

#define LOGNAME_SIZE	32

char logdir[LOGNAME_SIZE];
int log_index = 0;
/*
 *	msglog_print_option()
 */

static void msglog_print_option(void)
{
    int mylen = msglog_buf_len;
    int mystart = msglogbuf_start;
    int myend = msglogbuf_end;
    int mycount = msglogged_count;

    printk(KERN_CRIT"\n\n\n");
    printk(KERN_CRIT"==================================================\n");
    printk(KERN_CRIT"MSGLOG v%s\n", MSGLOG_VERSION);
    printk(KERN_CRIT"==================================================\n");
    printk(KERN_CRIT"Folder   = %s\n", (logdir[0]=='/')?logdir:"(not found)" );
    printk(KERN_CRIT"Log file   = %s\n", (log_index)?"log_1.txt":"log_0.txt" );
    printk(KERN_CRIT"Use MSGLOG   = %s\n", (opt_msglog)?"Yes":"No" );
    printk(KERN_CRIT"Save Interval  = %d sec\n", opt_interval);


#ifdef CONFIG_MSTAR_MSGLOG_EMERGENCY_LOG
    printk(KERN_CRIT"EMERG MAGIC    = 0x%x (0xbabe)\n", ram_log_magic);
#endif
    printk(KERN_CRIT"\n");

    printk(KERN_CRIT"Buffer Size    = %d\n", mylen);
    printk(KERN_CRIT"Start Index    = %d\n", mystart);
    printk(KERN_CRIT"End Index      = %d\n", myend);
    printk(KERN_CRIT"Saved Chars    = %d\n", mycount);
    printk(KERN_CRIT"Write Ok       = %d\n", msglogbuf_writeok);
    printk(KERN_CRIT"Write Fail     = %d\n", msglogbuf_writefail);
    printk(KERN_CRIT"\n");

}

#if 1
/*
 *	get_file_size()
 */

static long long get_file_size(char * file)
{
    long long llByte;
    struct file *fp;
    struct inode *inode;


    fp = filp_open(file, O_RDONLY | O_LARGEFILE, 0600);
    if (IS_ERR(fp))
    {
        //printk(KERN_CRIT"Log File Size  = 0 Byte\n");
        return 0;
    }
    inode = fp->f_mapping->host;
    llByte = inode->i_size;
    //printk(KERN_CRIT"Log File Size  = %lld Bytes\n", llByte);

    filp_close(fp, NULL);

    return llByte;
}
#endif
/*
 *	msglog_parse_option()
 */

static void msglog_parse_option(char *lval, char *rval)
{
    if ((lval == NULL) || (rval == NULL))
        return;
//printk("lval: \"%s\"\nrval:\"%s\"\n", lval, rval);

    if (!strncmp(lval, "msglog", 6))
    {
        sscanf(rval, "%d", &opt_msglog);
    }
    else if (!strncmp(lval, "interval", 8))
    {
        sscanf(rval, "%d", &opt_interval);
    }
    else if (!strncmp(lval, "logdir", 6))
    {
        sscanf(rval, "%s", logdir);
    }
}

/*
 *	msglog_read_option()
 */
static int msglog_read_option(char *con_file)
{
    char buf[128];
    char lval[32], rval[32];
    char *p;
    struct file *fp;
    ssize_t ret = -EBADF;

    fp = filp_open(con_file, O_RDONLY | O_LARGEFILE, 0600);

    if (IS_ERR(fp))
        return 0;

    ret = fp->f_op->read(fp, buf, 128, &fp->f_pos);

    if (ret < 0)
        return 0;

    for (p=buf; p != NULL; )
    {
        memset(lval, 0, 32);
        memset(rval, 0, 32);

        sscanf(p, "%s = %s", lval, rval);

        if (strlen(lval) > 1)
            msglog_parse_option(lval, rval);

        p = strstr(p, "\n");

        if (p == NULL)
            break;
        else
            p += 1;
    }

    filp_close(fp, NULL);

    msglog_print_option();

    return 1;
}

/*
 *	msglog_open() - opne log file
 */

static struct file* msglog_open(void)
{
    struct file *fp;
    char logname[LOGNAME_SIZE];
    int tmp_idx;

    tmp_idx = log_index;

    sprintf(logname, "%s/log_%d.txt", logdir, log_index);


    // log file size if too large
    if (msglogbuf_writeok && get_file_size(logname) > MAX_LOG_FILE_SZIE/2)
    {
        log_index = !log_index;
        sprintf(logname, "%s/log_%d.txt", logdir, log_index);
    }

    if (!msglogbuf_writeok || log_index != tmp_idx)  // first time to write or chnag e file
    {
        fp = filp_open(logname, O_CREAT|O_WRONLY|O_TRUNC|O_LARGEFILE, 0600);
    }
    else
    {
        fp = filp_open(logname, O_CREAT|O_WRONLY|O_APPEND|O_LARGEFILE, 0600);
    }

    return fp;
}

/*
 *	msgog_close() - close log file
 */

int msgog_close(struct file *filp, fl_owner_t id)
{
    return filp_close(filp, id);
}

/*
 *	__msglog_write() - write data into the file
 */


static int __msglog_write(struct file *file, char *str, int len)
{
    int ret;

    ret = file->f_op->write(file, str, len, &file->f_pos);

    return 0;
}

/*
 *	msglog_write() - write data into log file
 */

void msglog_write(char *s, int len)
{
    struct file *file;

    file = msglog_open();

    if (!IS_ERR(file))
    {
        __msglog_write(file, s, len);
        msgog_close(file, NULL);
        msglogbuf_writeok++;
    }
    else
    {
        memset(logdir, 0, LOGNAME_SIZE);
        msglogbuf_writefail++;
    }
}

/*
 *	msglog_flush()
 *
 *
 *	Flush all buffer message to log.
 */

void msglog_flush(void)
{
    static char flush_buf[__MSGLOG_BUF_LEN];
    int len = 0;

    if (logdir[0] != '/')
        msglog_read_option(MSG_LOG_CONF_FILE);

    if (logdir[0] != '/')
        return;

    // no char in the ring buffer (msg buffer)
    if (msglogged_count == 0)
        return;

    // disable by msglog.conf
    if (!opt_msglog)
        return;

    spin_lock_irq(&msglogbuf_lock);

      // copy data into temp_buffer form ring buffer
    if (MSGLOG_INDEX(msglogbuf_start) >= MSGLOG_INDEX(msglogbuf_end))
        len = msglog_buf_len - MSGLOG_INDEX(msglogbuf_start);
    else
        len = MSGLOG_INDEX(msglogbuf_end) - MSGLOG_INDEX(msglogbuf_start);

    memcpy(&flush_buf[0], &MSGLOG_BUF(msglogbuf_start), len);

    if (MSGLOG_INDEX(msglogbuf_start) >= MSGLOG_INDEX(msglogbuf_end))
    {
        memcpy(&flush_buf[len], &MSGLOG_BUF(0), MSGLOG_INDEX(msglogbuf_end));
        len += MSGLOG_INDEX(msglogbuf_end);
    }

    msglogbuf_start = msglogbuf_end;
    msglogged_count = 0;

    spin_unlock_irq(&msglogbuf_lock);

    // write temp buffer data into log
    msglog_write(flush_buf, len);

#if 0   // debug
    {
        char logname[LOGNAME_SIZE];

        sprintf(logname, "%s/log_%d.txt", logdir, log_index);

        printk("write done. File: %s. File size: %lld\n", logname, get_file_size(logname));
    }
#endif
}
#ifdef CONFIG_MSTAR_MSGLOG_EMERGENCY_LOG
static void ram_flush(void)
{
    char ram_logname[LOGNAME_SIZE];
    static char flush_buf[__DRAM_BUF_LEN];
    int len, i;
    int count = 0;
    int start, end, chars;
    struct file *fp;

    printk("\n\n\n*******************************************\n");
    printk("*******************************************\n");
    printk("******** Emergency Log Backup *************\n");
    printk("*******************************************\n");
    printk("*******************************************\n\n\n\n");


    snprintf(ram_logname, LOGNAME_SIZE, "/emeg_log.txt");

    fp = filp_open(ram_logname, O_CREAT|O_WRONLY|O_TRUNC|O_LARGEFILE, 0644);
    if (IS_ERR(fp))
        return;

    spin_lock_irq(&msglogbuf_lock);

    start = (int)readl((void __iomem *)RAM_START_ADDR(ram_info_buf));
    end  = (int)readl((void __iomem *)RAM_END_ADDR(ram_info_buf));
    chars = (int)readl((void __iomem *)RAM_CHARS_ADDR(ram_info_buf));

    if (DRAM_INDEX(start) >= DRAM_INDEX(end))
        len = dram_buf_len - DRAM_INDEX(start);
    else
        len = DRAM_INDEX(end) - DRAM_INDEX(start);

    for (i=start ; i<(start + len) ; i++)
    {
        flush_buf[count] = (char)readb(ram_char_buf + DRAM_INDEX(i));
        count++;
    }

    if (DRAM_INDEX(start) >= DRAM_INDEX(end))
    {
        for (i=0 ; i<DRAM_INDEX(end) ; i++)
        {
            flush_buf[count] = (char)readb(ram_char_buf + DRAM_INDEX(i));
            count++;
        }
        len += DRAM_INDEX(end);
    }

    spin_unlock_irq(&msglogbuf_lock);

    fp->f_op->write(fp, flush_buf, len, &fp->f_pos);

    filp_close(fp, NULL);

    ramwrite_start = 1;
    drambuf_start = drambuf_end = drambuf_chars = 0;
}
#endif

static void msglog_record_char(char c)
{
	MSGLOG_BUF(msglogbuf_end) = c;

#ifdef CONFIG_MSTAR_MSGLOG_EMERGENCY_LOG
    if (ramwrite_start == 1)
    {
        writeb(c, ram_char_buf + DRAM_INDEX(drambuf_end));
        drambuf_end++;

        if (drambuf_end - drambuf_start > dram_buf_len)
            drambuf_start = drambuf_end - dram_buf_len;
        if (drambuf_chars < dram_buf_len)
            drambuf_chars++;

        write_ram_info();
    }
#endif

    msglogbuf_end++;

      // ring buffer is full, discard the oldest data
    if (msglogbuf_end - msglogbuf_start > msglog_buf_len)
        msglogbuf_start = msglogbuf_end - msglog_buf_len;
    if (msglogged_count < msglog_buf_len)
        msglogged_count++;
}

/*
 *	LOG FUNCTIONS
 */

/*
 *  Referenced do_syslog()
 *
 *  Commands to do_msglog:
 *
 *      0 -- Close the log. Currently a NOP.
 *      1 -- Open the log. Currently a NOP.
 *      2 -- Read from msglog buffer.
 *      3 -- Write printk messages to msglog buffer.
 *      4 -- Read and clear all messages remaining in the msglog buffer.
 *      5 -- Synchronously read and clear all messages remaining in the msglog buffer.
 *      6 -- Read from ram buffer when booting time.
 *      9 -- Number of chars in log buffer.
 *     10 -- Print number of unread characters in the dtvlog buffer.
 *     11 -- Write printf messages to msglog buffer.
 */
int do_msglog(int type, const char __user *buf, int len)
{
    int error = 0;
    int preempt;
    int down_ret;
    //unsigned long i;
    //char c;

    switch (type)
    {

        case 0:
            break;
        case 1:
            break;

        case 2: /* Read from kernel buffer */
#if 0
            i = 0;
            spin_lock_irq(&msglogbuf_lock);
            while (!error && (msglogbuf_start != msglogbuf_end) && i < len)
            {
                c = MSGLOG_BUF(msglogbuf_start);
                msglogbuf_start++;
                spin_unlock_irq(&msglogbuf_lock);
                error = __put_user(c, buf);
                buf++;
                i++;
                cond_resched();
                spin_lock_irq(&msglogbuf_lock);
            }
            spin_unlock_irq(&msglogbuf_lock);

            if (!error)
                error = i;

#endif
            break;

        case 3: /* Write some data to ring buffer. */
            msglog_record_print_msg(buf, len, 2);     // from printk
            break;

        case 4: /* Read and clear all messages remaining in the msglog buffer. */
            down_ret = down_interruptible(&msglogbuf_sem);
            msglog_flush();
            up(&msglogbuf_sem);

            if (msglogbuf_complete)
            {
                up(&msglogbuf_complete_sem);
                msglogbuf_complete = 0;
            }
            break;

        case 5: /* Synchronously read and clear all messages remaining in the msglog buffer. */
            MSGLOG_PREEMPT_COUNT_SAVE(preempt);
            msglogbuf_complete = 1;
            wake_up_interruptible(&msglogbuf_wq);
            down_ret = down_interruptible(&msglogbuf_complete_sem);
            MSGLOG_PREEMPT_COUNT_RESTORE(preempt);
#ifdef CONFIG_MSTAR_MSGLOG_EMERGENCY_LOG
            writel(0xdead, ram_info_buf);
#endif
            break;

#ifdef CONFIG_MSTAR_MSGLOG_EMERGENCY_LOG
        case 6: /* Read from ram buffer when booting time. */
            down_ret = down_interruptible(&msglogbuf_sem);
            ram_flush();
            up(&msglogbuf_sem);
            break;
#endif
        case 9: /* Number of chars in log buffer */
            error = msglogbuf_end - msglogbuf_start;
            break;

        case 10: /* Print number of unread characters in the dtvlog buffer. */
            msglog_print_option();
            break;

        case 11: /* Write some data to ring buffer. */
            msglog_record_print_msg(buf, len, 1);     // from printf
            break;

        default:
            error = -EINVAL;
            break;
    }

    return error;

}

int msglog_record_string(char *str, int len)
{
    int printed_len=0;
    char *p;
    static int start_newline = 1;
    static char print_buf[1024];

    /* Prevent overflow */
    if (len > 1023)
        len = 1023;

    printed_len = snprintf(print_buf, len + 1, "%s\n", str);

    /* Copy the output into log buf */
    for (p = print_buf; *p ; p++)
    {
        if (start_newline)
        {
            start_newline = 0;
            if (!*p)
                break;
        }

        msglog_record_char(*p);

        if (*p == '\n')
            start_newline = 1;
    }
    return printed_len;
}


/*
 * @str   : Serial string (printf, printk msg)
 * @count : Serial string count
 * @type  : 1->printf, 2->printk
 */

int msglog_record_print_msg(const char __user *str, int count, int type)
{
    int len;

    char usr_buf[256];
    char *buf = NULL;

    if (type == 1)
    {   /* printf */

        /* Prevent overflow */
        if (count > 256) count = 256;

        if (copy_from_user(usr_buf, str, count))
        	return -EFAULT;

        buf = usr_buf;
    }
    else if (type == 2)
    {   /* printk */
        buf = (char *)str;
    }
    else
    {
        BUG();
    }

    len = msglog_record_string(buf, count);

    return 1;
}

/*
 *	MSGLOG KERNEL THREAD FUNCTIONS
 *	msglog()
 */

static int msglog_main_loop(void *p)
{
    daemonize("msglog");

#ifdef CONFIG_MSTAR_MSGLOG_EMERGENCY_LOG
    rambuf_init();
#endif

    wait_event_interruptible_timeout(msglogbuf_wq, msglogbuf_complete, 15*HZ); /* run after 15sec */


    for ( ; ; )
    {
#ifdef CONFIG_MSTAR_MSGLOG_EMERGENCY_LOG
        if (ram_clear_flush == 1)
        {
            do_msglog(6, NULL, 0);
            ram_clear_flush = 0;
        }
#endif
        //==================
        // debug
        //do_msglog(10, NULL, 0);
        //==================
        do_msglog(4, NULL, 0);

        wait_event_interruptible_timeout(msglogbuf_wq, msglogbuf_complete, opt_interval*HZ);
    }

    return 0;
}

/*
 *	msglog_init()
 */

static int __init msglog_init(void)
{
    pid_t pid;

    /* removed DECLARE_MUTEX_LOCKED() kernel 2.6.24 */
    init_MUTEX_LOCKED(&msglogbuf_complete_sem);

    init_waitqueue_head(&msglogbuf_wq);

    pid = kernel_thread(msglog_main_loop, NULL, CLONE_KERNEL);
    BUG_ON(pid < 0);

    return 0;
}

module_init(msglog_init)

