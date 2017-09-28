/*
 * Flowsens.c
 *
 *  Created on: 01/mar/2017
 *      Author: W15
 */

#include "Flowsens.h"
#include "Global.h"

#include "PE_Types.h"

#include "Uart3_rts.h"
#include "FLOWSENS_COMM.h"


void initUFlowSensor(void)
{
	iflag_uflow_sens = IFLAG_IDLE;

	sensor_UFLOW[0].sensorId = 0;
	sensor_UFLOW[0].sensorAddr = FLOW_SENSOR_ONE_ADDR;
	sensor_UFLOW[0].bufferToSendLenght = 0;
	sensor_UFLOW[0].bufferReceivedLenght = 0;
	sensor_UFLOW[0].ptrBufferReceived = &sensor_UFLOW[0].bufferReceived[0];
	sensor_UFLOW[0].volumeMlTot = 0;
	sensor_UFLOW[0].volumeDelta = 0;
	sensor_UFLOW[0].bubblePhysic = 0;
	sensor_UFLOW[0].byteSended = 0;
	sensor_UFLOW[0].bubbleSize = 0;
	sensor_UFLOW[0].bubblePresence = 0;

	sensor_UFLOW[1].sensorId = 1;
	sensor_UFLOW[1].sensorAddr = FLOW_SENSOR_TWO_ADDR;
	sensor_UFLOW[1].bufferToSendLenght = 0;
	sensor_UFLOW[1].bufferReceivedLenght = 0;
	sensor_UFLOW[1].ptrBufferReceived = &sensor_UFLOW[1].bufferReceived[0];
	sensor_UFLOW[1].volumeMlTot = 0;
	sensor_UFLOW[1].volumeDelta = 0;
	sensor_UFLOW[1].bubblePhysic = 0;
	sensor_UFLOW[1].byteSended = 0;
	sensor_UFLOW[1].bubbleSize = 0;
	sensor_UFLOW[1].bubblePresence = 0;

	iflag_uflow_sens = IFLAG_IDLE;
}

void alwaysUFlowSensor(void){
	static unsigned char uFlowState = 0;
	static unsigned char countIflag = 0;

	union NumFloatUFlowVal{
			uint32 ieee754NumFormat_Val;
			float numFormatFloat_Val;
				} numFloatUFlow_Val;

	if(iflag_uflow_sens == IFLAG_IDLE){

		switch(uFlowState){

		case 0:
			ptrMsg_UFLOW = buildCmdToFlowSens(sensor_UFLOW[0].sensorAddr,
						         			CMD_GET_VAL_CODE /*CMD_IDENT_CODE*/,
											0,
											0,
											ID_FLOW_VAL_MLMIN);

			Uart3_rts_SetVal();
			for(char k = 0; k < ptrMsg_UFLOW->bufferToSendLenght; k++)
			{
				//Uart3_rts_SetVal();
				FLOWSENS_COMM_SendChar(ptrMsg_UFLOW->bufferToSend[k]);
			}
		break;

		case 1:
			ptrMsg_UFLOW = buildCmdToFlowSens(sensor_UFLOW[0].sensorAddr,
									         CMD_GET_VAL_CODE /*CMD_IDENT_CODE*/,
											 0,
											 0,
											 ID_MAX_BUBBLE_SIZE);

			Uart3_rts_SetVal();
			for(char k = 0; k < ptrMsg_UFLOW->bufferToSendLenght; k++)
			{
				//Uart3_rts_SetVal();
				FLOWSENS_COMM_SendChar(ptrMsg_UFLOW->bufferToSend[k]);
			}
			break;

		default:
			uFlowState = 0;
			break;
		}
	}

	if(iflag_uflow_sens == IFLAG_UFLOW_SENS_RX){
		iflag_uflow_sens = IFLAG_IDLE;
		countIflag = 0;

		if(uFlowState == 0){
			numFloatUFlow_Val.ieee754NumFormat_Val = (sensor_UFLOW[0].bufferReceived[20] << 24) |
													 (sensor_UFLOW[0].bufferReceived[19] << 16) |
													 (sensor_UFLOW[0].bufferReceived[18] << 8) |
													 (sensor_UFLOW[0].bufferReceived[17]);

			sensor_UFLOW[0].volumeMlTot = numFloatUFlow_Val.numFormatFloat_Val;
		}
		else if(uFlowState == 1){
			sensor_UFLOW[0].bubbleSize = sensor_UFLOW[0].bufferReceived[17];
			sensor_UFLOW[0].bubblePresence = sensor_UFLOW[0].bufferReceived[14] & MASK_ERROR_BUBBLE_ALARM;
		}

		uFlowState = uFlowState + 1;
	}
	else if(iflag_uflow_sens == IFLAG_UFLOW_SENS_TX){
		countIflag = countIflag + 1;
		if(countIflag >= 20){ //se non ricevo risposta entro due secondi, inizio una nuova trasmissione
			iflag_uflow_sens = IFLAG_IDLE;
			countIflag = 0;
		}
	}
}

struct ultrsndFlowSens * buildCmdToFlowSens(unsigned char sensorAddress,
								  unsigned char cmdId,
								  unsigned char ctrlZeroAdjust,
								  float valueZeroAdjust,
								  unsigned char valueId)
{
	unsigned char sensId;
	union NumFloatValueZeroAdj{
				uint32 ieee754NumFormat;
				float numFormatFloat;
		} numFloatZeroAdj;

	//iflag_uflow_sens = IFLAG_IDLE;


	numFloatZeroAdj.numFormatFloat = valueZeroAdjust;

	if(sensorAddress == FLOW_SENSOR_ONE_ADDR)
		sensId = sensor_UFLOW[0].sensorId;
	else if(sensorAddress == FLOW_SENSOR_TWO_ADDR)
		sensId = sensor_UFLOW[1].sensorId;

	/* byte 0 - start byte */
	sensor_UFLOW[sensId].bufferToSend[0] = START_PATTERN_BASE + sensorAddress;
	sensor_UFLOW[sensId].byteSended = 0;

	switch(cmdId)
	{
	case CMD_PING_CODE: /* cmdPing */
		/* byte 1-2 - count byte H/L */
		sensor_UFLOW[sensId].bufferToSend[1] = 0x00;
		sensor_UFLOW[sensId].bufferToSend[2] = 0x05;
		/* byte 3 - cmdId */
		sensor_UFLOW[sensId].bufferToSend[3] = CMD_PING_CODE;
		/* byte 4 - cksm */
		sensor_UFLOW[sensId].bufferToSend[4] = computeCRCFlowSens(sensor_UFLOW[sensId].bufferToSend);
		sensor_UFLOW[sensId].bufferToSendLenght = 5;
		sensor_UFLOW[sensId].bufferReceivedLenght = 1 + sensor_UFLOW[sensId].bufferToSendLenght;
		sensor_UFLOW[sensId].ptrBufferReceived = &sensor_UFLOW[sensId].bufferReceived[0];
		break;

	case CMD_RESTART_CODE: /* cmdRestart */
		/* byte 1-2 - count byte H/L */
		sensor_UFLOW[sensId].bufferToSend[1] = 0x00;
		sensor_UFLOW[sensId].bufferToSend[2] = 0x05;
		/* byte 3 - cmdId */
		sensor_UFLOW[sensId].bufferToSend[3] = CMD_RESTART_CODE;
		/* byte 4 - cksm */
		sensor_UFLOW[sensId].bufferToSend[4] = computeCRCFlowSens(sensor_UFLOW[sensId].bufferToSend);
		sensor_UFLOW[sensId].bufferToSendLenght = 5;
		sensor_UFLOW[sensId].bufferReceivedLenght = 1 + sensor_UFLOW[sensId].bufferToSendLenght;
		sensor_UFLOW[sensId].ptrBufferReceived = &sensor_UFLOW[sensId].bufferReceived[0];
		break;

	case CMD_RESET_VOL_CODE: /* cmdResetVol */
		/* byte 1-2 - count byte H/L */
		sensor_UFLOW[sensId].bufferToSend[1] = 0x00;
		sensor_UFLOW[sensId].bufferToSend[2] = 0x05;
		/* byte 3 - cmdId */
		sensor_UFLOW[sensId].bufferToSend[3] = CMD_RESET_VOL_CODE;
		/* byte 4 - cksm */
		sensor_UFLOW[sensId].bufferToSend[4] = computeCRCFlowSens(sensor_UFLOW[sensId].bufferToSend);
		sensor_UFLOW[sensId].bufferToSendLenght = 5;
		sensor_UFLOW[sensId].bufferReceivedLenght = 1 + sensor_UFLOW[sensId].bufferToSendLenght;
		sensor_UFLOW[sensId].ptrBufferReceived = &sensor_UFLOW[sensId].bufferReceived[0];
		break;

	case CMD_IDENT_CODE: /* cmdIdent */
		/* byte 1-2 - count byte H/L */
		sensor_UFLOW[sensId].bufferToSend[1] = 0x00;
		sensor_UFLOW[sensId].bufferToSend[2] = 0x05;
		/* byte 3 - cmdId */
		sensor_UFLOW[sensId].bufferToSend[3] = CMD_IDENT_CODE;
		/* byte 4 - cksm */
		sensor_UFLOW[sensId].bufferToSend[4] = computeCRCFlowSens(sensor_UFLOW[sensId].bufferToSend);
		sensor_UFLOW[sensId].bufferToSendLenght = 5;
		sensor_UFLOW[sensId].bufferReceivedLenght = 85 + sensor_UFLOW[sensId].bufferToSendLenght;
		sensor_UFLOW[sensId].ptrBufferReceived = &sensor_UFLOW[sensId].bufferReceived[0];
		break;

	case CMD_ZERO_CODE: /* cmdZero */
		/* byte 1-2 - count byte H/L */
		sensor_UFLOW[sensId].bufferToSend[1] = 0x00;
		sensor_UFLOW[sensId].bufferToSend[2] = 0x0A;
		/* byte 3 - cmdId */
		sensor_UFLOW[sensId].bufferToSend[3] = CMD_ZERO_CODE;
		/* byte 4 - zero mode */
		sensor_UFLOW[sensId].bufferToSend[4] = 0x80; /* read value of zero - single 4 byte ieee754 */
		/* byte 5...8 - zero adjust value */
		sensor_UFLOW[sensId].bufferToSend[5] = numFloatZeroAdj.ieee754NumFormat;
		sensor_UFLOW[sensId].bufferToSend[6] = numFloatZeroAdj.ieee754NumFormat >> 8;
		sensor_UFLOW[sensId].bufferToSend[7] = numFloatZeroAdj.ieee754NumFormat >> 16;
		sensor_UFLOW[sensId].bufferToSend[8] = numFloatZeroAdj.ieee754NumFormat >> 24;
		/* byet 9 - cksm */
		sensor_UFLOW[sensId].bufferToSend[8] = computeCRCFlowSens(sensor_UFLOW[sensId].bufferToSend);
		sensor_UFLOW[sensId].bufferToSendLenght = 0x0A;
		sensor_UFLOW[sensId].bufferReceivedLenght = 0x0A + sensor_UFLOW[sensId].bufferToSendLenght;
		sensor_UFLOW[sensId].ptrBufferReceived = &sensor_UFLOW[sensId].bufferReceived[0];
		break;

	case CMD_GET_VAL_CODE: /* cmdGetVal - one value at a time */
		/* byte 1-2 - count byte H/L */
		sensor_UFLOW[sensId].bufferToSend[1] = 0x00;
		sensor_UFLOW[sensId].bufferToSend[2] = 0x09;
		/* byte 3 - cmdId */
		sensor_UFLOW[sensId].bufferToSend[3] = CMD_GET_VAL_CODE;
		/* byte 4 - type protocol */
		sensor_UFLOW[sensId].bufferToSend[4] = 0x0B;
		/* byte 5 - control register */
		sensor_UFLOW[sensId].bufferToSend[5] = 0x00;
		/* byte 6 - sequence code */
		sensor_UFLOW[sensId].bufferToSend[6] = 0x01;
		/* byte 7 - requested data */
		sensor_UFLOW[sensId].bufferToSend[7] = valueId;
		/* byte 8 - cksm */
		sensor_UFLOW[sensId].bufferToSend[8] = computeCRCFlowSens(sensor_UFLOW[sensId].bufferToSend);
		sensor_UFLOW[sensId].bufferToSendLenght = 0x09;
		sensor_UFLOW[sensId].bufferReceivedLenght = 0x0D + sensor_UFLOW[sensId].bufferToSendLenght;
		sensor_UFLOW[sensId].ptrBufferReceived = &sensor_UFLOW[sensId].bufferReceived[0];
		break;

	default:
		break;
	}

	ptrCurrent_UFLOW = &sensor_UFLOW[sensId];
	return ptrCurrent_UFLOW;
}

unsigned char computeCRCFlowSens(unsigned char buffer[])
{
	int i, ct;
	unsigned char a, b;
	const char CRC_Serial = 0xD4;

	b = buffer[0];
	ct = buffer[1]*(0x100) + buffer[2];

	for(i=1; i<(ct-1); i++)
	{
		a = b ^ buffer[i];
		if(a & 0x80)
			b = (a<<1) ^ CRC_Serial;
		else
			b = (a<<1);
	}

	return (b & 0x3F);
}
