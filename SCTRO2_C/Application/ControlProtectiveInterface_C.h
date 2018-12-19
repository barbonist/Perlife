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
void onNewSensTempVal(uint16_t PressOxyg, uint16_t TempRes,
		               uint16_t TempArt, uint16_t TempVen);
void onNewPinchVal(uint8_t AirFiltStat, uint16_t AlarmCode,
		           uint8_t Pinch2WPVF, uint8_t Pinch2WPVA, uint8_t Pinch2WPVV,
				   uint8_t free1, uint8_t free2);

#ifdef ENABLE_PROTECTIVE_ALARM_RESET
void onNewAlmToResetMsg(uint16_t AlmCodeToreset);
#endif

extern uint8_t *FilterPinchPos;
extern uint8_t *ArtPinchPos;
extern uint8_t *OxygPinchPos;
extern uint16_t *SpeedPump1Rpmx10;
extern uint16_t *SpeedPump2Rpmx10;
extern uint16_t *SpeedPump3Rpmx10;
extern uint16_t *SpeedPump4Rpmx10;

uint16_t GetAlarmCodeProt(void);


#endif /* CONTROLPROTECTIVEINTERFACE_H_ */
