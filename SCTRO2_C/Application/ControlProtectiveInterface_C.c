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
#include "FlexCANWrapper_c.h"
#include "Global.h"
#include "Alarm_Con.h"
#include "App_Ges.h"


#define VAL_JOLLY16	0x5A5A
#define VAL_JOLLY8	0x5A
#define SIZE_CAN_BUFFER	8
#define LAST_INDEX_TXBUFF2SEND 7	//



union UTxCan {
	uint8_t RawCanBuffer[SIZE_CAN_BUFFER];
	struct {
		uint8_t Filler[SIZE_CAN_BUFFER]; uint8_t NoSendCounter;
	} SAux;
	struct {
		uint16_t	State;	uint16_t	Parent;	uint16_t	Child;	uint16_t	Guard;
	} STxCan0;
	struct {
		uint16_t PressFilter;	uint16_t PressArt;	uint16_t PressVen;	uint16_t PressLevelx100;
	} STxCan1;
	struct {
		uint16_t PressOxy;	uint16_t TempFluidx10;	uint16_t TempArtx10;	uint16_t TempVenx10;
	} STxCan2;
	struct {
		uint8_t AirAlarm;	uint16_t AlarmCode;	uint8_t FilterPinchPos;	uint8_t ArtPinchPos; uint8_t OxygPinchPos;	uint8_t Free1; uint8_t Free2;
	} STxCan3;
	struct {
		uint16_t SpeedPump1Rpmx10;	uint16_t SpeedPump2Rpmx10;	uint16_t SpeedPump3Rpmx10;	uint16_t SpeedPump4Rpmx10;
	} STxCan4;

#ifdef ENABLE_PROTECTIVE_ALARM_RESET
	struct {
		uint16_t AlmCodeToreset;	uint8_t Free3;	uint8_t Free4;	uint8_t Free5;	uint8_t Free6;	uint8_t Free7;	uint8_t Free8;
	} STxCan5;
#else
	// Filippo - messo campo per lo scambio della temperatura di piatto
	struct {
		uint8_t Free1;	uint8_t Free2;	int16_t tempPlateC;	uint8_t Free5;	uint8_t Free6;	uint8_t Free7;	uint8_t Free8;
	} STxCan5;
#endif
	struct {
		uint8_t Free1;	uint8_t Free2;	uint8_t Free3;	uint8_t Free4;	uint8_t Free5;	uint8_t Free6;	uint8_t Free7;	uint8_t Free8;
	} STxCan6;
	struct {
		uint16_t MsgType;	uint8_t MsgParams[6];
	} STxCan7;
};

union URxCan {
	uint8_t RawCanBuffer[SIZE_CAN_BUFFER];
	struct {
		uint16_t PressFilter;	uint16_t PressArt;	uint16_t PressVen;	uint16_t PressLevelx100;
	} SRxCan0;
	struct {
		uint16_t PressOxy;	uint16_t TempFluidx10;	uint16_t TempArtx10;	uint16_t TempVenx10;
	} SRxCan1;
	struct {
		uint8_t AirAlarm;	uint16_t AlarmCode;	uint8_t Consenso;	uint8_t FilterPinchPos;	uint8_t ArtPinchPos; uint8_t OxygPinchPos;	uint8_t Free;
	} SRxCan2;
	struct {
		uint16_t SpeedPump1Rpmx10;	uint16_t SpeedPump2Rpmx10;	uint16_t SpeedPump3Rpmx10;	uint16_t SpeedPump4Rpmx10;
	} SRxCan3;
	// Filippo - messo campo per lo scambio della temperatura di piatto
	struct {
		uint8_t Free1;	uint8_t Free2;	uint8_t Free3;	uint8_t Free4;	uint8_t Free5;	uint8_t Free6;	uint8_t Free7;	uint8_t Free8;
	} SRxCan4;
	struct {
		uint8_t Free1;	uint8_t Free2;	int16_t tempPlateP;	uint8_t Free5;	uint8_t Free6;	uint8_t Free7;	uint8_t Free8;
	} SRxCan5;
	struct {
		uint8_t Free1;	uint8_t Free2;	uint8_t Free3;	uint8_t Free4;	uint8_t Free5;	uint8_t Free6;	uint8_t Free7;	uint8_t Free8;
	} SRxCan6;
	struct {
		uint16_t MsgType;	uint8_t MsgParams[6];
	} SRxCan7;
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

uint8_t *FilterPinchPos = &(RxCan2.SRxCan2.FilterPinchPos);
uint8_t *ArtPinchPos = &(RxCan2.SRxCan2.ArtPinchPos);
uint8_t *OxygPinchPos = &(RxCan2.SRxCan2.OxygPinchPos);

uint16_t *SpeedPump1Rpmx10 = &(RxCan3.SRxCan3.SpeedPump1Rpmx10);
uint16_t *SpeedPump2Rpmx10 = &(RxCan3.SRxCan3.SpeedPump2Rpmx10);
uint16_t *SpeedPump3Rpmx10 = &(RxCan3.SRxCan3.SpeedPump3Rpmx10);
uint16_t *SpeedPump4Rpmx10 = &(RxCan3.SRxCan3.SpeedPump4Rpmx10);




//#define CAN_DEBUG 1

void ManageTxCan10ms(void);
void ManageTxCan50ms(void);
void DebugFillTxBuffers(void);

void InitControlProtectiveInterface(void)
{
	int ii;

	for(ii=0;ii<SIZE_CAN_BUFFER;ii++){
		TxCan1.RawCanBuffer[ii] = VAL_JOLLY8;
		TxCan2.RawCanBuffer[ii] = VAL_JOLLY8;
		TxCan3.RawCanBuffer[ii] = VAL_JOLLY8;
		TxCan4.RawCanBuffer[ii] = VAL_JOLLY8;
		TxCan5.RawCanBuffer[ii] = VAL_JOLLY8;
		TxCan6.RawCanBuffer[ii] = VAL_JOLLY8;
		TxCan7.RawCanBuffer[ii] = VAL_JOLLY8;
		// cause difference to trigger data send 1st time
		OldTxCan1.RawCanBuffer[ii] = VAL_JOLLY8 + 1;
		OldTxCan2.RawCanBuffer[ii] = VAL_JOLLY8 + 1;
		OldTxCan3.RawCanBuffer[ii] = VAL_JOLLY8 + 1;
		OldTxCan4.RawCanBuffer[ii] = VAL_JOLLY8 + 1;
		OldTxCan5.RawCanBuffer[ii] = VAL_JOLLY8 + 1;
		OldTxCan6.RawCanBuffer[ii] = VAL_JOLLY8 + 1;
		OldTxCan7.RawCanBuffer[ii] = VAL_JOLLY8 + 1;
		// init rx buffers
		RxCan1.RawCanBuffer[ii] = VAL_JOLLY8;
		RxCan2.RawCanBuffer[ii] = VAL_JOLLY8;
		RxCan3.RawCanBuffer[ii] = VAL_JOLLY8;
		RxCan4.RawCanBuffer[ii] = VAL_JOLLY8;
		RxCan5.RawCanBuffer[ii] = VAL_JOLLY8;
		RxCan6.RawCanBuffer[ii] = VAL_JOLLY8;
		RxCan7.RawCanBuffer[ii] = VAL_JOLLY8;

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
	AddSwTimer(ManageTxCan50ms,5,TM_REPEAT);
}

#ifdef CAN_DEBUG

// incoming new press values
void onNewPressOxygen(uint16_t Value)  	{   }
void onNewTubPressLevel(uint16_t Value) { 	}
void onNewPressADSFLT(uint16_t  Value) 	{		}
void onNewPressVen(uint16_t  Value)		{	 }
void onNewPressArt(uint16_t  Value)		{	 }
void onNewState( struct machineState* MSp, struct machineParent* MPp ,
				  struct machineChild* MCp, uint16_t Guard   )
{
}

void onNewPumpSpeed(uint16_t Pump0Speed, uint16_t Pump1Speed ,
		            uint16_t Pump2Speed, uint16_t Pump3Speed)
{
}

void onNewSensPressVal(uint16_t PressFilt, uint16_t PressArt ,
		               uint16_t PressVen, uint16_t PressLev)
{
}

void onNewSensTempVal(uint16_t PressOxyg, uint16_t TempRes,
		               uint16_t TempArt, uint16_t TempVen)
{
}

void onNewPinchVal(uint8_t AirFiltStat, uint16_t AlarmCode,
		           uint8_t Pinch2WPVF, uint8_t Pinch2WPVA, uint8_t Pinch2WPVV,
				   uint8_t free1, uint8_t free2)
{
}

#ifdef ENABLE_PROTECTIVE_ALARM_RESET
void onNewAlmToResetMsg(uint16_t AlmCodeToreset)
{
}
#endif

#else

// incoming new press values
void onNewPressOxygen(uint16_t Value)  	{  	TxCan1.STxCan2.PressOxy = Value; }
void onNewTubPressLevel(uint16_t Value) { 	TxCan0.STxCan1.PressLevelx100 = Value; }
void onNewPressADSFLT(uint16_t  Value) 	{	TxCan0.STxCan1.PressFilter = Value;	}
void onNewPressVen(uint16_t  Value)		{	TxCan0.STxCan1.PressVen = Value; }
void onNewPressArt(uint16_t  Value)		{	TxCan0.STxCan1.PressArt = Value; }
void onNewState( struct machineState* MSp, struct machineParent* MPp ,
				  struct machineChild* MCp, uint16_t Guard   )
{
	TxCan0.STxCan0.State = MSp->state;
	TxCan0.STxCan0.Parent = MPp->parent;
	TxCan0.STxCan0.Child = MCp->child;
	TxCan0.STxCan0.Guard = Guard;
}

void onNewSensPressVal(uint16_t PressFilt, uint16_t PressArt,
		               uint16_t PressVen, uint16_t PressLev)
{
	TxCan1.STxCan1.PressFilter = PressFilt;
	TxCan1.STxCan1.PressArt = PressArt;
	TxCan1.STxCan1.PressVen = PressVen;
	TxCan1.STxCan1.PressLevelx100 = PressLev;
}

void onNewSensTempVal(uint16_t PressOxyg, uint16_t TempRes,
		               uint16_t TempArt, uint16_t TempVen)
{
	TxCan2.STxCan2.PressOxy = PressOxyg;
	TxCan2.STxCan2.TempFluidx10 = TempRes;
	TxCan2.STxCan2.TempArtx10 = TempArt;
	TxCan2.STxCan2.TempVenx10 = TempVen;
}

void onNewPinchVal(uint8_t AirFiltStat, uint16_t AlarmCode,
		           uint8_t Pinch2WPVF, uint8_t Pinch2WPVA, uint8_t Pinch2WPVV,
				   uint8_t free1, uint8_t free2)
{
	TxCan3.STxCan3.AirAlarm = AirFiltStat;
	TxCan3.STxCan3.AlarmCode = AlarmCode;
	TxCan3.STxCan3.FilterPinchPos = Pinch2WPVF;
	TxCan3.STxCan3.ArtPinchPos = Pinch2WPVA;
	TxCan3.STxCan3.OxygPinchPos = Pinch2WPVV;
	TxCan3.STxCan3.Free1 = free1;
	TxCan3.STxCan3.Free2 = free2;
}

void onNewPumpSpeed(uint16_t Pump0Speed, uint16_t Pump1Speed ,
		            uint16_t Pump2Speed, uint16_t Pump3Speed)
{
	TxCan4.STxCan4.SpeedPump1Rpmx10 = Pump0Speed;
	TxCan4.STxCan4.SpeedPump2Rpmx10 = Pump1Speed;
	TxCan4.STxCan4.SpeedPump3Rpmx10 = Pump2Speed;
	TxCan4.STxCan4.SpeedPump4Rpmx10 = Pump3Speed;
}

#ifdef ENABLE_PROTECTIVE_ALARM_RESET
void onNewAlmToResetMsg(uint16_t AlmCodeToreset)
{
	TxCan5.STxCan5.AlmCodeToreset = AlmCodeToreset;
	TxCan5.STxCan5.Free3 = 0;
	TxCan5.STxCan5.Free4 = 0;
	TxCan5.STxCan5.Free5 = 0;
	TxCan5.STxCan5.Free6 = 0;
	TxCan5.STxCan5.Free7 = 0;
	TxCan5.STxCan5.Free8 = 0;
}
#endif


#endif

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
void ManageTxCan50ms(void)
{
	int ii;

	for( ii=0 ; ii<=LAST_INDEX_TXBUFF2SEND ; ii++)
		SendCAN(TxBuffCanP[ii]->RawCanBuffer, SIZE_CAN_BUFFER, ii);
#ifdef CAN_DEBUG
	DebugFillTxBuffers();
#endif
}

union URxCan*	RxBuffCanP[8] =
{
	&RxCan0, &RxCan1, &RxCan2, &RxCan3, &RxCan4 , &RxCan5 , &RxCan6 , &RxCan7
};

union URxCan*	OldRxBuffCanP[8] =
{
	&OldRxCan0, &OldRxCan1, &OldRxCan2, &OldRxCan3, &OldRxCan4 , &OldRxCan5 , &OldRxCan6 , &OldRxCan7
};

void RetriggerAlarm(void);
void onNewCanBusMsg9(CANBUS_MSG_9 ReceivedCanBusMsg9);
void onNewCanBusMsg10(CANBUS_MSG_10 ReceivedCanBusMsg10);
void onNewCanBusMsg11( CANBUS_MSG_11 ReceivedCanBusMsg11);
void onNewCanBusMsg12(CANBUS_MSG_12 ReceivedCanBusMsg12);
// Filippo - gestione della ricezione del messaggio CAN dalla protective contenente il valore di temperatura del piatto come letto
// dalla protective stessa
void onNewCanBusMsg13(CANBUS_MSG_13 ReceivedCanBusMsg13);

void ReceivedCanData(uint8_t *rxbuff, int rxlen, int RxChannel)
{
	CANBUS_MSG_9 TempCanBusMsg9;
	CANBUS_MSG_10 TempCanBusMsg10;
	CANBUS_MSG_11 TempCanBusMsg11;
	CANBUS_MSG_12 TempCanBusMsg12;
	CANBUS_MSG_13 TempCanBusMsg13;

	RetriggerAlarm();
	if(( rxlen <= 8 ) && (RxChannel >= 8) /*>= 8) && (RxChannel <= 15)*/){
		memcpy( RxBuffCanP[RxChannel - 8]->RawCanBuffer , rxbuff, rxlen);
//		if(memcmp(RxBuffCanP[RxChannel - 8]->RawCanBuffer , OldRxBuffCanP[RxChannel - 8]->RawCanBuffer, SIZE_CAN_BUFFER) != 0){
			// data changed --> trigger some action
			memcpy(OldRxBuffCanP[RxChannel -8]->RawCanBuffer , RxBuffCanP[RxChannel - 8]->RawCanBuffer, SIZE_CAN_BUFFER);
			if( RxChannel == 8)
			{
				TempCanBusMsg9.PressSensFilter = RxBuffCanP[RxChannel-8]->SRxCan0.PressFilter;
				TempCanBusMsg9.PressSensArt = RxBuffCanP[RxChannel-8]->SRxCan0.PressArt;
				TempCanBusMsg9.PressSensVen = RxBuffCanP[RxChannel-8]->SRxCan0.PressVen;
				TempCanBusMsg9.PressSensLevel = RxBuffCanP[RxChannel-8]->SRxCan0.PressLevelx100;
				onNewCanBusMsg9(TempCanBusMsg9);
			}
			if( RxChannel == 9)
			{
				TempCanBusMsg10.PressSensOxyg = RxBuffCanP[RxChannel-8]->SRxCan1.PressOxy;
				TempCanBusMsg10.TempSensFluid = RxBuffCanP[RxChannel-8]->SRxCan1.TempFluidx10;
				TempCanBusMsg10.TempSensArt = RxBuffCanP[RxChannel-8]->SRxCan1.TempArtx10;
				TempCanBusMsg10.TempSensVen = RxBuffCanP[RxChannel-8]->SRxCan1.TempVenx10;
				onNewCanBusMsg10(TempCanBusMsg10);
			}
			if( RxChannel == 10)
			{
				TempCanBusMsg11.AirSensorFilter = RxBuffCanP[RxChannel-8]->SRxCan2.AirAlarm;
				TempCanBusMsg11.AlarmCode = RxBuffCanP[RxChannel-8]->SRxCan2.AlarmCode;
				TempCanBusMsg11.Consensus = RxBuffCanP[RxChannel-8]->SRxCan2.Consenso;
				TempCanBusMsg11.FilterPinchPos = RxBuffCanP[RxChannel-8]->SRxCan2.FilterPinchPos;
				TempCanBusMsg11.ArtPinchPos = RxBuffCanP[RxChannel-8]->SRxCan2.ArtPinchPos;
				TempCanBusMsg11.OxygPinchPos = RxBuffCanP[RxChannel-8]->SRxCan2.OxygPinchPos;
				TempCanBusMsg11.free = RxBuffCanP[RxChannel-8]->SRxCan2.Free;
				onNewCanBusMsg11(TempCanBusMsg11);
			}
			if( RxChannel == 11)
			{
				TempCanBusMsg12.SpeedPump1 = RxBuffCanP[RxChannel-8]->SRxCan3.SpeedPump1Rpmx10;
				TempCanBusMsg12.SpeedPump2 = RxBuffCanP[RxChannel-8]->SRxCan3.SpeedPump2Rpmx10;
				TempCanBusMsg12.SpeedPump3 = RxBuffCanP[RxChannel-8]->SRxCan3.SpeedPump3Rpmx10;
				TempCanBusMsg12.SpeedPump4 = RxBuffCanP[RxChannel-8]->SRxCan3.SpeedPump4Rpmx10;
				onNewCanBusMsg12(TempCanBusMsg12);
			}
			// Filippo gestita la ricezione del messaggio CAN che mi da il valore della temperatura letta sul piatto dalla protective
			if( RxChannel == 13)
			{
				TempCanBusMsg13.free1 = RxBuffCanP[RxChannel-8]->SRxCan5.Free1;
				TempCanBusMsg13.free2 = RxBuffCanP[RxChannel-8]->SRxCan5.Free2;
				TempCanBusMsg13.tempPlateP = RxBuffCanP[RxChannel-8]->SRxCan5.tempPlateP;
				TempCanBusMsg13.free3 = RxBuffCanP[RxChannel-8]->SRxCan5.Free5;
				TempCanBusMsg13.free4 = RxBuffCanP[RxChannel-8]->SRxCan5.Free6;
				TempCanBusMsg13.free5 = RxBuffCanP[RxChannel-8]->SRxCan5.Free7;
				TempCanBusMsg13.free6 = RxBuffCanP[RxChannel-8]->SRxCan5.Free8;
				onNewCanBusMsg13(TempCanBusMsg13);
			}

		//}
	}
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

uint16_t ReadProtectiveAlarmCode(void)
{
	uint16_t u16 = RxBuffCanP[2]->SRxCan2.AlarmCode;
	return u16;
}


#ifdef ENABLE_PROTECTIVE_ALARM_RESET
unsigned long msTick10_elapsed( unsigned long last );

typedef enum
{
	INIT_PROT_ALM_ST,
	WAIT_FOR_ALM_PROT_ALM_ST,
	ALM_ON_PROT_ALM_ST,
	WAIT_FOR_RES_ACK_FROM_PROT,
	WAIT_FOR_RES_ACK_FROM_PROT_1
}PROTECTIVE_ALARM_STATE;

// Controllo se sono in allarme su protective
// Se sono in allarme su protective invio su canbus il button reset in continuazione
// fino a quando l'allarme non va via o con un timeout di 1 sec,
// Una volta scaduto il timeout l'allarme ritorna
void HandleProtectiveAlarm(void)
{
	static uint32_t startTimeInterv = 0;
	static PROTECTIVE_ALARM_STATE ProtectiveAlarmState = INIT_PROT_ALM_ST;

	switch (ProtectiveAlarmState)
	{
		case INIT_PROT_ALM_ST:
			startTimeInterv = 0;
			ProAlmCodeToreset = 0;
			ProtectiveAlarmState = WAIT_FOR_ALM_PROT_ALM_ST;
			break;
		case WAIT_FOR_ALM_PROT_ALM_ST:
			if(IsProtectiveInAlarm())
			{
				ProtectiveAlarmState = ALM_ON_PROT_ALM_ST;
			}
			break;
		case ALM_ON_PROT_ALM_ST:
			if(buttonGUITreatment[BUTTON_RESET].state == GUI_BUTTON_RELEASED)
			{
				// faccio il release, altrimenti verrebbe preso dalla scheda control
				releaseGUIButton(BUTTON_RESET);
				// la scheda protective e' in allarme ed ho ricevuto un button reset per
				// resettare l'allarme.
				// lo invio su canbus
				ProAlmCodeToreset = alarmCurrent.code;
				startTimeInterv = FreeRunCnt10msec;
				ProtectiveAlarmState = WAIT_FOR_RES_ACK_FROM_PROT;
			}
			break;
		case WAIT_FOR_RES_ACK_FROM_PROT:
			if(!RxBuffCanP[2]->SRxCan2.AlarmCode)
			{
				ProtectiveAlarmState = WAIT_FOR_RES_ACK_FROM_PROT_1;
				startTimeInterv = FreeRunCnt10msec;
			}
			else if(startTimeInterv && (msTick10_elapsed(startTimeInterv) >= 100))
			{
				// dopo un secondo se l'allarme non e' andato via smetto di inviare reset
				// perche' vuol dire che la condizione fisica di allarme non e' stata rimossa
				ProtectiveAlarmState = INIT_PROT_ALM_ST;
			}
			break;
		case WAIT_FOR_RES_ACK_FROM_PROT_1:
			if(!RxBuffCanP[2]->SRxCan2.AlarmCode)
			{
				if(startTimeInterv && (msTick10_elapsed(startTimeInterv) >= 20))
				{
					// dopo 200 msec che l'allarme e' 0 torno in init
					ProtectiveAlarmState = INIT_PROT_ALM_ST;
				}
			}
			else
			{
				// dopo un secondo se l'allarme non e' andato via smetto di inviare reset
				ProtectiveAlarmState = WAIT_FOR_RES_ACK_FROM_PROT;
				startTimeInterv = FreeRunCnt10msec;
			}
			break;
	}
}




// PER IL MOMENTO NON USO QUESTA FUNZIONE

// Questo task fa partire gli allarmi in tutti gli stati anche in quelli dove, nello stato parent, non è prevista
// la gestione degli interrupt in base alle strutture machineParent
void HandleProtectiveAlarm_old(void)
{
	static uint16_t ProtectiveAlarm = 0;
	static HANDLE_PROTECTIVE_ALARM_STATE HandleProtectiveAlarmState = INIT_HNDLE_PROT_ALM;
	static uint32_t startTimeInterv = 0;
	static GLOBAL_FLAGS gbf;


	switch (HandleProtectiveAlarmState)
	{
		case INIT_HNDLE_PROT_ALM:
			startTimeInterv = 0;
			ProtectiveAlarm = 0;
			ProAlmCodeToreset = 0;
			HandleProtectiveAlarmState = WAIT_FOR_PROT_ALM;
			break;
		case WAIT_FOR_PROT_ALM:
			if(!IsControlInAlarm())
			{
				// NON C'E' UN ALLARME DELLA CONTROL
				uint16_t u16 = RxBuffCanP[2]->SRxCan2.AlarmCode;
				if((u16 >= START_PROTECTIVE_ALARM_CODE) && (u16 == ProtectiveAlarm))
				{
					if(AmJInAlarmHandledState())
					{
						// SONO IN UNO STATO IN CUI E' PREVISTA LA GENERAZIONE DELL'ALLARME IN BASE ALLA
						// STRUTTURA PARENTMACHINE, QUINDI NON HO BISOGNO
					}
					else
					{
						if(startTimeInterv && (msTick10_elapsed(startTimeInterv) >= 12))
						{
							// sono passati 120 msec e un allarme mi e' stato inviato dalla protective
							// devo inviarlo alla GUI
							StopAllCntrlAlarm(&gbf);
							ProtectiveAlarmStruct.code = ProtectiveAlarm; 			        /* alarm code */
							ProtectiveAlarmStruct.physic = PHYSIC_TRUE;			            /* alarm physic condition */
							ProtectiveAlarmStruct.active = ACTIVE_TRUE;			            /* alarm active condition */
							ProtectiveAlarmStruct.type = ALARM_TYPE_PROTECTION;			    /* alarm type: control, protection */
							ProtectiveAlarmStruct.secActType = SECURITY_STOP_ALL_ACTUATOR;	/* alarm security action: type of secuirty action required
																							  (FM modificato da char ad unsigned int perche' ho bisogno di piu'
																							  bit per distinguere i vari allarmi*/
							ProtectiveAlarmStruct.priority = PRIORITY_HIGH;		            /* alarm priority: low, medium, right */
							ProtectiveAlarmStruct.entryTime = 0;		                    /* entry time in ms */
							ProtectiveAlarmStruct.exitTime = 0;		                        /* exit time in ms */
							ProtectiveAlarmStruct.ovrdEnable = OVRD_NOT_ENABLED;		    /* override enable: alarm can be overridden when alarm condition is still present */
							ProtectiveAlarmStruct.resettable = RESET_ALLOWED;		        /* reset property */
							ProtectiveAlarmStruct.silence = SILENCE_ALLOWED;		        /* silence property: the alarm acoustic signal can be silenced for a limited period of time */
							ProtectiveAlarmStruct.memo = MEMO_NOT_ALLOWED;			        /* memo property: the system remain in the alarm state even if the alarm condition is no longer present */
							ProtectiveAlarmStruct.prySafetyActionFunc = 0;                  /* safety action: funzione che esegue la funzione di sicurezza in base alla priorità dell'allarme */

							// faccio partire l'allarme alla GUI
							alarmCurrent = ProtectiveAlarmStruct;
							HandleProtectiveAlarmState = ENTER_PROT_ALM;
						}
					}
				}
				else if(u16 >= START_PROTECTIVE_ALARM_CODE)
				{
					startTimeInterv = FreeRunCnt10msec;
					ProtectiveAlarm = u16;
				}
			}
			break;
		case ENTER_PROT_ALM:
			// aspetto il reset dalla GUI
			if(buttonGUITreatment[BUTTON_RESET].state == GUI_BUTTON_RELEASED)
			{
				ProAlmCodeToreset = alarmCurrent.code;
				HandleProtectiveAlarmState = WAIT_FOR_PROT_ALM_OFF;
				releaseGUIButton(BUTTON_RESET);
				startTimeInterv = FreeRunCnt10msec;
			}
			break;
		case WAIT_FOR_PROT_ALM_OFF:
			// aspetto che la protective tolga la condizione di allarme
			if(!RxBuffCanP[2]->SRxCan2.AlarmCode)
			{
				ProAlmCodeToreset = 0;
				memset(&alarmCurrent, 0, sizeof(struct alarm));
				RestoreAllCntrlAlarm(&gbf);
				// ritorno in attesa di un nuovo allarme
				HandleProtectiveAlarmState = INIT_HNDLE_PROT_ALM;
			}
			else if(startTimeInterv && (msTick10_elapsed(startTimeInterv) >= 50))
			{
				// se entro 500 msec la condizione di allarme non e' stata rimossa ritorno
				// nello stato di attesa reset e mantengo il codice di allarme inviato alla GUI
				HandleProtectiveAlarmState = ENTER_PROT_ALM;
			}
			break;

	}
}

#endif

