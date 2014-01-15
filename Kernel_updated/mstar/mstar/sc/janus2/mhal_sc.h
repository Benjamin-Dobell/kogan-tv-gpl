////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2008 MStar Semiconductor, Inc.
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

#ifndef _MHAL_SC_H_
#define _MHAL_SC_H_

//-------------------------------------------------------------------------------------------------
//  Driver Capability
//-------------------------------------------------------------------------------------------------
#define SC_DEV_NUM                  2               // number of device
#define SC_IRQ                      E_IRQ_UART3 //E_IRQ_UART1
#define SC_IRQ2                     E_IRQ_UART4 //E_IRQ_UART2

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define SC_READ(id, addr)           (!(id) ? UART1_READ(addr) : UART2_READ(addr))
#define SC_WRITE(id, addr, val)     if (!(id)) { UART1_WRITE(addr, val);} else { UART2_WRITE(addr, val);}
#define SC_OR(id,addr, val)         if (!(id)) { UART1_OR(addr, val);   } else { UART2_OR(addr, val);   }
#define SC_AND(id,addr, val)        if (!(id)) { UART1_AND(addr, val);  } else { UART2_AND(addr, val);  }
#define SC_XOR(id,addr, val)        if (!(id)) { UART1_XOR(addr, val);  } else { UART2_XOR(addr, val);  }

#define SC_3M_UARTDIV(d)            (((d*9/5)>>1)+((d*9/5)&0x1))        // 3M/d=43.2M/(16*uart_div) => div=d*3/5
#define SC_4P5M_UARTDIV(d)          (((d*6/5)>>1)+((d*6/5)&0x1))        // 4.5M/d=43.2M/(16*uart_div) => div=d*3/5
#define SC_6M_UARTDIV(d)            (((d*9/10)>>1)+((d*9/10)&0x1))      // 6M/d=43.2M/(16*uart_div) => div=d*9/20


//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------

#endif // _MHAL_TEMP_H_

