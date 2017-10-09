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


void buildSTResponseMsg(char code){
	sbcDebug_tx_data[0] = 0xA5;
	sbcDebug_tx_data[1] = 0xAA;
	sbcDebug_tx_data[2] = 0x55;
	sbcDebug_tx_data[3] = 0x00;
	sbcDebug_tx_data[4] = 0x0D;
	sbcDebug_tx_data[5] = 0x20;
	sbcDebug_tx_data[6] = 0x66;
	/* par: alarm code */
	sbcDebug_tx_data[7] = alarmCurrent.code;
	sbcDebug_tx_data[8] = alarmCurrent.physic;
	sbcDebug_tx_data[9] = alarmCurrent.type;
	sbcDebug_tx_data[10] = ptrCurrentState->state;
	sbcDebug_tx_data[11] = ptrCurrentParent->parent;
	sbcDebug_tx_data[12] = ptrCurrentChild->child;
	sbcDebug_tx_data[13] = 0x00;
	sbcDebug_tx_data[14] = 0x00;
	sbcDebug_tx_data[15] = 0x5A;

}

void buildPERResponseMsg(char code){
	sbcDebug_tx_data[0] = 0xA5;
	sbcDebug_tx_data[1] = 0xAA;
	sbcDebug_tx_data[2] = 0x55;
	sbcDebug_tx_data[3] = 0x00;
	sbcDebug_tx_data[4] = 0x0F;
	sbcDebug_tx_data[5] = 0x30;
	sbcDebug_tx_data[6] = 0x66;
	sbcDebug_tx_data[7] = perfusionParam.systolicPress >> 8;
	sbcDebug_tx_data[8] = perfusionParam.systolicPress;
	sbcDebug_tx_data[9] = 0x00;
	sbcDebug_tx_data[10] = 0x00;
	sbcDebug_tx_data[11] = 0x5A;

}

void buildPURResponseMsg(char code){
	sbcDebug_tx_data[0] = 0xA5;
	sbcDebug_tx_data[1] = 0xAA;
	sbcDebug_tx_data[2] = 0x55;
	sbcDebug_tx_data[3] = 0x00;
	sbcDebug_tx_data[4] = 0x12;
	sbcDebug_tx_data[5] = 0x40;
	sbcDebug_tx_data[6] = 0x66;
	sbcDebug_tx_data[7] = purificatParam.pressAbsorbFilt >> 8;
	sbcDebug_tx_data[8] = purificatParam.pressAbsorbFilt;
	sbcDebug_tx_data[9] = 0x00;
	sbcDebug_tx_data[10] = 0x00;
	sbcDebug_tx_data[11] = 0x5A;

}

void buildPeltierResponseMsg(char code){

	char numFloat[4];
	//long ret;
	//char data[8];
	sbcDebug_tx_data[0] = 0xA5;
	sbcDebug_tx_data[1] = 0xAA;
	sbcDebug_tx_data[2] = 0x55;
	sbcDebug_tx_data[3] = 0x00;
	sbcDebug_tx_data[4] = 0x01;
	sbcDebug_tx_data[5] = 0xCC;
	sbcDebug_tx_data[6] = code;
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

		sbcDebug_tx_data[7] = retIeee754Dummy>>24; //retIeee754>>24;
		sbcDebug_tx_data[8] = retIeee754Dummy>>16; //retIeee754>>16;
		sbcDebug_tx_data[9] = retIeee754Dummy>>8; //retIeee754>>8;
		sbcDebug_tx_data[10] = retIeee754Dummy; //retIeee754;


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
		sbcDebug_tx_data[7] = 0x00;
		sbcDebug_tx_data[8] = 0x00;
		sbcDebug_tx_data[9] = 0x00;
		sbcDebug_tx_data[10] = 0x00;
	}
	else if(code == 0x20) /*read int*/
	{
		dataIntPeltier[0] = *ptrMsgDataPeltierInt;
		sbcDebug_tx_data[7] = strtol(dataIntPeltier,NULL,16);
		sbcDebug_tx_data[8] = 0x00;
		sbcDebug_tx_data[9] = 0x00;
		sbcDebug_tx_data[10] = 0x00;
	}

	//sbcDebug_tx_data[7] = peltierCell.msgPeltierRx[0];
	//sbcDebug_tx_data[7] = peltierDebug_rx_data[0];
	//sbcDebug_tx_data[8] = peltierCell.msgPeltierRx[1];
	//sbcDebug_tx_data[8] = peltierDebug_rx_data[1];
	sbcDebug_tx_data[11] = 0x00;
	sbcDebug_tx_data[12] = 0x00;
	sbcDebug_tx_data[13] = 0x5A;
}

void buildModBusActResponseMsg(char code)
{
	sbcDebug_tx_data[0] = 0xA5;
	sbcDebug_tx_data[1] = 0xAA;
	sbcDebug_tx_data[2] = 0x55;
	sbcDebug_tx_data[3] = 0x00;
	sbcDebug_tx_data[4] = 0x01;
	sbcDebug_tx_data[5] = 0xCC;
	sbcDebug_tx_data[6] = code; //cmdId & 0x66

	if(code == 0x04) /* write */
	{
		sbcDebug_tx_data[7] = msgToRecvFrame10[2];
		sbcDebug_tx_data[8] = msgToRecvFrame10[3];
		sbcDebug_tx_data[9] = msgToRecvFrame10[4];
		sbcDebug_tx_data[10] = msgToRecvFrame10[5];
		sbcDebug_tx_data[11] = 0;
		sbcDebug_tx_data[12] = 0;
	}
	else if(code == 0x06) /* read */
	{
		sbcDebug_tx_data[7] = msgToRecvFrame3[3];
		sbcDebug_tx_data[8] = msgToRecvFrame3[4];
		sbcDebug_tx_data[9] = msgToRecvFrame3[31];
		sbcDebug_tx_data[10] = msgToRecvFrame3[32];
		sbcDebug_tx_data[11] = msgToRecvFrame3[29];
		sbcDebug_tx_data[12] = msgToRecvFrame3[30];
	}
	else if(code == 0x17)
	{
		sbcDebug_tx_data[7] = msgToRecvFrame17[3];
		sbcDebug_tx_data[8] = msgToRecvFrame17[4];
		sbcDebug_tx_data[9] = msgToRecvFrame17[5];
		sbcDebug_tx_data[10] = msgToRecvFrame17[6];
		sbcDebug_tx_data[11] = msgToRecvFrame17[7];
		sbcDebug_tx_data[12] = msgToRecvFrame17[8];
	}

	sbcDebug_tx_data[13] = 0x00;
	sbcDebug_tx_data[14] = 0x00;
	sbcDebug_tx_data[15] = 0x5A;
}

void buildWritePressSensResponseMsg(char code, char presssensId)
{
	union NumFloatUnion{
			uint32 ieee754NUmFormat;
			float numFormatFloat;
		} numFloatSensor;

	sbcDebug_tx_data[0] = 0xA5;
	sbcDebug_tx_data[1] = 0xAA;
	sbcDebug_tx_data[2] = 0x55;
	sbcDebug_tx_data[3] = 0x00;
	sbcDebug_tx_data[4] = 0x01;
	sbcDebug_tx_data[5] = 0xCC;
	sbcDebug_tx_data[6] = code; //cmdId & 0x66
	if(code == 0x30)
	{
		numFloatSensor.numFormatFloat = sensor_PRx[presssensId].prSensValue;

		sbcDebug_tx_data[7] = numFloatSensor.ieee754NUmFormat >> 24;
		sbcDebug_tx_data[8] = numFloatSensor.ieee754NUmFormat >> 16;
		sbcDebug_tx_data[9] = numFloatSensor.ieee754NUmFormat >> 8;
		sbcDebug_tx_data[10] = numFloatSensor.ieee754NUmFormat;
	}
	sbcDebug_tx_data[11] = 0x00;
	sbcDebug_tx_data[12] = 0x00;
	sbcDebug_tx_data[13] = 0x5A;
}

void buildReadPressSensResponseMsg(char code, char pressSensId)
{
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

	union NumFloatUnion{
				uint32 ieee754NUmFormat;
				float numFormatFloat;
			} numFloatSensor_Value;

	sbcDebug_tx_data[0] = 0xA5;
	sbcDebug_tx_data[1] = 0xAA;
	sbcDebug_tx_data[2] = 0x55;
	sbcDebug_tx_data[3] = 0x00;
	sbcDebug_tx_data[4] = 0x01;
	sbcDebug_tx_data[5] = 0xCC;
	sbcDebug_tx_data[6] = code;

	if(code == 0x31)
	{
		numFloatSensor_Gain.numFormatFloat_Gain = sensor_PRx[pressSensId].prSensGain;
		sbcDebug_tx_data[7] = numFloatSensor_Gain.ieee754NumFormat_Gain >> 24;
		sbcDebug_tx_data[8] = numFloatSensor_Gain.ieee754NumFormat_Gain >> 16;
		sbcDebug_tx_data[9] = numFloatSensor_Gain.ieee754NumFormat_Gain >> 8;
		sbcDebug_tx_data[10] = numFloatSensor_Gain.ieee754NumFormat_Gain;

		numFloatSensor_Offset.numFormatFloat_Offset = sensor_PRx[pressSensId].prSensOffset;
		sbcDebug_tx_data[11] = numFloatSensor_Offset.ieee754NumFormat_Offset >> 24;
		sbcDebug_tx_data[12] = numFloatSensor_Offset.ieee754NumFormat_Offset >> 16;
		sbcDebug_tx_data[13] = numFloatSensor_Offset.ieee754NumFormat_Offset >> 8;
		sbcDebug_tx_data[14] = numFloatSensor_Offset.ieee754NumFormat_Offset;

		numFloatSensor_OffVal.numFormatFloat_OffVal = sensor_PRx[pressSensId].prSensOffsetVal;
		sbcDebug_tx_data[15] = numFloatSensor_OffVal.ieee754NumFormat_OffVal >> 24;
		sbcDebug_tx_data[16] = numFloatSensor_OffVal.ieee754NumFormat_OffVal >> 16;
		sbcDebug_tx_data[17] = numFloatSensor_OffVal.ieee754NumFormat_OffVal >> 8;
		sbcDebug_tx_data[18] = numFloatSensor_OffVal.ieee754NumFormat_OffVal;

		numFloatSensor_Value.numFormatFloat = sensor_PRx[pressSensId].prSensValue;
		sbcDebug_tx_data[19] = numFloatSensor_Value.ieee754NUmFormat >> 24;
		sbcDebug_tx_data[20] = numFloatSensor_Value.ieee754NUmFormat >> 16;
		sbcDebug_tx_data[21] = numFloatSensor_Value.ieee754NUmFormat >> 8;
		sbcDebug_tx_data[22] = numFloatSensor_Value.ieee754NUmFormat;

		sbcDebug_tx_data[23] = sensor_PRx[pressSensId].prSensAdc >> 8;
		sbcDebug_tx_data[24] = sensor_PRx[pressSensId].prSensAdc;

		sbcDebug_tx_data[25] = pressSensId;

	}

	sbcDebug_tx_data[26] = 0x00;
	sbcDebug_tx_data[27] = 0x00;
	sbcDebug_tx_data[28] = 0x5A;
}

void buildWriteTempSensResponseMsg(char code, char tempSensId){
	union NumFloatUnion{
				uint32 ieee754NUmFormat;
				float numFormatFloat;
			} numFloatSensor;

	sbcDebug_tx_data[0] = 0xA5;
	sbcDebug_tx_data[1] = 0xAA;
	sbcDebug_tx_data[2] = 0x55;
	sbcDebug_tx_data[3] = 0x00;
	sbcDebug_tx_data[4] = 0x01;
	sbcDebug_tx_data[5] = 0xCC;
	sbcDebug_tx_data[6] = code; //cmdId & 0x66
	if(code == 0x35)
	{
		numFloatSensor.numFormatFloat = sensor_TMx[tempSensId].tempSensValue;

		sbcDebug_tx_data[7] = numFloatSensor.ieee754NUmFormat >> 24;
		sbcDebug_tx_data[8] = numFloatSensor.ieee754NUmFormat >> 16;
		sbcDebug_tx_data[9] = numFloatSensor.ieee754NUmFormat >> 8;
		sbcDebug_tx_data[10] = numFloatSensor.ieee754NUmFormat;
	}
	sbcDebug_tx_data[11] = 0x00;
	sbcDebug_tx_data[12] = 0x00;
	sbcDebug_tx_data[13] = 0x5A;
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

	sbcDebug_tx_data[0] = 0xA5;
	sbcDebug_tx_data[1] = 0xAA;
	sbcDebug_tx_data[2] = 0x55;
	sbcDebug_tx_data[3] = 0x00;
	sbcDebug_tx_data[4] = 0x01;
	sbcDebug_tx_data[5] = 0xCC;
	sbcDebug_tx_data[6] = code;

	if(code == 0x36)
	{
		numFloatSensor_Gain.numFormatFloat_Gain = sensor_TMx[tempSensId].tempSensGain;
		sbcDebug_tx_data[7] = numFloatSensor_Gain.ieee754NumFormat_Gain >> 24;
		sbcDebug_tx_data[8] = numFloatSensor_Gain.ieee754NumFormat_Gain >> 16;
		sbcDebug_tx_data[9] = numFloatSensor_Gain.ieee754NumFormat_Gain >> 8;
		sbcDebug_tx_data[10] = numFloatSensor_Gain.ieee754NumFormat_Gain;

		numFloatSensor_Offset.numFormatFloat_Offset = sensor_TMx[tempSensId].tempSensOffset;
		sbcDebug_tx_data[11] = numFloatSensor_Offset.ieee754NumFormat_Offset >> 24;
		sbcDebug_tx_data[12] = numFloatSensor_Offset.ieee754NumFormat_Offset >> 16;
		sbcDebug_tx_data[13] = numFloatSensor_Offset.ieee754NumFormat_Offset >> 8;
		sbcDebug_tx_data[14] = numFloatSensor_Offset.ieee754NumFormat_Offset;

		numFloatSensor_OffVal.numFormatFloat_OffVal = sensor_TMx[tempSensId].tempSensOffsetVal;
		sbcDebug_tx_data[15] = numFloatSensor_OffVal.ieee754NumFormat_OffVal >> 24;
		sbcDebug_tx_data[16] = numFloatSensor_OffVal.ieee754NumFormat_OffVal >> 16;
		sbcDebug_tx_data[17] = numFloatSensor_OffVal.ieee754NumFormat_OffVal >> 8;
		sbcDebug_tx_data[18] = numFloatSensor_OffVal.ieee754NumFormat_OffVal;

		sbcDebug_tx_data[19] = sensor_TMx[tempSensId].tempSensAdc >> 8;
		sbcDebug_tx_data[20] = sensor_TMx[tempSensId].tempSensAdc;
	}

	sbcDebug_tx_data[21] = 0x00;
	sbcDebug_tx_data[22] = 0x00;
	sbcDebug_tx_data[23] = 0x5A;
}

void buildReadIRTempRspMsg(char code, char tempIRSensId){
	union NumFloatOffVal{
				uint32 ieee754NumFormat_Val;
				float numFormatFloat_Val;
		} numFloatIRTempSensor_Val;

	sbcDebug_tx_data[0] = 0xA5;
	sbcDebug_tx_data[1] = 0xAA;
	sbcDebug_tx_data[2] = 0x55;
	sbcDebug_tx_data[3] = 0x00;
	sbcDebug_tx_data[4] = 0x01;
	sbcDebug_tx_data[5] = 0xCC;
	sbcDebug_tx_data[6] = code;
	if(code == 0x40)
	{
		numFloatIRTempSensor_Val.numFormatFloat_Val = sensorIR_TM[0].tempSensValue;
		sbcDebug_tx_data[7] = numFloatIRTempSensor_Val.ieee754NumFormat_Val >> 24;
		sbcDebug_tx_data[8] = numFloatIRTempSensor_Val.ieee754NumFormat_Val >> 16;
		sbcDebug_tx_data[9] = numFloatIRTempSensor_Val.ieee754NumFormat_Val >> 8;
		sbcDebug_tx_data[10] = numFloatIRTempSensor_Val.ieee754NumFormat_Val;
	}
	sbcDebug_tx_data[11] = 0x00;
	sbcDebug_tx_data[12] = 0x00;
	sbcDebug_tx_data[13] = 0x5A;
}

void buildReadFlowArtRspMsg(char code, char flowSensId){

	sbcDebug_tx_data[0] = 0xA5;
	sbcDebug_tx_data[1] = 0xAA;
	sbcDebug_tx_data[2] = 0x55;
	sbcDebug_tx_data[3] = 0x00;
	sbcDebug_tx_data[4] = 0x01;
	sbcDebug_tx_data[5] = 0xCC;
	sbcDebug_tx_data[6] = code;
	if(code == 0x50)
	{
		sbcDebug_tx_data[7] = sensor_UFLOW[0].bufferReceived[17];
		sbcDebug_tx_data[8] = sensor_UFLOW[0].bufferReceived[18];
		sbcDebug_tx_data[9] = sensor_UFLOW[0].bufferReceived[19];
		sbcDebug_tx_data[10] = sensor_UFLOW[0].bufferReceived[20];
	}
	sbcDebug_tx_data[11] = 0x00;
	sbcDebug_tx_data[12] = 0x00;
	sbcDebug_tx_data[13] = 0x5A;
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

	myCommunicatorToSBC.numByteToSend = 0;
}


void pollingSBCCommTreat(void){

	int val = 0;
	unsigned char perfParaRspMsgReady = 0;

	union NumFloatUnion{
			uint32 ieee754ValFormat;
			float valFormatFloat;
		} valueFloat;

	if(iflag_sbc_rx == IFLAG_SBC_RX)
	{
		iflag_sbc_rx = IFLAG_IDLE;

		/*decodifica messaggio*/
		switch(sbcDebug_rx_data[5])
		{
			/* PAR SET SBC */
			case COMMAND_ID_PAR_SET:
			{
				//depending on par_id the following four byte are interpreted as word or float
				if(checkParTypeFromGUI(sbcDebug_rx_data[6]) == 0x01) //word
				{
					valueFloat.ieee754ValFormat = (sbcDebug_rx_data[7] << 24) |
												  (sbcDebug_rx_data[8] << 16) |
												  (sbcDebug_rx_data[9] << 8) |
												   sbcDebug_rx_data[10];
					val = (int)(valueFloat.valFormatFloat);
					//val = BYTES_TO_WORD(sbcDebug_rx_data[9], sbcDebug_rx_data[10]);
					setParamWordFromGUI(sbcDebug_rx_data[6], val);
				}
				else if(checkParTypeFromGUI(sbcDebug_rx_data[6]) == 0x03) //float
				{
					valueFloat.ieee754ValFormat = (sbcDebug_rx_data[7] << 24) |
												  (sbcDebug_rx_data[8] << 16) |
												  (sbcDebug_rx_data[9] << 8) |
												  sbcDebug_rx_data[10];
					setParamFloatFromGUI(sbcDebug_rx_data[6], valueFloat.valFormatFloat);
				}
			}
			break;

			/*BUT-SBC*/
			case COMMAND_ID_BUT_SBC:
			{
				if(sbcDebug_rx_data[7] == 0x01)
				{
					setGUIButton(sbcDebug_rx_data[6]);
				}
				else
				{
					releaseGUIButton(sbcDebug_rx_data[6]);
				}
			}
			break;

			/* READ MACHINE STATES */
			case COMMAND_ID_ST:
			{
				myCommunicatorToSBC.dataMachineStateReadyFlag = DATA_COMM_READY_TO_BE_SEND;
			}
			break;
			default:{}
			break;
		}
	}
}

void pollingDataToSBCTreat(void){
	if(myCommunicatorToSBC.dataMachineStateReadyFlag == DATA_COMM_READY_TO_BE_SEND)
	{
		/*reset flag */
		myCommunicatorToSBC.dataMachineStateReadyFlag == DATA_COMM_IDLE;

		/* build response message */
		buildRDMachineStateResponseMsg(COMMAND_ID_ST);

		/* build response message */
		ptrMsgSbcTx = &sbcDebug_tx_data[0];

		for(char i = 0; i < (myCommunicatorToSBC.numByteToSend) ; i++)
		{
			SBC_COMM_SendChar(*(ptrMsgSbcTx+i));

			#ifdef	DEBUG_COMM_SBC
			//PC_DEBUG_COMM_SendChar(*(ptrMsgSbcTx+i));
			#endif
		}
	}
}

void buildRDMachineStateResponseMsg(char code){
	byte index = 0;

	sbcDebug_tx_data[index++] = 0xA5;
	sbcDebug_tx_data[index++] = 0xAA;
	sbcDebug_tx_data[index++] = 0x55;
	sbcDebug_tx_data[index++] = 0x00;
	sbcDebug_tx_data[index++] = 0x10;
	sbcDebug_tx_data[index++] = code;
	sbcDebug_tx_data[index++] = 0x66;
	/* TODO status parameters: life  */
	sbcDebug_tx_data[index++] = 0x00;
	/* TODO status parameters: rev fw-H  */
	sbcDebug_tx_data[index++] = 0x00;
	/* TODO status parameters: rev fw-L  */
	sbcDebug_tx_data[index++] = 0x00;
	/* status parameters: alarm code */
	sbcDebug_tx_data[index++] = alarmCurrent.code;
	/* status parameters: alarm physic */
	sbcDebug_tx_data[index++] = alarmCurrent.physic;
	/* status parameters: alarm type */
	sbcDebug_tx_data[index++] = alarmCurrent.type;
	/* status parameters: machine state state*/
	sbcDebug_tx_data[index++] = ptrCurrentState->state;
	/* status parameters: machine state parent*/
	sbcDebug_tx_data[index++] = ptrCurrentParent->parent;
	/* status parameters: machine state child*/
	sbcDebug_tx_data[index++] = ptrCurrentChild->child;
	/* perfusion parameters: systolic pressure */
	sbcDebug_tx_data[index++] = (perfusionParam.systolicPress >> 8) & 0xFF;
	sbcDebug_tx_data[index++] = (perfusionParam.systolicPress     ) & 0xFF;
	/* perfusion parameters: diastolic pressure */
	sbcDebug_tx_data[index++] = (perfusionParam.diastolicPress >> 8) & 0xFF;
	sbcDebug_tx_data[index++] = (perfusionParam.diastolicPress     ) & 0xFF;
	/* perfusion parameters: mean pressure */
	sbcDebug_tx_data[index++] = (perfusionParam.meanPress >> 8) & 0xFF;
	sbcDebug_tx_data[index++] = (perfusionParam.meanPress     ) & 0xFF;
	/* perfusion parameters: flow art */
	sbcDebug_tx_data[index++] = (perfusionParam.flowPerfArt >> 8) & 0xFF;
	sbcDebug_tx_data[index++] = (perfusionParam.flowPerfArt     ) & 0xFF;
	/* perfusion parameters: flow ven */
	sbcDebug_tx_data[index++] = (perfusionParam.flowPerfVen >> 8) & 0xFF;
	sbcDebug_tx_data[index++] = (perfusionParam.flowPerfVen     ) & 0xFF;
	/* perfusion parameters: flow oxy */
	sbcDebug_tx_data[index++] = (perfusionParam.flowOxygenat >> 8) & 0xFF;
	sbcDebug_tx_data[index++] = (perfusionParam.flowOxygenat     ) & 0xFF;
	/* perfusion parameters: temperature reservoir filter */
	sbcDebug_tx_data[index++] = (perfusionParam.tempReservOutlet >> 8) & 0xFF;
	sbcDebug_tx_data[index++] = (perfusionParam.tempReservOutlet     ) & 0xFF;
	/* perfusion parameters: temperature organ inlet */
	sbcDebug_tx_data[index++] = (perfusionParam.tempPerfInletCon >> 8) & 0xFF;
	sbcDebug_tx_data[index++] = (perfusionParam.tempPerfInletCon     ) & 0xFF;
	/* perfusion parameters: renal resistence */
	sbcDebug_tx_data[index++] = (perfusionParam.renalResistance >> 8) & 0xFF;
	sbcDebug_tx_data[index++] = (perfusionParam.renalResistance     ) & 0xFF;
	/* perfusion parameters: priming volume art */
	sbcDebug_tx_data[index++] = (perfusionParam.volumePrimingArt >> 8) & 0xFF;
	sbcDebug_tx_data[index++] = (perfusionParam.volumePrimingArt     ) & 0xFF;
	/* perfusion parameters: priming volume ven */
	sbcDebug_tx_data[index++] = (perfusionParam.volumePrimingVen >> 8) & 0xFF;
	sbcDebug_tx_data[index++] = (perfusionParam.volumePrimingVen     ) & 0xFF;
	/* perfusion parameters: priming volume oxy */
	sbcDebug_tx_data[index++] = (perfusionParam.volumePrimingOxygen >> 8) & 0xFF;
	sbcDebug_tx_data[index++] = (perfusionParam.volumePrimingOxygen     ) & 0xFF;
	/* perfusion parameters: treatment volume art */
	sbcDebug_tx_data[index++] = (perfusionParam.volumeTreatArt >> 8) & 0xFF;
	sbcDebug_tx_data[index++] = (perfusionParam.volumeTreatArt     ) & 0xFF;
	/* perfusion parameters: treatment volume ven */
	sbcDebug_tx_data[index++] = (perfusionParam.volumeTreatVen >> 8) & 0xFF;
	sbcDebug_tx_data[index++] = (perfusionParam.volumeTreatVen     ) & 0xFF;
	/* perfusion parameters: treatment volume oxy */
	sbcDebug_tx_data[index++] = (perfusionParam.volumeTreatOxygen >> 8) & 0xFF;
	sbcDebug_tx_data[index++] = (perfusionParam.volumeTreatOxygen     ) & 0xFF;
	/* TODO perfusion parameters: duration art */
	sbcDebug_tx_data[index++] = 0x00;
	sbcDebug_tx_data[index++] = 0x00;
	/* TODO perfusion parameters: duration ven */
	sbcDebug_tx_data[index++] = 0x00;
	sbcDebug_tx_data[index++] = 0x00;
	/* TODO perfusion parameters: duration oxy */
	sbcDebug_tx_data[index++] = 0x00;
	sbcDebug_tx_data[index++] = 0x00;
	/* purification parameters: pressure plasma filter */
	sbcDebug_tx_data[index++] = (purificatParam.pressPlasmaFilt >> 8) & 0xFF;
	sbcDebug_tx_data[index++] = (purificatParam.pressPlasmaFilt     ) & 0xFF;
	/* purification parameters: pressure fract filter */
	sbcDebug_tx_data[index++] = (purificatParam.pressFractFilt >> 8) & 0xFF;
	sbcDebug_tx_data[index++] = (purificatParam.pressFractFilt     ) & 0xFF;
	/* purification parameters: pressure absorbent filter */
	sbcDebug_tx_data[index++] = (purificatParam.pressAbsorbFilt >> 8) & 0xFF;
	sbcDebug_tx_data[index++] = (purificatParam.pressAbsorbFilt     ) & 0xFF;
	/* purification parameters: flow plasma filter */
	sbcDebug_tx_data[index++] = (purificatParam.flowPlasmaFilt >> 8) & 0xFF;
	sbcDebug_tx_data[index++] = (purificatParam.flowPlasmaFilt     ) & 0xFF;
	/* purification parameters: flow fract filter */
	sbcDebug_tx_data[index++] = (purificatParam.flowFractFilt >> 8) & 0xFF;
	sbcDebug_tx_data[index++] = (purificatParam.flowFractFilt     ) & 0xFF;
	/* purification parameters: flow absorbent filter */
	sbcDebug_tx_data[index++] = (purificatParam.flowAbsorbFilt >> 8) & 0xFF;
	sbcDebug_tx_data[index++] = (purificatParam.flowAbsorbFilt     ) & 0xFF;
	/* purification parameters: volume priming plasma filter */
	sbcDebug_tx_data[index++] = (purificatParam.volPrimingPlasmaFilt >> 8) & 0xFF;
	sbcDebug_tx_data[index++] = (purificatParam.volPrimingPlasmaFilt     ) & 0xFF;
	/* purification parameters: volume priming fract filter */
	sbcDebug_tx_data[index++] = (purificatParam.volPrimingFractFilt >> 8) & 0xFF;
	sbcDebug_tx_data[index++] = (purificatParam.volPrimingFractFilt     ) & 0xFF;
	/* purification parameters: volume priming absorbent filter */
	sbcDebug_tx_data[index++] = (purificatParam.volPrimingAbsorbFilt >> 8) & 0xFF;
	sbcDebug_tx_data[index++] = (purificatParam.volPrimingAbsorbFilt     ) & 0xFF;
	/* purification parameters: volume treatment plasma filter */
	sbcDebug_tx_data[index++] = (purificatParam.volTreatPlasmaFilt >> 8) & 0xFF;
	sbcDebug_tx_data[index++] = (purificatParam.volTreatPlasmaFilt     ) & 0xFF;
	/* purification parameters: volume treatment fract filter */
	sbcDebug_tx_data[index++] = (purificatParam.volTreatFractFilt >> 8) & 0xFF;
	sbcDebug_tx_data[index++] = (purificatParam.volTreatFractFilt     ) & 0xFF;
	/* purification parameters: volume treatment absorbent filter */
	sbcDebug_tx_data[index++] = (purificatParam.volTreatAbsorbFilt >> 8) & 0xFF;
	sbcDebug_tx_data[index++] = (purificatParam.volTreatAbsorbFilt     ) & 0xFF;
	/* TODO purification parameters: volume treatment ultrafiltered */
	sbcDebug_tx_data[index++] = 0x00;
	sbcDebug_tx_data[index++] = 0x00;
	/* purification parameters: volume treatment washing filter */
	sbcDebug_tx_data[index++] = (purificatParam.volTreatWashFilt >> 8) & 0xFF;
	sbcDebug_tx_data[index++] = (purificatParam.volTreatWashFilt     ) & 0xFF;
	/* TODO purification parameters: duration plasma filter */
	sbcDebug_tx_data[index++] = 0x00;
	sbcDebug_tx_data[index++] = 0x00;
	/* TODO purification parameters: duration fractional filter */
	sbcDebug_tx_data[index++] = 0x00;
	sbcDebug_tx_data[index++] = 0x00;
	/* TODO purification parameters: duration absorbent filter */
	sbcDebug_tx_data[index++] = 0x00;
	sbcDebug_tx_data[index++] = 0x00;
	/* TODO CRC */
	sbcDebug_tx_data[index++] = 0x00;
	sbcDebug_tx_data[index++] = 0x00;
	/* End */
	sbcDebug_tx_data[index++] = 0x5A;

	myCommunicatorToSBC.numByteToSend = index-1;
}

void buildRDPerfParamResponseMsg(char code){

	union NumFloatOffVal{
		uint32 ieee754NumFormat_Val;
		float numFormatFloat_Val;
			} numFloatPressArt_Val;

	union NumFloatIRTempVal{
		uint32 ieee754NumFormat_Val;
		float numFormatFloat_Val;
			} numFloatIRTempSensor_Val;

	sbcDebug_tx_data[0] = 0xA5;
	sbcDebug_tx_data[1] = 0xAA;
	sbcDebug_tx_data[2] = 0x55;
	sbcDebug_tx_data[3] = 0x00;
	sbcDebug_tx_data[4] = 0x00;
	sbcDebug_tx_data[5] = 0x30;
	sbcDebug_tx_data[6] = 0x66;
	/*vol priming art*/
	sbcDebug_tx_data[7] = perfusionParam.volumePrimingArt >> 8;
	sbcDebug_tx_data[8] = perfusionParam.volumePrimingArt;
	/*press. art. mean*/
	numFloatPressArt_Val.numFormatFloat_Val = sensor_PRx[0].prSensValueFilteredWA;
	sbcDebug_tx_data[9] = numFloatPressArt_Val.ieee754NumFormat_Val >> 24;
	sbcDebug_tx_data[10] = numFloatPressArt_Val.ieee754NumFormat_Val >> 16;
	sbcDebug_tx_data[11] = numFloatPressArt_Val.ieee754NumFormat_Val >> 8;
	sbcDebug_tx_data[12] = numFloatPressArt_Val.ieee754NumFormat_Val;

	/*flow sensor art*/
	sbcDebug_tx_data[13] = sensor_UFLOW[0].bufferReceived[17];	//LL
	sbcDebug_tx_data[14] = sensor_UFLOW[0].bufferReceived[18];	//LH
	sbcDebug_tx_data[15] = sensor_UFLOW[0].bufferReceived[19];	//HL
	sbcDebug_tx_data[16] = sensor_UFLOW[0].bufferReceived[20];  //HH

	/*temp sensor art */
	numFloatIRTempSensor_Val.numFormatFloat_Val = sensorIR_TM[0].tempSensValue;
	sbcDebug_tx_data[17] = numFloatIRTempSensor_Val.ieee754NumFormat_Val >> 24;
	sbcDebug_tx_data[18] = numFloatIRTempSensor_Val.ieee754NumFormat_Val >> 16;
	sbcDebug_tx_data[19] = numFloatIRTempSensor_Val.ieee754NumFormat_Val >> 8;
	sbcDebug_tx_data[20] = numFloatIRTempSensor_Val.ieee754NumFormat_Val;

	//sbcDebug_tx_data[9] =  (sensor_PRx[0].prSensValueFilteredWA)  >> 8;
	//sbcDebug_tx_data[10] = (sensor_PRx[0].prSensValueFilteredWA);

	sbcDebug_tx_data[21] = 0x00;
	sbcDebug_tx_data[22] = 0x00;
	sbcDebug_tx_data[23] = 0x5A;

	myCommunicatorToSBC.numByteToSend = 24;
}

/******************************************************************************************/
/*                          TREATMENT - END SECTION									  */
/******************************************************************************************/



































