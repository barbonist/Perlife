/*
 * VerificatorLocalSensors.c
 *
 *  Created on: 02/set/2018
 *      Author: W5
 */


#define MAX_ARTERIAL_PRESSURE_PROTE
#define MIN_ARTERIAL_PRESSURE_PROTE
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "stdint.h"
#include "SwTimer.h"
#include "Global.h"
#include "RPMGauge.h"
#include "showalarm.h"
#include "VerificatorRxCanValues.h"
#include "Alarm_Con_Protective.h"
#include "ActionsProtective.h"
#include "ControlProtectiveInterface.h"
#include "IncomingAlarmsManager.h"

static void Dummy(void);

void HighVenousPressAlarmAct(void);
void HighArterPressAlarmAct(void);
void HighAdsFiltPressAlarmAct(void);
void HighOxygenPressAlarmAct(void);
void HighLevelPressAlarmAct(void);

void HighPlateTempAlarmAct(void);
void HighArtTempAlarmAct(void);
void HighVenTempAlarmAct(void);
void HighFluidTempAlarmAct(void);

void LowPlateTempAlarmAct(void);
void LowArtTempAlarmAct(void);
void LowVenTempAlarmAct(void);
void LowFluidTempAlarmAct(void);

TAlarmTimer HighArterPressAlarmTimer = {true, 0,20,false,false,  HighArterPressAlarmAct , 0 , 65 , PumpsOrPinchNotRespond_EmergAct}; // alarm after 1 s alarm
TAlarmTimer HighVenousPressAlarmTimer = {true, 0,20,false,false, HighVenousPressAlarmAct , 0 ,  65 , PumpsOrPinchNotRespond_EmergAct}; // alarm after 1 s alarm
TAlarmTimer HighAdsFiltPressAlarmTimer = {true, 0,20,false,false, HighAdsFiltPressAlarmAct , 0 , 65 , PumpsOrPinchNotRespond_EmergAct}; // alarm after 1 s alarm
TAlarmTimer HighOxygenPressAlarmTimer = {true, 0,20,false,false, HighOxygenPressAlarmAct , 0 , 65 , PumpsOrPinchNotRespond_EmergAct}; // alarm after 1 s alarm
TAlarmTimer HighLevelPressAlarmTimer = {true, 0,40,false,false, HighLevelPressAlarmAct , 0 , 65 , PumpsOrPinchNotRespond_EmergAct}; // alarm after 4 s alarm
TAlarmTimer HighPlateTempAlarmTimer = {true, 0,10,false,false, HighPlateTempAlarmAct , 0 , 65 , PumpsOrPinchNotRespond_EmergAct}; // alarm after 1 s alarm
TAlarmTimer HighArtTempAlarmTimer = {true, 0,10,false,false, HighArtTempAlarmAct , 0 , 65 , PumpsOrPinchNotRespond_EmergAct}; // alarm after 1 s alarm
TAlarmTimer HighVenTempAlarmTimer = {true, 0,10,false,false, HighVenTempAlarmAct , 0 , 65 , PumpsOrPinchNotRespond_EmergAct}; // alarm after 1 s alarm
TAlarmTimer HighFluidTempAlarmTimer = {true, 0,10,false,false, HighFluidTempAlarmAct , 0 , 65 , PumpsOrPinchNotRespond_EmergAct}; // alarm after 1 s alarm
TAlarmTimer LowPlateTempAlarmTimer = {true, 0,10,false,false, LowPlateTempAlarmAct , 0 , 65 , PumpsOrPinchNotRespond_EmergAct}; // alarm after 1 s alarm
TAlarmTimer LowArtTempAlarmTimer = {true, 0,200,false,false, LowArtTempAlarmAct , 0 , 65 , PumpsOrPinchNotRespond_EmergAct}; // alarm after 1 s alarm
TAlarmTimer LowVenTempAlarmTimer = {true, 0,200,false,false, LowVenTempAlarmAct , 0 , 65 , PumpsOrPinchNotRespond_EmergAct}; // alarm after 1 s alarm
TAlarmTimer LowFluidTempAlarmTimer = {true, 0,200,false,false, LowFluidTempAlarmAct , 0 , 65 , PumpsOrPinchNotRespond_EmergAct}; // alarm after 1 s alarm

static TAlarmTimer	*AlarmTimerList[] = {
		&HighArterPressAlarmTimer,
		&HighVenousPressAlarmTimer,
		&HighAdsFiltPressAlarmTimer,
		&HighOxygenPressAlarmTimer,
		&HighLevelPressAlarmTimer,
		&HighPlateTempAlarmTimer,
		&HighArtTempAlarmTimer,
		&HighVenTempAlarmTimer,
		&HighFluidTempAlarmTimer,
		&LowPlateTempAlarmTimer,
		&LowArtTempAlarmTimer,
		&LowVenTempAlarmTimer,
		&LowFluidTempAlarmTimer
};

static void ManageLocalVerificatorAlarms100ms(void) ;

static int StartupDelayCnt = 0;


#define TEMPERATURE_SENSORS_AVAILABLE
//#undef TEMPERATURE_SENSORS_AVAILABLE

void InitVerificatorLocalParams(void)
{
	AddSwTimer(ManageLocalVerificatorAlarms100ms,10,TM_REPEAT);
}

static void ManageLocalVerificatorAlarms100ms(void)
{
int ii;

//	if( !IsVerifyRequired() ){
//		// avoid control if no verify required
//		return;
//	}

	if(StartupDelayCnt < 100){
		// verificator idle for 10 seconds from startup
		StartupDelayCnt++;
		return;
	}

	for( ii=0; ii< sizeof(AlarmTimerList)/sizeof(TAlarmTimer*); ii++)
	{
		if ( AlarmTimerList[ii]->AlarmCheckEnabled){
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
				// if reset
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
}


void DisableOrganProtectionChecks(void)
{
	HighArterPressAlarmTimer.AlarmCheckEnabled = false;
	HighVenousPressAlarmTimer.AlarmCheckEnabled = false;
	HighAdsFiltPressAlarmTimer.AlarmCheckEnabled = false;
	HighOxygenPressAlarmTimer.AlarmCheckEnabled = false;

	HighPlateTempAlarmTimer.AlarmCheckEnabled = true; // keep checking plate T
	HighArtTempAlarmTimer.AlarmCheckEnabled = false;
	HighVenTempAlarmTimer.AlarmCheckEnabled = false;
	HighFluidTempAlarmTimer.AlarmCheckEnabled = false;

	LowArtTempAlarmTimer.AlarmCheckEnabled = false;
	LowVenTempAlarmTimer.AlarmCheckEnabled = false;
	LowFluidTempAlarmTimer.AlarmCheckEnabled = false;

	HighArterPressAlarmTimer.AlarmCounter = 0;
	HighVenousPressAlarmTimer.AlarmCounter = 0;
	HighAdsFiltPressAlarmTimer.AlarmCounter = 0;
	HighOxygenPressAlarmTimer.AlarmCounter = 0;

	HighArtTempAlarmTimer.AlarmCounter = 0;
	HighVenTempAlarmTimer.AlarmCounter = 0;
	HighFluidTempAlarmTimer.AlarmCounter = 0;
	LowArtTempAlarmTimer.AlarmCounter = 0;
	LowVenTempAlarmTimer.AlarmCounter = 0;
	LowFluidTempAlarmTimer.AlarmCounter = 0;

}

void EnableOrganProtectionChecks(void)
{
	HighArterPressAlarmTimer.AlarmCheckEnabled = true;
	HighVenousPressAlarmTimer.AlarmCheckEnabled = true;
	HighAdsFiltPressAlarmTimer.AlarmCheckEnabled = true;
	HighOxygenPressAlarmTimer.AlarmCheckEnabled = true;

	HighPlateTempAlarmTimer.AlarmCheckEnabled = true;
	HighArtTempAlarmTimer.AlarmCheckEnabled = true;
	HighVenTempAlarmTimer.AlarmCheckEnabled = true;
	HighFluidTempAlarmTimer.AlarmCheckEnabled = true;

	LowArtTempAlarmTimer.AlarmCheckEnabled = true;
	LowVenTempAlarmTimer.AlarmCheckEnabled = true;
	LowFluidTempAlarmTimer.AlarmCheckEnabled = true;
}


//////////////////////////////////////////////////
//  actions to be performed upon alarm condition
/////////////////////////////////////////////////

void HighArterPressAlarmAct(void)
{

	if( GetControlFSMState() == STATE_TREATMENT)
	{
		ShowNewAlarmError(CODE_ALARM_PRESS_ART_XHIGH);
		DisablePinchNPumps();
		Enable_Heater(false);
		Enable_Frigo(false);
		TriggerSecondaryAction(&HighArterPressAlarmTimer);
	}
}

void HighVenousPressAlarmAct(void)
{

	if( GetControlFSMState() == STATE_TREATMENT)
	{
		ShowNewAlarmError(CODE_ALARM_PRESS_VEN_XHIGH);
		DisablePinchNPumps();
		Enable_Heater(false);
		Enable_Frigo(false);
		// trigger sec action
		TriggerSecondaryAction(&HighVenousPressAlarmTimer);
	}
}


void HighAdsFiltPressAlarmAct(void)
{
	if( GetControlFSMState() == STATE_TREATMENT)
	{
		ShowNewAlarmError(CODE_ALARM_PRESS_ADSFILT_XHIGH);
		DisablePinchNPumps();
		Enable_Heater(false);
		Enable_Frigo(false);
		TriggerSecondaryAction(&HighAdsFiltPressAlarmTimer);
	}
}


void HighOxygenPressAlarmAct(void)
{
	if( GetControlFSMState() == STATE_TREATMENT)
	{
		ShowNewAlarmError(CODE_ALARM_PRESS_OXYGEN_XHIGH);
		DisablePinchNPumps();
		Enable_Heater(false);
		Enable_Frigo(false);
		TriggerSecondaryAction(&HighOxygenPressAlarmTimer);
	}
}

void HighLevelPressAlarmAct(void)
{
	if( GetControlFSMState() == STATE_TREATMENT)
	{
		ShowNewAlarmError(CODE_ALARM_PRESS_LEVEL_XHIGH);
		DisablePinchNPumps();
		Enable_Heater(false);
		Enable_Frigo(false);
		TriggerSecondaryAction(&HighLevelPressAlarmTimer);
	}
}

void HighPlateTempAlarmAct(void)
{
	ShowNewAlarmError(CODE_ALARM_TEMP_PLATE_XHIGH);
	DisablePinchNPumps();
	Enable_Heater(false);
	Enable_Frigo(false);
	TriggerSecondaryAction(&HighPlateTempAlarmTimer);
}

void HighArtTempAlarmAct(void)
{
	if( SomePinchIsInPerfusionPosition() && (GetControlFSMState() == STATE_TREATMENT) )
	{
		DisablePinchNPumps();
		ShowNewAlarmError(CODE_ALARM_TEMP_ART_XHIGH);
		Enable_Heater(false);
		Enable_Frigo(false);
		TriggerSecondaryAction(&HighArtTempAlarmTimer);
	}
}

void HighVenTempAlarmAct(void)
{
	if( SomePinchIsInPerfusionPosition() && (GetControlFSMState() == STATE_TREATMENT))
	{
		ShowNewAlarmError(CODE_ALARM_TEMP_VEN_XHIGH);
		DisablePinchNPumps();
		Enable_Heater(false);
		Enable_Frigo(false);
		TriggerSecondaryAction(&HighVenTempAlarmTimer);
	}
}

void HighFluidTempAlarmAct(void)
{
	if( SomePinchIsInPerfusionPosition() && (GetControlFSMState() == STATE_TREATMENT))
	{
		ShowNewAlarmError(CODE_ALARM_TEMP_FLUID_XHIGH);
		DisablePinchNPumps();
		Enable_Heater(false);
		Enable_Frigo(false);
		TriggerSecondaryAction(&HighFluidTempAlarmTimer);
	}
}

void LowPlateTempAlarmAct(void)
{
	ShowNewAlarmError(CODE_ALARM_TEMP_PLATE_XLOW);
	DisablePinchNPumps();
	Enable_Heater(false);
	Enable_Frigo(false);
	TriggerSecondaryAction(&LowPlateTempAlarmTimer);
}

void LowArtTempAlarmAct(void)
{
	if( SomePinchIsInPerfusionPosition() && (GetControlFSMState() == STATE_TREATMENT))
	{
		ShowNewAlarmError(CODE_ALARM_TEMP_ART_XLOW);
		DisablePinchNPumps();
		Enable_Heater(false);
		Enable_Frigo(false);
		TriggerSecondaryAction(&LowArtTempAlarmTimer);
	}
}

void LowVenTempAlarmAct(void)
{
	if( SomePinchIsInPerfusionPosition() && (GetControlFSMState() == STATE_TREATMENT))
	{
		ShowNewAlarmError(CODE_ALARM_TEMP_VEN_XLOW);
		DisablePinchNPumps();
		Enable_Heater(false);
		Enable_Frigo(false);
		TriggerSecondaryAction(&LowVenTempAlarmTimer);
	}
}

void LowFluidTempAlarmAct(void)
{
	if  (SomePinchIsInPerfusionPosition() && ( GetControlFSMState() == STATE_TREATMENT))
	{
		ShowNewAlarmError(CODE_ALARM_TEMP_FLUID_XLOW);
		DisablePinchNPumps();
		Enable_Heater(false);
		Enable_Frigo(false);
		TriggerSecondaryAction(&LowFluidTempAlarmTimer);
	}
}

//
// Late action if pumps and pinch not stopping and moving safe
//
void TriggerSecondaryAction(TAlarmTimer* AlarmTimer)
{
	if(AlarmTimer->SecondaryActionTimerTreshold != 0){
		AlarmTimer->SecondaryActionTimer = AlarmTimer->SecondaryActionTimerTreshold;
	}
}

void PumpsOrPinchNotRespond_EmergAct(void)
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


//////////////////////////////////////
// SENSORS VALUES VERIFICATORS
//////////////////////////////////////

void VerifyArterialPressure(uint16_t ValPress)
{
	if( (ValPress - GetOffsetPressArt()) > PR_ART_XHIGH) // consider offset when checking
		HighArterPressAlarmTimer.AlarmConditionPending = true;
	else
		HighArterPressAlarmTimer.AlarmConditionPending = false;
}

void VerifyOxygenPressure(uint16_t Value)
{
	if( Value > PR_OXYG_XHIGH )
		HighOxygenPressAlarmTimer.AlarmConditionPending = true;
	else
		HighOxygenPressAlarmTimer.AlarmConditionPending = false;
}

//#define VERIFY_TUB_PRESSURE_REQUIRED
#undef VERIFY_TUB_PRESSURE_REQUIRED

void VerifyTubPressure(uint16_t Value)
{
#ifdef VERIFY_TUB_PRESSURE_REQUIRED
	if( Value > PR_TUB_XHIGH)
		HighLevelPressAlarmTimer.AlarmConditionPending = true;
	else
		HighLevelPressAlarmTimer.AlarmConditionPending = false;
#endif
}

void VerifyFilterPressure(uint16_t  Value)
{
	if( Value > PR_ADS_FILTER_XHIGH)
		HighAdsFiltPressAlarmTimer.AlarmConditionPending = true;
	else
		HighAdsFiltPressAlarmTimer.AlarmConditionPending = false;
}

void VerifyVenousPressure(uint16_t  Value)
{
	if( (Value - GetOffsetPressVen()) > PR_VEN_XHIGH)
		HighVenousPressAlarmTimer.AlarmConditionPending = true;
	else
		HighVenousPressAlarmTimer.AlarmConditionPending = false;
}


#ifdef TEMPERATURE_SENSORS_AVAILABLE
void VerifyPlateTemp(float Value)
{

	if( Value > XMAX_PLATE_TEMP )
		HighPlateTempAlarmTimer.AlarmConditionPending = true;
	else
		HighPlateTempAlarmTimer.AlarmConditionPending = false;

	if( Value < XMIN_PLATE_TEMP )
		LowPlateTempAlarmTimer.AlarmConditionPending = true;
	else
		LowPlateTempAlarmTimer.AlarmConditionPending = false;
}

void VerifyArtTemp(float Value)
{
	if( Value > (uint16_t)TEMPER_ART_XHIGH )
		HighArtTempAlarmTimer.AlarmConditionPending = true;
	else
		HighArtTempAlarmTimer.AlarmConditionPending = false;

	if( Value < TEMPER_ART_XLOW )
		LowArtTempAlarmTimer.AlarmConditionPending = true;
	else
		LowArtTempAlarmTimer.AlarmConditionPending = false;
}

void VerifyVenTemp(float Value)
{
	if( Value > TEMPER_VEN_XHIGH )
		HighVenTempAlarmTimer.AlarmConditionPending = true;
	else
		HighVenTempAlarmTimer.AlarmConditionPending = false;

	if( Value < TEMPER_VEN_XLOW )
		LowVenTempAlarmTimer.AlarmConditionPending = true;
	else
		LowVenTempAlarmTimer.AlarmConditionPending = false;
}

void VerifyFluidTemp(float Value)
{
	if( Value > TEMPER_FLUID_XHIGH )
		HighFluidTempAlarmTimer.AlarmConditionPending = true;
	else
		HighFluidTempAlarmTimer.AlarmConditionPending = false;

	if( Value < TEMPER_FLUID_XLOW )
		LowFluidTempAlarmTimer.AlarmConditionPending = true;
	else
		LowFluidTempAlarmTimer.AlarmConditionPending = false;
}
#endif

#ifndef TEMPERATURE_SENSORS_AVAILABLE
void VerifyPlateTemp(float Value)
{

}

void VerifyArtTemp(float Value)
{

}

void VerifyVenTemp(float Value)
{

}

void VerifyFluidTemp(float Value)
{

}
#endif


static void Dummy(void){}







