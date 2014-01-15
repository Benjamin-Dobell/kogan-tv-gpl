////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2011-2012 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// ("MStar Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    sec_logo_main.c
/// @brief  Show LOGO when boot up.
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/unistd.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/mm.h>
#include <linux/dma-mapping.h>
#include <linux/gfp.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/mach/map.h>
#include <linux/err.h>
#include <linux/delay.h>
#include "inverter.h"
#include "sec_logo_main.h"
#include "SdMisc.h"
#include "apiSWI2C.h"
#include "gpio_macro_x10.h"

// for kthread
#include <linux/wait.h>
#include <linux/kthread.h>
#include <linux/ioctl.h>

// for pnl init
#define REG_ARM_BASE              0xFD200000 //Use 8 bit addressing
#define SET_REG(addr)        (*(volatile unsigned char*)(REG_ARM_BASE + ( ((addr) & ~1) << 1 ) + (addr & 1) ))
#define SET_REG2(addr)        (*(volatile unsigned short*)(REG_ARM_BASE + ( ((addr) & ~1) << 1 ) + (addr & 1) ))



//-------------------------------------------------------------------------------------------------
// Macros
//-------------------------------------------------------------------------------------------------
#if defined(PRJ_X10)
#define FASTLOGO_MODEL "X10"                //PRJ_X10
#define FASTLOGO_VER "0036"
#define MAX_SEC_LOGO_SIZE 1920*1080*4
#define LOGO_MIU0_MEM_START (0x40000000)
#define LOGO_MIU1_MEM_START (0xA0000000)
#define REG_ADDR(addr)              (*((volatile U16*)(0xBF200000 + (addr << 1))))
#else
#define FASTLOGO_MODEL "X10P"                //PRJ_X10P
#define FASTLOGO_VER "0005"
#define MAX_SEC_LOGO_SIZE 1920*1080*4
//#define LOGO_MIU_PHY_ADDR  //GOP2_FB_ADR
//#define LOGO_MIU0_MEM_START ioremap((0x40000000 + LOGO_MIU_PHY_ADDR), MAX_SEC_LOGO_SIZE)
//#define LOGO_MIU1_MEM_START ioremap((0xA0000000 + LOGO_MIU_PHY_ADDR), MAX_SEC_LOGO_SIZE)
#define LOGO_MIU0_MEM_START (0x40000000)
#define LOGO_MIU1_MEM_START (0xA0000000)


#define REG_ADDR(addr)              (*((volatile U16*)(0xFD200000 + (addr << 1))))
#define REG_ADDR2(addr)              (*((volatile U16*)(0xFD220000 + (addr << 1))))
#endif

#define GOP0MIUSEL          0x12F6

#define LOGO_CENTER             -1
#define MSTAR_LOGO_Y_POS_RATIO 0.26
#define Y_POS_ON_FHD        280        // = 1080 * 0.26
#define Y_POS_ON_HD        200        // = 768 * 0.26
#define Y_POS_ON_MFM900     234        // = 900 * 0.26

#define LOWBYTE(w)                ((w) & 0x00ff)
#define HIGHBYTE(w)                (((w) >> 8) & 0x00ff)

// for ini parsing
#define STR_BUF_SIZE 100
//-------------------------------------------------------------------------------------------------
// Local Variables
//-------------------------------------------------------------------------------------------------
static char *logo_file = "/Customer/bootlogo/logo.bin";
static char *use_logo = "/Customer/bootlogo/logo.bin";    // if not used this buf, happened crash, in rmmod time.
static char *logo_ini = "/Customer/bootlogo/logo.ini";
static INFOHEADER tLogoInfo;
static int sec_logo_size = MAX_SEC_LOGO_SIZE;
static int pos_x = 0;
static int pos_y = 0;
static int full_screen = 1;
static int double_size = 0;
static int full_scale = 0;
static int Hmirror = 0;
static int Vmirror = 0;
static int display_time = 2500;
static mm_segment_t oldfs;
static unsigned char *logo_buf = NULL;
static dma_addr_t logo_phy_addr;
static char *FastLogoOff_file = "/mtd_rwarea/FastLogoOff";
static bool logo_threadfinish = false;
static volatile U16 gopMiuSelBackup = 0x200;
//-------------------------------------------------------------------------------------------------
//  External Functions
//-------------------------------------------------------------------------------------------------
//extern void sys_set_pnl_init_status(int status);
//extern unsigned int panel_hstart;

// for kthread
struct task_struct *ts;
static int show_logo_main(void);
static int read_logo_ini(unsigned long *width,unsigned long *height,unsigned long *format);
static void prepare_lvds_settings(void);
void MApi_PNL_En(MS_BOOL bPanelOn );
void _SWI2C_Init(void);
SdResult _SWI2C_IicRead(SdCommon_IicParam_t* pIicParam, unsigned char * regAddr, uint32_t byteCount, uint8_t* pBuffer);
SdResult _SWI2C_IicWrite(SdCommon_IicParam_t* pIicParam, uint8_t* regAddr, uint32_t byteCount, uint8_t* pBuffer);

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
struct file *openFile(char *path,int flag,int mode)
{
    struct file *fp;

    fp=filp_open(path, flag, 0);
    if (fp) return fp;
    else return NULL;
}

static int readFile(struct file *fp,char *buf,int readlen)
{
    if (fp->f_op && fp->f_op->read)
        return fp->f_op->read(fp,buf,readlen, &fp->f_pos);
    else
        return -1;
}

static int closeFile(struct file *fp)
{
    filp_close(fp,NULL);
    return 0;
}

static void initKernelEnv(void)
{
    oldfs = get_fs();
    set_fs(KERNEL_DS);
}

static int read_logo_ini(unsigned long *width,unsigned long *height,unsigned long *format)
{
    static struct file *fp;
    static unsigned char *ini_buf = NULL;
    static unsigned char *tmp_buf = NULL;
    static unsigned char *left_buf = NULL;
    static unsigned char *line = NULL;
    static unsigned char *pos = NULL;
    int ret = -1,i=0;
    int length = 0;
    unsigned char *endl = NULL;
    initKernelEnv();
    printk("Read log.ini: %s\n", logo_ini);
    fp = openFile(logo_ini,O_RDONLY,0);
    if(!IS_ERR(fp))
    {
        printk("Read log.ini fp ok: %s\n", use_logo);
        ini_buf = kmalloc(STR_BUF_SIZE, GFP_KERNEL);
        memset(ini_buf, 0, STR_BUF_SIZE);
        while((ret = readFile(fp, ini_buf, STR_BUF_SIZE))>0)
        {
            if(!tmp_buf)
            {
                tmp_buf = kmalloc(2*STR_BUF_SIZE, GFP_KERNEL);
                memset(tmp_buf, 0, 2*STR_BUF_SIZE);
                memcpy(tmp_buf, ini_buf, ret);
                strcat(tmp_buf,"\0");
            }
            else
            {
                strncat(tmp_buf, ini_buf, ret);
            }
            while((endl = strchr(tmp_buf, '\n')) > 0)
            {
                length = (int)(endl-tmp_buf)+1;
                line = kmalloc(length+1, GFP_KERNEL);
                memset(line,0,length);
                memcpy(line, tmp_buf, length);
                *(line+length) = '\0';
                memcpy(tmp_buf, endl+1, strlen(tmp_buf)-length);
                memset(tmp_buf + strlen(tmp_buf)-length, 0, length);
                if(strstr(line,"m_ucColorFormat") > 0) //get pic color format
                {
                    //printk("--- m_ucColorFormat:%s%d\n\n",line,strlen(line));
                    pos = strchr(line,'=');
                    *format = simple_strtoul(pos+1,0,10);
                    if(*format >= CLRFMAT_MAX || *format < 0)
                    {
                        printk("[logo.ini error] invalid format:%d ...\n",*format);
                        *format = 0;
                    }
                }
                else if(strstr(line,"m_wPicWidth") > 0)//get pic width
                {
                    //printk("--- m_wPicWidth:%s%d\n\n",line,strlen(line));
                    pos = strchr(line,'=');
                    *width = simple_strtoul(pos+1,0,10);

                }
                else if(strstr(line,"m_wPicHeight") > 0)//get pic height
                {
                    //printk("--- m_wPicHeight:%s%d\n\n",line,strlen(line));
                    pos = strchr(line,'=');
                    *height = simple_strtoul(pos+1,0,10);
                }
                else
                {
                    ;//do nothing
                    //printk("--- %s%d\n\n",line,strlen(line));
                }
                kfree(line);
            }
            memset(ini_buf, 0, 100);
        }
        kfree(tmp_buf);
        ret = 1;
    }
    set_fs(oldfs);
    return ret;
}

static int read_logo_file(INFOHEADER *pBmpInfo)
{
    static struct file *fp;
    int ret = -1;
    int pic_size = pBmpInfo->width*pBmpInfo->height*pBmpInfo->bits;
    initKernelEnv();
    printk("Read Logo: %s\n", use_logo);
    fp = openFile(use_logo,O_RDONLY,0);
    if (!IS_ERR(fp))
    {
        printk("Read Logo: %s\n", use_logo);


        if ((ret = readFile(fp,logo_buf,pic_size))>0)
        {
            /* NULL */
            printk("read file succuss %d\n",ret);
        }
        else
        {
            printk("read file error %d\n",ret);
        }
        closeFile(fp);
        if ( ret != pic_size)
            printk("[logo.ini error] miss match image size (format)\n");
    }
    set_fs(oldfs);
    return ret;
}

static int set_gop(unsigned char *buf_addr, INFOHEADER *pBmpInfo, int w_panel, int h_panel)
{
    unsigned int bk10reg;
    unsigned int width, height;
    unsigned int clrfmt;
    unsigned int reglength;
    unsigned int h_ratio, v_ratio;
    unsigned int gop_word;

    gop_word = 32;

    width = pBmpInfo->width;
    height = pBmpInfo->height;
    clrfmt = pBmpInfo->clrfmt;

    REG_ADDR(0x1FFE) =  (REG_ADDR(0x1FFE) & (~0x00FF)); /* switch to bank 0 */

    REG_ADDR(0x2F00) = 0x0000;
    REG_ADDR(0x2F0C) = 0xF000; /* enable SC_OP */

    REG_ADDR(0x1FFE) = 0x200;  /* Switch to GOP0*/

    REG_ADDR(0x1F00) = 0x510C; /* Reset GOP */

    REG_ADDR(0x1F02) = (REG_ADDR(0x1F02) & (~0x07))|0x02; /* Set GOP dst to OP */
#if 0
#if defined(PRJ_X9)
    REG_ADDR(0x1F1E) = panel_hstart + 0x7F; /* Pipeline offset */
#else
    REG_ADDR(0x1F1E) = panel_hstart + 0xAE; /* Pipeline offset */
#endif
#endif
    REG_ADDR(0x1F1C) = 0x01e1;
    REG_ADDR(0x1F1E) = 0x0143;
    REG_ADDR(0x1F60) = width/2; /* Stretch window H */
    REG_ADDR(0x1F62) = height; /* Stretch window V */

    REG_ADDR(0x1F00) = (REG_ADDR(0x1F00)&(~0x3000)); /* Clear HV mirror */
    if (Hmirror)
    {
        REG_ADDR(0x1F00) = REG_ADDR(0x1F00)|0x1000; /* enable H mirror */
    }
    if (Vmirror)
    {
        REG_ADDR(0x1F00) = REG_ADDR(0x1F00)|0x2000; /* enable V mirror */
    }

    if (full_screen) /* Full Screen */
    {
        /* Enlarge */
        h_ratio = (width*4096)/w_panel; /* h-ratio */
        v_ratio = (height*4096)/h_panel; /* V-ratio */
        pos_x = 0;
        pos_y = 0;
    }
    else
    {
        h_ratio = 0x1000;
        v_ratio = 0x1000;

        if (full_scale)
        {
            h_ratio = (width*4096)/w_panel;
            v_ratio = h_ratio;
            pos_x = 0;
        }
        else
        {
            if (double_size)
            {
                if ((w_panel >= (width*2))&&(h_panel >= (height*2)))
                {
                    h_ratio = 0x800;
                    v_ratio = 0x800;
                }
            }
        }
    }
    REG_ADDR(0x1F6A) = h_ratio; /* h-ratio */
    REG_ADDR(0x1F6C) = v_ratio; /* V-ratio */

    /* Position */
    if (pos_x < 0)    //pos_x is center
    {
        pos_x = (w_panel - width)/2;
    }

    if (pos_y < 0)    //pos_y is center
    {
        pos_y = (h_panel - height)/2;
    }
    else
    {
        /* check Vmirror */
        if(Vmirror)
        {
            pos_y = (h_panel - (height*w_panel/width) - pos_y);
        }
    }

    REG_ADDR(0x1F64) = pos_x; /* X */
    REG_ADDR(0x1F68) = pos_y; /* Y */

    REG_ADDR(0x1FFE) =  REG_ADDR(0x1FFE) | 0x0001; /* switch to bank 1 */

//#if defined(PRJ_X10P) || defined(SN)
    /* Buffer address access */
    //buf_addr = (unsigned char *)((unsigned long)LOGO_MIU_PHY_ADDR);
    REG_ADDR(0x1F02) = (((unsigned long)buf_addr)/gop_word)&0xFFFF;
    REG_ADDR(0x1F04) = ((((unsigned long)buf_addr)/gop_word))>>16;

    REG_ADDR(0x1F08) = 0x0000; /* set GWin start */
    REG_ADDR(0x1F0A) = width/8; /* set GWin end */

    REG_ADDR(0x1F0C) = 0x0000; /* set GWin Height */
    REG_ADDR(0x1F10) = height; /* set GWin Height */

    REG_ADDR(0x1F12) = width/8; /* set Gwin Pitch */

    REG_ADDR(0x1F14) = 0x00FF; /* Alpha Value */

    reglength = width*height*(pBmpInfo->bits)/8; /* WxHxBPP */

    //REG_ADDR(0x1F18) = ((reglength/gop_word) - (width/8))&0xFFFF; /* WxHxBPP/16 - W/4 */
    //REG_ADDR(0x1F1A) = (((reglength/gop_word) - (width/8))>>16)&0xFFFF;
    //REG_ADDR(0x1F20) = (reglength/gop_word)&0xFFFF; /* WxHxBPP/16 */
    //REG_ADDR(0x1F22) = ((reglength/gop_word)>>16)&0xFFFF;
    REG_ADDR(0x1F18) = 0x0000;
    REG_ADDR(0x1F1A) = 0x0000;
    REG_ADDR(0x1F20) = 0x0000;
    REG_ADDR(0x1F22) = 0x0000;

    REG_ADDR(0x1F40) = 0x00A0;

    REG_ADDR(0x1FFC) = 0x0000; /* set mux to GOP0 */
    REG_ADDR(0x0B80) = (REG_ADDR(0x0B80) & (~0x00FF))|0x00;  /* Set mixer to ODCLK */

    bk10reg = REG_ADDR(0x2F00);
    REG_ADDR(0x2F00) = 0x10;
    REG_ADDR(0x2F46) = 0xA0;
    REG_ADDR(0x2F00) = bk10reg;

    REG_ADDR(0x1F00) = 0x3F01 + (clrfmt<<4);// & 0x00F0); /* set color format & enable GWIN */
    REG_ADDR(0x1FFE) =  REG_ADDR(0x1FFE) & 0xFFFE; /* switch back to bank 0 */

    /* for Sharp panel flicker issue */
    bk10reg = REG_ADDR(0x2F00);
    REG_ADDR(0x2F00) = 0x19;
    REG_ADDR(0x2F20) = 0x80;
    REG_ADDR(0x2F00) = bk10reg;

    //TODO: only for A3 A5 A5p Agate , GOP0 MIU SEL = 0x12F6 BIT2 3 4 5 -> GOP0 1 2 3

    if (logo_phy_addr>= LOGO_MIU0_MEM_START && logo_phy_addr < LOGO_MIU1_MEM_START)
    {
        REG_ADDR(GOP0MIUSEL) &= ~BIT2; //gop0 for miu0
    }
    else
    {
        REG_ADDR(GOP0MIUSEL) |= BIT2; //gop0 for miu1
    }

    return 0;
}

void mute_logo(void)
{
    REG_ADDR(0x1FFE) =  (REG_ADDR(0x1FFE) & (~0x0FFF))|0x201; /* switch to bank 1 */

    REG_ADDR(0x1F00) = REG_ADDR(0x1F00) & (~0x0001); /* disable GWIN */

    REG_ADDR(0x1FFE) =  (REG_ADDR(0x1FFE) & (~0x0FFF)); /* switch to bank 0 */
}

// logo kthread
int logo_thread(void *data)
{
    show_logo_main();
    prepare_lvds_settings();

    //prepare_lvds_settings();
    printk("REG_ADDR(0x0B22) : %x", REG_ADDR(0x0B22));
    //2010_1115 Workaround at X5FHD Panel(small) white noise
    msleep(150);        //50ms -> 150ms, tunning the time for to remove flicker on led panel

    /* Turn on Inverter */
   // inverter_init();

    //MApi_PNL_En(1);
    //REG_ADDR(0x0B22) = 0x8100; /* STOP CPU */
    printk("REG_ADDR(0x0B22) : %x\n",REG_ADDR(0x0B22));
    printk("draw logo finish...\n");
    logo_threadfinish = true ;

    return 0;
}


static int __init show_logo_init(void)
{
    int waitloop = 0;

    use_logo = logo_file;

    logo_threadfinish = false ;

    // Backup Gop miu sel
    gopMiuSelBackup = REG_ADDR(GOP0MIUSEL);

    // Run kthread
    printk(KERN_INFO " 3 - logo_thread() run\n");
    ts = kthread_run(logo_thread, NULL, "kthread");
    printk("wait - 0  finish\n");

    while(!logo_threadfinish)
    #if 1
    {

        msleep(50);
        //printk("wait - 1\n");

        // avoid dead loop
        if( waitloop++ > 10 )
            break ;
    }
    #endif
    printk("wait - 1  finish\n");

    msleep(display_time);
    // Diable GOP0 GWin
    REG_ADDR(0x1FFE) =  REG_ADDR(0x1FFE) | 0x0001; /* switch to bank 1 */
    REG_ADDR(0x1F00) = REG_ADDR(0x1F00) & (~0x0001); /* disable GWIN */
    REG_ADDR(0x1FFE) =  REG_ADDR(0x1FFE) & 0xFFFE; /* switch to bank 0 */

    // Recover Gop miu sel
    REG_ADDR(GOP0MIUSEL) = gopMiuSelBackup;


    if(logo_buf)
        dma_free_coherent(NULL,tLogoInfo.width*tLogoInfo.height*tLogoInfo.bits,logo_buf,logo_phy_addr);
    printk("wait - 2 finish\n");

    return 0;
}



MS_BOOL g_bI2CInit = FALSE;

#if defined(PRJ_X9)
#define IIC_MAX_CH          9
#else
#define IIC_MAX_CH          10
#endif


#if defined(PRJ_X9)
SWI2C_BusCfg g_SWI2CBusCfg[IIC_MAX_CH]=
{
    //Bus_0 : DDCR_CK(SCL), DDCR_DA(SDA)        EEPROM
    {73,     72,  100},
    //Bus_1 : GPIO144(SCL), GPIO146(SDA)          TUNER
    {145,  147, 100},
    //Bus_2 : GPIO56(SCL), GPIO57(SDA)          TCON,LOGIC
    {57,     58,  100},
    //Bus_3 : GPIO50(SCL), GPIO51(SDA)          AMP
    {51,     52,  100},

};
#else
SWI2C_BusCfg g_SWI2CBusCfg[IIC_MAX_CH]=
{
    //Bus_0 : DDCR_CK(SCL), DDCR_DA(SDA)        ECHO_FS(*)
    {81,   80,  100},
    //Bus_1 : GPIO83(SCL), GPIO84(SDA)          TUNER(*)
    {84,   85,  100},
    //Bus_2 : GPIO63(SCL), GPIO64(SDA)             EEPROM(*)
    {64,   65,  100},
    //Bus_3 : UART2_RX(SCL),UART2_TX(SDA)             AMP
    {57,   56,  100},
    //Bus_4 : DDCA_CK(SCL), DDCA_DA(SDA)             DDC PC
    {32,   33,  100},
    //Bus_5 : DDCDC_CK(SCL), DDCDC_DA(SDA)             DDC HDMI1
    {38,   39,  100},
    //Bus_6 : DDCDA_CK(SCL),DDCDA_DA(SDA)             DDC HDMI2
    {34,   35,  100},
    //Bus_7 : DDCDB_CK(SCL),DDCDB_DA(SDA)             DDC HDMI3
    {36,   37,  100},
    //Bus_8 : DDCDD_CK(SCL),DDCDD_DA(SDA)             DDC HDMI4
    {40,   41,  100},
    //Bus_9: GPIO108(SCL),GPIO109(SDA)             TCON,LOGIC
    {109,   110,  100},
};
#endif

void _SWI2C_Init(void)
{
    MS_U8 u8BusCount;

    //2. Initialize SW I2C Buses
    //(2-1) Compute I2C bus count
    u8BusCount = sizeof(g_SWI2CBusCfg)/sizeof(SWI2C_BusCfg);

    //(2-3) Initialize all SW I2C buses
    MApi_SWI2C_Init(g_SWI2CBusCfg,u8BusCount);

    //3. Initialized successfully
    g_bI2CInit = TRUE;

}


SdResult _SWI2C_IicRead(SdCommon_IicParam_t* pIicParam, unsigned char * regAddr, uint32_t byteCount, uint8_t* pBuffer)
{
    MS_U16 u16BusNumSlaveID = 0;
    MS_U8 u8AddrNum = 0;
    MS_BOOL bResult = 0;

    if(g_bI2CInit==FALSE)
    {
        printk("SD_ERR_UNINIT\n");
        return SD_ERR_UNINIT;
    }

    if(pIicParam->ui8Channel>=IIC_MAX_CH)
    {
        printk("SD_NOT_SUPPORTED\n");
        return SD_NOT_SUPPORTED;
    }

    u16BusNumSlaveID = (MS_U16)((pIicParam->ui8Channel)<<8 | (MS_U8)(pIicParam->ui32SlaveAddress));
    u8AddrNum = (MS_U8)pIicParam->eAddrMode;
    MApi_SWI2C_SetReadMode((SWI2C_ReadMode)pIicParam->eReadMode);
    MApi_SWI2C_Speed_Setting((MS_U8)pIicParam->ui8Channel,(MS_U32)pIicParam->clkFreqInKhz);
    bResult = MApi_SWI2C_ReadBytes( u16BusNumSlaveID, u8AddrNum, (MS_U8*)regAddr, (MS_U32)byteCount, (MS_U8*)pBuffer);

    return (bResult)? SD_OK : SD_NOT_OK;
}


SdResult _SWI2C_IicWrite(SdCommon_IicParam_t* pIicParam, uint8_t* regAddr, uint32_t byteCount, uint8_t* pBuffer)
{
    MS_U16 u16BusNumSlaveID = 0;
    MS_U8 u8AddrNum = 0;
    MS_BOOL bResult = 0;

    if(g_bI2CInit==FALSE)
    {
        printk("SD_ERR_UNINIT\n");
        return SD_ERR_UNINIT;
    }

    if(pIicParam->ui8Channel>=IIC_MAX_CH)
    {
        printk("SD_NOT_SUPPORTED\n");
        return SD_NOT_SUPPORTED;
    }

    u16BusNumSlaveID = (MS_U16)((pIicParam->ui8Channel)<<8 | (MS_U8)(pIicParam->ui32SlaveAddress));
    u8AddrNum = (MS_U8)pIicParam->eAddrMode;
    MApi_SWI2C_Speed_Setting((MS_U8)pIicParam->ui8Channel,(MS_U32)pIicParam->clkFreqInKhz);
    bResult = MApi_SWI2C_WriteBytes( u16BusNumSlaveID, u8AddrNum, (MS_U8*)regAddr, (MS_U32)byteCount, (MS_U8*)pBuffer);

    return (bResult)? SD_OK : SD_NOT_OK;
}

static void prepare_lvds_settings(void)
{
      printk("Panel Init ...");
      //=====> turn on clk
      // MPLL Power on
      REG_ADDR(0x1e38) = 0x00 ;
      // MPLL enable
      REG_ADDR2(0x0c03) = 0x00 ;
      REG_ADDR2(0x0c83) = 0x00 ;
      //enable odclk
      REG_ADDR(0x0ba6) = 0x00 ;
      //==========================================//
      //     set scalar clk as LVDS2ch_150MHz     //
      //==========================================//
      REG_ADDR(0x311e) = 0x54d8;
      REG_ADDR(0x3120) = 0x001a;
      REG_ADDR(0x312a) = 0x0000;
      REG_ADDR(0x3102) = 0x0203;
      REG_ADDR(0x3104) = 0x1700;
      REG_ADDR(0x3106) = 0x000d;
      REG_ADDR(0x3108) = 0x0000;
      REG_ADDR(0x315c) = 0x07b0;
      REG_ADDR(0x3160) = 0x0000;
      REG_ADDR(0x3162) = 0x0000;
      REG_ADDR(0x3164) = 0x0000;
      REG_ADDR(0x3166) = 0x0020;
      REG_ADDR(0x3168) = 0x0000;
      REG_ADDR(0x316a) = 0x1f00;
      REG_ADDR(0x316c) = 0x0000;
      REG_ADDR(0x316e) = 0x0000;
      REG_ADDR(0x3172) = 0x0000;
      REG_ADDR(0x3174) = 0x0000;
      REG_ADDR(0x31c0) = 0x0000;
      //loop gain =   16
      //loop div  =   14
      REG_ADDR(0x310e) = 0x02;
      REG_ADDR(0x3112) = 0x02;
      REG_ADDR(0x3115) = 0xf0;
      REG_ADDR(0x3117) = 0x65;
      //===========================================//
      //     set OSD odclk as HS_LVDS_1ch_150M     //
      //===========================================//
      REG_ADDR(0x3190) = 0x7f44;
      REG_ADDR(0x3192) = 0x0027;
      REG_ADDR(0x3180) = 0x0000;
      REG_ADDR(0x3182) = 0x0302;
      REG_ADDR(0x3186) = 0x004d;
      REG_ADDR(0x3188) = 0x0000;
      REG_ADDR(0x318a) = 0x0000;
      REG_ADDR(0x318c) = 0x0000;
      REG_ADDR(0x3194) = 0x0000;
      REG_ADDR(0x3196) = 0x0000;
      //loop gain =   12
      //loop div  =    7

      //CLKGEN
      REG_ADDR(0x0BA6) = 0x0c00; //[11:8]ckg_bt656[3:0]ckg_odclk
      REG_ADDR(0x0BAE) = 0x0000; //[15:12]ckg_bt656 [3:0]ckg_fifo
      REG_ADDR(0x0BB0) = 0x0000; //[3:0]ckg_tx_mod
      REG_ADDR(0x0BBC) = 0x0000; //[11:8]ckg_tx_mod [3:0]ckg_osd2mod

      //DISP_TGEN green_box
      REG_ADDR(0x2f00) = 0x0010;
      REG_ADDR(0x2f02) = 0x0010;  //HSyncWidth
      REG_ADDR(0x2f08) = 0x007f; //hfde_st
      REG_ADDR(0x2f0A) = 0x07fe; //hfde_end
      REG_ADDR(0x2f0C) = 0x0015; //vfde_st
      REG_ADDR(0x2f0E) = 0x044a; //vfde_end
      REG_ADDR(0x2f10) = 0x007f; //hmde_st
      REG_ADDR(0x2f12) = 0x07fe; //hmde_end
      REG_ADDR(0x2f14) = 0x0015; //vmde_st
      REG_ADDR(0x2f16) = 0x044a; //vmde_end
      REG_ADDR(0x2f18) = 0x0897; //htt
      REG_ADDR(0x2f1A) = 0x0464; //vtt
      REG_ADDR(0x2f32) = 0x0003; //green box
      REG_ADDR(0x2f34) = 0x00ff;

      //MOD
      REG_ADDR(0x326E) = 0x0040; //[6]MOD PVDD power 0: 3.3V 1: 2.5V
      REG_ADDR(0x3294) = 0x0002; //[1]reg_dualmode[0]abswitch
      REG_ADDR(0x32F0) = 0x0000; //[0]pd_mod
      REG_ADDR(0x32EE) = 0x001f; //[4]ck_pd[3]ck_pc[2]ck_pb[1]ck_pa[0]en_ck
      REG_ADDR(0x32DA) = 0x5550; //[15:0]reg_output_conf[15:0]
      REG_ADDR(0x32DC) = 0x0555; //[7:0]reg_output_conf[27:16]
      // 002C for145B; 000C for 129F
      REG_ADDR(0x3280) = 0x002C; // lvds_swap_pol
      REG_ADDR(0x328A) = 0x003F; //[5]la_oen[4]lb_oen
      REG_ADDR(0x3240) = 0x0000; //[15:12]ckg_dot_mini_pre[11:8]ckg_dot_mini
      REG_ADDR(0x3266) = 0x8000; //[15]check enable[14]data_format

}


static int show_logo_main(void)
{
    int size;
    int ret = -1;
    int w_panel = 1920, h_panel = 1080;             /* Panel Width & Height */
    unsigned long w_logo = 0, h_logo = 0;
    COLORFORMAT fmt_logo = 0;
    int lcd_pdp_panel = 0;                         /* LCD/PDP panel */
    unsigned char data_FHD_HD = 0, data_PDP_SD = 0;     /* Panel Resolution */
    SdMisc_DispRes_k displayRes = SD_MISC_DISP_RES_MAX;
    static struct file *fp =  NULL;

#if defined(PRJ_X9) || defined(PRJ_X10) || defined(PRJ_X10P) || defined(SN)
    int jeida_vesa_lvds = 0;                        /* JEIDA/VESA LVDS format */
    //unsigned char inverted_Panel = 0;                    /* Panel Resolution */
#endif

     printk("## " FASTLOGO_MODEL " FASTLOGO ver : %s ##\n", FASTLOGO_VER);
     printk("# FastLogo On \n");

        /* detect Panel Resolution */
#if 0
        SdCommon_GpioRead(GPIO_LCD_PDP, (void *)&lcd_pdp_panel);

        if (lcd_pdp_panel)        //LCD
        {
            printk("        if (lcd_pdp_panel)        //LCD \n\n");

            /*------------------------------------------------------------------- */
            /* |        LCD/LED        |    1920x80    |    1600x900    |    1366x768    | */
            /* |    GPIO_PDP_SD     |            H        |        L        |        H        | */
            /* |    GPIO_FHD_HD     |         L        |        L        |        H        | */
            /*------------------------------------------------------------------- */

            /* Detect PDP_SD, High : no SD, Low : SD */
            SdCommon_GpioRead(GPIO_PDP_SD, (void *)&data_PDP_SD);
            if(data_PDP_SD)
            {
                /* Detect FHD_HD , High : HD, Low : FHD */
                SdCommon_GpioRead(GPIO_FHD_HD, (void *)&data_FHD_HD);

                if(data_FHD_HD)        //HD
                {
#endif
                    w_panel = 1920;
                    h_panel = 1080;
                    /* Init Panel Resolution.. */
                    printk("SD_MISC_DISP_RES_FHD_LCD\n");
                    displayRes = SD_MISC_DISP_RES_FHD_LED;
#if 0
                }
                else                    //FHD
                {
                    w_panel = 1920;
                    h_panel = 1080;
                    /* Init Panel Resolution.. */
                    printk("SD_MISC_DISP_RES_FHD_LCD\n");
                    displayRes =  SD_MISC_DISP_RES_FHD_LCD;
                    use_logo = "/Customer/bootlogo/_p400x400_ABGR8888bmp.bin";
                }
            }
            else
            {
                /* Detect FHD_HD , High : HD, Low : FHD */
                SdCommon_GpioRead(GPIO_FHD_HD, (void *)&data_FHD_HD);
                if(data_FHD_HD)
                {
                    /* NULL - There is no LCD/LED panel */
                    printk(" There is no  LCD/LED panel \n");
                }
                else                    //MFM 1600x900
                {
                    w_panel = 1600;
                    h_panel = 900;
                    /* Init Panel Resolution.. */
                    printk("SD_MISC_DISP_RES_WSXGA_LCD_MFM\n");
                    displayRes = SD_MISC_DISP_RES_WSXGA_LCD_MFM;
                    //use_logo = "/mtd_exe/FastLogo/TV_1600.bmp";
                    use_logo = "/mtd_exe/FastLogo/smart_TV_1920.bmp";
                }
            }
        }

        else                //PDP
        {
            /*------------------------------------------------------------------- */
            /* |        PDP            |    1920x80    |    1366x768    |    1024x768    | */
            /* |    GPIO_PDP_SD     |            H        |        H        |        L        | */
            /* |    GPIO_FHD_HD     |         L        |        H        |        H        | */
            /*------------------------------------------------------------------- */

            /* Detect PDP_SD, High : no SD, Low : SD */
            SdCommon_GpioRead(GPIO_PDP_SD, (void *)&data_PDP_SD);
            if(data_PDP_SD)
            {
                /* Detect FHD_HD , High : HD, Low : FHD */
                SdCommon_GpioRead(GPIO_FHD_HD, (void *)&data_FHD_HD);
                if(data_FHD_HD)        //HD
                {
                    w_panel = 1366;
                    h_panel = 768;
                    /* Init Panel Resolution.. */
                    printk("SD_MISC_DISP_RES_HD_PDP\n");
                    displayRes = SD_MISC_DISP_RES_HD_PDP;
                    use_logo = "/mtd_exe/FastLogo/smart_TV_1920.bmp";
                }
                else                    //FHD
                {
                    w_panel = 1920;
                    h_panel = 1080;
                    /* Init Panel Resolution.. */
                    printk("SD_MISC_DISP_RES_FHD_PDP\n");
                    displayRes = SD_MISC_DISP_RES_FHD_PDP;
                    use_logo = "/mtd_exe/FastLogo/smart_TV_1920.bmp";
                }
            }
            else
            {
                /* Detect FHD_HD , High : HD, Low : FHD */
                SdCommon_GpioRead(GPIO_FHD_HD, (void *)&data_FHD_HD);
                if(data_FHD_HD)
                {
                    w_panel = 1024;
                    h_panel = 768;
                    /* Init Panel Resolution.. */
                    printk("SD_MISC_DISP_RES_SD_PDP\n");
                    displayRes = SD_MISC_DISP_RES_SD_PDP;
                    use_logo = "/mtd_exe/FastLogo/smart_TV_1024.bmp";
                }
                else
                {
                    /* NULL - There is no PDP panel */
                    printk(" There is no PDP panel \n");
                }
            }

        }
#endif

        SdMisc_DisplayResolution(displayRes);
        SdDisplay_Init();
        SdMisc_LvdsOutput(SD_MISC_LVDS_OUT_CLK_DATA);

        /* detect LVDS format */
        SdCommon_GpioRead(GPIO_LCD_PDP, (void *)&lcd_pdp_panel);
        if (lcd_pdp_panel)        //LCD
        {
            //Vmirror=0;
            //Hmirror=1;

            SdCommon_GpioRead(GPIO_JEIDA_VESA, (void *)&jeida_vesa_lvds);
            if(jeida_vesa_lvds)        //VESA
            {
                printk("SD_MISC_LVDS_FMT_VESA_8\n");
                SdMisc_LvdsFormat(SD_MISC_LVDS_FMT_VESA_8);
            }
            else                    //JEIDA
            {
                printk("SD_MISC_LVDS_FMT_JEIDA_8\n");
                SdMisc_LvdsFormat(SD_MISC_LVDS_FMT_JEIDA_8);
            }

            //  turn on Dimming control from onboot
            //  Set BackLight Strength
            //SdMisc_PWMOnOff(SD_ON);
            //SdMisc_SetDimming(0x9300,0x6E77,SD_OFF);    //X5, X6 Common

        }

//         else                //PDP
        {
            /* temp code for mirror - PDP panel is not inverted */
  //          Vmirror=1;
    //        Hmirror=0;
            //printk("PDP is fixed,  Vmirror : %d, Hmirror : %d\n", Vmirror, Hmirror);
            //printk("SD_MISC_LVDS_FMT_PDP_10\n");
      //      SdMisc_LvdsFormat(SD_MISC_LVDS_FMT_PDP_10);
        }

        /* for new logo */
        full_scale = 0;
        /* Logo position is Center */
        pos_x = LOGO_CENTER;
        pos_y = LOGO_CENTER;

        /* Pasing logo pic info file */
        ret = read_logo_ini(&w_logo,&h_logo,&fmt_logo);
        printk("-- logo picture info -- \nwidth: %lu, height: %lu, fomat: %d\n",w_logo,h_logo,fmt_logo);
        tLogoInfo.width = w_logo;
        tLogoInfo.height = h_logo;
        tLogoInfo.clrfmt = fmt_logo;
        switch (fmt_logo)
        {
            case _8BIT_PALETTE:
                tLogoInfo.bits = 1;
                break;
            case RGB1555_BLINK:
            case RGB565:
            case _2266:
            case ARGB1555:
            case RGB1555_UV7Y8:
            case UV8Y8:
            case RGBA5551:
            case RGBA4444:
                tLogoInfo.bits = 2;
                break;
            case ARGB4444:
            case ARGB8888:
            case ABGR8888:
                tLogoInfo.bits = 4;
                break;
            default:
                tLogoInfo.bits = 2;
        }

        /* Allcate memory for logo pic */
        logo_buf=dma_alloc_coherent(NULL, tLogoInfo.width*tLogoInfo.height*tLogoInfo.bits, &logo_phy_addr, GFP_KERNEL);
        printk(" ============================%x  %x=============================", (int)logo_buf, (int)logo_phy_addr);

        /* Open logo file to logo_buf */
        size = read_logo_file(&tLogoInfo);
        sec_logo_size = size;
        if (size <= 0)
        {
            return -1;
        }
        tLogoInfo.bits = 4,
        /* set GOP */

        ret = set_gop((unsigned char *)logo_phy_addr, &tLogoInfo, w_panel, h_panel);
#if defined(PRJ_X9)
        //fixed X9 error - No OSD issus, if turn off FastLogo.
        REG_ADDR(0x2F00) = 0x0000;
        REG_ADDR(0x2F0C) = 0xF000; /* enable SC_OP */
#endif

    // tell the kernel, the panel is inited.
    //sys_set_pnl_init_status(1);
    return ret;
}


static void __exit show_logo_exit(void)
{
    // memset(logo_buf,0,sec_logo_size);
    mute_logo();
    // REG_ADDR(0x2F0C) = 0xF000; /* Disable SC_OP */
    // REG_ADDR(0x12F2) = value12f2;
    // kfree(logo_buf);
//#if defined(PRJ_X10P) || defined(SN)
//        iounmap(logo_buf);
//#endif
    printk("Exit Show Logo Successfully\n");
    //REG_ADDR(0x0B22) = 0x8100; /* STOP CPU */
    // printk("logo_buf %p\n", logo_buf);
}
module_init(show_logo_init);
module_exit(show_logo_exit);

module_param(logo_file, charp, S_IRUGO);
module_param(pos_x, int, S_IRUGO|S_IWUSR);
module_param(pos_y, int, S_IRUGO|S_IWUSR);
module_param(full_screen, int, S_IRUGO|S_IWUSR);
module_param(double_size, int, S_IRUGO|S_IWUSR);
module_param(Hmirror, int, S_IRUGO|S_IWUSR);
module_param(Vmirror, int, S_IRUGO|S_IWUSR);
module_param(display_time, int, S_IRUGO|S_IWUSR);


MODULE_DESCRIPTION("MStar STATIC LOGO");
MODULE_AUTHOR("mstarsemi.com");
MODULE_LICENSE("GPL");

