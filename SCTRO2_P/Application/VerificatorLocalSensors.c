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
void PumpsOrPinchNotRespond_EmergAct(void);

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

TAlarmTimer HighArterPressAlarmTimer = {0,10,false,false,  HighArterPressAlarmAct , 0 , 65 , PumpsOrPinchNotRespond_EmergAct}; // alarm after 1 s alarm
TAlarmTimer HighVenousPressAlarmTimer = {0,10,false,false, HighVenousPressAlarmAct , 0 ,  65 , PumpsOrPinchNotRespond_EmergAct}; // alarm after 1 s alarm
TAlarmTimer HighAdsFiltPressAlarmTimer = {0,10,false,false, HighAdsFiltPressAlarmAct , 0 , 65 , PumpsOrPinchNotRespond_EmergAct}; // alarm after 1 s alarm
TAlarmTimer HighOxygenPressAlarmTimer = {0,10,false,false, HighOxygenPressAlarmAct , 0 , 0 , Dummy}; // alarm after 1 s alarm
TAlarmTimer HighLevelPressAlarmTimer = {0,40,false,false, HighLevelPressAlarmAct , 0 , 0 , Dummy}; // alarm after 4 s alarm
TAlarmTimer HighPlateTempAlarmTimer = {0,10,false,false, HighPlateTempAlarmAct , 0 , 0 , Dummy}; // alarm after 1 s alarm
TAlarmTimer HighArtTempAlarmTimer = {0,10,false,false, HighArtTempAlarmAct , 0 , 0 , Dummy}; // alarm after 1 s alarm
TAlarmTimer HighVenTempAlarmTimer = {0,10,false,false, HighVenTempAlarmAct , 0 , 0 , Dummy}; // alarm after 1 s alarm
TAlarmTimer HighFluidTempAlarmTimer = {0,10,false,false, HighFluidTempAlarmAct , 0 , 0 , Dummy}; // alarm after 1 s alarm
TAlarmTimer LowPlateTempAlarmTimer = {0,10,false,false, LowPlateTempAlarmAct , 0 , 0 , Dummy}; // alarm after 1 s alarm
TAlarmTimer LowArtTempAlarmTimer = {0,10,false,false, LowArtTempAlarmAct , 0 , 0 , Dummy}; // alarm after 1 s alarm
TAlarmTimer LowVenTempAlarmTimer = {0,10,false,false, LowVenTempAlarmAct , 0 , 0 , Dummy}; // alarm after 1 s alarm
TAlarmTimer LowFluidTempAlarmTimer = {0,10,false,false, LowFluidTempAlarmAct , 0 , 0 , Dummy}; // alarm after 1 s alarm

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

//////////////////////////////////////////////////
//  actions to be performed upon alarm condition
/////////////////////////////////////////////////

void HighArterPressAlarmAct(void)
{
	ShowNewAlarmError(CODE_ALARM_PRESS_ART_XHIGH);
	if( GetControlFSMState() == STATE_TREATMENT)
	{
		DisablePinchNPumps();
	}
}

void HighVenousPressAlarmAct(void)
{
	ShowNewAlarmError(CODE_ALARM_PRESS_VEN_XHIGH);
	if( GetControlFSMState() == STATE_TREATMENT)
	{
		DisablePinchNPumps();
		// trigger sec action
	}
	if(HighVenousPressAlarmTimer.SecondaryActionTimerTreshold != 0){
		HighVenousPressAlarmTimer.SecondaryActionTimer = HighVenousPressAlarmTimer.SecondaryActionTimerTreshold;
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

void HighAdsFiltPressAlarmAct(void)
{
	ShowNewAlarmError(CODE_ALARM_PRESS_ADSFILT_XHIGH);
	if( GetControlFSMState() == STATE_TREATMENT)
	{
		DisablePinchNPumps();
	}
}

void HighOxygenPressAlarmAct(void)
{
	ShowNewAlarmError(CODE_ALARM_PRESS_OXYGEN_XHIGH);
	if( GetControlFSMState() == STATE_TREATMENT)
	{
		DisablePinchNPumps();
	}
}

void HighLevelPressAlarmAct(void)
{
	ShowNewAlarmError(CODE_ALARM_PRESS_LEVEL_XHIGH);
	if( GetControlFSMState() == STATE_TREATMENT)
	{
			DisablePinchNPumps();
	}
}

void HighPlateTempAlarmAct(void)
{
	ShowNewAlarmError(CODE_ALARM_TEMP_PLATE_XHIGH);
	DisablePinchNPumps();
	Enable_Heater(FALSE);
}

void HighArtTempAlarmAct(void)
{
	ShowNewAlarmError(CODE_ALARM_TEMP_ART_XHIGH);
	if( SomePinchIsInPerfusionPosition() && (GetControlFSMState() == STATE_TREATMENT) )
	{
		DisablePinchNPumps();
		Enable_Heater(FALSE);
	}
}

void HighVenTempAlarmAct(void)
{
	ShowNewAlarmError(CODE_ALARM_TEMP_VEN_XHIGH);
	if( SomePinchIsInPerfusionPosition() && (GetControlFSMState() == STATE_TREATMENT))
	{
		DisablePinchNPumps();
		Enable_Heater(FALSE);
	}
}

void HighFluidTempAlarmAct(void)
{
	ShowNewAlarmError(CODE_ALARM_TEMP_FLUID_XHIGH);
	if( SomePinchIsInPerfusionPosition() && (GetControlFSMState() == STATE_TREATMENT))
	{
		DisablePinchNPumps();
		Enable_Heater(FALSE);
	}
}

void LowPlateTempAlarmAct(void)
{
	ShowNewAlarmError(CODE_ALARM_TEMP_PLATE_XLOW);
	DisablePinchNPumps();
	Enable_Frigo(FALSE);
}

void LowArtTempAlarmAct(void)
{
	ShowNewAlarmError(CODE_ALARM_TEMP_ART_XLOW);
	if( SomePinchIsInPerfusionPosition() && (GetControlFSMState() == STATE_TREATMENT))
	{
		DisablePinchNPumps();
		Enable_Frigo(FALSE);
	}
}

void LowVenTempAlarmAct(void)
{
	ShowNewAlarmError(CODE_ALARM_TEMP_VEN_XLOW);
	if( SomePinchIsInPerfusionPosition() && (GetControlFSMState() == STATE_TREATMENT))
	{
		DisablePinchNPumps();
		Enable_Frigo(FALSE);
	}
}

void LowFluidTempAlarmAct(void)
{
	ShowNewAlarmError(CODE_ALARM_TEMP_FLUID_XLOW);
	if( GetControlFSMState() == STATE_TREATMENT)
	{
		DisablePinchNPumps();
		Enable_Frigo(FALSE);
	}
}


//////////////////////////////////////
// SENSORS VALUES VERIFICATORS
//////////////////////////////////////

void VerifyArterialPressure(uint16_t ValPress)
{
	if( ValPress > PR_ART_XHIGH)
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
	if( Value > PR_VEN_XHIGH)
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







