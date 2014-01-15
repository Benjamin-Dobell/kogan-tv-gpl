#include "mdrv_software_ir_io.h"
#include "mdrv_software_ir.h"
#include "mst_devid.h"
#include <linux/module.h>




#define MOD_SOFTWARE_IR_DEVICE_COUNT     1

static int                      _mod_software_ir_open (struct inode *inode, struct file *filp);
static int                      _mod_software_ir_release(struct inode *inode, struct file *filp);
static ssize_t                  _mod_software_ir_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
static ssize_t                  _mod_software_ir_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);
static unsigned int             _mod_software_ir_poll(struct file *filp, poll_table *wait);
static int                      _mod_software_ir_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);


SoftwareIRModHandle IRDev=
{
    .s32IRMajor=               MDRV_MAJOR_SOFTWARE_IR,
    .s32IRMinor=               MDRV_MINOR_SOFTWARE_IR,
    .cDevice=
    {
        .kobj=                  {.name= MDRV_NAME_SOFTWARE_IR, },
        .owner  =               THIS_MODULE,
    },
    .IRFop=
    {
        .open=                  _mod_software_ir_open,
        .release=               _mod_software_ir_release,
        .read=                  _mod_software_ir_read,
        .write=                 _mod_software_ir_write,
        .poll=                  _mod_software_ir_poll,
        .ioctl=                 _mod_software_ir_ioctl,
    },
};


static int _mod_software_ir_open(struct inode *inode, struct file *filp)
{


	SoftwareIRModHandle *dev;

    printk("%s is invoked\n", __FUNCTION__);

    dev = container_of(inode->i_cdev, SoftwareIRModHandle, cDevice);
	filp->private_data = dev;

    return 0;

}

static int _mod_software_ir_release(struct inode *inode, struct file *filp)
{


    printk("%s is invoked\n", __FUNCTION__);


    return 0;
}

static ssize_t _mod_software_ir_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{

    //printk("%s is invoked\n", __FUNCTION__);

    return MDrv_Software_IR_Read(filp, buf, count, f_pos);

}

static ssize_t _mod_software_ir_write (struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{


    printk("%s is invoked\n", __FUNCTION__);


    return 0;
}

static unsigned int _mod_software_ir_poll(struct file *filp, poll_table *wait)
{

    //printk("%s is invoked\n", __FUNCTION__);

    return MDrv_Software_IR_Poll(filp, wait);


}

static int _mod_software_ir_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
    int retval = 0;
    int keyinfo;

    //printk("%s is invoked\n", __FUNCTION__);

    if ((SOFTWARE_IR_IOC_MAGIC != _IOC_TYPE(cmd)) || (_IOC_NR(cmd)> SOFTWARE_IR_IOC_MAXNR))
    {
        return -ENOTTY;
    }

    switch(cmd)
    {
        case MDRV_SOFTWARE_IR_SEND_KEY:
            printk("\n\nThis is MDRV_SOFTWARE_IR_IO_SEND_KEY\n\n");
            retval = __get_user(keyinfo, (int __user *)arg);
            printk("\n\nkeyinfo is %d\n\n", keyinfo);
            MDrv_Software_IR_SendKey();
            break;
        case MDRV_SOFTWARE_IR_GET_KEY:
	     	printk("\n\nThis is MDRV_SOFTWARE_IR_GET_KEY\n\n");
            MDrv_Software_IR_GetKey();
            break;
        case MDRV_SOFTWARE_IR_TAKE_OVER:
            printk("\n\nThis is MDRV_SOFTWARE_IR_TAKE_OVER\n\n");
            MDrv_Software_IR_TakeOver();
            break;
        case MDRV_SOFTWARE_IR_DROP_OUT:
            printk("\n\nThis is MDRV_SOFTWARE_IR_DROP_OUT\n\n");
            MDrv_Software_IR_DropOut();
            break;
        case MDRV_SOFTWARE_IR_PLAY_SINGLE_KEY:
            printk("\n\nThis is MDRV_SOFTWARE_IR_PLAY_SINGLE_KEY\n\n");
            retval = __get_user(keyinfo, (int __user *)arg);
            MDrv_Software_IR_PlaySingleKey(keyinfo);
            break;
        default:
            printk("ioctl: unknown command\n");
            return -ENOTTY;
    }

    return 0;
}

static int __init mod_software_ir_init(void)
{
    int ret;
    dev_t dev;

    printk("%s is invoked\n", __FUNCTION__);

    if (IRDev.s32IRMajor)
    {
        dev = MKDEV(IRDev.s32IRMajor, IRDev.s32IRMinor);
        ret = register_chrdev_region(dev, MOD_SOFTWARE_IR_DEVICE_COUNT, MDRV_NAME_SOFTWARE_IR);
    }
    else
    {
        ret = alloc_chrdev_region(&dev, IRDev.s32IRMinor, MOD_SOFTWARE_IR_DEVICE_COUNT, MDRV_NAME_SOFTWARE_IR);
        IRDev.s32IRMajor = MAJOR(dev);
    }

    if ( 0 > ret)
    {
        printk("Unable to get major %d\n", IRDev.s32IRMajor);
        return ret;
    }

    cdev_init(&IRDev.cDevice, &IRDev.IRFop);
    if (0!= (ret= cdev_add(&IRDev.cDevice, dev, MOD_SOFTWARE_IR_DEVICE_COUNT)))
    {
        printk("Unable add a character device\n");
        unregister_chrdev_region(dev, MOD_SOFTWARE_IR_DEVICE_COUNT);
        return ret;
    }

    MDrv_Software_IR_Init();


    return 0;
}

static void __exit mod_software_ir_exit(void)
{
    printk("%s is invoked\n", __FUNCTION__);
    MDrv_Software_IR_Exit();
    cdev_del(&IRDev.cDevice);
    unregister_chrdev_region(MKDEV(IRDev.s32IRMajor, IRDev.s32IRMinor), MOD_SOFTWARE_IR_DEVICE_COUNT);

}


module_init(mod_software_ir_init);
module_exit(mod_software_ir_exit);

MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("Software IR driver");
MODULE_LICENSE("GPL");


