#ifndef _MDRV_SOFTWARE_IR_H_
#define _MDRV_SOFTWARE_IR_H_


//#include "mdrv_types.h"
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/poll.h>




typedef struct
{
    int                         s32IRMajor;
    int                         s32IRMinor;
    struct cdev                 cDevice;
    struct file_operations      IRFop;
}SoftwareIRModHandle;

unsigned int MDrv_Software_IR_Poll(struct file *, poll_table *);

ssize_t MDrv_Software_IR_Read(struct file *, char __user *, size_t, loff_t *);

void MDrv_Software_IR_SendKey(void);

void MDrv_Software_IR_TakeOver(void);

void MDrv_Software_IR_DropOut(void);

void MDrv_Software_IR_PlaySingleKey(unsigned int);

void MDrv_Software_IR_Init(void);

void MDrv_Software_IR_Exit(void);

void MDrv_Software_IR_GetKey(void);


#endif // _MDRV_SOFTWARE_IR_H_



