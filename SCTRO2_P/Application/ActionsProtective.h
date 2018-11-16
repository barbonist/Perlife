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
void Enable_Heater(bool status);
void Enable_Frigo (bool status);


#endif /* APPLICATION_ACTIONSPROTECTIVE_H_ */
