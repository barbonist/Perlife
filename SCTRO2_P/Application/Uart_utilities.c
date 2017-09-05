/*
 * Uart_utilities.c
 *
 *  Created on: 19/lug/2016
 *      Author: W15
 */

#include "Uart_utilities.h"
#include "ModBusCommProt.h"
#include "Global.h"

#include "Pins1.h"
#include "LED_GREEN.h"
#include "BitIoLdd1.h"
#include "LED_BLUE.h"
#include "BitIoLdd2.h"
#include "LED_RED.h"
#include "BitIoLdd3.h"


unsigned char debug_guard_value;

void ManageCommActuator(void)
{
	if(iflag_pmp1_rx == IFLAG_PMP1_RX)
	{
		iflag_pmp1_rx = IFLAG_IDLE;
	}
}

void ManageCommPcDebug(void)
{
	if(iflag_pc_rx == IFLAG_PC_RX)
	{
		iflag_pc_rx = IFLAG_IDLE;
		ptrCount = 0;
		ptr = &pc_rx_data[0];

		#ifdef	DEBUG_ENABLE
		LED_BLUE_ClrVal();
		#endif

		switch(pc_rx_data[0])
		{

		/* guard */
		case 0xA0:
			if(pc_rx_data[1] == 0xA0)
			{
				if(pc_rx_data[1] == 0xA0)
				{
				switch(pc_rx_data[13])
					{
					 case 0x00:
					 debug_guard_value = GUARD_VALUE_NULL;
					 break;

					 case 0xA5:
					 debug_guard_value = GUARD_VALUE_TRUE;
					 break;

					 case 0x5A:
					 debug_guard_value = GUARD_VALUE_FALSE;
					 break;

					 default:
					 break;
					 }

				switch(pc_rx_data[12])
					 {
					 case 0x00:
					 currentGuard[GUARD_NULL].guardValue = debug_guard_value;
					 break;

					 case 0x01:
					 currentGuard[GUARD_START_ENABLE].guardValue = debug_guard_value;
					 break;

					 case 0x02:
					 currentGuard[GUARD_HW_T1T_DONE].guardValue = debug_guard_value;
					 break;

					 case 0x03:
					 currentGuard[GUARD_COMM_ENABLED].guardValue = debug_guard_value;
					 break;

					 case 0x04:
					 currentGuard[GUARD_THERAPY_SELECTED].guardValue = debug_guard_value;
					 break;

					 case 0x05:
					 currentGuard[GUARD_THERAPY_CONFIRMED].guardValue = debug_guard_value;
					 break;

					 case 0x06:
					 currentGuard[GUARD_T1_NO_DISP_END].guardValue = debug_guard_value;
					 break;

					 case 0x07:
					 currentGuard[GUARD_KIT_MOUNTED_CONFIRMED].guardValue = debug_guard_value;
					 break;

					 case 0x08:
					 currentGuard[GUARD_START_LEAK_CONFIRMED].guardValue = debug_guard_value;
					 break;

					 case 0x09:
					 currentGuard[GUARD_T1_WITH_DISP_END].guardValue = debug_guard_value;
					 break;

					 case 0x0A:
					 currentGuard[GUARD_START_PRIMING].guardValue = debug_guard_value;
					 break;

					 case 0x0B:
					 currentGuard[GUARD_TREAT_1_SELECTED].guardValue = debug_guard_value;
					 break;

					 case 0x0C:
					 currentGuard[GUARD_TREAT_2_SELECTED].guardValue = debug_guard_value;
					 break;

					 case 0x0D:
					 currentGuard[GUARD_PRIMING_END].guardValue = debug_guard_value;
					 break;

					 case 0x0E:
					 currentGuard[GUARD_START_TREAT_CONFIRMED].guardValue = debug_guard_value;
					 break;

					 case 0x0F:
					 currentGuard[GUARD_STOP_TREAT_SELECTED].guardValue = debug_guard_value;
					 break;

					 case 0x10:
					 currentGuard[GUARD_TREAT_1_END].guardValue = debug_guard_value;
					 break;

					 case 0x11:
					 currentGuard[GUARD_EMPTY_DISPOSABLE_END].guardValue = debug_guard_value;
					 break;

					 case 0x12:
					 currentGuard[GUARD_OPEN_COVER_CONFIRMED].guardValue = debug_guard_value;
					 break;

					 case 0x13:
					 currentGuard[GUARD_CLEAN_EQUIP_CONFIRMED].guardValue = debug_guard_value;
					 break;

					 case 0x14:
					 currentGuard[GUARD_FATAL_ERROR].guardValue = debug_guard_value;
					 break;

					 default:
						 break;
				}
			}





		}
		break;

		/* pump 1 */
		case 0x02:
		break;

		/* pump 2 */
		case 0x03:
		break;

		/* clamp 1 */
		case 0x07:
		break;

		/* clamp 2 */
		case 0x08:
		break;

		default:
			break;
	  }

    }
}

void ManageCommSBC(void)
{

}

void ManageCommC2P(void)
{

}




