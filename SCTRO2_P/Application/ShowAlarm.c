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
#include <stdbool.h>
#include "string.h"
#include "BUZZER_MEDIUM_P.h"
#include "BUZZER_HIGH_P.h"
#include "BUZZER_LOW_P.h"
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
void Buzzer_Management_50ms(void);

void InitTest(void)
{
	AddSwTimer(ManageShowAlarm500ms,50,TM_REPEAT);
	AddSwTimer(Buzzer_Management_50ms,5,TM_REPEAT);
}

int ErrorCounter = 0;
int SevenSegTimer = 0;
int AlarmCounter = 0;
int ActualErrNum = 0;


BUZZER_LEVEL BuzzLevel = SILENT;
bool CommandProcessorActive = false;
void ManageShowAlarm500ms(void)
{
	if(SevenSegTimer == 0){
		ShowErrorSevenSeg(ActualErrNum); // send each 4 seconds
	}
	SevenSegTimer = (SevenSegTimer + 1) % 8;

	if( !CommandProcessorActive){
		if(ActualErrNum != 0){
	#ifndef PROTECTIVE_SLEEPS
			BuzzLevel = HIGH;
	#endif
		}
		else{
			BuzzLevel = SILENT;
		}
	}
}

void SetBuzzerMode(int val)
{
	switch(val){
		case 0: BuzzLevel = SILENT; CommandProcessorActive = false; break;
		case 1: BuzzLevel = LOW; CommandProcessorActive = true; break;
		case 2: BuzzLevel = MEDIUM; CommandProcessorActive = true;  break;
		case 3: BuzzLevel = HIGH; CommandProcessorActive = true; break;
	}
}


int BuzzCnt = 0;
void Buzzer_Management_50ms(void)
{
	extern unsigned long int gCounterTimerBuzzer;
	switch (BuzzLevel)
	{
		case SILENT:
			BUZZER_LOW_P_ClrVal(); 		//disattiva il buzzer low
			BUZZER_MEDIUM_P_ClrVal();	//disattiva il buzzer Medium
			BUZZER_HIGH_P_ClrVal();		//disattiva il buzzer HIGH
			BuzzCnt = 0;
		break;
		case LOW:
			BUZZER_MEDIUM_P_ClrVal();	//disattiva il buzzer Medium
			BUZZER_HIGH_P_ClrVal();		//disattiva il buzzer HIGH
			BUZZER_LOW_P_SetVal(); 		//attiva il buzzer low
			BuzzCnt = 0;
		break;
		case MEDIUM:
			BUZZER_LOW_P_ClrVal(); 		//disattiva il buzzer low
			BUZZER_HIGH_P_ClrVal();		//disattiva il buzzer HIGH
			BUZZER_MEDIUM_P_SetVal();	//attiva il buzzer Medium
			BuzzCnt = 0;
		break;
		case HIGH:
		   //				   +-----------------....
		   //   HIGH_P         |  HIGH should rise
		   //  		     ------+  after 300ms
		   //	            +-----------------....
		   //	 LOW_P      |
		   //             --+  LOW should rise after 100ms
		   if( BuzzCnt == 0) {
				BUZZER_LOW_P_ClrVal(); 		//disattiva il buzzer low
				BUZZER_MEDIUM_P_ClrVal();	//disattiva il buzzer Medium
				BUZZER_HIGH_P_ClrVal();		//disattiva il buzzer HIGH
		   }
		   if( BuzzCnt == 2) {
				BUZZER_LOW_P_SetVal(); 		//activate LOW
		   }
		   if (BuzzCnt == 5)
		   {
				BUZZER_HIGH_P_SetVal();		//activate HIW , LOW already active
		   }

		   if(BuzzCnt < 6)
				 BuzzCnt++;
		break;
		default:
		break;
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




