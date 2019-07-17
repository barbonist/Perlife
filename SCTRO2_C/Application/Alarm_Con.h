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

#define ALARM_ACTIVE_IN_STRUCT					54

#define CODE_ALARM_PRESS_ART_HIGH				1
#define CODE_ALARM_PRESS_ART_SET				2
#define CODE_ALARM_PRESS_VEN_HIGH				3
#define CODE_ALARM_PRESS_VEN_SET				4
#define CODE_ALARM_PRESS_ADS_FILTER_HIGH		5
#define CODE_ALARM_PRESS_OXYG_INLET				8
#define CODE_ALARM_PRESS_ADS_FILTER_LOW         9
#define CODE_ALARM_PRESS_OXYG_LOW               10
#define CODE_ALARM_FLOW_ART_SET					11
#define CODE_ALARM_FLOW_VEN_SET                 12
#define CODE_ALARM_FLOW_DEP_SET                 13
#define CODE_ALARM_FLOW_OXY_HIGH                14
#define CODE_ALARM_FLOW_OXY_SET                 15
#define CODE_ALARM_TUBE_ART_DISCONNECTED		16
#define CODE_ALARM_TUBE_VEN_DISCONNECTED		17

#define CODE_ALARM_TEMP_SENS_NOT_DETECTED		22
#define CODE_ALARM_DELTA_TEMP_REC_ART		    23
#define CODE_ALARM_DELTA_TEMP_REC_VEN		    24
#define CODE_ALARM_T_ART_OUT_OF_RANGE           25
#define CODE_ALARM_TEMP_MAX_IN_TRT              26
#define CODE_ALARM_TEMP_MIN_IN_TRT              27

#define CODE_ALARM_FLOW_PERF_ART_HIGH			32
#define CODE_ALARM_FLOW_PERF_VEN_HIGH			33
#define CODE_ALARM_DELTA_FLOW_ART       		37
#define CODE_ALARM_DELTA_FLOW_VEN       		38

#define CODE_ALARM_TANK_LEVEL_LOW				48
#define CODE_ALARM_TANK_LEVEL_HIGH				49
#define CODE_ALARM_FLOW_NOT_DETECTED		    52

#define CODE_ALARM_AIR_PRES_ART					80
#define CODE_ALARM_AIR_PRES_VEN					81
#define CODE_ALARM_AIR_PRES_FILTER      		82
#define CODE_ALARM_PRIM_AIR_PRES_ART            83
#define CODE_ALARM_PRIM_AIR_PRES_VEN			84
#define CODE_ALARM_PRIM_AIR_PRES_FILTER      	85

#define CODE_ALARM_PUMP_PERF_COVER_OPEN			112		//LIVER  - perfusion
#define CODE_ALARM_PUMP_PERF_COVER_OPEN_KD		113		//KIDNEY - perfusion
#define CODE_ALARM_PUMP_PURIF_COVER_OPEN		114		//LIVER  - depuration
#define CODE_ALARM_PUMP_VEN_COVER_OPEN			115		//LIVER  - venous
#define CODE_ALARM_PUMP_OXY_COVER_OPEN			116		//KIDNEY - oxygen
#define CODE_ALARM_ART_RES_HIGH                 117

#define CODE_ALARM_MODBUS_ACTUATOR_SEND         144
#define CODE_ALARM_CAN_BUS_ERROR                145
#define CODE_ALARM_BAD_PINCH_POS                0x92

#define CODE_ALARM_EMERGENCY_BUTTON             147 //NON USATO
#define CODE_ALARM_AIR_SENSOR_TEST_KO			149

#define CODE_ALARM_MACHINE_COVERS               160 //NON ATTIVO
#define CODE_ALARM_HOOKS_RESERVOIR              161
#define CODE_ALARM_DEP_PUMP_STILL    		    164		//LIVER 		- depuration
#define CODE_ALARM_PERF_ART_PUMP_STILL  		165     //LIVER-KIDNEY 	- perfusion
#define CODE_ALARM_PERF_VEN_PUMP_STILL          166     //LIVER 		- venous
#define CODE_ALARM_OXYG_PUMP_STILL            	167     //KIDNEY 		- oxygen

// fissato a 200 ed e' il codice di errore di partenza degli allarmi protective
#define CODE_ALARM_PROT_START_VAL               200

// da qui in avanti i codici delle warning
#define CODE_ALARM_PRESS_ADS_FILTER_WARN		261

// A partire dal codice 448 gli allarmi vengono visualizzati a GUI senza il tasto RESET
// L'allarme T1 Test è bloccante per il momento, in futuro potrebbe essere resettabile per forzare una riesecuzione del test fallito
#define CODE_ALARM_TEST_T1		  				600

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
#define FLOW_PERF_ART_LIVER_MAX    600 // ml/min
#define FLOW_PERF_ART_KIDNEY_MAX   600 // ml/min

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
#define DELTA_TARGET_FLOW_OXY_KIDNEY						100 // ml/min
#define DELTA_TARGET_FLOW_DEP_LIVER							50 // ml/min
#define DELTA_TARGET_FLOW_ART_KIDNEY						50 // ml/min

// Allarmi pressione ossigenatore [mmHg]
#define PR_OXYG_HIGH                                        500
#define PR_OXYG_LOW                                          10

// Allarmi pressione filtro [mmHg]
#define PR_ADS_FILTER_HIGH									450
#define PR_ADS_FILTER_LOW                                    10
#define PR_ADS_FILTER_WARN									100

#define MAX_MSG_CONSECUTIVE_ACTUATOR_MODBUS_NOT_RESPOND 	 10


#define MAX_BUBBLE_SIZE		50

enum ALARM
{
	 FIRST_ALARM = 0,
	 PRESS_ART_HIGH = FIRST_ALARM,			//CODE_ALARM_PRESS_ART_HIGH					//OK
	 PRESS_ART_SET,							//CODE_ALARM_PRESS_ART_SET					//OK
	 AIR_PRES_ART,							//CODE_ALARM_AIR_PRES_ART					//OK
	 AIR_PRES_VEN,							//CODE_ALARM_AIR_PRES_VEN					//OK
	 SAF_AIR_SENSOR,						//CODE_ALARM_AIR_PRES_FILTER				//OK
	 PRESS_ADS_FILTER_HIGH,					//CODE_ALARM_PRESS_ADS_FILTER_HIGH			//OK
	 FLOW_PERF_ART_HIGH,					//CODE_ALARM_FLOW_PERF_ART_HIGH				//OK
	 FLOW_PERF_VEN_HIGH,					//CODE_ALARM_FLOW_PERF_VEN_HIGH				//OK
	 FLOW_OXY_HIGH,							//CODE_ALARM_OXY_HIGH						//OK
	 FLOW_SENS_NOT_DETECTED,				//CODE_ALARM_FLOW_NOT_DETECTED
	 PRESS_VEN_HIGH,						//CODE_ALARM_PRESS_VEN_HIGH
	 PRESS_VEN_SET,							//CODE_ALARM_PRESS_VEN_SET
	 PRESS_OXYG_HIGH,						//CODE_ALARM_PRESS_OXYG_INLET
	 IR_SENS_NOT_DETECTED,					//CODE_ALARM_TEMP_SENS_NOT_DETECTED
	 PERF_COVER_OPEN,						//CODE_ALARM_PUMP_PERF_COVER_OPEN
	 PERF_COVER_OPEN_KD,					//CODE_ALARM_PUMP_PERF_COVER_OPEN_KD
	 PURIF_COVER_OPEN,						//CODE_ALARM_PUMP_PURIF_COVER_OPEN
	 VEN_COVER_OPEN,						//CODE_ALARM_PUMP_VEN_COVER_OPEN
	 OXY_COVER_OPEN,						//CODE_ALARM_PUMP_OXY_COVER_OPEN
	 LIQUID_LEVEL_LOW,						//CODE_ALARM_TANK_LEVEL_LOW
	 LIQUID_LEVEL_HIGH,						//CODE_ALARM_TANK_LEVEL_HIGH
	 DELTA_FLOW_ART,						//CODE_ALARM_DELTA_FLOW_ART
	 DELTA_FLOW_VEN,						//CODE_ALARM_DELTA_FLOW_VEN
	 DELTA_TEMP_REC_ART,					//CODE_ALARM_DELTA_TEMP_REC_ART
	 DELTA_TEMP_REC_VEN,					//CODE_ALARM_DELTA_TEMP_REC_VEN
	 CAN_BUS_ERROR,							//CODE_ALARM_CAN_BUS_ERROR
	 BAD_PINCH_POS,							//CODE_ALARM_BAD_PINCH_POS
	 PRIM_AIR_ON_ART,						//CODE_ALARM_PRIM_AIR_PRES_ART
	 PRIM_AIR_ON_VEN,						//CODE_ALARM_PRIM_AIR_PRES_VEN
	 PRIM_AIR_ON_FILTER,					//CODE_ALARM_PRIM_AIR_PRES_FILTER
	 PRESS_ADS_FILTER_LOW,					//CODE_ALARM_PRESS_ADS_FILTER_LOW
	 PRESS_OXYG_LOW,						//CODE_ALARM_PRESS_OXYG_LOW
	 FLOW_ART_SET,							//CODE_ALARM_FLOW_ART_SET
	 FLOW_VEN_SET,							//CODE_ALARM_FLOW_VEN_SET
	 FLOW_OXY_SET,							//CODE_ALARM_FLOW_OXY_SET
	 FLOW_DEP_SET,							//CODE_ALARM_FLOW_DEP_SET
	 MODBUS_ACTUATOR_SEND,					//CODE_ALARM_MODBUS_ACTUATOR_SEND
	 ALARM_FROM_PROTECTIVE,					//CODE_ALARM_PROT_START_VAL

	 // Filippo - inserisco il codice di allarme
	 ALARM_STOP_BUTTON,

	 // Filippo - inserisco il codice di allarme
	 ALARM_T1_TEST,

	 //Filippo - allarme per test sensore aria sbagliato
	 ALARM_AIR_SENSOR_TEST_KO,

	 //Allarme per sensori di cover anteriori
     ALARM_MACHINE_COVERS,
	 //Allarme per sensori dei ganci reservoire
	 ALARM_HOOKS_RESERVOIR,					//CODE_ALARM_HOOKS_RESERVOIR
     ARTERIAL_RESIST_HIGH,					//CODE_ALARM_ART_RES_HIGH
	 TEMP_ART_OOR,							//CODE_ALARM_T_ART_OUT_OF_RANGE
	 TEMP_MAX_IN_TREAT,						//CODE_ALARM_TEMP_MAX_IN_TRT
	 TEMP_MIN_IN_TREAT,						//CODE_ALARM_TEMP_MIN_IN_TRT
	 TUBE_ART_DISCONNECTED,					//CODE_ALARM_TUBE_ART_DISCONNECTED
	 TUBE_VEN_DISCONNECTED,					//CODE_ALARM_TUBE_VEN_DISCONNECTED
	 DEP_PUMP_STILL,						//CODE_ALARM_DEP_PUMP_STILL
	 PERF_ART_PUMP_STILL,					//CODE_ALARM_PERF_ART_PUMP_STILL
	 VEN_PUMP_STILL,						//CODE_ALARM_PERF_VEN_PUMP_STILL
	 OXYG_PUMP_STILL,						//CODE_ALARM_OXYG_PUMP_STILL
	 FIRST_WARNING,
	 // da qui in avanti i codici delle warning
	 PRESS_ADS_FILTER_WARN = FIRST_WARNING,	//CODE_ALARM_PRESS_ADS_FILTER_WARN
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

// Allarmi di pompe ferme a lungo
void EnableLongPumpStopAlarms(void);
void DisableLongPumpStopAlarms(void);

// Allarmi di flusso massimi
void SetFlowHigAlarmEnableFlags(void);
void DisableFlowHigAlarmEnableFlags(void);

void ForceAlarmOff(uint16_t code);
void ForceCurrentAlarmOff(void);
void DisableAllAlarm();
void DisableAllAirAlarm(bool dis);
void manageAlarmCanBus(void);
void manageAlarmBadPinchPos(void);
void manageAlarmPrimAirPresFilter(void);
void manageAlarmPrimAirPresArt(void);
void manageAlarmPrimAirPresVen(void);
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

void manageCheckConnectionTubeArtSensPress(void);
void manageCheckConnectionTubeVenSensPress(void);

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
