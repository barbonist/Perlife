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
uint16_t GetControlFSMState(void);
void GetPinchPos( uint8_t *Pinch0Pos ,  uint8_t *Pinch1_pos , uint8_t *Pinch2_pos);
void GetPressures(uint16_t* PressFilter, uint16_t* PressArt,  uint16_t* PressVen,  uint16_t* PressLevelx100,  uint16_t* PressOxy);
void GetPressOffsets( uint8_t *OffsetPressArt, uint8_t *OffsetPressVen );
void GetPumpsSpeedRpmx100(int16_t *SpeedPump0x100, int16_t *SpeedPump1x100,int16_t *SpeedPump2x100,int16_t *SpeedPump3x100);
void GetTemperatures(uint16_t *TempArtx10, uint16_t *TempFluidx10, uint16_t *TempVenx10, uint16_t *TempPlatex10);

uint16_t GetOffsetPressArt(void);
uint16_t GetOffsetPressVen(void);

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

void Enable_Heater_CommandCAN(bool status);
void Enable_Frigo_CommandCAN(bool status);

void LogControlBoardError(uint16_t NumCError);
void LogProtectiveBoardError(uint16_t NumPError);

#endif /* CONTROLPROTECTIVEINTERFACE_H_ */
