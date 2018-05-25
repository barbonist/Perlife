/*
 * TestProtectiveFALSEFALSETRUE.c
 *
 *  Created on: 23/mar/2FALSETRUE8
 *      Author: W5
 */
//
//
//	filename:   AuxAppControl.c
//	date:		23/3/2018
//	author:		SB
//
//
#include "string.h"
#include "BUZZER_MEDIUM_C.h"
#include "BUZZER_HIGH_C.h"
#include "SWtimer.h"
#include "SevenSeg.h"
#include "Global.h"
#include "ControlProtectiveInterface_C.h"
#include "ModBusCommProt.h"

void ManageTestP500ms(void);
void ManageTestP50ms(void);


int ErrorCounter = 0;
int ErrorTimer = 0;
int AlarmCounter = 0;
int ActualErrNum = 0;

bool BuzzStat = FALSE;
bool BuzzCnt = 0;
unsigned char TxCanMsg[8];
unsigned char OldTxCanMsg1[8];
CANBUS_MSG_9 CanBusMsg9;
CANBUS_MSG_10 CanBusMsg10;
CANBUS_MSG_11 CanBusMsg11;
CANBUS_MSG_12 CanBusMsg12;


void InitTest(void)
{
	AddSwTimer(ManageTestP500ms,50,TM_REPEAT);
	memset(TxCanMsg, 0, sizeof(TxCanMsg));
	memset(OldTxCanMsg1, 0, sizeof(OldTxCanMsg1));
}


bool IsCanBusError(void)
{
	return (ActualErrNum != 0);
}


void ManageTestP500ms(void)
{
	if(ErrorTimer == 0){
		ShowErrorSevenSeg(ActualErrNum); // send each 4 seconds
	}
	ErrorTimer = (ErrorTimer + 1) % 8;

	if( AlarmCounter < 4 ){
		AlarmCounter ++;
		if(AlarmCounter == 4){
			// alarm
			ActualErrNum = 5;
		}
	}
	if(ActualErrNum != 0){
		BuzzCnt = (BuzzCnt + 1) % 6;
		if(BuzzCnt == 0) BUZZER_MEDIUM_C_SetVal();
		if(BuzzCnt == 2) BUZZER_MEDIUM_C_ClrVal();
	}
	else{
		BUZZER_MEDIUM_C_ClrVal();
	}
}

void RetriggerAlarm(void){
	AlarmCounter = 0;
	ActualErrNum = 0;
}

void onNewCanBusMsg9(CANBUS_MSG_9 ReceivedCanBusMsg9)
{
	CanBusMsg9 = ReceivedCanBusMsg9;
}

void onNewCanBusMsg10(CANBUS_MSG_10 ReceivedCanBusMsg10)
{
	CanBusMsg10 = ReceivedCanBusMsg10;
}

void onNewCanBusMsg11( CANBUS_MSG_11 ReceivedCanBusMsg11){
	CanBusMsg11 = ReceivedCanBusMsg11;
}

void onNewCanBusMsg12(CANBUS_MSG_12 ReceivedCanBusMsg12)
{
	CanBusMsg12 = ReceivedCanBusMsg12;
}

// posizione delle pinch prima di collegare l'organo
unsigned char PinchPos_ref1[3] = {0xff, MODBUS_PINCH_POS_CLOSED, MODBUS_PINCH_POS_CLOSED};
// posizione delle pinch prima dello start trattamento
unsigned char PinchPos_ref2[3] = {0xff, MODBUS_PINCH_LEFT_OPEN, MODBUS_PINCH_LEFT_OPEN};

bool IsPinchPosOk(unsigned char *pArrPinchPos)
{
	bool PinchPosOk = TRUE;
#ifdef ENABLE_PROTECTIVE_BOARD
	if((pArrPinchPos[0] != 0xff) && (pArrPinchPos[0] != CanBusMsg11.FilterPinchPos))
		PinchPosOk = FALSE;
	if((pArrPinchPos[1] != 0xff) && (pArrPinchPos[1] != CanBusMsg11.ArtPinchPos))
		PinchPosOk = FALSE;
	if((pArrPinchPos[2] != 0xff) && (pArrPinchPos[2] != CanBusMsg11.OxygPinchPos))
		PinchPosOk = FALSE;
#endif

	//-------------------------------------------------------------------------------
	// codice aggiunto per il debug dell'allarme, da cancellare nella versione finale
	// provo a forzare manualmente una pinch posizionata male
//	if(PinchPosOk == TRUE)
//	{
//		if(ptrCurrentState->state == STATE_PRIMING_RICIRCOLO && (ptrCurrentParent->parent == PARENT_PRIM_WAIT_PINCH_CLOSE))
//		{
//			for(int i = 0; i < 3; i++)
//			{
//				if((PinchPos_ref1[i] != 0xff) && (pArrPinchPos[i] != PinchPos_ref1[i]))
//				{
//					PinchPosOk = FALSE;
//					break;
//				}
//			}
//		}
//		else if(ptrCurrentState->state == STATE_PRIMING_RICIRCOLO)
//		{
//			for(int i = 0; i < 3; i++)
//			{
//				if((PinchPos_ref1[i] != 0xff) && (pArrPinchPos[i] != PinchPos_ref2[i]))
//				{
//					PinchPosOk = FALSE;
//					break;
//				}
//			}
//		}
//
//		if(ptrCurrentState->state == STATE_TREATMENT_KIDNEY_1)
//		{
//			for(int i = 0; i < 3; i++)
//			{
//				if((PinchPos_ref2[i] != 0xff) && (pArrPinchPos[i] != PinchPos_ref2[i]))
//				{
//					PinchPosOk = FALSE;
//					break;
//				}
//			}
//		}
//	}
	//-------------------------------------------------------------------------------

	return PinchPosOk;
}

//void ReceivedCanDataFrank(uint8_t *rxbuff, int rxlen, int RxChannel)
//{
//	if((RxChannel == 0) && (memcmp(rxbuff,"VINCENZO",8) == 0)){
//		// expected channel n message
//		RetriggerAlarm();
//	}
//	else if(RxChannel == 11)
//	{
//		memcpy(&CanBusMsg11, rxbuff, 8);
//		// expected channel n message
//		RetriggerAlarm();
//	}
//}

//Message buffer 1 CON > PRO (STATE MACHINE)
// Byte 1	Byte 2	Byte 3	Byte 4	Byte 5	Byte 6	Byte 7	Byte 8
//State - H	State - L	Parent -H	Parent -L	Child - H	Child - L	Guard - H	Guard - L
void NotifyMachineStatus(void)
{
	int i;
	//memset(TxCanMsg, 0, sizeof(TxCanMsg));
//	*(word*)TxCanMsg = ptrCurrentState->state;
//	*(word*)(TxCanMsg + 2) = ptrCurrentParent->parent;
//	*(word*)(TxCanMsg + 4) = ptrCurrentChild->child;

	for(i = 0; i <= GUARD_END_NUMBER; i++)
	{
		if(currentGuard[i].guardValue == GUARD_VALUE_TRUE)
			break;
	}

//	if(i < GUARD_END_NUMBER)
//		*(word*)(TxCanMsg + 6) = (word)i;
//	else
//		*(word*)(TxCanMsg + 6) = 0;

	uint16_t Guard;
	Guard = (i < GUARD_END_NUMBER) ? (word) i : 0;

	onNewState( ptrCurrentState, ptrCurrentParent , ptrCurrentChild, Guard   );

}

void NotifyPumpsSpeed(void)
{
	int i;
	uint16_t Pump0Speed, Pump1Speed, Pump2Speed, Pump3Speed;
	Pump0Speed = 0;
	Pump1Speed = 0;
	Pump2Speed = 0;
	Pump3Speed = 0;
	if(GetTherapyType() == LiverTreat)
	{
		if(CoversState == 0)
			Pump0Speed = 0;
		else
		{
			Pump0Speed = modbusData[pumpPerist[3].pmpMySlaveAddress-2][17];
			//		if(!Pump0Speed)
			//			i = 0;
		}

		if(CoversState == 1)
			Pump1Speed = 0;
		else
		{
			Pump1Speed = modbusData[pumpPerist[0].pmpMySlaveAddress-2][17];
			//		if(!Pump1Speed)
			//			i = 0;
		}

		if(CoversState == 2)
			Pump2Speed = 0;
		else
		{
			Pump2Speed = modbusData[pumpPerist[1].pmpMySlaveAddress-2][17];
			//		if(!Pump2Speed)
			//			i = 0;
		}

		if(CoversState == 3)
			Pump3Speed = 0;
		else
		{
			Pump3Speed = modbusData[pumpPerist[2].pmpMySlaveAddress-2][17];
			//		if(!Pump3Speed)
			//			i = 0;
		}
	}
	else //if(GetTherapyType() == KidneyTreat)
	{
		if(CoversState == 0)
			Pump0Speed = 0;
		else
		{
			Pump0Speed = modbusData[pumpPerist[0].pmpMySlaveAddress-2][17];
			//Pump0Speed = pumpPerist[0].pmpSpeed;
			//		if(!Pump0Speed)
			//			i = 0;
		}

		if(CoversState == 1)
			Pump1Speed = 0;
		else
		{
			Pump1Speed = modbusData[1][17];
			//		if(!Pump1Speed)
			//			i = 0;
		}

		if(CoversState == 2)
			Pump2Speed = 0;
		else
		{
			//Pump2Speed = modbusData[pumpPerist[1].pmpMySlaveAddress-2][17];
			Pump2Speed = pumpPerist[1].pmpSpeed;
			//		if(!Pump2Speed)
			//			i = 0;
		}

		if(CoversState == 3)
			Pump3Speed = 0;
		else
		{
			//Pump3Speed = modbusData[pumpPerist[2].pmpMySlaveAddress-2][17];
			Pump3Speed = pumpPerist[2].pmpSpeed;
			//		if(!Pump3Speed)
			//			i = 0;
		}
	}
	onNewPumpSpeed(Pump0Speed, Pump1Speed, Pump2Speed, Pump3Speed);
}


void NotifyPressSens(void)
{
	if(GetTherapyType() == LiverTreat)
	{
		onNewSensPressVal(PR_ADS_FLT_mmHg_Filtered, PR_ART_Med_mmHg, PR_VEN_Med_mmHg, PR_LEVEL_mmHg_Filtered);
	}
	else
	{
		onNewSensPressVal(PR_ADS_FLT_mmHg_Filtered, PR_ART_Med_mmHg, PR_VEN_Med_mmHg, PR_LEVEL_mmHg_Filtered);
	}
}


void NotifyTempSens(void)
{
	if(GetTherapyType() == LiverTreat)
	{
		onNewSensTempVal(PR_OXYG_mmHg_Filtered,         // pressione ossigenatore
				         sensorIR_TM[1].tempSensValue,  // temperatura recipiente * 10
						 sensorIR_TM[0].tempSensValue,  // temperatura linea arteriosa
						 sensorIR_TM[2].tempSensValue); // temperatura linea venosa
	}
	else
	{
		onNewSensTempVal(PR_OXYG_mmHg_Filtered,         // pressione ossigenatore
				         sensorIR_TM[1].tempSensValue,  // temperatura recipiente * 10
						 sensorIR_TM[1].tempSensValue,  // sensorIR_TM[0].tempSensValue, probabilmente,
						                                // in questo caso, la temperatura della linea arteriosa coincide
						                                // con quella del recipiente
						 sensorIR_TM[2].tempSensValue); // temperatura ossigenatore
	}
}

void NotifyPinchPos(void)
{
	if(GetTherapyType() == LiverTreat)
	{
		onNewPinchVal(Air_1_Status,                              // presenza aria sul filtro
				      alarmCurrent.code,                         // allarme corrente sulla control
					  (uint8_t)pinchActuator[0].pinchPosTarget,  // posizione della pinch sul filtro 2WPVF
					  (uint8_t)pinchActuator[1].pinchPosTarget,  // posizione della pinch su linea arteriosa 2WPVA
					  (uint8_t)pinchActuator[2].pinchPosTarget,  // posizione della pinch su linea venosa 2WPVV
		              0,                                         // free1
					  0);                                        // free2
	}
	else
	{
		onNewPinchVal(Air_1_Status,                              // presenza aria sul filtro
				      alarmCurrent.code,                         // allarme corrente sulla control
					  (uint8_t)pinchActuator[0].pinchPosTarget,  // posizione della pinch sul filtro 2WPVF
					  (uint8_t)pinchActuator[1].pinchPosTarget,  // posizione della pinch su linea arteriosa 2WPVA
					  (uint8_t)pinchActuator[2].pinchPosTarget,  // posizione della pinch su linea venosa 2WPVV
		              0,                                         // free1
					  0);                                        // free2
	}
}

#ifdef ENABLE_PROTECTIVE_ALARM_RESET
void NotifyAlmToResetMsg(void)
{
	if(GetTherapyType() == LiverTreat)
	{
		// codice dell'allarme protective da resettare.
		onNewAlmToResetMsg(ProAlmCodeToreset);
	}
	else
	{
		// codice dell'allarme protective da resettare.
		onNewAlmToResetMsg(ProAlmCodeToreset);
	}
}
#endif

void ProtectiveTask(void)
{
	static unsigned short ProtTaskCnt = 0;

	NotifyMachineStatus();
	NotifyPressSens();
	NotifyTempSens();
	NotifyPinchPos();
	NotifyPumpsSpeed();
#ifdef ENABLE_PROTECTIVE_ALARM_RESET
	NotifyAlmToResetMsg();
#endif

	if(ProtTaskCnt % 2)
	{
		// invio gli altri messaggi
	}
}



