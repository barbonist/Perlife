/*
 * Peltier_Module.c
 *
 *  Created on: 03/nov/2016
 *      Author: W15
 */

#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#include "Peltier_Module.h"
#include "Global.h"

#include "PELTIER_COMM.h"
#include "ASerialLdd3.h"
#include "PC_DEBUG_COMM.h"
#include "ASerialLdd2.h"

void peltierAssInit(void){
	iflagPeltierBusy = IFLAG_PELTIER_FREE;
	iflag_peltier_tx = IFLAG_IDLE;
	iflag_peltier_rx = IFLAG_IDLE;
	iflagPeltierMsg = IFLAG_IDLE;


	peltierCell.commandDataFloatToWrite = 0;
	peltierCell.commandDataIntToWrite = 0;
	peltierCell.commandRegId = REG_IDLE;

	peltierCell.myFanModeSel = 1;
	peltierCell.myNTCSteinCoeff_A = 0.0013;
	peltierCell.myNTCSteinCoeff_B = 0.000235;
	peltierCell.myNTCSteinCoeff_C = 0.00000008;
	peltierCell.myOnOffDeadBand = 0.5;
	peltierCell.myOnOffHyster = 0.5;
	peltierCell.myRegMode = 2;
	peltierCell.mySet = 20.0;
	peltierCell.myTCDeadBand = 2.0;
	peltierCell.myTCLimit = 95;
	peltierCell.myThrsldMainCurrHigh = 30;

	peltierCell.readAlwaysEnable = 0;

	timerCounterPeltier = 0;

}

void alwaysPeltierActuator(void){
	static unsigned char  myStatePos = 0;
	static unsigned char myCountWrite = 0;
	static char myDataIntPeltier[8];
	static int myDataIntActual = 0;
	static float myDataFloatActual = 0;
	static char myDataIeee754[8];
	static unsigned long myUnsignedLongActual;
	static char myStringFloat[8];
	static char dummy = 0;

	static union NumFloatUnion{
				uint32 ieee754NUmFormat;
				float numFormatFloat;
			} myNumFloatActual;


	//if(timerCounterPeltier >= 2){
		//timerCounterPeltier = 0;
	if(iflag_peltier_rx == IFLAG_IDLE){
		//iflag_peltier_rx = IFLAG_BUSY;
		switch(myStatePos){
		//FAN 1 MODE - READ
		case 0:
			PeltierAssSendCommand(READ_DATA_REGISTER_XX,REG_16_FAN1_MOD_SEL,0,"0");
			ptrMsgPeltierRx = &peltierDebug_rx_data[0];
			ptrPeltierCountRx = 0;
			break;
		//FAN 1 MODE - WRITE
		case 1:
			if(myDataIntActual != peltierCell.myFanModeSel)
			{
				PeltierAssSendCommand(WRITE_DATA_REGISTER_XX, REG_16_FAN1_MOD_SEL, peltierCell.myFanModeSel, 0);
				//myStatePos = 0;
			}
			else
				myStatePos = 2;
			break;
		//REG. MODE - READ
		case 2:
			PeltierAssSendCommand(READ_DATA_REGISTER_XX,REG_13_REGULATOR_MODE,0,"0");
			ptrMsgPeltierRx = &peltierDebug_rx_data[0];
			ptrPeltierCountRx = 0;
			break;
		//REG. MODE - WRITE
		case 3:
			if(myDataIntActual != peltierCell.myRegMode)
			{
				PeltierAssSendCommand(WRITE_DATA_REGISTER_XX, REG_13_REGULATOR_MODE, peltierCell.myRegMode, 0);
				//myStatePos = 2;
			}
			else
				myStatePos = 4;
			break;
		//NTC STEIN COEFF A - READ
		case 4:
			PeltierAssSendCommand(READ_FLOAT_FROM_REG_XX,REG_59_TEMP1_STEIN_COEFF_A,0,"0");
			ptrMsgPeltierRx = &peltierDebug_rx_data[0];
			ptrPeltierCountRx = 0;
			break;
		//NTC STEIN COEFF A - WRITE
		case 5:
			if(myNumFloatActual.numFormatFloat != peltierCell.myNTCSteinCoeff_A)
			{
				myNumFloatActual.numFormatFloat = peltierCell.myNTCSteinCoeff_A;
				sprintf(myStringFloat, "%2X%2X%2X%2X", 	(unsigned char)(myNumFloatActual.ieee754NUmFormat >> 24),
														(unsigned char)(myNumFloatActual.ieee754NUmFormat >> 16),
														(unsigned char)(myNumFloatActual.ieee754NUmFormat >> 8),
														(unsigned char)(myNumFloatActual.ieee754NUmFormat));
				for(char i=0; i<8; i++)
				{
					if(myStringFloat[i] == 0x20)
						myStringFloat[i] = 0x30;
				}
				PeltierAssSendCommand(WRITE_FLOAT_REG_XX, REG_59_TEMP1_STEIN_COEFF_A, 0, myStringFloat);
				//myStatePos = 4;
			}
			else
				myStatePos = 6;
			break;
		//NTC STEIN COEFF B - READ
		case 6:
			PeltierAssSendCommand(READ_FLOAT_FROM_REG_XX,REG_60_TEMP1_STEIN_COEFF_B,0,"0");
			ptrMsgPeltierRx = &peltierDebug_rx_data[0];
			ptrPeltierCountRx = 0;
			break;
		//NTC STEIN COEFF B - WRITE
		case 7:
			if(myNumFloatActual.numFormatFloat != peltierCell.myNTCSteinCoeff_B)
			{
				myNumFloatActual.numFormatFloat = peltierCell.myNTCSteinCoeff_B;
				sprintf(myStringFloat, "%2X%2X%2X%2X", 	(unsigned char)(myNumFloatActual.ieee754NUmFormat>>24),
														(unsigned char)(myNumFloatActual.ieee754NUmFormat>>16),
														(unsigned char)(myNumFloatActual.ieee754NUmFormat>>8),
														(unsigned char)(myNumFloatActual.ieee754NUmFormat));
				for(char i=0; i<8; i++)
				{
					if(myStringFloat[i] == 0x20)
						myStringFloat[i] = 0x30;
				}
				PeltierAssSendCommand(WRITE_FLOAT_REG_XX, REG_60_TEMP1_STEIN_COEFF_B, 0, myStringFloat);
				//myStatePos = 4;
			}
			else
				myStatePos = 8;
			break;
		//NTC STEIN COEFF C - READ
		case 8:
			PeltierAssSendCommand(READ_FLOAT_FROM_REG_XX,REG_61_TEMP1_STEIN_COEFF_C,0,"0");
			ptrMsgPeltierRx = &peltierDebug_rx_data[0];
			ptrPeltierCountRx = 0;
			break;
		//NTC STEIN COEFF C - WRITE
		case 9:
			if(myNumFloatActual.numFormatFloat != peltierCell.myNTCSteinCoeff_C)
			{
				myNumFloatActual.numFormatFloat = peltierCell.myNTCSteinCoeff_C;
				sprintf(myStringFloat, "%2X%2X%2X%2X", 	(unsigned char)(myNumFloatActual.ieee754NUmFormat>>24),
														(unsigned char)(myNumFloatActual.ieee754NUmFormat>>16),
														(unsigned char)(myNumFloatActual.ieee754NUmFormat>>8),
														(unsigned char)(myNumFloatActual.ieee754NUmFormat));
				for(char i=0; i<8; i++)
				{
					if(myStringFloat[i] == 0x20)
						myStringFloat[i] = 0x30;
				}
				PeltierAssSendCommand(WRITE_FLOAT_REG_XX, REG_61_TEMP1_STEIN_COEFF_C, 0, myStringFloat);
				//myStatePos = 4;
			}
			else
				myStatePos = 10;
			break;
		//ON OFF DEAD BAND - READ
		case 10:
			PeltierAssSendCommand(READ_FLOAT_FROM_REG_XX,REG_14_ON_OFF_DEAD_BAND,0,"0");
			ptrMsgPeltierRx = &peltierDebug_rx_data[0];
			ptrPeltierCountRx = 0;
			break;
		//ON OFF DEAD BAND - WRITE
		case 11:
			if(myNumFloatActual.numFormatFloat != peltierCell.myOnOffDeadBand)
			{
				myNumFloatActual.numFormatFloat = peltierCell.myOnOffDeadBand;
				sprintf(myStringFloat, "%2X%2X%2X%2X", 	(unsigned char)(myNumFloatActual.ieee754NUmFormat>>24),
														(unsigned char)(myNumFloatActual.ieee754NUmFormat>>16),
														(unsigned char)(myNumFloatActual.ieee754NUmFormat>>8),
														(unsigned char)(myNumFloatActual.ieee754NUmFormat));
				for(char i=0; i<8; i++)
				{
					if(myStringFloat[i] == 0x20)
						myStringFloat[i] = 0x30;
				}
				PeltierAssSendCommand(WRITE_FLOAT_REG_XX, REG_14_ON_OFF_DEAD_BAND, 0, myStringFloat);
				//myStatePos = 4;
			}
			else
				myStatePos = 12;
			break;
		//ON OFF HYST - READ
		case 12:
			PeltierAssSendCommand(READ_FLOAT_FROM_REG_XX,REG_15_ON_OFF_HYSTER,0,"0");
			ptrMsgPeltierRx = &peltierDebug_rx_data[0];
			ptrPeltierCountRx = 0;
			break;
		//ON OFF HYST - WRITE
		case 13:
			if(myNumFloatActual.numFormatFloat != peltierCell.myOnOffHyster)
			{
				myNumFloatActual.numFormatFloat = peltierCell.myOnOffHyster;
				sprintf(myStringFloat, "%2X%2X%2X%2X", 	(unsigned char)(myNumFloatActual.ieee754NUmFormat>>24),
														(unsigned char)(myNumFloatActual.ieee754NUmFormat>>16),
														(unsigned char)(myNumFloatActual.ieee754NUmFormat>>8),
														(unsigned char)(myNumFloatActual.ieee754NUmFormat));
				for(char i=0; i<8; i++)
				{
					if(myStringFloat[i] == 0x20)
						myStringFloat[i] = 0x30;
				}
				PeltierAssSendCommand(WRITE_FLOAT_REG_XX, REG_15_ON_OFF_HYSTER, 0, myStringFloat);
				//myStatePos = 4;
			}
			else
				myStatePos = 14;
			break;
		// SET TEMP - READ
		case 14:
			PeltierAssSendCommand(READ_FLOAT_FROM_REG_XX,REG_0_SET_POINT,0,"0");
			ptrMsgPeltierRx = &peltierDebug_rx_data[0];
			ptrPeltierCountRx = 0;
			break;
		//SET TEMP - WRITE
		case 15:
			if(myNumFloatActual.numFormatFloat != peltierCell.mySet)
			{
				myNumFloatActual.numFormatFloat = peltierCell.mySet;
				sprintf(myStringFloat, "%2X%2X%2X%2X", 	(unsigned char)(myNumFloatActual.ieee754NUmFormat>>24),
														(unsigned char)(myNumFloatActual.ieee754NUmFormat>>16),
														(unsigned char)(myNumFloatActual.ieee754NUmFormat>>8),
														(unsigned char)(myNumFloatActual.ieee754NUmFormat));
				for(char i=0; i<8; i++)
				{
					if(myStringFloat[i] == 0x20)
						myStringFloat[i] = 0x30;
				}
				PeltierAssSendCommand(WRITE_FLOAT_REG_XX, REG_0_SET_POINT, 0, myStringFloat);
				//myStatePos = 4;
			}
			else
				myStatePos = 16;
			break;
		//TC DEAD BAND - READ
		case 16:
			PeltierAssSendCommand(READ_FLOAT_FROM_REG_XX,REG_7_MAIN_TCDEADBAND,0,"0");
			ptrMsgPeltierRx = &peltierDebug_rx_data[0];
			ptrPeltierCountRx = 0;
			break;
		//TC DEAD BAND - WRITE
		case 17:
			if(myNumFloatActual.numFormatFloat != peltierCell.myTCDeadBand)
			{
				myNumFloatActual.numFormatFloat = peltierCell.myTCDeadBand;
				sprintf(myStringFloat, "%2X%2X%2X%2X", 	(unsigned char)(myNumFloatActual.ieee754NUmFormat>>24),
														(unsigned char)(myNumFloatActual.ieee754NUmFormat>>16),
														(unsigned char)(myNumFloatActual.ieee754NUmFormat>>8),
														(unsigned char)(myNumFloatActual.ieee754NUmFormat));
				for(char i=0; i<8; i++)
				{
					if(myStringFloat[i] == 0x20)
						myStringFloat[i] = 0x30;
				}
				PeltierAssSendCommand(WRITE_FLOAT_REG_XX, REG_7_MAIN_TCDEADBAND, 0, myStringFloat);
				//myStatePos = 4;
			}
			else
				myStatePos = 18;
			break;
		//TC LIMIT - READ
		case 18:
			PeltierAssSendCommand(READ_FLOAT_FROM_REG_XX,REG_6_MAIN_TCLIMIT,0,"0");
			ptrMsgPeltierRx = &peltierDebug_rx_data[0];
			ptrPeltierCountRx = 0;
			break;
		//TC LIMIT - WRITE
		case 19:
			if(myNumFloatActual.numFormatFloat != peltierCell.myTCLimit)
			{
				myNumFloatActual.numFormatFloat = peltierCell.myTCLimit;
				sprintf(myStringFloat, "%2X%2X%2X%2X", 	(unsigned char)(myNumFloatActual.ieee754NUmFormat>>24),
														(unsigned char)(myNumFloatActual.ieee754NUmFormat>>16),
														(unsigned char)(myNumFloatActual.ieee754NUmFormat>>8),
														(unsigned char)(myNumFloatActual.ieee754NUmFormat));
				for(char i=0; i<8; i++)
				{
					if(myStringFloat[i] == 0x20)
						myStringFloat[i] = 0x30;
				}
				PeltierAssSendCommand(WRITE_FLOAT_REG_XX, REG_6_MAIN_TCLIMIT, 0, myStringFloat);
				//myStatePos = 4;
			}
			else
				myStatePos = 20;
			break;
		//CURR. HIGH THRSHLD - READ
		case 20:
			PeltierAssSendCommand(READ_FLOAT_FROM_REG_XX,REG_47_ALARM_CURRENT_HIGH,0,"0");
			ptrMsgPeltierRx = &peltierDebug_rx_data[0];
			ptrPeltierCountRx = 0;
			break;
		//CURR. HIGH THRSHLD - WRITE
		case 21:
			if(myNumFloatActual.numFormatFloat != peltierCell.myThrsldMainCurrHigh)
			{
				myNumFloatActual.numFormatFloat = peltierCell.myThrsldMainCurrHigh;
				sprintf(myStringFloat, "%2X%2X%2X%2X", 	(unsigned char)(myNumFloatActual.ieee754NUmFormat>>24),
														(unsigned char)(myNumFloatActual.ieee754NUmFormat>>16),
														(unsigned char)(myNumFloatActual.ieee754NUmFormat>>8),
														(unsigned char)(myNumFloatActual.ieee754NUmFormat));
				for(char i=0; i<8; i++)
				{
					if(myStringFloat[i] == 0x20)
						myStringFloat[i] = 0x30;
				}
				PeltierAssSendCommand(WRITE_FLOAT_REG_XX, REG_47_ALARM_CURRENT_HIGH, 0, myStringFloat);
				//myStatePos = 4;
			}
			else
				myStatePos = 0;
			break;

		default:
			break;
		}
	}

	if(iflag_peltier_rx == IFLAG_PELTIER_RX){
		iflag_peltier_rx = IFLAG_IDLE;

		for(int i=0; i<8; i++)
		{
			myDataIntPeltier[i] = 0;
			myDataIeee754[i] = 0;
		}

		switch(myStatePos){

		case 0:
		case 2:
			myDataIntPeltier[0] = *ptrMsgDataPeltierInt;
			myDataIntActual = strtol(myDataIntPeltier,NULL,16);
			myStatePos = myStatePos + 1;
			myCountWrite = 0;
			break;

		case 1:
		case 3:
			myStatePos = myStatePos + 1;
			//myStatePos = 0;
			/*myCountWrite = myCountWrite + 1;
			if(myCountWrite >= 10) //scritture ripetute......vado avanti
			{
				myStatePos = myStatePos + 1;
			}*/
			break;

		case 4:
			myDataIeee754[0] = *ptrMsgDataieee754start;
			myDataIeee754[1] = *(ptrMsgDataieee754start+1);
			myDataIeee754[2] = *(ptrMsgDataieee754start+2);
			myDataIeee754[3] = *(ptrMsgDataieee754start+3);
			myDataIeee754[4] = *(ptrMsgDataieee754start+4);
			myDataIeee754[5] = *(ptrMsgDataieee754start+5);
			myDataIeee754[6] = *(ptrMsgDataieee754start+6);
			myDataIeee754[7] = *(ptrMsgDataieee754start+7);
			myUnsignedLongActual = strtoul(myDataIeee754,NULL,16);
			myNumFloatActual.ieee754NUmFormat = myUnsignedLongActual;
			myStatePos = myStatePos + 1;
			myCountWrite = 0;
			break;

		case 5:
			myStatePos = myStatePos + 1;
			break;

		case 6:
			myDataIeee754[0] = *ptrMsgDataieee754start;
			myDataIeee754[1] = *(ptrMsgDataieee754start+1);
			myDataIeee754[2] = *(ptrMsgDataieee754start+2);
			myDataIeee754[3] = *(ptrMsgDataieee754start+3);
			myDataIeee754[4] = *(ptrMsgDataieee754start+4);
			myDataIeee754[5] = *(ptrMsgDataieee754start+5);
			myDataIeee754[6] = *(ptrMsgDataieee754start+6);
			myDataIeee754[7] = *(ptrMsgDataieee754start+7);
			myUnsignedLongActual = strtoul(myDataIeee754,NULL,16);
			myNumFloatActual.ieee754NUmFormat = myUnsignedLongActual;
			myStatePos = myStatePos + 1;
			break;

		case 7:
			myStatePos = myStatePos + 1;
			break;

		case 8:
			myDataIeee754[0] = *ptrMsgDataieee754start;
			myDataIeee754[1] = *(ptrMsgDataieee754start+1);
			myDataIeee754[2] = *(ptrMsgDataieee754start+2);
			myDataIeee754[3] = *(ptrMsgDataieee754start+3);
			myDataIeee754[4] = *(ptrMsgDataieee754start+4);
			myDataIeee754[5] = *(ptrMsgDataieee754start+5);
			myDataIeee754[6] = *(ptrMsgDataieee754start+6);
			myDataIeee754[7] = *(ptrMsgDataieee754start+7);
			myUnsignedLongActual = strtoul(myDataIeee754,NULL,16);
			myNumFloatActual.ieee754NUmFormat = myUnsignedLongActual;
			myStatePos = myStatePos + 1;
			break;

		case 9:
			myStatePos = myStatePos + 1;
			break;

		case 10:
			myDataIeee754[0] = *ptrMsgDataieee754start;
			myDataIeee754[1] = *(ptrMsgDataieee754start+1);
			myDataIeee754[2] = *(ptrMsgDataieee754start+2);
			myDataIeee754[3] = *(ptrMsgDataieee754start+3);
			myDataIeee754[4] = *(ptrMsgDataieee754start+4);
			myDataIeee754[5] = *(ptrMsgDataieee754start+5);
			myDataIeee754[6] = *(ptrMsgDataieee754start+6);
			myDataIeee754[7] = *(ptrMsgDataieee754start+7);
			myUnsignedLongActual = strtoul(myDataIeee754,NULL,16);
			myNumFloatActual.ieee754NUmFormat = myUnsignedLongActual;
			myStatePos = myStatePos + 1;
			break;

		case 11:
			myStatePos = myStatePos + 1;
			break;

		case 12:
			myDataIeee754[0] = *ptrMsgDataieee754start;
			myDataIeee754[1] = *(ptrMsgDataieee754start+1);
			myDataIeee754[2] = *(ptrMsgDataieee754start+2);
			myDataIeee754[3] = *(ptrMsgDataieee754start+3);
			myDataIeee754[4] = *(ptrMsgDataieee754start+4);
			myDataIeee754[5] = *(ptrMsgDataieee754start+5);
			myDataIeee754[6] = *(ptrMsgDataieee754start+6);
			myDataIeee754[7] = *(ptrMsgDataieee754start+7);
			myUnsignedLongActual = strtoul(myDataIeee754,NULL,16);
			myNumFloatActual.ieee754NUmFormat = myUnsignedLongActual;
			myStatePos = myStatePos + 1;
			break;

		case 13:
			myStatePos = myStatePos + 1;
			break;

		case 14:
			myDataIeee754[0] = *ptrMsgDataieee754start;
			myDataIeee754[1] = *(ptrMsgDataieee754start+1);
			myDataIeee754[2] = *(ptrMsgDataieee754start+2);
			myDataIeee754[3] = *(ptrMsgDataieee754start+3);
			myDataIeee754[4] = *(ptrMsgDataieee754start+4);
			myDataIeee754[5] = *(ptrMsgDataieee754start+5);
			myDataIeee754[6] = *(ptrMsgDataieee754start+6);
			myDataIeee754[7] = *(ptrMsgDataieee754start+7);
			myUnsignedLongActual = strtoul(myDataIeee754,NULL,16);
			myNumFloatActual.ieee754NUmFormat = myUnsignedLongActual;
			myStatePos = myStatePos + 1;
			break;

		case 15:
			myStatePos = myStatePos + 1;
			break;

		case 16:
			myDataIeee754[0] = *ptrMsgDataieee754start;
			myDataIeee754[1] = *(ptrMsgDataieee754start+1);
			myDataIeee754[2] = *(ptrMsgDataieee754start+2);
			myDataIeee754[3] = *(ptrMsgDataieee754start+3);
			myDataIeee754[4] = *(ptrMsgDataieee754start+4);
			myDataIeee754[5] = *(ptrMsgDataieee754start+5);
			myDataIeee754[6] = *(ptrMsgDataieee754start+6);
			myDataIeee754[7] = *(ptrMsgDataieee754start+7);
			myUnsignedLongActual = strtoul(myDataIeee754,NULL,16);
			myNumFloatActual.ieee754NUmFormat = myUnsignedLongActual;
			myStatePos = myStatePos + 1;
			break;

		case 17:
			myStatePos = myStatePos + 1;
			break;

		case 18:
			myDataIeee754[0] = *ptrMsgDataieee754start;
			myDataIeee754[1] = *(ptrMsgDataieee754start+1);
			myDataIeee754[2] = *(ptrMsgDataieee754start+2);
			myDataIeee754[3] = *(ptrMsgDataieee754start+3);
			myDataIeee754[4] = *(ptrMsgDataieee754start+4);
			myDataIeee754[5] = *(ptrMsgDataieee754start+5);
			myDataIeee754[6] = *(ptrMsgDataieee754start+6);
			myDataIeee754[7] = *(ptrMsgDataieee754start+7);
			myUnsignedLongActual = strtoul(myDataIeee754,NULL,16);
			myNumFloatActual.ieee754NUmFormat = myUnsignedLongActual;
			myStatePos = myStatePos + 1;
			break;

		case 19:
			myStatePos = myStatePos + 1;
			break;

		case 20:
			myDataIeee754[0] = *ptrMsgDataieee754start;
			myDataIeee754[1] = *(ptrMsgDataieee754start+1);
			myDataIeee754[2] = *(ptrMsgDataieee754start+2);
			myDataIeee754[3] = *(ptrMsgDataieee754start+3);
			myDataIeee754[4] = *(ptrMsgDataieee754start+4);
			myDataIeee754[5] = *(ptrMsgDataieee754start+5);
			myDataIeee754[6] = *(ptrMsgDataieee754start+6);
			myDataIeee754[7] = *(ptrMsgDataieee754start+7);
			myUnsignedLongActual = strtoul(myDataIeee754,NULL,16);
			myNumFloatActual.ieee754NUmFormat = myUnsignedLongActual;
			myStatePos = myStatePos + 1;
			break;

		case 21:
			myStatePos = 0;
			break;


		default:
			break;
		}

		/*for(int i=0; i<40; i++){
			peltierDebug_rx_data[i] = 0;
		}*/
	}

}

void startPeltierActuator(void){
	PeltierAssSendCommand(START_FLAG,"0",0,"0");
}

void stopPeltierActuator(void){
	PeltierAssSendCommand(STOP_FLAG,"0",0,"0");
}

//void PeltierAssSendCommand(char command[], char registerId, int dataIntUser, float dataFloatUser)
void PeltierAssSendCommand(char command[], char registerId[], int dataIntUser, char dataFloatUser[8])
{
	unsigned char	startChar;
	unsigned char	stopChar;
	int				dataInt;			/* 2 byte */
	float			dataFloat;			/* 4 byte */
	char			stringInt[16];
	char			stringIntlenght;
	char			stringFloat[16];
	char 			* ptrDataFloatUser;
	char			stringFloatlenght;
	char			regIdFieldLenght;
	char			dataFloatDebug[16];

	static unsigned char	maskCommLenght_1 = 0x00;
	static unsigned char	maskCommLenght_2 = 0x00;
	static unsigned char	maskCommLenght_3 = 0x00;

	unsigned char msgPeltier[32];
	unsigned char msgLenght;

	for(int i = 0; i < 32; i++)
		msgPeltier[i] = 0x00;

	for(int i = 0; i < 16; i++)
	{
		stringInt[i] = 0x00;
		stringFloat[i] = 0x00;
	}

	/* START CHAR */
	startChar = 0x24;
	msgPeltier[0] = startChar;

	stopChar = 0x0D;

	sprintf(stringInt, "%d", dataIntUser);
	stringIntlenght = strlen(stringInt);

	// attenzione che %3.1 è 3 character width compreso il punto con una cifra di precisione dopo la virgola; forse sarebbe meglio prevedere %5.1
	//sprintf(stringFloat, "%3.1f", dataFloatUser);
	//stringFloatlenght = strlen(stringFloat);
	stringFloatlenght = strlen(dataFloatUser);
	//stringFloatlenght = 8;
	ptrDataFloatUser = &dataFloatUser[0];

	for(int i=0; i<10; i++)
		dataFloatDebug[i] = *(ptrDataFloatUser+i);

	regIdFieldLenght = strlen(registerId);

	if(strlen(command) == 1)
	{
		maskCommLenght_1 = ((strcmp(command,STOP_FLAG) == 0)) |					/* 0x01 */
						   ((strcmp(command,START_FLAG) == 0)<<1) |				/* 0x02 */
						   ((strcmp(command,SHOW_CURRENT_SW) == 0)<<2) |		/* 0x04 */
						   ((strcmp(command,SHOW_CURRENT_SW_INT) == 0)<<3) |	/* 0x08 */
						   ((strcmp(command,GET_STATUS_FLAG) == 0)<<4) |		/* 0x10 */
						   ((strcmp(command,BOOT_LOADER_MODE) == 0)<<5);		/* 0x20 */

		switch(maskCommLenght_1)
		{
		case 0x01:
			msgPeltier[1] = 'Q';
			break;

		case 0x02:
			msgPeltier[1] = 'W';
			break;

		case 0x04:
			msgPeltier[1] = 'V';
			break;

		case 0x08:
			msgPeltier[1] = 'v';
			break;

		case 0x10:
			msgPeltier[1] = 'S';
			break;

		case 0x20:
			msgPeltier[1] = 'B';
			break;

		default:
			break;
		}

		msgPeltier[2] = stopChar;
		msgLenght = 3;

	}
	else if(strlen(command) == 2)
	{
		//maskCommLenght_2
		maskCommLenght_2 = 	((strcmp(command,CLEAR_STATUS_FLAG) == 0)) |					/* 0x01 */
							((strcmp(command,WRITE_REG_VAL_TO_EEPROM) == 0)<<1) |			/* 0x02 */
							((strcmp(command,READ_REG_REGISTER) == 0)<<2) |					/* 0x04 */
							((strcmp(command,REBOOT) == 0)<<3) |							/* 0x08 */
							((strcmp(command,GET_BOARD_INFO_ID) == 0)<<4) |					/* 0x10 */
							((strcmp(command,DISPLAY_LOG_DATA) == 0)<<5) |					/* 0x20 */
							((strcmp(command,LOAD_LOG_EEPROM_TO_RAM) == 0)<<6) |			/* 0x40 */
							((strcmp(command,CLEAR_LOG_EEPROM) == 0)<<7);					/* 0x80 */

		switch(maskCommLenght_2)
				{
				case 0x01:
					msgPeltier[1] = 'S';
					msgPeltier[2] = 'C';
					break;

				case 0x02:
					msgPeltier[1] = 'R';
					msgPeltier[2] = 'W';
					break;

				case 0x04:
					msgPeltier[1] = 'R';
					msgPeltier[2] = 'R';
					break;

				case 0x08:
					msgPeltier[1] = 'B';
					msgPeltier[2] = 'C';
					break;

				case 0x10:
					msgPeltier[1] = 'L';
					msgPeltier[2] = 'I';
					break;

				case 0x20:
					msgPeltier[1] = 'L';
					msgPeltier[2] = 'D';
					break;

				case 0x40:
					msgPeltier[1] = 'L';
					msgPeltier[2] = 'L';
					break;

				case 0x80:
					msgPeltier[1] = 'L';
					msgPeltier[2] = 'C';
					break;

				default:
					break;
				}

			msgPeltier[3] = stopChar;
			msgLenght = 4;
	}
	else
	{
		//maskCommLenght_3
		maskCommLenght_3 =  ((strcmp(command,WRITE_DATA_REGISTER_XX) == 0)) |				/* 0x01 */
							((strcmp(command,READ_DATA_REGISTER_XX) == 0)<<2) |				/* 0x04 */
							((strcmp(command,WRITE_FLOAT_REG_XX) == 0)<<4) |				/* 0x10 */
							((strcmp(command,READ_FLOAT_FROM_REG_XX) == 0)<<6) |			/* 0x40 */
							((strcmp(command,LOG_CONTINUOUS) == 0)<<7);						/* 0x80 */

		switch(maskCommLenght_3)
						{
						case 0x01:
							msgPeltier[1] = 'R';
							for(char i=0; i<regIdFieldLenght; i++)
								msgPeltier[2+i] = registerId[i];
							msgPeltier[2+regIdFieldLenght] = '=';
							for(int i = 0; i < stringIntlenght; i++)
							{
								msgPeltier[2+regIdFieldLenght+1+i] = stringInt[i];
							}
							msgPeltier[2+regIdFieldLenght+1+stringIntlenght] = stopChar;
							msgLenght = 4+regIdFieldLenght+stringIntlenght;
							break;

						case 0x04:
							msgPeltier[1] = 'R';
							for(char i=0; i<regIdFieldLenght; i++)
								msgPeltier[2+i] = registerId[i];
							msgPeltier[2+regIdFieldLenght] = '?';
							msgPeltier[3+regIdFieldLenght] = stopChar;
							msgLenght = 4+regIdFieldLenght;
							break;

						case 0x10:
							msgPeltier[1] = 'R';
							msgPeltier[2] = 'N';
							for(char i=0; i<regIdFieldLenght; i++)
								msgPeltier[3+i] = registerId[i];
							msgPeltier[3+regIdFieldLenght] = '=';
							for(int i = 0; i < stringFloatlenght; i++)
							{
								//msgPeltier[5+i] = stringFloat[i];
								msgPeltier[3+regIdFieldLenght+1+i] = (*(ptrDataFloatUser+i));
							}
							msgPeltier[3+regIdFieldLenght+1+stringFloatlenght] = stopChar;
							msgLenght = 5+regIdFieldLenght+stringFloatlenght;
							break;

						case 0x40:
							msgPeltier[1] = 'R';
							msgPeltier[2] = 'N';
							for(char i=0; i<regIdFieldLenght; i++)
								msgPeltier[3+i] = registerId[i];
							msgPeltier[3+regIdFieldLenght] = '?';
							msgPeltier[4+regIdFieldLenght] = stopChar;
							msgLenght = 5+regIdFieldLenght;
							break;

						case 0x80:
							msgPeltier[1] = 'A';
							for(char i=0; i<regIdFieldLenght; i++)
								msgPeltier[2+i] = registerId[i];
							msgPeltier[2+regIdFieldLenght] = stopChar;
							msgLenght = 3+regIdFieldLenght;
							break;

						default:
							break;
						}
	}

	for(int i = 0; i<msgLenght; i++)
	{
		PELTIER_COMM_SendChar(msgPeltier[i]);

		//iflag_peltier_rx = IFLAG_IDLE;

		#ifdef	DEBUG_COMM_SBC
		//PC_DEBUG_COMM_SendChar(msgPeltier[i]);
		#endif

		if(i == (msgLenght-1))
			iflag_peltier_tx = IFLAG_PELTIER_TX;
		//else
			//iflag_peltier_tx = IFLAG_IDLE;
	}

}

void PeltierAssWaitReadyToRcv(void)
{

}

void PeltierAssReadResponse(void)
{

}
