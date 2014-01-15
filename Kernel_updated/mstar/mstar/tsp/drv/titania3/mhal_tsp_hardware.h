
#ifndef _MHAL_TSP_HARDWARE_H_
#define _MHAL_TSP_HARDWARE_H_

// Titania2

//--------------------------------------------------------------------------------------------------
//  Global Variable
//--------------------------------------------------------------------------------------------------
#define TSP_ENGINE_NUM              1
#define TSP_PIDFLT_NUM              32  // REG_PID_FLT_0~1F           'h00210000 ~ 'h0021007c
#define TSP_PIDFLT1_NUM             8   // REG_PID1_FLT_0~7           'h00210080 ~ 'h0021009c, it's for PVR (while for section filtering, only PID1 0~7 can be used)
#define TSP_PIDFLT_NUM_ALL          (TSP_PIDFLT_NUM+ TSP_PIDFLT1_NUM)
#define TSP_SECFLT_NUM              TSP_PIDFLT_NUM
#define TSP_FILTER_DEPTH            16

#define TSP_CSACW_NUM               16

#define TSP_MIU_BASE_OFFSET         4
#define TSP_DMA_BASE_OFFSET         7

#endif // _MHAL_TSP_HARDWARE_H_
