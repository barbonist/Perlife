/*
 * Alarm_Con.h
 *
 *  Created on: 10/nov/2016
 *      Author: W15
 */

#ifndef APPLICATION_ALARM_CON_H_
#define APPLICATION_ALARM_CON_H_

#include "Global.h"

struct elementActiveListAlm {
	typeAlarmS *ptr;
	struct elementActiveListAlm* next;
};

struct elementActiveListWrn {
	typeAlarmS *ptr;
	struct elementActiveListWrn* next;
};

#define 	EMPTY_LIST_ALM 				(-1)
#define     MAX_ALARMS_ACTIVE_LIST_ALM 	10

#define 	EMPTY_LIST_WRN 				(-1)
#define     MAX_ALARMS_ACTIVE_LIST_WRN 	10

#define    NoWarningsPresent()			(LengthActiveListWrn() == 0)
#define    NoAlarmsPresent()			(LengthActiveListAlm() == 0)

typedef struct elementActiveListAlm sActiveListAlmS;
typedef struct elementActiveListWrn sActiveListWrnS;

//#define CODE_ALARM0		0x00
//#define CODE_ALARM1		0x01
//#define CODE_ALARM2		0x02
//#define ALARM_ACTIVE_IN_STRUCT					31
// Filippo - aggiunto allarme tasto di stop e allarme T1
#define ALARM_ACTIVE_IN_STRUCT					45
#define CODE_ALARM_PRESS_ART_HIGH				0X01
#define CODE_ALARM_PRESS_ART_SET				0X02
#define CODE_ALARM_PRESS_VEN_HIGH				0X03
#define CODE_ALARM_PRESS_VEN_SET				0X04
#define CODE_ALARM_PRESS_ADS_FILTER_HIGH		0X05
#define CODE_ALARM_PRESS_PLASMA_FILTER_HIGH		0X06
#define CODE_ALARM_PRESS_FRACTION_FILTER_HIGH	0X07
#define CODE_ALARM_PRESS_OXYG_INLET				0x08
#define CODE_ALARM_PRESS_ADS_FILTER_LOW         0x09
#define CODE_ALARM_PRESS_OXYG_LOW               0x0a
#define CODE_ALARM_FLOW_ART_SET					0x0b
#define CODE_ALARM_FLOW_VEN_SET                 0x0c
#define CODE_ALARM_FLOW_DEP_SET                 0x0d

// codice per allarme di superamento della temperatura massima o minima
// Viene usato in priming\ricircolo
#define CODE_ALARM_TEMP_ART_LOW					0x11
#define CODE_ALARM_TEMP_VEN_HIGH				0x12
#define CODE_ALARM_TEMP_VEN_LOW					0x13
#define CODE_ALARM_TEMP_NTC_HIGH				0x14
#define CODE_ALARM_TEMP_NTC_LOW					0x15
#define CODE_ALARM_TEMP_SENS_NOT_DETECTED		0x16
#define CODE_ALARM_DELTA_TEMP_REC_ART		    0x17
#define CODE_ALARM_DELTA_TEMP_REC_VEN		    0x18
// codice per allarme di temperatura fuori range di almeno due gradi.
// Viene usato in trattamento
#define CODE_ALARM_T_ART_OUT_OF_RANGE           0x19
// codice per allarme di superamento della temperatura massima o minima
// Viene usato in trattamento
#define CODE_ALARM_TEMP_MAX_IN_TRT              0x1a
#define CODE_ALARM_TEMP_MIN_IN_TRT              0x1b

#define CODE_ALARM_FLOW_PERF_ART_HIGH			0x20
#define CODE_ALARM_FLOW_PERF_VEN_HIGH			0x21
#define CODE_ALARM_FLOW_ART_NOT_DETECTED		0x24
#define CODE_ALARM_DELTA_FLOW_ART       		0x25
#define CODE_ALARM_DELTA_FLOW_VEN       		0x26


#define CODE_ALARM_TANK_LEVEL_LOW				0x30
#define CODE_ALARM_TANK_LEVEL_HIGH				0x31

#define CODE_ALARM_AIR_PRES_ART					0x50
#define CODE_ALARM_AIR_PRES_VEN					0x51
#define CODE_ALARM_AIR_PRES_ADSRB_FILTER		0x53
#define CODE_ALARM_SFA_PRIM_AIR_DET             0x54


#define CODE_ALARM_STOP_ALL_ACTUATOR			0X60 /* pump, pinch, peltier */
#define CODE_ALARM_STOP_ALL_PUMP				0X61 /* pump */
#define CODE_ALARM_STOP_PERF_PUMP				0x62 /* stop perfusion pump*/
#define CODE_ALARM_STOP_OXYG_PUMP				0x63 /* stop oxygenation pump */
#define CODE_ALARM_STOP_PURIF_PUMP				0x64 /* stop purification pump */
#define CODE_ALARM_STOP_PELTIER					0x65 /* stop peltier */

#define CODE_ALARM_PUMP_PERF_COVER_OPEN			0x70
#define CODE_ALARM_PUMP_PURIF_COVER_OPEN		0x71
#define CODE_ALARM_PUMP_OXYG_COVER_OPEN			0x73
#define CODE_ALARM_PUMPS_NOT_STILL              0x74
#define CODE_ALARM_ART_RES_HIGH                 0x75

#define CODE_ALARM_END_PRIMING_PHASE_1			0x80
#define CODE_ALARM_END_PRIMING_PHASE_2			0x81
#define CODE_ALARM_END_TREATMENT				0x85

#define CODE_ALARM_MODBUS_ACTUATOR_SEND         0x90
#define CODE_ALARM_CAN_BUS_ERROR                0x91
#define CODE_ALARM_BAD_PINCH_POS                0x92

// Filippo - definisco codice allarme per il tasto di stop
#define CODE_ALARM_EMERGENCY_BUTTON             0x93

#define CODE_ALARM_AIR_SENSOR_TEST_KO			0x95

#define CODE_ALARM_MACHINE_COVERS               0xA0
#define CODE_ALARM_HOOKS_RESERVOIR              0xA1


// fissato a 200 ed e' il codice di errore di partenza degli allarmi protective
#define CODE_ALARM_PROT_START_VAL               0xc8

// da qui in avanti i codici delle warning
#define CODE_ALARM_PRESS_ADS_FILTER_WARN		0X105

// warning pompa di depurazione ferma
#define CODE_ALARM_DEP_PUMP_STILL_WARN		    0x106
// warning di pompa arteriosa ferma (valido per rene e fegato)
#define CODE_ALARM_PERF_ART_PUMP_STILL_WARN		0x107
#define CODE_ALARM_OXYG_PUMP_STILL_WARN         0x108
// A partire dal codice 448 gli allarmi vengono visualizzati a GUI senza il tasto RESET
// L'allarme T1 Test � bloccante per il momento, in futuro potrebbe essere resettabile per forzare una riesecuzione del test fallito
#define CODE_ALARM_TEST_T1		  				0x258

#define	PHYSIC_TRUE		0xA5
#define PHYSIC_FALSE	0x5A

#define	ACTIVE_TRUE		0xA5
#define ACTIVE_FALSE	0x5A

#define ALARM_TYPE_CONTROL		0xA5
#define ALARM_TYPE_PROTECTION	0x5A

#define PRIORITY_LOW			0x55
#define PRIORITY_MEDIUM			0x5A
#define PRIORITY_HIGH			0xA5

#define SECURITY_STOP_ALL_ACTUATOR		0x01
#define SECURITY_STOP_ALL				0x02
#define SECURITY_STOP_PERF_PUMP			0x04
#define SECURITY_STOP_PURIF_PUMP		0x08
#define SECURITY_STOP_OXYG_PUMP			0x10
#define SECURITY_STOP_PELTIER			0x20
#define SECURITY_WAIT_CONFIRM           0x40
#define SECURITY_SAF_AIR_FILTER         0x80
#define SECURITY_SFV_AIR_DET            0x100
#define SECURITY_SFA_AIR_DET            0x200
#define SECURITY_FREE_SLOT_1            0x400
#define SECURITY_BAD_PINCH_POS          0x800
#define SECURITY_SFA_PRIM_AIR_DET       0x1000
#define SECURITY_STOP_ALL_ACT_WAIT_CMD  0x2000
#define SECURITY_MOD_BUS_ERROR          0x4000
#define SECURITY_DELTA_TEMP_HIGH        0x8000

#define OVRD_ENABLE				0xA5
#define OVRD_NOT_ENABLED		0x5A

#define RESET_ALLOWED			0xA5
#define RESET_NOT_ALLOWED		0x5A

#define SILENCE_ALLOWED			0xA5
#define SILENCE_NOT_ALLOWED		0x5A

#define MEMO_ALLOWED			0xA5
#define MEMO_NOT_ALLOWED		0x5A

// Flussi massimi di perfusione arteriosa
#define FLOW_PERF_ART_LIVER_MAX    550 // ml/min
#define FLOW_PERF_ART_KIDNEY_MAX   550 // ml/min

// Flussi massimi di ossigenazione/perfusione venosa
#define FLOW_PERF_VEN_LIVER_MAX    2800 // ml/min
#define FLOW_PERF_OXY_KIDNEY_MAX   2800 // ml/min

#define MAX_MSG_CONSECUTIVE_FLOW_SENS_NOT_DETECTED 			10
#define MAX_MSG_CONSECUTIVE_IR_TEMP_SENS_NOT_DETECTED 		10

// Allarmi di pressione arteriosa e venosa Liver [mmHg]
#define PR_ART_HIGH_PRIMING_LIVER							300
#define PR_VEN_HIGH_PRIMING_LIVER							300
#define PR_ART_HIGH_TREATMENT_LIVER							110
#define PR_VEN_HIGH_TREATMENT_LIVER						     10

// Allarmi di pressione arteriosa Kidney [mmHg]
#define PR_ART_HIGH_PRIMING_KIDNEY							250
#define PR_ART_HIGH_TREATMENT_KIDNEY						110

//Allarmi relativi ai set della pressione target
#define DELTA_TARGET_PRESS_VEN_LIVER						  5 //mmHg rispetto al Target
#define DELTA_TARGET_PRESS_ART_LIVER						 10 //mmHg rispetto al Target
#define DELTA_TARGET_PRESS_ART_KIDNEY						 10 //mmHg rispetto al Target

//Allarmi relativi ai set di flusso
#define DELTA_TARGET_FLOW_ART_LIVER							50 // ml/min
#define DELTA_TARGET_FLOW_VEN_LIVER							100 // ml/min
#define DELTA_TARGET_FLOW_DEP_LIVER							50 // ml/min
#define DELTA_TARGET_FLOW_ART_KIDNEY						50 // ml/min

// Allarmi pressione ossigenatore [mmHg]
#define PR_OXYG_HIGH                                        500
#define PR_OXYG_LOW                                          10

// Allarmi pressione filtro [mmHg]
#define PR_ADS_FILTER_HIGH									450
#define PR_ADS_FILTER_LOW                                    10
#define PR_ADS_FILTER_WARN									100

#define MAX_MSG_CONSECUTIVE_ACTUATOR_MODBUS_NOT_RESPOND 	10


#define MAX_BUBBLE_SIZE		50

enum ALARM
{
	 FIRST_ALARM = 0,
	 PRESS_ART_HIGH = FIRST_ALARM,
	 PRESS_ART_SET,
	 AIR_PRES_ART,
	 AIR_PRES_VEN,
	 SAF_AIR_SENSOR,
	 PRESS_ADS_FILTER_HIGH,
	 FLOW_PERF_ART_HIGH,
	 FLOW_PERF_VEN_HIGH,
	 FLOW_SENS_NOT_DETECTED,
	 PRESS_VEN_HIGH,
	 PRESS_VEN_SET,
	 PRESS_OXYG_HIGH,
	 IR_SENS_NOT_DETECTED,
	 PERF_COVER_OPEN,
	 PURIF_COVER_OPEN,
	 OXYG_COVER_OPEN,
	 LIQUID_LEVEL_LOW,
	 LIQUID_LEVEL_HIGH,
	 DELTA_FLOW_ART,
	 DELTA_FLOW_VEN,
	 DELTA_TEMP_REC_ART,
	 DELTA_TEMP_REC_VEN,
	 CAN_BUS_ERROR,
	 BAD_PINCH_POS,
	 PRIM_AIR_ON_FILTER,
	 PRESS_ADS_FILTER_LOW,
	 PRESS_OXYG_LOW,
	 FLOW_ART_SET,
	 FLOW_VEN_SET,
	 FLOW_DEP_SET,
	 MODBUS_ACTUATOR_SEND,
	 ALARM_FROM_PROTECTIVE,

	 // Filippo - inserisco il codice di allarme
	 ALARM_STOP_BUTTON,

	 // Filippo - inserisco il codice di allarme
	 ALARM_T1_TEST,

	 //Filippo - allarme per test sensore aria sbagliato
	 ALARM_AIR_SENSOR_TEST_KO,

	 //Allarme per sensori di cover anteriori
     ALARM_MACHINE_COVERS,
	 //Allarme per sensori dei ganci reservoire
	 ALARM_HOOKS_RESERVOIR,
     ARTERIAL_RESIST_HIGH,
	 TEMP_ART_OOR,
	 TEMP_MAX_IN_TREAT,
	 TEMP_MIN_IN_TREAT,

	 FIRST_WARNING,
	 // da qui in avanti i codici delle warning
	 PRESS_ADS_FILTER_WARN = FIRST_WARNING,
	 DEP_PUMP_STILL_WARN,
	 PERF_ART_PUMP_STILL_WRN,
	 OXYG_PUMP_STILL_WRN
};

// Nuova gestione allarmi: aprile 2019
unsigned char LengthActiveListAlm(void);
typeAlarmS* GetCurrentAlarmActiveListAlm(void);
bool AlarmPresentInActiveListAlm(typeAlarmS *alarmPtr);

unsigned char LengthActiveListWrn(void);
typeAlarmS* GetCurrentWarningActiveListWrn(void);
bool WarningPresentInActiveListWrn(typeAlarmS *alarmPtr);

void SetProcessingAlarm(bool statusProcessing);
bool ProcessingAlarm(void);

void CalcWarningActive(void);
void alarmConInit(void);
void alarmEngineAlways(void);
void WarningEngineAlways(void);
void alarmManageNull(void);
void manageAlarmChildGuard(typeAlarmS * ptrAlarm);
void manageAlarmPhysicFlowHigh(void);
void manageAlarmFlowSensNotDetected(void);
void manageAlarmIrTempSensNotDetected(void);
void manageAlarmActuatorModbusNotRespond(void);
void manageAlarmActuatorWRModbusNotRespond(void);
void manageAlarmFromProtective(void);

void manageAlarmPhysicPressSensHigh(void);
void manageAlarmPhysicSetFlowAndPressures(void);
void manageAlarmPhysicPressSensLow(void);

void manageAlarmPhysicTempSens(void);
void manageAlarmPhysicTempSensOOR(void);
void manageAlarmPhysicTempSensInTreat(void);

void manageAlarmPhysicUFlowSens(void);
void manageAlarmSAFAirSens(void);
void manageAlarmPhysicUFlowSensVen(void);
//void DebugStringStr(char *s);
//void DebugStringPID();

void Buzzer_Management(BUZZER_LEVEL level);
void manageAlarmCoversPumpLiver(void);
void manageAlarmCoversPumpKidney(void);
void manageAlarmLiquidLevelHigh(void);
void manageAlarmLiquidLevelLow(void);
void manageAlarmDeltaFlowArt(void);
void manageAlarmDeltaFlowVen(void);
void manageAlarmDeltaTempRecArt(void);
void manageAlarmDeltaTempRecVen(void);
void manageAlarmArterialResistance(void);
void CalcAlarmActive(void);
void SetAllAlarmEnableFlags(void);

// Allarmi di SET pressione e flusso
void EnableFlowAndPressSetAlarmEnableFlags(void);
void DisableFlowAndPressSetAlarmEnableFlags(void);

// Allarmi di flusso massimi
void SetFlowHigAlarmEnableFlags(void);
void DisableFlowHigAlarmEnableFlags(void);

void ForceAlarmOff(uint16_t code);
void ForceCurrentAlarmOff(void);
void DisableAllAlarm();
void DisableAllAirAlarm(bool dis);
void manageAlarmCanBus(void);
void manageAlarmBadPinchPos(void);
void manageAlarmPrimSFAAirDet(void);
void EnableBadPinchPosAlmFunc(void);
void EnablePrimAlmSFAAirDetAlmFunc(void);
void DisableBadPinchPosAlmFunc(void);
void DisablePrimAlmSFAAirDetAlmFunc(void);
void DisablePrimAirAlarm(bool dis);

void EnableNextAlarmFunc(void);
bool IsDisposableEmptyNoAlarm(void);
void ClearModBusAlarm(void);


void StopAllCntrlAlarm(GLOBAL_FLAGS *gbf);
void RestoreAllCntrlAlarm(GLOBAL_FLAGS *pgbf);
bool IsControlInAlarm(void);
bool IsProtectiveInAlarm(void);
void ClearAlarmState(void);

uint16_t ReadProtectiveAlarmCode(void);
bool IsAlarmActive(void);
bool IsAlarmCodeActive(uint16_t code);

void warningConInit(void);
void InitWarningsStates(void);
void EnableNextWarningFunc(void);
// Filippo - aggiunta gestione dell'allarme del tasto di stop
void manageAlarmStopButtonPressed(void);
void manageAlarmT1Test(void);
void manageResultT1TestDigital(void);
// Filippo - funzione che gestisce l'allarme per il fallimento del test del sensore aria
void manageAlarmAirSensorTestKO(void);
void manageCover_Hook_Sensor(void);

void EnableDeltaTHighAlmFunc(void);
void DisableDeltaTHighAlmFunc(void);
bool IsCanBusError(void);


#define START_WARNING_CODE		255

//----------------------------------------------------------------------------------------------------------
typedef enum
{
	WRN_INIT,
	WRN_CONTROL,
	WRN_CONTROL_DELAY,
	WRN_WAIT_END_ALARM,
	WRN_WAIT_LOWER_LEV
}WARNING_STATE;

// Struttura utilizzata per i T1 Test sui segnali digitali
typedef struct
{
	unsigned char previous;
	unsigned char current;
	bool pass;
} DigitalTestStruct;

#define PR_ADS_FILTER_WARN_LOWER_LEV    90
// tempo di pompa depurazione ferma prima di dare la warning in msec
#define DEPURATION_PUMP_STILL_TOUT  120000
// valore di velocita' al di sotto della quale considero la pompa ferma
#define PUMP_STOPPED_SPEED_VAL 1


// durata della resistenza arteriosa alta prima di scattare l'allarme
#define ART_RES_HIGH_TOUT_MSEC 10000
// tempo necessario per far scattare l'allarme
#define ART_RES_ALM_ON_MSEC    1000

void InitAlarmsStates(void);
#endif /* APPLICATION_ALARM_CON_H_ */
