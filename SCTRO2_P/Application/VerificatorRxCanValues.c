/*
 * VerificatorRx.c
 *
 *  Created on: 02/mag/2018
 *      Author: W5
 */
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
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


TAlarmTimer PumpMismatchAlarmTimer = {0,70,false,false,MismatchPumpSpeedAlarmAct , 0 , 0 , Dummy}; // alarm after 6 s mismatch
TAlarmTimer CanOfflineAlarmTimer = {0,20,false,false,NoCanCommunicationAlarmAct , 0 , 0 , Dummy}; // alarm after 2 s mismatch
TAlarmTimer PinchMismatchAlarmTimer = {0,20,false,false, MismatchPinchPosAlarmAct , 0 , 0 , Dummy}; // alarm after 2 s mismatch

TAlarmTimer PressArtMismatchAlarmTimer = {0,50,false,false, MismatchPressArtAlarmAct , 0 , 0 , Dummy}; // alarm after 5 s mismatch
TAlarmTimer PressVenMismatchAlarmTimer = {0,50,false,false, MismatchPressVenAlarmAct , 0 , 65 , MismatchPressVen2ndAlarmAct }; // alarm after 5 s mismatch and 6.5
TAlarmTimer PressFilterMismatchAlarmTimer = {0,50,false,false, MismatchPressFiltAlarmAct , 0 , 0 , Dummy}; // alarm after 5 s mismatch
TAlarmTimer PressOxyMismatchAlarmTimer = {0,50,false,false, MismatchPressOxyAlarmAct , 0 , 0 , Dummy}; // alarm after 5 s mismatch
TAlarmTimer PressLevelMismatchAlarmTimer = {0,70,false,false, MismatchPressLevelAlarmAct , 0 , 0 , Dummy}; // alarm after 7 s mismatch

TAlarmTimer TempArtMismatchAlarmTimer = {0,20,false,false, MismatchTempArtAlarmAct , 0 , 0 , Dummy}; // alarm after 2 s mismatch
TAlarmTimer TempVenMismatchAlarmTimer = {0,20,false,false, MismatchTempVenAlarmAct , 0 , 0 , Dummy}; // alarm after 2 s mismatch
TAlarmTimer TempFluidMismatchAlarmTimer = {0,20,false,false, MismatchTempFluidAlarmAct , 0 , 0 , Dummy}; // alarm after 2 s mismatch
TAlarmTimer TempPlateMismatchAlarmTimer = {0,20,false,false, MismatchTempPlateAlarmAct , 0 , 0 , Dummy}; // alarm after 2 s mismatch

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
		&TempPlateMismatchAlarmTimer
};

static void ManageVerificatorAlarms100ms(void);
bool ValueIsInRange(uint16_t RefValue, uint16_t Val2Test, uint16_t IPercent);

void InitVerificatorRx(void){
	AddSwTimer(ManageVerificatorAlarms100ms,10,TM_REPEAT);
}

static int StartupDelayCnt = 0;
static void ManageVerificatorAlarms100ms(void) {

int ii;

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

void NoCanCommunicationAlarmAct(void)
{
	ShowNewAlarmError(CODE_ALARM_NOCAN_COMMUNICATION);
	DisablePinchNPumps();
}

void MismatchPumpSpeedAlarmAct(void)
{
	ShowNewAlarmError(CODE_ALARM_PUMPSPEED_DONT_MATCH);
	DisablePinchNPumps();
}

void MismatchPinchPosAlarmAct(void)
{
	ShowNewAlarmError(CODE_PINCH_POS_DONTMATCH);
	DisablePinchNPumps();
}

void MismatchPressArtAlarmAct(void)
{
	ShowNewAlarmError(CODE_ALARM_PRESS_ART_DONTMATCH);
	DisablePinchNPumps();
}

// venous pressure
void MismatchPressVenAlarmAct(void)
{
	ShowNewAlarmError(CODE_ALARM_PRESS_VEN_DONTMATCH);
	DisablePinchNPumps();
	// trigger sec action
	if(PressVenMismatchAlarmTimer.SecondaryActionTimerTreshold != 0){
		PressVenMismatchAlarmTimer.SecondaryActionTimer = PressVenMismatchAlarmTimer.SecondaryActionTimerTreshold;
	}
}

void MismatchPressVen2ndAlarmAct(void)
{
	//DIS HEAT , DIS COOL if pinch not safety , OFF 24V  if motors not stopped within 6,5 seconds
	if( !PinchesAreInSafetyMode() ){
		Enable_Heater(FALSE);
		Enable_Frigo(FALSE);
	}
	if( !PumpsAreStopped() ){
		SwitchOFFPinchNPumps();
	}
}

// filter pressure
void MismatchPressFiltAlarmAct(void)
{
	ShowNewAlarmError(CODE_ALARM_PRESS_ADSFILT_DONTMATCH);
	DisablePinchNPumps();
}

// oxy pressure
void MismatchPressOxyAlarmAct(void)
{
	ShowNewAlarmError(CODE_ALARM_PRESS_OXYGEN_DONTMATCH);
	DisablePinchNPumps();
}

void MismatchPressLevelAlarmAct(void)
{
	ShowNewAlarmError(CODE_ALARM_PRESS_LEVEL_DONTMATCH);
	DisablePinchNPumps();
}

void MismatchTempPlateAlarmAct(void)
{
	ShowNewAlarmError(CODE_ALARM_TEMP_PLATE_DONTMATCH);
	DisablePinchNPumps();
}

void MismatchTempArtAlarmAct(void)
{
	ShowNewAlarmError(CODE_ALARM_TEMP_ART_DONTMATCH);
	DisablePinchNPumps();
}

void MismatchTempVenAlarmAct(void)
{
	ShowNewAlarmError(CODE_ALARM_TEMP_VEN_DONTMATCH);
	DisablePinchNPumps();
}

void MismatchTempFluidAlarmAct(void)
{
	ShowNewAlarmError(CODE_ALARM_TEMP_FLUID_DONTMATCH);
	DisablePinchNPumps();
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

void VerifyRxAirAlarm( uint8_t RxAirAlarm )
{
	// air alarm , don't know what to do since we have no dedicated air alarm input
}

void VerifyRxPinchPos( uint8_t Pinch0Pos , uint8_t Pinch1Pos ,  uint8_t Pinch2Pos)
{
uint8_t Locpinch0_pos, Locpinch1_pos , Locpinch2_pos;

bool Pinch0Ok = true;
bool Pinch1Ok = true;
bool Pinch2Ok = true;

	GetPinchPos( &Locpinch0_pos ,  &Locpinch1_pos , &Locpinch2_pos);

	if (( Pinch0Pos == Locpinch0_pos ) && ( Pinch1Pos == Locpinch1_pos ) && ( Pinch2Pos == Locpinch2_pos )){
		PinchMismatchAlarmTimer.AlarmConditionPending = false;
	}
	else{
		PinchMismatchAlarmTimer.AlarmConditionPending = true;
	}
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
		 PumpMismatchAlarmTimer.AlarmConditionPending = true;
	}
}

void ManageRxAlarmCode(uint16_t AlarmCode)
{

}

void NotifyCanOnline(bool Online)
{
	CanOfflineAlarmTimer.AlarmConditionPending = !Online;
	/* le tre righe di codice sotto servono a far si che
	 * se la PRO ha perso la comunicazione CAN e va in allarme,
	 * ma successivamente riprende la comunicaizone, allora l'allarme
	 * viene rimosso automaticamente senza intervento dell'operatore.
	 * Se si cambia gestione e si vuole l'intervento di un reset
	 * dell'operatore allora vanno rimosse*/
	CanOfflineAlarmTimer.AlarmCounter = 0;
	CanOfflineAlarmTimer.AlarmActive = false;
	ShowNewAlarmError(CODE_ALARM_NO_ERROR);
}


bool ValueIsInRangePerc(uint16_t RefValue, uint16_t Val2Test, uint16_t IPercent) {
	uint16_t max;
	uint16_t min;

	max = RefValue + ((RefValue * IPercent) / 100);
	min = RefValue - ((RefValue * IPercent) / 100);
	return ((Val2Test <= max) && (Val2Test >= min));

}

bool ValueIsInRange(uint16_t RefValue, uint16_t Val2Test, uint16_t IDelta) {
	int16_t max;
	int16_t min;

	max = RefValue + IDelta;
	if (RefValue > IDelta) 	min = RefValue - IDelta;
	else min = 0;
	if( (Val2Test <= max) && (Val2Test >= min) )
		return true;
	else
		return false;
//	return ((Val2Test <= max) && (Val2Test >= min));

}


static void Dummy(void){}

