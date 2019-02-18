
#include "PE_Types.h"
#include "Global.h"
//#include "PANIC_BUTTON_INPUT.h"
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

#include "EMERGENCY_BUTTON.h"
#include "FRONTAL_COVER_1.h"
#include "FRONTAL_COVER_2.h"
#include "HOOK_SENSOR_1.h"
#include "HOOK_SENSOR_2.h"

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
#include "general_func.h"

extern struct alarm alarmList[];

void Heater_ON()
{
	unsigned char HeaterStatus =  HEAT_ON_C_GetVal();

	if (!HeaterStatus)
		HEAT_ON_C_SetVal();
}

void Set_Lamp(unsigned char level)
{
	switch (level)
	{
		case LAMP_LEVEL_LOW:
			LAMP_LOW_SetVal();
			break;

		case LAMP_LEVEL_MEDIUM:
			LAMP_MEDIUM_SetVal();
			break;

		case LAMP_LEVEL_HIGH:
			LAMP_HIGH_SetVal();
			break;

		default:
			break;
	}
}

void Reset_Lamp(unsigned char level)
{
	switch (level)
	{
	case LAMP_LEVEL_LOW:
		LAMP_LOW_ClrVal();
		break;

	case LAMP_LEVEL_MEDIUM:
		LAMP_MEDIUM_ClrVal();
		break;

	case LAMP_LEVEL_HIGH:
		LAMP_HIGH_ClrVal();
		break;

	default:
		break;
	}
}



unsigned char Bubble_Keyboard_GetVal(unsigned char Button)
{
	unsigned char button_Value = 0;

		switch (Button)
		{
			case BUTTON_1:
				 button_Value = BUBBLE_KEYBOARD_BUTTON1_GetVal();
				 break;

			case BUTTON_2:
				 button_Value = BUBBLE_KEYBOARD_BUTTON2_GetVal();
				 break;

			case BUTTON_3:
				 button_Value = BUBBLE_KEYBOARD_BUTTON3_GetVal();
				 break;

			case BUTTON_4:
				 button_Value = BUBBLE_KEYBOARD_BUTTON4_GetVal();
				 break;

			default:
				break;
		}
	return (button_Value);
}

void DebugString()
{
	static char stringPr[STR_DBG_LENGHT];
//	sprintf(stringPr, "\r %i; %i; %i; %i; %i; %i; %i; %d;",
//							PR_ADS_FLT_mmHg,
//							PR_VEN_mmHg,
//							PR_ART_mmHg,
//							(int) (sensorIR_TM[0].tempSensValue*10),
//							(int) (sensorIR_TM[1].tempSensValue*10),
//							(int) (sensorIR_TM[2].tempSensValue*10),
//							(int) (sensor_PRx[0].prSensValueFilteredWA),
//							(int) pumpPerist[0].actualSpeed
//				);


//	sprintf(stringPr, "\r %i; %i; %i; %i; %i; %d; %d;",
//							(int) (sensorIR_TM[0].tempSensValue*10)/*PR_ADS_FLT_mmHg*/,
//							(int) (sensorIR_TM[1].tempSensValue*10)/*PR_VEN_mmHg*/,
//							(int) (sensorIR_TM[2].tempSensValue*10),
//							PR_ART_mmHg_Filtered,
//							TotalTreatDuration + TreatDuration,
//							(int) pumpPerist[0].actualSpeed,
//							(int) perfusionParam.priVolPerfArt
//				);

	if((ptrCurrentState->state == STATE_PRIMING_RICIRCOLO) ||
	   (ptrCurrentState->state == STATE_PRIMING_PH_1) ||
	   (ptrCurrentState->state == STATE_PRIMING_PH_2))
	{
		sprintf(stringPr, "\r %i; %i; %i; %i; %x; %d; %d;",
								(int) (sensorIR_TM[0].tempSensValue*10),
								(int) LiquidAmount, //sensor_UFLOW[VENOUS_AIR_SENSOR].bubbleSize,
								(int) (T_PLATE_C_GRADI_CENT * 10.0),  //sensor_UFLOW[ARTERIOUS_AIR_SENSOR].bubbleSize,
								(int) (sensorIR_TM[1].tempSensValue * 10),  // temp. recevoir
								(int) modbusData[2][18] & 0x0200, //alarmList[AIR_PRES_ART].physic,
								(int) pumpPerist[0].actualSpeed,
								(int) perfusionParam.priVolPerfArt
					);
	}
	else if(ptrCurrentState->state == STATE_EMPTY_DISPOSABLE)
	{
		sprintf(stringPr, "\r %i; %i; %i; %i; %i; %d; %d;",
								(int) (sensorIR_TM[0].tempSensValue*10),
								(int) sensor_UFLOW[0].Average_Flow_Val ,
								(int) PR_VEN_Sistolyc_mmHg,      //PR_VEN_mmHg_Filtered,
								(int) PR_ART_Sistolyc_mmHg,     // PR_ART_mmHg_Filtered,
								TotalTreatDuration + TreatDuration,
								(int) pumpPerist[3].actualSpeedOld * 100, //pumpPerist[1].actualSpeed,
								(int) perfusionParam.unlVolRes
					);

	}
	else if((ptrCurrentState->state == STATE_IDLE))
	{
		sprintf(stringPr, "\r %i; %i; %i; %i; %i; %d; %d;",
								(int) (sensorIR_TM[0].tempSensValue*10),
								(int) LiquidAmount, //(sensorIR_TM[1].tempSensValue*10),  // temp. recevoir
								(int) CoversState, //PR_VEN_Sistolyc_mmHg, //PR_VEN_mmHg,            //PR_VEN_Sistolyc_mmHg,      //PR_VEN_mmHg_Filtered,
								(int) modbusData[2][18] & 0x0200, //PR_ART_Sistolyc_mmHg, //PR_ART_mmHg,            //PR_ART_Sistolyc_mmHg,     // PR_ART_mmHg_Filtered,
								TotalTreatDuration + TreatDuration,
								(int) pumpPerist[0].actualSpeedOld * 100, //pumpPerist[1].actualSpeed,
								(int) pumpPerist[1].actualSpeedOld * 100
					);
	}
	else
	{
		sprintf(stringPr, "\r %i; %i; %i; %i; %i; %d; %d;",
								(int) (sensorIR_TM[0].tempSensValue*10),
								(int)(sensorIR_TM[1].tempSensValue*10),  // temp. recevoir
								(int) PR_VEN_Sistolyc_mmHg, //PR_VEN_mmHg,            //PR_VEN_Sistolyc_mmHg,      //PR_VEN_mmHg_Filtered,
								(int) PR_ART_Sistolyc_mmHg, //PR_ART_mmHg,            //PR_ART_Sistolyc_mmHg,     // PR_ART_mmHg_Filtered,
								TotalTreatDuration + TreatDuration,
								(int) pumpPerist[0].actualSpeedOld * 100, //pumpPerist[1].actualSpeed,
								(int) pumpPerist[1].actualSpeedOld * 100
					);

	}




//	sprintf(stringPr, "\r %i; %i; %i; - %i; %i; %d; %d;",
//							(int) (sensorIR_TM[0].tempSensValue*10)/*PR_ADS_FLT_mmHg*/,
//							(int) (sensorIR_TM[1].tempSensValue*10)/*PR_VEN_mmHg*/,
//							(int) (sensor_PRx[0].prSensValueFilteredWA),
//							(int) pumpPerist[0].actualSpeed,
//							(int) perfusionParam.priVolPerfArt,
//							(int) pumpPerist[1].actualSpeed,
//							(int) pumpPerist[2].actualSpeed
//				);

	for(int i=0; i<STR_DBG_LENGHT; i++)
	{
		if(stringPr[i])
			PC_DEBUG_COMM_SendChar(stringPr[i]);
		else
			break;
	}
	PC_DEBUG_COMM_SendChar(0x0A);
}

void DebugStringPID()
{
	static char stringPid[STR_DBG_LENGHT];

//	sprintf(stringPid, "\r %i; %i; %i; %i; %i; %d; %d;",
//							(int)(GlobINTEG * 100),
//							(int)(GlobPROP * 100),
//							(int)(GlobDER * 100),
//							PR_ART_mmHg_Filtered,
//							deltaSpeed,
//							(int) actualSpeed,
//							(int) perfusionParam.priVolPerfArt
//				);


//	sprintf(stringPid, "\r %i; %i; %i; %i; %i; %d; %d;",
//							(int)(GlobINTEG_Ven * 100),
//							(int)(GlobPROP_Ven * 100),
//							(int)(GlobDER_Ven * 100),
//							PR_VEN_mmHg_Filtered,
//							deltaSpeed_Ven,
//							(int) actualSpeed_Ven,
//							(int) perfusionParam.priVolPerfArt
//				);

//	for(int i=0; i<STR_DBG_LENGHT; i++)
//	{
//		if(stringPid[i])
//			PC_DEBUG_COMM_SendChar(stringPid[i]);
//		else
//			break;
//	}
//	PC_DEBUG_COMM_SendChar(0x0A);
}


void DebugStringStr(char *s)
{
	return;

	static char stringPr[STR_DBG_LENGHT];
	if(strstr(s, "ABBANDONA"))
	{
		int ii = 0;
	}
	sprintf(stringPr, "\r %s;\r\n", s);

	for(int i=0; i<STR_DBG_LENGHT; i++)
	{
		if(stringPr[i])
			PC_DEBUG_COMM_SendChar(stringPr[i]);
		else
			break;
	}
	PC_DEBUG_COMM_SendChar(0x0A);
}


// Adr 7..9
void TestPinch(void)
{
	static unsigned char PinchPos = 0;
	static unsigned char Adr = 7;   // BOTTOM_PINCH_ID = 7, LEFT_PINCH_ID = 8, RIGHT_PINCH_ID = 9
	static int Counter = 0;
	static unsigned char state = 0;
	if (Bubble_Keyboard_GetVal(BUTTON_1) && PinchPos != MODBUS_PINCH_POS_CLOSED)
	{
		PinchPos = MODBUS_PINCH_POS_CLOSED;
		//setPinchPosValue (Adr,MODBUS_PINCH_POS_CLOSED);
		setPinchPositionHighLevel(Adr, (int)MODBUS_PINCH_POS_CLOSED);
		Counter = 0;
		state = 0;
	}
	else if (Bubble_Keyboard_GetVal(BUTTON_2) && PinchPos != MODBUS_PINCH_RIGHT_OPEN)
	{
		PinchPos = MODBUS_PINCH_RIGHT_OPEN;
		//setPinchPosValue (Adr,MODBUS_PINCH_RIGHT_OPEN);
		setPinchPositionHighLevel(Adr, (int)MODBUS_PINCH_RIGHT_OPEN);
		Counter = 0;
		state = 0;
	}
	else if (Bubble_Keyboard_GetVal(BUTTON_3) && PinchPos != MODBUS_PINCH_LEFT_OPEN)
	{
		PinchPos = MODBUS_PINCH_LEFT_OPEN;
		//setPinchPosValue (Adr,MODBUS_PINCH_LEFT_OPEN);
		setPinchPositionHighLevel(Adr, (int)MODBUS_PINCH_LEFT_OPEN);
		Counter = 0;
		state = 0;
	}
	else if (Bubble_Keyboard_GetVal(BUTTON_4) && (state == 0))
	{
		Counter++;
		if( Counter > 100)
		{
			Adr++;
			if(Adr == 10)
				Adr = 7;
			Counter = 0;
			state = 1;
		}
	}
	else if (!Bubble_Keyboard_GetVal(BUTTON_4) && (state == 1))
	{
		Counter++;
		if( Counter > 100)
		{
			state = 0;
			Counter = 0;
		}
	}
	else
	{
		Counter = 0;
	}

}




unsigned int PumpAverageCurrent;
unsigned int PumpSpeedVal;
unsigned int PumpStatusVal;

// Test portata doppia pompa Davide CAPPI
//void TestPump(unsigned char Adr)
//{
//	static bool MotorOn = 0;
//
//	if (Bubble_Keyboard_GetVal(BUTTON_1) && !MotorOn)
//	{
//	  /*accendo il motore*/
//	  MotorOn = TRUE;
//	  EN_Motor_Control(ENABLE);
//	  setPumpSpeedValueHighLevel(Adr,7500);
//	}
//	else if (Bubble_Keyboard_GetVal(BUTTON_2) && !MotorOn)
//	{
//	  /*accendo il motore*/
//	  MotorOn = TRUE;
//	  EN_Motor_Control(ENABLE);
//	  setPumpSpeedValueHighLevel(Adr,10000);
//	}
//	else if (Bubble_Keyboard_GetVal(BUTTON_3) && !MotorOn)
//	{
//	  /*accendo il motore*/
//	  MotorOn = TRUE;
//	  EN_Motor_Control(ENABLE);
//	  setPumpSpeedValueHighLevel(Adr,13000);
//	}
//	else if (Bubble_Keyboard_GetVal(BUTTON_4) && MotorOn)
//	{
//	  /*spengo il motore*/
//	  MotorOn = FALSE;
//	  EN_Motor_Control(DISABLE);
//	  setPumpSpeedValueHighLevel(Adr,0);
//	}
//	else
//	{
//		PumpAverageCurrent = modbusData[Adr-2][16];
//		PumpSpeedVal = modbusData[Adr-2][17];
//		PumpStatusVal = modbusData[Adr-2][18];
//		//readPumpSpeedValue(pumpPerist[Adr - 2].pmpMySlaveAddress);
//		//readPumpSpeedValue(Adr - 2);
//	}
//}


// Adr 2..5
//void TestPump(unsigned char Adr)
//{
//	static bool MotorOn = 0;
//
//	if (Bubble_Keyboard_GetVal(BUTTON_1) && !MotorOn)
//	{
//	  /*accendo il motore*/
//	  MotorOn = TRUE;
//	  EN_Motor_Control(ENABLE);
//	  setPumpSpeedValueHighLevel(Adr,2000);
//	}
//	else if (Bubble_Keyboard_GetVal(BUTTON_4) && MotorOn)
//	{
//	  /*spengo il motore*/
//	  MotorOn = FALSE;
//	  EN_Motor_Control(DISABLE);
//	  setPumpSpeedValueHighLevel(Adr,0);
//	}
//	else
//	{
//		PumpAverageCurrent = modbusData[Adr-2][16];
//		PumpSpeedVal = modbusData[Adr-2][17];
//		PumpStatusVal = modbusData[Adr-2][18];
//		//readPumpSpeedValue(pumpPerist[Adr - 2].pmpMySlaveAddress);
//		//readPumpSpeedValue(Adr - 2);
//	}
//}


unsigned char ReadKey1(void)
{
	static int Counter = 0;
	static unsigned char state = 0;
	unsigned char Released = 0;
	unsigned char key;

	key = Bubble_Keyboard_GetVal(BUTTON_1);
	if (key && (state == 0))
	{
		Counter++;
		if( Counter > 100)
		{
			Counter = 0;
			state = 1; // tasto premuto
			Released = 0;
		}
	}
	else if (!key && (state == 1))
	{
		Counter++;
		if( Counter > 100)
		{
			state = 0;  // tasto rilasciato
			Counter = 0;
			Released = 1;
		}
	}
	else
	{
		Counter = 0;
	}
	return Released;
}


unsigned char ReadKey2(void)
{
	static int Counter = 0;
	static unsigned char state = 0;
	unsigned char Released = 0;
	unsigned char key;

	key = Bubble_Keyboard_GetVal(BUTTON_2);
	if (key && (state == 0))
	{
		Counter++;
		if( Counter > 100)
		{
			Counter = 0;
			state = 1; // tasto premuto
			Released = 0;
		}
	}
	else if (!key && (state == 1))
	{
		Counter++;
		if( Counter > 100)
		{
			state = 0;  // tasto rilasciato
			Counter = 0;
			Released = 1;
		}
	}
	else
	{
		Counter = 0;
	}
	return Released;
}

unsigned char ReadKey3(void)
{
	static int Counter = 0;
	static unsigned char state = 0;
	unsigned char Released = 0;
	unsigned char key;

	key = Bubble_Keyboard_GetVal(BUTTON_3);
	if (key && (state == 0))
	{
		Counter++;
		if( Counter > 100)
		{
			Counter = 0;
			state = 1; // tasto premuto
			Released = 0;
		}
	}
	else if (!key && (state == 1))
	{
		Counter++;
		if( Counter > 100)
		{
			state = 0;  // tasto rilasciato
			Counter = 0;
			Released = 1;
		}
	}
	else
	{
		Counter = 0;
	}
	return Released;
}

unsigned char ReadKey4(void)
{
	static int Counter = 0;
	static unsigned char state = 0;
	unsigned char Released = 0;
	unsigned char key;

	key = Bubble_Keyboard_GetVal(BUTTON_4);
	if (key && (state == 0))
	{
		Counter++;
		if( Counter > 100)
		{
			Counter = 0;
			state = 1; // tasto premuto
			Released = 0;
		}
	}
	else if (!key && (state == 1))
	{
		Counter++;
		if( Counter > 100)
		{
			state = 0;  // tasto rilasciato
			Counter = 0;
			Released = 1;
		}
	}
	else
	{
		Counter = 0;
	}
	return Released;
}

unsigned char Released1;
unsigned char Released2;
unsigned char Released3;
unsigned char Released4;

/* creata per debug quando non c'era la GUI
void GenEvntParentPrim(void)
{
	switch(ptrCurrentParent->parent){
		case PARENT_PRIMING_TREAT_KIDNEY_1_INIT:
			if(Released2)
			{
				Released2 = 0;
				if(ptrCurrentParent->action == ACTION_ALWAYS)
				{
					setGUIButton((unsigned char)BUTTON_START_PRIMING);
					parameterWordSetFromGUI[PAR_SET_PRESS_ART_TARGET].value = 60;
					setGUIButton((unsigned char)BUTTON_START_OXYGEN_PUMP);
					// evito che all'entry del parent mi fermi le pompe
					pumpPerist[0].entry = 1;

					if(perfusionParam.priVolPerfArt < GetTotalPrimingVolumePerf(0))
					{
						setGUIButton((unsigned char)BUTTON_CONFIRM);
					}
					else
					{
						DebugStringStr("START POMPA");
					}
				}
			}
			if(Released3)
			{
				setGUIButton((unsigned char)BUTTON_START_PRIMING);
				Released3 = 0;
			}
			break;

		case PARENT_PRIMING_TREAT_KIDNEY_1_RUN:
			if(Released2)
			{
				Released2 = 0;
				if(ptrCurrentParent->action == ACTION_ALWAYS)
				{
					setGUIButton((unsigned char)BUTTON_CONFIRM);
					DebugStringStr("STATE_TREATMENT_KIDNEY_1");
				}
			}
			if(Released3)
			{
				setGUIButton((unsigned char)BUTTON_START_PRIMING);
				Released3 = 0;
			}
			break;

		case PARENT_PRIMING_TREAT_KIDNEY_1_ALARM:
			if(Released3)
			{
				setGUIButton((unsigned char)BUTTON_START_PRIMING);
				Released3 = 0;
			}
			break;

		case PARENT_PRIMING_TREAT_KIDNEY_1_END:
			break;

		default:
			break;
	}
}

*/

/*
void GenEvntParentTreat(void)
{
	switch(ptrCurrentParent->parent){
		case PARENT_TREAT_KIDNEY_1_INIT:
//			if(perfusionParam.treatVolPerfArt >= 200)
//			{
//				// FM faccio partire la pompa
//			}
			if(ptrCurrentParent->action == ACTION_ON_ENTRY)
			{
				DebugStringStr("STATE_TREATMENT_KIDNEY_1___");
			}
			break;

		case PARENT_TREAT_KIDNEY_1_PUMP_ON:
			break;

		case PARENT_TREAT_KIDNEY_1_ALARM:
			break;

		case PARENT_TREAT_KIDNEY_1_END:
			break;

		default:
			break;
	}
}

*/

// viene chiamata ad intervalli di 50 msec
// sequenza di tasti 1,2 della tastiera a bolle fino ad arrivare a STATE_PRIMING_1
//  tasto 2 per lo start della pompa nella fase STATE_PRIMING_1 dopo un po..
//          Con il Tasto 3 faccio ripartire le pompe dopo un allarme
//  tasto 1 per passare a STATE_PRIMING_2 dopo un po..
//  tasto 2 per dare lo start alla pompa in STATE_PRIMING_2 una volta
//          raggiunto il volume richiesto di  50 ml premere di nuovo ..
//          Con il Tasto 3 faccio ripartire le pompe dopo un allarme
//  tasto 2 passo alla fase di trattamento poi ...
//  tasto 1 faccio partire le pompe all'inizio del trattamento
//          Con il Tasto 3 faccio ripartire le pompe dopo un allarme
//          DEVO USARE QUESTO TASTO ANCHE PER FAR RIPARTIRE I MOTORI
//          DOPO UN ALLARME E QUANDO SONO IN TRATTAMENTO
void GenerateSBCComm(void)
{
	static int timerCounterGenSBCComm = 0;

//	switch (ptrCurrentState->state)
//	{
//		case STATE_IDLE:
//			if(Released1)
//			{
//				Released1 = 0;
//				setGUIButton((unsigned char)BUTTON_KIDNEY);
//
//				//---------------------------------------------------------------
//				// IMPOSTO ALCUNI PARAMETRI NECESSARI AL TRATTAMENTO
//				// Imposto un volume molto basso per uscire subito dal priming
//				parameterWordSetFromGUI[PAR_SET_PRIMING_VOL_PERFUSION].value = 5;
//				parameterWordSetFromGUI[PAR_SET_PRESS_ART_TARGET].value = 60;
//				// imposto il volume iniziale (NON SERVE)
//				//perfusionParam.priVolPerfArt = 1300;
//				// 10 minuti di trattamento
//				parameterWordSetFromGUI[PAR_SET_DESIRED_DURATION].value = 600;
//				parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value = 1000;
//				parameterWordSetFromGUI[PAR_SET_MAX_FLOW_PERFUSION].value = 500;
//			}
//			else if(Released2)
//			{
//				setGUIButton((unsigned char)BUTTON_CONFIRM);
//				Released2 = 0;
//				DebugStringStr("STATE_SELECT_TREAT");
//			}
//			break;
//		case STATE_SELECT_TREAT:
//			if(Released1)
//			{
//				setGUIButton((unsigned char)BUTTON_EN_PERFUSION);
//				setGUIButton((unsigned char)BUTTON_EN_OXYGENATION);
//				Released1 = 0;
//			}
//			else if(Released2)
//			{
//				setGUIButton((unsigned char)BUTTON_CONFIRM);
//				Released2 = 0;
//				DebugStringStr("STATE_MOUNTING_DISP");
//			}
//			break;
//		case STATE_MOUNTING_DISP:
//			if(Released1)
//			{
//				setGUIButton((unsigned char)BUTTON_PERF_DISP_MOUNTED);
//				setGUIButton((unsigned char)BUTTON_OXYG_DISP_MOUNTED);
//				Released1 = 0;
//			}
//			else if(Released2)
//			{
//				setGUIButton((unsigned char)BUTTON_CONFIRM);
//				Released2 = 0;
//				DebugStringStr("STATE_TANK_FILL");
//			}
//			break;
//		case STATE_TANK_FILL:
//			if(Released1)
//			{
//				setGUIButton((unsigned char)BUTTON_PERF_TANK_FILL);
//				Released1 = 0;
//			}
//			else if(Released2)
//			{
//				setGUIButton((unsigned char)BUTTON_CONFIRM);
//				Released2 = 0;
//				DebugStringStr("STATE_PRIMING_PH_1");
//			}
//			break;
//		case STATE_PRIMING_PH_1:
//			if(Released1)
//			{
//				setGUIButton((unsigned char)BUTTON_PERF_FILTER_MOUNT);
//				setGUIButton((unsigned char)BUTTON_CONFIRM);
//				DebugStringStr("STATE_PRIMING_PH_2");
//				Released1 = 0;
//			}
//			else if(Released2)
//			{
//				// Questo tasto viene usato in GenEvntParentPrim per far partire la pompa
//				GenEvntParentPrim();
//				Released2 = 0;
//			}
//			else if(Released3)
//			{
//				// Questo tasto viene usato in GenEvntParentPrim per far ripartire la pompa
//				// dopo un allarme
//				GenEvntParentPrim();
//				Released3 = 0;
//			}
//			break;
//		case STATE_PRIMING_PH_2:
//			if(Released1)
//			{
//				setGUIButton((unsigned char)BUTTON_CONFIRM);
//				DebugStringStr("STATE_TREATMENT_KIDNEY_1");
//				Released1 = 0;
//			}
//			else if(Released2)
//			{
//				// Questo tasto viene usato in GenEvntParentPrim per far ripartire la pompa
//				GenEvntParentPrim();
//				Released2 = 0;
//			}
//			else if(Released3)
//			{
//				// Questo tasto viene usato in GenEvntParentPrim per far partire la pompa
//				// dopo un allarme
//				GenEvntParentPrim();
//				Released3 = 0;
//			}
//			break;
//		case STATE_TREATMENT_KIDNEY_1:
//			if(Released1)
//			{
//				// do lo start al trattamento
//				setGUIButton((unsigned char)BUTTON_START_PRIMING);
//				Released1 = 0;
//			}
//			else if(Released2)
//			{
//				// Questo tasto verra' usato per dare lo stop al trattamento
//				Released2 = 0;
//			}
//			break;
//		case STATE_EMPTY_DISPOSABLE_1:
//			break;
//	}

    //  viene chiamata ogni secondo
	if(msTick_elapsed(timerCounterGenSBCComm) >= 20)
	{
		timerCounterGenSBCComm = timerCounterModBus;
	//	DebugString();
	}

	// viene chiamata ogni 50 msec
	//DebugString();

//	// DEBUG visualizzo alcuni dati su seriale di debug ogni secondo
//	if(msTick_elapsed(timerCounterGenSBCComm) >= 20 &&
//
//	  (ptrCurrentState->state != STATE_TREATMENT_KIDNEY_1) )
//	{
//		timerCounterGenSBCComm = timerCounterModBus;
//		DebugString();
//	}
//	else if(msTick_elapsed(timerCounterGenSBCComm) >= 20 &&
//	     (ptrCurrentState->state == STATE_TREATMENT_KIDNEY_1) )
//	{
//		timerCounterGenSBCComm = timerCounterModBus;
//		//DebugStringPID();
//	}
}

void updateDurationToSendSBC(void)
{
	perfusionParam.treatDurPerfArt = (word)(TotalTreatDuration + TreatDuration);   // concordato con Iudici di esprimerlo in secondi
	perfusionParam.treatDurPerVenOxy = perfusionParam.treatDurPerfArt;

	perfusionParam.priDurPerfArt = (word)(TotalPrimingDuration + PrimingDuration);  // concordato con Iudici di esprimerlo in secondi
	perfusionParam.priDurPerVenOxy = 0;

		if(PrimDurUntilOxyStart)
	{
		// la durata comincia quando partono le pompe di ossigenazione
		perfusionParam.priDurPerVenOxy = perfusionParam.priDurPerfArt - PrimDurUntilOxyStart;
	}
}

//parameterWordSetFromGUI[parId].value = value;
//buttonGUITreatment[sbc_rx_data[7]]
void ShowButtonStr(struct buttonGUI *ButtonGuiArray, unsigned char id)
{
	switch (id)
	{
		case BUTTON_PINCH_2WPVF_RIGHT_OPEN:   DebugStringStr("2WPVF_RIGHT_OPEN");  break;
		case BUTTON_PINCH_2WPVF_LEFT_OPEN:    DebugStringStr("2WPVF_LEFT_OPEN");   break;
		case BUTTON_PINCH_2WPVF_BOTH_CLOSED:  DebugStringStr("2WPVF_BOTH_CLOSED"); break;
		case BUTTON_PINCH_2WPVA_RIGHT_OPEN:   DebugStringStr("2WPVA_RIGHT_OPEN");  break;
		case BUTTON_PINCH_2WPVA_LEFT_OPEN:    DebugStringStr("2WPVA_LEFT_OPEN");   break;
		case BUTTON_PINCH_2WPVA_BOTH_CLOSED:  DebugStringStr("2WPVA_BOTH_CLOSED"); break;
		case BUTTON_PINCH_2WPVV_RIGHT_OPEN:   DebugStringStr("2WPVV_RIGHT_OPEN");  break;
		case BUTTON_PINCH_2WPVV_LEFT_OPEN:    DebugStringStr("2WPVV_LEFT_OPEN");   break;
		case BUTTON_PINCH_2WPVV_BOTH_CLOSED:  DebugStringStr("2WPVV_BOTH_CLOSED"); break;
		case BUTTON_CONFIRM:                  DebugStringStr("BUT_CONFIRM");                  break;
		case BUTTON_RESET:                    DebugStringStr("BUT_RESET");                    break;
		case BUTTON_PRIMING_FILT_INS_CONFIRM: DebugStringStr("BUT_PRIMING_FILT_INS_CONFIRM"); break;
		case BUTTON_PRIMING_ABANDON:          DebugStringStr("BUT_PRIMING_ABANDON");          break;
		case BUTTON_START_PRIMING:            DebugStringStr("BUT_START_PRIMING");            break;
		case BUTTON_STOP_PRIMING:             DebugStringStr("BUT_STOP_PRIMING");              break;
		case BUTTON_STOP_ALL_PUMP:            DebugStringStr("BUT_STOP_ALL_PUMP");             break;
		case BUTTON_START_TREATMENT:          DebugStringStr("BUT_START_TREATMENT");           break;
		case BUTTON_STOP_TREATMENT:           DebugStringStr("BUT_STOP_TREATMENT");            break;
		case BUTTON_EN_PERFUSION:             DebugStringStr("BUT_EN_PERFUSION");              break;
		case BUTTON_EN_OXYGENATION:           DebugStringStr("BUT_EN_OXYGENATION");            break;
		case BUTTON_EN_PURIFICATION:         DebugStringStr("BUT_EN_PURIFICATION");            break;
		case BUTTON_START_PERF_PUMP:         DebugStringStr("BUT_START_PERF_PUMP");            break;
		case BUTTON_STOP_PERF_PUMP:          DebugStringStr("BUT_STOP_PERF_PUMP");             break;
		case BUTTON_START_OXYGEN_PUMP:       DebugStringStr("BUT_START_OXYGEN_PUMP");          break;
		case BUTTON_STOP_OXYGEN_PUMP:        DebugStringStr("BUT_STOP_OXYGEN_PUMP");           break;
		case BUTTON_START_PURIF_PUMP:        DebugStringStr("BUT_START_PURIF_PUMP");           break;
		case BUTTON_STOP_PURIF_PUMP:         DebugStringStr("BUT_STOP_PURIF_PUMP");            break;
		case BUTTON_PERF_DISP_MOUNTED:       DebugStringStr("BUT_PERF_DISP_MOUNTED");          break;
		case BUTTON_OXYG_DISP_MOUNTED:       DebugStringStr("BUT_OXYG_DISP_MOUNTED");          break;
		case BUTTON_PERF_TANK_FILL:          DebugStringStr("BUT_PERF_TANK_FILL");             break;
		case BUTTON_PERF_FILTER_MOUNT:       DebugStringStr("BUT_PERF_FILTER_MOUNT");          break;
		case BUTTON_OVERRIDE_ALARM:          DebugStringStr("BUT_OVERRIDE_ALARM");             break;
		case BUTTON_RESET_ALARM:             DebugStringStr("BUT_RESET_ALARM");                break;
		case BUTTON_SILENT_ALARM:            DebugStringStr("BUT_SILENT_ALARM");               break;
		case BUTTON_START_EMPTY_DISPOSABLE:  DebugStringStr("BUT_START_EMPTY_DISPOSABLE");     break;
		case BUTTON_UNMOUNT_END:             DebugStringStr("BUT_UNMOUNT_END");                break;
		case BUTTON_PRIMING_END_CONFIRM:     DebugStringStr("BUTTON_PRIMING_END_CONFIRM");     break;
		case BUTTON_STOP_EMPTY_DISPOSABLE:   DebugStringStr("BUTTON_STOP_EMPTY_DISPOSABLE");   break;
		case BUTTON_END_TREATMENT:           DebugStringStr("BUTTON_END_TREATMENT");           break;
	}
}


void ShowParameterStr(struct parWordSetFromGUI *ParamGuiArray, unsigned char id)
{
	char stringPr[STR_DBG_LENGHT];

	switch (id)
	{
//		case PAR_SET_PRIMING_VOL_PERFUSION = 0x10:
//			break;
		case PAR_SET_THERAPY_TYPE:
			sprintf(stringPr, "\rTherapy=%d", ParamGuiArray[id].value );
			break;
		case PAR_SET_PRIMING_VOL_PURIFICATION:
			sprintf(stringPr, "\rPrimVolPur=%d", ParamGuiArray[id].value );
			break;
//		case PAR_SET_PRIMING_TEMPERATURE_PERFUSION:
//			break;
		case PAR_SET_PRIMING_VOL_OXYGENATION:
			sprintf(stringPr, "\rPrimVolOxyg=%d", ParamGuiArray[id].value );
			break;
//		case PAR_SET_OXYGENATOR_ACTIVE:
//				break;
		case PAR_SET_OXYGENATOR_FLOW:
			sprintf(stringPr, "\rOxygFlow=%d", ParamGuiArray[id].value );
			break;
//		case PAR_SET_DEPURATION_ACTIVE:
//				break;
		case PAR_SET_PRIMING_TEMPERATURE_PERFUSION:
			sprintf(stringPr, "\rTemp=%d", ParamGuiArray[id].value );
			break;
		case PAR_SET_PRESS_ART_TARGET:
			sprintf(stringPr, "\rPressArtTrgt=%d", ParamGuiArray[id].value );
			break;
		case PAR_SET_DESIRED_DURATION:
			sprintf(stringPr, "\rDuration=%d", ParamGuiArray[id].value );
			break;
		case PAR_SET_MAX_FLOW_PERFUSION:
			sprintf(stringPr, "\rMaxFlowPerf=%d", ParamGuiArray[id].value );
			break;
		case PAR_SET_PRESS_VEN_TARGET:
			sprintf(stringPr, "\rPressVenTrgt=%d", ParamGuiArray[id].value );
			break;
		case PAR_SET_PURIF_FLOW_TARGET:
			sprintf(stringPr, "\rPurFlowTrgt=%d", ParamGuiArray[id].value );
			break;
		case PAR_SET_VENOUS_PRESS_TARGET:
			sprintf(stringPr, "\rPressVenTrgt=%d", ParamGuiArray[id].value );
			break;
	}

	for(int i=0; i<STR_DBG_LENGHT; i++)
	{
		if(stringPr[i])
			PC_DEBUG_COMM_SendChar(stringPr[i]);
		else
			break;
	}
	PC_DEBUG_COMM_SendChar(0x0A);
}

/********************************/
/*      CHECK STOP PUMP        */
/********************************/
/*funzione che controlla se ad ogni singola pompa è stato inviato un comando di stop
 * e la stessa sta ancora girando, nel qual caso si reinoltra il comando di stop*/
void CheckStopPump(void)
{
	word speed_PPAF, speed_PPAR, speed_PPV1, speed_PPV2;

	speed_PPV1 = modbusData[2][17];
	speed_PPV2 = modbusData[3][17];

	/*Per le pompe arteriose e filtro differenzio in base l tipo
	 * di terapia in quanto hanno un indirizzo differente*/
	if (GetTherapyType() == LiverTreat)
	{
		speed_PPAF = modbusData[1][17];
		speed_PPAR = modbusData[0][17];
	}
	else if (GetTherapyType() == KidneyTreat)
	{
		speed_PPAF = modbusData[0][17];
		speed_PPAR = modbusData[1][17];
	}

	if (pumpPerist[0].pmpSpeed == 0 &&  speed_PPAF !=0)
		setPumpSpeedValueHighLevel(PPAF, 0);

	if (pumpPerist[3].pmpSpeed == 0 &&  speed_PPAR !=0)
		setPumpSpeedValueHighLevel(PPAR, 0);

	/*le due pompe venose sono al livello logico una sola, quindi se
	 * ad una delle due ho mandato il comando di stop (velocità pari a 0)
	 * questo comando deve essere valido per entrambe quindi se almeno una
	 * delle due mi risponde con una veloictà diversa da zero le rifermo entrambe*/
	if ( (pumpPerist[1].pmpSpeed == 0 || pumpPerist[2].pmpSpeed == 0) && (speed_PPV1 !=0 || speed_PPV2 !=0) )
		setPumpSpeedValueHighLevel(PPV1, 0);

}

void updateMaxTempPlate (void)
{
	/*se mi è arrivato un coando di far partire il
	 * riscaldatore in debug fisso la t massima
	 * di piastra a 50 °C*/
	if (START_HEAT_ON_DEBUG)
	{
		MAX_PLATE_TEMP = 50.0;
		return;
	}
	word T0l = sensorIR_TM[1].tempSensValue * 10;
	word T1l = parameterWordSetFromGUI[PAR_SET_PRIMING_TEMPERATURE_PERFUSION].value + 10; //aggiungo un grado al target
	float Ktp = 0.6;

	MAX_PLATE_TEMP = (T1l - (T0l * Ktp)) / (1-Ktp);

	/*se le pompe anteriori sono entrambe ferme, non conosco
	 * più il valore corretto della tempratura del liquido,
	 * quindi metto la temperatura di piastra = al target + 1 °C
	 */
	if (modbusData[0][17] == 0 && modbusData[1][17] == 0)
		MAX_PLATE_TEMP = T1l / 10;

	MAX_PLATE_TEMP = MAX_PLATE_TEMP / 10;

	/*faccio in modo comunque da non superare 58 ° sulla piastraC*/
	if (MAX_PLATE_TEMP >= 58.0)
		MAX_PLATE_TEMP = 58.0;
}

void updateMinTempPlate (void)
{

	/*se mi è arrivato un comando di far partire il
	 * frigo in debug fisso la t minima
	 * di piastra a -10 °C*/
	if (START_FRIGO_ON_DEBUG)
	{
		MIN_PLATE_TEMP = -10.0;
		return;
	}

	word T0l = sensorIR_TM[1].tempSensValue * 10;
	word T1l = parameterWordSetFromGUI[PAR_SET_PRIMING_TEMPERATURE_PERFUSION].value - 10; //aggiungo un grado al target
	float Ktp = 0.85;

//	/*Se sono in priming do maggiore boost al frigo alzando Ktp*/
//	if (ptrCurrentState->state == STATE_PRIMING_PH_1   ||
//		ptrCurrentState->state == STATE_PRIMING_PH_2   ||
//		ptrCurrentState->state ==  STATE_PRIMING_WAIT  ||
//		ptrCurrentState->state ==  STATE_PRIMING_RICIRCOLO )
//	{
//		Ktp = 0.85;
//	}
//	else
//	{
//		Ktp = 0.7;
//	}

	MIN_PLATE_TEMP = (T1l - (T0l * Ktp)) / (1-Ktp);

	/*se le pompe anteriori sono entrambe ferme, non conosco
	 * più il valore corretto della tempratura del liquido,
	 * quindi metto la temperatura di piastra = al target - 1 °C
	 */
	if (modbusData[0][17] == 0 && modbusData[1][17] == 0)
		MIN_PLATE_TEMP = T1l / 10;

	MIN_PLATE_TEMP = MIN_PLATE_TEMP / 10;

	/*faccio in modo comunque da non andare sotto -10 °C sulla piastra*/
	if (MIN_PLATE_TEMP <= -10.0)
		MIN_PLATE_TEMP = -10.0;
}
