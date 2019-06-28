/*
 * ActionsProtective.h
 *
 *  Created on: 31/ago/2018
 *      Author: W5
 */

#ifndef APPLICATION_ACTIONSPROTECTIVE_H_
#define APPLICATION_ACTIONSPROTECTIVE_H_

void InitActuators(void);
void GoTotalSafety(void);
void DisablePinchNPumps(void);
void EnablePinchNPumps(void );
void SwitchOFFPinchNPumps(void);
void SwitchONPinchNPumps(void);

void Enable_Power_EVER_PUMP(bool status);
void Enable_Power_Motor(bool status);
void Enable_Pump_filter(bool status);
void Enable_Pump_Art_Livcer(bool status);
void Enable_Pump_OXY(bool status);
void Enable_Pinch_Filter(bool status);
void Enable_Pinch_Arterial(bool status);
void Enable_Pinch_Venous(bool status);

bool Pinch_Filter_IsEnabled(void);
bool Pinch_Arterial_IsEnabled(void);
bool Pinch_Venous_IsEnabled(void);

void Enable_Heater(bool status);
void Enable_Frigo (bool status);

typedef void(*TAlarmAction)(void);// TAlarmAction;

typedef struct {
	uint8_t AlarmCounter;
	uint8_t	CountTreshold;
	bool AlarmConditionPending;
	bool AlarmActive;
	TAlarmAction AlarmAction;
	uint8_t SecondaryActionTimer;
	uint8_t SecondaryActionTimerTreshold; // if 0 , never execute 2nd action
	TAlarmAction SecondaryAlarmAction;
} TAlarmTimer;


void TriggerSecondaryAction(TAlarmTimer* AlarmTimer);
void PumpsOrPinchNotRespond_EmergAct(void);


#endif /* APPLICATION_ACTIONSPROTECTIVE_H_ */
