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



void ManageTestP500ms(void);
void ManageTestP100ms(void);

void InitTest(void)
{
	AddSwTimer(ManageTestP500ms,50,TM_REPEAT);
	AddSwTimer(ManageTestP100ms,10,TM_REPEAT);
}


int ErrorCounter = 0;
int ErrorTimer = 0;
int AlarmCounter = 0;
int ActualErrNum = 0;

bool BuzzStat = FALSE;
bool BuzzCnt = 0;


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
void ManageTestP100ms(void)
{

// send a packet each 100ms if protective
	SendCAN("PERRONE", 7, 1);

}

void ReceivedCanData(uint8_t *rxbuff, int rxlen, int RxChannel)
{
	if((RxChannel == 0) && (memcmp(rxbuff,"VINCENZO",8) == 0)){
		// expected channel n message
		RetriggerAlarm();
	}
}

void Application(void)
{


}



