/*

. * VerificatorRx.c
 *
 *  Created on: 02/mag/2018
 *      Author: W5
 */
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "stdint.h"
#include "SwTimer.h"
#include "Global.h"
#include "RPMGauge.h"
#include "showalarm.h"
#include "Alarm_Con_protective.h"
#include "VerificatorRxCanValues.h"
#include "ControlProtectiveInterface.h"
#include "COMP_ENABLE.h"
#include "HEAT_ON_P.h"
#include "ActionsProtective.h"
#include "IncomingAlarmsManager.h"

//
// TimerCounter increased each 100ms .
// Alarm issued when TimerCounter == AlarmValue
// Alarm reset when ??
//

static void Dummy(void);
static void NoCanCommunicationAlarmAct(void);
static void MismatchPumpSpeedAlarmAct(void);
static void MismatchPinchPosAlarmAct(void);

void MismatchPressArtAlarmAct(void);
// ven press
void MismatchPressVenAlarmAct(void);
void MismatchPressVen2ndAlarmAct(void);

void MismatchPressFiltAlarmAct(void);
void MismatchPressOxyAlarmAct(void);
void MismatchPressLevelAlarmAct(void);

void MismatchTempPlateAlarmAct(void);
void MismatchTempArtAlarmAct(void);
void MismatchTempVenAlarmAct(void);
void MismatchTempFluidAlarmAct(void);
void AirPresentAlarmAct(void);

void PumpsOrPinchNotRespond_EmergAct_AirAlarm(void);

TAlarmTimer PumpMismatchAlarmTimer = {0,80,false,false,MismatchPumpSpeedAlarmAct , 0 , 65 , PumpsOrPinchNotRespond_EmergAct }; // alarm after 7 s mismatch
TAlarmTimer CanOfflineAlarmTimer = {0,20,false,false,NoCanCommunicationAlarmAct , 0 , 65 , PumpsOrPinchNotRespond_EmergAct }; // alarm after 2 s mismatch
TAlarmTimer PinchMismatchAlarmTimer = {0,20,false,false, MismatchPinchPosAlarmAct , 0 , 65 , PumpsOrPinchNotRespond_EmergAct }; // alarm after 2 s mismatch

TAlarmTimer PressArtMismatchAlarmTimer = {0,50,false,false, MismatchPressArtAlarmAct , 0 , 65 , PumpsOrPinchNotRespond_EmergAct }; // alarm after 5 s mismatch
TAlarmTimer PressVenMismatchAlarmTimer = {0,50,false,false, MismatchPressVenAlarmAct , 0 , 65 , PumpsOrPinchNotRespond_EmergAct }; // alarm after 5 s mismatch and 6.5
TAlarmTimer PressFilterMismatchAlarmTimer = {0,50,false,false, MismatchPressFiltAlarmAct , 0 , 65 , PumpsOrPinchNotRespond_EmergAct }; // alarm after 5 s mismatch
TAlarmTimer PressOxyMismatchAlarmTimer = {0,50,false,false, MismatchPressOxyAlarmAct , 0 , 65 , PumpsOrPinchNotRespond_EmergAct }; // alarm after 5 s mismatch
TAlarmTimer PressLevelMismatchAlarmTimer = {0,70,false,false, MismatchPressLevelAlarmAct , 0 , 65 , PumpsOrPinchNotRespond_EmergAct }; // alarm after 7 s mismatch

TAlarmTimer TempArtMismatchAlarmTimer = {0,20,false,false, MismatchTempArtAlarmAct , 0 , 65 , PumpsOrPinchNotRespond_EmergAct }; // alarm after 2 s mismatch
TAlarmTimer TempVenMismatchAlarmTimer = {0,20,false,false, MismatchTempVenAlarmAct , 0 , 65 , PumpsOrPinchNotRespond_EmergAct }; // alarm after 2 s mismatch
TAlarmTimer TempFluidMismatchAlarmTimer = {0,20,false,false, MismatchTempFluidAlarmAct , 0 , 65 , PumpsOrPinchNotRespond_EmergAct }; // alarm after 2 s mismatch
TAlarmTimer TempPlateMismatchAlarmTimer = {0,20,false,false, MismatchTempPlateAlarmAct , 0 , 50 , PumpsOrPinchNotRespond_EmergAct }; // alarm after 2 s mismatch

TAlarmTimer AirPresentAlarmTimer = {0,03,false,false, AirPresentAlarmAct , 0 , 65 , PumpsOrPinchNotRespond_EmergAct_AirAlarm }; // alarm after 300ms s mismatch , then check pumps stop

static TAlarmTimer	*AlarmTimerList[] = {
		&PumpMismatchAlarmTimer ,
		&CanOfflineAlarmTimer ,
		&PinchMismatchAlarmTimer,

		&PressArtMismatchAlarmTimer,
		&PressVenMismatchAlarmTimer,
		&PressFilterMismatchAlarmTimer,
		&PressOxyMismatchAlarmTimer,
		&PressLevelMismatchAlarmTimer,

		&TempArtMismatchAlarmTimer,
		&TempVenMismatchAlarmTimer,
		&TempFluidMismatchAlarmTimer,
		&TempPlateMismatchAlarmTimer,

		&AirPresentAlarmTimer  // 2-4-2019 new spex
};

static void ManageVerificatorAlarms100ms(void);
//bool ValueIsInRange(uint16_t RefValue, uint16_t Val2Test, uint16_t IPercent);
bool ValueIsInRange(short int RefValue, short int Val2Test, short unsigned int IDelta);

void InitVerificatorRx(void){
	AddSwTimer(ManageVerificatorAlarms100ms,10,TM_REPEAT);
}

static int StartupDelayCnt = 0;

int CounterDebugErrorGenerator = 0;

static void ManageVerificatorAlarms100ms(void) {

int ii;

CounterDebugErrorGenerator++;
	if( !IsVerifyRequired() ){
		// avoid control if no verify required
		return;
	}

	if(StartupDelayCnt < 100){
		// verificator idle for 10 seconds from startup
		StartupDelayCnt++;
		return;
	}

	for( ii=0; ii< sizeof(AlarmTimerList)/sizeof(TAlarmTimer*); ii++)
	{
		if (AlarmTimerList[ii]->AlarmConditionPending) {
			if (AlarmTimerList[ii]->AlarmCounter < AlarmTimerList[ii]->CountTreshold) {
				AlarmTimerList[ii]->AlarmCounter++;
				if (AlarmTimerList[ii]->AlarmCounter == AlarmTimerList[ii]->CountTreshold) {
					// manage alarm
					AlarmTimerList[ii]->AlarmActive = true;
					// manage alarm with specific action
					AlarmTimerList[ii]->AlarmAction();
				}
			}
		}
		else{
			// if AlarmActive , don't clear alarm anyway ,
			// if not AlarmActive ,  restart AlarmCounter
			AlarmTimerList[ii]->AlarmCounter = 0;
		}
		// manage secondary actions
		if( AlarmTimerList[ii]->SecondaryActionTimer > 0){
			AlarmTimerList[ii]->SecondaryActionTimer--;
			if(AlarmTimerList[ii]->SecondaryActionTimer == 0)
					AlarmTimerList[ii]->SecondaryAlarmAction();
		}
	}
}


// reset alarm condition pressing '1' on bubble keyboard
void onNewKey(char Key)
{
int ii;

	if( Key == '1' ){
		for( ii=0; ii< sizeof(AlarmTimerList); ii++)
		{
			if (AlarmTimerList[ii]->AlarmConditionPending){
				// reset alarm
				AlarmTimerList[ii]->AlarmActive = false;
				AlarmTimerList[ii]->AlarmCounter = 0;
				ShowNewAlarmError(CODE_ALARM_NO_ERROR);
			}
		}
	}
}



//////////////////////////////////////////////////
//  actions to be performed upon alarm condition
/////////////////////////////////////////////////

#define IGNORE_CAN_ALARM

void NoCanCommunicationAlarmAct(void)
{
#ifndef IGNORE_CAN_ALARM
	ShowNewAlarmError(CODE_ALARM_NOCAN_COMMUNICATION);
	DisablePinchNPumps();
	Enable_Heater(false);
	Enable_Frigo(false);
	// trigger sec action
	TriggerSecondaryAction(&CanOfflineAlarmTimer);
#endif
}

void MismatchPumpSpeedAlarmAct(void)
{
	if( GetControlFSMState() != STATE_T1TEST){
		ShowNewAlarmError(CODE_ALARM_PUMPSPEED_DONT_MATCH);
		DisablePinchNPumps();
		Enable_Heater(false);
		Enable_Frigo(false);
		// trigger sec action
		TriggerSecondaryAction(&PumpMismatchAlarmTimer);
	}
}

void MismatchPinchPosAlarmAct(void)
{
	if( GetControlFSMState() != STATE_T1TEST){
		ShowNewAlarmError(CODE_PINCH_POS_DONTMATCH);
		DisablePinchNPumps();
		Enable_Heater(false);
		Enable_Frigo(false);
		// trigger sec action
		TriggerSecondaryAction(&PinchMismatchAlarmTimer);
	}
}

void MismatchPressArtAlarmAct(void)
{
	if( GetControlFSMState() == STATE_TREATMENT )
	{
		ShowNewAlarmError(CODE_ALARM_PRESS_ART_DONTMATCH);
		DisablePinchNPumps();
		Enable_Heater(false);
		Enable_Frigo(false);
		// trigger sec action
		TriggerSecondaryAction(&PressArtMismatchAlarmTimer);
	}

}

// venous pressure
void MismatchPressVenAlarmAct(void)
{
	if( GetControlFSMState() == STATE_TREATMENT )
	{
		ShowNewAlarmError(CODE_ALARM_PRESS_VEN_DONTMATCH);
		DisablePinchNPumps();
		Enable_Heater(false);
		Enable_Frigo(false);
		// trigger sec action
		TriggerSecondaryAction(&PressVenMismatchAlarmTimer);
	}
}


// filter pressure
void MismatchPressFiltAlarmAct(void)
{
	if(GetControlFSMState() == STATE_TREATMENT)
	{
		ShowNewAlarmError(CODE_ALARM_PRESS_ADSFILT_DONTMATCH);
		DisablePinchNPumps();
		Enable_Heater(false);
		Enable_Frigo(false);
		// trigger sec action
		TriggerSecondaryAction(&PressFilterMismatchAlarmTimer);
	}
}

// oxy pressure
void MismatchPressOxyAlarmAct(void)
{
	if(GetControlFSMState() == STATE_TREATMENT)
	{
		ShowNewAlarmError(CODE_ALARM_PRESS_OXYGEN_DONTMATCH);
		DisablePinchNPumps();
		Enable_Heater(false);
		Enable_Frigo(false);
		// trigger sec action
		TriggerSecondaryAction(&PressOxyMismatchAlarmTimer);
	}
}

void MismatchPressLevelAlarmAct(void)
{
	if(GetControlFSMState() == STATE_TREATMENT)
	{
		ShowNewAlarmError(CODE_ALARM_PRESS_LEVEL_DONTMATCH);
		DisablePinchNPumps();
		Enable_Heater(false);
		Enable_Frigo(false);
		// trigger sec action
		TriggerSecondaryAction(&PressLevelMismatchAlarmTimer);
	}
}

void MismatchTempPlateAlarmAct(void)
{
	if(GetControlFSMState() == STATE_TREATMENT)
	{
		ShowNewAlarmError(CODE_ALARM_TEMP_PLATE_DONTMATCH);
		DisablePinchNPumps();
		Enable_Heater(false);
		Enable_Frigo(false);
		// trigger sec action
		TriggerSecondaryAction(&TempPlateMismatchAlarmTimer);
	}
}

void AirPresentAlarmAct(void)
{
	if(GetControlFSMState() == STATE_TREATMENT)
	{
		// do nothing , just trigger later check on pinch and pumps
		TriggerSecondaryAction(&AirPresentAlarmTimer);
	}
}

//
// if 5 seconds elapsed after air alarm from control and motors or pinch not stopped
// issue special alarm , stop pumps.
// This error should'nt never occur , but ... never say never  :(
//
void PumpsOrPinchNotRespond_EmergAct_AirAlarm(void)
{
bool some_error = false;

	//DIS HEAT , DIS COOL if pinch not safety , OFF 24V  if motors not stopped within 6,5 seconds
	// if( !PinchesAreInSafetyMode() ){    9/10/2019 --> MUST disable heater and cooler regardless of condition of pinches ( Verification )
		some_error = true;
		Enable_Heater(FALSE);
		Enable_Frigo(FALSE);
	// }
//	if( !PumpsAreStopped() ){  / don't check since it is possible that user reset alarm in the meantime
//		some_error = true;
//		SwitchOFFPinchNPumps();
//	}
	if( some_error ){
		ShowNewAlarmError(CODE_ALARM_ON_ALARMS_FSM);
	}
	else {
		// reset err condition if control stopped motors / pinch and acted as expected
		AirPresentAlarmTimer.AlarmAction = false;
		AirPresentAlarmTimer.AlarmActive = false;
	}
}


void MismatchTempArtAlarmAct(void)
{

	if(GetControlFSMState() == STATE_TREATMENT)
	{
		ShowNewAlarmError(CODE_ALARM_TEMP_ART_DONTMATCH);
		DisablePinchNPumps();
		Enable_Heater(false);
		Enable_Frigo(false);
		// trigger sec action
		TriggerSecondaryAction(&TempArtMismatchAlarmTimer);
	}
}

void MismatchTempVenAlarmAct(void)
{
	if(GetControlFSMState() == STATE_TREATMENT)
	{
		ShowNewAlarmError(CODE_ALARM_TEMP_VEN_DONTMATCH);
		DisablePinchNPumps();
		Enable_Heater(false);
		Enable_Frigo(false);
		// trigger sec action
		TriggerSecondaryAction(&TempVenMismatchAlarmTimer);
	}
}

void MismatchTempFluidAlarmAct(void)
{
	if(GetControlFSMState() == STATE_TREATMENT)
	{
		ShowNewAlarmError(CODE_ALARM_TEMP_FLUID_DONTMATCH);
		DisablePinchNPumps();
		Enable_Heater(false);
		Enable_Frigo(false);
		// trigger sec action
		TriggerSecondaryAction(&TempFluidMismatchAlarmTimer);
	}
}



///////////////////////////////////////////////////////////////////////////////////////
//  Verify can bus incoming parameters from control board to check possible mismatches
///////////////////////////////////////////////////////////////////////////////////////
void VerifyRxState(uint16_t State, uint16_t Parent, uint16_t Child,
		uint16_t Guard)
{

}



void VerifyRxPressures(uint16_t PressFilter, uint16_t PressArt, uint16_t PressVen, uint16_t PressLevelx100, uint16_t PressOxy )
{
//uint16_t *pressFilter_p, *pressArt_p,  *pressVen_p,  *pressLevelx100_p ,  *pressOxy_p;
uint16_t pressFilter, pressArt,  pressVen,  pressLevelx100 ,  pressOxy;

	GetPressures(&pressFilter, &pressArt,  &pressVen,  &pressLevelx100 ,  &pressOxy);
	// activate alarms hunting if mismatch detected
	PressFilterMismatchAlarmTimer.AlarmConditionPending = ValueIsInRange(pressFilter, PressFilter, 20) ? false : true;
	PressArtMismatchAlarmTimer.AlarmConditionPending = ValueIsInRange(pressArt, PressArt, 20) ? false : true;
	PressVenMismatchAlarmTimer.AlarmConditionPending = ValueIsInRange(pressVen, PressVen, 20) ? false : true;
	PressLevelMismatchAlarmTimer.AlarmConditionPending = ValueIsInRange(pressLevelx100, PressLevelx100, 200) ? false : true;
	PressOxyMismatchAlarmTimer.AlarmConditionPending = ValueIsInRange(pressOxy, PressOxy, 20) ? false : true;

	if( PressFilterMismatchAlarmTimer.AlarmConditionPending || PressArtMismatchAlarmTimer.AlarmConditionPending ||
			PressVenMismatchAlarmTimer.AlarmConditionPending || PressOxyMismatchAlarmTimer.AlarmConditionPending){
		int DebugVal = 0x55;
	}
}

void VerifyRxTemperatures(uint16_t TempArtx10, uint16_t TempFluidx10, uint16_t TempVenx10, uint16_t TempPlatex10)
{
//	uint16_t *tempArtx10_p, *tempFluidx10_p, *tempVenx10_p, *tempPlatex10_p;

	uint16_t tempArtx10_p, tempFluidx10_p, tempVenx10_p, tempPlatex10_p;
	GetTemperatures( &tempArtx10_p, &tempFluidx10_p, &tempVenx10_p, &tempPlatex10_p );

	TempArtMismatchAlarmTimer.AlarmConditionPending = ValueIsInRange(tempArtx10_p, TempArtx10, 40 ) ? false : true ;
	TempVenMismatchAlarmTimer.AlarmConditionPending = ValueIsInRange(tempVenx10_p, TempVenx10, 40 ) ? false : true ;
	TempFluidMismatchAlarmTimer.AlarmConditionPending = ValueIsInRange(tempFluidx10_p, TempFluidx10, 40 ) ? false : true ;
	//TempPlateMismatchAlarmTimer.AlarmConditionPending = ValueIsInRange(tempPlatex10_p, TempPlatex10, 40 ) ? false : true ;
	// causa inaccuratezza della misura ,  porto l'errore accettabile da 4 a 8 gradi , no a 15 gradi ( ah ah )
	TempPlateMismatchAlarmTimer.AlarmConditionPending = ValueIsInRange(tempPlatex10_p, TempPlatex10, 150 ) ? false : true ;
}

void VerifyRxAirLevels(uint8_t AirArtLevel, uint8_t AirVenLevel)
{
	if (GetControlFSMState() == STATE_TREATMENT) {
		if (GetTherapyType() == 0x50) {
			// liever therap
			if ((AirArtLevel > 50) || (AirVenLevel > 50))
				AirPresentAlarmTimer.AlarmConditionPending = true; // only set , deliberately don't clear if falling below treshold
		}
		else if (GetTherapyType() == 0x10) {
			// kidney therapy
			if (AirArtLevel > 50)
				AirPresentAlarmTimer.AlarmConditionPending = true; // only set , deliberately  don't clear if falling below treshold
		}
	}
}


void VerifyRxAirAlarm( uint8_t RxAirAlarm )
{
	// air alarm , don't know what to do since we have no dedicated air alarm input
}


// Nel messaggio can inviato da Control a Protective saranno presenti 2 informazioni per ogni PINCH:
// - La posizione reale della pinch : POS_RE_CONTROL ( LS nibble )
// - La posizione comandata dalla Control tramite il modbus :POS_CMD  ( MS nibble)
//
// La posizione letta dalla protective � POS_RE_PROTE ( letta dai sensori di hall )
//
// Algoritmo sulla protective:
// -(PinchVerifierStat = 1)  ENABLE_PINCH = ON :   POS_RE e POS_CMD devono corrispondere tra loro e devono corrispondere con il valore rilevato dalla protective stessa mediante i sensori di Hall.
//   Se non corrispondono --> Allarme
//
// -(PinchVerifierStat = 2) ENABLE_PINCH = OFF :   POS_RE_CONTROL e POS_RE_PROTE devono corrispondere . Non valutare POS_CMD perch� sopravanzato da ENABLE= OFF.
// - se dopo ENABLE_PINC = OFF , torna ENABLE_PINCH = ON vai a (PinchVerifierStat = 3)
//
// -(PinchVerifierStat = 3)
// COntinua a comportarsi come quando ENABLE_PINCH = OFF fino a quando si verifica un nuovo comando che riporta allo stato (PinchVerifierStat = 1)
//
// tutto questo per ogni pinch

int Pinch0VerifierStat = 0;
int Pinch1VerifierStat = 0;
int Pinch2VerifierStat = 0;

int OldCtrlPinch0_cmd = 0;
int OldCtrlPinch1_cmd = 0;
int OldCtrlPinch2_cmd = 0;


bool CheckAPinch(int* PinchVerifierStat , uint8_t CtrlPinch_pos, uint8_t CtrPinch_cmd , uint8_t OldCtrPinch_cmd , uint8_t LocPinchPos , bool PinchEnabled);

void VerifyRxPinchPos( uint8_t Pinch0Pos , uint8_t Pinch1Pos ,  uint8_t Pinch2Pos)
{

uint8_t CtrlPinch0_cmd, CtrlPinch1_cmd, CtrlPinch2_cmd;
uint8_t CtrlPinch0_pos, CtrlPinch1_pos, CtrlPinch2_pos;
uint8_t Locpinch0_pos, Locpinch1_pos , Locpinch2_pos;

bool Pinch0Ok = true;
bool Pinch1Ok = true;
bool Pinch2Ok = true;

	// extract data from received pinch values
	CtrlPinch0_cmd = (Pinch0Pos >> 4) & 0x0F;
	CtrlPinch1_cmd = (Pinch1Pos >> 4) & 0x0F;
	CtrlPinch2_cmd = (Pinch2Pos >> 4) & 0x0F;
	CtrlPinch0_pos = Pinch0Pos & 0x0F;
	CtrlPinch1_pos = Pinch1Pos & 0x0F;
	CtrlPinch2_pos = Pinch2Pos & 0x0F;

	GetPinchPos( &Locpinch0_pos ,  &Locpinch1_pos , &Locpinch2_pos);

	Pinch0Ok =  CheckAPinch( &Pinch0VerifierStat , CtrlPinch0_pos, CtrlPinch0_cmd, OldCtrlPinch0_cmd,  Locpinch0_pos , Pinch_Filter_IsEnabled());
	OldCtrlPinch0_cmd = CtrlPinch0_cmd;
	Pinch1Ok =  CheckAPinch( &Pinch1VerifierStat , CtrlPinch1_pos, CtrlPinch1_cmd, OldCtrlPinch1_cmd, Locpinch1_pos , Pinch_Arterial_IsEnabled());
	OldCtrlPinch1_cmd = CtrlPinch1_cmd;
	Pinch2Ok =  CheckAPinch( &Pinch2VerifierStat , CtrlPinch2_pos, CtrlPinch2_cmd, OldCtrlPinch1_cmd, Locpinch2_pos , Pinch_Venous_IsEnabled());
	OldCtrlPinch2_cmd = CtrlPinch2_cmd;

	if ( Pinch0Ok && Pinch1Ok && Pinch2Ok ){
		PinchMismatchAlarmTimer.AlarmConditionPending = false;
	}
	else{
		PinchMismatchAlarmTimer.AlarmConditionPending = true;
	}
}



bool CheckAPinch(int* PinchVerifierStat , uint8_t CtrlPinch_pos, uint8_t CtrlPinch_cmd , uint8_t OldCtrlPinch_cmd , uint8_t Locpinch_pos , bool PinchEnabled)
{
bool PinchOk;

	switch(*PinchVerifierStat)
	{
	case 1:
		// pinch should be enabled
		if(( CtrlPinch_pos == Locpinch_pos ) && ( CtrlPinch_pos == CtrlPinch_cmd ))
			PinchOk = true;
		else
			PinchOk = false;

		// check if status should be changed
		if(!PinchEnabled){
			*PinchVerifierStat = 2;
		}
		break;
	case 2:
		// pinch disabled
		if( CtrlPinch_pos == CtrlPinch_cmd )
			PinchOk = true;
		else
			PinchOk = false;
		// check if status should be changed
		if(PinchEnabled)
			*PinchVerifierStat = 3;
		break;
	case 3:
		// pinch reenabled but wait new command from
		if( CtrlPinch_pos == CtrlPinch_cmd )
			PinchOk = true;
		else
			PinchOk = false;
		// check if status should be changed
		if( !PinchEnabled )
			*PinchVerifierStat = 2;
		else if(CtrlPinch_cmd != OldCtrlPinch_cmd)
			// new command issued by control to pinch
			*PinchVerifierStat = 3;
		break;
	}
	return PinchOk;
}



void VerifyRxPumpsRpm(uint16_t SpeedPump0Rpmx100, uint16_t SpeedPump1Rpmx100,
		uint16_t SpeedPump2Rpmx100, uint16_t SpeedPump3Rpmx100) {
bool Pump0Ok = true;
bool Pump1Ok = true;
bool Pump2Ok = true;
bool Pump3Ok = true;

	Pump0Ok = ValueIsInRange(GetMeasuredPumpSpeed(0), SpeedPump0Rpmx100, 1000) ? true : false;
	Pump1Ok = ValueIsInRange(GetMeasuredPumpSpeed(1), SpeedPump1Rpmx100, 1000) ? true : false;
	//Pump1Ok = true; // debug SB 7 11 2018
	Pump2Ok = ValueIsInRange(GetMeasuredPumpSpeed(2), SpeedPump2Rpmx100, 1000) ? true : false;
	Pump3Ok = ValueIsInRange(GetMeasuredPumpSpeed(3), SpeedPump3Rpmx100, 1000) ? true : false;

	//Pump2Ok = true; // SB sensori di hall non funzionanti ,  soluzione temporanea 10 10 2018
	//Pump3Ok = true;

	if( Pump0Ok && Pump1Ok && Pump2Ok && Pump3Ok ){
		 PumpMismatchAlarmTimer.AlarmConditionPending = false;
	}
	else {
		// SB 2-5-2019 sometimes error occurs if control changes fast from 0 RPM to non 0 RPM and back , to overcome this issue
		// change CountTreshold depdnding on control RMP and local RPM vales.
		// if some values is too low then wait longer time before error
		if ( (SpeedPump0Rpmx100 > 610 ) && (SpeedPump1Rpmx100 > 610 ) &&
			 (SpeedPump2Rpmx100 > 610 ) && (SpeedPump3Rpmx100 > 610 ) &&
			 (GetMeasuredPumpSpeed(0) > 610) && (GetMeasuredPumpSpeed(1) > 610) &&
			 (GetMeasuredPumpSpeed(2) > 610) && (GetMeasuredPumpSpeed(3) > 610) )
			PumpMismatchAlarmTimer.CountTreshold = 80;
		else
			PumpMismatchAlarmTimer.CountTreshold = 140;

		 PumpMismatchAlarmTimer.AlarmConditionPending = true;
	}
}

void ManageRxAlarmCode(uint16_t AlarmCode)
{

}

bool OldStatusOnline = false;
void NotifyCanOnline(bool Online)
{
	if( OldStatusOnline == Online ) return;
	OldStatusOnline = Online;
	CanOfflineAlarmTimer.AlarmConditionPending = !Online;
	/* le tre righe di codice sotto servono a far si che
	 * se la PRO ha perso la comunicazione CAN e va in allarme,
	 * ma successivamente riprende la comunicaizone, allora l'allarme
	 * viene rimosso automaticamente senza intervento dell'operatore.
	 * Se si cambia gestione e si vuole l'intervento di un reset
	 * dell'operatore allora vanno rimosse*/


	if( Online ){
		CanOfflineAlarmTimer.AlarmCounter = 0;
		CanOfflineAlarmTimer.AlarmActive = false;
		ShowNewAlarmError(CODE_ALARM_NO_ERROR);
	}
}

bool GetCanOk(void)
{
	return OldStatusOnline;
}


bool ValueIsInRangePerc(uint16_t RefValue, uint16_t Val2Test, uint16_t IPercent) {
	uint16_t max;
	uint16_t min;

	max = RefValue + ((RefValue * IPercent) / 100);
	min = RefValue - ((RefValue * IPercent) / 100);
	return ((Val2Test <= max) && (Val2Test >= min));

}

//bool ValueIsInRange(uint16_t RefValue, uint16_t Val2Test, uint16_t IDelta) {
//	int16_t max;
//	int16_t min;
//
//	max = RefValue + IDelta;
//	if (RefValue > IDelta) 	min = RefValue - IDelta;
//	else min = 0;
//	if( (Val2Test <= max) && (Val2Test >= min) )
//		return true;
//	else
//		return false;
////	return ((Val2Test <= max) && (Val2Test >= min));
//
//}

bool ValueIsInRange(short int RefValue, short int Val2Test, short unsigned int IDelta) {

	if( Val2Test > RefValue ) return (( Val2Test - RefValue ) <= IDelta);
	else return (( RefValue - Val2Test ) <= IDelta);

//	return ((Val2Test <= max) && (Val2Test >= min));
}


static void Dummy(void){}

