#ifndef __UNFD_CONFIG_H__
#define __UNFD_CONFIG_H__

//=====================================================
// select a HW platform:
//   - 1: enable, 0: disable.
//   - only one platform can be 1, others have to be 0.
//=====================================================
#define NAND_DRV_TV_LINUX     1
#define NAND_DRV_G2_LINUX     0

//=====================================================
// do NOT edit the following content.
//=====================================================
#if defined(NAND_DRV_TV_LINUX) && NAND_DRV_TV_LINUX
  #if defined(CONFIG_MSTAR_TITANIA4)
  #include "drvNAND_t4_linux.h"
  #elif defined(CONFIG_MSTAR_JANUS)
  #include "drvNAND_j1_linux.h"
  #elif defined(CONFIG_MSTAR_URANUS4)
  #include "drvNAND_u4_linux.h"
  #elif defined(CONFIG_MSTAR_TITANIA8)
  #include "drvNAND_t8_linux.h"
  #elif defined(CONFIG_MSTAR_TITANIA9)
  #include "drvNAND_t9_linux.h"
  #elif defined(CONFIG_MSTAR_TITANIA12)
  #include "drvNAND_t12_linux.h"
  #elif defined(CONFIG_MSTAR_TITANIA13)
  #include "drvNAND_t13_linux.h"
  #elif defined(CONFIG_MSTAR_JANUS2)
  #include "drvNAND_j2_linux.h"
  #elif defined(CONFIG_MSTAR_AMBER1)
  #include "drvNAND_a1_linux.h"
  #elif defined(CONFIG_MSTAR_AMBER5)
  #include "drvNAND_a5_linux.h"
  #elif defined(CONFIG_MSTAR_AMBER3)
  #include "drvNAND_a3_linux.h"
  #elif defined(CONFIG_MSTAR_AGATE)
  #include "drvNAND_agate_linux.h"
  #elif defined(CONFIG_MSTAR_EAGLE)
  #include "drvNAND_eagle_linux.h"
  #elif defined(CONFIG_MSTAR_KRONUS)
  #include "drvNAND_k1_linux.h"
  #elif defined(CONFIG_MSTAR_AMBER7)
  #include "drvNAND_a7_linux.h"
  #elif defined(CONFIG_MSTAR_AMETHYST)
  #include "drvNAND_amethyst_linux.h"
  #elif defined(CONFIG_MSTAR_KAISERIN)
  #include "drvNAND_k2_linux.h"
  #elif defined(CONFIG_MSTAR_EMERALD)
  #include "drvNAND_emerald_linux.h"
  #elif defined(CONFIG_MSTAR_EDISON)
  #include "drvNAND_edison_linux.h"
  #elif defined(CONFIG_MSTAR_EIFFEL)
  #include "drvNAND_eiffel_linux.h"
  #elif defined(CONFIG_MSTAR_EINSTEIN)
  #include "drvNAND_einstein_linux.h"
  #endif
#else
  #error "Error! no platform selected."
#endif

#if NAND_DRV_TV_LINUX || NAND_DRV_G2_LINUX
#define NAND_DRV_FOR_LINUX           1
#endif

//do NOT edit the following content, for HAL use.

//=====================================================
// misc. do NOT edit the following content.
//=====================================================
#define NAND_DMA_RACING_PATCH        1
#define NAND_DMA_PATCH_WAIT_TIME     10000 // us -> 10ms
#define NAND_DMA_RACING_PATTERN0     (((U32)'M'<<24)|((U32)0<<16)|((U32)'S'<<8)|(U32)1)
#define NAND_DMA_RACING_PATTERN1     (((U32)'T'<<24)|((U32)6<<16)|((U32)'A'<<8)|(U32)8)

//===========================================================
// Time Dalay, do NOT edit the following content, for NC_WaitComplete use.
//===========================================================
#define DELAY_100us_in_us  100
#define DELAY_500us_in_us  500
#define DELAY_1ms_in_us    1000
#define DELAY_10ms_in_us   10000
#define DELAY_100ms_in_us  100000
#define DELAY_500ms_in_us  500000
#define DELAY_1s_in_us     1000000

#define WAIT_ERASE_TIME    (DELAY_1s_in_us<<2)
#define WAIT_WRITE_TIME    (DELAY_1s_in_us<<2)
#define WAIT_READ_TIME     (DELAY_1ms_in_us<<10)	//DELAY_500us_in_us
#define WAIT_PROBE_TIME    (DELAY_100us_in_us<<2)
#define WAIT_RESET_TIME    (DELAY_10ms_in_us)

extern void nand_reset_WatchDog(void);
extern U32 nand_clock_setting(U32 u32ClkParam);
extern U32 nand_config_clock(U16 u16_SeqAccessTime);
extern U32 nand_translate_DMA_address_Ex(U32 u32_DMAAddr, U32 u32_ByteCnt, int mode);
extern void nand_Invalidate_data_cache_buffer(U32 u32_addr, S32 s32_size);
extern U32 nand_WaitCompleteIntr(U16 u16_WaitEvent, U32 u32_MicroSec);
extern void nand_enable_intr_mode(void);
extern void nand_flush_miu_pipe(void);

extern void drvNAND_CHECK_FLASH_TYPE(void);

#define NAND_PARTITAION_BYTE_CNT    0x200
#endif /* __UNFD_CONFIG_H__ */
