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


void InitTest(void)
{
	AddSwTimer(ManageTestP500ms,50,TM_REPEAT);
	AddSwTimer(ManageTestP50ms,5,TM_REPEAT);
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


LDD_TError  SendCAN(uint8_t *txbuff, int txsize, LDD_CAN_TMBIndex ChIndex);
void ManageTestP50ms(void)
{
	// send a packet each 100ms if protective
	//SendCAN("PERRONE", 7, 1);

	// sono passati 50 msec devo controllare se e' ora di inviare qualche cosa su can bus
	SendCanMessageFlag = TRUE;
}

void ReceivedCanData(uint8_t *rxbuff, int rxlen, int RxChannel)
{
	if((RxChannel == 0) && (memcmp(rxbuff,"VINCENZO",8) == 0)){
		// expected channel n message
		RetriggerAlarm();
	}
}

//Message buffer 1 CON > PRO (STATE MACHINE)
// Byte 1	Byte 2	Byte 3	Byte 4	Byte 5	Byte 6	Byte 7	Byte 8
//State - H	State - L	Parent -H	Parent -L	Child - H	Child - L	Guard - H	Guard - L
void FillMessage1(void)
{
	int i;
	//memset(TxCanMsg, 0, sizeof(TxCanMsg));
	*(word*)TxCanMsg = ptrCurrentState->state;
	*(word*)(TxCanMsg + 2) = ptrCurrentParent->parent;
	*(word*)(TxCanMsg + 4) = ptrCurrentChild->child;

	for(i = 0; i <= GUARD_END_NUMBER; i++)
	{
		if(currentGuard[i].guardValue == GUARD_VALUE_TRUE)
			break;
	}

	if(i < GUARD_END_NUMBER)
		*(word*)(TxCanMsg + 6) = (word)i;
	else
		*(word*)(TxCanMsg + 6) = 0;
}


void ProtectiveTask(void)
{
	static unsigned short ProtTaskCnt = 0;

	if(SendCanMessageFlag == TRUE)
	{
		ProtTaskCnt++;
		FillMessage1();
		if( memcmp(TxCanMsg, OldTxCanMsg1, sizeof(OldTxCanMsg1)))
		{
			SendCAN(TxCanMsg, 8, 0);
			memcpy(OldTxCanMsg1, TxCanMsg, sizeof(OldTxCanMsg1));
		}

		if(ProtTaskCnt % 2)
		{
			// invio gli altri messaggi
		}

	}
}



