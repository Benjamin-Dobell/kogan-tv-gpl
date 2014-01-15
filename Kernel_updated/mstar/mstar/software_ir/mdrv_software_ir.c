#include <linux/autoconf.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/wait.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <asm/io.h>


#include "mdrv_software_ir.h"

#include <media/ir-common.h>
#include <version.h>
#include <linux/input.h>
#include <linux/semaphore.h>





//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------
int has_key = 0; //0 means no key, 1 means having key
unsigned char ReceivedKey = 0; //u8Key received from mdrv_ir.c
unsigned char ReceivedRepeatFlag = 0; //u8RepeatFlag received from mdrv_ir.c
unsigned char ReceivedSystem; //u8System received from mdrv_ir.c
unsigned char ReceivedRepeatFlag; //u8RepeatFlag received from mdrv_ir.c
struct input_dev       *Receivedinput; //received from mdrv_ir.c
struct ir_input_state   *Receivedir; //received from mdrv_ir.c
int software_ir_server_ready = 0;
struct semaphore sem;
int SoftwareIRProcessingUndone = 1;



int take_over_by_software_ir(unsigned char u8Key, unsigned char u8System, unsigned char u8RepeatFlag, struct input_dev *input_from_mdrv_ir, struct ir_input_state *ir_from_mdrv_ir)
{

    down(&sem);
    ReceivedKey = u8Key;
    ReceivedSystem = u8System;
    ReceivedRepeatFlag = u8RepeatFlag;
    up(&sem);
    Receivedinput = input_from_mdrv_ir;
    Receivedir = ir_from_mdrv_ir;
    has_key = 1;
	printk("\n\nnew software ir has been called\n\n");

	return 0;
}
EXPORT_SYMBOL(take_over_by_software_ir);

int take_over_by_software_ir_dfb(unsigned char u8Key, unsigned char u8RepeatFlag)
{
    printk("\n\nThe IR key received from mdrv-ir.ko is %u\n\n", u8Key);
    ReceivedKey = u8Key;
    printk("\n\nThe IR key received from mdrv-ir.ko is %u\n\n", ReceivedKey);

    printk("\n\nThe repeat flag received from mdrv-ir.ko is %u\n\n", u8RepeatFlag);
    ReceivedRepeatFlag = u8RepeatFlag;
    printk("\n\nThe repeat flag received from mdrv-ir.ko is %u\n\n", ReceivedRepeatFlag);

    has_key = 1;
    //SoftwareIRProcessingUndone = 0;
    return 1;
}
EXPORT_SYMBOL(take_over_by_software_ir_dfb);

int software_ir_processing_undone(void)
{
    return SoftwareIRProcessingUndone;
}
EXPORT_SYMBOL(software_ir_processing_undone);

int set_software_ir_processing_undone(void)
{
    SoftwareIRProcessingUndone = 1;
    return 0;
}
EXPORT_SYMBOL(set_software_ir_processing_undone);



int software_ir_enable(void)
{
    return software_ir_server_ready;
}
EXPORT_SYMBOL(software_ir_enable);

unsigned int MDrv_Software_IR_Poll(struct file *filp, poll_table *wait)
{
    unsigned int mask;

    mask = 0;

    if(has_key)
    {
        mask |= POLLIN|POLLRDNORM;
        has_key = 0;
    }

    return mask;
}

ssize_t MDrv_Software_IR_Read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{

    printk("%s is invoked\n", __FUNCTION__);

    if(count == sizeof(unsigned int))
    {
        if(ReceivedKey)
        {
            down(&sem);
            copy_to_user(buf, &ReceivedKey, sizeof(unsigned int));
            up(&sem);
            return count;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}

void MDrv_Software_IR_GetKey()
{/* user space program, "msvr", has received and processed the IR key passed by mdrv-software-ir.ko*/
    SoftwareIRProcessingUndone = 0;
}

void MDrv_Software_IR_SendKey()
{
    printk("\n\nThis is MDRV_SOFTWARE_IR_SEND_KEY\n\n");

#ifdef CONFIG_MSTAR_IR_INPUT_DEVICE

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 35)
    down(&sem);
    printk("\n\nThe key code recieved by mdrv_software_ir.c is %u %u %u\n\n", ReceivedKey, ReceivedSystem, ReceivedRepeatFlag);
    ir_input_keydown(Receivedinput, Receivedir, (ReceivedSystem << 8) | ReceivedKey);
#else
    ir_input_keydown(Receivedinput, Receivedir, ReceivedKey, (ReceivedRepeatFlag << 8) | ReceivedKey);
#endif
    ir_input_nokey(Receivedinput, Receivedir);

    ReceivedKey = 0;
    up(&sem);

#endif
}

void MDrv_Software_IR_TakeOver()
{
    software_ir_server_ready = 1;
}

void MDrv_Software_IR_DropOut()
{
    software_ir_server_ready = 0;
}

void MDrv_Software_IR_PlaySingleKey(unsigned int key)
{
        printk("\n\nThis is MDRV_SOFTWARE_IR_PLAY_SINGLE_KEY\n\n");

#ifdef CONFIG_MSTAR_IR_INPUT_DEVICE

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 35)
        printk("\n\nThe key code recieved by mdrv_software_ir.c is %u\n\n", key);
        ir_input_keydown(Receivedinput, Receivedir, (ReceivedSystem << 8) | key);
#else
        ir_input_keydown(Receivedinput, Receivedir, key, (ReceivedRepeatFlag << 8) | key);
#endif
        ir_input_nokey(Receivedinput, Receivedir);

#endif
}

void MDrv_Software_IR_Init()
{
    init_MUTEX(&sem);

    ReceivedKey = 0;
}

void MDrv_Software_IR_Exit()
{
    software_ir_server_ready = 0;
}




