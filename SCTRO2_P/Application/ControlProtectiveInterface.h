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


uint16_t GetReceivedAlarmCode(void);
void TxAlarmCode( uint16_t AlarmCode);

void GetPinchPos( uint8_t *Pinch0Pos ,  uint8_t *Pinch1_pos , uint8_t *Pinch2_pos);
void GetPressures(uint16_t* PressFilter, uint16_t* PressArt,  uint16_t* PressVen,  uint16_t* PressLevelx100,  uint16_t* PressOxy);


void onNewPinchStat(ActuatorHallStatus Ahs );
void onNewPressOxygen(uint16_t Value);
void onNewTubPressLevel(uint16_t Value);
void onNewPressADSFLT(uint16_t  Value);
void onNewPressVen(uint16_t  Value);
void onNewPressArt(uint16_t  Value);

void onNewAirSensorStat(unsigned char AirStatus);

void onNewPumpRPM(int16_t Value, int PumpIndex);
void onNewFilterPumpRPM( int16_t Value);
void onNewArtLiverPumpRPM( int16_t Value);
void onNewOxy1PumpRPM( int16_t Value );
void onNewOxy2PumpRPM( int16_t Value );

// Filippo - questa funzione serve per aggiornare il messaggio CAN da spedire alla Control
void onNewTempPlateValue(int16_t value);
void onNewTPlateCentDegrees( float Temper11 );

void onNewTPerfArteriosa(float Temper);
void onNewTPerfRicircolo(float Temper);
void onNewTPerfVenosa(float Temper);

void GetTemperatures(uint16_t *TempArtx10, uint16_t *TempFluidx10, uint16_t *TempVenx10, uint16_t *TempPlatex10);
#endif /* CONTROLPROTECTIVEINTERFACE_H_ */
