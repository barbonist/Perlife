/*
 * Alarm_Con.h
 *
 *  Created on: 10/nov/2016
 *      Author: W15
 */

#ifndef APPLICATION_ALARM_CON_PROTECTIVE_H_
#define APPLICATION_ALARM_CON_PROTECTIVE_H_

#include "Global.h"

//#define CODE_ALARM0		0x00
//#define CODE_ALARM1		0x01
//#define CODE_ALARM2		0x02
#define ALARM_ACTIVE_IN_STRUCT					24
#define CODE_ALARM_NO_ERROR						0x00
#define CODE_ALARM_PRESS_ART_HIGH				0X01
#define CODE_ALARM_PRESS_ART_LOW				0X02
#define CODE_ALARM_PRESS_VEN_HIGH				0X03
#define CODE_ALARM_PRESS_VEN_LOW				0X04
#define CODE_ALARM_PRESS_ADS_FILTER_HIGH		0X05
#define CODE_ALARM_PRESS_PLASMA_FILTER_HIGH		0X06
#define CODE_ALARM_PRESS_FRACTION_FILTER_HIGH	0X07
#define CODE_ALARM_PRESS_OXYG_INLET				0x08

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

#define CODE_ALARM_STOP_ALL_ACTUATOR			0X60 /* pump, pinch, peltier */
#define CODE_ALARM_STOP_ALL_PUMP				0X61 /* pump */
#define CODE_ALARM_STOP_PERF_PUMP				0x62 /* stop perfusion pump*/
#define CODE_ALARM_STOP_OXYG_PUMP				0x63 /* stop oxygenation pump */
#define CODE_ALARM_STOP_PURIF_PUMP				0x64 /* stop purification pump */
#define CODE_ALARM_STOP_PELTIER					0x65 /* stop peltier */

#define CODE_ALARM_PUMP_PERF_COVER_OPEN			0x70
#define CODE_ALARM_PUMP_PURIF_COVER_OPEN		0x71
#define CODE_ALARM_PUMP_OXYG_COVER_OPEN			0x73

#define CODE_ALARM_END_PRIMING_PHASE_1			0x80
#define CODE_ALARM_END_PRIMING_PHASE_2			0x81
#define CODE_ALARM_END_TREATMENT				0x85

#define CODE_ALARM_MODBUS_ACTUATOR_SEND         0x90
#define CODE_ALARM_CAN_BUS_ERROR                0x91


// protective codes

#define CODE_ALARM_NOCAN_COMMUNICATION			0x1C0 // 177 for obvious reasons can't be sent to control board

#define CODE_ALARM_PUMPSPEED_DONT_MATCH			0x1C1 // 185
#define CODE_ALARM_PRESS_ART_DONTMATCH			0x1C2
#define CODE_ALARM_PRESS_VEN_DONTMATCH			0x1C3
#define CODE_ALARM_PRESS_ADSFILT_DONTMATCH		0x1C4
#define CODE_ALARM_PRESS_OXYGEN_DONTMATCH		0x1C5
#define CODE_ALARM_PRESS_LEVEL_DONTMATCH		0x1C6

#define CODE_ALARM_TEMP_PLATE_DONTMATCH			0x1C7
#define CODE_ALARM_TEMP_ART_DONTMATCH			0x1C8
#define CODE_ALARM_TEMP_VEN_DONTMATCH			0x1C9
#define CODE_ALARM_TEMP_FLUID_DONTMATCH			0x1CA

#define CODE_PINCH_POS_DONTMATCH				0x1CB

#define CODE_ALARM_PRESS_ART_XHIGH			    0x1DD
#define CODE_ALARM_PRESS_VEN_XHIGH			    0x1DE
#define CODE_ALARM_PRESS_ADSFILT_XHIGH  		0x1DF
#define CODE_ALARM_PRESS_OXYGEN_XHIGH 			0x1E0
#define CODE_ALARM_PRESS_LEVEL_XHIGH			0x1E1

#define CODE_ALARM_TEMP_PLATE_XHIGH				0x1E2
#define CODE_ALARM_TEMP_ART_XHIGH				0x1E3
#define CODE_ALARM_TEMP_VEN_XHIGH				0x1E4
#define CODE_ALARM_TEMP_FLUID_XHIGH				0x1E5

#define CODE_ALARM_TEMP_PLATE_XLOW				0x1E6
#define CODE_ALARM_TEMP_ART_XLOW				0x1E7
#define CODE_ALARM_TEMP_VEN_XLOW				0x1E8
#define CODE_ALARM_TEMP_FLUID_XLOW				0x1E9

#define CODE_ALARM_GEN_HWFAILURE				0x1F0


#define CODE_ALARM_

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
#define SECURITY_SFV_AIR_DET           0x100
#define SECURITY_SFA_AIR_DET           0x200
#define SECURITY_STOP_ALL_ACT_WAIT_CMD 0x400

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
#define MAX_MSG_CONSECUTIVE_ACTUATOR_MODBUS_NOT_RESPOND 	10

//#define LEGACY_VERS

#ifdef LEGACY_VERS
#define PR_ART_HIGH											300  //100
#define PR_ART_XHIGH										(PR_ART_HIGH	+ 30)
#define PR_ART_LOW											0

#define PR_VEN_HIGH											10
#define PR_VEN_XHIGH										(PR_VEN_HIGH	+ 2)

//#define PR_OXYG_HIGH                                        800
#define PR_OXYG_HIGH                                        500

#define PR_OXYG_XHIGH										(PR_OXYG_HIGH + 80)

#define PR_ADS_FILTER_HIGH									250
#define PR_ADS_FILTER_XHIGH									(PR_ADS_FILTER_HIGH + 25)

#define PR_TUB_XHIGH										4

#define PR_ADS_FILTER_LOW                                   10
#define PR_ADS_FILTER_WARN									100

#else
#define PR_ART_HIGH											350  //100
#define PR_ART_XHIGH										(PR_ART_HIGH	+ 30)
#define PR_ART_LOW											0

#define PR_VEN_HIGH											350
#define PR_VEN_XHIGH										(PR_VEN_HIGH	+ 2)

//#define PR_OXYG_HIGH                                        800
#define PR_OXYG_HIGH                                        700

#define PR_OXYG_XHIGH										(PR_OXYG_HIGH + 80)

#define PR_ADS_FILTER_HIGH									250
#define PR_ADS_FILTER_XHIGH									(PR_ADS_FILTER_HIGH + 75)

#define PR_TUB_XHIGH										4

#define PR_ADS_FILTER_LOW                                   10
#define PR_ADS_FILTER_WARN									100


#endif


// temperature
#define TEMPER_ART_HIGH										40.0
#define TEMPER_ART_XHIGH									(TEMPER_ART_HIGH	+1)
#define TEMPER_ART_LOW										3.0
#define TEMPER_ART_XLOW										(TEMPER_ART_LOW	-1)

#define TEMPER_VEN_HIGH										40.0
#define TEMPER_VEN_XHIGH									(TEMPER_VEN_HIGH + 1)
#define TEMPER_VEN_LOW										3.0
#define TEMPER_VEN_XLOW										(TEMPER_VEN_LOW	-1)

#define TEMPER_FLUID_HIGH									40.0
#define TEMPER_FLUID_XHIGH									(TEMPER_FLUID_HIGH + 1)
#define TEMPER_FLUID_LOW									3.0
#define TEMPER_FLUID_XLOW									(TEMPER_FLUID_LOW - 1)

#define MAX_PLATE_TEMP  									70.0
#define XMAX_PLATE_TEMP  									(MAX_PLATE_TEMP + 15)

#define MIN_PLATE_TEMP  									-5.0
#define XMIN_PLATE_TEMP 									(MIN_PLATE_TEMP - 10)

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
	 MODBUS_ACTUATOR_SEND,

	 PUMP_NOT_STILL,
	 BAD_PINCH_POS,
	 PRIM_AIR_ON_FILTER,
	 PRESS_ADS_FILTER_LOW,
	 PRESS_OXYG_LOW,
	 ALARM_FROM_PROTECTIVE,

	 // da qui in avanti i codici delle warning
	 PRESS_ADS_FILTER_WARN

};

void alarmConInit(void);
void alarmEngineAlways(void);
void alarmManageNull(void);
//void manageAlarmChildGuard(struct alarm * ptrAlarm);
void manageAlarmPhysicFlowPerfArtHigh(void);
void manageAlarmFlowSensNotDetected(void);
void manageAlarmIrTempSensNotDetected(void);
void manageAlarmActuatorModbusNotRespond(void);

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

void ForceAlarmOff(unsigned char code);
void ForceCurrentAlarmOff(void);
void DisableAllAlarm();
void DisableAllAirAlarm(bool dis);
void manageAlarmCanBus(void);

#endif /* APPLICATION_ALARM_CON_PROTECTIVE_H_ */
