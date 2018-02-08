/*
 * App_Ges.c
 *
 *  Created on: 03/nov/2016
 *      Author: W15
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

float pressSample1_Ven = 0;
float pressSample2_Ven = 0;

float pressSample1 = 0;  // FM Questi due vanno inizializzati alla pressione corrente prima di far partire il pid
float pressSample2 = 0;
unsigned char PidFirstTime[4];

// tick di inizio del trattamento
unsigned long StartTreatmentTime = 0;


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

	// inizializza il target di pressione venosa necessaria al PID per lavorare
	// parameterWordSetFromGUI[PAR_SET_VENOUS_PRESS_TARGET].value = SET_POINT_PRESSURE_INIT;
	// ed parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value = 2000;
	initSetParamInSourceCode();
	LastOxygenationSpeed = parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value;
	StartOxygAndDepState = 0;
	AlarmInPrimingEntered = FALSE;
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
/*
	static float myTempValue = 200;

	if(myTempValue != parameterWordSetFromGUI[PAR_SET_TEMPERATURE].value){
		myTempValue = parameterWordSetFromGUI[PAR_SET_TEMPERATURE].value;

		if(myTempValue == 40)
		{
			peltierCell.mySet  = myTempValue - 80;
			peltierCell2.mySet = myTempValue - 80;
		}
		else if(myTempValue == 360)
		{
			peltierCell.mySet  = myTempValue + 60;
			peltierCell2.mySet = myTempValue + 60;
		}
		else
		{
			peltierCell.mySet  = 200;
			peltierCell2.mySet = 200;
		}
	}

	computeMachineStateGuardTankFill();

*/
}

/*--------------------------------------------------------------*/
/* This function manages the state priming phase 1 activity     */
/*--------------------------------------------------------------*/
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
	static char iflag_perf = 0;
	static char iflag_oxyg = 0;

	//-------------------------------------------------------------------
	// Questo codice era nello stato ..TANK_FILL, lo ho spostato qui perche' non passo
	// piu' per quello stato
	static float myTempValue = 200;

	if(myTempValue != parameterWordSetFromGUI[PAR_SET_TEMPERATURE].value){
		myTempValue = parameterWordSetFromGUI[PAR_SET_TEMPERATURE].value;

		if(myTempValue == 40)
		{
			peltierCell.mySet  = myTempValue - 80;
			peltierCell2.mySet = myTempValue - 80;
		}
		else if(myTempValue == 360)
		{
			peltierCell.mySet  = myTempValue + 60;
			peltierCell2.mySet = myTempValue + 60;
		}
		else
		{
			peltierCell.mySet  = 200;
			peltierCell2.mySet = 200;
		}
	}
	//-----------------------------------------------------------------------


	//guard macchina a stati (controllo quando arriva il segnale per i passaggio alla fase 2)
	computeMachineStateGuardPrimingPh1();
}

/*--------------------------------------------------------------*/
/* This function manages the state priming phase 2 activity     */
/*--------------------------------------------------------------*/
void managePrimingPh2(void)
{
	//releaseGUIButton(BUTTON_CONFIRM);

	pumpPerist[0].entry = 0;
	pumpPerist[1].entry = 0;
	pumpPerist[2].entry = 0;
	pumpPerist[3].entry = 0;

	//currentGuard[GUARD_ENABLE_PRIMING_WAIT].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
	//currentGuard[GUARD_ENABLE_PRIMING_WAIT].guardValue = GUARD_VALUE_FALSE;

}

void managePrimingPh2Always(void)
{
	static char iflag_perf = 0;
	static char iflag_oxyg = 0;

	computeMachineStateGuardPrimingPh2();

	/*if(
		(buttonGUITreatment[BUTTON_START_PRIMING].state == GUI_BUTTON_PRESSED) ||
		(buttonGUITreatment[BUTTON_START_PERF_PUMP].state == GUI_BUTTON_PRESSED)
		)
	{
		releaseGUIButton(BUTTON_START_PRIMING);
		releaseGUIButton(BUTTON_START_PERF_PUMP);

		if(iflag_perf == 0)
		{
			setPumpSpeedValue(pumpPerist[0].pmpMySlaveAddress, 2000);
			iflag_perf = 1;
		}
	}
	else if(
			(buttonGUITreatment[BUTTON_STOP_ALL_PUMP].state == GUI_BUTTON_PRESSED) ||
			(buttonGUITreatment[BUTTON_STOP_PERF_PUMP].state == GUI_BUTTON_PRESSED)
			)
	{
		releaseGUIButton(BUTTON_STOP_ALL_PUMP);
		releaseGUIButton(BUTTON_STOP_PERF_PUMP);

		setPumpSpeedValue(pumpPerist[0].pmpMySlaveAddress, 0);
		iflag_perf = 0;
	}*/

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
	else if(GetTotalPrimingVolumePerf() > perfusionParam.priVolPerfArt) // parameterWordSetFromGUI[PAR_SET_PRIMING_VOL_PERFUSION].value
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

		startPeltierActuator();
		peltierCell.readAlwaysEnable = 1;
		peltierCell2.readAlwaysEnable = 1;

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

unsigned char TemperatureStateMach(void)
{
	static unsigned long RicircTimeout;
	static unsigned char TempStateMach = 0;
	unsigned char TempReached = 0;

	float tmpr;
	word tmpr_trgt;
	// temperatura raggiunta dal reservoir
	tmpr = ((int)(sensorIR_TM[0].tempSensValue*10));
	// temperatura da raggiungere moltiplicata per 10
	tmpr_trgt = parameterWordSetFromGUI[PAR_SET_PRIMING_TEMPERATURE_PERFUSION].value;

	switch (TempStateMach)
	{
		case 0:
			if((tmpr >= (float)(tmpr_trgt - 10)) && (tmpr <= (float)(tmpr_trgt + 10)))
			{
				// ho raggiunto la temperatura target
				TempStateMach = 1;
				RicircTimeout = timerCounterModBus;
			}
			break;
		case 1:
			if((tmpr >= (float)(tmpr_trgt - 10)) && (tmpr <= (float)(tmpr_trgt + 10)))
			{
				// ho raggiunto la temperatura target
				if(msTick_elapsed(RicircTimeout) * 50L >= TIMEOUT_TEMPERATURE_RICIRC)
				{
					// per almeno 2 secondi la temperatura si e' mantenuta nell'intorno del target
					TempReached = 1;
					TempStateMach = 0;
				}
			}
			else
			{
				TempStateMach = 0;
			}
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
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 2000);
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
	}
	else if(buttonGUITreatment[BUTTON_START_PERF_PUMP].state == GUI_BUTTON_RELEASED)
	{
		releaseGUIButton(BUTTON_START_PERF_PUMP);

		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 2000);
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
		volumePriming = volumePriming + ((float)speed * CONV_RPMMIN_TO_ML_PER_INTERVAL);
		perfusionParam.priVolPerfArt = (int)(volumePriming);
		pumpPerist[0].dataReady = DATA_READY_FALSE;
	}
	if(pumpPerist[1].dataReady == DATA_READY_TRUE)
	{
		pumpPerist[1].actualSpeed = modbusData[pumpPerist[1].pmpMySlaveAddress-2][17] / 100;
		// la pompa 2 e' agganciata alla 1
		pumpPerist[2].actualSpeed = modbusData[pumpPerist[2].pmpMySlaveAddress-2][17] / 100; //pumpPerist[1].actualSpeed;
		// calcolo il volume complessivo di liquido trattato dall'ossigenatore
		perfusionParam.priVolPerfVenOxy = perfusionParam.priVolPerfVenOxy +(word)((float)pumpPerist[1].actualSpeed * CONV_RPMMIN_TO_ML_PER_INTERVAL);
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
				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 2000);
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
			}
			else if(buttonGUITreatment[BUTTON_START_PERF_PUMP].state == GUI_BUTTON_RELEASED)
			{
				releaseGUIButton(BUTTON_START_PERF_PUMP);

				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 2000);
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
			}
#ifdef DEBUG_WITH_SERVICE_SBC
			else if((ptrCurrentState->state == STATE_PRIMING_PH_1) &&
					((float)perfusionParam.priVolPerfArt >= ((float)GetTotalPrimingVolumePerf() * 50 / 100.0)))  // parameterWordSetFromGUI[PAR_SET_PRIMING_VOL_PERFUSION].value
#else
			// nel caso di debug considero il 50%
			else if((ptrCurrentState->state == STATE_PRIMING_PH_1) &&
					((float)perfusionParam.priVolPerfArt >= ((float)GetTotalPrimingVolumePerf() * PERC_OF_PRIM_FOR_FILTER / 100.0)))  // parameterWordSetFromGUI[PAR_SET_PRIMING_VOL_PERFUSION].value
#endif
			{
				// ho raggiunto il 95% del volume, fermo le pompe ed aspetto il caricamento del filtro
				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
				if(GetTherapyType() == LiverTreat)
				{
					// se sono nel trattamento fegato fermo anche l'altro motore !!
					setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
				}
			}
			else if((ptrCurrentState->state == STATE_PRIMING_PH_2) &&
					(perfusionParam.priVolPerfArt >= GetTotalPrimingVolumePerf())) // parameterWordSetFromGUI[PAR_SET_PRIMING_VOL_PERFUSION].value
			{
				// ho raggiunto il volume complessivo, fermo le pompe ed aspetto
				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
				if(GetTherapyType() == LiverTreat)
				{
					// se sono nel trattamento fegato fermo anche l'altro motore !!
					setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
				}
				// faccio andare lo stato principale in nello stato di attesa di un nuovo volume
				// o di un fine priming
				currentGuard[GUARD_ENABLE_PRIMING_WAIT].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			}
			else if(ptrCurrentState->state == STATE_PRIMING_RICIRCOLO)
			{
//				float tmpr;
//				word tmpr_trgt;
//				// temperatura raggiunta dal reservoir
//				tmpr = ((int)(sensorIR_TM[0].tempSensValue*10));
//				// temperatura da raggiungere moltiplicata per 10
//				tmpr_trgt = parameterWordSetFromGUI[PAR_SET_PRIMING_TEMPERATURE_PERFUSION].value;

				//if((tmpr >= (float)(tmpr_trgt - 10)) && (tmpr <= (float)(tmpr_trgt + 10)))
				if(TemperatureStateMach())
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
			if(ptrCurrentState->state != STATE_PRIMING_RICIRCOLO)
			{
				// nella fase di ricircolo non aggiorno piu' il volume
				volumePriming = volumePriming + ((float)speed * CONV_RPMMIN_TO_ML_PER_INTERVAL);
				perfusionParam.priVolPerfArt = (int)(volumePriming);
			}
			pumpPerist[0].dataReady = DATA_READY_FALSE;
		}
		if(pumpPerist[1].dataReady == DATA_READY_TRUE)
		{
			pumpPerist[1].actualSpeed = modbusData[pumpPerist[1].pmpMySlaveAddress-2][17] / 100;
			// la pompa 2 e' agganciata alla 1
			pumpPerist[2].actualSpeed = modbusData[pumpPerist[2].pmpMySlaveAddress-2][17] / 100;//pumpPerist[1].actualSpeed;
			// calcolo il volume complessivo di liquido trattato dall'ossigenatore
			perfusionParam.priVolPerfVenOxy = perfusionParam.priVolPerfVenOxy +(word)((float)pumpPerist[1].actualSpeed * CONV_RPMMIN_TO_ML_PER_INTERVAL);
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

		startPeltierActuator();
		peltierCell.readAlwaysEnable = 1;
		peltierCell2.readAlwaysEnable = 1;

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
				parameterWordSetFromGUI[PAR_SET_VENOUS_PRESS_TARGET].value = SET_POINT_PRESSURE_INIT;
				// faccio partire la pompa di depurazione ad una velocita' prestabilita
				setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, LIVER_PPAR_SPEED);
			}

			releaseGUIButton(BUTTON_START_TREATMENT);

			//if(iflag_perf == 0)
			setPumpPressLoop(0, PRESS_LOOP_ON);
			// FM Questi due vanno inizializzati alla pressione corrente prima di far partire il pid, altrimenti il motore
			// potrebbe partire anche se non dovrebbe.
			pressSample1 = PR_ART_mmHg_Filtered;
			pressSample2 = PR_ART_mmHg_Filtered;
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
		}
		else if(buttonGUITreatment[BUTTON_START_PERF_PUMP].state == GUI_BUTTON_RELEASED){
			releaseGUIButton(BUTTON_START_PERF_PUMP);

			setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 2000); //pump 0: start value = 20 rpm than pressure loop

			setPumpPressLoop(0, PRESS_LOOP_ON);
			// FM Questi due vanno inizializzati alla pressione corrente prima di far partire il pid, altrimenti il motore
			// potrebbe partire anche se non dovrebbe.
			pressSample1 = PR_ART_mmHg_Filtered;
			pressSample2 = PR_ART_mmHg_Filtered;
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
				parameterWordSetFromGUI[PAR_SET_VENOUS_PRESS_TARGET].value = SET_POINT_PRESSURE_INIT;
			}

//			// attivo il pid sull'ossigenazione e perfusione venosa solo se sono in fegato
			if(GetTherapyType() == LiverTreat)
			{
				setPumpPressLoop(1, PRESS_LOOP_ON);
				pressSample1_Ven = PR_VEN_mmHg_Filtered;
				pressSample2_Ven = PR_VEN_mmHg_Filtered;
			}
		}

		if((getPumpPressLoop(0) == PRESS_LOOP_ON) && (timerCounterPID >=1))
		{
			timerCounterPID = 0;
			alwaysPumpPressLoop(0, &PidFirstTime[0]);
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
					parameterWordSetFromGUI[PAR_SET_VENOUS_PRESS_TARGET].value = SET_POINT_PRESSURE_INIT;
					// faccio partire la pompa di depurazione ad una velocita' prestabilita
					setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, LIVER_PPAR_SPEED);
				}
				releaseGUIButton(BUTTON_START_TREATMENT);

				//if(iflag_perf == 0)
				setPumpPressLoop(0, PRESS_LOOP_ON);
				// FM Questi due vanno inizializzati alla pressione corrente prima di far partire il pid, altrimenti il motore
				// potrebbe partire anche se non dovrebbe.
				pressSample1 = PR_ART_mmHg_Filtered;
				pressSample2 = PR_ART_mmHg_Filtered;
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
					pressSample1_Ven = PR_VEN_mmHg_Filtered;
					pressSample2_Ven = PR_VEN_mmHg_Filtered;
				}
			}
			else if(buttonGUITreatment[BUTTON_START_PERF_PUMP].state == GUI_BUTTON_RELEASED){
				releaseGUIButton(BUTTON_START_PERF_PUMP);

				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 2000); //pump 0: start value = 20 rpm than pressure loop

				setPumpPressLoop(0, PRESS_LOOP_ON);
				// FM Questi due vanno inizializzati alla pressione corrente prima di far partire il pid, altrimenti il motore
				// potrebbe partire anche se non dovrebbe.
				pressSample1 = PR_ART_mmHg_Filtered;
				pressSample2 = PR_ART_mmHg_Filtered;

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
					parameterWordSetFromGUI[PAR_SET_VENOUS_PRESS_TARGET].value = SET_POINT_PRESSURE_INIT;
				}

//				// attivo il pid sull'ossigenazione e perfusione venosa solo se sono in fegato
				if(GetTherapyType() == LiverTreat)
				{
					setPumpPressLoop(1, PRESS_LOOP_ON);
					pressSample1_Ven = PR_VEN_mmHg_Filtered;
					pressSample2_Ven = PR_VEN_mmHg_Filtered;
				}
			}

			if((getPumpPressLoop(0) == PRESS_LOOP_ON) && (timerCounterPID >=1))
			{
				timerCounterPID = 0;
				alwaysPumpPressLoop(0, &PidFirstTime[0]);
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


float parKITC = 0.2;
float parKP = 1;
float parKD_TC = 0.8;
float GlobINTEG;
float GlobPROP;
float GlobDER;
int deltaSpeed = 0;
int actualSpeed = 0;

void alwaysPumpPressLoop(unsigned char pmpId, unsigned char *PidFirstTime){
	//int deltaSpeed = 0;
	//static int actualSpeed = 0;
	//static int actualSpeedOld = 0;
	//float parKITC = 0.0; //0.2;
	//float parKP = 5.0;   //1;
	//float parKD_TC = 0.0; //0.8;
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

	GlobINTEG = parKITC*errPress;
	GlobPROP = parKP*(pressSample0 - pressSample1);
	GlobDER = parKD_TC*(pressSample0 - 2*pressSample1 + pressSample2);

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

float CalcolaPres(float speed)
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

int SpeedCostante( int CurrSpeed)
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
		if(CurrSpeed <= min || CurrSpeed > max )
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

extern word MedForVenousPid;
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

    /*funzione col solo proporzionale usata per mandare in oscillazione la pressione
     * e calcolare il K minimo e il periodo di oscillazione
     * //	deltaSpeed_Ven = parKP_Ven * errPress;*/

/*************************************************************************************************/
	// Queste righe di codice possono essere inserite se voglio bloccare il pid per
// qualche secondo dopo che ho applicato una variazione di velocita' per effetto di SpeedCostante.
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
    }

    //pressSample0_Ven = PR_VEN_mmHg_Filtered;
    pressSample0_Ven = MedForVenousPid;
	errPress = parameterWordSetFromGUI[PAR_SET_VENOUS_PRESS_TARGET].value - pressSample0_Ven;

	   if (errPress > -5  && errPress < 5 )
	   {
		   Count ++;
		   Somma_Speed += actualSpeed_Ven;

			if ( Count >= 5)
			{
				Speed_Media = Somma_Speed/Count;
				Speed_Media_old = Speed_Media;

				parameterWordSetFromGUI[PAR_SET_VENOUS_PRESS_TARGET].value  = CalcolaPres(Speed_Media);
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
	   if (SpeedCostante((int)actualSpeed_Ven) && (actualSpeed_Ven <= 100))
	   {
		   actualSpeed_Ven += 5.0;
		   parameterWordSetFromGUI[PAR_SET_VENOUS_PRESS_TARGET].value  = CalcolaPres(actualSpeed_Ven);
		   StopPid = TRUE;
		   StartTimePidStop = timerCounterModBus;
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
			/* da ripristinare solo quando siamo sicuri del funzionamento dei flussimetri
			if (sensor_UFLOW[1].Average_Flow_Val > ( parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value))

			{
				actualSpeed_Ven = actualSpeed_Ven; //non aumento la velocità
			}
			else */if ( (actualSpeed + deltaSpeed  ) > Max_RPM_for_Flow_Max )
			{
				actualSpeed_Ven = Max_RPM_for_Flow_Max;
			}
			else
			{
				if(actualSpeed_Ven >= (float)MAX_OXYG_RPM)
					actualSpeed_Ven = (float)MAX_OXYG_RPM;
				else
					actualSpeed_Ven = actualSpeed_Ven + deltaSpeed_Ven;
			}
		}
	}


	if(actualSpeed_Ven <= 0 || pressSample0_Ven > 190)
	{
		actualSpeed_Ven = 0;
		parameterWordSetFromGUI[PAR_SET_VENOUS_PRESS_TARGET].value = SET_POINT_PRESSURE_INIT;
	}


	if(actualSpeed_Ven != pumpPerist[pmpId].actualSpeedOld)
	{
		setPumpSpeedValueHighLevel(pumpPerist[pmpId].pmpMySlaveAddress, ((int)(actualSpeed_Ven * 100)));
		pumpPerist[pmpId].actualSpeedOld = actualSpeed_Ven;
	}

	pressSample2_Ven = pressSample1_Ven;
	pressSample1_Ven = pressSample0_Ven;
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


// Processo di svutamento del disposable----------------------------------------------
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
}

void EmptyDispStateMach(void)
{
	THERAPY_TYPE TherType = GetTherapyType();
	switch (EmptyDispRunAlwaysState)
	{
		case INIT_EMPTY_DISPOSABLE:
			if(buttonGUITreatment[BUTTON_START_EMPTY_DISPOSABLE].state == GUI_BUTTON_RELEASED)
			{
				// attivo la pompa per iniziare ko svuotamento
				releaseGUIButton(BUTTON_START_EMPTY_DISPOSABLE);

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
			break;
		case WAIT_FOR_1000ML:
			if(!EmptyDisposStartOtherPump && VolumeDischarged >= DISCHARGE_AMOUNT_ART_PUMP)
			{
				// faccio partire le altre pompe
				if(TherType == LiverTreat)
				{
					setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, LIVER_PPAR_SPEED);
					setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, LIVER_PPAR_SPEED);
				}
				else if(TherType == KidneyTreat)
					setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, KIDNEY_EMPTY_PPAR_SPEED);
				EmptyDisposStartOtherPump = TRUE;
				EmptyDispRunAlwaysState = WAIT_FOR_AIR_ALARM;
			}
			break;
		case WAIT_FOR_AIR_ALARM:
			// verificare che ci passa
			if((ptrAlarmCurrent->code == CODE_ALARM_AIR_PRES_ART) && (ptrAlarmCurrent->active = ACTIVE_TRUE))
			{
				if(TherType == LiverTreat)
				{
					// ho rilevato una presenza aria nel circuito di perfusione arteriosa o venosa
					setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
					setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
					EmptyDispRunAlwaysState = WAIT_FOR_LEVEL_OR_AMOUNT;
				}
				else if(TherType == KidneyTreat)
				{
					// ho rilevato una presenza aria nel circuito di perfusione arteriosa
					setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
					EmptyDispRunAlwaysState = WAIT_FOR_LEVEL_OR_AMOUNT;
				}
			}
			break;
		case WAIT_FOR_LEVEL_OR_AMOUNT:
			// rimango in questo stato, il controllo sul fine livello nella funzione ParentEmptyDispStateMach
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
	if(GetTherapyType() == LiverTreat)
		setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, LIVER_PPAR_SPEED);
	else if(GetTherapyType() == KidneyTreat)
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, KIDNEY_EMPTY_PPAR_SPEED);
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
static void computeMachineStateGuardMountDisp(void){
	/* Questi comandi non ci sono piu' nella nuova macchina a stati
	if(
		(buttonGUITreatment[BUTTON_PERF_DISP_MOUNTED].state == GUI_BUTTON_RELEASED) &&
		(buttonGUITreatment[BUTTON_OXYG_DISP_MOUNTED].state == GUI_BUTTON_RELEASED) &&
		(buttonGUITreatment[BUTTON_CONFIRM].state == GUI_BUTTON_RELEASED)
			)
	{
		currentGuard[GUARD_ENABLE_TANK_FILL].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		releaseGUIButton(BUTTON_PERF_DISP_MOUNTED);
		releaseGUIButton(BUTTON_OXYG_DISP_MOUNTED);
		//releaseGUIButton(BUTTON_CONFIRM);
	}
	*/

	// controllo che sia finito il posizionamento delle pinch poi passo alla fase di priming
}

/*--------------------------------------------------------------------*/
/*  This function compute the guard value in tank fill state   */
/*--------------------------------------------------------------------*/
static void computeMachineStateGuardTankFill(void){
/*
	if(
		(buttonGUITreatment[BUTTON_PERF_TANK_FILL].state == GUI_BUTTON_RELEASED) &&
		(buttonGUITreatment[BUTTON_CONFIRM].state == GUI_BUTTON_RELEASED)
			)
	{
		currentGuard[GUARD_ENABLE_PRIMING_PHASE_1].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		releaseGUIButton(BUTTON_PERF_TANK_FILL);
		releaseGUIButton(BUTTON_CONFIRM);
	}
*/
}

/*--------------------------------------------------------------------*/
/*  This function compute the guard value in priming phase 1 state   */
/*--------------------------------------------------------------------*/
static void computeMachineStateGuardPrimingPh1(void){
/*	CODICE ORIGINALE
	if(
		(buttonGUITreatment[BUTTON_PERF_FILTER_MOUNT].state == GUI_BUTTON_RELEASED) &&
		(buttonGUITreatment[BUTTON_CONFIRM].state == GUI_BUTTON_RELEASED)
			)
	{
		currentGuard[GUARD_ENABLE_PRIMING_PHASE_2].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		releaseGUIButton(BUTTON_PERF_FILTER_MOUNT);
		releaseGUIButton(BUTTON_CONFIRM);
		//setPumpSpeedValue(pumpPerist[0].pmpMySlaveAddress, 0);
	}
	*/
	if( buttonGUITreatment[BUTTON_PRIMING_FILT_INS_CONFIRM].state == GUI_BUTTON_RELEASED)
	{
		FilterSelected = TRUE;
		currentGuard[GUARD_ENABLE_PRIMING_PHASE_2].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		releaseGUIButton(BUTTON_PRIMING_FILT_INS_CONFIRM);
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

/*--------------------------------------------------------------------*/
/*  This function compute the guard value in priming phase 2 state   */
/*  Controllo quando iniziare il trattamento
/*--------------------------------------------------------------------*/
static void computeMachineStateGuardPrimingPh2(void){
/* vecchia gestione
	if(
		(perfusionParam.priVolPerfArt >= GetTotalPrimingVolumePerf()) &&
		(buttonGUITreatment[BUTTON_CONFIRM].state == GUI_BUTTON_RELEASED) / *&&
		(iflag_pmp1_rx == IFLAG_PMP1_RX)* /
		)
	{
		StartTreatmentTime = timerCounterModBus;
		currentGuard[GUARD_ENABLE_TREATMENT_KIDNEY_1].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		releaseGUIButton(BUTTON_CONFIRM);
		//setPumpSpeedValue(pumpPerist[0].pmpMySlaveAddress, 0);
	}
*/
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



/*--------------------------------------------------------------------*/
/*  This function compute the guard value in treatment kidney 1 state   */
/*  Controllo la fine del trattamento
/*--------------------------------------------------------------------*/
static void computeMachineStateGuardTreatment(void)
{
//	static int EndTreatmentState = 0;
//	static unsigned long StartTimeout = 0;
//	unsigned long ul;

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
		// esaurita la durata massima del trattamento
		// forzo lo stop alle pompe passo allo svuotamento del circuito
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, newSpeedPmp_0);
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, newSpeedPmp1_2);
		if(GetTherapyType() == LiverTreat)
		{
			// se sono nel trattamento fegato fermo anche l'altro motore !!
			setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, newSpeedPmp_3);
		}

		if ( CommandModBusPMPExecute(newSpeedPmp_0,newSpeedPmp1_2,newSpeedPmp_3) )
		{
			currentGuard[GUARD_ENABLE_DISPOSABLE_EMPTY].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			DebugStringStr("TREATMENT END");
		}
		else
		{
			// controllo il tempo trascorso per un eventuale timeout di errore
		}
	}
	else
	{
		// controllo se e' cambiata la velocita' della pompa di ossigenazione e la aggiorno
		CheckOxygenationSpeed(LastOxygenationSpeed);
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
			   (PR_LEVEL_mmHg_Filtered <= (760 + (float)10.0 * 760.0 / 100.0)))
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
				// (FM) chiamo la funzione child che gestisce lo stato di allarme durante il trattamento
				// Dovra fare tutte le attuazioni sulle pompe, pinch necessarie per risolvere la condizione
				// di allarme
				ManageStateChildAlarmTreat1();
			}
			break;

		case PARENT_EMPTY_DISPOSABLE_END:
			break;
	}
}


// ritorna il volume complessivo di priming tenendo conto anche del volume di liquido
// necessario per riempire il disposable
word GetTotalPrimingVolumePerf(void)
{
	word TotVolume;
	TotVolume = parameterWordSetFromGUI[PAR_SET_PRIMING_VOL_PERFUSION].value + VOLUME_DISPOSABLE;
	return TotVolume;
}

/*----------------------------------------------------------------------------*/
/* This function compute the machine state transition based on guard - state level         */
/*----------------------------------------------------------------------------*/
void processMachineState(void)
{
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
			if(
				(currentGuard[GUARD_ENABLE_PRIMING_PHASE_2].guardValue == GUARD_VALUE_TRUE)
				)
			{
				currentGuard[GUARD_ENABLE_PRIMING_PHASE_2].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
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
			manageStateEntryAndStateAlways(14);
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
				/*torno indietro nella macchina a stati quindi resetto la flag di entry sullo stato in cui sono*/
				currentGuard[GUARD_ENABLE_DISPOSABLE_EMPTY].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
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
			}

			break;

		case PARENT_PRIMING_TREAT_KIDNEY_1_RUN:
//			if((float)perfusionParam.priVolPerfArt >= ((float)GetTotalPrimingVolumePerf() * PERC_OF_PRIM_FOR_FILTER / 100.0))
//			{
//				// posso usarlo per un eventuale cambio di stato nel parent ed entrare in una fase di caricamento del filtro
//			}
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
			}
			break;

		case PARENT_PRIMING_TREAT_KIDNEY_1_ALARM:
			if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_FALSE)
			{
				/* (FM) finita la situazione di allarme posso ritornare in PARENT_PRIMING_TREAT_KIDNEY_1_INIT*/
				// nella nuova gestione il priming viene fatto partendo direttamente dallo stato PARENT_PRIMING_TREAT_KIDNEY_1_RUN
				// e non da PARENT_PRIMING_TREAT_KIDNEY_1_INIT
				//ptrFutureParent = &stateParentPrimingTreatKidney1[1];
				ptrFutureParent = &stateParentPrimingTreatKidney1[3];
				ptrFutureChild = ptrFutureParent->ptrChild;
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
				// guardando a questo valore posso vedere il tipo di azione di sicurezza
				// e quindi posso decidere di andare anche in un qualche altro stato ad hoc
				// di allarme
				//ptrAlarmCurrent->secActType
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
				// guardando a questo valore posso vedere il tipo di azione di sicurezza
				// e quindi posso decidere di andare anche in un qualche altro stato ad hoc
				// di allarme
				//ptrAlarmCurrent->secActType
			}
			break;

		case PARENT_TREAT_KIDNEY_1_ALARM:
			if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_FALSE)
			{
				/* FM allarme finito posso ritornare nella fase iniziale del trattamento */
				ptrFutureParent = &stateParentTreatKidney1[1];
				ptrFutureChild = ptrFutureParent->ptrChild;
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
		static unsigned char maskGuard;

		case CHILD_PRIMING_ALARM_INIT:

            /* (FM) qui ci entra quando nel ciclo switch precedente del parent e durante il priming si verifica un allarme.
               La tabella del parent contiene gia' il riferimento alla struttura 1 di inizializzazione della tabella del child. */
			maskGuard = (currentGuard[GUARD_ALARM_STOP_ALL_ACTUATOR].guardValue == GUARD_VALUE_TRUE) 		|
						((currentGuard[GUARD_ALARM_STOP_ALL].guardValue == GUARD_VALUE_TRUE) << 1)			|
						((currentGuard[GUARD_ALARM_STOP_PERF_PUMP].guardValue == GUARD_VALUE_TRUE) << 2)	|
						((currentGuard[GUARD_ALARM_STOP_PURIF_PUMP].guardValue == GUARD_VALUE_TRUE) << 3)	|
						((currentGuard[GUARD_ALARM_STOP_OXYG_PUMP].guardValue == GUARD_VALUE_TRUE) << 4)	|
						((currentGuard[GUARD_ALARM_STOP_PELTIER].guardValue == GUARD_VALUE_TRUE) << 5);

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

	if ( config_data.EEPROM_Control != EEPROM_WRITTEN)
	{
		 config_data.sensor_PRx[OXYG].prSensGain   = PR_OXYG_GAIN;
		 config_data.sensor_PRx[OXYG].prSensOffset = PR_OXYG_OFFSET;

		 config_data.sensor_PRx[LEVEL].prSensGain   = PR_LEVEL_GAIN;
		 config_data.sensor_PRx[LEVEL].prSensOffset = PR_LEVEL_OFFSET;

		 config_data.sensor_PRx[ADS_FLT].prSensGain   = PR_ADS_FLT_GAIN;
		 config_data.sensor_PRx[ADS_FLT].prSensOffset = PR_ADS_FLT_OFFSET;

		 config_data.sensor_PRx[VEN].prSensGain   = PR_VEN_GAIN;
		 config_data.sensor_PRx[VEN].prSensOffset = PR_VEN_OFFSET;

		 config_data.sensor_PRx[ART].prSensGain   = PR_ART_GAIN;
		 config_data.sensor_PRx[ART].prSensOffset = PR_ART_OFFSET;

		 /*word di controllo che la flash usata come eeprom sia
		  * stata scritta almeno una volta; TODO si può mettere un CRC
		  * e non un valore fisso per renderlo ancora + sicuro*/
		 config_data.EEPROM_Control = EEPROM_WRITTEN;

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

