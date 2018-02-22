/*
 * pid.c
 *
 *  Created on: 09/feb/2018
 *      Author: franco mazzoni
 */



#include "PE_Types.h"
#include "Global.h"
#include "PANIC_BUTTON_INPUT.h"
#include "App_Ges.h"
#include "ModBusCommProt.h"
#include "Peltier_Module.h"

#include "Pins1.h"

#include "MODBUS_COMM.h"
#include "ASerialLdd1.h"

#include "D_7S_DP.h"
#include "D_7S_A.h"
#include "D_7S_B.h"
#include "D_7S_C.h"
#include "D_7S_D.h"
#include "D_7S_E.h"
#include "D_7S_F.h"
#include "D_7S_G.h"

#include "COVER_M1.h"
#include "COVER_M2.h"
#include "COVER_M3.h"
#include "COVER_M4.h"
#include "COVER_M5.h"

#include "BUBBLE_KEYBOARD_BUTTON1.h"
#include "BUBBLE_KEYBOARD_BUTTON2.h"
#include "BUBBLE_KEYBOARD_BUTTON3.h"
#include "BUBBLE_KEYBOARD_BUTTON4.h"

#include "BUZZER_LOW_C.h"
#include "BUZZER_MEDIUM_C.h"
#include "BUZZER_HIGH_C.h"

#include "HEAT_ON_C.h"
#include "LAMP_LOW.h"
#include "LAMP_MEDIUM.h"
#include "LAMP_HIGH.h"

#include "RTS_MOTOR.h"
#include "EN_CLAMP_CONTROL.h"
#include "EN_MOTOR_CONTROL.h"
#include "EN_24_M_C.h"
#include "child_gest.h"
#include "Alarm_Con.h"
#include "PC_DEBUG_COMM.h"
#include "stdio.h"
#include "string.h"
#include "statesStructs.h"
#include "general_func.h"
#include "pid.h"

extern word MedForArteriousPid;
extern word MedForVenousPid;

float pressSample1_Ven = 0;
float pressSample2_Ven = 0;

float pressSample1 = 0;  // FM Questi due vanno inizializzati alla pressione corrente prima di far partire il pid
float pressSample2 = 0;
unsigned char PidFirstTime[4];

float pressSample1_Art = 0;
float pressSample2_Art = 0;


//------------------------------NUOVO PID PER POMPA ARTERIOSA------------------------------------------------------------------------

float CalcolaPresArt(float speed)
{
//  float m = ((float)128.0 - (float)50.0) / ((float)100.0 );
//  float press = m * (speed) + (float)50.0;
	//float m;


	float m = ((float)60.0 - (float)40.0) / ((float)60.0 );


  float press = m * (speed) + (float)40.0;
  return press;
}

int SpeedCostanteArt( int CurrSpeed)
{
	static int SpeedCostanteState = 0;
	static int LastspeedValue;
	static int Cnt = 0;
	int delta;
	static int min, max;
	int SpeedCostanteFlag = 0;

	if(!SpeedCostanteState)
	{
		LastspeedValue = CurrSpeed;
		SpeedCostanteState = 1;
		Cnt = 0;

		delta = LastspeedValue * 5 / 100;  // prendo il 5% del valore corrente
		if(delta < 1)
			delta = 1;
		min = LastspeedValue - delta;
		if(min < 0)
			min = 0;
		max = LastspeedValue + delta;
	}
	else if(SpeedCostanteState)
	{
		if(CurrSpeed < min || CurrSpeed > max )
			SpeedCostanteState = 0;
		else
			Cnt++;
		if(Cnt >= 100)
		{
			SpeedCostanteFlag = 1;
			SpeedCostanteState = 0;
		}
	}
	return SpeedCostanteFlag;
}

void alwaysPumpPressLoopArt(unsigned char pmpId, unsigned char *PidFirstTime)
{
	static float deltaSpeed_Art = 0;
	static float actualSpeed_Art = 0;
	static bool StopPid = FALSE;
	static unsigned long StartTimePidStop = 0;

	float pressSample0_Art = 0;

	float errPress = 0;
	/*il valore sottostante, indica la massima velocità delle pompe
	 * sarà da ripristyinare quello in funzione del flusso ovvero:
	 * (int) (parameterWordSetFromGUI[PAR_SET_MAX_FLOW_PERFUSION].value / (PUMP_ART_GAIN * 2.0));*/
	int Max_RPM_for_Flow_Max = MAX_ART_RPM;
	int Speed_Media = 0;
	static int Somma_Speed = 0;
	static int Speed_Media_old = 0xfff; /* valore irraggiungibile*/

	static int Count = 0;
	static int Target_PID = 0;

	int MAX_ART_RPM_Val;
	MAX_ART_RPM_Val = parameterWordSetFromGUI[PAR_SET_MAX_FLOW_PERFUSION].value / 9.3;


    /*funzione col solo proporzionale usata per mandare in oscillazione la pressione
     * e calcolare il K minimo e il periodo di oscillazione
     * //	deltaSpeed_Art = parKP_Art * errPress;*/

/*************************************************************************************************/
	// Queste righe di codice possono essere inserite se voglio bloccare il pid per
// qualche secondo dopo che ho applicato una variazione di velocita' per effetto di SpeedCostanteVen.
// In questo modo do tempo alle variazioni di avere effetto.
//	if(StopPid && StartTimePidStop)
//	{
//		if(msTick_elapsed(StartTimePidStop) * 50L >= 3000)
//		{
//			StopPid = FALSE;
//		}
//		else
//			return;
//	}
/*************************************************************************************************/

    if(*PidFirstTime == PRESS_LOOP_ON)
    {
    	*PidFirstTime = PRESS_LOOP_OFF;
    	actualSpeed_Art = (float)pumpPerist[pmpId].actualSpeed;
    	Target_PID = parameterWordSetFromGUI[PAR_SET_PRESS_ART_TARGET].value + CalcolaPresArt(actualSpeed_Art);
    }

    //pressSample0_Art = PR_VEN_mmHg_Filtered;
    pressSample0_Art = MedForArteriousPid;
	errPress = Target_PID - pressSample0_Art;

	   if (errPress > -5  && errPress < 5 )
	   {
		   Count ++;
		   Somma_Speed += actualSpeed_Art;

			if ( Count >= 5)
			{
				Speed_Media = Somma_Speed/Count;
				Speed_Media_old = Speed_Media;

				Target_PID = parameterWordSetFromGUI[PAR_SET_PRESS_ART_TARGET].value  + CalcolaPresArt(Speed_Media);
				Count = 0;
				Somma_Speed = 0;
			}
	   }
	   else
	   {
		   Count = 0;
		   Somma_Speed = 0;
	   }

		//la velocità del messaggio di stato resta costante per 5 secondi && velocità minore del massimo)
	    // incrementiamo la actual speed di 5 RPM;
	   if (SpeedCostanteArt((int)actualSpeed_Art) && (actualSpeed_Art <= MAX_ART_RPM_Val))
	   {
		   actualSpeed_Art += 5.0;
		   Target_PID = parameterWordSetFromGUI[PAR_SET_PRESS_ART_TARGET].value  + CalcolaPresArt(actualSpeed_Art);
		   StopPid = TRUE;
		   StartTimePidStop = timerCounterModBus;
	   }

	deltaSpeed_Art = ((parKITC_Art * errPress) -
			         (parKP_Art * (pressSample0_Art - pressSample1_Art)) -
					 (parKD_TC_Art * (pressSample0_Art - 2 * pressSample1_Art + pressSample2_Art)));

	// valutare se mettere il deltaSpeed = 0 nel caso deltaSpeed sia negativo in modo da non far andare actualSpeed a zero troppo in fretta
	// in alternativa il deltaSpeed va considerato solo se è abbastanza negativo
	if ((sensor_UFLOW[0].Average_Flow_Val != 0.0) &&
			(sensor_UFLOW[0].Average_Flow_Val > ( parameterWordSetFromGUI[PAR_SET_MAX_FLOW_PERFUSION].value)) &&
			(actualSpeed_Art != 0.0))
	{
		float pmp_gain = sensor_UFLOW[0].Average_Flow_Val / pumpPerist[0].actualSpeed;
		actualSpeed_Art = (float) parameterWordSetFromGUI[PAR_SET_MAX_FLOW_PERFUSION].value / pmp_gain;
	}

	if((deltaSpeed_Art < -0.1) || (deltaSpeed_Art > 0.1))
	{
		if (deltaSpeed_Art < 0)
		{
			actualSpeed_Art = actualSpeed_Art + deltaSpeed_Art;
		}
		else
		{
			// da ripristinare solo quando siamo sicuri del funzionamento dei flussimetri
//			if ((sensor_UFLOW[0].Average_Flow_Val != 0.0) &&
//				(sensor_UFLOW[0].Average_Flow_Val > ( parameterWordSetFromGUI[PAR_SET_MAX_FLOW_PERFUSION].value)))
//
//			{
//				actualSpeed_Art = actualSpeed_Art; //non aumento la velocità
//			}
//			else
			if ( (actualSpeed_Art + deltaSpeed_Art  ) > Max_RPM_for_Flow_Max )
			{
				actualSpeed_Art = Max_RPM_for_Flow_Max;
			}
			else
			{
				if(actualSpeed_Art > (float)MAX_ART_RPM_Val)
					actualSpeed_Art = (float)MAX_ART_RPM_Val;
				else
					actualSpeed_Art = actualSpeed_Art + deltaSpeed_Art;
			}
		}
	}

	if((actualSpeed_Art <= 0) || (pressSample0_Art > 80))
	{
		actualSpeed_Art = 0;
		Target_PID = parameterWordSetFromGUI[PAR_SET_PRESS_ART_TARGET].value + CalcolaPresArt(actualSpeed_Art);
	}

	if(actualSpeed_Art != pumpPerist[pmpId].actualSpeedOld)
	{
		setPumpSpeedValueHighLevel(pumpPerist[pmpId].pmpMySlaveAddress, ((int)(actualSpeed_Art * 100)));
		pumpPerist[pmpId].actualSpeedOld = actualSpeed_Art;
	}

	pressSample2_Art = pressSample1_Art;
	pressSample1_Art = pressSample0_Art;
}


//------------------------------PID PER POMPA ARTERIOSA ORIGINALE----------------------------------------------------------


//float parKITC = 0.2;
//float parKP = 1;
//float parKD_TC = 0.8;
//float GlobINTEG;
//float GlobPROP;
//float GlobDER;
//int deltaSpeed = 0;
//int actualSpeed = 0;

void alwaysPumpPressLoop(unsigned char pmpId, unsigned char *PidFirstTime)
{

	int deltaSpeed = 0;
	static int actualSpeed = 0;
	//static int actualSpeedOld = 0;
	float parKITC = 0.2;
	float parKP = 1;
	float parKD_TC = 0.8;
	float pressSample0 = 0;
	//static float pressSample1 = 0;  // FM Questi due vanno inizializzati alla pressione corrente prima di far partire il pid
	//static float pressSample2 = 0;
	float errPress = 0;
	int Max_RPM_for_Flow_Max = (int) ( parameterWordSetFromGUI[PAR_SET_MAX_FLOW_PERFUSION].value / PUMP_ART_GAIN);

    if(*PidFirstTime == PRESS_LOOP_ON)
    {
    	*PidFirstTime = PRESS_LOOP_OFF;
    	actualSpeed = (int)pumpPerist[pmpId].actualSpeed;
    }

	// FM sostituito con il valore in mmHg

    pressSample0 = PR_ART_mmHg_Filtered;
	errPress = parameterWordSetFromGUI[PAR_SET_PRESS_ART_TARGET].value - pressSample0;

	//GlobINTEG = parKITC*errPress;
	//GlobPROP = parKP*(pressSample0 - pressSample1);
	//GlobDER = parKD_TC*(pressSample0 - 2*pressSample1 + pressSample2);

	deltaSpeed = (int)((parKITC*errPress) - (parKP*(pressSample0 - pressSample1)) - (parKD_TC*(pressSample0 - 2*pressSample1 + pressSample2)));

	// valutare se mettere il deltaSpeed = 0 nel caso deltaSpeed sia negativo in modo da non far andare actualSpeed a zero troppo in fretta
	// in alternativa il deltaSpeed va considerato solo se è abbastanza negativo
	if((deltaSpeed < -2) || (deltaSpeed > 2))
	{
		if (deltaSpeed < 0)
		{
			actualSpeed = actualSpeed + deltaSpeed;
		}
		else
		{
			if (sensor_UFLOW[0].Average_Flow_Val > ( parameterWordSetFromGUI[PAR_SET_MAX_FLOW_PERFUSION].value))

			{
				actualSpeed = actualSpeed; /*non aumento la velocità*/
			}
			else if ( (actualSpeed + deltaSpeed  ) > Max_RPM_for_Flow_Max )
			{
				actualSpeed = Max_RPM_for_Flow_Max;
			}
			else
			{
				actualSpeed = actualSpeed + deltaSpeed;
			}
		}
	}

//	if(actualSpeed >= 50)
//		actualSpeed = 50;
//	else if(actualSpeed <= 0)
//		actualSpeed = 0;

	if(actualSpeed < 0)
		actualSpeed = 0;

	if(actualSpeed != pumpPerist[pmpId].actualSpeedOld){
		//setPumpSpeedValue(pumpPerist[pmpId].pmpMySlaveAddress, (actualSpeed*100));
		setPumpSpeedValueHighLevel(pumpPerist[pmpId].pmpMySlaveAddress, (actualSpeed*100));
		pumpPerist[pmpId].actualSpeedOld = actualSpeed;
	}

	//se provengo da un allarme la pompa è ferma ed il controllo deve ripartire
	/*if(pumpPerist[pmpId].actualSpeed == 0){
		actualSpeedOld = 0;
	}*/

	pressSample2 = pressSample1;
	pressSample1 = pressSample0;

	//DebugStringPID(); // debug
}




//------------------------------PID PER POMPA VENOSA------------------------------------------------------------------------

//void alwaysPumpPressLoopVen_old(unsigned char pmpId, unsigned char *PidFirstTime){
//	//int deltaSpeed = 0;
//	//static int actualSpeed = 0;
//	//static int actualSpeedOld = 0;
//	//float parKITC = 0.0; //0.2;
//	//float parKP = 5.0;   //1;
//	//float parKD_TC = 0.0; //0.8;
//	float pressSample0_Ven = 0;
//	//static float pressSample1 = 0;  // FM Questi due vanno inizializzati alla pressione corrente prima di far partire il pid
//	//static float pressSample2 = 0;
//	float errPress = 0;
//	int Max_RPM_for_Flow_Max = 120; //( parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / (PUMP_ART_GAIN * 2.0));
//	int Speed_Media;
//
//	static int Somma_Speed = 0;
//
//	static int Count = 0;
//	static int Count2 = 0;
//
//    if(*PidFirstTime == PRESS_LOOP_ON)
//    {
//    	*PidFirstTime = PRESS_LOOP_OFF;
//    	actualSpeed_Ven = (int)pumpPerist[pmpId].actualSpeed;
//    }
//
//    pressSample0_Ven = PR_VEN_mmHg_Filtered;
//	errPress = parameterWordSetFromGUI[PAR_SET_VENOUS_PRESS_TARGET].value - pressSample0_Ven;
//
//   if (errPress > -15  && errPress < 15 )
//   {
//	   Count ++;
//
//	   Somma_Speed += actualSpeed_Ven;
//
//		if ( Count >= 100)
//		{
//			Speed_Media = Somma_Speed/Count;
//
////			if (Speed_Media <= Speed_Media_old)
////			{
////				Speed_Media+=10;
////				Speed_Media = Speed_Media_old;
////			}
//
//			parameterWordSetFromGUI[PAR_SET_VENOUS_PRESS_TARGET].value  = get_Set_Point_Pressure(Speed_Media);
//			Count = 0;
//			Somma_Speed = 0;
//		}
//   }
//   else
//   {
//	   Count = 0;
//	   Somma_Speed = 0;
//   }
//
//
//
//	GlobINTEG_Ven = parKITC_Ven * errPress;
//	GlobPROP_Ven = parKP_Ven * (pressSample0_Ven - pressSample1_Ven);
//	GlobDER_Ven = parKD_TC_Ven * (pressSample0_Ven - 2*pressSample1_Ven + pressSample2_Ven);
//
//	deltaSpeed_Ven = (int)((parKITC_Ven * errPress) -
//			         (parKP_Ven * (pressSample0_Ven - pressSample1_Ven)) -
//					 (parKD_TC_Ven * (pressSample0_Ven - 2 * pressSample1_Ven + pressSample2_Ven)));
////	deltaSpeed_Ven = parKP_Ven * errPress;
//
//	if (errPress < 0)
//	{
//		int a = 0;
//	}
//
//	// valutare se mettere il deltaSpeed = 0 nel caso deltaSpeed sia negativo in modo da non far andare actualSpeed a zero troppo in fretta
//	// in alternativa il deltaSpeed va considerato solo se è abbastanza negativo
//	if((deltaSpeed_Ven < -2) || (deltaSpeed_Ven > 2))
//	{
//		if (deltaSpeed_Ven < 0)
//		{
//			actualSpeed_Ven = actualSpeed_Ven + deltaSpeed_Ven;
//		}
//		else
//		{
//			if (sensor_UFLOW[1].Average_Flow_Val > ( parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value))
//
//			{
//				actualSpeed_Ven = actualSpeed_Ven; /*non aumento la velocità*/
//			}
//			else if ( (actualSpeed + deltaSpeed  ) > Max_RPM_for_Flow_Max )
//			{
//				actualSpeed_Ven = Max_RPM_for_Flow_Max;
//			}
//			else
//			{
//				if(actualSpeed_Ven > 100)
//					actualSpeed_Ven = 100;
//				else
//					actualSpeed_Ven = actualSpeed_Ven + deltaSpeed_Ven;
//			}
//		}
//	}
//
//	if(actualSpeed_Ven < 0)
//		actualSpeed_Ven = 0;
//
//	if(actualSpeed_Ven != pumpPerist[pmpId].actualSpeedOld)
//	{
//		setPumpSpeedValueHighLevel(pumpPerist[pmpId].pmpMySlaveAddress, (actualSpeed_Ven * 100));
//		pumpPerist[pmpId].actualSpeedOld = actualSpeed_Ven;
//	}
//
//	pressSample2_Ven = pressSample1_Ven;
//	pressSample1_Ven = pressSample0_Ven;
//
//	//DebugStringPID(); // debug
//}

// Pid per perfusione venosa

int get_Set_Point_Pressure(int Speed)
{
	int Presure_Set_Point;

	Presure_Set_Point = GAIN_PRESSURE * Speed + DELTA_PRESSURE;

	return (Presure_Set_Point);
}

float CalcolaPresVen(float speed)
{
//  float m = ((float)128.0 - (float)50.0) / ((float)100.0 );
//  float press = m * (speed) + (float)50.0;
	//float m;

//   m = (float) ( (50 - 45) / 10) ;
//   m = m + (float) ( (56 - 50) / 10);
//   m = m + (float) ( (65 - 56) / 10);
//   m = m + (float) ( (76 - 65) / 10);
//   m = m + (float) ( (101 - 76) / 10);
//   m = m + (float) ( (122 - 101) / 10);
//   m = m + (float) ( (131 - 122) / 10);
//   m = m + (float) ( (149 - 131) / 10);
//   m = m + (float) ( (167 - 149) / 10);
//   m = m + (float) ( (184 - 167) / 10);
//   m=m/10;
	float m = ((float)184.0 - (float)45.0) / ((float)100.0 );

  float press = m * (speed) + (float)45.0;
  return press;
}

int SpeedCostanteVen( int CurrSpeed)
{
	static int SpeedCostanteState = 0;
	static int LastspeedValue;
	static int Cnt = 0;
	int delta;
	static int min, max;
	int SpeedCostanteFlag = 0;

	if(!SpeedCostanteState)
	{
		LastspeedValue = CurrSpeed;
		SpeedCostanteState = 1;
		Cnt = 0;

		delta = LastspeedValue * 5 / 100;  // prendo il 5% del valore corrente
		if(delta < 1)
			delta = 1;
		min = LastspeedValue - delta;
		if(min < 0)
			min = 0;
		max = LastspeedValue + delta;
	}
	else if(SpeedCostanteState)
	{
		if(CurrSpeed < min || CurrSpeed > max )
			SpeedCostanteState = 0;
		else
			Cnt++;
		if(Cnt >= 100)
		{
			SpeedCostanteFlag = 1;
			SpeedCostanteState = 0;
		}
	}
	return SpeedCostanteFlag;
}

void alwaysPumpPressLoopVen(unsigned char pmpId, unsigned char *PidFirstTime){


	static float deltaSpeed_Ven = 0;
	static float actualSpeed_Ven = 0;
	static bool StopPid = FALSE;
	static unsigned long StartTimePidStop = 0;

	float pressSample0_Ven = 0;

	float errPress = 0;
	/*il valore sottostante, indica la massima velocità delle pompe
	 * sarà da ripristyinare quello in funzione del flusso ovvero:
	 * (int) ( parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / (PUMP_OXY_GAIN * 2.0));*/
	int Max_RPM_for_Flow_Max = MAX_OXYG_RPM; //( parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / (PUMP_ART_GAIN * 2.0));
	int Speed_Media;
	static int Somma_Speed = 0;
	static int Speed_Media_old = 0xfff; /* valore irraggiungibile*/

	static int Count = 0;
	static int Count2 = 0;
	static int Target_PID_VEN = 0;

	int MAX_OXYG_RPM_Val;

	MAX_OXYG_RPM_Val = parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / 20.0;


    /*funzione col solo proporzionale usata per mandare in oscillazione la pressione
     * e calcolare il K minimo e il periodo di oscillazione
     * //	deltaSpeed_Ven = parKP_Ven * errPress;*/

/*************************************************************************************************/
	// Queste righe di codice possono essere inserite se voglio bloccare il pid per
// qualche secondo dopo che ho applicato una variazione di velocita' per effetto di SpeedCostanteVen.
// In questo modo do tempo alle variazioni di avere effetto.
//	if(StopPid && StartTimePidStop)
//	{
//		if(msTick_elapsed(StartTimePidStop) * 50L >= 3000)
//		{
//			StopPid = FALSE;
//		}
//		else
//			return;
//	}
/*************************************************************************************************/

    if(*PidFirstTime == PRESS_LOOP_ON)
    {
    	*PidFirstTime = PRESS_LOOP_OFF;
    	actualSpeed_Ven = (float)pumpPerist[pmpId].actualSpeed;
    	Target_PID_VEN = parameterWordSetFromGUI[PAR_SET_VENOUS_PRESS_TARGET].value + SET_POINT_PRESSURE_INIT;
    }

    //pressSample0_Ven = PR_VEN_mmHg_Filtered;
    pressSample0_Ven = MedForVenousPid;
	errPress = Target_PID_VEN - pressSample0_Ven;

	   if (errPress > -5  && errPress < 5 )
	   {
		   Count ++;
		   Somma_Speed += actualSpeed_Ven;

			if ( Count >= 5)
			{
				Speed_Media = Somma_Speed/Count;
				Speed_Media_old = Speed_Media;

//				if ((sensor_UFLOW[1].Average_Flow_Val != 0.0) &&
//					(sensor_UFLOW[1].Average_Flow_Val > ( parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value)))
//				{
//				}
//				else
					Target_PID_VEN = parameterWordSetFromGUI[PAR_SET_VENOUS_PRESS_TARGET].value + CalcolaPresVen(Speed_Media);

				Count = 0;
				Somma_Speed = 0;
			}
	   }
	   else
	   {
		   Count = 0;
		   Somma_Speed = 0;
	   }

		//la velocità del messaggio di stato resta costante per 5 secondi && velocità minore del massimo)
	    // incrementiamo la actual speed di 5 RPM;
	   if (SpeedCostanteVen((int)actualSpeed_Ven) && (actualSpeed_Ven <= MAX_OXYG_RPM_Val))
	   {
//			if ((sensor_UFLOW[1].Average_Flow_Val != 0.0) &&
//				(sensor_UFLOW[1].Average_Flow_Val > ( parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value)))
//			{
//			}
//			else
//			{
			    actualSpeed_Ven += 5.0;
				Target_PID_VEN = parameterWordSetFromGUI[PAR_SET_VENOUS_PRESS_TARGET].value + CalcolaPresVen(actualSpeed_Ven);
				StopPid = TRUE;
				StartTimePidStop = timerCounterModBus;
//			}
	   }

	deltaSpeed_Ven = ((parKITC_Ven * errPress) -
			         (parKP_Ven * (pressSample0_Ven - pressSample1_Ven)) -
					 (parKD_TC_Ven * (pressSample0_Ven - 2 * pressSample1_Ven + pressSample2_Ven)));

	// valutare se mettere il deltaSpeed = 0 nel caso deltaSpeed sia negativo in modo da non far andare actualSpeed a zero troppo in fretta
	// in alternativa il deltaSpeed va considerato solo se è abbastanza negativo
	if((deltaSpeed_Ven < -0.1) || (deltaSpeed_Ven > 0.1))
	{
		if (deltaSpeed_Ven < 0)
		{
			actualSpeed_Ven = actualSpeed_Ven + deltaSpeed_Ven;
		}
		else
		{
			// da ripristinare solo quando siamo sicuri del funzionamento dei flussimetri
//			if ((sensor_UFLOW[1].Average_Flow_Val != 0.0) &&
//				(sensor_UFLOW[1].Average_Flow_Val > ( parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value)))
//
//			{
//				actualSpeed_Ven = actualSpeed_Ven; //non aumento la velocità
//			}
//			else
				if ( (actualSpeed_Ven + deltaSpeed_Ven  ) > Max_RPM_for_Flow_Max )
			{
				actualSpeed_Ven = Max_RPM_for_Flow_Max;
			}
			else
			{
				if(actualSpeed_Ven > (float)MAX_OXYG_RPM_Val)
					actualSpeed_Ven = (float)MAX_OXYG_RPM_Val;
				else if ((sensor_UFLOW[1].Average_Flow_Val != 0.0) &&
						(sensor_UFLOW[1].Average_Flow_Val > ( parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value)) &&
						(actualSpeed_Ven != 0.0))
				{
					float pmp_gain = sensor_UFLOW[1].Average_Flow_Val / actualSpeed_Ven;
					actualSpeed_Ven = (float) parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / pmp_gain;
				}
				else
					actualSpeed_Ven = actualSpeed_Ven + deltaSpeed_Ven;
			}
		}
	}


	if(actualSpeed_Ven <= 0 || pressSample0_Ven > 190)
	{
		actualSpeed_Ven = 0;
		Target_PID_VEN = parameterWordSetFromGUI[PAR_SET_VENOUS_PRESS_TARGET].value + SET_POINT_PRESSURE_INIT;
	}


	if(actualSpeed_Ven != pumpPerist[pmpId].actualSpeedOld)
	{
		setPumpSpeedValueHighLevel(pumpPerist[pmpId].pmpMySlaveAddress, ((int)(actualSpeed_Ven * 100)));
		pumpPerist[pmpId].actualSpeedOld = actualSpeed_Ven;
	}

	pressSample2_Ven = pressSample1_Ven;
	pressSample1_Ven = pressSample0_Ven;
}



