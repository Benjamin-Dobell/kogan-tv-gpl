#ifndef _CHIP_SETUP_H_
#define _CHIP_SETUP_H_

#define MIPS_MIU0_BASE              (0xA0000000)
#define MIPS_MIU1_BASE              (0xC0000000)
#define MIU1_BUS_BASE               (0x60000000)
#define HIGH_MEM_BUS_BASE           (0x50000000)
#define HIGH_MEM_PHY_BASE           (0x10000000)
#define HIGH_MEM_MAX_BUS_ADDR       (0x80000000)

void* Chip_mphy_cachevirt( unsigned long mphyaddr );
void* Chip_mphy_noncachevirt( unsigned long mphyaddr );
unsigned long Chip_mphy_bus( unsigned long phyaddress );
unsigned long Chip_bus_mphy( unsigned long busaddress );

extern void Chip_Flush_Memory(void);
extern void Chip_Read_Memory(void) ;
extern void Chip_Flush_Memory_Range(unsigned long pAddress , unsigned long  size);
extern void Chip_Read_Memory_Range(unsigned long pAddress , unsigned long  size);
extern unsigned int Chip_Query_MIPS_CLK(void);
extern unsigned int Chip_Query_Rev(void);
extern void Chip_Query_L2_Config(void);
extern void Chip_L2_cache_wback_inv( unsigned long addr, unsigned long size);
#endif
