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
extern bool AtLeastoneButResRcvd;

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

		// Filippo - inserito allarme per il tasto di stop
		{CODE_ALARM_EMERGENCY_BUTTON, PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH, 0, 500, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_ALLOWED, &alarmManageNull},	        /* 30 allarme tasto di stop*/

		// Filippo - inserito allarme per test T1
		{CODE_ALARM_TEST_T1, PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH, 0, 500, OVRD_NOT_ENABLED, RESET_NOT_ALLOWED, SILENCE_ALLOWED, MEMO_ALLOWED, &alarmManageNull},	        /* 31 allarme tasto di stop*/
		// Filippo - inserito allarme per test sensore aria fallito
		{CODE_ALARM_AIR_SENSOR_TEST_KO,PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,PRIORITY_HIGH, 0, 2000, OVRD_NOT_ENABLED, RESET_NOT_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull}, 		/* 32 */
        {CODE_ALARM_MACHINE_COVERS,       PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH,    0,  100, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull},         /* 33 */
        {CODE_ALARM_HOOKS_RESERVOIR,      PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH,    0,  100, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull},         /* 34 */
		// da qui in avanti solo le warning
		{CODE_ALARM_PRESS_ADS_FILTER_WARN, PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_LOW, 2000, 2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull},	        /* 30 esempio di warning*/
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
	// Filippo - da riabilitare
//	GlobalFlags.FlagsDef.EnablePressSensLowAlm = 0;       // abilito allarme pressione bassa
	GlobalFlags.FlagsDef.EnablePressSensHighAlm = 1;      // abilito allarme pressione alta
	GlobalFlags.FlagsDef.EnableTempArtHighAlm = 1;        // abilito allarme temperatura alta
	GlobalFlags.FlagsDef.EnableDeltaFlowArtAlarm = 1;     // abilito allarme delta flusso arterioso troppo alto
	GlobalFlags.FlagsDef.EnableDeltaFlowVenAlarm = 1;     // abilito allarme delta flusso venoso troppo alto
	GlobalFlags.FlagsDef.EnableDeltaTempRecVenAlarm = 1;  // abilito allarme delta temperatura recipiente e line venosa troppo alta
	GlobalFlags.FlagsDef.EnableDeltaTempRecArtAlarm = 1;  // abilito allarme delta temperatura recipiente e line arteriosa troppo alta
	GlobalFlags.FlagsDef.EnableMachineCovers = 1;         // abilito allarmi Hall Machine Covers
	GlobalFlags.FlagsDef.EnableHooksReservoir = 1;        // abilito allarmi Hall ganci contenitore organo
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
	// Filippo - abilito l'allarme di stop button
	GlobalFlags.FlagsDef.EnableStopButton=1;
	// Filippo - abilito l'allarme di T1 test
	GlobalFlags.FlagsDef.EnableT1TestAlarm=1;
	GlobalFlags.FlagsDef.EnableHooksReservoir = 1;

}

// Questa funzione serve per forzare ad off un eventuale allarme.
// Per il momento la uso solo per il caso di CODE_ALARM_TANK_LEVEL_HIGH
void ForceAlarmOff(uint16_t code)
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
        case CODE_ALARM_MACHINE_COVERS:
            GlobalFlags.FlagsDef.EnableMachineCovers = 0;   // forzo allarme machine covers a OFF
            break;
        case CODE_ALARM_HOOKS_RESERVOIR:
            GlobalFlags.FlagsDef.EnableHooksReservoir = 0;   // forzo allarme hooks a OFF
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

void alarmConInit_(void){
	ptrAlarmCurrent = &alarmList[0];
	StrAlarmWritten = 0;
	IdxCurrAlarm = 0xff;
	StartAlmArrIdx = 0;
}

void CalcAlarmActive(void)
{
	/*Faccio uno switch su tutta la macchina a stati in modo
	 * gestire ogni allarme in funzioine dello stato in cui sono*/

	switch(ptrCurrentState->state)
	{
		case STATE_NULL:
		{
			/* DA DEBUGGARE*/
			//manageAlarmFlowSensNotDetected();
			//manageAlarmIrTempSensNotDetected();

			break;
		}

		case STATE_ENTRY:
		{
			/* DA DEBUGGARE*/
			//manageAlarmFlowSensNotDetected();
			//manageAlarmIrTempSensNotDetected();

			break;
		}

		case STATE_IDLE:
		{
			/* DA DEBUGGARE*/
			//manageAlarmFlowSensNotDetected();
			//manageAlarmIrTempSensNotDetected();
				// Filippo - aggiungo la gestione del tasto di stop come allarme
				manageAlarmStopButtonPressed();

			break;
		}

		case STATE_SELECT_TREAT:
		{
			/* DA DEBUGGARE*/
			//manageAlarmFlowSensNotDetected();
			//manageAlarmIrTempSensNotDetected();

			break;
		}

		case STATE_T1_NO_DISPOSABLE:
		{
			/* DA DEBUGGARE*/
			//manageAlarmFlowSensNotDetected();
			//manageAlarmIrTempSensNotDetected();

			break;
		}

		case STATE_MOUNTING_DISP:
		{
			/* DA DEBUGGARE*/
			//manageAlarmFlowSensNotDetected();
			//manageAlarmIrTempSensNotDetected();

			break;
		}

		case STATE_TANK_FILL:
		{
			/* DA DEBUGGARE*/
			//manageAlarmFlowSensNotDetected();
			//manageAlarmIrTempSensNotDetected();

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
				// Filippo - aggiungo la gestione del tasto di stop come allarme
				manageAlarmStopButtonPressed();

				// Filippo - aggiunto allarme per test sensore aria sbagliato
				manageAlarmAirSensorTestKO();
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
				// Filippo - aggiungo la gestione del tasto di stop come allarme
				manageAlarmStopButtonPressed();
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
				// Filippo - aggiungo la gestione del tasto di stop come allarme
				manageAlarmStopButtonPressed();
			break;
		}

		case STATE_EMPTY_DISPOSABLE_2:
		{
			/* DA DEBUGGARE*/
			//manageAlarmFlowSensNotDetected();
			//manageAlarmIrTempSensNotDetected();
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
			//manageAlarmFlowSensNotDetected();
			//manageAlarmIrTempSensNotDetected();

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

void EnableNextAlarmFunc_(void)
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


void alarmEngineAlways_(void)
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


	CalcAlarmActive();

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

int ForcePressArtHigh = 0;
int ForcePressAdsFiltHigh = 0;

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

		//--------------------------------------------------------
		if(ForcePressArtHigh)  // DEBUG !!!!!!!!!!!!!!!!!!!
			alarmList[PRESS_ART_HIGH].physic = PHYSIC_TRUE;
		if(ForcePressAdsFiltHigh)  // DEBUG !!!!!!!!!!!!!!!!!!!
			alarmList[PRESS_ADS_FILTER_HIGH].physic = PHYSIC_TRUE;
		//--------------------------------------------------------


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

void manageCover_Hook_Sensor(void)
{
	if (GlobalFlags.FlagsDef.EnableHooksReservoir)
	{
	    if (HOOK_SENSOR_1_STATUS || HOOK_SENSOR_2_STATUS)
	        alarmList[ALARM_HOOKS_RESERVOIR].physic = PHYSIC_TRUE;
	    else
	        alarmList[ALARM_HOOKS_RESERVOIR].physic = PHYSIC_FALSE;
	}
	else
    {
        alarmList[ALARM_HOOKS_RESERVOIR].physic = PHYSIC_FALSE;
    }

    if (GlobalFlags.FlagsDef.EnableMachineCovers)
    {
        if (0/*coverLizzi*/)
        // if (FRONTAL_COVER_1_STATUS || FRONTAL_COVER_2_STATUS)
            alarmList[ALARM_MACHINE_COVERS].physic = PHYSIC_TRUE;
        else
            alarmList[ALARM_MACHINE_COVERS].physic = PHYSIC_FALSE;
    }
    else
    {
        alarmList[ALARM_MACHINE_COVERS].physic = PHYSIC_FALSE;
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

// Filippo - funzione che gestisce l'allarme di pressione del tasto di stop
void manageAlarmStopButtonPressed(void)
{
	if(!GlobalFlags.FlagsDef.EnableStopButton)
		alarmList[ALARM_STOP_BUTTON].physic = PHYSIC_FALSE;
	else
	{
		if(EMERGENCY_BUTTON_ACTIVATION)
		{
			alarmList[ALARM_STOP_BUTTON].physic = PHYSIC_TRUE;
			EMERGENCY_BUTTON_ACTIVATION=FALSE;
		}
		else
		{
			alarmList[ALARM_STOP_BUTTON].physic = PHYSIC_FALSE;
		}
	}

}

// Filippo - funzione che gestisce l'allarme di pressione del tasto di stop
void manageAlarmT1Test(void)
{
	if(!GlobalFlags.FlagsDef.EnableT1TestAlarm)
		alarmList[ALARM_T1_TEST].physic = PHYSIC_FALSE;
	else
	{
		if(allarmeTestT1Attivo)
		{
			alarmList[ALARM_T1_TEST].physic = PHYSIC_TRUE;
			allarmeTestT1Attivo=FALSE;
		}
		else
		{
			alarmList[ALARM_T1_TEST].physic = PHYSIC_FALSE;
		}
	}

}
// Filippo - funzione che gestisce l'allarme per il fallimento del test del sensore aria
void manageAlarmAirSensorTestKO(void)
{
	if (airSensorTestKO)
	{
//		alarmList[ALARM_AIR_SENSOR_TEST_KO].physic=PHYSIC_TRUE;
	}
	else
	{
	//	alarmList[ALARM_AIR_SENSOR_TEST_KO].physic=PHYSIC_FALSE;
	}

	/*SOLO PER TEST, DA RIMUOVERE*/
	//alarmList[ALARM_AIR_SENSOR_TEST_KO].physic=PHYSIC_FALSE;
	/*SOLO PER TEST, FINE*/
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
	static int CntErrModbusMSGAlmActive = 0;
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
				CntErrModbusMSGAlmActive = 1;
				alarmList[MODBUS_ACTUATOR_SEND].physic = PHYSIC_TRUE;
				//CountErrorModbusMSG[i] = 0;

				/*in questo caso bisogna comunicarlo all'SBC che metterà a video un pop up per le possibili soluzioni*/
				break;
			}
		}

		//se ho ciclato tutto il for senza trovare allarmi e precedentemente era stato attivato un allarme
		if((i == 8) && (alarmList[MODBUS_ACTUATOR_SEND].physic == PHYSIC_TRUE) && CntErrModbusMSGAlmActive)
		{
			CntErrModbusMSGAlmActive = 0;
			alarmList[MODBUS_ACTUATOR_SEND].physic = PHYSIC_FALSE;

			for (int j = 0; j<8; j++)
				CountErrorModbusMSG[j] = 0;
		}
	}
}

// errore su modbus durante le operazioni di scrittura
void manageAlarmActuatorWRModbusNotRespond(void)
{
	static int ActWriteCntAlmActive = 0;
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
				ActWriteCntAlmActive = 1;
				alarmList[MODBUS_ACTUATOR_SEND].physic = PHYSIC_TRUE;

				/*in questo caso bisogna comunicarlo all'SBC che metterà a video un pop up per le possibili soluzioni*/
				break;
			}
		}

		//se ho ciclato tutto il for senza trovare allarmi e precedentemente era stato attivato un allarme
		if((i == LAST_ACTUATOR) && (alarmList[MODBUS_ACTUATOR_SEND].physic == PHYSIC_TRUE) && ActWriteCntAlmActive)
		{
			ActWriteCntAlmActive = 0;
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



void alarmManageNull_(void)
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
	unsigned long elapsed = (unsigned long)timerCounterModBus;
	if ( elapsed >= last )
		elapsed -= last;
	else
		elapsed += (0xFFFFFFFFUL - last + 1);
	return elapsed;
}


// ritorna il numero di intervalli di 10 msec trascorsi dal valore last
unsigned long msTick10_elapsed( unsigned long last )
{
	unsigned long elapsed = (unsigned long)FreeRunCnt10msec;
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

// ritorna true se l'allarme code e' attivo
bool IsAlarmCodeActive(uint16_t code)
{
	if(alarmCurrent.code && (alarmCurrent.code == code) && (ptrAlarmCurrent->active == ACTIVE_TRUE))
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


//-----------------------------------------------------------------------------------------------
//----------------------------GESTIONE DELLE WARNING---------------------------------------------

void manageWarningPhysicPressSensHigh(void)
{
	// USO GLI STESSI ENABLE EDGLI ALLARMI, NON SO SE QUESTO E' GIUSTO
	if(GlobalFlags.FlagsDef.EnablePressSensHighAlm)
	{
		if(PR_ADS_FLT_mmHg_Filtered > PR_ADS_FILTER_WARN)
		{
			alarmList[PRESS_ADS_FILTER_WARN].physic = PHYSIC_TRUE;
		}
		else
		{
			alarmList[PRESS_ADS_FILTER_WARN].physic = PHYSIC_FALSE;
		}

	}
	else
	{
		alarmList[PRESS_ADS_FILTER_WARN].physic = PHYSIC_FALSE;
	}
}


void CalcWarningActive(void)
{
	/*Faccio uno switch su tutta la macchina a stati in modo
	 * gestire ogni allarme in funzioine dello stato in cui sono*/

	switch(ptrCurrentState->state)
	{
		case STATE_NULL:
		case STATE_ENTRY:
		case STATE_IDLE:
		case STATE_SELECT_TREAT:
		case STATE_T1_NO_DISPOSABLE:
		case STATE_MOUNTING_DISP:
		case STATE_TANK_FILL:
			break;

		case STATE_PRIMING_PH_1:
		{
			manageWarningPhysicPressSensHigh();

//			manageAlarmFlowSensNotDetected();
//			manageAlarmIrTempSensNotDetected();
//
//			//verifica physic pressioni
//			manageAlarmPhysicPressSensHigh();
//			//manageAlarmPhysicPressSensLow(); non serve questo allarme in priming
//
//			//verifica physic ir temp sens
//			manageAlarmPhysicTempSens();
//
//			manageAlarmLiquidLevelHigh();
//			if(GetTherapyType() == LiverTreat)
//				manageAlarmCoversPumpLiver();
//			else if(GetTherapyType() == KidneyTreat)
//				manageAlarmCoversPumpKidney();
//
//			manageAlarmCanBus();
//			manageAlarmActuatorModbusNotRespond();
//			manageAlarmActuatorWRModbusNotRespond();
//			manageAlarmFromProtective();
//			manageAlarmBadPinchPos();   // allarme di pinch posizionate correttamente
			break;
		}

		case STATE_PRIMING_PH_2:
		{
			manageWarningPhysicPressSensHigh();

//			manageAlarmFlowSensNotDetected();
//			manageAlarmIrTempSensNotDetected();
//
//			//verifica physic pressioni
//			manageAlarmPhysicPressSensHigh();
//			//manageAlarmPhysicPressSensLow(); non serve questo allarme in priming
//
//			//verifica physic ir temp sens
//			manageAlarmPhysicTempSens();
//
//			manageAlarmLiquidLevelHigh();
//			if(GetTherapyType() == LiverTreat)
//				manageAlarmCoversPumpLiver();
//			else if(GetTherapyType() == KidneyTreat)
//				manageAlarmCoversPumpKidney();
//			manageAlarmCanBus();
//			manageAlarmPrimSFAAirDet();
//			manageAlarmActuatorModbusNotRespond();
//			manageAlarmActuatorWRModbusNotRespond();
//			manageAlarmFromProtective();
//			manageAlarmBadPinchPos();   // allarme di pinch posizionate correttamente
			break;
		}

		case STATE_TREATMENT_KIDNEY_1:
		{
			manageWarningPhysicPressSensHigh();

//			//verifica physic pressioni
//			manageAlarmPhysicPressSensHigh();
//			manageAlarmPhysicPressSensLow();
//
//			//verifica physic flow sensor (presenza aria)
//			manageAlarmPhysicUFlowSens();
//			manageAlarmSAFAirSens();
//			manageAlarmPhysicUFlowSensVen();
//
//
//			//verifica physic ir temp sens
//			manageAlarmPhysicTempSens();
//
//			//verifica physic flusso di perfusione arteriosa alto
//			manageAlarmPhysicFlowPerfArtHigh();
//
//			//verifica  flusso  non rilevato
//			manageAlarmFlowSensNotDetected();
//
//			//verifica temperatura noin rilevata
//			manageAlarmIrTempSensNotDetected();
//
//			if(GetTherapyType() == LiverTreat)
//				manageAlarmCoversPumpLiver();
//			else if(GetTherapyType() == KidneyTreat)
//				manageAlarmCoversPumpKidney();
//
//			// Questo allarme lo commento per ora, perche' bisogna avere un sensore di livello
//			// che funziona bene e si e' sicuri del suo funzionamento
//			//manageAlarmLiquidLevelLow();
//			// i due allarmi che seguono devo essere gestiti attentamente perche' potrei avere delle
//			// segnalazioni di allarme anche durante la fase di accelerazione e decelerazione del pid
//			// Per ora li commento.
//			//manageAlarmDeltaFlowArt();
//			//manageAlarmDeltaFlowVen();
//			manageAlarmDeltaTempRecArt();
//			manageAlarmDeltaTempRecVen();
//			manageAlarmCanBus();
//			manageAlarmBadPinchPos(); // controllo il posizionamento delle pinch prima di iniziare un trattamento
//			manageAlarmActuatorModbusNotRespond();
//			manageAlarmActuatorWRModbusNotRespond();
//			manageAlarmFromProtective();
			break;
		}

		case STATE_PRIMING_WAIT:
			// in questo stato non sono gestiti gli allarmi, per ora
			break;

		case STATE_PRIMING_RICIRCOLO:
			manageWarningPhysicPressSensHigh();

//			//verifica physic pressioni
//			manageAlarmPhysicPressSensHigh();
//			manageAlarmPhysicPressSensLow();
//
//			//verifica physic ir temp sens
//			manageAlarmPhysicTempSens();
//			if(GetTherapyType() == LiverTreat)
//				manageAlarmCoversPumpLiver();
//			else if(GetTherapyType() == KidneyTreat)
//				manageAlarmCoversPumpKidney();
//			manageAlarmCanBus();
//			manageAlarmPumpNotStill();  // controllo allarme di pompe ferme alla fine del ricircolo
//			manageAlarmBadPinchPos();   // allarme di pinch posizionate correttamente
//			manageAlarmPrimSFAAirDet();
//			manageAlarmActuatorModbusNotRespond();
//			manageAlarmActuatorWRModbusNotRespond();
//			manageAlarmFromProtective();
			break;

		case STATE_WAIT_TREATMENT:
			break;

		case STATE_EMPTY_DISPOSABLE:
		case STATE_EMPTY_DISPOSABLE_1:
		{
			manageWarningPhysicPressSensHigh();

//			manageAlarmFlowSensNotDetected();
//			manageAlarmIrTempSensNotDetected();
//
//			manageAlarmPhysicPressSensHigh();
//			manageAlarmPhysicUFlowSens();
//			manageAlarmSAFAirSens();
//			manageAlarmPhysicUFlowSensVen();
//			if(GetTherapyType() == LiverTreat)
//				manageAlarmCoversPumpLiver();
//			else if(GetTherapyType() == KidneyTreat)
//				manageAlarmCoversPumpKidney();
//			manageAlarmActuatorModbusNotRespond();
//			manageAlarmActuatorWRModbusNotRespond();
//			manageAlarmFromProtective();
			break;
		}

		case STATE_TREATMENT_2:
		case STATE_EMPTY_DISPOSABLE_2:
		case STATE_UNMOUNT_DISPOSABLE:
		case STATE_WASHING:
		case STATE_FATAL_ERROR:
			break;

		default:
		{
			break;
		}
	}
}


//---------------------------------------------GESTIONE warning--------------------------------------------------
// numero totale di WARNING presE in considerazione
#define MAX_NUM_WARNING  4
#define MAX_NUM_ALARM  4
#define MAX_NUM_ALARM_ACTIVATED 10
#define MAX_NUM_WARNING_ACTIVATED 10

// task di controllo degli allarmi esegue una scansione della struttura alarmList ogni 50 msec.
// Potrebbe essere necessario allungare questo tempo se volessi effettivamente realizzare la gestione
// differenziata delle attuazioni in base all'allarme. Quuesto e' dovuto al fatto che, una volta stabilita
// l'attuazione da fare per risolvere l'allarme, la funzione ProcessStateMachine ha bisogno di almeno
// due timeslot (due giri di programma) per metterla in atto
#define ALARM_CHECK_PRESCALER 1

void WrnLisStateAlways(void);

// lista di variabili di stato corrispondenti a vari allarmi e warning attivi
int WrnLisStateArr[MAX_NUM_WARNING];
// lista degli indici della struttura alarmListcorrispondenti ai vari warning attivi
// inizializzato a 0xffffffff
int WrnListOf_AlarmListStrctPos[MAX_NUM_WARNING];
// indice dell'array di warning che sto esaminando
int CurrWrnLisStateArrdx;
// posizione nell'array WrnLisStateArr del primo allarme
int WrnLisStateArrFirstWrnPos;

unsigned short elapsedEntryTimeWrn[MAX_NUM_WARNING];
unsigned short elapsedExitTimeWrn[MAX_NUM_WARNING];

// lista dei codici degli allarmi attivi
// il codice di warning e' attivo se vale 0..n
// il codice di warning non e' attivo se vale 0xffff
uint16_t WarningCodeArray[MAX_NUM_ALARM];
struct alarm * ptrWarningCurrent_new;

// 1 controllo allarme
// 0 controllo warning
uint8_t AlarmCheckFlag;

// lista delle posizioni nell'array AlarmList che si sono attivati
int ListOf_WarningListStrctPos[MAX_NUM_ALARM_ACTIVATED];
// lista dei codici di allarme che sono stati attivati
int ListOf_WarningCode[MAX_NUM_ALARM_ACTIVATED];

void EnableNextWarningFunc(void)
{
	EnableNextWarning = TRUE;

	//if((ptrAlarmCurrent->active == ACTIVE_TRUE) && (ptrAlarmCurrent->code == alarmCurrent.code) && (ptrAlarmCurrent->physic == PHYSIC_TRUE))
	if(ptrWarningCurrent->code == warningCurrent.code)
	{
		// FM la warning e' stata disattivata perche' non sono piu'
		// verificate le condizioni fisiche che lo hanno generato
		memset(&warningCurrent, 0, sizeof(struct alarm));
		ptrWarningCurrent = 0;
		warningConInit();
	}
}

bool AddToWarningCodeArray(uint16_t code, int WarningListStrctPos)
{
	int i;
	bool Added = FALSE;
	for(i = 0; i < MAX_NUM_WARNING_ACTIVATED; i++)
	{
		if((ListOf_WarningCode[i] == code) && (ListOf_WarningListStrctPos[i] == WarningListStrctPos))
		{
			// gia' presente
			Added = TRUE;
			break;
		}
	}

	if(i >= MAX_NUM_WARNING_ACTIVATED)
	{
		for(i = 0; i < MAX_NUM_WARNING_ACTIVATED; i++)
		{
			if(ListOf_WarningCode[i] == -1)
			{
				// posizione libera
				ListOf_WarningCode[i] = code;
				ListOf_WarningListStrctPos[i] = WarningListStrctPos;
				Added = TRUE;
				break;
			}
		}
	}
	return Added;
}

bool DeleteFromWarningCodeArray(uint16_t code)
{
	int i;
	bool Deleted = FALSE;
	struct alarm * ptrAlm;
	for(i = 0; i < MAX_NUM_WARNING_ACTIVATED; i++)
	{
		if(ListOf_WarningCode[i] == code)
		{
			// forzo disattivazione allarme perche' poi riparto da 0
			// con il suo controllo (reset delle
			ptrAlm = &alarmList[ListOf_WarningListStrctPos[i]];
			ptrAlm->active = ACTIVE_FALSE;
			// NON SERVE CHIAMARE LA manageAlarmChildGuard PERCHE'
			// NEL CASO DI WARNING NON FACCIO ALCUNA ATTUAZIONE
			// devo azzerare anche GUARD_ALARM_.....
			// altrimenti mi rimane settato
			//manageAlarmChildGuard(ptrAlm);

			// posizione libera
			ListOf_WarningCode[i] = -1;
			ListOf_WarningListStrctPos[i] = -1;
			Deleted = TRUE;
		}
	}
	return Deleted;
}


bool IsWarningElem(struct alarm * ptrAlmArrElem)
{
//	bool ret = FALSE;
//	if(ptrAlmArrElem->priority == PRIORITY_LOW)
//		ret = TRUE;
	bool ret = FALSE;
	if(ptrAlmArrElem->code >= START_WARNING_CODE)
		ret = TRUE;
	return ret;
}

// ritorna true se la warning e' in osservazione
// WarningListStrctCurrPos posizione nella struttura  alarmList
bool IsWarningAlreadyIn_WrnLisStateArr(int WarningListStrctCurrPos)
{
	bool ret = FALSE;
	int i;
	for( i = 0; i < MAX_NUM_WARNING; i++)
	{
		if(IsWarningElem(&alarmList[WarningListStrctCurrPos]) &&
		   (WrnListOf_AlarmListStrctPos[i] == WarningListStrctCurrPos))
		{
			ret = TRUE;
			// la struttura alarm e' gia sotto controllo per gestire un eventuale warning
			break;
		}
	}
	return ret;
}

void warningConInit(void){
	ptrWarningCurrent = 0;
	ptrWarningCurrent_new = 0;
	WrnLisStateArrFirstWrnPos = 0xff;
	memset(WrnLisStateArr, 0, sizeof(WrnLisStateArr));
	// imposto gli indici a nessun warning presente
	memset(WrnListOf_AlarmListStrctPos, 0xff, sizeof(WrnListOf_AlarmListStrctPos));
	CurrWrnLisStateArrdx = 0;
	memset(elapsedEntryTimeWrn, 0, sizeof(elapsedEntryTimeWrn));
	memset(elapsedExitTimeWrn, 0, sizeof(elapsedExitTimeWrn));
	memset(WarningCodeArray, 0xff, sizeof(WarningCodeArray));
	memset(ListOf_WarningListStrctPos, 0xff, sizeof(ListOf_WarningListStrctPos));
	memset(ListOf_WarningCode, 0xff, sizeof(ListOf_WarningCode));
}



void WrnLisStateAlways(void)
{
	int pos_wrn;
	int CurrWarningIDX;
	int SearchNextWarningFlag;

	AlarmCheckFlag = 0;
//	if((WrnLisStateArr[CurrWrnLisStateArrdx] == 0) && !EnableNextWarning)
//	{
//		// non ho ancora premuto il tasto button reset per resettare l'allarme corrente quindi non posso
//		// andare avanti.
//		// Se andassi avanti comunque avrei dei problemi nella gestione di due allarmi diversi e contemporanei
//		// come nel caso di livello alto e cover.
//		return;
//	}

	CalcWarningActive();

	if(WrnListOf_AlarmListStrctPos[CurrWrnLisStateArrdx] == -1)
	{
		// a questo indice dell'array non corrisponde nessun allarme in esame quindi esamino la struttura partendo da 0
		pos_wrn = 0;
		SearchNextWarningFlag = 1;
	}
	else
	{
		// sono nella fase di inizio rilevazione di un allarme (calcolo del ritardo in entrata
		// o allarme in corso quindi parto direttamente dalla posizione dell'allarme attuale
		pos_wrn = WrnListOf_AlarmListStrctPos[CurrWrnLisStateArrdx];
		SearchNextWarningFlag = 0;
	}

	for(; pos_wrn<ALARM_ACTIVE_IN_STRUCT; pos_wrn++)
	{
		if(SearchNextWarningFlag && IsWarningAlreadyIn_WrnLisStateArr(pos_wrn) || (!IsWarningElem(&alarmList[pos_wrn])))
		{
			// la warning e' gia' sotto osservazione quindi devo vedere se ne
			// trovo un'altra
			// oppure l'elemento non e' una struttura di warning
			continue;
		}
		//if(alarmList[pos_wrn].priority == PRIORITY_LOW)
		if(IsWarningElem(&alarmList[pos_wrn]))
		{
			// CONSIDERO WARNING SOLO GLI ALLARMI DI PRIORITA' BASSA
			if((alarmList[pos_wrn].physic == PHYSIC_TRUE) && (alarmList[pos_wrn].active != ACTIVE_TRUE))
			{
				ptrWarningCurrent_new = &alarmList[pos_wrn];
				alarmList[pos_wrn].prySafetyActionFunc();
				WrnListOf_AlarmListStrctPos[CurrWrnLisStateArrdx] = pos_wrn;
				// salvo il codice dell'allarme nella lista alla posizione CurrAlmLisStateArrdx
				// che dovrebbe essere libera
				AddToWarningCodeArray(ptrWarningCurrent_new->code, WrnListOf_AlarmListStrctPos[CurrWrnLisStateArrdx]);
				if(WarningCodeArray[CurrWrnLisStateArrdx] == 0xffff)
					WarningCodeArray[CurrWrnLisStateArrdx] = ptrWarningCurrent_new->code;
				break;
			}
			else if((alarmList[pos_wrn].active == ACTIVE_TRUE) && (alarmList[pos_wrn].physic == PHYSIC_FALSE))
			{
				ptrWarningCurrent_new = &alarmList[pos_wrn];
				alarmList[pos_wrn].prySafetyActionFunc();
				break;
			}
			else
			{
				if(WrnLisStateArr[CurrWrnLisStateArrdx])
				{
					// allarme ancora in corso, sono in attesa di ACTIVE_FALSE
					// quindi, per ora, non posso prendere in considerazione altri allarmi
					break;
				}
			}
		}
	}

	if( !WrnLisStateArr[CurrWrnLisStateArrdx] && (CurrWrnLisStateArrdx < ALARM_ACTIVE_IN_STRUCT))
	{
		CurrWarningIDX = WrnListOf_AlarmListStrctPos[CurrWrnLisStateArrdx];
		if(CurrWarningIDX != -1)
		{
			if(alarmList[CurrWarningIDX].active == ACTIVE_TRUE)
			{
				WrnLisStateArr[CurrWrnLisStateArrdx] = 1;
			}
			else
			{
				// potrebbe essersi verificato un allarme molto breve che non e' riuscito
				// ad attivarsi, forzo una ripartenza dall'inizio della tabella
				WrnListOf_AlarmListStrctPos[CurrWrnLisStateArrdx] = -1;
				WarningCodeArray[CurrWrnLisStateArrdx] = 0xffff;
				EnableNextWarning = TRUE;
				//memset(&warningCurrent, 0, sizeof(struct alarm));
			}
		}
	}
	else if(WrnLisStateArr[CurrWrnLisStateArrdx] == 1)
	{
		CurrWarningIDX = WrnListOf_AlarmListStrctPos[CurrWrnLisStateArrdx];
		if(CurrWarningIDX != -1)
		{
			ShowAlarmStr((int)alarmList[CurrWarningIDX].code, " on");
			WrnLisStateArr[CurrWrnLisStateArrdx] = 2;
			EnableNextWarning = FALSE;
		}
	}
	else if(WrnLisStateArr[CurrWrnLisStateArrdx] == 2)
	{
		CurrWarningIDX = WrnListOf_AlarmListStrctPos[CurrWrnLisStateArrdx];
		if( (CurrWarningIDX != -1) && alarmList[CurrWarningIDX].active == ACTIVE_FALSE)
		{
			// allarme terminato
			WrnLisStateArr[CurrWrnLisStateArrdx] = 0;
			ShowAlarmStr((int)alarmList[CurrWarningIDX].code, " off");
			WrnListOf_AlarmListStrctPos[CurrWrnLisStateArrdx] = -1;
			WarningCodeArray[CurrWrnLisStateArrdx] = 0xffff;
		}
	}
}

void warningsEngineAlways(void)
{
	static int WrnLisStateArr_id = 0;
	static int WarningCheckPresc = 0; // il prescaler serve per la eventuale gestione differenziata dell'allarme

	WarningCheckPresc++;
	if(WarningCheckPresc >= ALARM_CHECK_PRESCALER)
	{
		WarningCheckPresc = 0;
		CurrWrnLisStateArrdx = WrnLisStateArr_id;

		WrnLisStateAlways();

		WrnLisStateArr_id++;
		if(WrnLisStateArr_id >= MAX_NUM_WARNING)
			WrnLisStateArr_id = 0;
	}


//	int i;
//	for(i = 0; i < MAX_NUM_WARNING; i++)
//	{
//		CurrWrnLisStateArrdx = i;
//		WrnLisStateAlways();
//	}
}


void warningManageNull(void)
{
	elapsedEntryTimeWrn[CurrWrnLisStateArrdx] = elapsedEntryTimeWrn[CurrWrnLisStateArrdx] + 50;
	elapsedExitTimeWrn[CurrWrnLisStateArrdx] = elapsedExitTimeWrn[CurrWrnLisStateArrdx] + 50;
	if(IsWarningElem(ptrWarningCurrent_new) && (ptrWarningCurrent_new->active != ACTIVE_TRUE) &&
	   (elapsedEntryTimeWrn[CurrWrnLisStateArrdx] > ptrWarningCurrent_new->entryTime))
	{
		// entro nella gestione di un allarme che ha bisogno di azioni sugli attuatori
		elapsedEntryTimeWrn[CurrWrnLisStateArrdx] = 0;
		elapsedExitTimeWrn[CurrWrnLisStateArrdx] = 0;
		ptrWarningCurrent_new->active = ACTIVE_TRUE;
		//currentGuard[GUARD_ALARM_ACTIVE].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;

		//-----------------------faccio partire l'allarme-------------------------------
		if(!ptrWarningCurrent)
		{
			// FM ora AlarmCurrent contiene l'allarme attivo corrente che sara' inviato ad SBC
			warningCurrent = *ptrWarningCurrent_new;
			ptrWarningCurrent = ptrWarningCurrent_new;
			if(alarmCurrent.code == 0)
			{
				// non ci sono allarmi visualizzo subito la warning
				alarmCurrent = *ptrWarningCurrent_new;
			}

			if(alarmCurrent.code == 0)
				alarmCurrent = *ptrWarningCurrent;
			// mi salvo in questa variabile globale la posizione nell'array WrnLisStateArr della
			// prima warning rilevata
			WrnLisStateArrFirstWrnPos = CurrWrnLisStateArrdx;
		}

		//------------------------------------------------------------------------------
	}
	else if(IsWarningElem(ptrWarningCurrent_new) && (ptrWarningCurrent_new->active == ACTIVE_TRUE) &&
		    (elapsedExitTimeWrn[CurrWrnLisStateArrdx] > ptrWarningCurrent_new->exitTime))
	{
		// esco dalla gestione di un allarme che ha bisogno di azioni sugli attuatori
		elapsedEntryTimeWrn[CurrWrnLisStateArrdx] = 0;
		elapsedExitTimeWrn[CurrWrnLisStateArrdx] = 0;
		ptrWarningCurrent_new->active = ACTIVE_FALSE;

		WrnListOf_AlarmListStrctPos[CurrWrnLisStateArrdx] = 0;
	}
}

// ritorna su 16 bit il codice di allarme e quello dello warning
uint16_t GetAlarmAndWarnCode(void)
{
	uint16_t u16;
	u16 = alarmCurrent.code;
	u16 |= (warningCurrent.code << 8);
	return u16;
}



//---------------------------------------------NUOVA GESTIONE ALLARMI--------------------------------------------------
// numero totale di allarmi e warning presi in considerazione
void AlmLisStateAlways(void);

// lista di variabili di stato corrispondenti a vari allarmi e warning attivi
int AlmLisStateArr[MAX_NUM_ALARM];
// lista degli indici della struttura alarmListcorrispondenti ai vari allarmi attivi
// inizializzato a 0xffffffff
int AlmListOf_AlarmListStrctPos[MAX_NUM_ALARM];
// indice dell'array AlmLisStateArr di di stato degli allarmi che sto esaminando
int CurrAlmLisStateArrdx;
// posizione nell'array AlmLisStateArr del primo allarme
int AlmLisStateArrFirstWrnPos;
unsigned short elapsedEntryTime[MAX_NUM_ALARM];
unsigned short elapsedExitTime[MAX_NUM_ALARM];

// lista dei codici degli allarmi che si sono attivati
// il codice di allarme e' attivo se vale 0..n
// il codice di allarme non e' attivo se vale 0xffff
uint16_t AlarmCodeArray[MAX_NUM_ALARM];
struct alarm * ptrAlarmCurrent_new;

// lista delle posizioni nell'array AlarmList che si sono attivati
int ListOf_AlarmListStrctPos[MAX_NUM_ALARM_ACTIVATED];
// lista dei codici di allarme che sono stati attivati
int ListOf_AlarmCode[MAX_NUM_ALARM_ACTIVATED];
// posizione nell'array WrnLisStateArr del primo allarme
int AlmLisStateArrFirstAlmPos;


void EnableNextAlarmFunc(void)
{
	EnableNextAlarm = TRUE;

	//if((ptrAlarmCurrent->active == ACTIVE_TRUE) && (ptrAlarmCurrent->code == alarmCurrent.code) && (ptrAlarmCurrent->physic == PHYSIC_TRUE))
	if(ptrAlarmCurrent->code == alarmCurrent.code)
	{
		// FM l'allarme e' stato disattivato perche' non sono piu'
		// verificate le condizioni fisiche che lo hanno generato
		memset(&alarmCurrent, 0, sizeof(struct alarm));
		ptrAlarmCurrent = 0;
		alarmConInit();
	}
}

bool AddToAlarmCodeArray(uint16_t code, int AlarmListStrctPos)
{
	int i;
	bool Added = FALSE;
	for(i = 0; i < MAX_NUM_ALARM_ACTIVATED; i++)
	{
		if((ListOf_AlarmCode[i] == code) && (ListOf_AlarmListStrctPos[i] == AlarmListStrctPos))
		{
			// gia' presente
			Added = TRUE;
			break;
		}
	}

	if(i >= MAX_NUM_ALARM_ACTIVATED)
	{
		for(i = 0; i < MAX_NUM_ALARM_ACTIVATED; i++)
		{
			if(ListOf_AlarmCode[i] == -1)
			{
				// posizione libera
				ListOf_AlarmCode[i] = code;
				ListOf_AlarmListStrctPos[i] = AlarmListStrctPos;
				Added = TRUE;
				break;
			}
		}
	}
	return Added;
}

// controlla se si tratta di un allarme aria e fa partire il recupero
// dopo l'ultimo reset
int IfAirAlmStartRecovery(struct alarm * ptrAlm)
{
	int AirAlmFlag = 0;
	switch (ptrAlm->secActType)
	{
	case SECURITY_SAF_AIR_FILTER:
		currentGuard[GUARD_ALARM_AIR_FILT_RECOVERY].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		AirAlmFlag = 1;
		break;
	case SECURITY_SFV_AIR_DET:
		currentGuard[GUARD_ALARM_AIR_SFV_RECOVERY].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		AirAlmFlag = 1;
		break;
	case SECURITY_SFA_AIR_DET:
		currentGuard[GUARD_ALARM_AIR_SFA_RECOVERY].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		AirAlmFlag = 1;
		break;
	case SECURITY_SFA_PRIM_AIR_DET:
		currentGuard[GUARD_ALARM_PRIM_AIR_FILT_RECOVERY].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		AirAlmFlag = 1;
		break;
	}
	return AirAlmFlag;
}


bool DeleteFromAlarmCodeArray(uint16_t code)
{
	int i;
	bool Deleted = FALSE;
	struct alarm * ptrAlm;
	for(i = 0; i < MAX_NUM_ALARM_ACTIVATED; i++)
	{
		if(ListOf_AlarmCode[i] == code)
		{
			// forzo disattivazione allarme perche' poi riparto da 0
			// con il suo controllo (reset delle
			ptrAlm = &alarmList[ListOf_AlarmListStrctPos[i]];
			ptrAlm->active = ACTIVE_FALSE;
			// devo azzerare anche GUARD_ALARM_.....
			// altrimenti mi rimane settato
			manageAlarmChildGuard(ptrAlm);
			if(!IfAirAlmStartRecovery(ptrAlm))
			{
				// In questo if vanno inseriti tutti i codici di allarme che hanno un codice di
				// attuazione pari a SECURITY_STOP_ALL_ACT_WAIT_CMD che richiede di forzare off l'allarme ed uscire
				// semplicemente con un reset
				if(ptrAlm->code == CODE_ALARM_TANK_LEVEL_HIGH)
				{
					// Questa tipologia di allarmi deve essere forzata in off dal software prima di poter riprendere il lavoro
					ForceCurrentAlarmOff();
					// setto la guard per fare in modo che quando l'allarme risultera' non attivo
					// la macchina a stati parent riprenda il funzionamento normale
					currentGuard[GUARD_ALARM_WAIT_CMD_TO_EXIT].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;

					// l'allarme viene generato dal sensore di livello, quindi, se voglio andare avanti normalmente
					// (dato che il sensore mi dice che il recipiente e' pieno) devo forzare anche la quantita' di liquido
					// di priming al target richiesto
					perfusionParam.priVolPerfArt = GetTotalPrimingVolumePerf(0);
				}
			}

			// posizione libera
			ListOf_AlarmCode[i] = -1;
			ListOf_AlarmListStrctPos[i] = -1;
			Deleted = TRUE;
			break;
		}
	}
	return Deleted;
}


// controlla se l'elemento alarm contiene un allarme o una warning
bool IsAlarmElem(struct alarm * ptrAlmArrElem)
{
	bool ret = FALSE;
	if(!IsWarningElem(ptrAlmArrElem))
		ret = TRUE;
	return ret;
}

// ritorna true se l'allarme e' in osservazione
// AlarmListStrctCurrPos posizione nella struttura  alarmList
bool IsAlarmAlreadyIn_AlmLisStateArr(int AlarmListStrctCurrPos)
{
	bool ret = FALSE;
	int i;
	for( i = 0; i < MAX_NUM_ALARM; i++)
	{
		if(IsAlarmElem(&alarmList[AlarmListStrctCurrPos]) &&
		   AlmListOf_AlarmListStrctPos[i] == AlarmListStrctCurrPos)
		{
			ret = TRUE;
			// la struttura alarm e' gia sotto controllo per gestire un eventuale allarme
			break;
		}
	}
	return ret;
}

void alarmConInit(void){
	ptrAlarmCurrent = 0;
	ptrAlarmCurrent_new = 0;
	AlmLisStateArrFirstAlmPos = 0xff;
	memset(AlmLisStateArr, 0, sizeof(AlmLisStateArr));
	// imposto gli indici a nessun allarme presente
	memset(AlmListOf_AlarmListStrctPos, 0xff, sizeof(AlmListOf_AlarmListStrctPos));
	CurrAlmLisStateArrdx = 0;
	memset(elapsedEntryTime, 0, sizeof(elapsedEntryTime));
	memset(elapsedExitTime, 0, sizeof(elapsedExitTime));
	memset(AlarmCodeArray, 0xff, sizeof(AlarmCodeArray));
	AlarmCheckFlag = 0;
	memset(ListOf_AlarmListStrctPos, 0xff, sizeof(ListOf_AlarmListStrctPos));
	memset(ListOf_AlarmCode, 0xff, sizeof(ListOf_AlarmCode));
}


void AlmLisStateAlways(void)
{
	int pos_alm;
	int CurrAlarmIDX;
	int SearchNextAlarmFlag;

	AlarmCheckFlag = 1;
//	// QUESTO CONTROLLO NON DOVREBBE SERVIRE PIU' NELLA NUOVA GESTIONE PERCHE' IL PROGRAMMA DEVE CONTINUAMENTE
//  // MONITORARE LE CONDIZIONI DI ALLARME PER TENERNE TRACCIA
//	if((alarmCurrent.code == alarmList[AlmListOf_AlarmListStrctPos[CurrAlmLisStateArrdx]].code) &&
//	   (AlmLisStateArr[CurrAlmLisStateArrdx] == 0) && !EnableNextAlarm)
//	{
//		// non ho ancora premuto il tasto button reset per resettare l'allarme corrente quindi non posso
//		// andare avanti.
//		// Se andassi avanti comunque avrei dei problemi nella gestione di due allarmi diversi e contemporanei
//		// come nel caso di livello alto e cover.
//		return;
//	}

	CalcAlarmActive();

	if(AlmListOf_AlarmListStrctPos[CurrAlmLisStateArrdx] == -1)
	{
		// a questo indice dell'array non corrisponde nessun allarme in esame quindi esamino la struttura partendo da 0
		pos_alm = 0;
		SearchNextAlarmFlag = 1;
	}
	else
	{
		// sono nella fase di inizio rilevazione di un allarme (calcolo del ritardo in entrata
		// o allarme in corso quindi parto direttamente dalla posizione dell'allarme attuale
		pos_alm = AlmListOf_AlarmListStrctPos[CurrAlmLisStateArrdx];
		SearchNextAlarmFlag = 0;
	}

	for(; pos_alm<ALARM_ACTIVE_IN_STRUCT; pos_alm++)
	{
		if(SearchNextAlarmFlag && IsAlarmAlreadyIn_AlmLisStateArr(pos_alm) || IsWarningElem(&alarmList[pos_alm]))
		{
			// l'allarme e' gia' sotto osservazione oppure si tratta di una warning
			// quindi devo saltare questa posizione
			continue;
		}

		if((alarmList[pos_alm].physic == PHYSIC_TRUE) && (alarmList[pos_alm].active != ACTIVE_TRUE))
		{
			ptrAlarmCurrent_new = &alarmList[pos_alm];
			alarmList[pos_alm].prySafetyActionFunc();
			AlmListOf_AlarmListStrctPos[CurrAlmLisStateArrdx] = pos_alm;
			// salvo il codice dell'allarme nella lista alla posizione CurrAlmLisStateArrdx
			// che dovrebbe essere libera
			AddToAlarmCodeArray(ptrAlarmCurrent_new->code, AlmListOf_AlarmListStrctPos[CurrAlmLisStateArrdx]);
			if(AlarmCodeArray[CurrAlmLisStateArrdx] == 0xffff)
				AlarmCodeArray[CurrAlmLisStateArrdx] = ptrAlarmCurrent_new->code;

			// FM forse qui devo interrompere perche' ho trovato una condizione di allarme da attivare
			// e devo gestirla prima di andare a vedere le altre
			break;
		}
		else if((alarmList[pos_alm].active == ACTIVE_TRUE) && (alarmList[pos_alm].physic == PHYSIC_FALSE))
		{
			ptrAlarmCurrent_new = &alarmList[pos_alm];
			alarmList[pos_alm].prySafetyActionFunc();

			// FM forse qui devo interrompere perche' ho trovato una condizione di allarme da disattivare
			// e devo gestirla prima di andare a vedere le altre
			break;
		}
		else
		{
			if(AlmLisStateArr[CurrAlmLisStateArrdx])
			{
				// allarme ancora in corso, sono in attesa di ACTIVE_FALSE
				// quindi, per ora, non posso prendere in considerazione altri allarmi
				break;
			}
		}
	}

	if( !AlmLisStateArr[CurrAlmLisStateArrdx] && (AlmListOf_AlarmListStrctPos[CurrAlmLisStateArrdx] < ALARM_ACTIVE_IN_STRUCT))
	{
		// prendo l'indice della struttura alarm in esame
		CurrAlarmIDX = AlmListOf_AlarmListStrctPos[CurrAlmLisStateArrdx];
		if(CurrAlarmIDX != -1)
		{
			if(alarmList[CurrAlarmIDX].active == ACTIVE_TRUE)
			{
				AlmLisStateArr[CurrAlmLisStateArrdx] = 1;
			}
			else
			{
				// potrebbe essersi verificato un allarme molto breve che non e' riuscito
				// ad attivarsi, forzo una ripartenza dall'inizio della tabella
				AlmListOf_AlarmListStrctPos[CurrAlmLisStateArrdx] = -1;
				AlarmCodeArray[CurrAlmLisStateArrdx] = 0xffff;
				EnableNextAlarm = TRUE;
				//memset(&alarmCurrent, 0, sizeof(struct alarm));
			}
		}
	}
	else if(AlmLisStateArr[CurrAlmLisStateArrdx] == 1)
	{
		CurrAlarmIDX = AlmListOf_AlarmListStrctPos[CurrAlmLisStateArrdx];
		if(CurrAlarmIDX != -1)
		{
			ShowAlarmStr((int)alarmList[CurrAlarmIDX].code, " on");
			AlmLisStateArr[CurrAlmLisStateArrdx] = 2;
			EnableNextAlarm = FALSE;
		}
	}
	else if(AlmLisStateArr[CurrAlmLisStateArrdx] == 2)
	{
		CurrAlarmIDX = AlmListOf_AlarmListStrctPos[CurrAlmLisStateArrdx];
		if((CurrAlarmIDX != -1) && alarmList[CurrAlarmIDX].active == ACTIVE_FALSE)
		{
			// allarme terminato
			AlmLisStateArr[CurrAlmLisStateArrdx] = 0;
			ShowAlarmStr((int)alarmList[CurrAlarmIDX].code, " off");
			AlmListOf_AlarmListStrctPos[CurrAlmLisStateArrdx] = -1;
			AlarmCodeArray[CurrAlmLisStateArrdx] = 0xffff;
		}
	}
}


void alarmEngineAlways(void)
{
	static int AlmLisStateArr_id = 0;
	static int AlarmCheckPresc = 0; // il prescaler serve per la eventuale gestione differenziata dell'allarme

	AlarmCheckPresc++;
	if(AlarmCheckPresc >= ALARM_CHECK_PRESCALER)
	{
		AlarmCheckPresc = 0;
		CurrAlmLisStateArrdx = AlmLisStateArr_id;

		AlmLisStateAlways();

		AlmLisStateArr_id++;
		if(AlmLisStateArr_id >= MAX_NUM_ALARM)
			AlmLisStateArr_id = 0;
	}

//	int i;
//	for(i = 0; i < MAX_NUM_ALARM; i++)
//	{
//		CurrAlmLisStateArrdx = i;
//		AlmLisStateAlways();
//	}
}


void alarmManageNull(void)
{
	if(!AlarmCheckFlag)
	{
		// se faccio l'analisi delle warning devo chiamare questa funzione
		warningManageNull();
	}
	else
	{
		elapsedEntryTime[CurrAlmLisStateArrdx] = elapsedEntryTime[CurrAlmLisStateArrdx] + 50;
		elapsedExitTime[CurrAlmLisStateArrdx] = elapsedExitTime[CurrAlmLisStateArrdx] + 50;
		if((ptrAlarmCurrent_new->active != ACTIVE_TRUE) && (elapsedEntryTime[CurrAlmLisStateArrdx] > ptrAlarmCurrent_new->entryTime))
		{
			// entro nella gestione di un allarme che ha bisogno di azioni sugli attuatori
			elapsedEntryTime[CurrAlmLisStateArrdx] = 0;
			elapsedExitTime[CurrAlmLisStateArrdx] = 0;
			ptrAlarmCurrent_new->active = ACTIVE_TRUE;

			//-----------------------faccio partire l'allarme-------------------------------
			// prendo il primo che si verifica
			if(!ptrAlarmCurrent)
			{
				ptrAlarmCurrent = ptrAlarmCurrent_new;
				alarmCurrent = *ptrAlarmCurrent_new;
				currentGuard[GUARD_ALARM_ACTIVE].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
				// FM ora AlarmCurrent contiene l'allarme attivo corrente che sara' inviato ad SBC
				//alarmCurrent = *ptrAlarmCurrent_new;
				if(IsNonPhysicalAlm((int)alarmCurrent.code))
				{
					// e' solo una segnalazione, l'allarme non deve essere gestito tramite le tabelle child
					currentGuard[GUARD_ALARM_ACTIVE].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				}
				// mi salvo in questa variabile globale la posizione nell'array WrnLisStateArr della
				// prima warning rilevata
				AlmLisStateArrFirstAlmPos = CurrAlmLisStateArrdx;
			}
			//------------------------------------------------------------------------------
			manageAlarmChildGuard(ptrAlarmCurrent_new);
		}
		else if((ptrAlarmCurrent_new->active == ACTIVE_TRUE) && (elapsedExitTime[CurrAlmLisStateArrdx] > ptrAlarmCurrent_new->exitTime))
		{
			// esco dalla gestione di un allarme che ha bisogno di azioni sugli attuatori
			elapsedEntryTime[CurrAlmLisStateArrdx] = 0;
			elapsedExitTime[CurrAlmLisStateArrdx] = 0;
			ptrAlarmCurrent_new->active = ACTIVE_FALSE;

			// devo spostarlo dopo l'ultimo reset
			//currentGuard[GUARD_ALARM_ACTIVE].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;

			manageAlarmChildGuard(ptrAlarmCurrent_new);

			// allarme terminato riparto dall'indice 0 dell'array di strutture
			AlmListOf_AlarmListStrctPos[CurrAlmLisStateArrdx] = 0;
		}
	}
}


//-----------------------------------------------------------------------------------------------------------------
// Copia la  successiva warning (se esiste) nella struttura alarmCurrent
// ritorna l'indice nella lista WrnLisStateArr della nuova warning (-1 se non esiste
// una nuova warning)
int GetNextWarningFromList(void)
{
	int NewIndex = -1;
	static int i = 0;
	for(; i < MAX_NUM_WARNING_ACTIVATED; i++)
	{
		if(ListOf_WarningListStrctPos[i] != -1)
		{
			ptrWarningCurrent = &alarmList[ListOf_WarningListStrctPos[i]];
			warningCurrent = *ptrWarningCurrent;
			// la copio nella struttura globale per farla vedere al PC
			alarmCurrent = *ptrWarningCurrent;
			NewIndex = i;
			break;
		}
		// per le warning questo controllo non serve
		//tanto sono sempre visualizzate dopo gli allarmi
//		if(ListOf_WarningListStrctPos[i] == WrnListOf_AlarmListStrctPos[WrnLisStateArrFirstWrnPos])
//		{
//			// warning visualizzata per prima
//			continue;
//		}
	}

	if(i >= MAX_NUM_WARNING_ACTIVATED)
		i = 0;
	return NewIndex;
}

int GetTotNumWarning( void )
{
	int cnt = 0;
	int i;
	for(i = 0; i < MAX_NUM_WARNING_ACTIVATED; i++)
	{
		if(ListOf_WarningCode[i] != -1)
		{
			cnt++;
		}
	}
	return cnt;
}

// Cancella dalla lista AlmLisStateArr l'allarme nella posizione IndiceArr
// Potrebbe anche non essere presente
void ResetWarningFromList( int code )
{
	int i;
	for(i = 0; i < MAX_NUM_WARNING; i++)
	{
		if(WarningCodeArray[i] == code)
		{
			WrnLisStateArr[i] = 0;   // annullo lo stato
			WrnListOf_AlarmListStrctPos[i] = -1;
			elapsedEntryTimeWrn[i] = 0;
			elapsedExitTimeWrn[i] = 0;
			WarningCodeArray[i] = 0xffff;
		}
	}
}

void ResetWarninigWithCode(uint16_t code)
{
	// cancello dalla lista totale degli warning  (MAX_NUM_WARNING_ACTIVATED elementi)
	DeleteFromWarningCodeArray(code);
	// cancello dalla lista di ricerca (MAX_NUM_WARNING elementi) degli allarmi
	ResetWarningFromList(code);
}


//-----------------------------------------------------------------------------------------------------------------

// Copia il successivo allarme (se esiste) nella struttura alarmCurrent
// ritorna l'indice nella lista ListOf_AlarmListStrctPos del nuovo allarme(-1 se non esiste
// un nuovo allarme)
int GetNextAlarmFromList( void )
{
	int NewIndex = -1;
	static int i = 0;
	for(; i < MAX_NUM_ALARM_ACTIVATED; i++)
	{
		if(ListOf_AlarmListStrctPos[i] != -1)
		{
			ptrAlarmCurrent = &alarmList[ListOf_AlarmListStrctPos[i]];
			alarmCurrent = *ptrAlarmCurrent;
			NewIndex = i;
			break;
		}
		if(ListOf_AlarmListStrctPos[i] == AlmListOf_AlarmListStrctPos[AlmLisStateArrFirstAlmPos])
			continue;
	}

	if(i >= MAX_NUM_ALARM_ACTIVATED)
		i = 0;
	return NewIndex;
}

int GetTotNumAlarm( void )
{
	int cnt = 0;
	int i;
	for(i = 0; i < MAX_NUM_ALARM_ACTIVATED; i++)
	{
		if(ListOf_AlarmCode[i] != -1)
		{
			cnt++;
		}
	}
	return cnt;
}

// Cancella dalla lista AlmLisStateArr l'allarme con il codice code
// Potrebbe anche non essere presente
void ResetAlarmFromList( int code )
{
	int i;
	for(i = 0; i < MAX_NUM_ALARM; i++)
	{
		if(AlarmCodeArray[i] == code)
		{
			AlmLisStateArr[i] = 0;   // annullo lo stato
			AlmListOf_AlarmListStrctPos[i] = -1;
			elapsedEntryTime[i] = 0;
			elapsedExitTime[i] = 0;
			AlarmCodeArray[i] = 0xffff;
		}
	}
}

void ResetAlarmWithCode(uint16_t code)
{
	// cancello dalla lista totale degli allarmi (MAX_NUM_ALARM_ACTIVATED elementi)
	DeleteFromAlarmCodeArray(code);
	// cancello dalla lista di ricerca (MAX_NUM_ALARM elementi) degli allarmi
	ResetAlarmFromList(code);
}


// devo chiamarla quando mi arriva un comando di reset allarme da utente
// se ritorna true vuol dire che il reset può essere lasciato transitare alla
// macchina a stati, altrimenti devo visualizzare l'allarme successivo
//bool ResetAlmHandleFunc(uint16_t code)
//{
//	bool ResetGoToProc = FALSE;
//	int ival;
//
//	if(code == 0xffff)
//	{
//		ResetGoToProc = TRUE;
//		return ResetGoToProc;
//	}
//
//	if(AmJInAlarmState())
//	{
//		// se e' una warning la resetto altrimenti non faccio niente
//		//ResetWarninigWithCode(code);
//		// se e' una allarme lo resetto altrimenti non faccio niente
//		ResetAlarmWithCode(code);
//		if(GetTotNumAlarm())
//		{
//			ResetGoToProc = FALSE;
//			ival = GetNextAlarmFromList();
//			if(ival == -1)
//			{
//				// qualcosa non va, questo non e' possibile
//			}
//		}
//		else if(GetTotNumWarning())
//		{
////			memset(&alarmCurrent,  0, sizeof(alarmCurrent));
////			currentGuard[GUARD_ALARM_ACTIVE].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
////			currentGuard[GUARD_ALARM_ACTIVE].guardValue = GUARD_VALUE_FALSE;
////			ptrAlm = &alarmList[ListOf_AlarmListStrctPos[i]];
////			ptrAlm->active = ACTIVE_FALSE;
////			// devo azzerare anche GUARD_ALARM_.....
////			// altrimenti mi rimane settato
////			manageAlarmChildGuard(ptrAlm);
//
//			AtLeastoneButResRcvd = TRUE;
//			ResetGoToProc = TRUE;
//			ival = GetNextWarningFromList();
//			if(ival == -1)
//			{
//				// qualcosa non va, questo non e' possibile
//			}
//		}
//		else
//		{
//			// non ci sono piu' allarmi o warning posso resettare anche le ultime variabili
//			ptrAlarmCurrent = 0;
//			ptrAlarmCurrent_new = 0;
//			AlmLisStateArrFirstWrnPos = 0xff;
//			CurrAlmLisStateArrdx = 0;
//			ptrWarningCurrent = 0;
//			ptrWarningCurrent_new = 0;
//			WrnLisStateArrFirstWrnPos = 0xff;
//			CurrWrnLisStateArrdx = 0;
//			memset(&alarmCurrent, 0, sizeof(struct alarm));
//
//			// forzo uscita dallo stato di allarme
//			currentGuard[GUARD_ALARM_ACTIVE].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
//			ResetGoToProc = TRUE;
//		}
//	}
//	else
//	{
//		ResetWarninigWithCode(code);
//		if(GetTotNumWarning())  //IsWarningElem(struct alarm * ptrAlmArrElem))
//		{
////			memset(&alarmCurrent,  0, sizeof(alarmCurrent));
////			currentGuard[GUARD_ALARM_ACTIVE].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
////			currentGuard[GUARD_ALARM_ACTIVE].guardValue = GUARD_VALUE_FALSE;
//			ResetGoToProc = FALSE;
//			//if(alarmCurrent.code == warningCurrent.code)
//			// se e' una warning la resetto altrimenti non faccio niente
//			ival = GetNextWarningFromList();
//			if(ival == -1)
//			{
//				// qualcosa non va, questo non e' possibile
//			}
//		}
//		else
//		{
//			// non sono in uno stato di allarme quindi il reset puo' essere inviato alla macchina a stati
//			ResetGoToProc = TRUE;
//		}
//	}
//	return ResetGoToProc;
//}


bool ResetAlmHandleFunc(uint16_t code)
{
	bool ResetGoToProc = FALSE;
	int ival;

	if(AmJInAlarmState())
	{
		if(GetTotNumAlarm())
		{
			// loop allarmi

			// se e' una allarme lo resetto altrimenti non faccio niente
			ResetAlarmWithCode(code);
			if(GetTotNumAlarm())
			{
				ResetGoToProc = FALSE;
				ival = GetNextAlarmFromList();
				if(ival == -1)
				{
					// qualcosa non va, questo non e' possibile
				}
			}
			else
			{
				// allarmi finiti
				// non ci sono piu' allarmi o warning posso resettare anche le ultime variabili
				ptrAlarmCurrent = 0;
				ptrAlarmCurrent_new = 0;
				AlmLisStateArrFirstWrnPos = 0xff;
				CurrAlmLisStateArrdx = 0;
				ptrWarningCurrent = 0;
				ptrWarningCurrent_new = 0;
				WrnLisStateArrFirstWrnPos = 0xff;
				CurrWrnLisStateArrdx = 0;
				memset(&alarmCurrent, 0, sizeof(struct alarm));

				// forzo uscita dallo stato di allarme
				currentGuard[GUARD_ALARM_ACTIVE].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				ResetGoToProc = TRUE;

				if(GetTotNumWarning())
				{
					ResetGoToProc = FALSE;
					ival = GetNextWarningFromList();
					if(ival == -1)
					{
						// qualcosa non va, questo non e' possibile
					}
				}
			}
		}
	}
	else if(GetTotNumWarning())
	{
		ResetWarninigWithCode(code);
		// loop warning
		if(GetTotNumWarning())
		{
			ResetGoToProc = FALSE;
			ival = GetNextWarningFromList();
			if(ival == -1)
			{
				// qualcosa non va, questo non e' possibile
			}
		}
	}
	else
	{
		// non sono in uno stato di allarme quindi il reset puo' essere inviato alla macchina a stati
		ResetGoToProc = TRUE;
	}

	return ResetGoToProc;
}

