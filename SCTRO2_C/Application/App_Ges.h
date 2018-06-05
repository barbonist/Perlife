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

#define SET_POINT_PRESSURE_INIT				0
#define DELTA_PRESSURE						30
#define GAIN_PRESSURE						1

#define TIMER_PANIC_BUTTON					40 //2 secondi di pressione consecutiva per il PANIC_BUTTON
/* MACHINE STATE FUNCTION */
void manageNull(void);
void manageStateLevel(void);

/* CORE FUNCTION */
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

void manageStatePrimingWait(void);
void manageStatePrimingWaitAlways(void);

void manageStatePrimingRicircolo(void);
void manageStatePrimingRicircoloAlways(void);

void manageStateWaitTreatmentAlways(void);
void manageStateWaitTreatment(void);

void manageStateUnmountDisposableEntry(void);
void manageStateUnmountDisposableAlways(void);


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

void computeMachineStateGuardPriming_1_Wait(void);
void manageStatePriming_1_WaitEntry(void);
void manageStatePriming_1_WaitAlways(void);


/* PARENT LEVEL FUNCTION */
/**************************************************************************************/
/******-----------------------------T1 TEST NO DISP----------------------------********/
/**************************************************************************************/
void initT1Test(void);
void manageParentT1InitEntry(void);
void manageParentT1NoDispEntry(void);
void manageParentT1NoDispAlways(void);
void manageParentT1EndEntry(void);
void manageParentChkConfig(void);
void manageParentChk24Vbrk(void);
void manageParentChkPress(void);
void manageParentTempSensIR(void);
void mangeParentUFlowSens(void);
void manageParentAir(void);
void manageParenT1PinchInit(void);
void manageParenT1Pinch(void);
void manageParentT1PumpInit(void);
void manageParentT1Pump(void);

/**************************************************************************************/
/******-----------------------------TREATMENT----------------------------------********/
/**************************************************************************************/
void manageParentPrimingEntry(void);
void SetPinchPosInPriming(void);
void manageParentPrimingAlways(void);
void manageParPrimWaitMotStopEntry(void);
void manageParPrimWaitMotStopEntryAlways(void);
void manageParPrimWaitPinchCloseEntry(void);
bool AreAllPinchClose( void );
void ResetPrimPinchAlm(void);
void manageParPrimWaitPinchCloseAlways(void);
void  manageParPrimEndRecAlarmEntry(void);
void manageParPrimEndRecAlarmAlways(void);
void manageParentPrimAirFiltEntry(void);
void manageParentPrimAirFiltAlways(void);
void manageParentPrimAirAlmRecEntry(void);
void manageParentPrimAirAlmRecAlways(void);


void manageParentPrimingAlarmEntry(void);
void manageParentPrimingAlarmAlways(void);

bool IsTreatSetPinchPosTaskAlm(void);
void ResetTreatSetPinchPosTaskAlm(void);
TREAT_SET_PINCH_POS_TASK_STATE TreatSetPinchPosTask(TREAT_SET_PINCH_POS_CMD cmd);
void manageParentTreatEntry(void);
void manageParentTreatAlways(void);
void manageParentTreatAlarmEntry(void);
void manageParentTreatAlarmAlways(void);

void setPumpPressLoop(unsigned char pmpId, unsigned char valOnOff);
unsigned char getPumpPressLoop(unsigned char pmpId);

void manageParentEntry(void);
void manageParentEntryAlways(void);

void ParentEmptyDispStateMach(void);
void manageParentEmptyDisposInitEntry(void);
void manageParentEmptyDisposInitAlways(void);
void manageParentEmptyDisposRunEntry(void);
void manageParentEmptyDisposRunAlways(void);
void manageParentEmptyDisposAlarmEntry(void);
void manageParentEmptyDisposAlarmAlways(void);
void manageParentEmptyDisposEndEntry(void);
void manageParentEmptyDisposEndAlways(void);

void manageParentTreatAirFiltEntry(void);
void manageParentTreatAirFiltAlways(void);

void manageParentTreatSFVEntry(void);
void manageParentTreatSFVAlways(void);

void manageParentTreatSFAEntry(void);
void manageParentTreatSFAAlways(void);

void manageParentTreatAirAlmRecEntry(void);
void manageParentTreatAirAlmRecAlways(void);

void manageParentTreatEndEntry(void);
void manageParentTreatEndAlways(void);



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
unsigned char getGUIButton(unsigned char buttonId);
void releaseGUIButton(unsigned char buttonId);

//void setParamFloatFromGUI(unsigned char parId, float value);
//void resetParamFloatFromGUI(unsigned char parId);
//char checkParTypeFromGUI(unsigned char parId);

/**************************************************************************************/
/******-------------------------DRIVERS MANAGEMENT-----------------------------********/
/**************************************************************************************/
void Display_7S_Management();
void Cover_Sensor_GetVal();
void Voltage_BM_Chk(void);

void EEPROM_Read(LDD_FLASH_TAddress Source, LDD_TData *Dest, LDD_FLASH_TDataSize Count);
void Set_Data_EEPROM_Default(void);
void Manage_Panic_Button(void);

#include "EEPROM.h"
byte EEPROM_GetFlash(LDD_FLASH_TAddress Source, LDD_TData *Dest, LDD_FLASH_TDataSize Count);
byte EEPROM_SetFlash(EEPROM_TDataAddress Source, EEPROM_TAddress Dest, word Count);
void EEPROM_write(EEPROM_TDataAddress Src, EEPROM_TAddress Dst, word Count);
void EEPROM_Read(LDD_FLASH_TAddress Source, LDD_TData *Dest, LDD_FLASH_TDataSize Count);
/*******************/
/* VARIABLE */
/******************/

void CallInIdleState(void);
word GetTotalPrimingVolumePerf(int cmd);
void CheckOxygenationSpeed(word value);
void CheckDepurationSpeed(word value, bool ForceValue, bool DisableUpdateCmd);
void GoToRecoveryParentState(int MachineParentState);

void HandlePinch( int cmd);
unsigned char TemperatureStateMach(int cmd);
void CheckTemperatureSet(void);

void LiquidTempContrTask(LIQUID_TEMP_CONTR_CMD LiqTempContrCmd);
bool IsPumpStopAlarmActive(void);
void ClearPumpStopAlarm(void);
bool AreAllPumpsStopped( void );
CHECK_PUMP_STOP_STATE CheckPumpStopTask(CHECK_PUMP_STOP_CMD cmd);

//bool IsPinchPosOk(void);
//CHECK_PINCH_POS_STATE CheckPinchPosTask(CHECK_PINCH_CMD cmd);

void ParentFuncT1Test(void);
void ParentFunc(void);
void AirAlarmRecoveryStateMach(void);

#endif /* APPLICATION_APP_GES_H_ */
