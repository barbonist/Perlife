/*
 * Alarm_Con.c
 *
 *  Created on: 10/nov/2016
 *      Author: W15
 */

#include "PE_Types.h"
#include "Global.h"
#include "Alarm_Con.h"
#include "Flowsens.h"
#include "string.h"
#include "PE_Types.h"
#include "ModBusCommProt.h"


// FM questa lista devo costruirla mettendo prima i PHYSIC_TRUE e poi i PHYSIC_FALSE,
// ognuno deve poi essere ordinato in base alla priorita' ???
struct alarm alarmList[] =
{
		//{CODE_ALARM0, PHYSIC_TRUE, TYPE_ALARM_CONTROL, PRIORITY_LOW, OVRD_ENABLE, SILENCE_ALLOWED},
		{CODE_ALARM_PRESS_ART_HIGH,        PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR, PRIORITY_HIGH, 2000, 2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull}, 		/* 0 */
		{CODE_ALARM_PRESS_ART_LOW,         PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR, PRIORITY_HIGH, 2000, 2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull}, 		/* 1 */
		{CODE_ALARM_AIR_PRES_ART,          PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR, PRIORITY_HIGH, 1000, 2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_ALLOWED,     &alarmManageNull}, 		/* 2 */
		{CODE_ALARM_TEMP_ART_HIGH,         PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR, PRIORITY_HIGH, 5000, 2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull}, 	    /* 3 */
		{CODE_ALARM_PRESS_ADS_FILTER_HIGH, PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR, PRIORITY_HIGH, 2000, 2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull},	    /* 4 */
		{CODE_ALARM_FLOW_PERF_ART_HIGH,    PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR, PRIORITY_HIGH, 2000, 2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull}, 		/* 5 */
		{CODE_ALARM_FLOW_ART_NOT_DETECTED, PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR, PRIORITY_HIGH, 2000, 2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull}, 		/* 6 */
		{CODE_ALARM_PRESS_VEN_HIGH, 	   PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR, PRIORITY_HIGH, 2000, 2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull}, 		/* 7 */
		{CODE_ALARM_PRESS_VEN_LOW, 		   PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR, PRIORITY_HIGH, 2000, 2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull}, 		/* 8 */
		{CODE_ALARM_TEMP_SENS_NOT_DETECTED,PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR, PRIORITY_HIGH, 2000, 2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull}, 		/* 9 */
		{CODE_ALARM_MODBUS_ACTUATOR_SEND,  PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_WAIT_CONFIRM,      PRIORITY_LOW,     0,    0, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull},	    /* 10 */


		{}

};


void ShowAlarmStr(int i, char * str)
{
	char s[100];
	switch (i)
	{
		case CODE_ALARM_PRESS_ART_HIGH:
			strcpy(s, "ALARM_PRESS_ART_HIGH");
			strcat(s, str);
			DebugStringStr(s);
			break;
		case CODE_ALARM_PRESS_ART_LOW:
			strcpy(s, "ALARM_PRESS_ART_LOW");
			strcat(s, str);
			DebugStringStr(s);
			break;
		case CODE_ALARM_AIR_PRES_ART:
			strcpy(s, "ALARM_AIR_PRES_ART");
			strcat(s, str);
			DebugStringStr(s);
			break;
		case CODE_ALARM_TEMP_ART_HIGH:
			strcpy(s, "ALARM_TEMP_ART_HIGH");
			strcat(s, str);
			DebugStringStr(s);
			break;
		case CODE_ALARM_PRESS_ADS_FILTER_HIGH:
			strcpy(s, "ALARM_PRESS_ADS_FILTER_HIGH");
			strcat(s, str);
			DebugStringStr(s);
			break;
		case CODE_ALARM_MODBUS_ACTUATOR_SEND:
			strcpy(s, "ALARM_MODBUS_ACTUATOR_SEND");
			strcat(s, str);
			DebugStringStr(s);
			break;
	}

}


void alarmConInit(void){
	ptrAlarmCurrent = &alarmList[0];
}

int StartAlmArrIdx = 0;
int i;

void alarmEngineAlways(void)
{

	static int StrAlarmWritten = 0;
	static int IdxCurrAlarm = 0xff;


/*Faccio uno switch su tutta la macchina a stati in modo
 * gestire ogni allarme in funzioine dello stato in cui sono*/

	switch(ptrCurrentState->state)
	{
		case STATE_NULL:
		{
			/* DA DEBUGGARE
			manageAlarmFlowSensNotDetected();
			manageAlarmIrTempSensNotDetected();
			*/
			break;
		}

		case STATE_ENTRY:
		{
			/* DA DEBUGGARE
			manageAlarmFlowSensNotDetected();
			manageAlarmIrTempSensNotDetected();
			*/
			break;
		}

		case STATE_IDLE:
		{
			/* DA DEBUGGARE
			manageAlarmFlowSensNotDetected();
			manageAlarmIrTempSensNotDetected();
			*/
			break;
		}

		case STATE_SELECT_TREAT:
		{
			/* DA DEBUGGARE
			manageAlarmFlowSensNotDetected();
			manageAlarmIrTempSensNotDetected();
			*/
			break;
		}

		case STATE_T1_NO_DISPOSABLE:
		{
			/* DA DEBUGGARE
			manageAlarmFlowSensNotDetected();
			manageAlarmIrTempSensNotDetected();
			*/
			break;
		}

		case STATE_MOUNTING_DISP:
		{
			/* DA DEBUGGARE
			manageAlarmFlowSensNotDetected();
			manageAlarmIrTempSensNotDetected();
			*/
			break;
		}

		case STATE_TANK_FILL:
		{
			/* DA DEBUGGARE
			manageAlarmFlowSensNotDetected();
			manageAlarmIrTempSensNotDetected();
			*/
			break;
		}

		case STATE_PRIMING_PH_1:
		{
			/* DA DEBUGGARE
			manageAlarmFlowSensNotDetected();
			manageAlarmIrTempSensNotDetected();
			*/

			//verifica physic pressioni
			manageAlarmPhysicPressSens();

			//verifica physic ir temp sens
			manageAlarmPhysicTempSens();
			break;
		}

		case STATE_PRIMING_PH_2:
		{
			/* DA DEBUGGARE
			manageAlarmFlowSensNotDetected();
			manageAlarmIrTempSensNotDetected();
			*/
			//verifica physic pressioni
			manageAlarmPhysicPressSens();

			//verifica physic ir temp sens
			manageAlarmPhysicTempSens();
			break;
		}

		case STATE_TREATMENT_KIDNEY_1:
		{
			//verifica physic pressioni
			manageAlarmPhysicPressSens();

			/*DA DEBUGGARE*/
			//verifica physic flow sensor
		//	manageAlarmPhysicUFlowSens();

			//verifica physic ir temp sens
			manageAlarmPhysicTempSens();

			/*DA DEBUGGARE*/
			//verifica physic flusso di perfusione arteriosa alto
		//	manageAlarmPhysicFlowPerfArtHigh();

			/*DA DEBUGGARE*/
			//verifica  flusso  non rilevato
		//	manageAlarmFlowSensNotDetected();

			/*DA DEBUGGARE*/
			//verifica temperatura noin rilevata
		//	manageAlarmIrTempSensNotDetected();
			break;
		}

		case STATE_PRIMING_WAIT:
			//verifica physic pressioni
			manageAlarmPhysicPressSens();

			//verifica physic ir temp sens
			manageAlarmPhysicTempSens();
			break;

		case STATE_PRIMING_RICIRCOLO:
			//verifica physic pressioni
			manageAlarmPhysicPressSens();

			//verifica physic ir temp sens
			manageAlarmPhysicTempSens();
			break;

		case STATE_WAIT_TREATMENT:
			//verifica physic pressioni
			manageAlarmPhysicPressSens();

			//verifica physic ir temp sens
			manageAlarmPhysicTempSens();
			break;

		/*case STATE_T1_WITH_DISPOSABLE:
		{
			break;
		}

		case STATE_PRIMING_TREAT_1:
		{
			break;
		}

		case STATE_PRIMING_TREAT_2:
		{
			break;
		}*/


		case STATE_TREATMENT_2:
		{
			//verifica physic pressioni
			manageAlarmPhysicPressSens();

			/*DA DEBUGGARE*/
			//verifica physic flow sensor
		//	manageAlarmPhysicUFlowSens();

			//verifica physic ir temp sens
			manageAlarmPhysicTempSens();

			/*DA DEBUGGARE*/
			//verifica physic flusso di perfusione arteriosa alto
		//	manageAlarmPhysicFlowPerfArtHigh();

			/*DA DEBUGGARE*/
			//verifica  flusso  non rilevato
		//	manageAlarmFlowSensNotDetected();

			/*DA DEBUGGARE*/
			//verifica temperatura noin rilevata
		//	manageAlarmIrTempSensNotDetected();
			break;
		}

		case STATE_EMPTY_DISPOSABLE_1:
		{
			/* DA DEBUGGARE
			manageAlarmFlowSensNotDetected();
			manageAlarmIrTempSensNotDetected();
			*/
			break;
		}

		case STATE_EMPTY_DISPOSABLE_2:
		{
			/* DA DEBUGGARE
			manageAlarmFlowSensNotDetected();
			manageAlarmIrTempSensNotDetected();
			*/
			break;
		}

		case STATE_WASHING:
		{
			/* DA DEBUGGARE
			manageAlarmFlowSensNotDetected();
			manageAlarmIrTempSensNotDetected();
			*/
			break;
		}

		case STATE_FATAL_ERROR:
		{
			break;
		}

		default:
		{
			break;
		}
	}


	for(i=StartAlmArrIdx; i<ALARM_ACTIVE_IN_STRUCT; i++)
	{
		if((alarmList[i].physic == PHYSIC_TRUE) && (alarmList[i].active != ACTIVE_TRUE))
		{
			ptrAlarmCurrent = &alarmList[i];
			alarmList[i].prySafetyActionFunc();
			StartAlmArrIdx = i;
			//IdxCurrAlarm = i;

			// FM forse qui devo interrompere perche' ho trovato una condizione di allarme da attivare
			// e devo gestirla prima di andare a vedere le altre
			break;
		}
		else if((alarmList[i].active == ACTIVE_TRUE) && (alarmList[i].physic == PHYSIC_FALSE))
		{
			ptrAlarmCurrent = &alarmList[i];
			alarmList[i].prySafetyActionFunc();

			//StartAlmArrIdx = i;  // allarme finito riparto dall'inizio della struttura
			//IdxCurrAlarm = i;

			// FM forse qui devo interrompere perche' ho trovato una condizione di allarme da disattivare
			// e devo gestirla prima di andare a vedere le altre
			break;
		}
	}

	/*
	if( !StrAlarmWritten && (StartAlmArrIdx < ALARM_ACTIVE_IN_STRUCT))
	{
		if(alarmList[IdxCurrAlarm].active == ACTIVE_TRUE)
			StrAlarmWritten = 1;
	}
	else if(StrAlarmWritten == 1)
	{
		ShowAlarmStr((int)alarmList[IdxCurrAlarm].code, " alarm");
		StrAlarmWritten = 2;
	}
	else if(StrAlarmWritten == 2)
	{
		if(alarmList[IdxCurrAlarm].active == ACTIVE_FALSE)
		{
			// allarme terminato
			StrAlarmWritten = 0;
			ShowAlarmStr((int)alarmList[IdxCurrAlarm].code, " terminato");
			StartAlmArrIdx = 0;
		}
	}
	*/
}


void manageAlarmPhysicPressSens(void)
{

	if(PR_ART_mmHg_Filtered > PR_ART_HIGH)
	{
		alarmList[PRESS_ART_HIGH].physic = PHYSIC_TRUE;
	}
	else
	{
		alarmList[PRESS_ART_HIGH].physic = PHYSIC_FALSE;
	}

	if(PR_ART_mmHg_Filtered <= PR_ART_LOW)
	{
		alarmList[PRESS_ART_LOW].physic = PHYSIC_TRUE;
	}
	else
	{
		alarmList[PRESS_ART_LOW].physic = PHYSIC_FALSE;
	}


	if(PR_ADS_FLT_mmHg_Filtered > PR_ADS_FILTER_HIGH)
	{
		alarmList[PRESS_ADS_FILTER_HIGH].physic = PHYSIC_TRUE;
	}
	else
	{
		alarmList[PRESS_ADS_FILTER_HIGH].physic = PHYSIC_FALSE;
	}

	/*il sensore Venoso è suato solo nel trattamento Liver, il Kidney non ha la linea Venosa*/
	if(PR_VEN_mmHg_Filtered > PR_VEN_HIGH && GetTherapyType() == LiverTreat)
	{
		alarmList[PRESS_VEN_HIGH].physic = PHYSIC_TRUE;
	}
	else
	{
		alarmList[PRESS_VEN_HIGH].physic = PHYSIC_FALSE;
	}

	if(PR_VEN_mmHg_Filtered <= PR_VEN_LOW && GetTherapyType() == LiverTreat)
	{
		alarmList[PRESS_VEN_LOW].physic = PHYSIC_TRUE;
	}
	else
	{
		alarmList[PRESS_VEN_LOW].physic = PHYSIC_FALSE;
	}


}

void manageAlarmPhysicTempSens(void)
{
	if((sensorIR_TM[0].tempSensValue > 40.0))
	{
		alarmList[TEMP_ART_HIGH].physic = PHYSIC_TRUE;
	}
	else
	{
		alarmList[TEMP_ART_HIGH].physic = PHYSIC_FALSE;
	}
}

void manageAlarmPhysicUFlowSens(void){
	if(
		(sensor_UFLOW[0].bubbleSize >= 25) ||
		(sensor_UFLOW[0].bubblePresence == MASK_ERROR_BUBBLE_ALARM)
		)
	{
		alarmList[AIR_PRES_ART].physic = PHYSIC_TRUE;
	}
	else
	{
		alarmList[AIR_PRES_ART].physic = PHYSIC_FALSE;
	}
}

// Imposto un allarme non fisico da inviare ad SBC
void SetNonPhysicalAlm( int AlarmCode)
{
	switch (AlarmCode)
	{
		case CODE_ALARM_MODBUS_ACTUATOR_SEND:
			// faccio partire un allarme perche' ho superato il numero delle retry nella scrittura su modbus
			//alarmList[6].active = ACTIVE_TRUE;
			alarmList[MODBUS_ACTUATOR_SEND].physic = PHYSIC_TRUE;  // uso questo parametro per far partire l'allarme
			break;
	}
}

// Ritorna 1 se e' un allarme non gestito tramite le tabelle child
int IsNonPhysicalAlm(int AlarmCode)
{
	int NonPhysAlm = 0;
	if(AlarmCode == CODE_ALARM_MODBUS_ACTUATOR_SEND)
		NonPhysAlm = 1;
	return NonPhysAlm;
}

// Deve essere chiamata per togliere gli allarmi attivati tramite la SetNonPhysicalAlm
// Potrebbe essere chiamata automaticamente dopo un certo tempo che l'allarme e' stato inviato. se e' un
// allarme che non implica alcuna azione sul sistema (tipo arresto pompe)
void ClearNonPhysicalAlm( int AlarmCode)
{
	switch (AlarmCode)
	{
		case CODE_ALARM_MODBUS_ACTUATOR_SEND:
			//alarmList[5].active = ACTIVE_TRUE;
			alarmList[MODBUS_ACTUATOR_SEND].physic = PHYSIC_FALSE;  // uso questo parametro per terminare l'allarme
			break;
	}
}

void manageAlarmPhysicFlowPerfArtHigh(void)
{

	int FlowMax = 0;
	bool chekFlow = FALSE;


	switch (GetTherapyType())
	{
		case LiverTreat:
			FlowMax = FLOW_LIVER_MAX;
			chekFlow = TRUE;
			break;

		case KidneyTreat:
			FlowMax = FLOW_KIDNEY_MAX;
			chekFlow = TRUE;
			break;

		default:
			chekFlow = FALSE;
			break;

	}

	if (chekFlow)
	{
		if(sensor_UFLOW[0].Average_Flow_Val > FlowMax)
		{
			/*l'indice dell'array deve corrispondere all'indice della riga della tabella alarmList*/
			alarmList[FLOW_PERF_ART_HIGH].physic = PHYSIC_TRUE;
		}
		else
		{
			alarmList[FLOW_PERF_ART_HIGH].physic = PHYSIC_FALSE;
		}
	}

}

void manageAlarmFlowSensNotDetected(void)
{

	for (int i = 0; i <2; i++)
	{
		/*se non ricevo 10 msg consecutivi da un sensore di flusso ossia il sensore non risposnde per 5 secondi consecutivi vado in allarme*/
		if (sensor_UFLOW[i].RequestMsgProcessed > MAX_MSG_CONSECUTIVE_FLOW_SENS_NOT_DETECTED)
		{
			alarmList[FLOW_SENS_NOT_DETECTED].physic = PHYSIC_TRUE;
			sensor_UFLOW[i].RequestMsgProcessed = 0;
			/*in questo caso bisogna comunicarlo all'SBC che metterà a video un pop up per le possibili soluzioni*/
		}
		else
		{
			alarmList[FLOW_SENS_NOT_DETECTED].physic = PHYSIC_FALSE;
		}
	}
}

void manageAlarmIrTempSensNotDetected(void)
{

	for (int i = 0; i <3; i++)
	{
		/*se non ricevo 10 msg consecutivi da un sensore di temperatura ossia il sensore non risposnde per 6 secondi consecutivi vado in allarme*/
		if (sensorIR_TM[i].ErrorMSG > MAX_MSG_CONSECUTIVE_IR_TEMP_SENS_NOT_DETECTED)
		{
			alarmList[IR_SENS_NOT_DETECTED].physic = PHYSIC_TRUE;
			sensorIR_TM[i].ErrorMSG = 0;
		}

		else
		{
			alarmList[IR_SENS_NOT_DETECTED].physic = PHYSIC_FALSE;
		}
	}
}

void manageAlarmActuatorModbusNotRespond(void)
{

	for (int i = 0; i <8; i++)
	{
		/*se non ricevo 10 msg consecutivi da un sensore di temperatura ossia il sensore non risposnde per 6 secondi consecutivi vado in allarme*/
		if (CountErrorModbusMSG[i] = 0 > MAX_MSG_CONSECUTIVE_ACTUATOR_MODBUS_NOT_RESPOND)
		{
			alarmList[MODBUS_ACTUATOR_SEND].physic = PHYSIC_TRUE;
			CountErrorModbusMSG[i] = 0;
		}

		else
		{
			alarmList[MODBUS_ACTUATOR_SEND].physic = PHYSIC_FALSE;
		}
	}
}

void alarmManageNull(void)
{
	static unsigned char dummy = 0;
	static unsigned short elapsedEntryTime = 0;
	static unsigned short elapsedExitTime = 0;

	elapsedEntryTime = elapsedEntryTime + 50;
	elapsedExitTime = elapsedExitTime + 50;
	if((ptrAlarmCurrent->active != ACTIVE_TRUE) && (elapsedEntryTime > ptrAlarmCurrent->entryTime))
	{
		// entro nella gestione di un allarme che ha bisogno di azioni sugli attuatori
		elapsedEntryTime = 0;
		elapsedExitTime = 0;
		ptrAlarmCurrent->active = ACTIVE_TRUE;
		currentGuard[GUARD_ALARM_ACTIVE].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;

		// FM ora AlarmCurrent contiene l'allarme attivo corrente che sara' inviato ad SBC
		alarmCurrent = *ptrAlarmCurrent;
		if(IsNonPhysicalAlm((int)alarmCurrent.code))
		{
			// e' solo una segnalazione, l'allarme non deve essere gestito tramite le tabelle child
			currentGuard[GUARD_ALARM_ACTIVE].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
		}

		manageAlarmChildGuard(ptrAlarmCurrent);
		ShowAlarmStr((int)alarmList[StartAlmArrIdx].code, " alarm");
	}
	else if((ptrAlarmCurrent->active == ACTIVE_TRUE) && (elapsedExitTime > ptrAlarmCurrent->exitTime))
	{
		// esco dalla gestione di un allarme che ha bisogno di azioni sugli attuatori
		elapsedEntryTime = 0;
		elapsedExitTime = 0;
		ptrAlarmCurrent->active = ACTIVE_FALSE;
		currentGuard[GUARD_ALARM_ACTIVE].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;

		// FM l'allarme e' stato disattivato perche' non sono piu'
		// verificate le condizioni fisiche che lo hanno generato
		memset(&alarmCurrent, 0, sizeof(struct alarm));

		manageAlarmChildGuard(ptrAlarmCurrent);

		// allarme terminato riparto dall'indice 0 dell'array di strutture
		ShowAlarmStr((int)alarmList[StartAlmArrIdx].code, " terminato");
		StartAlmArrIdx = 0;
	}


	dummy = dummy + 1;
}

void manageAlarmChildGuard(struct alarm * ptrAlarm){
	struct alarm * myAlarmPointer;

	myAlarmPointer = ptrAlarm;

	//setting child guard depending on alarm security action
	switch(myAlarmPointer->secActType){
	case SECURITY_STOP_ALL_ACTUATOR:
		if(myAlarmPointer->active == ACTIVE_TRUE)
			currentGuard[GUARD_ALARM_STOP_ALL_ACTUATOR].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		else
			currentGuard[GUARD_ALARM_STOP_ALL_ACTUATOR].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
		break;

	case SECURITY_STOP_ALL:
		if(myAlarmPointer->active == ACTIVE_TRUE)
			currentGuard[GUARD_ALARM_STOP_ALL].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		else
			currentGuard[GUARD_ALARM_STOP_ALL].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
		break;

	case SECURITY_STOP_PERF_PUMP:
		if(myAlarmPointer->active == ACTIVE_TRUE)
			currentGuard[GUARD_ALARM_STOP_PERF_PUMP].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		else
			currentGuard[GUARD_ALARM_STOP_PERF_PUMP].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
		break;

	case SECURITY_STOP_PURIF_PUMP:
		if(myAlarmPointer->active == ACTIVE_TRUE)
			currentGuard[GUARD_ALARM_STOP_PURIF_PUMP].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		else
			currentGuard[GUARD_ALARM_STOP_PURIF_PUMP].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
		break;

	case SECURITY_STOP_OXYG_PUMP:
		if(myAlarmPointer->active == ACTIVE_TRUE)
			currentGuard[GUARD_ALARM_STOP_OXYG_PUMP].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		else
			currentGuard[GUARD_ALARM_STOP_OXYG_PUMP].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
		break;

	case SECURITY_STOP_PELTIER:
		if(myAlarmPointer->active == ACTIVE_TRUE)
			currentGuard[GUARD_ALARM_STOP_PELTIER].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		else
			currentGuard[GUARD_ALARM_STOP_PELTIER].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
		break;

	case SECURITY_WAIT_CONFIRM:
		if(myAlarmPointer->active == ACTIVE_TRUE)
			currentGuard[GUARD_ALARM_WAIT_CONFIRM].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		else
			currentGuard[GUARD_ALARM_WAIT_CONFIRM].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
		break;

	default:
		break;
	}

}

// ritorna il numero di intervalli di 50 msec trascorsi dal valore last
unsigned long msTick_elapsed( unsigned long last )
{
	int elapsed = timerCounterModBus;
	if ( elapsed >= last )
		elapsed -= last;
	else
		elapsed += (0xFFFFFFFFUL - last + 1);
	return elapsed;
}
