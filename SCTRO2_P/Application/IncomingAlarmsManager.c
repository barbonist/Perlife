/*
 * IncomingAlarmsManager.c
 *
 *  Created on: 05/set/2018
 *      Author: W5
 */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "stdint.h"
#include "SwTimer.h"
#include "Global.h"
#include "ShowAlarm.h"
#include "Alarm_Con_Protective.h"
#include "ActionsProtective.h"
#include "IncomingAlarmsManager.h"
#include "ControlProtectiveInterface.h"
#include "RPMGauge.h"

bool IsAlarmTemperatureRelated(uint16_t alarm);
bool IsAlarmNotTemperatureRelated(uint16_t alarm);
//bool PinchesAreInSafetyMode(void);
//bool PumpsAreStopped(void);
void ManaageInAlarmManager100ms(void);

void InitIncomingAlarmManager(void)
{
	AddSwTimer(ManaageInAlarmManager100ms,10,TM_REPEAT);
}

int TimeSecsFromControlAlarm = 0;
bool HuntingPinchNoSafeMode = false;  // in this status , waiting pinch return to supply organ
bool UnrecoverableAlarm = false;


static uint16_t controlBoard_AlarmCode;
static uint16_t OldcontrolBoard_AlarmCode;


void ManaageInAlarmManager100ms(void)
{
bool SomePumpMoving = false;
bool SomePinchNotDisabled = false;

	///////////////////////////////////////
	/// UNRECOVERABLE ALARM --> Do Nothing
	///////////////////////////////////////

	if( UnrecoverableAlarm )
		return;

	controlBoard_AlarmCode = GetReceivedAlarmCode();
	if( controlBoard_AlarmCode != OldcontrolBoard_AlarmCode){

		if( controlBoard_AlarmCode == CODE_ALARM_NO_ERROR){
			TimeSecsFromControlAlarm = 0;
			if( IsAlarmTemperatureRelated( OldcontrolBoard_AlarmCode ) ){
				// was temperature alarm --> start hunting pinch no safe mode to restart verification
				HuntingPinchNoSafeMode = true;
			}
			else{
				EnablePinchNPumps();
			}
		}

		if( IsAlarmTemperatureRelated( controlBoard_AlarmCode ) ){
				DisablePinchNPumps();
				Enable_Heater(false);
				TimeSecsFromControlAlarm = 65; // trigger check pumps after 6 seconds ( limit of hall sensors pos )
		}

		if( IsAlarmNotTemperatureRelated( controlBoard_AlarmCode ) ){
				DisablePinchNPumps();
				Enable_Heater(false);
		}
	}
	// manage possible failure in pump stop or pinch disable
	// in this case , go to irreversible error and communicate HW error
	if( TimeSecsFromControlAlarm > 0 ){
		TimeSecsFromControlAlarm--;
		if(TimeSecsFromControlAlarm == 0)
		{
			if( !PumpsAreStopped() || !PinchesAreInSafetyMode()){
				// hardware error --> must remove supply to pinch and motors
				SwitchOFFPinchNPumps();
				UnrecoverableAlarm = true;
				ShowNewAlarmError(CODE_ALARM_GEN_HWFAILURE);
			}
		}
	}

	if(HuntingPinchNoSafeMode){
		// check pinch status , if no longer in safety , exit hunting mode and enable system
		if( !PinchesAreInSafetyMode()){
			// end safety
			EnablePinchNPumps();
			HuntingPinchNoSafeMode = false;
		}
	}

	OldcontrolBoard_AlarmCode = controlBoard_AlarmCode;
}

bool PinchesAreInSafetyMode(void)
{
	uint8_t pch0, pch1, pch2;

	GetPinchPos( &pch0 ,  &pch1, &pch2);
	return ((pch0 == 0x02) && (pch1 == 0x02) && (pch2 == 0x02));
}

bool PumpsAreStopped(void)
{
bool SomePumpIsMoving =
			(
					(GetMeasuredPumpSpeed(0) != 0) ||
					(GetMeasuredPumpSpeed(1) != 0) ||
					(GetMeasuredPumpSpeed(2) != 0) ||
					(GetMeasuredPumpSpeed(3) != 0)		);

	return !SomePumpIsMoving;
}


bool IsAlarmTemperatureRelated(uint16_t alarm)
{
bool result;

	switch(alarm)
	{
			case CODE_ALARM_NO_ERROR:
				result = false;
			break;
			case CODE_ALARM_TEMP_ART_HIGH:
			case CODE_ALARM_TEMP_ART_LOW:
			case CODE_ALARM_TEMP_VEN_HIGH:
			case CODE_ALARM_TEMP_VEN_LOW:
			case CODE_ALARM_TEMP_NTC_HIGH:
			case CODE_ALARM_TEMP_NTC_LOW:
			case CODE_ALARM_TEMP_SENS_NOT_DETECTED:
			case CODE_ALARM_DELTA_TEMP_REC_ART:
			case CODE_ALARM_DELTA_TEMP_REC_VEN:
				result = true;
			break;
			default:
				result = false;
			break;
	}
	return result;
}

bool IsAlarmNotTemperatureRelated(uint16_t alarm)
{
	return !IsAlarmTemperatureRelated(alarm);
}


bool IsVerifyRequired(void)
{
	return ((GetReceivedAlarmCode() == CODE_ALARM_NO_ERROR) &&  (HuntingPinchNoSafeMode == false) && (!UnrecoverableAlarm));
}

