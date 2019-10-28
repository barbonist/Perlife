/*
 * IncomingAlarmsManager.h
 *
 *  Created on: 06/set/2018
 *      Author: W5
 */

#ifndef APPLICATION_INCOMINGALARMSMANAGER_H_
#define APPLICATION_INCOMINGALARMSMANAGER_H_


void InitIncomingAlarmManager(void);
bool IsVerifyRequired(void);
bool PinchesAreInSafetyMode(void);
bool PinchesAreInSafetyMode(void);
bool PinchArteriousInSafetyMode(void);
bool PinchVenousInSafetyMode(void);
bool PinchAdsorbentInSafetyMode(void);

bool SomePinchIsInPerfusionPosition(void);
bool PumpsAreStopped(void);


#endif /* APPLICATION_INCOMINGALARMSMANAGER_H_ */
