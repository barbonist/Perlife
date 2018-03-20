/*
 * App_Ges.c
 *
 *  Created on: 03/nov/2016
 *      Author: W15
 */

#include "PE_Types.h"
#include "Global.h"
#include "PANIC_BUTTON_INPUT.h"
#include "ModBusCommProt.h"
#include "App_Ges.h"
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
#include "Comm_Sbc.h"

extern float pressSample1_Ven;
extern float pressSample2_Ven;
extern float pressSample1_Art;
extern float pressSample2_Art;
extern word MedForArteriousPid;
extern word MedForVenousPid;


extern float pressSample1;  // FM Questi due vanno inizializzati alla pressione corrente prima di far partire il pid
extern float pressSample2;
extern unsigned char PidFirstTime[];

// tick di inizio del trattamento
unsigned long StartTreatmentTime = 0;
// tick di inizio del priming
unsigned long StartPrimingTime = 0;

// ferma il conteggio del tempo durante il trattamento
unsigned char StopTreatmentTime;

float volumePriming = 0;
float volumeTreatArt = 0;
// FALSE il filtro non e' ancora stato montato
// TRUE il filtro e' stato montato
bool FilterSelected = FALSE;

// ultimo valore della velocita' di ossigenazione impostata.
// viene inizializzata nella initSetParamInSourceCode
word LastOxygenationSpeed;

// serve per dare lo start alla depurazione ed ossigenazione dopo che e' stato
// scaricato un po di materiale nel reservoir.
// 0 = materiale non scaricato
// 1 = materiale scaricato asufficienza per far partire le altre pompe
unsigned char StartOxygAndDepState = 0;

bool AlarmInPrimingEntered = FALSE;

// quando e' true indica che si e' verificato un allarme durante la fase di ricircolo per cui mi
// sono dovuto fermare. Alla ripartenza devo decidere se ripartire in alta velocita' o no.
bool AlarmOrStopInRecircFlag = FALSE;


void CallInIdleState(void)
{
	memset(ParamRcvdInMounting, 0, sizeof(ParamRcvdInMounting));
	AllParametersReceived = 0;
	TherapyCmdArrived = 0;

	// inizializzo tutte  i guard ed i guibutton nel caso qualcuno rimanesse impostato
	initAllGuard();
	initGUIButton();

	// inserisco un valore di terapia indefinita per costringe sbc ad inviare una nuova terapia prima di ripartire per un secondo trattamento
	// non faccio la init perche' verra' fatta dopo che avro' selezionato la nuova terapia
	SetTherapyType(Undef);

	// inizializzo a 0 il volume accumulato per il priming
	volumeTreatArt = 0;
	perfusionParam.treatVolPerfArt = 0;
	// inizializzo a 0 il volume accumulato durante il trattamento
	volumePriming = 0;
	perfusionParam.priVolPerfArt = 0;
	perfusionParam.priVolPerfVenOxy = 0;
	perfusionParam.treatVolPerfVenOxy = 0;

	TotalTreatDuration = 0;
	StartTreatmentTime = 0;
	TreatDuration = 0;
	FilterSelected = FALSE;
	TotalPrimingDuration = 0;
	StartPrimingTime = 0;
	PrimingDuration = 0;

	// inizializza il target di pressione venosa necessaria al PID per lavorare
	// parameterWordSetFromGUI[PAR_SET_VENOUS_PRESS_TARGET].value = SET_POINT_PRESSURE_INIT;
	// ed parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value = 2000;
	initSetParamInSourceCode();
	LastOxygenationSpeed = parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value;
	StartOxygAndDepState = 0;
	AlarmInPrimingEntered = FALSE;
	AlarmOrStopInRecircFlag = FALSE;

	// reset del totale del volume accumulato
	GetTotalPrimingVolumePerf((int)RESET_CMD_TOT_PRIM_VOL);
	TemperatureStateMach(TEMP_STATE_RESET);

//	GlobalFlags.FlagsDef.TankLevelHigh = 0;

	// abilito tutti gli allarmi previsti
	GlobalFlags.FlagsDef.EnableAllAlarms = 1;
	SetAllAlarmEnableFlags();

	if (PeltierOn && (peltierCell.StopEnable == 0))
	{
		// se erano accese le spengo
		peltierCell.StopEnable = 1;
		peltierCell2.StopEnable = 1;
	}
	LevelBuzzer = 0;
}






/********************************/
/* general purpose function     */
/********************************/

void manageNull(void)
{
	#ifdef DEBUG_TREATMENT
	//currentGuard[GUARD_START_ENABLE].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
	//currentGuard[GUARD_HW_T1T_DONE].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
	//currentGuard[GUARD_COMM_ENABLED].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
	#endif

	int x = 0;
}

void manageStateLevel(void)
{

}

/************************/
/* STATE LEVEL FUNCTION */
/************************/

/*-----------------------------------------------------------*/
/* This function manages the state entry activity 			 */
/*-----------------------------------------------------------*/
void manageStateEntry(void)
{
	releaseGUIButton(BUTTON_CONFIRM);

	#ifdef DEBUG_ENABLE
	//Bit1_NegVal();
	#endif
}

void manageStateEntryAlways(void)
{
	computeMachineStateGuardEntryState();

	#ifdef DEBUG_ENABLE
	//Bit2_NegVal();
	#endif
}

/*-----------------------------------------------------------*/
/* This function manages the state idle activity 			 */
/*-----------------------------------------------------------*/
void manageStateIdle(void)
{
	// inizializzo a 0 il numero di parametri ricevuti durante la fase di montaggio
	CallInIdleState();

	releaseGUIButton(BUTTON_CONFIRM);
}

void manageStateIdleAlways(void)
{
	computeMachineStateGuardIdle();
}

/*-----------------------------------------------------------*/
/* This function manages the state select treatment activity */
/*-----------------------------------------------------------*/
void manageStateSelTreat(void)
{
	releaseGUIButton(BUTTON_CONFIRM);

	//-----------------------------------------------------------------------------
	// TODO Questo blocco di codice viene usato solo in debug per fare saltare lo
	// stato STATE_SELECT_TREAT (non so se servira' piu')
//	setGUIButton((unsigned char)BUTTON_EN_PERFUSION);
//	setGUIButton((unsigned char)BUTTON_EN_OXYGENATION);
//	setGUIButton((unsigned char)BUTTON_CONFIRM);
//	DebugStringStr("STATE_MOUNTING_DISP");
	//-----------------------------------------------------------------------------
}

void manageStateSelTreatAlways(void)
{
	computeMachineStateGuardSelTreat();

	#ifdef DEBUG_ENABLE

	#endif
}

/*--------------------------------------------------------------*/
/* This function manages the state mounting disposable activity */
/*--------------------------------------------------------------*/
void manageStateMountDisp(void)
{
	//releaseGUIButton(BUTTON_CONFIRM);
}


void HandlePinch( int cmd)
{
	switch (cmd)
	{
		case BUTTON_PINCH_2WPVF_RIGHT_OPEN:   //= 0xA0,   // pinch filter (pinch in basso aperto a destra)
			setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_RIGHT_OPEN);
			break;
		case BUTTON_PINCH_2WPVF_LEFT_OPEN:    //= 0xA1,    // pinch filter (pinch in basso aperto a sinistra)
			setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_LEFT_OPEN);
			break;
		case BUTTON_PINCH_2WPVF_BOTH_CLOSED:  //= 0xA2,  // pinch filter (pinch in basso entrambi chiusi)
			setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_POS_CLOSED);
			break;

		case BUTTON_PINCH_2WPVA_RIGHT_OPEN:   // = 0xA3,   // pinch arterial (pinch di sinistra - aperto a destra)
			setPinchPositionHighLevel(PINCH_2WPVA, MODBUS_PINCH_RIGHT_OPEN);
			break;
		case BUTTON_PINCH_2WPVA_LEFT_OPEN:    // = 0xA4,    // pinch arterial (pinch di sinistra - aperto a sinistra)
			setPinchPositionHighLevel(PINCH_2WPVA, MODBUS_PINCH_LEFT_OPEN);
			break;
		case BUTTON_PINCH_2WPVA_BOTH_CLOSED:  // = 0xA5,  // pinch arterial (pinch di sinistra - entrambi chiusi)
			setPinchPositionHighLevel(PINCH_2WPVA, MODBUS_PINCH_POS_CLOSED);
			break;

		case BUTTON_PINCH_2WPVV_RIGHT_OPEN:   // = 0xA6,   // pinch venous (pinch di destra - aperto a destra)
			setPinchPositionHighLevel(PINCH_2WPVV, MODBUS_PINCH_RIGHT_OPEN);
			break;
		case BUTTON_PINCH_2WPVV_LEFT_OPEN:    // = 0xA7,    // pinch venous (pinch di destra - aperto a sinistra)
			setPinchPositionHighLevel(PINCH_2WPVV, MODBUS_PINCH_LEFT_OPEN);
			break;
		case BUTTON_PINCH_2WPVV_BOTH_CLOSED:  // = 0xA8,  // pinch venous (pinch di destra - entrambi chiusi)
			setPinchPositionHighLevel(PINCH_2WPVV, MODBUS_PINCH_POS_CLOSED);
			break;
	}
}

void manageStateMountDispAlways(void)
{
	// questa funzione non viene piu' usata
	//computeMachineStateGuardMountDisp();

	// numero di parametri ricevuti durante la fase di montaggio
	if(ParamRcvdInMounting[0] && ParamRcvdInMounting[1] && ParamRcvdInMounting[2] && ParamRcvdInMounting[3] )
	{
		// ho ricevuto tutti i parametri che dovevo ricevere, invio il comando che mi fa iniziare il priming
		AllParametersReceived = 1;
	}
	else if(buttonGUITreatment[BUTTON_PINCH_2WPVF_RIGHT_OPEN].state == GUI_BUTTON_RELEASED)
	{
		// pinch filter (pinch in basso aperto a destra)
		releaseGUIButton(BUTTON_PINCH_2WPVF_RIGHT_OPEN);
		HandlePinch(BUTTON_PINCH_2WPVF_RIGHT_OPEN);
	}
	else if(buttonGUITreatment[BUTTON_PINCH_2WPVF_LEFT_OPEN].state == GUI_BUTTON_RELEASED)
	{
		// pinch filter (pinch in basso aperto a sinistra)
		releaseGUIButton(BUTTON_PINCH_2WPVF_LEFT_OPEN);
		HandlePinch(BUTTON_PINCH_2WPVF_LEFT_OPEN);
	}
	else if(buttonGUITreatment[BUTTON_PINCH_2WPVF_BOTH_CLOSED].state == GUI_BUTTON_RELEASED)
	{
		// pinch filter (pinch in basso entrambi chiusi)
		releaseGUIButton(BUTTON_PINCH_2WPVF_BOTH_CLOSED);
		HandlePinch(BUTTON_PINCH_2WPVF_BOTH_CLOSED);
	}
	else if(buttonGUITreatment[BUTTON_PINCH_2WPVA_RIGHT_OPEN].state == GUI_BUTTON_RELEASED)
	{
		// pinch arterial (pinch di sinistra - aperto a destra)
		releaseGUIButton(BUTTON_PINCH_2WPVA_RIGHT_OPEN);
		HandlePinch(BUTTON_PINCH_2WPVA_RIGHT_OPEN);
	}
	else if(buttonGUITreatment[BUTTON_PINCH_2WPVA_LEFT_OPEN].state == GUI_BUTTON_RELEASED)
	{
		// pinch arterial (pinch di sinistra - aperto a sinistra)
		releaseGUIButton(BUTTON_PINCH_2WPVA_LEFT_OPEN);
		HandlePinch(BUTTON_PINCH_2WPVA_LEFT_OPEN);
	}
	else if(buttonGUITreatment[BUTTON_PINCH_2WPVA_BOTH_CLOSED].state == GUI_BUTTON_RELEASED)
	{
		// pinch arterial (pinch di sinistra - entrambi chiusi)
		releaseGUIButton(BUTTON_PINCH_2WPVA_BOTH_CLOSED);
		HandlePinch(BUTTON_PINCH_2WPVA_BOTH_CLOSED);
	}
	else if(buttonGUITreatment[BUTTON_PINCH_2WPVV_RIGHT_OPEN].state == GUI_BUTTON_RELEASED)
	{
		// pinch venous (pinch di destra - aperto a destra)
		releaseGUIButton(BUTTON_PINCH_2WPVV_RIGHT_OPEN);
		HandlePinch(BUTTON_PINCH_2WPVV_RIGHT_OPEN);
	}
	else if(buttonGUITreatment[BUTTON_PINCH_2WPVV_LEFT_OPEN].state == GUI_BUTTON_RELEASED)
	{
		// pinch venous (pinch di destra - aperto a sinistra)
		releaseGUIButton(BUTTON_PINCH_2WPVV_LEFT_OPEN);
		HandlePinch(BUTTON_PINCH_2WPVV_LEFT_OPEN);
	}
	else if(buttonGUITreatment[BUTTON_PINCH_2WPVV_BOTH_CLOSED].state == GUI_BUTTON_RELEASED)
	{
		// pinch venous (pinch di destra - aperto a sinistra)
		releaseGUIButton(BUTTON_PINCH_2WPVV_BOTH_CLOSED);
		HandlePinch(BUTTON_PINCH_2WPVV_BOTH_CLOSED);
	}

	if(AllParametersReceived && (buttonGUITreatment[BUTTON_START_PRIMING].state == GUI_BUTTON_RELEASED))
	{
		releaseGUIButton(BUTTON_START_PRIMING);
		AllParametersReceived = 0;
		//currentGuard[GUARD_ENABLE_TANK_FILL].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;

		// passo alla fase di riempimento fino al 95%
		currentGuard[GUARD_ENABLE_PRIMING_PHASE_1].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;

		if (!PeltierOn && (peltierCell.readAlwaysEnable == 1))
		{
			// do il comando di start alle peltier
			peltierCell.readAlwaysEnable = 0;
			peltierCell2.readAlwaysEnable = 0;
		}
	}

	// se in questa fase ricevo un nuovo valore di flusso nella linea di ossigenazione lo prendo
	// come valore attuale.
	LastOxygenationSpeed = parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value;
}

/*--------------------------------------------------------------*/
/* This function manages the state tank fill activity           */
/*--------------------------------------------------------------*/
void manageStateTankFill(void)
{
	//releaseGUIButton(BUTTON_CONFIRM);

	if(((PARAMETER_ACTIVE_TYPE)parameterWordSetFromGUI[PAR_SET_DEPURATION_ACTIVE].value) == YES)
	{
		// FORZO IL PASSAGGIO IMMEDIATO ALLO STATO GUARD_ENABLE_PRIMING_PHASE_1 PERCHE' ORA LO
		// TANK_FILL NON SERVE PIU', MA LO LASCIO PER OGNI EVENIENZA
		currentGuard[GUARD_ENABLE_PRIMING_PHASE_1].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		// forzo anche uno start priming per il nuovo stato, dato che lo avevo gia' ricevuto
		// prima
		setGUIButton(BUTTON_START_PRIMING);
	}
	else if(((PARAMETER_ACTIVE_TYPE)parameterWordSetFromGUI[PAR_SET_DEPURATION_ACTIVE].value) == NO)
	{
		// vado direttamente alla fase di priming_ph2

		// FORZO IL PASSAGGIO IMMEDIATO ALLO STATO GUARD_ENABLE_PRIMING_PHASE_2 PERCHE' ORA LO
		// TANK_FILL NON SERVE PIU', MA LO LASCIO PER OGNI EVENIENZA
		currentGuard[GUARD_ENABLE_PRIMING_PHASE_2].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		// forzo anche uno start priming per il nuovo stato, dato che lo avevo gia' ricevuto
		// prima
		setGUIButton(BUTTON_START_PRIMING);
	}
	else
	{
		// non si dovrebbe mai verificare perche' il controllo viene fatto nello stato precedente
	}
}

void manageStateTankFillAlways(void)
{
	// QUESTO STATO NON VIENE PIU' USATO
	//QUESTA FUNZIONE NON DEVE FARE PIU' NIENTE DATO CHE LO STATO TANK_FILL ORA E' SOLO UNO STATO DI PASSAGGIO
	computeMachineStateGuardTankFill();
}

/*--------------------------------------------------------------*/
/* This function manages the state priming phase 1 activity     */
/*--------------------------------------------------------------*/

// Per ora il controllo sulla temperatura viene fatto solo in base al valore impostato dall'utente.
// Nella versione finale il controllo dovra' essere piu' articolato perche' perche' dovra' tenere conto
// degli allarmi e della temperatura effettivamente raggiunta dal liquido
void CheckTemperatureSet(void)
{
	//-------------------------------------------------------------------
	// Questo codice era nello stato ..TANK_FILL, lo ho spostato qui perche' non passo
	// piu' per quello stato
	static float myTempValue = 20;

	if(myTempValue != parameterWordSetFromGUI[PAR_SET_PRIMING_TEMPERATURE_PERFUSION].value)
	{
	   myTempValue = parameterWordSetFromGUI[PAR_SET_PRIMING_TEMPERATURE_PERFUSION].value;

		if(myTempValue == 40)
		{
			peltierCell.mySet  = (float) myTempValue/10 - 8;
			peltierCell2.mySet = (float) myTempValue/10 - 8;
			peltierCell.readAlwaysEnable = 0;
			peltierCell2.readAlwaysEnable = 0;
		}
		else if(myTempValue == 360)
		{
		//	peltierCell.mySet  = (float) myTempValue/10 + 6;
		//	peltierCell2.mySet = (float) myTempValue/10 + 6;
		//peltierCell.mySet  = (float) myTempValue/10 + 19;
		//	peltierCell2.mySet = (float) myTempValue/10 + 19;
		//	peltierCell.mySet  = (float) myTempValue/10 + 6;
		//	peltierCell2.mySet = (float) myTempValue/10 + 6;
			peltierCell.mySet  = (float) 62.0;
			peltierCell2.mySet = (float) 62.0;
			peltierCell.readAlwaysEnable = 0;
			peltierCell2.readAlwaysEnable = 0;
		}
		else
		{
			peltierCell.mySet  = 20;
			peltierCell2.mySet = 20;
			peltierCell.readAlwaysEnable = 0;
			peltierCell2.readAlwaysEnable = 0;
		}
	}
	//-----------------------------------------------------------------------
}

void managePrimingPh1(void)
{
	releaseGUIButton(BUTTON_CONFIRM);

	timerCounter = 0;

	pumpPerist[0].entry = 0;
	pumpPerist[1].entry = 0;
	pumpPerist[2].entry = 0;
	pumpPerist[3].entry = 0;
}

void managePrimingPh1Always(void)
{
	//guard macchina a stati (controllo quando arriva il segnale per i passaggio alla fase 2)
	computeMachineStateGuardPrimingPh1();
}

/*--------------------------------------------------------------*/
/* This function manages the state priming phase 2 activity     */
/*--------------------------------------------------------------*/
void managePrimingPh2(void)
{
	pumpPerist[0].entry = 0;
	pumpPerist[1].entry = 0;
	pumpPerist[2].entry = 0;
	pumpPerist[3].entry = 0;
}

void managePrimingPh2Always(void)
{
	computeMachineStateGuardPrimingPh2();
}

/*-----------------------------------------------------------*/
/* This function manages the state treatment 1 activity      */
/*-----------------------------------------------------------*/
void manageStateTreatKidney1(void)
{
	releaseGUIButton(BUTTON_CONFIRM);

	pumpPerist[0].entry = 0;
	pumpPerist[1].entry = 0;
	pumpPerist[2].entry = 0;
	pumpPerist[3].entry = 0;

	// confermo il fermo di tutte le pompe, gia' inviato nella fase
	// di priming_2 - run
	setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
	setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
	if(GetTherapyType() == LiverTreat)
	{
		// se sono nel trattamento fegato fermo anche l'altro motore !!
		setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
	}

}

void manageStateTreatKidney1Always(void)
{
	computeMachineStateGuardTreatment();
}

/*-----------------------------------------------------------*/
/* This function manages the state empty disposable activity */
/* Entrata nello stato di svutamento del disposable          */
/*-----------------------------------------------------------*/

void manageStateEmptyDisp(void)
{
}

void manageStateEmptyDispAlways(void)
{
	ParentEmptyDispStateMach();
}

//------------------------------------------------------------------
void manageStatePrimingWait(void){
}

// le pompe sono ferme, aspetto un nuovo volume per continuare priming_ph2 oppure
// un BUTTON_PRIMING_END_CONFIRM per passare alla fase di ricircolo per riscaldamento o raffreddamento
void manageStatePrimingWaitAlways(void){

	if(buttonGUITreatment[BUTTON_PRIMING_END_CONFIRM].state == GUI_BUTTON_RELEASED)
	{
		// vado nello stato di ricircolo
		releaseGUIButton(BUTTON_PRIMING_END_CONFIRM);
		currentGuard[GUARD_PRIMING_END].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		TemperatureStateMach(TEMP_START_RICIRCOLO);
	}
	else if(buttonGUITreatment[BUTTON_PRIMING_ABANDON].state == GUI_BUTTON_RELEASED)
	{
		releaseGUIButton(BUTTON_PRIMING_ABANDON);
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
		if(GetTherapyType() == LiverTreat)
		{
			// se sono nel trattamento fegato fermo anche l'altro motore !!
			setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
		}
		currentGuard[GUARD_ABANDON_PRIMING].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
	}
	else if(GetTotalPrimingVolumePerf(0) > perfusionParam.priVolPerfArt) // parameterWordSetFromGUI[PAR_SET_PRIMING_VOL_PERFUSION].value
	{
		// dopo che era finito il priming precedente mi e' stato inviato un nuovo volume
		// quindi, devo ritornare nello stato di priming_ph2 perche' devo aggiungere altro liquido
		currentGuard[GUARD_ENABLE_PRIMING_PHASE_2].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
	}
}

//------------------------------------------------------------------
void manageStatePrimingRicircolo(void)
{
}

void manageStatePrimingRicircoloAlways(void)
{
//	float tmpr;
//	word tmpr_trgt;
//	// temperatura raggiunta dal reservoir
//	tmpr = ((int)(sensorIR_TM[0].tempSensValue*10));
//	// temperatura da raggiungere moltiplicata per 10
//	tmpr_trgt = parameterWordSetFromGUI[PAR_SET_PRIMING_TEMPERATURE_PERFUSION].value;

	if(buttonGUITreatment[BUTTON_PRIMING_ABANDON].state == GUI_BUTTON_RELEASED)
	{
		releaseGUIButton(BUTTON_PRIMING_ABANDON);
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
		if(GetTherapyType() == LiverTreat)
		{
			// se sono nel trattamento fegato fermo anche l'altro motore !!
			setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
		}
		currentGuard[GUARD_ABANDON_PRIMING].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
	}
	else
	{
		// controllo se e' cambiata la velocita' della pompa di ossigenazione e la aggiorno
		CheckOxygenationSpeed(LastOxygenationSpeed);
	}

//	// temperatura del reservoir
//	else if((tmpr >= (float)(tmpr_trgt - 10)) && (tmpr <= (float)(tmpr_trgt + 10)))
//	{
//		// ho raggiunto la temperatura ( + o - un grado)richiesta posso passare nello stato di
//		// attesa ricezione comando di start trattamento
//		currentGuard[GUARD_ENABLE_WAIT_TREATMENT].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
//	}
}


//------------------------------------------------------------------

void manageStateWaitTreatment(void){
}

void manageStateWaitTreatmentAlways(void)
{
	// aspetto che mi arrivi il comando per passare in trattamento oppure abbandono
	if(buttonGUITreatment[BUTTON_PRIMING_ABANDON].state == GUI_BUTTON_RELEASED)
	{
		releaseGUIButton(BUTTON_PRIMING_ABANDON);
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
		if(GetTherapyType() == LiverTreat)
		{
			// se sono nel trattamento fegato fermo anche l'altro motore !!
			setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
		}
		currentGuard[GUARD_ABANDON_PRIMING].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
	}
//	else if(buttonGUITreatment[BUTTON_START_TREATMENT].state == GUI_BUTTON_RELEASED)
//	{
//		// e' arrivato il comando di partenza del trattamento uso BUTTON_START_PRIMING
//		// perche' mi fa partire subito le pompe
//		releaseGUIButton(BUTTON_START_TREATMENT);
//		currentGuard[GUARD_ENABLE_TREATMENT_KIDNEY_1].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
//	}
}


void manageStateUnmountDisposableEntry(void)
{
}
void manageStateUnmountDisposableAlways(void)
{
	if(buttonGUITreatment[BUTTON_PINCH_2WPVF_RIGHT_OPEN].state == GUI_BUTTON_RELEASED)
	{
		// pinch filter (pinch in basso aperto a destra)
		releaseGUIButton(BUTTON_PINCH_2WPVF_RIGHT_OPEN);
		HandlePinch(BUTTON_PINCH_2WPVF_RIGHT_OPEN);
	}
	else if(buttonGUITreatment[BUTTON_PINCH_2WPVF_LEFT_OPEN].state == GUI_BUTTON_RELEASED)
	{
		// pinch filter (pinch in basso aperto a sinistra)
		releaseGUIButton(BUTTON_PINCH_2WPVF_LEFT_OPEN);
		HandlePinch(BUTTON_PINCH_2WPVF_LEFT_OPEN);
	}
	else if(buttonGUITreatment[BUTTON_PINCH_2WPVF_BOTH_CLOSED].state == GUI_BUTTON_RELEASED)
	{
		// pinch filter (pinch in basso entrambi chiusi)
		releaseGUIButton(BUTTON_PINCH_2WPVF_BOTH_CLOSED);
		HandlePinch(BUTTON_PINCH_2WPVF_BOTH_CLOSED);
	}
	else if(buttonGUITreatment[BUTTON_PINCH_2WPVA_RIGHT_OPEN].state == GUI_BUTTON_RELEASED)
	{
		// pinch arterial (pinch di sinistra - aperto a destra)
		releaseGUIButton(BUTTON_PINCH_2WPVA_RIGHT_OPEN);
		HandlePinch(BUTTON_PINCH_2WPVA_RIGHT_OPEN);
	}
	else if(buttonGUITreatment[BUTTON_PINCH_2WPVA_LEFT_OPEN].state == GUI_BUTTON_RELEASED)
	{
		// pinch arterial (pinch di sinistra - aperto a sinistra)
		releaseGUIButton(BUTTON_PINCH_2WPVA_LEFT_OPEN);
		HandlePinch(BUTTON_PINCH_2WPVA_LEFT_OPEN);
	}
	else if(buttonGUITreatment[BUTTON_PINCH_2WPVA_BOTH_CLOSED].state == GUI_BUTTON_RELEASED)
	{
		// pinch arterial (pinch di sinistra - entrambi chiusi)
		releaseGUIButton(BUTTON_PINCH_2WPVA_BOTH_CLOSED);
		HandlePinch(BUTTON_PINCH_2WPVA_BOTH_CLOSED);
	}
	else if(buttonGUITreatment[BUTTON_PINCH_2WPVV_RIGHT_OPEN].state == GUI_BUTTON_RELEASED)
	{
		// pinch venous (pinch di destra - aperto a destra)
		releaseGUIButton(BUTTON_PINCH_2WPVV_RIGHT_OPEN);
		HandlePinch(BUTTON_PINCH_2WPVV_RIGHT_OPEN);
	}
	else if(buttonGUITreatment[BUTTON_PINCH_2WPVV_LEFT_OPEN].state == GUI_BUTTON_RELEASED)
	{
		// pinch venous (pinch di destra - aperto a sinistra)
		releaseGUIButton(BUTTON_PINCH_2WPVV_LEFT_OPEN);
		HandlePinch(BUTTON_PINCH_2WPVV_LEFT_OPEN);
	}
	else if(buttonGUITreatment[BUTTON_PINCH_2WPVV_BOTH_CLOSED].state == GUI_BUTTON_RELEASED)
	{
		// pinch venous (pinch di destra - aperto a sinistra)
		releaseGUIButton(BUTTON_PINCH_2WPVV_BOTH_CLOSED);
		HandlePinch(BUTTON_PINCH_2WPVV_BOTH_CLOSED);
	}

	// ho ricevuto un comando dal computer che mi dice che la fase di smontaggio e' terminata
	// Posso tornare nello stato IDLE
	if(buttonGUITreatment[BUTTON_UNMOUNT_END].state == GUI_BUTTON_RELEASED)
	{
		releaseGUIButton(BUTTON_UNMOUNT_END);
		currentGuard[GUARD_ENABLE_UNMOUNT_END].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
	}
	else if(buttonGUITreatment[BUTTON_PRIMING_ABANDON].state == GUI_BUTTON_RELEASED)
	{
		// mi e' stato richiesto di abbandonare la fase di smontaggio e andare direttamente in IDLE
		releaseGUIButton(BUTTON_PRIMING_ABANDON);
		currentGuard[GUARD_ABANDON_PRIMING].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
	}

}

/*-----------------------------------------------------------*/
/*---------------------FUNZIONI OLD--------------------------*/
/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/
/* This function manages the state T1 no disposable activity */
/*-----------------------------------------------------------*/
void manageStateT1NoDisp(void)
{

}

void manageStateT1NoDispAlways(void)
{

}
/*-----------------------------------------------------------*/
/* This function manages the state T1 with disposable activity */
/*-----------------------------------------------------------*/
void manageStateT1WithDisp(void)
{

}

void manageStateT1WithDispAlways(void)
{

}

/*-----------------------------------------------------------*/
/* This function manages the state priming 1 activity        */
/*-----------------------------------------------------------*/
void manageStatePrimingTreat1(void)
{

}

void manageStatePrimingTreat1Always(void)
{

}
/*-----------------------------------------------------------*/
/* This function manages the state priming 2 activity        */
/*-----------------------------------------------------------*/
void manageStatePrimingTreat2(void)
{
	pumpPerist[0].entry = 0;
	pumpPerist[1].entry = 0;
	pumpPerist[2].entry = 0;
	pumpPerist[3].entry = 0;
}

void manageStatePrimingTreat2Always(void)
{

}

/*-----------------------------------------------------------*/
/* This function manages the state treatment 2 activity      */
/*-----------------------------------------------------------*/
void manageStateTreat2(void)
{

}

void manageStateTreat2Always(void)
{

}

/*-----------------------------------------------------------*/
/* This function manages the state disposable empty 1 activity */
/*-----------------------------------------------------------*/
void manageStateEmptyDisp1(void)
{

}

void manageStateEmptyDisp1Always(void)
{

}

/*-----------------------------------------------------------*/
/* This function manages the state disposable empty 2 activity */
/*-----------------------------------------------------------*/
void manageStateEmptyDisp2(void)
{

}

void manageStateEmptyDisp2Always(void)
{

}

/*-----------------------------------------------------------*/
/* This function manages the state washing activity          */
/*-----------------------------------------------------------*/
void manageStateWashing(void)
{

}

void manageStateWashingAlways(void)
{

}
/*-----------------------------------------------------------*/
/* This function manages the state fatal error activity      */
/*-----------------------------------------------------------*/
void manageStateFatalError(void)
{

}

void manageStateFatalErrorAlways(void)
{

}

/*-----------------------------------------------------------*/
/* This function manages the state Priming_1_Wait activity */
/*-----------------------------------------------------------*/
void computeMachineStateGuardPriming_1_Wait(void)
{
	if( buttonGUITreatment[BUTTON_PRIMING_FILT_INS_CONFIRM].state == GUI_BUTTON_RELEASED)
	{
		FilterSelected = TRUE;
		currentGuard[GUARD_FILTER_INSTALLED].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		releaseGUIButton(BUTTON_PRIMING_FILT_INS_CONFIRM);
		// forzo anche uno start priming per il nuovo stato per far partire subito le pompe
		setGUIButton(BUTTON_START_PRIMING);	}
}

void manageStatePriming_1_WaitEntry(void)
{
}

void manageStatePriming_1_WaitAlways(void)
{
	computeMachineStateGuardPriming_1_Wait();
}

/*-----------------------------------------------------------*/
/*------------------FUNZIONI OLD-----------------------------*/
/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/




/* PARENT LEVEL FUNCTION */

void manageParentPrimingEntry(void){
	if(pumpPerist[0].entry == 0)
	{
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
		if(!FilterSelected)
		{
			// in questo caso il liquido passa per il bypass del filtro che deve essere riempito
			setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_LEFT_OPEN);
		}
		else
			setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_RIGHT_OPEN);

		setPinchPositionHighLevel(PINCH_2WPVA, MODBUS_PINCH_RIGHT_OPEN);
		if(GetTherapyType() == LiverTreat)
		{
			// ho selezionato il fegato, quindi devo riempire anche il circuito venoso
			setPinchPositionHighLevel(PINCH_2WPVV, MODBUS_PINCH_RIGHT_OPEN);
		}

		// lo start viene fatto automaticamente nelle funzioni NewSetTempPeltierActuator..
		//startPeltierActuator();
		//startPeltier2Actuator();
		//peltierCell.readAlwaysEnable = 1;
		//peltierCell2.readAlwaysEnable = 1;

		pumpPerist[0].entry = 1;
	}

	if(pumpPerist[1].entry == 0)
	{
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
		pumpPerist[1].entry = 1;
	}

	/*viene comandata direttamente col comando di quella sopra*/
//	if(pumpPerist[2].entry == 0)
//	{
//		setPumpSpeedValueHighLevel(pumpPerist[2].pmpMySlaveAddress, 0);
//		pumpPerist[2].entry = 1;
//	}

	if(pumpPerist[3].entry == 0)
	{
		setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
		pumpPerist[3].entry = 1;
	}

	// tolgo eventuali comandi impostati per errore
	releaseGUIButton(BUTTON_STOP_OXYGEN_PUMP);
	releaseGUIButton(BUTTON_STOP_PRIMING);
	releaseGUIButton(BUTTON_STOP_PERF_PUMP);
	releaseGUIButton(BUTTON_STOP_ALL_PUMP);
	releaseGUIButton(BUTTON_START_OXYGEN_PUMP);
	releaseGUIButton(BUTTON_START_PERF_PUMP);

	// NB  DEVO IMPOSTARE QUESTO BOTTONE PER FARE IN MODO DI FORZARE LO START DELLA SECONDA FASE
	// DEL PRIMING SENZA ASPETTARE IL COMANDO DALL'OPERATORE CHE IN QUESTA NUOVA GESTIONE NON CI SARA'
	// quando entro in questa fase, ora, ho sempre un po' di liquido da caricare. Quindi, do subito
	// lo start alle pompe.
	if(!AlarmInPrimingEntered)
		setGUIButton((unsigned char)BUTTON_START_PRIMING);

}


//#define NUM_OF_FLOW_SAMPLES  5
//typedef enum
//{
//	RESET_PUMP_GAIN_ARRAY_CMD,
//	CALC_PUMP_GAIN_CMD
//}CALC_PUMP_GAIN_CMD;
//
//// calcolo il guadagno della pompa arteriosa
//char CalcArtPumpGain(float flow, float pump_speed, int cmd, float *pmp_gain)
//{
//  char OK = 0;
//  static word PumpGainArrArt[NUM_OF_FLOW_SAMPLES];
//  static int  PumpGainArrArtIdx = 0;
//  static unsigned char BufferFull = 0;
//  float Pump_Gain = 0.0
//  float Tot_Pump_Gain;
//
//  if(cmd == RESET_PUMP_GAIN_ARRAY_CMD)
//  {
//	  for(int i = 0; i < NUM_OF_FLOW_SAMPLES; i++)
//		  PumpGainArrArt[i] = 0.0;
//  }
//  else if(cmd == CALC_PUMP_GAIN_CMD)
//  {
//	  if((pump_speed > 0.0) && (flow != 0))
//	  {
//		  Pump_Gain = flow / pump_speed;
//		  PumpGainArrArt[PumpGainArrArtIdx] = Pump_Gain;
//		  PumpGainArrArtIdx++;
//		  if(PumpGainArrArtIdx >= NUM_OF_FLOW_SAMPLES)
//		  {
//			  Tot_Pump_Gain = 0.0;
//			  for(int i = 0; i < NUM_OF_FLOW_SAMPLES; i++)
//				  Tot_Pump_Gain += PumpGainArrArt[i];
//			  Pump_Gain = Tot_Pump_Gain / NUM_OF_FLOW_SAMPLES;
//			  OK = 1;
//			  *pmp_gain = Pump_Gain;
//		  }
//	  }
//  }
//  return OK;
//}
//
//
//char CalcVenPumpGain(float flow, float pump_speed, int cmd, float *pmp_gain)
//{
//  char OK = 0;
//  static word PumpGainArrVen[NUM_OF_FLOW_SAMPLES];
//  static int  PumpGainArrVenIdx = 0;
//  static unsigned char BufferFull = 0;
//  float Pump_Gain = 0.0
//  float Tot_Pump_Gain;
//
//  if(cmd == RESET_PUMP_GAIN_ARRAY_CMD)
//  {
//	  for(int i = 0; i < NUM_OF_FLOW_SAMPLES; i++)
//		  PumpGainArrVen[i] = 0.0;
//  }
//  else if(cmd == CALC_PUMP_GAIN_CMD)
//  {
//	  if((pump_speed > 0.0) && (flow != 0))
//	  {
//		  Pump_Gain = flow / pump_speed;
//		  PumpGainArrVen[PumpGainArrVenIdx] = Pump_Gain;
//		  PumpGainArrVenIdx++;
//		  if(PumpGainArrVenIdx >= NUM_OF_FLOW_SAMPLES)
//		  {
//			  Tot_Pump_Gain = 0.0;
//			  for(int i = 0; i < NUM_OF_FLOW_SAMPLES; i++)
//				  Tot_Pump_Gain += PumpGainArrVen[i];
//			  Pump_Gain = Tot_Pump_Gain / NUM_OF_FLOW_SAMPLES;
//			  OK = 1;
//			  *pmp_gain = Pump_Gain;
//		  }
//	  }
//  }
//  return OK;
//}

float CalcArtPumpGain(float flow)
{
	float Pump_Gain;
	if(flow != 0)
	{
		Pump_Gain = flow / ((float)RECIRC_PUMP_HIGH_SPEED / 100.0);
		if(Pump_Gain > ((float)DEFAULT_ART_PUMP_GAIN + DEFAULT_ART_PUMP_GAIN * 30.0 / 100.0))
			Pump_Gain = DEFAULT_ART_PUMP_GAIN;
		else if(Pump_Gain < ((float)DEFAULT_ART_PUMP_GAIN - DEFAULT_ART_PUMP_GAIN * 30.0 / 100.0))
			Pump_Gain = DEFAULT_ART_PUMP_GAIN;
	}
	else
		Pump_Gain = DEFAULT_ART_PUMP_GAIN;
	return Pump_Gain;
}

float CalcVenPumpGain(float flow)
{
	float Pump_Gain;
	if(flow != 0)
	{
		Pump_Gain = flow / ((float)RECIRC_PUMP_HIGH_SPEED / 100.0);
		if(Pump_Gain > ((float)DEFAULT_VEN_PUMP_GAIN + DEFAULT_VEN_PUMP_GAIN * 30.0 / 100.0))
			Pump_Gain = DEFAULT_VEN_PUMP_GAIN;
		else if(Pump_Gain < ((float)DEFAULT_VEN_PUMP_GAIN - DEFAULT_VEN_PUMP_GAIN * 30.0 / 100.0))
			Pump_Gain = DEFAULT_VEN_PUMP_GAIN;
	}
	else
		Pump_Gain = DEFAULT_VEN_PUMP_GAIN;
	return Pump_Gain;
}


// cmd comando per eventuare riposizionamento della macchina a stati
unsigned char TemperatureStateMach(int cmd)
{
	static unsigned long RicircTimeout;
	static unsigned char TempStateMach = START_RECIRC_HIGH_SPEED;
	static int Delay = 0;
	unsigned char TempReached = 0;

	float tmpr;
	word tmpr_trgt;
	// temperatura raggiunta dal reservoir
	tmpr = ((int)(sensorIR_TM[0].tempSensValue*10));
	// temperatura da raggiungere moltiplicata per 10
	tmpr_trgt = parameterWordSetFromGUI[PAR_SET_PRIMING_TEMPERATURE_PERFUSION].value;

	if(cmd == RESTART_CMD)
	{
		if((TempStateMach == STOP_RECIRC_HIGH_SPEED) || (TempStateMach == CALC_PUMPS_GAIN))
		{
			// faccio ripartire i motori ad alta velocita'
			TempStateMach = START_RECIRC_HIGH_SPEED;
		}
		else if(TempStateMach == TEMP_CHECK_DURATION_STATE)
		{
			// faccio ripartire il controllo della temperatura
			TempStateMach = TEMP_START_CHECK_STATE;
		}
	}
	else if(cmd == TEMP_STATE_RESET)
	{
		TempStateMach = TEMP_ABANDONE_STATE;
		Delay = 0;
	}
	else if(cmd == TEMP_ABANDONE_CMD)
	{
		// ho ricevuto un comando di abbandonare e ritornare in idle
		TempStateMach = TEMP_ABANDONE_STATE;
	}
	else if(cmd == TEMP_START_RICIRCOLO)
	{
		// ho ricevuto un comando di prepararmi a far partire il ricircolo
		TempStateMach = START_RECIRC_IDLE;
	}

	if(StartPrimingTime)
		PrimingDuration = msTick_elapsed(StartPrimingTime) * 50L / 1000;

	switch (TempStateMach)
	{
		case START_RECIRC_IDLE:
			// in questo stato ci va all'inizio del priming ed in attesa di iniziare la fase di ricircolo
			Delay++;
			if(Delay > 40)
			{
				// aspetto un paio di secondi poi faccio partire le pompe ad alta velocita'
				// per cercare di eliminare l'aria
				TempStateMach = START_RECIRC_HIGH_SPEED;
				Delay = 0;

				if(!StartPrimingTime)
				{
					// faccio ripartire il timer del priming
					StartPrimingTime = (unsigned long)timerCounterModBus;
				}
			}
			break;

		case START_RECIRC_HIGH_SPEED:
			RicircTimeout = timerCounterModBus;
			setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, (int)RECIRC_PUMP_HIGH_SPEED);
			if((GetTherapyType() == KidneyTreat) &&
			   (((PARAMETER_ACTIVE_TYPE)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_ACTIVE].value) == YES))
			{
				// sono nel ricircolo rene con ossigenatore abilitato
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, (int)RECIRC_PUMP_HIGH_SPEED);
			}
			else if((GetTherapyType() == LiverTreat))
			{
				// sono nel priming fegato
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, (int)RECIRC_PUMP_HIGH_SPEED_ART);
				setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, (int)RECIRC_PUMP_HIGH_SPEED);
			}
			TempStateMach = CALC_PUMPS_GAIN;
			ArteriousPumpGainForPid = DEFAULT_ART_PUMP_GAIN;
			VenousPumpGainForPid = DEFAULT_VEN_PUMP_GAIN;
			break;
		case CALC_PUMPS_GAIN:
			if((msTick_elapsed(RicircTimeout) * 50L) >= (HIGH_PUMP_SPEED_DURATION - 3000))
			{
				// 3 secondi prima della fine del ricircolo calcolo il guadagno delle pompe e le memorizzo
				ArteriousPumpGainForPid = CalcArtPumpGain(sensor_UFLOW[0].Average_Flow_Val);
				VenousPumpGainForPid = CalcVenPumpGain(sensor_UFLOW[1].Average_Flow_Val);
				TempStateMach = STOP_RECIRC_HIGH_SPEED;
			}
			break;
		case STOP_RECIRC_HIGH_SPEED:
			if((msTick_elapsed(RicircTimeout) * 50L) >= HIGH_PUMP_SPEED_DURATION)
			{
				// ho raggiunto il tempo di ricircolo ad alta velocita' per eliminare l'aria eventuale
				// proseguo con il controllo della temperatura
				TempStateMach = TEMP_START_CHECK_STATE;

				// ripristino le velocita' di priming normali
				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, RPM_IN_PRIMING_PHASES);
				if((GetTherapyType() == KidneyTreat) &&
				   (((PARAMETER_ACTIVE_TYPE)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_ACTIVE].value) == YES))
				{
					// sono nel ricircolo rene con ossigenatore abilitato
					setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress,
							                  (int)((float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / OXYG_FLOW_TO_RPM_CONV * 100.0));
				}
				else if(GetTherapyType() == LiverTreat)
				{
					// sono nel priming fegato ed ho superato una quantita' minima nel reservoir, quindi, la pompa venosa
					// per il riempimento del disposable di ossigenazione e la pompa di depurazione PPAR
					setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, (int)LIVER_PRIMING_PMP_OXYG_SPEED);
					setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, LIVER_PPAR_SPEED);
				}
			}
			break;

		case TEMP_START_CHECK_STATE:
			if((tmpr >= (float)(tmpr_trgt - 10)) && (tmpr <= (float)(tmpr_trgt + 10)))
			{
				// ho raggiunto la temperatura target
				TempStateMach = TEMP_CHECK_DURATION_STATE;
				RicircTimeout = timerCounterModBus;
			}
			break;
		case TEMP_CHECK_DURATION_STATE:
			if((tmpr >= (float)(tmpr_trgt - 10)) && (tmpr <= (float)(tmpr_trgt + 10)))
			{
				// ho raggiunto la temperatura target
				if(msTick_elapsed(RicircTimeout) * 50L >= TIMEOUT_TEMPERATURE_RICIRC)
				{
					// per almeno 2 secondi la temperatura si e' mantenuta nell'intorno del target,
					// posso uscire dalla fase di ricircolo
					TempReached = 1;
					TempStateMach = TEMP_ABANDONE_STATE;
				}
			}
			else
			{
				TempStateMach = TEMP_START_CHECK_STATE;
			}
			break;
		case TEMP_ABANDONE_STATE:
			// ho ricevuto il comando di abbandonare e tornare in idle

			// il priming e' terminato blocco il conteggio del timer
			TotalPrimingDuration += PrimingDuration;
			PrimingDuration = 0;
			// faccio in modo che il conteggio riprenda al prossimo button_start_priming
			StartPrimingTime = 0;
			break;
	}
	return TempReached;
}


void manageParentPrimingAlways(void){

	static char iflag_perf = 0;
	static char iflag_oxyg = 0;
	int speed = 0;
	static int timerCopy = 0;

	// reset dell'eventuale flag di entrata nello stato di gestione allarme.
	AlarmInPrimingEntered = FALSE;

	//manage pump
	switch(ptrCurrentParent->parent){
	case PARENT_PRIMING_TREAT_KIDNEY_1_INIT:
	if(buttonGUITreatment[BUTTON_START_PRIMING].state == GUI_BUTTON_RELEASED)
	{
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, RPM_IN_PRIMING_PHASES);
		if((GetTherapyType() == KidneyTreat) && (((PARAMETER_ACTIVE_TYPE)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_ACTIVE].value) == YES) &&
			(perfusionParam.priVolPerfArt > MIN_LIQ_IN_RES_TO_START_OXY_VEN))
		{
			// sono nel priming rene con ossigenatore abilitato ed ho superato una quantita' minima nel reservoir
			setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress,
					                  (int)((float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / OXYG_FLOW_TO_RPM_CONV * 100.0));
		}
		else if((GetTherapyType() == LiverTreat) && (perfusionParam.priVolPerfArt > MIN_LIQ_IN_RES_TO_START_OXY_VEN))
		{
			// sono nel priming fegato ed ho superato una quantita' minima nel reservoir, quindi, la pompa venosa
			// per il riempimento del disposable di ossigenazione e la pompa di depurazione PPAR
			setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, (int)LIVER_PRIMING_PMP_OXYG_SPEED);
			setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, LIVER_PPAR_SPEED);
		}

		if(!FilterSelected)
			setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_LEFT_OPEN);
		else
			setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_RIGHT_OPEN);
		setPinchPositionHighLevel(PINCH_2WPVA, MODBUS_PINCH_RIGHT_OPEN);
		if(GetTherapyType() == LiverTreat)
		{
			// ho selezionato il fegato, quindi devo riempire anche il circuito venoso
			setPinchPositionHighLevel(PINCH_2WPVV, MODBUS_PINCH_RIGHT_OPEN);
		}

		releaseGUIButton(BUTTON_START_PRIMING);
		if(!StartPrimingTime)
		{
			// prendo il tempo di start del priming solo se il valore vale 0, cioe' sono partito da IDLE
			StartPrimingTime = (unsigned long)timerCounterModBus;
		}
	}
	else if(buttonGUITreatment[BUTTON_START_PERF_PUMP].state == GUI_BUTTON_RELEASED)
	{
		releaseGUIButton(BUTTON_START_PERF_PUMP);

		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, RPM_IN_PRIMING_PHASES);
		if(!FilterSelected)
			setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_LEFT_OPEN);
		else
			setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_RIGHT_OPEN);
		setPinchPositionHighLevel(PINCH_2WPVA, MODBUS_PINCH_RIGHT_OPEN);
		if(GetTherapyType() == LiverTreat)
		{
			// ho selezionato il fegato, quindi devo riempire anche il circuito venoso
			setPinchPositionHighLevel(PINCH_2WPVV, MODBUS_PINCH_RIGHT_OPEN);
		}
	}
	else if(buttonGUITreatment[BUTTON_START_OXYGEN_PUMP].state == GUI_BUTTON_RELEASED)
	{
		releaseGUIButton(BUTTON_START_OXYGEN_PUMP);
		// codice originale
		//setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, (int)((float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / OXYG_FLOW_TO_RPM_CONV * 100.0));

		if((GetTherapyType() == KidneyTreat) && (((PARAMETER_ACTIVE_TYPE)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_ACTIVE].value) == YES) &&
			(perfusionParam.priVolPerfArt > MIN_LIQ_IN_RES_TO_START_OXY_VEN))
		{
			// sono nel priming rene con ossigenatore abilitato ed ho superato una quantita' minima nel reservoir
			setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress,
					                  (int)((float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / OXYG_FLOW_TO_RPM_CONV * 100.0));
		}
		else if((GetTherapyType() == LiverTreat) && (perfusionParam.priVolPerfArt > MIN_LIQ_IN_RES_TO_START_OXY_VEN))
		{
			// sono nel priming fegato ed ho superato una quantita' minima nel reservoir, quindi, la pompa venosa
			// per il riempimento del disposable di ossigenazione e la pompa di depurazione PPAR
			setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, (int)LIVER_PRIMING_PMP_OXYG_SPEED);
		}
	}
	else if(buttonGUITreatment[BUTTON_STOP_ALL_PUMP].state == GUI_BUTTON_RELEASED)
	{
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
		if(GetTherapyType() == LiverTreat)
		{
			// se sono nel trattamento fegato fermo anche l'altro motore !!
			setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
		}
		releaseGUIButton(BUTTON_STOP_ALL_PUMP);

	}
	else if(buttonGUITreatment[BUTTON_STOP_PERF_PUMP].state == GUI_BUTTON_RELEASED)
	{
		releaseGUIButton(BUTTON_STOP_PERF_PUMP);

		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
		iflag_perf = 0;
	}
	else if(buttonGUITreatment[BUTTON_STOP_PRIMING].state == GUI_BUTTON_RELEASED)
	{
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
		if(GetTherapyType() == LiverTreat)
		{
			// se sono nel trattamento fegato fermo anche l'altro motore !!
			setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
		}
		releaseGUIButton(BUTTON_STOP_PRIMING);
		TotalPrimingDuration += PrimingDuration;
		PrimingDuration = 0;
		// faccio in modo che il conteggio riprenda al prossimo button_start_priming
		StartPrimingTime = 0;
	}
	else if(buttonGUITreatment[BUTTON_STOP_OXYGEN_PUMP].state == GUI_BUTTON_RELEASED)
	{
		releaseGUIButton(BUTTON_STOP_OXYGEN_PUMP);

		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
		iflag_perf = 0;
	}

	if((timerCounterModBus%9) == 8)
	{
		if(timerCounterModBus != 0)
			timerCopy = timerCounterModBus;
		timerCounter = 0;

		//readPumpSpeedValue(pumpPerist[0].pmpMySlaveAddress);
		readPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress);
		readPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress);
		// non ho messo la pompa 2 perche'  e' agganciata alla 1
		readPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress);
	}

	//if(iflag_pmp1_rx == IFLAG_PMP1_RX)
	if(pumpPerist[0].dataReady == DATA_READY_TRUE)
	{
		//iflag_pmp1_rx = IFLAG_IDLE;
		//speed = ((BYTES_TO_WORD_SIGN(msgToRecvFrame3[3], msgToRecvFrame3[4]))/100)*(timerCopy);
		// la velocita' ora posso leggerla direttamente dall'array di registry modbus
		speed = modbusData[pumpPerist[0].pmpMySlaveAddress-2][17] / 100;
		pumpPerist[0].actualSpeed = speed;
		if((ptrCurrentState->state == STATE_PRIMING_PH_1) || (ptrCurrentState->state == STATE_PRIMING_PH_2))
		{
			volumePriming = volumePriming + ((float)speed * CONV_RPMMIN_TO_ML_PER_INTERVAL);
			perfusionParam.priVolPerfArt = (int)(volumePriming);
		}
		pumpPerist[0].dataReady = DATA_READY_FALSE;
	}
	if(pumpPerist[1].dataReady == DATA_READY_TRUE)
	{
		float fl;
		pumpPerist[1].actualSpeed = modbusData[pumpPerist[1].pmpMySlaveAddress-2][17] / 100;
		// la pompa 2 e' agganciata alla 1
		pumpPerist[2].actualSpeed = modbusData[pumpPerist[2].pmpMySlaveAddress-2][17] / 100; //pumpPerist[1].actualSpeed;
		// calcolo il volume complessivo di liquido trattato dall'ossigenatore
		fl = ((float)pumpPerist[1].actualSpeed * CONV_RPMMIN_TO_ML_PER_INT_OXYG * 2.0);
		perfusionParam.priVolPerfVenOxy = perfusionParam.priVolPerfVenOxy + (word)fl;
		if((GetTherapyType() == LiverTreat) && (StartOxygAndDepState == 1) &&
		   ((ptrCurrentState->state == STATE_PRIMING_PH_1) || (ptrCurrentState->state == STATE_PRIMING_PH_2)))
		{
			// faccio l'aggiornamento solo nel fegato e se lo start alle pompe di
			// ossigenazione e' stato dato (questo secondo test sarebbe ridondante)
			volumePriming = volumePriming + fl;
			perfusionParam.priVolPerfArt = (int)(volumePriming);
		}
		pumpPerist[1].dataReady = DATA_READY_FALSE;
	}
	if(pumpPerist[3].dataReady == DATA_READY_TRUE)
	{
		pumpPerist[3].actualSpeed = modbusData[pumpPerist[3].pmpMySlaveAddress-2][17] / 100;
		pumpPerist[3].dataReady = DATA_READY_FALSE;
	}
	break;

	case PARENT_PRIMING_TREAT_KIDNEY_1_RUN:
			if(buttonGUITreatment[BUTTON_START_PRIMING].state == GUI_BUTTON_RELEASED)
			{
				if(ptrCurrentState->state == STATE_PRIMING_RICIRCOLO)
				{
					// sono nella fase di ricircolo e si e' verificato un allarme oppure l'utente ha
					// fermato il processo con stop priming e poi e' ripartito
					if(AlarmOrStopInRecircFlag)
						AlarmOrStopInRecircFlag = FALSE;
					TemperatureStateMach(RESTART_CMD);
				}
				else
				{
					setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, RPM_IN_PRIMING_PHASES);
					//if(((PARAMETER_ACTIVE_TYPE)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_ACTIVE].value) == YES)
					//	setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, (int)((float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / OXYG_FLOW_TO_RPM_CONV * 100.0));
					if((GetTherapyType() == KidneyTreat) && (((PARAMETER_ACTIVE_TYPE)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_ACTIVE].value) == YES) &&
						(perfusionParam.priVolPerfArt > MIN_LIQ_IN_RES_TO_START_OXY_VEN))
					{
						// sono nel priming rene con ossigenatore abilitato ed ho superato una quantita' minima nel reservoir
						setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress,
												  (int)((float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / OXYG_FLOW_TO_RPM_CONV * 100.0));
					}
					else if((GetTherapyType() == LiverTreat) && (perfusionParam.priVolPerfArt > MIN_LIQ_IN_RES_TO_START_OXY_VEN))
					{
						// sono nel priming fegato ed ho superato una quantita' minima nel reservoir, quindi, la pompa venosa
						// per il riempimento del disposable di ossigenazione e la pompa di depurazione PPAR
						setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, (int)LIVER_PRIMING_PMP_OXYG_SPEED);
						setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, LIVER_PPAR_SPEED);
					}
				}

				if(!FilterSelected)
					setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_LEFT_OPEN);
				else
					setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_RIGHT_OPEN);
				setPinchPositionHighLevel(PINCH_2WPVA, MODBUS_PINCH_RIGHT_OPEN);
				if(GetTherapyType() == LiverTreat)
				{
					// ho selezionato il fegato, quindi devo riempire anche il circuito venoso
					setPinchPositionHighLevel(PINCH_2WPVV, MODBUS_PINCH_RIGHT_OPEN);
				}

				releaseGUIButton(BUTTON_START_PRIMING);
				if(!StartPrimingTime)
				{
					// prendo il tempo di start del priming solo se il valore vale 0, cioe' sono partito da IDLE
					StartPrimingTime = (unsigned long)timerCounterModBus;
				}
			}
			else if(buttonGUITreatment[BUTTON_START_PERF_PUMP].state == GUI_BUTTON_RELEASED)
			{
				releaseGUIButton(BUTTON_START_PERF_PUMP);

				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, RPM_IN_PRIMING_PHASES);
				if(!FilterSelected)
					setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_LEFT_OPEN);
				else
					setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_RIGHT_OPEN);
				setPinchPositionHighLevel(PINCH_2WPVA, MODBUS_PINCH_RIGHT_OPEN);
				if(GetTherapyType() == LiverTreat)
				{
					// ho selezionato il fegato, quindi devo riempire anche il circuito venoso
					setPinchPositionHighLevel(PINCH_2WPVV, MODBUS_PINCH_RIGHT_OPEN);
				}
			}
			else if(buttonGUITreatment[BUTTON_START_OXYGEN_PUMP].state == GUI_BUTTON_RELEASED)
			{
				releaseGUIButton(BUTTON_START_OXYGEN_PUMP);
				//setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, (int)((float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / OXYG_FLOW_TO_RPM_CONV * 100.0));
				if((GetTherapyType() == KidneyTreat) && (((PARAMETER_ACTIVE_TYPE)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_ACTIVE].value) == YES) &&
					(perfusionParam.priVolPerfArt > MIN_LIQ_IN_RES_TO_START_OXY_VEN))
				{
					// sono nel priming rene con ossigenatore abilitato ed ho superato una quantita' minima nel reservoir
					setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress,
							                  (int)((float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / OXYG_FLOW_TO_RPM_CONV * 100.0));
				}
				else if((GetTherapyType() == LiverTreat) && (perfusionParam.priVolPerfArt > MIN_LIQ_IN_RES_TO_START_OXY_VEN))
				{
					// sono nel priming fegato ed ho superato una quantita' minima nel reservoir, quindi, la pompa venosa
					// per il riempimento del disposable di ossigenazione e la pompa di depurazione PPAR
					setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, (int)LIVER_PRIMING_PMP_OXYG_SPEED);
				}

			}
			else if(buttonGUITreatment[BUTTON_STOP_ALL_PUMP].state == GUI_BUTTON_RELEASED)
			{
				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
				if(GetTherapyType() == LiverTreat)
				{
					// se sono nel trattamento fegato fermo anche l'altro motore !!
					setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
				}
				releaseGUIButton(BUTTON_STOP_ALL_PUMP);
			}
			else if(buttonGUITreatment[BUTTON_STOP_PERF_PUMP].state == GUI_BUTTON_RELEASED)
			{
				releaseGUIButton(BUTTON_STOP_PERF_PUMP);

				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
				iflag_perf = 0;
			}
			else if(buttonGUITreatment[BUTTON_STOP_OXYGEN_PUMP].state == GUI_BUTTON_RELEASED)
			{
				releaseGUIButton(BUTTON_STOP_OXYGEN_PUMP);

				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
				iflag_perf = 0;
			}
			else if(buttonGUITreatment[BUTTON_STOP_PRIMING].state == GUI_BUTTON_RELEASED)
			{
				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
				if(GetTherapyType() == LiverTreat)
				{
					// se sono nel trattamento fegato fermo anche l'altro motore !!
					setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
				}
				releaseGUIButton(BUTTON_STOP_PRIMING);
				TotalPrimingDuration += PrimingDuration;
				PrimingDuration = 0;
				// faccio in modo che il conteggio riprenda al prossimo button_start_priming
				StartPrimingTime = 0;
			}
#ifdef DEBUG_WITH_SERVICE_SBC
			else if((ptrCurrentState->state == STATE_PRIMING_PH_1) &&
					((float)perfusionParam.priVolPerfArt >= ((float)GetTotalPrimingVolumePerf(0) * 50 / 100.0)))  // parameterWordSetFromGUI[PAR_SET_PRIMING_VOL_PERFUSION].value
#else
			// nel caso di debug considero il 50%
			else if((ptrCurrentState->state == STATE_PRIMING_PH_1) &&
					((float)perfusionParam.priVolPerfArt >= ((float)GetTotalPrimingVolumePerf(0) * PERC_OF_PRIM_FOR_FILTER / 100.0)))  // parameterWordSetFromGUI[PAR_SET_PRIMING_VOL_PERFUSION].value
#endif
			{
				if(currentGuard[GUARD_ENABLE_STATE_PRIMING_PH_1_WAIT].guardEntryValue != GUARD_ENTRY_VALUE_TRUE)
				{
					// ho raggiunto il 95% del volume, fermo le pompe ed aspetto il caricamento del filtro
					setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
					setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
					if(GetTherapyType() == LiverTreat)
					{
						// se sono nel trattamento fegato fermo anche l'altro motore !!
						setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
					}
					// vengono fermate le pompe in attesa del montaggio del filtro poi il priming riprende
					TotalPrimingDuration += PrimingDuration;
					PrimingDuration = 0;
					// faccio in modo che il conteggio riprenda al prossimo button_start_priming
					StartPrimingTime = 0;
				}
				currentGuard[GUARD_ENABLE_STATE_PRIMING_PH_1_WAIT].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			}
			else if((ptrCurrentState->state == STATE_PRIMING_PH_2) &&
					(perfusionParam.priVolPerfArt >= GetTotalPrimingVolumePerf(0))) // parameterWordSetFromGUI[PAR_SET_PRIMING_VOL_PERFUSION].value
			{
				// aggiorno il totale del volume accumulato per gli eventuali priming successivi
				GetTotalPrimingVolumePerf((int)NEW_PRIM_CMD_TOT_PRIM_VOL);

				// ho raggiunto il volume complessivo, fermo le pompe ed aspetto
				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
				if(GetTherapyType() == LiverTreat)
				{
					// se sono nel trattamento fegato fermo anche l'altro motore !!
					setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
				}
				// faccio andare lo stato principale nello stato di attesa di un nuovo volume
				// o di un fine priming
				currentGuard[GUARD_ENABLE_PRIMING_WAIT].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;

				// vengono fermate le pompe in attesa della partenza della fase di ricircolo
				TotalPrimingDuration += PrimingDuration;
				PrimingDuration = 0;
				// faccio in modo che il conteggio riprenda al prossimo button_start_priming
				StartPrimingTime = 0;
			}
			else if(ptrCurrentState->state == STATE_PRIMING_RICIRCOLO)
			{
				if(TemperatureStateMach(0))
				{
					// ho raggiunto la temperatura ( + o - un grado)richiesta posso passare nello stato di
					// attesa ricezione comando di start trattamento
					// fermo le pompe ed aspetto
					setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
					setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
					if(GetTherapyType() == LiverTreat)
					{
						// se sono nel trattamento fegato fermo anche l'altro motore !!
						setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
					}
					//currentGuard[GUARD_ENABLE_WAIT_TREATMENT].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
					//StartTreatmentTime = (unsigned long)timerCounterModBus;
					currentGuard[GUARD_ENABLE_TREATMENT_KIDNEY_1].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
				}
				else if(buttonGUITreatment[BUTTON_PRIMING_ABANDON].state == GUI_BUTTON_RELEASED)
				{
					releaseGUIButton(BUTTON_PRIMING_ABANDON);
					setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
					setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
					if(GetTherapyType() == LiverTreat)
					{
						// se sono nel trattamento fegato fermo anche l'altro motore !!
						setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
					}
					currentGuard[GUARD_ABANDON_PRIMING].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
					// sposto la macchina a stati in TEMP_ABANDONE_CMD per evitare problemi
					TemperatureStateMach(TEMP_ABANDONE_CMD);
				}

			}
//			Questo non serve tanto non ci passa mai perche' nello stato STATE_PRIMING_WAIT non viene impostata la ManageParentPrimingAlways
//			else if(ptrCurrentState->state == STATE_PRIMING_WAIT &&
//					GetTotalPrimingVolumePerf() > perfusionParam.priVolPerfArt)
//			{
//				// devo ritornare nello stato di priming_ph2 perche' devo aggiungere altro liquido
//				currentGuard[GUARD_ENABLE_PRIMING_PHASE_2].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
//			}


			/*if(
				(buttonGUITreatment[BUTTON_START_PRIMING].state == GUI_BUTTON_PRESSED) ||
				(buttonGUITreatment[BUTTON_START_PERF_PUMP].state == GUI_BUTTON_PRESSED)
				)
			{
				releaseGUIButton(BUTTON_START_PRIMING);
				releaseGUIButton(BUTTON_START_PERF_PUMP);

				if(iflag_perf == 0)
				{
					setPumpSpeedValue(pumpPerist[0].pmpMySlaveAddress, 4000);
					iflag_perf = 1;
				}
			}
			else if(
					(buttonGUITreatment[BUTTON_STOP_ALL_PUMP].state == GUI_BUTTON_PRESSED) ||
					(buttonGUITreatment[BUTTON_STOP_PERF_PUMP].state == GUI_BUTTON_PRESSED)
					)
			{
				releaseGUIButton(BUTTON_STOP_ALL_PUMP);
				releaseGUIButton(BUTTON_STOP_PERF_PUMP;

				setPumpSpeedValue(pumpPerist[0].pmpMySlaveAddress, 0);
				iflag_perf = 0;
			}*/

		if((timerCounterModBus%9) == 8)
			{
				if(timerCounterModBus != 0)
					timerCopy = timerCounterModBus;
				timerCounter = 0;

				//readPumpSpeedValue(pumpPerist[0].pmpMySlaveAddress);
				readPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress);
				// non ho messo la pompa 2 perche'  e' agganciata alla 1
				readPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress);
				readPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress);
			}

		if(pumpPerist[0].dataReady == DATA_READY_TRUE)
		{
			//iflag_pmp1_rx = IFLAG_IDLE;
			//speed = ((BYTES_TO_WORD_SIGN(msgToRecvFrame3[3], msgToRecvFrame3[4]))/100)*(timerCopy);
			// la velocita' ora posso leggerla direttamente dall'array di registry modbus
			speed = modbusData[pumpPerist[0].pmpMySlaveAddress-2][17] / 100;
			pumpPerist[0].actualSpeed = speed;
			if((ptrCurrentState->state == STATE_PRIMING_PH_1) || (ptrCurrentState->state == STATE_PRIMING_PH_2))
			{
				// nella fase di ricircolo non aggiorno piu' il volume
				volumePriming = volumePriming + ((float)speed * CONV_RPMMIN_TO_ML_PER_INTERVAL);
				perfusionParam.priVolPerfArt = (int)(volumePriming);
			}
			pumpPerist[0].dataReady = DATA_READY_FALSE;
		}
		if(pumpPerist[1].dataReady == DATA_READY_TRUE)
		{
			float fl;
			pumpPerist[1].actualSpeed = modbusData[pumpPerist[1].pmpMySlaveAddress-2][17] / 100;
			// la pompa 2 e' agganciata alla 1
			pumpPerist[2].actualSpeed = modbusData[pumpPerist[2].pmpMySlaveAddress-2][17] / 100;//pumpPerist[1].actualSpeed;
			// calcolo il volume complessivo di liquido trattato dall'ossigenatore
			fl = ((float)pumpPerist[1].actualSpeed * CONV_RPMMIN_TO_ML_PER_INT_OXYG * 2.0);
			perfusionParam.priVolPerfVenOxy = perfusionParam.priVolPerfVenOxy + (word)fl;
			if((GetTherapyType() == LiverTreat) && (StartOxygAndDepState == 1) &&
			   ((ptrCurrentState->state == STATE_PRIMING_PH_1) || (ptrCurrentState->state == STATE_PRIMING_PH_2)))
			{
				// faccio l'aggiornamento solo nel fegato e se lo start alle pompe di
				// ossigenazione e' stato dato (questo secondo test sarebbe ridondante)
				volumePriming = volumePriming + fl;
				perfusionParam.priVolPerfArt = (int)(volumePriming);
			}
			pumpPerist[1].dataReady = DATA_READY_FALSE;
		}
		if(pumpPerist[3].dataReady == DATA_READY_TRUE)
		{
			pumpPerist[3].actualSpeed = modbusData[pumpPerist[3].pmpMySlaveAddress-2][17] / 100;
			pumpPerist[3].dataReady = DATA_READY_FALSE;
		}

		break;

	case PARENT_PRIMING_TREAT_KIDNEY_1_ALARM:
		setPumpSpeedValue(pumpPerist[0].pmpMySlaveAddress, 0);
		break;

	case PARENT_PRIMING_TREAT_KIDNEY_1_END:
		setPumpSpeedValue(pumpPerist[0].pmpMySlaveAddress, 0);
		break;

	default:
		break;
	}

}

void manageParentPrimingAlarmEntry(void)
{
	/*
	 * (FM) per ora commento tutto questo perche' per la gestione dell'allarme puo' essere fatta in diversi modi ed e'
	 * meglio farla dentro lo switch del child
	//static unsigned char oneShot = 0;
	if(pumpPerist[0].dataReady == DATA_READY_FALSE)
	{
	setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
	setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
	setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_POS_CLOSED);
	setPinchPositionHighLevel(PNCHVLV2_ADDRESS, MODBUS_PINCH_POS_CLOSED);
	setPinchPositionHighLevel(PNCHVLV3_ADDRESS, MODBUS_PINCH_POS_CLOSED);
	//oneShot = 1;

	stopPeltierActuator();
	}

	pumpPerist[0].entry = 0;
	*/

	// entro in uno stato di allarme durante il priming
	AlarmInPrimingEntered = TRUE;

	if(ptrCurrentState->state == STATE_PRIMING_RICIRCOLO)
		AlarmOrStopInRecircFlag = TRUE;
}

void manageParentTreatAlarmEntry(void){

	/*
	 * (FM) per ora commento tutto questo perche' per la gestione dell'allarme puo' essere fatta in diversi modi ed e'
	 * meglio farla dentro lo switch del child
	if(pumpPerist[0].dataReady == DATA_READY_FALSE)
	{
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
		setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_LEFT_OPEN);
		setPinchPositionHighLevel(PNCHVLV2_ADDRESS, MODBUS_PINCH_POS_CLOSED);
		setPinchPositionHighLevel(PNCHVLV3_ADDRESS, MODBUS_PINCH_LEFT_OPEN);

		stopPeltierActuator();
	}

	pumpPerist[0].actualSpeedOld = 0;
	pumpPerist[0].entry = 0;
	*/
}



void manageParentPrimingAlarmAlways(void){
/*
 * (FM) per ora commento tutto questo perche' per la gestione dell'allarme puo' essere fatta in diversi modi ed e'
 * meglio farla dentro lo switch del child
	//static unsigned char oneShot = 0;
	static int speed = 0;
	static int timerCopy = 0;

	if((pumpPerist[0].dataReady == DATA_READY_FALSE) && (speed != 0))
	{
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
		setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_POS_CLOSED); // forse vanno messe in scarico e non chiuse !!!!
		setPinchPositionHighLevel(PNCHVLV2_ADDRESS, MODBUS_PINCH_POS_CLOSED);
		setPinchPositionHighLevel(PNCHVLV3_ADDRESS, MODBUS_PINCH_POS_CLOSED);
		//oneShot = 1;
	}

	if((timerCounterModBus%9) == 8)
	{
		if(timerCounterModBus != 0)
			timerCopy = timerCounterModBus;
		timerCounter = 0;

		readPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress);
	}

	if(pumpPerist[0].dataReady == DATA_READY_TRUE)
	{
		//speed = ((BYTES_TO_WORD_SIGN(msgToRecvFrame3[3], msgToRecvFrame3[4]))/100)*(timerCopy);
		// la velocita' ora posso leggerla direttamente dall'array di registry modbus
		speed = modbusData[pumpPerist[0].pmpMySlaveAddress-2][17] / 100;
		pumpPerist[0].actualSpeed = speed;
		pumpPerist[0].dataReady = DATA_READY_FALSE;
	}
*/
}

void manageParentTreatAlarmAlways(void){

	/*
	 * (FM) per ora commento tutto questo perche' per la gestione dell'allarme puo' essere fatta in diversi modi ed e'
	 * meglio farla dentro lo switch del child
	static int speed = 0;
	static int timerCopy = 0;

	if((pumpPerist[0].dataReady == DATA_READY_FALSE) && (speed != 0))
	{
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
		setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_LEFT_OPEN);
		setPinchPositionHighLevel(PNCHVLV2_ADDRESS, MODBUS_PINCH_POS_CLOSED);
		setPinchPositionHighLevel(PNCHVLV3_ADDRESS, MODBUS_PINCH_LEFT_OPEN);
	}

	if((timerCounterModBus%9) == 8)
	{
		if(timerCounterModBus != 0)
			timerCopy = timerCounterModBus;
		timerCounter = 0;

		readPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress);
	}

	if(pumpPerist[0].dataReady == DATA_READY_TRUE)
	{
		//speed = ((BYTES_TO_WORD_SIGN(msgToRecvFrame3[3], msgToRecvFrame3[4]))/100)*(timerCopy);
		// la velocita' ora posso leggerla direttamente dall'array di registry modbus
		speed = modbusData[pumpPerist[0].pmpMySlaveAddress-2][17] / 100;
		pumpPerist[0].actualSpeed = speed;
		pumpPerist[0].dataReady = DATA_READY_FALSE;
	}
*/
}

// In questa funzione ci va quando sono nella fase iniziale del trattamento
// Le pompe verranno fatte partire successivamente su richiesta da parte dell'utente
void manageParentTreatEntry(void){
	if(pumpPerist[0].entry == 0)
	{
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
		if(!FilterSelected)
		{
			// il filtro non viene usato quindi devo passare sempre sul bypass
			setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_LEFT_OPEN);
		}
		else
			setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_RIGHT_OPEN);
		// quando entro in trattamento la pinch deve essere attaccata all'organo
		setPinchPositionHighLevel(PINCH_2WPVA, MODBUS_PINCH_LEFT_OPEN);
		if(GetTherapyType() == LiverTreat)
		{
			// ho selezionato il fegato, quindi se entro in trattamento devo collegarmi all'organo
			setPinchPositionHighLevel(PINCH_2WPVV, MODBUS_PINCH_LEFT_OPEN);
		}
		setPumpPressLoop(0, PRESS_LOOP_OFF);

		// lo start viene fatto automaticamente nelle funzioni NewSetTempPeltierActuator..
		//startPeltierActuator();
		//startPeltier2Actuator();
		//peltierCell.readAlwaysEnable = 1;
		//peltierCell2.readAlwaysEnable = 1;

		pumpPerist[0].entry = 1;
	}

	if(pumpPerist[1].entry == 0)
	{
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
		if(GetTherapyType() == LiverTreat)
			setPumpPressLoop(1, PRESS_LOOP_OFF); // pid venoso off
		pumpPerist[1].entry = 1;
	}

	/*viene comandata direttamente col comando di quella sopra*/
//	if(pumpPerist[2].entry == 0)
//	{
//		setPumpSpeedValueHighLevel(pumpPerist[2].pmpMySlaveAddress, 0);
//		pumpPerist[2].entry = 1;
//	}

	if(pumpPerist[3].entry == 0)
	{
		setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
		pumpPerist[3].entry = 1;
	}
}



void manageParentTreatAlways(void){
		static char iflag_perf = 0;
		static char iflag_oxyg = 0;
		int speed = 0;
		static int timerCopy = 0;

		//manage pump
		switch(ptrCurrentParent->parent){
		case PARENT_TREAT_KIDNEY_1_INIT:
		if(buttonGUITreatment[BUTTON_STOP_ALL_PUMP].state == GUI_BUTTON_RELEASED)
		{
			setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
			setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
			if(GetTherapyType() == LiverTreat)
			{
				// se sono nel trattamento fegato fermo anche l'altro motore !!
				setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
			}
			releaseGUIButton(BUTTON_STOP_ALL_PUMP);
			setPumpPressLoop(0, PRESS_LOOP_OFF);
			if(GetTherapyType() == LiverTreat)
				setPumpPressLoop(1, PRESS_LOOP_OFF); // pid venoso off
		}
		else if(buttonGUITreatment[BUTTON_STOP_TREATMENT].state == GUI_BUTTON_RELEASED)
		{
			// fermo le pompe ed il conteggio del tempo e volume del trattamento
			setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
			setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
			if(GetTherapyType() == LiverTreat)
			{
				// se sono nel trattamento fegato fermo anche l'altro motore !!
				setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
			}
			releaseGUIButton(BUTTON_STOP_TREATMENT);
			setPumpPressLoop(0, PRESS_LOOP_OFF);
			if(GetTherapyType() == LiverTreat)
				setPumpPressLoop(1, PRESS_LOOP_OFF); // pid venoso off
			TotalTreatDuration += TreatDuration;
			TreatDuration = 0;
			StartTreatmentTime = 0;
			StartPrimingTime 	= 0;
			GlobalFlags.FlagsDef.EnableAllAlarms = 0;
		}
		else if(buttonGUITreatment[BUTTON_STOP_PERF_PUMP].state == GUI_BUTTON_RELEASED)
		{
			releaseGUIButton(BUTTON_STOP_PERF_PUMP);

			setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
			iflag_perf = 0;

			setPumpPressLoop(0, PRESS_LOOP_OFF);
		}
		else if(buttonGUITreatment[BUTTON_STOP_OXYGEN_PUMP].state == GUI_BUTTON_RELEASED)
		{
			releaseGUIButton(BUTTON_STOP_OXYGEN_PUMP);
			setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
			iflag_perf = 0;
			if(GetTherapyType() == LiverTreat)
				setPumpPressLoop(1, PRESS_LOOP_OFF); // pid venoso off
		}
		else if(buttonGUITreatment[BUTTON_START_TREATMENT].state == GUI_BUTTON_RELEASED)
		{
			setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 2000); //pump 0: start value = 20 rpm than pressure loop
			//pump 1: start value = 30 rpm than open loop
			//if(((PARAMETER_ACTIVE_TYPE)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_ACTIVE].value) == YES)
			//	setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, (int)((float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / OXYG_FLOW_TO_RPM_CONV * 100.0));
			if((GetTherapyType() == KidneyTreat) && (((PARAMETER_ACTIVE_TYPE)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_ACTIVE].value) == YES) )
			{
				// sono nel trattamento rene con ossigenatore abilitato ed ho superato una quantita' minima nel reservoir
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress,
						                  (int)((float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / OXYG_FLOW_TO_RPM_CONV * 100.0));
			}
			else if(GetTherapyType() == LiverTreat)
			{
				// sono nel trattamento fegato, devo impostare la pressione di lavoro del PID sull'ossigenatore.
				// Ci pensera' poi il pid a far partire la pompa.
			//	parameterWordSetFromGUI[PAR_SET_VENOUS_PRESS_TARGET].value = SET_POINT_PRESSURE_INIT;
				// faccio partire la pompa di depurazione ad una velocita' prestabilita
				setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, LIVER_PPAR_SPEED);
			}

			releaseGUIButton(BUTTON_START_TREATMENT);

			//if(iflag_perf == 0)
			setPumpPressLoop(0, PRESS_LOOP_ON);
			// FM Questi due vanno inizializzati alla pressione corrente prima di far partire il pid, altrimenti il motore
			// potrebbe partire anche se non dovrebbe.
			// Pid originale
			//pressSample1 = PR_ART_mmHg_Filtered;
			//pressSample2 = PR_ART_mmHg_Filtered;
			//pressSample1_Art = MedForArteriousPid;
			//pressSample2_Art = MedForArteriousPid;
			pressSample1_Art = PR_ART_Sistolyc_mmHg_ORG;
			pressSample2_Art = PR_ART_Sistolyc_mmHg_ORG;
			if(!StartTreatmentTime)
			{
				// prendo il tempo di start del trattamento solo se il valore vale 0, cioe' sono partito da IDLE
				// Qui ci passa anche quando esce dall'allarme
				StartTreatmentTime = (unsigned long)timerCounterModBus;
			}

//			// attivo il pid sull'ossigenazione e perfusione venosa solo se sono in fegato
			if(GetTherapyType() == LiverTreat)
			{
				setPumpPressLoop(1, PRESS_LOOP_ON);
				pressSample1_Ven = PR_VEN_mmHg_Filtered;
				pressSample2_Ven = PR_VEN_mmHg_Filtered;
			}
			GlobalFlags.FlagsDef.EnableAllAlarms = 1;
			// disabilito allarme di livello alto in trattamento (per ora)
			GlobalFlags.FlagsDef.EnableLevHighAlarm = 0;
			//GlobalFlags.FlagsDef.TankLevelHigh = 0;
		}
		else if(buttonGUITreatment[BUTTON_START_PERF_PUMP].state == GUI_BUTTON_RELEASED){
			releaseGUIButton(BUTTON_START_PERF_PUMP);

			setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 2000); //pump 0: start value = 20 rpm than pressure loop

			setPumpPressLoop(0, PRESS_LOOP_ON);
			// FM Questi due vanno inizializzati alla pressione corrente prima di far partire il pid, altrimenti il motore
			// potrebbe partire anche se non dovrebbe.
			// pid originale
			//pressSample1 = PR_ART_mmHg_Filtered;
			//pressSample2 = PR_ART_mmHg_Filtered;
			pressSample1_Art = MedForArteriousPid;
			pressSample2_Art = MedForArteriousPid;
			pressSample1_Art = PR_ART_Sistolyc_mmHg_ORG;
			pressSample2_Art = PR_ART_Sistolyc_mmHg_ORG;
        }
		else if(buttonGUITreatment[BUTTON_START_OXYGEN_PUMP].state == GUI_BUTTON_RELEASED){
			releaseGUIButton(BUTTON_START_OXYGEN_PUMP);

			//pump 1: start value = 30 rpm than open loop
			//setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress,(int)((float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / OXYG_FLOW_TO_RPM_CONV * 100.0));
			if((GetTherapyType() == KidneyTreat) && (((PARAMETER_ACTIVE_TYPE)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_ACTIVE].value) == YES) )
			{
				// sono nel trattamento rene con ossigenatore abilitato ed ho superato una quantita' minima nel reservoir
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress,
						                  (int)((float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / OXYG_FLOW_TO_RPM_CONV * 100.0));
			}
			else if(GetTherapyType() == LiverTreat)
			{
				// sono nel trattamento fegato, devo impostare la pressione di lavoro del PID sull'ossigenatore.
				// Ci pensera' poi il pid a far partire la pompa.
			//	parameterWordSetFromGUI[PAR_SET_VENOUS_PRESS_TARGET].value = SET_POINT_PRESSURE_INIT;
			}

//			// attivo il pid sull'ossigenazione e perfusione venosa solo se sono in fegato
			if(GetTherapyType() == LiverTreat)
			{
				setPumpPressLoop(1, PRESS_LOOP_ON);
				pressSample1_Ven = PR_VEN_mmHg_Filtered;
				pressSample2_Ven = PR_VEN_mmHg_Filtered;
			}
		}

		//if((getPumpPressLoop(0) == PRESS_LOOP_ON) && (timerCounterPID >=1))
		// per calcolare il pid ogni 450 msec (sarebbe meglio usare questo per dare il tempo alle attuazioni di avere effetto)
		//if((timerCounterModBus%9) == 7)
		if(timerCounterPID >=1) //codice originale
		{
			timerCounterPID = 0;
			if(getPumpPressLoop(0) == PRESS_LOOP_ON)
			{
				// pid originale
				//alwaysPumpPressLoop(0, &PidFirstTime[0]);
				alwaysPumpPressLoopArt(0, &PidFirstTime[0]);
			}
			if(getPumpPressLoop(1) == PRESS_LOOP_ON)
				alwaysPumpPressLoopVen(1, &PidFirstTime[1]);
		}
		else
		{
			//setPumpSpeedValue(pumpPerist[0].pmpMySlaveAddress, 10);
		}


		if((timerCounterModBus%9) == 8)
		{
			if(timerCounterModBus != 0)
				timerCopy = timerCounterModBus;
            timerCounter = 0;
			readPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress);
			// non ho messo la pompa 2 perche'  e' agganciata alla 1
			readPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress);
			readPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress);
		}

		if(pumpPerist[0].dataReady == DATA_READY_TRUE)
		{
			//iflag_pmp1_rx = IFLAG_IDLE;
			//speed = ((BYTES_TO_WORD_SIGN(msgToRecvFrame3[3], msgToRecvFrame3[4]))/100)*(timerCopy);
			// la velocita' ora posso leggerla direttamente dall'array di registry modbus
			speed = modbusData[pumpPerist[0].pmpMySlaveAddress-2][17] / 100;
			pumpPerist[0].actualSpeed = speed;
			volumeTreatArt = volumeTreatArt + ((float)speed * CONV_RPMMIN_TO_ML_PER_INTERVAL);
			perfusionParam.treatVolPerfArt = (int)(volumeTreatArt);
			pumpPerist[0].dataReady = DATA_READY_FALSE;
		}
		if(pumpPerist[1].dataReady == DATA_READY_TRUE)
		{
			pumpPerist[1].actualSpeed = modbusData[pumpPerist[1].pmpMySlaveAddress-2][17] / 100;
			// la pompa 2 e' agganciata alla 1
			pumpPerist[2].actualSpeed = modbusData[pumpPerist[2].pmpMySlaveAddress-2][17] / 100;//pumpPerist[1].actualSpeed;
			// calcolo il volume complessivo di liquido trattato dall'ossigenatore
			perfusionParam.treatVolPerfVenOxy = perfusionParam.priDurPerVenOxy +(word)((float)pumpPerist[1].actualSpeed * CONV_RPMMIN_TO_ML_PER_INTERVAL);
			pumpPerist[1].dataReady = DATA_READY_FALSE;
		}
		if(pumpPerist[3].dataReady == DATA_READY_TRUE)
		{
			pumpPerist[3].actualSpeed = modbusData[pumpPerist[3].pmpMySlaveAddress-2][17] / 100;
			pumpPerist[3].dataReady = DATA_READY_FALSE;
		}
		break;

		case PARENT_TREAT_KIDNEY_1_PUMP_ON:
			if(buttonGUITreatment[BUTTON_STOP_ALL_PUMP].state == GUI_BUTTON_RELEASED)
			{
				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
				if(GetTherapyType() == LiverTreat)
				{
					// se sono nel trattamento fegato fermo anche l'altro motore !!
					setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
					setPumpPressLoop(1, PRESS_LOOP_OFF); // pid venoso off
				}
				releaseGUIButton(BUTTON_STOP_ALL_PUMP);
				setPumpPressLoop(0, PRESS_LOOP_OFF);
			}
			else if(buttonGUITreatment[BUTTON_STOP_TREATMENT].state == GUI_BUTTON_RELEASED)
			{
				// fermo le pompe ed il conteggio del tempo e volume del trattamento
				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
				if(GetTherapyType() == LiverTreat)
				{
					// se sono nel trattamento fegato fermo anche l'altro motore !!
					setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
					setPumpPressLoop(1, PRESS_LOOP_OFF); // pid venoso off
				}
				releaseGUIButton(BUTTON_STOP_TREATMENT);
				setPumpPressLoop(0, PRESS_LOOP_OFF);
				TotalTreatDuration += TreatDuration;
				TreatDuration = 0;
				StartTreatmentTime = 0;
				GlobalFlags.FlagsDef.EnableAllAlarms = 0;
			}
			else if(buttonGUITreatment[BUTTON_STOP_PERF_PUMP].state == GUI_BUTTON_RELEASED)
			{
				releaseGUIButton(BUTTON_STOP_PERF_PUMP);

				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
				iflag_perf = 0;

				setPumpPressLoop(0, PRESS_LOOP_OFF);
			}
			else if(buttonGUITreatment[BUTTON_STOP_OXYGEN_PUMP].state == GUI_BUTTON_RELEASED)
			{
				releaseGUIButton(BUTTON_STOP_OXYGEN_PUMP);

				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
				if(GetTherapyType() == LiverTreat)
				{
					setPumpPressLoop(1, PRESS_LOOP_OFF); // pid venoso off
				}
				iflag_perf = 0;
			}
			else if(buttonGUITreatment[BUTTON_START_TREATMENT].state == GUI_BUTTON_RELEASED)
			{
				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 2000); //pump 0: start value = 20 rpm than pressure loop
				//pump 1: start value = 30 rpm than open loop
				//if(((PARAMETER_ACTIVE_TYPE)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_ACTIVE].value) == YES)
				//	setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, (int)((float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / OXYG_FLOW_TO_RPM_CONV * 100.0));
				if((GetTherapyType() == KidneyTreat) && (((PARAMETER_ACTIVE_TYPE)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_ACTIVE].value) == YES) )
				{
					// sono nel trattamento rene con ossigenatore abilitato ed ho superato una quantita' minima nel reservoir
					setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress,
							                  (int)((float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / OXYG_FLOW_TO_RPM_CONV * 100.0));
				}
				else if(GetTherapyType() == LiverTreat)
				{
					// sono nel trattamento fegato, devo impostare la pressione di lavoro del PID sull'ossigenatore.
					// Ci pensera' poi il pid a far partire la pompa.
				//	parameterWordSetFromGUI[PAR_SET_VENOUS_PRESS_TARGET].value = SET_POINT_PRESSURE_INIT;
					// faccio partire la pompa di depurazione ad una velocita' prestabilita
					setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, LIVER_PPAR_SPEED);
				}
				releaseGUIButton(BUTTON_START_TREATMENT);

				//if(iflag_perf == 0)
				setPumpPressLoop(0, PRESS_LOOP_ON);
				// FM Questi due vanno inizializzati alla pressione corrente prima di far partire il pid, altrimenti il motore
				// potrebbe partire anche se non dovrebbe.
				// Pid originale
				//pressSample1 = PR_ART_mmHg_Filtered;
				//pressSample2 = PR_ART_mmHg_Filtered;
				pressSample1_Art = MedForArteriousPid;
				pressSample2_Art = MedForArteriousPid;
				pressSample1_Art = PR_ART_Sistolyc_mmHg_ORG;
				pressSample2_Art = PR_ART_Sistolyc_mmHg_ORG;

				if(!StartTreatmentTime)
				{
					// prendo il tempo di start del trattamento solo se il valore vale 0, cioe' sono partito da IDLE
					// Qui ci passa anche quando esce dall'allarme
					StartTreatmentTime = (unsigned long)timerCounterModBus;
				}

//				// attivo il pid sull'ossigenazione e perfusione venosa solo se sono in fegato
				if(GetTherapyType() == LiverTreat)
				{
					setPumpPressLoop(1, PRESS_LOOP_ON);
					pressSample1_Ven = MedForVenousPid;
					pressSample2_Ven = MedForVenousPid;
					pressSample1_Ven = PR_VEN_Sistolyc_mmHg;
					pressSample2_Ven = PR_VEN_Sistolyc_mmHg;
				}
				GlobalFlags.FlagsDef.EnableAllAlarms = 1;
				// disabilito allarme di livello alto in trattamento (per ora)
				GlobalFlags.FlagsDef.EnableLevHighAlarm = 0;
				//GlobalFlags.FlagsDef.TankLevelHigh = 0;
			}
			else if(buttonGUITreatment[BUTTON_START_PERF_PUMP].state == GUI_BUTTON_RELEASED){
				releaseGUIButton(BUTTON_START_PERF_PUMP);

				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 2000); //pump 0: start value = 20 rpm than pressure loop

				setPumpPressLoop(0, PRESS_LOOP_ON);
				// FM Questi due vanno inizializzati alla pressione corrente prima di far partire il pid, altrimenti il motore
				// potrebbe partire anche se non dovrebbe.
				// Pid originale
				//pressSample1 = PR_ART_mmHg_Filtered;
				//pressSample2 = PR_ART_mmHg_Filtered;
				pressSample1_Art = MedForArteriousPid;
				pressSample2_Art = MedForArteriousPid;
				pressSample1_Art = PR_ART_Sistolyc_mmHg_ORG;
				pressSample2_Art = PR_ART_Sistolyc_mmHg_ORG;

			}
			else if(buttonGUITreatment[BUTTON_START_OXYGEN_PUMP].state == GUI_BUTTON_RELEASED){
				releaseGUIButton(BUTTON_START_OXYGEN_PUMP);

				//pump 1: start value = 30 rpm than open loop
				//setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, (int)((float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / OXYG_FLOW_TO_RPM_CONV * 100.0));
				if((GetTherapyType() == KidneyTreat) && (((PARAMETER_ACTIVE_TYPE)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_ACTIVE].value) == YES) )
				{
					// sono nel trattamento rene con ossigenatore abilitato ed ho superato una quantita' minima nel reservoir
					setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress,
							                  (int)((float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / OXYG_FLOW_TO_RPM_CONV * 100.0));
				}
				else if(GetTherapyType() == LiverTreat)
				{
					// sono nel trattamento fegato, devo impostare la pressione di lavoro del PID sull'ossigenatore.
					// Ci pensera' poi il pid a far partire la pompa.
			//		parameterWordSetFromGUI[PAR_SET_VENOUS_PRESS_TARGET].value = SET_POINT_PRESSURE_INIT;
				}

//				// attivo il pid sull'ossigenazione e perfusione venosa solo se sono in fegato
				if(GetTherapyType() == LiverTreat)
				{
					setPumpPressLoop(1, PRESS_LOOP_ON);
					pressSample1_Ven = MedForVenousPid;
					pressSample2_Ven = MedForVenousPid;
					pressSample1_Ven = PR_VEN_Sistolyc_mmHg;
					pressSample2_Ven = PR_VEN_Sistolyc_mmHg;
				}
			}

			//if((getPumpPressLoop(0) == PRESS_LOOP_ON) && (timerCounterPID >=1))
			// per calcolare il pid ogni 450 msec (sarebbe meglio usare questo per dare il tempo alle attuazioni di avere effetto)
			//if((timerCounterModBus%9) == 7)
			if(timerCounterPID >=1)
			{
				timerCounterPID = 0;
				if(getPumpPressLoop(0) == PRESS_LOOP_ON)
				{
					// pid originale
					//alwaysPumpPressLoop(0, &PidFirstTime[0]);
					alwaysPumpPressLoopArt(0, &PidFirstTime[0]);
				}
				if(getPumpPressLoop(1) == PRESS_LOOP_ON)
					alwaysPumpPressLoopVen(1, &PidFirstTime[1]);

			}
			else
			{
				//setPumpSpeedValue(pumpPerist[0].pmpMySlaveAddress, 10);
			}


			if((timerCounterModBus%9) == 8)
			{
				if(timerCounterModBus != 0)
					timerCopy = timerCounterModBus;
                timerCounter = 0;

				readPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress);
				// non ho messo la pompa 2 perche'  e' agganciata alla 1
				readPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress);
				readPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress);
			}

			if(pumpPerist[0].dataReady == DATA_READY_TRUE)
			{
				//iflag_pmp1_rx = IFLAG_IDLE;
				//speed = ((BYTES_TO_WORD_SIGN(msgToRecvFrame3[3], msgToRecvFrame3[4]))/100)*(timerCopy);
				// la velocita' ora posso leggerla direttamente dall'array di registry modbus
				speed = modbusData[pumpPerist[0].pmpMySlaveAddress-2][17];
				pumpPerist[0].actualSpeed = speed / 100;
				volumeTreatArt = volumeTreatArt + ((float)speed * CONV_RPMMIN_TO_ML_PER_INTERVAL);
				perfusionParam.treatVolPerfArt = (int)(volumeTreatArt);
				pumpPerist[0].dataReady = DATA_READY_FALSE;
			}
			if(pumpPerist[1].dataReady == DATA_READY_TRUE)
			{
				pumpPerist[1].actualSpeed = modbusData[pumpPerist[1].pmpMySlaveAddress-2][17] / 100;
				// la pompa 2 e' agganciata alla 1
				pumpPerist[2].actualSpeed = modbusData[pumpPerist[2].pmpMySlaveAddress-2][17] / 100;//pumpPerist[1].actualSpeed;
				// calcolo il volume complessivo di liquido trattato dall'ossigenatore
				perfusionParam.treatVolPerfVenOxy = perfusionParam.priDurPerVenOxy +(word)((float)pumpPerist[1].actualSpeed * CONV_RPMMIN_TO_ML_PER_INTERVAL);
				pumpPerist[1].dataReady = DATA_READY_FALSE;
			}
			if(pumpPerist[3].dataReady == DATA_READY_TRUE)
			{
				pumpPerist[3].actualSpeed = modbusData[pumpPerist[3].pmpMySlaveAddress-2][17] / 100;
				pumpPerist[3].dataReady = DATA_READY_FALSE;
			}
			break;

		case PARENT_TREAT_KIDNEY_1_ALARM:
			setPumpSpeedValue(pumpPerist[0].pmpMySlaveAddress, 0);
			break;

		case PARENT_TREAT_KIDNEY_1_END:
			setPumpSpeedValue(pumpPerist[0].pmpMySlaveAddress, 0);
			break;

		default:
			break;
		}
}

void setPumpPressLoop(unsigned char pmpId, unsigned char valOnOff){
	pumpPerist[pmpId].pmpPressLoop = valOnOff;
	PidFirstTime[pmpId] = valOnOff;
}

unsigned char getPumpPressLoop(unsigned char pmpId){
	return pumpPerist[pmpId].pmpPressLoop;
}




void manageParentEntry(void)
{
	// (FM) qui posso far partire il T1 test e l'inizializzazione della comm
	// TODO
}

void manageParentEntryAlways(void)
{
	static int index3 = 0;
	index3++;

	// TODO
	// (FM) qui posso gestire il T1 test e l'inizializzazione della comm ed alla fine
	// devo eseguire le due istruzioni
	currentGuard[GUARD_HW_T1T_DONE].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
	currentGuard[GUARD_COMM_ENABLED].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
}


// Processo di svuotamento del disposable----------------------------------------------
word VolumeDischarged = 0;
bool EmptyDisposStartOtherPump = FALSE;
EMPTY_DISPOSABLE_STATE EmptyDispRunAlwaysState = INIT_EMPTY_DISPOSABLE;

void CalcVolumeDischarged(void)
{
	int speed;
	THERAPY_TYPE TherType = GetTherapyType();
	if((timerCounterModBus%9) == 8)
	{
		if(TherType == LiverTreat)
			readPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress);
		else
		{
			// nel caso del rene leggo le pompe di ossigenazione
			readPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress);
		}
	}
	if(TherType == LiverTreat)
	{
		if(pumpPerist[3].dataReady == DATA_READY_TRUE)
		{
			// la velocita' ora posso leggerla direttamente dall'array di registry modbus
			speed = modbusData[pumpPerist[3].pmpMySlaveAddress-2][17] / 100;
			pumpPerist[3].actualSpeed = speed;
			VolumeDischarged = VolumeDischarged + ((float)speed * CONV_RPMMIN_TO_ML_PER_INTERVAL);
			pumpPerist[3].dataReady = DATA_READY_FALSE;
		}
	}
	else
	{
		if(pumpPerist[1].dataReady == DATA_READY_TRUE)
		{
			// la velocita' ora posso leggerla direttamente dall'array di registry modbus
			speed = modbusData[pumpPerist[1].pmpMySlaveAddress-2][17] / 100;
			pumpPerist[1].actualSpeed = speed;
			// moltiplico per due perche' in questo caso sono due le pompe che lavorano contemporaneamente
			VolumeDischarged = VolumeDischarged + ((float)speed * CONV_RPMMIN_TO_ML_PER_INTERVAL * 2);
			pumpPerist[1].dataReady = DATA_READY_FALSE;
		}
	}

	perfusionParam.unlVolRes = VolumeDischarged;
}

// I bottoni usati in questa funzione sono:
//        BUTTON_START_EMPTY_DISPOSABLE per partire o ripartire con lo scaricamento
//        BUTTON_START_TREATMENT        per tornare al trattamento
//        BUTTON_PRIMING_ABANDON        per abbandonare lo stato ed andare in idle
//        BUTTON_STOP_ALL_PUMP          per fermare momentaneamente tutte le pompe
void EmptyDispStateMach(void)
{
	THERAPY_TYPE TherType = GetTherapyType();
	int StarEmptyDispButId;
	int StopAllPumpButId;

	StarEmptyDispButId = BUTTON_START_OXYGEN_PUMP;    // BUTTON_START_EMPTY_DISPOSABLE;
	StopAllPumpButId = BUTTON_STOP_OXYGEN_PUMP;       // BUTTON_STOP_ALL_PUMP;

	switch (EmptyDispRunAlwaysState)
	{
		case INIT_EMPTY_DISPOSABLE:
			if(buttonGUITreatment[StarEmptyDispButId].state == GUI_BUTTON_RELEASED)
			{
				// attivo la pompa per iniziare ko svuotamento
				releaseGUIButton(StarEmptyDispButId);

				// attivazione della pompa di svuotamento
				if(GetTherapyType() == LiverTreat)
				{
					// nel caso del fegato e' la pompa di depurazione che svuota il recevoir
					setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, LIVER_PPAR_SPEED);
				}
				else if(GetTherapyType() == KidneyTreat)
				{
					// nel caso del rene sono le pompe di ossigenazione che svuotano il recevoir
					setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, KIDNEY_EMPTY_PPAR_SPEED);
				}
				// nel processo di svuotamento non mi servono tutti gli allarmi ma solo quelli di aria
				DisableAllAlarm();
				EmptyDispRunAlwaysState = WAIT_FOR_1000ML;
			}
			else if(buttonGUITreatment[BUTTON_START_TREATMENT].state == GUI_BUTTON_RELEASED)
			{
				// mi e' arrivato un comando per ritornare in trattamento
				// Non faccio la release perche', in questo modo, quando arriva nello stato
				// STATE_TREATMENT_KIDNEY_1 parte subito il trattamento
				//releaseGUIButton(BUTTON_START_TREATMENT);
				currentGuard[GUARD_ENABLE_TREATMENT_KIDNEY_1].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			}
			else if(buttonGUITreatment[BUTTON_PRIMING_ABANDON].state == GUI_BUTTON_RELEASED)
			{
				// nessuna pompa e' partita ancora
				releaseGUIButton(BUTTON_PRIMING_ABANDON);
				currentGuard[GUARD_ABANDON_PRIMING].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			}
			break;
		case WAIT_FOR_1000ML:
			if(!EmptyDisposStartOtherPump && VolumeDischarged >= DISCHARGE_AMOUNT_ART_PUMP)
			{
				// faccio partire le altre pompe per svuotare i tubi
				if(TherType == LiverTreat)
				{
					setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, LIVER_PPAR_SPEED);
					setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, LIVER_PPAR_SPEED);
				}
				else if(TherType == KidneyTreat)
					setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, KIDNEY_EMPTY_PPAR_SPEED);
				EmptyDisposStartOtherPump = TRUE;

				// abilito gli allarmi aria
				DisableAllAirAlarm(FALSE);
				EmptyDispRunAlwaysState = WAIT_FOR_AIR_ALARM;
			}
			else if(buttonGUITreatment[StopAllPumpButId].state == GUI_BUTTON_RELEASED)
			{
				releaseGUIButton(StopAllPumpButId);
				if(GetTherapyType() == LiverTreat)
					setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
				else if(GetTherapyType() == KidneyTreat)
					setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
			}
			else if(buttonGUITreatment[StarEmptyDispButId].state == GUI_BUTTON_RELEASED)
			{
				// faccio ripartire le pompe per lo svuotamento
				releaseGUIButton(StarEmptyDispButId);
				if(GetTherapyType() == LiverTreat)
					setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, LIVER_PPAR_SPEED);
				else if(GetTherapyType() == KidneyTreat)
					setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, KIDNEY_EMPTY_PPAR_SPEED);
			}
			else if(buttonGUITreatment[BUTTON_PRIMING_ABANDON].state == GUI_BUTTON_RELEASED)
			{
				releaseGUIButton(BUTTON_PRIMING_ABANDON);
				if(TherType == LiverTreat)
				{
					// se sono nel trattamento fegato fermo anche l'altro motore !!
					setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
				}
				else if(TherType == KidneyTreat)
					setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
				currentGuard[GUARD_ABANDON_PRIMING].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			}
			break;
		case WAIT_FOR_AIR_ALARM:
			// verificare che ci passa
			//if((ptrAlarmCurrent->code == CODE_ALARM_AIR_PRES_ART) && (ptrAlarmCurrent->active = ACTIVE_TRUE))
			if(IsDisposableEmpty())
			{
				if(TherType == LiverTreat)
				{
					// ho rilevato una presenza aria nel circuito di perfusione arteriosa e venosa
					// fermo le pompe e proseguo con lo svuotamento dall'ultimo liquido rimasto
					setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
					setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
					EmptyDispRunAlwaysState = WAIT_FOR_LEVEL_OR_AMOUNT;
				}
				else if(TherType == KidneyTreat)
				{
					// ho rilevato una presenza aria nel circuito di perfusione arteriosa
					setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
					// fermo la pompa e proseguo con lo svuotamento dall'ultimo liquido rimasto
					EmptyDispRunAlwaysState = WAIT_FOR_LEVEL_OR_AMOUNT;
				}
			}
			else if(buttonGUITreatment[StopAllPumpButId].state == GUI_BUTTON_RELEASED)
			{
				releaseGUIButton(StopAllPumpButId);
				if(GetTherapyType() == LiverTreat)
				{
					setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
					setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
					setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
				}
				else if(GetTherapyType() == KidneyTreat)
				{
					setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
					setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
				}
			}
			else if(buttonGUITreatment[StarEmptyDispButId].state == GUI_BUTTON_RELEASED)
			{
				// faccio ripartire le pompe per lo svuotamento
				releaseGUIButton(StarEmptyDispButId);
				if(GetTherapyType() == LiverTreat)
				{
					setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, LIVER_PPAR_SPEED);
					setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, LIVER_PPAR_SPEED);
					setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, LIVER_PPAR_SPEED);
				}
				else if(GetTherapyType() == KidneyTreat)
				{
					setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, KIDNEY_EMPTY_PPAR_SPEED);
					setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, KIDNEY_EMPTY_PPAR_SPEED);
				}
			}
			else if(buttonGUITreatment[BUTTON_PRIMING_ABANDON].state == GUI_BUTTON_RELEASED)
			{
				releaseGUIButton(BUTTON_PRIMING_ABANDON);
				// sono in uno stato di attesa allarme e tutte le pompe sono attive
				// se sono nel trattamento fegato devo fermare tutti i motori prima di abbandonare
				if(TherType == LiverTreat)
				{
					setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
					setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
					setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
				}
				else if(TherType == KidneyTreat)
				{
					setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, KIDNEY_EMPTY_PPAR_SPEED);
					setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
				}
				currentGuard[GUARD_ABANDON_PRIMING].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			}
			break;
		case WAIT_FOR_LEVEL_OR_AMOUNT:
			// rimango in questo stato, il controllo sul fine livello nella funzione ParentEmptyDispStateMach
			if(buttonGUITreatment[BUTTON_PRIMING_ABANDON].state == GUI_BUTTON_RELEASED)
			{
				releaseGUIButton(BUTTON_PRIMING_ABANDON);
				if(TherType == LiverTreat)
				{
					// se sono nel trattamento fegato fermo anche l'altro motore !!
					setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
				}
				else if(TherType == KidneyTreat)
					setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
				currentGuard[GUARD_ABANDON_PRIMING].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			}
			else if(buttonGUITreatment[StopAllPumpButId].state == GUI_BUTTON_RELEASED)
			{
				releaseGUIButton(StopAllPumpButId);
				if(GetTherapyType() == LiverTreat)
					setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
				else if(GetTherapyType() == KidneyTreat)
					setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
			}
			else if(buttonGUITreatment[StarEmptyDispButId].state == GUI_BUTTON_RELEASED)
			{
				// faccio ripartire le pompe per lo svuotamento
				releaseGUIButton(StarEmptyDispButId);
				if(GetTherapyType() == LiverTreat)
					setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, LIVER_PPAR_SPEED);
				else if(GetTherapyType() == KidneyTreat)
					setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, KIDNEY_EMPTY_PPAR_SPEED);
			}
			break;
	}
}


void manageParentEmptyDisposInitEntry(void)
{
	//inizializzo calcolo volume scaricato
	VolumeDischarged = 0;
	EmptyDisposStartOtherPump = FALSE;
	EmptyDispRunAlwaysState = INIT_EMPTY_DISPOSABLE;
	// attivazione della pompa di depurazione
//	if(GetTherapyType() == LiverTreat)
//		setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, LIVER_PPAR_SPEED);
//	else if(GetTherapyType() == KidneyTreat)
//		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, KIDNEY_EMPTY_PPAR_SPEED);
}
void manageParentEmptyDisposInitAlways(void)
{
	EmptyDispStateMach();
	CalcVolumeDischarged();
}

void manageParentEmptyDisposRunEntry(void)
{
	// attivazione della pompa di depurazione se dovessi ritornare da un allarme
//	if(GetTherapyType() == LiverTreat)
//		setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, LIVER_PPAR_SPEED);
//	else if(GetTherapyType() == KidneyTreat)
//		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, KIDNEY_EMPTY_PPAR_SPEED);
}
void manageParentEmptyDisposRunAlways(void)
{
	EmptyDispStateMach();
	CalcVolumeDischarged();

}

void manageParentEmptyDisposAlarmEntry(void)
{
}
void manageParentEmptyDisposAlarmAlways(void)
{
	EmptyDispStateMach();
	// durante l'allarme le pompe continuano a girare quindi
	// devo continuare a contare il volume scaricato
	CalcVolumeDischarged();
}

void manageParentEmptyDisposEndEntry(void)
{
	// fine del processo di svuotamento
	if(GetTherapyType() == LiverTreat)
	{
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
		setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
	}
	else if(GetTherapyType() == KidneyTreat)
	{
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
	}
}
void manageParentEmptyDisposEndAlways(void)
{
	EmptyDispStateMach();
}

// GESTIONE DEGLI STATI PER L'ELIMINAZIONE DELL'ALLARME ARIA-----------------------------------

void AirAlarmRecoveryStateMach(void)
{
	static unsigned long StarDelay = 0;
	THERAPY_TYPE TherType = GetTherapyType();
	switch (AirAlarmRecoveryState)
	{
		case INIT_AIR_ALARM_RECOVERY:
			AirAlarmRecoveryState = START_AIR_PUMP;
			break;
		case START_AIR_PUMP:
			if(AirParentState == PARENT_TREAT_KIDNEY_1_AIR_FILT)
			{
				if(TherType == KidneyTreat)
					setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, AIR_REJECT_SPEED);
				else if(TherType == LiverTreat)
					setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, AIR_REJECT_SPEED);
				AirAlarmRecoveryState = AIR_CHANGE_START_TIME;
			}
			else if(AirParentState == PARENT_TREAT_KIDNEY_1_SFV)
			{
				// faccio partire la coppia di pompe per la venosa nel fegato e per l'ossigenazione nel rene
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, AIR_REJECT_SPEED);
				AirAlarmRecoveryState = AIR_CHANGE_START_TIME;
			}
			else if(AirParentState == PARENT_TREAT_KIDNEY_1_SFA)
			{
				// parte la sempre la pompa a cui si riferisce la struttura pumpPerist 0
				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, AIR_REJECT_SPEED);
				AirAlarmRecoveryState = AIR_CHANGE_START_TIME;
			}
			break;
		case AIR_CHANGE_START_TIME:
			if((AirParentState == PARENT_TREAT_KIDNEY_1_AIR_FILT) && (Air_1_Status == LIQUID))
			{
				//  la bolla se ne e' andata faccio; ripartire il timer per misurare il tempo a partire da ora
				StarTimeToRejAir = timerCounterModBus;
				AirAlarmRecoveryState = STOP_AIR_PUMP;
			}
			else if((AirParentState == PARENT_TREAT_KIDNEY_1_SFV) && (sensor_UFLOW[VENOUS_AIR_SENSOR].bubbleSize < 25))
			{
				//  la bolla se ne e' andata faccio; ripartire il timer per misurare il tempo a partire da ora
				StarTimeToRejAir = timerCounterModBus;
				AirAlarmRecoveryState = STOP_AIR_PUMP;
			}
			else if((AirParentState == PARENT_TREAT_KIDNEY_1_SFA) && (sensor_UFLOW[ARTERIOUS_AIR_SENSOR].bubbleSize < 25))
			{
				//  la bolla se ne e' andata faccio; ripartire il timer per misurare il tempo a partire da ora
				StarTimeToRejAir = timerCounterModBus;
				AirAlarmRecoveryState = STOP_AIR_PUMP;
			}
			break;
		case STOP_AIR_PUMP:
			if(StarTimeToRejAir && ((msTick_elapsed(StarTimeToRejAir) + TotalTimeToRejAir) * 50L >= TIME_TO_REJECT_AIR))
			{
				// la pompa per l'espulsione dell'aria ha fatto 10 giri, che ritengo sufficienti per
				// buttare fuori l'aria
				if(AirParentState == PARENT_TREAT_KIDNEY_1_AIR_FILT)
				{
					if(TherType == KidneyTreat)
						setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
					else if(TherType == LiverTreat)
						setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
					AirAlarmRecoveryState = AIR_REJECTED;
					//currentGuard[GUARD_AIR_RECOVERY_END].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
					StarDelay = timerCounterModBus;
				}
				else if(AirParentState == PARENT_TREAT_KIDNEY_1_SFV)
				{
					// fermo la coppia di pompe per la venosa nel fegato e per l'ossigenazione nel rene
					setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
					AirAlarmRecoveryState = AIR_REJECTED;
					//currentGuard[GUARD_AIR_RECOVERY_END].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
					StarDelay = timerCounterModBus;
				}
				else if(AirParentState == PARENT_TREAT_KIDNEY_1_SFA)
				{
					// fermo la pompa a cui si riferisce la struttura pumpPerist 0
					setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
					AirAlarmRecoveryState = AIR_REJECTED;
					//currentGuard[GUARD_AIR_RECOVERY_END].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
					StarDelay = timerCounterModBus;
				}
			}
			break;
		case AIR_REJECTED:
			if(StarDelay && ((msTick_elapsed(StarDelay) * 50L) >= 2000))
			{
				AirAlarmRecoveryState = AIR_REJECTED1;
				currentGuard[GUARD_AIR_RECOVERY_END].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			}
			break;
		case AIR_REJECTED1:
			break;
	}
}
// gestione dello stato PARENT_TREAT_KIDNEY_1_AIR_FILT
// faccio fare un cero numero di giri alla pompa 2 fino a quando
// l'aria on arriva al reservoir
void manageParentTreatAirFiltEntry(void)
{
	AirAlarmRecoveryState = INIT_AIR_ALARM_RECOVERY;
}
void manageParentTreatAirFiltAlways(void)
{
	AirAlarmRecoveryStateMach();
}


// gestione dello stato PARENT_TREAT_KIDNEY_1_SFV
// faccio fare un cero numero di giri alle pompe 4 e 5 fino a quando
// l'aria on arriva al reservoir
void manageParentTreatSFVEntry(void)
{
	AirAlarmRecoveryState = INIT_AIR_ALARM_RECOVERY;
}
void manageParentTreatSFVAlways(void)
{
	AirAlarmRecoveryStateMach();
}


// gestione dello stato PARENT_TREAT_KIDNEY_1_SFA
// faccio fare un cero numero di giri alla pompa 3 fino a quando
// l'aria on arriva al reservoir
// Questa e' usata solo nel fegato
void manageParentTreatSFAEntry(void)
{
	AirAlarmRecoveryState = INIT_AIR_ALARM_RECOVERY;
}
void manageParentTreatSFAAlways(void)
{
	AirAlarmRecoveryStateMach();
}


// procedura di allarme durante una fase di recupero da un allarme aria
void manageParentTreatAirAlmRecEntry(void)
{
}
// procedura di allarme durante una fase di recupero da un allarme aria
void manageParentTreatAirAlmRecAlways(void)
{
}
// ------------FINE STATI ELIMINAZIONE ALLARME ARIA---------------------------------



/* CHILD LEVEL FUNCTION */

void manageChildNull(void)
{
	unsigned short dummy = 0;
	#ifdef DEBUG_TREATMENT
	dummy = dummy + 1;
	#endif
}

void manageChildEntry(void)
{
}

void manageChildEntryAlways(void)
{
	#ifdef DEBUG_ENABLE
	static int index2 = 0;
	index2++;
	#endif
}

/* CORE FUNCTION */

/*-----------------------------------------------------------*/
/* This function manages the communication with the pc board */
/*-----------------------------------------------------------*/
void manageCommWithPc(void)
{
}

/*----------------------------------------------------------------------------*/
/* This function manages the communication with the protection microprocessor */
/*----------------------------------------------------------------------------*/
void manageCommWithProtection(void)
{
}

/*----------------------------------------------------------------------------*/
/* This function reads the analog sensor on the control microprocessor        */
/*----------------------------------------------------------------------------*/
void readAnalogSensor(void)
{
}

/*----------------------------------------------------------------------------*/
/* This function reads the digital sensor on the control microprocessor       */
/*----------------------------------------------------------------------------*/
void readDigitalSensor(void)
{
}


/*--------------------------------------------------------*/
/*  This function compute the guard value in entry state   */
/*--------------------------------------------------------*/
static void computeMachineStateGuardEntryState(void){
	if((currentGuard[GUARD_HW_T1T_DONE].guardValue == GUARD_VALUE_TRUE) &&
		(currentGuard[GUARD_COMM_ENABLED].guardValue == GUARD_VALUE_TRUE))
	{
		currentGuard[GUARD_ENABLE_STATE_IDLE].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
	}
}

/*--------------------------------------------------------*/
/*  This function compute the guard value in idle state   */
/*--------------------------------------------------------*/
static void computeMachineStateGuardIdle(void){
	//only the significant guard for the current state are computed --> to be evaluated: for each state a function is required to evaluate the guard
	if(parameterWordSetFromGUI[PAR_SET_THERAPY_TYPE].value == (word)KidneyTreat)
	{
		SetTherapyType(KidneyTreat);
		if(TherapyCmdArrived)
		{
			UpdatePmpAddress(GetTherapyType());
			currentGuard[GUARD_ENABLE_SELECT_TREAT_PAGE].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			TherapyCmdArrived = 0;
		}
	}
	else if (parameterWordSetFromGUI[PAR_SET_THERAPY_TYPE].value == (word)LiverTreat)
	{
		SetTherapyType(LiverTreat);
		if(TherapyCmdArrived)
		{
			UpdatePmpAddress(GetTherapyType());
			currentGuard[GUARD_ENABLE_SELECT_TREAT_PAGE].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			TherapyCmdArrived = 0;
		}
	}
}

/*--------------------------------------------------------------------*/
/*  This function compute the guard value in select treatment state   */
/*--------------------------------------------------------------------*/
static void computeMachineStateGuardSelTreat(void){
	if(
		(buttonGUITreatment[BUTTON_EN_PERFUSION].state == GUI_BUTTON_RELEASED) &&
		(buttonGUITreatment[BUTTON_EN_OXYGENATION].state == GUI_BUTTON_RELEASED) &&
		(buttonGUITreatment[BUTTON_CONFIRM].state == GUI_BUTTON_RELEASED)
	 )
	{
		currentGuard[GUARD_ENABLE_MOUNT_DISPOSABLE].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		releaseGUIButton(BUTTON_EN_PERFUSION);
		releaseGUIButton(BUTTON_EN_OXYGENATION);
		releaseGUIButton(BUTTON_CONFIRM);
	}
}

/*--------------------------------------------------------------------*/
/*  This function compute the guard value in mounting disposable state   */
/*--------------------------------------------------------------------*/
static void computeMachineStateGuardMountDisp(void)
{
}

/*--------------------------------------------------------------------*/
/*  This function compute the guard value in tank fill state   */
/*--------------------------------------------------------------------*/
static void computeMachineStateGuardTankFill(void)
{
}

/*--------------------------------------------------------------------*/
/*  This function compute the guard value in priming phase 1 state   */
/*--------------------------------------------------------------------*/
static void computeMachineStateGuardPrimingPh1(void){

//	if( buttonGUITreatment[BUTTON_PRIMING_FILT_INS_CONFIRM].state == GUI_BUTTON_RELEASED)
//	{
//		FilterSelected = TRUE;
//		currentGuard[GUARD_ENABLE_PRIMING_PHASE_2].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
//		releaseGUIButton(BUTTON_PRIMING_FILT_INS_CONFIRM);
//	}
//	else
//	if(!StartPrimingTime)
//	{
//		// prendo il tempo di start del priming solo se il valore vale 0, cioe' sono partito da IDLE
//		StartPrimingTime = (unsigned long)timerCounterModBus;
//	}

	// tempo trascorso di priming in sec
	if(StartPrimingTime)
		PrimingDuration = msTick_elapsed(StartPrimingTime) * 50L / 1000;

	if(buttonGUITreatment[BUTTON_PRIMING_ABANDON].state == GUI_BUTTON_RELEASED)
	{
		releaseGUIButton(BUTTON_PRIMING_ABANDON);
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
		if(GetTherapyType() == LiverTreat)
		{
			// se sono nel trattamento fegato fermo anche l'altro motore !!
			setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
		}
		currentGuard[GUARD_ABANDON_PRIMING].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
	}
	else
	{
		// faccio partire l'ossigenazione e la pompa di depurazione in liver
		if((GetTherapyType() == KidneyTreat) && (((PARAMETER_ACTIVE_TYPE)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_ACTIVE].value) == YES) &&
		   (perfusionParam.priVolPerfArt > MIN_LIQ_IN_RES_TO_START_OXY_VEN) && (StartOxygAndDepState == 0))
		{
			// sono nel priming rene con ossigenatore abilitato ed ho superato una quantita' minima nel reservoir
			setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress,
					                  (int)((float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / OXYG_FLOW_TO_RPM_CONV * 100.0));
			StartOxygAndDepState = 1;
		}
		else if((GetTherapyType() == LiverTreat) && (perfusionParam.priVolPerfArt > MIN_LIQ_IN_RES_TO_START_OXY_VEN) && (StartOxygAndDepState == 0))
		{
			// sono nel priming fegato ed ho superato una quantita' minima nel reservoir, quindi, faccio partire la pompa venosa
			// per il riempimento del disposable di ossigenazione e la pompa di depurazione PPAR
			if(!pumpPerist[1].actualSpeed)
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, (int)LIVER_PRIMING_PMP_OXYG_SPEED);
			if(!pumpPerist[3].actualSpeed)
				setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, LIVER_PPAR_SPEED);
			StartOxygAndDepState = 1;
		}

		// controllo se e' cambiata la velocita' della pompa di ossigenazione e la aggiorno
		CheckOxygenationSpeed(LastOxygenationSpeed);
	}
}

/*--------------------------------------------------------------------*/
/*  This function compute the guard value in priming phase 2 state   */
/*  Controllo quando iniziare il trattamento
/*--------------------------------------------------------------------*/
static void computeMachineStateGuardPrimingPh2(void){

//	if(!StartPrimingTime)
//	{
//		// prendo il tempo di start del priming solo se il valore vale 0, cioe' sono partito da IDLE
//		StartPrimingTime = (unsigned long)timerCounterModBus;
//	}

	// tempo trascorso di priming in sec
	if(StartPrimingTime)
		PrimingDuration = msTick_elapsed(StartPrimingTime) * 50L / 1000;

	if(buttonGUITreatment[BUTTON_PRIMING_ABANDON].state == GUI_BUTTON_RELEASED)
	{
		releaseGUIButton(BUTTON_PRIMING_ABANDON);
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
		if(GetTherapyType() == LiverTreat)
		{
			// se sono nel trattamento fegato fermo anche l'altro motore !!
			setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
		}
		currentGuard[GUARD_ABANDON_PRIMING].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
	}
	else
	{
		// faccio partire l'ossigenazione e la pompa di depurazione in liver
		if((GetTherapyType() == KidneyTreat) && (((PARAMETER_ACTIVE_TYPE)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_ACTIVE].value) == YES) &&
		   (perfusionParam.priVolPerfArt > MIN_LIQ_IN_RES_TO_START_OXY_VEN) && (StartOxygAndDepState == 0))
		{
			// sono nel priming rene con ossigenatore abilitato ed ho superato una quantita' minima nel reservoir
			setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress,
					                  (int)((float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / OXYG_FLOW_TO_RPM_CONV * 100.0));
			StartOxygAndDepState = 1;
		}
		else if((GetTherapyType() == LiverTreat) && (perfusionParam.priVolPerfArt > MIN_LIQ_IN_RES_TO_START_OXY_VEN) && (StartOxygAndDepState == 0))
		{
			// sono nel priming fegato ed ho superato una quantita' minima nel reservoir, quindi, la pompa venosa
			// per il riempimento del disposable di ossigenazione e la pompa di depurazione PPAR
			if(!pumpPerist[1].actualSpeed)
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, (int)LIVER_PRIMING_PMP_OXYG_SPEED);
			if(!pumpPerist[3].actualSpeed)
				setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, LIVER_PPAR_SPEED);
			StartOxygAndDepState = 1;
		}
		// controllo se e' cambiata la velocita' della pompa di ossigenazione e la aggiorno
		CheckOxygenationSpeed(LastOxygenationSpeed);
	}
}


int computeMachineStateGuardTreatmentState = 0;
/*--------------------------------------------------------------------*/
/*  This function compute the guard value in treatment kidney 1 state   */
/*  Controllo la fine del trattamento
/*--------------------------------------------------------------------*/
static void computeMachineStateGuardTreatment(void)
{
//	static int EndTreatmentState = 0;
//	static unsigned long StartTimeout = 0;
//	unsigned long ul;

	//stoppo il conteggio del timer del priming
	StartPrimingTime = 0;

	// tempo trascorso di trattamento in sec
	if(StartTreatmentTime)
		TreatDuration = msTick_elapsed(StartTreatmentTime) * 50L / 1000;

	// DA INSERIRE QUANDO VOGLIO USCIRE ED ANDARE ALLO SVUOTAMENTO
//	if((buttonGUITreatment[BUTTON_STOP_ALL_PUMP].state == GUI_BUTTON_RELEASED) &&
//	   (buttonGUITreatment[BUTTON_CONFIRM].state == GUI_BUTTON_RELEASED))
//	{
//		// passo allo svuotamento del circuito
//		currentGuard[GUARD_ENABLE_DISPOSABLE_EMPTY].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
//		releaseGUIButton(BUTTON_STOP_ALL_PUMP);
//		releaseGUIButton(BUTTON_CONFIRM);
//	}
//	else

	unsigned long ival = (unsigned long)(parameterWordSetFromGUI[PAR_SET_DESIRED_DURATION].value >> 8);
	unsigned long isec;
	isec = ival * 3600L;      // numero di ore
	ival = (unsigned long)(parameterWordSetFromGUI[PAR_SET_DESIRED_DURATION].value & 0xff);
	isec = isec + ival * 60L; // numero di minuti
	int newSpeedPmp_0 = 0;
	int newSpeedPmp1_2 = 0;
	int newSpeedPmp_3 = 0;

	if((TotalTreatDuration + TreatDuration) >= isec)
	{
		if(!computeMachineStateGuardTreatmentState)
		{
			setPumpPressLoop(0, PRESS_LOOP_OFF);
			// esaurita la durata massima del trattamento
			// forzo lo stop alle pompe passo allo svuotamento del circuito
			setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, newSpeedPmp_0);
			setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, newSpeedPmp1_2);
			if(GetTherapyType() == LiverTreat)
			{
				// se sono nel trattamento fegato fermo anche l'altro motore !!
				setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, newSpeedPmp_3);
				setPumpPressLoop(1, PRESS_LOOP_OFF);
			}
			computeMachineStateGuardTreatmentState++;
		}
		else if(computeMachineStateGuardTreatmentState && ((TotalTreatDuration + TreatDuration) >= (isec + computeMachineStateGuardTreatmentState)))
		{
			// ci riprovo fino a 5 volte
			setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, newSpeedPmp_0);
			setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, newSpeedPmp1_2);
			if(GetTherapyType() == LiverTreat)
			{
				// se sono nel trattamento fegato fermo anche l'altro motore !!
				setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, newSpeedPmp_3);
			}
			computeMachineStateGuardTreatmentState++;
		}

		if ( CommandModBusPMPExecute(newSpeedPmp_0,newSpeedPmp1_2,newSpeedPmp_3) )
		{
			currentGuard[GUARD_ENABLE_DISPOSABLE_EMPTY].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			if (PeltierOn && (peltierCell.StopEnable == 0))
			{
				// se erano accese le spengo
				peltierCell.StopEnable = 1;
				peltierCell2.StopEnable = 1;
			}
			DebugStringStr("TREATMENT END");
		}
		else
		{
			// controllo il tempo trascorso per un eventuale timeout di errore
			if((TotalTreatDuration + TreatDuration) >= (isec + 5))
			{
				// se sono trascorsi 5 secondi dalla fine del trattamento e non sono ancora uscito.
				// la forzo io
				currentGuard[GUARD_ENABLE_DISPOSABLE_EMPTY].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
				if (PeltierOn && (peltierCell.StopEnable == 0))
				{
					// se erano accese le spengo
					peltierCell.StopEnable = 1;
					peltierCell2.StopEnable = 1;
				}
				DebugStringStr("TREATMENT END");
			}
		}
	}
	else
	{
		computeMachineStateGuardTreatmentState = 0;
		//QUESTO NON LO DEVO FARE PERCHE' IL FLUSSO E' CONTROLLATO DAL PID DURANTE IL TRATTAMENTO !!!!!!!
		if(GetTherapyType() == KidneyTreat)
		{
			// controllo se e' cambiata la velocita' della pompa di ossigenazione e la aggiorno
			CheckOxygenationSpeed(LastOxygenationSpeed);
		}
	}

/*aggiunta gestione con timer per cambio stato --> da testare*/
//	if( (EndTreatmentState == 0) && (TreatDuration >= isec) )
//	{
//		// esaurita la durata massima del trattamento
//		// forzo lo stop alle pompe passo allo svuotamento del circuito
//		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, newSpeedPmp_0);
//		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, newSpeedPmp1_2);
//		StartTimeout = (unsigned long)timerCounterModBus;
//
//		EndTreatmentState = 1;
//	}
//	else if(EndTreatmentState == 1)
//	{
//		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, newSpeedPmp_0);
//		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, newSpeedPmp1_2);
//		if ( CommandModBusPMPExecute(newSpeedPmp_0,newSpeedPmp1_2,newSpeedPmp_3) )
//		{
//			currentGuard[GUARD_ENABLE_DISPOSABLE_EMPTY].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
//			DebugStringStr("TREATMENT END");
//			EndTreatmentState = 0;
//		}
//		else
//		{
//			// controllo il tempo trascorso per un eventuale timeout di errore
//			ul = msTick_elapsed(StartTimeout) * 50L;
//			if(ul > 100)
//			{
//				// trascorsi 100 msec do un allarme
//			}
//		}
//	}

}


/*--------------------------------------------------------------------*/
/*  This function manage the entry and always function for each state */
/*--------------------------------------------------------------------*/
static void manageStateEntryAndStateAlways(unsigned short stateId){
	if(ptrCurrentState->action == ACTION_ON_ENTRY)
	{
		/* execute state callback function */
		ptrCurrentState->callBackFunct();
		/* compute future state */
		ptrFutureState = &stateState[stateId];
		/* execute parent callback function */
		ptrCurrentParent->callBackFunct();
		/* compute future parent */
		ptrFutureParent = ptrFutureState->ptrParent;
		/* execute child callback function */
		ptrCurrentChild->callBackFunct();
		/* compute future child */
		ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
		actionFlag = 1;
	}
	else if(ptrCurrentState->action == ACTION_ALWAYS)
	{
		ptrCurrentState->callBackFunct();

		ptrCurrentParent->callBackFunct();
		//ptrFutureParent = ptrCurrentState->ptrParent;

		ptrCurrentChild->callBackFunct();
	}
}

/*----------------------------------------------------------------------------*/
/* This function compute the guard to be used for machine state transition    */
/*----------------------------------------------------------------------------*/
void computeMachineStateGuard(void)
{
	/* compute guard */
	for(int i = 0; i <= GUARD_END_NUMBER; i++)
		{
			if(currentGuard[i].guardEntryValue == GUARD_ENTRY_VALUE_TRUE)
			{
				currentGuard[i].guardValue = GUARD_VALUE_TRUE;

			}
			else if(currentGuard[i].guardEntryValue == GUARD_ENTRY_VALUE_FALSE &&
					currentGuard[i].guardValue != GUARD_VALUE_FALSE ){
				currentGuard[i].guardValue = GUARD_VALUE_FALSE;
			}
		}
}



// macchina a stati del processo Parent da usare nello stato STATE_EMPTY_DISPOSABLE
void ParentEmptyDispStateMach(void)
{
	switch(ptrCurrentParent->parent)
	{
		case PARENT_EMPTY_DISPOSABLE_INIT:
			if(VolumeDischarged >= 100)
			{
				/* ho gia' scaricato una certa quantita' di liquido dal reservoir,
				 * passo nella fase run */
				ptrFutureParent = &stateParentEmptyDisp[3];
				ptrFutureChild = ptrFutureParent->ptrChild;
				break;
			}

			if(ptrCurrentParent->action == ACTION_ON_ENTRY)
			{
				/* compute future parent */
				/* FM entro nello stato in cui l'azione e' di tipo ACTION_ALWAYS */
				ptrFutureParent = &stateParentEmptyDisp[2];
				ptrFutureChild = ptrFutureParent->ptrChild;
			}
			else if(ptrCurrentParent->action == ACTION_ALWAYS)
			{
			}

			if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_TRUE)
			{
				/* (FM) si e' verificato un allarme, passo alla sua gestione */
				ptrFutureParent = &stateParentEmptyDisp[5];
				ptrFutureChild = ptrFutureParent->ptrChild;
			}
			break;

		case PARENT_EMPTY_DISPOSABLE_RUN:
			if((VolumeDischarged >= (perfusionParam.priVolPerfArt + (float)10.0 * (float)perfusionParam.priVolPerfArt / 100.0)) &&
			   (PR_LEVEL_mmHg_Filtered <= 0))
			{
				// ho scaricato tutto, mi fermo
				ptrFutureParent = &stateParentEmptyDisp[7];
				ptrFutureChild = ptrFutureParent->ptrChild;
				currentGuard[GUARD_EMPTY_DISPOSABLE_END].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
				break;
			}

			if(ptrCurrentParent->action == ACTION_ON_ENTRY)
			{
				/* compute future parent */
				/* FM passo alla gestione ACTION_ALWAYS */
				ptrFutureParent = &stateParentEmptyDisp[4];
				ptrFutureChild = ptrFutureParent->ptrChild;
			}
			else if(ptrCurrentParent->action == ACTION_ALWAYS)
			{
			}

			if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_TRUE)
			{
				/* (FM) si e' verificato un allarme, passo alla sua gestione */
				ptrFutureParent = &stateParentEmptyDisp[5];
				ptrFutureChild = ptrFutureParent->ptrChild;
			}
			break;

		case PARENT_EMPTY_DISPOSABLE_ALARM:
			if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_FALSE)
			{
				/* FM allarme finito posso ritornare nella fase run dello scarico */
				ptrFutureParent = &stateParentEmptyDisp[3];
				ptrFutureChild = ptrFutureParent->ptrChild;
				break;
			}

			if(ptrCurrentParent->action == ACTION_ON_ENTRY)
			{
				/* (FM) passo alla gestione ACTION_ALWAYS dell'allarme */
				ptrFutureParent = &stateParentTreatKidney1[6];
				ptrFutureChild = ptrFutureParent->ptrChild;
			}
			else if(ptrCurrentParent->action == ACTION_ALWAYS)
			{
				// chiamo la funzione child che gestisce lo stato di allarme durante la fase di svuotamento
				ManageStateChildAlarmEmpty();
			}
			break;

		case PARENT_EMPTY_DISPOSABLE_END:
			break;
	}
}

// gestisce il passaggio dallo stato di allarme ad un nuovo stato parent dove si
// cerchera' di eliminare la condizione di allarme.
void GoToRecoveryParentState(int MachineParentState)
{
	switch(MachineParentState)
	{
		case PARENT_TREAT_KIDNEY_1_SFA:
			AirParentState = PARENT_TREAT_KIDNEY_1_SFA;
			StarTimeToRejAir = timerCounterModBus;
			TotalTimeToRejAir = 0;
			/* (FM) passo nello stato per la risoluzione dell'allarme aria nella linea arteriosa*/
			ptrFutureParent = &stateParentTreatKidney1[11];
			ptrFutureChild = ptrFutureParent->ptrChild;
			break;
		case PARENT_TREAT_KIDNEY_1_SFV:
			AirParentState = PARENT_TREAT_KIDNEY_1_SFV;
			StarTimeToRejAir = timerCounterModBus;
			TotalTimeToRejAir = 0;
			/* (FM) passo nello stato per la risoluzione dell'allarme aria nella linea venosa*/
			ptrFutureParent = &stateParentTreatKidney1[9];
			ptrFutureChild = ptrFutureParent->ptrChild;
			break;
		case PARENT_TREAT_KIDNEY_1_AIR_FILT:
			AirParentState = PARENT_TREAT_KIDNEY_1_AIR_FILT;
			StarTimeToRejAir = timerCounterModBus;
			TotalTimeToRejAir = 0;
			/* (FM) passo nello stato per la risoluzione dell'allarme aria nella linea del filtro*/
			ptrFutureParent = &stateParentTreatKidney1[7];
			ptrFutureChild = ptrFutureParent->ptrChild;
			break;
	}
}


word GetTotalPrimingVolumePerf(int cmd)
{
	word TotVolume;
	TotVolume = parameterWordSetFromGUI[PAR_SET_PRIMING_VOL_PERFUSION].value + VOLUME_DISPOSABLE;
	return TotVolume;
}



// ritorna il volume complessivo di priming tenendo conto anche del volume di liquido
// necessario per riempire il disposable
word GetTotalPrimingVolumePerf_new(int cmd)
{
	static TOTAL_PRIMING_VOL_STATE TotalPrimingVolState = INIT_TOT_PRIM_VOL_STATE;
	static word TotalPrimingVolume = 0;
	word TotVolume = 0;
	// original code
	//TotVolume = parameterWordSetFromGUI[PAR_SET_PRIMING_VOL_PERFUSION].value + VOLUME_DISPOSABLE;

	switch (TotalPrimingVolState)
	{
		case INIT_TOT_PRIM_VOL_STATE:
			// ritorna il volume da caricare nel primo priming
			TotVolume = parameterWordSetFromGUI[PAR_SET_PRIMING_VOL_PERFUSION].value + VOLUME_DISPOSABLE;
			if(cmd = NEW_PRIM_CMD_TOT_PRIM_VOL)
			{
				TotalPrimingVolume = perfusionParam.priVolPerfArt;
				TotalPrimingVolState = NEW_TOT_PRIM_VOL_STATE;
			}
			else if(cmd = RESET_CMD_TOT_PRIM_VOL)
			{
				TotalPrimingVolume = 0;
			}
			break;
		case NEW_TOT_PRIM_VOL_STATE:
			// ritorna il volume totale nei priming successivi
			TotVolume = TotalPrimingVolume + parameterWordSetFromGUI[PAR_SET_PRIMING_VOL_PERFUSION].value;
			if(cmd = NEW_PRIM_CMD_TOT_PRIM_VOL)
				TotalPrimingVolume = perfusionParam.priVolPerfArt;
			else if(cmd = RESET_CMD_TOT_PRIM_VOL)
			{
				TotalPrimingVolume = 0;
				TotalPrimingVolState = INIT_TOT_PRIM_VOL_STATE;
			}
			break;
	}
	return TotVolume;
}


/*----------------------------------------------------------------------------*/
/* This function compute the machine state transition based on guard - state level         */
/*----------------------------------------------------------------------------*/
void processMachineState(void)
{
	if(buttonGUITreatment[BUTTON_SILENT_ALARM].state == GUI_BUTTON_RELEASED)
	{
		if(LevelBuzzer)
			LevelBuzzer = 0;
		releaseGUIButton(BUTTON_SILENT_ALARM);
	}

	/* process state structure --> in base alla guard si decide lo stato --> in base allo stato si eseguono certe funzioni in modalità init o always */
	switch(ptrCurrentState->state)
	{
		case STATE_NULL:
			/* compute future state */
			if(currentGuard[GUARD_START_ENABLE].guardValue == GUARD_VALUE_TRUE)
			{
				/* compute future state */
				ptrFutureState = &stateState[1];
				/* compute future parent */
				ptrFutureParent = ptrFutureState->ptrParent;
				/* compute future child */
				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
			}

			/* execute function state level */
			if(ptrCurrentState->action == ACTION_ON_ENTRY)
			{
				ptrCurrentState->callBackFunct();
				actionFlag = 1;

				// (FM) faccio in modo che dopo un aprima inizializzazione esca subito e vada
				// nello stato STATE_ENTRY
				if( currentGuard[GUARD_START_ENABLE].guardValue != GUARD_VALUE_TRUE)
					currentGuard[GUARD_START_ENABLE].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			}

			/* execute function parent and child level */
			//NONE

			#ifdef DEBUG_ENABLE
			static int index = 0;
			index++;
			#endif

			break;

		case STATE_ENTRY:
			/* compute future state */
			if((currentGuard[GUARD_HW_T1T_DONE].guardValue == GUARD_VALUE_TRUE) &&
				(currentGuard[GUARD_COMM_ENABLED].guardValue == GUARD_VALUE_TRUE))
			{
				/* (FM) VADO NELLO STATO IDLE,ACTION_ON_ENTRY DATO CHE LA FASE INIZIALE DI TEST E' FINITA */
				/* compute future state */
				ptrFutureState = &stateState[3];
				/* compute future parent */
				ptrFutureParent = ptrFutureState->ptrParent;
				/* compute future child */
				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
				DebugStringStr("IDLE");
			}

			/* compute parent......compute child */
			if(ptrCurrentParent->action == ACTION_ON_ENTRY)
			{
				/* execute parent callback function */
				// non serve qui verra' chiamata nella manageStateEntryAndStateAlways
				//ptrCurrentParent->callBackFunct();
				/* compute future parent */
				ptrFutureParent = &stateParentEntry[2];
			}
			else if(ptrCurrentParent->action == ACTION_ALWAYS)
			{
				// non serve qui verra' chiamata nella manageStateEntryAndStateAlways
				// ptrCurrentParent->callBackFunct();
			}

			if(ptrCurrentChild->child == CHILD_ENTRY)
			{
				if(ptrCurrentChild->action == ACTION_ON_ENTRY)
				{
					/* (FM) esegue la parte ACTION_ON_ENTRY dello stato entry - child */
					// non serve qui verra' chiamata nella manageStateEntryAndStateAlways
					//ptrCurrentChild->callBackFunct();
					ptrFutureChild = &stateChildEntry[2];
				}
				else if(ptrCurrentChild->action == ACTION_ALWAYS){
					/* (FM) esegue la parte ACTION_ALWAYS dello stato entry - child */
					// non serve qui verra' chiamata nella manageStateEntryAndStateAlways
					// ptrCurrentChild->callBackFunct();
				}
			}

			/* execute function state level */
            /* (FM) DOPO AVER ESEGUITO IL CODICE IN ENTRATA DELLO STATO (ACTION_ON_ENTRY) SI SPOSTA NELLO STATO ACTION_ALWAYS
               E LI' RIMANE FINO A QUANDO NON GLI ARRIVA UN NUOVO COMANDO */
			manageStateEntryAndStateAlways(2);
			break;

		case STATE_IDLE:
			/* compute future state */
			if(
				(currentGuard[GUARD_ENABLE_SELECT_TREAT_PAGE].guardValue == GUARD_VALUE_TRUE)
				)
			{
				currentGuard[GUARD_ENABLE_SELECT_TREAT_PAGE].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
			    /* (FM) HO RICEVUTO UN COMANDO (DA TASTIERA O SERIALE) CHE MI CHIEDE DI ENTRARE NELLO STATO DI
			       SELEZIONE DEL TRATTAMENTO */

				/* compute future state */
				// provvisoriamente per provare il pid passo direttamente allo stato di trattamento con il tasto 3
				// della tastiera a bolle
				//ptrFutureState = &stateState[17];

				// Passo direttamente allo stato mounting disposable senza passare per STATE_SELECT_TREAT
				// perche' il trattamento e' gia' stato selezionato.
				ptrFutureState = &stateState[9];
				/* compute future parent */
				ptrFutureParent = ptrFutureState->ptrParent;
				/* compute future child */
				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
				DebugStringStr("STATE_MOUNTING_DISP");
			}
			/* execute function state level */
			// (FM) DOPO LA PRIMA VOLTA PASSA AUTOMATICAMENTE NELLO STATO IDLE,ACTION_ALWAYS
			manageStateEntryAndStateAlways(4);

//			#ifdef DEBUG_ENABLE
//			Bit1_NegVal();
//			TestPump(3, 1000 );
//			#endif

			break;

		case STATE_SELECT_TREAT:     // NON E' USATO CON LA GESTIONE DA SERVICE
			/* compute future state */
			if( (currentGuard[GUARD_ENABLE_MOUNT_DISPOSABLE].guardValue == GUARD_VALUE_TRUE) )
			{
				currentGuard[GUARD_ENABLE_MOUNT_DISPOSABLE].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				/* (FM) E' ARRIVATO UN COMANDO CHE MI CHIEDE DI PASSARE ALLA FASE DI INSTALLAZIONE DEL DISPOSABLE */
				/* compute future state */
				ptrFutureState = &stateState[9];
				/* compute future parent */
				ptrFutureParent = ptrFutureState->ptrParent;
				/* compute future child */
				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
				DebugStringStr("STATE_MOUNTING_DISP");
			}

			/* execute function state level */
			manageStateEntryAndStateAlways(6);
			break;

		case STATE_T1_NO_DISPOSABLE:
			break;

		case STATE_MOUNTING_DISP:
			if(
				(currentGuard[GUARD_ENABLE_PRIMING_PHASE_1].guardValue == GUARD_VALUE_TRUE)
				)
			{
				currentGuard[GUARD_ENABLE_PRIMING_PHASE_1].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
			    /* (FM) FINITA LA FASE DI MONTAGGIO DEL DISPOSABLE, POSSO PASSARE ALLA FASE DI RIEMPIMENTO
			       DEL RECIPIENTE */
				/* compute future state */
				ptrFutureState = &stateState[11];
				/* compute future parent */
				ptrFutureParent = ptrFutureState->ptrParent;
				/* compute future child */
				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
				DebugStringStr("STATE_TANK_FILL");
			}

			/* execute function state level */
			manageStateEntryAndStateAlways(10);
			break;

		// ORA DA QUESTO STATO PASSO DIRETTAMENTE ALLO STATO DI PRIMING SENZA ASPETTARE ALCUN SEGNALE
		case STATE_TANK_FILL:
			if((currentGuard[GUARD_ENABLE_PRIMING_PHASE_1].guardValue == GUARD_VALUE_TRUE))
			{
				currentGuard[GUARD_ENABLE_PRIMING_PHASE_1].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				/* (FM) FINITA LA FASE DI RIEMPIMENTO POSSO PASSARE ALLA FASE 1 DEL PRIMING */
				/* compute future state */
				ptrFutureState = &stateState[13];
				/* compute future parent */
				ptrFutureParent = ptrFutureState->ptrParent;
				/* compute future child */
				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
				DebugStringStr("STATE_PRIMING_PH_1");
			}
			else if((currentGuard[GUARD_ENABLE_PRIMING_PHASE_2].guardValue == GUARD_VALUE_TRUE))
			{
				currentGuard[GUARD_ENABLE_PRIMING_PHASE_2].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				/* VADO DIRETTAMENTE ALLA FASE 2 DEL PRIMING PER POI ANDARE IN RICIRCOLO */
				/* compute future state */
				ptrFutureState = &stateState[15];
				/* compute future parent */
				ptrFutureParent = ptrFutureState->ptrParent;
				/* compute future child */
				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
				DebugStringStr("STATE_PRIMING_PH_2");
			}

			/* execute function state level */
			manageStateEntryAndStateAlways(12);
			break;

		case STATE_PRIMING_PH_1:
//			if(
//				(currentGuard[GUARD_ENABLE_PRIMING_PHASE_2].guardValue == GUARD_VALUE_TRUE)
//				)
//			{
//				currentGuard[GUARD_ENABLE_PRIMING_PHASE_2].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
//				/* (FM) FINITA LA FASE 1 DEL PRIMING POSSO PASSARE ALLA FASE 2 */
//				/* compute future state */
//				ptrFutureState = &stateState[15];
//				/* compute future parent */
//				ptrFutureParent = ptrFutureState->ptrParent;
//				/* compute future child */
//				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
//				DebugStringStr("STATE_PRIMING_PH_2");
//			}
			if(currentGuard[GUARD_ENABLE_STATE_PRIMING_PH_1_WAIT].guardValue == GUARD_VALUE_TRUE)
			{
				// nello stato priming 1 aspetto che mi arrivi il comando di caricamento del filtro
				// quando mi arriva passo subito a priming 2 per completare il 5% di priming che mi rimane
				// le pompe sono ferme e vado nello stato di attesa che l'utente monti il filtro
				currentGuard[GUARD_ENABLE_STATE_PRIMING_PH_1_WAIT].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				/* compute future state */
				ptrFutureState = &stateState[39];
				/* compute future parent */
				ptrFutureParent = ptrFutureState->ptrParent;
				/* compute future child */
				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
				DebugStringStr("PRIMING_PH_1_WAIT");
				break;
			}
			else if(currentGuard[GUARD_ABANDON_PRIMING].guardValue == GUARD_VALUE_TRUE)
			{
				currentGuard[GUARD_ABANDON_PRIMING].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				/* (FM) HO DECISO DI ABBANDONARE IL PRIMING, RITORNO IN IDLE */
				/* compute future state */
				ptrFutureState = &stateState[3];
				/* compute future parent */
				ptrFutureParent = ptrFutureState->ptrParent;
				/* compute future child */
				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
				DebugStringStr("STATE_IDLE(ABBANDONA)");
			}
			/* execute function state level */
			manageStateEntryAndStateAlways(14);
			break;

		case STATE_PRIMING_PH_1_WAIT:
			if( (currentGuard[GUARD_FILTER_INSTALLED].guardValue == GUARD_VALUE_TRUE) )
			{
				// il filtro e' stato montato passo alla seconda fase di priming
				currentGuard[GUARD_FILTER_INSTALLED].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				/* (FM) FINITA LA FASE 1 DEL PRIMING POSSO PASSARE ALLA FASE 2 */
				/* compute future state */
				ptrFutureState = &stateState[15];
				/* compute future parent */
				ptrFutureParent = ptrFutureState->ptrParent;
				/* compute future child */
				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
				DebugStringStr("STATE_PRIMING_PH_2");
			}
			else if(currentGuard[GUARD_ABANDON_PRIMING].guardValue == GUARD_VALUE_TRUE)
			{
				currentGuard[GUARD_ABANDON_PRIMING].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				/* (FM) HO DECISO DI ABBANDONARE IL PRIMING, RITORNO IN IDLE */
				/* compute future state */
				ptrFutureState = &stateState[3];
				/* compute future parent */
				ptrFutureParent = ptrFutureState->ptrParent;
				/* compute future child */
				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
				DebugStringStr("STATE_IDLE(ABBANDONA)");
			}
			/* execute function state level */
			manageStateEntryAndStateAlways(40);
			break;

		case STATE_PRIMING_PH_2:
			if((currentGuard[GUARD_ENABLE_PRIMING_WAIT].guardValue == GUARD_VALUE_TRUE))
			{
				currentGuard[GUARD_ENABLE_PRIMING_WAIT].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				/* (FM) FINITA LA FASE 2 DEL PRIMING PASSO ALL'ATTESA DI UN NUOVO VOLUME O UN BUTTON_PRIMING_END */
				/* compute future state */
				ptrFutureState = &stateState[21];
				/* compute future parent */
				ptrFutureParent = ptrFutureState->ptrParent;
				/* compute future child */
				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
				DebugStringStr("STATE_PRIMING_WAIT");

				/*torno indietro nella macchina a stati quindi resetto la flag di entry sullo stato in cui sono*/
				currentGuard[GUARD_ENABLE_PRIMING_PHASE_2].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
			}
			else if(currentGuard[GUARD_ABANDON_PRIMING].guardValue == GUARD_VALUE_TRUE)
			{
				currentGuard[GUARD_ABANDON_PRIMING].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				/* (FM) HO DECISO DI ABBANDONARE IL PRIMING, RITORNO IN IDLE */
				/* compute future state */
				ptrFutureState = &stateState[3];
				/* compute future parent */
				ptrFutureParent = ptrFutureState->ptrParent;
				/* compute future child */
				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
				DebugStringStr("STATE_IDLE(ABBANDONA)");
			}
			/* execute function state level */
			manageStateEntryAndStateAlways(16);
			break;

		case STATE_TREATMENT_KIDNEY_1:
            /* (FM) SONO IN TRATTAMENTO A QUESTO PUNTO FUNZIONANO GLI ALLARMI E SONO IN ATTESA DELL'ATTIVAZIONE DI
               currentGuard[GUARD_ENABLE_DISPOSABLE_EMPTY].guardEntryValue (AVVERRA' QUANDO L'UTENTE PREMERA' LO STOP ALLE POMPE
               O SEMPLICEMENTE IL TASTO ENTER (VEDI FUNZIONE manageStateTreatKidney1Always).
               QUANDO ARRIVERA', POTRO' TORNARE NELLO STATE_ENTRY INIZIALE*/
			if( currentGuard[GUARD_ENABLE_DISPOSABLE_EMPTY].guardValue == GUARD_VALUE_TRUE )
			{
				currentGuard[GUARD_ENABLE_DISPOSABLE_EMPTY].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				// se voglio andare in idle e selezionare un nuovo trattamento
				//ptrFutureState = &stateState[3];
				// se voglio andare nella procedura di svuotamento
				ptrFutureState = &stateState[19];
				/* compute future parent */
				ptrFutureParent = ptrFutureState->ptrParent;
				/* compute future child */
				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
				DebugStringStr("STATE_EMPTY_DISPOSABLE");

				/*per poter tornare indietro dallo stato STATE_EMPTY_DISPOSABLE allo stato STATE_TREATMENT_KIDNEY_1
				 * resetto la flag di entry sullo stato in cui sono*/
				currentGuard[GUARD_ENABLE_TREATMENT_KIDNEY_1].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				currentGuard[GUARD_ENABLE_TREATMENT_KIDNEY_1].guardValue = GUARD_VALUE_FALSE;
			}

			/* execute function state level */
			manageStateEntryAndStateAlways(18);
			break;

		case STATE_PRIMING_WAIT:
				if( currentGuard[GUARD_PRIMING_END].guardValue == GUARD_VALUE_TRUE )
				{
					// ho terminato il riempimento del reservoir vado nello stato di attesa di raggiungimento della temperatura
					currentGuard[GUARD_PRIMING_END].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
					ptrFutureState = &stateState[23];
					/* compute future parent */
					ptrFutureParent = ptrFutureState->ptrParent;
					/* compute future child */
					ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
					DebugStringStr("STATE_RICICLO");
				}
				else if( currentGuard[GUARD_ABANDON_PRIMING].guardValue == GUARD_VALUE_TRUE )
				{
					// abbandono il priming e ritorno in IDLE
					currentGuard[GUARD_PRIMING_END].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
					ptrFutureState = &stateState[3];
					/* compute future parent */
					ptrFutureParent = ptrFutureState->ptrParent;
					/* compute future child */
					ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
					DebugStringStr("STATE_IDLE(ABBANDONA)");
				}
				else if( currentGuard[GUARD_ENABLE_PRIMING_PHASE_2].guardValue == GUARD_VALUE_TRUE )
				{
					// il volume e' stato modificato ritorno nella fase di riempimento
					currentGuard[GUARD_ENABLE_PRIMING_PHASE_2].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;

					ptrFutureState = &stateState[15];
					/* compute future parent */
					ptrFutureParent = ptrFutureState->ptrParent;
					/* compute future child */
					ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
					DebugStringStr("STATE_PRIMING_PHASE_2");

					/*torno indietro nella macchina a stati quindi resetto la flag di entry sullo stato in cui sono*/
					currentGuard[GUARD_ENABLE_PRIMING_WAIT].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				}

				/* execute function state level */
				manageStateEntryAndStateAlways(22);
				break;
		case STATE_PRIMING_RICIRCOLO:
				if( currentGuard[GUARD_ENABLE_TREATMENT_KIDNEY_1].guardValue == GUARD_VALUE_TRUE )
				{
					// PASSO ALLA FASE DI TRATTAMENTO
					currentGuard[GUARD_ENABLE_TREATMENT_KIDNEY_1].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
					// ho ricevuto il comando per entrare in trattamento
					ptrFutureState = &stateState[17];
					/* compute future parent */
					ptrFutureParent = ptrFutureState->ptrParent;
					/* compute future child */
					ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
					DebugStringStr("STATE_TREATMENT");
				}
				else if( currentGuard[GUARD_ABANDON_PRIMING].guardValue == GUARD_VALUE_TRUE )
				{
					// abbandono il priming e ritorno in IDLE
					currentGuard[GUARD_PRIMING_END].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
					ptrFutureState = &stateState[3];
					/* compute future parent */
					ptrFutureParent = ptrFutureState->ptrParent;
					/* compute future child */
					ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
					DebugStringStr("STATE_IDLE(ABBANDONA)");
				}

// NON VADO NELLO STATO DI ATTESA TRATTAMENTO MA VADO DIRETTAMENTE NELLO STATO STATE_TREATMENT_KIDNEY_1 DOVE DOVREI
// CONSIDERARE ANCHE LA POSSIBILITA' DI ABBANDONA
//				if( currentGuard[GUARD_ENABLE_WAIT_TREATMENT].guardValue == GUARD_VALUE_TRUE )
//				{
//					// ho raggiunto la temperatura, vado nello stato di attesa start trattamento
//					currentGuard[GUARD_ENABLE_WAIT_TREATMENT].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
//					ptrFutureState = &stateState[25];
//					/* compute future parent */
//					ptrFutureParent = ptrFutureState->ptrParent;
//					/* compute future child */
//					ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
//					DebugStringStr("STATE_WAIT_TREATMENT");
//				}
				/* execute function state level */
				manageStateEntryAndStateAlways(24);
				break;
		case STATE_WAIT_TREATMENT:
// QUESTO STATO NON VIENE PIU USATO PER ORA
//				if((currentGuard[GUARD_ENABLE_TREATMENT_KIDNEY_1].guardValue == GUARD_VALUE_TRUE))
//				{
//					// PASSO ALLA FASE DI TRATTAMENTO
//					currentGuard[GUARD_ENABLE_TREATMENT_KIDNEY_1].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
//					// ho ricevuto il comando per entrare in trattamento
//					ptrFutureState = &stateState[17];
//					/* compute future parent */
//					ptrFutureParent = ptrFutureState->ptrParent;
//					/* compute future child */
//					ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
//					DebugStringStr("STATE_TREATMENT");
//
//					// memorizzo il tempo di inizio trattamento
//					StartTreatmentTime = timerCounterModBus;
//					// forzo anche uno start per far ripartire i motori nello stato di trattamento
//					// altrimenti non partono. Nella vecchia gestione partono solo con un comando da utente.
//					setGUIButton(BUTTON_START_TREATMENT);
//				}
//				/* execute function state level */
//				manageStateEntryAndStateAlways(26);
				break;


		/*case STATE_T1_WITH_DISPOSABLE:
			break;

		case STATE_PRIMING_TREAT_1:
			break;

		case STATE_PRIMING_TREAT_2:
			break;*/


		case STATE_TREATMENT_2:
			break;

		case STATE_EMPTY_DISPOSABLE:
			if( currentGuard[GUARD_EMPTY_DISPOSABLE_END].guardValue == GUARD_VALUE_TRUE )
			{
				// ho terminato lo svuotamento del reservoir vado nello stato di smontaggio del disposable
				currentGuard[GUARD_EMPTY_DISPOSABLE_END].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				// se voglio andare in idle e selezionare un nuovo trattamento
				ptrFutureState = &stateState[3];
				// se voglio andare in STATE_UNMOUNT_DISPOSABLE e staccare il disposable
				//ptrFutureState = &stateState[27];
				/* compute future parent */
				ptrFutureParent = ptrFutureState->ptrParent;
				/* compute future child */
				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
				DebugStringStr("STATE_UNMOUNT_DISPOSABLE");
				break;
			}
			else if( currentGuard[GUARD_ENABLE_TREATMENT_KIDNEY_1].guardValue == GUARD_VALUE_TRUE )
			{
				// ritorno direttamente in trattamento per cominciarne uno nuovo
				currentGuard[GUARD_ENABLE_TREATMENT_KIDNEY_1].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;

				ptrFutureState = &stateState[17];
				/* compute future parent */
				ptrFutureParent = ptrFutureState->ptrParent;
				/* compute future child */
				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
				DebugStringStr("STATE_TREATMENT");

				// riparte come se fosse un nuovo trattamento
				StartTreatmentTime = 0;
				TotalTreatDuration = 0;
				TreatDuration = 0;
				/*torno indietro nella macchina a stati quindi resetto la flag di entry sullo stato in cui sono*/
				currentGuard[GUARD_ENABLE_DISPOSABLE_EMPTY].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				currentGuard[GUARD_EMPTY_DISPOSABLE_END].guardValue = GUARD_VALUE_FALSE;
				break;
			}
			else if( currentGuard[GUARD_ABANDON_PRIMING].guardValue == GUARD_VALUE_TRUE )
			{
				// abbandono il priming e ritorno in IDLE
				currentGuard[GUARD_PRIMING_END].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				ptrFutureState = &stateState[3];
				/* compute future parent */
				ptrFutureParent = ptrFutureState->ptrParent;
				/* compute future child */
				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
				DebugStringStr("STATE_IDLE(ABBANDONA)");
				break;
			}

			/* execute function state level */
			manageStateEntryAndStateAlways(20);
			break;

		case STATE_UNMOUNT_DISPOSABLE:
			if( currentGuard[GUARD_ENABLE_UNMOUNT_END].guardValue == GUARD_VALUE_TRUE )
			{
				// ho smontato il disposable ritorno in idle
				currentGuard[GUARD_ENABLE_UNMOUNT_END].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				ptrFutureState = &stateState[3];
				/* compute future parent */
				ptrFutureParent = ptrFutureState->ptrParent;
				/* compute future child */
				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
				DebugStringStr("STATE_IDLE");
				break;
			}
			else if( currentGuard[GUARD_ABANDON_PRIMING].guardValue == GUARD_VALUE_TRUE )
			{
				// abbandono il priming e ritorno in IDLE
				currentGuard[GUARD_PRIMING_END].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				ptrFutureState = &stateState[3];
				/* compute future parent */
				ptrFutureParent = ptrFutureState->ptrParent;
				/* compute future child */
				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
				DebugStringStr("STATE_IDLE(ABBANDONA)");
				break;
			}

			/* execute function state level */
			manageStateEntryAndStateAlways(28);
			break;

		case STATE_EMPTY_DISPOSABLE_1:
			break;

		case STATE_EMPTY_DISPOSABLE_2:
			break;

		case STATE_WASHING:
			break;

		case STATE_FATAL_ERROR:
			break;

		default:
			break;
	}

	//questo switch andrà suddiviso e portato dentro i singoli case dello switch sopra........
	/* (FM) POSSO FARE UNA FUNZIONE CHIAMATA stateParentPrimingTreatKidney1_func ED INSERIRE TUTTI I CASE
	   RELATIVI AL PRIMING E CHIAMARLA SEMPRE NEI CASE STATE_PRIMING_PH_1, STATE_PRIMING_PH_2 DELLO SWITCH PRECEDENTE
	   L'ALTRA FUNZIONE, ANALOGAMENTE POTRA' ESSERE CHIAMATA stateParentTreatKidney1_func, CONTERRA' TUTTI I CASE DAL PARENT_TREAT_KIDNEY_1_INIT
	   IN POI. LA FUNZIONE CREATA DOVRA' ESSERE CHIAMATA NEL CASE STATE_TREATMENT_KIDNEY_1. */
	switch(ptrCurrentParent->parent){
		case PARENT_PRIMING_TREAT_KIDNEY_1_INIT:
			if(buttonGUITreatment[BUTTON_CONFIRM].state == GUI_BUTTON_RELEASED)
			{
				releaseGUIButton(BUTTON_CONFIRM);
				/* (FM) alla ricezione del comando da tastiera faccio partire la fase di priming */
				ptrFutureParent = &stateParentPrimingTreatKidney1[3];
				ptrFutureChild = ptrFutureParent->ptrChild;
			}

			if(ptrCurrentParent->action == ACTION_ON_ENTRY)
			{
				/* execute parent callback function */
				ptrCurrentParent->callBackFunct();
				/* compute future parent */
				ptrFutureParent = &stateParentPrimingTreatKidney1[2];
				ptrFutureChild = ptrFutureParent->ptrChild;
			}
			else if(ptrCurrentParent->action == ACTION_ALWAYS)
			{
				ptrCurrentParent->callBackFunct();
			}

			/* (FM) se si e' verificato un allarme lo faccio partire */
			if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_TRUE)
			{
				ptrFutureParent = &stateParentPrimingTreatKidney1[5];
				ptrFutureChild = ptrFutureParent->ptrChild;
				// guardando a questo valore posso vedere il tipo di azione di sicurezza
				// e quindi posso decidere di andare anche in un qualche altro stato ad hoc
				// di allarme
				//ptrAlarmCurrent->secActType
//				if(ptrAlarmCurrent->code == CODE_ALARM_TANK_LEVEL_HIGH)
//				{
//					// si e' verificato un allarme di troppo pieno, fermo tutto ed al successivo
//					// reset termino la procedura di priming e passo direttamente al ricircolo
//					GlobalFlags.FlagsDef.TankLevelHigh = 1;
//				}
				LevelBuzzer = 2;
			}

			break;

		case PARENT_PRIMING_TREAT_KIDNEY_1_RUN:
			if(ptrCurrentParent->action == ACTION_ON_ENTRY)
			{
				/* execute parent callback function */
				ptrCurrentParent->callBackFunct();
				/* compute future parent */
				ptrFutureParent = &stateParentPrimingTreatKidney1[4];
				ptrFutureChild = ptrFutureParent->ptrChild;
			}
			else if(ptrCurrentParent->action == ACTION_ALWAYS)
			{
				ptrCurrentParent->callBackFunct();
			}

			/* (FM) se si e' verificato un allarme lo faccio partire */
			if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_TRUE)
			{
				ptrFutureParent = &stateParentPrimingTreatKidney1[5];
				ptrFutureChild = ptrFutureParent->ptrChild;
				// guardando a questo valore posso vedere il tipo di azione di sicurezza
				// e quindi posso decidere di andare anche in un qualche altro stato ad hoc
				// di allarme
				//ptrAlarmCurrent->secActType
//				if(ptrAlarmCurrent->code == CODE_ALARM_TANK_LEVEL_HIGH)
//				{
//					// si e' verificato un allarme di troppo pieno, fermo tutto ed al successivo
//					// reset termino la procedura di priming e passo direttamente al ricircolo
//					GlobalFlags.FlagsDef.TankLevelHigh = 1;
//				}
				LevelBuzzer = 2;
			}
			break;

		case PARENT_PRIMING_TREAT_KIDNEY_1_ALARM:
			if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_FALSE)
			{
				if(currentGuard[GUARD_ALARM_WAIT_CMD_TO_EXIT].guardValue == GUARD_VALUE_TRUE)
				{
					// vado nello stato parent dove posso cercare di recuperare la condizione di allarme
					currentGuard[GUARD_ALARM_WAIT_CMD_TO_EXIT].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
					currentGuard[GUARD_ALARM_WAIT_CMD_TO_EXIT].guardValue = GUARD_VALUE_FALSE;

					/* (FM) finita la situazione di allarme posso ritornare in PARENT_PRIMING_TREAT_KIDNEY_1_INIT*/
					// nella nuova gestione il priming viene fatto partendo direttamente dallo stato PARENT_PRIMING_TREAT_KIDNEY_1_RUN
					// e non da PARENT_PRIMING_TREAT_KIDNEY_1_INIT
					//ptrFutureParent = &stateParentPrimingTreatKidney1[1];
					ptrFutureParent = &stateParentPrimingTreatKidney1[3];
					ptrFutureChild = ptrFutureParent->ptrChild;

					// forzo anche una pressione del tasto BUTTON_PRIMING_END_CONFIRM per fare in modo che
					// il riempimento termini subito e si vada alla fase di riciclo
					setGUIButton(BUTTON_PRIMING_END_CONFIRM);
				}
				else if(buttonGUITreatment[BUTTON_RESET_ALARM].state == GUI_BUTTON_RELEASED)
				{
					// Il ritorno al priming viene fatto solo dopo la pressione del tasto BUTTON_RESET_ALARM
					releaseGUIButton(BUTTON_RESET_ALARM);
//					if(GlobalFlags.FlagsDef.TankLevelHigh)
//					{
//						// era un allarme di troppo pieno, forzo uscita dal priming
//						setGUIButton(BUTTON_PRIMING_END_CONFIRM);
//						GlobalFlags.FlagsDef.TankLevelHigh = 0;
//					}
//					else
//					{
						// forzo anche una pressione del tasto BUTTON_START_PRIMING START per fare in modo che
						// il priming riprenda automaticamente
						setGUIButton(BUTTON_START_PRIMING);
//					}

					/* (FM) finita la situazione di allarme posso ritornare in PARENT_PRIMING_TREAT_KIDNEY_1_INIT*/
					// nella nuova gestione il priming viene fatto partendo direttamente dallo stato PARENT_PRIMING_TREAT_KIDNEY_1_RUN
					// e non da PARENT_PRIMING_TREAT_KIDNEY_1_INIT
					//ptrFutureParent = &stateParentPrimingTreatKidney1[1];
					ptrFutureParent = &stateParentPrimingTreatKidney1[3];
					ptrFutureChild = ptrFutureParent->ptrChild;

					LevelBuzzer = 0;
				}
			}

			if(ptrCurrentParent->action == ACTION_ON_ENTRY)
			{
				/* execute parent callback function */
				ptrCurrentParent->callBackFunct();
				/* compute future parent */
				ptrFutureParent = &stateParentPrimingTreatKidney1[6];
				ptrFutureChild = ptrFutureParent->ptrChild;
			}
			else if(ptrCurrentParent->action == ACTION_ALWAYS)
			{
				ptrCurrentParent->callBackFunct();
			}
			break;

		case PARENT_PRIMING_TREAT_KIDNEY_1_END:
			break;


		case PARENT_TREAT_KIDNEY_1_INIT:
			if(perfusionParam.treatVolPerfArt >= 200)
			{
				/* FM faccio partire la pompa */
				ptrFutureParent = &stateParentTreatKidney1[3];
				ptrFutureChild = ptrFutureParent->ptrChild;
			}

			if(ptrCurrentParent->action == ACTION_ON_ENTRY)
			{
				/* execute parent callback function */
				ptrCurrentParent->callBackFunct();
				/* compute future parent */
				/* FM entro nello stato in cui l'azione e' di tipo ACTION_ALWAYS */
				ptrFutureParent = &stateParentTreatKidney1[2];
				ptrFutureChild = ptrFutureParent->ptrChild;
			}
			else if(ptrCurrentParent->action == ACTION_ALWAYS)
			{
				ptrCurrentParent->callBackFunct();
			}

			if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_TRUE)
			{
				/* (FM) si e' verificato un allarme, passo alla sua gestione */
				ptrFutureParent = &stateParentTreatKidney1[5];
				ptrFutureChild = ptrFutureParent->ptrChild;
				DisableAllAirAlarm(FALSE);
				LevelBuzzer = 2;
				// guardando a questo valore posso vedere il tipo di azione di sicurezza
				// e quindi posso decidere di andare anche in un qualche altro stato ad hoc
				// di allarme
				//ptrAlarmCurrent->secActType
//				if(ptrAlarmCurrent->code == CODE_ALARM_TANK_LEVEL_HIGH)
//				{
//					// si e' verificato un allarme di troppo pieno, fermo tutto ed al successivo
//					// reset termino la procedura di priming e passo direttamente al ricircolo
//					GlobalFlags.FlagsDef.TankLevelHigh = 1;
//				}
			}
			break;

		case PARENT_TREAT_KIDNEY_1_PUMP_ON:
			if(ptrCurrentParent->action == ACTION_ON_ENTRY)
			{
				/* execute parent callback function */
				ptrCurrentParent->callBackFunct();
				/* compute future parent */
				/* FM passo alla gestione ACTION_ALWAYS */
				ptrFutureParent = &stateParentTreatKidney1[4];
				ptrFutureChild = ptrFutureParent->ptrChild;
			}
			else if(ptrCurrentParent->action == ACTION_ALWAYS)
			{
				ptrCurrentParent->callBackFunct();
			}

			if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_TRUE)
			{
				/* (FM) si e' verificato un allarme, passo alla sua gestione */
				ptrFutureParent = &stateParentTreatKidney1[5];
				ptrFutureChild = ptrFutureParent->ptrChild;
				DisableAllAirAlarm(FALSE);
				LevelBuzzer = 2;
				// guardando a questo valore posso vedere il tipo di azione di sicurezza
				// e quindi posso decidere di andare anche in un qualche altro stato ad hoc
				// di allarme
				//ptrAlarmCurrent->secActType
//				if(ptrAlarmCurrent->code == CODE_ALARM_TANK_LEVEL_HIGH)
//				{
//					// si e' verificato un allarme di troppo pieno, fermo tutto ed al successivo
//					// reset abortisco il trattamento
//					GlobalFlags.FlagsDef.TankLevelHigh = 1;
//				}
			}
			break;

		case PARENT_TREAT_KIDNEY_1_ALARM:
			if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_FALSE)
			{
				bool ButtonResetRcvd = FALSE;
				if(buttonGUITreatment[BUTTON_RESET_ALARM].state == GUI_BUTTON_RELEASED)
				{
					releaseGUIButton(BUTTON_RESET_ALARM);
					ButtonResetRcvd = TRUE;
					LevelBuzzer = 0;
				}

				if((currentGuard[GUARD_ALARM_AIR_FILT_RECOVERY].guardValue == GUARD_VALUE_TRUE) || (ButtonResetRcvd && TreatAlm1SafAirFiltActive))
				{
					// ho ricevuto un BUTTON_RESET_ALARM e l'allarme era ancora attivo oppure
					// ho ricevuto un BUTTON_RESET_ALARM dopo che l'allarme e' stato cancellato perche' la condizione
					// fisica di allarme e' venuta meno
					TreatAlm1SafAirFiltActive = FALSE;
					// vado nello stato parent dove posso cercare di recuperare la condizione di allarme
					currentGuard[GUARD_ALARM_AIR_FILT_RECOVERY].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
					currentGuard[GUARD_ALARM_AIR_FILT_RECOVERY].guardValue = GUARD_VALUE_FALSE;
					GoToRecoveryParentState(PARENT_TREAT_KIDNEY_1_AIR_FILT);
				}
				else if((currentGuard[GUARD_ALARM_AIR_SFV_RECOVERY].guardValue == GUARD_VALUE_TRUE) || (ButtonResetRcvd && TreatAlm1SFVActive))
				{
					TreatAlm1SFVActive = FALSE;
					// vado nello stato parent dove posso cercare di recuperare la condizione di allarme
					currentGuard[GUARD_ALARM_AIR_SFV_RECOVERY].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
					currentGuard[GUARD_ALARM_AIR_SFV_RECOVERY].guardValue = GUARD_VALUE_FALSE;
					GoToRecoveryParentState(PARENT_TREAT_KIDNEY_1_SFV);
				}
				else if((currentGuard[GUARD_ALARM_AIR_SFA_RECOVERY].guardValue == GUARD_VALUE_TRUE) || (ButtonResetRcvd && TreatAlm1SFAActive))
				{
					TreatAlm1SFAActive = FALSE;
					// vado nello stato parent dove posso cercare di recuperare la condizione di allarme
					currentGuard[GUARD_ALARM_AIR_SFA_RECOVERY].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
					currentGuard[GUARD_ALARM_AIR_SFA_RECOVERY].guardValue = GUARD_VALUE_FALSE;
					GoToRecoveryParentState(PARENT_TREAT_KIDNEY_1_SFA);
				}
				else if(currentGuard[GUARD_ALARM_WAIT_CMD_TO_EXIT].guardValue == GUARD_VALUE_TRUE)
				{
					// vado nello stato parent dove posso cercare di recuperare la condizione di allarme
					currentGuard[GUARD_ALARM_WAIT_CMD_TO_EXIT].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
					currentGuard[GUARD_ALARM_WAIT_CMD_TO_EXIT].guardValue = GUARD_VALUE_FALSE;

					// FM allarme resettato posso ritornare nella fase iniziale del trattamento
					ptrFutureParent = &stateParentTreatKidney1[1];
					ptrFutureChild = ptrFutureParent->ptrChild;
					// forzo anche una pressione del tasto TREATMENT START per fare in modo che
					// il trattamento riprenda automaticamente
					setGUIButton(BUTTON_START_TREATMENT);
				}
				else
				{
					if(ButtonResetRcvd)
					{
						ButtonResetRcvd = FALSE;
//						if(GlobalFlags.FlagsDef.TankLevelHigh)
//						{
//							// era un allarme di troppo pieno, riprendo il trattamento
//							// TODO DA VEDERE MEGLIO COME TRATTARLO, ABORTIRE IL TRATTAMENTO  O NO?
//							GlobalFlags.FlagsDef.TankLevelHigh = 0;
//							ptrFutureParent = &stateParentTreatKidney1[1];
//							ptrFutureChild = ptrFutureParent->ptrChild;
//							setGUIButton(BUTTON_START_TREATMENT);
//						}
//						else
//						{
							// FM allarme finito posso ritornare nella fase iniziale del trattamento
							ptrFutureParent = &stateParentTreatKidney1[1];
							ptrFutureChild = ptrFutureParent->ptrChild;
							// forzo anche una pressione del tasto TREATMENT START per fare in modo che
							// il trattamento riprenda automaticamente
							setGUIButton(BUTTON_START_TREATMENT);
//						}
					}
				}
			}

			if(ptrCurrentParent->action == ACTION_ON_ENTRY)
			{
				/* execute parent callback function */
				ptrCurrentParent->callBackFunct();
				/* compute future parent */
				/* (FM) passo alla gestione ACTION_ALWAYS dell'allarme */
				ptrFutureParent = &stateParentTreatKidney1[6];
				ptrFutureChild = ptrFutureParent->ptrChild;

				// reinserire queste due istruzioni se si vuole fermare il conteggio del tempo di
				// trattamento durante un allarme
//    			TotalTreatDuration += TreatDuration;
//    			TreatDuration = 0;
			}
			else if(ptrCurrentParent->action == ACTION_ALWAYS)
			{
				ptrCurrentParent->callBackFunct();
				// (FM) chiamo la funzione child che gestisce lo stato di allarme durante il trattamento
				// Dovra fare tutte le attuazioni sulle pompe, pinch necessarie per risolvere la condizione
				// di allarme
				ManageStateChildAlarmTreat1();
			}
			break;

		// STATI PER LA GESTIONE DELLA PROCEDURA DI RIMOZIONE DELL'ARIA DAL CIRCUITO -----------------------
		case PARENT_TREAT_KIDNEY_1_AIR_FILT:
			if(currentGuard[GUARD_AIR_RECOVERY_END].guardValue == GUARD_VALUE_TRUE)
			{
				currentGuard[GUARD_AIR_RECOVERY_END].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				currentGuard[GUARD_AIR_RECOVERY_END].guardValue = GUARD_VALUE_FALSE;
				// ho rimosso l'aria dal circuito, posso ritornare nello stato di allarme in cui sono partito
				// oppure nello stato di lavoro normale (dopo aver riabilitato gli allarmi).
				// Decido di ripartire dalla fase iniziale del trattamento.
				DisableAllAirAlarm(FALSE);
				ptrFutureParent = &stateParentTreatKidney1[1];
				ptrFutureChild = ptrFutureParent->ptrChild;

				// forzo anche una pressione del tasto TREATMENT START per fare in modo che
				// il trattamento riprenda automaticamente
				setGUIButton(BUTTON_START_TREATMENT);
				break;
			}

			if(ptrCurrentParent->action == ACTION_ON_ENTRY)
			{
				/* compute future parent */
				/* FM passo alla gestione ACTION_ALWAYS */
				ptrFutureParent = &stateParentTreatKidney1[8];
				ptrFutureChild = ptrFutureParent->ptrChild;
			}
			else if(ptrCurrentParent->action == ACTION_ALWAYS)
			{
			}

			if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_TRUE)
			{
				/* (FM) si e' verificato un allarme, durante la procedura di recupero dell'allarme aria.
				 * Per la sua gestione uso un nuovo stato 13  */
				ptrFutureParent = &stateParentTreatKidney1[13];
				ptrFutureChild = ptrFutureParent->ptrChild;
				// guardando a questo valore posso vedere il tipo di azione di sicurezza
				// e quindi posso decidere di andare anche in un qualche altro stato ad hoc
				// di allarme
				//ptrAlarmCurrent->secActType
				TotalTimeToRejAir += msTick_elapsed(StarTimeToRejAir);
			}
			break;
		case PARENT_TREAT_KIDNEY_1_SFV:
			if(currentGuard[GUARD_AIR_RECOVERY_END].guardValue == GUARD_VALUE_TRUE)
			{
				currentGuard[GUARD_AIR_RECOVERY_END].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				currentGuard[GUARD_AIR_RECOVERY_END].guardValue = GUARD_VALUE_FALSE;
				// ho rimosso l'aria dal circuito, posso ritornare nello stato di allarme in cui sono partito
				// oppure nello stato di lavoro normale (dopo aver riabilitato gli allarmi).
				// Decido di ripartire dalla fase iniziale del trattamento.
				DisableAllAirAlarm(FALSE);
				ptrFutureParent = &stateParentTreatKidney1[1];
				ptrFutureChild = ptrFutureParent->ptrChild;

				// forzo anche una pressione del tasto TREATMENT START per fare in modo che
				// il trattamento riprenda automaticamente
				setGUIButton(BUTTON_START_TREATMENT);
				break;
			}

			if(ptrCurrentParent->action == ACTION_ON_ENTRY)
			{
				/* compute future parent */
				/* FM passo alla gestione ACTION_ALWAYS */
				ptrFutureParent = &stateParentTreatKidney1[10];
				ptrFutureChild = ptrFutureParent->ptrChild;
			}
			else if(ptrCurrentParent->action == ACTION_ALWAYS)
			{
			}

			if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_TRUE)
			{
				/* (FM) si e' verificato un allarme, durante la procedura di recupero dell'allarme aria.
				 * Per la sua gestione uso un nuovo stato 13  */
				ptrFutureParent = &stateParentTreatKidney1[13];
				ptrFutureChild = ptrFutureParent->ptrChild;
				// guardando a questo valore posso vedere il tipo di azione di sicurezza
				// e quindi posso decidere di andare anche in un qualche altro stato ad hoc
				// di allarme
				//ptrAlarmCurrent->secActType
				TotalTimeToRejAir += msTick_elapsed(StarTimeToRejAir);
			}
			break;
		case PARENT_TREAT_KIDNEY_1_SFA:
			if(currentGuard[GUARD_AIR_RECOVERY_END].guardValue == GUARD_VALUE_TRUE)
			{
				currentGuard[GUARD_AIR_RECOVERY_END].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				currentGuard[GUARD_AIR_RECOVERY_END].guardValue = GUARD_VALUE_FALSE;
				// ho rimosso l'aria dal circuito, posso ritornare nello stato di allarme in cui sono partito
				// oppure nello stato di lavoro normale (dopo aver riabilitato gli allarmi).
				// Decido di ripartire dalla fase iniziale del trattamento.
				DisableAllAirAlarm(FALSE);
				ptrFutureParent = &stateParentTreatKidney1[1];
				ptrFutureChild = ptrFutureParent->ptrChild;

				// forzo anche una pressione del tasto TREATMENT START per fare in modo che
				// il trattamento riprenda automaticamente
				setGUIButton(BUTTON_START_TREATMENT);
				break;
			}

			if(ptrCurrentParent->action == ACTION_ON_ENTRY)
			{
				/* compute future parent */
				/* FM passo alla gestione ACTION_ALWAYS */
				ptrFutureParent = &stateParentTreatKidney1[12];
				ptrFutureChild = ptrFutureParent->ptrChild;
			}
			else if(ptrCurrentParent->action == ACTION_ALWAYS)
			{
			}

			if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_TRUE)
			{
				/* (FM) si e' verificato un allarme, durante la procedura di recupero dell'allarme aria.
				 * Per la sua gestione uso un nuovo stato 13  */
				ptrFutureParent = &stateParentTreatKidney1[13];
				ptrFutureChild = ptrFutureParent->ptrChild;
				// guardando a questo valore posso vedere il tipo di azione di sicurezza
				// e quindi posso decidere di andare anche in un qualche altro stato ad hoc
				// di allarme
				//ptrAlarmCurrent->secActType
				TotalTimeToRejAir += msTick_elapsed(StarTimeToRejAir);
			}
			break;

		case PARENT_TREAT_KIDNEY_1_ALM_AIR_REC:
			if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_FALSE)
			{
				/* FM allarme finito posso ritornare nello stato di partenza
				 * quando si e' verificato l'allarme */
				if(AirParentState == PARENT_TREAT_KIDNEY_1_AIR_FILT)
				{
					// ero nello stato recupero da allarme aria nel sensore aria on/off
					ptrFutureParent = &stateParentTreatKidney1[7];
					ptrFutureChild = ptrFutureParent->ptrChild;
					StarTimeToRejAir = timerCounterModBus;
					break;
				}
				else if(AirParentState == PARENT_TREAT_KIDNEY_1_SFV)
				{
					// ero nello stato recupero da allarme aria nel sensore aria venoso
					ptrFutureParent = &stateParentTreatKidney1[9];
					ptrFutureChild = ptrFutureParent->ptrChild;
					StarTimeToRejAir = timerCounterModBus;
					break;
				}
				else if(AirParentState == PARENT_TREAT_KIDNEY_1_SFA)
				{
					// ero nello stato recupero da allarme aria nel sensore aria arterioso
					ptrFutureParent = &stateParentTreatKidney1[11];
					ptrFutureChild = ptrFutureParent->ptrChild;
					StarTimeToRejAir = timerCounterModBus;
					break;
				}
				else
				{
					ptrFutureParent = &stateParentTreatKidney1[1];
					ptrFutureChild = ptrFutureParent->ptrChild;
					break;
				}
			}

			if(ptrCurrentParent->action == ACTION_ON_ENTRY)
			{
				/* compute future parent */
				/* (FM) passo alla gestione ACTION_ALWAYS dell'allarme */
				ptrFutureParent = &stateParentTreatKidney1[14];
				ptrFutureChild = ptrFutureParent->ptrChild;
			}
			else if(ptrCurrentParent->action == ACTION_ALWAYS)
			{
				// (FM) chiamo la funzione child che gestisce lo stato di allarme
				ManageStateChildAlarmTreat1();
			}
			break;
		//---------------------------------------------------------------------------------------------


		case PARENT_TREAT_KIDNEY_1_END:
            /* (FM) fine del trattamento  devo rimanere fermo qui, fino a quando non ricevo un nuovo
             comando di inizio trattamento */
			break;

		default:
			break;
	}


    /* (FM) IL CODICE CONTENUTO POTREBBE ESSERE INSERITO ALL'INTERNO DELLA FUNZIONE stateChildAlarmPriming_func E CHIAMATA DA DENTRO
       LA FUNZIONE stateParentPrimingTreatKidney1_func. QUESTA STESSA FUNZIONE DOVREBBE ESSERE CHIAMATA ANCHE DENTRO LA FUNZIONE
       stateParentTreatKidney1_func NEL MOMENTO IN CUI AGGIUNGERO' LA GESTIONE DELL'ALLARME ANCHE NEL TRATTAMENTO. */
	switch(ptrCurrentChild->child){
//		static unsigned char maskGuard;

		case CHILD_PRIMING_ALARM_INIT:

            /* (FM) qui ci entra quando nel ciclo switch precedente del parent e durante il priming si verifica un allarme.
               La tabella del parent contiene gia' il riferimento alla struttura 1 di inizializzazione della tabella del child. */
//			maskGuard = (currentGuard[GUARD_ALARM_STOP_ALL_ACTUATOR].guardValue == GUARD_VALUE_TRUE) 		|
//						((currentGuard[GUARD_ALARM_STOP_ALL].guardValue == GUARD_VALUE_TRUE) << 1)			|
//						((currentGuard[GUARD_ALARM_STOP_PERF_PUMP].guardValue == GUARD_VALUE_TRUE) << 2)	|
//						((currentGuard[GUARD_ALARM_STOP_PURIF_PUMP].guardValue == GUARD_VALUE_TRUE) << 3)	|
//						((currentGuard[GUARD_ALARM_STOP_OXYG_PUMP].guardValue == GUARD_VALUE_TRUE) << 4)	|
//						((currentGuard[GUARD_ALARM_STOP_PELTIER].guardValue == GUARD_VALUE_TRUE) << 5);

			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				/* (FM) esegue la parte ACTION_ON_ENTRY della gestione dell'allarme */
				ptrCurrentChild->callBackFunct();
				ptrFutureChild = &stateChildAlarmPriming[2];
			}
			else if(ptrCurrentChild->action == ACTION_ALWAYS){
				/* (FM) esegue la parte ACTION_ALWAYS della gestione dell'allarme */
				ptrCurrentChild->callBackFunct();
			}

            /* (FM) probabilmente cio' che deve essere fatto per completare la gestione dell'allarme e'quello che segue.
               I valori vengono messi a GUARD_VALUE_TRUE nella funzione manageAlarmChildGuard in base alle impostazioni
               fatte per la gestione degli allarmi */
            if(currentGuard[GUARD_ALARM_STOP_ALL_ACTUATOR].guardValue == GUARD_VALUE_TRUE)
            {
                /* (FM) risolvo la situazione di allarme andando a spegnere tutti gli attuatori */
                ptrFutureChild = &stateChildAlarmPriming[11];
            }
            else if(currentGuard[GUARD_ALARM_STOP_ALL].guardValue == GUARD_VALUE_TRUE)
            {
            	/* (FM) risolvo la situazione di allarme andando a spegnere tutti gli attuatori e pompe */
                ptrFutureChild = &stateChildAlarmPriming[7];
            }
            else if(currentGuard[GUARD_ALARM_STOP_PERF_PUMP].guardValue == GUARD_VALUE_TRUE)
            {
            	/* (FM) risolvo la situazione di allarme andando a spegnere la PERF PUMP */
                ptrFutureChild = &stateChildAlarmPriming[3];
            }
            else if(currentGuard[GUARD_ALARM_STOP_PURIF_PUMP].guardValue == GUARD_VALUE_TRUE)
            {
            	/* (FM) risolvo la situazione di allarme andando a spegnere la PURIF PUMP */
                ptrFutureChild = &stateChildAlarmPriming[5];
            }
            else if(currentGuard[GUARD_ALARM_STOP_OXYG_PUMP].guardValue == GUARD_VALUE_TRUE)
            {
            	/* (FM) risolvo la situazione di allarme andando a spegnere la OXYG PUMP */
                ptrFutureChild = &stateChildAlarmPriming[5];
            }
            else if(currentGuard[GUARD_ALARM_STOP_PELTIER].guardValue == GUARD_VALUE_TRUE)
            {
            	/* (FM) risolvo la situazione di allarme andando ad agire sulla cella di peltier */
                ptrFutureChild = &stateChildAlarmPriming[9];
            }
            else if(currentGuard[GUARD_ALARM_STOP_ALL_ACT_WAIT_CMD].guardValue == GUARD_VALUE_TRUE)
            {
            	/* (FM) risolvo la situazione di allarme andando  a spegnere tutti gli attuatori */
                ptrFutureChild = &stateChildAlarmPriming[15];
            }
			break;

		case CHILD_PRIMING_ALARM_STOP_PERFUSION:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				ptrCurrentChild->callBackFunct();

				ptrFutureChild = &stateChildAlarmPriming[4];
			}
            else if(currentGuard[GUARD_ALARM_STOP_PERF_PUMP].guardValue == GUARD_VALUE_FALSE )
                ptrFutureChild = &stateChildAlarmPriming[13]; /* (FM) allarme chiuso */
			else if(ptrCurrentChild->action == ACTION_ALWAYS){
				ptrCurrentChild->callBackFunct();
			}
			break;

		case CHILD_PRIMING_ALARM_STOP_PURIFICATION:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				ptrCurrentChild->callBackFunct();

				ptrFutureChild = &stateChildAlarmPriming[6];
			}
            else if((currentGuard[GUARD_ALARM_STOP_PURIF_PUMP].guardValue == GUARD_VALUE_FALSE) ||
                     (currentGuard[GUARD_ALARM_STOP_OXYG_PUMP].guardValue == GUARD_VALUE_FALSE) )
                ptrFutureChild = &stateChildAlarmPriming[13]; /* (FM) allarme chiuso */
			else if(ptrCurrentChild->action == ACTION_ALWAYS){
				ptrCurrentChild->callBackFunct();
			}
			break;

		case CHILD_PRIMING_ALARM_STOP_ALL_PUMP:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				ptrCurrentChild->callBackFunct();

				ptrFutureChild = &stateChildAlarmPriming[8];
			}
            else if(currentGuard[GUARD_ALARM_STOP_ALL].guardValue == GUARD_VALUE_FALSE )
                ptrFutureChild = &stateChildAlarmPriming[13]; /* FM allarme chiuso */
			else if(ptrCurrentChild->action == ACTION_ALWAYS){
				ptrCurrentChild->callBackFunct();
			}
			break;

		case CHILD_PRIMING_ALARM_STOP_PELTIER:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				ptrCurrentChild->callBackFunct();

				ptrFutureChild = &stateChildAlarmPriming[10];
			}
            else if(currentGuard[GUARD_ALARM_STOP_PELTIER].guardValue == GUARD_VALUE_FALSE )
                ptrFutureChild = &stateChildAlarmPriming[13]; /* FM allarme chiuso */
			else if(ptrCurrentChild->action == ACTION_ALWAYS){
				ptrCurrentChild->callBackFunct();
			}
			break;

		case CHILD_PRIMING_ALARM_STOP_ALL_ACTUATOR:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				ptrCurrentChild->callBackFunct();
				ptrFutureChild = &stateChildAlarmPriming[12];
			}
            else if( currentGuard[GUARD_ALARM_STOP_ALL_ACTUATOR].guardValue == GUARD_VALUE_FALSE )
                ptrFutureChild = &stateChildAlarmPriming[13]; /* FM allarme chiuso */
			else if(ptrCurrentChild->action == ACTION_ALWAYS){
				ptrCurrentChild->callBackFunct();
			}
			break;

		case CHILD_PRIMING_ALARM_END:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				ptrCurrentChild->callBackFunct();
				ptrFutureChild = &stateChildAlarmPriming[14];
			}
			else if(ptrCurrentChild->action == ACTION_ALWAYS){
                /* (FM) RIMANGO FERMO QUI FINO AL PROSSIMO ALLARME. NON E' NECESSARIO USCIRE DA QUESTO STATO DOPO IL TERMINE
                   DELLA CONDIZIONE DI ALLARME. LA CONDIZIONE DI ALLARME SUCCESSIVA MI FARA' RIPARTIRE DA CHILD_PRIMING_ALARM_INIT */
				ptrCurrentChild->callBackFunct();
			}
			break;

		case CHILD_PRIM_ALARM_1_WAIT_CMD:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				ptrFutureChild = &stateChildAlarmPriming[16];
			}
            else if( currentGuard[GUARD_ALARM_STOP_ALL_ACT_WAIT_CMD].guardValue == GUARD_VALUE_FALSE )
                ptrFutureChild = &stateChildAlarmPriming[13]; /* FM allarme chiuso */
			else if(ptrCurrentChild->action == ACTION_ALWAYS)
			{
			}
			break;

		default:
			break;
	}

	/*****************************************/
	/***  !!!!!!!!!! DO NOT TOUCH !!!!!!!! ***/
	/*****************************************/
	/* update state */
	ptrCurrentState = ptrFutureState;
	/* update parent */
	ptrCurrentParent = ptrFutureParent;
	/* update child */
	ptrCurrentChild = ptrFutureChild;
	/*****************************************/
	/***  !!!!!!!!!! DO NOT TOUCH !!!!!!!! ***/
	/*****************************************/

	/* process parent structure */
	/* process child structure */
}



/*----------------------------------------------------------------------------*/
/* This function initialise machine state variables                           */
/*----------------------------------------------------------------------------*/
void initAllState(void)
{

	  /**/
	  ptrCurrentState = &stateState[0];
	  ptrPreviousState = &stateState[0];
	  ptrFutureState = &stateState[0];

	  ptrCurrentParent = &stateParentIdle[0];
	  ptrPreviousParent = &stateParentIdle[0];
	  ptrFutureParent = &stateParentIdle[0];

	  ptrCurrentChild = &stateChildIdle[0];
	  ptrPreviousChild = &stateChildIdle[0];
	  ptrFutureChild = &stateChildIdle[0];

	  actionFlag = 0;
	  PidFirstTime[0] = 0;
	  PidFirstTime[1] = 0;
	  PidFirstTime[2] = 0;
	  PidFirstTime[3] = 0;
}

/*----------------------------------------------------------------------------*/
/* This function initialise machine state guard                           */
/*----------------------------------------------------------------------------*/
void initAllGuard(void)
{
	for(int i = 0; i <= GUARD_END_NUMBER; i++)
	{
		currentGuard[i].guardIdentifier = (GUARD_NULL + i);
		currentGuard[i].guardValue = GUARD_VALUE_FALSE;
		currentGuard[i].guardFlagActive = GUARD_FLAG_FALSE;
		currentGuard[i].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
		ptrCurrentGuard[i] = &currentGuard[i];
	}
}


// Questa cosa deve essere fatta nelle funzioni always e nonquando arriva il parametro
void CheckOxygenationSpeed(word value)
{
	if(parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value != value)
	{
		// ho ricevuto un valore del flusso di ossigenazione diverso dal precedente
		// devo aggiornare la velocita' delle pompe
		if(((PARAMETER_ACTIVE_TYPE)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_ACTIVE].value) == YES)
		{
			setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, (int)((float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / OXYG_FLOW_TO_RPM_CONV * 100.0));
			LastOxygenationSpeed = parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value;
		}
	}
}




void Display_7S_Management()
{
	//funzioni per spegnere e accenderfe display a 7 Segmenti

	// D_7S_A_SetVal(); //spegne led orizzontale alto
	// D_7S_A_ClrVal(); //accende led orizzontale alto

	// D_7S_B_SetVal(); //spegne led verticale alto Dx
	// D_7S_B_ClrVal(); //accende led verticale alto Dx

	// D_7S_C_SetVal(); //spegne led led verticale basso Dx
	// D_7S_C_ClrVal(); //accende led verticale basso Dx

	// D_7S_D_SetVal(); //spegne led orizzontale basso
	// D_7S_D_ClrVal(); //accende led orizzontale basso

	// D_7S_E_SetVal(); //spegne led verticale basso Sx
	// D_7S_E_ClrVal(); //accende led verticale basso Sx

	// D_7S_F_SetVal(); //spegne led verticale alto Sx
	// D_7S_F_ClrVal(); //accende led verticale alto Sx

	// D_7S_G_SetVal(); //spegne led orizzontale centrale
	// D_7S_G_ClrVal(); //accende led orizzontale centrale

	// D_7S_DP_SetVal(); //spegne puntino led
	// D_7S_DP_ClrVal(); //accende puntino led
}

void Cover_Sensor_GetVal()
{
	unsigned char CoverM1,CoverM2,CoverM3,CoverM4,CoverM5;

	CoverM1 = COVER_M1_GetVal();
	CoverM2 = COVER_M2_GetVal();
	CoverM3 = COVER_M3_GetVal();
	CoverM4 = COVER_M4_GetVal();
	CoverM5 = COVER_M5_GetVal();

}







/* Source --> indirizzo di partenza da ci si legge in flash (es 0xFF000)
 * *Dest  --> indirizzo della variabile o array su cui copiamo il valore letto
 * Count  --> numero di byte che andiamo a leggere
 * */

void EEPROM_Read(LDD_FLASH_TAddress Source, LDD_TData *Dest, LDD_FLASH_TDataSize Count)
{
	EEPROM_GetFlash(Source,Dest,Count);
}

/* Src   --> indirizzo della variabile o array da cui prendiamo i dati da scrivere
 * Dst   --> indirizzo di partenza da cui si scrive in flash (es 0xFF000)
 * Count --> numero di byte che andiamo a scrivere
 * */
void EEPROM_write(EEPROM_TDataAddress Src, EEPROM_TAddress Dst, word Count)
{
	EEPROM_SetFlash(Src,Dst,Count);
}


void Set_Data_EEPROM_Default(void)
{

	unsigned char *ptr_EEPROM = (EEPROM_TDataAddress)&config_data;

	/*Calcolo il CRC sui dati letti dalla EEPROM
	 * * IL CRC lo clacolo su tutta la struttura meno i due byte ndel CRC stesso*/
	unsigned int Calc_CRC_EEPROM = ComputeChecksum(ptr_EEPROM, sizeof(config_data)-2);

	/*Se il CRC calcolato non è uguale a quello letto o la revsione non è uguale a quella attesa
	 * scrivo i parametri di default*/
	if ( config_data.EEPROM_CRC != Calc_CRC_EEPROM || config_data.EEPROM_Revision != EEPROM_REVISION)
	{
		 config_data.sensor_PRx[OXYG].prSensGain      = PR_OXYG_GAIN_DEFAULT;
		 config_data.sensor_PRx[OXYG].prSensOffset    = PR_OXYG_OFFSET_DEFAULT;

		 config_data.sensor_PRx[LEVEL].prSensGain     = PR_LEVEL_GAIN_DEFAULT;
		 config_data.sensor_PRx[LEVEL].prSensOffset   = PR_LEVEL_OFFSET_DEFAULT;

		 config_data.sensor_PRx[ADS_FLT].prSensGain   = PR_ADS_FLT_GAIN_DEFAULT;
		 config_data.sensor_PRx[ADS_FLT].prSensOffset = PR_ADS_FLT_OFFSET_DEFAULT;

		 config_data.sensor_PRx[VEN].prSensGain       = PR_VEN_GAIN_DEFAULT;
		 config_data.sensor_PRx[VEN].prSensOffset     = PR_VEN_OFFSET_DEFAULT;

		 config_data.sensor_PRx[ART].prSensGain       = PR_ART_GAIN_DEFAULT;
		 config_data.sensor_PRx[ART].prSensOffset     = PR_ART_OFFSET_DEFAULT;

		 config_data.FlowSensor_Ven_Gain              = GAIN_FLOW_SENS_VEN;
		 config_data.FlowSensor_Ven_Offset            = OFFSET_FLOW_SENS_VEN;

		 config_data.FlowSensor_Art_Gain              = GAIN_FLOW_SENS_ART;
		 config_data.FlowSensor_Art_Offset            = OFFSET_FLOW_SENS_ART;

		 //revsione della EEPROM
		 config_data.EEPROM_Revision 				  = EEPROM_REVISION;

		 /*carico il CRC della EEPROM (usata la stessa funzione di CRC del MOD_BUS
		  * IL CRC lo clacolo su tutta la struttura meno i due byte ndel CRC stesso*/
		 config_data.EEPROM_CRC = ComputeChecksum(ptr_EEPROM, sizeof(config_data)-2);

		 EEPROM_write((EEPROM_TDataAddress)&config_data, START_ADDRESS_EEPROM, sizeof(config_data));
	}

}

/*funzione che gestione il Panic Button
 * se viene premuto per almeno TIMER_PANIC_BUTTON * 50 msec,
 * impostiamo PANIC_BUTTON_ACTIVATION = TRUE. Negli allarmi
 * gestiremo PANIC_BUTTON_ACTIVATION e sempre negli allarmi,
 * se dovesse arrivare un reset dopo l'attivazione, metteremo
 * PANIC_BUTTON_ACTIVATION = FALSE.
 * PANIC_BUTTON_OUTPUT viene posto alto e PANIC_BUTTON_INPUT in pull down
 * la pressione del bottone mette in corto i due pin quindi anche l'input sarà alto*/
void Manage_Panic_Button(void)
{
	static unsigned char Status_Panic_Button = 0;
	static unsigned long timer_button = 0;

	switch (Status_Panic_Button)
	{
		case 0:
			if (PANIC_BUTTON_INPUT_GetVal() )
			{
				timer_button = timerCounterModBus;
				Status_Panic_Button = 1;
			}

			break;

		case 1:
			if (!PANIC_BUTTON_INPUT_GetVal())
			{
				Status_Panic_Button = 0;
				PANIC_BUTTON_ACTIVATION = FALSE;
				timer_button = timerCounterModBus;
			}
			else if (msTick_elapsed(timer_button) >= TIMER_PANIC_BUTTON)
				PANIC_BUTTON_ACTIVATION = TRUE;
			break;

		default:
			Status_Panic_Button = 0;
			break;
	}
}

// Task di controllo della temperatura
// Se supero di 1 grado la temperatura target spengo le Peltier e le riaccendo quando
// la temperatura ha raggiunto di nuovo il target.
// Questo avviene solo quando sono in trattamento
void LiquidTempContrTask(LIQUID_TEMP_CONTR_CMD LiqTempContrCmd)
{
	static unsigned long TempOkTimeout;
	float tmpr;
	word tmpr_trgt;
	static LIQUID_TEMP_CONTR_STATE LiquidTempContrState = INIT_LIQTEMPCONTR_STATE;
	int CurrTemp;
	int myTempValue;

	if(ptrCurrentState->state != STATE_TREATMENT_KIDNEY_1)
		return;

	if(LiqTempContrCmd == WAIT_FOR_NEW_TARGET_T)
	{
		// devo aspettare il raggiungimento del nuovo target prima di monitorare
		// il mantenimento della nuova temperatura
		LiquidTempContrState = TEMP_START_CHECK_LIQTEMPCONTR_STATE;
	}

	// temperatura raggiunta dal reservoir
	tmpr = ((int)(sensorIR_TM[0].tempSensValue*10));
	// temperatura da raggiungere moltiplicata per 10
	tmpr_trgt = parameterWordSetFromGUI[PAR_SET_PRIMING_TEMPERATURE_PERFUSION].value;


	CurrTemp = (int)(sensorIR_TM[1].tempSensValue * 10.0);
	myTempValue = (int)parameterWordSetFromGUI[PAR_SET_PRIMING_TEMPERATURE_PERFUSION].value;
	switch (LiquidTempContrState)
	{
		// controllo del mantenimento della temperatura precedentemente raggiunta
		case INIT_LIQTEMPCONTR_STATE:
			LiquidTempContrState = READ_LIQTEMPCONTR_STATE;
			break;
		case READ_LIQTEMPCONTR_STATE:
			if(CurrTemp > (myTempValue + 10))
			{
				// la temperatura del reservoir e' superiore a quella impostata di almeno un grado
				// fermo le Peltier e mi metto in attesa che si raffreddino
				peltierCell.StopEnable = 1;
				peltierCell2.StopEnable = 1;
				LiquidTempContrState = WAIT_FOR_SET_T_LIQTEMPCONTR_STATE;
			}
			break;
		case WAIT_FOR_SET_T_LIQTEMPCONTR_STATE:
			if(CurrTemp <= (myTempValue + 2))
			{
				// la temperatura e' ritornata vicino al valore target posso far ripartire le Peltier
				// la differenza di 2 decimi di grado serve per poter far ripartire le peltier
				peltierCell.readAlwaysEnable = 0;
				peltierCell2.readAlwaysEnable = 0;
				LiquidTempContrState = INIT_LIQTEMPCONTR_STATE;
			}
			break;

		// controllo che il recipiente raggiunga il nuovo target impostato prima di passare
	    // nello stato di controllo del mantenimento del target
		case TEMP_START_CHECK_LIQTEMPCONTR_STATE:
			if((tmpr >= (float)(tmpr_trgt - 10)) && (tmpr <= (float)(tmpr_trgt + 10)))
			{
				// ho raggiunto la temperatura target
				LiquidTempContrState = TEMP_CHECK_DUR_LIQTEMPCONTR_STATE;
				TempOkTimeout = timerCounterModBus;
			}
			break;
		case TEMP_CHECK_DUR_LIQTEMPCONTR_STATE:
			if((tmpr >= (float)(tmpr_trgt - 10)) && (tmpr <= (float)(tmpr_trgt + 10)))
			{
				// ho raggiunto la temperatura target
				if(msTick_elapsed(TempOkTimeout) * 50L >= TIMEOUT_TEMPERATURE_RICIRC)
				{
					// per almeno 2 secondi la temperatura si e' mantenuta nell'intorno del target,
					// Considero il target di temperatura raggiunto e quindi posso passare alla fase
					// di controllo del mantenimento della temperatura
					LiquidTempContrState = INIT_LIQTEMPCONTR_STATE;
				}
			}
			else
			{
				LiquidTempContrState = TEMP_START_CHECK_LIQTEMPCONTR_STATE;
			}
			break;
	}
}

