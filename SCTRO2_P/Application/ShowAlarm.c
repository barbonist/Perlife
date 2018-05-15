/*
 * TestProtectiveFALSEFALSETRUE.c
 *
 *  Created on: 23/mar/2FALSETRUE8
 *      Author: W5
 */
//
//
//	filename:   ShowAlarm.c
//	date:		23/3/2108
//	author:		SB
//
//
#include "string.h"
#include "BUZZER_MEDIUM_P.h"
#include "BUZZER_HIGH_P.h"
#include "SWtimer.h"
#include "SevenSeg.h"
#include "Global.h"
#include "Alarm_Con_protective.h"

//#define CONTROL_BOARD
//#undef PROTECTIVE_BOARD
//#undef CONTROL_BOARD
//#define PROTECTIVE_BOARD

void ManageTestP500ms(void);
void ManageTestP100ms(void);

void InitTest(void)
{
	AddSwTimer(ManageTestP500ms,50,TM_REPEAT);
}

int ErrorCounter = 0;
int ErrorTimer = 0;
int AlarmCounter = 0;
int ActualErrNum = 0;

bool BuzzCnt = 0;

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
			ActualErrNum =  CODE_ALARM_NOCAN_COMMUNICATION;
		}
	}
	if(ActualErrNum != 0){
		BuzzCnt = (BuzzCnt + 1) % 6;
		if(BuzzCnt == 0) BUZZER_HIGH_P_SetVal();
		if(BuzzCnt == 2) BUZZER_HIGH_P_ClrVal();
	}
	else{
		BUZZER_HIGH_P_ClrVal();
	}
}


ShowNewAlarmError(uint16_t AlarmCode)
{
	ActualErrNum = AlarmCode;
}



void RetriggerNoCANRxTxAlarm(void){
	AlarmCounter = 0;
	//ActualErrNum = 0;
}

