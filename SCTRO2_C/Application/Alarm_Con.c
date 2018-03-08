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
#include "BUZZER_LOW_C.h"
#include "BUZZER_MEDIUM_C.h"
#include "BUZZER_HIGH_C.h"
#include "general_func.h"


// FM questa lista devo costruirla mettendo prima i PHYSIC_TRUE e poi i PHYSIC_FALSE,
// ognuno deve poi essere ordinato in base alla priorita' ???
// Quando si aggiorna questa lista bisogna ricordarsi di aggiornare anche la define ALARM_ACTIVE_IN_STRUCT
// che definisce il numero di elementi contenuti in alarmList
struct alarm alarmList[] =
{
		//{CODE_ALARM0, PHYSIC_TRUE, TYPE_ALARM_CONTROL, PRIORITY_LOW, OVRD_ENABLE, SILENCE_ALLOWED},
		{CODE_ALARM_PRESS_ART_HIGH,        PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH, 2000, 2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull}, 		/* 0 */
		{CODE_ALARM_PRESS_ART_LOW,         PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH, 2000, 2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull}, 		/* 1 */
		{CODE_ALARM_AIR_PRES_ART,          PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_SFA_AIR_DET,           PRIORITY_HIGH, 1000, 1000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_ALLOWED,     &alarmManageNull}, 		/* 2 */
		{CODE_ALARM_AIR_PRES_VEN,          PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_SFV_AIR_DET,           PRIORITY_HIGH, 1000, 1000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_ALLOWED,     &alarmManageNull}, 		/* 3 */
		{CODE_ALARM_AIR_PRES_ADSRB_FILTER, PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_SAF_AIR_FILTER,        PRIORITY_HIGH, 1000, 1000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_ALLOWED,     &alarmManageNull}, 		/* 4 */
		{CODE_ALARM_TEMP_ART_HIGH,         PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH, 5000, 2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull}, 	    /* 5 */
		{CODE_ALARM_PRESS_ADS_FILTER_HIGH, PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH, 2000, 2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull},	    /* 6 */
		{CODE_ALARM_FLOW_PERF_ART_HIGH,    PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH, 2000, 2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull}, 		/* 7 */
		{CODE_ALARM_FLOW_ART_NOT_DETECTED, PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH, 2000, 2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull}, 		/* 8 */
		{CODE_ALARM_PRESS_VEN_HIGH, 	   PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH, 2000, 2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull}, 		/* 9 */
		{CODE_ALARM_PRESS_VEN_LOW, 		   PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH, 2000, 2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull}, 		/* 10 */
		{CODE_ALARM_TEMP_SENS_NOT_DETECTED,PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH, 2000, 2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull}, 		/* 11 */

		// allarmi di cover aperte
		{CODE_ALARM_PUMP_PERF_COVER_OPEN,  PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH,     0,    0, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull},	    /* 12 */
		{CODE_ALARM_PUMP_PURIF_COVER_OPEN, PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH,     0,    0, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull},	    /* 13 */
		{CODE_ALARM_PUMP_OXYG_COVER_OPEN,  PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH,     0,    0, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull},	    /* 14 */
		// allarme di livello liquido troppo basso
		{CODE_ALARM_TANK_LEVEL_LOW,        PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH,    500, 500, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull},	    /* 15 */
		// allarme livello del liquido troppo alto
		{CODE_ALARM_TANK_LEVEL_HIGH,       PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACT_WAIT_CMD, PRIORITY_HIGH,    500, 500, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull},	    /* 16 */

		// allarme differenza tra flusso venoso misurato e flusso calcolato in base al guadagno pompa troppo alto
		{CODE_ALARM_DELTA_FLOW_ART,        PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH,     500, 500, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull},	    /* 17 */
		// allarme differenza tra flusso arterioso misurato e flusso calcolato in base al guadagno pompa troppo alto
		{CODE_ALARM_DELTA_FLOW_VEN,        PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH,     500, 500, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull},	    /* 18 */
		// allarme differenza tra temperatura vaschetta e temperatura fluido arterioso troppo alta
		{CODE_ALARM_DELTA_TEMP_REC_ART,    PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH,     500, 500, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull},	    /* 19 */
		// allarme differenza tra temperatura vaschetta e temperatura fluido venoso troppo alta
		{CODE_ALARM_DELTA_TEMP_REC_VEN,    PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH,     500, 500, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull},	    /* 20 */

		{CODE_ALARM_MODBUS_ACTUATOR_SEND,  PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_WAIT_CONFIRM,          PRIORITY_LOW,        0,   0, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull},	    /* 21 */
		{}
};

void SetAllAlarmEnableFlags(void)
{
	GlobalFlags.FlagsDef.EnableLevHighAlarm = 1;          // Abilito allarme di livello alto (troppo pieno)
	GlobalFlags.FlagsDef.EnableLevLowAlarm = 1;           // Abilito allarme di livello troppo basso
	GlobalFlags.FlagsDef.EnableCoversAlarm = 1;           // Abilito allarme di cover
	GlobalFlags.FlagsDef.EnablePressSensLowAlm = 1;       // abilito allarme pressione bassa
	GlobalFlags.FlagsDef.EnablePressSensHighAlm = 1;      // abilito allarme pressione alta
	GlobalFlags.FlagsDef.EnableTempArtHighAlm = 1;        // abilito allarme temperatura alta
	GlobalFlags.FlagsDef.EnableDeltaFlowArtAlarm = 1;     // abilito allarme delta flusso arterioso troppo alto
	GlobalFlags.FlagsDef.EnableDeltaFlowVenAlarm = 1;     // abilito allarme delta flusso venoso troppo alto
	GlobalFlags.FlagsDef.EnableDeltaTempRecVenAlarm = 1;  // abilito allarme delta temperatura recipiente e line venosa troppo alta
	GlobalFlags.FlagsDef.EnableDeltaTempRecArtAlarm = 1;  // abilito allarme delta temperatura recipiente e line arteriosa troppo alta
}

// Questa funzione serve per forzare ad off un eventuale allarme.
// Per il momento la uso solo per il caso di CODE_ALARM_TANK_LEVEL_HIGH
void ForceAlarmOff(char code)
{
	switch (code)
	{
		case CODE_ALARM_TANK_LEVEL_HIGH:
			GlobalFlags.FlagsDef.EnableLevHighAlarm = 0;    // forzo allarme di livello alto (troppo pieno) a off
			break;
		case CODE_ALARM_TANK_LEVEL_LOW:
			GlobalFlags.FlagsDef.EnableLevLowAlarm = 0;    // forzo allarme di livello troppo basso a off
			break;
		case CODE_ALARM_PUMP_PERF_COVER_OPEN:
		case CODE_ALARM_PUMP_PURIF_COVER_OPEN:
		case CODE_ALARM_PUMP_OXYG_COVER_OPEN:
			GlobalFlags.FlagsDef.EnableCoversAlarm = 0;    // forzo allarme di cover a off
			break;
		case CODE_ALARM_PRESS_ART_LOW:
		case CODE_ALARM_PRESS_VEN_LOW:
			GlobalFlags.FlagsDef.EnablePressSensLowAlm = 0;  // forzo allarme pressione bassa off
			break;
		case CODE_ALARM_PRESS_ART_HIGH:
		case CODE_ALARM_PRESS_VEN_HIGH:
		case CODE_ALARM_PRESS_ADS_FILTER_HIGH:
			GlobalFlags.FlagsDef.EnablePressSensHighAlm = 0; // forzo allarme pressione alta off
			break;
		case CODE_ALARM_TEMP_ART_HIGH:
			GlobalFlags.FlagsDef.EnableTempArtHighAlm = 0;        // forzo allarme temperatura arteriosa alta off
			break;
		case CODE_ALARM_DELTA_FLOW_ART:
			GlobalFlags.FlagsDef.EnableDeltaFlowArtAlarm = 0;     // forzo allarme delta flusso arterioso troppo alto off
			break;
		case CODE_ALARM_DELTA_FLOW_VEN:
			GlobalFlags.FlagsDef.EnableDeltaFlowVenAlarm = 0;     // forzo allarme delta flusso venoso troppo alto off
			break;
		case CODE_ALARM_DELTA_TEMP_REC_VEN:
			GlobalFlags.FlagsDef.EnableDeltaTempRecVenAlarm = 0;  // forzo allarme delta temperatura recipiente e line venosa troppo alta off
			break;
		case CODE_ALARM_DELTA_TEMP_REC_ART:
			GlobalFlags.FlagsDef.EnableDeltaTempRecArtAlarm = 0;  // forzo allarme delta temperatura recipiente e line arteriosa troppo alta off
			break;
	}
}

// forzo l'allarme corrente in off utilizzando la flag di controllo
// serve per poter andare avanti con il programma dopo l'arrivo di
// un comando da gui ignorando l'allarme corrente
void ForceCurrentAlarmOff(void)
{
	if(alarmCurrent.code)
	{
		for(int i = 0; i < ALARM_ACTIVE_IN_STRUCT; i++)
		{
			if(alarmList[i].code == alarmCurrent.code)
			{
				ForceAlarmOff(alarmCurrent.code);
				break;
			}
		}
	}
}

void ShowAlarmStr(int i, char * str)
{
	char s[100];
	switch (i)
	{
		case CODE_ALARM_PRESS_ART_HIGH:
			strcpy(s, "AL_PRESS_ART_HIGH");
			strcat(s, str);
			DebugStringStr(s);
			break;
		case CODE_ALARM_PRESS_ART_LOW:
			strcpy(s, "AL_PRESS_ART_LOW");
			strcat(s, str);
			DebugStringStr(s);
			break;
		case CODE_ALARM_AIR_PRES_ART:
			strcpy(s, "AL_AIR_PRES_ART");
			strcat(s, str);
			DebugStringStr(s);
			break;
		case CODE_ALARM_AIR_PRES_VEN:
			strcpy(s, "AL_AIR_PRES_VEN");
			strcat(s, str);
			DebugStringStr(s);
			break;
		case CODE_ALARM_AIR_PRES_ADSRB_FILTER:
			strcpy(s, "AL_AIR_PRES_ADSRB_FILT");
			strcat(s, str);
			DebugStringStr(s);
			break;
		case CODE_ALARM_TEMP_ART_HIGH:
			strcpy(s, "AL_TEMP_ART_HIGH");
			strcat(s, str);
			DebugStringStr(s);
			break;
		case CODE_ALARM_PRESS_ADS_FILTER_HIGH:
			strcpy(s, "AL_PRESS_ADS_FILT_HIGH");
			strcat(s, str);
			DebugStringStr(s);
			break;
		case CODE_ALARM_FLOW_PERF_ART_HIGH:
			strcpy(s, "AL_FLOW_PERF_ART_HIGH");
			strcat(s, str);
			DebugStringStr(s);
			break;
		case CODE_ALARM_FLOW_ART_NOT_DETECTED:
			strcpy(s, "AL_FLOW_ART_NOT_DETEC");
			strcat(s, str);
			DebugStringStr(s);
			break;
		case CODE_ALARM_PRESS_VEN_HIGH:
			strcpy(s, "AL_PRESS_VEN_HIGH");
			strcat(s, str);
			DebugStringStr(s);
			break;
		case CODE_ALARM_PRESS_VEN_LOW:
			strcpy(s, "AL_PRESS_VEN_LOW");
			strcat(s, str);
			DebugStringStr(s);
			break;
		case CODE_ALARM_TEMP_SENS_NOT_DETECTED:
			strcpy(s, "AL_TEMP_SENS_NOT_DETEC");
			strcat(s, str);
			DebugStringStr(s);
			break;
		case CODE_ALARM_PUMP_PERF_COVER_OPEN:
			strcpy(s, "PUMP_COVER_OPEN");
			strcat(s, str);
			DebugStringStr(s);
			break;
		case CODE_ALARM_PUMP_PURIF_COVER_OPEN:
			strcpy(s, "PURIF_COVER_OPEN");
			strcat(s, str);
			DebugStringStr(s);
			break;
		case CODE_ALARM_PUMP_OXYG_COVER_OPEN:
			strcpy(s, "OXYG_COVER_OPEN");
			strcat(s, str);
			DebugStringStr(s);
			break;
		case CODE_ALARM_TANK_LEVEL_HIGH:
			strcpy(s, "TANK_LEVEL_HIGH");
			strcat(s, str);
			DebugStringStr(s);
			break;
		case CODE_ALARM_MODBUS_ACTUATOR_SEND:
			strcpy(s, "AL_MODBUS_ACTUAT_SEND");
			strcat(s, str);
			DebugStringStr(s);
			break;
		case CODE_ALARM_TANK_LEVEL_LOW:
			strcpy(s, "AL_TANK_LEVEL_LOW");
			strcat(s, str);
			DebugStringStr(s);
			break;
		case CODE_ALARM_DELTA_FLOW_ART:
			strcpy(s, "AL_DELTA_FLOW_ART");
			strcat(s, str);
			DebugStringStr(s);
			break;
		case CODE_ALARM_DELTA_FLOW_VEN:
			strcpy(s, "AL_DELTA_FLOW_VEN");
			strcat(s, str);
			DebugStringStr(s);
			break;
		case CODE_ALARM_DELTA_TEMP_REC_ART:
			strcpy(s, "AL_DELTA_TEMP_REC_ART");
			strcat(s, str);
			DebugStringStr(s);
			break;
		case CODE_ALARM_DELTA_TEMP_REC_VEN:
			strcpy(s, "AL_DELTA_TEMP_REC_VEN");
			strcat(s, str);
			DebugStringStr(s);
			break;
	}
}

void alarmConInit(void){
	ptrAlarmCurrent = &alarmList[0];
}

int StartAlmArrIdx = 0;
int i_al;

void alarmEngineAlways(void)
{

	static int StrAlarmWritten = 0;
	static int IdxCurrAlarm = 0xff;


/*Faccio uno switch su tutta la macchina a stati in modo
 * gestire ogni allarme in funzioine dello stato in cui sono*/

	if(GlobalFlags.FlagsDef.EnableAllAlarms)
	{
		switch(ptrCurrentState->state)
		{
			case STATE_NULL:
			{
				/* DA DEBUGGARE*/
				manageAlarmFlowSensNotDetected();
				manageAlarmIrTempSensNotDetected();

				break;
			}

			case STATE_ENTRY:
			{
				/* DA DEBUGGARE*/
				manageAlarmFlowSensNotDetected();
				manageAlarmIrTempSensNotDetected();

				break;
			}

			case STATE_IDLE:
			{
				/* DA DEBUGGARE*/
				manageAlarmFlowSensNotDetected();
				manageAlarmIrTempSensNotDetected();

				break;
			}

			case STATE_SELECT_TREAT:
			{
				/* DA DEBUGGARE*/
				manageAlarmFlowSensNotDetected();
				manageAlarmIrTempSensNotDetected();

				break;
			}

			case STATE_T1_NO_DISPOSABLE:
			{
				/* DA DEBUGGARE*/
				manageAlarmFlowSensNotDetected();
				manageAlarmIrTempSensNotDetected();

				break;
			}

			case STATE_MOUNTING_DISP:
			{
				/* DA DEBUGGARE*/
				manageAlarmFlowSensNotDetected();
				manageAlarmIrTempSensNotDetected();

				break;
			}

			case STATE_TANK_FILL:
			{
				/* DA DEBUGGARE*/
				manageAlarmFlowSensNotDetected();
				manageAlarmIrTempSensNotDetected();

				break;
			}

			case STATE_PRIMING_PH_1:
			{
				manageAlarmFlowSensNotDetected();
				manageAlarmIrTempSensNotDetected();

				//verifica physic pressioni
				manageAlarmPhysicPressSensHigh();
				//manageAlarmPhysicPressSensLow(); non serve questo allarme in priming

				//verifica physic ir temp sens
				manageAlarmPhysicTempSens();

				manageAlarmLiquidLevelHigh();
				if(GetTherapyType() == LiverTreat)
					manageAlarmCoversPumpLiver();
				else if(GetTherapyType() == KidneyTreat)
					manageAlarmCoversPumpKidney();
				break;
			}

			case STATE_PRIMING_PH_2:
			{
				manageAlarmFlowSensNotDetected();
				manageAlarmIrTempSensNotDetected();

				//verifica physic pressioni
				manageAlarmPhysicPressSensHigh();
				//manageAlarmPhysicPressSensLow(); non serve questo allarme in priming

				//verifica physic ir temp sens
				manageAlarmPhysicTempSens();

				manageAlarmLiquidLevelHigh();
				if(GetTherapyType() == LiverTreat)
					manageAlarmCoversPumpLiver();
				else if(GetTherapyType() == KidneyTreat)
					manageAlarmCoversPumpKidney();
				break;
			}

			case STATE_TREATMENT_KIDNEY_1:
			{
				//verifica physic pressioni
				manageAlarmPhysicPressSensHigh();
				manageAlarmPhysicPressSensLow();

				//verifica physic flow sensor (presenza aria)
				manageAlarmPhysicUFlowSens();
				manageAlarmSAFAirSens();
				manageAlarmPhysicUFlowSensVen();


				//verifica physic ir temp sens
				manageAlarmPhysicTempSens();

				//verifica physic flusso di perfusione arteriosa alto
				manageAlarmPhysicFlowPerfArtHigh();

				//verifica  flusso  non rilevato
				manageAlarmFlowSensNotDetected();

				//verifica temperatura noin rilevata
				manageAlarmIrTempSensNotDetected();

				if(GetTherapyType() == LiverTreat)
					manageAlarmCoversPumpLiver();
				else if(GetTherapyType() == KidneyTreat)
					manageAlarmCoversPumpKidney();

				manageAlarmLiquidLevelLow();
				manageAlarmDeltaFlowArt();
				manageAlarmDeltaFlowVen();
				manageAlarmDeltaTempRecArt();
				manageAlarmDeltaTempRecVen();
				break;
			}

			case STATE_PRIMING_WAIT:
				//verifica physic pressioni
				manageAlarmPhysicPressSensHigh();
				manageAlarmPhysicPressSensLow();

				//verifica physic ir temp sens
				manageAlarmPhysicTempSens();
				break;

			case STATE_PRIMING_RICIRCOLO:
				//verifica physic pressioni
				manageAlarmPhysicPressSensHigh();
				manageAlarmPhysicPressSensLow();

				//verifica physic ir temp sens
				manageAlarmPhysicTempSens();
				if(GetTherapyType() == LiverTreat)
					manageAlarmCoversPumpLiver();
				else if(GetTherapyType() == KidneyTreat)
					manageAlarmCoversPumpKidney();
				break;

			case STATE_WAIT_TREATMENT:
				//verifica physic pressioni
				manageAlarmPhysicPressSensHigh();
				manageAlarmPhysicPressSensLow();

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
				manageAlarmPhysicPressSensHigh();
				manageAlarmPhysicPressSensLow();

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
				manageAlarmIrTempSensNotDetected();
				break;
			}

			case STATE_EMPTY_DISPOSABLE:
			case STATE_EMPTY_DISPOSABLE_1:
			{
				manageAlarmFlowSensNotDetected();
				/* DA DEBUGGARE*/
				manageAlarmIrTempSensNotDetected();

				break;
			}

			case STATE_EMPTY_DISPOSABLE_2:
			{
				/* DA DEBUGGARE*/
				manageAlarmFlowSensNotDetected();
				manageAlarmIrTempSensNotDetected();

				break;
			}

			case STATE_UNMOUNT_DISPOSABLE:
				// per il momento, in questo stato non sono previsti allarmi.
				// In questo stato sono azionate solo le pinch per smontare
				// il disposable
				break;

			case STATE_WASHING:
			{
				/* DA DEBUGGARE*/
				manageAlarmFlowSensNotDetected();
				manageAlarmIrTempSensNotDetected();

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
	}


	for(i_al=StartAlmArrIdx; i_al<ALARM_ACTIVE_IN_STRUCT; i_al++)
	{
		if((alarmList[i_al].physic == PHYSIC_TRUE) && (alarmList[i_al].active != ACTIVE_TRUE))
		{
			ptrAlarmCurrent = &alarmList[i_al];
			alarmList[i_al].prySafetyActionFunc();
			StartAlmArrIdx = i_al;
			IdxCurrAlarm = i_al;

			// FM forse qui devo interrompere perche' ho trovato una condizione di allarme da attivare
			// e devo gestirla prima di andare a vedere le altre
			break;
		}
		else if((alarmList[i_al].active == ACTIVE_TRUE) && (alarmList[i_al].physic == PHYSIC_FALSE))
		{
			ptrAlarmCurrent = &alarmList[i_al];
			alarmList[i_al].prySafetyActionFunc();

			//StartAlmArrIdx = i_al;  // allarme finito riparto dall'inizio della struttura
			//IdxCurrAlarm = i_al;

			// FM forse qui devo interrompere perche' ho trovato una condizione di allarme da disattivare
			// e devo gestirla prima di andare a vedere le altre
			break;
		}
		else
		{
			if(StrAlarmWritten)
			{
				// allarme ancora in corso, sono in attesa di ACTIVE_FALSE
				// quindi, per ora, non posso prendere in considerazione altri allarmi
				break;
			}
		}
	}

	if( !StrAlarmWritten && (StartAlmArrIdx < ALARM_ACTIVE_IN_STRUCT))
	{
		if(alarmList[IdxCurrAlarm].active == ACTIVE_TRUE)
			StrAlarmWritten = 1;
		else
		{
			// potrebbe essersi verificato un allarme molto breve che non e' riuscito
			// ad attivarsi, forzo una ripartenza dall'inizio della tabella
			StartAlmArrIdx = 0;
		}
	}
	else if(StrAlarmWritten == 1)
	{
		ShowAlarmStr((int)alarmList[IdxCurrAlarm].code, " on");
		StrAlarmWritten = 2;
	}
	else if(StrAlarmWritten == 2)
	{
		if(alarmList[IdxCurrAlarm].active == ACTIVE_FALSE)
		{
			// allarme terminato
			StrAlarmWritten = 0;
			ShowAlarmStr((int)alarmList[IdxCurrAlarm].code, " off");
			StartAlmArrIdx = 0;
		}
	}
}


// controllo se il delta di flusso tra quello misurato e quello calcolato in base al guadagno pompa e'
// troppo elevato
void manageAlarmDeltaFlowArt(void)
{
	if(GlobalFlags.FlagsDef.EnableDeltaFlowArtAlarm)
	{
		if(sensor_UFLOW[ARTERIOUS_AIR_SENSOR].Average_Flow_Val > 0.0)
		{
			float Pump_Gain = DEFAULT_ART_PUMP_GAIN;
			float flow = Pump_Gain * pumpPerist[0].actualSpeedOld;
			float delta = flow - sensor_UFLOW[ARTERIOUS_AIR_SENSOR].Average_Flow_Val;
			float max = (float)MAX_ART_FLOW_DIFF_FROM_CALC_AND_MIS;
			float min = -(float)MAX_ART_FLOW_DIFF_FROM_CALC_AND_MIS;
			if((delta > max) || (delta < min))
			{
				alarmList[DELTA_FLOW_ART].physic = PHYSIC_TRUE;
			}
			else
			{
				alarmList[DELTA_FLOW_ART].physic = PHYSIC_FALSE;
			}
		}
	}
	else
		alarmList[DELTA_FLOW_ART].physic = PHYSIC_FALSE;
}

// controllo se il delta di flusso venoso tra quello misurato e quello calcolato in base al guadagno pompa e'
// troppo elevato
void manageAlarmDeltaFlowVen(void)
{
	if(GlobalFlags.FlagsDef.EnableDeltaFlowVenAlarm)
	{
		if(sensor_UFLOW[VENOUS_AIR_SENSOR].Average_Flow_Val > 0.0)
		{
			float Pump_Gain = DEFAULT_VEN_PUMP_GAIN;
			float flow = Pump_Gain * pumpPerist[1].actualSpeedOld;
			float delta = flow - sensor_UFLOW[VENOUS_AIR_SENSOR].Average_Flow_Val;
			float max = (float)MAX_VEN_FLOW_DIFF_FROM_CALC_AND_MIS;
			float min = -(float)MAX_VEN_FLOW_DIFF_FROM_CALC_AND_MIS;
			if((delta > max) || (delta < min))
			{
				alarmList[DELTA_FLOW_VEN].physic = PHYSIC_TRUE;
			}
			else
			{
				alarmList[DELTA_FLOW_VEN].physic = PHYSIC_FALSE;
			}
		}
	}
	else
		alarmList[DELTA_FLOW_VEN].physic = PHYSIC_FALSE;
}

// controllo se il deta di temperatura tra recipiente e liquido arterioso e' troppo alta
void manageAlarmDeltaTempRecArt(void)
{
	if(GlobalFlags.FlagsDef.EnableDeltaTempRecArtAlarm)
	{
		float tart = sensorIR_TM[0].tempSensValue;
		float trec = sensorIR_TM[1].tempSensValue;
		float delta = tart - trec;
		if((delta > MAX_DELTA_TEMP_ART_AND_REC) || (delta < (-MAX_DELTA_TEMP_ART_AND_REC)) )
		{
			alarmList[DELTA_TEMP_REC_ART].physic = PHYSIC_TRUE;
		}
		else
		{
			alarmList[DELTA_TEMP_REC_ART].physic = PHYSIC_FALSE;
		}
	}
	else
		alarmList[DELTA_TEMP_REC_ART].physic = PHYSIC_FALSE;
}

// controllo se il deta di temperatura tra recipiente e liquido venoso e' troppo alta
void manageAlarmDeltaTempRecVen(void)
{
	if(GlobalFlags.FlagsDef.EnableDeltaTempRecVenAlarm)
	{
		float tven = sensorIR_TM[2].tempSensValue;
		float trec = sensorIR_TM[1].tempSensValue;
		float delta = tven - trec;
		if((delta > MAX_DELTA_TEMP_VEN_AND_REC) || (delta < (-MAX_DELTA_TEMP_VEN_AND_REC)) )
		{
			alarmList[DELTA_TEMP_REC_VEN].physic = PHYSIC_TRUE;
		}
		else
		{
			alarmList[DELTA_TEMP_REC_VEN].physic = PHYSIC_FALSE;
		}
	}
	else
		alarmList[DELTA_TEMP_REC_VEN].physic = PHYSIC_FALSE;
}


// controlla se sono al di sotto del livello minimo
void manageAlarmLiquidLevelLow(void)
{
	if(GlobalFlags.FlagsDef.EnableLevLowAlarm)
	{
		if(LiquidAmount <= MIN_LIQUID_LEV_IN_PERC)
		{
			alarmList[LIQUID_LEVEL_LOW].physic = PHYSIC_TRUE;
		}
		else
		{
			alarmList[LIQUID_LEVEL_LOW].physic = PHYSIC_FALSE;
		}
	}
	else
		alarmList[LIQUID_LEVEL_LOW].physic = PHYSIC_FALSE;
}


// controlla se supera il livello massimo
void manageAlarmLiquidLevelHigh(void)
{
	if(GlobalFlags.FlagsDef.EnableLevHighAlarm)
	{
		if(LiquidAmount >= MAX_LIQUID_LEV_IN_PERC)
		{
			alarmList[LIQUID_LEVEL_HIGH].physic = PHYSIC_TRUE;
		}
		else
		{
			alarmList[LIQUID_LEVEL_HIGH].physic = PHYSIC_FALSE;
		}
	}
	else
		alarmList[LIQUID_LEVEL_HIGH].physic = PHYSIC_FALSE;
}


// cover della pompa di depurazione nel caso di fegato o della
// pompa arteriosa in kidney
void manageAlarmCoversPumpLiver(void)
{
	if(GlobalFlags.FlagsDef.EnableCoversAlarm)
	{
		if(CoversState == 0)
			alarmList[PURIF_COVER_OPEN].physic = PHYSIC_TRUE;
		else
			alarmList[PURIF_COVER_OPEN].physic = PHYSIC_FALSE;

		if(CoversState == 1)
			alarmList[PERF_COVER_OPEN].physic = PHYSIC_TRUE;
		else
			alarmList[PERF_COVER_OPEN].physic = PHYSIC_FALSE;

		if((CoversState == 2) || (CoversState == 3))
			alarmList[OXYG_COVER_OPEN].physic = PHYSIC_TRUE;
		else
			alarmList[OXYG_COVER_OPEN].physic = PHYSIC_FALSE;
	}
	else
	{
		alarmList[PURIF_COVER_OPEN].physic = PHYSIC_FALSE;
		alarmList[PERF_COVER_OPEN].physic = PHYSIC_FALSE;
		alarmList[OXYG_COVER_OPEN].physic = PHYSIC_FALSE;
	}
}

// cover della pompa arteriosa  nel fegato (non usata in kidney)
void manageAlarmCoversPumpKidney(void)
{
	if(GlobalFlags.FlagsDef.EnableCoversAlarm)
	{
		if(CoversState == 0)
			alarmList[PERF_COVER_OPEN].physic = PHYSIC_TRUE;
		else
			alarmList[PERF_COVER_OPEN].physic = PHYSIC_FALSE;

		if((CoversState == 2) || (CoversState == 3))
			alarmList[OXYG_COVER_OPEN].physic = PHYSIC_TRUE;
		else
			alarmList[OXYG_COVER_OPEN].physic = PHYSIC_FALSE;
	}
	else
	{
		alarmList[PERF_COVER_OPEN].physic = PHYSIC_FALSE;
		alarmList[OXYG_COVER_OPEN].physic = PHYSIC_FALSE;
	}
}



void manageAlarmPhysicPressSensLow(void)
{
	if(GlobalFlags.FlagsDef.EnablePressSensLowAlm)
	{
		if(PR_ART_mmHg_Filtered <= PR_ART_LOW)
		{
			alarmList[PRESS_ART_LOW].physic = PHYSIC_TRUE;
		}
		else
		{
			alarmList[PRESS_ART_LOW].physic = PHYSIC_FALSE;
		}
	/*
		if(PR_VEN_mmHg_Filtered <= PR_VEN_LOW && GetTherapyType() == LiverTreat)
		{
			alarmList[PRESS_VEN_LOW].physic = PHYSIC_TRUE;
		}
		else
		{
			alarmList[PRESS_VEN_LOW].physic = PHYSIC_FALSE;
		}
	*/
	}
	else
	{
		alarmList[PRESS_ART_LOW].physic = PHYSIC_FALSE;
		alarmList[PRESS_VEN_LOW].physic = PHYSIC_FALSE;
	}
}

void manageAlarmPhysicPressSensHigh(void)
{

	if(GlobalFlags.FlagsDef.EnablePressSensHighAlm)
	{
		if(PR_ART_mmHg_Filtered > PR_ART_HIGH)
		{
			alarmList[PRESS_ART_HIGH].physic = PHYSIC_TRUE;
		}
		else
		{
			alarmList[PRESS_ART_HIGH].physic = PHYSIC_FALSE;
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
		if((PR_VEN_Sistolyc_mmHg /*PR_VEN_mmHg_Filtered*/ > PR_VEN_HIGH) && (GetTherapyType() == LiverTreat))
		{
			alarmList[PRESS_VEN_HIGH].physic = PHYSIC_TRUE;
		}
		else
		{
			alarmList[PRESS_VEN_HIGH].physic = PHYSIC_FALSE;
		}
	}
	else
	{
		alarmList[PRESS_ART_HIGH].physic = PHYSIC_FALSE;
		alarmList[PRESS_ADS_FILTER_HIGH].physic = PHYSIC_FALSE;
		alarmList[PRESS_VEN_HIGH].physic = PHYSIC_FALSE;
	}
}


void manageAlarmPhysicTempSens(void)
{
	if(GlobalFlags.FlagsDef.EnableTempArtHighAlm)
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
	else
		alarmList[TEMP_ART_HIGH].physic = PHYSIC_FALSE;
}


// sensore aria filtro
void manageAlarmSAFAirSens(void)
{
	if(DisableAllAirAlarm)
	{
		alarmList[SAF_AIR_SENSOR].physic = PHYSIC_FALSE;
		return;
	}
	else
	{
		if(Air_1_Status == AIR)
			alarmList[SAF_AIR_SENSOR].physic = PHYSIC_TRUE;
		else
			alarmList[SAF_AIR_SENSOR].physic = PHYSIC_FALSE;
	}
}

void manageAlarmPhysicUFlowSensVen(void)
{
	if(DisableAllAirAlarm)
	{
		alarmList[AIR_PRES_VEN].physic = PHYSIC_FALSE;
		return;
	}
	else
	{
//		if((sensor_UFLOW[VENOUS_AIR_SENSOR].bubbleSize >= 25) ||
//			(sensor_UFLOW[VENOUS_AIR_SENSOR].bubblePresence == MASK_ERROR_BUBBLE_ALARM))
		if(((sensor_UFLOW[VENOUS_AIR_SENSOR].bubbleSize >= 50) ||
			(sensor_UFLOW[VENOUS_AIR_SENSOR].bubblePresence == MASK_ERROR_BUBBLE_ALARM)) &&
			(sensor_UFLOW[VENOUS_AIR_SENSOR].bubbleSize != 255))
		{
			alarmList[AIR_PRES_VEN].physic = PHYSIC_TRUE;
		}
		else
		{
			alarmList[AIR_PRES_VEN].physic = PHYSIC_FALSE;
		}
	}
}


void manageAlarmPhysicUFlowSens(void){
	if(DisableAllAirAlarm)
	{
		alarmList[AIR_PRES_ART].physic = PHYSIC_FALSE;
		return;
	}
	else
	{
//		if(
//			(sensor_UFLOW[ARTERIOUS_AIR_SENSOR].bubbleSize >= 25) ||
//			(sensor_UFLOW[ARTERIOUS_AIR_SENSOR].bubblePresence == MASK_ERROR_BUBBLE_ALARM)
//			)
		if(
			((sensor_UFLOW[ARTERIOUS_AIR_SENSOR].bubbleSize >= 50) ||
			(sensor_UFLOW[ARTERIOUS_AIR_SENSOR].bubblePresence == MASK_ERROR_BUBBLE_ALARM)) &&
			(sensor_UFLOW[ARTERIOUS_AIR_SENSOR].bubbleSize != 255)
			)
		{
			alarmList[AIR_PRES_ART].physic = PHYSIC_TRUE;
		}
		else
		{
			alarmList[AIR_PRES_ART].physic = PHYSIC_FALSE;
		}
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
	int i;
	for ( i = 0; i <2; i++)
	{
		/*se non ricevo 10 msg consecutivi da un sensore di flusso ossia il sensore non risponde per 5 secondi consecutivi vado in allarme*/
		if (sensor_UFLOW[i].RequestMsgProcessed > MAX_MSG_CONSECUTIVE_FLOW_SENS_NOT_DETECTED)
		{
			alarmList[FLOW_SENS_NOT_DETECTED].physic = PHYSIC_TRUE;
			//sensor_UFLOW[i].RequestMsgProcessed = 0;
			/*in questo caso bisogna comunicarlo all'SBC che metterà a video un pop up per le possibili soluzioni*/
			break;
		}
	}

	//se ho ciclato tutto il for senza trovare allarmi e precedentemente era stato attivato un allarme
	if(i == 2 && alarmList[FLOW_SENS_NOT_DETECTED].physic == PHYSIC_TRUE)
	{
		alarmList[FLOW_SENS_NOT_DETECTED].physic = PHYSIC_FALSE;

		//sensor_UFLOW[i].RequestMsgProcessed = 0;
		for (int j = 0; j<2; j++)
			sensor_UFLOW[i].RequestMsgProcessed = 0;
	}
}

void manageAlarmIrTempSensNotDetected(void)
{
	int i;

	for (i = 0; i <3; i++)
	{
		/*se non ricevo 10 msg consecutivi da un sensore di temperatura ossia il sensore non risposnde per 6 secondi consecutivi vado in allarme*/
		if (sensorIR_TM[i].ErrorMSG > MAX_MSG_CONSECUTIVE_IR_TEMP_SENS_NOT_DETECTED)
		{
			alarmList[IR_SENS_NOT_DETECTED].physic = PHYSIC_TRUE;

			/*in questo caso bisogna comunicarlo all'SBC che metterà a video un pop up per le possibili soluzioni*/
			break;
		}
	}

	//se ho ciclato tutto il for senza trovare allarmi e precedentemente era stato attivato un allarme
	if(i == 3 && alarmList[IR_SENS_NOT_DETECTED].physic == PHYSIC_TRUE)
	{
		alarmList[IR_SENS_NOT_DETECTED].physic = PHYSIC_FALSE;

		for (int j = 0; j<3; j++)
			sensorIR_TM[j].ErrorMSG = 0;
	}
}

void manageAlarmActuatorModbusNotRespond(void)
{
	int i;
	for (i = 0; i <8; i++)
	{
		/*se non ricevo 10 msg consecutivi da un sensore di temperatura ossia il sensore non risposnde per 6 secondi consecutivi vado in allarme*/
		if (CountErrorModbusMSG[i] > MAX_MSG_CONSECUTIVE_ACTUATOR_MODBUS_NOT_RESPOND)
		{
			alarmList[MODBUS_ACTUATOR_SEND].physic = PHYSIC_TRUE;
			//CountErrorModbusMSG[i] = 0;

			/*in questo caso bisogna comunicarlo all'SBC che metterà a video un pop up per le possibili soluzioni*/
			break;
		}
	}

	//se ho ciclato tutto il for senza trovare allarmi e precedentemente era stato attivato un allarme
	if(i == 8 && alarmList[MODBUS_ACTUATOR_SEND].physic == PHYSIC_TRUE)
	{
		alarmList[MODBUS_ACTUATOR_SEND].physic = PHYSIC_FALSE;

		for (int j = 0; j<8; j++)
			CountErrorModbusMSG[j] = 0;
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
		//ShowAlarmStr((int)alarmList[StartAlmArrIdx].code, " alarm");
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
		//ShowAlarmStr((int)alarmList[StartAlmArrIdx].code, " terminato");
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

	case SECURITY_SAF_AIR_FILTER:
		if(myAlarmPointer->active == ACTIVE_TRUE)
			currentGuard[GUARD_ALARM_SAF_AIR_FILT].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		else
			currentGuard[GUARD_ALARM_SAF_AIR_FILT].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
		break;

	case SECURITY_SFV_AIR_DET:
		if(myAlarmPointer->active == ACTIVE_TRUE)
			currentGuard[GUARD_ALARM_SFV_AIR].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		else
			currentGuard[GUARD_ALARM_SFV_AIR].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
		break;

	case SECURITY_SFA_AIR_DET:
		if(myAlarmPointer->active == ACTIVE_TRUE)
			currentGuard[GUARD_ALARM_SFA_AIR].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		else
			currentGuard[GUARD_ALARM_SFA_AIR].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
		break;

	case SECURITY_STOP_ALL_ACT_WAIT_CMD:
		if(myAlarmPointer->active == ACTIVE_TRUE)
			currentGuard[GUARD_ALARM_STOP_ALL_ACT_WAIT_CMD].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		else
			currentGuard[GUARD_ALARM_STOP_ALL_ACT_WAIT_CMD].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
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

void Buzzer_Management(BUZZER_LEVEL level)
{
	switch (level)
	{
		case SILENT:
			BUZZER_LOW_C_ClrVal(); 		//disattiva il buzzer low
			BUZZER_MEDIUM_C_ClrVal();	//disattiva il buzzer Medium
			BUZZER_HIGH_C_ClrVal();		//disattiva il buzzer HIGH
			break;

		case LOW:
			BUZZER_LOW_C_SetVal(); 		//attiva il buzzer low
			break;

		case MEDIUM:
			BUZZER_MEDIUM_C_SetVal();	//attiva il buzzer Medium
			break;

		case HIGH:
			BUZZER_HIGH_C_SetVal();		//attiva il buzzer High
			break;

		default:
			break;
	}
}

