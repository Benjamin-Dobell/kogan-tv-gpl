#ifndef _SLEEP_HELPER_
#define _SLEEP_HELPER_
#include "sleep_common.h"

#define TRUE  1
#define FALSE 0

#define A9_SMP_BIT     (1<<6)
#define A15_SMP_BIT    (1<<6)
#define MPIDR_U_BIT    (1<<30)

typedef unsigned int appf_u32;
typedef signed int appf_i32;

#define A9_GLOBAL_TIMER_OFFSET      0x200
#define A9_GT_TIMER_ENABLE          (1<<0)
#define A9_GT_COMPARE_ENABLE        (1<<1)
#define A9_GT_AUTO_INCREMENT_ENABLE (1<<3)
#define A9_GT_EVENT_FLAG            (1<<0)

typedef struct
{
    /* 0x00 */ volatile unsigned timer_load;
    /* 0x04 */ volatile unsigned timer_counter;
    /* 0x08 */ volatile unsigned timer_control;
    /* 0x0c */ volatile unsigned timer_interrupt_status;
                char padding1[0x10];
    /* 0x20 */ volatile unsigned watchdog_load;
    /* 0x24 */ volatile unsigned watchdog_counter;
    /* 0x28 */ volatile unsigned watchdog_control;
    /* 0x2c */ volatile unsigned watchdog_interrupt_status;
    /* 0x30 */ volatile unsigned watchdog_reset_status;
    /* 0x34 */ volatile unsigned watchdog_disable;
} a9_timer_registers;

typedef struct
{
    unsigned timer_load;
    unsigned timer_counter;
    unsigned timer_control;
    unsigned timer_interrupt_status;
    unsigned watchdog_load;
    unsigned watchdog_counter;
    unsigned watchdog_control;
    unsigned watchdog_interrupt_status;
} a9_timer_context;

typedef struct
{
    /* 0x00 */ volatile unsigned counter_lo;
    /* 0x04 */ volatile unsigned counter_hi;
    /* 0x08 */ volatile unsigned control;
    /* 0x0c */ volatile unsigned status;
    /* 0x10 */ volatile unsigned comparator_lo;
    /* 0x14 */ volatile unsigned comparator_hi;
    /* 0x18 */ volatile unsigned auto_increment;
} a9_global_timer_registers;

typedef struct
{
    unsigned counter_lo;
    unsigned counter_hi;
    unsigned control;
    unsigned status;
    unsigned comparator_lo;
    unsigned comparator_hi;
    unsigned auto_increment;
} a9_global_timer_context;

typedef struct
{
    /* 0x00 */  volatile unsigned int control;
    /* 0x04 */  volatile unsigned int priority_mask;
    /* 0x08 */  volatile unsigned int binary_point;
    /* 0x0c */  volatile unsigned const int interrupt_ack;
    /* 0x10 */  volatile unsigned int end_of_interrupt;
    /* 0x14 */  volatile unsigned const int running_priority;
    /* 0x18 */  volatile unsigned const int highest_pending;
    /* 0x1c */  volatile unsigned int aliased_binary_point;
} cpu_interface;

/* This macro sets either the NS or S enable bit in the GIC distributor control register */
#define GIC_DIST_ENABLE      0x00000001

struct set_and_clear_regs
{
    volatile unsigned int set[32], clear[32];
};

typedef struct
{
    /* 0x000 */  volatile unsigned int control;
                 const unsigned int controller_type;
                 const unsigned int implementer;
                 const char padding1[116];
    /* 0x080 */  volatile unsigned int security[32];
    /* 0x100 */  struct set_and_clear_regs enable;
    /* 0x200 */  struct set_and_clear_regs pending;
    /* 0x300 */  volatile const unsigned int active[32];
                 const char padding2[128];
    /* 0x400 */  volatile unsigned int priority[256];
    /* 0x800 */  volatile unsigned int target[256];
    /* 0xC00 */  volatile unsigned int configuration[64];
    /* 0xD00 */  const char padding3[512];
    /* 0xF00 */  volatile unsigned int software_interrupt;
                 const char padding4[220];
    /* 0xFE0 */  unsigned const int peripheral_id[4];
    /* 0xFF0 */  unsigned const int primecell_id[4];
} interrupt_distributor;

#define DIDR_VERSION_SHIFT 16
#define DIDR_VERSION_MASK  0xF
#define DIDR_VERSION_7_1   5
#define DIDR_BP_SHIFT      24
#define DIDR_BP_MASK       0xF
#define DIDR_WP_SHIFT      28
#define DIDR_WP_MASK       0xF
#define CLAIMCLR_CLEAR_ALL 0xff

#define DRAR_VALID_MASK   0x00000003
#define DSAR_VALID_MASK   0x00000003
#define DRAR_ADDRESS_MASK 0xFFFFF000
#define DSAR_ADDRESS_MASK 0xFFFFF000
#define OSLSR_OSLM_MASK   0x00000009
#define OSLAR_UNLOCKED    0x00000000
#define OSLAR_LOCKED      0xC5ACCE55
#define LAR_UNLOCKED      0xC5ACCE55
#define LAR_LOCKED        0x00000000
#define OSDLR_UNLOCKED    0x00000000
#define OSDLR_LOCKED      0x00000001

typedef volatile struct
{                             /* Registers  Save?                                          */
    appf_u32 const didr;        /*         0  Read only                                    */
    appf_u32 dscr_i;            /*         1  ignore - use dscr_e instead                  */
    appf_u32 const dummy1[3];   /*       2-4  ignore                                       */
    appf_u32 dtrrx_dtrtx_i;     /*         5  ignore                                       */
    appf_u32 wfar;              /*         6  ignore - transient information               */
    appf_u32 vcr;               /*         7  Save                                         */
    appf_u32 const dummy2;      /*         8  ignore                                       */
    appf_u32 ecr;               /*         9  ignore                                       */
    appf_u32 dsccr;             /*        10  ignore                                       */
    appf_u32 dsmcr;             /*        11  ignore                                       */
    appf_u32 const dummy3[20];  /*     12-31  ignore                                       */
    appf_u32 dtrrx_e;           /*        32  ignore                                       */
    appf_u32 itr_pcsr;          /*        33  ignore                                       */
    appf_u32 dscr_e;            /*        34  Save                                         */
    appf_u32 dtrtx_e;           /*        35  ignore                                       */
    appf_u32 drcr;              /*        36  ignore                                       */
    appf_u32 eacr;              /*        37  Save - V7.1 only                             */
    appf_u32 const dummy4[2];   /*     38-39  ignore                                       */
    appf_u32 pcsr;              /*        40  ignore                                       */
    appf_u32 cidsr;             /*        41  ignore                                       */
    appf_u32 vidsr;             /*        42  ignore                                       */
    appf_u32 const dummy5[21];  /*     43-63  ignore                                       */
    appf_u32 bvr[16];           /*     64-79  Save                                         */
    appf_u32 bcr[16];           /*     80-95  Save                                         */
    appf_u32 wvr[16];           /*    96-111  Save                                         */
    appf_u32 wcr[16];           /*   112-127  Save                                         */
    appf_u32 const dummy6[16];  /*   128-143  ignore                                       */
    appf_u32 bxvr[16];          /*   144-159  Save if have Virtualization extensions       */
    appf_u32 const dummy7[32];  /*   160-191  ignore                                       */
    appf_u32 oslar;             /*       192  If oslsr[0] is 1, unlock before save/restore */
    appf_u32 const oslsr;       /*       193  ignore                                       */
    appf_u32 ossrr;             /*       194  ignore                                       */
    appf_u32 const dummy8;      /*       195  ignore                                       */
    appf_u32 prcr;              /*       196  ignore                                       */
    appf_u32 prsr;              /*       197  clear SPD on restore                         */
    appf_u32 const dummy9[762]; /*   198-959  ignore                                       */
    appf_u32 itctrl;            /*       960  ignore                                       */
    appf_u32 const dummy10[39]; /*   961-999  ignore                                       */
    appf_u32 claimset;          /*      1000  Restore claim bits to here                   */
    appf_u32 claimclr;          /*      1001  Save claim bits from here                    */
    appf_u32 const dummy11[2];  /* 1002-1003  ignore                                       */
    appf_u32 lar;               /*      1004  Unlock before restore                        */
    appf_u32 const lsr;         /*      1005  ignore                                       */
    appf_u32 const authstatus;  /*      1006  Read only                                    */
    appf_u32 const dummy12;     /*      1007  ignore                                       */
    appf_u32 const devid2;      /*      1008  Read only                                    */
    appf_u32 const devid1;      /*      1009  Read only                                    */
    appf_u32 const devid;       /*      1010  Read only                                    */
    appf_u32 const devtype;     /*      1011  Read only                                    */
    appf_u32 const pid[8];      /* 1012-1019  Read only                                    */
    appf_u32 const cid[4];      /* 1020-1023  Read only                                    */
} debug_registers_t;

typedef struct
{
    appf_u32 vcr;
    appf_u32 dscr_e;
    appf_u32 eacr;
    appf_u32 bvr[16];
    appf_u32 bcr[16];
    appf_u32 wvr[16];
    appf_u32 wcr[16];
    appf_u32 bxvr[16];
    appf_u32 claim;
} debug_context_t;    /* total size 86 * 4 = 344 bytes */

typedef struct
{
    /* 0x00 */  volatile unsigned int control;
    /* 0x04 */  const unsigned int configuration;
    /* 0x08 */  union
                {
                    volatile unsigned int w;
                    volatile unsigned char b[4];
                } power_status;
    /* 0x0c */  volatile unsigned int invalidate_all;
                char padding1[48];
    /* 0x40 */  volatile unsigned int filtering_start;
    /* 0x44 */  volatile unsigned int filtering_end;
                char padding2[8];
    /* 0x50 */  volatile unsigned int access_control;
    /* 0x54 */  volatile unsigned int ns_access_control;
} a9_scu_registers;

#define C_BIT 0x01

struct lockdown_regs
{
    unsigned int d, i;
};

typedef struct
{
    /* 0x000 */ const unsigned cache_id;
    /* 0x004 */ const unsigned cache_type;
                char padding1[0x0F8];
    /* 0x100 */ volatile unsigned control;
    /* 0x104 */ volatile unsigned aux_control;
    /* 0x108 */ volatile unsigned tag_ram_control;
    /* 0x10C */ volatile unsigned data_ram_control;
                char padding2[0x0F0];
    /* 0x200 */ volatile unsigned ev_counter_ctrl;
    /* 0x204 */ volatile unsigned ev_counter1_cfg;
    /* 0x208 */ volatile unsigned ev_counter0_cfg;
    /* 0x20C */ volatile unsigned ev_counter1;
    /* 0x210 */ volatile unsigned ev_counter0;
    /* 0x214 */ volatile unsigned int_mask;
    /* 0x218 */ const volatile unsigned int_mask_status;
    /* 0x21C */ const volatile unsigned int_raw_status;
    /* 0x220 */ volatile unsigned int_clear;
                char padding3[0x50C];
    /* 0x730 */ volatile unsigned cache_sync;
                char padding4[0x03C];
    /* 0x770 */ volatile unsigned inv_pa;
                char padding5[0x008];
    /* 0x77C */ volatile unsigned inv_way;
                char padding6[0x030];
    /* 0x7B0 */ volatile unsigned clean_pa;
                char padding7[0x004];
    /* 0x7B8 */ volatile unsigned clean_index;
    /* 0x7BC */ volatile unsigned clean_way;
                char padding8[0x030];
    /* 0x7F0 */ volatile unsigned clean_inv_pa;
                char padding9[0x004];
    /* 0x7F8 */ volatile unsigned clean_inv_index;
    /* 0x7FC */ volatile unsigned clean_inv_way;
                char paddinga[0x100];
    /* 0x900 */ volatile struct lockdown_regs lockdown[8];
                char paddingb[0x010];
    /* 0x950 */ volatile unsigned lock_line_en;
    /* 0x954 */ volatile unsigned unlock_way;
                char paddingc[0x2A8];
    /* 0xC00 */ volatile unsigned addr_filtering_start;
    /* 0xC04 */ volatile unsigned addr_filtering_end;
                char paddingd[0x338];
    /* 0xF40 */ volatile unsigned debug_ctrl;
                char paddinge[0x01C];
    /* 0xF60 */ volatile unsigned prefetch_ctrl;
                char paddingf[0x01C];
    /* 0xF80 */ volatile unsigned power_ctrl;
} pl310_registers;


typedef struct
{
    unsigned int aux_control;
    unsigned int tag_ram_control;
    unsigned int data_ram_control;
    unsigned int ev_counter_ctrl;
    unsigned int ev_counter1_cfg;
    unsigned int ev_counter0_cfg;
    unsigned int ev_counter1;
    unsigned int ev_counter0;
    unsigned int int_mask;
    unsigned int lock_line_en;
    struct lockdown_regs lockdown[8];
    unsigned int unlock_way;
    unsigned int addr_filtering_start;
    unsigned int addr_filtering_end;
    unsigned int debug_ctrl;
    unsigned int prefetch_ctrl;
    unsigned int power_ctrl;
} pl310_context;

/*
 * Maximum size of each item of context, in bytes
 * We round these up to 32 bytes to preserve cache-line alignment
 */

#define PMU_DATA_SIZE               128
#define TIMER_DATA_SIZE             128
#define VFP_DATA_SIZE               288
#define GIC_INTERFACE_DATA_SIZE      64
#define GIC_DIST_PRIVATE_DATA_SIZE   96
#define BANKED_REGISTERS_SIZE       128
#define CP15_DATA_SIZE               64
#define DEBUG_DATA_SIZE             352
#define MMU_DATA_SIZE                64
#define OTHER_DATA_SIZE              32
#define CONTROL_DATA_SIZE            64

#define GIC_DIST_SHARED_DATA_SIZE  2592
#define SCU_DATA_SIZE                32
#define L2_DATA_SIZE                 96
#define GLOBAL_TIMER_DATA_SIZE      128

#define WORD_SIZE 4



#define PM_SLEEP_SAVE_LEN        (PMU_DATA_SIZE/WORD_SIZE)
#define GIC_INTERFACE_SAVE_LEN       (GIC_INTERFACE_DATA_SIZE/WORD_SIZE)
#define GIC_DISTRIBUTOR_PRIVATE_SAVE_LEN (GIC_DIST_PRIVATE_DATA_SIZE/WORD_SIZE)
#define CP15_SAVE_LEN   (CP15_DATA_SIZE/WORD_SIZE)
#define A9_OTHER_LEN    (OTHER_DATA_SIZE/WORD_SIZE)
#define A9_GLOBAL_TIMER_LEN (GLOBAL_TIMER_DATA_SIZE/WORD_SIZE)
#define GIC_DISTRIBUTOR_SHARED_SAVE_LEN (GIC_DIST_SHARED_DATA_SIZE/WORD_SIZE)
#define A9_SCU_SAVE_LEN (SCU_DATA_SIZE/WORD_SIZE)
#define A9_DBG_SAVE_LEN (DEBUG_DATA_SIZE/WORD_SIZE)
#define CONTROL_DATA_SAVE_LEN (CONTROL_DATA_SIZE/WORD_SIZE)
#define MMU_DATA_SAVE_LEN (MMU_DATA_SIZE/WORD_SIZE)

#define __nop() __asm__ __volatile__( "nop\n" )
extern void _dsb(void);
extern void _dmb(void);
extern void _wfi(void);
extern void _wfe(void);
extern void _sev(void);

extern unsigned read_drar(void);
extern unsigned read_dsar(void);
extern void write_osdlr(unsigned value);
extern void disable_clean_inv_dcache_v7_l1(void);
extern void write_actlr(unsigned value);
extern unsigned read_actlr(void);
extern unsigned * copy_words(volatile unsigned *destination, volatile unsigned *source, unsigned num_words);
extern int gic_distributor_set_enabled(int enabled, unsigned gic_distributor_address);

extern void save_performance_monitors(appf_u32 *pointer);
extern void restore_performance_monitors(appf_u32 *pointer);
extern void save_a9_timers(appf_u32 *pointer, unsigned scu_address);
extern void save_a9_global_timer(appf_u32 *pointer, unsigned scu_address);
extern void restore_a9_timers(appf_u32 *pointer, unsigned scu_address);
extern void restore_a9_global_timer(appf_u32 *pointer, unsigned scu_address);
extern void save_vfp(appf_u32 *pointer);
extern void restore_vfp(appf_u32 *pointer);
extern void save_banked_registers(appf_u32 *pointer);
extern void restore_banked_registers(appf_u32 *pointer);
extern void save_gic_interface(appf_u32 *pointer, unsigned gic_interface_address, int is_secure);
extern int save_gic_distributor_private(appf_u32 *pointer, unsigned gic_distributor_address, int is_secure);
extern void restore_gic_interface(appf_u32 *pointer, unsigned gic_interface_address, int is_secure);
extern void restore_gic_distributor_private(appf_u32 *pointer, unsigned gic_distributor_address, int is_secure);
extern void save_cp15(appf_u32 *pointer);
extern void restore_cp15(appf_u32 *pointer);
extern void save_a9_other(appf_u32 *pointer, int is_secure);
extern void restore_a9_other(appf_u32 *pointer, int is_secure);
extern void save_control_registers(appf_u32 *pointer, int is_secure);
extern void restore_control_registers(appf_u32 *pointer, int is_secure);
extern void save_v7_debug(appf_u32 *pointer);
extern void restore_v7_debug(appf_u32 *pointer);
extern int save_gic_distributor_shared(appf_u32 *pointer, unsigned gic_distributor_address, int is_secure);
extern void restore_gic_distributor_shared(appf_u32 *pointer, unsigned gic_distributor_address, int is_secure);
extern void save_mmu(appf_u32 *pointer);
extern void restore_mmu(appf_u32 *pointer);
extern void save_a9_scu(appf_u32 *pointer, unsigned scu_address);
extern void restore_a9_scu(appf_u32 *pointer, unsigned scu_address);
extern void save_pl310(appf_u32 *pointer, unsigned pl310_address);
extern void restore_pl310(appf_u32 *pointer, unsigned pl310_address, int dormant);
extern int is_enabled_pl310(unsigned pl310_address);
extern void clean_pl310(unsigned pl310_address);


extern appf_u32 performance_monitor_save[];
extern a9_timer_context a9_timer_save;
extern appf_u32 gic_interface_save[];
extern appf_u32 gic_distributor_private_save[];
extern appf_u32 cp15_save[];
extern appf_u32 a9_other_save[];
extern appf_u32 a9_global_timer_save[];
extern appf_u32 gic_distributor_shared_save[];
extern appf_u32 a9_scu_save[];
extern pl310_context pl310_context_save;
extern debug_context_t a9_dbg_data_save;
extern appf_u32 control_data[];
extern appf_u32 mmu_data[];

extern void SerPrintf(char *fmt,...);
#endif //_SLEEP_HELPER_
