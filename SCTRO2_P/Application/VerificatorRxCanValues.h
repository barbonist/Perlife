/*
 * VerificatorRxCanValues.h
 *
 *  Created on: 02/mag/2018
 *      Author: W5
 */

#ifndef APPLICATION_VERIFICATORRXCANVALUES_H_
#define APPLICATION_VERIFICATORRXCANVALUES_H_




void InitVerificatorRx(void);
void VerifyRxState(uint16_t State, uint16_t Parent, uint16_t Child, uint16_t Guard);
void VerifyRxPressures(uint16_t PressFilter, uint16_t PressArt, uint16_t PressVen, uint16_t PressLevelx100, uint16_t PressOxy);
void VerifyRxTemperatures(uint16_t TempArtx10, uint16_t TempFluidx10, uint16_t TempVenx10, uint16_t TempPlatex10);
void VerifyRxAirAlarm( uint8_t RxAirAlarm );
void VerifyRxPinchPos( uint8_t Pinch0Pos , uint8_t Pinch1Pos ,  uint8_t Pinch2Pos);
void VerifyRxPumpsRpm(uint16_t SpeedPump0Rpmx100, uint16_t SpeedPump1Rpmx100, uint16_t SpeedPump2Rpmx100, uint16_t SpeedPump3Rpmx100);
void ManageRxAlarmCode(uint16_t AlarmCode);
void NotifyCanOnline(bool Online);

#endif /* APPLICATION_VERIFICATORRXCANVALUES_H_ */
