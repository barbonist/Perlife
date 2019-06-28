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
#include "ControlProtectiveInterface.h"

//#define CONTROL_BOARD
//#undef PROTECTIVE_BOARD
//#undef CONTROL_BOARD
//#define PROTECTIVE_BOARD

void ManageShowAlarm500ms(void);


void InitTest(void)
{
	AddSwTimer(ManageShowAlarm500ms,50,TM_REPEAT);
}

int ErrorCounter = 0;
int ErrorTimer = 0;
int AlarmCounter = 0;
int ActualErrNum = 0;

bool BuzzCnt = 0;

void ManageShowAlarm500ms(void)
{
	if(ErrorTimer == 0){
		ShowErrorSevenSeg(ActualErrNum); // send each 4 seconds
	}
	ErrorTimer = (ErrorTimer + 1) % 8;
	if(ActualErrNum != 0){
		BuzzCnt = (BuzzCnt + 1) % 6;
#ifndef PROTECTIVE_SLEEPS
		if(BuzzCnt == 0) BUZZER_HIGH_P_SetVal();
		if(BuzzCnt == 2) BUZZER_HIGH_P_ClrVal();
#endif
	}
	else{
		BUZZER_HIGH_P_ClrVal();
	}
}


#ifdef PROTECTIVE_SLEEPS
	#define NO_SEND_ERROR
#endif

void ShowNewAlarmError(uint16_t AlarmCode)
{
	ActualErrNum = AlarmCode;

#ifndef NO_SEND_ERROR
	TxAlarmCode( AlarmCode);
#endif
	LogProtectiveBoardError(AlarmCode);
}




