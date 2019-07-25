/*
 * ActionsProtective.c
 *
 *  Created on: 18/apr/2018
 *      Author: W5
 */


#include "Cpu.h"
#include "Events.h"
#include "Global.h"
#include "ActionsProtective.h"
#include "ControlProtectiveInterface.h"

#ifdef PROTECTIVE_SLEEPS
	#define NO_DISABLE_FORTEST 1
	#define TEST_T1
#else
	#undef NO_DISABLE_FORTEST
#endif

void InitActuators(void)
{
	  Enable_Power_EVER_PUMP(TRUE);
	  Enable_Power_Motor(TRUE);
	  Enable_Pump_filter(TRUE);
	  Enable_Pump_Art_Liver(TRUE);
	  Enable_Pump_OXY(TRUE);
	  Enable_Pinch_Filter(TRUE);
	  Enable_Pinch_Arterial(TRUE);
	  Enable_Pinch_Venous(TRUE);
	  Enable_Heater_CommandCAN(FALSE); // funzione non disabilitabile a scopo di debug scheda control
	  Enable_Frigo_CommandCAN(FALSE);
	  Heat_ON = FALSE;
	  Frigo_ON = FALSE;
}

void GoTotalSafetyxxdsfdsasddsadsa(void) // mai chiamata
{
	  Enable_Power_EVER_PUMP(FALSE);
	  Enable_Power_Motor(FALSE);
	  Enable_Pump_filter(FALSE);
	  Enable_Pump_Art_Liver(FALSE);
	  Enable_Pump_OXY(FALSE);
	  Enable_Pinch_Filter(FALSE);
	  Enable_Pinch_Arterial(FALSE);
	  Enable_Pinch_Venous(FALSE);
	  Enable_Heater(FALSE);
	  Enable_Frigo(FALSE);
	  Heat_ON = FALSE;
	  Frigo_ON = FALSE;
}

void DisablePinchNPumps(void )
{
#ifndef NO_DISABLE_FORTEST
	  Enable_Pump_filter(FALSE);
	  Enable_Pump_Art_Liver(FALSE);
	  Enable_Pump_OXY(FALSE);
	  Enable_Pinch_Filter(FALSE);
	  Enable_Pinch_Arterial(FALSE);
	  Enable_Pinch_Venous(FALSE);
#endif
}

void EnablePinchNPumps(void )
{
	  Enable_Pump_filter(TRUE);
	  Enable_Pump_Art_Liver(TRUE);
	  Enable_Pump_OXY(TRUE);
	  Enable_Pinch_Filter(TRUE);
	  Enable_Pinch_Arterial(TRUE);
	  Enable_Pinch_Venous(TRUE);
}

void DisablePinches(void )
{
#ifndef NO_DISABLE_FORTEST
	  Enable_Pinch_Filter(FALSE);
	  Enable_Pinch_Arterial(FALSE);
	  Enable_Pinch_Venous(FALSE);
#endif
}

void EnablePinches(void )
{
	  Enable_Pinch_Filter(TRUE);
	  Enable_Pinch_Arterial(TRUE);
	  Enable_Pinch_Venous(TRUE);
}

void DisablePumps(void )
{
#ifndef NO_DISABLE_FORTEST
	  Enable_Pump_filter(FALSE);
	  Enable_Pump_Art_Liver(FALSE);
	  Enable_Pump_OXY(FALSE);
#endif
}

void EnablePumps(void )
{
	  Enable_Pump_filter(TRUE);
	  Enable_Pump_Art_Liver(TRUE);
	  Enable_Pump_OXY(TRUE);
}



void SwitchOFFPinchNPumps(void)
{
//#ifndef NO_DISABLE_FORTEST
	Enable_Power_EVER_PUMP(FALSE);
	Enable_Power_Motor(FALSE);
//#endif
}

void SwitchONPinchNPumps(void)
{
	Enable_Power_EVER_PUMP(TRUE);
	Enable_Power_Motor(TRUE);
}




/*Funtion to enable power and actuation of teh actuator*/
void Enable_Power_EVER_PUMP(bool status)
{
#ifndef NO_DISABLE_FORTEST
	if (status && !EN_P_1_C_GetVal())
	{
		EN_P_1_C_SetVal();
		EN_P_2_C_SetVal();
	}

	if (!status && EN_P_1_C_GetVal())
	{
		EN_P_1_C_ClrVal();
		EN_P_2_C_ClrVal();
	}
#else
	EN_P_1_C_SetVal();
	EN_P_2_C_SetVal();
#endif
}

// temp routines to test T1 , to be replaced later
void Enable_Power_EVER_PUMP_TestT1(bool status)
{
	if (status && !EN_P_1_C_GetVal())
	{
		EN_P_1_C_SetVal();
		EN_P_2_C_SetVal();
	}
	if (!status && EN_P_1_C_GetVal())
	{
		EN_P_1_C_ClrVal();
		EN_P_2_C_ClrVal();
	}
}

void Enable_Power_Motor_TestT1(bool status)
{
	if (status && !EN_24_M_P_GetVal())
		EN_24_M_P_SetVal();
	if (!status && EN_24_M_P_GetVal())
		EN_24_M_P_ClrVal();
}



void Enable_Power_Motor(bool status)
{
#ifndef NO_DISABLE_FORTEST
	if (status && !EN_24_M_P_GetVal())
		EN_24_M_P_SetVal();
	if (!status && EN_24_M_P_GetVal())
		EN_24_M_P_ClrVal();
#else
	EN_24_M_P_SetVal();
#endif
}

void Enable_Pump_filter(bool status)
{
#ifndef NO_DISABLE_FORTEST
	if (status && !EN_MOTOR_P_1_GetVal())
		EN_MOTOR_P_1_SetVal();

	if (!status && EN_MOTOR_P_1_GetVal())
		EN_MOTOR_P_1_ClrVal();
#else
	EN_MOTOR_P_1_SetVal();
#endif
}

void Enable_Pump_Art_Liver(bool status)
{
#ifndef NO_DISABLE_FORTEST
	if (status && !EN_MOTOR_P_2_GetVal())
		EN_MOTOR_P_2_SetVal();

	if (!status && EN_MOTOR_P_2_GetVal())
		EN_MOTOR_P_2_ClrVal();
#else
	EN_MOTOR_P_2_SetVal();
#endif
}

void Enable_Pump_OXY(bool status)
{
#ifndef NO_DISABLE_FORTEST
	if (status && !EN_MOTOR_P_3_GetVal())
		EN_MOTOR_P_3_SetVal();

	if (status && !EN_MOTOR_P_4_GetVal())
		EN_MOTOR_P_4_SetVal();

	if (!status && EN_MOTOR_P_3_GetVal())
		EN_MOTOR_P_3_ClrVal();

	if (!status && EN_MOTOR_P_4_GetVal())
		EN_MOTOR_P_4_ClrVal();
#else
	EN_MOTOR_P_3_SetVal();
	EN_MOTOR_P_4_SetVal();
#endif
}


void Enable_Pinch_Filter(bool status)
{
#ifndef NO_DISABLE_FORTEST
	if (status && !EN_CLAMP_P_1_GetVal()){
		EN_CLAMP_P_1_SetVal();
	}
	if (!status && EN_CLAMP_P_1_GetVal())
		EN_CLAMP_P_1_ClrVal();
#else
	EN_CLAMP_P_1_SetVal();
#endif

}


void Enable_Pinch_Arterial(bool status)
{
#ifndef NO_DISABLE_FORTEST
	if (status && !EN_CLAMP_P_2_GetVal())
		EN_CLAMP_P_2_SetVal();

	if (!status && EN_CLAMP_P_2_GetVal())
		EN_CLAMP_P_2_ClrVal();
#else
	EN_CLAMP_P_2_SetVal();
#endif
}

void Enable_Pinch_Venous(bool status)
{
#ifndef NO_DISABLE_FORTEST
	if (status && !EN_CLAMP_P_3_GetVal())
		EN_CLAMP_P_3_SetVal();

	if (!status && EN_CLAMP_P_3_GetVal())
		EN_CLAMP_P_3_ClrVal();
#else
	EN_CLAMP_P_3_SetVal();
#endif
}


//
// special for T1 test
//

void Enable_Pinch_Filter_TestT1(bool status)
{
	if (status && !EN_CLAMP_P_1_GetVal()){
		EN_CLAMP_P_1_SetVal();
	}
	if (!status && EN_CLAMP_P_1_GetVal())
		EN_CLAMP_P_1_ClrVal();
}

void Enable_Pinch_Arterial_TestT1(bool status)
{
	if (status && !EN_CLAMP_P_2_GetVal())
		EN_CLAMP_P_2_SetVal();

	if (!status && EN_CLAMP_P_2_GetVal())
		EN_CLAMP_P_2_ClrVal();
}

void Enable_Pinch_Venous_TestT1(bool status)
{
	if (status && !EN_CLAMP_P_3_GetVal())
		EN_CLAMP_P_3_SetVal();
	if (!status && EN_CLAMP_P_3_GetVal())
		EN_CLAMP_P_3_ClrVal();
}


void Enable_Pump_filter_TestT1(bool status)
{
	if (status && !EN_MOTOR_P_1_GetVal())
		EN_MOTOR_P_1_SetVal();

	if (!status && EN_MOTOR_P_1_GetVal())
		EN_MOTOR_P_1_ClrVal();

}

void Enable_Pump_Art_Liver_TestT1(bool status)
{
	if (status && !EN_MOTOR_P_2_GetVal())
		EN_MOTOR_P_2_SetVal();

	if (!status && EN_MOTOR_P_2_GetVal())
		EN_MOTOR_P_2_ClrVal();
}

void Enable_Pump_OXY_TestT1(bool status)
{
	if (status && !EN_MOTOR_P_3_GetVal())
		EN_MOTOR_P_3_SetVal();

	if (status && !EN_MOTOR_P_4_GetVal())
		EN_MOTOR_P_4_SetVal();

	if (!status && EN_MOTOR_P_3_GetVal())
		EN_MOTOR_P_3_ClrVal();

	if (!status && EN_MOTOR_P_4_GetVal())
		EN_MOTOR_P_4_ClrVal();
}

//
//
//

bool Pinch_Filter_IsEnabled(void)
{
	return EN_CLAMP_P_1_GetVal();
}


bool Pinch_Arterial_IsEnabled(void)
{
	return EN_CLAMP_P_2_GetVal();
}


bool Pinch_Venous_IsEnabled(void)
{
	return EN_CLAMP_P_3_GetVal();
}

#ifdef PROTECTIVE_SLEEPS
	#define DISABLE_HEATER_FRIGO_CONTROL
#else
	#undef DISABLE_HEATER_FRIGO_CONTROL
#endif


void Enable_Heater(bool status)
{
#ifndef 	DISABLE_HEATER_FRIGO_CONTROL
	if (status && !HEAT_ON_P_GetVal())
	{
		HEAT_ON_P_SetVal();
		Heat_ON = TRUE;
	}

	if (!status && HEAT_ON_P_GetVal())
	{
		HEAT_ON_P_ClrVal();
		Heat_ON = FALSE;
	}
#endif
}

void Enable_Frigo(bool status)
{
#ifndef 	DISABLE_HEATER_FRIGO_CONTROL
	if (status && !COMP_ENABLE_GetVal())
	{
		COMP_ENABLE_SetVal();
		Frigo_ON = TRUE;
	}

	if (!status && COMP_ENABLE_GetVal())
	{
		COMP_ENABLE_ClrVal();
		Frigo_ON = FALSE;
	}
#endif
}



void Enable_Heater_CommandCAN(bool status)
{
	if (status && !HEAT_ON_P_GetVal())
	{
		HEAT_ON_P_SetVal();
		Heat_ON = TRUE;
	}

	if (!status && HEAT_ON_P_GetVal())
	{
		HEAT_ON_P_ClrVal();
		Heat_ON = FALSE;
	}
}

void Enable_Frigo_CommandCAN(bool status)
{
	if (status && !COMP_ENABLE_GetVal())
	{
		COMP_ENABLE_SetVal();
		Frigo_ON = TRUE;
	}

	if (!status && COMP_ENABLE_GetVal())
	{
		COMP_ENABLE_ClrVal();
		Frigo_ON = FALSE;
	}
}






