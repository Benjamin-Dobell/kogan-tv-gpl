/**
 * @file SdMisc.h
 * @brief Platform Specific API header file
 */
#ifndef __SDMISC_H__
#define __SDMISC_H__

typedef enum
{
    SD_OK                         = 0
    ,SD_NOT_OK                    = -1
    ,SD_NOT_SUPPORTED             = -2
    ,SD_PARAMETER_ERROR            = -3
    ,SD_DONT_CARE                = -4
    ,SD_ERR_SYS                    = -5        ///< operating system call failure
    ,SD_ERR_INV                    = -6        ///< invalid argument passed
    ,SD_ERR_TIMEDOUT            = -7        ///< timeout occured
    ,SD_ERR_DEFERRED            = -8        ///< action has been deferred
    ,SD_ERR_UNIMPLEM            = -9        ///< Unimplemented service
    ,SD_ERR_LOCKED                = -10        ///< object locked
    ,SD_ERR_DELETED                = -11        ///< Object has been deleted
    ,SD_ERR_HEAP                = -12        ///< Heap full
    ,SD_ERR_UNINIT                = -13        ///< module is not initialised
    ,SD_ERR_UNIQUE                = -14        ///< unique id needed
    ,SD_ERR_STATE                = -15        ///< invalid state to invoke function
    ,SD_ERR_DEV                    = -16        ///< no valid device exists
    ,SD_ERR_NOT_FOUND            = -17        ///< Not found
    ,SD_ERR_EOL                    = -18        ///< a list traversal has ended
    ,SD_ERR_TERM                = -19        ///< module has terminated
    ,SD_ERR_LIMIT                = -20        ///< System limit was reached.
    ,SD_ERR_RESOURCE            = -21        ///< insufficient resources for request
    ,SD_ERR_NOT_AVAIL            = -22        ///< resources not available
    ,SD_ERR_BAD_DATA            = -23        ///< Bad data in transport stream
    ,SD_ERR_INLINE_SUCCESS        = -24
    ,SD_ERR_DEFERRED_SUCCESS    = -25
    ,SD_ERR_IN_USE                = -26        ///< Object is in use.
    ,SD_ERR_INTERMEDIATE        = -27        ///< Intermediate return - successful but not complete
    ,SD_ERR_OVERRUN                = -28        ///< Buffer overrun occured (e.g. in filters).
    ,SD_ERR_ABORT                = -29        ///< Request not supported.
    ,SD_ERR_NOTIMPLEMENTED        = -30        ///< Request not implemented.
    ,SD_ERR_INVALID_HANDLE        = -31
    ,SD_ERR_NOMEM                = -32        ///< No Memory
    ,SD_ERR_USER                 = -99        ///< start of subsystem specific errors
} SdResult; ///< SDAL Error code

typedef enum
{
    SD_OFF,
    SD_ON
} SdMisc_OnOff_k;

typedef enum
{
    SD_MISC_DISP_RES_FHD_LED,    ///< 1080p
    SD_MISC_DISP_RES_HD_LED,    ///< 1366x768
    SD_MISC_DISP_RES_SD_LED,    ///< 1024x768
    SD_MISC_DISP_RES_FHD_LCD,    ///< 1080p
    SD_MISC_DISP_RES_HD_LCD,    ///< 1366x768
    SD_MISC_DISP_RES_SD_LCD,    ///< 1024x768
    SD_MISC_DISP_RES_FHD_PDP,    ///< 1080p
    SD_MISC_DISP_RES_HD_PDP,    ///< 1366x768
    SD_MISC_DISP_RES_SD_PDP,    ///< 1024x768
    SD_MISC_DISP_RES_VESA_HD,    ///< 1360x768
    SD_MISC_DISP_RES_CEA_FHD,     ///< 1080p
    SD_MISC_DISP_RES_FHD_LED_MFM,     ///< 1080p
    SD_MISC_DISP_RES_HD_LED_MFM,     ///< 1366x768
    SD_MISC_DISP_RES_WSXGA_LED_MFM, ///< 1600x900
    SD_MISC_DISP_RES_FHD_LCD_MFM,     ///< 1080p
    SD_MISC_DISP_RES_HD_LCD_MFM,     ///< 1366x768
    SD_MISC_DISP_RES_WSXGA_LCD_MFM, ///< 1600x900
    SD_MISC_DISP_RES_FHD_R, ///< 1080p_Reading
    SD_MISC_DISP_RES_MAX
}SdMisc_DispRes_k;

typedef enum
{
    SD_MISC_LVDS_FMT_VESA_8,
    SD_MISC_LVDS_FMT_JEIDA_8,
    SD_MISC_LVDS_FMT_JEIDA_10,
    SD_MISC_LVDS_FMT_PDP_8,
    SD_MISC_LVDS_FMT_PDP_10,
    SD_MISC_LVDS_FMT_NS_10,
    SD_MISC_LVDS_FMT_MAX
} SdMisc_LvdsFmt_k;

typedef enum
{
    SD_MISC_LVDS_OUT_NO_OUTPUT, ///< default state
    SD_MISC_LVDS_OUT_CLK_ONLY,
    SD_MISC_LVDS_OUT_CLK_DATA,
    SD_MISC_LVDS_OUT_MAX
} SdMisc_LvdsOutput_k;


/// data type unsigned char, data length 1 byte
typedef unsigned char               MS_U8;                              // 1 byte
/// data type unsigned short, data length 2 byte
typedef unsigned short              MS_U16;                             // 2 bytes
/// data type unsigned int, data length 4 byte
typedef unsigned int                MS_U32;                             // 4 bytes
/// data type signed char, data length 1 byte
typedef signed char                 MS_S8;                              // 1 byte
/// data type signed short, data length 2 byte
typedef signed short                MS_S16;                             // 2 bytes
/// data type signed int, data length 4 byte
typedef signed int                  MS_S32;                             // 4 bytes

/// data type unsigned int, data length 8 byte
typedef unsigned long long          MS_U64;                             // 8 bytes

#define TRACE(x)	printk("point %d => F:%s, L:%d\n",(int)x,__FUNCTION__,__LINE__)

#if defined(PRJ_X5)  || defined(PRJ_X9)
#if defined(PRJ_X5_PM51)    //PRJ_X5_PM51
#define GPIO_FHD_HD         74
#define GPIO_LCD_PDP        75
#define GPIO_PDP_SD         77
#define GPIO_JEIDA_VESA 76
#define GPIO_MIRROR_DET 8

#else    //PRJ_X5 and PRJ_X5_VOD and PRJ_X9
#define GPIO_FHD_HD     15
#define GPIO_LCD_PDP    16
#define GPIO_PDP_SD     19
#define GPIO_JEIDA_VESA 18
#define GPIO_MIRROR_DET 8

#endif    //#ifdef PRJ_X5

#else    //PRJ_X10
#define GPIO_FHD_HD     19 // MODE_SEL0
#define GPIO_LCD_PDP    20 // MODE_SEL1
#define GPIO_JEIDA_VESA    21 // MODE_SEL2
#define GPIO_PDP_SD     15 // MODE_SEL3
#define GPIO_60HZ_120HZ_SELECT 46
#define GPIO_N240_NRESET    149 //Parma(external 3D chip) reset write - nod decided
#define GPIO_MIRROR_DET 72
#endif    //#if defined(PRJ_X5)


#ifdef PRJ_X5_PM51
#define MBX_REGISTER_1  0x0E6C
#define MBX_REGISTER_2  0x0E6D
#define MBX_REGISTER_3  0x0E70
#define MBX_REGISTER_4  0x0E71
#define MBX_REGISTER_5  0x0E72
#define MBX_REGISTER_6  0x0E73

#define REG_CHIP_BASE    0xBF000000 //Use 8 bit addressing

#define MBX_CTRL1         0x0E5E

#define MBX_HEADER_COUNT 2
#define MBX_REGISTER_COUNT 6
#endif    //#ifdef  PRJ_X5_PM51


SdResult SdCommon_GpioRead(MS_U32 pinIndex, MS_U32 *pOnOff);
SdResult SdMisc_DisplayResolution(SdMisc_DispRes_k resDisp);
SdResult SdDisplay_Init(void);
SdResult SdMisc_LvdsOutput(SdMisc_LvdsOutput_k outputLVDS);
SdResult SdMisc_PWMOnOff(SdMisc_OnOff_k eMode);
SdResult SdMisc_SetDimming(MS_U32 dwFreq,MS_U32 dwDuty,SdMisc_OnOff_k eMode);
SdResult SdMisc_LvdsFormat(SdMisc_LvdsFmt_k fmtLVDS);

#ifdef PRJ_X5_PM51
void SdMisc_GetMailBoxHeader(unsigned short *pusMBHeader);
void SdMisc_SetMailBoxHeader(unsigned short usMBHeader);
void SdMisc_GetMailBoxData(unsigned char *pbMBData, unsigned char ucSize);
void SdMisc_SetMailBoxData(const unsigned char *pbMBData, unsigned char ucSize);
#endif    //#ifdef  PRJ_X5_PM51


#define R1BYTEMSK( u32Reg, u16mask)\
            ( { RIU_READ_BYTE(((u32Reg) << 1) - ((u32Reg) & 1)) & u16mask ; } )

#define W1BYTEMSK( u32Addr, u8Val, u8mask)     \
            ( { RIU_WRITE_BYTE( (((u32Addr) <<1) - ((u32Addr) & 1)), (RIU_READ_BYTE((((u32Addr) <<1) - ((u32Addr) & 1))) & ~(u8mask)) | ((u8Val) & (u8mask))); })


#define R2BYTEMSK( u32Reg, u16mask)\
            ( { RIU_READ_2BYTE( (u32Reg)<< 1) & u16mask ; } )

#define W2BYTEMSK( u32Reg, u16Val, u16Mask)\
            ( { RIU_WRITE_2BYTE( (u32Reg)<< 1 , (RIU_READ_2BYTE((u32Reg) << 1) & ~(u16Mask)) | ((u16Val) & (u16Mask)) ) ; } )

#endif // __SDMISC_H__

