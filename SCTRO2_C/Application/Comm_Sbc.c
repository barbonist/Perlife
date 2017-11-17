/*
 * Comm_Sbc.c
 *
 *  Created on: 10/nov/2016
 *      Author: W15
 */

#include "Comm_Sbc.h"
#include "Global.h"
#include "ModBusCommProt.h"
#include "App_Ges.h"

#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#include "PE_Types.h"

#include "SBC_COMM.h"
#include "ASerialLdd5.h"

void buildModBusActResponseMsg(char *ptrMsgSbcRx, char *ptrMsgModbusRx)
{
	byte index = 0;

	char subcode = ptrMsgSbcRx[6];

	sbc_tx_data[index++] = 0xA5;
	sbc_tx_data[index++] = 0xAA;
	sbc_tx_data[index++] = 0x55;
	sbc_tx_data[index++] = 0x00;
	if(subcode == 0x15)
	{
		sbc_tx_data[index++] = ptrMsgSbcRx[4]+1;
	}
	else if(subcode == 0x16)
	{
		sbc_tx_data[index++] = ptrMsgSbcRx[4]+1+ptrMsgModbusRx[2];
	}

	sbc_tx_data[index++] = ptrMsgSbcRx[5];
	sbc_tx_data[index++] = subcode;
	sbc_tx_data[index++] = 0x66;

	for(int i = 0 ; i < ptrMsgSbcRx[4] ; i++)
	{
		sbc_tx_data[index++] = ptrMsgSbcRx[7+i];
	}

	if(subcode == 0x16)
	{
		for(int i = 0 ; i < ptrMsgModbusRx[2] ; i++)
		{
			sbc_tx_data[index++] = ptrMsgModbusRx[3+i];
		}
	}

	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;
}

void buildPressSensResponseMsg(char *ptrMsgSbcRx)
{
	/*union NumFloatGain{
				uint32 ieee754NumFormat_Gain;
				float numFormatFloat_Gain;
			} numFloatSensor_Gain;

	union NumFloatOffset{
				uint32 ieee754NumFormat_Offset;
				float numFormatFloat_Offset;
			} numFloatSensor_Offset;

	union NumFloatOffVal{
				uint32 ieee754NumFormat_OffVal;
				float numFormatFloat_OffVal;
			} numFloatSensor_OffVal;

	union NumFloatUnion{
				uint32 ieee754NUmFormat;
				float numFormatFloat;
			} numFloatSensor_Value;*/

    byte index = 0;

    char subcode = ptrMsgSbcRx[6];

	sbc_tx_data[index++] = 0xA5;
	sbc_tx_data[index++] = 0xAA;
	sbc_tx_data[index++] = 0x55;
	sbc_tx_data[index++] = 0x00;

	sbc_tx_data[index++] = 0x16;

	sbc_tx_data[index++] = ptrMsgSbcRx[5];
	sbc_tx_data[index++] = subcode;

	sbc_tx_data[index++] = 0x66;
	sbc_tx_data[index++] = 0x00; //stability byte

	for(int i = 0 ; i < 5 ; i++)
	{
		sbc_tx_data[index++] = ((word)sensor_PRx[i].prSensValue) >> 8;
		sbc_tx_data[index++] = ((word)sensor_PRx[i].prSensValue);
		sbc_tx_data[index++] = sensor_PRx[i].prSensAdc >> 8;
		sbc_tx_data[index++] = sensor_PRx[i].prSensAdc;
	}

	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index;

	/*numFloatSensor_Gain.numFormatFloat_Gain = sensor_PRx[pressSensId].prSensGain;
	sbc_tx_data[7] = numFloatSensor_Gain.ieee754NumFormat_Gain >> 24;
	sbc_tx_data[8] = numFloatSensor_Gain.ieee754NumFormat_Gain >> 16;
	sbc_tx_data[9] = numFloatSensor_Gain.ieee754NumFormat_Gain >> 8;
	sbc_tx_data[10] = numFloatSensor_Gain.ieee754NumFormat_Gain;

	numFloatSensor_Offset.numFormatFloat_Offset = sensor_PRx[pressSensId].prSensOffset;
	sbc_tx_data[11] = numFloatSensor_Offset.ieee754NumFormat_Offset >> 24;
	sbc_tx_data[12] = numFloatSensor_Offset.ieee754NumFormat_Offset >> 16;
	sbc_tx_data[13] = numFloatSensor_Offset.ieee754NumFormat_Offset >> 8;
	sbc_tx_data[14] = numFloatSensor_Offset.ieee754NumFormat_Offset;

	numFloatSensor_OffVal.numFormatFloat_OffVal = sensor_PRx[pressSensId].prSensOffsetVal;
	sbc_tx_data[15] = numFloatSensor_OffVal.ieee754NumFormat_OffVal >> 24;
	sbc_tx_data[16] = numFloatSensor_OffVal.ieee754NumFormat_OffVal >> 16;
	sbc_tx_data[17] = numFloatSensor_OffVal.ieee754NumFormat_OffVal >> 8;
	sbc_tx_data[18] = numFloatSensor_OffVal.ieee754NumFormat_OffVal;*/
}

void buildPeltierResponseMsg(char code){

	char numFloat[4];
	//long ret;
	//char data[8];
	sbc_tx_data[0] = 0xA5;
	sbc_tx_data[1] = 0xAA;
	sbc_tx_data[2] = 0x55;
	sbc_tx_data[3] = 0x00;
	sbc_tx_data[4] = 0x01;
	sbc_tx_data[5] = 0xCC;
	sbc_tx_data[6] = code;
	if(code == 0x26) /*read float*/
	{
		dataIeee754[0] = *ptrMsgDataieee754start;
		dataIeee754[1] = *(ptrMsgDataieee754start+1);
		dataIeee754[2] = *(ptrMsgDataieee754start+2);
		dataIeee754[3] = *(ptrMsgDataieee754start+3);
		dataIeee754[4] = *(ptrMsgDataieee754start+4);
		dataIeee754[5] = *(ptrMsgDataieee754start+5);
		dataIeee754[6] = *(ptrMsgDataieee754start+6);
		dataIeee754[7] = *(ptrMsgDataieee754start+7);
		//retIeee754 = strtol(dataIeee754,NULL,16);
		retIeee754Dummy = strtoul(dataIeee754,NULL,16); //ok anche per numeri negativi

		sbc_tx_data[7] = retIeee754Dummy>>24; //retIeee754>>24;
		sbc_tx_data[8] = retIeee754Dummy>>16; //retIeee754>>16;
		sbc_tx_data[9] = retIeee754Dummy>>8; //retIeee754>>8;
		sbc_tx_data[10] = retIeee754Dummy; //retIeee754;


		/*sbcDebug_tx_data[7] = *ptrMsgDataieee754start; /* most significant byte */
		/*sbcDebug_tx_data[8] = *(ptrMsgDataieee754start+1);
		sbcDebug_tx_data[9] = *(ptrMsgDataieee754start+2);
		sbcDebug_tx_data[10] = *(ptrMsgDataieee754start+3);
		sbcDebug_tx_data[11] = *(ptrMsgDataieee754start+4);
		sbcDebug_tx_data[12] = *(ptrMsgDataieee754start+5);
		sbcDebug_tx_data[13] = *(ptrMsgDataieee754start+6);
		sbcDebug_tx_data[14] = *(ptrMsgDataieee754start+7);*/
	}
	else if((code == 0x24) || (code == 0x22))
	{
		sbc_tx_data[7] = 0x00;
		sbc_tx_data[8] = 0x00;
		sbc_tx_data[9] = 0x00;
		sbc_tx_data[10] = 0x00;
	}
	else if(code == 0x20) /*read int*/
	{
		dataIntPeltier[0] = *ptrMsgDataPeltierInt;
		sbc_tx_data[7] = strtol(dataIntPeltier,NULL,16);
		sbc_tx_data[8] = 0x00;
		sbc_tx_data[9] = 0x00;
		sbc_tx_data[10] = 0x00;
	}

	//sbcDebug_tx_data[7] = peltierCell.msgPeltierRx[0];
	//sbcDebug_tx_data[7] = peltierDebug_rx_data[0];
	//sbcDebug_tx_data[8] = peltierCell.msgPeltierRx[1];
	//sbcDebug_tx_data[8] = peltierDebug_rx_data[1];
	sbc_tx_data[11] = 0x00;
	sbc_tx_data[12] = 0x00;
	sbc_tx_data[13] = 0x5A;
}

void buildWritePressSensResponseMsg(char code, char presssensId)
{
	union NumFloatUnion{
			uint32 ieee754NUmFormat;
			float numFormatFloat;
		} numFloatSensor;

	sbc_tx_data[0] = 0xA5;
	sbc_tx_data[1] = 0xAA;
	sbc_tx_data[2] = 0x55;
	sbc_tx_data[3] = 0x00;
	sbc_tx_data[4] = 0x01;
	sbc_tx_data[5] = 0xCC;
	sbc_tx_data[6] = code; //cmdId & 0x66
	if(code == 0x30)
	{
		numFloatSensor.numFormatFloat = sensor_PRx[presssensId].prSensValue;

		sbc_tx_data[7] = numFloatSensor.ieee754NUmFormat >> 24;
		sbc_tx_data[8] = numFloatSensor.ieee754NUmFormat >> 16;
		sbc_tx_data[9] = numFloatSensor.ieee754NUmFormat >> 8;
		sbc_tx_data[10] = numFloatSensor.ieee754NUmFormat;
	}
	sbc_tx_data[11] = 0x00;
	sbc_tx_data[12] = 0x00;
	sbc_tx_data[13] = 0x5A;
}

void buildWriteTempSensResponseMsg(char code, char tempSensId){
	union NumFloatUnion{
				uint32 ieee754NUmFormat;
				float numFormatFloat;
			} numFloatSensor;

	sbc_tx_data[0] = 0xA5;
	sbc_tx_data[1] = 0xAA;
	sbc_tx_data[2] = 0x55;
	sbc_tx_data[3] = 0x00;
	sbc_tx_data[4] = 0x01;
	sbc_tx_data[5] = 0xCC;
	sbc_tx_data[6] = code; //cmdId & 0x66
	if(code == 0x35)
	{
		numFloatSensor.numFormatFloat = sensor_TMx[tempSensId].tempSensValue;

		sbc_tx_data[7] = numFloatSensor.ieee754NUmFormat >> 24;
		sbc_tx_data[8] = numFloatSensor.ieee754NUmFormat >> 16;
		sbc_tx_data[9] = numFloatSensor.ieee754NUmFormat >> 8;
		sbc_tx_data[10] = numFloatSensor.ieee754NUmFormat;
	}
	sbc_tx_data[11] = 0x00;
	sbc_tx_data[12] = 0x00;
	sbc_tx_data[13] = 0x5A;
}

void buildReadTempSensResponseMsg(char code, char tempSensId){
	union NumFloatGain{
			uint32 ieee754NumFormat_Gain;
			float numFormatFloat_Gain;
	} numFloatSensor_Gain;

	union NumFloatOffset{
			uint32 ieee754NumFormat_Offset;
			float numFormatFloat_Offset;
	} numFloatSensor_Offset;

	union NumFloatOffVal{
			uint32 ieee754NumFormat_OffVal;
			float numFormatFloat_OffVal;
	} numFloatSensor_OffVal;

	sbc_tx_data[0] = 0xA5;
	sbc_tx_data[1] = 0xAA;
	sbc_tx_data[2] = 0x55;
	sbc_tx_data[3] = 0x00;
	sbc_tx_data[4] = 0x01;
	sbc_tx_data[5] = 0xCC;
	sbc_tx_data[6] = code;

	if(code == 0x36)
	{
		numFloatSensor_Gain.numFormatFloat_Gain = sensor_TMx[tempSensId].tempSensGain;
		sbc_tx_data[7] = numFloatSensor_Gain.ieee754NumFormat_Gain >> 24;
		sbc_tx_data[8] = numFloatSensor_Gain.ieee754NumFormat_Gain >> 16;
		sbc_tx_data[9] = numFloatSensor_Gain.ieee754NumFormat_Gain >> 8;
		sbc_tx_data[10] = numFloatSensor_Gain.ieee754NumFormat_Gain;

		numFloatSensor_Offset.numFormatFloat_Offset = sensor_TMx[tempSensId].tempSensOffset;
		sbc_tx_data[11] = numFloatSensor_Offset.ieee754NumFormat_Offset >> 24;
		sbc_tx_data[12] = numFloatSensor_Offset.ieee754NumFormat_Offset >> 16;
		sbc_tx_data[13] = numFloatSensor_Offset.ieee754NumFormat_Offset >> 8;
		sbc_tx_data[14] = numFloatSensor_Offset.ieee754NumFormat_Offset;

		numFloatSensor_OffVal.numFormatFloat_OffVal = sensor_TMx[tempSensId].tempSensOffsetVal;
		sbc_tx_data[15] = numFloatSensor_OffVal.ieee754NumFormat_OffVal >> 24;
		sbc_tx_data[16] = numFloatSensor_OffVal.ieee754NumFormat_OffVal >> 16;
		sbc_tx_data[17] = numFloatSensor_OffVal.ieee754NumFormat_OffVal >> 8;
		sbc_tx_data[18] = numFloatSensor_OffVal.ieee754NumFormat_OffVal;

		sbc_tx_data[19] = sensor_TMx[tempSensId].tempSensAdc >> 8;
		sbc_tx_data[20] = sensor_TMx[tempSensId].tempSensAdc;
	}

	sbc_tx_data[21] = 0x00;
	sbc_tx_data[22] = 0x00;
	sbc_tx_data[23] = 0x5A;
}

void buildReadIRTempRspMsg(char code, char tempIRSensId){
	union NumFloatOffVal{
				uint32 ieee754NumFormat_Val;
				float numFormatFloat_Val;
		} numFloatIRTempSensor_Val;

	sbc_tx_data[0] = 0xA5;
	sbc_tx_data[1] = 0xAA;
	sbc_tx_data[2] = 0x55;
	sbc_tx_data[3] = 0x00;
	sbc_tx_data[4] = 0x01;
	sbc_tx_data[5] = 0xCC;
	sbc_tx_data[6] = code;
	if(code == 0x40)
	{
		numFloatIRTempSensor_Val.numFormatFloat_Val = sensorIR_TM[0].tempSensValue;
		sbc_tx_data[7] = numFloatIRTempSensor_Val.ieee754NumFormat_Val >> 24;
		sbc_tx_data[8] = numFloatIRTempSensor_Val.ieee754NumFormat_Val >> 16;
		sbc_tx_data[9] = numFloatIRTempSensor_Val.ieee754NumFormat_Val >> 8;
		sbc_tx_data[10] = numFloatIRTempSensor_Val.ieee754NumFormat_Val;
	}
	sbc_tx_data[11] = 0x00;
	sbc_tx_data[12] = 0x00;
	sbc_tx_data[13] = 0x5A;
}

void buildReadFlowArtRspMsg(char code, char flowSensId){

	sbc_tx_data[0] = 0xA5;
	sbc_tx_data[1] = 0xAA;
	sbc_tx_data[2] = 0x55;
	sbc_tx_data[3] = 0x00;
	sbc_tx_data[4] = 0x01;
	sbc_tx_data[5] = 0xCC;
	sbc_tx_data[6] = code;
	if(code == 0x50)
	{
		sbc_tx_data[7] = sensor_UFLOW[0].bufferReceived[17];
		sbc_tx_data[8] = sensor_UFLOW[0].bufferReceived[18];
		sbc_tx_data[9] = sensor_UFLOW[0].bufferReceived[19];
		sbc_tx_data[10] = sensor_UFLOW[0].bufferReceived[20];
	}
	sbc_tx_data[11] = 0x00;
	sbc_tx_data[12] = 0x00;
	sbc_tx_data[13] = 0x5A;
}



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

				myCommunicatorToSBC.dataParamSetSBCReadyFlag = DATA_COMM_READY_TO_BE_SEND;
			}
			break;

			/* Button pressed from SBC */
			case COMMAND_ID_BUT_SBC:
			{
				if(sbc_rx_data[8] == 0x01)
				{
					setGUIButton(sbc_rx_data[7]);
				}
				else
				{
					releaseGUIButton(sbc_rx_data[7]);
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
		myCommunicatorToSBC.dataMachineStateReadyFlag == DATA_COMM_IDLE;

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

void buildRDMachineStateResponseMsg(char code, char subcode){
	byte index = 0;

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
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x00;
	/* TODO status parameters: rev fw-H  */
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x00;
	/* TODO status parameters: rev fw-L  */
	sbc_tx_data[index++] = 0x00;
	sbc_tx_data[index++] = 0x00;
	/* status parameters: alarm code */
	sbc_tx_data[index++] = (alarmCurrent.code >> 8 ) & 0xFF;
	sbc_tx_data[index++] = (alarmCurrent.code 	    ) & 0xFF;
	/* status parameters: alarm physic */
	sbc_tx_data[index++] = (alarmCurrent.physic >> 8 ) & 0xFF;
	sbc_tx_data[index++] = (alarmCurrent.physic      ) & 0xFF;
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
	sbc_tx_data[index++] = (sensorsValues.pressAdsFilter >> 8) & 0xFF;
	sbc_tx_data[index++] = (sensorsValues.pressAdsFilter     ) & 0xFF;
	/* sensors parameters: pressure arterial */
	sbc_tx_data[index++] = (sensorsValues.pressArt >> 8) & 0xFF;
	sbc_tx_data[index++] = (sensorsValues.pressArt     ) & 0xFF;
	/* sensors parameters: pressure venous */
	sbc_tx_data[index++] = (sensorsValues.pressVen >> 8) & 0xFF;
	sbc_tx_data[index++] = (sensorsValues.pressVen     ) & 0xFF;
	/* sensors parameters: pressure oxygenation */
	sbc_tx_data[index++] = (sensorsValues.pressOxy >> 8) & 0xFF;
	sbc_tx_data[index++] = (sensorsValues.pressOxy     ) & 0xFF;
	/* sensors parameters: pressure level */
	sbc_tx_data[index++] = (sensorsValues.pressLevel >> 8) & 0xFF;
	sbc_tx_data[index++] = (sensorsValues.pressLevel     ) & 0xFF;
	/* sensors parameters: pressure systolic arterial */
	sbc_tx_data[index++] = (sensorsValues.pressSystArt >> 8) & 0xFF;
	sbc_tx_data[index++] = (sensorsValues.pressSystArt     ) & 0xFF;
	/* sensors parameters: pressure diastolic arterial */
	sbc_tx_data[index++] = (sensorsValues.pressDiasArt >> 8) & 0xFF;
	sbc_tx_data[index++] = (sensorsValues.pressDiasArt     ) & 0xFF;
	/* sensors parameters: pressure mean arterial */
	//TODO remove this
	sensorsValues.pressMeanArt = sensor_PRx[0].prSensValueFilteredWA;
	sbc_tx_data[index++] = (sensorsValues.pressMeanArt >> 8) & 0xFF;
	sbc_tx_data[index++] = (sensorsValues.pressMeanArt     ) & 0xFF;
	/* sensors parameters: pressure systolic venous */
	sbc_tx_data[index++] = (sensorsValues.pressSystVen >> 8) & 0xFF;
	sbc_tx_data[index++] = (sensorsValues.pressSystVen     ) & 0xFF;
	/* sensors parameters: pressure diastolic venous */
	sbc_tx_data[index++] = (sensorsValues.pressDiasVen >> 8) & 0xFF;
	sbc_tx_data[index++] = (sensorsValues.pressDiasVen     ) & 0xFF;
	/* sensors parameters: pressure mean venous */
	sbc_tx_data[index++] = (sensorsValues.pressMeanVen >> 8) & 0xFF;
	sbc_tx_data[index++] = (sensorsValues.pressMeanVen     ) & 0xFF;
	/* sensors parameters: pressure flow arterial */
	//TODO remove this
	sensorsValues.flowArt = (sensor_UFLOW[0].bufferReceived[18] << 8) + sensor_UFLOW[0].bufferReceived[17];
	sbc_tx_data[index++] = (sensorsValues.flowArt >> 8) & 0xFF;
	sbc_tx_data[index++] = (sensorsValues.flowArt     ) & 0xFF;
	/* sensors parameters: pressure flow venous / oxygenation */
	sbc_tx_data[index++] = (sensorsValues.flowVenOxy >> 8) & 0xFF;
	sbc_tx_data[index++] = (sensorsValues.flowVenOxy     ) & 0xFF;
	/* sensors parameters: temperature reservoir outlet */
	sbc_tx_data[index++] = (sensorsValues.tempResOut >> 8) & 0xFF;
	sbc_tx_data[index++] = (sensorsValues.tempResOut     ) & 0xFF;
	/* sensors parameters: temperature organ inlet */
	//TODO remove this
	sensorsValues.tempOrganIn = (int)(sensorIR_TM[0].tempSensValue);
	sbc_tx_data[index++] = (sensorsValues.tempOrganIn >> 8) & 0xFF;
	sbc_tx_data[index++] = (sensorsValues.tempOrganIn     ) & 0xFF;
	/* sensors parameters: temperature venous / oxygenation */
	sbc_tx_data[index++] = (sensorsValues.tempVenOxy >> 8) & 0xFF;
	sbc_tx_data[index++] = (sensorsValues.tempVenOxy     ) & 0xFF;
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



































