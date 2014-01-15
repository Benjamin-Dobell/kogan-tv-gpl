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

/*
--------------------------------------------------------------------------------
 LOBOT : System Built-in Self-Auto-Test
 (Component #1: tty snooping driver)
--------------------------------------------------------------------------------
*/

/////////////////////////////////////////////////////////////////////////////////////
///
/// file    msglog.c
/// @brief  This used to save console message to RAM area
/// @author MStar Semiconductor Inc.
/////////////////////////////////////////////////////////////////////////////////////

#include <asm/uaccess.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/kfifo.h>
#include <linux/spinlock.h>

#define MODULE_VERS "1.0"
#define MODULE_NAME "msgio"

#define MAX_CHAR 4096
#define MSGIO_DIR_NAME "msgio"
#define MSGIO_BUF_SIZE 1024*32

typedef struct
{
    int enabled;
    struct kfifo *fifo;
    spinlock_t fifo_lock;
    struct proc_dir_entry *proc_dir;
    struct proc_dir_entry *proc_enable;
    struct proc_dir_entry *proc_data;
} msgio_data;

static msgio_data private;

int do_msgio(const char __user *buf, int len)
{
    char log_buf[MAX_CHAR];

    //enable only when user enabled this option in /proc/enable
    if(private.enabled == 0)
	return 0;

    if(len > MAX_CHAR)
	len = MAX_CHAR;

    if (copy_from_user(log_buf, buf, len))
	return -EFAULT;

    len = kfifo_put(private.fifo, log_buf, len);
    return len;
}

static int proc_read_enable(char *page, char **start, off_t off,
			    int count, int *eof, void *data)
{
    int len;
    len = sprintf(page, "%s\n", (private.enabled==0)?"0":"1");
    return len;
}

static int proc_write_enable(struct file *file, const char *buffer,
			     unsigned long count, void *data)
{
    int len;
    char buf[4];

    if(count > 4)
	len = 4;
    else
	len = count;

    if(copy_from_user(buf, buffer, len))
    {
	return -EFAULT;
    }

    if(buf[0]=='1')
	private.enabled = 1;
    else if(buf[0]=='0')
	private.enabled = 0;

    return len;
}

static int proc_read_data(char *page, char **start, off_t off,
			  int count, int *eof, void *data)
{
    int len;
    int size;

    if(private.enabled == 0)
    {
	len = sprintf(page, "%s\n", "MSGIO_DISABLED");
	return len;
    }
    else
    {
	size = kfifo_len(private.fifo);

	if(size==0)
	    return 0;

	size = (size > count) ? count : size;
	len = kfifo_get(private.fifo, page, size);
	printk("<lobot>----> off = %lu, count = %d, size=%d, len=%d\n", 
	       off, count, size, len);
	return len;
    }
    return 0;
}

static int __init msgio_init(void)
{
    printk(KERN_INFO "msgio_module_init. Module is now loaded.\n");

    memset(&private, 0, sizeof(private));

    private.fifo = kfifo_alloc(MSGIO_BUF_SIZE, GFP_KERNEL, &private.fifo_lock);
    if(IS_ERR(private.fifo))
    {
	printk(KERN_ERR "msgio: kfifo alloc failed\n");
	return 0;
    }

    private.proc_dir = proc_mkdir(MSGIO_DIR_NAME, NULL);
    if(private.proc_dir == NULL)
    {
	remove_proc_entry(MSGIO_DIR_NAME, NULL);
	return -ENOMEM;
    }
    private.proc_dir->owner = THIS_MODULE;

    private.proc_enable = create_proc_entry("enable", 06444, private.proc_dir);
    if(private.proc_enable == NULL)
    {
	remove_proc_entry("enable", private.proc_dir);
	remove_proc_entry(MSGIO_DIR_NAME, NULL);
	return -ENOMEM;
    }
    private.proc_enable->owner = THIS_MODULE;
    private.proc_enable->read_proc = proc_read_enable;
    private.proc_enable->write_proc = proc_write_enable;

    private.proc_data = create_proc_entry("data", 06444, private.proc_dir);
    if(private.proc_data == NULL)
    {
	remove_proc_entry("data", private.proc_dir);
	remove_proc_entry("enable", private.proc_dir);
	remove_proc_entry(MSGIO_DIR_NAME, NULL);
	return -ENOMEM;
    }
    private.proc_data->read_proc = proc_read_data;

    printk(KERN_INFO "/proc/%s created.\n", MSGIO_DIR_NAME);
    return 0;
}

static void __exit msgio_exit(void)
{
    //do nothing here, due to this can not be module.
}

module_init(msgio_init);
module_exit(msgio_exit);

MODULE_AUTHOR("Roger Tsai");
MODULE_DESCRIPTION("procfs tty snooping");
