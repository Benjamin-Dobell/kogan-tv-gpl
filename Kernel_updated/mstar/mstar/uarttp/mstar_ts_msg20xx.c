#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/types.h>
#include <linux/input.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/time.h>
#include <asm/irq.h>
#include <asm/gpio.h>

#include <asm/io.h>
#include "hal_touch_screen_c_msg20xx.h"
#include "drv_touch_screen_pub.h"
#include "touchscreen_msg_pub.h"
#include "drvUART.h"
#include "halUART.h"
#include "chip_int.h"



/*=============================================================*/
// Macro definition
/*=============================================================*/

#define TS_UART_IRQ             E_IRQEXPH_UART2MCU

#define MS_TS_MSG20XX_DEBUG 0
#if (MS_TS_MSG20XX_DEBUG == 1)
#define DBG(fmt, arg...) pr_info(fmt, ##arg)
#else
#define DBG(fmt, arg...)
#endif

#define MS_TS_MSG20XX_X_MIN   0
#define MS_TS_MSG20XX_X_MAX   2047
#define MS_TS_MSG20XX_Y_MIN   0
#define MS_TS_MSG20XX_Y_MAX   2047

#define MDRV_NAME_MSTP                 "mstp"
#define MDRV_MAJOR_MSTP                0xfb
#define MDRV_MINOR_MSTP                0x00
#define     MDRV_MSTP_DEVICE_COUNT           1
#define     MDRV_MSTP_NAME                           "MSTP"
#define MAX_DELAY_SPAN                    (HZ*50/1000)
//#define MAX_CLICK_SPAN                    (HZ*80/1000)
//#define MIN_CLICK_SPAN                    (HZ*75/1000)
#define MAX_CLICK_DISTANCE                 40
//#define MAX_DOUBLE_CLICK_DELAY         4
/*=============================================================*/
// Data type definition
/*=============================================================*/
struct ms_ts_msg20xx_drvdata {
  struct input_dev *input;
  MdlTouchScreenInfo_t tsInfo;
  uint32_t lastjiff; 
  vm_TouchPoint_t _pretouchpoint;
//  vm_TouchPoint_t _firsttouchpoint;  
//  int extint_pin;
};

typedef struct
{
    int s32Major;
    int s32Minor;
    int refCnt;
    struct cdev cdev;
    struct file_operations fops;
}MSTP_DEV;

static MSTP_DEV _devMSTP =
{
    .s32Major = MDRV_MAJOR_MSTP,
    .s32Minor = MDRV_MINOR_MSTP,
    .refCnt = 0,
    .cdev =
    {
        .kobj = {.name= MDRV_NAME_MSTP, },
        .owner = THIS_MODULE,
    },
};

//int touch_state = 0;

static void ms_ts_msg20xx_report_touchdata(struct ms_ts_msg20xx_drvdata *tdata)
{
    struct input_dev *input_dev = tdata->input;
    uint32_t cur_jiff = jiffies;
     uint32_t diff_t;

    if(cur_jiff>= tdata->lastjiff)
        diff_t = cur_jiff-tdata->lastjiff;
    else
        diff_t = (((uint32_t)-1)-tdata->lastjiff)+cur_jiff+1;

//////////////////////   Extract FingerNum for Multi-touch/////////////////////////////
//      for (i = 0; i < tdata->tsInfo.nFingerNum; i++) 
//    {
//        if(i==1)
//            DBG("*************** Multi-touch!! ***************\n");
//      
//        input_report_abs(input_dev, ABS_MT_POSITION_X, tdata->tsInfo.atPoint[i].Col);
//            input_report_abs(input_dev, ABS_MT_POSITION_Y, tdata->tsInfo.atPoint[i].Row);
//
//        DBG("==========================================================\n");
//        DBG("*** finger_id:%d  ABS_MT_POSITION_X: %d  ABS_MT_POSITION_Y: %d ***\n", 
//          i, tdata->tsInfo.atPoint[i].Col, tdata->tsInfo.atPoint[i].Row);
//
//        /* Report ABS_MT_TOUCH_MAJOR as 0 to indicate the finger release */
//        if(tdata->tsInfo.nKeyMode == TOUCHSCREEN_RELEASED)
//        {
//            input_report_abs(input_dev, ABS_MT_TOUCH_MAJOR, 0);
//            DBG("*** ABS_MT_TOUCH_MAJOR = 0 ***\n");
//        }
//        input_mt_sync(input_dev);
//      }    
//////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////single click/////////////////////////////////////////////////////
    if(diff_t<MAX_DELAY_SPAN && (tdata->_pretouchpoint.Col|| tdata->_pretouchpoint.Row)){
        input_report_rel(input_dev, REL_X, tdata->tsInfo.atPoint[0].Col - tdata->_pretouchpoint.Col);
        input_report_rel(input_dev, REL_Y, tdata->_pretouchpoint.Row - tdata->tsInfo.atPoint[0].Row);


// only report the first point info
/*        input_report_abs(input_dev, ABS_X, tdata->tsInfo.atPoint[0].Col);
       input_report_abs(input_dev, ABS_Y, TOUCH_SCREEN_HEIGHT-100-tdata->tsInfo.atPoint[0].Row);
*/     
    input_sync(input_dev);  
}

    if(diff_t<MAX_DELAY_SPAN*10 &&
//        diff_t>MIN_CLICK_SPAN &&
        abs(tdata->tsInfo.atPoint[0].Col - tdata->_pretouchpoint.Col) < MAX_CLICK_DISTANCE &&
        abs(tdata->_pretouchpoint.Row - tdata->tsInfo.atPoint[0].Row) < MAX_CLICK_DISTANCE &&
        tdata->tsInfo.nKeyMode == TOUCHSCREEN_RELEASED){
            printk("###########Report the click###########\n");
            printk("abs(x):%d, abs(y):%d\n",
                abs(tdata->tsInfo.atPoint[0].Col - tdata->_pretouchpoint.Col),
                abs(tdata->_pretouchpoint.Row - tdata->tsInfo.atPoint[0].Row));
            printk("time:%d\n",diff_t);
            
            input_report_key(input_dev, BTN_LEFT, 0x01);
            input_report_key(input_dev, BTN_LEFT, 0x00);
         input_sync(input_dev);   
        }
        
    
    tdata->_pretouchpoint.Col = tdata->tsInfo.atPoint[0].Col;
    tdata->_pretouchpoint.Row = tdata->tsInfo.atPoint[0].Row;
    tdata->lastjiff = cur_jiff;  
    
////////////////////////////////////Double click////////////////////////////////////////////
/*if(tdata->_pretouchpoint.Col|| tdata->_pretouchpoint.Row){
    if(touch_state  == 0 && tdata->tsInfo.nKeyMode != TOUCHSCREEN_RELEASED && diff_t < MAX_DELAY_SPAN)
    {
            printk("1\n");
            input_report_rel(input_dev, REL_X, tdata->tsInfo.atPoint[0].Col - tdata->_pretouchpoint.Col);
            input_report_rel(input_dev, REL_Y, tdata->_pretouchpoint.Row - tdata->tsInfo.atPoint[0].Row);
            input_sync(input_dev);         
     }
    else if(touch_state == 0 && tdata->tsInfo.nKeyMode == TOUCHSCREEN_RELEASED)
    {
            printk("2\n");
            touch_state = 1;
            tdata->_firsttouchpoint.Col = tdata->tsInfo.atPoint[0].Col;
            tdata->_firsttouchpoint.Row = tdata->tsInfo.atPoint[0].Row;
     }
          else if(touch_state ==1 && tdata->tsInfo.nKeyMode != TOUCHSCREEN_RELEASED && diff_t < MAX_DELAY_SPAN)
            {
                printk("3\n");
                touch_state = 2;
                input_report_rel(input_dev, REL_X, tdata->tsInfo.atPoint[0].Col - tdata->_pretouchpoint.Col);
                input_report_rel(input_dev, REL_Y, tdata->_pretouchpoint.Row - tdata->tsInfo.atPoint[0].Row);
                input_sync(input_dev);               
            }
                else if(touch_state ==2 && 
//                          double_click_status <= MAX_DOUBLE_CLICK_DELAY &&
                          tdata->tsInfo.nKeyMode == TOUCHSCREEN_RELEASED &&
                          abs(tdata->tsInfo.atPoint[0].Col - tdata->_firsttouchpoint.Col) < MAX_CLICK_DISTANCE &&
                          abs(tdata->_firsttouchpoint.Row - tdata->tsInfo.atPoint[0].Row) < MAX_CLICK_DISTANCE &&
                          diff_t<MAX_DELAY_SPAN*10)
                    {
                        printk("4\n");
                        touch_state = 0;
                        input_report_key(input_dev, BTN_LEFT, 0x01);
                        input_report_key(input_dev, BTN_LEFT, 0x00);
                        input_sync(input_dev);
                    }
                        else if(touch_state ==2 && 
//                               double_click_status <= MAX_DOUBLE_CLICK_DELAY &&
                                  tdata->tsInfo.nKeyMode == TOUCHSCREEN_RELEASED &&
                                  (abs(tdata->tsInfo.atPoint[0].Col - tdata->_firsttouchpoint.Col) < MAX_CLICK_DISTANCE ||
                                  abs(tdata->_firsttouchpoint.Row - tdata->tsInfo.atPoint[0].Row) < MAX_CLICK_DISTANCE ||
                                  diff_t>MAX_DELAY_SPAN*10))
                            {
                                   printk("4.1\n");
                                   touch_state = 0;
                            }
                                else if(diff_t < MAX_DELAY_SPAN)
                            {
                                 printk("5\n");
//                              double_click_status++;
//                              printk("double_click_status=%d\n",double_click_status);
//                              touch_state = 0;
                                 input_report_rel(input_dev, REL_X, tdata->tsInfo.atPoint[0].Col - tdata->_pretouchpoint.Col);
                                 input_report_rel(input_dev, REL_Y, tdata->_pretouchpoint.Row - tdata->tsInfo.atPoint[0].Row);
                                 input_sync(input_dev);               
                            }
}
      printk("6\n");  
      printk("%d,%d\n",abs(tdata->tsInfo.atPoint[0].Col - tdata->_firsttouchpoint.Col),abs(tdata->_firsttouchpoint.Row - tdata->tsInfo.atPoint[0].Row));
      printk("%d,%d,%d,%d\n\n",tdata->tsInfo.atPoint[0].Col,tdata->tsInfo.atPoint[0].Row,tdata->_firsttouchpoint.Col,tdata->_firsttouchpoint.Row);

      tdata->_pretouchpoint.Col = tdata->tsInfo.atPoint[0].Col;
      tdata->_pretouchpoint.Row = tdata->tsInfo.atPoint[0].Row;
      tdata->lastjiff = cur_jiff;*/
}


/*===============================*/
irqreturn_t ms_ts_msg20xx_isr(int irq, void *dev_id)
{
#if 0 // no use now!
    u32 tscr_process_cnt;
#endif
    s32 ret;
    struct ms_ts_msg20xx_drvdata *tdata = dev_id;

    ret = DrvTouchScreenGetData(&tdata->tsInfo);

    if (ret > 0)
    {
        // send coordinate and function info to MMI
        ms_ts_msg20xx_report_touchdata(tdata);
    }
    else if(ret <= 0)
    {
        DBG("*** ret <= 0 fail !! ***\n");
    }

    return IRQ_HANDLED;
}



static int ms_ts_msg20xx_probe(void)
{
    struct ms_ts_msg20xx_drvdata *tdata;
    struct input_dev *input_dev;
    int ret;

    DBG("*** %s ***\n", __FUNCTION__);

    tdata = kzalloc(sizeof(*tdata) + 
      MAX_TOUCH_FINGER * sizeof(*tdata->tsInfo.atPoint), GFP_KERNEL);
    if (!tdata)
            return -ENOMEM;

    tdata->_pretouchpoint.Col = 0;
    tdata->_pretouchpoint.Row = 0;    
    input_dev = input_allocate_device();
    if (!input_dev) {
            ret = -ENOMEM;
            pr_err("*** input device allocation failed ***\n");
            goto err1;
      }

        
    input_dev->name = "ms-touchscreen-msg20xx";
    input_dev->phys = "ms-touchscreen-msg20xx/input0";
    input_dev->id.bustype = BUS_HOST;
    input_dev->id.vendor = 0x0001;
    input_dev->id.product = 0x0001;
    input_dev->id.version = 0x0100;

    tdata->input = input_dev;

    input_set_drvdata(input_dev, tdata);

//be detected as mouse device
    set_bit(EV_KEY, input_dev->evbit);
    set_bit(BTN_LEFT, input_dev->keybit);
    input_dev->keybit[BIT_WORD(BTN_LEFT)] = BIT_MASK(BTN_LEFT) |
		BIT_MASK(BTN_MIDDLE) | BIT_MASK(BTN_RIGHT);    
    
    set_bit(EV_REL, input_dev->evbit);
    set_bit(REL_X, input_dev->relbit);
    set_bit(REL_Y, input_dev->relbit);

/* report the absolute axis event only */
/*    set_bit(EV_ABS, input_dev->evbit);
    set_bit(ABS_X, input_dev->absbit);
    set_bit(ABS_Y, input_dev->absbit);   
*/

///////////////////////// For muti-touch///////////////////////////////////    
//    input_set_abs_params(input_dev, ABS_MT_POSITION_X, MS_TS_MSG20XX_X_MIN,
//          MS_TS_MSG20XX_X_MAX, 0, 0);
//    input_set_abs_params(input_dev, ABS_MT_POSITION_Y, MS_TS_MSG20XX_Y_MIN,
//          MS_TS_MSG20XX_Y_MAX, 0, 0);
//    /* just be used to indicate the finger-release */
//    input_set_abs_params(input_dev, ABS_MT_TOUCH_MAJOR, 0, 255, 0, 0);
/////////////////////////////////////////////////////////////////////////

/*    input_set_abs_params(input_dev, ABS_X, MS_TS_MSG20XX_X_MIN,
          MS_TS_MSG20XX_X_MAX, 0, 0);  
    input_set_abs_params(input_dev, ABS_Y, MS_TS_MSG20XX_Y_MIN,
          MS_TS_MSG20XX_Y_MAX, 0, 0);
*/

    ret = input_register_device(input_dev);
    if (ret < 0)
    {
        input_free_device(input_dev);
        printk("*** Unable to register ms-touchscreen input device ***\n");
        goto err1;
    }

    /* request an irq and register the isr */
    ret = request_irq(TS_UART_IRQ, ms_ts_msg20xx_isr,
                    IRQF_SHARED |
                    IRQF_TRIGGER_HIGH,
                    "ms_touchscreen_msg20xx", tdata);
                    
        if (ret) {
              printk("*** ms-ts-msg20xx: Unable to claim irq %d; error %d ***\n",    TS_UART_IRQ, ret);
            goto err1;
        }

    printk("*** ms touchscreen registered ***\n");
    return 0;

err1:
    kfree(tdata);
    return ret;
/////////////////////////////////GPIO=>UART///////////////////////////////////    
}

static int __devexit ms_ts_msg20xx_remove(struct platform_device *pdev)
{
  struct ms_ts_msg20xx_drvdata *tdata = platform_get_drvdata(pdev);
  struct input_dev *input_dev = tdata->input;
 
  input_unregister_device(input_dev);
  kfree(tdata);
    /*
     * Clear the interrupt registers.
     */
    (void) FASTU_ReadByte(UART_LSR);
    (void) FASTU_ReadByte(UART_RX);
    (void) FASTU_ReadByte(UART_IIR);
    (void) FASTU_ReadByte(UART_MSR);
    
  return 0;
}

static int __init ms_ts_msg20xx_init(void)
{
    int s32Ret;
    dev_t  dev;
    
//       MDrv_MSTP_MMIO_Init();
        
    //register the device driver
    if(_devMSTP.s32Major)
    {
        dev = MKDEV(_devMSTP.s32Major, _devMSTP.s32Minor);
        s32Ret = register_chrdev_region(dev, MDRV_MSTP_DEVICE_COUNT, MDRV_MSTP_NAME);
    }
    else
    {
        s32Ret = alloc_chrdev_region(&dev, _devMSTP.s32Minor, MDRV_MSTP_DEVICE_COUNT, MDRV_MSTP_NAME);
        _devMSTP.s32Major = MAJOR(dev);
    }    
    
    cdev_init(&_devMSTP.cdev, &_devMSTP.fops);    
    if (0 != (s32Ret= cdev_add(&_devMSTP.cdev, dev, MDRV_MSTP_DEVICE_COUNT)))
    {
        unregister_chrdev_region(dev, MDRV_MSTP_DEVICE_COUNT);
        goto out;
    }
    
    /*
     * Clear the interrupt registers.
     */
    (void) FASTU_ReadByte(UART_LSR);
    (void) FASTU_ReadByte(UART_RX);
    (void) FASTU_ReadByte(UART_IIR);
    (void) FASTU_ReadByte(UART_MSR);

    //Disable the interrupt    
    FASTU_WriteByte(UART_IER, 0);

        // card_reset_reg must set to 1 for rx
        FASTU_WriteByte(8*2, 0x10);
     FASTU_WriteByte(UART_FCR, 0x10);

        // Reset receiver and transmiter
        FASTU_WriteByte(UART_FCR, UART_FCR_ENABLE_FIFO | UART_FCR_CLEAR_RCVR | UART_FCR_CLEAR_XMIT | UART_FCR_TRIGGER_1);

    /*
     * Now, initialize the UART
     */
        FASTU_WriteByte(UART_LCR,  UART_LCR_WLEN8);

    //set the baudrate    
        MDrv_UART_MSTP_Init(57600);

    //connect FastUart<=>Port1 as source; 
    //select PAD_TS0_D4/PAD_TS0_D2 as Fast Uart RX/TX     
        mdrv_uart_mstp_connect();
        
/*     while (1)
       {
            //while (!(FASTU_REG8(UART_LSR) & UART_LSR_THRE))
                FASTU_REG8(UART_TX)='A';
        //    printk(".");
       }
*/
    ms_ts_msg20xx_probe();
    
/*    printk("!!IER=0x%08x\n\n", FASTU_ReadByte(UART_IER));
    
    printk("!!TX=0x%08x\n", FASTU_ReadByte(UART_TX));
    printk("!!RX=0x%08x\n", FASTU_ReadByte(UART_RX));
    printk("!!LSR=0x%08x\n", FASTU_ReadByte(UART_LSR));
    printk("!!IIR=0x%08x\n", FASTU_ReadByte(UART_IIR));
    printk("!!MSR=0x%08x\n", FASTU_ReadByte(UART_MSR));
    printk("!!IER=0x%08x\n", FASTU_ReadByte(UART_IER));    
    printk("!!LCR=0x%08x\n", FASTU_ReadByte(UART_LCR));
    printk("0x101EA6=0x%08x\n", GENERAL_REG8(0x101E00, 0xA6));
    printk("0x101E05=0x%08x\n", GENERAL_REG8(0x101E00, 0x05));    
    printk("0x101EA7=0x%08x\n", GENERAL_REG8(0x101E00, 0xA7));    
    printk("0x101EA8=0x%08x\n", GENERAL_REG8(0x101E00, 0xA8));    */

    FASTU_WriteByte(UART_IER,  UART_IER_RDI );
    //printk("IER = %08x\n", &FASTU_REG8(UART_IER));
//    while (1)
//       {
//            while (!(FASTU_REG8(UART_LSR) & UART_LSR_THRE));
//                FASTU_REG8(UART_TX)='A';
//       }
    //for(;;)
        //{
        //FASTU_WriteByte(UART_TX, 'c');
        //printk(".");
//}
    return 0;
    
out:
    printk("Unable to register driver\n");        
    return s32Ret;
}

static void __exit ms_ts_msg20xx_exit(void)
{

//    free_irq(TS_UART_IRQ, NULL);
    
    /*
     * Clear the interrupt registers.
     */
    (void) FASTU_ReadByte(UART_LSR);
    (void) FASTU_ReadByte(UART_RX);
    (void) FASTU_ReadByte(UART_IIR);
    (void) FASTU_ReadByte(UART_MSR);
    
}

module_init(ms_ts_msg20xx_init);
module_exit(ms_ts_msg20xx_exit);
MODULE_LICENSE("GPL");


