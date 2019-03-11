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

#define TIMER_EMERGENCY_BUTTON_ALARM		10 //2 secondi di pressione consecutiva per il PANIC_BUTTON
// Filippo - inserito timeout per allarme e timeout per spegnimento PC
#define TIMER_EMERGENCY_BUTTON				60 //2 secondi di pressione consecutiva per il PANIC_BUTTON

/*Vincenzo*/
#ifdef DEMO
// messo a 12 ovvero 1.2°C solo per le DEMO
#define DELTA_TEMP_TERGET_FOR_STABILITY_PRIMING		12
// messo a 10 sec invece che un minuto solo le DEMO --> da ripristinare a 60000 ovvero un minuto
#define TIMEOUT_TEMPERATURE_RICIRC					10000 //2000L passo da 2 seocndi a 60 secondi
#else

#define DELTA_TEMP_TERGET_FOR_STABILITY_PRIMING		5 //range per identificare il raggungimento della tmeperatura target esperro in decimi di grado (es 4 = 0.4°C)
//TODO messo a 10 sec invece che un minuto solo per Test Bergamo --> da ripristinare a 60000 ovvero un minuto
#define TIMEOUT_TEMPERATURE_RICIRC					60000 //2000L passo da 2 seocndi a 60 secondi

#endif

#define DELTA_RPM_ACC_IN_PRIMING_ART	50  //200 //centesimi di RPM
#define DELTA_RPM_ACC_IN_PRIMING_VEN	100 //200 //centesimi di RPM
#define DELTA_RPM_ACC_IN_PRIMING_FLT	50  //200 //centesimi di RPM
#define TIMER_RPM_ACC_IN_PRIMING_ART    40  //200 //multipli di 50 msec ---> 200 = 10 sec --- 40 = 2 sec
#define TIMER_RPM_ACC_IN_PRIMING_VEN    40  //200 //multipli di 50 msec ---> 200 = 10 sec --- 40 = 2 sec
#define TIMER_RPM_ACC_IN_PRIMING_FLT    40  //200 //multipli di 50 msec ---> 200 = 10 sec --- 40 = 2 sec

#define DELAY_FOR_RESTART_BUZZER		12000 // timeout di 120 sec oltre il quale il buzzer viene riattivato

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
void ManageMuteButton(void);
static void manageStateEntryAndStateAlways(unsigned short stateId);

//Definizioni usate per i T1 Test sensori digitali
enum {
	HOOK1 = 0,
	HOOK2,
	PUMP_COVER1,
	PUMP_COVER2,
	PUMP_COVER3,
	PUMP_COVER4,
	FRONTAL_COVER1,
	FRONTAL_COVER2,

	NUM_SENSORI_DIGITALI
};

/* STATE LEVEL FUNCTION */
/**************************************************************************************/
/******-----------------------------TREATMENT----------------------------------********/
/**************************************************************************************/
void manageStateEntry(void);
void manageStateEntryAlways(void);

void manageStateIdle(void);
void manageStateIdleAlways(void);
// Filippo - funzione per la gestione degli allarmi in idle
void manageIdleAlarm(void);

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
void InitDigitalT1Test(void);
void manageParentDigitalT1Test(void);
void manageParenT1PinchInit(void);
void manageParenT1Pinch(void);
void manageParentT1PumpInit(void);
void manageParentT1Pump(void);
// Filippo - funzioni di test aggiuntive
void manageLevelSensorTest(void);
void manageParentT1HeaterInit(void);
void manageParentT1Heater(void);
void manageParentT1FridgeInit(void);
void manageParentT1Fridge(void);
void manageParentAirInit(void);



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

void manageParentPrimWaitPauseEntry(void);


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

void manageParentTreatWaitPauseEntry(void);
void manageParentTreatWaitStartEntry(void);

void manageParentTreatDeltaTHRcvEntry(void);
void manageParentTreatDeltaTHRcvAlways(void);

void manageParentTreatAlmDeltaTHRcvAlways(void);
void manageParentTreatAlmDeltaTHRcvEntry(void);

void manageParentTreatDeltaTHWaitEntry(void);
void manageParentTreatDeltaTHWaitAlways(void);

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
void Manage_Emergency_Button(void);
void Manage_Frontal_Cover();
void Manage_Hook_Sensors();

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

bool IsPinchPosOk(unsigned char *pArrPinchPos);


void ParentFuncT1Test(void);
void ParentFunc(void);
void AirAlarmRecoveryStateMach(void);
void SetAbandonGuard(void);

bool AmJInAlarmHandledState(void);
bool AmJInAlarmState(void);
void CheckAlarmForGuiStateMsg(void);
bool IsButtResUsedByChild(void);

CHECK_CURR_PINCH_POS_TASK_STATE CheckCurrPinchPosTask(CHECK_CURR_PINCH_POS_TASK_CMD cmd);
bool IsTreatCurrPinchPosOk(void);
void ResetTreatCurrPinchPosOk(void);
bool Start_Frigo_AMS(float DeltaT);
void StopFrigo(void);
MOD_BUS_RESPONSE WaitForModBusResponseTask(WAIT_FOR_MB_RESP_TASK_CMD WaitForMBRespTskCmd);
void LiquidTempControlTask(LIQ_TEMP_CONTR_TASK_CMD LiqTempContrTaskCmd);
LIQ_TEMP_CONTR_TASK_STATE FrigoHeatTempControlTask(LIQ_TEMP_CONTR_TASK_CMD LiqTempContrTaskCmd);
// Filippo - utilizzo una nuova funzione per poter gestire il PID nuovo che usa insieme il frigo e il riscaldatore
LIQ_TEMP_CONTR_TASK_STATE FrigoHeatTempControlTaskNewPID(LIQ_TEMP_CONTR_TASK_CMD LiqTempContrTaskCmd);
bool StartHeating(float DeltaT);

bool IsHeating(void);
bool EnableHeating(void);
bool DisableHeating(void);
bool IsFrigo();
// Filippo - inserito funzione per gestire stop attuatori a pompe ferme
//bool IsFrigoStoppedInAlarm();
bool EnableFrigo(void);
bool DisableFrigo(void);
bool StartHeating(float DeltaT);
void StopHeating(void);
void SetFan(bool On);
void Manage_Frontal_Cover(void);
void Manage_Hook_Sensors(void);

// Filippo - gestione della ricezione del messaggio CAN dalla protective contenente il valore di temperatura del piatto come letto
// dalla protective stessa
float getValTempPlateProt(void);
// Filippo - devo verificare che le temperature piatto lette dalla control e dalla protective siano le stesse
void verificaTempPlate(void);
// Filippo - funzione per eseguire il test del sensore aria
void airSensorTest(void);
// Filippo - aggiungo una nuova funzione di stop per il frigo per gestire il nuovo PID che utilizza insieme frigo e riscaldatore
void StopFrigoNewPID(unsigned char spegniFrigo);
// Filippo - Definisco questa nuova funzione per gestire il nuovo PID che utilizza sia frigo che riscaldatore e non deve sempre
// staccare l'alimentazione al frigo
bool Start_Frigo_AMSNewPID(float DeltaT,unsigned char spegniFrigo);

bool GetHeaterOn(void);
bool GetFrigoOn(void);
int  GetHeatingPwmPerc(void);
void setHeatingPwmPerc(char value);
void HeatingPwmDebug(int Perc);
void Start_Frigo_AMSDebug(int Perc);
int  GetFrigoPercDebug(void);
void setFrigoPercDebug(char value);
void setHeaterOn (bool value);
void setFrigoOn  (bool value);


#endif /* APPLICATION_APP_GES_H_ */
