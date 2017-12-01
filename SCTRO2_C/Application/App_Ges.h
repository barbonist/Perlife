/*
 * App_Ges.h
 *
 *  Created on: 03/nov/2016
 *      Author: W15
 */

#ifndef APPLICATION_APP_GES_H_
#define APPLICATION_APP_GES_H_

/* DEFINES */
#define MASK_SECURITY_STOP_ALL_ACTUATOR		0x01
#define MASK_SECURITY_STOP_ALL				0x02
#define MASK_SECURITY_STOP_PERF_PUMP		0x04
#define MASK_SECURITY_STOP_PURIF_PUMP		0x08
#define MASK_SECURITY_STOP_OXYG_PUMP		0x10
#define MASK_SECURITY_STOP_PELTIER			0x20

/* MACHINE STATE FUNCTION */
void manageNull(void);
void manageStateLevel(void);

/* CORE FUNCTION */
void manageCommWithPc(void);
void manageCommWithProtection(void);
void readAnalogSensor(void);
void readDigitalSensor(void);
void computeMachineStateGuard(void);
static void computeMachineStateGuardEntryState(void);
static void computeMachineStateGuardIdle(void);
static void computeMachineStateGuardSelTreat(void);
static void computeMachineStateGuardMountDisp(void);
static void computeMachineStateGuardTankFill(void);
static void computeMachineStateGuardPrimingPh1(void);
static void computeMachineStateGuardPrimingPh2(void);
static void computeMachineStateGuardTreatment(void);
void processMachineState(void);
static void manageStateEntryAndStateAlways(unsigned short stateId);


/* STATE LEVEL FUNCTION */
/**************************************************************************************/
/******-----------------------------TREATMENT----------------------------------********/
/**************************************************************************************/
void manageStateEntry(void);
void manageStateEntryAlways(void);

void manageStateIdle(void);
void manageStateIdleAlways(void);

void manageStateSelTreat(void);
void manageStateSelTreatAlways(void);

void manageStateT1NoDisp(void);
void manageStateT1NoDispAlways(void);

void manageStateMountDisp(void);
void manageStateMountDispAlways(void);

void manageStateTankFill(void);
void manageStateTankFillAlways(void);

void managePrimingPh1(void);
void managePrimingPh1Always(void);

void managePrimingPh2(void);
void managePrimingPh2Always(void);

void manageStateTreatKidney1(void);
void manageStateTreatKidney1Always(void);

void manageStateEmptyDisp(void);
void manageStateEmptyDispAlways(void);
/**************************************************************************************/
/******-----------------------------TREATMENT----------------------------------********/
/**************************************************************************************/


void manageStateT1WithDisp(void);
void manageStateT1WithDispAlways(void);

void manageStatePrimingTreat1(void);
void manageStatePrimingTreat1Always(void);

void manageStateEmptyDisp1(void);
void manageStateEmptyDisp1Always(void);

void manageStatePrimingTreat2(void);
void manageStatePrimingTreat2Always(void);

void manageStateTreat2(void);
void manageStateTreat2Always(void);

void manageStateEmptyDisp2(void);
void manageStateEmptyDisp2Always(void);

void manageStateWashing(void);
void manageStateWashingAlways(void);

void manageStateFatalError(void);
void manageStateFatalErrorAlways(void);

/* PARENT LEVEL FUNCTION */
/**************************************************************************************/
/******-----------------------------TREATMENT----------------------------------********/
/**************************************************************************************/
void manageParentPrimingEntry(void);
void manageParentPrimingAlways(void);
void manageParentPrimingAlarmEntry(void);
void manageParentPrimingAlarmAlways(void);

void manageParentTreatEntry(void);
void manageParentTreatAlways(void);
void manageParentTreatAlarmEntry(void);
void manageParentTreatAlarmAlways(void);

void setPumpPressLoop(unsigned char pmpId, unsigned char valOnOff);
unsigned char getPumpPressLoop(unsigned char pmpId);
void alwaysPumpPressLoop(unsigned char pmpId);

void manageParentEntry(void);
void manageParentEntryAlways(void);
/**************************************************************************************/
/******-----------------------------TREATMENT----------------------------------********/
/**************************************************************************************/

/* CHILD LEVEL FUNCTION */
void manageChildNull(void);
void manageChildEntry(void);
void manageChildEntryAlways(void);

/* NESTED LEVEL FUNCTION */

/* MACHINE STATE FUNCTION */
void initAllState(void);
void initAllGuard(void);

/*  GUI FUNCTION */
void initGUIButton(void);
void setGUIButton(unsigned char buttonId);
void releaseGUIButton(unsigned char buttonId);

void initSetParamFromGUI(void);
void setParamWordFromGUI(unsigned char parId, int value);
void setParamFloatFromGUI(unsigned char parId, float value);
void resetParamWordFromGUI(unsigned char parId);
void resetParamFloatFromGUI(unsigned char parId);
char checkParTypeFromGUI(unsigned char parId);

/**************************************************************************************/
/******-------------------------DRIVERS MANAGEMENT-----------------------------********/
/**************************************************************************************/
void Display_7S_Management();
void Cover_Sensor_GetVal();
unsigned char Bubble_Keyboard_GetVal(unsigned char Button);
void Buzzer_Management();
void Heater_ON();
void Set_Lamp(unsigned char level);
void Reset_Lamp(unsigned char level);
void RTS_Motor_Management(unsigned char action);
void EN_Clamp_Control (unsigned char action);
void EN_Motor_Control(unsigned char action);
void EN_24_M_C_Management(unsigned char action);
void Coversion_From_ADC_To_mmHg_Pressure_Sensor(void);
/*******************/
/* VARIABLE */
/******************/

#endif /* APPLICATION_APP_GES_H_ */
