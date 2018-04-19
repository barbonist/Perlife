/*
 * ProtectiveControlInterface.c
 *
 *  Created on: 12/apr/2018
 *      Author: SB
 */

#include "stdint.h"
#include "Global.h"
#include "SWTimer.h"


void InitSensorsAccess(void)
{
	// init pressure sensors
	ADC1_Calibration();
	PR_Sens_ADC_Init();
	AD1_Start();
	AddSwTimer(AD1_Start,1,TM_REPEAT);
}


void LoopSensorsAccess(void)
{
	 Manange_ADC1();
}




