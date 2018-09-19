/*
 * Comm_Sbc.c
 *
 *  Created on: 03/set/2018
 *      Author: W32
 */

#include "PE_Types.h"
#include "Global.h"
#include "Comm_Sbc.h"


#include "App_Ges.h"

#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#include "SBC_COMM.h"


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

	leggiOffset=(int32)(config_data.T_Plate_Sensor_Offset_heat*10);

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

/*21*/  sbc_tx_data[index++] = 0;//(MedForVenousPid >> 8)  & 0xFF;
/*22*/	sbc_tx_data[index++] = 0;//(MedForVenousPid)  & 0xFF;
/*23*/	sbc_tx_data[index++] = (PR_VEN_ADC_Filtered >> 8)  & 0xFF;
/*24*/	sbc_tx_data[index++] = PR_VEN_ADC_Filtered  & 0xFF;

/*25*/	sbc_tx_data[index++] = 0;//(MedForArteriousPid >> 8)  & 0xFF;
/*26*/	sbc_tx_data[index++] = 0;//MedForArteriousPid  & 0xFF;
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

	value=(word)(T_PLATE_P_GRADI_CENT*10);
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
