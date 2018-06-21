/*
 * ControlProtectiveInterface.c
 *
 *  Created on: 13/apr/2018
 *      Author: W5
 */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "stdint.h"
#include "SwTimer.h"
#include "FlexCANWrapper.h"
#include "Global.h"
#include "ShowAlarm.h"
#include "VerificatorRx.h"
#include "COMP_ENABLE.h"
#include "HEAT_ON_P.h"

#define VAL_JOLLY16	0x5A5A
#define VAL_JOLLY8	0x5A
#define SIZE_CAN_BUFFER	8
#define LAST_INDEX_TXBUFF2SEND 15	//

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
		uint8_t AirAlarm;	uint16_t AlarmCode;	uint8_t Pinch0Pos;	uint8_t Pinch1Pos;	uint8_t Pinch2Pos;	uint8_t Free1; uint8_t Free2;
	} SRxCan3;
	struct {
		int16_t SpeedPump0Rpmx100;	int16_t SpeedPump1Rpmx100;	int16_t SpeedPump2Rpmx100;	int16_t SpeedPump3Rpmx100;
	} SRxCan4;
	struct {
		uint8_t Free1;	uint8_t Free2;	uint8_t Free3;	uint8_t Free4;	uint8_t Free5;	uint8_t Free6;	uint8_t Free7;	uint8_t Free8;
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
		uint8_t Filler[SIZE_CAN_BUFFER]; uint8_t NoSendCounter;
	} SAux;
	struct {
		uint16_t PressFilter;	uint16_t PressArt;	uint16_t PressVen;	uint16_t PressLevelx100;
	} STxCan0;
	struct {
		uint16_t PressOxy;	uint16_t TempFluidx10;	uint16_t TempArtx10;	uint16_t TempVenx10;
	} STxCan1;
	struct {
		uint8_t AirAlarm;	uint16_t AlarmCode;	uint8_t Consenso;	uint8_t Pinch0Pos;	uint8_t Pinch1Pos;	uint8_t Pinch2Pos;	uint8_t Free;
	} STxCan2;
	struct {
		int16_t SpeedPump0Rpmx100;	int16_t SpeedPump1Rpmx100;	int16_t SpeedPump2Rpmx100;	int16_t SpeedPump3Rpmx100;
	} STxCan3;
	struct {
		uint8_t Free1;	uint8_t Free2;	uint8_t Free3;	uint8_t Free4;	uint8_t Free5;	uint8_t Free6;	uint8_t Free7;	uint8_t Free8;
	} STxCan4;
	struct {
		uint8_t Free1;	uint8_t Free2;	uint8_t Free3;	uint8_t Free4;	uint8_t Free5;	uint8_t Free6;	uint8_t Free7;	uint8_t Free8;
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
}

// incoming new press values
#ifdef CAN_DEBUG
void onNewPressOxygen(uint16_t Value)  	{  }
void onNewTubPressLevel(uint16_t Value) {  }
void onNewPressADSFLT(uint16_t  Value) 	{  }
void onNewPressVen(uint16_t  Value)		{  }
void onNewPressArt(uint16_t  Value)		{  }
#else
void onNewPressOxygen(uint16_t Value)  	{  	TxCan1.STxCan1.PressOxy = Value; }
void onNewTubPressLevel(uint16_t Value) { 	TxCan0.STxCan0.PressLevelx100 = Value; }
void onNewPressADSFLT(uint16_t  Value) 	{	TxCan0.STxCan0.PressFilter = Value;	}
void onNewPressVen(uint16_t  Value)		{	TxCan0.STxCan0.PressVen = Value; }
void onNewPressArt(uint16_t  Value)		{	TxCan0.STxCan0.PressArt = Value; }
#endif


/* 6 RPM is the minimum speed value calculate by protective */
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

// incoming new RPM values ( reduntant funtions , for possible future use )
void onNewFilterPumpRPM(int16_t Value) {
	TxCan3.STxCan3.SpeedPump0Rpmx100 = Value;
}
//
//void onNewArtLiverPumpRPM(int16_t Value) {
//	TxCan3.STxCan3.SpeedPump1Rpmx100 = Value;
//}
//void onNewOxy1PumpRPM(int16_t Value) {
//	TxCan3.STxCan3.SpeedPump2Rpmx100 = Value;
//}
//void onNewOxy2PumpRPM(int16_t Value) {
//	TxCan3.STxCan3.SpeedPump3Rpmx100 = Value;
//}


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


int TxBuffIndex = 0;

union UTxCan*	TxBuffCanP[8] =
{
	&TxCan0, &TxCan1, &TxCan2, &TxCan3, &TxCan4 , &TxCan5 , &TxCan6 , &TxCan7
};

union UTxCan*	OldTxBuffCanP[8] =
{
	&OldTxCan0, &OldTxCan1, &OldTxCan2, &OldTxCan3, &OldTxCan4 , &OldTxCan5 , &OldTxCan6 , &OldTxCan7
};


//
// send altorithm 1
//
void ManageTxCan10ms(void)
{
	if(memcmp(TxBuffCanP[TxBuffIndex]->RawCanBuffer,OldTxBuffCanP[TxBuffIndex]->RawCanBuffer,SIZE_CAN_BUFFER) != 0) {
		// 8 buffer are scanned each 80ms , amximum delay since value changed and tx on can
		SendCAN(TxBuffCanP[TxBuffIndex]->RawCanBuffer, SIZE_CAN_BUFFER, 0);
		memcpy(OldTxBuffCanP[TxBuffIndex]->RawCanBuffer, TxBuffCanP[TxBuffIndex]->RawCanBuffer, SIZE_CAN_BUFFER);
		TxBuffCanP[TxBuffIndex]->SAux.NoSendCounter = 0;
	}
	else {
		TxBuffCanP[TxBuffIndex]->SAux.NoSendCounter++;
		if(TxBuffCanP[TxBuffIndex]->SAux.NoSendCounter == 5) {
			// in any case , each 400ms send a buffer even if nothing changed on it , to prevent possible data loss
			SendCAN(TxBuffCanP[TxBuffIndex]->RawCanBuffer, SIZE_CAN_BUFFER, 0);
			// don't need to copy new buff in old one because they already match
			TxBuffCanP[TxBuffIndex]->SAux.NoSendCounter = 0;
		}
	}
	TxBuffIndex = (TxBuffIndex + 1) % (LAST_INDEX_TXBUFF2SEND+1);
}


//
// send all at a time
//
void ManageTxCan100ms(void)
{
int ii ;//= 11;

	for( ii=8 ; ii<=LAST_INDEX_TXBUFF2SEND ; ii++)
		SendCAN(TxBuffCanP[ii - 8]->RawCanBuffer, SIZE_CAN_BUFFER, ii);

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
			}
			// !! keep this statement after vals management
			memcpy(OldRxBuffCanP[RxChannel]->RawCanBuffer,
					RxBuffCanP[RxChannel]->RawCanBuffer, SIZE_CAN_BUFFER);
		}
	}
}

void NewDataRxChannel0(void) {
	VerifyRxState(RxCan0.SRxCan0.State, RxCan0.SRxCan0.Parent,
			RxCan0.SRxCan0.Child, RxCan0.SRxCan0.Guard);
}

void NewDataRxChannel1(void) {
	VerifyRxPressures(RxCan1.SRxCan1.PressFilter, RxCan1.SRxCan1.PressArt,
			RxCan1.SRxCan1.PressVen, RxCan1.SRxCan1.PressLevelx100,
			RxCan2.SRxCan2.PressOxy);
}

void NewDataRxChannel2(void) {
	// pressure changed
	if (RxCan2.SRxCan2.PressOxy != OldRxCan2.SRxCan2.PressOxy) {
		// oxy pressure changed
		VerifyRxPressures(RxCan1.SRxCan1.PressFilter, RxCan1.SRxCan1.PressArt,
				RxCan1.SRxCan1.PressVen, RxCan1.SRxCan1.PressLevelx100,
				RxCan2.SRxCan2.PressOxy);
	}

	// temperature changed
	if ((RxCan2.SRxCan2.TempArtx10 != OldRxCan2.SRxCan2.TempArtx10)
			|| (RxCan2.SRxCan2.TempVenx10 != OldRxCan2.SRxCan2.TempVenx10)
			|| (RxCan2.SRxCan2.TempFluidx10 != OldRxCan2.SRxCan2.TempFluidx10)) {

		VerifyRxTemperatures(RxCan2.SRxCan2.TempArtx10,
				RxCan2.SRxCan2.TempFluidx10, RxCan2.SRxCan2.TempVenx10);
	}
}

void NewDataRxChannel3(void) {
	// air alarm
	if (RxCan3.SRxCan3.AirAlarm != OldRxCan3.SRxCan3.AirAlarm) {
		VerifyRxAirAlarm(RxCan3.SRxCan3.AirAlarm & 0x01);

		if ( (RxCan3.SRxCan3.AirAlarm & 0x02) && !(RxCan3.SRxCan3.AirAlarm & 0x04) )
		{
			//aziono il relè del riscaldatore
			if (!COMP_ENABLE_GetVal())
			{
				Enable_Heat(TRUE);
				Enable_Frigo(FALSE);
			}
		}
		else if ( !(RxCan3.SRxCan3.AirAlarm & 0x02) && (RxCan3.SRxCan3.AirAlarm & 0x04) )
		{
			//aziono il relè del frigo
			if (!HEAT_ON_P_GetVal())
			{
				Enable_Frigo(TRUE);
				Enable_Heat(FALSE);
			}
		}
		else if ( (RxCan3.SRxCan3.AirAlarm & 0x02) && (RxCan3.SRxCan3.AirAlarm & 0x04) )
		{
			//vado in allarme e spengo entrambi i relè perchè
			//la control mi chiede di attivarli entrambi, cosa non corretta
			Enable_Heat(FALSE);
			Enable_Frigo(FALSE);
		}

	}

	// Alarm code
	if (RxCan3.SRxCan3.AlarmCode != OldRxCan3.SRxCan3.AlarmCode) {
		ManageRxAlarmCode(RxCan3.SRxCan3.AlarmCode);
	}

	// pinch pos
	if ((RxCan3.SRxCan3.Pinch0Pos != OldRxCan3.SRxCan3.Pinch0Pos)
			|| (RxCan3.SRxCan3.Pinch1Pos != OldRxCan3.SRxCan3.Pinch1Pos)
			|| (RxCan3.SRxCan3.Pinch2Pos != OldRxCan3.SRxCan3.Pinch2Pos)) {

		VerifyRxPinchPos(RxCan3.SRxCan3.Pinch0Pos, RxCan3.SRxCan3.Pinch1Pos,
				RxCan3.SRxCan3.Pinch2Pos);
	}
}

void NewDataRxChannel4(void) {
	VerifyRxPumpsRpm(RxCan4.SRxCan4.SpeedPump0Rpmx100,
			RxCan4.SRxCan4.SpeedPump1Rpmx100, RxCan4.SRxCan4.SpeedPump2Rpmx100,
			RxCan4.SRxCan4.SpeedPump3Rpmx100);
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

