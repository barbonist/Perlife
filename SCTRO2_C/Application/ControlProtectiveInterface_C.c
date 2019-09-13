/*
 * ControlProtectiveInterface.c
 *
 *  Created on: 13/apr/2018
 *      Author: W5
 */

#include <stdio.h>
#include <float.h>
#include "stdint.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "stdint.h"
#include "SwTimer.h"
#include "FlexCANWrapper_c.h"
#include "Global.h"
#include "Alarm_Con.h"
#include "App_Ges.h"
#include "ModBusCommProt.h"
#include "PC_DEBUG_COMM.h"
#include "VOLTAGE_B_CHK.h"
#include "VOLTAGE_M_CHK.h"
#include "ControlProtectiveInterface_C.h"

#define VAL_JOLLY16	0x5A5A
#define VAL_JOLLY8	0x5A
#define SIZE_CAN_BUFFER	8
#define LAST_INDEX_TXBUFF2SEND 7	//

#define ESC 27
#define CR  0x0D
#define LF  0x0A

uint8_t CharReceived(void);
void ManageTxDebug(void);
void ManageGetParams(void);
void TxDebugPressures(void);
void TxDebugTemperatures(void);
void TxDebugPinch(void);
void TxDebugErrors(void);
void SetLogCommand( uint8_t Command);
void TxDebugPumpSpeed(void);
void TxPumpCovers(void);
void TxAirSensors(void);
void TxCanBus(void);
void TxPowerVoltage(void);
void TxDoors(void);
void TxReservorireHooks(void);
void TxFWVersion(void);

union UTxCan {
	uint8_t RawCanBuffer[SIZE_CAN_BUFFER];
//	struct {
//		uint8_t Filler[SIZE_CAN_BUFFER]; uint8_t NoSendCounter;
//	} SAux;
	struct {
		uint16_t	State;	uint16_t	Parent;	uint16_t	Child;	uint16_t	Guard;
	} STxCan0;
	struct {
		uint16_t PressFilter;	uint16_t PressArt;	uint16_t PressVen;	uint16_t PressLevelx100;
	} STxCan1;
	struct {
		uint16_t PressOxy;	int16_t TempFluidx10;	int16_t TempArtx10;	int16_t TempVenx10;
	} STxCan2;
	struct {
		uint16_t AlarmCode; uint8_t AirAlarm; uint8_t FilterPinchPos; uint8_t ArtPinchPos; uint8_t OxygPinchPos; uint8_t Offset_Press_Ven; uint8_t Offset_Press_Art;
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
		int16_t AlarmCodeToReset;	int16_t tempPlateC;	uint8_t TerapyType;	uint8_t ArtLineAirMeasure;	uint8_t VenLineAirMeasure;	uint8_t Command;
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

	struct {
		uint8_t Free1;	uint8_t Free2;	uint8_t Free3;	uint8_t Free4;	uint8_t Free5;	uint8_t Free6;	uint8_t Free7;	uint8_t Free8;
	} SRxCan4;
	// Filippo - messo campo per lo scambio della temperatura di piatto
	struct {
		int16_t FW_Revision_Protective;	int16_t tempPlateP;	uint8_t Free5;	uint8_t Free6;	uint8_t Free7;	uint8_t Free8;
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

// Valori di pressione in arrivo dalla Protective
uint16_t *PressOxy       = &(RxCan1.SRxCan1.PressOxy);
uint16_t *PressFilter    = &(RxCan0.SRxCan0.PressFilter);
uint16_t *PressArt       = &(RxCan0.SRxCan0.PressArt);
uint16_t *PressVen       = &(RxCan0.SRxCan0.PressVen);
uint16_t *PressLevelx100 = &(RxCan0.SRxCan0.PressLevelx100);

uint16_t *SpeedPump1Rpmx10 = &(RxCan3.SRxCan3.SpeedPump1Rpmx10);
uint16_t *SpeedPump2Rpmx10 = &(RxCan3.SRxCan3.SpeedPump2Rpmx10);
uint16_t *SpeedPump3Rpmx10 = &(RxCan3.SRxCan3.SpeedPump3Rpmx10);
uint16_t *SpeedPump4Rpmx10 = &(RxCan3.SRxCan3.SpeedPump4Rpmx10);

//Valori di temperatura in arrivo dalla Protective
uint16_t *STA2       = &(RxCan1.SRxCan1.TempArtx10);
uint16_t *STV2       = &(RxCan1.SRxCan1.TempVenx10);
uint16_t *STF2       = &(RxCan1.SRxCan1.TempFluidx10);

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

//	TxCan1.SAux.NoSendCounter = 0;
//	TxCan2.SAux.NoSendCounter = 0;
//	TxCan3.SAux.NoSendCounter = 0;
//	TxCan4.SAux.NoSendCounter = 0;
//	TxCan5.SAux.NoSendCounter = 0;
//	TxCan6.SAux.NoSendCounter = 0;
//	TxCan7.SAux.NoSendCounter = 0;

	//AddSwTimer(ManageTxCan10ms,1,TM_REPEAT);
	AddSwTimer(ManageTxCan50ms,5,TM_REPEAT);
	AddSwTimer(ManageTxDebug,5,TM_REPEAT);
	AddSwTimer(ManageGetParams,5,TM_REPEAT);

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
		           uint8_t Pinch2WPVF, uint8_t Pinch2WPVA, uint8_t Pinch2WPVV)
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


void onNewSensTempVal(uint16_t PressOxyg, float TempRes,
		               float TempArt, float TempVen)
{
	TxCan2.STxCan2.PressOxy = PressOxyg;
	TxCan2.STxCan2.TempFluidx10 = (int16_t) (TempRes * 10);
	TxCan2.STxCan2.TempArtx10   = (int16_t) (TempArt * 10);
	TxCan2.STxCan2.TempVenx10   = (int16_t) (TempVen * 10);

    // SB added plate temperature

    TxCan5.STxCan5.tempPlateC = (int16_t)(T_PLATE_C_GRADI_CENT*10);

}

/*VP 9-1-2019: aggiunta informazione su tipo di trattamento
 * se Kidney o Liver nel byte 5 del message buffer STxCan5 */
void onNewTherapyType()
{
    TxCan5.STxCan5.TerapyType = (THERAPY_TYPE) GetTherapyType();
    TxCan5.STxCan5.ArtLineAirMeasure = sensor_UFLOW[ARTERIOUS_AIR_SENSOR].bubbleSize;
	TxCan5.STxCan5.VenLineAirMeasure = sensor_UFLOW[VENOUS_AIR_SENSOR].bubbleSize;
}
void onNewPinchVal(uint8_t AirFiltStat, uint16_t AlarmCode,
		           uint8_t Pinch2WPVF, uint8_t Pinch2WPVA, uint8_t Pinch2WPVV)
{
	TxCan3.STxCan3.AlarmCode = AlarmCode;
	TxCan3.STxCan3.AirAlarm = AirFiltStat;
	TxCan3.STxCan3.FilterPinchPos = Pinch2WPVF;
	TxCan3.STxCan3.ArtPinchPos = Pinch2WPVA;
	TxCan3.STxCan3.OxygPinchPos = Pinch2WPVV;
	/* Aggiungo l'informnazione della posizione della pinch
	 * letta dal driver tramite la matrice globale modbusData;
	 * la posizione settata (Pinch2WPVF -- Pinch2WPVA -- Pinch2WPVV)
	 * si sposta nel nibble più alto del byte e nel nibble più
	 * basso metto la posizione inviatami dal driver*/
	TxCan3.STxCan3.FilterPinchPos  = TxCan3.STxCan3.FilterPinchPos << 4 ;
	TxCan3.STxCan3.FilterPinchPos |= (modbusData[4][0] & 0x0F); // in modbusData[4][0] ho la posizione della pinch filtro inviatami dal driver

	TxCan3.STxCan3.ArtPinchPos  = TxCan3.STxCan3.ArtPinchPos << 4 ;
	TxCan3.STxCan3.ArtPinchPos |= (modbusData[5][0] & 0x0F); // in modbusData[5][0] ho la posizione della pinch arteriosa inviatami dal driver

	TxCan3.STxCan3.OxygPinchPos  = TxCan3.STxCan3.OxygPinchPos << 4 ;
	TxCan3.STxCan3.OxygPinchPos |= (modbusData[6][0] & 0x0F); // in modbusData[6][0] ho la posizione della pinch venosa/oxy inviatami dal driver

}

void onNewOffsetPressInlet(uint8_t Offset_Pr_Ven, uint8_t Offset_Pr_Art)
{
	/*aggiungo le informazioni sull'offset di pressione venosa
	 * e arteriosa calcolati all'inizio del trattamento e che andranno sottratti*/
	TxCan3.STxCan3.Offset_Press_Ven = Offset_Pr_Ven;
	TxCan3.STxCan3.Offset_Press_Art = Offset_Pr_Art;
}
void onNewPumpSpeed(uint16_t Pump0Speed, uint16_t Pump1Speed ,
		            uint16_t Pump2Speed, uint16_t Pump3Speed)
{
	TxCan4.STxCan4.SpeedPump1Rpmx10 = Pump0Speed;
	TxCan4.STxCan4.SpeedPump2Rpmx10 = Pump1Speed;
	TxCan4.STxCan4.SpeedPump3Rpmx10 = Pump2Speed;
	TxCan4.STxCan4.SpeedPump4Rpmx10 = Pump3Speed;
}

void onNewCommadT1TEST(TControl2ProtCommands Command)
{
	TxCan5.STxCan5.Command = Command;
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
//void ManageTxCan10ms(void)
//{
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
//}


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
void onNewCanBusMsg14(CANBUS_MSG_14 ReceivedCanBusMsg14);

uint16_t GetAlarmCodeProt(void)
{
	return (RxBuffCanP[2]->SRxCan2.AlarmCode);
}

uint16_t GetRevisionFWProt()
{
	return (RxBuffCanP[5]->SRxCan5.FW_Revision_Protective);
}

void ReceivedCanData(uint8_t *rxbuff, int rxlen, int RxChannel)
{
	CANBUS_MSG_9 TempCanBusMsg9;
	CANBUS_MSG_10 TempCanBusMsg10;
	CANBUS_MSG_11 TempCanBusMsg11;
	CANBUS_MSG_12 TempCanBusMsg12;
	CANBUS_MSG_14 TempCanBusMsg14;

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
				TempCanBusMsg14.FW_Revision_Protective = RxBuffCanP[RxChannel-8]->SRxCan5.FW_Revision_Protective;
				TempCanBusMsg14.tempPlateP = RxBuffCanP[RxChannel-8]->SRxCan5.tempPlateP;
				TempCanBusMsg14.free3 = RxBuffCanP[RxChannel-8]->SRxCan5.Free5;
				TempCanBusMsg14.free4 = RxBuffCanP[RxChannel-8]->SRxCan5.Free6;
				TempCanBusMsg14.free5 = RxBuffCanP[RxChannel-8]->SRxCan5.Free7;
				TempCanBusMsg14.free6 = RxBuffCanP[RxChannel-8]->SRxCan5.Free8;
				onNewCanBusMsg14(TempCanBusMsg14);
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
				memset(&alarmCurrent, 0, sizeof(typeAlarmS));
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
	char StrPrompt[40];

	char * pointer_StrPrompt = &StrPrompt[0];

	strcpy(pointer_StrPrompt,GREEN_TEXT) ;
	strcat(pointer_StrPrompt,(const char *)"\r\nPerlife>") ;
	strcat(pointer_StrPrompt, WHITE_TEXT);

	if(FirstTime){
		PC_DEBUG_COMM_SendBlock((PC_DEBUG_COMM_TComData*)pointer_StrPrompt, strlen(pointer_StrPrompt) , &sent_data);
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
 	 			PC_DEBUG_COMM_SendBlock((PC_DEBUG_COMM_TComData*)StrPrompt, strlen(StrPrompt) , &sent_data);
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
			sprintf(stringPtr, "\"sep=,\"\r\n Press Ven [mmHg] , Pr Art [mmHg] , Pr Filt [mmHg], Pr Oxy [mmHg], Press Lev [Count]");
			PC_DEBUG_COMM_SendBlock(stringPtr, strlen(stringPtr), &sent_data);
			break;
		case 'T':
			LogMode = 4;
			sprintf(stringPtr, "\"sep=,\"\r\n TempArt [C] , TempReservoire [C], TempVen [C] , TempPlate [C] , Frigo ON , Heater ON\r\n");
			PC_DEBUG_COMM_SendBlock(stringPtr, strlen(stringPtr), &sent_data);
			break;
		case 'C':
			LogMode = 7;
			sprintf(stringPtr, "\"sep=,\"\r\n Pinch FILT , Pinch ART, Pinch VEN \r\n");
			PC_DEBUG_COMM_SendBlock(stringPtr, strlen(stringPtr), &sent_data);
			break;
		case 'M':
			LogMode = 10;
			sprintf(stringPtr, "\"sep=,\"\r\n Pump FILT LIVER / ART KIDNEY [rpm], Pump ART LIVER[rpm], Pump OXY 1[rpm], Pump OXY 2[rpm] \r\n");
			PC_DEBUG_COMM_SendBlock(stringPtr, strlen(stringPtr), &sent_data);
			break;
		case 'E': // log errors
			LogMode = 13;
			sprintf(stringPtr, "\"sep=,\"\r\n Error Ctrl , Error Prot \r\n");
			PC_DEBUG_COMM_SendBlock(stringPtr, strlen(stringPtr), &sent_data);
			break;
		case '1': // log pumps cover
			LogMode = 16;
			sprintf(stringPtr, "\"sep=,\"\r\n Cover filt , Cover art , Cover oxy1 , Cover oxy2 \r\n");
			PC_DEBUG_COMM_SendBlock(stringPtr, strlen(stringPtr), &sent_data);
			break;
		case '2': // log air sensors
			LogMode = 19;
			sprintf(stringPtr, "\"sep=,\"\r\n Air filt , Air art , Air Ven \r\n");
			PC_DEBUG_COMM_SendBlock(stringPtr, strlen(stringPtr), &sent_data);
			break;
		case '3': // log can bus status
			LogMode = 22;
			sprintf(stringPtr, "\"sep=,\"\r\n Can Bus Status \r\n");
			PC_DEBUG_COMM_SendBlock(stringPtr, strlen(stringPtr), &sent_data);
			break;
		case '4': // log Power Voltage
			LogMode = 25;
			sprintf(stringPtr, "\"sep=,\"\r\n Power: 5V_Board,  5V_Analogic, 24V_Motor, 24V_Board, 48V_Ever_Motor \r\n");
			PC_DEBUG_COMM_SendBlock(stringPtr, strlen(stringPtr), &sent_data);
			break;
		case '5': // log Door sensors
			LogMode = 28;
			sprintf(stringPtr, "\"sep=,\"\r\n Door SX , Doors DX \r\n");
			PC_DEBUG_COMM_SendBlock(stringPtr, strlen(stringPtr), &sent_data);
			break;
		case '6': // log Hook Reservoire sensors
			LogMode = 31;
			sprintf(stringPtr, "\"sep=,\"\r\n Reservorie Hook SX , Reservore Hook DX \r\n");
			PC_DEBUG_COMM_SendBlock(stringPtr, strlen(stringPtr), &sent_data);
			break;
		case '7': // log FW Version
			LogMode = 34;
			sprintf(stringPtr, "\"sep=,\"\r\n FW VERSION: Control, Protective, PMP FLT, PMP ART, Pinch FLT, Pinch ART, Pinch VEN \r\n");
			PC_DEBUG_COMM_SendBlock(stringPtr, strlen(stringPtr), &sent_data);
			break;
		default:
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
	case 16: // log pump cover
		LogMode = 17;
		break;
	case 19: // log Air Sensors
		LogMode = 20;
		break;
	case 22: // log CanBus
		LogMode = 23;
		break;
	case 25: // log Voltage power
		LogMode = 26;
		break;
	case 28: // log doors
		LogMode = 29;
		break;
	case 31: // log reservoire hooks
		LogMode = 32;
		break;
	case 34: // log FW Version
		LogMode = 35;
		break;
	/*-----------------*/
	case 2: // pressure
		if( PrescalerCnt == 0){
			TxDebugPressures();
		}
		PrescalerCnt = (PrescalerCnt + 1) % 10;
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
	case 17: // log pump cover status
		if( PrescalerCnt == 0){
			TxPumpCovers();
		}
		PrescalerCnt = (PrescalerCnt + 1) % 20;
		break;
	case 20: // log Air Sensors
		if( PrescalerCnt == 0){
			TxAirSensors();
		}
		PrescalerCnt = (PrescalerCnt + 1) % 20;
		break;
	case 23: // log Can Bus
		if( PrescalerCnt == 0){
			TxCanBus();
		}
		PrescalerCnt = (PrescalerCnt + 1) % 20;
		break;
	case 26: // log Voltage
		if( PrescalerCnt == 0){
			TxPowerVoltage();
		}
		PrescalerCnt = (PrescalerCnt + 1) % 20;
		break;
	case 29: // log doors
		if( PrescalerCnt == 0){
			TxDoors();
		}
		PrescalerCnt = (PrescalerCnt + 1) % 20;
		break;
	case 32: // log Reservorire Hooks
		if( PrescalerCnt == 0){
			TxReservorireHooks();
		}
		PrescalerCnt = (PrescalerCnt + 1) % 20;
		break;
	case 35: // log Reservorire Hooks
		if( PrescalerCnt == 0){
			TxFWVersion();
		}
		PrescalerCnt = (PrescalerCnt + 1) % 300;
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
	word sent_data;

    sprintf(stringPtr, " %03u ,  %03u , %03u , %03u , %05u \r\n", PR_VEN_mmHg_Filtered, PR_ART_mmHg_Filtered, PR_ADS_FLT_mmHg_Filtered, PR_OXYG_mmHg_Filtered,PR_LEVEL_ADC_Filtered);
    PC_DEBUG_COMM_SendBlock(stringPtr, strlen(stringPtr) , &sent_data);
}

void TxDebugTemperatures(void)
{
    static char stringPtr[200];
    float TempArt          = sensorIR_TM[0].tempSensValue;
    float TempReservoire   = sensorIR_TM[1].tempSensValue;
    float TempVen          = sensorIR_TM[2].tempSensValue;
    float TempPlate        = T_PLATE_C_GRADI_CENT;

	word sent_data;

	short int FrigoOn = GetFrigoOn();
	short int HeaterOn = GetHeaterOn();

	/*per motivi di visualizzazione su excel metto 100 se acceso e 0 se spento*/
	if (FrigoOn)
		FrigoOn = 100;
	else
		FrigoOn = 0;

	if (HeaterOn)
		HeaterOn = 100;
	else
		HeaterOn = 0;

    sprintf(stringPtr, "% 4.1f , % 4.1f  ,  % 4.1f , % 4.1f  ,  %03u , %03u \r\n", TempArt, TempReservoire,TempVen,TempPlate,FrigoOn,HeaterOn);

    PC_DEBUG_COMM_SendBlock(stringPtr, strlen(stringPtr) , &sent_data);

}

void TxDebugPinch(void)
{
    static char stringPtr[200];
    word Pinch_Filt = modbusData[4][0];
	word Pinch_Art  = modbusData[5][0];
	word Pinch_Ven  = modbusData[6][0];

	word sent_data;

    sprintf(stringPtr, " %02x , %02x , %02x \r\n", Pinch_Filt, Pinch_Art , Pinch_Ven);

    PC_DEBUG_COMM_SendBlock(stringPtr, strlen(stringPtr) , &sent_data);
}


void TxDebugPumpSpeed(void)
{
    static char stringPtr[200];

    float speed1 = modbusData[0][17];
    	  speed1 /=  100;

    float speed2 = modbusData[1][17];
          speed2 /= 100;

    float speed3 = modbusData[2][17];
    	  speed3 /= 100;

    float speed4 = modbusData[3][17];
		  speed4 /= 100;

	word sent_data;

	sprintf(stringPtr, "% 5.1f , % 5.1f , % 5.1f , % 5.1f \r\n", speed1, speed2 , speed3, speed4);

    PC_DEBUG_COMM_SendBlock(stringPtr, strlen(stringPtr) , &sent_data);
}

void TxDebugErrors(void)
{
    static char stringPtr[200];
    word sent_data;
    word ControlError    = alarmCurrent.code;
    word ProtectiveError = GetAlarmCodeProt();

    sprintf(stringPtr, " %05u , %05u \r\n", ControlError, ProtectiveError);
    PC_DEBUG_COMM_SendBlock(stringPtr, strlen(stringPtr) , &sent_data);

}


void TxPumpCovers(void)
{
    static char stringPtr[200];
    word sent_data;
    unsigned char * pumps_cover;
    pumps_cover = GetCoverState();
	char spump_filt[7];
	char spump_art[7];
	char spump_oxy1[7];
	char spump_oxy2[7];

	if( pumps_cover[0] == 0)
		strcpy(spump_filt, "closed");
	else if (pumps_cover[0] == 1)
		strcpy(spump_filt, "opened");
	else
		strcpy(spump_filt, "undef");

	if( pumps_cover[1] == 0)
		strcpy(spump_art, "closed");
	else if (pumps_cover[1] == 1)
		strcpy(spump_art, "opened");
	else
		strcpy(spump_art, "undef");

	if( pumps_cover[2] == 0)
		strcpy(spump_oxy1, "closed");
	else if (pumps_cover[2] == 1)
		strcpy(spump_oxy1, "opened");
	else
		strcpy(spump_oxy1, "undef");

	if( pumps_cover[3] == 0)
		strcpy(spump_oxy2, "closed");
	else if (pumps_cover[3] == 1)
		strcpy(spump_oxy2, "opened");
	else
		strcpy(spump_oxy2, "undef");

    sprintf(stringPtr, " %s , %s , %s , %s \r\n", spump_filt, spump_art , spump_oxy1, spump_oxy2);

    PC_DEBUG_COMM_SendBlock(stringPtr, strlen(stringPtr) , &sent_data);
}

void TxAirSensors(void)
{
	static char stringPtr[200];
	word sent_data;

	char air_filt[7];
	char air_art[7];
	char air_ven[7];

	if( Air_1_Status == AIR)
		strcpy(air_filt, "AIR");
	else if (Air_1_Status == LIQUID)
		strcpy(air_filt, "LIQUID");
	else
		strcpy(air_filt, "undef");

	if( Air_Arterious == AIR)
		strcpy(air_art, "AIR");
	else if (Air_Arterious == LIQUID)
		strcpy(air_art, "LIQUID");
	else
		strcpy(air_art, "undef");

	if( Air_Venous == AIR)
		strcpy(air_ven, "AIR");
	else if (Air_Venous == LIQUID)
		strcpy(air_ven, "LIQUID");
	else
		strcpy(air_ven, "undef");

    sprintf(stringPtr, " %s , %s , %s \r\n", air_filt, air_art , air_ven);

    PC_DEBUG_COMM_SendBlock(stringPtr, strlen(stringPtr) , &sent_data);

}

void TxCanBus(void)
{
	static char stringPtr[200];
	word sent_data;

	char communication_status[19];

	if (IsCanBusError())
		 strcpy(communication_status, "COMMUNICATION FAIL");
	else
		 strcpy(communication_status, "COMMUNICATION OK");

	sprintf(stringPtr, " %s \r\n", communication_status);

	PC_DEBUG_COMM_SendBlock(stringPtr, strlen(stringPtr) , &sent_data);
}

void TxPowerVoltage(void)
{
	static char stringPtr[200];
	word sent_data;

	unsigned char V24Motor;
	unsigned char V24Board;

	if (VOLTAGE_M_CHK_GetVal())
		V24Motor = 24;
	else
		V24Motor = 0;

	if (VOLTAGE_B_CHK_GetVal())
		V24Board = 24;
	else
		V24Board = 0;

	sprintf(stringPtr, "% 4.2f , % 4.2f , % u , % u, % 4.2f \r\n", V5_B_Value, V5_An_Value , V24Motor, V24Board, V24_Ever_Motor_Value);

    PC_DEBUG_COMM_SendBlock(stringPtr, strlen(stringPtr) , &sent_data);

}

void TxDoors(void)
{
	static char stringPtr[200];
	word sent_data;
	char DoorSX[7];
	char DoorDX[7];

	if (FRONTAL_COVER_1_STATUS == FALSE)
		strcpy(DoorSX, "closed");
	else
		strcpy(DoorSX, "opened");

	if (FRONTAL_COVER_2_STATUS == FALSE)
		strcpy(DoorDX, "closed");
	else
		strcpy(DoorDX, "opened");

    sprintf(stringPtr, " %s , %s \r\n", DoorSX, DoorDX);

    PC_DEBUG_COMM_SendBlock(stringPtr, strlen(stringPtr) , &sent_data);

}

void TxReservorireHooks(void)
{
	static char stringPtr[200];
	word sent_data;
	char HookSX[7];
	char HookDX[7];

	if (HOOK_SENSOR_2_STATUS == FALSE)
		strcpy(HookSX, "closed");
	else
		strcpy(HookSX, "opened");

	if (HOOK_SENSOR_1_STATUS == FALSE)
		strcpy(HookDX, "closed");
	else
		strcpy(HookDX, "opened");

    sprintf(stringPtr, " %s , %s \r\n", HookSX, HookDX);

    PC_DEBUG_COMM_SendBlock(stringPtr, strlen(stringPtr) , &sent_data);
}

void TxFWVersion(void)
{
	static char stringPtr[200];
	word sent_data;

	int FirstRevNumCon;
	int SecondRevNumCon;
	int ThirdRevNumCon;

	int FirstRevNumPro;
	int SecondRevNumPro;
	int ThirdRevNumPro;

	int FirstRevNumPmpFlt;
	int SecondRevNumPmpFlt;
	int ThirdRevNumPmpFlt;

	int FirstRevNumPmpArt;
	int SecondRevNumPmpArt;
	int ThirdRevNumPmpArt;

	int FirstRevNumPinchFlt;
	int SecondRevNumPinchFlt;
	int ThirdRevNumPinchFlt;

	int FirstRevNumPinchArt;
	int SecondRevNumPinchArt;
	int ThirdRevNumPinchArt;

	int FirstRevNumPinchVen;
	int SecondRevNumPinchVen;
	int ThirdRevNumPinchVen;

	FirstRevNumCon  = REVISION_FW_CONTROL & 0xF800;
	FirstRevNumCon  = FirstRevNumCon >> 11;
	SecondRevNumCon = REVISION_FW_CONTROL & 0x07C0;
	SecondRevNumCon = SecondRevNumCon >> 6;
	ThirdRevNumCon  = REVISION_FW_CONTROL & 0x003F;

	FirstRevNumPro  = GetRevisionFWProt() & 0xF800;
	FirstRevNumPro  = FirstRevNumCon >> 11;
	SecondRevNumPro = GetRevisionFWProt() & 0x07C0;
	SecondRevNumPro = SecondRevNumCon >> 6;
	ThirdRevNumPro  = GetRevisionFWProt() & 0x003F;

	FirstRevNumPmpFlt  = modbusData[0][31] & 0xF000;
	FirstRevNumPmpFlt = FirstRevNumPmpFlt >> 12;
	SecondRevNumPmpFlt = modbusData[0][31] & 0x0F00;
	SecondRevNumPmpFlt = SecondRevNumPmpFlt >> 8;
	ThirdRevNumPmpFlt  = modbusData[0][31] & 0x00FF;

	FirstRevNumPmpArt  = modbusData[1][31] & 0xF000;
	FirstRevNumPmpArt = FirstRevNumPmpArt >> 12;
	SecondRevNumPmpArt = modbusData[1][31] & 0x0F00;
	SecondRevNumPmpArt = SecondRevNumPmpArt >> 8;
	ThirdRevNumPmpArt  = modbusData[1][31] & 0x00FF;

	FirstRevNumPinchFlt  = modbusData[4][31] & 0xF000;
	FirstRevNumPinchFlt = FirstRevNumPinchFlt >> 12;
	SecondRevNumPinchFlt = modbusData[4][31] & 0x0F00;
	SecondRevNumPinchFlt = SecondRevNumPinchFlt >> 8;
	ThirdRevNumPinchFlt  = modbusData[4][31] & 0x00FF;

	FirstRevNumPinchArt  = modbusData[5][31] & 0xF000;
	FirstRevNumPinchArt = FirstRevNumPinchArt >> 12;
	SecondRevNumPinchArt = modbusData[5][31] & 0x0F00;
	SecondRevNumPinchArt = SecondRevNumPinchArt >> 8;
	ThirdRevNumPinchArt  = modbusData[5][31] & 0x00FF;

	FirstRevNumPinchVen  = modbusData[6][31] & 0xF000;
	FirstRevNumPinchVen= FirstRevNumPinchVen >> 12;
	SecondRevNumPinchVen = modbusData[6][31] & 0x0F00;
	SecondRevNumPinchVen= SecondRevNumPinchVen >> 8;
	ThirdRevNumPinchVen  = modbusData[6][31] & 0x00FF;

    sprintf(stringPtr, " %d.%d.%d , %d.%d.%d , %d.%d.%d , %d.%d.%d ,%d.%d.%d , %d.%d.%d ,%d.%d.%d \r\n",
    		           FirstRevNumCon, SecondRevNumCon, ThirdRevNumCon,
					   FirstRevNumPro, SecondRevNumPro, ThirdRevNumPro,
					   FirstRevNumPmpFlt, SecondRevNumPmpFlt,ThirdRevNumPmpFlt,
					   FirstRevNumPmpArt, SecondRevNumPmpArt, ThirdRevNumPmpArt,
					   FirstRevNumPinchFlt, SecondRevNumPinchFlt, ThirdRevNumPinchFlt,
					   FirstRevNumPinchArt, SecondRevNumPinchArt, ThirdRevNumPinchArt,
					   FirstRevNumPinchVen, SecondRevNumPinchVen, ThirdRevNumPinchVen);

    PC_DEBUG_COMM_SendBlock(stringPtr, strlen(stringPtr) , &sent_data);
}

uint8_t CharReceived(void)
{
	uint8_t rxchr;

	if ( PC_DEBUG_COMM_RecvChar(&rxchr) == ERR_OK )
		return rxchr;
	else
		return 0xFF;

}
