/*
 * ActionsProtective.c
 *
 *  Created on: 18/apr/2018
 *      Author: W5
 */


/*
Possible events and Protective Reactions

e001)	Communication with master is no longer available --> (a001) Cosa fare cosa fare ??
e002)   State transition not expected ( state parent child guard ) --> (a002)
e003)	Mismatch Received parameter and measured parameter	--> (a003)
	PressFilter	--> (a003-A)
	PressArt--> (a003-B)
	PressVen--> (a003-C)
	PressLevelx100;--> (a003-D)
	PressOxy--> (a003-E)

	TempFluidx10--> (a003-F)
	TempArtx10--> (a003-G)
	TempVenx10--> (a003-H)

	Pimch1Pos--> (a003-I)
	Pimch2Pos--> (a003-L)

	SpeedPump1Rpmx10--> (a003-M)
	SpeedPump2Rpmx10--> (a003-N)
	SpeedPump3Rpmx10--> (a003-O)
	SpeedPump4Rpmx10--> (a003-P)

e004)	Received AirAlarm; 	--> (a004)
e005)	Received AlarmCode	--> (a005)

*/



/*
cosa fa la protective:
specifiche derivate da risk analysis

I sensori di pressione SPA2 e SPV2 vengono letti e differiscono del 20%
	--> invia l'allarme alla control.

---------------------------------------
Viene superato il valore di pressione del 20% oltre il  limite

1) comunica l'allarme alla CONTROL
2) Toglie la coppia (enable) ad uno o + motori
3) toglie l'enable ad una o + pinch che (in base al FW custom delle pinch) farà si che la stessa si porti in stato di sicurezza ossia aperta a sinistra (bypas organo e/o bypass filtro)
4) togliere l'alimentazione ai motori e/o alle pinch
5) Disalimenta il frogo /riscaldatore tramite relè"
---------------------------------------

T incongruente o T piastra troppo bassa (< -15 )
--> 1) disattivare il frigo ,
	2) segnala allarme
	3) dirottare il fluido su reservoir

---------------------------------------

Se STA2 o STV2 incongruenti  o valore < 2 gradi
	1) disattivare il frigo ,
	2) segnala allarme , quale ?
	3) dirottare il fluido su reservoir
	4) fermare pompe

---------------------------------------

Se STV2 o STA2  > 40 gradi
--> segnala un allarme
	disabilita la Pinch per dirottare il fluido sul reservoir
	fermare pompe
	disalimentare riscaldatore
	bloccare il frigo ( fosse mai acceso )

-----------------------------------------

sensori di posizione delle Pinch , se incongruenza
--> segnala un allarme
	disabilita la Pinch per dirottare il fluido sul reservoir
	fermare pompe
	disalimenta riscandatore
	blocca il frigo

------------------------------------------
sensori di velocità della Pompe incongruenti:
--> segnala un allarme
	disattiva la piompa
	pilota la Pinch per dirottare il fluido sul reservoir
	ferma il frigo
	ferma il riscandatore

---------------------------------------------

sensore SPL2 livello incongruente con SPL1
	1) inviare allarme
	2) bloccare pompe.
	3) pinch dirotatte su reservoir

	ATTENZIONE , FILTRARE E METTERE UNA DIFFERENZA MOLTO ALTA PER EVITARE FALSI ALLARMI dati i valori molto bassi di pressione

--------------------------------------------

*/
#include "Cpu.h"
#include "Events.h"
#include "Global.h"
#include "ActionsProtective.h"
#include "ControlProtectiveInterface.h"

//#define NO_DISABLE_FORTEST 1
#undef NO_DISABLE_FORTEST

void InitActuators(void)
{
	  Enable_Power_EVER_PUMP(TRUE);
	  Enable_Power_Motor(TRUE);
	  Enable_Pump_filter(TRUE);
	  Enable_Pump_Art_Livcer(TRUE);
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
	  Enable_Pump_Art_Livcer(FALSE);
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
	  Enable_Pump_Art_Livcer(FALSE);
	  Enable_Pump_OXY(FALSE);
	  Enable_Pinch_Filter(FALSE);
	  Enable_Pinch_Arterial(FALSE);
	  Enable_Pinch_Venous(FALSE);
#endif
}

void EnablePinchNPumps(void )
{
	  Enable_Pump_filter(TRUE);
	  Enable_Pump_Art_Livcer(TRUE);
	  Enable_Pump_OXY(TRUE);
	  Enable_Pinch_Filter(TRUE);
	  Enable_Pinch_Arterial(TRUE);
	  Enable_Pinch_Venous(TRUE);
}

void SwitchOFFPinchNPumps(void)
{
#ifndef NO_DISABLE_FORTEST
	Enable_Power_EVER_PUMP(FALSE);
	Enable_Power_Motor(FALSE);
#endif
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

void Enable_Pump_Art_Livcer(bool status)
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
	if (status && !EN_CLAMP_P_1_GetVal())
		EN_CLAMP_P_1_SetVal();

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

//#define DISABLE_HEATER_FRIGO_CONTROL
#undef DISABLE_HEATER_FRIGO_CONTROL

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




