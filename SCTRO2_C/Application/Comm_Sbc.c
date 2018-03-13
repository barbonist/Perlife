/*
 * Comm_Sbc.c
 *
 *  Created on: 10/nov/2016
 *      Author: W15
 */

#include "PE_Types.h"
#include "Global.h"
#include "Comm_Sbc.h"

#include "ModBusCommProt.h"
#include "App_Ges.h"

#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#include "SBC_COMM.h"
#include "ASerialLdd5.h"
#include "general_func.h"

extern word MedForArteriousPid;
extern word MedForVenousPid;

void buildModBusWriteRegActResponseMsg(char *ptrMsgSbcRx)
{
	byte index = 0;

	sbc_tx_data[index++] = 0xA5;
	sbc_tx_data[index++] = 0xAA;
	sbc_tx_data[index++] = 0x55;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = ptrMsgSbcRx[4]+1;
	sbc_tx_data[index++] = ptrMsgSbcRx[5];
	sbc_tx_data[index++] = ptrMsgSbcRx[6];
	sbc_tx_data[index++] = 0x66;

	for(int i = 0 ; i < ptrMsgSbcRx[4] ; i++)
	{
		sbc_tx_data[index++] = ptrMsgSbcRx[7+i];
	}

	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

void buildModBusReadRegActResponseMsg(char *ptrMsgSbcRx,
									  char slaveAddr,
									  unsigned int readStartAddr,
									  unsigned int numRegisterRead)
{
	byte index = 0;

	sbc_tx_data[index++] = 0xA5;
	sbc_tx_data[index++] = 0xAA;
	sbc_tx_data[index++] = 0x55;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = ptrMsgSbcRx[4]+1+2*numRegisterRead;
	sbc_tx_data[index++] = ptrMsgSbcRx[5];
	sbc_tx_data[index++] = ptrMsgSbcRx[6];
	sbc_tx_data[index++] = 0x66;

	for(int i = 0 ; i < ptrMsgSbcRx[4] ; i++)
	{
		sbc_tx_data[index++] = ptrMsgSbcRx[7+i];
	}

	for(int i = 0 ; i < numRegisterRead; i++)
	{
		sbc_tx_data[index++] = (modbusData[slaveAddr][readStartAddr+i] >> 8) & 0xFF;
		sbc_tx_data[index++] = (modbusData[slaveAddr][readStartAddr+i]     ) & 0xFF;
	}

	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

void buildModBusReadStatusResponseMsg(char *ptrMsgSbcRx)
{
	byte index = 0;

	sbc_tx_data[index++] = 0xA5;
	sbc_tx_data[index++] = 0xAA;
	sbc_tx_data[index++] = 0x55;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x25;
	sbc_tx_data[index++] = ptrMsgSbcRx[5];
	sbc_tx_data[index++] = ptrMsgSbcRx[6];
	sbc_tx_data[index++] = 0x66;

	for(int i = 0 ; i < 4 ; i++)
	{
		sbc_tx_data[index++] = (modbusData[i][0x0010] >> 8) & 0xFF;
		sbc_tx_data[index++] = (modbusData[i][0x0010]     ) & 0xFF;
		sbc_tx_data[index++] = (modbusData[i][0x0011] >> 8) & 0xFF;
		sbc_tx_data[index++] = (modbusData[i][0x0011]     ) & 0xFF;
		sbc_tx_data[index++] = (modbusData[i][0x0012] >> 8) & 0xFF;
		sbc_tx_data[index++] = (modbusData[i][0x0012]     ) & 0xFF;
	}
	for(int i = 4 ; i < 7 ; i++)
	{
		sbc_tx_data[index++] = (modbusData[i][0x0010] >> 8) & 0xFF;
		sbc_tx_data[index++] = (modbusData[i][0x0010]     ) & 0xFF;
		sbc_tx_data[index++] = (modbusData[i][0x0011] >> 8) & 0xFF;
		sbc_tx_data[index++] = (modbusData[i][0x0011]     ) & 0xFF;
	}

	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

void buildPressSensReadValuesResponseMsg(char *ptrMsgSbcRx)
{
    byte index = 0;

	sbc_tx_data[index++] = 0xA5;
	sbc_tx_data[index++] = 0xAA;
	sbc_tx_data[index++] = 0x55;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x16;
	sbc_tx_data[index++] = ptrMsgSbcRx[5];
	sbc_tx_data[index++] = ptrMsgSbcRx[6];
	sbc_tx_data[index++] = 0x66;
	sbc_tx_data[index++] = 0x00; //TODO stable byte

	sbc_tx_data[index++] = (PR_OXYG_mmHg_Filtered >> 8)  & 0xFF;
	sbc_tx_data[index++] = PR_OXYG_mmHg_Filtered  & 0xFF;
	sbc_tx_data[index++] = (PR_OXYG_ADC_Filtered >> 8)  & 0xFF;
	sbc_tx_data[index++] = PR_OXYG_ADC_Filtered  & 0xFF;

	sbc_tx_data[index++] = (PR_LEVEL_mmHg_Filtered >> 8)  & 0xFF;
	sbc_tx_data[index++] = PR_LEVEL_mmHg_Filtered & 0xFF;
	sbc_tx_data[index++] = (PR_LEVEL_ADC_Filtered >> 8)  & 0xFF;
	sbc_tx_data[index++] = PR_LEVEL_ADC_Filtered  & 0xFF;

	sbc_tx_data[index++] = (PR_ADS_FLT_mmHg_Filtered >> 8)  & 0xFF;
	sbc_tx_data[index++] = (PR_ADS_FLT_mmHg_Filtered)  & 0xFF;
	sbc_tx_data[index++] = (PR_ADS_FLT_ADC_Filtered >> 8)  & 0xFF;
	sbc_tx_data[index++] = PR_ADS_FLT_ADC_Filtered  & 0xFF;

	sbc_tx_data[index++] = (MedForVenousPid >> 8)  & 0xFF;
	sbc_tx_data[index++] = (MedForVenousPid)  & 0xFF;
	sbc_tx_data[index++] = (PR_VEN_ADC_Filtered >> 8)  & 0xFF;
	sbc_tx_data[index++] = PR_VEN_ADC_Filtered  & 0xFF;

	sbc_tx_data[index++] = (MedForArteriousPid >> 8)  & 0xFF;
	sbc_tx_data[index++] = MedForArteriousPid  & 0xFF;
	sbc_tx_data[index++] = (PR_ART_ADC_Filtered >> 8)  & 0xFF;
	sbc_tx_data[index++] = PR_ART_ADC_Filtered  & 0xFF;

	/*
	for(int i = 0 ; i < 5 ; i++)
	{
		sbc_tx_data[index++] = ((word)sensor_PRx[i].prSensValue) >> 8;
		sbc_tx_data[index++] = ((word)sensor_PRx[i].prSensValue);
		sbc_tx_data[index++] = sensor_PRx[i].prSensAdc >> 8;
		sbc_tx_data[index++] = sensor_PRx[i].prSensAdc;
	}
*/
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

void buildPressSensReadParamResponseMsg(char *ptrMsgSbcRx)
{
	union NumFloatGain{
				uint32 ieee754NumFormat_Gain;
				float numFormatFloat_Gain;
			} numFloatSensor_Gain;

	union NumFloatOffset{
				uint32 ieee754NumFormat_Offset;
				float numFormatFloat_Offset;
			} numFloatSensor_Offset;

    byte index = 0;

	sbc_tx_data[index++] = 0xA5;
	sbc_tx_data[index++] = 0xAA;
	sbc_tx_data[index++] = 0x55;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x29;
	sbc_tx_data[index++] = ptrMsgSbcRx[5];
	sbc_tx_data[index++] = ptrMsgSbcRx[6];
	sbc_tx_data[index++] = 0x66;

	for(int i = OXYG ; i < TOTAL_PPRESS_SENS ; i++)
	{
		numFloatSensor_Gain.numFormatFloat_Gain = config_data.sensor_PRx[i].prSensGain;

		sbc_tx_data[index++] = numFloatSensor_Gain.ieee754NumFormat_Gain >> 24;
		sbc_tx_data[index++] = numFloatSensor_Gain.ieee754NumFormat_Gain >> 16;
		sbc_tx_data[index++] = numFloatSensor_Gain.ieee754NumFormat_Gain >> 8;
		sbc_tx_data[index++] = numFloatSensor_Gain.ieee754NumFormat_Gain;

		numFloatSensor_Offset.numFormatFloat_Offset = config_data.sensor_PRx[i].prSensOffset;

		sbc_tx_data[index++] = numFloatSensor_Offset.ieee754NumFormat_Offset >> 24;
		sbc_tx_data[index++] = numFloatSensor_Offset.ieee754NumFormat_Offset >> 16;
		sbc_tx_data[index++] = numFloatSensor_Offset.ieee754NumFormat_Offset >> 8;
		sbc_tx_data[index++] = numFloatSensor_Offset.ieee754NumFormat_Offset;
	}

	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}


void buildPressSensCalibResponseMsg(char *ptrMsgSbcRx)
{
    byte index = 0;


	sbc_tx_data[index++] = 0xA5;
	sbc_tx_data[index++] = 0xAA;
	sbc_tx_data[index++] = 0x55;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x05;
	sbc_tx_data[index++] = ptrMsgSbcRx[5];
	sbc_tx_data[index++] = ptrMsgSbcRx[6];
	sbc_tx_data[index++] = 0x66;
	sbc_tx_data[index++] = ptrMsgSbcRx[7];
	sbc_tx_data[index++] = ptrMsgSbcRx[8] >> 8;
	sbc_tx_data[index++] = ptrMsgSbcRx[8];


	/*nel byte 7 dell'array di ricezione ho il numero del sensore di riferimento*/
	switch (ptrMsgSbcRx[7])
	{
		case 1:
			sbc_tx_data[index++] = (PR_OXYG_ADC >> 8)  & 0xFF;
			sbc_tx_data[index++] = PR_OXYG_ADC  & 0xFF;
			break;

		case 2:
			sbc_tx_data[index++] = (PR_LEVEL_ADC >> 8)  & 0xFF;
			sbc_tx_data[index++] = (PR_LEVEL_ADC)  & 0xFF;
			break;

		case 3:
			sbc_tx_data[index++] = (PR_ADS_FLT_ADC >> 8)  & 0xFF;
			sbc_tx_data[index++] = (PR_ADS_FLT_ADC)  & 0xFF;
			break;

		case 4:
			sbc_tx_data[index++] = (PR_VEN_ADC >> 8)  & 0xFF;
			sbc_tx_data[index++] = PR_VEN_ADC  & 0xFF;
			break;

		case 5:
			sbc_tx_data[index++] = (PR_ART_ADC >> 8)  & 0xFF;
			sbc_tx_data[index++] = PR_ART_ADC  & 0xFF;
			break;

		default:
			sbc_tx_data[index++] = 0;
			sbc_tx_data[index++] = 0;
			break;
	}


	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

void buildTempIRSensReadValuesResponseMsg(char *ptrMsgSbcRx)
{
	byte index = 0;
	word value = 0;

	sbc_tx_data[index++] = 0xA5;
	sbc_tx_data[index++] = 0xAA;
	sbc_tx_data[index++] = 0x55;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x0D;
	sbc_tx_data[index++] = ptrMsgSbcRx[5];
	sbc_tx_data[index++] = ptrMsgSbcRx[6];
	sbc_tx_data[index++] = 0x66;
	sbc_tx_data[index++] = 0x00; //TODO stable byte

	for(int i = 0 ; i < 3 ; i++)
	{
		value = (word)(sensorIR_TM[i].tempSensValue*10);
		sbc_tx_data[index++] = (value >> 8) & 0xFF;
		sbc_tx_data[index++] = (value     ) & 0xFF;

		value = sensorIR_TM[i].tempSensAdc;
		sbc_tx_data[index++] = (value >> 8) & 0xFF;
		sbc_tx_data[index++] = (value     ) & 0xFF;
	}

	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

void buildTempIRSensReadRegResponseMsg(char *ptrMsgSbcRx)
{
	byte index = 0;
	word value = 0;

	sbc_tx_data[index++] = 0xA5;
	sbc_tx_data[index++] = 0xAA;
	sbc_tx_data[index++] = 0x55;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x05;
	sbc_tx_data[index++] = ptrMsgSbcRx[5];
	sbc_tx_data[index++] = ptrMsgSbcRx[6];
	sbc_tx_data[index++] = 0x66;
	sbc_tx_data[index++] = ptrMsgSbcRx[7];
	sbc_tx_data[index++] = ptrMsgSbcRx[8];
	sbc_tx_data[index++] = 0x00;	//TODO add mem value
	sbc_tx_data[index++] = 0x00;	//TODO add mem value
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

void buildTempIRSensWriteRegResponseMsg(char *ptrMsgSbcRx)
{
	byte index = 0;

	sbc_tx_data[index++] = 0xA5;
	sbc_tx_data[index++] = 0xAA;
	sbc_tx_data[index++] = 0x55;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x05;
	sbc_tx_data[index++] = ptrMsgSbcRx[5];
	sbc_tx_data[index++] = ptrMsgSbcRx[6];
	sbc_tx_data[index++] = 0x66;
	sbc_tx_data[index++] = ptrMsgSbcRx[7];
	sbc_tx_data[index++] = ptrMsgSbcRx[8];
	sbc_tx_data[index++] = ptrMsgSbcRx[9];
	sbc_tx_data[index++] = ptrMsgSbcRx[10];
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

void buildReadFlowAirResponseMsg(char *ptrMsgSbcRx)
{
	byte index = 0;

	sbc_tx_data[index++] = 0xA5;
	sbc_tx_data[index++] = 0xAA;
	sbc_tx_data[index++] = 0x55;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x13;
	sbc_tx_data[index++] = ptrMsgSbcRx[5];
	sbc_tx_data[index++] = ptrMsgSbcRx[6];
	sbc_tx_data[index++] = 0x66;
	sbc_tx_data[index++] = 0x00; //TODO stability flag
	sbc_tx_data[index++] = 0x00; //TODO air bits

	for(int i = 0 ; i < 2 ; i++)
	{
		//TODO add instant flow
		sbc_tx_data[index++] = 0x00;
		sbc_tx_data[index++] = 0x00;
		sbc_tx_data[index++] = 0x00;
		sbc_tx_data[index++] = 0x00;

		//TODO check if this data is valid or not
		sbc_tx_data[index++] = sensor_UFLOW[i].bufferReceived[20];
		sbc_tx_data[index++] = sensor_UFLOW[i].bufferReceived[19];
		sbc_tx_data[index++] = sensor_UFLOW[i].bufferReceived[18];
		sbc_tx_data[index++] = sensor_UFLOW[i].bufferReceived[17];
	}

	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

void buildReadFlowResetResponseMsg(char *ptrMsgSbcRx)
{
	byte index = 0;

	sbc_tx_data[index++] = 0xA5;
	sbc_tx_data[index++] = 0xAA;
	sbc_tx_data[index++] = 0x55;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x02;
	sbc_tx_data[index++] = ptrMsgSbcRx[5];
	sbc_tx_data[index++] = ptrMsgSbcRx[6];
	sbc_tx_data[index++] = 0x66;
	sbc_tx_data[index++] = ptrMsgSbcRx[7];
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

void buildPeltierReadFloatResponseMsg(char *ptrMsgSbcRx, char *ieee754ptr)
{
	byte index = 0;

	sbc_tx_data[index++] = 0xA5;
	sbc_tx_data[index++] = 0xAA;
	sbc_tx_data[index++] = 0x55;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x06;
	sbc_tx_data[index++] = ptrMsgSbcRx[5];
	sbc_tx_data[index++] = ptrMsgSbcRx[6];
	sbc_tx_data[index++] = 0x66;
	sbc_tx_data[index++] = ptrMsgSbcRx[7];
	sbc_tx_data[index++] = *ieee754ptr; //TODO HH value
	sbc_tx_data[index++] = *(ieee754ptr+1); //TODO HL value
	sbc_tx_data[index++] = *(ieee754ptr+2); //TODO LH value
	sbc_tx_data[index++] = *(ieee754ptr+3); //TODO LL value
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

void buildPeltierReadIntResponseMsg(char *ptrMsgSbcRx, char *intptr)
{
	byte index = 0;

	sbc_tx_data[index++] = 0xA5;
	sbc_tx_data[index++] = 0xAA;
	sbc_tx_data[index++] = 0x55;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x03;
	sbc_tx_data[index++] = ptrMsgSbcRx[5];
	sbc_tx_data[index++] = ptrMsgSbcRx[6];
	sbc_tx_data[index++] = 0x66;
	sbc_tx_data[index++] = ptrMsgSbcRx[7];
	sbc_tx_data[index++] = *intptr; //TODO int value
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

void buildPeltierWriteFloatResponseMsg(char *ptrMsgSbcRx)
{
	byte index = 0;

	sbc_tx_data[index++] = 0xA5;
	sbc_tx_data[index++] = 0xAA;
	sbc_tx_data[index++] = 0x55;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x05;
	sbc_tx_data[index++] = ptrMsgSbcRx[5];
	sbc_tx_data[index++] = ptrMsgSbcRx[6];
	sbc_tx_data[index++] = 0x66;
	sbc_tx_data[index++] = ptrMsgSbcRx[7];
	sbc_tx_data[index++] = ptrMsgSbcRx[8];
	sbc_tx_data[index++] = ptrMsgSbcRx[9];
	sbc_tx_data[index++] = ptrMsgSbcRx[10];
	sbc_tx_data[index++] = ptrMsgSbcRx[11];
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

void buildPeltierWriteIntResponseMsg(char *ptrMsgSbcRx)
{
	byte index = 0;

	sbc_tx_data[index++] = 0xA5;
	sbc_tx_data[index++] = 0xAA;
	sbc_tx_data[index++] = 0x55;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x02;
	sbc_tx_data[index++] = ptrMsgSbcRx[5];
	sbc_tx_data[index++] = ptrMsgSbcRx[6];
	sbc_tx_data[index++] = 0x66;
	sbc_tx_data[index++] = ptrMsgSbcRx[7];
	sbc_tx_data[index++] = ptrMsgSbcRx[8];
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

void buildPeltierStartResponseMsg(char *ptrMsgSbcRx)
{
	byte index = 0;

	sbc_tx_data[index++] = 0xA5;
	sbc_tx_data[index++] = 0xAA;
	sbc_tx_data[index++] = 0x55;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x01;
	sbc_tx_data[index++] = ptrMsgSbcRx[5];
	sbc_tx_data[index++] = ptrMsgSbcRx[6];
	sbc_tx_data[index++] = 0x66;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

void buildPeltierStopResponseMsg(char *ptrMsgSbcRx)
{
	byte index = 0;

	sbc_tx_data[index++] = 0xA5;
	sbc_tx_data[index++] = 0xAA;
	sbc_tx_data[index++] = 0x55;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x01;
	sbc_tx_data[index++] = ptrMsgSbcRx[5];
	sbc_tx_data[index++] = ptrMsgSbcRx[6];
	sbc_tx_data[index++] = 0x66;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

void buildPeltierWriteEEResponseMsg(char *ptrMsgSbcRx)
{
	byte index = 0;

	sbc_tx_data[index++] = 0xA5;
	sbc_tx_data[index++] = 0xAA;
	sbc_tx_data[index++] = 0x55;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x01;
	sbc_tx_data[index++] = ptrMsgSbcRx[5];
	sbc_tx_data[index++] = ptrMsgSbcRx[6];
	sbc_tx_data[index++] = 0x66;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

//void buildPeltierResponseMsg(char code){
//
//	char numFloat[4];
//	//long ret;
//	//char data[8];
//	sbc_tx_data[0] = 0xA5;
//	sbc_tx_data[1] = 0xAA;
//	sbc_tx_data[2] = 0x55;
//	sbc_tx_data[3] = 0x00;
//	sbc_tx_data[4] = 0x01;
//	sbc_tx_data[5] = 0xCC;
//	sbc_tx_data[6] = code;
//	if(code == 0x26) /*read float*/
//	{
//		dataIeee754[0] = *ptrMsgDataieee754start;
//		dataIeee754[1] = *(ptrMsgDataieee754start+1);
//		dataIeee754[2] = *(ptrMsgDataieee754start+2);
//		dataIeee754[3] = *(ptrMsgDataieee754start+3);
//		dataIeee754[4] = *(ptrMsgDataieee754start+4);
//		dataIeee754[5] = *(ptrMsgDataieee754start+5);
//		dataIeee754[6] = *(ptrMsgDataieee754start+6);
//		dataIeee754[7] = *(ptrMsgDataieee754start+7);
//		//retIeee754 = strtol(dataIeee754,NULL,16);
//		retIeee754Dummy = strtoul(dataIeee754,NULL,16); //ok anche per numeri negativi
//
//		sbc_tx_data[7] = retIeee754Dummy>>24; //retIeee754>>24;
//		sbc_tx_data[8] = retIeee754Dummy>>16; //retIeee754>>16;
//		sbc_tx_data[9] = retIeee754Dummy>>8; //retIeee754>>8;
//		sbc_tx_data[10] = retIeee754Dummy; //retIeee754;
//
//
//		/*sbcDebug_tx_data[7] = *ptrMsgDataieee754start; /* most significant byte */
//		/*sbcDebug_tx_data[8] = *(ptrMsgDataieee754start+1);
//		sbcDebug_tx_data[9] = *(ptrMsgDataieee754start+2);
//		sbcDebug_tx_data[10] = *(ptrMsgDataieee754start+3);
//		sbcDebug_tx_data[11] = *(ptrMsgDataieee754start+4);
//		sbcDebug_tx_data[12] = *(ptrMsgDataieee754start+5);
//		sbcDebug_tx_data[13] = *(ptrMsgDataieee754start+6);
//		sbcDebug_tx_data[14] = *(ptrMsgDataieee754start+7);*/
//	}
//	else if((code == 0x24) || (code == 0x22))
//	{
//		sbc_tx_data[7] = 0x00;
//		sbc_tx_data[8] = 0x00;
//		sbc_tx_data[9] = 0x00;
//		sbc_tx_data[10] = 0x00;
//	}
//	else if(code == 0x20) /*read int*/
//	{
//		dataIntPeltier[0] = *ptrMsgDataPeltierInt;
//		sbc_tx_data[7] = strtol(dataIntPeltier,NULL,16);
//		sbc_tx_data[8] = 0x00;
//		sbc_tx_data[9] = 0x00;
//		sbc_tx_data[10] = 0x00;
//	}
//
//	//sbcDebug_tx_data[7] = peltierCell.msgPeltierRx[0];
//	//sbcDebug_tx_data[7] = peltierDebug_rx_data[0];
//	//sbcDebug_tx_data[8] = peltierCell.msgPeltierRx[1];
//	//sbcDebug_tx_data[8] = peltierDebug_rx_data[1];
//	sbc_tx_data[11] = 0x00;
//	sbc_tx_data[12] = 0x00;
//	sbc_tx_data[13] = 0x5A;
//}
//
//void buildWriteTempSensResponseMsg(char code, char tempSensId){
//	union NumFloatUnion{
//				uint32 ieee754NUmFormat;
//				float numFormatFloat;
//			} numFloatSensor;
//
//	sbc_tx_data[0] = 0xA5;
//	sbc_tx_data[1] = 0xAA;
//	sbc_tx_data[2] = 0x55;
//	sbc_tx_data[3] = 0x00;
//	sbc_tx_data[4] = 0x01;
//	sbc_tx_data[5] = 0xCC;
//	sbc_tx_data[6] = code; //cmdId & 0x66
//	if(code == 0x35)
//	{
//		numFloatSensor.numFormatFloat = sensor_TMx[tempSensId].tempSensValue;
//
//		sbc_tx_data[7] = numFloatSensor.ieee754NUmFormat >> 24;
//		sbc_tx_data[8] = numFloatSensor.ieee754NUmFormat >> 16;
//		sbc_tx_data[9] = numFloatSensor.ieee754NUmFormat >> 8;
//		sbc_tx_data[10] = numFloatSensor.ieee754NUmFormat;
//	}
//	sbc_tx_data[11] = 0x00;
//	sbc_tx_data[12] = 0x00;
//	sbc_tx_data[13] = 0x5A;
//}
//
//void buildReadTempSensResponseMsg(char code, char tempSensId){
//	union NumFloatGain{
//			uint32 ieee754NumFormat_Gain;
//			float numFormatFloat_Gain;
//	} numFloatSensor_Gain;
//
//	union NumFloatOffset{
//			uint32 ieee754NumFormat_Offset;
//			float numFormatFloat_Offset;
//	} numFloatSensor_Offset;
//
//	union NumFloatOffVal{
//			uint32 ieee754NumFormat_OffVal;
//			float numFormatFloat_OffVal;
//	} numFloatSensor_OffVal;
//
//	sbc_tx_data[0] = 0xA5;
//	sbc_tx_data[1] = 0xAA;
//	sbc_tx_data[2] = 0x55;
//	sbc_tx_data[3] = 0x00;
//	sbc_tx_data[4] = 0x01;
//	sbc_tx_data[5] = 0xCC;
//	sbc_tx_data[6] = code;
//
//	if(code == 0x36)
//	{
//		numFloatSensor_Gain.numFormatFloat_Gain = sensor_TMx[tempSensId].tempSensGain;
//		sbc_tx_data[7] = numFloatSensor_Gain.ieee754NumFormat_Gain >> 24;
//		sbc_tx_data[8] = numFloatSensor_Gain.ieee754NumFormat_Gain >> 16;
//		sbc_tx_data[9] = numFloatSensor_Gain.ieee754NumFormat_Gain >> 8;
//		sbc_tx_data[10] = numFloatSensor_Gain.ieee754NumFormat_Gain;
//
//		numFloatSensor_Offset.numFormatFloat_Offset = sensor_TMx[tempSensId].tempSensOffset;
//		sbc_tx_data[11] = numFloatSensor_Offset.ieee754NumFormat_Offset >> 24;
//		sbc_tx_data[12] = numFloatSensor_Offset.ieee754NumFormat_Offset >> 16;
//		sbc_tx_data[13] = numFloatSensor_Offset.ieee754NumFormat_Offset >> 8;
//		sbc_tx_data[14] = numFloatSensor_Offset.ieee754NumFormat_Offset;
//
//		numFloatSensor_OffVal.numFormatFloat_OffVal = sensor_TMx[tempSensId].tempSensOffsetVal;
//		sbc_tx_data[15] = numFloatSensor_OffVal.ieee754NumFormat_OffVal >> 24;
//		sbc_tx_data[16] = numFloatSensor_OffVal.ieee754NumFormat_OffVal >> 16;
//		sbc_tx_data[17] = numFloatSensor_OffVal.ieee754NumFormat_OffVal >> 8;
//		sbc_tx_data[18] = numFloatSensor_OffVal.ieee754NumFormat_OffVal;
//
//		sbc_tx_data[19] = sensor_TMx[tempSensId].tempSensAdc >> 8;
//		sbc_tx_data[20] = sensor_TMx[tempSensId].tempSensAdc;
//	}
//
//	sbc_tx_data[21] = 0x00;
//	sbc_tx_data[22] = 0x00;
//	sbc_tx_data[23] = 0x5A;
//}
//
//void buildReadIRTempRspMsg(char code, char tempIRSensId){
//	union NumFloatOffVal{
//				uint32 ieee754NumFormat_Val;
//				float numFormatFloat_Val;
//		} numFloatIRTempSensor_Val;
//
//	sbc_tx_data[0] = 0xA5;
//	sbc_tx_data[1] = 0xAA;
//	sbc_tx_data[2] = 0x55;
//	sbc_tx_data[3] = 0x00;
//	sbc_tx_data[4] = 0x01;
//	sbc_tx_data[5] = 0xCC;
//	sbc_tx_data[6] = code;
//	if(code == 0x40)
//	{
//		numFloatIRTempSensor_Val.numFormatFloat_Val = sensorIR_TM[0].tempSensValue;
//		sbc_tx_data[7] = numFloatIRTempSensor_Val.ieee754NumFormat_Val >> 24;
//		sbc_tx_data[8] = numFloatIRTempSensor_Val.ieee754NumFormat_Val >> 16;
//		sbc_tx_data[9] = numFloatIRTempSensor_Val.ieee754NumFormat_Val >> 8;
//		sbc_tx_data[10] = numFloatIRTempSensor_Val.ieee754NumFormat_Val;
//	}
//	sbc_tx_data[11] = 0x00;
//	sbc_tx_data[12] = 0x00;
//	sbc_tx_data[13] = 0x5A;
//}

/******************************************************************************************/
/*                          TREATMENT - START SECTION									  */
/******************************************************************************************/
void initCommSBC(void){
	myCommunicatorToSBC.dataFlowSensReadyFlag = DATA_COMM_IDLE;
	myCommunicatorToSBC.dataIRTempSensReadyFlag = DATA_COMM_IDLE;
	myCommunicatorToSBC.dataMachineStateReadyFlag = DATA_COMM_IDLE;
	myCommunicatorToSBC.dataModBusReadyFlag = DATA_COMM_IDLE;
	myCommunicatorToSBC.dataPeltierReadyFlag = DATA_COMM_IDLE;
	myCommunicatorToSBC.dataPressSensReadyFlag = DATA_COMM_IDLE;
	myCommunicatorToSBC.dataPerfParamReadyFlag = DATA_COMM_IDLE;
	myCommunicatorToSBC.dataButtonSBCReadyFlag = DATA_COMM_IDLE;
	myCommunicatorToSBC.dataParamSetSBCReadyFlag = DATA_COMM_IDLE;

	myCommunicatorToSBC.numByteToSend = 0;
}



void pollingSBCCommTreat(void){

	word valueWord = 0;

	union NumFloatUnion{
			uint32 ieee754ValFormat;
			float valFormatFloat;
		} valueFloat;

	if(iflag_sbc_rx == IFLAG_SBC_RX)
	{
		iflag_sbc_rx = IFLAG_IDLE;

		byte code = sbc_rx_data[SBC_RX_DATA_POS_CODE];
		byte subcode = sbc_rx_data[SBC_RX_DATA_POS_SUBCODE];


		/* decodifica messaggio */
		switch(code)
		{
			/* Status message */
			case COMMAND_ID_ST:
			{
				myCommunicatorToSBC.dataMachineStateReadyFlag = DATA_COMM_READY_TO_BE_SEND;
			}
			break;

			/* Set parameter from SBC */
			case COMMAND_ID_PAR_SET:
			{
				valueWord = (sbc_rx_data[8] << 8) + sbc_rx_data[9];
				setParamWordFromGUI(sbc_rx_data[7],valueWord);
				ShowParameterStr(parameterWordSetFromGUI, sbc_rx_data[7]);

				myCommunicatorToSBC.dataParamSetSBCReadyFlag = DATA_COMM_READY_TO_BE_SEND;
			}
			break;

			/* Button pressed from SBC */
			case COMMAND_ID_BUT_SBC:
			{
				if(sbc_rx_data[8] == 0x01)
				{
					// FM scambiati eventi
					//setGUIButton(sbc_rx_data[7]);
					releaseGUIButton(sbc_rx_data[7]);
				}
				else
				{
					// FM scambiati eventi
					// releaseGUIButton(sbc_rx_data[7]);
					setGUIButton(sbc_rx_data[7]);
					ShowButtonStr(buttonGUITreatment, sbc_rx_data[7]);
				}
				myCommunicatorToSBC.dataButtonSBCReadyFlag = DATA_COMM_READY_TO_BE_SEND;
			}
			break;

			default:{}
			break;
		}
	}
}


void pollingDataToSBCTreat(void)
{
	if(myCommunicatorToSBC.dataMachineStateReadyFlag == DATA_COMM_READY_TO_BE_SEND)
	{
		word snd;

		/*reset flag */
		myCommunicatorToSBC.dataMachineStateReadyFlag = DATA_COMM_IDLE;

		/* build response message */
		buildRDMachineStateResponseMsg(COMMAND_ID_ST,sbc_rx_data[6]);

		/* build response message */
		ptrMsgSbcTx = &sbc_tx_data[0];

		/* send answer */
		SBC_COMM_SendBlock(ptrMsgSbcTx,myCommunicatorToSBC.numByteToSend,&snd);
	}
	else if(myCommunicatorToSBC.dataButtonSBCReadyFlag == DATA_COMM_READY_TO_BE_SEND)
	{
		word snd;

		/*reset flag */
		myCommunicatorToSBC.dataButtonSBCReadyFlag = DATA_COMM_IDLE;

		/* build response message */
		buildButtonSBCResponseMsg(COMMAND_ID_BUT_SBC, sbc_rx_data[6], sbc_rx_data[7]);

		/* build response message */
		ptrMsgSbcTx = &sbc_tx_data[0];

		/* send answer */
		SBC_COMM_SendBlock(ptrMsgSbcTx,myCommunicatorToSBC.numByteToSend,&snd);
	}
	else if(myCommunicatorToSBC.dataParamSetSBCReadyFlag == DATA_COMM_READY_TO_BE_SEND)
	{
		word snd;

		/*reset flag */
		myCommunicatorToSBC.dataParamSetSBCReadyFlag = DATA_COMM_IDLE;

		/* build response message */
		buildParamSetSBCResponseMsg(COMMAND_ID_PAR_SET, sbc_rx_data[6],
									sbc_rx_data[7],sbc_rx_data[8],
									sbc_rx_data[9]);

		/* build response message */
		ptrMsgSbcTx = &sbc_tx_data[0];

		/* send answer */
		SBC_COMM_SendBlock(ptrMsgSbcTx,myCommunicatorToSBC.numByteToSend,&snd);
	}
}

void buildRDMachineStateResponseMsg(char code, char subcode)
{
	byte index = 0;
	unsigned int life = FreeRunCnt10msec *10;

	sbc_tx_data[index++] = 0xA5;
	sbc_tx_data[index++] = 0xAA;
	sbc_tx_data[index++] = 0x55;
	sbc_tx_data[index++] = 0x00;
	/* byte count: 79 byte */
	sbc_tx_data[index++] = 0x5F;
	/* command id */
	sbc_tx_data[index++] = code;
	/* command id */
	sbc_tx_data[index++] = subcode;
	/* TODO what is the meaning of this byte? */
	sbc_tx_data[index++] = 0x66;

	/* STATUS PARAMETERS */
	/* TODO status parameters: life  */
	sbc_tx_data[index++] = (life >> 8) & 0xFF;
	sbc_tx_data[index++] = life & 0xFF;
	/* TODO status parameters: rev fw-H
	 * potrei usare questa come versione FW
	 * della CON usando 5 bit per il primo numero
	 * 5 bit per il seocndo numero e 6 bit per il
	 * terzo numero avendo quindi una versione massima
	 * pari a 32.32.64  */

	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x00;

	/* TODO status parameters: rev fw-H
	 * potrei usare questa come versione FW
	 * della PRO usando 5 bit per il primo numero
	 * 5 bit per il seocndo numero e 6 bit per il
	 * terzo numero avendo quindi una versione massima
	 * pari a 32.32.64  */

	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x00;

	/* status parameters: alarm code */
	sbc_tx_data[index++] = (alarmCurrent.code >> 8 ) & 0xFF;
	sbc_tx_data[index++] = (alarmCurrent.code 	    ) & 0xFF;
	/* status parameters: alarm physic */
	sbc_tx_data[index++] = (alarmCurrent.active >> 8 ) & 0xFF;
	sbc_tx_data[index++] = (alarmCurrent.active      ) & 0xFF;
	/* status parameters: alarm type */
	sbc_tx_data[index++] = (alarmCurrent.type >> 8 ) & 0xFF;
	sbc_tx_data[index++] = (alarmCurrent.type      ) & 0xFF;
	/* status parameters: machine state state*/
	sbc_tx_data[index++] = (ptrCurrentState->state >> 8) & 0xFF;
	sbc_tx_data[index++] = (ptrCurrentState->state     ) & 0xFF;
	/* status parameters: machine state parent*/
	sbc_tx_data[index++] = (ptrCurrentParent->parent >> 8) & 0xFF;
	sbc_tx_data[index++] = (ptrCurrentParent->parent     ) & 0xFF;
	/* status parameters: machine state child*/
	sbc_tx_data[index++] = (ptrCurrentChild->child >> 8) & 0xFF;
	sbc_tx_data[index++] = (ptrCurrentChild->child     ) & 0xFF;
	/* STATUS PARAMETERS */

	/* SENS PARAMETERS */
	/* sensors parameters: pressure adsorbent filter */

	sbc_tx_data[index++] = (PR_ADS_FLT_mmHg_Filtered >> 8) & 0xFF;
	sbc_tx_data[index++] = (PR_ADS_FLT_mmHg_Filtered) & 0xFF;
	/* sensors parameters: pressure arterial */

	sbc_tx_data[index++] = (MedForArteriousPid >> 8) & 0xFF;
	sbc_tx_data[index++] = (MedForArteriousPid) & 0xFF;
	/* sensors parameters: pressure venous */

	sbc_tx_data[index++] = (MedForVenousPid >> 8) & 0xFF;
	sbc_tx_data[index++] = (MedForVenousPid) & 0xFF;
	/* sensors parameters: pressure oxygenation */

	sbc_tx_data[index++] = (PR_OXYG_mmHg_Filtered >> 8) & 0xFF;
	sbc_tx_data[index++] = (PR_OXYG_mmHg_Filtered) & 0xFF;
	/* sensors parameters: pressure level */

	sbc_tx_data[index++] = (LiquidAmount >> 8) & 0xFF; // PR_LEVEL_mmHg_Filtered
	sbc_tx_data[index++] = (LiquidAmount) & 0xFF;      // PR_LEVEL_mmHg_Filtered

	/* sensors parameters: pressure systolic arterial */
	sbc_tx_data[index++] = (PR_ART_Sistolyc_mmHg >> 8) & 0xFF;
	sbc_tx_data[index++] = (PR_ART_Sistolyc_mmHg     ) & 0xFF;
	/* sensors parameters: pressure diastolic arterial */
	sbc_tx_data[index++] = (PR_ART_Diastolyc_mmHg >> 8) & 0xFF;
	sbc_tx_data[index++] = (PR_ART_Diastolyc_mmHg     ) & 0xFF;
	/* sensors parameters: pressure mean arterial */

	sbc_tx_data[index++] = (PR_ART_Med_mmHg >> 8) & 0xFF;
	sbc_tx_data[index++] = (PR_ART_Med_mmHg     ) & 0xFF;
	/* sensors parameters: pressure systolic venous */
	sbc_tx_data[index++] = (PR_VEN_Sistolyc_mmHg_ORG >> 8) & 0xFF;
	sbc_tx_data[index++] = (PR_VEN_Sistolyc_mmHg_ORG     ) & 0xFF;
	/* sensors parameters: pressure diastolic venous */
	sbc_tx_data[index++] = (PR_VEN_Diastolyc_mmHg_ORG >> 8) & 0xFF;
	sbc_tx_data[index++] = (PR_VEN_Diastolyc_mmHg_ORG     ) & 0xFF;
	/* sensors parameters: pressure mean venous */
	sbc_tx_data[index++] = (PR_VEN_Med_mmHg_ORG >> 8) & 0xFF;
	sbc_tx_data[index++] = (PR_VEN_Med_mmHg_ORG     ) & 0xFF;

	/* sensors parameters: pressure flow arterial */
	sbc_tx_data[index++] = (((int)sensor_UFLOW[0].Average_Flow_Val) >> 8) & 0xFF;  // sensorsValues.flowArt
	sbc_tx_data[index++] = ((int)sensor_UFLOW[0].Average_Flow_Val) & 0xFF;
	/* sensors parameters: pressure flow venous / oxygenation */
	sbc_tx_data[index++] = (((int)sensor_UFLOW[1].Average_Flow_Val) >> 8) & 0xFF;  // sensorsValues.flowVenOxy
	sbc_tx_data[index++] = ((int)sensor_UFLOW[1].Average_Flow_Val) & 0xFF;
	/* sensors parameters: temperature reservoir outlet */
	sbc_tx_data[index++] = (((int)(sensorIR_TM[0].tempSensValue*10)) >> 8) & 0xFF;
	sbc_tx_data[index++] = (((int)(sensorIR_TM[0].tempSensValue*10))     ) & 0xFF;
	/* sensors parameters: temperature organ inlet */
	sbc_tx_data[index++] = (((int)(sensorIR_TM[1].tempSensValue*10)) >> 8) & 0xFF;
	sbc_tx_data[index++] = (((int)(sensorIR_TM[1].tempSensValue*10))     ) & 0xFF;
	/* sensors parameters: temperature venous / oxygenation */
	sbc_tx_data[index++] = (((int)(sensorIR_TM[2].tempSensValue*10)) >> 8) & 0xFF;
	sbc_tx_data[index++] = (((int)(sensorIR_TM[2].tempSensValue*10))     ) & 0xFF;
	/* SENS PARAMETERS */

	/* PERFUSION PARAMETERS */
	/* perfusion parameters: priming volume adsorbent filter */
	sbc_tx_data[index++] = (perfusionParam.priVolAdsFilter >> 8) & 0xFF;
	sbc_tx_data[index++] = (perfusionParam.priVolAdsFilter     ) & 0xFF;
	/* perfusion parameters: priming volume perfusion arterial */
	sbc_tx_data[index++] = (perfusionParam.priVolPerfArt >> 8) & 0xFF;
	sbc_tx_data[index++] = (perfusionParam.priVolPerfArt     ) & 0xFF;
	/* perfusion parameters: priming volume perfusion venous / oxygenation */
	sbc_tx_data[index++] = (perfusionParam.priVolPerfVenOxy >> 8) & 0xFF;
	sbc_tx_data[index++] = (perfusionParam.priVolPerfVenOxy     ) & 0xFF;
	/* perfusion parameters: priming duration perfusion arterial */
	sbc_tx_data[index++] = (perfusionParam.priDurPerfArt >> 8) & 0xFF;
	sbc_tx_data[index++] = (perfusionParam.priDurPerfArt     ) & 0xFF;
	/* perfusion parameters: priming duration perfusion venous / oxygenation */
	sbc_tx_data[index++] = (perfusionParam.priDurPerVenOxy >> 8) & 0xFF;
	sbc_tx_data[index++] = (perfusionParam.priDurPerVenOxy     ) & 0xFF;
	/* perfusion parameters: treatment volume adsorbent filter */
	sbc_tx_data[index++] = (perfusionParam.treatVolAdsFilter >> 8) & 0xFF;
	sbc_tx_data[index++] = (perfusionParam.treatVolAdsFilter     ) & 0xFF;
	/* perfusion parameters: treatment volume perfusion arterial */
	sbc_tx_data[index++] = (perfusionParam.treatVolPerfArt >> 8) & 0xFF;
	sbc_tx_data[index++] = (perfusionParam.treatVolPerfArt     ) & 0xFF;
	/* perfusion parameters: treatment volume perfusion venous / oxygenation */
	sbc_tx_data[index++] = (perfusionParam.treatVolPerfVenOxy >> 8) & 0xFF;
	sbc_tx_data[index++] = (perfusionParam.treatVolPerfVenOxy     ) & 0xFF;
	/* perfusion parameters: treatment duration perfusion arterial */
	sbc_tx_data[index++] = (perfusionParam.treatDurPerfArt >> 8) & 0xFF;
	sbc_tx_data[index++] = (perfusionParam.treatDurPerfArt     ) & 0xFF;
	/* perfusion parameters: treatment duration perfusion venous / oxygenation */
	sbc_tx_data[index++] = (perfusionParam.treatDurPerVenOxy >> 8) & 0xFF;
	sbc_tx_data[index++] = (perfusionParam.treatDurPerVenOxy     ) & 0xFF;

	/* perfusion parameters: unload volume adsorbent filter */
	sbc_tx_data[index++] = (perfusionParam.unlVolAdsFilter >> 8) & 0xFF;
	sbc_tx_data[index++] = (perfusionParam.unlVolAdsFilter     ) & 0xFF;
	/* perfusion parameters: unload volume perfusion arterial */
	sbc_tx_data[index++] = (perfusionParam.unlVolPerfArt >> 8) & 0xFF;
	sbc_tx_data[index++] = (perfusionParam.unlVolPerfArt     ) & 0xFF;
	/* perfusion parameters: unload volume perfusion venous / oxygenation */
	sbc_tx_data[index++] = (perfusionParam.unlVolPerfVenOxy >> 8) & 0xFF;
	sbc_tx_data[index++] = (perfusionParam.unlVolPerfVenOxy     ) & 0xFF;
	/* perfusion parameters: unload volume reservoir */
	sbc_tx_data[index++] = (perfusionParam.unlVolRes >> 8) & 0xFF;
	sbc_tx_data[index++] = (perfusionParam.unlVolRes     ) & 0xFF;
	/* perfusion parameters: unload duration perfusion arterial */
	sbc_tx_data[index++] = (perfusionParam.unlDurPerfArt >> 8) & 0xFF;
	sbc_tx_data[index++] = (perfusionParam.unlDurPerfArt     ) & 0xFF;
	/* perfusion parameters: unload duration perfusion venous / oxygenation */
	sbc_tx_data[index++] = (perfusionParam.unlDurPerVenOxy >> 8) & 0xFF;
	sbc_tx_data[index++] = (perfusionParam.unlDurPerVenOxy     ) & 0xFF;

	/* perfusion parameters: renal resistence */
	sbc_tx_data[index++] = (perfusionParam.renalResistance >> 8) & 0xFF;
	sbc_tx_data[index++] = (perfusionParam.renalResistance     ) & 0xFF;

	/* perfusion parameters: pulsatility */
	sbc_tx_data[index++] = (perfusionParam.pulsatility >> 8) & 0xFF;
	sbc_tx_data[index++] = (perfusionParam.pulsatility     ) & 0xFF;
	/* perfusion parameters: pressure drop adsorbent filter */
	sbc_tx_data[index++] = (perfusionParam.pressDropAdsFilter >> 8) & 0xFF;
	sbc_tx_data[index++] = (perfusionParam.pressDropAdsFilter     ) & 0xFF;
	/* PERFUSION PARAMETERS */

	/* TODO CRC H */
	sbc_tx_data[index++] = 0x00;
	/* TODO CRC L */
	sbc_tx_data[index++] = 0x00;
	/* End */
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

void buildButtonSBCResponseMsg(char code, char subcode, unsigned char buttonId)
{
	byte index = 0;


	sbc_tx_data[index++] = 0xA5;
	sbc_tx_data[index++] = 0xAA;
	sbc_tx_data[index++] = 0x55;
	sbc_tx_data[index++] = 0x00;
	/* byte count: 1 byte */
	sbc_tx_data[index++] = 0x03;
	/* command id */
	sbc_tx_data[index++] = code;
	/* sub command id */
	sbc_tx_data[index++] = subcode;
	/* status id */
	sbc_tx_data[index++] = 0x66;
	/* button id */
	sbc_tx_data[index++] = buttonId;
	/* button state */
	sbc_tx_data[index++] = getGUIButton(buttonId);
	/* TODO CRC H */
	sbc_tx_data[index++] = 0x00;
	/* TODO CRC L */
	sbc_tx_data[index++] = 0x00;
	/* End */
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

void buildParamSetSBCResponseMsg(char code, char subcode, unsigned char paramId, unsigned char param_h, unsigned char param_l)
{
	byte index = 0;

	sbc_tx_data[index++] = 0xA5;
	sbc_tx_data[index++] = 0xAA;
	sbc_tx_data[index++] = 0x55;
	sbc_tx_data[index++] = 0x00;
	/* byte count: 1 byte */
	sbc_tx_data[index++] = 0x04;
	/* command id */
	sbc_tx_data[index++] = code;
	/* sub command id */
	sbc_tx_data[index++] = subcode;
	/* status id */
	sbc_tx_data[index++] = 0x66;
	/* param id */
	sbc_tx_data[index++] = paramId;
	/* param value high */
	sbc_tx_data[index++] = param_h;
	/* param value low */
	sbc_tx_data[index++] = param_l;
	/* TODO CRC H */
	sbc_tx_data[index++] = 0x00;
	/* TODO CRC L */
	sbc_tx_data[index++] = 0x00;
	/* End */
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

/******************************************************************************************/
/*                          TREATMENT - END SECTION									  */
/******************************************************************************************/



/**/
void initGUIButton(void){
	buttonGUITreatment[BUTTON_PINCH_2WPVF_RIGHT_OPEN].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_PINCH_2WPVF_LEFT_OPEN].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_PINCH_2WPVF_BOTH_CLOSED].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_PINCH_2WPVA_RIGHT_OPEN].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_PINCH_2WPVA_LEFT_OPEN].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_PINCH_2WPVA_BOTH_CLOSED].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_PINCH_2WPVV_RIGHT_OPEN].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_PINCH_2WPVV_LEFT_OPEN].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_PINCH_2WPVV_BOTH_CLOSED].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_CONFIRM].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_RESET].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_PRIMING_END_CONFIRM].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_PRIMING_FILT_INS_CONFIRM].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_PRIMING_ABANDON].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_START_PRIMING].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_STOP_PRIMING].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_STOP_ALL_PUMP].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_START_TREATMENT].state = GUI_BUTTON_NULL; // viene dato alla fine del ricircolo per far partire il trattamento
	buttonGUITreatment[BUTTON_STOP_TREATMENT].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_EN_PERFUSION].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_EN_OXYGENATION].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_EN_PURIFICATION].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_START_PERF_PUMP].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_STOP_PERF_PUMP].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_START_OXYGEN_PUMP].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_STOP_OXYGEN_PUMP].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_START_PURIF_PUMP].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_STOP_PURIF_PUMP].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_PERF_DISP_MOUNTED].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_OXYG_DISP_MOUNTED].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_PERF_TANK_FILL].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_PERF_FILTER_MOUNT].state = GUI_BUTTON_NULL;

	buttonGUITreatment[BUTTON_OVERRIDE_ALARM].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_RESET_ALARM].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_SILENT_ALARM].state = GUI_BUTTON_NULL;

	buttonGUITreatment[BUTTON_START_EMPTY_DISPOSABLE].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_UNMOUNT_END].state = GUI_BUTTON_NULL;
}

// forzo stato GUI_BUTTON_RELEASED sul tasto buttonId
// Questo e' stato fatto perche' ora si vuol lavorare sugli eventi di release
// e non di press.
// Facendo questa inversione non cambio tutto il resto del codice.
void setGUIButton(unsigned char buttonId){
	//buttonGUITreatment[buttonId].state = GUI_BUTTON_PRESSED;
	buttonGUITreatment[buttonId].state = GUI_BUTTON_RELEASED;
	actionFlag = 2;
}

unsigned char getGUIButton(unsigned char buttonId)
{
	return buttonGUITreatment[buttonId].state;
}

// forzo stato GUI_BUTTON_PRESSED sul tasto buttonId
void releaseGUIButton(unsigned char buttonId){
	// FM visto che ora si lavora sugli eventi release, l'assenza di evento tasto diventa GUI_BUTTON_PRESSED !!!
	//buttonGUITreatment[buttonId].state = GUI_BUTTON_RELEASED;
	buttonGUITreatment[buttonId].state = GUI_BUTTON_PRESSED;
}
/**/


/**/
void initSetParamFromGUI(void){
	parameterWordSetFromGUI[PAR_SET_PRIMING_VOL_PERFUSION].value = 0;
	parameterWordSetFromGUI[PAR_SET_PRIMING_VOL_PURIFICATION].value = 0;
	parameterWordSetFromGUI[PAR_SET_PRIMING_VOL_OXYGENATION].value = 0;
	parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value = 0;
	parameterWordSetFromGUI[PAR_SET_TEMPERATURE].value = 0;
	parameterWordSetFromGUI[PAR_SET_PRIMING_TEMPERATURE_PERFUSION].value = 0;
	parameterWordSetFromGUI[PAR_SET_PRESS_ART_TARGET].value = 0;
	parameterWordSetFromGUI[PAR_SET_PRESS_VEN_TARGET].value = 0;
	parameterWordSetFromGUI[PAR_SET_PURIF_FLOW_TARGET].value = 0;
	parameterWordSetFromGUI[PAR_SET_PURIF_UF_FLOW_TARGET].value = 0;
}
void initSetParamInSourceCode(void)
{
	parameterWordSetFromGUI[PAR_SET_VENOUS_PRESS_TARGET].value = 0;
	parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value = 2000;
}


void setParamWordFromGUI(unsigned char parId, int value)
{
	if(parId == PAR_SET_PRIMING_VOL_PERFUSION)
	{
		// controllo range del volume di liquido caricato nel reservoir
		if(value > MAX_VOLUME_RESERVOIR)
			value = MAX_VOLUME_RESERVOIR;
		if(value < MIN_VOLUME_RESERVOIR)
			value = MIN_VOLUME_RESERVOIR;
#ifdef DEBUG_WITH_SERVICE_SBC
		if(!ParamRcvdInMounting[0])
			value = 62; // usato per debug con service sbc
		else
			value = 93;
#endif
		ParamRcvdInMounting[0] = 1;
		DebugStringStr("PAR_SET_PRIMING_VOL_PERF");
	}
	else if(parId == PAR_SET_PRIMING_TEMPERATURE_PERFUSION)
	{
//#ifdef DEBUG_WITH_SERVICE_SBC
//		value = value * 10; // usato per debug con service sbc
//#endif
		// controllo range del volume di liquido caricato nel reservoir
		if(value > (MAX_TEMP_PRIMING * 10))
			value = MAX_TEMP_PRIMING * 10;
		if(value < (MIN_TEMP_PRIMING * 10))
			value = MIN_TEMP_PRIMING * 10;
		ParamRcvdInMounting[1] = 1;
		DebugStringStr("PAR_SET_PRIMING_TEMP_PERF");
	}
	else if(parId == PAR_SET_OXYGENATOR_ACTIVE)
	{
		if((PARAMETER_ACTIVE_TYPE)value == YES)
		{
			ParamRcvdInMounting[2] = 1;
			DebugStringStr("PAR_SET_OXYGEN_ACTIVE");
		}
		else if((PARAMETER_ACTIVE_TYPE)value == NO)
		{
			ParamRcvdInMounting[2] = 1;
			DebugStringStr("PAR_SET_OXYGEN_NOT_ACTIVE");
		}
		else
			DebugStringStr("PAR_SET_OXYGEN_UNDEF");
	}
	else if(parId == PAR_SET_DEPURATION_ACTIVE)
	{
		if((PARAMETER_ACTIVE_TYPE)value == YES)
		{
			ParamRcvdInMounting[3] = 1;
			DebugStringStr("PAR_SET_DEPUR_ACTIVE");
		}
		else if((PARAMETER_ACTIVE_TYPE)value == NO)
		{
			ParamRcvdInMounting[3] = 1;
			DebugStringStr("PAR_SET_DEPUR_NOT_ACTIVE");
		}
		else
			DebugStringStr("PAR_SET_DEPUR_UNDEF");
	}
	else if (parId == PAR_SET_MAX_FLOW_PERFUSION)
	{
		int MaxFlow;
		int MinFlow;

		if (GetTherapyType() == KidneyTreat)
		{
			MaxFlow = MAX_FLOW_ART_KIDNEY;
			MinFlow = MIN_FLOW_ART_KIDNEY;
		}
		else if (GetTherapyType() == LiverTreat)
		{
			MaxFlow = MAX_FLOW_ART_LIVER;
			MinFlow = MIN_FLOW_ART_LIVER;
		}

		// controllo range del volume di liquido caricato nel reservoir
		if(value >= MaxFlow)
			value = MaxFlow;
		if(value <= MinFlow)
			value = MinFlow;
	}
	else if(parId == PAR_SET_OXYGENATOR_FLOW)
	{
		//CheckOxygenationSpeed(value);
	}
	else if(parId == PAR_SET_THERAPY_TYPE)
	{
		TherapyCmdArrived = 1;
	}

	parameterWordSetFromGUI[parId].value = value;
}

void resetParamWordFromGUI(unsigned char parId){
	parameterWordSetFromGUI[parId].value = 0;
}



#define NUM_LQUID_LEV_VALUES 39

// tabella relativa al tubo piccolo della cassetta

const word LiquidLevelTable[NUM_LQUID_LEV_VALUES][2] =
{
		// valore letto dal sensore di pressione mmHg, ml presenti nel recipiente
		66, 100,
		81,	150,
		90,	200,
		95,	250,
		105, 300,
		113, 350,
		123, 400,
		134, 450,
		143, 500,
		149, 550,
		155, 600,
		161, 650,
		171, 700,
		175, 750,
		180, 800,
		185, 850,
		191, 900,
		196, 950,
		199, 1000,
		208, 1100,
		218, 1200,
		226, 1300,
		233, 1400,
		242, 1500,
		248, 1600,
		256, 1700,
		265, 1800,
		271, 1900,
		279, 2000,
		287, 2100,
		292, 2200,
		298, 2300,
		309, 2400,
		314, 2500,
		320, 2600,
		329, 2700,
		336, 2800,
		343, 2900,
		346, 3000
};


/*
#define NUM_LQUID_LEV_VALUES 20

const word LiquidLevelTable[NUM_LQUID_LEV_VALUES][2] =
{
		// valore letto dal sensore di pressione mmHg, ml presenti nel recipiente
		105, 100,  // 108
		136, 200,  // 139
		153, 300,  // 156
		174, 400,  // 177
		188, 500,  // 191
		200, 600,  // 203
		206, 700,  // 209
		219, 800,  // 221
		223, 900,  // 226
		234, 1000,  // 237
		245, 1200,  // 248
		262, 1400,  // 265
		268, 1600,  // 271
		305, 1800,  // 308
		316, 2000,  // 319
		330, 2200,  // 333
		353, 2400,  // 356
		363, 2600,  // 366
		376, 2800,  // 379
		391, 3000   // 394
};
*/

/*
#define NUM_LQUID_LEV_VALUES 12

const word LiquidLevelTable[NUM_LQUID_LEV_VALUES][2] =
{
		379,250,
		420,500,
		452,750,
		479,1000,
		500,1250,
		522,1500,
		545,1750,
		567,2000,
		590,2250,
		612,2500,
		635,2750,
		657,3000,
};
*/
/*
#define NUM_LQUID_LEV_VALUES 56

const word LiquidLevelTable[NUM_LQUID_LEV_VALUES][2] =
{
		379,250,
		387,300,
		395,350,
		404,400,
		412,450,
		420,500,
		426,550,
		433,600,
		439,650,
		446,700,
		452,750,
		457,800,
		463,850,
		468,900,
		474,950,
		479,1000,
		483,1050,
		487,1100,
		492,1150,
		496,1200,
		500,1250,
		504,1300,
		509,1350,
		513,1400,
		518,1450,
		522,1500,
		527,1550,
		531,1600,
		536,1650,
		540,1700,
		545,1750,
		549,1800,
		554,1850,
		558,1900,
		563,1950,
		567,2000,
		572,2050,
		576,2100,
		581,2150,
		585,2200,
		590,2250,
		594,2300,
		599,2350,
		603,2400,
		608,2450,
		612,2500,
		617,2550,
		621,2600,
		626,2650,
		630,2700,
		635,2750,
		639,2800,
		644,2850,
		648,2900,
		653,2950,
		657,3000,
};

*/

// converte il valore mmhg moltiplicato per 10 in ml di contenuto nella baccinella
word ConvertMMHgToMl( word mmhg)
{
	word wd = 0;
	int i;
	for(i = 0; i < NUM_LQUID_LEV_VALUES; i++)
	{
		if(LiquidLevelTable[i][0] > mmhg)
		{
			if(i)
			{
				wd = LiquidLevelTable[i - 1][1];
			}
			else
				wd = 0;
			break;
		}

	}
	if(i == NUM_LQUID_LEV_VALUES)
		wd = 3000;
	return wd;
}





























