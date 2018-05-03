/*
 * ControlProtectiveInterface.h
 *
 *  Created on: 18/apr/2018
 *      Author: W5
 */

#ifndef CONTROLPROTECTIVEINTERFACE_H_
#define CONTROLPROTECTIVEINTERFACE_H_

void InitControlProtectiveInterface(void);

void ReceivedCanData(uint8_t *rxbuff, int rxlen, int RxChannel);

void onNewPinchStat(ActuatorHallStatus Ahs );
void onNewPressOxygen(uint16_t Value);
void onNewTubPressLevel(uint16_t Value);
void onNewPressADSFLT(uint16_t  Value);
void onNewPressVen(uint16_t  Value);
void onNewPressArt(uint16_t  Value);


void onNewPumpRPM(int16_t Value, int PumpIndex);
void onNewFilterPumpRPM( int16_t Value);
void onNewArtLiverPumpRPM( int16_t Value);
void onNewOxy1PumpRPM( int16_t Value );
void onNewOxy2PumpRPM( int16_t Value );

#endif /* CONTROLPROTECTIVEINTERFACE_H_ */
