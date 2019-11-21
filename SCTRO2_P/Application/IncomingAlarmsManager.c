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
#include "VerificatorLocalSensors.h"
#include "RPMGauge.h"

bool IsAlarmTemperatureRelated(uint16_t alarm);
bool IsAlarmPressureRelated(uint16_t alarm);
bool IsAlarmNotTemperatureRelated(uint16_t alarm);
void ManaageInAlarmManager100ms(void);

void InitIncomingAlarmManager(void)
{
	AddSwTimer(ManaageInAlarmManager100ms,10,TM_REPEAT);
}

int Time2CheckPumpsStopped = 0;
int Time2CheckPinchSafe = 0;

bool HuntingPinchNoSafeMode = false;  // in this status , waiting pinch return to supply organ
bool UnrecoverableAlarm = false;

void ControlAlarmReset( void );

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
			ControlAlarmReset();
			HuntingPinchNoSafeMode = true;
		}

		//if( IsAlarmTemperatureRelated( controlBoard_AlarmCode ) || IsAlarmPressureRelated( controlBoard_AlarmCode )){
		if( IsAlarmTemperatureRelated( controlBoard_AlarmCode )){
				Time2CheckPumpsStopped = 65; // trigger check pumps after 6 seconds ( limit of hall sensors pos )
				Time2CheckPinchSafe = 20;
		}
	}

	// manage possible failure in pump stop or pinch disable
	// in this case , go to irreversible error and communicate HW error
	if( Time2CheckPumpsStopped > 0 ){
		Time2CheckPumpsStopped--;
		if(Time2CheckPumpsStopped == 0)
		{
			if( !PumpsAreStopped() ){
				// hardware error --> must remove supply to pinch and motors
				SwitchOFFPinchNPumps();
				UnrecoverableAlarm = true;
				DisableHwVerification();
				ShowNewAlarmError(CODE_ALARM_GEN_HWFAILURE);
			}
		}
	}
	if( Time2CheckPinchSafe > 0 ){
		Time2CheckPinchSafe--;
		if(Time2CheckPinchSafe == 0)
		{
			if( !PinchesAreInSafetyMode() ){
				// hardware error --> must remove supply to pinch and motors
				SwitchOFFPinchNPumps();
				UnrecoverableAlarm = true;
				DisableHwVerification();
				ShowNewAlarmError(CODE_ALARM_GEN_HWFAILURE);
			}
			else {
				// pinches in safety mode --> disable protective check not needed when fluid cannot enter organ
				DisableOrganProtectionChecks();
			}
		}
	}

	if(HuntingPinchNoSafeMode){
		// check pinch status , if no longer in safety , exit hunting mode and enable system
		if( !PinchesAreInSafetyMode()){
			// end safety mode --> restart organ checks
			HuntingPinchNoSafeMode = false;
			EnableOrganProtectionChecks();
		}
	}

	OldcontrolBoard_AlarmCode = controlBoard_AlarmCode;
}


void ControlAlarmReset( void )
{

	Time2CheckPumpsStopped = 0;
	Time2CheckPinchSafe = 0;

	if( IsAlarmTemperatureRelated( OldcontrolBoard_AlarmCode ) ){
		// was temperature alarm --> start hunting pinch no safe mode to restart verification
		HuntingPinchNoSafeMode = true;
	}
	else{
		EnablePinchNPumps();
	}
}


// pinch chiuse --> 1
// aperte a sinistra --> 2
// aperte a destra --> 4

// si considera che le pinch sono in sicurezza quando è verificata questa condizione:
// pinch 0 ( in basso ) --> aperta a sinistra o chiusa
// pinch 1 e 2 ( le 2 in alto ) --> aperta a destra o chiusa
//
bool PinchesAreInSafetyMode(void)
{
	uint8_t pch0, pch1, pch2;

	GetPinchPos( &pch0 ,  &pch1, &pch2);
	// la pinch 0 è quella in basso ( per il filtro )
	// pa pinch 1 e 2 sono quelle in alto , venosa e arteriosa . La 2 è la venosa la 1 l'arteriosa ( in modalità liever )
	return ( ((pch0 == 0x02) || (pch0 == 0x01)) &&
			 ((pch1 == 0x04) || (pch1 == 0x01)) &&
			 ((pch2 == 0x04) || (pch2 == 0x01)) );
}

bool PinchArteriousInSafetyMode(void)
{
	uint8_t pch0, pch1, pch2;

	GetPinchPos( &pch0 ,  &pch1, &pch2);
	// pa pinch 1 e 2 sono quelle in alto , venosa e arteriosa . La 2 è la venosa la 1 l'arteriosa ( in modalità liever )
	return ((pch1 == 0x04) || (pch1 == 0x01));
}

bool PinchVenousInSafetyMode(void)
{
	uint8_t pch0, pch1, pch2;

	GetPinchPos( &pch0 ,  &pch1, &pch2);
	// pa pinch 1 e 2 sono quelle in alto , venosa e arteriosa . La 2 è la venosa la 1 l'arteriosa ( in modalità liever )
	return ((pch2 == 0x04) || (pch2 == 0x01));
}

bool PinchAdsorbentInSafetyMode(void)
{
	uint8_t pch0, pch1, pch2;
	// la pinch 0 è quella in basso ( per il filtro )
	GetPinchPos( &pch0 ,  &pch1, &pch2);
	return ((pch2 == 0x02) || (pch2 == 0x01));
}





bool SomePinchIsInPerfusionPosition(void)
{
	uint8_t pch0, pch1, pch2;

	GetPinchPos( &pch0 ,  &pch1, &pch2);
	return ((pch1 == 0x02) || (pch2 == 0x02));
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

bool OxyPumpsAreStopped(void)
{
	bool OxyPumpsAreMoving =
				( (GetMeasuredPumpSpeed(2) != 0) ||
				  (GetMeasuredPumpSpeed(3) != 0)	);

		return !OxyPumpsAreMoving;
}

bool IsAlarmTemperatureRelated(uint16_t alarm)
{
bool result;

	switch(alarm)
	{
			case CODE_ALARM_TEMP_SENS_NOT_DETECTED:
			case CODE_ALARM_DELTA_TEMP_REC_ART:
			case CODE_ALARM_DELTA_TEMP_REC_VEN:
			case CODE_ALARM_T_ART_OUT_OF_RANGE:
			case CODE_ALARM_TEMP_MAX_IN_TRT:
			case CODE_ALARM_TEMP_MIN_IN_TRT:
				result = true;
			break;
			default:
				result = false;
			break;
	}
	return result;
}

bool IsAlarmPressureRelated(uint16_t alarm)
{
bool result;

	switch(alarm)
	{
			case CODE_ALARM_PRESS_ART_HIGH:
			case CODE_ALARM_PRESS_ART_SET:
			case CODE_ALARM_PRESS_VEN_HIGH:
			case CODE_ALARM_PRESS_VEN_SET:
			case CODE_ALARM_PRESS_ADS_FILTER_HIGH:
			case CODE_ALARM_PRESS_OXYG_INLET:
			case CODE_ALARM_PRESS_ADS_FILTER_LOW:
			case CODE_ALARM_PRESS_OXYG_LOW:
			case CODE_ALARM_PRIM_AIR_PRES_ART:
			case CODE_ALARM_PRIM_AIR_PRES_VEN:
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

//
//bool IsVerifyRequired(void)
//{
//	return ((GetReceivedAlarmCode() == CODE_ALARM_NO_ERROR) &&  (HuntingPinchNoSafeMode == false) && (!UnrecoverableAlarm));
//}

