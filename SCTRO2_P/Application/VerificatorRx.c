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

//
// TimerCounter increased each 100ms .
// Alarm issued when TimerCounter == AlarmValue
// Alarm reset when ??
//

typedef struct {
	uint8_t AlarmCounter;
	uint8_t	CountTreshold;
	bool AlarmConditionPending;
	bool AlarmActive;
} TAlarmTimer;

TAlarmTimer PumpAlarmTimer = {0,60,false,false}; // alarm after 2 s mismatch

void ManageVerificatorAlarms100ms(void);
bool ValueIsInRange(uint16_t RefValue, uint16_t Val2Test, uint16_t IPercent);

void InitVerificatorRx(void){
	AddSwTimer(ManageVerificatorAlarms100ms,10,TM_REPEAT);
}

void ManageVerificatorAlarms100ms(void) {
	if (PumpAlarmTimer.AlarmConditionPending) {
		if (PumpAlarmTimer.AlarmCounter < PumpAlarmTimer.CountTreshold) {
			PumpAlarmTimer.AlarmCounter++;
			if (PumpAlarmTimer.AlarmCounter == PumpAlarmTimer.CountTreshold) {
				// show alarm
				PumpAlarmTimer.AlarmActive = true;
				ShowNewAlarmError(CODE_ALARM_PUMPSPEED_DONT_MATCH);
			}
		}
	}
	else{
		// if AlarmActive , don't clear alarm anyway ,
		// if not AlarmActive ,  restart AlarmCounter
		PumpAlarmTimer.AlarmCounter = 0;
	}
}

// reset alarm condition pressing '1' on bubble keyboard
void onNewKey(char Key)
{
	if( Key == '1' ){
		// reset alarm
		PumpAlarmTimer.AlarmActive = false;
		PumpAlarmTimer.AlarmCounter = 0;
		ShowNewAlarmError(CODE_ALARM_NO_ERROR);
	}
}

void VerifyRxState(uint16_t State, uint16_t Parent, uint16_t Child,
		uint16_t Guard) {

}

void VerifyRxPressures(uint16_t PressFilter, uint16_t PressArt,
		uint16_t PressVen, uint16_t PressLevelx100, uint16_t PressOxy ) {

}

void VerifyRxTemperatures(uint16_t TempArtx10, uint16_t TempFluidx10,
		uint16_t TempVenx10) {

}

void VerifyRxAirAlarm( uint8_t RxAirAlarm )
{
	// air alarm , don't know what to do since we have no dedicated air alarm input
}

void VerifyRxPinchPos( uint8_t Pinch0Pos , uint8_t Pinch1Pos ,  uint8_t Pinch2Pos)
{

}

void VerifyRxPumpsRpm(uint16_t SpeedPump0Rpmx100, uint16_t SpeedPump1Rpmx100,
		uint16_t SpeedPump2Rpmx100, uint16_t SpeedPump3Rpmx100) {
bool Pump0Ok = true;
bool Pump1Ok = true;
bool Pump2Ok = true;
bool Pump3Ok = true;

	Pump0Ok = ValueIsInRange(GetMeasuredPumpSpeed(0), SpeedPump0Rpmx100, 1000) ? true : false;
	Pump1Ok = ValueIsInRange(GetMeasuredPumpSpeed(1), SpeedPump1Rpmx100, 1000) ? true : false;
	Pump2Ok = ValueIsInRange(GetMeasuredPumpSpeed(2), SpeedPump2Rpmx100, 1000) ? true : false;
	Pump3Ok = ValueIsInRange(GetMeasuredPumpSpeed(3), SpeedPump3Rpmx100, 1000) ? true : false;

	if( Pump0Ok && Pump1Ok && Pump2Ok && Pump3Ok ){
		PumpAlarmTimer.AlarmConditionPending = false;
	}
	else {
		PumpAlarmTimer.AlarmConditionPending = true;
	}
}

void ManageRxAlarmCode(uint16_t AlarmCode)
{

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
