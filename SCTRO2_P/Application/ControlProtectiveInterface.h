/*
 * ControlProtectiveInterface.h
 *
 *  Created on: 18/apr/2018
 *      Author: W5
 */

#ifndef CONTROLPROTECTIVEINTERFACE_H_
#define CONTROLPROTECTIVEINTERFACE_H_


void onNewPinchStat(ActuatorHallStatus Ahs );
void onNewPressOxygen(uint16_t Value);
void onNewTubPressLevel(uint16_t Value);
void onNewPressADSFLT(uint16_t  Value);
void onNewPressVen(uint16_t  Value);
void onNewPressArt(uint16_t  Value);


#endif /* CONTROLPROTECTIVEINTERFACE_H_ */
