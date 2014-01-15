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
/// file    mdrv_rfid.c
/// @brief  RFID Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
//#include "MsCommon.h"
#include <linux/autoconf.h>
#include <linux/undefconf.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/poll.h>
#include <linux/wait.h>
#include <linux/cdev.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <asm/io.h>

#include "mst_devid.h"


//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define RFID_DBG_ENABLE              0
#define MOD_RFID_DEVICE_COUNT        1


#if RFID_DBG_ENABLE
#define RFID_DBG(_f)                 (_f)
#else
#define RFID_DBG(_f)
#endif

#define RFID_PRINT(fmt, args...)		//printk("[RFID][%05d] " fmt, __LINE__, ## args)
#define RFID_WARNING(fmt, args...)      //printk(KERN_WARNING "[RFID][%06d] " fmt, __LINE__, ## args)

#define MOD_RFID_NAME                 "ModRFID"

static unsigned int RFID_device = 0;
//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------
// Forward declaration
//--------------------------------------------------------------------------------------------------
static int                      _MDrv_RFID_Open (struct inode *inode, struct file *filp);
static int                      _MDrv_RFID_Release(struct inode *inode, struct file *filp);
static ssize_t                  _MDrv_RFID_Read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
static ssize_t                  _MDrv_RFID_Write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
static struct cdev RFIDDev=
{
    .kobj = {.name= MDRV_NAME_RFID, },
    .owner = THIS_MODULE,
};

static struct file_operations RFID_fops=
{
    open :     _MDrv_RFID_Open,
    release :  _MDrv_RFID_Release,
    read :     _MDrv_RFID_Read,
    write :    _MDrv_RFID_Write,
};

//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
#define REG_ADDR(addr)              (*((volatile unsigned int*)(0xBF800000 + (addr << 1))))
// write 2 bytes
#define REG_WR(_reg_, _val_)        REG_ADDR(_reg_) = (_val_)
#define REG_WI(_reg_, _bit_, _pos_)    \
        REG_ADDR(_reg_) = (_bit_) ? (REG_ADDR(_reg_) | _pos_) : (REG_ADDR(_reg_) & ~(_pos_))

#define REG_RI(_reg_, _pos_)        (REG_ADDR(_reg_) & _pos_)

typedef enum State_BB_e{
    ST_Start,
    ST_Wait_SOF_3T_HI,
    ST_Wait_SOF_1T_LO,
    ST_Wait_SOF_1T_HI,
    ST_Got_Full_Bit,
    ST_Got_Half_Bit,
    ST_Got_EOF_3T_HI,
    ST_Got_EOF_3T_LO
}State_SOF_et;

#define BB_1T_CNT_MIN   5
#define BB_1T_CNT_MAX   11
#define BB_2T_CNT_MIN   13
#define BB_2T_CNT_MAX   19
#define BB_3T_CNT_MIN   21
#define BB_3T_CNT_MAX   27
typedef enum ErrCode_e{
    Err_None=0,
    Err_SOF_Not_Found=  -1,
    Err_Data_Decode=    -2,
    Err_Buffer_Overflow=-3,
    Err_CRC = -4,
    Err_Unknown
}ErrCode_et;


/*************************
 * Global variables
 *************************/
#define CF_pin_pull_high() REG_WI(0x1e86, 1 ,0x01)  //MHal_GPIO_ET_CRS_High
#define CF_pin_pull_low()  REG_WI(0x1e86, 0 ,0x01)   //MHal_GPIO_ET_CRS_Low

#define CF_pin_set_input()   REG_WI(0x1e88, 1 ,0x100)
#define CF_pin_set_output()   REG_WI(0x1e88,0 ,0x100)

#define CF_pin_get_level()  REG_RI(0x1E82 ,0x01)  //MHal_GPIO_ET_CRS_In

#define BB_CLK_ENABLE()     //{PWM2_ON;}
#define BB_CLK_DISABLE()    //{PWM2_OFF;}
#define Delay128fc()         delay16(379)     // delay of sending signel    1/(6M*2) * 128
#define Delay16fc()          delay16(43)     //delay half period of recieving signel
#define Delay80fc()          delay16(248)    // deay of recieving signel

#define TAG_ID_LEN  8

#define BB_CMD_LEN_MAX  31
#define BB_LEN_PER_BIT  8
#define RF_BUF_LEN   (BB_CMD_LEN_MAX*BB_LEN_PER_BIT)
#define TO_CNT_MAX  1000000L

static unsigned char  g_au8Buf[RF_BUF_LEN];
static unsigned char g_u8Idx = RF_BUF_LEN-1;
static unsigned char g_u8Mask = 0x80;
static unsigned char g_u8CurIdx =0;

static unsigned char g_u8TagID[TAG_ID_LEN];
unsigned short  g_u16CrcVal;

#define SearchCmdLen 3
static unsigned char  SearchCmd[SearchCmdLen] = {0x24, 0x01, 0x0};

/*****************************************
 *  System Utilities
 *****************************************/
void Init_GPIO(void)
{
    REG_WR(0x3200, 1);  //init PWM2  6Mhz clk out put
    REG_WR(0x3202, 0);
    REG_WR(0x3216, 1);   //pwm3 period
    REG_WR(0x3218, 0);  //pwm3 duty
    REG_WR(0x321a, 0);  //pwm3 divider

    REG_WR(0x1ea2, 0);//pwm3 do not use as gpio
    REG_WR(0x1E06, 0);
    REG_WR(0x1e5e, 0x1000); //pwm3 do not use as gpio out
    REG_WI(0x1e60, 1,0x0020); //pwm3 do not use as gpio  in
    //REG_WI(0x1e60, 0,0x0040);
    //REG_WI(0x1e60, 0,0x0100);
    REG_WR(0x1ea0, 0x0400); //pwm3 do not use as gpio
    //REG_WI(0x1ee0, 1,0x0080);

    REG_WI(0x1edf, 1 ,0x04);   //init GPIO for CF pin
}


void delay16(int n)
{
    int volatile u32Dummy = 0;
    while(n-- >0)
    {
        u32Dummy++;
    };
}


/**
  base-band 15693 1/4 Start-of-frame.

  T= 128/fc
       _______________      _________
  |___|               |____|

    T       4T          T        2T

 */
void BB_Send_SOF(void)
{
    CF_pin_pull_low();
    Delay128fc();
    CF_pin_pull_high();
    Delay128fc();
    Delay128fc();
    Delay128fc();

    Delay128fc();
    CF_pin_pull_low();
    Delay128fc();
    CF_pin_pull_high();
    Delay128fc();
    Delay128fc();
}

/**
  base-band 15693 End-Of-Frame.

  T= 128/fc
  _________      ____
           |____|

      2T     T    T


 */
void BB_Send_EOF(void)
{
    Delay128fc();
    Delay128fc();
    CF_pin_pull_low();
    Delay128fc();
    CF_pin_pull_high();
    Delay128fc();

}
/**
    base-band send 1-out-of-4
    Send the right most 2 bits of 'val'

    |-                  8T                  -|
 00: ____      ______________________________
         |____|
 01: ______________      ____________________
                   |____|
 10: ________________________      __________
                             |____|
 11: ___________________________________
                                        |____|

                                   |    |    |
                                     T    T
 */
void BB_Send_1O4( unsigned char val )
{
    switch( val )
    {
        case 0:
            //Delay128fc();
            CF_pin_pull_low();
            Delay128fc();
            CF_pin_pull_high();
            Delay128fc();
            Delay128fc();

            Delay128fc();
            Delay128fc();
            Delay128fc();
            Delay128fc();
            break;
        case 1:
            //Delay128fc();
            Delay128fc();
            Delay128fc();
            CF_pin_pull_low();
            Delay128fc();
            CF_pin_pull_high();

            Delay128fc();
            Delay128fc();
            Delay128fc();
            Delay128fc();
            break;
        case 2:
            //Delay128fc();
            Delay128fc();
            Delay128fc();
            Delay128fc();

            Delay128fc();
            CF_pin_pull_low();
            Delay128fc();
            CF_pin_pull_high();
            Delay128fc();
            Delay128fc();
            break;
        case 3:
            //Delay128fc();
            Delay128fc();
            Delay128fc();
            Delay128fc();

            Delay128fc();
            Delay128fc();
            Delay128fc();
            CF_pin_pull_low();
            Delay128fc();
            CF_pin_pull_high();
            break;
        default:
            //should not have this case.
            //does not do anything.
            break;
    }
}


/**
    Send a byte via base-band (GPIO)
 */
void BB_Send_Byte(unsigned char val)
{

    BB_Send_1O4(val & 0x03);
    val >>= 2;
    Delay128fc();
    BB_Send_1O4(val & 0x03);
    val >>= 2;
    Delay128fc();
    BB_Send_1O4(val & 0x03);
    val >>= 2;
    Delay128fc();
    BB_Send_1O4(val & 0x03);
}

/**
    base-band send 15693 command.

    @param buf buffer to be send.
    @param len buffer length
 */
void BB_SEND_693CMD(unsigned char *buf, unsigned char len)
{

#if(1)
    unsigned char i;
    RFID_DBG(printk("\r\nCMD: ");)
    for(i=0;i<len;i++)
    {
        RFID_DBG(printk("%x ", buf[i]);)
    }
#endif
    BB_CLK_ENABLE();        //will be disalbed in RECV_693RPT
    BB_Send_SOF();
    Delay128fc();
    BB_Send_Byte( *buf );
    buf++;
    len--;
    while(len>0)
    {
        Delay128fc();
        BB_Send_Byte( *buf );
        buf++;
        len--;
    }
    BB_Send_EOF();
}


/**
    Reset the counter of base-band bit stream.
 */
static void ResetBBStream(void)
{
    g_u8Mask = 0x80;
    g_u8CurIdx =0;
}
/**
    Turn received buffer data into base-band bit stream.
 */
static unsigned char GetBitFromBBStream(void)
{
    unsigned char val = g_au8Buf[g_u8CurIdx]&g_u8Mask;

    g_u8Mask >>=1;
    if(0 == g_u8Mask){
        g_u8Mask =0x80;
        g_u8CurIdx++;
    }
    if(val)
        return 1;
    else
        return 0;
}

/*SOF decode
               ____________     ___
  ____________|            |___|
      3T            3T      1T   1T

  Data decode
      0       1         1        0
  ____         ____      ________
      |_______|    |____|        |____

    1T   2T    1T    1T    2T      1T


  EOF decode
  ____      ____________
      |____|            |__________

*/
int BB15693_Decode(unsigned char *oBuf)
{
    unsigned char st = ST_Start;
    unsigned char curVal = 0;
    unsigned char bitCnt = 0;

    unsigned char *oPtr = oBuf;
    unsigned char oMask = 0x01;


    ResetBBStream();
    while(1){

        if( curVal == GetBitFromBBStream() ){
            bitCnt++;
            continue;
        }

        switch(st){
            case ST_Start:
                st = ST_Wait_SOF_3T_HI;
                break;
            case ST_Wait_SOF_3T_HI:
                if( bitCnt <BB_3T_CNT_MIN || bitCnt > BB_3T_CNT_MAX){
                    return Err_SOF_Not_Found-10;
                }
                st = ST_Wait_SOF_1T_LO;
                break;
            case ST_Wait_SOF_1T_LO:
                if( bitCnt <BB_1T_CNT_MIN || bitCnt > BB_1T_CNT_MAX){
                    return Err_SOF_Not_Found-11;
                }
                st = ST_Wait_SOF_1T_HI;
                break;
            case ST_Wait_SOF_1T_HI:
                if( bitCnt >=BB_1T_CNT_MIN && bitCnt <= BB_1T_CNT_MAX){
                    st = ST_Got_Full_Bit;
                }
                else if ( bitCnt >=BB_2T_CNT_MIN && bitCnt <= BB_2T_CNT_MAX){
                    st = ST_Got_Half_Bit;
                }
                else{
                    return Err_SOF_Not_Found-12;
                }
                break;
            case ST_Got_Full_Bit:
                if( bitCnt >=BB_1T_CNT_MIN && bitCnt <= BB_1T_CNT_MAX){
                    st = ST_Got_Half_Bit;
                }
                else if ( bitCnt >=BB_3T_CNT_MIN && bitCnt <= BB_3T_CNT_MAX){
                    //The EOF is low, skip reading.
                    st = ST_Got_EOF_3T_HI;
                    return (oPtr - oBuf);   //EOF matched, return byte length decoded
                }
                else{
                    return Err_Data_Decode-20;
                }
                break;
            case ST_Got_Half_Bit:
                if( bitCnt >=BB_1T_CNT_MIN && bitCnt <= BB_1T_CNT_MAX){
                    st = ST_Got_Full_Bit;
                }
                else if ( bitCnt >=BB_2T_CNT_MIN && bitCnt <= BB_2T_CNT_MAX){
                    st = ST_Got_Half_Bit;
                }
                else{
                    return Err_Data_Decode-21;
                }
                if( curVal ){        //an manchester bit 1
                    *oPtr |= oMask;
                }
                oMask <<= 1;
                if( oMask == 0){
                    oMask = 0x01;
                    oPtr++;
                }

                break;
            case ST_Got_EOF_3T_HI:
                if ( bitCnt >=BB_3T_CNT_MIN ){
                    return (oPtr - oBuf);   //EOF matched, return byte length decoded
                }
                else{
                    return Err_Data_Decode-22;
                }
                break;
            default:
                return Err_Unknown;
        }
        curVal = curVal?0:1;
        bitCnt = 1;
    }
    return Err_Unknown;
}
/**
    CRC16 routine for ISO15693 standard.
 */
unsigned int CRC16_CALC693(unsigned char *inPTR, unsigned char inWNO)
{
    unsigned char i, j;
    unsigned int POLYNOMIAL=0x8408, crcREG;

    /*************** For ISO15693 ***************/
    crcREG=0xFFFF;//Preset Value is 0xFFFF
    for (i=0;i<inWNO;i++)
    {
        crcREG=(crcREG)^((unsigned int)(*inPTR));
        for (j=0;j<8;j++)
        {
            crcREG = (crcREG&0x0001)?((crcREG>>1)^POLYNOMIAL):(crcREG>>1);
        }
        inPTR++;
    }
    crcREG =~ (crcREG);
    return crcREG;
}

/**
    Receive data from CF_PIN.
    0. skip begining '1's
    1. sample raw data into buffer.
    2. decode the raw data

 */
char BB_RECV_693RPT(unsigned char *buf)
{

    char result;
    unsigned char *pu8Ptr = &g_au8Buf[0];
    unsigned char mask = 0x80;
    unsigned long int volatile TO_Cnt=0;
    unsigned char i;

    memset(g_au8Buf, 0, RF_BUF_LEN);

    g_u8Idx = RF_BUF_LEN-1;

    //toggle(2);
    CF_pin_set_input();
    TO_Cnt = 0;
    // 0. Skip '1's
    while( CF_pin_get_level() )
    {//skip till SOF
        TO_Cnt++;
        if( TO_Cnt > TO_CNT_MAX )
        {
            BB_CLK_DISABLE();   //BB is enabled in BB_SEND_693CMD()
            return -1;
        }
    }
    TO_Cnt = 0;
    while( !CF_pin_get_level() )
    {
        TO_Cnt++;
        if( TO_Cnt > TO_CNT_MAX )
        {
            return -1;
        }
    }
    // 1. start storing probe results
    while( g_u8Idx > 0){
        if(CF_pin_get_level())
            *pu8Ptr |= mask;
        else
            *pu8Ptr |= 0;
        Delay16fc();
        if(CF_pin_get_level())
            *pu8Ptr |= mask;
                Delay16fc();
        if(CF_pin_get_level())
            *pu8Ptr |= mask;
                Delay16fc();
        if(CF_pin_get_level())
            *pu8Ptr |= mask;
        mask >>= 1;
        if( 0 == mask ){
            mask = 0x80;
            g_u8Idx --;
            pu8Ptr ++;
        }
        Delay80fc();
    }
    BB_CLK_DISABLE();   //BB is enabled in BB_SEND_693CMD()

    for(i = 0;i < RF_BUF_LEN;i++)
    RFID_DBG(printk("\r\ng_au8Buf[i]=0x%x",g_au8Buf[i]);)
    // 2. start decoding.
    result=BB15693_Decode(buf);
    if( result >= 0 )
    {
        g_u16CrcVal = CRC16_CALC693(buf, result-2);
#if(0)
        //DBGputstr("\r\nRES: ");
        for(i=0;i<result;i++)
        {
            //DBGprintf("%x ", buf[i]);
        }
#endif
        if( ((g_u16CrcVal & 0xff) != buf[result-2])  || ((g_u16CrcVal >> 8) !=  buf[result-1])  )
        {
            RFID_DBG(printk(" CRC:%x", g_u16CrcVal & 0xff);)
            RFID_DBG(printk(" %x", g_u16CrcVal >> 8);)
            RFID_DBG(printk(" RECV:%x", buf[result-2]);)
            RFID_DBG(printk(" %x", buf[result-1]);)
            return Err_CRC;
        }
        return result;
    }
    else
    {
        //DBGprintf("\r\nErr: %d", -result);
        return result;
    }

    return result;
}


/**
    Fill the pu8CMD with ISO15693 search command.
 */
void Make693SearchCmd(unsigned char *pu8CMD)
{
    memcpy(pu8CMD, SearchCmd, SearchCmdLen);
    g_u16CrcVal = CRC16_CALC693(pu8CMD, SearchCmdLen);
    pu8CMD[SearchCmdLen]   = g_u16CrcVal & 0xff;
    pu8CMD[SearchCmdLen+1] = g_u16CrcVal>>8;
}


void Get_TagID(void)
{
    unsigned char BBCmdBuf[20];
    unsigned char  BBRptBuf[32];
    char result = 0;
    int i = 0;
    Init_GPIO();
    memset(BBRptBuf, 0, 32);
    printk("\r\nGet Tag ");
    Make693SearchCmd(BBCmdBuf);
    CF_pin_set_output();

    while(result <= 0 && i < 10)   //try 20 times for ID
    {
        BB_SEND_693CMD(BBCmdBuf, SearchCmdLen + 2);
        result = BB_RECV_693RPT(BBRptBuf);
        CF_pin_set_output();
        i++;
    }
    printk("\r\nresult = %d",result);
    if(  result > 0)   //when get ID, copy it to g_u8TagID
    {
        memcpy(g_u8TagID, BBRptBuf+2, TAG_ID_LEN);
    }
    for(i = 0;i < TAG_ID_LEN;i ++)
        printk("  %x;", g_u8TagID[i]);
}
//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

static int _MDrv_RFID_Open (struct inode *inode, struct file *filp)
{
    RFID_PRINT("%s is invoked\n", __FUNCTION__);
    return 0;
}

static int _MDrv_RFID_Release(struct inode *inode, struct file *filp)
{
    RFID_PRINT("%s is invoked\n", __FUNCTION__);
    return 0;
}

static int _MDrv_RFID_Read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    RFID_PRINT("%s is invoked\n", __FUNCTION__);
    if(count != TAG_ID_LEN)
        return(-1);
    copy_to_user(buf,g_u8TagID,TAG_ID_LEN);
    return TAG_ID_LEN;
}

static int _MDrv_RFID_Write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    return 0;
}

static int __init mod_rfid_init(void)
{
    //int result;
    dev_t       dev;
    int s32Ret;

    dev=MKDEV(MDRV_MAJOR_RFID, MDRV_MINOR_RFID);
    s32Ret=register_chrdev_region(dev, MOD_RFID_DEVICE_COUNT, MDRV_NAME_RFID);
    if (0> s32Ret)
    {
        RFID_WARNING("Unable to get major %d\n", MDRV_MAJOR_RFID);
        return s32Ret;
    }
    cdev_init(&RFIDDev, &RFID_fops);
    if (0!= (s32Ret= cdev_add(&RFIDDev, dev, MOD_RFID_DEVICE_COUNT)))
    {
        RFID_WARNING("Unable add a character device\n");
        unregister_chrdev_region(dev, MOD_RFID_DEVICE_COUNT);
        return s32Ret;
    }

    /*result = register_chrdev(dev,MOD_RFID_NAME,&RFID_fops);
    if(result < 0)
    {
        printk("RFID get major name wrong");
        return result;
    }*/
    RFID_device = dev;//result;
    local_irq_disable();  //disable IRQ for the CF_PIN timing
    Get_TagID();   // get the RFID tag ID
    local_irq_enable();
    return 0;
}

static void __exit mod_rfid_exit(void)
{
    //unregister_chrdev(RFID_device,MOD_RFID_NAME);
    unregister_chrdev_region(RFID_device, MOD_RFID_DEVICE_COUNT);
}

module_init(mod_rfid_init);
module_exit(mod_rfid_exit);

MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("RFID driver");
MODULE_LICENSE("MSTAR");

