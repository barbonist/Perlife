/*
 * Alarm_Con.h
 *
 *  Created on: 10/nov/2016
 *      Author: W15
 */

#ifndef APPLICATION_ALARM_CON_H_
#define APPLICATION_ALARM_CON_H_

#include "Global.h"

//#define CODE_ALARM0		0x00
//#define CODE_ALARM1		0x01
//#define CODE_ALARM2		0x02
//#define ALARM_ACTIVE_IN_STRUCT					31
// Filippo - aggiunto allarme tasto di stop e allarme T1
#define ALARM_ACTIVE_IN_STRUCT					36//34
#define CODE_ALARM_PRESS_ART_HIGH				0X01
#define CODE_ALARM_PRESS_ART_LOW				0X02
#define CODE_ALARM_PRESS_VEN_HIGH				0X03
#define CODE_ALARM_PRESS_VEN_LOW				0X04
#define CODE_ALARM_PRESS_ADS_FILTER_HIGH		0X05
#define CODE_ALARM_PRESS_PLASMA_FILTER_HIGH		0X06
#define CODE_ALARM_PRESS_FRACTION_FILTER_HIGH	0X07
#define CODE_ALARM_PRESS_OXYG_INLET				0x08
#define CODE_ALARM_PRESS_ADS_FILTER_LOW         0x09
#define CODE_ALARM_PRESS_OXYG_LOW               0x0a

#define CODE_ALARM_TEMP_ART_HIGH				0x10
#define CODE_ALARM_TEMP_ART_LOW					0x11
#define CODE_ALARM_TEMP_VEN_HIGH				0x12
#define CODE_ALARM_TEMP_VEN_LOW					0x13
#define CODE_ALARM_TEMP_NTC_HIGH				0x14
#define CODE_ALARM_TEMP_NTC_LOW					0x15
#define CODE_ALARM_TEMP_SENS_NOT_DETECTED		0x16
#define CODE_ALARM_DELTA_TEMP_REC_ART		    0x17
#define CODE_ALARM_DELTA_TEMP_REC_VEN		    0x18

#define CODE_ALARM_FLOW_PERF_ART_HIGH			0x20
#define CODE_ALARM_FLOW_PERF_ART_LOW			0x21
#define CODE_ALARM_FLOW_VEN_HIGH				0x22
#define CODE_ALARM_FLOW_VEN_LOW					0x23
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

#define CODE_ALARM_END_PRIMING_PHASE_1			0x80
#define CODE_ALARM_END_PRIMING_PHASE_2			0x81
#define CODE_ALARM_END_TREATMENT				0x85

#define CODE_ALARM_MODBUS_ACTUATOR_SEND         0x90
#define CODE_ALARM_CAN_BUS_ERROR                0x91
#define CODE_ALARM_BAD_PINCH_POS                0x92

// Filippo - definisco codice allarme per il tasto di stop
#define CODE_ALARM_EMERGENCY_BUTTON  				0x93
#define CODE_ALARM_TEST_T1		  				0x94
#define CODE_ALARM_AIR_SENSOR_TEST_KO			0x95

#define CODE_ALARM_MACHINE_COVERS               0xA0
#define CODE_ALARM_HOOKS_RESERVOIR              0xA1


// fissato a 200 ed e' il codice di errore di partenza degli allarmi protective
#define CODE_ALARM_PROT_START_VAL               0xc8

// da qui in avanti i codici delle warning
#define CODE_ALARM_PRESS_ADS_FILTER_WARN		0X105


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
#define SECURITY_PUMPS_NOT_STILL        0x400
#define SECURITY_BAD_PINCH_POS          0x800
#define SECURITY_SFA_PRIM_AIR_DET       0x1000
#define SECURITY_STOP_ALL_ACT_WAIT_CMD  0x2000
#define SECURITY_MOD_BUS_ERROR          0x4000

#define OVRD_ENABLE				0xA5
#define OVRD_NOT_ENABLED		0x5A

#define RESET_ALLOWED			0xA5
#define RESET_NOT_ALLOWED		0x5A

#define SILENCE_ALLOWED			0xA5
#define SILENCE_NOT_ALLOWED		0x5A

#define MEMO_ALLOWED			0xA5
#define MEMO_NOT_ALLOWED		0x5A


#define FLOW_LIVER_MAX   2500
#define FLOW_KIDNEY_MAX  2500

#define MAX_MSG_CONSECUTIVE_FLOW_SENS_NOT_DETECTED 			10
#define MAX_MSG_CONSECUTIVE_IR_TEMP_SENS_NOT_DETECTED 		10
#define PR_ART_HIGH											100
#define PR_ART_LOW											0
#define PR_VEN_HIGH											10  //290
//TODO da definire esattamente
#define PR_OXYG_HIGH                                        800
#define PR_OXYG_LOW                                          10

//#define PR_VEN_LOW											40
// modificato altrimenti viene sempre fuori allarme di pressione bassa
#define PR_VEN_LOW											25
#define PR_ADS_FILTER_HIGH									250
#define PR_ADS_FILTER_LOW                                   10
#define MAX_MSG_CONSECUTIVE_ACTUATOR_MODBUS_NOT_RESPOND 	10

#define PR_ADS_FILTER_WARN									100

enum ALARM
{
	 PRESS_ART_HIGH = 0,
	 PRESS_ART_LOW,
	 AIR_PRES_ART,
	 AIR_PRES_VEN,
	 SAF_AIR_SENSOR,
	 TEMP_ART_HIGH,
	 PRESS_ADS_FILTER_HIGH,
	 FLOW_PERF_ART_HIGH,
	 FLOW_SENS_NOT_DETECTED,
	 PRESS_VEN_HIGH,
	 PRESS_VEN_LOW,
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
	 PUMP_NOT_STILL,
	 BAD_PINCH_POS,
	 PRIM_AIR_ON_FILTER,
	 PRESS_ADS_FILTER_LOW,
	 PRESS_OXYG_LOW,
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

	 // da qui in avanti i codici delle warning
	 PRESS_ADS_FILTER_WARN
};

void alarmConInit(void);
void alarmEngineAlways(void);
void alarmManageNull(void);
void manageAlarmChildGuard(struct alarm * ptrAlarm);
void manageAlarmPhysicFlowPerfArtHigh(void);
void manageAlarmFlowSensNotDetected(void);
void manageAlarmIrTempSensNotDetected(void);
void manageAlarmActuatorModbusNotRespond(void);
void manageAlarmActuatorWRModbusNotRespond(void);
void manageAlarmFromProtective(void);

void manageAlarmPhysicPressSensHigh(void);
void manageAlarmPhysicPressSensLow(void);

void manageAlarmPhysicTempSens(void);
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

void SetAllAlarmEnableFlags(void);

void ForceAlarmOff(uint16_t code);
void ForceCurrentAlarmOff(void);
void DisableAllAlarm();
void DisableAllAirAlarm(bool dis);
void manageAlarmCanBus(void);
void manageAlarmPumpNotStill(void);
void manageAlarmBadPinchPos(void);
void manageAlarmPrimSFAAirDet(void);

void EnablePumpNotStillAlmFunc(void);
void EnableBadPinchPosAlmFunc(void);
void EnablePrimAlmSFAAirDetAlmFunc(void);
void DisablePumpNotStillAlmFunc(void);
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
void EnableNextWarningFunc(void);
void warningsEngineAlways(void);
void warningManageNull(void);
// Filippo - aggiunta gestione dell'allarme del tasto di stop
void manageAlarmStopButtonPressed(void);
void manageAlarmT1Test(void);
// Filippo - funzione che gestisce l'allarme per il fallimento del test del sensore aria
void manageAlarmAirSensorTestKO(void);
void manageCover_Hook_Sensor(void);
bool ResetAlmHandleFunc(uint16_t code);

#define START_WARNING_CODE		255

#endif /* APPLICATION_ALARM_CON_H_ */
