/*
 * ControlProtectiveInterface.h
 *
 *  Created on: 18/apr/2018
 *      Author: W5
 */

#ifndef CONTROLPROTECTIVEINTERFACE_H_
#define CONTROLPROTECTIVEINTERFACE_H_

void onNewSensPressVal(uint16_t PressFilt, uint16_t PressArt ,
		               uint16_t PressVen, uint16_t PressLev);
void onNewPumpSpeed(uint16_t Pump0Speed, uint16_t Pump1Speed ,
		            uint16_t Pump2Speed, uint16_t Pump3Speed);
void onNewState( struct machineState* MSp, struct machineParent* MPp ,
				  struct machineChild* MCp, uint16_t Guard   );
void InitControlProtectiveInterface(void);


#endif /* CONTROLPROTECTIVEINTERFACE_H_ */
