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
extern bool gDigitalTest;
extern unsigned char coverStateGlobal;

// Gestione allarmi: aprile 2019 -- begin
static signed int sIdxCurrentActiveListAlm = EMPTY_LIST_ALM;
static signed int sIdxLastActiveListAlm = EMPTY_LIST_ALM;
static signed int sSizeActiveListaAlm = 0;

static signed int sIdxCurrentActiveListWrn = EMPTY_LIST_WRN;
static signed int sIdxLastActiveListWrn = EMPTY_LIST_WRN;
static signed int sSizeActiveListaWrn = 0;

static bool sProcessingAlarm = FALSE;
static bool sProcessingWarning = FALSE;

sActiveListAlmS sActiveListAlm[MAX_ALARMS_ACTIVE_LIST_ALM];
sActiveListWrnS sActiveListWrn[MAX_ALARMS_ACTIVE_LIST_WRN];
// Gestione allarmi: aprile 2019 -- end


// FM questa lista devo costruirla mettendo prima i PHYSIC_TRUE e poi i PHYSIC_FALSE,
// ognuno deve poi essere ordinato in base alla priorita' ???
// Quando si aggiorna questa lista bisogna ricordarsi di aggiornare anche la define ALARM_ACTIVE_IN_STRUCT
// che definisce il numero di elementi contenuti in alarmList
typeAlarmS alarmList[] =
{
   //Pressione Arteriosa Elevata
   {CODE_ALARM_PRESS_ART_HIGH,        PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH, 1000, 1000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0, FALSE},

   // Pressione Arteriosa Alta
   {CODE_ALARM_PRESS_ART_SET,         PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH,60000, 1000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_ALLOWED,     &alarmManageNull, 0,  TRUE},

   // allarmi di aria nel TRATTAMENTO
   {CODE_ALARM_AIR_PRES_ART,          PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_SFA_AIR_DET,           PRIORITY_HIGH,  100,    0, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_ALLOWED,     &alarmManageNull, 0, FALSE},
   {CODE_ALARM_AIR_PRES_VEN,          PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_SFV_AIR_DET,           PRIORITY_HIGH,  100,    0, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_ALLOWED,     &alarmManageNull, 0, FALSE},
   {CODE_ALARM_AIR_PRES_FILTER,       PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_SAF_AIR_FILTER,        PRIORITY_HIGH,  100,    0, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_ALLOWED,     &alarmManageNull, 0, FALSE},

   // Pressione Pre-Cartuccia Elevata
   {CODE_ALARM_PRESS_ADS_FILTER_HIGH, PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH, 1000, 1000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0, FALSE},

   // Flusso Perf. Arteriosa Elevato
   {CODE_ALARM_FLOW_PERF_ART_HIGH,    PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH, 2000, 2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0, FALSE},

   // Flusso Perf. Venosa Elevato
   {CODE_ALARM_FLOW_PERF_VEN_HIGH,    PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH, 2000, 2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0, FALSE},

   // Flusso Perf. Venosa Elevato
   {CODE_ALARM_FLOW_OXY_HIGH,		  PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH, 2000, 2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0, FALSE},

   // Flusso non rilevato
   {CODE_ALARM_FLOW_NOT_DETECTED,     PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH, 2000, 2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0, FALSE},

   // Pressione Venosa Elevata
   {CODE_ALARM_PRESS_VEN_HIGH,        PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH, 1000, 1000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0, FALSE},

   // Pressione Venosa Alta
   {CODE_ALARM_PRESS_VEN_SET,         PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH,60000, 1000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_ALLOWED,     &alarmManageNull, 0, TRUE},

   // Pressione Ossigenatore Elevata
   {CODE_ALARM_PRESS_OXYG_INLET,      PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH, 1000, 1000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0, FALSE},

   // Temperatura non rilevata
   {CODE_ALARM_TEMP_SENS_NOT_DETECTED,PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH, 2000, 2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0, FALSE},

   // allarmi di cover aperte
   {CODE_ALARM_PUMP_PERF_COVER_OPEN,  PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH,    0,  100, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0, FALSE},
   {CODE_ALARM_PUMP_PERF_COVER_OPEN_KD,PHYSIC_FALSE,ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH,    0,  100, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0, FALSE},
   {CODE_ALARM_PUMP_PURIF_COVER_OPEN, PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH,    0,  100, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0, FALSE},
   {CODE_ALARM_PUMP_VEN_COVER_OPEN,   PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH,    0,  100, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0, FALSE},
   {CODE_ALARM_PUMP_OXY_COVER_OPEN,   PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH,    0,  100, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0, FALSE},

   // Allarme livello BASSO
   {CODE_ALARM_TANK_LEVEL_LOW,        PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH,30000,30000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0, FALSE},

   // Allarme livello ALTO (NON USATO AL MOMENTO)
   {CODE_ALARM_TANK_LEVEL_HIGH,       PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACT_WAIT_CMD, PRIORITY_HIGH,  500,  500, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0, FALSE},

   // allarme differenza tra flusso venoso misurato e flusso calcolato in base al guadagno pompa troppo alto (NON USATO AL MOMENTO)
   {CODE_ALARM_DELTA_FLOW_ART,        PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH,  3000, 3000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0, FALSE},

   // allarme differenza tra flusso arterioso misurato e flusso calcolato in base al guadagno pompa troppo alto (NON USATO AL MOMENTO)
   {CODE_ALARM_DELTA_FLOW_VEN,        PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH,  3000, 3000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0, FALSE},

   // allarme differenza tra temperatura vaschetta e temperatura fluido arterioso troppo alta
   {CODE_ALARM_DELTA_TEMP_REC_ART,    PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_DELTA_TEMP_HIGH,       PRIORITY_HIGH, 10000, 3000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0, TRUE},

   // allarme differenza tra temperatura vaschetta e temperatura fluido venoso troppo alta
   {CODE_ALARM_DELTA_TEMP_REC_VEN,    PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_DELTA_TEMP_HIGH,       PRIORITY_HIGH, 10000, 3000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0, TRUE},

   // allarme comunicazione canbus
   {CODE_ALARM_CAN_BUS_ERROR,         PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH,  3000, 3000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0, FALSE},

   // allarme pinch non posizionate correttamente (NON USATO AL MOMENTO)
   {CODE_ALARM_BAD_PINCH_POS,         PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_BAD_PINCH_POS,         PRIORITY_HIGH,   500,  500, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0, FALSE},

   // allarmi di aria nel PRIMING
   {CODE_ALARM_PRIM_AIR_PRES_ART,     PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_SFA_PRIM_AIR_DET,      PRIORITY_HIGH,  100,     0, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_ALLOWED,     &alarmManageNull, 0, FALSE},
   {CODE_ALARM_PRIM_AIR_PRES_VEN,     PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_SFA_PRIM_AIR_DET,      PRIORITY_HIGH,  100,     0, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_ALLOWED,     &alarmManageNull, 0, FALSE},
   {CODE_ALARM_PRIM_AIR_PRES_FILTER,  PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_SFA_PRIM_AIR_DET,      PRIORITY_HIGH,  100,     0, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0, FALSE},

   // Pressione filtro basso
   {CODE_ALARM_PRESS_ADS_FILTER_LOW,  PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH, 60000, 2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0,  TRUE},

   // Pressione ossigenatore bassa
   {CODE_ALARM_PRESS_OXYG_LOW,        PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH, 60000, 2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0,  TRUE},

   // Flusso Perf. Arteriosa Alto
   {CODE_ALARM_FLOW_ART_SET,          PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH, 60000, 2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0,  TRUE},

   // Flusso Perf. Venosa Alto
   {CODE_ALARM_FLOW_VEN_SET,          PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH, 60000, 2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0,  TRUE},

   // Flusso Depurazione Alto
   {CODE_ALARM_FLOW_DEP_SET,          PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH, 60000, 2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0,  TRUE},

   //Allarme per errore nella lettura e scrittura modbus. Se dopo 10 ripetizioni non ottengo risposta alla lettura o scrittura genero un allarme.
   //Per questo allarme uso la stessa procedura per le pompe non ferme. (Dovrei tolgliere direttamente l'enable alle pompe.
   {CODE_ALARM_MODBUS_ACTUATOR_SEND,  PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_MOD_BUS_ERROR,         PRIORITY_HIGH,    0,   500, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0, FALSE},

   // allarmi provenienti dalla protective. Serve per fare in modo che quando la protective e' in allarme le pompe vengano fermate e le pinch (NON USATO AL MOMENTO)
   // messe in sicurezza
   {CODE_ALARM_PROT_START_VAL,        PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_PROTECTION, SECURITY_STOP_ALL_ACTUATOR,  PRIORITY_HIGH,    0,   500, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0, FALSE},

   // Filippo - inserito allarme per il tasto di stop
   {CODE_ALARM_EMERGENCY_BUTTON,      PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH, 0, 500, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_ALLOWED, &alarmManageNull, 0, FALSE},

   // Filippo - inserito allarme per test T1
   {CODE_ALARM_TEST_T1,               PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH, 0, 500, OVRD_NOT_ENABLED, RESET_NOT_ALLOWED, SILENCE_ALLOWED, MEMO_ALLOWED, &alarmManageNull, 0, FALSE},

   // Filippo - inserito allarme per test sensore aria fallito (NON USATO AL MOMENTO)
   {CODE_ALARM_AIR_SENSOR_TEST_KO,   PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH,    0, 2000, OVRD_NOT_ENABLED, RESET_NOT_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0, FALSE},

   //Coperchi frontali aperti (NON USATO AL MOMENTO)
   {CODE_ALARM_MACHINE_COVERS,       PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH,    0,   100, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0, FALSE},

   // Distacco Reservoir
   {CODE_ALARM_HOOKS_RESERVOIR,      PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH,    0,   100, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0, FALSE},

   // Resistenza Arteriosa Elevata
   {CODE_ALARM_ART_RES_HIGH,         PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH,    0,   100, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0, FALSE},

   // Elevata Differenza di Temperatura
   {CODE_ALARM_T_ART_OUT_OF_RANGE,   PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_DELTA_TEMP_HIGH,       PRIORITY_HIGH, 1000,     0, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0, FALSE},

   // Temperatura Elevata
   {CODE_ALARM_TEMP_MAX_IN_TRT,      PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_DELTA_TEMP_HIGH,       PRIORITY_HIGH, 1000,     0, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0, TRUE},

   // Temperatura Bassa
   {CODE_ALARM_TEMP_MIN_IN_TRT,      PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_DELTA_TEMP_HIGH,       PRIORITY_HIGH, 1000,     0, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0, TRUE},

   // Sensore Art. non Connesso
   {CODE_ALARM_TUBE_ART_DISCONNECTED,PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH, 10000,    0, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0, TRUE},

   // Sensore Ven. non Connesso
   {CODE_ALARM_TUBE_VEN_DISCONNECTED,PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH, 10000,    0, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0, TRUE},

   // Depurazione Ferma
   {CODE_ALARM_DEP_PUMP_STILL,       PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH, 120000,  500, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0, TRUE},

   // Perfusione Arteriosa Ferma
   {CODE_ALARM_PERF_ART_PUMP_STILL,  PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH, 120000,  500, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0, TRUE},

   // Perfusione Venosa Ferma
   {CODE_ALARM_PERF_VEN_PUMP_STILL,  PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH, 120000,  500, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0, TRUE},

   // Ossigenazione Ferma
   {CODE_ALARM_OXYG_PUMP_STILL,      PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR,     PRIORITY_HIGH, 120000,  500, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0, TRUE},

   // da qui in avanti solo le warning
   {CODE_ALARM_PRESS_ADS_FILTER_WARN, PHYSIC_FALSE, ACTIVE_FALSE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL_ACTUATOR, PRIORITY_LOW,  2000,  2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull, 0, FALSE},
};

void EnableDeltaTHighAlmFunc(void)
{
	GlobalFlags.FlagsDef.EnableTempMaxMin = 1;
	GlobalFlags.FlagsDef.EnableTempArtOORAlm = 1;
	GlobalFlags.FlagsDef.EnableDeltaTempRecVenAlarm = 1;
	GlobalFlags.FlagsDef.EnableDeltaTempRecArtAlarm = 1;

}

void DisableDeltaTHighAlmFunc(void)
{
	GlobalFlags.FlagsDef.EnableTempMaxMin = 0;
	GlobalFlags.FlagsDef.EnableTempArtOORAlm = 0;
	GlobalFlags.FlagsDef.EnableDeltaTempRecVenAlarm = 0;
	GlobalFlags.FlagsDef.EnableDeltaTempRecArtAlarm = 0;
}


void EnableBadPinchPosAlmFunc(void)
{
	GlobalFlags.FlagsDef.EnableBadPinchPosAlm = 1;
}
void EnablePrimAlmSFAAirDetAlmFunc(void)
{
	GlobalFlags.FlagsDef.EnableAlmPrimAirDetection = 1;
}

void DisableBadPinchPosAlmFunc(void)
{
	GlobalFlags.FlagsDef.EnableBadPinchPosAlm = 0;
}
void DisablePrimAlmSFAAirDetAlmFunc(void)
{
	GlobalFlags.FlagsDef.EnableAlmPrimAirDetection = 0;
}

void DisablePrimAirAlarm(bool dis)
{
	if(dis)
	{
		GlobalFlags.FlagsDef.EnableAlmPrimAirDetection = 0;
	}
	else
	{
		GlobalFlags.FlagsDef.EnableAlmPrimAirDetection = 1;
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
}

void EnableFlowAndPressSetAlarmEnableFlags(void)
{
   GlobalFlags.FlagsDef.EnableFlowAndPressSetAlm = 1;    // abilito allarmi di pressione e flusso di SET operatore
}

void DisableFlowAndPressSetAlarmEnableFlags(void)
{
   GlobalFlags.FlagsDef.EnableFlowAndPressSetAlm = 0;    // disabilito allarmi di pressione e flusso di SET operatore
}

void EnableLongPumpStopAlarms(void)
{
	GlobalFlags2.FlagsDef2.EnableOxyPumpStopAlarms = 1; // abilito allarmi pompe ferme a lungo
	GlobalFlags2.FlagsDef2.EnableArtPumpStopAlarms = 1; // abilito allarmi pompe ferme a lungo
	GlobalFlags2.FlagsDef2.EnableDepPumpStopAlarms = 1; // abilito allarmi pompe ferme a lungo
}

void DisableLongPumpStopAlarms(void)
{
   GlobalFlags2.FlagsDef2.EnableOxyPumpStopAlarms = 0; // disabilito allarmi pompe ferme a lungo
   GlobalFlags2.FlagsDef2.EnableArtPumpStopAlarms = 0; // disabilito allarmi pompe ferme a lungo
   GlobalFlags2.FlagsDef2.EnableDepPumpStopAlarms = 0; // disabilito allarmi pompe ferme a lungo
}

void SetFlowHigAlarmEnableFlags(void)
{
   GlobalFlags.FlagsDef.EnableFlowHighAlm = 1;    // abilito allarmi di flusso massimi
}

void DisableFlowHigAlarmEnableFlags(void)
{
   GlobalFlags.FlagsDef.EnableFlowHighAlm = 0;    // disabilito allarmi di flusso massimi
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
	GlobalFlags.FlagsDef.EnableTempMaxMin       = 1;      // abilito allarme temperatura massima e minima
	GlobalFlags.FlagsDef.EnableDeltaFlowArtAlarm = 1;     // abilito allarme delta flusso arterioso troppo alto
	GlobalFlags.FlagsDef.EnableDeltaFlowVenAlarm = 1;     // abilito allarme delta flusso venoso troppo alto
	GlobalFlags.FlagsDef.EnableDeltaTempRecVenAlarm = 1;  // abilito allarme delta temperatura recipiente e line venosa troppo alta
	GlobalFlags.FlagsDef.EnableDeltaTempRecArtAlarm = 1;  // abilito allarme delta temperatura recipiente e line arteriosa troppo alta
	GlobalFlags.FlagsDef.EnableSAFAir = 1;
	GlobalFlags.FlagsDef.EnableSFVAir = 1;
	GlobalFlags.FlagsDef.EnableSFAAir = 1;
	GlobalFlags.FlagsDef.EnableMachineCovers = 1;         // abilito allarmi Hall Machine Covers
	GlobalFlags.FlagsDef.EnableHooksReservoir = 1;        // abilito allarmi Hall ganci contenitore organo
#ifdef ENABLE_PROTECTIVE_BOARD
	GlobalFlags.FlagsDef.EnableCANBUSErr = 1;
#else
	GlobalFlags.FlagsDef.EnableCANBUSErr = 0;
#endif
	GlobalFlags.FlagsDef.EnableBadPinchPosAlm = 0;        // viene attivato in un'altro momento
	GlobalFlags.FlagsDef.EnableAlmPrimAirDetection = 0;   // viene attivato in un'altro momento

	GlobalFlags.FlagsDef.EnableModbusNotRespAlm = 1;      // abilito l'allarme dovuto ad un cattivo funzionamento del modbus
	GlobalFlags.FlagsDef.EnableFromProtectiveAlm = 0;
	// Filippo - abilito l'allarme di stop button
	GlobalFlags.FlagsDef.EnableStopButton=1;
	// Filippo - abilito l'allarme di T1 test
	GlobalFlags.FlagsDef.EnableT1TestAlarm=1;

	GlobalFlags.FlagsDef.EnableArtResAlarm = 1;
	GlobalFlags.FlagsDef.EnableTempArtOORAlm = 1;
	GlobalFlags.FlagsDef.EnableArtPressDisconnect = 1;
	GlobalFlags.FlagsDef.EnableVenPressDisconnect = 1;

	EnableLongPumpStopAlarms();
	SetFlowHigAlarmEnableFlags();
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
		case CODE_ALARM_PUMP_OXY_COVER_OPEN:
		case CODE_ALARM_PUMP_PERF_COVER_OPEN_KD:
			GlobalFlags.FlagsDef.EnableCoversAlarm = 0;    // forzo allarme di cover a off
			break;
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
		case CODE_ALARM_TEMP_MAX_IN_TRT:
		case CODE_ALARM_TEMP_MIN_IN_TRT:
			GlobalFlags.FlagsDef.EnableTempMaxMin = 0;        // forzo allarme temperatura mx o min off
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
		case CODE_ALARM_AIR_PRES_FILTER:
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
		case CODE_ALARM_BAD_PINCH_POS:
			GlobalFlags.FlagsDef.EnableBadPinchPosAlm = 0;
			break;
		case CODE_ALARM_PRIM_AIR_PRES_ART:
		case CODE_ALARM_PRIM_AIR_PRES_VEN:
		case CODE_ALARM_PRIM_AIR_PRES_FILTER:
			GlobalFlags.FlagsDef.EnableAlmPrimAirDetection = 0; // disabilito allarmi aria durante il priming
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
        case CODE_ALARM_ART_RES_HIGH:
            GlobalFlags.FlagsDef.EnableArtResAlarm = 0;     // forzo allarme allarme resistenza arteriosa OFF
            break;
        case CODE_ALARM_T_ART_OUT_OF_RANGE:
            GlobalFlags.FlagsDef.EnableTempArtOORAlm = 0;
            break;
		case CODE_ALARM_PRESS_ART_SET:
			GlobalFlags.FlagsDef.EnablePressSensHighAlm = 0; // forzo allarme pressione e flusso SET a off
			break;
		case CODE_ALARM_PRESS_VEN_SET:
			GlobalFlags.FlagsDef.EnablePressSensHighAlm = 0; // forzo allarme pressione e flusso SET a off
			break;
		case CODE_ALARM_FLOW_ART_SET:
			GlobalFlags.FlagsDef.EnableFlowAndPressSetAlm = 0; // forzo allarme pressione e flusso SET a off
			break;
		case CODE_ALARM_FLOW_VEN_SET:
			GlobalFlags.FlagsDef.EnableFlowAndPressSetAlm = 0; // forzo allarme pressione e flusso SET a off
			break;
		case CODE_ALARM_FLOW_DEP_SET:
			GlobalFlags.FlagsDef.EnableFlowAndPressSetAlm = 0; // forzo allarme pressione e flusso SET a off
			break;
		case CODE_ALARM_DEP_PUMP_STILL:
			GlobalFlags2.FlagsDef2.EnableDepPumpStopAlarms = 0; //Forzo allarme pompa dep a OFF
			break;
		case CODE_ALARM_PERF_ART_PUMP_STILL:
			GlobalFlags2.FlagsDef2.EnableArtPumpStopAlarms = 0; //Forzo allarme pompa art a OFF
			break;
		case CODE_ALARM_OXYG_PUMP_STILL:
			GlobalFlags2.FlagsDef2.EnableOxyPumpStopAlarms = 0; //Forzo allarme pompa oxy a OFF
			break;
		case CODE_ALARM_FLOW_OXY_HIGH:
		case CODE_ALARM_FLOW_PERF_ART_HIGH:
		case CODE_ALARM_FLOW_PERF_VEN_HIGH:
			GlobalFlags.FlagsDef.EnableFlowHighAlm = 0;
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
		case CODE_ALARM_AIR_PRES_FILTER:
			strcpy(s, "AL_AIR_PRES_ADSRB_FILT");
			strcat(s, str);
			DebugStringStr(s);
			break;
		case CODE_ALARM_TEMP_MAX_IN_TRT:
		case CODE_ALARM_TEMP_MIN_IN_TRT:
			strcpy(s, "AL_TEMP_MAX_MIN");
			strcat(s, str);
			DebugStringStr(s);
			break;
		case CODE_ALARM_T_ART_OUT_OF_RANGE:
			strcpy(s, "AL_TEMP_ART_OOR");
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
		case CODE_ALARM_FLOW_NOT_DETECTED:
			strcpy(s, "AL_FLOW_NOT_DETEC");
			strcat(s, str);
			DebugStringStr(s);
			break;
		case CODE_ALARM_PRESS_VEN_HIGH:
			strcpy(s, "AL_PRESS_VEN_HIGH");
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
		case CODE_ALARM_PUMP_OXY_COVER_OPEN:
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
		case CODE_ALARM_BAD_PINCH_POS:
			strcpy(s, "AL_BAD_PINCH_POS");
			strcat(s, str);
			DebugStringStr(s);
			break;
		case CODE_ALARM_PRIM_AIR_PRES_FILTER:
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
        case CODE_ALARM_ART_RES_HIGH:
			strcpy(s, "AL_ART_RESIST");
			strcat(s, str);
			DebugStringStr(s);
            break;
		case CODE_ALARM_PRESS_ART_SET:
			strcpy(s, "AL_PRESS_ART_SET");
			strcat(s, str);
			DebugStringStr(s);
			break;
        case CODE_ALARM_PRESS_VEN_SET:
			strcpy(s, "AL_PRESS_VEN_SET");
			strcat(s, str);
			DebugStringStr(s);
            break;
	}
}

static int StrAlarmWritten = 0;
static int IdxCurrAlarm = 0xff;
int StartAlmArrIdx = 0;
int i_al;

// serve per evitare allarmi residui quando entro in uno stato
// della macchina a stati principale in cui non sono gestiti
void ResetAllUnusedAlmWrn(void)
{
	int i;
	for( i = 0; i < ALARM_ACTIVE_IN_STRUCT; i++)
		alarmList[i].physic = PHYSIC_FALSE;
}


void CalcAlarmActive(void)
{
	static int CalcAlarmActiveOldState = 0;
	if(ptrCurrentState->state != CalcAlarmActiveOldState)
	{
		CalcAlarmActiveOldState = ptrCurrentState->state;
		ResetAllUnusedAlmWrn();
	}

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
		//		manageAlarmStopButtonPressed();

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
			//check sul flusso massimo
			manageAlarmPhysicFlowHigh();

			manageResultT1TestDigital(); //Valutazione finale dei check sui sensori digitali
			manageAlarmFlowSensNotDetected();
			manageAlarmIrTempSensNotDetected();

			//verifica physic pressioni
			manageAlarmPhysicPressSensHigh();
			if(GetTherapyType() == LiverTreat)
				manageAlarmCoversPumpLiver();
			else if(GetTherapyType() == KidneyTreat)
				manageAlarmCoversPumpKidney();

			manageAlarmCanBus();
			manageAlarmActuatorModbusNotRespond();
			manageAlarmActuatorWRModbusNotRespond();
			manageAlarmFromProtective();
			manageCover_Hook_Sensor();
			break;
		}

		case STATE_PRIMING_PH_2:
		{
			//check sul flusso massimo
			manageAlarmPhysicFlowHigh();

			manageAlarmFlowSensNotDetected();
			manageAlarmIrTempSensNotDetected();

			//verifica physic pressioni
			manageAlarmPhysicPressSensHigh();

			if(GetTherapyType() == LiverTreat)
				manageAlarmCoversPumpLiver();
			else if(GetTherapyType() == KidneyTreat)
				manageAlarmCoversPumpKidney();
			manageAlarmCanBus();
			manageAlarmPrimAirPresFilter();
			manageAlarmActuatorModbusNotRespond();
			manageAlarmActuatorWRModbusNotRespond();
			manageAlarmFromProtective();
			manageCover_Hook_Sensor();

			break;
		}

		case STATE_TREATMENT_KIDNEY_1:
		{
			//verifica physic pressioni
			manageAlarmPhysicPressSensHigh();
			manageAlarmPhysicPressSensLow();
			manageAlarmPhysicSetFlowAndPressures();

			//verifica physic flow sensor (presenza aria)
			manageAlarmPhysicUFlowSens();
			manageAlarmSAFAirSens();

			if(GetTherapyType() == LiverTreat)
			    manageAlarmPhysicUFlowSensVen();

			//verifica physic ir temp sens
			manageAlarmPhysicTempSensInTreat();
			manageAlarmPhysicTempSensOOR();

			//verifica physic flusso di perfusione arteriosa e venosa massimi
			manageAlarmPhysicFlowHigh();

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
			/*22/01/19 Vincy: riattivo l'allarme facendo una foto del livello minimo di ADC sul
			 * sensore di livello che sarà Soglia_minima_ADC_allarme_Livello e controllando solo
			 * in trattamento se Gli ADC del sensore di livello vanno sotto tale soglia
			 */

			manageAlarmLiquidLevelLow();

			// i due allarmi che seguono devo essere gestiti attentamente perche' potrei avere delle
			// segnalazioni di allarme anche durante la fase di accelerazione e decelerazione del pid
			// Per ora li commento.
			//get();
			//manageAlarmDeltaFlowVen();
			manageAlarmDeltaTempRecArt();
			manageAlarmDeltaTempRecVen();
			manageAlarmCanBus();
			manageAlarmActuatorModbusNotRespond();
			manageAlarmActuatorWRModbusNotRespond();
			manageAlarmFromProtective();

			manageAlarmAirSensorTestKO();
            manageAlarmArterialResistance();
            manageCover_Hook_Sensor();
			break;
		}

		case STATE_PRIMING_WAIT:
			manageAlarmActuatorModbusNotRespond();
			manageAlarmActuatorWRModbusNotRespond();
			break;

		case STATE_PRIMING_RICIRCOLO:
			//verifica physic pressioni
			manageAlarmPhysicPressSensHigh();
			manageAlarmPhysicPressSensLow();

			//check sul flusso massimo
			manageAlarmPhysicFlowHigh();

			//verifica physic ir temp sens
			if(GetTherapyType() == LiverTreat)
				manageAlarmCoversPumpLiver();
			else if(GetTherapyType() == KidneyTreat)
				manageAlarmCoversPumpKidney();
			manageAlarmCanBus();
			manageAlarmBadPinchPos();   // allarme di pinch posizionate correttamente
			manageAlarmPrimAirPresFilter();
			manageAlarmPrimAirPresArt();
			manageAlarmPrimAirPresVen();
			manageAlarmActuatorModbusNotRespond();
			manageAlarmActuatorWRModbusNotRespond();
			manageAlarmFromProtective();
			manageCover_Hook_Sensor();
			manageCheckConnectionTubeArtSensPress();

			if (GetTherapyType() == LiverTreat)
				manageCheckConnectionTubeVenSensPress();

			break;

		case STATE_WAIT_TREATMENT:

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
			//check sul flusso massimo
			manageAlarmPhysicFlowHigh();

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
			manageCover_Hook_Sensor();
			break;
		}

		case STATE_EMPTY_DISPOSABLE_2:
		{
			break;
		}

		case STATE_UNMOUNT_DISPOSABLE:
			// per il momento, in questo stato non sono previsti allarmi.
			// In questo stato sono azionate solo le pinch per smontare
			// il disposable
			break;

		case STATE_WASHING:
		{
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


//------------------------FUNZIONI PER DETERMINARE LA CONDIZIONE DI ALLARME----------------------------------

// Allarme generato dalla condizione di aria nel filtro durante il priming. Viene preso in considerazione
// dal momento in cui il filtro viene installato alla fine del priming
void manageAlarmPrimAirPresFilter(void)
{
	if(!GlobalFlags.FlagsDef.EnableAlmPrimAirDetection)
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

// Allarme generato dalla condizione di aria in linea arteriosa durante il priming. Viene preso in considerazione
// dal momento in cui il filtro viene installato alla fine del priming
void manageAlarmPrimAirPresArt(void)
{
	if(!GlobalFlags.FlagsDef.EnableAlmPrimAirDetection)
	{
		alarmList[PRIM_AIR_ON_ART].physic = PHYSIC_FALSE;
		return;
	}
	else
	{
		if((sensor_UFLOW[ARTERIOUS_AIR_SENSOR].bubbleSize >= MAX_BUBBLE_SIZE) ||
		   (sensor_UFLOW[ARTERIOUS_AIR_SENSOR].bubblePresence == MASK_ERROR_BUBBLE_ALARM))
		{
			alarmList[PRIM_AIR_ON_ART].physic = PHYSIC_TRUE;
		}
		else
		{
			alarmList[PRIM_AIR_ON_ART].physic = PHYSIC_FALSE;
		}
	}
}

// Allarme generato dalla condizione di aria in linea venosa durante il priming. Viene preso in considerazione
// dal momento in cui il filtro viene installato alla fine del priming
void manageAlarmPrimAirPresVen(void)
{
	if((!GlobalFlags.FlagsDef.EnableAlmPrimAirDetection) || (GetTherapyType() == KidneyTreat))
	{
		alarmList[PRIM_AIR_ON_VEN].physic = PHYSIC_FALSE;
		return;
	}
	else
	{
		if((sensor_UFLOW[VENOUS_AIR_SENSOR].bubbleSize >= MAX_BUBBLE_SIZE) ||
		   (sensor_UFLOW[VENOUS_AIR_SENSOR].bubblePresence == MASK_ERROR_BUBBLE_ALARM))
		{
			alarmList[PRIM_AIR_ON_VEN].physic = PHYSIC_TRUE;
		}
		else
		{
			alarmList[PRIM_AIR_ON_VEN].physic = PHYSIC_FALSE;
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


void manageAlarmCanBus(void)
{
	if (!GlobalFlags.FlagsDef.EnableCANBUSErr)
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
	if (GlobalFlags.FlagsDef.EnableDeltaFlowArtAlarm)
	{
		/*TODO Vincenzo: credo che il controllo vcada fatto anche, anzi soprattutto, se il flusso letto è+ zero (esempio tubo tolto)*/
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
	if (GlobalFlags.FlagsDef.EnableDeltaFlowVenAlarm)
	{
		/*TODO Vincenzo: credo che il controllo vcada fatto anche, anzi soprattutto, se il flusso letto è+ zero (esempio tubo tolto)*/
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
#if 0
	if (GlobalFlags.FlagsDef.EnableDeltaTempRecArtAlarm)
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
#endif
		alarmList[DELTA_TEMP_REC_ART].physic = PHYSIC_FALSE;
}

// controllo se il delta di temperatura tra recipiente e liquido venoso e' troppo alta
void manageAlarmDeltaTempRecVen(void)
{
#if 0
	if (GlobalFlags.FlagsDef.EnableDeltaTempRecVenAlarm)
	{
		float trec = sensorIR_TM[1].tempSensValue;
		float tven = sensorIR_TM[2].tempSensValue;

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
#endif
		alarmList[DELTA_TEMP_REC_VEN].physic = PHYSIC_FALSE;
}


// controlla se sono al di sotto del livello minimo
void manageAlarmLiquidLevelLow(void)
{
#if 0
	/*controllo l'alamre solo se ho calcolato la soglia Soglia_minima_ADC_allarme_Livello*/
	if (GlobalFlags.FlagsDef.EnableLevLowAlarm && TARA_PRESS_DONE)
	{
		//if(LiquidAmount <= MIN_LIQUID_LEV_IN_PERC)
		if (PR_LEVEL_ADC_Filtered <= Soglia_minima_ADC_allarme_Livello)
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
#endif
}


// controlla se supera il livello massimo
void manageAlarmLiquidLevelHigh(void)
{
#if 0
	if (GlobalFlags.FlagsDef.EnableLevHighAlarm)
	{
		if (LiquidAmount >= MAX_LIQUID_LEV_IN_PERC)
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
#endif
}


// cover della pompa di depurazione nel caso di fegato o della
// pompa arteriosa in kidney
void manageAlarmCoversPumpLiver(void)
{
	if (GlobalFlags.FlagsDef.EnableCoversAlarm)
	{
		if (coverStateGlobal & 0x01)
			alarmList[PURIF_COVER_OPEN].physic = PHYSIC_TRUE;
		else
			alarmList[PURIF_COVER_OPEN].physic = PHYSIC_FALSE;

		if (coverStateGlobal & 0x02)
			alarmList[PERF_COVER_OPEN].physic = PHYSIC_TRUE;
		else
			alarmList[PERF_COVER_OPEN].physic = PHYSIC_FALSE;

		if (coverStateGlobal & 0x04 || coverStateGlobal & 0x08)
			alarmList[VEN_COVER_OPEN].physic = PHYSIC_TRUE;
		else
			alarmList[VEN_COVER_OPEN].physic = PHYSIC_FALSE;
	}
	else
	{
		alarmList[PURIF_COVER_OPEN].physic = PHYSIC_FALSE;
		alarmList[PERF_COVER_OPEN].physic = PHYSIC_FALSE;
		alarmList[VEN_COVER_OPEN].physic = PHYSIC_FALSE;
	}
}

// cover della pompa arteriosa  nel fegato (non usata in kidney)
void manageAlarmCoversPumpKidney(void)
{
	if (GlobalFlags.FlagsDef.EnableCoversAlarm)
	{
		if (coverStateGlobal & 0x01)
			alarmList[PERF_COVER_OPEN_KD].physic = PHYSIC_TRUE;
		else
			alarmList[PERF_COVER_OPEN_KD].physic = PHYSIC_FALSE;

		if (coverStateGlobal & 0x04 || coverStateGlobal & 0x08)
			alarmList[OXY_COVER_OPEN].physic = PHYSIC_TRUE;
		else
			alarmList[OXY_COVER_OPEN].physic = PHYSIC_FALSE;
	}
	else
	{
		alarmList[PERF_COVER_OPEN_KD].physic = PHYSIC_FALSE;
		alarmList[OXY_COVER_OPEN].physic = PHYSIC_FALSE;
	}

}

void manageAlarmPhysicPressSensLow(void)
{
	if (GlobalFlags.FlagsDef.EnablePressSensLowAlm)
	{
		/*abilito l'allarme di pressione filtro bassa solo se la pompa filtro si sta muovendo a velocità superiore a 5 RPM*/
		if (PR_ADS_FLT_mmHg_Filtered < PR_ADS_FILTER_LOW && modbusData[3][17] > 500)
		{
			alarmList[PRESS_ADS_FILTER_LOW].physic = PHYSIC_TRUE;
		}
		else
		{
			alarmList[PRESS_ADS_FILTER_LOW].physic = PHYSIC_FALSE;
		}

		/*abilito l'allarme di pressione OXY bassa solo se almeno una delle due pompe OXY si sta muovendo  a velocità superiore a 5 RPM*/
		if (PR_OXYG_mmHg_Filtered < PR_OXYG_LOW && (modbusData[1][17] > 500 || modbusData[2][17] > 500))
		{
			alarmList[PRESS_OXYG_LOW].physic = PHYSIC_TRUE;
		}
		else
		{
			alarmList[PRESS_OXYG_LOW].physic = PHYSIC_FALSE;
		}
	}
	else
	{
		alarmList[PR_ADS_FILTER_LOW].physic = PHYSIC_FALSE;
		alarmList[PR_OXYG_LOW].physic = PHYSIC_FALSE;
	}
}

void manageAlarmPhysicPressSensHigh(void)
{
	word MaxPressArt = 0;
	word MaxPressVen = 0;

	if (GetTherapyType() == LiverTreat)
	{
        // PRIMING LIVER
        if ((ptrCurrentState->state == STATE_PRIMING_PH_1) || (ptrCurrentState->state == STATE_PRIMING_PH_2) ||
            (ptrCurrentState->state == STATE_PRIMING_RICIRCOLO))
		{
			MaxPressArt = PR_ART_HIGH_PRIMING_LIVER;
			MaxPressVen = PR_VEN_HIGH_PRIMING_LIVER;
		}
		else
		{
			//TRATTAMENTO LIVER
			MaxPressArt = PR_ART_HIGH_TREATMENT_LIVER;
			MaxPressVen = PR_VEN_HIGH_TREATMENT_LIVER;
		}
	}
	else //KidneyTreat
	{
		if ((ptrCurrentState->state == STATE_PRIMING_PH_1) || (ptrCurrentState->state == STATE_PRIMING_PH_2) ||
	       (ptrCurrentState->state == STATE_PRIMING_RICIRCOLO))
		{
			// PRIMING KIDNEY
			MaxPressArt = PR_ART_HIGH_PRIMING_KIDNEY;
		}
		else
		{
			// TRATTAMENTO KIDNEY
			MaxPressArt = PR_ART_HIGH_TREATMENT_KIDNEY;
		}
	}

	if (GlobalFlags.FlagsDef.EnablePressSensHighAlm)
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
		if ((PR_VEN_Sistolyc_mmHg /*PR_VEN_mmHg_Filtered*/ > MaxPressVen) && (GetTherapyType() == LiverTreat))
		{
			/*aggiungo controllo che non fa alzare allarem di sovrapressione
			 * venosa in trattamento se lo stesso non è ancora partito
			 * se è partito, vuol dire che ho fatto la tara sulle presisoni*/
			if ( (MaxPressVen > PR_VEN_HIGH_TREATMENT_LIVER) || ( (MaxPressVen == PR_VEN_HIGH_TREATMENT_LIVER) && TARA_PRESS_DONE == TRUE) )
				alarmList[PRESS_VEN_HIGH].physic = PHYSIC_TRUE;

		}
		else
		{
			alarmList[PRESS_VEN_HIGH].physic = PHYSIC_FALSE;
		}

		if (PR_OXYG_mmHg_Filtered > PR_OXYG_HIGH)
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

// Allarmi di Pressione e Flusso di SET
void manageAlarmPhysicSetFlowAndPressures(void)
{
	word pressureTargetArt = parameterWordSetFromGUI[PAR_SET_PRESS_ART_TARGET].value;
	word pressureTargetVen = parameterWordSetFromGUI[PAR_SET_VENOUS_PRESS_TARGET].value;

	word flowTargetArt = parameterWordSetFromGUI[PAR_SET_MAX_FLOW_PERFUSION].value;
	word flowTargetDep = parameterWordSetFromGUI[PAR_SET_PURIF_FLOW_TARGET].value;
	word flowTargetVen = parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value;

	/*vado a leggere la posizione della pinch così come è stata inviata dal driver*/
    word Pinch_Filt_Position = modbusData[4][0];
	word Pinch_Art_Position  = modbusData[5][0];
	word Pinch_Ven_Position  = modbusData[6][0];

	//Abilitazione allarmi di SET
	if (GlobalFlags.FlagsDef.EnableFlowAndPressSetAlm)
	{
		//Liver, considero anche la pressione venosa
		if (GetTherapyType() == LiverTreat)
		{
			// Allarme di SET pressione venosa -- Vincenzo: lo attivo solo se la pinch venosa è aperta sull'organo
			if ((PR_VEN_mmHg_Filtered > pressureTargetVen + DELTA_TARGET_PRESS_VEN_LIVER) && (Pinch_Ven_Position == MODBUS_PINCH_LEFT_OPEN))
				alarmList[PRESS_VEN_SET].physic = PHYSIC_TRUE;
			else
				alarmList[PRESS_VEN_SET].physic = PHYSIC_FALSE;

			// Allarme di SET pressione arteriosa -- Vincenzo: lo attivo solo se la pinch arteriosa è aperta sull'organo
			if ((PR_ART_mmHg_Filtered > pressureTargetArt + DELTA_TARGET_PRESS_ART_LIVER) && (Pinch_Art_Position == MODBUS_PINCH_LEFT_OPEN))
				alarmList[PRESS_ART_SET].physic = PHYSIC_TRUE;
			else
				alarmList[PRESS_ART_SET].physic = PHYSIC_FALSE;

			// Allarme di SET flusso arterioso -- Vincenzo: lo attivo solo se la pinch arteriosa è aperta sull'organo
			if ((sensor_UFLOW[0].Average_Flow_Val  > flowTargetArt + DELTA_TARGET_FLOW_ART_LIVER) && (Pinch_Art_Position == MODBUS_PINCH_LEFT_OPEN)) //50
				alarmList[FLOW_ART_SET].physic = PHYSIC_TRUE;
			else
				alarmList[FLOW_ART_SET].physic = PHYSIC_FALSE;

			// Allarme di SET flusso venoso -- Vincenzo: lo attivo solo se la pinch venosa è aperta sull'organo
			if ((sensor_UFLOW[1].Average_Flow_Val  > flowTargetVen + DELTA_TARGET_FLOW_VEN_LIVER) && (Pinch_Ven_Position == MODBUS_PINCH_LEFT_OPEN)) //solo fegato
				alarmList[FLOW_VEN_SET].physic = PHYSIC_TRUE;
			else
				alarmList[FLOW_VEN_SET].physic = PHYSIC_FALSE;

			// Allarme di SET flusso depurazione -- Vincenzo: lo attivo solo se la pinch filtro è aperta sul filtro
			if ((FilterFlowVal > flowTargetDep + DELTA_TARGET_FLOW_DEP_LIVER) && (Pinch_Filt_Position == MODBUS_PINCH_RIGHT_OPEN)) //solo fegato
				alarmList[FLOW_DEP_SET].physic = PHYSIC_TRUE;
			else
				alarmList[FLOW_DEP_SET].physic = PHYSIC_FALSE;
		}
		else //Kidney, solo arteriosa
		{
			// Allarme di SET pressione arteriosa -- Vincenzo: lo attivo solo se la pinch arteriosa è aperta sull'organo
			if ((PR_ART_mmHg_Filtered > pressureTargetArt + DELTA_TARGET_PRESS_ART_KIDNEY) && (Pinch_Art_Position == MODBUS_PINCH_LEFT_OPEN))
				alarmList[PRESS_ART_SET].physic = PHYSIC_TRUE;
			else
				alarmList[PRESS_ART_SET].physic = PHYSIC_FALSE;

			// Allarme di SET flusso arterioso -- Vincenzo: lo attivo solo se la pinch arteriosa è aperta sull'organo
			if ((sensor_UFLOW[0].Average_Flow_Val  > flowTargetArt + DELTA_TARGET_FLOW_ART_KIDNEY) && (Pinch_Art_Position == MODBUS_PINCH_LEFT_OPEN))
				alarmList[FLOW_ART_SET].physic = PHYSIC_TRUE;
			else
				alarmList[FLOW_ART_SET].physic = PHYSIC_FALSE;
		}
	}
	else
	{
		alarmList[PRESS_ART_SET].physic = PHYSIC_FALSE;
		alarmList[PRESS_VEN_SET].physic = PHYSIC_FALSE;
		alarmList[FLOW_ART_SET].physic = PHYSIC_FALSE;
		alarmList[FLOW_VEN_SET].physic = PHYSIC_FALSE;
		alarmList[FLOW_DEP_SET].physic = PHYSIC_FALSE;
	}
}

//Hook sensori reservoir
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

/*funzione che durante il ricircolo del priming
 * intercetta una mancata connessione del disposible
 * al sensore di pressione Arteriosa della macchina*/
void manageCheckConnectionTubeArtSensPress(void)
{
	if (GlobalFlags.FlagsDef.EnableArtPressDisconnect)
	{
		/*attivo l'allarme se la pompa arteriosa sta girando e la pressione sta sotto 10 mmHg*/
		if (PR_ART_mmHg_Filtered < 10 && modbusData[0][17] != 0)
		{
			alarmList[TUBE_ART_DISCONNECTED].physic = PHYSIC_TRUE;
		}
		else
		{
			alarmList[TUBE_ART_DISCONNECTED].physic = PHYSIC_FALSE;
		}
	}
}

/*funzione che durante il ricircolo del priming
 * intercetta una mancata connessione del disposible
 * al sensore di pressione Venosa della macchina*/
void manageCheckConnectionTubeVenSensPress(void)
{
	if (GlobalFlags.FlagsDef.EnableVenPressDisconnect)
	{
		/*attivo l'allarme se la pompa arteriosa sta girando e la pressione sta sotto 10 mmHg*/
		if (PR_VEN_mmHg_Filtered < 10 && modbusData[2][17] != 0 && modbusData[3][17] != 0)
		{
			alarmList[TUBE_VEN_DISCONNECTED].physic = PHYSIC_TRUE;
		}
		else
		{
			alarmList[TUBE_VEN_DISCONNECTED].physic = PHYSIC_FALSE;
		}
	}
}
//------------------------------------------------------------------------------------------
ALARM_STATE checkAlmPhysicTempState;
ALARM_STATE checkAlmPhysicTempOORState;

void SetAllAlarmsEnableFlags(void)
{
	checkAlmPhysicTempState = ALM_INIT;
	AlmGlobalFlags.AlmFlagsDef.EnableTArtMaxVal = 1;

	checkAlmPhysicTempOORState = ALM_INIT;
}

void InitAlarmsStates(void)
{
	AlmGlobalFlags.AlmFlagsVal = 0;
	SetAllAlarmsEnableFlags();
}

void checkAlmPhysicTempOOR(void)
{
	static uint32_t StartTime50Msec;
	float TargetT;
	float deltaT;
	static float CurrMaxDeltaT = 0.0;

	TargetT = (float)parameterWordSetFromGUI[PAR_SET_PRIMING_TEMPERATURE_PERFUSION].value / 10;  // (gradi Centigradi * 10)
	deltaT = TargetT - sensorIR_TM[0].tempSensValue;  //Differenza sensore arterioso e target
	if(deltaT < 0)
		deltaT = -deltaT;
	switch (checkAlmPhysicTempOORState)
	{
		case ALM_INIT:
			alarmList[TEMP_ART_OOR].physic = PHYSIC_FALSE;
			CurrMaxDeltaT = (float)MAX_DELTA_T_ART;
			checkAlmPhysicTempOORState = ALM_CONTROL;
			break;
		case ALM_CONTROL:
			alarmList[TEMP_ART_OOR].physic = PHYSIC_FALSE;
			if (deltaT > (float)CurrMaxDeltaT)
			{
				StartTime50Msec = timerCounterModBus;
				checkAlmPhysicTempOORState = ALM_CONTROL_DELAY;
			}
			break;
		case ALM_CONTROL_DELAY:
			alarmList[TEMP_ART_OOR].physic = PHYSIC_FALSE;
			if(deltaT > CurrMaxDeltaT)
			{
				if((msTick_elapsed(StartTime50Msec) * 50L) >= MAX_TEMP_ALM_TIME_DET_IN_TREAT)
				{
					alarmList[TEMP_ART_OOR].physic = PHYSIC_TRUE;
					CurrMaxDeltaT = (float)DELTA_T_ART_IF_OK;
					checkAlmPhysicTempOORState = ALM_CONTROL_NEW_LEVEL;
				}
			}
			else
			{
				// la temperatura e' ritornata al di sotto della massima e minima
				checkAlmPhysicTempOORState = ALM_CONTROL;
			}
			break;
		case ALM_CONTROL_NEW_LEVEL:
			// controllo che la temperatura rientri in un range ridotto altrimenti genero un allarme
			alarmList[TEMP_ART_OOR].physic = PHYSIC_TRUE;
			if(deltaT < CurrMaxDeltaT)
			{
				// sono ritornato nelle vicinanze del target
				alarmList[TEMP_ART_OOR].physic = PHYSIC_FALSE;
				checkAlmPhysicTempOORState = ALM_INIT;
			}
			break;
	}
}

//-----------------------------------------------------------------------------------------

// usata durante la fase di trattamento genera un allarme che cerchera' di ripristinare
// la temperatura target
void manageAlarmPhysicTempSensInTreat(void)
{
	float tempSTF = 0.0;
	float tempSTA = 0.0;
	float tempSTV = 0.0;

	tempSTF = sensorIR_TM[0].tempSensValue;
	tempSTA = sensorIR_TM[1].tempSensValue;
	tempSTV = sensorIR_TM[2].tempSensValue;

	if(GlobalFlags.FlagsDef.EnableTempMaxMin)
	{
		if((tempSTF > (float)MAX_TEMPERATURE_VALUE_IN_TREAT) ||
           (tempSTA > (float)MAX_TEMPERATURE_VALUE_IN_TREAT) ||
           (tempSTV > (float)MAX_TEMPERATURE_VALUE_IN_TREAT))
		{
			alarmList[TEMP_MAX_IN_TREAT].physic = PHYSIC_TRUE;
		}
		else
		{
			alarmList[TEMP_MAX_IN_TREAT].physic = PHYSIC_FALSE;
		}

		if((tempSTF < (float)MIN_TEMPERATURE_VALUE_IN_TREAT) ||
           (tempSTA < (float)MIN_TEMPERATURE_VALUE_IN_TREAT) ||
           (tempSTV < (float)MIN_TEMPERATURE_VALUE_IN_TREAT))
		{
			alarmList[TEMP_MIN_IN_TREAT].physic = PHYSIC_TRUE;
		}
		else
		{
			alarmList[TEMP_MIN_IN_TREAT].physic = PHYSIC_FALSE;
		}
	}
	else
	{
		alarmList[TEMP_MAX_IN_TREAT].physic = PHYSIC_FALSE;
		alarmList[TEMP_MIN_IN_TREAT].physic = PHYSIC_FALSE;
	}

}

// allarme di temperatura fuori range di almeno due gradi
void manageAlarmPhysicTempSensOOR(void)
{
	bool pompeInMovimento = FALSE;

	if (GetTherapyType() == LiverTreat)
	{
		pompeInMovimento = (modbusData[1][17] > 500) ? TRUE : FALSE;	//PPAF in movimento, rpm*100
	}
	else if (GetTherapyType() == KidneyTreat)
	{
		pompeInMovimento = (modbusData[0][17] > 500) ? TRUE : FALSE;	//PPAR in movimento, rpm*100
	}

	if (GlobalFlags.FlagsDef.EnableTempArtOORAlm && pompeInMovimento)
	{
		checkAlmPhysicTempOOR();
	}
	else
	{
		alarmList[TEMP_ART_OOR].physic = PHYSIC_FALSE;
		checkAlmPhysicTempOORState = ALM_INIT;
	}
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
//		if((sensor_UFLOW[VENOUS_AIR_SENSOR].bubbleSize >= MAX_BUBBLE_SIZE) ||
//			(sensor_UFLOW[VENOUS_AIR_SENSOR].bubblePresence == MASK_ERROR_BUBBLE_ALARM))
		/*TODO Vincenzo: con sensor_UFLOW[VENOUS_AIR_SENSOR].bubbleSize == 255 secondo me dovremmo comunque dare errore*/
		if(((sensor_UFLOW[VENOUS_AIR_SENSOR].bubbleSize >= MAX_BUBBLE_SIZE) ||
			(sensor_UFLOW[VENOUS_AIR_SENSOR].bubblePresence == MASK_ERROR_BUBBLE_ALARM)) /*&&
			(sensor_UFLOW[VENOUS_AIR_SENSOR].bubbleSize != 255)*/)
		{
			alarmList[AIR_PRES_VEN].physic = PHYSIC_TRUE;
		}
		else
		{
			alarmList[AIR_PRES_VEN].physic = PHYSIC_FALSE;
		}
	}
}


void manageAlarmPhysicUFlowSens(void)
{
	if(!GlobalFlags.FlagsDef.EnableSFAAir)
	{
		alarmList[AIR_PRES_ART].physic = PHYSIC_FALSE;
		return;
	}
	else
	{
//		if(
//			(sensor_UFLOW[ARTERIOUS_AIR_SENSOR].bubbleSize >= MAX_BUBBLE_SIZE) ||
//			(sensor_UFLOW[ARTERIOUS_AIR_SENSOR].bubblePresence == MASK_ERROR_BUBBLE_ALARM)
//			)
		/*TODO Vincenzo: con sensor_UFLOW[VENOUS_AIR_SENSOR].bubbleSize == 255 secondo me dovremmo comunque dare errore*/
		if(
			((sensor_UFLOW[ARTERIOUS_AIR_SENSOR].bubbleSize >= MAX_BUBBLE_SIZE) ||
			(sensor_UFLOW[ARTERIOUS_AIR_SENSOR].bubblePresence == MASK_ERROR_BUBBLE_ALARM))/* &&
			(sensor_UFLOW[ARTERIOUS_AIR_SENSOR].bubbleSize != 255)*/
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


void manageAlarmArterialResistance(void)
{
	static uint8_t manageAlarmArtResState = 0;
	static uint32_t ArtResStartTime = 0;

	if(GlobalFlags.FlagsDef.EnableArtResAlarm && (GetTherapyType() == KidneyTreat))
	{
		switch (manageAlarmArtResState)
		{
			case 0:
				alarmList[ARTERIAL_RESIST_HIGH].physic = PHYSIC_FALSE;
				if(perfusionParam.renalResistance >= RENAL_RESIST_HIGH_LEVEL)
				{
					ArtResStartTime = timerCounterModBus;
					manageAlarmArtResState = 1;
				}
				break;
			case 1:
				// monitor tempo a livello alto
				alarmList[ARTERIAL_RESIST_HIGH].physic = PHYSIC_FALSE;
				if(perfusionParam.renalResistance < RENAL_RESIST_HIGH_LEVEL)
				{
					manageAlarmArtResState = 0;
					break;
				}
				if((msTick_elapsed(ArtResStartTime) * 50L) >= ART_RES_HIGH_TOUT_MSEC)
				{
					manageAlarmArtResState = 2;
					ArtResStartTime = timerCounterModBus;
				}
				break;
			case 2:
				// allarme attivo
				alarmList[ARTERIAL_RESIST_HIGH].physic = PHYSIC_TRUE;
				if((msTick_elapsed(ArtResStartTime) * 50L) >= ART_RES_ALM_ON_MSEC)
				{
					// una volta scattato l'allarme posso tornare al monitoraggio
					// del parametro
					manageAlarmArtResState = 0;
				}
				break;
		}
	}
	else
	{
		alarmList[ARTERIAL_RESIST_HIGH].physic = PHYSIC_FALSE;
		manageAlarmArtResState = 0;
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

// Gestione allarmi di flusso arterioso e venoso massimi
void manageAlarmPhysicFlowHigh(void)
{
	int flowMaxArt = 0;
	int flowMaxVen = 0;

	if (GlobalFlags.FlagsDef.EnableFlowHighAlm)
	{
		switch (GetTherapyType())
		{
			case LiverTreat:
				flowMaxArt = FLOW_PERF_ART_LIVER_MAX;
				flowMaxVen = FLOW_PERF_VEN_LIVER_MAX;
				break;

			case KidneyTreat:
			default:
				flowMaxArt = FLOW_PERF_ART_KIDNEY_MAX;
				flowMaxVen = FLOW_PERF_OXY_KIDNEY_MAX;
				break;
		}
		// UF Sensor 0 è il sensore di perfusione arteriosa
		if (sensor_UFLOW[0].Average_Flow_Val > flowMaxArt)
		{
			alarmList[FLOW_PERF_ART_HIGH].physic = PHYSIC_TRUE;
		}
		else
		{
			alarmList[FLOW_PERF_ART_HIGH].physic = PHYSIC_FALSE;
		}

		if (LiverTreat == GetTherapyType())
		{
			// UF Sensor 1 è il sensore di perfusione venosa / oxy
			if (sensor_UFLOW[1].Average_Flow_Val > flowMaxVen)
			{
				alarmList[FLOW_PERF_VEN_HIGH].physic = PHYSIC_TRUE;
			}
			else
			{
				alarmList[FLOW_PERF_VEN_HIGH].physic = PHYSIC_FALSE;
			}
			alarmList[FLOW_OXY_HIGH].physic = PHYSIC_FALSE;

		}
		else if (KidneyTreat == GetTherapyType())
		{
			// UF Sensor 1 è il sensore di perfusione venosa / oxy
			if (sensor_UFLOW[1].Average_Flow_Val > flowMaxVen)
			{
				alarmList[FLOW_OXY_HIGH].physic = PHYSIC_TRUE;
			}
			else
			{
				alarmList[FLOW_OXY_HIGH].physic = PHYSIC_FALSE;
			}
			alarmList[FLOW_PERF_VEN_HIGH].physic = PHYSIC_FALSE;
		}
	}
	else
	{
		alarmList[FLOW_PERF_ART_HIGH].physic = PHYSIC_FALSE;
		alarmList[FLOW_PERF_VEN_HIGH].physic = PHYSIC_FALSE;
		alarmList[FLOW_OXY_HIGH].physic = PHYSIC_FALSE;
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

//Allarme T1 test relativo ai segnali digitali
//Cover pompe 1-2-3-4, coperchi frontali sx e dx, segnali gancio sx e dx
void manageResultT1TestDigital(void)
{
    #ifdef T1_TEST_ENABLED
		//Quando questa funzione viene chiamata, si controlla che il risultato del test sia positivo.
		//In caso contrario viene generato un allarme T1 Test
		if (gDigitalTest)
			alarmList[ALARM_T1_TEST].physic = PHYSIC_FALSE; //Check OK
		else
			alarmList[ALARM_T1_TEST].physic = PHYSIC_TRUE; //Almeno un check sui segnali digitali non è passato
    #endif
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




void manageAlarmChildGuard(typeAlarmS * ptrAlarm){
	typeAlarmS * myAlarmPointer;

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

	case SECURITY_DELTA_TEMP_HIGH:
		if(myAlarmPointer->active == ACTIVE_TRUE)
			currentGuard[GUARD_ALARM_DELTA_TEMP_HIGH].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		else
			currentGuard[GUARD_ALARM_DELTA_TEMP_HIGH].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
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
	extern unsigned long int gCounterTimerBuzzer;
	switch (level)
	{
		case SILENT:
			BUZZER_LOW_C_ClrVal(); 		//disattiva il buzzer low
			BUZZER_MEDIUM_C_ClrVal();	//disattiva il buzzer Medium
			BUZZER_HIGH_C_ClrVal();		//disattiva il buzzer HIGH
			gCounterTimerBuzzer = 0;
			break;

		case LOW:
			BUZZER_LOW_C_SetVal(); 		//attiva il buzzer low
			break;

		case MEDIUM:
			BUZZER_MEDIUM_C_SetVal();	//attiva il buzzer Medium
			break;

		case HIGH:
		{
			if (gCounterTimerBuzzer < 2)
			{
				; //Silenzio per 100 msec
			}
			else if (gCounterTimerBuzzer >= 2 && gCounterTimerBuzzer < 5)
			{
				BUZZER_LOW_C_SetVal(); 		//attiva il buzzer low
			}
			else
			{
				BUZZER_HIGH_C_SetVal();		//attiva il buzzer High (insieme a low, già attivo)
			}
		}
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

	if(((sensor_UFLOW[VENOUS_AIR_SENSOR].bubbleSize >= MAX_BUBBLE_SIZE) ||
			(sensor_UFLOW[VENOUS_AIR_SENSOR].bubblePresence == MASK_ERROR_BUBBLE_ALARM)) &&
			/*(sensor_UFLOW[VENOUS_AIR_SENSOR].bubbleSize != 255) &&*/
		(ChildEmptyFlags.FlagsDef.SFVAirDetected == 0))
	{
		ChildEmptyFlags.FlagsDef.SFVAirDetected = 1;
		DebugStringStr("AL_AIR_PRES_VEN");
	}

	if(((sensor_UFLOW[ARTERIOUS_AIR_SENSOR].bubbleSize >= MAX_BUBBLE_SIZE) ||
		(sensor_UFLOW[ARTERIOUS_AIR_SENSOR].bubblePresence == MASK_ERROR_BUBBLE_ALARM)) &&
		/*(sensor_UFLOW[ARTERIOUS_AIR_SENSOR].bubbleSize != 255) &&*/
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
	memset(&alarmCurrent, 0, sizeof(typeAlarmS));
	LevelBuzzer = SILENT;// 0;
	//EnableNextAlarm = 1;
	EnableNextAlarmFunc();
	currentGuard[GUARD_ALARM_ACTIVE].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
	currentGuard[GUARD_ALARM_ACTIVE].guardValue = GUARD_VALUE_FALSE;
}


//-----------------------------------------------------------------------------------------------
//----------------------------GESTIONE DELLE WARNING---------------------------------------------

WARNING_STATE WrnPressSensHighState;
WARNING_STATE WrnDepPumpStillState;
WARNING_STATE WrnPerfArtPumpStillState;
WARNING_STATE WrnOxygPumpStillState;

void SetAllWarningEnableFlags(void)
{
	WrnPressSensHighState = WRN_INIT;
	WrnDepPumpStillState = WRN_INIT;
	WrnPerfArtPumpStillState = WRN_INIT;
	WrnOxygPumpStillState = WRN_INIT;

	WrnGlobalFlags.WrnFlagsDef.EnableAdsPressHigh = 1;
	WrnGlobalFlags.WrnFlagsDef.EnablePumpDepStill = 1;
	WrnGlobalFlags.WrnFlagsDef.EnablePumpPerfArtStill = 1;
	WrnGlobalFlags.WrnFlagsDef.EnablePumpOxygStill = 1;

	WrnGlobalFlags.WrnFlagsDef.GenEnableAdsPressHigk = 1;
	WrnGlobalFlags.WrnFlagsDef.GenEnablePumpDepStill = 1;
	WrnGlobalFlags.WrnFlagsDef.GenEnablePumpPerfArtStill = 1;
	WrnGlobalFlags.WrnFlagsDef.GenEnablePumpOxygStill = 1;
}

void SetWarningEnableFlag(uint16_t WrnCode)
{
}

void InitWarningsStates(void)
{
	WrnGlobalFlags.WrnFlagsVal = 0;
	SetAllWarningEnableFlags();
}
// ritorna true se c'e' un allarme attivo (non una warning) e quindi le pompe sono sempre ferme
bool IsAlamActiveAndNotWrn(void)
{
	if(alarmCurrent.code &&
	   alarmCurrent.code <= CODE_ALARM_ART_RES_HIGH &&
	   (alarmCurrent.code == ptrAlarmCurrent->code) &&
	   (ptrAlarmCurrent->active == ACTIVE_TRUE))
		return TRUE;
	else
		return FALSE;
}


void WarningPhysicPressSensHigh(void)
{
	typeAlarmS* warningPtr = &alarmList[PRESS_ADS_FILTER_WARN];
	switch (WrnPressSensHighState)
	{
		case WRN_INIT:
			warningPtr->physic = PHYSIC_FALSE;
			warningPtr->faultConditionTimer = 0;

			if(WrnGlobalFlags.WrnFlagsDef.EnableAdsPressHigh)
				WrnPressSensHighState = WRN_CONTROL;
			break;

		case WRN_CONTROL:
			if ((PR_ADS_FLT_mmHg_Filtered > PR_ADS_FILTER_WARN) && (PR_ADS_FLT_mmHg_Filtered < PR_ADS_FILTER_HIGH))
			{
				warningPtr->faultConditionTimer += ALARM_TICK;
				if (warningPtr->faultConditionTimer >  warningPtr->entryTime)
				//if ((warningPtr->faultConditionTimer += ALARM_TICK) >= warningPtr->entryTime)
					warningPtr->active = ACTIVE_TRUE;
			}
			else if (warningPtr->faultConditionTimer >= ALARM_TICK)
				warningPtr->faultConditionTimer -= ALARM_TICK;

			if (warningPtr->active == ACTIVE_TRUE)
			{
				warningPtr->faultConditionTimer = 0;
				WrnPressSensHighState = WRN_WAIT_LOWER_LEV;
			}

			//Shortcut
			if (!WrnGlobalFlags.WrnFlagsDef.EnableAdsPressHigh)
				WrnPressSensHighState = WRN_INIT;
			break;

		case WRN_WAIT_LOWER_LEV:
			if (PR_ADS_FLT_mmHg_Filtered < PR_ADS_FILTER_WARN_LOWER_LEV)
			{
				if ((warningPtr->faultConditionTimer += ALARM_TICK) >= warningPtr->entryTime)
				{
					WrnPressSensHighState = WRN_INIT;
				}
			}
			else if (warningPtr->faultConditionTimer >= ALARM_TICK)
				warningPtr->faultConditionTimer -= ALARM_TICK;

			//Shortcut
			if (!WrnGlobalFlags.WrnFlagsDef.EnableAdsPressHigh)
				WrnPressSensHighState = WRN_INIT;
			break;
	}
}

void WarningDepPumpStill(void)
{

}


// allarme di pompa arteriosa ferma (valida per rene e fegato)
void WarningPerfArtPumpStill(void)
{

}


// allarme di pompa ossigenazione ferma (valida per rene e fegato)
void WarningOxygPumpStill(void)
{

}

void manageWarningPhysicPressSensHigh(void)
{
	if (WrnGlobalFlags.WrnFlagsDef.GenEnableAdsPressHigk)
	{
		WarningPhysicPressSensHigh();
	}
	else
	{
		alarmList[PRESS_ADS_FILTER_WARN].physic = PHYSIC_FALSE;
	}
}

void manageAlarmDepPumpStill(void)
{
	// Allarme attivo && DEPURAZIONE ON
	if ((GlobalFlags2.FlagsDef2.EnableDepPumpStopAlarms) && (((PARAMETER_ACTIVE_TYPE)parameterWordSetFromGUI[PAR_SET_DEPURATION_ACTIVE].value) == YES))
	{
		if (((PARAMETER_ACTIVE_TYPE)parameterWordSetFromGUI[PAR_SET_PURIF_FLOW_TARGET].value > 0) && 			// SET DEPURAZIONE > 0
		    (((int)(modbusData[pumpPerist[3].pmpMySlaveAddress-2][17] / 100)) <= (int)PUMP_STOPPED_SPEED_VAL))  // Velocità bassa
		{
			alarmList[DEP_PUMP_STILL].physic = PHYSIC_TRUE;
		}
		else
		{
			alarmList[DEP_PUMP_STILL].physic = PHYSIC_FALSE;
		}
	}
	else
	{
		alarmList[DEP_PUMP_STILL].physic = PHYSIC_FALSE;
	}
}

void manageAlarmPerfArtPumpStill(void)
{
	// Allarme attivo
	if (GlobalFlags2.FlagsDef2.EnableArtPumpStopAlarms)
	{
		// Velocità pompa bassa?
		if (((int)(modbusData[pumpPerist[0].pmpMySlaveAddress-2][17] / 100)) <= (int)PUMP_STOPPED_SPEED_VAL)
		{
			alarmList[PERF_ART_PUMP_STILL].physic = PHYSIC_TRUE;
		}
		else
		{
			alarmList[PERF_ART_PUMP_STILL].physic = PHYSIC_FALSE;
		}
	}
	else
	{
		alarmList[PERF_ART_PUMP_STILL].physic = PHYSIC_FALSE;
	}
}

void manageAlarmOxygPumpStill(void)
{
	if (GlobalFlags2.FlagsDef2.EnableOxyPumpStopAlarms)
	{
		if (GetTherapyType() == KidneyTreat)
		{
			if ((((PARAMETER_ACTIVE_TYPE)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_ACTIVE].value) == YES) && (parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value != 0))
			{
				if(((int)(modbusData[pumpPerist[1].pmpMySlaveAddress-2][17] / 100)) <= (int)PUMP_STOPPED_SPEED_VAL)
				{
					alarmList[OXYG_PUMP_STILL].physic = PHYSIC_TRUE;
				}
				else
				{
					alarmList[OXYG_PUMP_STILL].physic = PHYSIC_FALSE;
				}
			}
			else
				alarmList[OXYG_PUMP_STILL].physic = PHYSIC_FALSE;

			// perfusione venosa non presente nei trattamenti kidney
			alarmList[VEN_PUMP_STILL].physic = PHYSIC_FALSE;
		}
		else if (GetTherapyType() == LiverTreat)
		{
			// Set perfusione venosa
			if (parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value != 0)
			{
				if(((int)(modbusData[pumpPerist[1].pmpMySlaveAddress-2][17] / 100)) <= (int)PUMP_STOPPED_SPEED_VAL)
				{
					alarmList[VEN_PUMP_STILL].physic = PHYSIC_TRUE;
				}
				else
				{
					alarmList[VEN_PUMP_STILL].physic = PHYSIC_FALSE;
				}
			}
			else
				alarmList[VEN_PUMP_STILL].physic = PHYSIC_FALSE;

			// nei trattamenti liver l'ossigenazione coincide con la perfusione venosa
			alarmList[OXYG_PUMP_STILL].physic = PHYSIC_FALSE;
		}
	}
	else
	{
		alarmList[OXYG_PUMP_STILL].physic = PHYSIC_FALSE;
		alarmList[VEN_PUMP_STILL].physic = PHYSIC_FALSE;
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
			break;
		}

		case STATE_PRIMING_PH_2:
		{
			manageWarningPhysicPressSensHigh();
			break;
		}

		case STATE_TREATMENT_KIDNEY_1:
		{
			manageWarningPhysicPressSensHigh();
			if(GetTherapyType() == LiverTreat)
				manageAlarmDepPumpStill();

			manageAlarmPerfArtPumpStill();
			manageAlarmOxygPumpStill();
			break;
		}

		case STATE_PRIMING_WAIT:
			// in questo stato non sono gestiti gli allarmi, per ora
			break;

		case STATE_PRIMING_RICIRCOLO:
			manageWarningPhysicPressSensHigh();
			break;

		case STATE_WAIT_TREATMENT:
			break;

		case STATE_EMPTY_DISPOSABLE:
		case STATE_EMPTY_DISPOSABLE_1:
		{
			manageWarningPhysicPressSensHigh();
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
//unsigned long elapsedEntryStartTimeWrn[MAX_NUM_WARNING];
//unsigned long elapsedExitStartTimeWrn[MAX_NUM_WARNING];

// lista dei codici degli allarmi attivi
// il codice di warning e' attivo se vale 0..n
// il codice di warning non e' attivo se vale 0xffff
uint16_t WarningCodeArray[MAX_NUM_ALARM];
typeAlarmS * ptrWarningCurrent_new;

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
		memset(&warningCurrent, 0, sizeof(typeAlarmS));
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
	typeAlarmS * ptrAlm;
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


bool IsWarningElem(typeAlarmS * ptrAlmArrElem)
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
//	memset(elapsedEntryStartTimeWrn, 0, sizeof(elapsedEntryStartTimeWrn));
//	memset(elapsedExitStartTimeWrn, 0, sizeof(elapsedExitStartTimeWrn));
}




//---------------------------------------------NUOVA GESTIONE ALLARMI--------------------------------------------------
// lista di variabili di stato corrispondenti a vari allarmi e warning attivi
int AlmLisStateArr[MAX_NUM_ALARM];
// lista degli indici della struttura alarmListcorrispondenti ai vari allarmi attivi
// inizializzato a 0xffffffff
int AlmListOf_AlarmListStrctPos[MAX_NUM_ALARM];
// indice dell'array AlmLisStateArr di di stato degli allarmi che sto esaminando
int CurrAlmLisStateArrdx;
// posizione nell'array AlmLisStateArr del primo allarme
int AlmLisStateArrFirstWrnPos;
unsigned long elapsedEntryTime[MAX_NUM_ALARM];
unsigned long elapsedExitTime[MAX_NUM_ALARM];
//unsigned long elapsedEntryStartTime[MAX_NUM_ALARM];
//unsigned long elapsedExitStartTime[MAX_NUM_ALARM];

// lista dei codici degli allarmi che si sono attivati
// il codice di allarme e' attivo se vale 0..n
// il codice di allarme non e' attivo se vale 0xffff
uint16_t AlarmCodeArray[MAX_NUM_ALARM];
typeAlarmS * ptrAlarmCurrent_new;

// lista delle posizioni nell'array AlarmList che si sono attivati
int ListOf_AlarmListStrctPos[MAX_NUM_ALARM_ACTIVATED];
// lista dei codici di allarme che sono stati attivati
int ListOf_AlarmCode[MAX_NUM_ALARM_ACTIVATED];
// posizione nell'array WrnLisStateArr del primo allarme
int AlmLisStateArrFirstAlmPos;

// controlla se si tratta di un allarme aria e fa partire il recupero
// dopo l'ultimo reset
int IfAirAlmStartRecovery(typeAlarmS * ptrAlm)
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
//	memset(elapsedEntryStartTime, 0, sizeof(elapsedEntryStartTime));
//	memset(elapsedExitStartTime, 0, sizeof(elapsedExitStartTime));
}

// ----------------------------------------------------------------------------
// GESTIONE ALLARMI RIDOTTA
// ----------------------------------------------------------------------------


void SetProcessingAlarm(bool statusProcessing)
{
	sProcessingAlarm = statusProcessing;
}

bool ProcessingAlarm(void)
{
	return sProcessingAlarm;
}

//Inserisce un nuovo elemento nella lista, in fondo alla lista (posizione Last)
void InsertElementInActiveListAlm(typeAlarmS *alarmPtr)
{
	// Lista vuota: inizializzo i puntatori Current e Last, inizializzo dimensione lista
	if (sIdxCurrentActiveListAlm == EMPTY_LIST_ALM)
	{
		sIdxCurrentActiveListAlm = 0;
		sIdxLastActiveListAlm = 0;
		sSizeActiveListaAlm = 1;
        sActiveListAlm[sIdxCurrentActiveListAlm].ptr = alarmPtr;
	}
	else // Lista con almeno un elemento: incremento Last e dimensione lista
	{
        sIdxLastActiveListAlm ++;
        if (sIdxLastActiveListAlm >= MAX_ALARMS_ACTIVE_LIST_ALM)
        	sIdxLastActiveListAlm = 0;

		sSizeActiveListaAlm ++;
		sActiveListAlm[sIdxLastActiveListAlm].ptr = alarmPtr;
	}
}

//Inserisce un nuovo elemento nella lista, in fondo alla lista (posizione Last)
void InsertElementInActiveListWrn(typeAlarmS *warningPtr)
{
	// Lista vuota: inizializzo i puntatori Current e Last, inizializzo dimensione lista
	if (sIdxCurrentActiveListWrn == EMPTY_LIST_WRN)
	{
		sIdxCurrentActiveListWrn = 0;
		sIdxLastActiveListWrn = 0;
		sSizeActiveListaWrn = 1;
        sActiveListWrn[sIdxCurrentActiveListWrn].ptr = warningPtr;
	}
	else // Lista con almeno un elemento: incremento Last e dimensione lista
	{
        sIdxLastActiveListWrn ++;

		sSizeActiveListaWrn ++;
		sActiveListWrn[sIdxLastActiveListWrn].ptr = warningPtr;
	}
}

//Rimuove il primo elemento dalla lista
void RemoveElementFromActiveListAlm(void)
{
	bool removingStatus = FALSE;
	typeAlarmS *pointerCurrentAlarm = sActiveListAlm[sIdxCurrentActiveListAlm].ptr;

	//Lista non vuota?
	if (sSizeActiveListaAlm > 0)
	{
		pointerCurrentAlarm->active = ACTIVE_FALSE;
		manageAlarmChildGuard(GetCurrentAlarmActiveListAlm());
		if (pointerCurrentAlarm->init == TRUE)
			pointerCurrentAlarm->faultConditionTimer = 0;
		pointerCurrentAlarm = (typeAlarmS *)0; //clear

		// Ho un solo elemento?
		if (sSizeActiveListaAlm == 1)
		{
			//Reset dei puntatori
			sIdxCurrentActiveListAlm = EMPTY_LIST_ALM;
			sIdxLastActiveListAlm = EMPTY_LIST_ALM;
			sSizeActiveListaAlm = 0;
		}
		else
		{
			sIdxCurrentActiveListAlm ++;
	        if (sIdxCurrentActiveListAlm >= MAX_ALARMS_ACTIVE_LIST_ALM)
	        	sIdxCurrentActiveListAlm = 0;

			sSizeActiveListaAlm --;
		}
	}
}

//Rimuove il primo elemento dalla lista
void RemoveElementFromActiveListWrn(void)
{
	//Lista non vuota?
	if (sSizeActiveListaWrn > 0)
	{
		sActiveListWrn[sIdxCurrentActiveListWrn].ptr->active = ACTIVE_FALSE;
		sActiveListWrn[sIdxCurrentActiveListWrn].ptr = (typeAlarmS *)0; //clear

		// Ho un solo elemento?
		if (sSizeActiveListaWrn == 1)
		{
			//Reset dei puntatori
			sIdxCurrentActiveListWrn = EMPTY_LIST_WRN;
			sIdxLastActiveListWrn = EMPTY_LIST_WRN;
			sSizeActiveListaWrn = 0;
		}
		else
		{
			sIdxCurrentActiveListWrn ++;
			sSizeActiveListaWrn --;
		}
	}
}

// Fornisce il codice dell'allarme corrente (primo elemento della lista FIFO)
typeAlarmS* GetCurrentAlarmActiveListAlm(void)
{
	if (LengthActiveListAlm() > 0)
		return sActiveListAlm[sIdxCurrentActiveListAlm].ptr;
	else
		return (typeAlarmS*)0;
}

// Fornisce il codice dell'allarme corrente (primo elemento della lista FIFO)
typeAlarmS* GetCurrentWarningActiveListWrn(void)
{
	if (LengthActiveListWrn() > 0)
		return sActiveListWrn[sIdxCurrentActiveListWrn].ptr;
	else
		return (typeAlarmS*)0;
}

//Restituisce TRUE se un allarme è già presente in lista
bool AlarmPresentInActiveListAlm(typeAlarmS *alarmPtr)
{
	bool retValue = FALSE;
	unsigned char idx = sIdxCurrentActiveListAlm;

	//Lista non deve essere vuota
	if (sSizeActiveListaAlm == 1)
	{
		if (sActiveListAlm[idx].ptr->code == alarmPtr->code)
			retValue = TRUE; //Trovato: esco
	}
	else if (sSizeActiveListaAlm > 1)
	{
		while (idx != sIdxLastActiveListAlm)
		{
			if (sActiveListAlm[idx].ptr->code == alarmPtr->code)
			{
				retValue = TRUE; //Trovato: esco
				break;
			}
			idx ++;

			if (idx >= MAX_ALARMS_ACTIVE_LIST_ALM)
				idx = 0;
		}

		if (idx == sIdxLastActiveListAlm)
		{
			if (sActiveListAlm[idx].ptr->code == alarmPtr->code)
				retValue = TRUE; //Trovato: esco
		}
	}

	return retValue;
}

//Restituisce TRUE se un allarme è già presente in lista
bool WarningPresentInActiveListWrn(typeAlarmS *warningPtr)
{
	bool retValue = FALSE;
	unsigned char idx = 0;

	//Lista non deve essere vuota
	if (sSizeActiveListaWrn > 0)
	{
		//Scansione da Current a Last
		for (idx = sIdxCurrentActiveListWrn; idx <= sIdxLastActiveListWrn; idx ++)
		{
			if (sActiveListWrn[idx].ptr->code == warningPtr->code)
			{
				retValue = TRUE; //Trovato: esco
				break;
			}
		}
	}

	return retValue;
}

//Rimuove il primo elemento dalla lista degli allarmi e considera l'allarme processato
void EnableNextAlarmFunc(void)
{
	RemoveElementFromActiveListAlm();
	SetProcessingAlarm(FALSE);
	currentGuard[GUARD_ALARM_ACTIVE].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
}

//Restituisce il numero degli elementi in lista
unsigned char LengthActiveListAlm(void)
{
	return sSizeActiveListaAlm;
}

unsigned char LengthActiveListWrn(void)
{
	return sSizeActiveListaWrn;
}

//Gestore principale degli allarmi
void alarmEngineAlways(void)
{
	unsigned char scanAlarm = 0;
	typeAlarmS *alarmPtr = 0;

	// Scansione di tutta la struttura degli allarmi
	for (scanAlarm = 0; scanAlarm < FIRST_WARNING; scanAlarm++)
	{
		// puntatore alla singola riga della struttura degli allarmi
		alarmPtr = &alarmList[scanAlarm];
		if (alarmPtr->active != ACTIVE_TRUE) //L'allarme specifico NON è attivo
		{
			// La condizione di allarme è presente
			if (alarmPtr->physic == PHYSIC_TRUE)
			{
				//Incremento lo specifico timer della condizione di allarme, se supera la soglia di intervento pongo l'allarme ATTIVO
				if ((alarmPtr->faultConditionTimer += ALARM_TICK) >= alarmPtr->entryTime)
					alarmPtr->active = ACTIVE_TRUE;
			}
			else if (alarmPtr->faultConditionTimer >= ALARM_TICK)
				alarmPtr->faultConditionTimer -= ALARM_TICK; //Decremento il timer se la condizione di allarme non è più presente
			    // ma solo se l'allarme nel frattempo non è diventato ATTIVO
		}
	}
    // Al termine della scansione la struttura alarmList[] conterrà gli eventuali allarmi che sono diventati ACTIVE_TRUE

	// Scansione di tutta la struttura degli allarmi
	for (scanAlarm = 0; scanAlarm < FIRST_WARNING; scanAlarm++)
	{
		// puntatore alla singola riga della struttura degli allarmi
		alarmPtr = &alarmList[scanAlarm];
		if (alarmPtr->active == ACTIVE_TRUE)
		{
			//Per ogni allarme attivo verifico se è già presente nella lista allarmi attivi "ACTIVE LIST A"
			if (FALSE == AlarmPresentInActiveListAlm(alarmPtr))
				InsertElementInActiveListAlm(alarmPtr); //in caso non sia presente, lo inserisco
		}
	}
	// Al termine della scansione, la struttura alarmListActive[] contiene gli allarmi attivi

	// Se ho una lista di allarmi attivi non vuota, processo un singolo allarme (il primo in lista)
	if (LengthActiveListAlm() > 0)
	{
		//Se non sto già processando un allarme, prelevo il primo in lista
		if (!ProcessingAlarm())
		{
			sActiveListAlmS* punt;
			punt = &sActiveListAlm[sIdxCurrentActiveListAlm];
			punt->ptr->prySafetyActionFunc();
			SetProcessingAlarm(TRUE);
		}
	}
}

void WarningEngineAlways(void)
{
	unsigned char scanWarning = 0;
	typeAlarmS *warningPtr = 0;

	// Scansione di tutta la struttura degli allarmi
	for (scanWarning = FIRST_WARNING; scanWarning < ALARM_ACTIVE_IN_STRUCT; scanWarning++)
	{
		// puntatore alla singola riga della struttura degli allarmi
		warningPtr = &alarmList[scanWarning];
		if (warningPtr->active == ACTIVE_TRUE)
		{
			//Per ogni allarme attivo verifico se è già presente nella lista allarmi attivi "ACTIVE LIST A"
			if (FALSE == WarningPresentInActiveListWrn(warningPtr))
				InsertElementInActiveListWrn(warningPtr); //in caso non sia presente, lo inserisco
		}
	}
	// Al termine della scansione, la struttura alarmListActive[] contiene gli allarmi attivi

	// Se siamo in assenza di allarmi...
	if (LengthActiveListAlm() == 0)
	{
		//...verifico se c'è uno warning attivo
		if (LengthActiveListWrn() > 0)
		{
			sActiveListWrnS *warningPtr = 0;
			uint16_t warningCode = 0;

			warningPtr = &sActiveListWrn[sIdxCurrentActiveListWrn];
			warningCode = warningPtr->ptr->code;

			//Intercetto la pressione del tasto reset per rimuovere lo warning corrente
			if (buttonGUITreatment[BUTTON_RESET_ALARM].state == GUI_BUTTON_RELEASED)
			{
				releaseGUIButton(BUTTON_RESET_ALARM);
				RemoveElementFromActiveListWrn();
			}
		}
	}
}

//alarmList[pos_alm].prySafetyActionFunc();
void alarmManageNull(void)
{
	currentGuard[GUARD_ALARM_ACTIVE].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
	manageAlarmChildGuard(GetCurrentAlarmActiveListAlm()); //Gestisce le sicurezze associate all'allarme
}
