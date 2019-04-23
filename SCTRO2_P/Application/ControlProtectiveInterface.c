/*
 * ControlProtectiveInterface.c
 *
 *  Created on: 13/apr/2018
 *      Author: W5
 */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <float.h>
#include "stdint.h"
#include "SwTimer.h"
#include "FlexCANWrapper.h"
#include "Global.h"
#include "ShowAlarm.h"
#include "VerificatorRxCanValues.h"
#include "VerificatorLocalSensors.h"
#include "COMP_ENABLE.h"
#include "HEAT_ON_P.h"
#include "ActionsProtective.h"
#include "Alarm_Con_Protective.h"
#include "IncomingAlarmsManager.h"
#include "ControlProtectiveInterface.h"
#include "PC_DEBUG_COMM.h"
#include "FRONTAL_COVER_1.h"
#include "FRONTAL_COVER_2.h"
#include "HOOK_SENSOR_1.h"
#include "HOOK_SENSOR_2.h"


#define VAL_JOLLY16	0x5A5A
#define VAL_JOLLY8	0x5A
#define SIZE_CAN_BUFFER	8
#define LAST_INDEX_TXBUFF2SEND 15	//

#define ESC 0x1B
#define CR  0x0D
#define LF  0x0A
#define BS  0x08
#define DEL 0x7F

//#define CAN_DEBUG 1

union URxCan {
	uint8_t RawCanBuffer[SIZE_CAN_BUFFER];
	struct {
		uint16_t	State;	uint16_t	Parent;	uint16_t	Child;	uint16_t	Guard;
	} SRxCan0;
	struct {
		uint16_t PressFilter;	uint16_t PressArt;	uint16_t PressVen;	uint16_t PressLevelx100;
	} SRxCan1;
	struct {
		uint16_t PressOxy;	uint16_t TempFluidx10;	uint16_t TempArtx10;	uint16_t TempVenx10;
	} SRxCan2;
	struct {
		uint16_t AlarmCode; uint8_t AirAlarm_N_Therm;	uint8_t Pinch0Pos;	uint8_t Pinch1Pos;	uint8_t Pinch2Pos;	uint8_t OffsetPrVen; uint8_t OffsetPrArt;
	} SRxCan3;
	struct {
		int16_t SpeedPump0Rpmx100;	int16_t SpeedPump1Rpmx100;	int16_t SpeedPump2Rpmx100;	int16_t SpeedPump3Rpmx100;
	} SRxCan4;
	// Filippo - messo campo per lo scambio del valore di temperatura piatto
	struct {
		uint8_t Free1;	uint8_t Free2;	int16_t tempPlateC;	uint8_t TherapyType ;	uint8_t AirArtLevel;	uint8_t AirVenLevel;	uint8_t Free8;
	} SRxCan5;
	struct {
		uint8_t Free1;	uint8_t Free2;	uint8_t Free3;	uint8_t Free4;	uint8_t Free5;	uint8_t Free6;	uint8_t Free7;	uint8_t Free8;
	} SRxCan6;
	struct {
		uint16_t MsgType;	uint8_t MsgParams[6];
	} SRxCan7;
};

union UTxCan {
	uint8_t RawCanBuffer[SIZE_CAN_BUFFER];
	struct {
		uint16_t PressFilter;	uint16_t PressArt;	uint16_t PressVen;	uint16_t PressLevelx100;
	} STxCan0;
	struct {
		uint16_t PressOxy;	uint16_t TempFluidx10;	uint16_t TempArtx10;	uint16_t TempVenx10;
	} STxCan1;
	struct {
		uint8_t AirAlarm_N_Therm;	uint16_t AlarmCode;	uint8_t Consenso;	uint8_t Pinch0Pos;	uint8_t Pinch1Pos;	uint8_t Pinch2Pos;	uint8_t Free;
	} STxCan2;
	struct {
		int16_t SpeedPump0Rpmx100;	int16_t SpeedPump1Rpmx100;	int16_t SpeedPump2Rpmx100;	int16_t SpeedPump3Rpmx100;
	} STxCan3;
	struct {
		uint8_t Free1;	uint8_t Free2;	uint8_t Free3;	uint8_t Free4;	uint8_t Free5;	uint8_t Free6;	uint8_t Free7;	uint8_t Free8;
	} STxCan4;
	// Filippo - messo campo per lo scambio del valore di temperatura piatto
	struct {
		uint16_t ProtFwVersion;	int16_t tempPlateP;	uint8_t Free5;	uint8_t Free6;	uint8_t Free7;	uint8_t Free8;
	} STxCan5;
	struct {
		uint8_t Free1;	uint8_t Free2;	uint8_t Free3;	uint8_t Free4;	uint8_t Free5;	uint8_t Free6;	uint8_t Free7;	uint8_t Free8;
	} STxCan6;
	struct {
		uint16_t MsgType;	uint8_t MsgParams[6];
	} STxCan7;
};

union UTxCan  TxCan0,  OldTxCan0;
union UTxCan  TxCan1,  OldTxCan1;
union UTxCan  TxCan2,  OldTxCan2;
union UTxCan  TxCan3,  OldTxCan3;
union UTxCan  TxCan4,  OldTxCan4;
union UTxCan  TxCan5,  OldTxCan5;
union UTxCan  TxCan6,  OldTxCan6;
union UTxCan  TxCan7,  OldTxCan7;

union URxCan  RxCan0,  OldRxCan0;
union URxCan  RxCan1,  OldRxCan1;
union URxCan  RxCan2,  OldRxCan2;
union URxCan  RxCan3,  OldRxCan3;
union URxCan  RxCan4,  OldRxCan4;
union URxCan  RxCan5,  OldRxCan5;
union URxCan  RxCan6,  OldRxCan6;
union URxCan  RxCan7,  OldRxCan7;

void ManageTxCan10ms(void);
void ManageTxCan100ms(void);
void DebugFillTxBuffers(void);
void CanCheckTimer(void);

void ManageTxDebug(void);
void ManageGetParams(void);
void TxDebugPressures(void);
void TxDebugTemperatures(void);
void TxDebugPinch(void);
void TxDebugErrors(void);
void TxPumpsHall(void);
void TxDoorsStatus(void);
void TxHooksStatus(void);
void TxIfCanOk(void);

uint8_t CharReceived(void);

#ifdef CAN_DEBUG
void InitControlProtectiveInterface(void)
{
	int ii;
//TODO forse meglio inizializzare a 0
	for(ii=0;ii<SIZE_CAN_BUFFER;ii++){
		TxCan0.RawCanBuffer[ii] = VAL_JOLLY8 ;
		TxCan1.RawCanBuffer[ii] = VAL_JOLLY8 + 1;
		TxCan2.RawCanBuffer[ii] = VAL_JOLLY8 + 2;
		TxCan3.RawCanBuffer[ii] = VAL_JOLLY8 +3;
		TxCan4.RawCanBuffer[ii] = VAL_JOLLY8 +4;
		TxCan5.RawCanBuffer[ii] = VAL_JOLLY8 +5;
		TxCan6.RawCanBuffer[ii] = VAL_JOLLY8 +6;
		TxCan7.RawCanBuffer[ii] = VAL_JOLLY8 +7;
		// cause difference to trigger data send 1st time
		OldTxCan0.RawCanBuffer[ii] = VAL_JOLLY8 + 11;
		OldTxCan1.RawCanBuffer[ii] = VAL_JOLLY8 + 11;
		OldTxCan2.RawCanBuffer[ii] = VAL_JOLLY8 + 11;
		OldTxCan3.RawCanBuffer[ii] = VAL_JOLLY8 + 11;
		OldTxCan4.RawCanBuffer[ii] = VAL_JOLLY8 + 11;
		OldTxCan5.RawCanBuffer[ii] = VAL_JOLLY8 + 11;
		OldTxCan6.RawCanBuffer[ii] = VAL_JOLLY8 + 11;
		OldTxCan7.RawCanBuffer[ii] = VAL_JOLLY8 + 11;
		// init rx buffers
		RxCan0.RawCanBuffer[ii] = VAL_JOLLY8;
		RxCan1.RawCanBuffer[ii] = VAL_JOLLY8;
		RxCan2.RawCanBuffer[ii] = VAL_JOLLY8;
		RxCan3.RawCanBuffer[ii] = VAL_JOLLY8;
		RxCan4.RawCanBuffer[ii] = VAL_JOLLY8;
		RxCan5.RawCanBuffer[ii] = VAL_JOLLY8;
		RxCan6.RawCanBuffer[ii] = VAL_JOLLY8;
		RxCan7.RawCanBuffer[ii] = VAL_JOLLY8;

		OldRxCan0.RawCanBuffer[ii] = VAL_JOLLY8;
		OldRxCan1.RawCanBuffer[ii] = VAL_JOLLY8;
		OldRxCan2.RawCanBuffer[ii] = VAL_JOLLY8;
		OldRxCan3.RawCanBuffer[ii] = VAL_JOLLY8;
		OldRxCan4.RawCanBuffer[ii] = VAL_JOLLY8;
		OldRxCan5.RawCanBuffer[ii] = VAL_JOLLY8;
		OldRxCan6.RawCanBuffer[ii] = VAL_JOLLY8;
		OldRxCan7.RawCanBuffer[ii] = VAL_JOLLY8;
	}

	TxCan1.SAux.NoSendCounter = 0;
	TxCan2.SAux.NoSendCounter = 0;
	TxCan3.SAux.NoSendCounter = 0;
	TxCan4.SAux.NoSendCounter = 0;
	TxCan5.SAux.NoSendCounter = 0;
	TxCan6.SAux.NoSendCounter = 0;
	TxCan7.SAux.NoSendCounter = 0;

	//AddSwTimer(ManageTxCan10ms,1,TM_REPEAT);
	AddSwTimer(ManageTxCan100ms,10,TM_REPEAT);
	InitVerificatorRx();
	InitVerificatorLocalParams();

}
#endif

#ifndef CAN_DEBUG

uint16_t GetFwVersionProtective(void);

void InitControlProtectiveInterface(void)
{
	int ii;
// incoming new press values
	for(ii=0;ii<SIZE_CAN_BUFFER;ii++){
		TxCan0.RawCanBuffer[ii] = 0 ;
		TxCan1.RawCanBuffer[ii] = 0;
		TxCan2.RawCanBuffer[ii] = 0;
		TxCan3.RawCanBuffer[ii] = 0;
		TxCan4.RawCanBuffer[ii] = 0;
		TxCan5.RawCanBuffer[ii] = 0;
		TxCan6.RawCanBuffer[ii] = 0;
		TxCan7.RawCanBuffer[ii] = 0;
		// cause difference to trigger data send 1st time
		OldTxCan0.RawCanBuffer[ii] = 1;
		OldTxCan1.RawCanBuffer[ii] = 1;
		OldTxCan2.RawCanBuffer[ii] = 1;
		OldTxCan3.RawCanBuffer[ii] = 1;
		OldTxCan4.RawCanBuffer[ii] = 1;
		OldTxCan5.RawCanBuffer[ii] = 1;
		OldTxCan6.RawCanBuffer[ii] = 1;
		OldTxCan7.RawCanBuffer[ii] = 1;
		// init rx buffers
		RxCan0.RawCanBuffer[ii] = 0;
		RxCan1.RawCanBuffer[ii] = 0;
		RxCan2.RawCanBuffer[ii] = 0;
		RxCan3.RawCanBuffer[ii] = 0;
		RxCan4.RawCanBuffer[ii] = 0;
		RxCan5.RawCanBuffer[ii] = 0;
		RxCan6.RawCanBuffer[ii] = 0;
		RxCan7.RawCanBuffer[ii] = 0;

		OldRxCan0.RawCanBuffer[ii] = 1;
		OldRxCan1.RawCanBuffer[ii] = 1;
		OldRxCan2.RawCanBuffer[ii] = 1;
		OldRxCan3.RawCanBuffer[ii] = 1;
		OldRxCan4.RawCanBuffer[ii] = 1;
		OldRxCan5.RawCanBuffer[ii] = 1;
		OldRxCan6.RawCanBuffer[ii] = 1;
		OldRxCan7.RawCanBuffer[ii] = 1;
	}


	TxCan5.STxCan5.ProtFwVersion = GetFwVersionProtective();
	AddSwTimer(ManageTxCan100ms,10,TM_REPEAT);
	AddSwTimer(ManageTxDebug,5,TM_REPEAT);
	AddSwTimer(ManageGetParams,5,TM_REPEAT);
	InitVerificatorRx();
	InitVerificatorLocalParams();
	InitIncomingAlarmManager();

}
#endif


/////////////////////////////////////////////////////////////////////////////////
//
//   RECEIVING LOCAL SENSORS NEW VALUES n Tx Alarm code
//
/////////////////////////////////////////////////////////////////////////////////


// incoming new press values
#ifdef CAN_DEBUG
void onNewPressOxygen(uint16_t Value)  	{  }
void onNewTubPressLevel(uint16_t Value) {  }
void onNewPressADSFLT(uint16_t  Value) 	{  }
void onNewPressVen(uint16_t  Value)		{  }
void onNewPressArt(uint16_t  Value)		{  }
#else


void TxAlarmCode( uint16_t AlarmCode)
{
	TxCan2.STxCan2.AlarmCode = AlarmCode;
	// alarm code can be written just once
//	if(TxCan2.STxCan2.AlarmCode ==  CODE_ALARM_NO_ERROR	) // quick and dirty solution. A single severe alarm can be issued , then a machine reset is needed.
//		TxCan2.STxCan2.AlarmCode = AlarmCode;
//	// reset alarm allowed anyway
//	if( AlarmCode ==  CODE_ALARM_NO_ERROR )
//		TxCan2.STxCan2.AlarmCode = AlarmCode;
}

void onNewPressOxygen(uint16_t Value)
{
	TxCan1.STxCan1.PressOxy = Value;
	if( IsVerifyRequired() )
			VerifyOxygenPressure(Value);
}

void onNewTubPressLevel(uint16_t Value)
{
	TxCan0.STxCan0.PressLevelx100 = Value;
	if( IsVerifyRequired() )
			VerifyTubPressure(Value);
}

void onNewPressADSFLT(uint16_t  Value)
{
	TxCan0.STxCan0.PressFilter = Value;
	if( IsVerifyRequired() )
			VerifyFilterPressure(Value);
}

void onNewPressVen(uint16_t  Value)
{
	TxCan0.STxCan0.PressVen = Value;
	if( IsVerifyRequired() )
			VerifyVenousPressure(Value);
}

void onNewPressArt(uint16_t  Value)
{
	TxCan0.STxCan0.PressArt = Value;
	if( IsVerifyRequired() )
		VerifyArterialPressure(Value);
}


void onNewAirSensorStat(unsigned char AirStatus)
{
	if( AirStatus == AIR)
		TxCan2.STxCan2.AirAlarm_N_Therm |= 1;
	else if( AirStatus == LIQUID)
		TxCan2.STxCan2.AirAlarm_N_Therm &= 0xFE;
}

#endif

uint16_t GetControlFSMState(void)
{
    return RxCan0.SRxCan0.State;
	// debug return STATE_TREATMENT;
}

void GetPressures(uint16_t* PressFilter, uint16_t* PressArt,  uint16_t* PressVen,  uint16_t* PressLevelx100,  uint16_t* PressOxy)
{
	*PressFilter =		TxCan0.STxCan0.PressFilter;
	*PressArt =			TxCan0.STxCan0.PressArt;
	*PressVen =			TxCan0.STxCan0.PressVen;
	*PressLevelx100 =   TxCan0.STxCan0.PressLevelx100;
	*PressOxy = 		TxCan1.STxCan1.PressOxy;
}
/* 6 RPM is the minimum speed value calculate by protective */

void GetTemperatures(uint16_t *TempArtx10, uint16_t *TempFluidx10, uint16_t *TempVenx10, uint16_t *TempPlatex10)
{
	*TempArtx10	=	TxCan1.STxCan1.TempArtx10;
	*TempFluidx10 = TxCan1.STxCan1.TempFluidx10;
	*TempVenx10 = 	TxCan1.STxCan1.TempVenx10;
	*TempPlatex10 = TxCan5.STxCan5.tempPlateP;
}


uint16_t GetOffsetPressArt(void)
{
	return (uint16_t) RxCan3.SRxCan3.OffsetPrArt;
}

uint16_t GetOffsetPressVen(void)
{
	return (uint16_t) RxCan3.SRxCan3.OffsetPrVen;
}

//
// 0x50 liever
// 0x10 kidney
//
uint8_t GetTherapyType(void)
{
	return RxCan5.SRxCan5.TherapyType;
}

/* 6 RPM is the minimum speed value calculate by protective */
/*
 * OnNewPumpRPM
 * input:   Value = rmpx100
 * 			PumpIndex = number of pump
 * output:  none
 * description:  local RPM values are measured and charged on CAN bus for later transmission to control board
 */
void onNewPumpRPM(int16_t Value, int PumpIndex) {
#ifndef CAN_DEBUG
	switch (PumpIndex) {
	case 0:
		/*Pump filter--art kidney*/
		TxCan3.STxCan3.SpeedPump0Rpmx100 = Value;
		break;
	case 1:
		/*pump art liver--not used in kidney*/
		TxCan3.STxCan3.SpeedPump1Rpmx100 = Value;
		break;
	case 2:
		/*pump oxy first*/
		TxCan3.STxCan3.SpeedPump2Rpmx100 = Value;
		break;
	case 3:
		/*pump oxy second*/
		TxCan3.STxCan3.SpeedPump3Rpmx100 = Value;
		break;
	}
#endif
}

void GetPumpsSpeedRpmx100(int16_t *SpeedPump0x100, int16_t *SpeedPump1x100,int16_t *SpeedPump2x100,int16_t *SpeedPump3x100)
{
	*SpeedPump0x100 = TxCan3.STxCan3.SpeedPump0Rpmx100;
	*SpeedPump1x100 = TxCan3.STxCan3.SpeedPump1Rpmx100;
	*SpeedPump2x100 = TxCan3.STxCan3.SpeedPump2Rpmx100;
	*SpeedPump3x100 = TxCan3.STxCan3.SpeedPump3Rpmx100;
}

// incoming new RPM values ( reduntant funtions , for possible future use )
void onNewFilterPumpRPM(int16_t Value) {
	TxCan3.STxCan3.SpeedPump0Rpmx100 = Value;
}
//
//void onNewArtLiverPumpRPM(int16_t Value) {
void onNewPinchStat(ActuatorHallStatus Ahs )
{
#ifndef CAN_DEBUG
	if (Ahs.PinchFilter_Left)
		  TxCan2.STxCan2.Pinch0Pos = 0x02;
	else if (Ahs.PinchFilter_Right)
		  TxCan2.STxCan2.Pinch0Pos = 0x04;
	else
		  TxCan2.STxCan2.Pinch0Pos = 0x01;

	if (Ahs.PinchArt_Left)
		  TxCan2.STxCan2.Pinch1Pos = 0x02;
	else if (Ahs.PinchArt_Right)
		  TxCan2.STxCan2.Pinch1Pos = 0x04;
	else
		  TxCan2.STxCan2.Pinch1Pos = 0x01;

	if (Ahs.PinchVen_Left)
		  TxCan2.STxCan2.Pinch2Pos = 0x02;
	 else if (Ahs.PinchVen_Right)
		   TxCan2.STxCan2.Pinch2Pos = 0x04;
	 else
		   TxCan2.STxCan2.Pinch2Pos = 0x01;
#endif
}

void GetPinchPos( uint8_t *Pinch0Pos ,  uint8_t *Pinch1Pos , uint8_t *Pinch2Pos)
{
	 *Pinch0Pos = TxCan2.STxCan2.Pinch0Pos;
	 *Pinch1Pos = TxCan2.STxCan2.Pinch1Pos;
	 *Pinch2Pos = TxCan2.STxCan2.Pinch2Pos;
}

// Filippo - questa funzione serve per aggiornare il messaggio CAN da spedire alla Control
void onNewTempPlateValue(int16_t value)
{
	TxCan5.STxCan5.tempPlateP=value;
}

int TxBuffIndex = 0;

union UTxCan*	TxBuffCanP[8] =
{
	&TxCan0, &TxCan1, &TxCan2, &TxCan3, &TxCan4 , &TxCan5 , &TxCan6 , &TxCan7
};

union UTxCan*	OldTxBuffCanP[8] =
{
	&OldTxCan0, &OldTxCan1, &OldTxCan2, &OldTxCan3, &OldTxCan4 , &OldTxCan5 , &OldTxCan6 , &OldTxCan7
};

void onNewTPlateCentDegrees(  float Temper11  )
{
	int16_t Tempx10;

       Tempx10 = (int16_t)(Temper11 * 10);
       TxCan5.STxCan5.tempPlateP = Tempx10;
       if( IsVerifyRequired() )
                    VerifyPlateTemp( Temper11 );
}

void onNewTPerfArteriosa(float Temper)
{
       int16_t Tempx10;
       Tempx10 = (int16_t)(Temper * 10);
       TxCan1.STxCan1.TempArtx10 = Tempx10;
       if( IsVerifyRequired() )
                    VerifyArtTemp(Temper);
}

void onNewTPerfRicircolo(float Temper)
{
      int16_t Tempx10;
       Tempx10 = (int16_t)(Temper * 10);
       TxCan1.STxCan1.TempFluidx10 = Tempx10;
       if( IsVerifyRequired() )
                    VerifyFluidTemp(Temper);
}


void onNewTPerfVenosa(float Temper)
{
       int16_t Tempx10;
       Tempx10 = (int16_t)(Temper * 10);
       TxCan1.STxCan1.TempVenx10 = Tempx10;
       if( IsVerifyRequired() )
             VerifyVenTemp(Temper);
}


//
// send altorithm 1
//
void ManageTxCan10ms(void)
{
//	if(memcmp(TxBuffCanP[TxBuffIndex]->RawCanBuffer,OldTxBuffCanP[TxBuffIndex]->RawCanBuffer,SIZE_CAN_BUFFER) != 0) {
//		// 8 buffer are scanned each 80ms , amximum delay since value changed and tx on can
//		SendCAN(TxBuffCanP[TxBuffIndex]->RawCanBuffer, SIZE_CAN_BUFFER, 0);
//		memcpy(OldTxBuffCanP[TxBuffIndex]->RawCanBuffer, TxBuffCanP[TxBuffIndex]->RawCanBuffer, SIZE_CAN_BUFFER);
//		TxBuffCanP[TxBuffIndex]->SAux.NoSendCounter = 0;
//	}
//	else {
//		TxBuffCanP[TxBuffIndex]->SAux.NoSendCounter++;
//		if(TxBuffCanP[TxBuffIndex]->SAux.NoSendCounter == 5) {
//			// in any case , each 400ms send a buffer even if nothing changed on it , to prevent possible data loss
//			SendCAN(TxBuffCanP[TxBuffIndex]->RawCanBuffer, SIZE_CAN_BUFFER, 0);
//			// don't need to copy new buff in old one because they already match
//			TxBuffCanP[TxBuffIndex]->SAux.NoSendCounter = 0;
//		}
//	}
//	TxBuffIndex = (TxBuffIndex + 1) % (LAST_INDEX_TXBUFF2SEND+1);
}


//
// send all at a time
//
void ManageTxCan100ms(void)
{
	int ii ;//= 11;

	for( ii=8 ; ii<=LAST_INDEX_TXBUFF2SEND ; ii++)
	{
		if (SendCAN(TxBuffCanP[ii - 8]->RawCanBuffer, SIZE_CAN_BUFFER, ii)!=ERR_OK)
		{
			// Filippo - serve per il debug
			ii++;
			ii--;
		}
	}

#ifdef CAN_DEBUG
	DebugFillTxBuffers();
#endif
	CanCheckTimer();
}

union URxCan*	RxBuffCanP[8] =
{
	&RxCan0, &RxCan1, &RxCan2, &RxCan3, &RxCan4 , &RxCan5 , &RxCan6 , &RxCan7
};

union URxCan*	OldRxBuffCanP[8] =
{
	&OldRxCan0, &OldRxCan1, &OldRxCan2, &OldRxCan3, &OldRxCan4 , &OldRxCan5 , &OldRxCan6 , &OldRxCan7
};

void NewDataRxChannel0(void);
void NewDataRxChannel1(void);
void NewDataRxChannel2(void);
void NewDataRxChannel3(void);
void NewDataRxChannel4(void);
void NewDataRxChannel5(void);
// Filippo - aggiunta funzione per gestire la lettura del buffer 5
void NewDataRxChannel5(void);

void ReceivedCanData(uint8_t *rxbuff, int rxlen, int RxChannel) {
	if ((rxlen <= 8) && (RxChannel <= 7)) {
		RetriggerNoCANRxTxAlarm();
		memcpy(RxBuffCanP[RxChannel]->RawCanBuffer, rxbuff, rxlen);
//		if (memcmp(RxBuffCanP[RxChannel]->RawCanBuffer,
//				OldRxBuffCanP[RxChannel]->RawCanBuffer, SIZE_CAN_BUFFER) != 0) {
		if(1){
			// data changed --> trigger some action
			switch (RxChannel) {
			case 0:
				NewDataRxChannel0();
				break;
			case 1:
				NewDataRxChannel1();
				break;
			case 2:
				NewDataRxChannel2();
				break;
			case 3:
				NewDataRxChannel3();
				break;
			case 4:
				NewDataRxChannel4();
				break;
			// Filippo - gestisco anche la ricezione del messaggio 5
			case 5:
				NewDataRxChannel5();
				break;
			}
			// !! keep this statement after vals management
			memcpy(OldRxBuffCanP[RxChannel]->RawCanBuffer,
					RxBuffCanP[RxChannel]->RawCanBuffer, SIZE_CAN_BUFFER);
		}
	}
}

void NewDataRxChannel0(void) {
	if( IsVerifyRequired() )
	VerifyRxState(RxCan0.SRxCan0.State, RxCan0.SRxCan0.Parent,
			RxCan0.SRxCan0.Child, RxCan0.SRxCan0.Guard);
}

void NewDataRxChannel1(void) {
	if( IsVerifyRequired() )
	VerifyRxPressures(RxCan1.SRxCan1.PressFilter, RxCan1.SRxCan1.PressArt,
			RxCan1.SRxCan1.PressVen, RxCan1.SRxCan1.PressLevelx100,
			RxCan2.SRxCan2.PressOxy);
}


void DebugMIsmatchPressure(void)
{
		VerifyRxPressures(RxCan1.SRxCan1.PressFilter, RxCan1.SRxCan1.PressArt,
				RxCan1.SRxCan1.PressVen, RxCan1.SRxCan1.PressLevelx100,
				RxCan2.SRxCan2.PressOxy);
}

void NewDataRxChannel2(void) {
	// pressure changed
	if (RxCan2.SRxCan2.PressOxy != OldRxCan2.SRxCan2.PressOxy) {
		// oxy pressure changed
		if( IsVerifyRequired() )
		VerifyRxPressures(RxCan1.SRxCan1.PressFilter, RxCan1.SRxCan1.PressArt,
				RxCan1.SRxCan1.PressVen, RxCan1.SRxCan1.PressLevelx100,
				RxCan2.SRxCan2.PressOxy);
	}

	// temperature changed
//	if ((RxCan2.SRxCan2.TempArtx10 != OldRxCan2.SRxCan2.TempArtx10)
//			|| (RxCan2.SRxCan2.TempVenx10 != OldRxCan2.SRxCan2.TempVenx10)
//			|| (RxCan2.SRxCan2.TempFluidx10 != OldRxCan2.SRxCan2.TempFluidx10)) {

			if( IsVerifyRequired() )
				VerifyRxTemperatures(RxCan2.SRxCan2.TempArtx10, RxCan2.SRxCan2.TempFluidx10, RxCan2.SRxCan2.TempVenx10, RxCan5.SRxCan5.tempPlateC);
//	}
}

void NewDataRxChannel3(void) {
	// air alarm
	//if (RxCan3.SRxCan3.AirAlarm_N_Therm != OldRxCan3.SRxCan3.AirAlarm_N_Therm) {
		if( IsVerifyRequired() )
			VerifyRxAirAlarm(RxCan3.SRxCan3.AirAlarm_N_Therm & 0x01);

		if ( (RxCan3.SRxCan3.AirAlarm_N_Therm & 0x02) && !(RxCan3.SRxCan3.AirAlarm_N_Therm & 0x04) )
		{
			//aziono il relè del riscaldatore
			if (!COMP_ENABLE_GetVal())
			{
				Enable_Heater_CommandCAN(TRUE);
				Enable_Frigo_CommandCAN(FALSE);
			}
		}
		else if ( !(RxCan3.SRxCan3.AirAlarm_N_Therm & 0x02) && (RxCan3.SRxCan3.AirAlarm_N_Therm & 0x04) )
		{
			//aziono il relè del frigo
			if (!HEAT_ON_P_GetVal())
			{
				Enable_Frigo_CommandCAN(TRUE);
				Enable_Heater_CommandCAN(FALSE);
			}
		}
		else if ( (RxCan3.SRxCan3.AirAlarm_N_Therm & 0x02) && (RxCan3.SRxCan3.AirAlarm_N_Therm & 0x04) )
		{
			//vado in allarme e spengo entrambi i relè perchè
			//la control mi chiede di attivarli entrambi, cosa non corretta
//			Enable_Heat(FALSE);
//			Enable_Frigo(FALSE);
			// Filippo - questa combinazione è consentita per gestire il nuovo PID che usa il frigo e il riscaldatore insieme
			Enable_Heater_CommandCAN(TRUE);
			Enable_Frigo_CommandCAN(TRUE);
		}
		else if ( !(RxCan3.SRxCan3.AirAlarm_N_Therm & 0x02) && !(RxCan3.SRxCan3.AirAlarm_N_Therm & 0x04) )
		{
			//disattivo i relè del frigo e del riscaldatore
			Enable_Frigo_CommandCAN(FALSE);
			Enable_Heater_CommandCAN(FALSE);
		}


	// Alarm code
	if (RxCan3.SRxCan3.AlarmCode != OldRxCan3.SRxCan3.AlarmCode) {
		ManageRxAlarmCode(RxCan3.SRxCan3.AlarmCode);
		LogControlBoardError(RxCan3.SRxCan3.AlarmCode);
	}

	// pinch pos


	if( IsVerifyRequired() )
		VerifyRxPinchPos(RxCan3.SRxCan3.Pinch0Pos, RxCan3.SRxCan3.Pinch1Pos, RxCan3.SRxCan3.Pinch2Pos);

}

uint16_t GetReceivedAlarmCode(void)
{
	return RxCan3.SRxCan3.AlarmCode;
}

void NewDataRxChannel4(void) {
	if( IsVerifyRequired() )
	VerifyRxPumpsRpm(RxCan4.SRxCan4.SpeedPump0Rpmx100,
			RxCan4.SRxCan4.SpeedPump1Rpmx100, RxCan4.SRxCan4.SpeedPump2Rpmx100,
			RxCan4.SRxCan4.SpeedPump3Rpmx100);
}

void NewDataRxChannel5(void)
{
	// verifichiamo la temperatura di piatto
	tempPlateControl=(float)RxCan5.SRxCan5.tempPlateC/10.0;


	//if (RxCan5.SRxCan5.tempPlateC != OldRxCan5.SRxCan5.tempPlateC) {
		// temp plate changed
	if( IsVerifyRequired() )
		VerifyRxTemperatures(RxCan2.SRxCan2.TempArtx10, RxCan2.SRxCan2.TempFluidx10, RxCan2.SRxCan2.TempVenx10, RxCan5.SRxCan5.tempPlateC);
	//}
	VerifyRxAirLevels(RxCan5.SRxCan5.AirArtLevel, RxCan5.SRxCan5.AirVenLevel);
}

void DebugFillTxBuffers(void)
{
	static uint16_t seq_number = 0;
	int ii;
	uint16_t *p16;

	for(ii=0; ii<4;ii++){
		p16 = (uint16_t*)&TxCan0.RawCanBuffer[ii*2];
		*p16 = (ii + seq_number) | 0x8000;
		p16 = (uint16_t*)&TxCan1.RawCanBuffer[ii*2];
		*p16 = (ii + seq_number) | 0xC000;
		p16 = (uint16_t*)&TxCan2.RawCanBuffer[ii*2];
		*p16 = (ii + seq_number) | 0xE000;
		p16 = (uint16_t*)&TxCan3.RawCanBuffer[ii*2];
		*p16 = (ii + seq_number) | 0xF000;
		p16 = (uint16_t*)&TxCan4.RawCanBuffer[ii*2];
		*p16 = (ii + seq_number) | 0xF800;
		p16 = (uint16_t*)&TxCan5.RawCanBuffer[ii*2];
		*p16 = (ii + seq_number) | 0xFC00;
		p16 = (uint16_t*)&TxCan6.RawCanBuffer[ii*2];
		*p16 = (ii + seq_number) | 0xFE00;
		p16 = (uint16_t*)&TxCan7.RawCanBuffer[ii*2];
		*p16 = (ii + seq_number) | 0xFF00;
	}
	seq_number++;
}


static int CanAlarmCounter = 0;

void RetriggerNoCANRxTxAlarm(void){
	NotifyCanOnline(true);
	CanAlarmCounter = 0;
}

void CanCheckTimer(void)
{
	if( CanAlarmCounter < 10 ){
		CanAlarmCounter ++;
		if(CanAlarmCounter == 10){
			// notify can alarm
			NotifyCanOnline(false);
			// cause repeated notification if alarm persist
			CanAlarmCounter = 0;
		}
	}
}


//////////////////////////////////////////////////////////
//
//  debug routines using debug serial interface
//
//////////////////////////////////////////////////////////


//
//	T or t Pressed --> generate head line for csv file and temperatures headers
//  P or p pressed --> generate head line for csv file and pressures headers
//  esc --> stop logging
//
#define GREEN_TEXT   "\033[32;1m"
#define YELLOW_TEXT  "\033[33;1m"
#define BLUE_TEXT    "\033[34;1m"
#define MAGENTA_TEXT "\033[35;1m"
#define CYAN_TEXT    "\033[36;1m"
#define WHITE_TEXT   "\033[37;1m"
void ParseNExecuteCommand( char* CommadnNParamsString);
bool FirstTime = true;
uint8_t LogMode = 0;  // 0: don't log , 4 log temp, 2 log press ..

void ManageTxDebug(void)
{
	word sent_data;
	uint8_t ch1;
	static char CommandBuff[100];
	static int BuffCnt = 0;
	int ii;
	char* StrPrompt[40];

	strcpy(StrPrompt,GREEN_TEXT) ; strcat(StrPrompt,"\r\nPerlife>") ; strcat(StrPrompt, WHITE_TEXT);

	if(FirstTime){
		PC_DEBUG_COMM_SendBlock(StrPrompt, strlen(StrPrompt) , &sent_data);
		FirstTime = false;
	}
	ch1 = CharReceived();

	if( LogMode != 0){
		if( ch1 == ESC ) SetLogCommand(ESC);
		return;
	}

	int CmdBuff;
	if(ch1 != 0xFF){
		CommandBuff[BuffCnt++] = ch1;
 		PC_DEBUG_COMM_SendChar(ch1);
 		if( ch1 == CR ){
 			PC_DEBUG_COMM_SendChar(LF);
 			if( BuffCnt > 2) {
	 			ParseNExecuteCommand( CommandBuff );
			}
 			if( LogMode == 0){
 				// if logging data have been triggered , avoid showing prompt
 	 			PC_DEBUG_COMM_SendBlock(StrPrompt, strlen(StrPrompt) , &sent_data);
 			}
 			for( ii = 0; ii< BuffCnt; ii++){
 	 			CommandBuff[ii] = 0;
 			}
 			BuffCnt = 0;
 		}
	}
}



uint8_t cmd = '_' ; // nothing

void SetLogCommand( uint8_t Command)
{
	cmd = Command;
	ManageGetParams();
}

int PrescalerCnt = 0;
void ManageGetParams(void)
{
static char stringPtr[200];
word sent_data;

	//uint8_t cmd;

	switch(cmd){
		case ESC:
			LogMode = 0;
			break;
		case 'P':
			LogMode = 1;
			sprintf(stringPtr, "\"sep=,\"\r\n Press Ven Rem[mmHg] , Pr Ven Loc[mmHg], Pr Art Rem[mmHg] , Pr Art Loc[mmHg],");
			PC_DEBUG_COMM_SendBlock(stringPtr, strlen(stringPtr), &sent_data);
			sprintf(stringPtr, "Press Lev Remx100[mmHg] , Pr Lev Locx100[mmHg], Pr Filt Rem[mmHg] , Pr Filt Loc[mmHg] , Pr Oxy Rem[mmHg] , Pr Oxy Loc[mmHg]\r\n");
			PC_DEBUG_COMM_SendBlock(stringPtr, strlen(stringPtr), &sent_data);
			break;
		case 'T':
			LogMode = 4;
			sprintf(stringPtr, "\"sep=,\"\r\n Tempx10 Ven Rem[mmHg] , Tempx10 Ven Loc[mmHg], Tempx10 Art Rem[mmHg] , Tempx10 Art Loc[mmHg] , Tempx10 Recycle Rem[mmHg] , Tempx10 Recycle Loc[mmHg], Tempx10 Plate Rem[mmHg] , Tempx10 Plate Loc[mmHg] , Heat ON , Refrig ON\r\n");
			PC_DEBUG_COMM_SendBlock(stringPtr, strlen(stringPtr), &sent_data);
			break;
		case 'C':
			LogMode = 7;
			sprintf(stringPtr, "\"sep=,\"\r\n Pinch 1 Rem , Pinch 1 Loc, Pinch 2 Rem , Pinch 2 Loc , Pinch 3 Rem , Pinch 3 Loc \r\n");
			PC_DEBUG_COMM_SendBlock(stringPtr, strlen(stringPtr), &sent_data);
			break;
		case 'M':
			LogMode = 10;
			sprintf(stringPtr, "\"sep=,\"\r\n Pump 1 Rem [rpm], Pump 1 Loc[rpm], Pump 2 Rem[rpm], Pump 2 Loc[rpm], Pump 3 Rem[rpm], Pump 3 Loc[rpm], "
					"						  Pump 4 Rem[rpm], Pump4 Loc[rpm]\r\n");
			PC_DEBUG_COMM_SendBlock(stringPtr, strlen(stringPtr), &sent_data);
			break;
		case 'E': // log errors
			LogMode = 13;
			sprintf(stringPtr, "\"sep=,\"\r\n Error Ctrl , Error Prot \r\n");
			PC_DEBUG_COMM_SendBlock(stringPtr, strlen(stringPtr), &sent_data);
			break;
		case 'H': // log pumps hall
			LogMode = 16;
			sprintf(stringPtr, "   Pump1         Pump2      Pump3     Pump4 \r\n");
			PC_DEBUG_COMM_SendBlock(stringPtr, strlen(stringPtr), &sent_data);
			break;
		case '1': // log doors stat
			LogMode = 19;
			sprintf(stringPtr, "   Door1	 Door2 \r\n");
			PC_DEBUG_COMM_SendBlock(stringPtr, strlen(stringPtr), &sent_data);
			break;
		case '2': // log hooks stat
			LogMode = 22;
			sprintf(stringPtr, "   Hook1     Hook2 \r\n");
			PC_DEBUG_COMM_SendBlock(stringPtr, strlen(stringPtr), &sent_data);
			break;
		case '3': // log if can is OK
			LogMode = 25;
			sprintf(stringPtr, "  CAN bus communication  \r\n");
			PC_DEBUG_COMM_SendBlock(stringPtr, strlen(stringPtr), &sent_data);
			break;
	}
	cmd = '_';

	switch(LogMode){

	case 1: // pressure
		LogMode = 2;
		break;
	case 4: // temp
		LogMode = 5;
		break;
	case 7: // pinch
		LogMode = 8;
		break;
	case 10: // pumps
		LogMode = 11;
		break;
	case 13: // log errors
		LogMode = 14;
		break;
	case 16: // log errors
		LogMode = 17;
		break;
	case 19: // log doors
		LogMode = 20;
		break;
	case 22: // log hooks
		LogMode = 23;
		break;
	case 25: // log if can ok
		LogMode = 26;
		break;
	case 2: // pressure
		TxDebugPressures();
		break;
	case 5: // temperature
		if( PrescalerCnt == 0){
			TxDebugTemperatures();
		}
		PrescalerCnt = (PrescalerCnt + 1) % 10;
		break;
	case 8: // pinc
		if( PrescalerCnt == 0){
			TxDebugPinch();
		}
		PrescalerCnt = (PrescalerCnt + 1) % 10;
		break;
	case 11: // pumps speed
		if( PrescalerCnt == 0){
			TxDebugPumpSpeed();
		}
		PrescalerCnt = (PrescalerCnt + 1) % 10;
		break;
	case 14: // log errors
		if( PrescalerCnt == 0){
			TxDebugErrors();
		}
		PrescalerCnt = (PrescalerCnt + 1) % 20;
		break;
	case 17: // log hall sensors pumps
		if( PrescalerCnt == 0){
			TxPumpsHall();
		}
		PrescalerCnt = (PrescalerCnt + 1) % 3;
		break;
	case 20: // log doors
		if( PrescalerCnt == 0){
			TxDoorsStatus();
		}
		PrescalerCnt = (PrescalerCnt + 1) % 20;
		break;
	case 24: // log hooks
		if( PrescalerCnt == 0){
			TxHooksStatus();
		}
		PrescalerCnt = (PrescalerCnt + 1) % 20;
		break;
	case 26: // log if can status ok
		if( PrescalerCnt == 0){
			TxIfCanOk();
		}
		PrescalerCnt = (PrescalerCnt + 1) % 20;
		break;
	}
}

/////////////////////////////////////////////
// send data on serial debug
// escpressed true or false
////////////////////////////////////////////
void TxDebugPressures(void)
{
    static char stringPtr[200];
	uint16_t RPFilt , RPArt ,RPVen, RPLevelx100 ,RPOxy;
	uint16_t LPFilt , LPArt ,LPVen, LPLevelx100 ,LPOxy;
	word sent_data;

	GetPressures(&LPFilt, &LPArt,  &LPVen,  &LPLevelx100 ,  &LPOxy);

	RPFilt = RxCan1.SRxCan1.PressFilter;
	RPArt  = RxCan1.SRxCan1.PressArt;
	RPVen  = RxCan1.SRxCan1.PressVen;
	RPLevelx100 = RxCan1.SRxCan1.PressLevelx100;
	RPOxy = RxCan2.SRxCan2.PressOxy;

    sprintf(stringPtr, "%04u ,  %04u , %04u , %04u , %04u ,  %04u , %04u , %04u , %04u , %04u \r\n",
    				   RPVen , LPVen , RPArt, LPArt, RPLevelx100 , LPLevelx100 , RPFilt , LPFilt , RPOxy , LPOxy);
    PC_DEBUG_COMM_SendBlock(stringPtr, strlen(stringPtr) , &sent_data);
}

void TxDebugTemperatures(void)
{
    static char stringPtr[200];
	uint16_t RTempArtx10, RTempFluidx10, RTempVenx10, RTempPlatex10;
	uint16_t LTempArtx10, LTempFluidx10, LTempVenx10, LTempPlatex10;
	word sent_data;
	uint16_t FrigoOn, HeaterOn;

	GetTemperatures(&LTempArtx10, &LTempFluidx10, &LTempVenx10, &LTempPlatex10);

	RTempArtx10 = RxCan2.SRxCan2.TempArtx10;
	RTempFluidx10 = RxCan2.SRxCan2.TempFluidx10;
	RTempVenx10 = RxCan2.SRxCan2.TempVenx10;
	RTempPlatex10 = RxCan5.SRxCan5.tempPlateC;
	FrigoOn = COMP_ENABLE_GetVal() ? 100:0;
	HeaterOn = HEAT_ON_P_GetVal() ? 100:0;

    sprintf(stringPtr, "%+04d , %+04d  ,  %+04d , %+04d  ,  %+04d , %+04d  ,  %+04d , %+04d ,     %03u , %03u \r\n", (short int)RTempVenx10, (short int)LTempVenx10 ,
    																						   (short int)RTempArtx10, (short int)LTempArtx10,
																							   (short int)RTempFluidx10 , (short int)LTempFluidx10,
																							   (short int)RTempPlatex10, (short int)LTempPlatex10 ,
																							   HeaterOn, FrigoOn);
    PC_DEBUG_COMM_SendBlock(stringPtr, strlen(stringPtr) , &sent_data);

}

void TxDebugPinch(void)
{
    static char stringPtr[200];
	uint8_t RPinch1, RPinch2, RPinch3;
	uint8_t LPinch1, LPinch2, LPinch3;
	word sent_data;

	GetPinchPos( &LPinch1 ,  &LPinch2 , &LPinch3);

	RPinch1 = RxCan3.SRxCan3.Pinch0Pos;
	RPinch2 = RxCan3.SRxCan3.Pinch1Pos;
	RPinch3 = RxCan3.SRxCan3.Pinch2Pos;

    sprintf(stringPtr, "%02x , %01x   ,   %02x , %01x   ,   %02x , %01x \r\n", RPinch1, LPinch1 , RPinch2, LPinch2, RPinch3 , LPinch3);

    PC_DEBUG_COMM_SendBlock(stringPtr, strlen(stringPtr) , &sent_data);

}


void TxDebugPumpSpeed(void)
{
    static char stringPtr[200];
	uint16_t RSpeed1, RSpeed2, RSpeed3, RSpeed4;
	uint16_t LSpeed1, LSpeed2, LSpeed3, LSpeed4;
	float FRSpeed1, FRSpeed2, FRSpeed3, FRSpeed4;
	float FLSpeed1, FLSpeed2, FLSpeed3, FLSpeed4;
	word sent_data;

	GetPumpsSpeedRpmx100(&LSpeed1, &LSpeed2 , &LSpeed3 , &LSpeed4);
	FLSpeed1 = ((float)LSpeed1)/100.0;
	FLSpeed2 = ((float)LSpeed2)/100.0;
	FLSpeed3 = ((float)LSpeed3)/100.0;
	FLSpeed4 = ((float)LSpeed4)/100.0;

	RSpeed1 = RxCan4.SRxCan4.SpeedPump0Rpmx100;
	RSpeed2 = RxCan4.SRxCan4.SpeedPump1Rpmx100;
	RSpeed3 = RxCan4.SRxCan4.SpeedPump2Rpmx100;
	RSpeed4 = RxCan4.SRxCan4.SpeedPump3Rpmx100;
	FRSpeed1 = ((float)RSpeed1)/100.0;
	FRSpeed2 = ((float)RSpeed2)/100.0;
	FRSpeed3 = ((float)RSpeed3)/100.0;
	FRSpeed4 = ((float)RSpeed4)/100.0;

    sprintf(stringPtr, "%.2f , %.2f   ,   %.2f , %.2f   ,   %.2f , %.2f  ,   %.2f , %.2f \r\n", FRSpeed1, FLSpeed1 , FRSpeed2, FLSpeed2 ,FRSpeed3, FLSpeed3 ,FRSpeed4, FLSpeed4);

    PC_DEBUG_COMM_SendBlock(stringPtr, strlen(stringPtr) , &sent_data);

}


uint16_t OldProtectiveError = 0;
uint16_t NewProtectiveError = 0;
uint16_t OldControlError = 0;
uint16_t NewControlError = 0;

void LogControlBoardError(uint16_t NumCError)
{
	NewControlError = NumCError;
    if(( OldControlError != NewControlError) && (LogMode == 14)){
    	OldControlError = NewControlError;
    	TxDebugErrors();
    }
}

void LogProtectiveBoardError(uint16_t NumPError)
{
	NewProtectiveError = NumPError;
    if(( OldProtectiveError != NewProtectiveError) && (LogMode == 14)){
    	OldProtectiveError = NewProtectiveError;
    	TxDebugErrors();
    }
}


void TxDebugErrors(void)
{
    static char stringPtr[200];
    word sent_data;

    sprintf(stringPtr, "%03u , %03u \r\n", NewControlError, NewProtectiveError);
    PC_DEBUG_COMM_SendBlock(stringPtr, strlen(stringPtr) , &sent_data);

}

void TxPumpsHall(void)
{
word sent_data;

char PFilt1[10]; // "|   ." if 0 , or ".    |" if 1;
char PFilt2[10];
char PArt1[10];
char PArt2[10];
char POxy11[10];
char POxy12[10];
char POxy21[10];
char POxy22[10];
char ResultStr[200];

	  if( HallSens.PumpFilter_HSens1 ) strcpy(PFilt1,".  |");  else strcpy(PFilt1,"|  .");
	  if( HallSens.PumpFilter_HSens2 ) strcpy(PFilt2,".  |");  else strcpy(PFilt2,"|  .");
	  if( HallSens.PumpArt_Liver_HSens1 ) strcpy(PArt1,".  |");  else strcpy(PArt1,"|  .");
	  if( HallSens.PumpArt_Liver_HSens2 ) strcpy(PArt2,".  |");  else strcpy(PArt2,"|  .");
	  if( HallSens.PumpOxy_1_HSens1 ) strcpy(POxy11,".  |");  else strcpy(POxy11,"|  .");
	  if( HallSens.PumpOxy_1_HSens2 ) strcpy(POxy12,".  |");  else strcpy(POxy12,"|  .");
	  if( HallSens.PumpOxy_2_HSens1 ) strcpy(POxy21,".  |");  else strcpy(POxy21,"|  .");
	  if( HallSens.PumpOxy_2_HSens2 ) strcpy(POxy22,".  |\r\n");  else strcpy(POxy22,"|  .\r\n");

	  strcpy(ResultStr, "        "); strcat(ResultStr, PFilt1); 	  strcat(ResultStr, "        "); strcat(ResultStr, PFilt2);
	  strcat(ResultStr, "        "); strcat(ResultStr, PArt1); 	  strcat(ResultStr, "        "); strcat(ResultStr, PArt2);
	  strcat(ResultStr, "        "); strcat(ResultStr, POxy11);   strcat(ResultStr, "        "); strcat(ResultStr, POxy12);
	  strcat(ResultStr, "        "); strcat(ResultStr, POxy21);   strcat(ResultStr, "        "); strcat(ResultStr, POxy22);

	  PC_DEBUG_COMM_SendBlock(ResultStr, strlen(ResultStr) , &sent_data);

}


void TxDoorsStatus(void)
{
static char ResultStr[200];
word sent_data;
char fc1stat[8];
char fc2stat[8];

	strcpy(fc1stat,	FRONTAL_COVER_1_GetVal() ? "opened" : "closed");
	strcpy(fc2stat,	FRONTAL_COVER_2_GetVal() ? "opened" : "closed");
	sprintf(ResultStr, "%s  ,  %s \r\n", fc1stat, fc2stat);
	PC_DEBUG_COMM_SendBlock(ResultStr, strlen(ResultStr) , &sent_data);
}

void TxHooksStatus(void)
{
	static char ResultStr[200];
	word sent_data;
	char hs1stat[8];
	char hs2stat[8];

		strcpy(hs1stat,	HOOK_SENSOR_1_GetVal() ? "clamped" : "released");
		strcpy(hs2stat,	HOOK_SENSOR_2_GetVal()  ? "clamped" : "released");
		sprintf(ResultStr, "%s  ,  %s \r\n", hs1stat, hs2stat);
		PC_DEBUG_COMM_SendBlock(ResultStr, strlen(ResultStr) , &sent_data);
}

bool GetCanOk(void);

void TxIfCanOk(void)
{
	static char ResultStr[200];
	word sent_data;
	char sCanOk[8];

	if(GetCanOk()) strcpy(ResultStr,"can communication OK\r\n");
	else strcpy(ResultStr,"can communication NOT OK\r\n");
	PC_DEBUG_COMM_SendBlock(ResultStr, strlen(ResultStr) , &sent_data);
}


uint8_t CharReceived(void)
{
uint8_t rxchr;

	if ( PC_DEBUG_COMM_RecvChar(&rxchr) == ERR_OK )  return rxchr;
	else return 0xFF;

}



