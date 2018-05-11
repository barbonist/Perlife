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
bool HallARise(int PumpIndex);
bool HallBRise(int PumpIndex);
void ManageRPMPump(int ii);

int16_t SignedSpeedRPMx100[4] = {0,0,0,0};

void InitRPMGauge(void)
{
	AddSwTimer(RPMGaugeTimer10ms,1,TM_REPEAT);
}


uint16_t GetMeasuredPumpSpeed(int PumpIndex) {
	if (PumpIndex <= 3)
		return SignedSpeedRPMx100[PumpIndex];
	else
		return 0xFFFF;
}


//
//  each rotor include a couple of magnets at 180 degrees each other. Therefore  the following sequence is expected for 1 turn
//
//	---+timeAB +-------------timeBA--------------------+timeAB-+---------------TimeBA---+----------
//     +-----------+                                   +---------+                      +------------+
//     |           |                                   |         |                      |            |
// A --+           +-----------------------------------+         +----------------------+            +--------
//             +----------+                                    +----------+
//             |          |                                    |          !
// B ----------+          +------------------------------------+          +-------------------------
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
//  When ADetected == 2 and BDetected == 1  , it is possible to perform an RPM evaluation
//  When ADetected == 1 and BDetected == 2  , it is possible to perform an RPM evaluation
//
void RPMGaugeTimer10ms()
{
	int ii;
	for( ii=0; ii<4 ; ii++)
		ManageRPMPump(ii);
}


void ManageRPMPump(int ii)
{
	static int ADetectCnt[4] 	= {0,0,0,0};
	static int BDetectCnt[4]	= {0,0,0,0};
	static int CurrentCounter[4] = {0,0,0,0};
	static int TimeABms[4] 		= {0,0,0,0};
	static int TimeBAms[4] 		= {0,0,0,0};
	static int Rpmx100[4] 		= {0,0,0,0};


		CurrentCounter[ii]++;
		if(HallARise(ii)){
			TimeBAms[ii] = CurrentCounter[ii] * 10;
			CurrentCounter[ii] = 0;
			ADetectCnt[ii]++;
		}
		else if( HallBRise(ii)){
			TimeABms[ii] = CurrentCounter[ii] * 10;
			CurrentCounter[ii] = 0;
			BDetectCnt[ii]++;
		}

		// verify timeout --> motor stopped
		// if rpm < 6 , send rpm = 0
		// 6 rpm  --> 10s per round -->  5 sec per half round --> 5000 ms
		if(CurrentCounter[ii] >= 500){
			Rpmx100[ii] = 0;
			BDetectCnt[ii] = ADetectCnt[ii]	= 0;
			CurrentCounter[ii] = 0;
			onNewPumpRPM(0, ii);
			SignedSpeedRPMx100[ii] = 0; // store locally
		}
		else if(((ADetectCnt[ii] == 2) && (BDetectCnt[ii] == 1)) || ((BDetectCnt[ii] == 2) && (ADetectCnt[ii] == 1)))
		{
			Rpmx100[ii] = (60 * 1000 * 100) / (2* (TimeBAms[ii] + TimeABms[ii]));
			//if(TimeBAms <= TimeABms) Rpmx100 *= -1;
			BDetectCnt[ii] = ADetectCnt[ii]	= 0;
			CurrentCounter[ii] = 0;
			int16_t NewVal = (int16_t)Rpmx100[ii];
			if( ii != 1 ){
				if(TimeBAms[ii] >= TimeABms[ii]) NewVal *= -1;
			}
			else{
				if(TimeBAms[ii] <= TimeABms[ii]) NewVal *= -1;
			}
			onNewPumpRPM(NewVal, ii);
			SignedSpeedRPMx100[ii] = NewVal; // store locally
		}
}


bool* ASensorValP[4] = {
		&HallSens.PumpFilter_HSens1,
		&HallSens.PumpArt_Liver_HSens1,
		&HallSens.PumpOxy_1_HSens1,
		&HallSens.PumpOxy_2_HSens1
};

bool HallARise(int PumpIndex)
{
static	bool OldValA[4] = {FALSE,FALSE,FALSE,FALSE};
	  if(*ASensorValP[PumpIndex] != OldValA[PumpIndex]){
		  // something changed
		  OldValA[PumpIndex] = *ASensorValP[PumpIndex];
		  // rising or falling edge ?
		  return *ASensorValP[PumpIndex] ? FALSE : TRUE ;
	  }
	  // nothing changed
	  return FALSE;
}

bool* BSensorValP[4] = {
		&HallSens.PumpFilter_HSens2,
		&HallSens.PumpArt_Liver_HSens2,
		&HallSens.PumpOxy_1_HSens2,
		&HallSens.PumpOxy_2_HSens2
};

bool HallBRise(int PumpIndex) {
	static bool OldValB[4] = { FALSE, FALSE, FALSE, FALSE };
	if (*BSensorValP[PumpIndex] != OldValB[PumpIndex]) {
		// something changed
		OldValB[PumpIndex] = *BSensorValP[PumpIndex];
		// rising or falling edge ?
		return *BSensorValP[PumpIndex] ? FALSE : TRUE;
	}
	// nothing changed
	return FALSE;
}





