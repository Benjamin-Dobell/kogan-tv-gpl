/***************************************************************************************************************
 *
 * FileName hal_card_regs.c
 *     @author jeremy.wang (2011/08/30)
 * Desc:
 * 	   For Dynamic IP Address Reading and Port Setting.
 * 	   We could get current REG/CIFC/CIFD Bank Address anytime!
 * 	   We use global varible to record current port setting of IP, then we could use it to decide reg postions.
 * 	   The goal is that we don't need to change HAL Level code. (But its h file code)
 *
 * 	   The limitations were listed as below:
 * 	   (1) This c file could not use project/cpu/icver/specific define option here, but its h file could.
 *
 ***************************************************************************************************************/

#include "hal_card_regs.h"

static volatile PortEmType gePort[2];


void Hal_CREG_SET_PORT(IPEmType eIP, PortEmType ePort)
{
	gePort[eIP] = ePort;
}

volatile PortEmType Hal_CREG_GET_PORT(IPEmType eIP)
{
	return gePort[eIP];
}

volatile U32_T Hal_CREG_GET_REG_BANK(IPEmType eIP, IPBankEmType eBANK)
{
	if(eIP == EV_IP_FCIE1)
	{
		switch((U8_T)eBANK)
		{
			case (U8_T)EV_REG_BANK:
				return A_FCIE1_0_BANK;
			case (U8_T)EV_CIFC_BANK:
				return A_FCIE1_1_BANK;
			case (U8_T)EV_CIFD_BANK:
				return A_FCIE1_2_BANK;
			default:
				return A_FCIE1_0_BANK;
		}
	}
	else if(eIP == EV_IP_FCIE2)
	{
		switch((U8_T)eBANK)
		{
			case (U8_T)EV_REG_BANK:
				return A_FCIE2_0_BANK;
			case (U8_T)EV_CIFC_BANK:
				return A_FCIE2_1_BANK;
			case (U8_T)EV_CIFD_BANK:
				return A_FCIE2_2_BANK;
			default:
				return A_FCIE2_0_BANK;
		}
	}

	return 0;

}



