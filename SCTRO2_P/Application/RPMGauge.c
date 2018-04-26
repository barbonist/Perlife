/*
 * RPMGauge.c
 *
 *  Created on: 24/apr/2018
 *      Author: W5
 */

#include <stdint.h>
#include "SwTimer.h"
#include "PE_Types.h"
#include "global.h"
#include "ControlProtectiveInterface.h"

void RPMGaugeTimer10ms(void);
bool HallARise(void);
bool HallBRise(void);


void InitRPMGauge(void)
{
	AddSwTimer(RPMGaugeTimer10ms,1,TM_REPEAT);
}

//
//  each rotor include a couple of magnets at 180 degrees each other. Therefore  the following sequence is expected for 1 turn
//
//	----+-timeAB--+----------timeBA-------------------+---timeAB--+-----------TimeBA---+----------
//     +-+                                            +-+                              +-+
//     | |                                            | |                              | |
// A --+ +--------------------------------------------+ +------------------------------+ +--------
//                +-+                                            +-+
//                | |                                            | |
// B -------------+ +--------------------------------------------+ +------------------------------
//

//  if timeBA > timeAB ,  CCW turning is supposed
//  if timeAB > timeBA ,  CW  turning is supposed
//  TimePerHalfTurnMs = timeAB + timeBA = 1/2 revolution time in milliseconds
//  rounds per ms =  2 / TimePerHalfTurnms ( if 60 RPM ,  2 / 2000 half round per ms -->  0,001 round per ms.
//  to get 100 x RPM ,  Rpmx100 =  ( 60 * 1000 * 100 ) / ( TimePerTurn in ms )
//  TimePerTurn in ms = 2 x TimePerHalfTurnms
//
//  ADetectedCnt is 2 when 2 times A have been detected
//  BDetectedCnt is 2 when 2 times B have been detected
//  When ADetected == 2 and BDetected == 2  , it is possible to perform an RPM evaluation
//  both it A B ... A found or B A .. B found
//
int CurrentCounter = 0;
int TimeABms = 0;
int TimeBAms = 0;
int Rpmx100 = 0;

void RPMGaugeTimer10ms()
{
int ADetectCnt = 0;
int BDetectCnt = 0;

	CurrentCounter++;
	if(HallARise()){
		TimeBAms = CurrentCounter * 10;
		CurrentCounter = 0;
		ADetectCnt++;
	}
	else if( HallBRise()){
		TimeABms = CurrentCounter * 10;
		CurrentCounter = 0;
		BDetectCnt++;
	}

	// verify timeout --> motor stopped
	// if rpm < 6 , send rpm = 0
	// 6 rpm  --> 10s per round -->  5 sec per half round --> 5000 ms
	if(CurrentCounter >= 500){
		Rpmx100 = 0;
		BDetectCnt = ADetectCnt	= 0;
		CurrentCounter = 0;
		onNewFilterPumpRPM(0);
	}
	else if(((ADetectCnt == 2) && (BDetectCnt == 1)) || ((BDetectCnt == 2) && (ADetectCnt == 1)))
	{
		Rpmx100 = (60 * 1000 * 100) / (2* (TimeBAms + TimeABms));
		if(TimeBAms <= TimeABms) Rpmx100 *= -1;
		BDetectCnt = ADetectCnt	= 0;
		CurrentCounter = 0;
		onNewFilterPumpRPM((uint16_t)Rpmx100);
	}
}

bool OldValA = FALSE;
bool OldValB = FALSE;

bool HallARise(void)
{
	  if(HallSens.PumpFilter_HSens1 != OldValA){
		  // something changed
		  OldValA = HallSens.PumpFilter_HSens1;
		  // rising or falling edge ?
		  return HallSens.PumpFilter_HSens1 ? TRUE : FALSE ;
	  }
	  // nothing changed
	  return FALSE;
}

bool HallBRise(void)
{
	  if(HallSens.PumpFilter_HSens1 != OldValB){
		  // something changed
		  OldValB = HallSens.PumpFilter_HSens2;
		  // rising or falling edge ?
		  return HallSens.PumpFilter_HSens2 ? TRUE : FALSE ;
	  }
	  // nothing changed
	  return FALSE;
}





