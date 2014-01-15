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
/// file    drvIR.c
/// @brief  IR Control Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#define _HAL_IR_C

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
//#include "MsCommon.h"
#include <linux/autoconf.h>
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
#include <linux/time.h>  //added
#include <linux/timer.h> //added
#include <asm/io.h>
#include <linux/semaphore.h>
#include "board/Board.h"
#include "mst_devid.h"
#include "mdrv_ir_st.h"//IR Pulse Shot Mode
#include "mhal_ir_multiple_reg.h"
#include "mdrv_ir.h"
#include "chip_int.h"
#include "mdrv_system.h"

#if (IR_TYPE_SEL == IR_TYPE_OLD)
#include "IR_MSTAR_OLD.h"
#elif (IR_TYPE_SEL == IR_TYPE_NEW)
#include "IR_MSTAR_NEW.h"
#elif (IR_TYPE_SEL == IR_TYPE_MSTAR_DTV)
#include "IR_MSTAR_DTV.h"
#elif (IR_TYPE_SEL == IR_TYPE_RC_V16)
#include "IR_MSTAR_RC_V16.h"
#elif (IR_TYPE_SEL == IR_TYPE_MSTAR_RAW)
#include "IR_MSTAR_RAW.h"
#elif (IR_TYPE_SEL == IR_TYPE_CUS03_DTV)
#include "IR_CUS03_DTV.h"
#elif (IR_TYPE_SEL == IR_TYPE_MSTAR_FANTASY)
#include "IR_MSTAR_FANTASY.h"
#elif (IR_TYPE_SEL == IR_TYPE_MSTAR_SZ1)
#include "IR_MSTAR_SZ1.h"
#elif (IR_TYPE_SEL==IR_TYPE_CUS08_RC5)
#include "IR_CUS08_RC5.h"
#elif (IR_TYPE_SEL == IR_TYPE_CHANGHONG)
#include "IR_CHANGHONG.h"
#elif (IR_TYPE_SEL == IR_TYPE_HAIER)
#include "IR_HAIER.h"
#elif (IR_TYPE_SEL == IR_TYPE_HISENSE)
#include "IR_HISENSE.h"
#elif (IR_TYPE_SEL == IR_TYPE_SKYWORTH)
#include "IR_SKYWORTH.h"
#elif (IR_TYPE_SEL==IR_TYPE_TCL)
#include "IR_TCL.h"
#elif (IR_TYPE_SEL == IR_TYPE_KONKA)
#include "IR_KONKA.h"
#else
#include "IR_MSTAR_DTV.h"
#endif

#ifdef CONFIG_IR_SUPPLY_RNG
#include <linux/input.h>
#include <random.h>
#include "mhal_rng_reg.h"
#endif

#if (IR_TYPE_SEL == IR_TYPE_MSTAR_FANTASY)
#define MSTAR_FANTASY_PROTOCOL_SUPPORTED 1
#else
#define MSTAR_FANTASY_PROTOCOL_SUPPORTED 0
#endif

#ifdef CONFIG_MSTAR_IR_FANTASY_MODE
#ifdef MSTAR_FANTASY_PROTOCOL_SUPPORTED
#undef MSTAR_FANTASY_PROTOCOL_SUPPORTED
#endif
#define MSTAR_FANTASY_PROTOCOL_SUPPORTED 1

#endif      //#if (MSTAR_IR_FANTASY_MODE)

extern IRModHandle IRDev;
//-------------------------------------------------------------------------------------------------
// Macros
//-------------------------------------------------------------------------------------------------
#define REG(addr)                   (*(volatile u32 *)(addr))
#define IR_PRINT(fmt, args...)      //printk("[IR][%05d] " fmt, __LINE__, ## args)

//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define IR_RAW_DATA_NUM	            4
//#define IR_FILTER_REPEAT_NUM    1
#define IR_SWDECODE_MODE_BUF_LEN    100

//#define IR_DEBUG
#ifdef IR_DEBUG
#define DEBUG_IR(x) (x)
#else
#define DEBUG_IR(x)
#endif

//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------
typedef enum
{
    E_IR_KEY_PROPERTY_INIT,
    E_IR_KEY_PROPERTY_1st,
    E_IR_KEY_PROPERTY_FOLLOWING
} IRKeyProperty;

typedef struct
{
	U16 u16HdcUpb;
	U16 u16HdcLob;
	U16 u16OfcUpb;
	U16 u16OfcLob;
	U16 u16OfcRpUpb;
	U16 u16OfcRpLob;
	U16 u16Lg01HUpb;
	U16 u16Lg01HLob;
	U16 u16Lg0Upb;
	U16 u16Lg0Lob;
	U16 u16Lg1Upb;
	U16 u16Lg1Lob;

} MS_IR_TimeSet;

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
// Forward declaration
//--------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Local Variables
//-------------------------------------------------------------------------------------------------
static MS_IR_InitCfg gstIRInitParam;
static MS_IR_TimeCfg gstIRTimeParam;
static MS_IR_TimeSet gstIRTimeSet;
static U8 bIRPass = 0;
static pid_t MasterPid = 0;
static U8   _u8IRRawModeBuf[IR_RAW_DATA_NUM];
static U32  _u8IRRawModeCount;

//--@@@--IR Pulse Shot Mode
#if 0//This can be used for debugging later
#define NEC_TAIL_KEY_MIN  22820*(1+0.2)
#define NEC_TAIL_KEY_MAX  58660*(1-0.2)
#define NEC_TAIL_RPT_MIN  96500*(1-0.2)
#define NEC_TAIL_RPT_MAX  96500*(1+0.2)
#endif
#define PULSE_SHOT_INT      0x01
#define PULSE_SHOT_BUF      0x02
#define PULSE_SHOT_MAX_TIME 0x7FFFF
static U32 gu32IRPulseShot;
static MS_IR_ShotInfo gu32IRShot;
struct semaphore gstIRSem;
//--@@@--IR Pulse Shot Mode

//# for SW decode mode
struct
{
    wait_queue_head_t inq;
    struct semaphore sem;
    U32 data; // 0=nothing, defined in fantasy protocol
}fantasy_protocol;

#if (IR_TYPE_SEL != IR_TYPE_CUS08_RC5)
static U32  _u32IRData[IR_SWDECODE_MODE_BUF_LEN];
#endif
static U32  _u32IRCount=0;

#if (IR_TYPE_SEL == IR_TYPE_HISENSE)
static U8   _u8IRHeadReceived=0;
static U8   _u8IRRepeateDetect=0;
static U8   _u8IRRepeated=0;
static U8   _u8IRType=0;
#elif (IR_TYPE_SEL == IR_TYPE_CUS08_RC5)
static U16  _u16IrRc5Data=0;          // for store shift ir data
static U16  _u16PreIrRc5Data=0;          // for store previous ir data
static U8 _u8IrRc5Bits=0;                // for store bit count
static U8 _u8IrRc5LastBit=0;//IR_RC5_LAST_BIT;            // for store bit var
#elif (IR_TYPE_SEL == IR_TYPE_SKYWORTH)
static BOOL _btoshiba_code=FALSE;
static BOOL _brepeat_flag=FALSE;
#endif
//# for SW decode mode

static U32  _u32_1stDelayTimeMs;
static U32  _u32_2ndDelayTimeMs;
static IRKeyProperty _ePrevKeyProperty;
static unsigned long  _ulPrevKeyTime;
static unsigned long  _ulPrevKeyRepeatTime;
static BOOL  _bCheckQuickRepeat;
static U8   _u8PrevKeyCode;

static unsigned long  _ulLastKeyPresentTime;
static MS_IR_KeyInfo _KeyReceived;   //temporary solution

//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
static irqreturn_t _MDrv_IR_ISR(int irq, void *dev_id);
static BOOL _MDrv_IR_GetKeyMultiple(U8 *pu8Key, U8 *pu8System, U8 *pu8Flag);
static unsigned long _MDrv_IR_GetSystemTime(void);
static BOOL _MDrv_IR_GetPulseShot(U32* pu32ShotCount, U8* pu8Mode);//--@@@--IR Pulse Shot Mode

//-------------------------------------------------------------------------------------------------
/// Translate from IR key to internal key.
/// @param  u8KeyData  \b IN: IR key value.
/// @return translated internal IR key.
//-------------------------------------------------------------------------------------------------
#if 0
static U8 _MDrv_IR_ParseKey(U8 u8KeyData)
{
    U8 u8RetVal;

    if ( bIRPass ) return KEYCODE_DUMMY;

    switch(u8KeyData)
    {
    case IRKEY_TV_RADIO        : u8RetVal = KEYCODE_TV_RADIO;         break;
    case IRKEY_CHANNEL_LIST    : u8RetVal = KEYCODE_CHANNEL_LIST;     break;
    case IRKEY_CHANNEL_FAV_LIST: u8RetVal = KEYCODE_CHANNEL_FAV_LIST; break;
    case IRKEY_CHANNEL_RETURN  : u8RetVal = KEYCODE_CHANNEL_RETURN;   break;
    case IRKEY_CHANNEL_PLUS    : u8RetVal = KEYCODE_CHANNEL_PLUS;     break;
    case IRKEY_CHANNEL_MINUS   : u8RetVal = KEYCODE_CHANNEL_MINUS;    break;

    case IRKEY_AUDIO           : u8RetVal = KEYCODE_AUDIO;            break;
    case IRKEY_VOLUME_PLUS     : u8RetVal = KEYCODE_VOLUME_PLUS;      break;
    case IRKEY_VOLUME_MINUS    : u8RetVal = KEYCODE_VOLUME_MINUS;     break;

    case IRKEY_UP              : u8RetVal = KEYCODE_UP;               break;
    case IRKEY_POWER           : u8RetVal = KEYCODE_POWER;            break;
    case IRKEY_EXIT            : u8RetVal = KEYCODE_EXIT;             break;
    case IRKEY_MENU            : u8RetVal = KEYCODE_MENU;             break;
    case IRKEY_DOWN            : u8RetVal = KEYCODE_DOWN;             break;
    case IRKEY_LEFT            : u8RetVal = KEYCODE_LEFT;             break;
    case IRKEY_SELECT          : u8RetVal = KEYCODE_SELECT;           break;
    case IRKEY_RIGHT           : u8RetVal = KEYCODE_RIGHT;            break;

    case IRKEY_NUM_0           : u8RetVal = KEYCODE_NUMERIC_0;        break;
    case IRKEY_NUM_1           : u8RetVal = KEYCODE_NUMERIC_1;        break;
    case IRKEY_NUM_2           : u8RetVal = KEYCODE_NUMERIC_2;        break;
    case IRKEY_NUM_3           : u8RetVal = KEYCODE_NUMERIC_3;        break;
    case IRKEY_NUM_4           : u8RetVal = KEYCODE_NUMERIC_4;        break;
    case IRKEY_NUM_5           : u8RetVal = KEYCODE_NUMERIC_5;        break;
    case IRKEY_NUM_6           : u8RetVal = KEYCODE_NUMERIC_6;        break;
    case IRKEY_NUM_7           : u8RetVal = KEYCODE_NUMERIC_7;        break;
    case IRKEY_NUM_8           : u8RetVal = KEYCODE_NUMERIC_8;        break;
    case IRKEY_NUM_9           : u8RetVal = KEYCODE_NUMERIC_9;        break;

    case IRKEY_MUTE            : u8RetVal = KEYCODE_MUTE;             break;
    case IRKEY_PAGE_UP         : u8RetVal = KEYCODE_PAGE_UP;          break;
    case IRKEY_PAGE_DOWN       : u8RetVal = KEYCODE_PAGE_DOWN;        break;
    case IRKEY_CLOCK           : u8RetVal = KEYCODE_CLOCK;            break;

    case IRKEY_INFO            : u8RetVal = KEYCODE_INFO;             break;
    case IRKEY_RED             : u8RetVal = KEYCODE_RED;              break;
    case IRKEY_GREEN           : u8RetVal = KEYCODE_GREEN;            break;
    case IRKEY_YELLOW          : u8RetVal = KEYCODE_YELLOW;           break;
    case IRKEY_BLUE            : u8RetVal = KEYCODE_BLUE;             break;
    case IRKEY_MTS             : u8RetVal = KEYCODE_MTS;              break;
    case IRKEY_NINE_LATTICE    : u8RetVal = KEYCODE_NINE_LATTICE;     break;
#if defined(DVB_SYSTEM)
    case IRKEY_TTX             : u8RetVal = KEYCODE_TTX;              break;
#elif defined(ATSC_SYSTEM)
    case IRKEY_CC              : u8RetVal = KEYCODE_CC;               break;
#endif
    case IRKEY_INPUT_SOURCE    : u8RetVal = KEYCODE_INPUT_SOURCE;     break;
    case IRKEY_CRADRD          : u8RetVal = KEYCODE_CRADRD;           break;
//    case IRKEY_PICTURE         : u8RetVal = KEYCODE_PICTURE;          break;
    case IRKEY_ZOOM            : u8RetVal = KEYCODE_ZOOM;             break;
    case IRKEY_DASH            : u8RetVal = KEYCODE_DASH;             break;
    case IRKEY_SLEEP           : u8RetVal = KEYCODE_SLEEP;            break;
    case IRKEY_EPG             : u8RetVal = KEYCODE_EPG;              break;
    case IRKEY_PIP             : u8RetVal = KEYCODE_PIP;              break;

  	case IRKEY_MIX             : u8RetVal = KEYCODE_MIX;              break;
    case IRKEY_INDEX           : u8RetVal = KEYCODE_INDEX;            break;
    case IRKEY_HOLD            : u8RetVal = KEYCODE_HOLD;             break;
    case IRKEY_PREVIOUS        : u8RetVal = KEYCODE_PREVIOUS;         break;
    case IRKEY_NEXT            : u8RetVal = KEYCODE_NEXT;             break;
    case IRKEY_BACKWARD        : u8RetVal = KEYCODE_BACKWARD;         break;
    case IRKEY_FORWARD         : u8RetVal = KEYCODE_FORWARD;          break;
    case IRKEY_PLAY            : u8RetVal = KEYCODE_PLAY;             break;
    case IRKEY_RECORD          : u8RetVal = KEYCODE_RECORD;           break;
    case IRKEY_STOP            : u8RetVal = KEYCODE_STOP;             break;
    case IRKEY_PAUSE           : u8RetVal = KEYCODE_PAUSE;            break;

    case IRKEY_SIZE            : u8RetVal = KEYCODE_SIZE;             break;
    case IRKEY_REVEAL          : u8RetVal = KEYCODE_REVEAL;           break;
    case IRKEY_SUBCODE         : u8RetVal = KEYCODE_SUBCODE;          break;

    default                    : u8RetVal = KEYCODE_DUMMY;            break;
    }

    return u8RetVal;
}
#endif

static unsigned long _MDrv_IR_GetSystemTime(void)
{
    return((unsigned long)((jiffies)*(1000/HZ)));
    //return 0;
}

static void _MDrv_IR_ClearFIFO(void)
{
    unsigned long i;

    // Empty the FIFO
    for(i=0; i<8; i++)
    {
        U8 u8Garbage;

        if(MHal_IR_REG(REG_IR_STATUS_H) & IR_FIFO_EMPTY)
            break;
        u8Garbage = MHal_IR_REG(REG_IR_KEY_DATA);
        MHal_IR_REG(REG_IR_STATUS_CTRL) |= IR_FIFO_RD_PULSE; //read
    }

}

static U32 _MDrv_IR_GetShotCount(void)
{
    U32 u32ShotCntLo,u32ShotCntHi,u32ShotOverCnt;
    U32 u32ShotCount;

    u32ShotCntLo = (U32)(MHal_IR_REG(REG_IR_SHOT_CNT_L) & IR_SHOT_CNT_L_MSK);
    u32ShotCntHi = (U32)((MHal_IR_REG(REG_IR_SHOT_CNT_H) & IR_SHOT_CNT_H_MSK) << 8);
    u32ShotOverCnt = (U32)((MHal_IR_REG(REG_IR_SHOT_CNT_O) & IR_SHOT_CNT_O_MSK) << 16);
    u32ShotCount = u32ShotOverCnt | u32ShotCntHi | u32ShotCntLo;
    return u32ShotCount;
}

//-------------------------------------------------------------------------------------------------
/// Get shot count when ISR happens.
/// @return BOOL
//-------------------------------------------------------------------------------------------------
//--@@@--IR Pulse Shot Mode
//default shot mode is BUF mode
static BOOL _MDrv_IR_GetPulseShot(U32* pu32ShotCount, U8* pu8Mode)
{
    static U32 su32Length = 0;
    BOOL bSignalDataReady=FALSE;

    *pu32ShotCount = _MDrv_IR_GetShotCount();
    #if 1//Buffer mode
    if(*pu32ShotCount>=gstIRTimeParam.u32TimeoutCyc)
    {
        su32Length = 0;
        gu32IRShot.u32Buffer[gu32IRShot.u8WtIdx][su32Length++] = PULSE_SHOT_MAX_TIME;
        return bSignalDataReady;
    }
    gu32IRShot.u32Buffer[gu32IRShot.u8WtIdx][su32Length++] = *pu32ShotCount;
    if( (*pu32ShotCount>=gstIRTimeParam.tTail.gu32KeyMin && *pu32ShotCount<=gstIRTimeParam.tTail.gu32KeyMax) \
      ||(*pu32ShotCount>=gstIRTimeParam.tTail.gu32RptMin && *pu32ShotCount<=gstIRTimeParam.tTail.gu32RptMax) \
      || su32Length>=IR_MAX_BUF_LEN)
    {
        IR_PRINT(printk("MATCH!(su32Length=%d)\n",su32Length));
        if(down_trylock(&gstIRSem)==0)
        {
            gu32IRShot.u8RdIdx = gu32IRShot.u8WtIdx;
            gu32IRShot.u8WtIdx = (++gu32IRShot.u8WtIdx)%2;
            gu32IRShot.u32Length = su32Length;
            su32Length = 0;
            up(&gstIRSem);
        }
        *pu8Mode = PULSE_SHOT_BUF;
        bSignalDataReady=TRUE;
    }
    #else//Interrupt mode
    *pu8Mode = PULSE_SHOT_INT;
    bSignalDataReady=TRUE;
    #endif
    
    return bSignalDataReady;
}
//--@@@--IR Pulse Shot Mode

//-------------------------------------------------------------------------------------------------
/// ISR when receive IR key.
/// @return None
//-------------------------------------------------------------------------------------------------
static irqreturn_t _MDrv_IR_ISR(int irq, void *dev_id)
{
    U8 u8Key = 0, u8RepeatFlag = 0;
    U8 u8System = 0;

    if(gstIRInitParam.u8DecMode == IR_DECMODE_SW)
    {
      #if (IR_TYPE_SEL == IR_TYPE_SKYWORTH)
      U16 tmp;
      tmp = _MDrv_IR_GetShotCount();
      _u32IRData[_u32IRCount++] = tmp;
      if(tmp>10000)
      {
          _u32IRCount=0;
          _u32IRData[_u32IRCount++] = tmp;
      }
      else if(tmp> (gstIRTimeSet.u16HdcLob)&&tmp < (gstIRTimeSet.u16HdcUpb))
      {
          _u32IRCount=1;
          _u32IRData[_u32IRCount++] = tmp;
      }
      
      if(_u32IRCount==2)
      {
          if ((_u32IRData[1] > (gstIRTimeSet.u16HdcLob)) && (_u32IRData[1] < (gstIRTimeSet.u16HdcUpb)))
              _btoshiba_code=TRUE;
          else
              _btoshiba_code=FALSE;
      }
      else if(_u32IRCount==3)
      {
          if (_u32IRData[2] > gstIRTimeSet.u16Lg1Lob && _u32IRData[2] < gstIRTimeSet.u16Lg1Upb)   //repeat key
              _brepeat_flag=TRUE;
      }
      /*if(!btoshiba_code&&_u32IRCount>18)
      {
          for(tmp=0;tmp<18;tmp++)
              printk("<<<%d data %d\n",tmp,_u32IRData[tmp]);
          _u32IRCount=0;
      }
      else if(btoshiba_code&&_u32IRCount>33)
      {
          for(tmp=0;tmp<34;tmp++)
              printk("<<<%d data %d\n",tmp,_u32IRData[tmp]);
          _u32IRCount=0;
      }*/
      
    #elif (IR_TYPE_SEL == IR_TYPE_CUS08_RC5)
      U16 u16IrCounter,u16N_Shot;
      
      u16IrCounter = _MDrv_IR_GetShotCount();
      u16N_Shot = MHal_IR_REG(REG_IR_STATUS_L) & IR_SHOT_P;
      
      if ((u16IrCounter > ONE_BIT_UB) || (u16IrCounter < HALF_BIT_LB))    // if ir counter so big, then reset and return
      {
          _u8IrRc5LastBit = TRUE;
          _u16IrRc5Data = 0;
          _u8IrRc5Bits = 0;
      }
      else
      {
          if (u16IrCounter >= HALF_BIT_LB && u16IrCounter <= HALF_BIT_UB)     //detect
          {
              _u8IrRc5Bits++;
              if (_u8IrRc5LastBit)
              {
                  _u8IrRc5LastBit = FALSE;
                  _u16IrRc5Data <<= 1;
                  if (u16N_Shot==0) // Is nshot
                      _u16IrRc5Data |= 1;
              }
              else
              {
                  _u8IrRc5LastBit = TRUE;
              }
          }
          else if (u16IrCounter >= ONE_BIT_LB && u16IrCounter <= ONE_BIT_UB)
          {
              if (_u8IrRc5LastBit)
              {
                  _u16IrRc5Data <<= 1;
                  if (u16N_Shot==0) // Is nshot
                      _u16IrRc5Data |= 1;
                  _u8IrRc5Bits += 2;
              }
              else    //error stste
              {
                  _u16IrRc5Data = 0;
                  _u8IrRc5Bits = 0;
              }
          }
      
          if (_u8IrRc5Bits == IR_RC5_DATA_BITS)
          {
              if(u16N_Shot==0)
              {
                  _u16IrRc5Data <<= 1;
                  _u8IrRc5Bits++;
              }
          }
      }
      
    #elif (IR_TYPE_SEL == IR_TYPE_HISENSE)
      static unsigned long ulPreTime;
      U32 tmp;
      
      if ((_MDrv_IR_GetSystemTime() - ulPreTime > gstIRTimeParam.u32TimeoutCyc/1000))//||_u8IRRepeated) //timeout or not handler yet
      {
          _u32IRCount = 0;
          _u8IRRepeateDetect=0;
          _u8IRRepeated=0;
          _u8IRHeadReceived=0;
      }
      if (_u32IRCount <IR_SWDECODE_MODE_BUF_LEN)
      {
          tmp = _MDrv_IR_GetShotCount();
          if(tmp<gstIRTimeSet.u16Lg0Lob)//error signal
          {
              _u32IRCount = 0;
              _u8IRRepeateDetect=0;
              _u8IRRepeated=0;
              _u8IRHeadReceived=0;
          }
      
          if( tmp>(gstIRTimeSet.u16HdcLob))//Head received
          {
              _u32IRCount=0;
              _u8IRHeadReceived=1;
          }
          if(_u8IRHeadReceived)//begin data received
          {
              _u32IRData[_u32IRCount++] = tmp;
      
              if(_u8IRRepeateDetect)
              {
                  if( tmp>(gstIRTimeSet.u16OfcRpLob) && tmp<(gstIRTimeSet.u16OfcRpUpb) )
                  {
                      _u8IRRepeated=1;
                      _u8IRHeadReceived=0;
                  }
              }
          }
      
      }
      ulPreTime = _MDrv_IR_GetSystemTime();
      
    #else
      static unsigned long ulPreTime;
      if (_MDrv_IR_GetSystemTime() - ulPreTime > gstIRTimeParam.u32TimeoutCyc/1000) //timeout
      {
          _u32IRCount = 0;
      }
      
      if (_u32IRCount <IR_SWDECODE_MODE_BUF_LEN)
      {
          _u32IRData[_u32IRCount++] = _MDrv_IR_GetShotCount();
      }
      ulPreTime = _MDrv_IR_GetSystemTime();
    #endif
    }
  
#ifdef CONFIG_IR_SUPPLY_RNG
    {
        unsigned int u32Temp;
        unsigned short u16Temp;

        u16Temp = MIPS_REG(REG_RNG_OUT);
        memcpy((unsigned char *)&u32Temp+0, &u16Temp, 2);
        u16Temp = MIPS_REG(REG_RNG_OUT);
        memcpy((unsigned char *)&u32Temp+2, &u16Temp, 2);
        add_input_randomness(EV_MSC, MSC_SCAN, u32Temp);
    }
#endif

    if(MSTAR_FANTASY_PROTOCOL_SUPPORTED)
    {
        if (_MDrv_IR_GetKeyMultiple(&u8Key, &u8System, &u8RepeatFlag))
        {
            if(down_trylock(&fantasy_protocol.sem)==0)
            {
                if(fantasy_protocol.data!=0)
                {
                    //printk(KERN_WARNING"unprocessed fantasy protocol data!!\n");
                }
                else
                {
					#if (IR_TYPE_SEL == IR_TYPE_HISENSE)//for hisense need suport 2 ir head code ;
					U8 hisense_factory_ir=0;
					if(u8RepeatFlag&FCTORY_HEADER_RECEIVE)
					{
						u8RepeatFlag &= (~FCTORY_HEADER_RECEIVE);
	                    hisense_factory_ir=1;
					}
                    if(gstIRInitParam.u8DecMode == IR_DECMODE_SHOT)//--@@@--IR Pulse Shot Mode
                    {
                        fantasy_protocol.data = (u8Key<<24) | gu32IRPulseShot;//Pulse Shot value here is only for INT mode
                    }
                    else
                    {
                        fantasy_protocol.data = (u8Key<<8 | u8RepeatFlag);
                    }
					if(hisense_factory_ir)//HISENSE_FACTORY IR
                    	fantasy_protocol.data |= (0x04 << 28);
					else
	                    fantasy_protocol.data |= (0x01 << 28);
					#else //
                    if(gstIRInitParam.u8DecMode == IR_DECMODE_SHOT)//--@@@--IR Pulse Shot Mode
                    {
                        fantasy_protocol.data = (u8Key<<24) | gu32IRPulseShot;//Pulse Shot value here is only for INT mode
                    }
                    else
                    {
                        fantasy_protocol.data = (u8Key<<8 | u8RepeatFlag);
                    }
                    //set fantasy class to IR
                    fantasy_protocol.data |= (0x01 << 28);
					#endif
                }
                up(&fantasy_protocol.sem);

                wake_up_interruptible(&fantasy_protocol.inq);
            }
        }
    }
    else if (_MDrv_IR_GetKeyMultiple(&u8Key, &u8System, &u8RepeatFlag))
    {
        _ulLastKeyPresentTime = _MDrv_IR_GetSystemTime();
        IR_PRINT("_MDrv_IR_ISR() -> %x, %x\n", u8Key, u8RepeatFlag);

        //temporary solution, need to implement ring buffer for this
        _KeyReceived.u8Key = u8Key;
        _KeyReceived.u8System = u8System;
        _KeyReceived.u8Flag = u8RepeatFlag;
        _KeyReceived.u8Valid = 1;

       	if (IRDev.async_queue)
    		kill_fasync(&IRDev.async_queue, SIGIO, POLL_IN);
    }

    //Disabled already by SA_INTERRUPT at initial
    //MsOS_EnableInterrupt(E_FIQ_IR);
    return IRQ_HANDLED;
}

//-------------------------------------------------------------------------------------------------
/// Get IR key. It is a non-blocking function.
/// @param pu8Key  \b IN: Return IR key value.
/// @param pu8Flag \b IN: Return IR repeat code.
///
/// @return TRUE:  Success
/// @return FALSE: No key or repeat key is faster than the specified period
//-------------------------------------------------------------------------------------------------
static BOOL _MDrv_IR_GetKey_FULL(U8 *pu8Key, U8 *pu8System, U8 *pu8Flag)
{
    unsigned long i;
    BOOL bRet=FALSE;
    *pu8System = 0;

    if(MHal_IR_REG(REG_IR_STATUS_H) & IR_FIFO_EMPTY)
    {
        _bCheckQuickRepeat = 0;
        return FALSE;
    }

    if(((_MDrv_IR_GetSystemTime() - _ulPrevKeyTime) >= IR_TIMEOUT_CYC/1000))
    {
        *pu8Key = MHal_IR_REG(REG_IR_KEY_DATA);
        MHal_IR_REG(REG_IR_STATUS_CTRL) |= IR_FIFO_RD_PULSE; //read
        udelay(1000*10);
        _u8PrevKeyCode = *pu8Key;
        *pu8Flag = 0;
        _ulPrevKeyTime = _MDrv_IR_GetSystemTime();
        _ePrevKeyProperty = E_IR_KEY_PROPERTY_INIT;
        _bCheckQuickRepeat = 0;
        _MDrv_IR_ClearFIFO();
        return TRUE;
    }
    else
    {
        if(_bCheckQuickRepeat==0)
        {
           _bCheckQuickRepeat = 1;
            _ulPrevKeyTime = _MDrv_IR_GetSystemTime();
            _MDrv_IR_ClearFIFO();
            return FALSE;
        }
        *pu8Key = MHal_IR_REG(REG_IR_KEY_DATA);
        MHal_IR_REG(REG_IR_STATUS_CTRL) |= IR_FIFO_RD_PULSE; //read
        udelay(1000*10);
        *pu8Flag = (MHal_IR_REG(REG_IR_STATUS_H) & IR_RPT_FLAG)? 1 : 0;
        bRet = FALSE;
        _ulPrevKeyTime = _MDrv_IR_GetSystemTime();

        if ( (*pu8Flag == 1) && ( *pu8Key == _u8PrevKeyCode ))
        {
            i = _MDrv_IR_GetSystemTime();
            if( _ePrevKeyProperty == E_IR_KEY_PROPERTY_INIT)
            {
                _u8PrevKeyCode     = *pu8Key;
                _ulPrevKeyRepeatTime    = i;
                _ePrevKeyProperty  = E_IR_KEY_PROPERTY_1st;
            }
            else if(_ePrevKeyProperty == E_IR_KEY_PROPERTY_1st)
            {
                if( (i - _ulPrevKeyRepeatTime) > _u32_1stDelayTimeMs)
                {
                    _ulPrevKeyRepeatTime = i;
                    _ePrevKeyProperty  = E_IR_KEY_PROPERTY_FOLLOWING;
                    bRet = TRUE;
                }
            }
            else //E_IR_KEY_PROPERTY_FOLLOWING
            {
                if( (i - _ulPrevKeyRepeatTime) > _u32_2ndDelayTimeMs)
                {
                    _ulPrevKeyRepeatTime = i;
                    bRet = TRUE;
                }
            }
        }
    }

    // Empty the FIFO
    _MDrv_IR_ClearFIFO();
    return bRet;

}

//-------------------------------------------------------------------------------------------------
/// Get IR key.
/// @param pu8Key  \b IN: Return IR key value.
/// @param pu8Flag \b IN: Return IR repeat code.
///
/// @return TRUE:  Success
/// @return FALSE: Failure
//-------------------------------------------------------------------------------------------------
static BOOL _MDrv_IR_GetKey_RAW(U8 *pu8Key, U8 *pu8System, U8 *pu8Flag)
{
    BOOL bRet = FALSE;

    u32 i, j;
    *pu8System = 0;

    for (j=0; j<IR_RAW_DATA_NUM; j++)
    {
        if ( MHal_IR_REG(REG_IR_STATUS_H) & IR_FIFO_EMPTY)  // check FIFO empty
            break;

        _u8IRRawModeBuf[_u8IRRawModeCount++] = MHal_IR_REG(REG_IR_KEY_DATA);
        MHal_IR_REG(REG_IR_STATUS_CTRL) |= IR_FIFO_RD_PULSE; //read

    	for(i=0;i<5;i++); //Delay

        if(_u8IRRawModeCount == IR_RAW_DATA_NUM)
        {
            _u8IRRawModeCount = 0;
            if( (_u8IRRawModeBuf[0]==gstIRInitParam.u8HdrCode0) &&
                (_u8IRRawModeBuf[1]==gstIRInitParam.u8HdrCode1) )
            {
                if(_u8IRRawModeBuf[2] == (U8)(~_u8IRRawModeBuf[3]))
                {
                    //if (!MDrv_SYS_GetDisplayControllerSeparated())
                        *pu8Key = _u8IRRawModeBuf[2];    // translate for MIPS

                    //TODO: Implement repeat code later.
                    *pu8Flag = 0;
                    bRet = TRUE;
                    break;
                }
            }
        }
    }

    // Empty the FIFO
    _MDrv_IR_ClearFIFO();

    return bRet;
}

static BOOL _MDrv_IR_GetKey_RC(U8 *pu8Key, U8 *pu8System, U8 *pu8Flag)
{
    HAL_IR_RC_KeyData IRRCKeyData;

    if(HAL_IR_RC_IsFIFOEmpty()==TRUE)
        return FALSE;
    HAL_IR_RC_GetKeyData(&IRRCKeyData);
    HAL_IR_RC_ClearFIFO();
    if(IRRCKeyData.u8Valid)
    {
        *pu8Key= IRRCKeyData.u8Command;
        *pu8System = IRRCKeyData.u8Address;
        *pu8Flag = IRRCKeyData.u8Repeat;
        IR_PRINT("Key=0x%X, System=%X Flag=%X\n",*pu8Key,*pu8System,*pu8Flag);
        return TRUE;
    }

    return FALSE;

}

//-------------------------------------------------------------------------------------------------
/// Get IR key.
/// @param pu8Key  \b IN: Return IR key value.
/// @param pu8Flag \b IN: Return IR repeat code.
///
/// @return TRUE:  Success
/// @return FALSE: Failure
//-------------------------------------------------------------------------------------------------
static BOOL _MDrv_IR_GetKey_SW(U8 *pu8Key, U8 *pu8System, U8 *pu8Flag)
{
    BOOL bRet = FALSE;

    #if (IR_TYPE_SEL != IR_TYPE_CUS08_RC5)
    U32 u8Byte, u8Bit;
    U8 u8IRSwModeBuf[IR_RAW_DATA_NUM];
    U32 *pu32IRData = NULL;
    *pu8System = 0;
    #else
    unsigned long i;
    #endif

    #if (IR_TYPE_SEL == IR_TYPE_SKYWORTH)
    //U8 u8tmp;

    if(_btoshiba_code)
    {
        if(_brepeat_flag)
        {
            *pu8Key = _u8PrevKeyCode;
            *pu8Flag = 1;
            _u32IRCount = 0;
            _btoshiba_code=FALSE;
            _brepeat_flag=FALSE;
            printk("repeat input key is 0x%x\n",*pu8Key);
            return TRUE;
        }

        if (_u32IRCount< (34))  //multiple 8
            return FALSE; //not complete yet
        else
        {
            for( u8Byte=0; u8Byte<IR_RAW_DATA_NUM; u8Byte++)
            {
               u8IRSwModeBuf[u8Byte] = 0;
            }
            pu32IRData = &_u32IRData[2];

            for( u8Byte=0; u8Byte<IR_RAW_DATA_NUM; u8Byte++)
            {
                for( u8Bit=0; u8Bit<8; u8Bit++)
                {
                    u32 u32BitLen = pu32IRData[u8Byte*8+u8Bit];
                    u8IRSwModeBuf[u8Byte] >>= 1;

                    if( u32BitLen > gstIRTimeSet.u16Lg0Lob && u32BitLen < gstIRTimeSet.u16Lg0Upb ) //0
                    {
                        u8IRSwModeBuf[u8Byte] |= 0x00;
                    }
                    else // if (u32BitLen > gstIRTimeSet.u16Lg1Lob && u32BitLen < gstIRTimeSet.u16Lg1Upb) //1
                    {
                        u8IRSwModeBuf[u8Byte] |= 0x80;
                    }
                    /*else
                    {
                        DEBUG_IR(printk(" invalid waveform,0x%x\n",u32BitLen));
                        for(u8tmp=0;u8tmp<_u32IRCount;u8tmp++)
                            printk("%d data=%d\n",u8tmp,_u32IRData[u8tmp]);
                        _u32IRCount = 0;
                        btoshiba_code=FALSE;
                        return FALSE;
                    }*/
                }
                if((u8IRSwModeBuf[0]!=gstIRInitParam.u8HdrCode0)||(u8Byte==1&&u8IRSwModeBuf[1]!=gstIRInitParam.u8HdrCode1))
                {
                    U8 u8tmp;
                    for(u8tmp=0;u8tmp<_u32IRCount;u8tmp++)
                            printk("%d data=%d\n",u8tmp,_u32IRData[u8tmp]);
                    _u32IRCount = 0;
                    _btoshiba_code=FALSE;
                    DEBUG_IR(printk("invalid header\n"));
                    return FALSE;
                }
                else if(u8Byte==3)
                {
                    if(u8IRSwModeBuf[2] == (u8)~u8IRSwModeBuf[3])
                    {
                        *pu8Key = u8IRSwModeBuf[2];
                        *pu8Flag = 0;
                        _u8PrevKeyCode=*pu8Key;
                        _u32IRCount = 0;
                        _btoshiba_code=FALSE;
                        DEBUG_IR(printk("input key is 0x%x\n",*pu8Key));
                        printk("input key is 0x%x\n",*pu8Key);
                        return TRUE;
                    }
                }
            }//for( u8Byte=0; u8Byte<IR_RAW_DATA_NUM; u8Byte++)
        }//else
    }//toshiba code
    else
    {
        if(_u32IRCount< (18))  //multiple 4
            return FALSE; //not complete yet
        else
        {
            for( u8Byte=0; u8Byte<2; u8Byte++)
            {
               u8IRSwModeBuf[u8Byte] = 0;
            }
            pu32IRData = &_u32IRData[2];
            for( u8Byte=0; u8Byte<2; u8Byte++)
            {
                for( u8Bit=0; u8Bit<8; u8Bit++)
                {
                    u32 u32BitLen = pu32IRData[u8Byte*8+u8Bit];
                    u8IRSwModeBuf[u8Byte] >>= 1;

                    if( u32BitLen > gstIRTimeSet.u16Lg0Lob && u32BitLen < gstIRTimeSet.u16Lg0Upb ) //0
                    {
                        u8IRSwModeBuf[u8Byte] |= 0x00;
                    }
                    else // if (u32BitLen > gstIRTimeSet.u16Lg1Lob && u32BitLen < gstIRTimeSet.u16Lg1Upb) //1
                    {
                        u8IRSwModeBuf[u8Byte] |= 0x80;
                    }
                    /*else
                    {
                        DEBUG_IR(printk(" invalid waveform,0x%x\n",u32BitLen));
                        bRet = FALSE;
                    }*/
                }
            }//for( u8Byte=0; u8Byte<2; u8Byte++)

            if(((u8IRSwModeBuf[0]&0xf0)>>4)==IR_HEADER_CODE3)
            {
                u8IRSwModeBuf[2]=u8IRSwModeBuf[0]&0x0f;
                u8IRSwModeBuf[3]=u8IRSwModeBuf[1]&0x0f;
                if((u8IRSwModeBuf[2]+u8IRSwModeBuf[3])==0x0f)
                {
                    switch(u8IRSwModeBuf[2])
                    {
                        case IRKEY_DESIL1:
                        case IRKEY_DESIL2:
                        case IRKEY_DESIL3:
                        case IRKEY_DESIL4:
                        case IRKEY_DESIL5:
                        case IRKEY_DESIL6:
                            *pu8Key=IRKEY_SLIDE_L1+(u8IRSwModeBuf[2]-IRKEY_DESIL1);
                             break;
                        case IRKEY_REVERSE1:
                        case IRKEY_REVERSE2:
                        case IRKEY_REVERSE3:
                        case IRKEY_REVERSE4:
                        case IRKEY_REVERSE5:
                        case IRKEY_REVERSE6:
                            *pu8Key=IRKEY_SLIDE_R1+(u8IRSwModeBuf[2]-IRKEY_REVERSE1);
                            break;
                    }
                    _u8PrevKeyCode=*pu8Key;
                    DEBUG_IR(printk("slide key 0x%x\n",*pu8Key));
                    printk("slide key 0x%x\n",*pu8Key);
                    *pu8Flag = 0;
                    bRet = TRUE;
                }
            }
            _u32IRCount = 0;
            _btoshiba_code=FALSE;
            return bRet;
        }
    }
    return bRet;

    #elif (IR_TYPE_SEL == IR_TYPE_CUS08_RC5)

        if(_u8IrRc5Bits > IR_RC5_DATA_BITS)
        {
            if ((_u16IrRc5Data&0xF000)==0x3000)
            {
                if(_u16PreIrRc5Data==_u16IrRc5Data)
                    *pu8Flag = TRUE;

                *pu8Key = _u16IrRc5Data&0x3F;
                *pu8System = (_u16IrRc5Data&0x3C0)>>6;
                _u16PreIrRc5Data=_u16IrRc5Data;
                _u8IrRc5Bits=0;
                bRet = TRUE;
            }
        }

        if(bRet)
        {
            if ( (_u8PrevKeyCode != *pu8Key) || (!*pu8Flag) )
            {
                _ePrevKeyProperty = E_IR_KEY_PROPERTY_INIT;
            }

            i = _MDrv_IR_GetSystemTime();
            if( _ePrevKeyProperty == E_IR_KEY_PROPERTY_INIT)
            {
                _u8PrevKeyCode     = *pu8Key;
                _ulPrevKeyTime    = i;
                _ePrevKeyProperty  = E_IR_KEY_PROPERTY_1st;
            }
            else if(_ePrevKeyProperty == E_IR_KEY_PROPERTY_1st)
            {
                if( (i - _ulPrevKeyTime) > _u32_1stDelayTimeMs)
                {
                    _ulPrevKeyTime = i;
                    _ePrevKeyProperty  = E_IR_KEY_PROPERTY_FOLLOWING;
                }
                else
                {
                    bRet = FALSE;
                }
            }
            else //E_IR_KEY_PROPERTY_FOLLOWING
            {
                if( (i - _ulPrevKeyTime) > _u32_2ndDelayTimeMs)
                {
                    _ulPrevKeyTime = i;
                }
                else
                {
                    bRet = FALSE;
                }
            }
        }

        return bRet;

    #else

    for( u8Byte=0; u8Byte<IR_RAW_DATA_NUM; u8Byte++)
    {
       u8IRSwModeBuf[u8Byte] = 0;
    }
	#if (IR_TYPE_SEL == IR_TYPE_HISENSE)
	if(_u8IRRepeated)//repeate key
	{
		_u8IRRepeated=0;
		_u8IRHeadReceived=0;//clear head receive flag
        *pu8Key = _u8PrevKeyCode;
        *pu8Flag = 1;
        bRet = TRUE;
		if(_u8IRType)
		*pu8Flag |=FCTORY_HEADER_RECEIVE;
        goto done;
	}
    if (_u32IRCount<(2+IR_RAW_DATA_NUM*8))
        return FALSE; //not complete yet
	#else
    if (_u32IRCount< 3+IR_RAW_DATA_NUM*8)
        return FALSE; //not complete yet
	#endif
    DEBUG_IR(printk("_u32IRCount=%d", _u32IRCount));
    for( u8Byte=0; u8Byte<_u32IRCount; u8Byte++)
    {
       DEBUG_IR(printk(" %d", _u32IRData[u8Byte]));
    }

    if( _u32IRData[0] > gstIRTimeSet.u16HdcLob && _u32IRData[1] > (gstIRTimeSet.u16OfcLob+gstIRTimeSet.u16Lg01HLob) && _u32IRData[1] < (gstIRTimeSet.u16OfcUpb+gstIRTimeSet.u16Lg01HUpb))
    {
        pu32IRData = &_u32IRData[2];
        DEBUG_IR(printk(" H1 "));
    }
    else if( _u32IRData[1] > gstIRTimeSet.u16HdcLob && _u32IRData[2] > (gstIRTimeSet.u16OfcLob+gstIRTimeSet.u16Lg01HLob) && _u32IRData[2] < (gstIRTimeSet.u16OfcUpb+gstIRTimeSet.u16Lg01HUpb) )
    {
        pu32IRData = &_u32IRData[3];
        DEBUG_IR(printk(" H2 "));
    }
    else
    {
        DEBUG_IR(printk(" invalid leader code\n"));
        bRet = FALSE;
        goto done;
    }

    for( u8Byte=0; u8Byte<IR_RAW_DATA_NUM; u8Byte++)
    {
        for( u8Bit=0; u8Bit<8; u8Bit++)
        {
            u32 u32BitLen = pu32IRData[u8Byte*8+u8Bit];
            u8IRSwModeBuf[u8Byte] >>= 1;

            if( u32BitLen > gstIRTimeSet.u16Lg0Lob && u32BitLen < gstIRTimeSet.u16Lg0Upb ) //0
            {
                u8IRSwModeBuf[u8Byte] |= 0x00;
            }
            else if (u32BitLen > gstIRTimeSet.u16Lg1Lob && u32BitLen < gstIRTimeSet.u16Lg1Upb) //1
            {
                u8IRSwModeBuf[u8Byte] |= 0x80;
            }
            else
            {
                DEBUG_IR(printk(" invalid waveform,0x%x\n",u32BitLen));
                bRet = FALSE;
                goto done;
            }
        }
    }

    if(u8IRSwModeBuf[0] == gstIRInitParam.u8HdrCode0)
    {
        if(u8IRSwModeBuf[1] == gstIRInitParam.u8HdrCode1)
        {
            if(u8IRSwModeBuf[2] == (u8)~u8IRSwModeBuf[3])
            {
                *pu8Key = u8IRSwModeBuf[2];
                *pu8Flag = 0;
                bRet = TRUE;
				#if (IR_TYPE_SEL == IR_TYPE_HISENSE)
                _u8PrevKeyCode=*pu8Key;
				_u8IRRepeateDetect=1;
				_u8IRHeadReceived=0;
				_u8IRType=0;
				#endif

                goto done;
            }
        }
    }
	#if (IR_TYPE_SEL == IR_TYPE_HISENSE)
	if(u8IRSwModeBuf[0] == IR_FHEADER_CODE0)
	{
		if(u8IRSwModeBuf[1] == IR_FHEADER_CODE1)
        {
            if(u8IRSwModeBuf[2] == (u8)~u8IRSwModeBuf[3])
            {
                *pu8Key = u8IRSwModeBuf[2];
                *pu8Flag = 0;
				*pu8Flag |=FCTORY_HEADER_RECEIVE;
                bRet = TRUE;
                _u8PrevKeyCode=*pu8Key;
				_u8IRRepeateDetect=1;
				_u8IRHeadReceived=0;
				_u8IRType=1;
                goto done;
            }
        }
    }
	#endif
    DEBUG_IR(printk(" invalid data\n"));
    bRet = FALSE;

done:
    _u32IRCount = 0;
    return bRet;
    #endif
}

static BOOL _MDrv_IR_GetKeyMultiple(U8 *pu8Key, U8 *pu8System, U8 *pu8Flag)
{
    BOOL bRet;

    if(gstIRInitParam.u8DecMode == IR_DECMODE_HWFULL)
    {
        bRet = _MDrv_IR_GetKey_FULL(pu8Key,pu8System,pu8Flag);
    }
    else if(gstIRInitParam.u8DecMode == IR_DECMODE_HWRAW)
    {
        bRet = _MDrv_IR_GetKey_RAW(pu8Key,pu8System,pu8Flag);
    }
    else if(gstIRInitParam.u8DecMode == IR_DECMODE_HWRC5||gstIRInitParam.u8DecMode == IR_DECMODE_HWRC5X||gstIRInitParam.u8DecMode == IR_DECMODE_HWRC6)
    {
        bRet = _MDrv_IR_GetKey_RC(pu8Key,pu8System,pu8Flag);
    }
    else if(gstIRInitParam.u8DecMode == IR_DECMODE_SW)
    {
        bRet = _MDrv_IR_GetKey_SW(pu8Key,pu8System,pu8Flag);
    }
    else if(gstIRInitParam.u8DecMode == IR_DECMODE_SHOT)//--@@@--IR Pulse Shot Mode
    {
        *pu8System = 0; *pu8Flag = 0;
        bRet = _MDrv_IR_GetPulseShot(&gu32IRPulseShot, pu8Key);//return key with shot mode(INT(0x01) or BUF(0x02))
    }
    else 
    {
        *pu8Key = 0xFF; *pu8System = 0; *pu8Flag = 0;
        bRet = FALSE;
    }
    return bRet;
}


//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
/// Initialize IR timing and enable IR.
/// @return None
//-------------------------------------------------------------------------------------------------
void MDrv_IR_Init(void)
{
	int result;

    //reset global variables
    _u8IRRawModeCount = 0;
    _u32IRCount = 0;
    _u32_1stDelayTimeMs = 0;
    _u32_2ndDelayTimeMs = 0;
    _ePrevKeyProperty = E_IR_KEY_PROPERTY_INIT;
    memset(&_KeyReceived, 0 , sizeof(_KeyReceived) );
    init_MUTEX(&gstIRSem);//--@@@--IR Pulse Shot Mode
    init_MUTEX(&fantasy_protocol.sem);
    init_waitqueue_head(&fantasy_protocol.inq);
    fantasy_protocol.data=0;

    //request interrupt and hook ISR
    result = request_irq(E_FIQ_IR, _MDrv_IR_ISR, SA_INTERRUPT, "IR", NULL);
    if (result)
    {
        IR_PRINT("IR IRQ registartion ERROR\n");
	}
	else
    {
        IR_PRINT("IR IRQ registartion OK\n");
    }

    #if (defined(CONFIG_MSTAR_TITANIA)||defined(CONFIG_MSTAR_TITANIA2))

    #else
    // unmask IR IRQ on PM
    REG(REG_IRQ_MASK_IR) &= IRQ_UNMASK_IR;
    #endif
    //enable_irq(E_FIQ_IR);
}
EXPORT_SYMBOL(MDrv_IR_Init);

//-------------------------------------------------------------------------------------------------
/// Set the initialized control parameters for IrDa at BOOT stage.
/// @param  pIRInitCfg \b IN: carry with initialized control parameters.
/// @return TRUE/FALSE
//-------------------------------------------------------------------------------------------------
BOOL MDrv_IR_InitCfg(MS_IR_InitCfg* pIRInitCfg)
{
    if(!pIRInitCfg)
    {
        return FALSE;
    }

    if (pIRInitCfg->u8DecMode == IR_DECMODE_EXT)
    {
        return FALSE;//not support this mode
    }

    //backup parameter
    memcpy((void*)&gstIRInitParam,(void*)pIRInitCfg,sizeof(MS_IR_InitCfg));
    
    if(gstIRInitParam.u8DecMode == IR_DECMODE_HWRAW)
    {
        MHal_IR_REG(REG_IR_CTRL0) = gstIRInitParam.u8Ctrl0; //0x93
        MHal_IR_REG(REG_IR_CTRL1) = gstIRInitParam.u8Ctrl1; //0x01
    }
    else
    if(gstIRInitParam.u8DecMode == IR_DECMODE_HWFULL)
    {
        MHal_IR_REG(REG_IR_CTRL0) = gstIRInitParam.u8Ctrl0; //0xBF
        MHal_IR_REG(REG_IR_CTRL1) = gstIRInitParam.u8Ctrl1; //0x01
    }
    //configure Header(customer) code
    MHal_IR_REG(REG_IR_CCODE_L) = gstIRInitParam.u8HdrCode0;
    MHal_IR_REG(REG_IR_CCODE_H) = gstIRInitParam.u8HdrCode1;
    //configure fifo parameters
    HAL_IR_CfgSetControl(REG_IR_FIFO_SHOT_CTRL, 0x07, IR_FIFO_DEPTH);
    HAL_IR_CfgSetControl(REG_IR_FIFO_SHOT_CTRL, IR_FIFO_EN, IR_FIFO_EN);
    //configure glitch remove parameters
    MHal_IR_REG(REG_IR_GLHRM_NUM_L) = (gstIRInitParam.u16GlhrmNum & IR_GLHRM_NUM_L_MSK);
    MHal_IR_REG(REG_IR_GLHRM_NUM_H) = ((gstIRInitParam.u16GlhrmNum>>8) & IR_GLHRM_NUM_L_MSK);
    MHal_IR_REG(REG_IR_GLHRM_NUM_H) |= IR_GLHRM_EN;
    //configure code bits and customer code bytes
    #if defined(CONFIG_MSTAR_TITANIA)||defined(CONFIG_MSTAR_TITANIA2)||defined(CONFIG_MSTAR_EUCLID)||defined(CONFIG_MSTAR_TITANIA3) || defined(CONFIG_MSTAR_TITANIA10) 
    HAL_IR_CfgSetControl(REG_IR_CBIT_CCBYTE, (gstIRInitParam.u8CodeBits/8), IR_CODE_BIT);
    HAL_IR_CfgSetControl(REG_IR_CBIT_CCBYTE, ((gstIRInitParam.u8CCodeBytes-1)<<4), IR_CCODE_BYTE);
    #else
    HAL_IR_CfgSetControl(REG_IR_CBIT_CCBYTE, (gstIRInitParam.u8CodeBits-1), IR_CODE_BIT);
    HAL_IR_CfgSetControl(REG_IR_CBIT_CCBYTE, ((gstIRInitParam.u8CCodeBytes-1)<<7), IR_CCODE_BYTE);
    #endif

    //configure decode mode & (wakeup) key select
    if(gstIRInitParam.u8DecMode == IR_DECMODE_HWFULL)
    {
        HAL_IR_CfgSetControl(REG_IR_DECODE_MODE, IR_DECODE_FULL, IR_DECODE_MSK);
        HAL_IR_CfgSetControl(REG_IR_STATUS_CTRL, ((gstIRInitParam.u8KeySelect-1)<<4), IR_WKUP_KEY_SEL); //wakeup key sel
        //configure HW extention(iformat)
        #if defined(CONFIG_MSTAR_TITANIA)||defined(CONFIG_MSTAR_TITANIA2)||defined(CONFIG_MSTAR_EUCLID)||defined(CONFIG_MSTAR_TITANIA3) || defined(CONFIG_MSTAR_TITANIA10)
        #else
        HAL_IR_CfgSetControl(REG_IR_IFORMAT_SEL, gstIRInitParam.u8ExtFormat, IR_IFORMAT_SEL);
        #endif
    }
    else
    if (gstIRInitParam.u8DecMode == IR_DECMODE_HWRAW)
    {
        HAL_IR_CfgSetControl(REG_IR_DECODE_MODE, IR_DECODE_RAW, IR_DECODE_MSK);
        HAL_IR_CfgSetControl(REG_IR_STATUS_CTRL, ((gstIRInitParam.u8KeySelect-1)<<4), IR_WKUP_KEY_SEL); //wakeup key sel
    }
    else
    if (gstIRInitParam.u8DecMode == IR_DECMODE_HWRC5||gstIRInitParam.u8DecMode == IR_DECMODE_HWRC5X||gstIRInitParam.u8DecMode == IR_DECMODE_HWRC6)
    {
        //normal configuration
        HAL_IR_CfgSetControl(REG_IR_RC_CTRL0,0x00,0xFF);
        HAL_IR_CfgSetControl(REG_IR_RC_CTRL1,0x00,0xFF);
        //config RC mode
        if(gstIRInitParam.u8DecMode == IR_DECMODE_HWRC5)//RC5
        {
            HAL_IR_RC_CfgDecMode(HAL_IR_RC_RC5);
            HAL_IR_RC_CfgAutoConfig(DISABLE);
        }
        if(gstIRInitParam.u8DecMode == IR_DECMODE_HWRC5X)//RC5X
        {
            HAL_IR_RC_CfgDecMode(HAL_IR_RC_RC5X);
            HAL_IR_RC_CfgAutoConfig(DISABLE);
        }
        if(gstIRInitParam.u8DecMode == IR_DECMODE_HWRC6)//RC6
        {
            HAL_IR_RC_CfgDecMode(HAL_IR_RC_RC6);
            HAL_IR_RC_CfgAutoConfig(ENABLE);
            HAL_IR_RC_CfgEcoFunc(ENABLE);
        }
        //HAL_IR_RC_CfgInputInvert(ENABLE);
        //HAL_IR_CfgIntrMask(ENABLE);//enable mask
        //HAL_IR_CfgHwDecode(DISABLE);//disable Full/Raw decode
    }
    else
    {
        HAL_IR_CfgSetControl(REG_IR_DECODE_MODE, IR_DECODE_SW, IR_DECODE_MSK);
        //configure shot selection for SW decoder 
        HAL_IR_CfgSetControl(REG_IR_FIFO_SHOT_CTRL, (gstIRInitParam.enShotSel<<4), IR_SHOT_SEL_MSK);
        //configure the polarity for input IR signal 
        HAL_IR_CfgSetControl(REG_IR_CTRL0, (gstIRInitParam.bInvertPolar<<7), IR_INV);
    }

    if(gstIRInitParam.u8DecMode == IR_DECMODE_HWRAW||gstIRInitParam.u8DecMode == IR_DECMODE_HWFULL)
    {
        // Empty the FIFO
        _MDrv_IR_ClearFIFO();
    }
    return TRUE;

}
EXPORT_SYMBOL(MDrv_IR_InitCfg);

//-------------------------------------------------------------------------------------------------
/// Set the initialized time parameters for IrDa at BOOT stage.
/// @param  pIRTimeCfg \b IN: carry with initialized time parameter.
/// @return TRUE/FALSE
//-------------------------------------------------------------------------------------------------
BOOL MDrv_IR_TimeCfg(MS_IR_TimeCfg* pIRTimeCfg)
{
    #define _IRGetCnt(irclk_mhz, time, tolerance) ((time*(100UL+tolerance)*irclk_mhz) / (irclk_mhz+1) / 100)
    #define U16UPBND   _IRGetCnt(u8Clk_mhz,irTimeBnd.s16Time, irTimeBnd.s8UpBnd)
    #define U16LOBND   _IRGetCnt(u8Clk_mhz,irTimeBnd.s16Time, irTimeBnd.s8LoBnd)
    U8 u8Clk_mhz;
    MS_IR_TimeBnd irTimeBnd;
    U16 u16RCBitTime;
    U32 u32TimeoutCyc;

    if(!pIRTimeCfg)
    {
        return FALSE;
    }

    //backup parameter
    memcpy((void*)&gstIRTimeParam,(void*)pIRTimeCfg,sizeof(MS_IR_TimeCfg));

    //get back IR clock
    u8Clk_mhz = gstIRInitParam.u8Clk_mhz;

    if (gstIRInitParam.u8DecMode == IR_DECMODE_HWRC5||gstIRInitParam.u8DecMode == IR_DECMODE_HWRC5X||gstIRInitParam.u8DecMode == IR_DECMODE_HWRC6)
    {
        //configure RC timing
        u16RCBitTime = gstIRTimeParam.u16RCBitTime;
        HAL_IR_RC_CfgLgpsThr(u16RCBitTime);
        HAL_IR_RC_CfgIntgThr(u16RCBitTime);
        HAL_IR_RC_CfgWdogCount(u16RCBitTime);
        HAL_IR_RC_CfgLgpsMar(u16RCBitTime);
        HAL_IR_RC_CfgLdpsThr(u16RCBitTime);
        //config divider number
        HAL_IR_RC_CfgClkDiv(u8Clk_mhz);
    }
    else
    {
        // header code upper bound & lower bound
        irTimeBnd = gstIRTimeParam.tHdr;
        MHal_IR_REG(REG_IR_HDC_UPB_L) = (U8)(U16UPBND      & IR_HDC_UPB_L_MSK);
        MHal_IR_REG(REG_IR_HDC_UPB_H) = (U8)((U16UPBND>>8) & IR_HDC_UPB_H_MSK);
        MHal_IR_REG(REG_IR_HDC_LOB_L) = (U8)(U16LOBND      & IR_HDC_LOB_L_MSK);
        MHal_IR_REG(REG_IR_HDC_LOB_H) = (U8)((U16LOBND>>8) & IR_HDC_LOB_H_MSK);
        gstIRTimeSet.u16HdcUpb = U16UPBND;
        gstIRTimeSet.u16HdcLob = U16LOBND;

        // header code upper bound & lower bound
        irTimeBnd = gstIRTimeParam.tOff;
        MHal_IR_REG(REG_IR_OFC_UPB_L) = (U8)(U16UPBND      & IR_OFC_UPB_L_MSK);
        MHal_IR_REG(REG_IR_OFC_UPB_H) = (U8)((U16UPBND>>8) & IR_OFC_UPB_H_MSK);
        MHal_IR_REG(REG_IR_OFC_LOB_L) = (U8)(U16LOBND      & IR_OFC_LOB_L_MSK);
        MHal_IR_REG(REG_IR_OFC_LOB_H) = (U8)((U16LOBND>>8) & IR_OFC_LOB_H_MSK);
        gstIRTimeSet.u16OfcUpb = U16UPBND;
        gstIRTimeSet.u16OfcLob = U16LOBND;

        // off code repeat upper bound & lower bound
        irTimeBnd = gstIRTimeParam.tOffRpt;
        MHal_IR_REG(REG_IR_OFC_RP_UPB_L) = (U8)(U16UPBND      & IR_OFC_RP_UPB_L_MSK);
        MHal_IR_REG(REG_IR_OFC_RP_UPB_H) = (U8)((U16UPBND>>8) & IR_OFC_RP_UPB_H_MSK);
        MHal_IR_REG(REG_IR_OFC_RP_LOB_L) = (U8)(U16LOBND      & IR_OFC_RP_LOB_L_MSK);
        MHal_IR_REG(REG_IR_OFC_RP_LOB_H) = (U8)((U16LOBND>>8) & IR_OFC_RP_LOB_H_MSK);
        gstIRTimeSet.u16OfcRpUpb = U16UPBND;
        gstIRTimeSet.u16OfcRpLob = U16LOBND;

        // logical 0/1 high upper bound & lower bound
        irTimeBnd = gstIRTimeParam.tLg01Hg;
        MHal_IR_REG(REG_IR_LG01H_UPB_L) = (U8)(U16UPBND      & IR_LG01H_UPB_L_MSK);
        MHal_IR_REG(REG_IR_LG01H_UPB_H) = (U8)((U16UPBND>>8) & IR_LG01H_UPB_H_MSK);
        MHal_IR_REG(REG_IR_LG01H_LOB_L) = (U8)(U16LOBND      & IR_LG01H_LOB_L_MSK);
        MHal_IR_REG(REG_IR_LG01H_LOB_H) = (U8)((U16LOBND>>8) & IR_LG01H_LOB_H_MSK);
        gstIRTimeSet.u16Lg01HUpb = U16UPBND;
        gstIRTimeSet.u16Lg01HLob = U16LOBND;

        // logical 0 upper bound & lower bound
        irTimeBnd = gstIRTimeParam.tLg0;
        MHal_IR_REG(REG_IR_LG0_UPB_L) = (U8)(U16UPBND      & IR_LG0_UPB_L_MSK);
        MHal_IR_REG(REG_IR_LG0_UPB_H) = (U8)((U16UPBND>>8) & IR_LG0_UPB_H_MSK);
        MHal_IR_REG(REG_IR_LG0_LOB_L) = (U8)(U16LOBND      & IR_LG0_LOB_L_MSK);
        MHal_IR_REG(REG_IR_LG0_LOB_H) = (U8)((U16LOBND>>8) & IR_LG0_LOB_H_MSK);
        gstIRTimeSet.u16Lg0Upb = U16UPBND;
        gstIRTimeSet.u16Lg0Lob = U16LOBND;

        // logical 1 upper bound & lower bound
        irTimeBnd = gstIRTimeParam.tLg1;
        MHal_IR_REG(REG_IR_LG1_UPB_L) = (U8)(U16UPBND      & IR_LG1_UPB_L_MSK);
        MHal_IR_REG(REG_IR_LG1_UPB_H) = (U8)((U16UPBND>>8) & IR_LG1_UPB_H_MSK);
        MHal_IR_REG(REG_IR_LG1_LOB_L) = (U8)(U16LOBND      & IR_LG1_LOB_L_MSK);
        MHal_IR_REG(REG_IR_LG1_LOB_H) = (U8)((U16LOBND>>8) & IR_LG1_LOB_H_MSK);
        gstIRTimeSet.u16Lg1Upb = U16UPBND;
        gstIRTimeSet.u16Lg1Lob = U16LOBND;

        // Separate code width upper bound & lower bound
        irTimeBnd = gstIRTimeParam.tSepr;
        MHal_IR_REG(REG_IR_SEPR_UPB_L) = (U8)(U16UPBND      & IR_SEPR_UPB_L_MSK);
        MHal_IR_REG(REG_IR_SEPR_UPB_H) = (U8)((U16UPBND>>8) & IR_SEPR_UPB_H_MSK);
        MHal_IR_REG(REG_IR_SEPR_LOB_L) = (U8)(U16LOBND      & IR_SEPR_LOB_L_MSK);
        MHal_IR_REG(REG_IR_SEPR_LOB_H) = (U8)((U16LOBND>>8) & IR_SEPR_LOB_H_MSK);

        // timeout cycles
        u32TimeoutCyc = gstIRTimeParam.u32TimeoutCyc;
        MHal_IR_REG(REG_IR_TIMEOUT_CYC_L) = (U8)(u32TimeoutCyc & IR_TIMEOUT_CYC_L_MSK);
        MHal_IR_REG(REG_IR_TIMEOUT_CYC_H) = (U8)((u32TimeoutCyc >> 8) & IR_TIMEOUT_CYC_H_MSK);
        MHal_IR_REG(REG_IR_TIMEOUT_CYC_O) = (U8)((u32TimeoutCyc >>16) & IR_TIMEOUT_CYC_O_MSK);

        //config divider number
        MHal_IR_REG(REG_IR_CKDIV_NUM) = u8Clk_mhz; //XTal=12MHz, if counter set to 1M, clkdiv must be set to 12

        #if 0//This can be used for debugging later
        gstIRTimeParam.tTail.gu32KeyMin = NEC_TAIL_KEY_MIN;
        gstIRTimeParam.tTail.gu32KeyMax = NEC_TAIL_KEY_MAX;
        gstIRTimeParam.tTail.gu32RptMin = NEC_TAIL_RPT_MIN;
        gstIRTimeParam.tTail.gu32RptMax = NEC_TAIL_RPT_MAX;
        #endif
    }
    return TRUE;

}
EXPORT_SYMBOL(MDrv_IR_TimeCfg);

//-------------------------------------------------------------------------------------------------
/// Get the shot count information from ping-pong/ring buffer
/// @param  pstShotInfo \b OUT: Get the read index,length and shot count sequence.
/// @return TRUE/FALSE
//-------------------------------------------------------------------------------------------------
BOOL MDrv_IR_ReadShotBuffer(MS_IR_ShotInfo* pstShotInfo)//--@@@--IR Pulse Shot Mode
{
    if(down_trylock(&gstIRSem)==0)
    {
        memcpy(pstShotInfo,&gu32IRShot,sizeof(MS_IR_ShotInfo));
        //printk("[kernel] u8RdIdx = %d\n",pstShotInfo->u8RdIdx);
        //printk("[kernel] u8WtIdx = %d\n",pstShotInfo->u8WtIdx);
        //printk("[kernel] u32Length = %d\n",pstShotInfo->u32Length);
        up(&gstIRSem);
    }
    return TRUE;
}
EXPORT_SYMBOL(MDrv_IR_ReadShotBuffer);

//-------------------------------------------------------------------------------------------------
/// Set the IR delay time to recognize a valid key.
/// @param  u32_1stDelayTimeMs \b IN: Set the delay time to get the 1st key.
/// @param  u32_2ndDelayTimeMs \b IN: Set the delay time to get the following keys.
/// @return None
//-------------------------------------------------------------------------------------------------
void MDrv_IR_SetDelayTime(u32 u32_1stDelayTimeMs, u32 u32_2ndDelayTimeMs)
{
//TBD
    //u32 u32OldIntr;

    //u32OldIntr = MsOS_DisableAllInterrupts();

    _u32_1stDelayTimeMs = u32_1stDelayTimeMs;
    _u32_2ndDelayTimeMs = u32_2ndDelayTimeMs;

    //MsOS_RestoreAllInterrupts(u32OldIntr);
}
EXPORT_SYMBOL(MDrv_IR_SetDelayTime);
void MDrv_IR_SendKey(U8 u8Key, U8 u8RepeatFlag)
{
	#if (IR_TYPE_SEL == IR_TYPE_HISENSE)//for hisense need suport 2 ir head code ;
			U8 hisense_factory_ir=0;
			if(u8RepeatFlag&FCTORY_HEADER_RECEIVE)
			{
				u8RepeatFlag &= (~FCTORY_HEADER_RECEIVE);
	                     hisense_factory_ir=1;
			}
                     fantasy_protocol.data = (u8Key<<8 | u8RepeatFlag);

			if(hisense_factory_ir)//HISENSE_FACTORY IR
                    	    fantasy_protocol.data |= (0x04 << 28);
			else
	                   fantasy_protocol.data |= (0x01 << 28);
	#else //
//                    fantasy_protocol.data=u32InputData;
                    fantasy_protocol.data = (u8Key<<8 | u8RepeatFlag);

                    //set fantasy class to IR
                    fantasy_protocol.data |= (0x01 << 28);
	#endif
                wake_up_interruptible(&fantasy_protocol.inq);
}
EXPORT_SYMBOL(MDrv_IR_SendKey);

//-------------------------------------------------------------------------------------------------
/// Get IR key. It is a non-blocking function.
/// @param pu8Key  \b IN: Return IR key value.
/// @param pu8Flag \b IN: Return IR repeat code.
///
/// @return TRUE:  Success
/// @return FALSE: No key.
//-------------------------------------------------------------------------------------------------
BOOL MDrv_IR_GetKey(U8 *pu8Key, U8 *pu8System, U8 *pu8Flag)
{
    //eCos
    #if 0
    //u32 u32OldIntr;
    //MsOS_DiableAllInterrupts(u32OldIntr); //need ???

    //bRet = _MDrv_IR_GetKeyMultiple(pu8Key, pu8Flag);

    //MsOS_RestoreAllInterrupts(u32OldIntr);
    #endif

    if (_KeyReceived.u8Valid)
    {
        *pu8Key = _KeyReceived.u8Key;
        *pu8System = _KeyReceived.u8System;
        *pu8Flag = _KeyReceived.u8Flag ;

        //memset(&_KeyReceived, 0 , sizeof(_KeyReceived) );

        return(TRUE);
    }
    else
    {
        return(FALSE);
    }
}

//-------------------------------------------------------------------------------------------------
/// Get IR Code. It is a non-blocking function.
/// @param pu8Key  \b IN: Return IR key value.
/// @param pu8Flag \b IN: Return IR repeat code.
///
/// @return TRUE:  Success
/// @return FALSE: No key.
//-------------------------------------------------------------------------------------------------
BOOL MDrv_IR_GetCode(U8 *pu8Key, U8 *pu8Flag)
{
    if (_KeyReceived.u8Valid)
    {
        *pu8Key = _KeyReceived.u8Key;
        *pu8Flag = _KeyReceived.u8Flag ;
        _KeyReceived.u8Valid = 0;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
EXPORT_SYMBOL(MDrv_IR_GetCode);


//-------------------------------------------------------------------------------------------------
/// Return the time that IR key present. It is a non-blocking function.
/// @return Last key present time.
//-------------------------------------------------------------------------------------------------
unsigned long MDrv_IR_GetLastKeyTime(void)
{
    return _ulLastKeyPresentTime;
}


//-------------------------------------------------------------------------------------------------
/// Translate from IR key to internal key.
/// @param  u8Key  \b IN: IR key value.
/// @return translated internal IR key.
//-------------------------------------------------------------------------------------------------
U8 MDrv_IR_ParseKey(U8 u8Key)
{
    #if 1
    return u8Key;
    #else
    return _MDrv_IR_ParseKey(u8Key);
    #endif
}

void MDrv_IR_EnableIR(U8 bEnable)
{
    irqreturn_t isrStatus;
    bIRPass = !bEnable;
    if (bEnable)
    {
        isrStatus = request_irq(E_FIQ_IR, _MDrv_IR_ISR, SA_INTERRUPT, "IR", NULL);
        //enable_irq(E_FIQ_IR);
    }
    else
    {
        disable_irq(E_FIQ_IR);
        //free_irq(E_FIQ_IR, NULL);
    }
}

BOOL MDrv_IR_IsFantasyProtocolSupported(void)
{
    return MSTAR_FANTASY_PROTOCOL_SUPPORTED;
}

ssize_t MDrv_IR_Read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    if(MSTAR_FANTASY_PROTOCOL_SUPPORTED&&count==sizeof(U32))
    {
        if (down_interruptible(&fantasy_protocol.sem))
        {
            return -ERESTARTSYS;
        }

        while(fantasy_protocol.data==0)
        {
            up(&fantasy_protocol.sem); /* release the lock */
            if (filp->f_flags & O_NONBLOCK)
            {
                return -EAGAIN;
            }

            if (wait_event_interruptible(fantasy_protocol.inq, (fantasy_protocol.data!=0)))
            {
                return -ERESTARTSYS;
            }

            if (down_interruptible(&fantasy_protocol.sem))
            {
                return -ERESTARTSYS;
            }
        }

        if(copy_to_user(buf,&fantasy_protocol.data,sizeof(U32)))
        {
            up(&fantasy_protocol.sem);
            return -EFAULT;
        }

        //success
        fantasy_protocol.data=0;
        up(&fantasy_protocol.sem);

        return count;

    }
    else
    {

        return 0;
    }
}
EXPORT_SYMBOL(MDrv_IR_Read);

unsigned int MDrv_IR_Poll(struct file *filp, poll_table *wait)
{
    if (MSTAR_FANTASY_PROTOCOL_SUPPORTED)
    {
        unsigned int mask=0;

        down(&fantasy_protocol.sem);
        poll_wait(filp, &fantasy_protocol.inq, wait);
        if (fantasy_protocol.data != 0)
        {
            mask |= POLLIN|POLLRDNORM;
        }
        up(&fantasy_protocol.sem);

        return mask;
    }
    else
    {
        return 0;
    }
}
EXPORT_SYMBOL(MDrv_IR_Poll);

void MDrv_IR_SetMasterPid(pid_t pid)
{
    MasterPid = pid;
}

pid_t MDrv_IR_GetMasterPid(void)
{
    return MasterPid;
}

#if 1//(IR_MODE_SEL == IR_TYPE_HWDECODE_RC)
//#######################################
//#  For Hardware RC Decode
//#  IR RC Operation Frequency(<5M) :  1 ~ 2MHz
//#     ==> Based on 3/2=1.5 MHz
//#######################################
#define IR_RC5_BIT_TIME             1778 //us
#define IR_RC6_BIT_TIME             889 //us
#define IR_RC_CLK_DIV(ClkMhz)       (ClkMhz*2/3-1)
#define IR_RC_LGPS_THR(UnitTime)    ((UnitTime)*9/8)        //(UnitTime)*(3/2)*(3/4)
#define IR_RC_INTG_THR(UnitTime)    (((UnitTime)*3/4-7)/8) //((UnitTime)*(3/2)*(1/2)-7)/8
#define IR_RC_WDOG_CNT(UnitTime)    ((UnitTime)*3/2048)     //(UnitTime)*(3/2)/1024
#define IR_RC_TMOUT_CNT(UnitTime)   ((UnitTime)*3/4096)     //(UnitTime)*(3/2)/2048
#define IR_RC6_LDPS_THR(UnitTime)   ((UnitTime)-31)         //(UnitTime)*(3/2)*(2/3)
#define IR_RC6_LGPS_MAR(UnitTime)   ((UnitTime)*3/4)        //(UnitTime)*(3/2)*(1/2)

void HAL_IR_CfgSetControl(U32 u32Reg,U8 u8CtrlData,U8 u8Mask)
{
    MHal_IR_REG(u32Reg) = (MHal_IR_REG(u32Reg) & (~u8Mask)) | (u8CtrlData & u8Mask);
}

void HAL_IR_CfgHwDecode(U8 u8Enable)//for Full/Raw decode mode
{
    HAL_IR_CfgSetControl(REG_IR_CTRL0, ((u8Enable)? IR_EN : 0), IR_EN);
}

void HAL_IR_CfgIntrMask(U8 u8Enable)
{
    HAL_IR_CfgSetControl(REG_IR_CTRL0, ((u8Enable)? IR_INT_MASK : 0), IR_INT_MASK);
}

U32 HAL_IR_GetTimeInerval(void)
{
    U8 u8RCDecode;

    u8RCDecode = HAL_IR_RC_GetDecode();
    switch(u8RCDecode)
    {
        case HAL_IR_RC_RC5:
            return HAL_IR_RC5_INTERVAL;
        case HAL_IR_RC_RC6:
            return HAL_IR_RC6_INTERVAL;
        case HAL_IR_RC_DISABLE:
            return HAL_IR_NEC_INTERVAL;
        default:
            break;
    }
    return 0xFFFF;
}

U32 HAL_IR_GetTimeout(void)
{
    U8 u8RCDecode;

    u8RCDecode = HAL_IR_RC_GetDecode();
    switch(u8RCDecode)
    {
        case HAL_IR_RC_RC5:
            return HAL_IR_RC5_TIMEOUT;
        case HAL_IR_RC_RC6:
            return HAL_IR_RC6_TIMEOUT;
        case HAL_IR_RC_DISABLE:
            return HAL_IR_NEC_TIMEOUT;
        default:
            break;
    }
    return 0xFFFF;
}

void HAL_IR_CfgControl(U8 u8Index, U8 u8CtrlData,U8 u8Mask)
{
    //IR control
    switch(u8Index)
    {
        case HAL_IR_RC_CTRL_0:
            HAL_IR_CfgSetControl(REG_IR_RC_CTRL0,u8CtrlData,u8Mask);
            break;
        case HAL_IR_RC_CTRL_1:
            HAL_IR_CfgSetControl(REG_IR_RC_CTRL1,u8CtrlData,u8Mask);
            break;
        default:
            break;
    }
}

U8 HAL_IR_RC_GetDecode(void)
{
    U8 u8CtlVal,u8Decode;

    u8CtlVal = MHal_IR_REG(REG_IR_RC_CTRL1);
    if(!(u8CtlVal & _IR_RC_ENABLE))
        return HAL_IR_RC_DISABLE;
    if(u8CtlVal & _IR_RC_RC6_EN)
    {
        u8Decode = HAL_IR_RC_RC6;
    }
    else
    {
        if(u8CtlVal & _IR_RC_RC5X_EN)
            u8Decode = HAL_IR_RC_RC5X;
        else
            u8Decode = HAL_IR_RC_RC5;
    }
    return u8Decode;
}

void HAL_IR_RC_CfgClkDiv(U8 u8ClkMhz)
{
    HAL_IR_CfgSetControl(REG_IR_RC_CLK_DIV,IR_RC_CLK_DIV(u8ClkMhz),_IR_RC_CLK_DIV_MSK);
}

void HAL_IR_RC_CfgAutoConfig(U8 u8Enable)
{
    HAL_IR_CfgSetControl(REG_IR_RC_CTRL0, ((u8Enable)? _IR_RC_AUTOCONFIG : 0), _IR_RC_AUTOCONFIG);
}

void HAL_IR_RC_CfgFifoClear(U8 u8Enable)
{
    HAL_IR_CfgSetControl(REG_IR_RC_CTRL0, ((u8Enable)? _IR_RC_FIFO_CLR : 0), _IR_RC_FIFO_CLR);
}

void HAL_IR_RC_CfgFifoWFirst(U8 u8Enable)
{
    HAL_IR_CfgSetControl(REG_IR_RC_CTRL0, ((u8Enable)? _IR_RC_FIFO_WFST : 0), _IR_RC_FIFO_WFST);
}

void HAL_IR_RC_CfgDecMode(U8 u8DecMode)
{
    U8 u8CtrlData;

    switch(u8DecMode)
    {
        case HAL_IR_RC_RC5:
            u8CtrlData = (_IR_RC_ENABLE);
            break;
        case HAL_IR_RC_RC5X:
            u8CtrlData = (_IR_RC_RC5X_EN|_IR_RC_ENABLE);
            break;
        case HAL_IR_RC_RC6:
            u8CtrlData = (_IR_RC_RC6_EN|_IR_RC_ENABLE);
            break;
        case HAL_IR_RC_DISABLE:
        default:
            u8CtrlData = 0;
            break;
    }
    HAL_IR_CfgSetControl(REG_IR_RC_CTRL1, u8CtrlData, _IR_RC_MODE_MSK);
}

void HAL_IR_RC_CfgWakeup(U8 u8Enable)
{
    HAL_IR_CfgSetControl(REG_IR_RC_CTRL1, ((u8Enable)? _IR_RC_WKUP_EN : 0), _IR_RC_WKUP_EN);
}

void HAL_IR_RC_CfgOutputSelect(U8 u8OutputSel)
{
    HAL_IR_CfgSetControl(REG_IR_RC_CTRL1, u8OutputSel, _IR_RC_DGB_SEL);
}

void HAL_IR_RC_CfgInputInvert(U8 u8Enable)
{
    HAL_IR_CfgSetControl(REG_IR_RC_CTRL1, ((u8Enable)? _IR_RC_IN_INV : 0), _IR_RC_IN_INV);
}

void HAL_IR_RC_CfgLgpsThr(U16 u16BitTime)
{
    U16 u16RefTime,u16LongPulseThr;
    //U8 u8Value,u8LgpsThr;
    U8 u8RCDecode;

    //get rc decode type
    u8RCDecode = HAL_IR_RC_GetDecode();
    if(u8RCDecode==HAL_IR_RC_DISABLE)
        return;
    //setup unit time
    switch(u8RCDecode)
    {
        case HAL_IR_RC_RC5:
        case HAL_IR_RC_RC5X: u16RefTime = u16BitTime; break;
        case HAL_IR_RC_RC6: u16RefTime = u16BitTime; break;
        default: u16RefTime = 0; break;
    }
    //get config value
    u16LongPulseThr = (U16)IR_RC_LGPS_THR(u16RefTime);
    printk("[LGPS]: u16LongPulseThr = %04X\n",u16LongPulseThr);
    //configure
    //u8LgpsThr = (U8)(u16LongPulseThr & _IR_RC_LGPS_THR_L_MSK);
    //u8Value = MDrv_ReadByte(REG_IR_RC_LGPS_THR_L) | u8LgpsThr;
    //MDrv_WriteByte(REG_IR_RC_LGPS_THR_L, u8Value);
    HAL_IR_CfgSetControl(REG_IR_RC_LGPS_THR_L, (U8)u16LongPulseThr, _IR_RC_LGPS_THR_L_MSK);

    //u8LgpsThr = (U8)((u16LongPulseThr>>8) & _IR_RC_LGPS_THR_H_MSK);
    //u8Value = MDrv_ReadByte(REG_IR_RC_LGPS_THR_H) | u8LgpsThr;
    //MDrv_WriteByte(REG_IR_RC_LGPS_THR_H, u8Value);
    HAL_IR_CfgSetControl(REG_IR_RC_LGPS_THR_H, (U8)(u16LongPulseThr>>8), _IR_RC_LGPS_THR_H_MSK);
}

void HAL_IR_RC_CfgIntgThr(U16 u16BitTime)
{
    U16 u16RefTime,u16IntegrationThr;
    //U8 u8Value,u8IntgThr;
    U8 u8RCDecode;

    //get rc decode type
    u8RCDecode = HAL_IR_RC_GetDecode();
    if(u8RCDecode==HAL_IR_RC_DISABLE)
        return;
    //setup unit time
    switch(u8RCDecode)
    {
        case HAL_IR_RC_RC5:
        case HAL_IR_RC_RC5X: u16RefTime = (u16BitTime / 2); break;
        case HAL_IR_RC_RC6: u16RefTime = (u16BitTime / 2); break;
        default: u16RefTime = 0; break;
    }
    //get config value
    u16IntegrationThr = (U16)IR_RC_INTG_THR(u16RefTime);
    IR_PRINT("[INTG]: u16IntegrationThr = %04X\n",u16IntegrationThr);
    //configure
    //u8IntgThr = (U8)((u16IntegrationThr<<1) & _IR_RC_INTG_THR_MSK);
    //u8IntgThr = (U8)((u16IntegrationThr) & _IR_RC_INTG_THR_MSK);
    //u8Value = MDrv_ReadByte(REG_IR_RC_INTG_THR) | u8IntgThr;
    //MDrv_WriteByte(REG_IR_RC_INTG_THR, u8Value);
    HAL_IR_CfgSetControl(REG_IR_RC_INTG_THR, (U8)(u16IntegrationThr<<1), _IR_RC_INTG_THR_MSK);
}

void HAL_IR_RC_CfgWdogCount(U16 u16BitTime)
{
    U16 u16RefTime;
    U8 u8WatchDogCount;
    U8 u8RCDecode;

    //get rc decode type
    u8RCDecode = HAL_IR_RC_GetDecode();
    if(u8RCDecode==HAL_IR_RC_DISABLE)
        return;
    //setup unit time
    switch(u8RCDecode)
    {
        case HAL_IR_RC_RC5:
        case HAL_IR_RC_RC5X: u16RefTime = (u16BitTime * 14); break;
        case HAL_IR_RC_RC6: u16RefTime = (u16BitTime * 29); break;
        default: u16RefTime = 0; break;
    }
    //get config value
    u8WatchDogCount = (U8)IR_RC_WDOG_CNT(u16RefTime);
    IR_PRINT("[WDOG]: u8WatchDogCount = %04X\n",u8WatchDogCount);
    //configure
    MHal_IR_REG(REG_IR_RC_WDOG_COUNT) = u8WatchDogCount;
}

void HAL_IR_RC_CfgTimeoutCount(void)
{
    U16 u16RefTime;
    U8 u8TimeoutCount;

    //setup unit time
    u16RefTime = (U16)HAL_IR_GetTimeout();
    //get config value
    u8TimeoutCount = (U8)IR_RC_TMOUT_CNT(u16RefTime);
    //configure
    MHal_IR_REG(REG_IR_RC_TIMEOUT_COUNT) = u8TimeoutCount;
}

void HAL_IR_RC_CfgLgpsMar(U16 u16BitTime)//Only for RC6
{
    U16 u16RefTime,u16LongPulseMar;
    //U8 u8Value,u8LgpsMar;

    //get rc decode type
    if(HAL_IR_RC_GetDecode()!=HAL_IR_RC_RC6)
        return;
    //setup unit time
    u16RefTime = (u16BitTime / 2);
    //get config value
    u16LongPulseMar = (U16)IR_RC6_LGPS_MAR(u16RefTime);
    //configure
    //u8LgpsMar = (U8)(u16LongPulseMar & _IR_RC_RC6_LGPS_MAR_L_MSK);
    //u8Value = MDrv_ReadByte(REG_IR_RC_RC6_LGPS_MAR_L) | u8LgpsMar;
    //MDrv_WriteByte(REG_IR_RC_RC6_LGPS_MAR_L, u8Value);
    HAL_IR_CfgSetControl(REG_IR_RC_RC6_LGPS_MAR_L, (U8)u16LongPulseMar, _IR_RC_RC6_LGPS_MAR_L_MSK);

    //u8LgpsMar = (U8)((u16LongPulseMar>>8) & _IR_RC_RC6_LGPS_MAR_H_MSK);
    //u8Value = MDrv_ReadByte(REG_IR_RC_RC6_LGPS_MAR_H) | u8LgpsMar;
    //MDrv_WriteByte(REG_IR_RC_RC6_LGPS_MAR_H, u8Value);
    HAL_IR_CfgSetControl(REG_IR_RC_RC6_LGPS_MAR_H, (U8)(u16LongPulseMar>>8), _IR_RC_RC6_LGPS_MAR_H_MSK);
}

void HAL_IR_RC_CfgLdpsThr(U16 u16BitTime)//Only for RC6
{
    U16 u16RefTime,u16LeadPulseThr;
    //U8 u8Value,u8LdpsThr;

    //get rc decode type
    if(HAL_IR_RC_GetDecode()!=HAL_IR_RC_RC6)
        return;
    //setup unit time
    u16RefTime = (u16BitTime * 3);
    //get config value
    u16LeadPulseThr = (U16)IR_RC6_LDPS_THR(u16RefTime);
    //configure
    //u8LdpsThr = (U8)((u16LeadPulseThr>>5) & _IR_RC_RC6_LDPS_THR_L_MSK);
    //u8Value = MDrv_ReadByte(REG_IR_RC_RC6_LDPS_THR_L) | u8LdpsThr;
    //MDrv_WriteByte(REG_IR_RC_RC6_LDPS_THR_L, u8Value);
    HAL_IR_CfgSetControl(REG_IR_RC_RC6_LDPS_THR_L, (U8)(u16LeadPulseThr>>5), _IR_RC_RC6_LDPS_THR_L_MSK);

    //u8LdpsThr = (U8)((u16LeadPulseThr>>8) & _IR_RC_RC6_LDPS_THR_H_MSK);
    //u8Value = MDrv_ReadByte(REG_IR_RC_RC6_LDPS_THR_H) | u8LdpsThr;
    //MDrv_WriteByte(REG_IR_RC_RC6_LDPS_THR_H, u8Value);
    HAL_IR_CfgSetControl(REG_IR_RC_RC6_LDPS_THR_H, (U8)(u16LeadPulseThr>>8), _IR_RC_RC6_LDPS_THR_H_MSK);
}

void HAL_IR_RC_CfgEcoFunc(BOOL bEnable)//Only for RC6
{
    U8 u8Value;

    //get rc decode type
    if(HAL_IR_RC_GetDecode()!=HAL_IR_RC_RC6)
        return;
    if(bEnable)
        u8Value = MHal_IR_REG(REG_IR_RC_RC6_ECO_EN) | _IR_RC_RC6_ECO_EN;
    else
        u8Value = MHal_IR_REG(REG_IR_RC_RC6_ECO_EN) & (~_IR_RC_RC6_ECO_EN);
    MHal_IR_REG(REG_IR_RC_RC6_ECO_EN) = u8Value;
}

BOOL HAL_IR_RC_IsTimeout(void)
{
    return (MHal_IR_REG(REG_IR_RC_FIFO_STATUS1) & _IR_RC_TIMEOUT_FLAG)? TRUE : FALSE;
}

BOOL HAL_IR_RC_IsFIFOEmpty(void)
{
    return (MHal_IR_REG(REG_IR_RC_FIFO_STATUS1) & _IR_RC_FIFO_EMPTY)? TRUE : FALSE;
}

BOOL HAL_IR_RC_IsFIFOFull(void)
{
    return (MHal_IR_REG(REG_IR_RC_FIFO_STATUS1) & _IR_RC_FIFO_FULL)? TRUE : FALSE;
}

void HAL_IR_RC_ClearFIFO(void)
{
    HAL_IR_RC_KeyData IRRCKeyData;
    U8 i,u8Garbage;
    U8 u8FifoDepth=HAL_IR_RC_FIFO_DEPTH_MAX;

    for ( i=0; i<=u8FifoDepth; i++ )
    {
        if (HAL_IR_RC_IsFIFOEmpty())
        {
            break;
        }
        HAL_IR_RC_GetKeyData(&IRRCKeyData);
        u8Garbage = MHal_IR_REG(REG_IR_RC_FIFO_STATUS1);
    }
}

U8 HAL_IR_RC_GetWakeupKey(U8 u8KeyNum)
{
    switch(u8KeyNum)
    {
        case HAL_IR_RC_WKUP_KEY1:
            return MHal_IR_REG(REG_IR_RC_COMP_KEY1);
        case HAL_IR_RC_WKUP_KEY2:
            return MHal_IR_REG(REG_IR_RC_COMP_KEY2);
        default:
            break;
    }
    return 0xFF;
}

void HAL_IR_RC_GetKeyData(HAL_IR_RC_KeyData *pIRRCKeyData)
{
    U8 u8KeyAddr,u8KeyCmd,u8RC6KeyMisc;
    U8 u8RCDecode;

    //get rc decode type
    u8RCDecode = HAL_IR_RC_GetDecode();
    if(u8RCDecode==HAL_IR_RC_DISABLE)
    {
        pIRRCKeyData->u8Valid =0;
        return;
    }
    //get key address
    u8KeyAddr = MHal_IR_REG(REG_IR_RC_KEY_ADDR);
    u8KeyCmd = MHal_IR_REG(REG_IR_RC_KEY_CMD);
    u8RC6KeyMisc = MHal_IR_REG(REG_IR_RC_RC6_KEY_MISC);
    switch(u8RCDecode)
    {
        case HAL_IR_RC_RC5:
        case HAL_IR_RC_RC5X:
            //if(u8RCDecode==HAL_IR_RC_RC5)
            //    IR_PRINT("HAL_IR_RC_RC5\n");
            //else
            //    IR_PRINT("HAL_IR_RC_RC5X\n");
            pIRRCKeyData->u8Address = u8KeyAddr & _IR_RC_RC5_KEY_ADDR_MSK;
            pIRRCKeyData->u8Toggle = (u8KeyAddr & _IR_RC_RC5_KEY_ADDR_TGL)? 1:0;
            if(u8RCDecode==HAL_IR_RC_RC5)
                pIRRCKeyData->u8Command = u8KeyCmd & _IR_RC_RC5_KEY_CMD_MSK;
            else
                pIRRCKeyData->u8Command = u8KeyCmd & _IR_RC_RC5X_KEY_CMD_MSK;
            pIRRCKeyData->u8Repeat =  (u8KeyCmd & _IR_RC_RC5_KEY_CMD_RPT)? 1:0;
            pIRRCKeyData->u8Valid =1;
            break;
        case HAL_IR_RC_RC6:
            //IR_PRINT("HAL_IR_RC_RC6\n");
            pIRRCKeyData->u8Address = u8KeyAddr & _IR_RC_RC6_KEY_ADDR_MSK;
            pIRRCKeyData->u8Command = u8KeyCmd & _IR_RC_RC6_KEY_CMD_MSK;
            pIRRCKeyData->u8Toggle = (u8RC6KeyMisc & _IR_RC_RC6_KEY_MISC_TGL)? 1:0;
            pIRRCKeyData->u8Repeat =  (u8RC6KeyMisc & _IR_RC_RC6_KEY_MISC_RPT)? 1:0;
            pIRRCKeyData->u8RC6Mode =  u8RC6KeyMisc & _IR_RC_RC6_KEY_MISC_MOD_MSK;
            pIRRCKeyData->u8Valid =1;
            break;
        default:
            pIRRCKeyData->u8Valid =0;
            break;
    }
    //write rc read pulse gen
    MHal_IR_REG(REG_IR_RC_FIFO_READ_PULSE) |= _IR_RC_FIFO_RD_PULSE;

}
#endif
