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

float CalcolaPresArt_with_Flow(unsigned char type_flow)
{
	float Gain,Offset, Target_Flow;

	if (type_flow == 0)
		Target_Flow = (float)parameterWordSetFromGUI[PAR_SET_MAX_FLOW_PERFUSION].value;
	else //if (type_flow == 1)
		Target_Flow = (float)sensor_UFLOW[0].Average_Flow_Val;


	if (Target_Flow <= 50.0)
	{
		Gain =  ( ((float) 43.0 - 40.0) / 50);
		Offset = (float) 43.0 - Gain * 50.0;
	}
	else if (Target_Flow > 50.0 && Target_Flow <= 145.0)
	{
		Gain =  ( ((float) 53.0 - 43.0) / (145.0 - 50.0));
		Offset = (float) 53.0 - Gain * 145.0;
	}
	else if (Target_Flow > 145.0 && Target_Flow <= 245.0)
	{
		Gain =  ( ((float) 56.0 - 53.0) / (245.0 - 145.0));
		Offset = (float) 56.0 - Gain * 245.0;
	}
	else if (Target_Flow > 245.0 && Target_Flow <= 340.0)
	{
		Gain = ( ((float) 61.0 - 56.0) / (340.0 - 245.0));
		Offset = (float) 61.0 - Gain * 340.0;
	}
	else if (Target_Flow > 340.0 && Target_Flow<= 445.0)
	{
		Gain = ( ((float) 66.0 - 61.0) / (445.0 - 340.0));
		Offset = (float) 66.0 - Gain * 445.0;
	}
	else if (Target_Flow > 445.0)
	{
		Gain = ( ((float) 69.0 - 66.0) / (545.0 - 445.0) );
		Offset = (float) 69.0 - Gain * 545.0;
	}

	float press = Gain * (Target_Flow) + Offset;
    return press;
}

/*
float CalcolaPresArt(float speed)
{
//  float m = ((float)128.0 - (float)50.0) / ((float)100.0 );
//  float press = m * (speed) + (float)50.0;
	//float m;


	float m = ((float)60.0 - (float)40.0) / ((float)60.0 );


  float press = m * (speed) + (float)40.0;
  return press;
}
*/

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

float kpForTuning = 1.0;
void alwaysPumpPressLoopArt(unsigned char pmpId, unsigned char *PidFirstTime){

	static float deltaSpeed_Art = 0;
	static float actualSpeed_Art = 0;
	float pressSample0_Art = 0;

	float errPress = 0;

	static int Target_PID_ART = 0;
	float Pump_Gain = 0;
	int MAX_ART_RPM_Val;

/* SERVE A FARE IL CALCOLO DEL PUMP_GAIN E NON STIMARLO
 * MA QUESTO CALCOLO ANDREBBE FATTO SOLO QUANDO IL FLUSSO E' GROSSOMODO STABILE
 * PERCHE' IL FLUSSO SI ALLINEA CON UN CERTO RITARDO RISPETTO ALLA PRESSIONE E AL PID
 * SI POTREBBE AFRLO ANCHE OGNI 5 SEC
	if ( (sensor_UFLOW[0].Average_Flow_Val > 0.0) && (actualSpeed_Art > 0.0) )
		Pump_Gain = sensor_UFLOW[0].Average_Flow_Val / actualSpeed_Art;

	if(Pump_Gain == 0 || Pump_Gain > 10)
		Pump_Gain = 9.3; */
	Pump_Gain = ArteriousPumpGainForPid;


	/*calcolo il massimo numnero di giri in funzione del flusso massimo che mi è stato impostato*/
	MAX_ART_RPM_Val = (int)((float)parameterWordSetFromGUI[PAR_SET_MAX_FLOW_PERFUSION].value / Pump_Gain);

	/*blocco al massimo totale ammissibile per non perdere il passo il numero di giri*/
	if (MAX_ART_RPM_Val > MAX_ART_RPM)
		MAX_ART_RPM_Val = MAX_ART_RPM;

    if(*PidFirstTime == PRESS_LOOP_ON)
    {
    	*PidFirstTime = PRESS_LOOP_OFF;
    	actualSpeed_Art = (float)pumpPerist[pmpId].actualSpeed;
    	Target_PID_ART = parameterWordSetFromGUI[PAR_SET_PRESS_ART_TARGET].value + CalcolaPresArt_with_Flow(0);
    }

    //pressSample0_Art = PR_ART_mmHg_Filtered;
    //pressSample0_Art = MedForArteriousPid;
    // cambio il parametro in ingresso al PID dal valore filtrato al valore sistolico di pressione preso all'organo
    // oppure, posso prendere quello misurato direttamente al sensore che e' PR_ART_Sistolyc_mmHg.
    // Ricordarsi che se cambio il valore usato dal pid devo modificare anche quelli in pressSample1_Art e pressSample2_Art
    // alla partenza del pid
    pressSample0_Art = PR_ART_Sistolyc_mmHg;

	errPress = Target_PID_ART - pressSample0_Art;


	/*se la veilocità resta costante ed inferiore alla masisma, sono in equilibrio, provo ad aumentarla per
	 * vedere se trovo un equilibrio andando + forte e avvicindandomi al massimo flusso impostato*/
   if (SpeedCostanteArt((int)actualSpeed_Art) && (actualSpeed_Art <= MAX_ART_RPM_Val))
   {
		actualSpeed_Art += 2.0;
   }

   //deltaSpeed_Art = kpForTuning * errPress;
	deltaSpeed_Art = (((float)parKITC_Art * errPress) -
			         ((float)parKP_Art * (pressSample0_Art - pressSample1_Art)) -
					 ((float)parKD_TC_Art * (pressSample0_Art - 2 * pressSample1_Art + pressSample2_Art)));

	// valutare se mettere il deltaSpeed = 0 nel caso deltaSpeed sia negativo in modo da non far andare actualSpeed a zero troppo in fretta
	// in alternativa il deltaSpeed va considerato solo se è abbastanza negativo
	if((deltaSpeed_Art < -0.01) || (deltaSpeed_Art > 0.01))
	{
		actualSpeed_Art = actualSpeed_Art + deltaSpeed_Art;
	}

	/*se misuro un flusso e ho una velocità >0 e sto misurando uin flusso superiore al limite impostato
	 * aggiorno la velocità al massimo flusso impostato */
	if ((sensor_UFLOW[0].Average_Flow_Val > 0.0) &&
		(sensor_UFLOW[0].Average_Flow_Val > ( parameterWordSetFromGUI[PAR_SET_MAX_FLOW_PERFUSION].value)) &&
		(actualSpeed_Art > 0.0))
	{
		float pmp_gain = sensor_UFLOW[0].Average_Flow_Val / actualSpeed_Art;
		actualSpeed_Art = (float)parameterWordSetFromGUI[PAR_SET_MAX_FLOW_PERFUSION].value / pmp_gain;
	}

	/*se ho velocità negativa o pressione oltre soglia massima, fermo le pompe*/
	if(actualSpeed_Art < 0 || pressSample0_Art > 300)
	{
		actualSpeed_Art = 0;
	//	Target_PID_ART = parameterWordSetFromGUI[PAR_SET_PRESS_ART_TARGET].value ;//+ CalcolaPresArt_with_Flow(0);
	}

	/*vincolo la velocità massima impostata dal pid al massimo valore che non mi fa perdere il passo*/
	if(actualSpeed_Art > (float)MAX_ART_RPM_Val)
		actualSpeed_Art = (float)MAX_ART_RPM_Val;

	/*aggiorno il target ogni 3 secondi se sto decrementando la velocità
	 * altrimenti lo aggiorno subito, altrimenti il pid va troppo veloce per il target*/
	if (timerCounterUpdateTargetPressPidArt > 60)
	{
		timerCounterUpdateTargetPressPidArt = 0;
		Target_PID_ART = parameterWordSetFromGUI[PAR_SET_PRESS_ART_TARGET].value + CalcolaPresArt_with_Flow(0);
	}


	/*aggiorno la velocità se è diversa dalla precedente*/
	if((actualSpeed_Art != pumpPerist[pmpId].actualSpeedOld) || (pumpPerist[pmpId].actualSpeedOld == 0.0))
	{
		setPumpSpeedValueHighLevel(pumpPerist[pmpId].pmpMySlaveAddress, ((int)(actualSpeed_Art * 100)));
		pumpPerist[pmpId].actualSpeedOld = actualSpeed_Art;
	}

	pressSample2_Art = pressSample1_Art;
	pressSample1_Art = pressSample0_Art;
}

//void alwaysPumpPressLoopArt(unsigned char pmpId, unsigned char *PidFirstTime)
//{
//	static float deltaSpeed_Art = 0;
//	static float actualSpeed_Art = 0;
//	static bool StopPid = FALSE;
//	static unsigned long StartTimePidStop = 0;
//
//	float pressSample0_Art = 0;
//
//	float errPress = 0;
//	/*il valore sottostante, indica la massima velocità delle pompe
//	 * sarà da ripristyinare quello in funzione del flusso ovvero:
//	 * (int) (parameterWordSetFromGUI[PAR_SET_MAX_FLOW_PERFUSION].value / (PUMP_ART_GAIN * 2.0));*/
//	int Max_RPM_for_Flow_Max = MAX_ART_RPM;
//	int Speed_Media = 0;
//	static int Somma_Speed = 0;
//	static int Speed_Media_old = 0xfff; /* valore irraggiungibile*/
//
//	static int Count = 0;
//	static int Target_PID = 0;
//
//	int MAX_ART_RPM_Val;
//	MAX_ART_RPM_Val = parameterWordSetFromGUI[PAR_SET_MAX_FLOW_PERFUSION].value / 9.3;
//
//
//    /*funzione col solo proporzionale usata per mandare in oscillazione la pressione
//     * e calcolare il K minimo e il periodo di oscillazione
//     * //	deltaSpeed_Art = parKP_Art * errPress;*/
//
///*************************************************************************************************/
//	// Queste righe di codice possono essere inserite se voglio bloccare il pid per
//// qualche secondo dopo che ho applicato una variazione di velocita' per effetto di SpeedCostanteVen.
//// In questo modo do tempo alle variazioni di avere effetto.
////	if(StopPid && StartTimePidStop)
////	{
////		if(msTick_elapsed(StartTimePidStop) * 50L >= 3000)
////		{
////			StopPid = FALSE;
////		}
////		else
////			return;
////	}
///*************************************************************************************************/
//
//    if(*PidFirstTime == PRESS_LOOP_ON)
//    {
//    	*PidFirstTime = PRESS_LOOP_OFF;
//    	actualSpeed_Art = (float)pumpPerist[pmpId].actualSpeed;
//    	Target_PID = parameterWordSetFromGUI[PAR_SET_PRESS_ART_TARGET].value + CalcolaPresArt(actualSpeed_Art);
//    }
//
//    //pressSample0_Art = PR_VEN_mmHg_Filtered;
//    pressSample0_Art = MedForArteriousPid;
//	errPress = Target_PID - pressSample0_Art;
//
//	   if (errPress > -5  && errPress < 5 )
//	   {
//		   Count ++;
//		   Somma_Speed += actualSpeed_Art;
//
//			if ( Count >= 5)
//			{
//				Speed_Media = Somma_Speed/Count;
//				Speed_Media_old = Speed_Media;
//
//				Target_PID = parameterWordSetFromGUI[PAR_SET_PRESS_ART_TARGET].value  + CalcolaPresArt(Speed_Media);
//				Count = 0;
//				Somma_Speed = 0;
//			}
//	   }
//	   else
//	   {
//		   Count = 0;
//		   Somma_Speed = 0;
//	   }
//
//		//la velocità del messaggio di stato resta costante per 5 secondi && velocità minore del massimo)
//	    // incrementiamo la actual speed di 5 RPM;
//	   if (SpeedCostanteArt((int)actualSpeed_Art) && (actualSpeed_Art <= MAX_ART_RPM_Val))
//	   {
//		   actualSpeed_Art += 5.0;
//		   Target_PID = parameterWordSetFromGUI[PAR_SET_PRESS_ART_TARGET].value  + CalcolaPresArt(actualSpeed_Art);
//		   StopPid = TRUE;
//		   StartTimePidStop = timerCounterModBus;
//	   }
//
//	deltaSpeed_Art = ((parKITC_Art * errPress) -
//			         (parKP_Art * (pressSample0_Art - pressSample1_Art)) -
//					 (parKD_TC_Art * (pressSample0_Art - 2 * pressSample1_Art + pressSample2_Art)));
//
//	// valutare se mettere il deltaSpeed = 0 nel caso deltaSpeed sia negativo in modo da non far andare actualSpeed a zero troppo in fretta
//	// in alternativa il deltaSpeed va considerato solo se è abbastanza negativo
//	if ((sensor_UFLOW[0].Average_Flow_Val != 0.0) &&
//			(sensor_UFLOW[0].Average_Flow_Val > ( parameterWordSetFromGUI[PAR_SET_MAX_FLOW_PERFUSION].value)) &&
//			(actualSpeed_Art != 0.0))
//	{
//		float pmp_gain = sensor_UFLOW[0].Average_Flow_Val / pumpPerist[0].actualSpeed;
//		actualSpeed_Art = (float) parameterWordSetFromGUI[PAR_SET_MAX_FLOW_PERFUSION].value / pmp_gain;
//	}
//
//	if((deltaSpeed_Art < -0.1) || (deltaSpeed_Art > 0.1))
//	{
//		if (deltaSpeed_Art < 0)
//		{
//			actualSpeed_Art = actualSpeed_Art + deltaSpeed_Art;
//		}
//		else
//		{
//			// da ripristinare solo quando siamo sicuri del funzionamento dei flussimetri
////			if ((sensor_UFLOW[0].Average_Flow_Val != 0.0) &&
////				(sensor_UFLOW[0].Average_Flow_Val > ( parameterWordSetFromGUI[PAR_SET_MAX_FLOW_PERFUSION].value)))
////
////			{
////				actualSpeed_Art = actualSpeed_Art; //non aumento la velocità
////			}
////			else
//			if ( (actualSpeed_Art + deltaSpeed_Art  ) > Max_RPM_for_Flow_Max )
//			{
//				actualSpeed_Art = Max_RPM_for_Flow_Max;
//			}
//			else
//			{
//				if(actualSpeed_Art > (float)MAX_ART_RPM_Val)
//					actualSpeed_Art = (float)MAX_ART_RPM_Val;
//				else
//					actualSpeed_Art = actualSpeed_Art + deltaSpeed_Art;
//			}
//		}
//	}
//
//	if((actualSpeed_Art <= 0) || (pressSample0_Art > 80))
//	{
//		actualSpeed_Art = 0;
//		Target_PID = parameterWordSetFromGUI[PAR_SET_PRESS_ART_TARGET].value + CalcolaPresArt(actualSpeed_Art);
//	}
//
//	if(actualSpeed_Art != pumpPerist[pmpId].actualSpeedOld)
//	{
//		setPumpSpeedValueHighLevel(pumpPerist[pmpId].pmpMySlaveAddress, ((int)(actualSpeed_Art * 100)));
//		pumpPerist[pmpId].actualSpeedOld = actualSpeed_Art;
//	}
//
//	pressSample2_Art = pressSample1_Art;
//	pressSample1_Art = pressSample0_Art;
//}


//------------------------------PID PER POMPA ARTERIOSA ORIGINALE----------------------------------------------------------

//
////float parKITC = 0.2;
////float parKP = 1;
////float parKD_TC = 0.8;
////float GlobINTEG;
////float GlobPROP;
////float GlobDER;
////int deltaSpeed = 0;
////int actualSpeed = 0;
//
//void alwaysPumpPressLoop(unsigned char pmpId, unsigned char *PidFirstTime)
//{
//
//	int deltaSpeed = 0;
//	static int actualSpeed = 0;
//	//static int actualSpeedOld = 0;
//	float parKITC = 0.2;
//	float parKP = 1;
//	float parKD_TC = 0.8;
//	float pressSample0 = 0;
//	//static float pressSample1 = 0;  // FM Questi due vanno inizializzati alla pressione corrente prima di far partire il pid
//	//static float pressSample2 = 0;
//	float errPress = 0;
//	int Max_RPM_for_Flow_Max = (int) ( parameterWordSetFromGUI[PAR_SET_MAX_FLOW_PERFUSION].value / PUMP_ART_GAIN);
//
//    if(*PidFirstTime == PRESS_LOOP_ON)
//    {
//    	*PidFirstTime = PRESS_LOOP_OFF;
//    	actualSpeed = (int)pumpPerist[pmpId].actualSpeed;
//    }
//
//	// FM sostituito con il valore in mmHg
//
//    pressSample0 = PR_ART_mmHg_Filtered;
////    /*cambio il parametro in ingresso al PID dal valore filtrato al valore sistolico di pressione*/
////    pressSample0 = PR_ART_Sistolyc_mmHg;
//
//	errPress = parameterWordSetFromGUI[PAR_SET_PRESS_ART_TARGET].value - pressSample0;
//
//	//GlobINTEG = parKITC*errPress;
//	//GlobPROP = parKP*(pressSample0 - pressSample1);
//	//GlobDER = parKD_TC*(pressSample0 - 2*pressSample1 + pressSample2);
//
//	deltaSpeed = (int)((parKITC*errPress) - (parKP*(pressSample0 - pressSample1)) - (parKD_TC*(pressSample0 - 2*pressSample1 + pressSample2)));
//
//	// valutare se mettere il deltaSpeed = 0 nel caso deltaSpeed sia negativo in modo da non far andare actualSpeed a zero troppo in fretta
//	// in alternativa il deltaSpeed va considerato solo se è abbastanza negativo
//	if((deltaSpeed < -2) || (deltaSpeed > 2))
//	{
//		if (deltaSpeed < 0)
//		{
//			actualSpeed = actualSpeed + deltaSpeed;
//		}
//		else
//		{
//			if (sensor_UFLOW[0].Average_Flow_Val > ( parameterWordSetFromGUI[PAR_SET_MAX_FLOW_PERFUSION].value))
//
//			{
//				actualSpeed = actualSpeed; /*non aumento la velocità*/
//			}
//			else if ( (actualSpeed + deltaSpeed  ) > Max_RPM_for_Flow_Max )
//			{
//				actualSpeed = Max_RPM_for_Flow_Max;
//			}
//			else
//			{
//				actualSpeed = actualSpeed + deltaSpeed;
//			}
//		}
//	}
//
////	if(actualSpeed >= 50)
////		actualSpeed = 50;
////	else if(actualSpeed <= 0)
////		actualSpeed = 0;
//
//	if(actualSpeed < 0)
//		actualSpeed = 0;
//
//	if(actualSpeed != pumpPerist[pmpId].actualSpeedOld){
//		//setPumpSpeedValue(pumpPerist[pmpId].pmpMySlaveAddress, (actualSpeed*100));
//		setPumpSpeedValueHighLevel(pumpPerist[pmpId].pmpMySlaveAddress, (actualSpeed*100));
//		pumpPerist[pmpId].actualSpeedOld = actualSpeed;
//	}
//
//	//se provengo da un allarme la pompa è ferma ed il controllo deve ripartire
//	/*if(pumpPerist[pmpId].actualSpeed == 0){
//		actualSpeedOld = 0;
//	}*/
//
//	pressSample2 = pressSample1;
//	pressSample1 = pressSample0;
//
//	//DebugStringPID(); // debug
//}




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



//---------------------------------------------------------------------------------------------------------------
// Pid per perfusione venosa

int get_Set_Point_Pressure(int Speed)
{
	int Presure_Set_Point;

	Presure_Set_Point = GAIN_PRESSURE * Speed + DELTA_PRESSURE;

	return (Presure_Set_Point);
}

float CalcolaPresVen_with_Flow(unsigned char type_flow)
{
	float Gain,Offset, Target_Flow;
	return 0.0;

	if (type_flow == 0)
		Target_Flow = (float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value;
	else //if (type_flow == 1)
		Target_Flow = (float)sensor_UFLOW[1].Average_Flow_Val;

	if (Target_Flow <= 158.0)
	{
		 Gain =  ( ((float) 61.0 - 53.0) / 158);
		 Offset = (float) 61.0 - Gain * 158.0;
	}
	else if (Target_Flow > 158.0 && Target_Flow<= 356.0)
	{
		 Gain =  ( ((float) 71.0 - 61.0) / (356.0 - 158.0));
		 Offset = (float) 71.0 - Gain * 356.0;
	}
	else if (Target_Flow > 356.0 && Target_Flow<= 562.0)
	{
		 Gain =  ( ((float) 87.0 - 71.0) / (562.0 - 356.0));
		 Offset = (float) 87.0 - Gain * 562.0;
	}
	else if (Target_Flow > 562.0 && Target_Flow<= 790.0)
	{
		 Gain = ( ((float) 104.0 - 87.0) / (790.0 - 562.0));
		 Offset = (float) 104.0 - Gain * 790.0;
	}
	else if (Target_Flow > 790.0 && Target_Flow<= 1025.0)
	{
		 Gain = ( ((float) 124.0 - 104.0) / (1025.0 - 790.0));
		 Offset = (float) 124.0 - Gain * 1025.0;
	}
	else if (Target_Flow > 1025.0 && Target_Flow<= 1230.0)
	{
		 Gain = ( ((float) 147.0 - 124.0) / (1230.0 - 1025.0) );
		 Offset = (float) 147.0 - Gain * 1230.0;
	}
	else if (Target_Flow > 1230.0 && Target_Flow<= 1422.0)
	{
		 Gain = ( ( (float) 173.0 - 147.0) / (1422.0 - 1230.0));
		 Offset = (float) 173.0 - Gain * 1422.0;
	}
	else if (Target_Flow > 1422.0 && Target_Flow<= 1614.0)
	{
		 Gain = ( ((float) 200.0 - 173.0) / (1614.0 - 1422.0));
		 Offset = (float) 200.0 - Gain * 1614.0;
	}
	else if (Target_Flow > 1614.0 && Target_Flow<= 1826.0)
	{
		 Gain = ( ((float) 235.0 - 200.0) / (1826.0 - 1614.0));
		 Offset = (float) 235.0 - Gain * 1826.0;
	}
	else if (Target_Flow > 1826.0 && Target_Flow<= 2022.0)
	{
		 Gain = ( ((float) 264.0 - 235.0) / (2022.0 - 1826.0));
		 Offset = (float) 264.0 - Gain * 2022.0;
	}
	else if (Target_Flow > 2022.0)
	{
		 Gain = ( ((float) 279.0 - 264.0) / (2214.0 - 2022));
		 Offset = (float) 279.0 - Gain * 2214.0;
	}

	float press = Gain * (Target_Flow) + Offset;

    return press;

}

/* OLD in data 09-03-2018
float CalcolaPresVen_with_Flow(unsigned char type_flow)
{
	float Gain,Offset, Target_Flow;

	if (type_flow == 0)
		Target_Flow = (float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value;
	else //if (type_flow == 1)
		Target_Flow = (float)sensor_UFLOW[1].Average_Flow_Val;



	if (Target_Flow <= 158.0)
	{
		Gain =  ( ((float) 61.0 - 53.0) / 158);
		Offset = (float) 61.0 - Gain * 158.0;
	}
	else if (Target_Flow > 158.0 && Target_Flow<= 356.0)
	{
		Gain =  ( ((float) 71.0 - 61.0) / (356.0 - 158.0));
		Offset = (float) 71.0 - Gain * 356.0;
	}

	else if (Target_Flow > 356.0 && Target_Flow<= 562.0)
	{
		Gain =  ( ((float) 87.0 - 71.0) / (562.0 - 356.0));
		Offset = (float) 87.0 - Gain * 562.0;
	}
	else if (Target_Flow > 562.0 && Target_Flow<= 790.0)
	{
		Gain = ( ((float) 104.0 - 87.0) / (790.0 - 562.0));
		Offset = (float) 104.0 - Gain * 790.0;
	}
	else if (Target_Flow > 790.0 && Target_Flow<= 1025.0)
	{
		Gain = ( ((float) 124.0 - 104.0) / (1025.0 - 790.0));
		Offset = (float) 124.0 - Gain * 1025.0;
	}
	else if (Target_Flow > 1025.0 && Target_Flow<= 1230.0)
	{
		Gain = ( ((float) 147.0 - 124.0) / (1230.0 - 1025.0) );
		Offset = (float) 147.0 - Gain * 1230.0;
	}
	else if (Target_Flow > 1230.0 && Target_Flow<= 1422.0)
	{
		Gain = ( ( (float) 173.0 - 147.0) / (1422.0 - 1230.0));
		Offset = (float) 173.0 - Gain * 1422.0;
	}
	else if (Target_Flow > 1422.0 && Target_Flow<= 1614.0)
	{
		Gain = ( ((float) 200.0 - 173.0) / (1614.0 - 1422.0));
		Offset = (float) 200.0 - Gain * 1614.0;
	}
	else if (Target_Flow > 1614.0 && Target_Flow<= 1826.0)
	{
		Gain = ( ((float) 235.0 - 200.0) / (1826.0 - 1614.0));
		Offset = (float) 235.0 - Gain * 1826.0;
	}

	else if (Target_Flow > 1826.0 && Target_Flow<= 2022.0)
	{
		Gain = ( ((float) 264.0 - 235.0) / (2022.0 - 1826.0));
		Offset = (float) 264.0 - Gain * 2022.0;
	}
	else if (Target_Flow > 2022.0)
	{
		Gain = ( ((float) 279.0 - 264.0) / (2214.0 - 2022));
		Offset = (float) 279.0 - Gain * 2214.0;
	}

	float press = Gain * (Target_Flow) + Offset;

    return press;
}


float CalcolaPresVen_with_Flow_old()
{
	float Gain,Offset, Target_Flow = (float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value;

	if (Target_Flow <= 190.0)
	{
		Gain =  ( ((float) 55.0 - 43.0) / 190);
		Offset = (float) 55.0 - Gain * 190.0;
	}
	else if (Target_Flow > 190.0 && Target_Flow<= 425.0)
	{
		Gain =  ( ((float) 67.0 - 55.0) / 235.0);
		Offset = (float) 67.0 - Gain * 425.0;
	}
	else if (Target_Flow > 425.0 && Target_Flow<= 660.0)
	{
		Gain =  ( ((float) 89.0 - 67.0) / 235.0);
		Offset = (float) 89.0 - Gain * 660.0;
	}
	else if (Target_Flow > 660.0 && Target_Flow<= 900.0)
	{
		Gain = ( ((float) 111.0 - 89.0) / 240.0);
		Offset = (float) 111.0 - Gain * 900.0;
	}
	else if (Target_Flow > 900.0 && Target_Flow<= 1130.0)
	{
		Gain = ( ((float) 133.0 - 111.0) / 230.0);
		Offset = (float) 133.0 - Gain * 1130.0;
	}
	else if (Target_Flow > 1130.0 && Target_Flow<= 1380.0)
	{
		Gain = ( ((float) 156.0 - 133.0) / 250.0);
		Offset = (float) 156.0 - Gain * 1380.0;
	}
	else if (Target_Flow > 1380.0 && Target_Flow<= 1680.0)
	{
		Gain = ( ( (float) 196.0 - 156.0) / 300.0);
		Offset = (float) 196.0 - Gain * 1680.0;
	}
	else if (Target_Flow > 1680.0 && Target_Flow<= 1930.0)
	{
		Gain = ( ((float) 227.0 - 196.0) / 250.0);
		Offset = (float) 227.0 - Gain * 1930.0;
	}
	else if (Target_Flow > 1930.0 && Target_Flow<= 2150.0)
	{
		Gain = ( ((float) 255.0 - 227.0) / 220.0);
		Offset = (float) 255.0 - Gain * 2150.0;
	}
	else if (Target_Flow > 2150.0 && Target_Flow<= 2350.0)
	{
		Gain = ( ((float) 273.0 - 255.0) / 200.0);
		Offset = (float) 273.0 - Gain * 2350.0;
	}
	else if (Target_Flow > 2350.0)
	{
		Gain = ( ((float) 281.0 - 273.0) / 190.0);
		Offset = (float) 281.0 - Gain * 2540.0;
	}

	float press = Gain * (Target_Flow) + Offset;

    return press;
}
*/

float CalcolaPresVen_with_Speed(float speed)
{
//  float m = ((float)128.0 - (float)50.0) / ((float)100.0 );
//  float press = m * (speed) + (float)50.0;
	float Gain,Offset;

	if (speed <= 10.0)
	{
		Gain =  ( ((float)59.0 - 50.0) / (float)10.0);
		Offset = (float)59.0 - Gain * 10.0;
	}
	else if (speed > 10.0 && speed<= 20.0)
	{
		Gain =  ( ((float)76.0 - 59.0) / (float)10.0);
		Offset = (float)76.0 - Gain * 20.0;
	}
	else if (speed > 20.0 && speed<= 30.0)
	{
		Gain =  ( ((float)95.0 - 76.0) / (float)10.0);
		Offset = (float)95.0 - Gain * 30.0;
	}
	else if (speed > 30.0 && speed<= 40.0)
	{
		Gain =  ( ((float)114.0 - 95.0) / (float)10.0);
		Offset = (float)114.0 - Gain * 40.0;
	}
	else if (speed > 40.0 && speed<= 50.0)
	{
		Gain =  ( ((float)127.0 - 114.0) / (float)10.0);
		Offset = (float)127.0 - Gain * 50.0;
	}
	else if (speed > 50.0 && speed<= 60.0)
	{
		Gain =  ( ((float)170.0 - 127.0) / (float)10.0);
		Offset = (float)170.0 - Gain * 60.0;
	}
	else if (speed > 60.0 && speed<= 70.0)
	{
		Gain =  ( ((float)196.0 - 170.0) / (float)10.0);
		Offset = (float)196.0 - Gain * 70.0;
	}
	else if (speed > 70 && speed<= 80)
	{
		Gain =  ( ((float)222.0 - 196.0) / 10.0);
		Offset = (float)222.0 - Gain * 80.0;
	}
	else if (speed > 80.0 && speed<= 90.0)
	{
		Gain =  ( ((float)247.0 - 222.0) / 10.0);
		Offset = (float)247.0 - Gain * 90.0;
	}
	else if (speed > 90.0 && speed<= 100.0)
	{
		Gain =  ( ((float)266.0 - 247.0) / 10.0);
		Offset = (float)266.0 - Gain * 100.0;
	}
	else if (speed > 100.0 && speed<= 110.0)
	{
		Gain =  ( ((float)276.0 - 266.0) / 10.0);
		Offset = (float)276.0 - Gain * 110.0;
	}

  float press = Gain * (speed) + Offset;

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

// 12_03_2018 ultima versione del pid prima dello spostamento del controllo subito prima dell'organo
//void alwaysPumpPressLoopVen(unsigned char pmpId, unsigned char *PidFirstTime){
//
//
//	static float deltaSpeed_Ven = 0;
//	static float actualSpeed_Ven = 0;
//
//	float pressSample0_Ven = 0;
//
//	float errPress = 0;
//
//	static int Target_PID_VEN = 0;
//	float Pump_Gain = 0;
//	int MAX_OXYG_RPM_Val;
//
//	Pump_Gain = VenousPumpGainForPid;
//
//
//	/*calcolo il massimo numnero di giri in funzione del flusso massimo che mi è stato impostato*/
//	MAX_OXYG_RPM_Val = (int)((float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / Pump_Gain);
//
//	/*blocco al massimo totale ammissibile per non perdere il passo il numero di giri*/
//	if (MAX_OXYG_RPM_Val > MAX_OXYG_RPM)
//		MAX_OXYG_RPM_Val = MAX_OXYG_RPM;
//
//    if(*PidFirstTime == PRESS_LOOP_ON)
//    {
//    	*PidFirstTime = PRESS_LOOP_OFF;
//    	actualSpeed_Ven = (float)pumpPerist[pmpId].actualSpeed;
//    	Target_PID_VEN = parameterWordSetFromGUI[PAR_SET_VENOUS_PRESS_TARGET].value + SET_POINT_PRESSURE_INIT;
//    }
//
//    //pressSample0_Ven = PR_VEN_mmHg_Filtered;
////    pressSample0_Ven = MedForVenousPid;
////    /*cambio il parametro in ingresso al PID dal valore filtrato al valore sistolico di pressione*/
//    pressSample0_Ven = PR_VEN_Sistolyc_mmHg;
//
//	errPress = Target_PID_VEN - pressSample0_Ven;
//
//
//	/*se la veilocità resta costante ed inferiore alla masisma, sono in equilibrio, provo ad aumentarla per
//	 * vedere se trovo un equilibrio andando + forte e avvicindandomi al massimo flusso impostato*/
//   if (SpeedCostanteVen((int)actualSpeed_Ven) && (actualSpeed_Ven <= MAX_OXYG_RPM_Val))
//   {
//		actualSpeed_Ven += 2.0;
//   }
//
//	deltaSpeed_Ven = ((parKITC_Ven * errPress) -
//			         (parKP_Ven * (pressSample0_Ven - pressSample1_Ven)) -
//					 (parKD_TC_Ven * (pressSample0_Ven - 2 * pressSample1_Ven + pressSample2_Ven)));
//
//	// valutare se mettere il deltaSpeed = 0 nel caso deltaSpeed sia negativo in modo da non far andare actualSpeed a zero troppo in fretta
//	// in alternativa il deltaSpeed va considerato solo se è abbastanza negativo
//	if((deltaSpeed_Ven < -0.1) || (deltaSpeed_Ven > 0.1))
//	{
//		actualSpeed_Ven = actualSpeed_Ven + deltaSpeed_Ven;
//	}
//
//	/*se misuro un flusso e ho una velocità > 0 e sto misurando un flusso superiore al limite impostato
//	 * e il delta di velocità del pid non mi fa diminuire la velocità (deltaSpeed_Ven>0)
//	 * aggiorno la velocità al massimo flusso impostato */
//	if ((sensor_UFLOW[1].Average_Flow_Val > 0.0) &&
//			(sensor_UFLOW[1].Average_Flow_Val > ( parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value)) &&
//			(actualSpeed_Ven > 0.0) && deltaSpeed_Ven > 0 )
//
//	{
//		//float pmp_gain = sensor_UFLOW[1].Average_Flow_Val / actualSpeed_Ven;
//		actualSpeed_Ven = (float) parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / Pump_Gain;
//	}
//
//	/*se ho velocità negativa o pressione oltre soglia massima, fermo le pompe*/
//	if(actualSpeed_Ven <= 0 || pressSample0_Ven > 290)
//	{
//		actualSpeed_Ven = 0;
//		//Target_PID_VEN = parameterWordSetFromGUI[PAR_SET_VENOUS_PRESS_TARGET].value + SET_POINT_PRESSURE_INIT;
//	}
//
//	/*vincolo la velocità massima impostata dal pid al massimo valore che non mi fa perdere il passo*/
//	if(actualSpeed_Ven > (float)MAX_OXYG_RPM_Val)
//		actualSpeed_Ven = (float)MAX_OXYG_RPM_Val;
//
//	/*aggiorno il target ogni 3 secondi se sto decrementando la velocità
//	 * altrimenti lo aggiorno subito, altrimenti il pid va troppo veloce per il target*/
//	if (deltaSpeed_Ven > 0)
//	{
//		//Target_PID_VEN = parameterWordSetFromGUI[PAR_SET_VENOUS_PRESS_TARGET].value + CalcolaPresVen_with_Flow(0);
//		timerCounterUpdateTargetPressurePid = 0;
//	}
//
//	else if (timerCounterUpdateTargetPressurePid > 60)
//	{
//		timerCounterUpdateTargetPressurePid = 0;
//		Target_PID_VEN = parameterWordSetFromGUI[PAR_SET_VENOUS_PRESS_TARGET].value + CalcolaPresVen_with_Flow(0);
//	}
//
//	/*aggiorno la velocità se è doiversa dalla precedente*/
//	if(actualSpeed_Ven != pumpPerist[pmpId].actualSpeedOld)
//	{
//		setPumpSpeedValueHighLevel(pumpPerist[pmpId].pmpMySlaveAddress, ((int)(actualSpeed_Ven * 100)));
//		pumpPerist[pmpId].actualSpeedOld = actualSpeed_Ven;
//	}
//
//	pressSample2_Ven = pressSample1_Ven;
//	pressSample1_Ven = pressSample0_Ven;
//}



void alwaysPumpPressLoopVen(unsigned char pmpId, unsigned char *PidFirstTime){

	static float deltaSpeed_Ven = 0;
	static float actualSpeed_Ven = 0;

	float pressSample0_Ven = 0;

	float errPress = 0;
	float fl;

	static int Target_PID_VEN = 0;
	float Pump_Gain = 0;
	int MAX_OXYG_RPM_Val;

	Pump_Gain = VenousPumpGainForPid;


	/*calcolo il massimo numnero di giri in funzione del flusso massimo che mi è stato impostato*/
	MAX_OXYG_RPM_Val = (int)((float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / Pump_Gain);

	/*blocco al massimo totale ammissibile per non perdere il passo il numero di giri*/
	if (MAX_OXYG_RPM_Val > MAX_OXYG_RPM)
		MAX_OXYG_RPM_Val = MAX_OXYG_RPM;

    if(*PidFirstTime == PRESS_LOOP_ON)
    {
    	*PidFirstTime = PRESS_LOOP_OFF;
    	actualSpeed_Ven = (float)pumpPerist[pmpId].actualSpeed;
    	Target_PID_VEN = parameterWordSetFromGUI[PAR_SET_VENOUS_PRESS_TARGET].value + SET_POINT_PRESSURE_INIT;
    }

    //pressSample0_Ven = PR_VEN_mmHg_Filtered;
//    pressSample0_Ven = MedForVenousPid;
//    /*cambio il parametro in ingresso al PID dal valore filtrato al valore sistolico di pressione*/
    pressSample0_Ven = PR_VEN_Sistolyc_mmHg;

	errPress = Target_PID_VEN - pressSample0_Ven;


	/*se la veilocità resta costante ed inferiore alla masisma, sono in equilibrio, provo ad aumentarla per
	 * vedere se trovo un equilibrio andando + forte e avvicindandomi al massimo flusso impostato*/
   if (SpeedCostanteVen((int)actualSpeed_Ven) && (actualSpeed_Ven <= MAX_OXYG_RPM_Val))
   {
		actualSpeed_Ven += 2.0;
   }

	deltaSpeed_Ven = ((parKITC_Ven * errPress) -
			         (parKP_Ven * (pressSample0_Ven - pressSample1_Ven)) -
					 (parKD_TC_Ven * (pressSample0_Ven - 2 * pressSample1_Ven + pressSample2_Ven)));

	/*se misuro un flusso e ho una velocità > 0 e sto misurando un flusso superiore al limite impostato
	 * e il delta di velocità del pid non mi fa diminuire la velocità (deltaSpeed_Ven>0)
	 * aggiorno la velocità al massimo flusso impostato */
	fl = (float) parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value;
	if ((sensor_UFLOW[1].Average_Flow_Val > 0.0) &&
		(sensor_UFLOW[1].Average_Flow_Val > (fl - fl * 10.0 / 100.0)) &&
		(actualSpeed_Ven > 0.0) && (deltaSpeed_Ven > 0))
	{
		deltaSpeed_Ven = 0.0;
	}
	else if((sensor_UFLOW[1].Average_Flow_Val == 0.0) && (pressSample0_Ven < 10.0))
	{
		// se il flusso e' 0 e la pressione e' al di sotto di 10 mmhg
		// forzo una velocita' delle pompe != 0. Altrimenti si ferma in equilibrio a flusso 0;
		deltaSpeed_Ven = 1.0;
	}
//	else if((pressSample0_Ven > 10.0) && (deltaSpeed_Ven > 0))
//	{
//		// se la pressione supera i 10 mmHg e sto accelerando annullo la variazione di velocita
//		deltaSpeed_Ven = 0.0;
//	}

	// valutare se mettere il deltaSpeed = 0 nel caso deltaSpeed sia negativo in modo da non far andare actualSpeed a zero troppo in fretta
	// in alternativa il deltaSpeed va considerato solo se è abbastanza negativo
	if((deltaSpeed_Ven < -0.01) || (deltaSpeed_Ven > 0.01))
	{
		actualSpeed_Ven = actualSpeed_Ven + deltaSpeed_Ven;
	}


	/*se ho velocità negativa o pressione oltre soglia massima, fermo le pompe*/
	if((actualSpeed_Ven <= 0) || (pressSample0_Ven > 10.0))
	{
		actualSpeed_Ven = 0;
	}

	/*vincolo la velocità massima impostata dal pid al massimo valore che non mi fa perdere il passo*/
	if(actualSpeed_Ven > (float)MAX_OXYG_RPM_Val)
		actualSpeed_Ven = (float)MAX_OXYG_RPM_Val;


	/*aggiorno la velocità se è doiversa dalla precedente*/
	if(actualSpeed_Ven != pumpPerist[pmpId].actualSpeedOld)
	{
		setPumpSpeedValueHighLevel(pumpPerist[pmpId].pmpMySlaveAddress, ((int)(actualSpeed_Ven * 100)));
		pumpPerist[pmpId].actualSpeedOld = actualSpeed_Ven;
	}

	pressSample2_Ven = pressSample1_Ven;
	pressSample1_Ven = pressSample0_Ven;
}
