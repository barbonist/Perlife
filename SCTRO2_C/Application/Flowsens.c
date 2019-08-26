/*
 * Flowsens.c
 *
 *  Created on: 01/mar/2017
 *      Author: W15
 */


#include "PE_Types.h"
#include "Global.h"
#include "Flowsens.h"
#include "FLOWSENS_RE.h"
#include "FLOWSENS_DE.h"
#include "Alarm_Con.h"



#include "FLOWSENS_COMM.h"

//Necessari per i check sul funzionamento del sensore aria
bool gAirTransitionDetectedUF0 = FALSE;
bool gAirTransitionDetectedUF1 = FALSE;
bool gAirTransitionDetectedFilt = FALSE;

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
	sensor_UFLOW[0].RequestMsgProcessed = 0;

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
	sensor_UFLOW[1].RequestMsgProcessed = 0;

	/*Disable Data and Receiver enable */
	FLOWSENS_DE_ClrVal();
	/*RE attivo basso, quindi la disabilito mettendolo alto*/
	FLOWSENS_RE_SetVal();

	for (char i= 0; i< TOT_UF_SENSOR; i++)
	{
		for (char j = 0; j < SAMPLE; j++)
		{
			buffer_flow_value[i] [j] = 0;
		}
	}
}

void Manage_UFlow_Sens()
{
	static unsigned char Id_Sensor_UF = 0;

	union NumFloatUFlowVal
	{
		uint32 ieee754NumFormat_Val;
		float numFormatFloat_Val;
	} numFloatUFlow_Val;

	/***************SPEDIZIONE MESSAGGIO*****************/
	/*mando il messaggio di interrogazione ogni 250 nsec*/
	if (timerCounterUFlowSensor >= 1 )
	{

		timerCounterUFlowSensor = 0;

		/*se l'indirizzo supera l'ultimo sensore, resetto l'indirizzo al primo sensore per rifare il giro dei 3*/
		if (Id_Sensor_UF > 1)
			Id_Sensor_UF = 0;

		ptrMsg_UFLOW = buildCmdToFlowSens(sensor_UFLOW[Id_Sensor_UF].sensorAddr,CMD_GET_VAL_CODE,0,0);
		//ptrMsg_UFLOW = buildCmdToFlowSens(sensor_UFLOW[Id_Sensor_UF].sensorAddr,CMD_IDENT_CODE,0,0);
		/*prima di una nuova trasmissione devo comunqie disabilitare il Receiver Enable*/
		FLOWSENS_RE_SetVal();
		FLOWSENS_DE_SetVal();

		for(char k = 0; k < ptrMsg_UFLOW->bufferToSendLenght; k++)
		{
			FLOWSENS_COMM_SendChar(ptrMsg_UFLOW->bufferToSend[k]);
		}

		/*incremento il contatore dei messaggi inviati per quel sensore*/
		sensor_UFLOW[Id_Sensor_UF].RequestMsgProcessed++;

		/*ho spedito, incremento per spedire il prossimo*/
		Id_Sensor_UF++;
	}
	/*************FINE SPEDIZIONE MESSAGGIO****************/

	/***************MEMORIZZAZIONE MESSAGGIO RICEVUTO*****************/
	if(iflag_uflow_sens == IFLAG_UFLOW_SENS_RX)
	{
		/*così la risposta la leggo e la copio solo una volta*/
		iflag_uflow_sens = IFLAG_IDLE;

		/*qui ho già variato l'indirizzo alla fine della spedizione precdente
		 * ma devo riferirmi al buffer del vecchio indirizzo quindi per
		 * questo questi due if/else sono invertiti rispetto al caso precedente*/

		unsigned char Id_Buffer = Id_Sensor_UF -1;

		unsigned char Check_IEEE_Number;

		/*controllo che nella risposta il byte count councida con quello  che mi aspetto*/
		int count_byte_RX = (sensor_UFLOW[Id_Buffer].bufferReceived[1] << 8) + sensor_UFLOW[Id_Buffer].bufferReceived[2];

		/*copio i valori solo se la lunghezza del buffer ricevuto
		 * contenuta nel byte 1 e 2 di ricezione è esattamenmte
		 * quella che mi aspetto*/
		if (count_byte_RX == BYTE_COUNT_GET_VAL_CODE)
		{
			sensor_UFLOW[Id_Buffer].Bubble_Alarm 					= sensor_UFLOW[Id_Buffer].bufferReceived[5] & MASK_Bubble_Alarm;
			sensor_UFLOW[Id_Buffer].Error_during_Bubble_Detection 	= sensor_UFLOW[Id_Buffer].bufferReceived[5] & MASK_Error_during_Bubble_Detection;
			sensor_UFLOW[Id_Buffer].Error_In_Flow_Meas 				= sensor_UFLOW[Id_Buffer].bufferReceived[5] & MASK_Error_In_Flow_Meas;
			sensor_UFLOW[Id_Buffer].Error_In_Temp_Meas 				= sensor_UFLOW[Id_Buffer].bufferReceived[5] & MASK_Error_In_Temp_Meas;
			sensor_UFLOW[Id_Buffer].Device_Fault 					= sensor_UFLOW[Id_Buffer].bufferReceived[5] & MASK_Device_Fault;

			numFloatUFlow_Val.ieee754NumFormat_Val = (sensor_UFLOW[Id_Buffer].bufferReceived[11] << 24) |
													 (sensor_UFLOW[Id_Buffer].bufferReceived[10] << 16) |
													 (sensor_UFLOW[Id_Buffer].bufferReceived[9] << 8) |
													 (sensor_UFLOW[Id_Buffer].bufferReceived[8]);

			/*prima di memorizzare il valore, controllo che non sia un Not a Number opp un infinito
			 * ossia che non abbia 255 nel byte formato dai bit da 0 a 6 del byte + signifativo
			 * e del bit 7 del byte immediatamente precdente*/
			Check_IEEE_Number = sensor_UFLOW[Id_Buffer].bufferReceived[11] & 0x7F;
			Check_IEEE_Number << 1;

			unsigned char temp = sensor_UFLOW[Id_Buffer].bufferReceived[10] & 0x80;
			temp >> 7;
			Check_IEEE_Number |= temp;

			if (Check_IEEE_Number != 0xFF)
			{

				sensor_UFLOW[Id_Buffer].Inst_Flow_Value = numFloatUFlow_Val.numFormatFloat_Val;
				//sensor_UFLOW[Id_Buffer].Average_Flow_Val = Average_Flow_Value(Id_Buffer, numFloatUFlow_Val.numFormatFloat_Val);
				sensor_UFLOW[Id_Buffer].Average_Flow_Val = Average_Flow_Value(Id_Buffer, sensor_UFLOW[Id_Buffer].Inst_Flow_Value);
				/*adesso eseguo un filtraggio più pesante per il dato a video*/
				sensor_UFLOW[Id_Buffer].Average_Flow_Val_for_GUI = Average_Flow_Value_for_GUI(Id_Buffer, sensor_UFLOW[Id_Buffer].Inst_Flow_Value);
				

				/*Se sono in service e non ho ancora calibrato i sensori di flusso,
				 * faccio vedere a video il valore non calibrato così come mi arriva dal sensore*/
				if (Service == TRUE && FlowSensCalibDone == FALSE )
				{
					config_data.FlowSensor_Ven_Gain = 1;
					config_data.FlowSensor_Ven_Offset = 0;

					config_data.FlowSensor_Art_Gain = 1;
					config_data.FlowSensor_Art_Offset = 0;
				}
				if (Id_Buffer == 1)
				{
					sensor_UFLOW[Id_Buffer].Average_Flow_Val         = sensor_UFLOW[Id_Buffer].Average_Flow_Val * config_data.FlowSensor_Ven_Gain + config_data.FlowSensor_Ven_Offset;
					sensor_UFLOW[Id_Buffer].Average_Flow_Val_for_GUI = sensor_UFLOW[Id_Buffer].Average_Flow_Val_for_GUI * config_data.FlowSensor_Ven_Gain + config_data.FlowSensor_Ven_Offset;
				}
				else //if (Id_Buffer == 0)
				{
					sensor_UFLOW[Id_Buffer].Average_Flow_Val         = sensor_UFLOW[Id_Buffer].Average_Flow_Val * config_data.FlowSensor_Art_Gain + config_data.FlowSensor_Art_Offset;
					sensor_UFLOW[Id_Buffer].Average_Flow_Val_for_GUI = sensor_UFLOW[Id_Buffer].Average_Flow_Val_for_GUI * config_data.FlowSensor_Art_Gain + config_data.FlowSensor_Art_Offset;
				}

				/*se sul flusso ricevo un valore neativo lo metto a zero
				 * in quanto il sensore di flusso ha un verso e non
				 * può leggere flussi negativi*/
				if (sensor_UFLOW[Id_Buffer].Average_Flow_Val < 0)
					sensor_UFLOW[Id_Buffer].Average_Flow_Val = 0;

				if (sensor_UFLOW[Id_Buffer].Average_Flow_Val_for_GUI < 0)
					sensor_UFLOW[Id_Buffer].Average_Flow_Val_for_GUI = 0;


				/*decremento il contatore dei messaggi inviati per quel sensore*/
				sensor_UFLOW[Id_Buffer].RequestMsgProcessed = 0;
			}

			/*DA TESTARE
			 * qui dovrebbe esserci la laregezza della bolla dall'ultima richiesta
			 * 1 indica 0.1 mm, 2 0.2 mm e così via; una bolla di 40 ul con un tubo da 4.8
			 * di diametro interno avrebbe una largezza di circa 2.3 mm quindi darò allarme
			 * se in questo byte ho un numero > 23 && != FF (is an error)*/
			sensor_UFLOW[Id_Buffer].bubbleSize = sensor_UFLOW[Id_Buffer].bufferReceived[13];

			numFloatUFlow_Val.ieee754NumFormat_Val = (sensor_UFLOW[Id_Buffer].bufferReceived[21] << 24) |
													 (sensor_UFLOW[Id_Buffer].bufferReceived[20] << 16) |
													 (sensor_UFLOW[Id_Buffer].bufferReceived[19] << 8) |
													 (sensor_UFLOW[Id_Buffer].bufferReceived[18]);
			/*prima di memorizzare il valore, controllo che non sia un Not a Number opp un infinito
			 * ossia che non abbia 255 nel byte formato dai bit da 0 a 6 del byte + signifativo
			 * e del bit 7 del byte immediatamente precdente*/
			Check_IEEE_Number = sensor_UFLOW[Id_Buffer].bufferReceived[21] & 0x7F;
			Check_IEEE_Number << 1;

			temp = sensor_UFLOW[Id_Buffer].bufferReceived[20] & 0x80;
			temp >> 7;
			Check_IEEE_Number |= temp;

			if (Check_IEEE_Number != 0xFF)
			{
				sensor_UFLOW[Id_Buffer].Accumulated_Volume_ul = numFloatUFlow_Val.numFormatFloat_Val;
			}
		}

		/*valorizzo le variabili globali per definire aria o liquido sui due sensori*/
		if ((sensor_UFLOW[ARTERIOUS_AIR_SENSOR].bubbleSize >= MAX_BUBBLE_SIZE) ||
					(sensor_UFLOW[ARTERIOUS_AIR_SENSOR].bubblePresence == MASK_ERROR_BUBBLE_ALARM))
			Air_Arterious = AIR;
		else
			Air_Arterious = LIQUID;

		if ((sensor_UFLOW[VENOUS_AIR_SENSOR].bubbleSize >= MAX_BUBBLE_SIZE) ||
					(sensor_UFLOW[VENOUS_AIR_SENSOR].bubblePresence == MASK_ERROR_BUBBLE_ALARM))
			Air_Venous = AIR;
		else
			Air_Venous = LIQUID;


		//...e se un guasto del sensore lo fa derivare verso valori negativi? -->questo if deve essere assolutamente tolto
		// e sostituito da un controllo sul valore del flusso negativo
		//per verificare i t1 test la devo commentare
//		if (sensor_UFLOW[Id_Buffer].Average_Flow_Val < 0)
//			sensor_UFLOW[Id_Buffer].Average_Flow_Val = 0;
	}
	/***************FINE MEMORIZZAZIONE MESSAGGIO RICEVUTO*****************/
}

float Average_Flow_Value(unsigned char Id_sensor, float new_UF_Value)
{
	float Average_Flow = 0;
	char i;

	/*shifto tutti i campioni a destra di uno*/
	for (i = 0; i < SAMPLE -1; i++)
	{
		buffer_flow_value [Id_sensor] [i] = buffer_flow_value [Id_sensor] [i+1];
	}
	/*memorizzo l'ultimo campione e scarto il più vecchio*/
	buffer_flow_value [Id_sensor] [SAMPLE - 1] = new_UF_Value;

	for (i = 0; i < SAMPLE ; i++)
	{
		Average_Flow = Average_Flow + buffer_flow_value [Id_sensor] [i];
	}

	return (Average_Flow/SAMPLE);
}

float Average_Flow_Value_for_GUI(unsigned char Id_sensor, float new_UF_Value)
{
	float Average_Flow = 0;
	int i;

	/*shifto tutti i campioni a destra di uno*/
	for (i = 0; i < SAMPLE_FOR_GUI -1; i++)
	{
		buffer_flow_value_for_GUI [Id_sensor] [i] = buffer_flow_value_for_GUI [Id_sensor] [i+1];
	}
	/*memorizzo l'ultimo campione e scarto il più vecchio*/
	buffer_flow_value_for_GUI [Id_sensor] [SAMPLE_FOR_GUI - 1] = new_UF_Value;

	for (i = 0; i < SAMPLE_FOR_GUI ; i++)
	{
		Average_Flow = Average_Flow + buffer_flow_value_for_GUI [Id_sensor] [i];
	}

	return (Average_Flow/SAMPLE_FOR_GUI);
}


struct ultrsndFlowSens * buildCmdToFlowSens(unsigned char sensorAddress,
								  unsigned char cmdId,
								  unsigned char ctrlZeroAdjust,
								  float valueZeroAdjust)
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
		sensor_UFLOW[sensId].bufferReceivedLenght = 1; // + sensor_UFLOW[sensId].bufferToSendLenght;
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
		sensor_UFLOW[sensId].bufferReceivedLenght = 1;// + sensor_UFLOW[sensId].bufferToSendLenght;
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
		sensor_UFLOW[sensId].bufferReceivedLenght = 1;// + sensor_UFLOW[sensId].bufferToSendLenght;
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
		sensor_UFLOW[sensId].bufferReceivedLenght = 85; // + sensor_UFLOW[sensId].bufferToSendLenght;
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
		sensor_UFLOW[sensId].bufferReceivedLenght = 0x0A;// + sensor_UFLOW[sensId].bufferToSendLenght;
		sensor_UFLOW[sensId].ptrBufferReceived = &sensor_UFLOW[sensId].bufferReceived[0];
		break;

	case CMD_GET_VAL_CODE: /* cmdGetVal - one value at a time */
		/* byte 1-2 - count byte H/L */
		sensor_UFLOW[sensId].bufferToSend[1] = 0x00;
		sensor_UFLOW[sensId].bufferToSend[2] = 0x0B;
		/* byte 3 - cmdId */
		sensor_UFLOW[sensId].bufferToSend[3] = CMD_GET_VAL_CODE;
		/* byte 4 - type protocol */
		sensor_UFLOW[sensId].bufferToSend[4] = 0x0B;
		/* byte 5 - control register */
		sensor_UFLOW[sensId].bufferToSend[5] = 0x00;
		/* byte 6 - sequence code */
		sensor_UFLOW[sensId].bufferToSend[6] = 0x01;
		/* byte 7 - requested data */
		sensor_UFLOW[sensId].bufferToSend[7] = 0x82;		//Measured instantaneous value of flow [ml/min] float format, in case of fault: 0
		/* byte 8 - requested data */
		sensor_UFLOW[sensId].bufferToSend[8] = 0x2A;		//Size of largest occurred bubble since last request [0.1 mm], in range of 0…10 mm, in case of fault: 255
		/* byte 9 - requested data */
		sensor_UFLOW[sensId].bufferToSend[9] = 0x8B;		//Accumulated volume [ul]
		/* byte 10 - cksm */
		sensor_UFLOW[sensId].bufferToSend[10] = computeCRCFlowSens(sensor_UFLOW[sensId].bufferToSend);
		sensor_UFLOW[sensId].bufferToSendLenght = (sensor_UFLOW[sensId].bufferToSend[1] << 8) + sensor_UFLOW[sensId].bufferToSend[2];
		/* Ricevo 21 byte (0x12): per 0x82 e 0x8B e 0x2A ricevo 5 byte a testa (il prio con la replica della richiesta gli altri 4 col dato richiesto, + i due del CRC */
		sensor_UFLOW[sensId].bufferReceivedLenght = BYTE_COUNT_GET_VAL_CODE;
		//sensor_UFLOW[sensId].bufferReceivedLenght = 0x17; 	// Ricevo 13 byte (0x0D) only for test
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

//Necessaria una lettura di aria e di acqua per poter ritenere i 3 sensori funzionanti
//La transizione avviene se il disposable è correttamente montato solo dopo che la macchina è stata accesa
void ManageAirDetectionTransition(void)
{
	static unsigned char sOldAirDetectionUF0 = LIQUID;
	static unsigned char sOldAirDetectionUF1 = LIQUID;
	static unsigned char sOldAirDetectionFilt = LIQUID;
	static unsigned long sWaitTime = 0;

	sWaitTime ++;
	if (sWaitTime > 100) //attende 100 cicli di chiamata (nel main loop, a 50 msec, sono 5 sec)
	// Questo per evitare transitori nella lettura dei sensori al power on
	{
		if ((Air_Arterious == AIR) && (sOldAirDetectionUF0 == LIQUID))
			gAirTransitionDetectedUF0 = TRUE;

		if ((Air_Venous == AIR) && (sOldAirDetectionUF1 == LIQUID))
			gAirTransitionDetectedUF1 = TRUE;

		if ((Air_1_Status == AIR) && (sOldAirDetectionFilt == LIQUID))
			gAirTransitionDetectedFilt = TRUE;

		sOldAirDetectionUF0 = Air_Arterious;
		sOldAirDetectionUF1 = Air_Venous;
		sOldAirDetectionFilt = Air_1_Status;
	}
}

