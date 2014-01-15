#ifndef __UBOOT_MMAP_H__
#define __UBOOT_MMAP_H__

#include <config.h>

#if defined(__ARM__)
#define CACHE_ADDR 0x40000000
#define NONCACHE_ADDR 0x60000000
#define MIU_INTERVAL  0x60000000

#else
#define CACHE_ADDR 0x80000000
#define NONCACHE_ADDR 0xA0000000
#define MIU_INTERVAL  0x20000000

#endif

#define UPGRADE_ADDR_STR        "UPGRADE_MEM_ADDR"
#define BOOT_MODE_TMP_BUF_ADDR  0x50000000
#define UPGRADE_BUFFER_ADDR     (CACHE_ADDR|0x200000)//(CACHE_ADDR|0x00000000)


/*************************************
***  Secure booting memory setting ***
**************************************/
//#define SHA256_OUTPUT_BUFFER                              0x200000 //AESDMA sha256 output buffer address
#define UPGRADE_SECUREINFO_BUFFER_ADDR        (CACHE_ADDR|0x500000)
#define AES_DECRYPTED_ADDR                    (CACHE_ADDR|0x3000000)
#define AES_DECRYPTED_LEN                      0x500000 // Len can't smallier than SEGMENT_SIZE
#define RSA_AUTHENTICATION_ADDR               (CACHE_ADDR|0x3500000)
#define RSA_AUTHENTICATION_LEN                 0x500000// Len can't smallier than SEGMENT_SIZE
#define FILESYSTEM_AUTHENTICATION_BUFFER_ADDR (CACHE_ADDR|0x4000000)
#define SECURITY_COMMON_BUFFER_ADDR (CACHE_ADDR|0xA00000)//0x80A00000


/*************************************
*****   panel memory setting   *******
**************************************/
#if defined(CONFIG_A3_STB)
#define VE_BUFFER_ADDR          0x15EC0000//0x06C00000
#else
#define VE_BUFFER_ADDR          0x06C00000
#endif

#define CANVAS_BUFFER_ADDR      0x06E00000
#define DMX_SEC_BUFFER_ADR      0x073FC000


/*************************************
*****   audio memory setting   *******
**************************************/
#if (CONFIG_AUDIO_CUSTOM_CONFIG)
#define AUDIO_R2_MEM_INFO_ADR       CONFIG_AUDIO_R2_MEM_INFO_ADR
#define AUDIO_S_MEM_INFO_ADR        CONFIG_AUDIO_S_MEM_INFO_ADR
#define AUDIO_D_MEM_INFO_ADR        CONFIG_AUDIO_D_MEM_INFO_ADR
#else
#define AUDIO_R2_MEM_INFO_ADR       0x000A200000 //E_MMAP_ID_MAD_R2_ADR
#define AUDIO_S_MEM_INFO_ADR        0x000A340000 //E_MMAP_ID_MAD_SE_ADR
#define AUDIO_D_MEM_INFO_ADR        0x000A440000 //E_MMAP_ID_MAD_DEC_ADR
#endif
#define AUDIO_ES1_ADR               AUDIO_D_MEM_INFO_ADR


/*************************************
*****   logo memory setting   *******
**************************************/
#if (CONFIG_LOGO_CUSTOM_CONFIG)
#define JPD_READBUFF_ADR     CONFIG_JPD_READBUFF_ADR
#define JPD_READBUFF_LEN     CONFIG_JPD_READBUFF_LEN
#define JPD_INTERBUFF_ADR    CONFIG_JPD_INTERBUFF_ADR
#define JPD_INTERBUFF_LEN    CONFIG_JPD_INTERBUFF_LEN
#define JPD_OUT_ADR          CONFIG_JPD_OUT_ADR
#define JPD_OUT_LEN          CONFIG_JPD_OUT_LEN
#define LOGO_JPG_DRAM_ADDR   CONFIG_LOGO_JPG_DRAM_ADDR
#define GOP_BUFFER_ADDR      CONFIG_GOP_BUFFER_ADDR
#else
#define JPD_READBUFF_ADR        0x0E9C2000 		//E_DFB_JPD_READ_ADR
#define JPD_READBUFF_LEN        0x00100000 		//E_DFB_JPD_READ_LEN
#define JPD_INTERBUFF_ADR       0x0EAC2000 		//E_DFB_JPD_INTERNAL_ADR
#define JPD_INTERBUFF_LEN       0x00100000 		//E_DFB_JPD_INTERNAL_LEN
#define JPD_OUT_ADR             0x0EBC2000 		//E_DFB_JPD_WRITE_ADR
#define JPD_OUT_LEN             0x009CA000 		//E_DFB_JPD_WRITE_LEN

#define LOGO_JPG_DRAM_ADDR      0x05B2C000
    #if(ENABLE_EAGLE==1 || ENABLE_EDISON==1 || ENABLE_EIFFEL==1 || ENABLE_NIKE==1 || ENABLE_EINSTEIN==1)
    #define GOP_BUFFER_ADDR         0x0E600000	//E_MMAP_ID_GOP_FB_ADR
    #else
    #define GOP_BUFFER_ADDR         (0x0E600000|MIU_INTERVAL)
    #endif
#endif

#endif //__UBOOT_MMAP_H__
