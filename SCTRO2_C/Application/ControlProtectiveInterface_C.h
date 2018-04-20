/*
 * ControlProtectiveInterface.h
 *
 *  Created on: 18/apr/2018
 *      Author: W5
 */

#ifndef CONTROLPROTECTIVEINTERFACE_H_
#define CONTROLPROTECTIVEINTERFACE_H_


void onNewState( struct machineState* MSp, struct machineParent* MPp ,
				  struct machineChild* MCp, uint16_t Guard   );
void InitControlProtectiveInterface(void);


#endif /* CONTROLPROTECTIVEINTERFACE_H_ */
