
#ifndef _MDRV_MIOMAP_ST_H_
#define _MDRV_MIOMAP_ST_H_


//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
// MIOMAP_IOC_INFO 
typedef struct
{
    unsigned int                u32Addr;
    unsigned int                u32Size;
} DrvMIOMap_Info_t;

typedef struct
{
	unsigned short u16chiptype ; // T2/Oberon/Euclid ...
	unsigned short u16chipversion ; // U01 U02 U03 ...
} DrvMIOMap_ChipInfo_t ;

#endif // _MDRV_MIOMAP_ST_H_

