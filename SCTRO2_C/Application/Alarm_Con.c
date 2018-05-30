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
#include "child_gest.h"
#include "App_Ges.h"

extern bool FilterSelected;

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

		// allarme pressione filtro ossigenazione alta
		{CODE_ALARM_PRESS_OXYG_INLET, 	   PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH, 2000, 2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull}, 		/* 11 */
		{CODE_ALARM_TEMP_SENS_NOT_DETECTED,PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH, 2000, 2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull}, 		/* 12 */

		// allarmi di cover aperte
		{CODE_ALARM_PUMP_PERF_COVER_OPEN,  PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH,    0,  100, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull},	    /* 13 */
		{CODE_ALARM_PUMP_PURIF_COVER_OPEN, PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH,    0,  100, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull},	    /* 14 */
		{CODE_ALARM_PUMP_OXYG_COVER_OPEN,  PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH,    0,  100, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull},	    /* 15 */
		// allarme di livello liquido troppo basso
		{CODE_ALARM_TANK_LEVEL_LOW,        PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH,  500,  500, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull},	    /* 16 */
		// allarme livello del liquido troppo alto
		{CODE_ALARM_TANK_LEVEL_HIGH,       PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACT_WAIT_CMD, PRIORITY_HIGH,  500,  500, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull},	    /* 17 */

		// allarme differenza tra flusso venoso misurato e flusso calcolato in base al guadagno pompa troppo alto
		{CODE_ALARM_DELTA_FLOW_ART,        PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH,  500,  500, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull},	    /* 18 */
		// allarme differenza tra flusso arterioso misurato e flusso calcolato in base al guadagno pompa troppo alto
		{CODE_ALARM_DELTA_FLOW_VEN,        PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH,  500,  500, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull},	    /* 19 */
		// allarme differenza tra temperatura vaschetta e temperatura fluido arterioso troppo alta
		{CODE_ALARM_DELTA_TEMP_REC_ART,    PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH,  500,  500, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull},	    /* 20 */
		// allarme differenza tra temperatura vaschetta e temperatura fluido venoso troppo alta
		{CODE_ALARM_DELTA_TEMP_REC_VEN,    PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH,  500,  500, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull},	    /* 21 */
		// allarme comunicazione canbus
		{CODE_ALARM_CAN_BUS_ERROR,         PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH,  500,  0,   OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull},	    /* 22 */

		// allarme pompe non completamente ferme (usato nello stato di trattamento)
		{CODE_ALARM_PUMPS_NOT_STILL,       PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_PUMPS_NOT_STILL,       PRIORITY_HIGH,  500,  500, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull},	    /* 23 */
		// allarme pinch non posizionate correttamente (usato nello stato di trattamento)
		{CODE_ALARM_BAD_PINCH_POS,         PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_BAD_PINCH_POS,         PRIORITY_HIGH,  500,  500, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull},	    /* 24 */
		// allarme aria nel filtro (usato nello stato di trattamento)
		{CODE_ALARM_SFA_PRIM_AIR_DET,      PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_SFA_PRIM_AIR_DET,      PRIORITY_HIGH,  1000, 0,   OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull},	    /* 25 */

		{CODE_ALARM_PRESS_ADS_FILTER_LOW,  PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH, 2000, 2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull},	    /* 26 */
		{CODE_ALARM_PRESS_OXYG_LOW, 	   PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH, 2000, 2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull}, 		/* 27 */

		//Allarme per errore nella lettura e scrittura modbus. Se dopo 10 ripetizioni non ottengo risposta alla lettura o scrittura genero un allarme.
		// Per questo allarme uso la stessa procedura per le pompe non ferme. (Dovrei tolgliere direttamente l'enable alle pompe.
		//{CODE_ALARM_MODBUS_ACTUATOR_SEND,  PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_WAIT_CONFIRM,        PRIORITY_LOW,     0, 100, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull},	        /* 28 */
		{CODE_ALARM_MODBUS_ACTUATOR_SEND,  PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_MOD_BUS_ERROR,         PRIORITY_HIGH,    0, 500, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull},	        /* 28 */

		// allarmi provenienti dalla protective. Serve per fare in modo che quando la protective e' in allarme le pompe vengano fermate e le pinch
		// messe in sicurezza
		{CODE_ALARM_PROT_START_VAL,        PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_PROTECTION, SECURITY_STOP_ALL_ACTUATOR,  PRIORITY_HIGH,    0, 500, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull},	        /* 29 */
		{}
};


void EnablePumpNotStillAlmFunc(void)
{
	GlobalFlags.FlagsDef.EnablePumpNotStillAlm = 1;
}
void EnableBadPinchPosAlmFunc(void)
{
	GlobalFlags.FlagsDef.EnableBadPinchPosAlm = 1;
}
void EnablePrimAlmSFAAirDetAlmFunc(void)
{
	GlobalFlags.FlagsDef.EnablePrimAlmSFAAirDetAlm = 1;
}

void DisablePumpNotStillAlmFunc(void)
{
	GlobalFlags.FlagsDef.EnablePumpNotStillAlm = 0;
}
void DisableBadPinchPosAlmFunc(void)
{
	GlobalFlags.FlagsDef.EnableBadPinchPosAlm = 0;
}
void DisablePrimAlmSFAAirDetAlmFunc(void)
{
	GlobalFlags.FlagsDef.EnablePrimAlmSFAAirDetAlm = 0;
}

void DisablePrimAirAlarm(bool dis)
{
	if(dis)
	{
		GlobalFlags.FlagsDef.EnablePrimAlmSFAAirDetAlm = 0;
	}
	else
	{
		GlobalFlags.FlagsDef.EnablePrimAlmSFAAirDetAlm = 1;
	}
}

// se dis == TRUE disabilito tutti gli allarmi aria
void DisableAllAirAlarm(bool dis)
{
	if(dis)
	{
		GlobalFlags.FlagsDef.EnableSAFAir = 0;
		GlobalFlags.FlagsDef.EnableSFVAir = 0;
		GlobalFlags.FlagsDef.EnableSFAAir = 0;
	}
	else
	{
		GlobalFlags.FlagsDef.EnableSAFAir = 1;
		GlobalFlags.FlagsDef.EnableSFVAir = 1;
		GlobalFlags.FlagsDef.EnableSFAAir = 1;
	}
}

// Disabilita singolarmente tutti gli allarmi
void DisableAllAlarm()
{
	GlobalFlags.FlagsVal = 0;
	//GlobalFlags.FlagsDef.EnableAllAlarms = 1;
}



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
	GlobalFlags.FlagsDef.EnableSAFAir = 1;
	GlobalFlags.FlagsDef.EnableSFVAir = 1;
	GlobalFlags.FlagsDef.EnableSFAAir = 1;
#ifdef ENABLE_PROTECTIVE_BOARD
	GlobalFlags.FlagsDef.EnableCANBUSErr = 1;
#else
	GlobalFlags.FlagsDef.EnableCANBUSErr = 0;
#endif
	GlobalFlags.FlagsDef.EnablePumpNotStillAlm = 0;       // viene attivato in un'altro momento
	GlobalFlags.FlagsDef.EnableBadPinchPosAlm = 0;        // viene attivato in un'altro momento
	GlobalFlags.FlagsDef.EnablePrimAlmSFAAirDetAlm = 0;   // viene attivato in un'altro momento

	GlobalFlags.FlagsDef.EnableModbusNotRespAlm = 1;      // abilito l'allarme dovuto ad un cattivo funzionamento del modbus
	GlobalFlags.FlagsDef.EnableFromProtectiveAlm = 0;
}

// Questa funzione serve per forzare ad off un eventuale allarme.
// Per il momento la uso solo per il caso di CODE_ALARM_TANK_LEVEL_HIGH
void ForceAlarmOff(unsigned char code)
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
		case CODE_ALARM_PRESS_ADS_FILTER_LOW:
		case CODE_ALARM_PRESS_OXYG_LOW:
			GlobalFlags.FlagsDef.EnablePressSensLowAlm = 0;  // forzo allarme pressione bassa off
			break;
		case CODE_ALARM_PRESS_ART_HIGH:
		case CODE_ALARM_PRESS_VEN_HIGH:
		case CODE_ALARM_PRESS_ADS_FILTER_HIGH:
		case CODE_ALARM_PRESS_OXYG_INLET:
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
		case CODE_ALARM_AIR_PRES_ADSRB_FILTER:
			GlobalFlags.FlagsDef.EnableSAFAir = 0;     // forzo allarme aria sul filtro off
			break;
		case CODE_ALARM_AIR_PRES_VEN:
			GlobalFlags.FlagsDef.EnableSFVAir = 0;     // forzo allarme aria sul circuito venoso off
			break;
		case CODE_ALARM_AIR_PRES_ART:
			GlobalFlags.FlagsDef.EnableSFAAir = 0;     // forzo allarme aria sul circuito arterioso off
			break;
		case CODE_ALARM_CAN_BUS_ERROR:
			GlobalFlags.FlagsDef.EnableCANBUSErr = 0;
			break;
		case CODE_ALARM_PUMPS_NOT_STILL:
			GlobalFlags.FlagsDef.EnablePumpNotStillAlm = 0;
			break;
		case CODE_ALARM_BAD_PINCH_POS:
			GlobalFlags.FlagsDef.EnableBadPinchPosAlm = 0;
			break;
		case CODE_ALARM_SFA_PRIM_AIR_DET:
			GlobalFlags.FlagsDef.EnablePrimAlmSFAAirDetAlm = 0; // disabilito allarme aria su filtro durante il priming
			break;
		case CODE_ALARM_MODBUS_ACTUATOR_SEND:
			GlobalFlags.FlagsDef.EnableModbusNotRespAlm = 0;    // disabilito allarme modbus
			break;
		case CODE_ALARM_PROT_START_VAL:
			GlobalFlags.FlagsDef.EnableFromProtectiveAlm = 0;   // forzo allarme ricevuto dal protective off
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
		case CODE_ALARM_PRESS_OXYG_INLET:
			strcpy(s, "ALARM_PRESS_OXYG");
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
		case CODE_ALARM_CAN_BUS_ERROR:
			strcpy(s, "ALARM_CAN_BUS_ERR");
			strcat(s, str);
			DebugStringStr(s);
			break;
		case CODE_ALARM_PUMPS_NOT_STILL:
			strcpy(s, "AL_PUMPS_NOT_STILL");
			strcat(s, str);
			DebugStringStr(s);
			break;
		case CODE_ALARM_BAD_PINCH_POS:
			strcpy(s, "AL_BAD_PINCH_POS");
			strcat(s, str);
			DebugStringStr(s);
			break;
		case CODE_ALARM_SFA_PRIM_AIR_DET:
			strcpy(s, "AL_SFA_PRIM_AIR_DET");
			strcat(s, str);
			DebugStringStr(s);
			break;
		case CODE_ALARM_PRESS_ADS_FILTER_LOW:
			strcpy(s, "AL_PRESS_ADS_FILT_LOW");
			strcat(s, str);
			DebugStringStr(s);
			break;
		case CODE_ALARM_PRESS_OXYG_LOW:
			strcpy(s, "AL_PRESS_OXYG_LOW");
			strcat(s, str);
			DebugStringStr(s);
			break;
		case CODE_ALARM_PROT_START_VAL:
			strcpy(s, "AL_FROM_PROTECTIVE");
			strcat(s, str);
			DebugStringStr(s);
			break;

	}
}

static int StrAlarmWritten = 0;
static int IdxCurrAlarm = 0xff;
int StartAlmArrIdx = 0;
int i_al;

void alarmConInit(void){
	ptrAlarmCurrent = &alarmList[0];
	StrAlarmWritten = 0;
	IdxCurrAlarm = 0xff;
	StartAlmArrIdx = 0;
}


void EnableNextAlarmFunc(void)
{
	EnableNextAlarm = TRUE;

	//if((ptrAlarmCurrent->active == ACTIVE_TRUE) && (ptrAlarmCurrent->code == alarmCurrent.code) && (ptrAlarmCurrent->physic == PHYSIC_TRUE))
	if(ptrAlarmCurrent->code == alarmCurrent.code)
	{
		// FM l'allarme e' stato disattivato perche' non sono piu'
		// verificate le condizioni fisiche che lo hanno generato
		memset(&alarmCurrent, 0, sizeof(struct alarm));
	}
}


void alarmEngineAlways(void)
{
//	static int CntTickDelay = 0;

//	CntTickDelay++;
//	if((StartAlmArrIdx == 0) && (CntTickDelay <= 3))
//			return;
//	else if(StartAlmArrIdx == 0)
//		CntTickDelay = 3;
	if((StrAlarmWritten == 0) && !EnableNextAlarm)
	{
		// non ho ancora premuto il tasto button reset per resettare l'allarme corrente quindi non posso
		// andare avanti.
		// Se andassi avanti comunque avrei dei problemi nella gestione di due allarmi diversi e contemporanei
		// come nel caso di livello alto e cover.
		return;
	}
//	else if((StrAlarmWritten == 2) && EnableNextAlarm)
//	{
//		// ho ricevuto un reset allarme da utente ma la condizione di allarme e' ancora attiva
//		// devo reinizializzare le variabili che seguono altrimenti l'allarme non viene riproposto
//		StrAlarmWritten = 0;
//		StartAlmArrIdx = 0;
//	}



/*Faccio uno switch su tutta la macchina a stati in modo
 * gestire ogni allarme in funzioine dello stato in cui sono*/

	//if(GlobalFlags.FlagsDef.EnableAllAlarms)
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

				manageAlarmCanBus();
				manageAlarmActuatorModbusNotRespond();
				manageAlarmActuatorWRModbusNotRespond();
				manageAlarmFromProtective();
				manageAlarmBadPinchPos();   // allarme di pinch posizionate correttamente
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
				manageAlarmCanBus();
				manageAlarmPrimSFAAirDet();
				manageAlarmActuatorModbusNotRespond();
				manageAlarmActuatorWRModbusNotRespond();
				manageAlarmFromProtective();
				manageAlarmBadPinchPos();   // allarme di pinch posizionate correttamente
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

				// Questo allarme lo commento per ora, perche' bisogna avere un sensore di livello
				// che funziona bene e si e' sicuri del suo funzionamento
				//manageAlarmLiquidLevelLow();
				// i due allarmi che seguono devo essere gestiti attentamente perche' potrei avere delle
				// segnalazioni di allarme anche durante la fase di accelerazione e decelerazione del pid
				// Per ora li commento.
				//manageAlarmDeltaFlowArt();
				//manageAlarmDeltaFlowVen();
				manageAlarmDeltaTempRecArt();
				manageAlarmDeltaTempRecVen();
				manageAlarmCanBus();
				manageAlarmBadPinchPos(); // controllo il posizionamento delle pinch prima di iniziare un trattamento
				manageAlarmActuatorModbusNotRespond();
				manageAlarmActuatorWRModbusNotRespond();
				manageAlarmFromProtective();
				break;
			}

			case STATE_PRIMING_WAIT:
				// in questo stato non sono gestiti gli allarmi, per ora
				//verifica physic pressioni
				//manageAlarmPhysicPressSensHigh();
				//manageAlarmPhysicPressSensLow();

				//verifica physic ir temp sens
				//manageAlarmPhysicTempSens();
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
				manageAlarmCanBus();
				manageAlarmPumpNotStill();  // controllo allarme di pompe ferme alla fine del ricircolo
				manageAlarmBadPinchPos();   // allarme di pinch posizionate correttamente
				manageAlarmPrimSFAAirDet();
				manageAlarmActuatorModbusNotRespond();
				manageAlarmActuatorWRModbusNotRespond();
				manageAlarmFromProtective();
				break;

			case STATE_WAIT_TREATMENT:
				//verifica physic pressioni
				//manageAlarmPhysicPressSensHigh();
				//manageAlarmPhysicPressSensLow();

				//verifica physic ir temp sens
				//manageAlarmPhysicTempSens();
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
				break;
			}

			case STATE_EMPTY_DISPOSABLE:
			case STATE_EMPTY_DISPOSABLE_1:
			{
				manageAlarmFlowSensNotDetected();
				manageAlarmIrTempSensNotDetected();

				manageAlarmPhysicPressSensHigh();
				manageAlarmPhysicUFlowSens();
				manageAlarmSAFAirSens();
				manageAlarmPhysicUFlowSensVen();
				if(GetTherapyType() == LiverTreat)
					manageAlarmCoversPumpLiver();
				else if(GetTherapyType() == KidneyTreat)
					manageAlarmCoversPumpKidney();
				manageAlarmActuatorModbusNotRespond();
				manageAlarmActuatorWRModbusNotRespond();
				manageAlarmFromProtective();
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
			//CntTickDelay = 0;
			EnableNextAlarm = TRUE;
			memset(&alarmCurrent, 0, sizeof(struct alarm));
		}
	}
	else if(StrAlarmWritten == 1)
	{
		ShowAlarmStr((int)alarmList[IdxCurrAlarm].code, " on");
		StrAlarmWritten = 2;
		EnableNextAlarm = FALSE;
	}
	else if(StrAlarmWritten == 2)
	{
		if( alarmList[IdxCurrAlarm].active == ACTIVE_FALSE)
		{
			// allarme terminato
			StrAlarmWritten = 0;
			ShowAlarmStr((int)alarmList[IdxCurrAlarm].code, " off");
			StartAlmArrIdx = 0;
			//CntTickDelay = 0;
		}
	}
}

//------------------------FUNZIONI PER DETERMINARE LA CONDIZIONE DI ALLARME----------------------------------

// Allarme generato dalla condizione di aria nel filtro durante il priming. Viene preso in considerazione
// dal momento in cui il filtro viene installato alla fine del priming
void manageAlarmPrimSFAAirDet(void)
{
	if(!GlobalFlags.FlagsDef.EnablePrimAlmSFAAirDetAlm)
		alarmList[PRIM_AIR_ON_FILTER].physic = PHYSIC_FALSE;
	else if(!FilterSelected)
		alarmList[PRIM_AIR_ON_FILTER].physic = PHYSIC_FALSE;
	else
	{
		if(Air_1_Status == AIR)
		{
			alarmList[PRIM_AIR_ON_FILTER].physic = PHYSIC_TRUE;
		}
		else
		{
			alarmList[PRIM_AIR_ON_FILTER].physic = PHYSIC_FALSE;
		}
	}
}


bool IsTreatSetPinchPosTaskAlm(void);

// Allarme generato dalla condizione di pinch posizionate male. Viene preso in considerazione alla fine
// del ricircolo prima di attaccare l'organo.
void manageAlarmBadPinchPos(void)
{
	if(!GlobalFlags.FlagsDef.EnableBadPinchPosAlm)
		alarmList[BAD_PINCH_POS].physic = PHYSIC_FALSE;
	else
	{
		// allarme generato se:
		// -  sono a fine riciclo e le pinch arteriosa e venosa, dopo lo stop delle pompe,
		//    non sono completamente chiuse
		// -  allo start del trattamento il controllo delle pinch arteriosa e venosa non sono posizionate
		//    correttamente per iniziare il trattamento
		// -  durante l'esecuzione del trattamento nel caso, per un qualsiasi motivo, cambiasse la
		//    configurazione delle pinch
		if(AreAllPinchClose() || IsTreatSetPinchPosTaskAlm() || !IsTreatCurrPinchPosOk())
		{
			alarmList[BAD_PINCH_POS].physic = PHYSIC_TRUE;
		}
		else
		{
			alarmList[BAD_PINCH_POS].physic = PHYSIC_FALSE;
		}
	}
}


bool IsPumpStopAlarmActive(void);

// Allarme generato dalla condizione di pompe non ferme. Viene preso in considerazione alla fine
// del ricircolo prima di attaccare l'organo.
void manageAlarmPumpNotStill(void)
{
	if(!GlobalFlags.FlagsDef.EnablePumpNotStillAlm)
		alarmList[PUMP_NOT_STILL].physic = PHYSIC_FALSE;
	else
	{
		if(IsPumpStopAlarmActive() )
		{
			alarmList[PUMP_NOT_STILL].physic = PHYSIC_TRUE;
		}
		else
		{
			alarmList[PUMP_NOT_STILL].physic = PHYSIC_FALSE;
		}
	}
}


bool IsCanBusError(void);

void manageAlarmCanBus(void)
{
	if(!GlobalFlags.FlagsDef.EnableCANBUSErr)
		alarmList[CAN_BUS_ERROR].physic = PHYSIC_FALSE;
	else
	{
		if(IsCanBusError() )
		{
			alarmList[CAN_BUS_ERROR].physic = PHYSIC_TRUE;
		}
		else
		{
			alarmList[CAN_BUS_ERROR].physic = PHYSIC_FALSE;
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

// controllo se il delta di temperatura tra recipiente e liquido arterioso e' troppo alta
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

// controllo se il delta di temperatura tra recipiente e liquido venoso e' troppo alta
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
	static unsigned char alarmList_physic_OLD = PHYSIC_FALSE;

	if(GlobalFlags.FlagsDef.EnableCoversAlarm)
	{
		if(CoversState == 4)
		{
			alarmList[PURIF_COVER_OPEN].physic = PHYSIC_FALSE;
			alarmList[PERF_COVER_OPEN].physic = PHYSIC_FALSE;
			alarmList[OXYG_COVER_OPEN].physic = PHYSIC_FALSE;
		}
		else
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

		if((alarmList_physic_OLD == PHYSIC_TRUE) && (alarmList[PERF_COVER_OPEN].physic == PHYSIC_FALSE))
			alarmList_physic_OLD = alarmList[PERF_COVER_OPEN].physic;
		alarmList_physic_OLD = alarmList[PERF_COVER_OPEN].physic;
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
		if(CoversState == 4)
		{
			alarmList[PERF_COVER_OPEN].physic = PHYSIC_FALSE;
			alarmList[OXYG_COVER_OPEN].physic = PHYSIC_FALSE;
		}
		else
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
		if(PR_ADS_FLT_mmHg_Filtered < PR_ADS_FILTER_LOW)
		{
			alarmList[PRESS_ADS_FILTER_LOW].physic = PHYSIC_TRUE;
		}
		else
		{
			alarmList[PRESS_ADS_FILTER_LOW].physic = PHYSIC_FALSE;
		}

		if(PR_OXYG_mmHg_Filtered < PR_OXYG_LOW)
		{
			alarmList[PRESS_OXYG_LOW].physic = PHYSIC_TRUE;
		}
		else
		{
			alarmList[PRESS_OXYG_LOW].physic = PHYSIC_FALSE;
		}
/*
		// se il sensore di pressione arteriosa misura la pressione subito prima dell'organo
		// questo codice va commentato perche' la pressione prima di andare in trattamento e' sempre 0
		// Per ora faccio una prova mettendo PR_ART_LOW a 0
		if(PR_ART_mmHg_Filtered < PR_ART_LOW)
		{
			alarmList[PRESS_ART_LOW].physic = PHYSIC_TRUE;
		}
		else
		{
			alarmList[PRESS_ART_LOW].physic = PHYSIC_FALSE;
		}

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
		alarmList[PR_ADS_FILTER_LOW].physic = PHYSIC_FALSE;
		alarmList[PR_OXYG_LOW].physic = PHYSIC_FALSE;
	}
}

void manageAlarmPhysicPressSensHigh(void)
{
	word MaxPressArt;

	MaxPressArt = PR_ART_HIGH;
	if((ptrCurrentState->state == STATE_PRIMING_PH_1) || (ptrCurrentState->state == STATE_PRIMING_PH_2) ||
	   (ptrCurrentState->state == STATE_PRIMING_RICIRCOLO))
	{
		// nel caso di priming considero una pressione arteriosa piu' alta
		MaxPressArt = PR_ART_HIGH + 50;
	}

	if(GlobalFlags.FlagsDef.EnablePressSensHighAlm)
	{
		if(PR_ART_Sistolyc_mmHg > MaxPressArt)
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

		/*il sensore Venoso è usato solo nel trattamento Liver, il Kidney non ha la linea Venosa*/
		if((PR_VEN_Sistolyc_mmHg /*PR_VEN_mmHg_Filtered*/ > PR_VEN_HIGH) && (GetTherapyType() == LiverTreat))
		{
			alarmList[PRESS_VEN_HIGH].physic = PHYSIC_TRUE;
		}
		else
		{
			alarmList[PRESS_VEN_HIGH].physic = PHYSIC_FALSE;
		}

		if(PR_OXYG_mmHg_Filtered > PR_OXYG_HIGH)
		{
			alarmList[PRESS_OXYG_HIGH].physic = PHYSIC_TRUE;
		}
		else
		{
			alarmList[PRESS_OXYG_HIGH].physic = PHYSIC_FALSE;
		}
	}
	else
	{
		alarmList[PRESS_ART_HIGH].physic = PHYSIC_FALSE;
		alarmList[PRESS_ADS_FILTER_HIGH].physic = PHYSIC_FALSE;
		alarmList[PRESS_VEN_HIGH].physic = PHYSIC_FALSE;
		alarmList[PRESS_OXYG_HIGH].physic = PHYSIC_FALSE;
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
	if(!GlobalFlags.FlagsDef.EnableSAFAir)
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
	if(!GlobalFlags.FlagsDef.EnableSFVAir)
	{
		alarmList[AIR_PRES_VEN].physic = PHYSIC_FALSE;
		return;
	}
	else
	{
//		if((sensor_UFLOW[VENOUS_AIR_SENSOR].bubbleSize >= 25) ||
//			(sensor_UFLOW[VENOUS_AIR_SENSOR].bubblePresence == MASK_ERROR_BUBBLE_ALARM))
		if(((sensor_UFLOW[VENOUS_AIR_SENSOR].bubbleSize >= 25) ||
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
	if(!GlobalFlags.FlagsDef.EnableSFAAir)
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
			((sensor_UFLOW[ARTERIOUS_AIR_SENSOR].bubbleSize >= 25) ||
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
//	if(AlarmCode == CODE_ALARM_MODBUS_ACTUATOR_SEND)
//		NonPhysAlm = 1;
	return NonPhysAlm;
}

// Deve essere chiamata per togliere gli allarmi attivati tramite la SetNonPhysicalAlm
// Potrebbe essere chiamata automaticamente dopo un certo tempo che l'allarme e' stato inviato. se e' un
// allarme che non implica alcuna azione sul sistema (tipo arresto pompe)
void ClearNonPhysicalAlm( int AlarmCode)
{
	EnableNextAlarm = TRUE;
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


// elimina la condizione che ha generato l'allarme
void ClearModBusAlarm(void)
{
	for (int j = 0; j<8; j++)
		CountErrorModbusMSG[j] = 0;
	for (int j = 0; j<LAST_ACTUATOR; j++)
		ActuatorWriteCnt[j] = 0;
}

void manageAlarmActuatorModbusNotRespond(void)
{
	int i;
	if(!GlobalFlags.FlagsDef.EnableModbusNotRespAlm)
	{
		alarmList[MODBUS_ACTUATOR_SEND].physic = PHYSIC_FALSE;
		memset(CountErrorModbusMSG, 0, sizeof(CountErrorModbusMSG));
	}
	else
	{
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
}

// errore su modbus durante le operazioni di scrittura
void manageAlarmActuatorWRModbusNotRespond(void)
{
	int i;
	if(!GlobalFlags.FlagsDef.EnableModbusNotRespAlm)
	{
		alarmList[MODBUS_ACTUATOR_SEND].physic = PHYSIC_FALSE;
		memset(ActuatorWriteCnt, 0, sizeof(ActuatorWriteCnt));
	}
	else
	{
		for (i = 0; i <LAST_ACTUATOR; i++)
		{
			/*se non ricevo 10 msg consecutivi da un sensore di temperatura ossia il sensore non risposnde per 6 secondi consecutivi vado in allarme*/
			if (ActuatorWriteCnt[i] > MAX_MSG_CONSECUTIVE_ACTUATOR_MODBUS_NOT_RESPOND)
			{
				alarmList[MODBUS_ACTUATOR_SEND].physic = PHYSIC_TRUE;

				/*in questo caso bisogna comunicarlo all'SBC che metterà a video un pop up per le possibili soluzioni*/
				break;
			}
		}

		//se ho ciclato tutto il for senza trovare allarmi e precedentemente era stato attivato un allarme
		if(i == LAST_ACTUATOR && alarmList[MODBUS_ACTUATOR_SEND].physic == PHYSIC_TRUE)
		{
			alarmList[MODBUS_ACTUATOR_SEND].physic = PHYSIC_FALSE;

			for (int j = 0; j<LAST_ACTUATOR; j++)
				ActuatorWriteCnt[j] = 0;
		}
	}
}



void manageAlarmFromProtective(void)
{
	int i;
	if(!GlobalFlags.FlagsDef.EnableFromProtectiveAlm)
	{
		alarmList[ALARM_FROM_PROTECTIVE].physic = PHYSIC_FALSE;
	}
	else
	{
		uint16_t u16 = ReadProtectiveAlarmCode();
		if(u16 && (u16 >= CODE_ALARM_PROT_START_VAL))
		{
			alarmList[ALARM_FROM_PROTECTIVE].physic = PHYSIC_TRUE;
		}
		else
		{
			alarmList[ALARM_FROM_PROTECTIVE].physic = PHYSIC_FALSE;
		}
	}
}

//------------------------FINE FUNZIONI PER DETERMINARE LA CONDIZIONE DI ALLARME----------------------------------



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
	//else if((ptrAlarmCurrent->active == ACTIVE_TRUE) && (elapsedExitTime > ((EnableNextAlarm) ? 0 : ptrAlarmCurrent->exitTime)))
	{
		// esco dalla gestione di un allarme che ha bisogno di azioni sugli attuatori
		elapsedEntryTime = 0;
		elapsedExitTime = 0;
		ptrAlarmCurrent->active = ACTIVE_FALSE;
		currentGuard[GUARD_ALARM_ACTIVE].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;

		// FM l'allarme e' stato disattivato perche' non sono piu'
		// verificate le condizioni fisiche che lo hanno generato
		//memset(&alarmCurrent, 0, sizeof(struct alarm));

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

	case SECURITY_PUMPS_NOT_STILL:
		if(myAlarmPointer->active == ACTIVE_TRUE)
			currentGuard[GUARD_ALARM_PUMPS_NOT_STILL].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		else
			currentGuard[GUARD_ALARM_PUMPS_NOT_STILL].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
		break;
	case SECURITY_BAD_PINCH_POS:
		if(myAlarmPointer->active == ACTIVE_TRUE)
			currentGuard[GUARD_ALARM_BAD_PINCH_POS].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		else
			currentGuard[GUARD_ALARM_BAD_PINCH_POS].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
		break;
	case SECURITY_SFA_PRIM_AIR_DET:
		if(myAlarmPointer->active == ACTIVE_TRUE)
			currentGuard[GUARD_ALARM_SFA_PRIM_AIR_DET].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		else
			currentGuard[GUARD_ALARM_SFA_PRIM_AIR_DET].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
		break;
	case SECURITY_MOD_BUS_ERROR:
		if(myAlarmPointer->active == ACTIVE_TRUE)
			currentGuard[GUARD_ALARM_MOD_BUS_ERROR].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		else
			currentGuard[GUARD_ALARM_MOD_BUS_ERROR].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
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


// ritorna il numero di intervalli di 10 msec trascorsi dal valore last
unsigned long msTick10_elapsed( unsigned long last )
{
	int elapsed = FreeRunCnt10msec;
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

// sono definiti in child_gest.c
extern CHILD_EMPTY_FLAGS ChildEmptyFlags;

// rileva la presenza di aria nei sensori senza usare gli allarmi
bool IsDisposableEmptyNoAlarm(void)
{
	bool DispEmpty = FALSE;

	if((Air_1_Status == AIR) && (ChildEmptyFlags.FlagsDef.SAFAirDetected == 0))
	{
		ChildEmptyFlags.FlagsDef.SAFAirDetected = 1;
		DebugStringStr("AL_AIR_PRES_ADSRB_FILT");
	}

	if(((sensor_UFLOW[VENOUS_AIR_SENSOR].bubbleSize >= 50) ||
		(sensor_UFLOW[VENOUS_AIR_SENSOR].bubblePresence == MASK_ERROR_BUBBLE_ALARM)) &&
		(sensor_UFLOW[VENOUS_AIR_SENSOR].bubbleSize != 255) &&
		(ChildEmptyFlags.FlagsDef.SFVAirDetected == 0))
	{
		ChildEmptyFlags.FlagsDef.SFVAirDetected = 1;
		DebugStringStr("AL_AIR_PRES_VEN");
	}

	if(((sensor_UFLOW[ARTERIOUS_AIR_SENSOR].bubbleSize >= 50) ||
		(sensor_UFLOW[ARTERIOUS_AIR_SENSOR].bubblePresence == MASK_ERROR_BUBBLE_ALARM)) &&
		(sensor_UFLOW[ARTERIOUS_AIR_SENSOR].bubbleSize != 255) &&
		(ChildEmptyFlags.FlagsDef.SFAAirDetected == 0)
		)
	{
		ChildEmptyFlags.FlagsDef.SFAAirDetected = 1;
		DebugStringStr("AL_AIR_PRES_ART");
	}

	if(GetTherapyType() == LiverTreat)
	{
		if(ChildEmptyFlags.FlagsDef.SFAAirDetected && ChildEmptyFlags.FlagsDef.SFVAirDetected && ChildEmptyFlags.FlagsDef.SAFAirDetected)
			DispEmpty = TRUE;
	}
	else if(GetTherapyType() == KidneyTreat)
	{
		if(/*ChildEmptyFlags.FlagsDef.SFAAirDetected &&*/ ChildEmptyFlags.FlagsDef.SFVAirDetected && ChildEmptyFlags.FlagsDef.SAFAirDetected)
			DispEmpty = TRUE;
	}
	return DispEmpty;
}

// ritorna true se c'e' un allarme attivo
bool IsAlarmActive(void)
{
	if(alarmCurrent.code && (alarmCurrent.code == ptrAlarmCurrent->code) && (ptrAlarmCurrent->active == ACTIVE_TRUE))
		return TRUE;
	else
		return FALSE;
}


// usata nel task HandleProtectiveAlarm per la gestione dell'allarme generato dalla protective
bool IsControlInAlarm(void)
{
	bool InAlarm = FALSE;
	if(alarmCurrent.code && alarmCurrent.type ==ALARM_TYPE_CONTROL)
		InAlarm = TRUE;
	return InAlarm;
}


bool IsProtectiveInAlarm(void)
{
	bool InAlarm = FALSE;
	if(alarmCurrent.code && (alarmCurrent.type == ALARM_TYPE_PROTECTION) && (alarmCurrent.code >= CODE_ALARM_PROT_START_VAL))
		InAlarm = TRUE;
	return InAlarm;
}


// disabilito tutti gli allarmi
void StopAllCntrlAlarm(GLOBAL_FLAGS *pgbf)
{
	pgbf->FlagsVal = GlobalFlags.FlagsVal;
	GlobalFlags.FlagsVal = 0;
}

// disabilito tutti gli allarmi
void RestoreAllCntrlAlarm(GLOBAL_FLAGS *pgbf)
{
	GlobalFlags.FlagsVal = pgbf->FlagsVal;
}

void ClearAlarmState(void)
{
	StrAlarmWritten = 0;
	StartAlmArrIdx = 0;
	memset(&alarmCurrent, 0, sizeof(struct alarm));
	LevelBuzzer = 0;
	//EnableNextAlarm = 1;
	EnableNextAlarmFunc();
	currentGuard[GUARD_ALARM_ACTIVE].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
	currentGuard[GUARD_ALARM_ACTIVE].guardValue = GUARD_VALUE_FALSE;
}
