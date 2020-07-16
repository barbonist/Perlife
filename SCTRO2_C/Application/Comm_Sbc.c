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
#include "Alarm_Con.h"
#include "ControlProtectiveInterface_C.h"
#include "PC_DEBUG_COMM.h"
#include "SWTimer.h"

extern int PinchFilterCurrValue;
extern word MedForArteriousPid;
extern word MedForVenousPid;
extern typeAlarmS* GetCurrentAlarmActiveListAlm(void);
extern typeAlarmS* GetCurrentWarningActiveListWrn(void);

//Flag che tiene conto di eventuale CRC fallito
bool failedCRC = FALSE;

void buildModBusWriteRegActResponseMsg(char *ptrMsgSbcRx)
{
	byte index = 0;
	word wd;

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

	wd = ComputeChecksum(sbc_tx_data, index);

	/*  CRC H */
	sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/*  CRC L */
	sbc_tx_data[index++] = (wd     ) & 0xFF;

	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

void buildModBusReadRegActResponseMsg(char *ptrMsgSbcRx,
									  char slaveAddr,
									  unsigned int readStartAddr,
									  unsigned int numRegisterRead)
{
	byte index = 0;
	word wd;

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

	wd = ComputeChecksum(sbc_tx_data, index);

	/*  CRC H */
	sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/*  CRC L */
	sbc_tx_data[index++] = (wd     ) & 0xFF;
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

void buildModBusReadStatusResponseMsg(char *ptrMsgSbcRx)
{
	byte index = 0;
	word wd;

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

	wd = ComputeChecksum(sbc_tx_data, index);

	/*  CRC H */
	sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/*  CRC L */
	sbc_tx_data[index++] = (wd     ) & 0xFF;
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

void buildPressSensReadValuesResponseMsg(char *ptrMsgSbcRx)
{
    byte index = 0;
    word wd;

/*0*/	sbc_tx_data[index++] = 0xA5;
/*1*/	sbc_tx_data[index++] = 0xAA;
/*2*/	sbc_tx_data[index++] = 0x55;
/*3*/	sbc_tx_data[index++] = 0x00;
/*4*/	sbc_tx_data[index++] = 0x16;
/*5*/	sbc_tx_data[index++] = ptrMsgSbcRx[5];
/*6*/	sbc_tx_data[index++] = ptrMsgSbcRx[6];
/*7*/	sbc_tx_data[index++] = 0x66;
/*8*/	sbc_tx_data[index++] = 0x00; //TODO stable byte

/*9*/	sbc_tx_data[index++] = (PR_OXYG_mmHg_Filtered >> 8)  & 0xFF;
/*10*/	sbc_tx_data[index++] = PR_OXYG_mmHg_Filtered  & 0xFF;
/*11*/	sbc_tx_data[index++] = (PR_OXYG_ADC_Filtered >> 8)  & 0xFF;
/*12*/	sbc_tx_data[index++] = PR_OXYG_ADC_Filtered  & 0xFF;

/*13*/	sbc_tx_data[index++] = (PR_LEVEL_mmHg_Filtered >> 8)  & 0xFF;
/*14*/	sbc_tx_data[index++] = PR_LEVEL_mmHg_Filtered & 0xFF;
/*15*/	sbc_tx_data[index++] = (PR_LEVEL_ADC_Filtered >> 8)  & 0xFF;
/*16*/	sbc_tx_data[index++] = PR_LEVEL_ADC_Filtered  & 0xFF;

/*17*/	sbc_tx_data[index++] = (PR_ADS_FLT_mmHg_Filtered >> 8)  & 0xFF;
/*18*/	sbc_tx_data[index++] = (PR_ADS_FLT_mmHg_Filtered)  & 0xFF;
/*19*/	sbc_tx_data[index++] = (PR_ADS_FLT_ADC_Filtered >> 8)  & 0xFF;
/*20*/	sbc_tx_data[index++] = PR_ADS_FLT_ADC_Filtered  & 0xFF;

/*21*/  sbc_tx_data[index++] = (MedForVenousPid >> 8)  & 0xFF;
/*22*/	sbc_tx_data[index++] = (MedForVenousPid)  & 0xFF;
/*23*/	sbc_tx_data[index++] = (PR_VEN_ADC_Filtered >> 8)  & 0xFF;
/*24*/	sbc_tx_data[index++] = PR_VEN_ADC_Filtered  & 0xFF;

/*25*/	sbc_tx_data[index++] = (MedForArteriousPid >> 8)  & 0xFF;
/*26*/	sbc_tx_data[index++] = MedForArteriousPid  & 0xFF;
/*27*/	sbc_tx_data[index++] = (PR_ART_ADC_Filtered >> 8)  & 0xFF;
/*28*/	sbc_tx_data[index++] = PR_ART_ADC_Filtered  & 0xFF;

	/*
	for(int i = 0 ; i < 5 ; i++)
	{
		sbc_tx_data[index++] = ((word)sensor_PRx[i].prSensValue) >> 8;
		sbc_tx_data[index++] = ((word)sensor_PRx[i].prSensValue);
		sbc_tx_data[index++] = sensor_PRx[i].prSensAdc >> 8;
		sbc_tx_data[index++] = sensor_PRx[i].prSensAdc;
	}
*/
	wd = ComputeChecksum(sbc_tx_data, index);

	/*  CRC H */
	sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/*  CRC L */
	sbc_tx_data[index++] = (wd     ) & 0xFF;
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

void buildPressSensReadParamResponseMsg(char *ptrMsgSbcRx)
{
	word wd;
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

	wd = ComputeChecksum(sbc_tx_data, index);

	/*  CRC H */
	sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/*  CRC L */
	sbc_tx_data[index++] = (wd     ) & 0xFF;
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

// Filippo - costruisco la risposta al comando di calibrazione sonda PT1000
void buildPT1000CalibResponseMsg(char *ptrMsgSbcRx)
{
    byte index = 0;
    byte i;

    word wd;

	sbc_tx_data[index++] = 0xA5;
	sbc_tx_data[index++] = 0xAA;
	sbc_tx_data[index++] = 0x55;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x05;
	sbc_tx_data[index++] = ptrMsgSbcRx[5];
	sbc_tx_data[index++] = ptrMsgSbcRx[6];
	sbc_tx_data[index++] = 0x66;

	for (i=0;i<5;i++)
	{
		sbc_tx_data[index++]=0x55;	// valore inutile per raggiungere la dimensione di 16 byte
	}

	wd = ComputeChecksum(sbc_tx_data, index);

	/*  CRC H */
	sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/*  CRC L */
	sbc_tx_data[index++] = (wd     ) & 0xFF;
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;

}

// Filippo - funzione che risponde al comando di lettura parametro di calibrazione per il sensore PT1000
void buildPT1000SensReadParamResponseMsg(char *ptrMsgSbcRx)
{
	word wd;
    byte index = 0;
	int32 leggiOffset;

	sbc_tx_data[index++] = 0xA5;
	sbc_tx_data[index++] = 0xAA;
	sbc_tx_data[index++] = 0x55;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x29;
	sbc_tx_data[index++] = ptrMsgSbcRx[5];
	sbc_tx_data[index++] = ptrMsgSbcRx[6];
	sbc_tx_data[index++] = 0x66;

	leggiOffset=(int32)(config_data.T_Plate_Sensor_Offset_Heat*10);

	sbc_tx_data[index++] = 0;
	if (leggiOffset<0)
	{
		sbc_tx_data[index++] = 1;
		leggiOffset=-leggiOffset;
	}
	else
	{
		sbc_tx_data[index++] = 0;
	}
	sbc_tx_data[index++] = (unsigned char)((leggiOffset >> 8) & 0x000000FF);
	sbc_tx_data[index++] = (unsigned char)(leggiOffset & 0x000000FF);

	wd = ComputeChecksum(sbc_tx_data, index);

	/*  CRC H */
	sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/*  CRC L */
	sbc_tx_data[index++] = (wd     ) & 0xFF;
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;

}

void buildPressSensCalibResponseMsg(char *ptrMsgSbcRx)
{
    byte index = 0;
    word wd;

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


	wd = ComputeChecksum(sbc_tx_data, index);

	/*  CRC H */
	sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/*  CRC L */
	sbc_tx_data[index++] = (wd     ) & 0xFF;
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

void buildTempIRSensReadValuesResponseMsg(char *ptrMsgSbcRx)
{
	byte index = 0;
	word value = 0;
	word wd;

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

	value=(word)(T_PLATE_C_GRADI_CENT*10);
	sbc_tx_data[index++] = (value >> 8) & 0xFF;
	sbc_tx_data[index++] = (value     ) & 0xFF;

	wd = ComputeChecksum(sbc_tx_data, index);

	/*  CRC H */
	sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/*  CRC L */
	sbc_tx_data[index++] = (wd     ) & 0xFF;
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

void buildTempIRSensReadRegResponseMsg(char *ptrMsgSbcRx)
{
	byte index = 0;
	word value = 0;
	word wd;

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
	wd = ComputeChecksum(sbc_tx_data, index);

	/*  CRC H */
	sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/*  CRC L */
	sbc_tx_data[index++] = (wd     ) & 0xFF;
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

void buildTempIRSensWriteRegResponseMsg(char *ptrMsgSbcRx)
{
	byte index = 0;
	word wd;

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
	wd = ComputeChecksum(sbc_tx_data, index);

	/*  CRC H */
	sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/*  CRC L */
	sbc_tx_data[index++] = (wd     ) & 0xFF;
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

void buildReadFlowAirResponseMsg(char *ptrMsgSbcRx)
{
	byte index = 0;
	word wd;

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

	wd = ComputeChecksum(sbc_tx_data, index);

	/*  CRC H */
	sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/*  CRC L */
	sbc_tx_data[index++] = (wd     ) & 0xFF;
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

void buildReadFlowResetResponseMsg(char *ptrMsgSbcRx)
{
	byte index = 0;
	word wd;

	sbc_tx_data[index++] = 0xA5;
	sbc_tx_data[index++] = 0xAA;
	sbc_tx_data[index++] = 0x55;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x02;
	sbc_tx_data[index++] = ptrMsgSbcRx[5];
	sbc_tx_data[index++] = ptrMsgSbcRx[6];
	sbc_tx_data[index++] = 0x66;
	sbc_tx_data[index++] = ptrMsgSbcRx[7];
	wd = ComputeChecksum(sbc_tx_data, index);

	/*  CRC H */
	sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/*  CRC L */
	sbc_tx_data[index++] = (wd     ) & 0xFF;
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

void buildPeltierReadFloatResponseMsg(char *ptrMsgSbcRx, char *ieee754ptr)
{
	byte index = 0;
	word wd;

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
	wd = ComputeChecksum(sbc_tx_data, index);

	/*  CRC H */
	sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/*  CRC L */
	sbc_tx_data[index++] = (wd     ) & 0xFF;
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

void buildPeltierReadIntResponseMsg(char *ptrMsgSbcRx, char *intptr)
{
	byte index = 0;
	word wd;

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
	wd = ComputeChecksum(sbc_tx_data, index);

	/*  CRC H */
	sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/*  CRC L */
	sbc_tx_data[index++] = (wd     ) & 0xFF;
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

void buildPeltierWriteFloatResponseMsg(char *ptrMsgSbcRx)
{
	byte index = 0;
	word wd;

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
	wd = ComputeChecksum(sbc_tx_data, index);

	/*  CRC H */
	sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/*  CRC L */
	sbc_tx_data[index++] = (wd     ) & 0xFF;
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

void buildPeltierWriteIntResponseMsg(char *ptrMsgSbcRx)
{
	byte index = 0;
	word wd;

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
	wd = ComputeChecksum(sbc_tx_data, index);

	/*  CRC H */
	sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/*  CRC L */
	sbc_tx_data[index++] = (wd     ) & 0xFF;
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

void buildPeltierStartResponseMsg(char *ptrMsgSbcRx)
{
	byte index = 0;
	word wd;

	sbc_tx_data[index++] = 0xA5;
	sbc_tx_data[index++] = 0xAA;
	sbc_tx_data[index++] = 0x55;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x01;
	sbc_tx_data[index++] = ptrMsgSbcRx[5];
	sbc_tx_data[index++] = ptrMsgSbcRx[6];
	sbc_tx_data[index++] = 0x66;
	wd = ComputeChecksum(sbc_tx_data, index);

	/*  CRC H */
	sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/*  CRC L */
	sbc_tx_data[index++] = (wd     ) & 0xFF;
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

void buildPeltierStopResponseMsg(char *ptrMsgSbcRx)
{
	byte index = 0;
	word wd;

	sbc_tx_data[index++] = 0xA5;
	sbc_tx_data[index++] = 0xAA;
	sbc_tx_data[index++] = 0x55;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x01;
	sbc_tx_data[index++] = ptrMsgSbcRx[5];
	sbc_tx_data[index++] = ptrMsgSbcRx[6];
	sbc_tx_data[index++] = 0x66;
	wd = ComputeChecksum(sbc_tx_data, index);

	/*  CRC H */
	sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/*  CRC L */
	sbc_tx_data[index++] = (wd     ) & 0xFF;
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

void buildPeltierWriteEEResponseMsg(char *ptrMsgSbcRx)
{
	byte index = 0;
	word wd;

	sbc_tx_data[index++] = 0xA5;
	sbc_tx_data[index++] = 0xAA;
	sbc_tx_data[index++] = 0x55;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x01;
	sbc_tx_data[index++] = ptrMsgSbcRx[5];
	sbc_tx_data[index++] = ptrMsgSbcRx[6];
	sbc_tx_data[index++] = 0x66;
	wd = ComputeChecksum(sbc_tx_data, index);

	/*  CRC H */
	sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/*  CRC L */
	sbc_tx_data[index++] = (wd     ) & 0xFF;
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


word CRCVal;

void pollingSBCCommTreat(void){

	word valueWord = 0;
	word CRCReceived = 0;

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
				CRCVal = ComputeChecksum(sbc_rx_data, 7); // calcolo il CRC del messaggio ricevuto per controllare che sia giusto
			}
			break;

			/* Set parameter from SBC */
			case COMMAND_ID_PAR_SET:
			{
				unsigned char *ptr_CRC = &sbc_rx_data[0];
				valueWord = (sbc_rx_data[8] << 8) + sbc_rx_data[9];
				setParamWordFromGUI(sbc_rx_data[7],valueWord);
				ShowParameterStr(parameterWordSetFromGUI, sbc_rx_data[7]);

				myCommunicatorToSBC.dataParamSetSBCReadyFlag = DATA_COMM_READY_TO_BE_SEND;
				CRCVal = ComputeChecksum(ptr_CRC, 10); // calcolo il CRC del messaggio ricevuto per controllare che sia giusto
				CRCReceived = (sbc_rx_data[10] << 8) + sbc_rx_data[11];
				if (CRCReceived != CRCVal)
				{
					  failedCRC = TRUE;
				}
				else
				{
					  failedCRC = FALSE;
				}
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
				CRCVal = ComputeChecksum(sbc_rx_data, 9);  // calcolo il CRC del messaggio ricevuto per controllare che sia giusto

				if(buttonGUITreatment[BUTTON_PRIMING_ABANDON].state == GUI_BUTTON_RELEASED)
				{
					int ii = 0;
				}

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
	// contatore usato per determinare il numero si status inviati con allarme a 0
	// dopo che ho ricevuto un BUTTON_ALARM_RESET per ripartire dopo un allarme
	static unsigned char StatrespCnt = 0;
	word wd;
	byte index = 0;
	unsigned int life = FreeRunCnt10msec *10;

	if (SBC_Control_communication_OK == FALSE)
		SBC_Control_communication_OK = TRUE;

	/*0*/	sbc_tx_data[index++] = 0xA5;
	/*1*/   sbc_tx_data[index++] = 0xAA;
	/*2*/   sbc_tx_data[index++] = 0x55;
	/*3*/	sbc_tx_data[index++] = 0x00;
	/* byte count (esclusi i primi 4): 95 byte */
	/*4*/	sbc_tx_data[index++] = STATUS_MSG_LENGTH;
	/* command id */
	/*5*/	sbc_tx_data[index++] = code;
	/* command id */
	/*6*/	sbc_tx_data[index++] = subcode;
	/* what is the meaning of this byte? */
	/*7*/	sbc_tx_data[index++] = 0x66;

#ifdef DEBUG_SBC

	for (word i = 0; i<= 63 ; i++)
	{
		if (i != 3 && i != 4 && i != 5)
		{
			sbc_tx_data[index++] = (i >> 8) & 0xFF;
			sbc_tx_data[index++] = i & 0xFF;
		}
		else
		{
			sbc_tx_data[index++] = (0 >> 8) & 0xFF;
			sbc_tx_data[index++] = 0 & 0xFF;
		}

	}


#else

	/* STATUS PARAMETERS */
	/* status parameters: life  */
	/*8*/	sbc_tx_data[index++] = (life >> 8) & 0xFF;
	/*9*/	sbc_tx_data[index++] = life & 0xFF;
	/* status parameters: rev fw-H
	 * potrei usare questa come versione FW
	 * della CON usando 5 bit per il primo numero
	 * 5 bit per il seocndo numero e 6 bit per il
	 * terzo numero avendo quindi una versione massima
	 * pari a 32.32.64  */

	/*0x0800 --> 1.0.0
	 *0x0801 --> 1.0.1
	 *0x0802 --> 1.0.2
	 *0x0803 --> 1.0.3
	 *0x0804 --> 1.0.4  */

	wd = REVISION_FW_CONTROL;

	/*10*/	sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/*11*/	sbc_tx_data[index++] = (wd     ) & 0xFF;

	/* TODO status parameters: rev fw-H
	 * potrei usare questa come versione FW
	 * della PRO usando 5 bit per il primo numero
	 * 5 bit per il seocndo numero e 6 bit per il
	 * terzo numero avendo quindi una versione massima
	 * pari a 32.32.64  */

	/*0x0800 --> 1.0.0
     *0x0801 --> 1.0.1  */
	wd = GetRevisionFWProt();

	/*12*/  sbc_tx_data[index++] = (wd >> 8) & 0xFF; //sbc_tx_data[index++] = 0x08;
	/*13*/  sbc_tx_data[index++] = (wd     ) & 0xFF; //sbc_tx_data[index++] = 0x01;

//	if(SuspendInvioAlarmCode)
//	{
//		StatrespCnt++;
//		/* status parameters: alarm code */
//		/*14*/	sbc_tx_data[index++] = 0;
//		/*15*/	sbc_tx_data[index++] = 0;
//		/* status parameters: alarm physic */
//		/*16*/	sbc_tx_data[index++] = 0;
//		/*17*/	sbc_tx_data[index++] = 0;
//		/* status parameters: alarm type */
//		/*18*/	sbc_tx_data[index++] = 0;
//		/*19*/  sbc_tx_data[index++] = 0;
//		if(StatrespCnt >= 3)
//		{
//			StatrespCnt = 0;
//			SuspendInvioAlarmCode = 0;
//		}
//	}
//	else
//	{

		/* status parameters: alarm code  se GetAlarmCodeProt() != 0
		 * oppre GetAlarmCodeProt() dando priorità alla visualizzazione
		 * degli alalrmi protective*/
		if (GetAlarmCodeProt() != 0)
			wd = GetAlarmCodeProt();
		else if (LengthActiveListAlm() > 0)
			wd = GetCurrentAlarmActiveListAlm()->code;
		else if (LengthActiveListWrn() > 0)
			wd = GetCurrentWarningActiveListWrn()->code;
		else
			wd = 0;

	    /*14*/	sbc_tx_data[index++] = ( wd >> 8) & 0xFF; // (alarmCurrent.code >> 8 ) & 0xFF;
		/*15*/	sbc_tx_data[index++] = ( wd )     & 0xFF; // (alarmCurrent.code 	    ) & 0xFF;
		/* status parameters: alarm physic */
		/*16*/	sbc_tx_data[index++] = (alarmCurrent.active >> 8 ) & 0xFF;
		/*17*/	sbc_tx_data[index++] = (alarmCurrent.active      ) & 0xFF;
		/* status parameters: alarm type */
		/*18*/	sbc_tx_data[index++] = (alarmCurrent.type >> 8 ) & 0xFF;
		/*19*/  sbc_tx_data[index++] = (alarmCurrent.type      ) & 0xFF;
//	}

	if(ptrCurrentState->state == STATE_WAIT_TREATMENT)
		wd = 0;
	/* status parameters: machine state state*/
	/*20*/	sbc_tx_data[index++] = (ptrCurrentState->state >> 8) & 0xFF;
	/*21*/	sbc_tx_data[index++] = (ptrCurrentState->state     ) & 0xFF;
	/* status parameters: machine state parent*/
	/*22*/	sbc_tx_data[index++] = (ptrCurrentParent->parent >> 8) & 0xFF;
	/*23*/	sbc_tx_data[index++] = (ptrCurrentParent->parent     ) & 0xFF;
	/* status parameters: machine state child*/
	/*24*/	sbc_tx_data[index++] = (ptrCurrentChild->child >> 8) & 0xFF;
	/*25*/	sbc_tx_data[index++] = (ptrCurrentChild->child     ) & 0xFF;
	/* STATUS PARAMETERS */

	/* SENS PARAMETERS */
	/* sensors parameters: pressure adsorbent filter */

	/*26*/	sbc_tx_data[index++] = (PR_ADS_FLT_mmHg_Filtered >> 8) & 0xFF;
	/*27*/	sbc_tx_data[index++] = (PR_ADS_FLT_mmHg_Filtered) & 0xFF;
	/* sensors parameters: pressure arterial */

	/*28*/	sbc_tx_data[index++] = (PR_ART_mmHg_Filtered >> 8) & 0xFF;  // PR_ART_Sistolyc_mmHg
	/*29*/	sbc_tx_data[index++] = (PR_ART_mmHg_Filtered) & 0xFF;       // PR_ART_Sistolyc_mmHg
	/* sensors parameters: pressure venous */

	/*30*/	sbc_tx_data[index++] = (MedForVenousPid >> 8) & 0xFF;
	/*31*/	sbc_tx_data[index++] = (MedForVenousPid) & 0xFF;
	/* sensors parameters: pressure oxygenation */

	/*32*/	sbc_tx_data[index++] = (PR_OXYG_mmHg_Filtered >> 8) & 0xFF;
	/*33*/	sbc_tx_data[index++] = (PR_OXYG_mmHg_Filtered) & 0xFF;
	/* sensors parameters: pressure level */

	/*34*/	sbc_tx_data[index++] = (LiquidAmount >> 8) & 0xFF; // PR_LEVEL_mmHg_Filtered
	/*35*/	sbc_tx_data[index++] = (LiquidAmount) & 0xFF;      // PR_LEVEL_mmHg_Filtered

	/* sensors parameters: pressure systolic arterial */
	/*36*/	sbc_tx_data[index++] = (PR_ART_Sistolyc_mmHg >> 8) & 0xFF;
	/*37*/	sbc_tx_data[index++] = (PR_ART_Sistolyc_mmHg     ) & 0xFF;
	/* sensors parameters: pressure diastolic arterial */
	/*38*/	sbc_tx_data[index++] = (PR_ART_Diastolyc_mmHg >> 8) & 0xFF;
	/*39*/	sbc_tx_data[index++] = (PR_ART_Diastolyc_mmHg     ) & 0xFF;
	/* sensors parameters: pressure mean arterial */

	/*40*/	sbc_tx_data[index++] = (PR_ART_Med_mmHg >> 8) & 0xFF;
	/*41*/  sbc_tx_data[index++] = (PR_ART_Med_mmHg     ) & 0xFF;
	/* sensors parameters: pressure systolic venous */
	/*42*/  sbc_tx_data[index++] = (PR_VEN_Sistolyc_mmHg >> 8) & 0xFF;
	/*43*/  sbc_tx_data[index++] = (PR_VEN_Sistolyc_mmHg     ) & 0xFF;
	/* sensors parameters: pressure diastolic venous */
	/*44*/  sbc_tx_data[index++] = (PR_VEN_Diastolyc_mmHg >> 8) & 0xFF;
	/*45*/  sbc_tx_data[index++] = (PR_VEN_Diastolyc_mmHg     ) & 0xFF;
	/* sensors parameters: pressure mean venous */
	/*46*/  sbc_tx_data[index++] = (PR_VEN_Med_mmHg >> 8) & 0xFF;
	/*47*/  sbc_tx_data[index++] = (PR_VEN_Med_mmHg     ) & 0xFF;

	/* sensors parameters: pressure flow arterial */
	/*48*/  sbc_tx_data[index++] = (((int)sensor_UFLOW[0].Average_Flow_Val_for_GUI) >> 8) & 0xFF;  // sensorsValues.flowArt
	/*49*/  sbc_tx_data[index++] = ((int)sensor_UFLOW[0].Average_Flow_Val_for_GUI) & 0xFF;
	/* sensors parameters: pressure flow venous / oxygenation */
	/*50*/  sbc_tx_data[index++] = (((int)sensor_UFLOW[1].Average_Flow_Val_for_GUI) >> 8) & 0xFF;  // sensorsValues.flowVenOxy
	/*51*/  sbc_tx_data[index++] = ((int)sensor_UFLOW[1].Average_Flow_Val_for_GUI) & 0xFF;
	/* sensors parameters: temperature reservoir outlet */
	/*52*/  sbc_tx_data[index++] = (((int)(sensorIR_TM[1].tempSensValue*10)) >> 8) & 0xFF;
	/*53*/  sbc_tx_data[index++] = (((int)(sensorIR_TM[1].tempSensValue*10))     ) & 0xFF;
	/* sensors parameters: temperature organ inlet */
	/*54*/  sbc_tx_data[index++] = (((int)(sensorIR_TM[0].tempSensValue*10)) >> 8) & 0xFF;
	/*55*/  sbc_tx_data[index++] = (((int)(sensorIR_TM[0].tempSensValue*10))     ) & 0xFF;
	/* sensors parameters: temperature venous / oxygenation */
	/*56*/  sbc_tx_data[index++] = (((int)(sensorIR_TM[2].tempSensValue*10)) >> 8) & 0xFF;
	/*57*/  sbc_tx_data[index++] = (((int)(sensorIR_TM[2].tempSensValue*10))     ) & 0xFF;
	/* SENS PARAMETERS */

	/* PERFUSION PARAMETERS */
	/* perfusion parameters: priming volume adsorbent filter */

	/*58*/  sbc_tx_data[index++] = (perfusionParam.priVolAdsFilter >> 8) & 0xFF;
	/*59*/  sbc_tx_data[index++] = (perfusionParam.priVolAdsFilter     ) & 0xFF;

	/* perfusion parameters: priming volume perfusion arterial */
	wd = perfusionParam.priVolPerfArt;
	if(wd <= VOLUME_DISPOSABLE)
		wd = 0;
	else
		wd = wd - VOLUME_DISPOSABLE;
	/*60*/  sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/*61*/  sbc_tx_data[index++] = (wd     ) & 0xFF;
	/* perfusion parameters: priming volume perfusion venous / oxygenation */
	/*62*/  sbc_tx_data[index++] = (perfusionParam.priVolPerfVenOxy >> 8) & 0xFF;
	/*63*/  sbc_tx_data[index++] = (perfusionParam.priVolPerfVenOxy     ) & 0xFF;
	/* perfusion parameters: priming duration perfusion arterial */
	/*64*/  sbc_tx_data[index++] = (perfusionParam.priDurPerfArt >> 8) & 0xFF;  // espressa in secondi
	/*65*/  sbc_tx_data[index++] = (perfusionParam.priDurPerfArt     ) & 0xFF;
	/* perfusion parameters: priming duration perfusion venous / oxygenation */
	/*66*/  sbc_tx_data[index++] = (perfusionParam.priDurPerVenOxy >> 8) & 0xFF; // espressa in secondi
	/*67*/  sbc_tx_data[index++] = (perfusionParam.priDurPerVenOxy     ) & 0xFF;
	/* perfusion parameters: treatment volume adsorbent filter */
	/*68*/  sbc_tx_data[index++] = (perfusionParam.treatVolAdsFilter >> 8) & 0xFF;
	/*69*/  sbc_tx_data[index++] = (perfusionParam.treatVolAdsFilter     ) & 0xFF;
	/* perfusion parameters: treatment volume perfusion arterial */
	/*70*/  sbc_tx_data[index++] = (perfusionParam.treatVolPerfArt >> 8) & 0xFF;    // espressa in ml
	/*71*/  sbc_tx_data[index++] = (perfusionParam.treatVolPerfArt     ) & 0xFF;
	/* perfusion parameters: treatment volume perfusion venous / oxygenation */
	/*72*/  sbc_tx_data[index++] = (perfusionParam.treatVolPerfVenOxy >> 8) & 0xFF; // espressa in ml
	/*73*/  sbc_tx_data[index++] = (perfusionParam.treatVolPerfVenOxy     ) & 0xFF;
	/* perfusion parameters: treatment duration perfusion arterial */
	/*74*/  sbc_tx_data[index++] = (perfusionParam.treatDurPerfArt >> 8) & 0xFF;  // espressa in secondi
	/*75*/  sbc_tx_data[index++] = (perfusionParam.treatDurPerfArt     ) & 0xFF;
	/* perfusion parameters: treatment duration perfusion venous / oxygenation */
	/*76*/  sbc_tx_data[index++] = (perfusionParam.treatDurPerVenOxy >> 8) & 0xFF; // espressa in secondi
	/*77*/  sbc_tx_data[index++] = (perfusionParam.treatDurPerVenOxy     ) & 0xFF;

	/* perfusion parameters: unload volume adsorbent filter */
	/*78*/  sbc_tx_data[index++] = (perfusionParam.unlVolAdsFilter >> 8) & 0xFF;
	/*79*/  sbc_tx_data[index++] = (perfusionParam.unlVolAdsFilter     ) & 0xFF;
	/* perfusion parameters: unload volume perfusion arterial */
	/*80*/  sbc_tx_data[index++] = (perfusionParam.unlVolPerfArt >> 8) & 0xFF;
	/*81*/  sbc_tx_data[index++] = (perfusionParam.unlVolPerfArt     ) & 0xFF;
	/* perfusion parameters: unload volume perfusion venous / oxygenation */
	/*82*/  sbc_tx_data[index++] = (perfusionParam.unlVolPerfVenOxy >> 8) & 0xFF;
	/*83*/  sbc_tx_data[index++] = (perfusionParam.unlVolPerfVenOxy     ) & 0xFF;
	/* perfusion parameters: unload volume reservoir */
	/*84*/  sbc_tx_data[index++] = (perfusionParam.unlVolRes >> 8) & 0xFF;
	/*85*/  sbc_tx_data[index++] = (perfusionParam.unlVolRes     ) & 0xFF;
	/* perfusion parameters: unload duration perfusion arterial */
	/*86*/  sbc_tx_data[index++] = (perfusionParam.unlDurPerfArt >> 8) & 0xFF;
	/*87*/  sbc_tx_data[index++] = (perfusionParam.unlDurPerfArt     ) & 0xFF;
	/* perfusion parameters: unload duration perfusion venous / oxygenation */
	/*88*/  sbc_tx_data[index++] = (perfusionParam.unlDurPerVenOxy >> 8) & 0xFF;
	/*89*/  sbc_tx_data[index++] = (perfusionParam.unlDurPerVenOxy     ) & 0xFF;

	/* perfusion parameters: renal resistence */
	/*90*/  sbc_tx_data[index++] = (perfusionParam.renalResistance >> 8) & 0xFF;
	/*91*/  sbc_tx_data[index++] = (perfusionParam.renalResistance     ) & 0xFF;

	/* perfusion parameters: pulsatility */
	/*92*/  sbc_tx_data[index++] = (perfusionParam.pulsatility >> 8) & 0xFF;
	/*93*/  sbc_tx_data[index++] = (perfusionParam.pulsatility     ) & 0xFF;
	/* perfusion parameters: pressure drop adsorbent filter */
	/*94*/  sbc_tx_data[index++] = (perfusionParam.pressDropAdsFilter >> 8) & 0xFF;
	/*95*/  sbc_tx_data[index++] = (perfusionParam.pressDropAdsFilter     ) & 0xFF;
	/* PERFUSION PARAMETERS */

#ifdef NEW_STATE_RESPONSE_MSG
	/* programmed therapy type */
	wd = (word)GetTherapyType();
	/*96*/  sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/*97*/  sbc_tx_data[index++] = (wd     ) & 0xFF;
	/* programmed priming volume perfusion (ml)*/
	wd = parameterWordSetFromGUI[PAR_SET_PRIMING_VOL_PERFUSION].value;
	/*98*/  sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/*99*/  sbc_tx_data[index++] = (wd     ) & 0xFF;
	/* programmed priming temperature perfusion (gradi Centigradi * 10)*/
	wd = parameterWordSetFromGUI[PAR_SET_PRIMING_TEMPERATURE_PERFUSION].value;
	/*100*/  sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/*101*/  sbc_tx_data[index++] = (wd     ) & 0xFF;

	/* FREE */
	wd = 0;
	/*102*/  sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/*103*/  sbc_tx_data[index++] = (wd     ) & 0xFF;

	/* programmed oxygenation flow  ml/min*/
	wd = parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value;
	/*104*/  sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/*105*/  sbc_tx_data[index++] = (wd     ) & 0xFF;
	/* programmed oxygenetaion active */
	wd = parameterWordSetFromGUI[PAR_SET_OXYGENATOR_ACTIVE].value;
	/*oxygenation is bit 2-3*/
	wd = wd << 2;
	/* programmed depuration active is in bit 0-1*/
	wd = wd | parameterWordSetFromGUI[PAR_SET_DEPURATION_ACTIVE].value;
	/*106*/  sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/*107*/  sbc_tx_data[index++] = (wd     ) & 0xFF;
	/* programmed target arterial pressure (mmHg)*/
	wd = parameterWordSetFromGUI[PAR_SET_PRESS_ART_TARGET].value;
	/*108*/  sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/*109*/  sbc_tx_data[index++] = (wd     ) & 0xFF;
	/* programmed desired duration (MSB H (hours) LSB M minutes*/
	wd = parameterWordSetFromGUI[PAR_SET_DESIRED_DURATION].value;
	/*110*/  sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/*111*/  sbc_tx_data[index++] = (wd     ) & 0xFF;
	/* programmed flow max perfusion arterial ml/min*/
	wd = parameterWordSetFromGUI[PAR_SET_MAX_FLOW_PERFUSION].value;
	/*112*/  sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/*113*/  sbc_tx_data[index++] = (wd     ) & 0xFF;
	/* programmed target venous pressure (mmHg)*/
	wd = parameterWordSetFromGUI[PAR_SET_VENOUS_PRESS_TARGET].value;
	/*114*/  sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/*115*/  sbc_tx_data[index++] = (wd     ) & 0xFF;
	/* programmed temperature (gradi Centigradi * 10)*/
	wd = parameterWordSetFromGUI[PAR_SET_PRIMING_TEMPERATURE_PERFUSION].value;
	/*116*/  sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/*117*/  sbc_tx_data[index++] = (wd     ) & 0xFF;
	// durata del priming calcolata
	/*118*/  sbc_tx_data[index++] = (ExpectedPrimDuration >> 8) & 0xFF;
	/*119*/  sbc_tx_data[index++] = (ExpectedPrimDuration     ) & 0xFF;
	/*Nel caso trattamento Kidney questo flusso coincide col flusso arteriso
	 * altrimenti è perfusionParam.priVolAdsFilter*/
	if (GetTherapyType() == KidneyTreat)
	{
    /*copio il valore dei byte 48 e 49 che sono flusso arterioso*/
	/*120*/ sbc_tx_data[index++] = sbc_tx_data[48];
	/*121*/ sbc_tx_data[index++] = sbc_tx_data[49];
	}
	else //if (GetTherapyType() == LiverTreat)
	{
	/*120*/  sbc_tx_data[index++] = ((word)FilterFlowVal >> 8) & 0xFF;  // flusso nel filtro in ml/min (!= 0 solo se il filtro e' presente)
	/*121*/  sbc_tx_data[index++] = ((word)FilterFlowVal     ) & 0xFF;
	}
	wd = 0;
	/*122*/  sbc_tx_data[index++] = (TimeoutAirEjection >> 8) & 0xFF;  // flag di timeout nell'espulsione dell'aria
	/*123*/  sbc_tx_data[index++] = (TimeoutAirEjection     ) & 0xFF;
	wd = (word)PinchFilterCurrValue;
	/*124*/  sbc_tx_data[index++] = (wd >> 8) & 0xFF;                  // posizione corrente della pinch del filtro
	/*125*/  sbc_tx_data[index++] = (wd     ) & 0xFF;
	wd = (word)OxygenFlowRpm;
	/*126*/  sbc_tx_data[index++] = (wd >> 8) & 0xFF;                  // flusso ossigenazione calcolato in base a rpm
	/*127*/  sbc_tx_data[index++] = (wd     ) & 0xFF;
	/* rsistenza renale venosa*/
	wd = perfusionParam.renalResistanceVenous;
	/*128*/  sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/*129*/  sbc_tx_data[index++] = (wd     ) & 0xFF;
	/* pulsatilità venosa*/
	wd = perfusionParam.pulsatilityVenous;
	/*130*/  sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/*131*/  sbc_tx_data[index++] = (wd     ) & 0xFF;
	// Filippo - metto la necessità o meno di spegnere il PC
	if (buttonGUITreatment[BUTTON_SHUT_DOWN_PC].state==GUI_BUTTON_RELEASED)
	{
		wd=1;
	}
	else
	{
		wd=(word)EMERGENCY_BUTTON_ACTIVATION_PC;
	}
	/*132*/  sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/*133*/  sbc_tx_data[index++] = (wd     ) & 0xFF;
	/*set point flusso depurazione usato solo per il liver*/
	if (GetTherapyType() == LiverTreat)
		wd = parameterWordSetFromGUI[PAR_SET_PURIF_FLOW_TARGET].value;
	else // if (GetTherapyType() == KidneyTreat)
		wd = 0;
	/*134*/  sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/*135*/  sbc_tx_data[index++] = (wd     ) & 0xFF;

#endif //NEW_STATE_RESPONSE_MSG

#endif	//DEBUG_SBC
	wd = ComputeChecksum(sbc_tx_data, index);
	/* CRC H */
	/*136*/  sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/* CRC L */
	/*137*/  sbc_tx_data[index++] = (wd     ) & 0xFF;
	/* End */
	/*138*/  sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

void buildButtonSBCResponseMsg(char code, char subcode, unsigned char buttonId)
{
	byte index = 0;
	word wd;


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

	wd = ComputeChecksum(sbc_tx_data, index);
	/* CRC H */
	sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/* CRC L */
	sbc_tx_data[index++] = (wd     ) & 0xFF;
	/* End */
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

void buildParamSetSBCResponseMsg(char code, char subcode, unsigned char paramId, unsigned char param_h, unsigned char param_l)
{
	byte index = 0;
	word wd;

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

	wd = ComputeChecksum(sbc_tx_data,index);

	/*  CRC H */
	sbc_tx_data[index++] = (wd >> 8) & 0xFF;
	/*  CRC L */
	sbc_tx_data[index++] = (wd     ) & 0xFF;
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
void setGUIButton(unsigned char buttonId)
{
	static char ButtonStr[40];

	buttonGUITreatment[buttonId].state = GUI_BUTTON_RELEASED;

	if (buttonId ==	BUTTON_RESET_ALARM && failedCRCState == ACTIVATED)
		failedCRCState = DEACTIVATED;

	/*tengo traccia della pressione del tasto di stop col flag ButtonStopPressed (serve per la gestione dello sbollamento) */
	if (buttonId == BUTTON_STOP_TREATMENT || buttonId == BUTTON_STOP_PRIMING)
	{
		ButtonStopPressed =TRUE;
	}
	else if (buttonId == BUTTON_START_TREATMENT || buttonId == BUTTON_START_PRIMING)
	{
		ButtonStopPressed = FALSE;
	}
	/*se mi è arrivato il primo start treatment faccio la tara delle pressioni*/
    if (buttonId == BUTTON_START_TREATMENT && TARA_PRESS_DONE == FALSE && ptrCurrentState->state == STATE_TREATMENT_KIDNEY_1)
	{
		/*calcolo la tara della pressione arteriosa dopo la connessione dell'organo*/
		PR_ART_TARA_mmHg = PR_ART_mmHg_Filtered;

		/*impongo che la tara sia non negativa*/
		if (PR_ART_TARA_mmHg < 0)
			PR_ART_TARA_mmHg = 0;

		/*calcolo la tara della pressione venosa dopo la connessione dell'organo*/
		PR_VEN_TARA_mmHg = PR_VEN_mmHg_Filtered;

		/*impongo che la tara sia non negativa*/
		if (PR_VEN_TARA_mmHg < 0)
			PR_VEN_TARA_mmHg = 0;

		Soglia_minima_ADC_allarme_Livello = (signed long int)PR_LEVEL_ADC_Filtered - SogliaVaschettaVuotaADC;
		/*su questo delta calcolo la percentuale al di sotto della quale dare allarme*/
		Soglia_minima_ADC_allarme_Livello = Soglia_minima_ADC_allarme_Livello * Percentuale_Massima_perdita_liquido;
		/*Se dopo il calcolo dela percentuale ottengo un valore di ADC inferiore a SOGLIA_ADC_MINIMA, lo blocco a SOGLIA_ADC_MINIMA*/
		if (Soglia_minima_ADC_allarme_Livello < SOGLIA_ADC_MINIMA)
			Soglia_minima_ADC_allarme_Livello = SOGLIA_ADC_MINIMA;
		/*trovato il valore percentuale di ADC calcolo la soglia minima di ADC rispetto al valore di ADC dopo il priming*/
		Soglia_minima_ADC_allarme_Livello = (signed long int)PR_LEVEL_ADC_Filtered - Soglia_minima_ADC_allarme_Livello;

		TARA_PRESS_DONE = TRUE;
		TARA_LIVELLO_DONE = TRUE;
	}

//BSW8 START:

	/*Se mi arriva uno stop, devo
	 * memorizzare lo stato delle pinch
	 * memorizzare che mi è arrivatyo uno stop
	 * chiudere tutte le pinch*/
	if (
		(buttonId == BUTTON_STOP_PRIMING 		||
		 buttonId == BUTTON_STOP_TREATMENT 		||
		 buttonId == BUTTON_STOP_EMPTY_DISPOSABLE
		)
		&& Stop_Button_clicked == FALSE
	   )
	{
		/*memorizzo l'ultima posizione comandata sulle pinch nella variabile pinchActuatorLastCommand[i].pinchPosTarget*/
		for (int i = 0; i<3; i++)
			pinchActuatorLastCommand[i].pinchPosTarget = pinchActuator[i].pinchPosTarget;

		/*chiudo tutte le pinch*/
		setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_POS_CLOSED);
		setPinchPositionHighLevel(PINCH_2WPVA, MODBUS_PINCH_POS_CLOSED);
		setPinchPositionHighLevel(PINCH_2WPVV, MODBUS_PINCH_POS_CLOSED);

		/*tengo traccia che sia avvenuto uno stop*/
		Stop_Button_clicked = TRUE;

	}
	/*Se mi arriva uno start (priming -- Treatment -- Empty)
	 *  succsasivo ad uno stop devo rimettere nella posizione salvata dopo lo stop*/
	else if  (
			   (buttonId == BUTTON_START_PRIMING 		||
			    buttonId == BUTTON_START_TREATMENT 		||
		        buttonId == BUTTON_START_EMPTY_DISPOSABLE
		        )
				&& Stop_Button_clicked == TRUE
			 )
	{
		/*rimetto le pinch a posto*/
		setPinchPositionHighLevel(PINCH_2WPVF, pinchActuatorLastCommand[0].pinchPosTarget);
		setPinchPositionHighLevel(PINCH_2WPVA, pinchActuatorLastCommand[1].pinchPosTarget);
		setPinchPositionHighLevel(PINCH_2WPVV, pinchActuatorLastCommand[2].pinchPosTarget);

		/*tengo traccia che sia avvenuto uno start*/
			Stop_Button_clicked = FALSE;
	}


/*invio l'eco sulla seriale di servizio del comando ricevuto*/
	sprintf(ButtonStr , "\r %s - ButtonId = %d \n", GetRTCVal() , buttonId);
	uint16_t sent_size;
	PC_DEBUG_COMM_SendBlock(ButtonStr , strlen(ButtonStr) , &sent_size); //SB 7-2020

	/*for(int i=0; i<10; i++)
	{
		if(Button[i])
			PC_DEBUG_COMM_SendChar(Button[i]);
		else
			break;
	}
	PC_DEBUG_COMM_SendChar(0x0A);
	*/

//BSW8 STOP:

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
	parameterWordSetFromGUI[PAR_SET_VENOUS_PRESS_TARGET].value = 0;
}
void initSetParamInSourceCode(void)
{
	// QUESTA FUNZIONE ORA NON SERVE PIU' PERCHE' I DATI DEVONO ESSERE INVIATi DALLA GUI
	// SOLO SE SONO IN SERVICE DEVO IMPOSTARE PAR_SET_VENOUS_PRESS_TARGET PERCHE' NON C'E' NELLA USER INTERFACE
	if(Service)
		parameterWordSetFromGUI[PAR_SET_VENOUS_PRESS_TARGET].value = 5;
	//parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value = 2000;
}

void setParamWordFromGUI(unsigned char parId, int value)
{
	static char ParamValue[30];

	if (parId == PAR_SET_THERAPY_TYPE)
	{
		if (value == LiverTreat || value == KidneyTreat)
		{
			SetTherapyType(value);
			/*aggiorno la mappatura degli indirizzi logici delle pompe
			 * tutte le volte che ricevo una terapia; viene fatto anche
			 * in computeMachineStateGuardIdle ma di li, dopo la prima
			 * selezione della terapia non ci passa più*/
			UpdatePmpAddress((THERAPY_TYPE)value);
		}
	}
	if(parId == PAR_SET_PRIMING_VOL_PERFUSION)
	{
		// controllo range del volume di liquido caricato nel reservoir
		if(GetTherapyType() == LiverTreat && value > MAX_VOLUME_RESERVOIR_THERAPY_LIVER)
		{
			value = MAX_VOLUME_RESERVOIR_THERAPY_LIVER;
		}
		if(GetTherapyType() == KidneyTreat && value > MAX_VOLUME_RESERVOIR_THERAPY_KIDNEY)
		{
			value = MAX_VOLUME_RESERVOIR_THERAPY_KIDNEY;
		}

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
	/*durata del trattamento*/
	else if (parId == PAR_SET_DESIRED_DURATION)
	{
		/*variabili in cui memorizzare i minuti e le ore già
		 * fatte nel caso in cui ci sia un proseguimento di trattamento
		 * dopo che un trattamento è finito*/
		int minuti_da_fare = 0;

		/*Dal valore che mi è arrivato 'value' in ore--minuti calcolo i secondi*/
		unsigned long ival = (unsigned long)(value >> 8);
		unsigned long isec;
		isec = ival * 3600L;      // numero di ore
		ival = (unsigned long)(value & 0xff);
		isec = isec + ival * 60L; // numero di minuti

		/*ci salviamo la durata del trattamento fatto
		 * nel caso si arrivi qui come proseguimento di un trattamento
		 * che era arrivato alla fine*/
		TotalTreatDuration += TreatDuration;
		TreatDuration = 0;

		/*Alla quantità di secondi già eseguiti sommo la quantità di seocndi che mi è arrivata*/
		isec += TotalTreatDuration;

		/*calcolo i minuti da fare dividendo i seondi per 60 */
		minuti_da_fare = isec / 60;

		/*dividendo ulteriormente i minuti per 60 e prendendo
		 * la parte intera trovo le ore totali da fare*/
		value = minuti_da_fare / 60;

		/*metto le ore da afre nella parte alta della word come da protocollo*/
		value = value << 8;
		/*nella parte bassa della word metto i minuti da fare calcolati come
		 * resto della divisione per 60 dei minuti da fare*/
		value = value + (minuti_da_fare%60);
	}
	parameterWordSetFromGUI[parId].value = value;

	if(parId == PAR_SET_PRIMING_VOL_PERFUSION)
	{
		int timesec;
		word vol = GetTotalPrimingVolumePerf(0);
		timesec =  CalcPrimingDuration(vol);
		ExpectedPrimDuration = CalcHoursMin(timesec);
	}

	/*invio l'eco sulla seriale di servizio del parametro e del valore ricevuto*/
	sprintf(ParamValue, "\r %d; %i ", parId, value);

	for(int i=0; i<30; i++)
	{
		if(ParamValue[i])
			PC_DEBUG_COMM_SendChar(ParamValue[i]);
		else
			break;
	}
	PC_DEBUG_COMM_SendChar(0x0A);

//	// TODO DA RIMUOVERE SOLO PER DEBUG GUI !!!!
//	parameterWordSetFromGUI[PAR_SET_DESIRED_DURATION].value = 0x0000; // due ore
//	parameterWordSetFromGUI[PAR_SET_DESIRED_DURATION].value = 0x0002; // 2 minuto
}

void resetParamWordFromGUI(unsigned char parId){
	parameterWordSetFromGUI[parId].value = 0;
}


/*
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
*/


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

// ritorna il tempo previsto per completare il priming espresso in secondi
int CalcPrimingDuration(word volume)
{
	float pump_speed;
	float mlpersecArt, mlpersecOxy;
	float vol;
	int timeSec;

	pump_speed = (float)RPM_IN_PRIMING_PHASES / 100.0;
	mlpersecArt = pump_speed * CONV_RPMMIN_TO_ML_PER_SEC;
	// tempo per caricare i primi 400 ml
	timeSec = (int)(((float)MIN_LIQ_IN_RES_TO_START_OXY_VEN / mlpersecArt) + 0.5);

	if(volume > MIN_LIQ_IN_RES_TO_START_OXY_VEN)
	{
		if(GetTherapyType() == LiverTreat)
			pump_speed = (int)LIVER_PRIMING_PMP_OXYG_SPEED / 100.0;
		else
			pump_speed = (int)((float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / OXYG_FLOW_TO_RPM_CONV);

		mlpersecOxy = pump_speed * CONV_RPMMIN_TO_ML_PER_SEC_OXYG * 2.0;
		timeSec =  timeSec + (int)((((float)volume - (float)MIN_LIQ_IN_RES_TO_START_OXY_VEN) / (mlpersecArt + mlpersecOxy)) + 0.5);
	}
	return timeSec;
}

// PumpSpeed in rpm
int CalcFilterFlow(unsigned char PumpSpeed)
{
	float pump_speed;
	float mlpersec;
	static float mlpermin;
	static unsigned char cnt = 0;
	cnt++;
	//if(cnt >= 10)
	if(cnt >= 1)
	{
		cnt = 0;
		pump_speed = (float)PumpSpeed;
		mlpersec = pump_speed * CONV_RPMMIN_TO_ML_PER_SEC;
		mlpermin = mlpersec * 60.0 + 0.5;
	}
	return (int)mlpermin;
}

int CalcOxygenatorFlow(unsigned char PumpSpeed)
{
	float pump_speed;
	float mlpersec;
	float mlpermin;

	pump_speed = (float)PumpSpeed;
	mlpersec = pump_speed * CONV_RPMMIN_TO_ML_PER_SEC_OXYG * 2;
	mlpermin = mlpersec * 60.0 + 0.5;
	return (int)mlpermin;
}

void UpdateFilterFlowVal(void)
{
	static unsigned long u32 = 0;

	if((msTick_elapsed(u32) * 50L) >= 500L)
	{
		u32 = timerCounterModBus;
		if(GetTherapyType() == KidneyTreat)
		{
		//	FilterFlowVal = CalcFilterFlow((unsigned char)(modbusData[pumpPerist[0].pmpMySlaveAddress-2][17] / 100));
			/*Nel caso di trattamento Kidney, il flusso del filtro conicide col flusso arterioso*/
			FilterFlowVal = sensor_UFLOW[0].Average_Flow_Val_for_GUI;
		}
		else if(GetTherapyType() == LiverTreat)
		{
			FilterFlowVal = CalcFilterFlow((unsigned char)((float)modbusData[pumpPerist[3].pmpMySlaveAddress-2][17] / 100.0 +0.5));
		}
		OxygenFlowRpm = CalcOxygenatorFlow((unsigned char)((float)modbusData[pumpPerist[0].pmpMySlaveAddress-2][17] / 100.0 + 0.5));
	}
}

word CalcHoursMin(int seconds)
{
	static int sec = 0;
	static word wd;
	if(sec != seconds)
	{
		sec = seconds;
		int min;
		wd = ((unsigned char)(seconds / 3600.0)) << 8;
		min = seconds % 3600;
		wd = wd + (unsigned char)(((float)min / (float)60.0) + (float)0.5);
	}
	return wd;
}
