/*
 * TestProtectiveFALSEFALSETRUE.c
 *
 *  Created on: 23/mar/2FALSETRUE8
 *      Author: W5
 */
//
//
//	filename:   TestProtective001.c
//	date:		23/3/2FALSETRUE8
//	author:		SB
//
//
#include "string.h"
//#include "D_7S_DP.h"
//#include "D_7S_A.h"
//#include "D_7S_B.h"
//#include "D_7S_C.h"
//#include "D_7S_D.h"
//#include "D_7S_E.h"
//#include "D_7S_F.h"
//#include "D_7S_G.h"
#include "BUZZER_MEDIUM_P.h"
#include "BUZZER_HIGH_P.h"
#include "SWtimer.h"
#include "SevenSeg.h"

#define CONTROL_BOARD
#undef PROTECTIVE_BOARD
//#undef CONTROL_BOARD
//#define PROTECTIVE_BOARD

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

void ManageTestP500ms(void)
{

	if(ErrorTimer == 0){
		ShowErrorSevenSeg(ActualErrNum); // send each 4 seconds
	}
	ErrorTimer = (ErrorTimer + 1) % 8;

#ifdef CONTROL_BOARD
	if( AlarmCounter < 4 ){
		AlarmCounter ++;
		if(AlarmCounter == 4){
			// alarm
			ActualErrNum = 5;
		}
	}
	if(ActualErrNum != 0){
		BuzzCnt = (BuzzCnt + 1) % 6;
		if(BuzzCnt == 0) BUZZER_MEDIUM_P_SetVal();
		if(BuzzCnt == 2) BUZZER_MEDIUM_P_ClrVal();
	}
	else{
		BUZZER_MEDIUM_P_ClrVal();
	}
#endif

#ifdef PROTECTIVE_BOARD
	if( AlarmCounter < 4 ){
		AlarmCounter ++;
		if(AlarmCounter == 4){
			// alarm
			ActualErrNum = 6;
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
#endif

}

void RetriggerAlarm(void){
	AlarmCounter = 0;
	ActualErrNum = 0;
}


LDD_TError  SendCAN(uint8_t *txbuff, int txsize, LDD_CAN_TMBIndex ChIndex);
void ManageTestP100ms(void)
{
#ifdef PROTECTIVE_BOARD
// send a packet each 100ms if protective
	SendCAN("VINCENZO", 8, 0);
#endif
#ifdef CONTROL_BOARD
// send a packet each 100ms if protective
	SendCAN("PERRONE", 7, 1);
#endif

}

void ReceivedCanData_1(uint8_t *rxbuff, int rxlen, int RxChannel)
{
#ifdef CONTROL_BOARD
	if((RxChannel == 0) && (memcmp(rxbuff,"VINCENZO",8) == 0)){
		// expected channel n message
		RetriggerAlarm();
	}
#endif
#ifdef PROTECTIVE_BOARD
	if((RxChannel == 1) && (memcmp(rxbuff,"PERRONE",7) == 0)){
		// expected channel n message
		RetriggerAlarm();
	}
#endif
}

void Application(void)
{


}



