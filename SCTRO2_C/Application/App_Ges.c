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
#include "ControlProtectiveInterface_C.h"
#include "VOLTAGE_B_CHK.h"
#include "BitIoLdd36.h"
#include "VOLTAGE_M_CHK.h"
#include "BitIoLdd37.h"

#include "Pins1.h"

#include "MODBUS_COMM.h"
#include "ASerialLdd1.h"
#include "EEPROM.h"
#include "IntFlashLdd1.h"

#include "D_7S_DP.h"
#include "D_7S_A.h"
#include "D_7S_B.h"
#include "D_7S_C.h"
#include "D_7S_D.h"
#include "D_7S_E.h"
#include "D_7S_F.h"
#include "D_7S_G.h"
/*
#include "COVER_M1.h"
#include "COVER_M2.h"
#include "COVER_M3.h"
#include "COVER_M4.h"
#include "COVER_M5.h"
*/

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
#include "AIR_T_1.h"
#include "AIR_T_2.h"
#include "AIR_T_3.h"
#include "AIR_SENSOR.h"
#include "EMERGENCY_BUTTON.h"
#include "FRONTAL_COVER_1.h"
#include "FRONTAL_COVER_2.h"
#include "HOOK_SENSOR_1.h"
#include "HOOK_SENSOR_2.h"
#include "Adc_Ges.h"
#include "SWTimer.h"


bool IsPinchPosOk(unsigned char *pArrPinchPos);

// Variabili usate per i T1 Test sui segnali digitali
extern unsigned char coverStateGlobal;
bool gDigitalTest = FALSE; //Risultato finale dei T1 test sui sensori digitali

DigitalTestStruct digitalStruct[NUM_SENSORI_DIGITALI];
// Flags usati nel processo di svuotamento
extern CHILD_EMPTY_FLAGS ChildEmptyFlags;

extern float pressSample1_Ven;
extern float pressSample2_Ven;
extern float pressSample1_Art;
extern float pressSample2_Art;
extern word MedForArteriousPid;
extern word MedForVenousPid;

extern float pressSample1; // FM Questi due vanno inizializzati alla pressione corrente prima di far partire il pid
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
// ultimo valore della velocita' della pompa di depurazione impostata.
word LastDepurationSpeed;

// serve per dare lo start alla depurazione ed ossigenazione dopo che e' stato
// scaricato un po di materiale nel reservoir.
// 0 = materiale non scaricato
// 1 = materiale scaricato asufficienza per far partire le altre pompe
unsigned char StartOxygAndDepState = 0;

bool AlarmInPrimingEntered = FALSE;

// quando e' true indica che si e' verificato un allarme durante la fase di ricircolo per cui mi
// sono dovuto fermare. Alla ripartenza devo decidere se ripartire in alta velocita' o no.
bool AlarmOrStopInRecircFlag = FALSE;

// usato negli stati di allarme per ricordarmi di aver ricevuto almeno un BUTTON_RESET
bool AtLeastoneButResRcvd = FALSE;
// usato negli stati di allarme child per ricordarmi di aver ricevuto un BUTTON_RESET
bool ResButInChildFlag = FALSE;

// variabile usata dalla StopAllCntrlAlarm per salvare l'ultima configurazione delle flags prima di azzerarle
GLOBAL_FLAGS AlarmEnableConf;
// Filippo - aggiunto per test T1 del frigo
int counterFridgeStability;
float tempMinFridgePlate;
float tempMaxHeatPlate;

int speed_filter;
int speed_Arterious;
int speed_Venous;
int timerCounterRampaAccPmpFlt;
int timerCounterRampaAccPmpVenArt;
int TimeAtStopRiciclo;
int TimerCHK24V;


void CallInIdleState(void)
{
	memset(ParamRcvdInMounting, 0, sizeof(ParamRcvdInMounting));
	AllParametersReceived = 0;
	TherapyCmdArrived = 0;

	// inizializzo tutte  i guard ed i guibutton nel caso qualcuno rimanesse impostato
	initAllGuard();
	initGUIButton();

	// inserisco un valore di terapia indefinita per costringere sbc ad inviare una nuova terapia prima di ripartire per un secondo trattamento
	// non faccio la init perche' verra' fatta dopo che avro' selezionato la nuova terapia
	SetTherapyType(Undef);
	parameterWordSetFromGUI[PAR_SET_PRIMING_VOL_PERFUSION].value = 0;
	parameterWordSetFromGUI[PAR_SET_PRIMING_TEMPERATURE_PERFUSION].value = 0;
	parameterWordSetFromGUI[PAR_SET_OXYGENATOR_ACTIVE].value = NOT_DEF; // undef
	parameterWordSetFromGUI[PAR_SET_DEPURATION_ACTIVE].value = NOT_DEF; // undef

	// questi parametri non li reinizializzo di nuovo
//	parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value = 1000;
//	parameterWordSetFromGUI[PAR_SET_PRESS_ART_TARGET].value = 50;
//	parameterWordSetFromGUI[PAR_SET_DESIRED_DURATION].value = 0x100;
//	parameterWordSetFromGUI[PAR_SET_MAX_FLOW_PERFUSION].value = 400;

	// inizializzo a 0 il volume accumulato per il priming
	volumeTreatArt = 0;
	perfusionParam.treatVolPerfArt = 0;
	// inizializzo a 0 il volume accumulato durante il trattamento
	volumePriming = 0;
	perfusionParam.priVolPerfArt = 0;
	perfusionParam.priVolPerfVenOxy = 0;
	perfusionParam.priVolAdsFilter = 0;
	perfusionParam.treatVolPerfVenOxy = 0;
	perfusionParam.treatVolAdsFilter = 0;

	perfusionParam.unlVolAdsFilter = 0;
	perfusionParam.unlVolPerfArt = 0;
	perfusionParam.unlVolPerfVenOxy = 0;
	perfusionParam.unlVolRes = 0;
	perfusionParam.unlDurPerfArt = 0;
	perfusionParam.unlDurPerVenOxy = 0;
	perfusionParam.pressDropAdsFilter = 0;

	TotalTreatDuration = 0;
	StartTreatmentTime = 0;
	TreatDuration = 0;
	FilterSelected = FALSE;
	TotalPrimingDuration = 0;
	StartPrimingTime = 0;
	PrimingDuration = 0;

	// inizializza il target di pressione venosa necessaria al PID per lavorare
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
	SetAllAlarmEnableFlags();
	//GlobalFlags.FlagsDef.EnableCANBUSErr = 0;  // disabilito CAN bus alarm

	if (PeltierOn && (peltierCell.StopEnable == 0))
	{
		// se erano accese le spengo
		peltierCell.StopEnable = 1;
		peltierCell2.StopEnable = 1;
	}
	LevelBuzzer = SILENT;//0;
	PeltierStarted = FALSE;
	LiquidTempContrTask(RESET_LIQUID_TEMP_CONTR_CMD);
	PrimDurUntilOxyStart = 0;
	FilterFlowVal = 0;
	OxygenFlowRpm = 0;
	LastDepurationSpeed = 0;
	TreatSetPinchPosTask((TREAT_SET_PINCH_POS_CMD)T_SET_PINCH_DISABLE_CMD);
	DisableBadPinchPosAlmFunc();
	DisablePrimAlmSFAAirDetAlmFunc();
	ClearAlarmState();
	ResetPrimPinchAlm();
	CheckCurrPinchPosTask(CHECK_CURR_PINCH_POS_DISABLE_CMD);
	AtLeastoneButResRcvd = FALSE;
	ResButInChildFlag = FALSE;
	// default
	parameterWordSetFromGUI[PAR_SET_DESIRED_DURATION].value = 0x0014; // 20 minuti
	// Filippo - resetto il frigo e il riscaldatore perch� potrei arrivare da stati in cui li uso
//  	FrigoHeatTempControlTask((LIQ_TEMP_CONTR_TASK_CMD)LIQ_T_CONTR_TASK_RESET_CMD);
	// Filippo - cambiato funzione per gestire nuovo PID che usa il frigo e il riscaldatore insieme
  	FrigoHeatTempControlTaskNewPID((LIQ_TEMP_CONTR_TASK_CMD)LIQ_T_CONTR_TASK_RESET_CMD);
  	// durante la fase di priming questo allarme deve essere disabilitato
  	GlobalFlags.FlagsDef.EnableTempArtOORAlm = 0;
  	speed_filter = 0;
  	speed_Arterious = 0;
  	speed_Venous = 0;
  	timerCounterRampaAccPmpFlt = 0;
  	timerCounterRampaAccPmpVenArt = 0;
	TimeAtStopRiciclo = 0;
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
	// inzializzo il task di controllo della velocita' delle pompe a 0
	CheckPumpStopTask((CHECK_PUMP_STOP_CMD)INIT_CHECK_SEQ_CMD);

	releaseGUIButton(BUTTON_CONFIRM);
}

// Filippo - funzione per la gestione degli allarmi in idle
void manageIdleAlarm(void)
{
	if (ptrCurrentParent->parent==PARENT_IDLE_ALARM)
	{
		manageStateChildAlarmIdle();
	}
}

void manageStateIdleAlways(void)
{
	// Filippo qui devo inserire la gestione dell'allarme?!?!?!?!?!?
	if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_TRUE)
	{
		// Filippo - adesso devi gestire l'allarme e chiamo la funzione di gestione
		manageIdleAlarm();
	}
	else
	{
		computeMachineStateGuardIdle();
		CheckPumpStopTask((CHECK_PUMP_STOP_CMD)NO_CHECK_PUMP_STOP_CMD);
	}
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
		case BUTTON_PINCH_2WPVF_RIGHT_OPEN:   //= 0xA1,   // pinch filter (pinch in basso aperto a destra)
			setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_RIGHT_OPEN);
			break;
		case BUTTON_PINCH_2WPVF_LEFT_OPEN:    //= 0xA0,    // pinch filter (pinch in basso aperto a sinistra)
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

	if(buttonGUITreatment[BUTTON_PINCH_2WPVF_RIGHT_OPEN].state == GUI_BUTTON_RELEASED)
	{
		// pinch filter (pinch in basso aperto a destra)
		releaseGUIButton(BUTTON_PINCH_2WPVF_RIGHT_OPEN);
		HandlePinch(BUTTON_PINCH_2WPVF_RIGHT_OPEN);
	}
	if(buttonGUITreatment[BUTTON_PINCH_2WPVF_LEFT_OPEN].state == GUI_BUTTON_RELEASED)
	{
		// pinch filter (pinch in basso aperto a sinistra)
		releaseGUIButton(BUTTON_PINCH_2WPVF_LEFT_OPEN);
		HandlePinch(BUTTON_PINCH_2WPVF_LEFT_OPEN);
	}
	if(buttonGUITreatment[BUTTON_PINCH_2WPVF_BOTH_CLOSED].state == GUI_BUTTON_RELEASED)
	{
		// pinch filter (pinch in basso entrambi chiusi)
		releaseGUIButton(BUTTON_PINCH_2WPVF_BOTH_CLOSED);
		HandlePinch(BUTTON_PINCH_2WPVF_BOTH_CLOSED);
	}
	if(buttonGUITreatment[BUTTON_PINCH_2WPVA_RIGHT_OPEN].state == GUI_BUTTON_RELEASED)
	{
		// pinch arterial (pinch di sinistra - aperto a destra)
		releaseGUIButton(BUTTON_PINCH_2WPVA_RIGHT_OPEN);
		HandlePinch(BUTTON_PINCH_2WPVA_RIGHT_OPEN);
	}
	if(buttonGUITreatment[BUTTON_PINCH_2WPVA_LEFT_OPEN].state == GUI_BUTTON_RELEASED)
	{
		// pinch arterial (pinch di sinistra - aperto a sinistra)
		releaseGUIButton(BUTTON_PINCH_2WPVA_LEFT_OPEN);
		HandlePinch(BUTTON_PINCH_2WPVA_LEFT_OPEN);
	}
	if(buttonGUITreatment[BUTTON_PINCH_2WPVA_BOTH_CLOSED].state == GUI_BUTTON_RELEASED)
	{
		// pinch arterial (pinch di sinistra - entrambi chiusi)
		releaseGUIButton(BUTTON_PINCH_2WPVA_BOTH_CLOSED);
		HandlePinch(BUTTON_PINCH_2WPVA_BOTH_CLOSED);
	}
	if(buttonGUITreatment[BUTTON_PINCH_2WPVV_RIGHT_OPEN].state == GUI_BUTTON_RELEASED)
	{
		// pinch venous (pinch di destra - aperto a destra)
		releaseGUIButton(BUTTON_PINCH_2WPVV_RIGHT_OPEN);
		HandlePinch(BUTTON_PINCH_2WPVV_RIGHT_OPEN);
	}
	if(buttonGUITreatment[BUTTON_PINCH_2WPVV_LEFT_OPEN].state == GUI_BUTTON_RELEASED)
	{
		// pinch venous (pinch di destra - aperto a sinistra)
		releaseGUIButton(BUTTON_PINCH_2WPVV_LEFT_OPEN);
		HandlePinch(BUTTON_PINCH_2WPVV_LEFT_OPEN);
	}
	if(buttonGUITreatment[BUTTON_PINCH_2WPVV_BOTH_CLOSED].state == GUI_BUTTON_RELEASED)
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
			// questo verra' fatto nella funzione LiquidTempContrTask
			//peltierCell.readAlwaysEnable = 0;
			//peltierCell2.readAlwaysEnable = 0;
			CheckTemperatureSet();
			LiquidTempContrTask(RESET_LIQUID_TEMP_CONTR_CMD);
			PeltierStarted = TRUE;
		}
	}

	// se in questa fase ricevo un nuovo valore di flusso nella linea di ossigenazione lo prendo
	// come valore attuale.
	LastOxygenationSpeed = parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value;
	LastDepurationSpeed = 0;
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
			// questo verra' fatto nella funzione LiquidTempContrTask
			//peltierCell.readAlwaysEnable = 0;
			//peltierCell2.readAlwaysEnable = 0;
		}
		else if(myTempValue >= 320 && myTempValue <= 370)
		{
			peltierCell.mySet  = (float) 62.0;
			peltierCell2.mySet = (float) 62.0;
			// questo verra' fatto nella funzione LiquidTempContrTask
			//peltierCell.readAlwaysEnable = 0;
			//peltierCell2.readAlwaysEnable = 0;
		}
		else
		{
			peltierCell.mySet  = 20;
			peltierCell2.mySet = 20;
			// questo verra' fatto nella funzione LiquidTempContrTask
			//peltierCell.readAlwaysEnable = 0;
			//peltierCell2.readAlwaysEnable = 0;
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

	// il codice che segue serve SOLO PER DEBUG E NELLA VERSIONE DEFINITIVA
	// DEVE ESSERE CANCELLATO
//	if(buttonGUITreatment[BUTTON_START_OXYGEN_PUMP].state == GUI_BUTTON_RELEASED)
//	{
//		releaseGUIButton(BUTTON_START_OXYGEN_PUMP);
//		ActuatorWriteCnt[0] += 1;
//	}
//	else if(buttonGUITreatment[BUTTON_STOP_OXYGEN_PUMP].state == GUI_BUTTON_RELEASED)
//	{
//		releaseGUIButton(BUTTON_STOP_OXYGEN_PUMP);
//		ActuatorWriteCnt[0] = 0;
//	}
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
	EnableBadPinchPosAlmFunc();
	// abilito allarme di delta di temperatura eccessivo
	GlobalFlags.FlagsDef.EnableTempArtOORAlm = 1;
}

void manageStateTreatKidney1Always(void)
{
	computeMachineStateGuardTreatment();
	// Filippo - in questo punto devo verificare ogni 10 secondi (TBD) che il sensore d'aria funzioni
	// tramite un test
	airSensorTest();



	// il codice che segue serve SOLO PER DEBUG E NELLA VERSIONE DEFINITIVA
	// DEVE ESSERE CANCELLATO
//	if(buttonGUITreatment[BUTTON_START_OXYGEN_PUMP].state == GUI_BUTTON_RELEASED)
//	{
//		releaseGUIButton(BUTTON_START_OXYGEN_PUMP);
//		ActuatorWriteCnt[0] += 1;
//	}
//	else if(buttonGUITreatment[BUTTON_STOP_OXYGEN_PUMP].state == GUI_BUTTON_RELEASED)
//	{
//		releaseGUIButton(BUTTON_STOP_OXYGEN_PUMP);
//		ActuatorWriteCnt[0] = 0;
//	}
}

/*-----------------------------------------------------------*/
/* This function manages the state empty disposable activity */
/* Entrata nello stato di svutamento del disposable          */
/*-----------------------------------------------------------*/

void manageStateEmptyDisp(void)
{
	setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_LEFT_OPEN);
	setPinchPositionHighLevel(PINCH_2WPVA, MODBUS_PINCH_LEFT_OPEN);
	if(GetTherapyType() == LiverTreat)
	{
		// ho selezionato il fegato, quindi devo chiudere anche questa
		setPinchPositionHighLevel(PINCH_2WPVV, MODBUS_PINCH_LEFT_OPEN);
	}
	DisableBadPinchPosAlmFunc();
	DisablePrimAlmSFAAirDetAlmFunc();
}

void manageStateEmptyDispAlways(void)
{
	ParentEmptyDispStateMach();

	/*Vincenzo: controllo se durante lo svuotamento viene premuto il tasto di abandon
	 * in tal caso fermo tutte le pompe senza verificare se sono usate o no, poi la
	 * la macchina a stati mi porter� nello stato di smontaggio*/

	if(buttonGUITreatment[BUTTON_ABANDON_EMPTY].state == GUI_BUTTON_RELEASED)
	{
		releaseGUIButton(BUTTON_ABANDON_EMPTY);
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);

		setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);


		currentGuard[GUARD_ABANDON_EMPTY].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
	}

	// il codice che segue serve SOLO PER DEBUG E NELLA VERSIONE DEFINITIVA
	// DEVE ESSERE CANCELLATO
//	if(buttonGUITreatment[BUTTON_START_OXYGEN_PUMP].state == GUI_BUTTON_RELEASED)
//	{
//		releaseGUIButton(BUTTON_START_OXYGEN_PUMP);
//		ActuatorWriteCnt[0] += 1;
//	}
//	else if(buttonGUITreatment[BUTTON_STOP_OXYGEN_PUMP].state == GUI_BUTTON_RELEASED)
//	{
//		releaseGUIButton(BUTTON_STOP_OXYGEN_PUMP);
//		ActuatorWriteCnt[0] = 0;
//	}
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
		//currentGuard[GUARD_ABANDON_PRIMING].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		SetAbandonGuard();
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
		//currentGuard[GUARD_ABANDON_PRIMING].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		SetAbandonGuard();
	}
	else
	{
		// controllo se e' cambiata la velocita' della pompa di ossigenazione e la aggiorno
		//CheckOxygenationSpeed(LastOxygenationSpeed);
	}
}


//------------------------------------------------------------------

void manageStateWaitTreatment(void)
{
}

void manageStateWaitTreatmentAlways(void)
{
	if(buttonGUITreatment[BUTTON_START_TREATMENT].state == GUI_BUTTON_RELEASED)
	{
		// prendo BUTTON_START_TREATMENT per ricominciare un nuovo trattamento
		releaseGUIButton(BUTTON_START_TREATMENT);
		currentGuard[GUARD_ENABLE_TREATMENT_KIDNEY_1].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		// il trattamento deve ricominciare, quindi abilito di nuovo il task delle peltier definitivamente le peltier
		PeltierStarted = TRUE;
	}
	else if(buttonGUITreatment[BUTTON_END_TREATMENT].state == GUI_BUTTON_RELEASED)
	{
		// prendo BUTTON_END_TREATMENT per andare in empty disposable
		releaseGUIButton(BUTTON_END_TREATMENT);
		currentGuard[GUARD_ENABLE_DISPOSABLE_EMPTY].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;

		// fermo eventuale riscaldamento e raffreddamento
//		FrigoHeatTempControlTask((LIQ_TEMP_CONTR_TASK_CMD)TEMP_MANAGER_STOPPED_CMD);
		// Filippo - cambio funzione per gestire nuovo PID che usa frigo e riscaldatore insieme
		FrigoHeatTempControlTaskNewPID((LIQ_TEMP_CONTR_TASK_CMD)TEMP_MANAGER_STOPPED_CMD);
	}

}

void manageStateUnmountDisposableEntry(void)
{
	// inzializzo il task di controllo della velocita' delle pompe a 0
	CheckPumpStopTask((CHECK_PUMP_STOP_CMD)INIT_CHECK_SEQ_CMD);
}
void manageStateUnmountDisposableAlways(void)
{
	CheckPumpStopTask((CHECK_PUMP_STOP_CMD)NO_CHECK_PUMP_STOP_CMD);
	if(buttonGUITreatment[BUTTON_PINCH_2WPVF_RIGHT_OPEN].state == GUI_BUTTON_RELEASED)
	{
		// pinch filter (pinch in basso aperto a destra)
		releaseGUIButton(BUTTON_PINCH_2WPVF_RIGHT_OPEN);
		HandlePinch(BUTTON_PINCH_2WPVF_RIGHT_OPEN);
	}
	if(buttonGUITreatment[BUTTON_PINCH_2WPVF_LEFT_OPEN].state == GUI_BUTTON_RELEASED)
	{
		// pinch filter (pinch in basso aperto a sinistra)
		releaseGUIButton(BUTTON_PINCH_2WPVF_LEFT_OPEN);
		HandlePinch(BUTTON_PINCH_2WPVF_LEFT_OPEN);
	}
	if(buttonGUITreatment[BUTTON_PINCH_2WPVF_BOTH_CLOSED].state == GUI_BUTTON_RELEASED)
	{
		// pinch filter (pinch in basso entrambi chiusi)
		releaseGUIButton(BUTTON_PINCH_2WPVF_BOTH_CLOSED);
		HandlePinch(BUTTON_PINCH_2WPVF_BOTH_CLOSED);
	}
	if(buttonGUITreatment[BUTTON_PINCH_2WPVA_RIGHT_OPEN].state == GUI_BUTTON_RELEASED)
	{
		// pinch arterial (pinch di sinistra - aperto a destra)
		releaseGUIButton(BUTTON_PINCH_2WPVA_RIGHT_OPEN);
		HandlePinch(BUTTON_PINCH_2WPVA_RIGHT_OPEN);
	}
	if(buttonGUITreatment[BUTTON_PINCH_2WPVA_LEFT_OPEN].state == GUI_BUTTON_RELEASED)
	{
		// pinch arterial (pinch di sinistra - aperto a sinistra)
		releaseGUIButton(BUTTON_PINCH_2WPVA_LEFT_OPEN);
		HandlePinch(BUTTON_PINCH_2WPVA_LEFT_OPEN);
	}
	if(buttonGUITreatment[BUTTON_PINCH_2WPVA_BOTH_CLOSED].state == GUI_BUTTON_RELEASED)
	{
		// pinch arterial (pinch di sinistra - entrambi chiusi)
		releaseGUIButton(BUTTON_PINCH_2WPVA_BOTH_CLOSED);
		HandlePinch(BUTTON_PINCH_2WPVA_BOTH_CLOSED);
	}
	if(buttonGUITreatment[BUTTON_PINCH_2WPVV_RIGHT_OPEN].state == GUI_BUTTON_RELEASED)
	{
		// pinch venous (pinch di destra - aperto a destra)
		releaseGUIButton(BUTTON_PINCH_2WPVV_RIGHT_OPEN);
		HandlePinch(BUTTON_PINCH_2WPVV_RIGHT_OPEN);
	}
	if(buttonGUITreatment[BUTTON_PINCH_2WPVV_LEFT_OPEN].state == GUI_BUTTON_RELEASED)
	{
		// pinch venous (pinch di destra - aperto a sinistra)
		releaseGUIButton(BUTTON_PINCH_2WPVV_LEFT_OPEN);
		HandlePinch(BUTTON_PINCH_2WPVV_LEFT_OPEN);
	}
	if(buttonGUITreatment[BUTTON_PINCH_2WPVV_BOTH_CLOSED].state == GUI_BUTTON_RELEASED)
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
	else if(buttonGUITreatment[BUTTON_ABANDON_UNMOUNT].state == GUI_BUTTON_RELEASED)
	{
		// mi e' stato richiesto di abbandonare la fase di smontaggio e andare direttamente in IDLE
		releaseGUIButton(BUTTON_ABANDON_UNMOUNT);
		currentGuard[GUARD_ABANDON_UNMOUNT].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
	//	SetAbandonGuard();
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
	// Filippo - gestisco eventuali situazioni di allarme
	if (ptrCurrentParent->parent==PARENT_T1_NO_DISP_ALARM)
	{
		// sono andato in allarme devo gestirlo
		manageStateChildAlarmT1();
	}



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
		// abilito allarme di aria nel filtro durante il priming
		EnablePrimAlmSFAAirDetAlmFunc();
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


/*----------------------------------------INIZIO PARENT T1 test -----------------------------------------------*/

void initT1Test(void){
	ptrT1Test->result_T1_cfg_data = T1_TEST_KO;
	ptrT1Test->result_T1_24vbrk = T1_TEST_KO;
	ptrT1Test->result_T1_press = T1_TEST_KO;
	ptrT1Test->result_T1_tempIR = T1_TEST_KO;
	ptrT1Test->result_T1_level = T1_TEST_KO;
	ptrT1Test->result_T1_flwmtr = T1_TEST_KO;
	ptrT1Test->result_T1_air = T1_TEST_KO;
	ptrT1Test->result_T1_pinch = T1_TEST_KO;
	ptrT1Test->result_T1_pump = T1_TEST_KO;
	ptrT1Test->result_T1_termo = T1_TEST_KO;
}


void manageParentChkConfig(void)
{
	//leggo dati da eeprom
	DebugStringStr("manage chk config");
	EEPROM_Read(START_ADDRESS_EEPROM, (EEPROM_TDataAddress)&config_data, sizeof(config_data));

	unsigned char *ptr_EEPROM = (EEPROM_TDataAddress)&config_data;

	/*Calcolo il CRC sui dati letti dalla EEPROM */
	unsigned int Calc_CRC_EEPROM = ComputeChecksum(ptr_EEPROM, sizeof(config_data)-2);

	/*Se il CRC calcolato non � uguale a quello letto o la revsione non � uguale a quella attesa
			 vado in allarme t1 test */
	if (config_data.EEPROM_CRC != Calc_CRC_EEPROM || config_data.EEPROM_Revision != EEPROM_REVISION)
	{
		ptrT1Test->result_T1_cfg_data = T1_TEST_KO;
		currentGuard[GUARD_ENABLE_T1_ALARM].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		DebugStringStr("alarm from chk config");
	}
	else{
		ptrT1Test->result_T1_cfg_data = T1_TEST_OK;
		DebugStringStr("test config ok");
	}
}

void manageParentChk24Vbrk_init(void)
{
	t1Test_chk24_state = IDLE;
	TimerCHK24V = 0;
}

void manageParentChk24Vbrk(void){
	// attenzione ai guadagni e agli offset delle 24V, che scalate, diventano 2,5V;
	// modifiche necessarie in global.h

	unsigned char voltageBoard, voltageMotor;

	voltageBoard = VOLTAGE_B_CHK_GetVal();
	voltageMotor = VOLTAGE_M_CHK_GetVal();

	/*VINCY lavoro T1 Test:
	 *qui posso mettere anche il test su disattivazione Control e Protective
	 *qui della 24, alla riattvazione dovr� mettere un'attesa di 5 secondi */
	
	switch (t1Test_chk24_state)
	{
		case IDLE:
			/*mi assicuro di abilitare la 24 VOLT sia su OCN che su PRO*/
			onNewCommadT1TEST(C2PCOM_POWER_ON);
			EN_24_M_C_Management(ENABLE);
			t1Test_chk24_state = WAIT_1;
			TimerCHK24V = timerCounterModBus;
		break;

		case WAIT_1:
			/*aspetto 5 secondi poi cambio stato*/
			if (msTick_elapsed(TimerCHK24V) >= 100)
				t1Test_chk24_state = CHK_VOLTAGE;
			break;

		case CHK_VOLTAGE:
			/*controllo i livelli di alimentazioni per la scheda e i motori*/
			if(
			   (V24_P1_CHK_VOLT <= V24BRK_LOW_THRSLD) ||
			   (V24_P1_CHK_VOLT >= V24BRK_HIGH_THRSLD) ||
			   (V24_P2_CHK_VOLT <= V24BRK_LOW_THRSLD) ||
			   (V24_P2_CHK_VOLT >= V24BRK_HIGH_THRSLD) ||
			   (voltageBoard != 0x01) ||
			   (voltageMotor != 0x01)
			   )
				t1Test_chk24_state = CHK_VOLTAGE_KO;
			else
				t1Test_chk24_state = DISABLE_CON_VOLTAGE;
		break;

		case DISABLE_CON_VOLTAGE:
			/*spengo la 24 dei motori*/
			EN_24_M_C_Management(DISABLE);
			t1Test_chk24_state = CHK_NO_POWER_1;
			TimerCHK24V = timerCounterModBus;
		break;

		case CHK_NO_POWER_1:
			//verifico che perda almeno 10 messaggi su ogni dirver
			if (CountErrorModbusMSG[0] >= 10 &&
				CountErrorModbusMSG[1] >= 10 &&
				CountErrorModbusMSG[2] >= 10 &&
				CountErrorModbusMSG[3] >= 10 &&
				CountErrorModbusMSG[5] >= 10 &&
				CountErrorModbusMSG[6] >= 10 &&
				CountErrorModbusMSG[7] >= 10 )
			{
				t1Test_chk24_state = ENABLE_CON_VOLTAGE;
			}
			/*se passano 15 secondi e non perdo pi� di 10 messaggi vuol dire che almeno un driver � acceso*/
			else if (msTick_elapsed(TimerCHK24V) >= 300)
			{
				t1Test_chk24_state = CHK_VOLTAGE_KO;
				DebugStringStr("Disable Power Motor CONTROL Failure");
			}
		break;

		case ENABLE_CON_VOLTAGE:
			/*abilito nuovamente la 24 Motor*/
			EN_24_M_C_Management(ENABLE);
			TimerCHK24V = timerCounterModBus;
			t1Test_chk24_state = WAIT_2;
		break;

		case WAIT_2:
			/*aspetto un timer di 5 sec (100 volte 50 msec)*/
			if (msTick_elapsed(TimerCHK24V) >= 100)
				t1Test_chk24_state = CHK_VOLTAGE_ENABLED_1;
		break;

		case CHK_VOLTAGE_ENABLED_1:
			/*verifico che l'alimentazione sia tornata..se ho ancora + di 10 messaggi
			 * non ricevuti suppongo che l'alimentazione non sia tornata */
			if (CountErrorModbusMSG[0] >= 10 &&
				CountErrorModbusMSG[1] >= 10 &&
				CountErrorModbusMSG[2] >= 10 &&
				CountErrorModbusMSG[3] >= 10 &&
				CountErrorModbusMSG[5] >= 10 &&
				CountErrorModbusMSG[6] >= 10 &&
				CountErrorModbusMSG[7] >= 10 )
			{
				t1Test_chk24_state = CHK_VOLTAGE_KO;
				DebugStringStr("Enable Power Motor CONTROL Failure");
			}
			else
				t1Test_chk24_state = COMMAND_PRO_DISABLE_VOLTAGE;
		break;

		case COMMAND_PRO_DISABLE_VOLTAGE:
			/*mando il comando alla PRO di staccare la 24*/
			onNewCommadT1TEST(C2PCOM_POWER_OFF);
			TimerCHK24V = timerCounterModBus;
			t1Test_chk24_state = CHEK_NO_POWER_2;
		break;

		case CHEK_NO_POWER_2:
			/*verifico che la potenza sia andata gi�*/
			if (CountErrorModbusMSG[0] >= 10 &&
				CountErrorModbusMSG[1] >= 10 &&
				CountErrorModbusMSG[2] >= 10 &&
				CountErrorModbusMSG[3] >= 10 &&
				CountErrorModbusMSG[5] >= 10 &&
				CountErrorModbusMSG[6] >= 10 &&
				CountErrorModbusMSG[7] >= 10)
				t1Test_chk24_state = COMMAND_PRO_ENABLE_VOLTAGE;
			/*altrimenti se scadono 5 secondi vado in allarme*/
			else if (msTick_elapsed(TimerCHK24V) >= 100)
			{
				t1Test_chk24_state = CHK_VOLTAGE_KO;
				DebugStringStr("Disable Power Motor PROTECTIVE Failure");
			}
		break;

		case COMMAND_PRO_ENABLE_VOLTAGE:
			/*chiedo alla PRO di riattivare la 24*/
			onNewCommadT1TEST(C2PCOM_POWER_ON);
			TimerCHK24V = timerCounterModBus;
			t1Test_chk24_state = CHK_VOLTAGE_ENABLED_2;
		break;

		case CHK_VOLTAGE_ENABLED_2:
			/*verifico che la tensione sia tornata su*/
			if (CountErrorModbusMSG[0] <= 10 &&
				CountErrorModbusMSG[1] <= 10 &&
				CountErrorModbusMSG[2] <= 10 &&
				CountErrorModbusMSG[3] <= 10 &&
				CountErrorModbusMSG[5] <= 10 &&
				CountErrorModbusMSG[6] <= 10 &&
				CountErrorModbusMSG[7] <= 10)
			{
				t1Test_chk24_state = CHK_VOLTAGE_OK;
			}
			/*altrimenti se scadono 5 secondi vado in allarme*/
			else if (msTick_elapsed(TimerCHK24V) >= 100)
			{
				t1Test_chk24_state = CHK_VOLTAGE_KO;
				DebugStringStr("Enable Power Motor PROTECTIVE Failure");
			}
		break;

		case CHK_VOLTAGE_OK:
			/*test superato*/
			ptrT1Test->result_T1_24vbrk = T1_TEST_OK;
			t1Test_chk24_state = IDLE;
		break;

		case CHK_VOLTAGE_KO:
			ptrT1Test->result_T1_24vbrk = T1_TEST_KO;
			currentGuard[GUARD_ENABLE_T1_ALARM].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			DebugStringStr("alarm from chk 24vbrk");
			t1Test_chk24_state = IDLE;
		break;

		default:
			ptrT1Test->result_T1_24vbrk = T1_TEST_KO;
			currentGuard[GUARD_ENABLE_T1_ALARM].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			DebugStringStr("alarm from chk 24vbrk --> WRONG MACINE STATE");
			t1Test_chk24_state = IDLE;
		break;

	}

//	if(
//	   (V24_P1_CHK_VOLT <= V24BRK_LOW_THRSLD) ||
//	   (V24_P1_CHK_VOLT >= V24BRK_HIGH_THRSLD) ||
//	   (V24_P2_CHK_VOLT <= V24BRK_LOW_THRSLD) ||
//	   (V24_P2_CHK_VOLT >= V24BRK_HIGH_THRSLD) ||
//	   (voltageBoard != 0x01) ||
//	   (voltageMotor != 0x01)
//	   )
//
//	{
//		ptrT1Test->result_T1_24vbrk = T1_TEST_KO;
//		currentGuard[GUARD_ENABLE_T1_ALARM].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
//		DebugStringStr("alarm from chk 24vbrk");
//	}
//	else{
//		ptrT1Test->result_T1_24vbrk = T1_TEST_OK;
//	}
}

void manageParentChkPress(void){

// Check sui sensori Control e Protective
	if(
		(PR_OXYG_mmHg_Filtered <= T1_TEST_PRESS_LOW_THRSLD)  	  ||
		(PR_OXYG_mmHg_Filtered >= T1_TEST_PRESS_HIGH_THRSLD) 	  ||
		(PR_LEVEL_mmHg_Filtered <= T1_TEST_PRESS_LOW_THRSLD_LEV)  ||
		(PR_LEVEL_mmHg_Filtered >= T1_TEST_PRESS_HIGH_THRSLD_LEV) ||
		(PR_ADS_FLT_mmHg_Filtered <= T1_TEST_PRESS_LOW_THRSLD) 	  ||
		(PR_ADS_FLT_mmHg_Filtered >= T1_TEST_PRESS_HIGH_THRSLD)   ||
		(PR_VEN_mmHg_Filtered <= T1_TEST_PRESS_LOW_THRSLD) 		  ||
		(PR_VEN_mmHg_Filtered >= T1_TEST_PRESS_HIGH_THRSLD) 	  ||
		(PR_ART_mmHg_Filtered <= T1_TEST_PRESS_LOW_THRSLD) 		  ||
		(PR_ART_mmHg_Filtered >= T1_TEST_PRESS_HIGH_THRSLD)       ||
		(*PressOxy <= T1_TEST_PRESS_LOW_THRSLD)  	              ||
		(*PressOxy >= T1_TEST_PRESS_HIGH_THRSLD)  	              ||
		(*PressFilter <= T1_TEST_PRESS_LOW_THRSLD)     	          ||
		(*PressFilter >= T1_TEST_PRESS_HIGH_THRSLD) 	          ||
		(*PressArt <= T1_TEST_PRESS_LOW_THRSLD)  	              ||
		(*PressArt >= T1_TEST_PRESS_HIGH_THRSLD) 	              ||
		(*PressVen <= T1_TEST_PRESS_LOW_THRSLD) 	              ||
		(*PressVen >= T1_TEST_PRESS_HIGH_THRSLD)  				  ||
		(*PressLevelx100 <= T1_TEST_PRESS_LOW_THRSLD_LEV) 	      ||
		(*PressLevelx100 >= T1_TEST_PRESS_HIGH_THRSLD_LEV)
		){
		ptrT1Test->result_T1_press = T1_TEST_KO;
		currentGuard[GUARD_ENABLE_T1_ALARM].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		DebugStringStr("alarm from chk press");
	}
	else{
		ptrT1Test->result_T1_press = T1_TEST_OK;
	}
}

void manageParentTempSensIR(void){

	static unsigned char index = 0;

//Check sui sensori Control e Protective
	float tempProtSTA2 = ((float)(*STA2)/10.0);
	float tempProtSTF2 = ((float)(*STF2)/10.0);
	float tempProtSTV2 = ((float)(*STV2)/10.0);

	if(
	   (sensorIR_TM[0].tempSensValue < LOW_ENV_TEMP_T1_TEST)  ||
	   (sensorIR_TM[0].tempSensValue > HIGH_ENV_TEMP_T1_TEST) ||
	   (sensorIR_TM[1].tempSensValue < LOW_ENV_TEMP_T1_TEST)  ||
	   (sensorIR_TM[1].tempSensValue > HIGH_ENV_TEMP_T1_TEST) ||
	   (sensorIR_TM[2].tempSensValue < LOW_ENV_TEMP_T1_TEST)  ||
	   (sensorIR_TM[2].tempSensValue > HIGH_ENV_TEMP_T1_TEST) ||
	   (tempProtSTA2 < LOW_ENV_TEMP_T1_TEST)  ||
	   (tempProtSTA2 > HIGH_ENV_TEMP_T1_TEST) ||
	   (tempProtSTF2 < LOW_ENV_TEMP_T1_TEST)  ||
	   (tempProtSTF2 > HIGH_ENV_TEMP_T1_TEST) ||
	   (tempProtSTV2 < LOW_ENV_TEMP_T1_TEST)  ||
	   (tempProtSTV2 > HIGH_ENV_TEMP_T1_TEST) ||
	   sensorIR_TM[0].ErrorMSG > MAX_MSG_CONSECUTIVE_IR_TEMP_SENS_NOT_DETECTED ||
	   sensorIR_TM[1].ErrorMSG > MAX_MSG_CONSECUTIVE_IR_TEMP_SENS_NOT_DETECTED ||
	   sensorIR_TM[2].ErrorMSG > MAX_MSG_CONSECUTIVE_IR_TEMP_SENS_NOT_DETECTED

	   ){
			ptrT1Test->result_T1_tempIR = T1_TEST_KO;
			currentGuard[GUARD_ENABLE_T1_ALARM].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			DebugStringStr("alarm from chk tempIR");
	}
	else{
		ptrT1Test->result_T1_tempIR = T1_TEST_OK;
	}

}


void mangeParentUFlowSens(void){
	if(
			(sensor_UFLOW[0].Average_Flow_Val > HIGH_ENV_FLOW_T1_TEST) ||
			(sensor_UFLOW[0].Average_Flow_Val < LOW_ENV_FLOW_T1_TEST)  ||
			(sensor_UFLOW[1].Average_Flow_Val > HIGH_ENV_FLOW_T1_TEST) ||
			(sensor_UFLOW[1].Average_Flow_Val < LOW_ENV_FLOW_T1_TEST)
		){
		ptrT1Test->result_T1_flwmtr = T1_TEST_KO;
		currentGuard[GUARD_ENABLE_T1_ALARM].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		DebugStringStr("alarm from chk flow");
	}
	else{
		ptrT1Test->result_T1_flwmtr = T1_TEST_OK;
	}
}

// Inizializzazione delle strutture dati usate per i T1 test sui sensori digitali
void InitDigitalT1Test(void)
{
	unsigned char id = 0;

	for (id = 0; id < NUM_SENSORI_DIGITALI; id ++)
	{
		digitalStruct[id].pass = FALSE;
		digitalStruct[id].current = 0;
		digitalStruct[id].previous = 0;
	}
}

//T1 test relativo ai segnali digitali
//Cover pompe 1-2-3-4, coperchi frontali sx e dx, segnali gancio sx e dx
void manageParentDigitalT1Test(void)
{
	char id = 0;

	//Acquisizione degli stati dei sensori digitali
	digitalStruct[HOOK1].current = HOOK_SENSOR_1_STATUS;						// 1 --> premuto
	digitalStruct[HOOK2].current = HOOK_SENSOR_2_STATUS;						// 1 --> premuto
	digitalStruct[PUMP_COVER1].current = (coverStateGlobal & 0x01);				// 1 --> open, 0 --> close
	digitalStruct[PUMP_COVER2].current = ((coverStateGlobal & 0x02) >> 1);		// 1 --> open, 0 --> close
	digitalStruct[PUMP_COVER3].current = ((coverStateGlobal & 0x04) >> 2);		// 1 --> open, 0 --> close
	digitalStruct[PUMP_COVER4].current = ((coverStateGlobal & 0x08) >> 3);		// 1 --> open, 0 --> close
#ifdef FRONTAL_COVERS_T1_TEST_ENABLED
	digitalStruct[FRONTAL_COVER1].current = FRONTAL_COVER_1_STATUS;				// 1 --> chiuso
	digitalStruct[FRONTAL_COVER2].current = FRONTAL_COVER_2_STATUS;				// 1 --> chiuso
#endif

    // Viene eseguita una scansione sui sensori digitali
	for (id = 0; id < NUM_SENSORI_DIGITALI; id ++)
	{
		//Stato attuale: 0 --> premuto oppure chiuso mentre lo stato precedente era NON premuto oppure aperto <-- 1
		if ((digitalStruct[id].current == 0) && (digitalStruct[id].previous == 1))
			digitalStruct[id].pass = TRUE;
		//Aggiorna il valore precedente
		digitalStruct[id].previous = digitalStruct[id].current;
	}

	// Il test, nella sua interezza (gDigitalTest), � considerato PASS solo se tutti i test sono a loro volta passati.
	for (id = 0; id < NUM_SENSORI_DIGITALI; id ++)
	{
		if (digitalStruct[id].pass == FALSE)
		{
			gDigitalTest = FALSE;
			return; // Al primo test NON passato, esce anzitempo
		}
	}

	// Se sono arrivato qui significa che tutti i test sono passati
	gDigitalTest = TRUE;
}

void manageParenT1PinchInit(void){
	t1Test_pinch_state = 0;
	t1Test_pinch_cmd = 0;
	timerCounterT1Test = 0;
}

void manageParenT1Pinch(void)
{
	switch(t1Test_pinch_state)
	{
	case PINCH_IDLE: //idle
		t1Test_pinch_state = PINCH_CLOSE;
		onNewCommadT1TEST(C2PCOM_ENABPINCH_ON);
		EN_Clamp_Control(ENABLE);
		break;

	case PINCH_CLOSE: //close
		/*per sicurezza, se ho appena attivato l'enable, attendo 3 secondi*/
		if (timerCounterT1Test < 60)
			break;

		if(t1Test_pinch_cmd == 0){
		setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_POS_CLOSED); //7: pinch filtro
		setPinchPositionHighLevel(PINCH_2WPVA, MODBUS_PINCH_POS_CLOSED); //8: pinch art
		setPinchPositionHighLevel(PINCH_2WPVV, MODBUS_PINCH_POS_CLOSED); //9: pinch ven
		t1Test_pinch_cmd = 1;
		}

		if(
			(*FilterPinchPos == MODBUS_PINCH_POS_CLOSED) &&
			(*ArtPinchPos == MODBUS_PINCH_POS_CLOSED)    &&
			(*OxygPinchPos == MODBUS_PINCH_POS_CLOSED)   &&
			modbusData[4][0] == MODBUS_PINCH_POS_CLOSED  &&
			modbusData[5][0] == MODBUS_PINCH_POS_CLOSED  &&
			modbusData[6][0] == MODBUS_PINCH_POS_CLOSED  &&
			(timerCounterT1Test > 60)
			)
		{ //60*50ms equal 3 sec
			t1Test_pinch_state = PINCH_LEFT;
			t1Test_pinch_cmd = 0;
			timerCounterT1Test = 0;
		}
		else if(timerCounterT1Test > 200)
		{//200*50ms equal 10 sec
			t1Test_pinch_state = PINCH_KO; //ho avuto un'errore, finisco nello stato d'errore per sincornizzarmi cn la macchina a stati del parent.c
			DebugStringStr("Pinch not all closed");
		}
		break;

	case PINCH_LEFT: //left
		if(t1Test_pinch_cmd == 0){
			setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_LEFT_OPEN); //7: pinch filtro
			setPinchPositionHighLevel(PINCH_2WPVA, MODBUS_PINCH_LEFT_OPEN); //8: pinch art
			setPinchPositionHighLevel(PINCH_2WPVV, MODBUS_PINCH_LEFT_OPEN); //9: pinch ven
			t1Test_pinch_cmd = 1;
		}

		if(
			(*FilterPinchPos == MODBUS_PINCH_LEFT_OPEN) &&
			(*ArtPinchPos == MODBUS_PINCH_LEFT_OPEN)    &&
			(*OxygPinchPos == MODBUS_PINCH_LEFT_OPEN)   &&
			modbusData[4][0] == MODBUS_PINCH_LEFT_OPEN  &&
			modbusData[5][0] == MODBUS_PINCH_LEFT_OPEN  &&
			modbusData[6][0] == MODBUS_PINCH_LEFT_OPEN  &&
			(timerCounterT1Test > 60)
			)
		{ //60*50ms equal 3 sec
			t1Test_pinch_state = PINCH_RIGHT;
			t1Test_pinch_cmd = 0;
			timerCounterT1Test = 0;
		}
		else if(timerCounterT1Test > 200)
		{//200*50ms equal 10 sec
			t1Test_pinch_state = PINCH_KO; //ho avuto un'errore, finisco nello stato d'errore per sincornizzarmi cn la macchina a stati del parent.c
			DebugStringStr("Pinch not all left position");
		}
		break;

	case PINCH_RIGHT: //right
		if(t1Test_pinch_cmd == 0){
		setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_RIGHT_OPEN); //7: pinch filtro
		setPinchPositionHighLevel(PINCH_2WPVA, MODBUS_PINCH_RIGHT_OPEN); //8: pinch art
		setPinchPositionHighLevel(PINCH_2WPVV, MODBUS_PINCH_RIGHT_OPEN); //9: pinch ven
		t1Test_pinch_cmd = 1;
		}

		if(
			(*FilterPinchPos == MODBUS_PINCH_RIGHT_OPEN) &&
			(*ArtPinchPos == MODBUS_PINCH_RIGHT_OPEN)    &&
			(*OxygPinchPos == MODBUS_PINCH_RIGHT_OPEN)   &&
			modbusData[4][0] == MODBUS_PINCH_RIGHT_OPEN  &&
			modbusData[5][0] == MODBUS_PINCH_RIGHT_OPEN  &&
			modbusData[6][0] == MODBUS_PINCH_RIGHT_OPEN  &&
			(timerCounterT1Test > 60)
			)
		{ //60*50ms equal 3 sec
			t1Test_pinch_state = PINCH_DIS_CON; //test finito con esito positivo
			t1Test_pinch_cmd = 0;
			timerCounterT1Test = 0;
			//chiudo nuovamente tutte le pinch per tornare nella situazione di partenza
			setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_POS_CLOSED); //7: pinch filtro
			setPinchPositionHighLevel(PINCH_2WPVA, MODBUS_PINCH_POS_CLOSED); //8: pinch art
			setPinchPositionHighLevel(PINCH_2WPVV, MODBUS_PINCH_POS_CLOSED); //9: pinch ven
		}
		else if(timerCounterT1Test > 200)
		{//200*50ms equal 10 sec
			t1Test_pinch_state = PINCH_KO; //ho avuto un'errore, finisco nello stato d'errore per sincornizzarmi cn la macchina a stati del parent.
			DebugStringStr("Pinch not all right position");
		}
		break;

	case PINCH_DIS_CON:
		/*in questo stato la control toglie l'enable*/
		EN_Clamp_Control(DISABLE);
		t1Test_pinch_state = CHK_PINCH_DIS_CON;
		timerCounterT1Test = 0;
		break;

	case CHK_PINCH_DIS_CON:
		/*verifico che le posizioni viste da pro e inviate dai driver a con
		 * siano coerenti con le posizioni di disable (filtro a sinistra e le altre due a destra)*/
		if (
			(*FilterPinchPos == MODBUS_PINCH_LEFT_OPEN) &&
			(*ArtPinchPos == MODBUS_PINCH_RIGHT_OPEN) &&
			(*OxygPinchPos == MODBUS_PINCH_RIGHT_OPEN) &&
			modbusData[4][0] == MODBUS_PINCH_LEFT_OPEN &&
			modbusData[5][0] == MODBUS_PINCH_RIGHT_OPEN &&
			modbusData[6][0] == MODBUS_PINCH_RIGHT_OPEN
			)
		{
			t1Test_pinch_state = PINCH_EN_CON;
			timerCounterT1Test = 0;
		}
		else if (timerCounterT1Test > 60)//60*50ms equal 3 sec
		{
			t1Test_pinch_state = PINCH_KO; //ho avuto un'errore, finisco nello stato d'errore per sincornizzarmi cn la macchina a stati del parent.
			DebugStringStr("Disable Pinch CONTROL not working");
		}
		break;

	case PINCH_EN_CON:
		/*in questo stato la control rimette l'enable e comanda
		 * la chiusura di tutte le pinch*/
		EN_Clamp_Control(ENABLE);
		//aspetto 3 secondi poi chiudo tutte le pinch
		if (timerCounterT1Test < 60)
			break;

		setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_POS_CLOSED); //7: pinch filtro
		setPinchPositionHighLevel(PINCH_2WPVA, MODBUS_PINCH_POS_CLOSED); //8: pinch art
		setPinchPositionHighLevel(PINCH_2WPVV, MODBUS_PINCH_POS_CLOSED); //9: pinch ven

		t1Test_pinch_state = CHK_PINCH_EN_CON;
		timerCounterT1Test = 0;
		break;

	case CHK_PINCH_EN_CON:
		/*verifico che le posizioni viste da pro e inviate dai driver a con
		 * siano coerenti con le posizioni di chiuso*/
		if (
			(*FilterPinchPos == MODBUS_PINCH_POS_CLOSED) &&
			(*ArtPinchPos == MODBUS_PINCH_POS_CLOSED) &&
			(*OxygPinchPos == MODBUS_PINCH_POS_CLOSED) &&
			modbusData[4][0] == MODBUS_PINCH_POS_CLOSED &&
			modbusData[5][0] == MODBUS_PINCH_POS_CLOSED &&
			modbusData[6][0] == MODBUS_PINCH_POS_CLOSED
			)
		{
			t1Test_pinch_state = PINCH_DIS_PRO;
			timerCounterT1Test = 0;
		}
		else if (timerCounterT1Test > 60)//60*50ms equal 3 sec
		{
			t1Test_pinch_state = PINCH_KO; //ho avuto un'errore, finisco nello stato d'errore per sincornizzarmi cn la macchina a stati del parent.
			DebugStringStr("Enable Pinch CONTROL not working");
		}
		break;

	case PINCH_DIS_PRO:
		/*in questo stato la Control chiede alla pro di togliere l'enable*/
		onNewCommadT1TEST(C2PCOM_ENABPINCH_OFF);
		t1Test_pinch_state = CHK_PINCH_DIS_PRO;
		timerCounterT1Test = 0;
		break;

	case CHK_PINCH_DIS_PRO:
		/*verifico che le posizioni viste da pro e inviate dai driver a con
		 * siano coerenti con le posizioni di disale (filtro a sinistra e le altre due a destra)*/
		if (
			(*FilterPinchPos == MODBUS_PINCH_LEFT_OPEN) &&
			(*ArtPinchPos == MODBUS_PINCH_RIGHT_OPEN) &&
			(*OxygPinchPos == MODBUS_PINCH_RIGHT_OPEN) &&
			modbusData[4][0] == MODBUS_PINCH_LEFT_OPEN &&
			modbusData[5][0] == MODBUS_PINCH_RIGHT_OPEN &&
			modbusData[6][0] == MODBUS_PINCH_RIGHT_OPEN
			)
		{
			t1Test_pinch_state = PINCH_EN_PRO;
			timerCounterT1Test = 0;
		}
		else if (timerCounterT1Test > 60)//60*50ms equal 3 sec
		{
			t1Test_pinch_state = PINCH_KO; //ho avuto un'errore, finisco nello stato d'errore per sincornizzarmi cn la macchina a stati del parent.
			DebugStringStr("Disable Pinch PROTECTIVE not working");
		}
		break;

	case PINCH_EN_PRO:
		/*in questo stato la control chiede alla pro di rimettere l'enable e comanda
		 * la chiusura di tutte le pinch*/
		onNewCommadT1TEST(C2PCOM_ENABPINCH_ON);
		//aspetto 3 secondi poi chiudo tutte le pinch
		if (timerCounterT1Test < 60)
			break;

		setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_POS_CLOSED); //7: pinch filtro
		setPinchPositionHighLevel(PINCH_2WPVA, MODBUS_PINCH_POS_CLOSED); //8: pinch art
		setPinchPositionHighLevel(PINCH_2WPVV, MODBUS_PINCH_POS_CLOSED); //9: pinch ven

		t1Test_pinch_state = CHK_PINCH_EN_PRO;
		timerCounterT1Test = 0;
		break;

	case CHK_PINCH_EN_PRO:
		/*verifico che le posizioni viste da pro e inviate dai driver a con
		 * siano coerenti con le posizioni di chiuso*/
		if (
			(*FilterPinchPos == MODBUS_PINCH_POS_CLOSED) &&
			(*ArtPinchPos == MODBUS_PINCH_POS_CLOSED) &&
			(*OxygPinchPos == MODBUS_PINCH_POS_CLOSED) &&
			modbusData[4][0] == MODBUS_PINCH_POS_CLOSED &&
			modbusData[5][0] == MODBUS_PINCH_POS_CLOSED &&
			modbusData[6][0] == MODBUS_PINCH_POS_CLOSED
			)
		{
			t1Test_pinch_state = PINCH_OK;
			timerCounterT1Test = 0;
		}
		else if (timerCounterT1Test > 60)//60*50ms equal 3 sec
		{
			t1Test_pinch_state = PINCH_KO; //ho avuto un'errore, finisco nello stato d'errore per sincornizzarmi cn la macchina a stati del parent.
			DebugStringStr("Enable Pinch PROTECTIVE not working");
		}
		break;
	case PINCH_OK:
		//fine test positivo
		t1Test_pinch_state = PINCH_IDLE;
		timerCounterT1Test = 0;
		break;

	case PINCH_KO:
		//fine test con esito negativo
		timerCounterT1Test = 0;
		currentGuard[GUARD_ENABLE_T1_ALARM].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		DebugStringStr("alarm from chk pinch");
		break;

	default:
		/*se finisco qui c'� un errore nella gesgtione della macchina a stati*/
		timerCounterT1Test = 0;
		currentGuard[GUARD_ENABLE_T1_ALARM].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		DebugStringStr("alarm from chk pinch --> WRONG machine state");
		t1Test_pinch_state = PINCH_KO; //ho avuto un'errore, finisco nello stato d'errore per sincornizzarmi cn la macchina a stati del parent.c
		t1Test_pinch_cmd = 0;
		break;
	}
}

void manageParentT1PumpInit(void){
	t1Test_pump_state = 0;
	t1Test_pump_cmd = 0;
	timerCounterT1Test = 0;
}

void manageParentT1Pump(void){
	//setPumpSpeedValueHighLevel
	switch(t1Test_pump_state){
	case PUMP_IDLE: //idle
		t1Test_pump_state = PUMP_RAMP;
		onNewCommadT1TEST(C2PCOM_ENABPUMPS_ON);
		EN_Motor_Control(ENABLE);
		break;

	case PUMP_RAMP: //ramp
		/*per sicurezza, se ho appena attivato l'enable, attendo 3 secondi*/
		if (timerCounterT1Test < 60)
			break;

		if(t1Test_pump_cmd == PUMP_IDLE){
			setPumpSpeedValueHighLevel(PPAR, T1TEST_PUMP_SPEED);
			setPumpSpeedValueHighLevel(PPAF, T1TEST_PUMP_SPEED);
			setPumpSpeedValueHighLevel(PPV1, T1TEST_PUMP_SPEED);
			setPumpSpeedValueHighLevel(PPV2, T1TEST_PUMP_SPEED);
			t1Test_pump_cmd = PUMP_RAMP;
		}
		//aspetto 10 secondi prima di cambiar stato
		if(timerCounterT1Test > 200){
			t1Test_pump_state = PUMP_RUNNING;
			timerCounterT1Test = 0;
		}

		break;

	case PUMP_RUNNING: //running
		if(
			(*SpeedPump1Rpmx10 >= (T1TEST_PUMP_SPEED - 500) ) &&
			(*SpeedPump1Rpmx10 <= (T1TEST_PUMP_SPEED + 500) ) &&
			(*SpeedPump2Rpmx10 >= (T1TEST_PUMP_SPEED - 500) ) &&
			(*SpeedPump2Rpmx10 <= (T1TEST_PUMP_SPEED + 500) ) &&
			(*SpeedPump3Rpmx10 >= (T1TEST_PUMP_SPEED - 500) ) &&
			(*SpeedPump3Rpmx10 <= (T1TEST_PUMP_SPEED + 500) ) &&
			(*SpeedPump4Rpmx10 >= (T1TEST_PUMP_SPEED - 500) ) &&
			(*SpeedPump4Rpmx10 <= (T1TEST_PUMP_SPEED + 500) ) &&
			( modbusData[0][17]>= (T1TEST_PUMP_SPEED - 100) ) &&
			( modbusData[0][17]<= (T1TEST_PUMP_SPEED + 100) ) &&
			( modbusData[1][17]>= (T1TEST_PUMP_SPEED - 100) ) &&
			( modbusData[1][17]<= (T1TEST_PUMP_SPEED + 100) ) &&
			( modbusData[2][17]>= (T1TEST_PUMP_SPEED - 100) ) &&
			( modbusData[2][17]<= (T1TEST_PUMP_SPEED + 100) ) &&
			( modbusData[3][17]>= (T1TEST_PUMP_SPEED - 100) ) &&
			( modbusData[3][17]<= (T1TEST_PUMP_SPEED + 100) ) /*&&
			(timerCounterT1Test > 200)*/
			)
		{
			t1Test_pump_state = PUMP_DIS_CON;
			t1Test_pump_cmd = 0;
			timerCounterT1Test = 0;
		}
		//se passati 10 secondi la PRO e la CON non vedono una velocit� corretta vado in allarme
		else if(timerCounterT1Test > 200){
			setPumpSpeedValueHighLevel(PPAR, 0);
			setPumpSpeedValueHighLevel(PPAF, 0);
			setPumpSpeedValueHighLevel(PPV1, 0);
			setPumpSpeedValueHighLevel(PPV2, 0);
			timerCounterT1Test = 0;
			currentGuard[GUARD_ENABLE_T1_ALARM].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			DebugStringStr("alarm from chk pump");
			t1Test_pump_state = PUMP_KO;//vado nello stato PUMP_KO che identifica un fail per sincornizzarmi con la macchina a stati nel parent.c
			t1Test_pump_cmd = 0;
		}
		break;

	case PUMP_DIS_CON:
		/*in questo stato la control toglie l'enable*/
		EN_Motor_Control(DISABLE);
		t1Test_pump_state = CHK_PUMP_DIS_CON;
		timerCounterT1Test = 0;
		break;

	case CHK_PUMP_DIS_CON:
		/*verifico che le pompe si siano fermate a causa dell'enable tolto
		 * e che le vedano ferme sia la Control che la Protective*/
		if(
			(*SpeedPump1Rpmx10  == 0 ) &&
			(*SpeedPump2Rpmx10  == 0 ) &&
			(*SpeedPump3Rpmx10  == 0 ) &&
			(*SpeedPump4Rpmx10  == 0 ) &&
			( modbusData[0][17] == 0 ) &&
			( modbusData[1][17] == 0 ) &&
			( modbusData[2][17] == 0 ) &&
			( modbusData[3][17] == 0 ) /*&&
			(timerCounterT1Test > 200)*/
			)
		{
			t1Test_pump_state = PUMP_EN_CON;
			t1Test_pump_cmd = 0;
			timerCounterT1Test = 0;
		}
		//se passati 10 secondi la PRO e la CON non vedono una velocit� corretta vado in allarme
		else if(timerCounterT1Test > 200){
			setPumpSpeedValueHighLevel(PPAR, 0);
			setPumpSpeedValueHighLevel(PPAF, 0);
			setPumpSpeedValueHighLevel(PPV1, 0);
			setPumpSpeedValueHighLevel(PPV2, 0);
			timerCounterT1Test = 0;
			currentGuard[GUARD_ENABLE_T1_ALARM].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			DebugStringStr("Disable Pump CONTROL not working");
			t1Test_pump_state = PUMP_KO;//vado nello stato PUMP_KO che identifica un fail per sincornizzarmi con la macchina a stati nel parent.c
			t1Test_pump_cmd = 0;
		}
		break;

	case PUMP_EN_CON:
		/*rid� l'enable alle pompe aspetto 3 secondi e poi le faccio partire a RPM*/
		EN_Motor_Control(ENABLE);
		//aspetto 3 secondi poi fermo tutte le pompe
		if (timerCounterT1Test < 60)
			break;

		setPumpSpeedValueHighLevel(PPAR, T1TEST_PUMP_SPEED);
		setPumpSpeedValueHighLevel(PPAF, T1TEST_PUMP_SPEED);
		setPumpSpeedValueHighLevel(PPV1, T1TEST_PUMP_SPEED);
		setPumpSpeedValueHighLevel(PPV2, T1TEST_PUMP_SPEED);

		t1Test_pump_state = CHK_PUMP_EN_CON;
		timerCounterT1Test = 0;
		break;

	case CHK_PUMP_EN_CON:
		/*verifico che sia la Con che la PRO tornino a vedere le pompe alla giusta velocit�
		 * la PRO con una tolleranza di 5 RPM, la CON con uan tolelranza di 1 RPM*/
		if(
			(*SpeedPump1Rpmx10 >= (T1TEST_PUMP_SPEED - 500) ) &&
			(*SpeedPump1Rpmx10 <= (T1TEST_PUMP_SPEED + 500) ) &&
			(*SpeedPump2Rpmx10 >= (T1TEST_PUMP_SPEED - 500) ) &&
			(*SpeedPump2Rpmx10 <= (T1TEST_PUMP_SPEED + 500) ) &&
			(*SpeedPump3Rpmx10 >= (T1TEST_PUMP_SPEED - 500) ) &&
			(*SpeedPump3Rpmx10 <= (T1TEST_PUMP_SPEED + 500) ) &&
			(*SpeedPump4Rpmx10 >= (T1TEST_PUMP_SPEED - 500) ) &&
			(*SpeedPump4Rpmx10 <= (T1TEST_PUMP_SPEED + 500) ) &&
			( modbusData[0][17]>= (T1TEST_PUMP_SPEED - 100) ) &&
			( modbusData[0][17]<= (T1TEST_PUMP_SPEED + 100) ) &&
			( modbusData[1][17]>= (T1TEST_PUMP_SPEED - 100) ) &&
			( modbusData[1][17]<= (T1TEST_PUMP_SPEED + 100) ) &&
			( modbusData[2][17]>= (T1TEST_PUMP_SPEED - 100) ) &&
			( modbusData[2][17]<= (T1TEST_PUMP_SPEED + 100) ) &&
			( modbusData[3][17]>= (T1TEST_PUMP_SPEED - 100) ) &&
			( modbusData[3][17]<= (T1TEST_PUMP_SPEED + 100) ) /*&&
			(timerCounterT1Test > 200)*/
			)
		{
			t1Test_pump_state = PUMP_DIS_PRO;
			t1Test_pump_cmd = 0;
			timerCounterT1Test = 0;
		}
		//se passati 10 secondi la PRO e la CON non vedono una velocit� corretta vado in allarme
		else if(timerCounterT1Test > 200){
			setPumpSpeedValueHighLevel(PPAR, 0);
			setPumpSpeedValueHighLevel(PPAF, 0);
			setPumpSpeedValueHighLevel(PPV1, 0);
			setPumpSpeedValueHighLevel(PPV2, 0);
			timerCounterT1Test = 0;
			currentGuard[GUARD_ENABLE_T1_ALARM].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			DebugStringStr("Enable Pump CONTROL not working");
			t1Test_pump_state = PUMP_KO;//vado nello stato PUMP_KO che identifica un fail per sincornizzarmi con la macchina a stati nel parent.c
			t1Test_pump_cmd = 0;
		}
		break;

	case PUMP_DIS_PRO:
		/*in questo stato la Control chiede alla pro di togliere l'enable*/
		onNewCommadT1TEST(C2PCOM_ENABPUMPS_OFF);
		t1Test_pump_state = CHK_PUMP_DIS_PRO;
		timerCounterT1Test = 0;
		break;

	case CHK_PUMP_DIS_PRO:
		/*verifico che le pompe si siano fermate a causa dell'enable tolto
		 * e che le vedano ferme sia la Control che la Protective*/
		if(
			(*SpeedPump1Rpmx10  == 0 ) &&
			(*SpeedPump2Rpmx10  == 0 ) &&
			(*SpeedPump3Rpmx10  == 0 ) &&
			(*SpeedPump4Rpmx10  == 0 ) &&
			( modbusData[0][17] == 0 ) &&
			( modbusData[1][17] == 0 ) &&
			( modbusData[2][17] == 0 ) &&
			( modbusData[3][17] == 0 ) /*&&
			(timerCounterT1Test > 200)*/
			)
		{
			t1Test_pump_state = PUMP_EN_PRO;
			t1Test_pump_cmd = 0;
			timerCounterT1Test = 0;
		}
		//se passati 10 secondi la PRO e la CON non vedono una velocit� corretta vado in allarme
		else if(timerCounterT1Test > 200){
			setPumpSpeedValueHighLevel(PPAR, 0);
			setPumpSpeedValueHighLevel(PPAF, 0);
			setPumpSpeedValueHighLevel(PPV1, 0);
			setPumpSpeedValueHighLevel(PPV2, 0);
			timerCounterT1Test = 0;
			currentGuard[GUARD_ENABLE_T1_ALARM].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			DebugStringStr("Disable Pump PROTECTIVE not working");
			t1Test_pump_state = PUMP_KO;//vado nello stato PUMP_KO che identifica un fail per sincornizzarmi con la macchina a stati nel parent.c
			t1Test_pump_cmd = 0;
		}
		break;

	case PUMP_EN_PRO:
		/*in questo stato la control chiede alla pro di rimettere l'enable e comanda
		 * il movimento delle pompe*/
		onNewCommadT1TEST(C2PCOM_ENABPUMPS_ON);
		//aspetto 3 secondi poi fermo tutte le pompe
		if (timerCounterT1Test < 60)
			break;

		setPumpSpeedValueHighLevel(PPAR, T1TEST_PUMP_SPEED);
		setPumpSpeedValueHighLevel(PPAF, T1TEST_PUMP_SPEED);
		setPumpSpeedValueHighLevel(PPV1, T1TEST_PUMP_SPEED);
		setPumpSpeedValueHighLevel(PPV2, T1TEST_PUMP_SPEED);

		t1Test_pump_state = CHK_PUMP_EN_PRO;
		timerCounterT1Test = 0;
		break;

	case CHK_PUMP_EN_PRO:
		if(
			(*SpeedPump1Rpmx10 >= (T1TEST_PUMP_SPEED - 500) ) &&
			(*SpeedPump1Rpmx10 <= (T1TEST_PUMP_SPEED + 500) ) &&
			(*SpeedPump2Rpmx10 >= (T1TEST_PUMP_SPEED - 500) ) &&
			(*SpeedPump2Rpmx10 <= (T1TEST_PUMP_SPEED + 500) ) &&
			(*SpeedPump3Rpmx10 >= (T1TEST_PUMP_SPEED - 500) ) &&
			(*SpeedPump3Rpmx10 <= (T1TEST_PUMP_SPEED + 500) ) &&
			(*SpeedPump4Rpmx10 >= (T1TEST_PUMP_SPEED - 100) ) &&
			(*SpeedPump4Rpmx10 <= (T1TEST_PUMP_SPEED + 100) ) &&
			( modbusData[0][17]>= (T1TEST_PUMP_SPEED - 100) ) &&
			( modbusData[0][17]<= (T1TEST_PUMP_SPEED + 100) ) &&
			( modbusData[1][17]>= (T1TEST_PUMP_SPEED - 100) ) &&
			( modbusData[1][17]<= (T1TEST_PUMP_SPEED + 100) ) &&
			( modbusData[2][17]>= (T1TEST_PUMP_SPEED - 100) ) &&
			( modbusData[2][17]<= (T1TEST_PUMP_SPEED + 100) ) &&
			( modbusData[3][17]>= (T1TEST_PUMP_SPEED - 100) ) &&
			( modbusData[3][17]<= (T1TEST_PUMP_SPEED + 100) ) /*&&
			(timerCounterT1Test > 200)*/
			)
		{
			t1Test_pump_state = PUMP_STOP;
			t1Test_pump_cmd = 0;
			timerCounterT1Test = 0;
		}
		//se passati 10 secondi la PRO e la CON non vedono una velocit� corretta vado in allarme
		else if(timerCounterT1Test > 200){
			setPumpSpeedValueHighLevel(PPAR, 0);
			setPumpSpeedValueHighLevel(PPAF, 0);
			setPumpSpeedValueHighLevel(PPV1, 0);
			setPumpSpeedValueHighLevel(PPV2, 0);
			timerCounterT1Test = 0;
			currentGuard[GUARD_ENABLE_T1_ALARM].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			DebugStringStr("Enable Pump PROTECTIVE not working");
			t1Test_pump_state = PUMP_KO;//vado nello stato 5 che identifica un fail per sincornizzarmi con la macchina a stati nel parent.c
			t1Test_pump_cmd = 0;
		}
		break;

	case PUMP_STOP: //stop
		if(t1Test_pump_cmd == 0){
			setPumpSpeedValueHighLevel(PPAR, 0);
			setPumpSpeedValueHighLevel(PPAF, 0);
			setPumpSpeedValueHighLevel(PPV1, 0);
			setPumpSpeedValueHighLevel(PPV2, 0);
			t1Test_pump_cmd = 1;
			t1Test_pump_state = PUMP_OK;
		}
		break;

	case PUMP_OK: //end
		t1Test_pump_state = PUMP_IDLE;//vado nello stato 5 che identifica un fail per sincornizzarmi con la macchina a stati nel parent.c
		t1Test_pump_cmd = 0;
		break;

	case PUMP_KO:  //end with fail
		setPumpSpeedValueHighLevel(PPAR, 0);
		setPumpSpeedValueHighLevel(PPAF, 0);
		setPumpSpeedValueHighLevel(PPV1, 0);
		setPumpSpeedValueHighLevel(PPV2, 0);
		timerCounterT1Test = 0;
		currentGuard[GUARD_ENABLE_T1_ALARM].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		DebugStringStr("alarm from chk pump");
		break;

	default:
		setPumpSpeedValueHighLevel(PPAR, 0);
		setPumpSpeedValueHighLevel(PPAF, 0);
		setPumpSpeedValueHighLevel(PPV1, 0);
		setPumpSpeedValueHighLevel(PPV2, 0);
		timerCounterT1Test = 0;
		currentGuard[GUARD_ENABLE_T1_ALARM].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		DebugStringStr("alarm from chk pump --> WRONG Machine state");
		t1Test_pump_state = PUMP_KO;
		t1Test_pump_cmd = 0;
		break;
	}
}


/*----------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------INIZIO PARENT PRIMING LEVEL FUNCTION -----------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------------*/

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

		setPinchPositionHighLevel(PINCH_2WPVA, PRIM_PINCH_2WPVA_POS);
		if(GetTherapyType() == LiverTreat)
		{
			// ho selezionato il fegato, quindi devo riempire anche il circuito venoso
			setPinchPositionHighLevel(PINCH_2WPVV, PRIM_PINCH_2WPVV_POS);
		}

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

	// inizializzo il controllo delle pinch fatto durante l'esecuzione del trattamento
	//CheckCurrPinchPosTask(CHECK_CURR_PINCH_POS_INIT_CMD);

	// se non sono in allarme (dovrebbe essere sempre cosi') abilito la generazione del prossimo per sicurezza
	if(!IsAlarmActive())
		EnableNextAlarm = TRUE;

}


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

void RestorePrimSpeedVal(void)
{
	// ripristino le velocita' di priming normali
	setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, RPM_IN_PRIMING_PHASES);
	if((GetTherapyType() == KidneyTreat) && (((PARAMETER_ACTIVE_TYPE)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_ACTIVE].value) == YES))
	{
		// sono nel ricircolo rene con ossigenatore abilitato
			setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress,KIDNEY_PRIMING_PMP_OXYG_SPEED);
	}
	else if(GetTherapyType() == LiverTreat)
	{
		// sono nel priming fegato ed ho superato una quantita' minima nel reservoir, quindi, la pompa venosa
		// per il riempimento del disposable di ossigenazione e la pompa di depurazione PPAR
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, (int)LIVER_PRIMING_PMP_OXYG_SPEED);
		setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, LIVER_PPAR_SPEED);
	}
}


// cmd comando per eventuare riposizionamento della macchina a stati
unsigned char TemperatureStateMach(int cmd)
{
	static unsigned long RicircTimeout;
	static unsigned char TempStateMach = START_RECIRC_HIGH_SPEED;
	static int Delay = 0;
	static int TimeRemaining = HIGH_PUMP_SPEED_DURATION;
	unsigned char TempReached = 0;

	float tmpr;
	word tmpr_trgt;
//	// temperatura raggiunta dal reservoir
//	tmpr = ((int)(sensorIR_TM[1].tempSensValue*10));
	/*31/10/2019 VP: � pi� corretto andare a controllare la linea arteriosa
	 * ossia quella + vicina all'ingresso dell'organo usata in entrambi i trattamenti*/
	// temperatura raggiunta sulla linea arteriosa
	tmpr = ((int)(sensorIR_TM[0].tempSensValue*10));
	// temperatura da raggiungere moltiplicata per 10
	tmpr_trgt = parameterWordSetFromGUI[PAR_SET_PRIMING_TEMPERATURE_PERFUSION].value;

	if(cmd == RESTART_CMD)
	{
		if(TempStateMach == START_RECIRC_IDLE)
		{
			Delay = 0;
		}
		else if((TempStateMach == STOP_RECIRC_HIGH_SPEED) || (TempStateMach == CALC_PUMPS_GAIN))
		{
			// faccio ripartire i motori ad alta velocita'
			TempStateMach = START_RECIRC_HIGH_SPEED;
			TimeRemaining = (int)HIGH_PUMP_SPEED_DURATION - (TimeAtStopRiciclo - RicircTimeout) * 50;
			RicircTimeout = timerCounterModBus;
			if(TimeRemaining > HIGH_PUMP_SPEED_DURATION)
				TimeRemaining = HIGH_PUMP_SPEED_DURATION;
			else if(TimeRemaining < 0)
				TimeRemaining = HIGH_PUMP_SPEED_DURATION;
		}
		else if((TempStateMach == TEMP_CHECK_DURATION_STATE) || (TempStateMach == TEMP_START_CHECK_STATE))
		{
			// dopo uno stop o un allarme devo far partire le pompe prima di continuare con il controllo di temperatura
			RestorePrimSpeedVal();
			TempStateMach = TEMP_START_CHECK_STATE;
		}
		else if(TempStateMach == SWITCH_PINCH_IN_INLET_LINE)
		{
			RestorePrimSpeedVal();
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
				// spostato da START_RECIRC_HIGH_SPEED  a qui per evitare di ricominciare daccapo
				// il minuto di riciclo
				RicircTimeout = timerCounterModBus;

				if(!StartPrimingTime)
				{
					// faccio ripartire il timer del priming
					StartPrimingTime = (unsigned long)timerCounterModBus;
				}
				TimeRemaining = HIGH_PUMP_SPEED_DURATION;
			}
			break;

	case START_RECIRC_HIGH_SPEED:
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress,(int)RECIRC_PUMP_HIGH_SPEED_ART);
		if((GetTherapyType() == KidneyTreat) && (((PARAMETER_ACTIVE_TYPE)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_ACTIVE].value) == YES))
		{
			// sono nel ricircolo rene con ossigenatore abilitato
			setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, (int)RECIRC_PUMP_HIGH_SPEED);
		}
		else if((GetTherapyType() == LiverTreat))
		{
			// sono nel priming fegato
			setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, (int)RECIRC_PUMP_HIGH_SPEED);
			setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, (int)RECIRC_PUMP_HIGH_SPEED_ART);
		}
		TempStateMach = CALC_PUMPS_GAIN;
		ArteriousPumpGainForPid = DEFAULT_ART_PUMP_GAIN;
		VenousPumpGainForPid = DEFAULT_VEN_PUMP_GAIN;
		break;
	case CALC_PUMPS_GAIN:
		if((msTick_elapsed(RicircTimeout) * 50L) >= (TimeRemaining - 3000))
		{
			// 3 secondi prima della fine del ricircolo calcolo il guadagno delle pompe e le memorizzo
			ArteriousPumpGainForPid = CalcArtPumpGain(sensor_UFLOW[0].Average_Flow_Val);
			VenousPumpGainForPid = CalcVenPumpGain(sensor_UFLOW[1].Average_Flow_Val);
			TempStateMach = STOP_RECIRC_HIGH_SPEED;
		}
		break;
	case STOP_RECIRC_HIGH_SPEED:
		if((msTick_elapsed(RicircTimeout) * 50L) >= TimeRemaining)
		{
			// ho raggiunto il tempo di ricircolo ad alta velocita' per eliminare l'aria eventuale
			// proseguo con il controllo della temperatura
			TempStateMach = SWITCH_PINCH_IN_INLET_LINE;

			// ripristino le velocita' di priming normali
			setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, RPM_IN_PRIMING_PHASES);
			if((GetTherapyType() == KidneyTreat) && (((PARAMETER_ACTIVE_TYPE)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_ACTIVE].value) == YES))
			{
				// sono nel ricircolo rene con ossigenatore abilitato
					setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress,KIDNEY_PRIMING_PMP_OXYG_SPEED);
                /*(int)((float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / OXYG_FLOW_TO_RPM_CONV * 100.0));*/
			}
			else if(GetTherapyType() == LiverTreat)
			{
				// sono nel priming fegato ed ho superato una quantita' minima nel reservoir, quindi, la pompa venosa
				// per il riempimento del disposable di ossigenazione e la pompa di depurazione PPAR
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, (int)LIVER_PRIMING_PMP_OXYG_SPEED);
				setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, LIVER_PPAR_SPEED);
			}

			/*qui metto le pinch venosa e arteria a destra per riempire i
			 * tubi di ingresso all'organo e resetto il timer*/
			RicircTimeout = timerCounterModBus;

			/*Vincenzo: quando finisco il ricircolo veloce, sposto le pinch sull'organo
			 * per rimepire e linee organo;le terr� l� per un minuto, scaduto il quale
			 * nel case successivo, le rimetto sullo scarico*/
			setPinchPositionHighLevel(PINCH_2WPVA, MODBUS_PINCH_LEFT_OPEN);
			if(GetTherapyType() == LiverTreat)
			{
				// ho selezionato il fegato, quindi devo chiudere anche questa
				setPinchPositionHighLevel(PINCH_2WPVV, MODBUS_PINCH_LEFT_OPEN);
			}
		}
		break;

	case SWITCH_PINCH_IN_INLET_LINE:
		if ((msTick_elapsed(RicircTimeout) * 50L) >= TIMER_PRIMING_INLET_LINE)
		{
			/*scaduto il timer di riempimento linee ingresso organo
			 * switcho le pinch sullo scarico e
			 * vado in controllo raggiungimento temperatura*/
			setPinchPositionHighLevel(PINCH_2WPVA, MODBUS_PINCH_RIGHT_OPEN);
			if(GetTherapyType() == LiverTreat)
			{
				// ho selezionato il fegato, quindi devo chiudere anche questa
				setPinchPositionHighLevel(PINCH_2WPVV, MODBUS_PINCH_RIGHT_OPEN);
			}

			TempStateMach = TEMP_START_CHECK_STATE;
		}
		break;

	case TEMP_START_CHECK_STATE:

		if((tmpr >= (float)(tmpr_trgt - DELTA_TEMP_TERGET_FOR_STABILITY_PRIMING)) && (tmpr <= (float)(tmpr_trgt + DELTA_TEMP_TERGET_FOR_STABILITY_PRIMING)))
		{
			// ho raggiunto la temperatura target
			TempStateMach = TEMP_CHECK_DURATION_STATE;
			RicircTimeout = timerCounterModBus;
		}
		break;
	case TEMP_CHECK_DURATION_STATE:
		/*VP 07/11/2019: vincolo la fine del priming/ricircolo e quindi il raggiungimento della temperatura al fatto che non siano presenti allarmi o warning Control*/
		if((tmpr >= (float)(tmpr_trgt - DELTA_TEMP_TERGET_FOR_STABILITY_PRIMING)) && (tmpr <= (float)(tmpr_trgt + DELTA_TEMP_TERGET_FOR_STABILITY_PRIMING)) &&
				LengthActiveListAlm() == 0 && LengthActiveListWrn() == 0)
		{
			// ho raggiunto la temperatura target
			if(msTick_elapsed(RicircTimeout) * 50L >= TIMEOUT_TEMPERATURE_RICIRC)
			{
				// per almeno TIMEOUT_TEMPERATURE_RICIRC/1000 secondi la temperatura si e' mantenuta nell'intorno del target,
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

		// si chiude la fase di priming per entrare in quella di trattamento, disabilito il controllo
		// delle pinch fatte in priming
		CheckCurrPinchPosTask(CHECK_CURR_PINCH_POS_DISABLE_CMD);
		break;
	}
	return TempReached;
}

void SetPinchPosInPriming(void)
{
	if(ptrCurrentState->state == STATE_PRIMING_RICIRCOLO)
	{
		if(!FilterSelected)
			setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_LEFT_OPEN);
		else
			setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_RIGHT_OPEN);
		// Durante il ricircolo e' stato richiesto di spostare le pinch sull'organo per fare in modo che anche la parte finale delle linee si
		// riempa di liquido
		/*15-11-2018 Vincenzo: richiesta successiva, si parte con le pinch sullo scarico
		 * poi, finita la parte di ricircolo veloce, si spostano per 30 secondi sull'organo
		 * poi nuovamente sullo scarico*/
		/*05-12-2018 Vincenzo: richiesta successiva, si parte con la pinch arteriosa sullo scarico e la venosa sull'organo
		 * poi, finita la parte di ricircolo veloce, si spostano per 60 secondi sull'organo
		 * poi nuovamente sullo scarico*/
		setPinchPositionHighLevel(PINCH_2WPVA, MODBUS_PINCH_RIGHT_OPEN);
		if(GetTherapyType() == LiverTreat)
		{
			// ho selezionato il fegato, quindi devo chiudere anche questa
			setPinchPositionHighLevel(PINCH_2WPVV, MODBUS_PINCH_LEFT_OPEN);
		}
	}
	else
	{
		if(!FilterSelected)
			setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_LEFT_OPEN);
		else
			setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_RIGHT_OPEN);
		setPinchPositionHighLevel(PINCH_2WPVA, PRIM_PINCH_2WPVA_POS);
		if(GetTherapyType() == LiverTreat)
		{
			// ho selezionato il fegato, quindi devo riempire anche il circuito venoso
			setPinchPositionHighLevel(PINCH_2WPVV, PRIM_PINCH_2WPVV_POS);
		}
	}
}

void manageParentPrimingAlways(void){

	static char iflag_perf = 0;
	static char iflag_oxyg = 0;
	int speed = 0;
	static int timerCopy = 0;

	// reset dell'eventuale flag di entrata nello stato di gestione allarme.
	AlarmInPrimingEntered = FALSE;

	//manage pump
	switch(ptrCurrentParent->parent)
	{
	case PARENT_PRIMING_TREAT_KIDNEY_1_INIT:
		if(buttonGUITreatment[BUTTON_START_PRIMING].state == GUI_BUTTON_RELEASED)
		{
			// Filippo - aggiunto da file FM nel merge 1-10-2018
			//al rientro dell'allarme, se in allarme era stato spento il frigo, lo riaccendo
		//	if (IsFrigoStoppedInAlarm())
			FrigoHeatTempControlTaskNewPID(LIQ_T_CONTR_TASK_RESET_CMD);

			setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, RPM_IN_PRIMING_PHASES_INIT);
			if((GetTherapyType() == KidneyTreat) && (((PARAMETER_ACTIVE_TYPE)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_ACTIVE].value) == YES) &&
				(perfusionParam.priVolPerfArt > MIN_LIQ_IN_RES_TO_START_OXY_VEN))
			{
				// sono nel priming rene con ossigenatore abilitato ed ho superato una quantita' minima nel reservoir
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress,KIDNEY_PRIMING_PMP_OXYG_SPEED_INIT);
						                  /*(int)((float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / OXYG_FLOW_TO_RPM_CONV * 100.0));*/
			}
			else if((GetTherapyType() == LiverTreat) && (perfusionParam.priVolPerfArt > MIN_LIQ_IN_RES_TO_START_OXY_VEN))
			{
				// sono nel priming fegato ed ho superato una quantita' minima nel reservoir, quindi, la pompa venosa
				// per il riempimento del disposable di ossigenazione e la pompa di depurazione PPAR
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, (int)LIVER_PRIMING_PMP_OXYG_SPEED_INIT);
				setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, LIVER_PPAR_SPEED_INIT);
			}

			SetPinchPosInPriming();
		// QUESTO CODICE E' STATO SPOSTATO NELLA FUNZIONE SetPinchPosInPriming
//		if(!FilterSelected)
//			setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_LEFT_OPEN);
//		else
//			setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_RIGHT_OPEN);
//		setPinchPositionHighLevel(PINCH_2WPVA, PRIM_PINCH_2WPVA_POS);
//		if(GetTherapyType() == LiverTreat)
//		{
//			// ho selezionato il fegato, quindi devo riempire anche il circuito venoso
//			setPinchPositionHighLevel(PINCH_2WPVV, PRIM_PINCH_2WPVV_POS);
//		}

			releaseGUIButton(BUTTON_START_PRIMING);
			if(!StartPrimingTime)
			{
				// prendo il tempo di start del priming solo se il valore vale 0, cioe' sono partito da IDLE
				StartPrimingTime = (unsigned long)timerCounterModBus;
			}
			else
			{
				// Filippo - messo per far ripartire il PID dopo un allarme
				//al rientro dell'allarme, se in allarme era stato spento il frigo, lo riaccendo
//				if (IsFrigoStoppedInAlarm())
					FrigoHeatTempControlTaskNewPID(LIQ_T_CONTR_TASK_RESET_CMD);

			}

			// inizializzo il controllo delle pinch fatto durante l'esecuzione del trattamento
			CheckCurrPinchPosTask(CHECK_CURR_PINCH_POS_INIT_CMD);
			// abilito allarme di pinch posizionate male
			EnableBadPinchPosAlmFunc();
		}
		else if(buttonGUITreatment[BUTTON_START_PERF_PUMP].state == GUI_BUTTON_RELEASED)
		{
			releaseGUIButton(BUTTON_START_PERF_PUMP);

			setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, RPM_IN_PRIMING_PHASES_INIT);

			SetPinchPosInPriming();
		// QUESTO CODICE E' STATO SPOSTATO NELLA FUNZIONE SetPinchPosInPriming
//		if(!FilterSelected)
//			setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_LEFT_OPEN);
//		else
//			setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_RIGHT_OPEN);
//		setPinchPositionHighLevel(PINCH_2WPVA, PRIM_PINCH_2WPVA_POS);
//		if(GetTherapyType() == LiverTreat)
//		{
//			// ho selezionato il fegato, quindi devo riempire anche il circuito venoso
//			setPinchPositionHighLevel(PINCH_2WPVV, PRIM_PINCH_2WPVV_POS);
//		}
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
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress,KIDNEY_PRIMING_PMP_OXYG_SPEED_INIT);
						                  /*(int)((float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / OXYG_FLOW_TO_RPM_CONV * 100.0));*/
			}
			else if((GetTherapyType() == LiverTreat) && (perfusionParam.priVolPerfArt > MIN_LIQ_IN_RES_TO_START_OXY_VEN))
			{
				// sono nel priming fegato ed ho superato una quantita' minima nel reservoir, quindi, la pompa venosa
				// per il riempimento del disposable di ossigenazione e la pompa di depurazione PPAR
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, (int)LIVER_PRIMING_PMP_OXYG_SPEED_INIT);
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
			//FilterFlowVal = 0;
			// disabilito allarme di pinch posizionate male
			DisableBadPinchPosAlmFunc();
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
				if(GetTherapyType() == KidneyTreat)
				{
					// aggiorno il volume che passa attraverso il filtro
					perfusionParam.priVolAdsFilter += (word)((float)speed * CONV_RPMMIN_TO_ML_PER_INTERVAL);
					//FilterFlowVal = CalcFilterFlow(pumpPerist[0].actualSpeed);
				}
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
			if(GetTherapyType() == LiverTreat)
			{
				// nella fase di ricircolo non aggiorno piu' il volume
				perfusionParam.priVolAdsFilter += (word)((float)pumpPerist[3].actualSpeed * CONV_RPMMIN_TO_ML_PER_INTERVAL);
				//FilterFlowVal = CalcFilterFlow(pumpPerist[3].actualSpeed);
			}
		}

		// Controllo continuo quando il trattamento e' in corso. Viene confrontata la posizione impostata delle
		// pinch durante il trattamento con quella rilevata dalla protective.
		// Se non coincidono viene generato un allarme
		CheckCurrPinchPosTask(CHECK_CURR_PINCH_POS_NO_CMD);
		break;

	case PARENT_PRIMING_TREAT_KIDNEY_1_RUN:
	{

	/*25-10-2018 Vincenzo --> partenza pompe a RAMPA all0inizio del priming*/
//		static int speed_filter		 	         = 0;
//		static int speed_Arterious 			     = 0;
//		static int speed_Venous				     = 0;
//		static int timerCounterRampaAccPmpFlt    = 0;
//		static int timerCounterRampaAccPmpVenArt = 0;

		//TODO sarebbe sempre meglio controllare la velocit� che si vuol dare alla pompa
		//con quella letta dalla pompa stessa e dare ilo comando se � diversa.

		/*se sono in STATE_PRIMING_PH_1_WAIT le pompe devono star ferme quindi non faccio l'incremento
		 * se sono in ricircolo le pompe devono andare in alto flusso quindi non vado pi� in rampa*/
		if (ptrCurrentState->state != STATE_PRIMING_PH_1_WAIT &&
			ptrCurrentState->state != STATE_PRIMING_RICIRCOLO   )
		{
			/*blocco l'incremento alla velocit� target*/
			if (speed_Arterious >= RPM_IN_PRIMING_PHASES)
				speed_Arterious = RPM_IN_PRIMING_PHASES;
			/*finche la velocit� � inferiore al target ogni 2 sec (40 intervalli da 50 msec) incremento di 0.5 RPM la velocit�*/
			else if (msTick_elapsed(timerCounterRampaAccPmpFlt) >= TIMER_RPM_ACC_IN_PRIMING_ART && speed_Arterious <= RPM_IN_PRIMING_PHASES)
			{
				speed_Arterious += DELTA_RPM_ACC_IN_PRIMING_ART;
				timerCounterRampaAccPmpFlt = timerCounterModBus;
				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, speed_Arterious);
			}

			if((GetTherapyType() == KidneyTreat) && (((PARAMETER_ACTIVE_TYPE)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_ACTIVE].value) == YES) &&
				(perfusionParam.priVolPerfArt > MIN_LIQ_IN_RES_TO_START_OXY_VEN))
			{
				/*blocco l'incremento alla velocit� target*/
				if (speed_Venous >= KIDNEY_PRIMING_PMP_OXYG_SPEED)
					speed_Venous = KIDNEY_PRIMING_PMP_OXYG_SPEED;

				/*finche la velocit� � inferiore al target ogni 2 sec (40 intervalli da 50 msec) incremento di 0.5 RPM la velocit�*/
				if (msTick_elapsed(timerCounterRampaAccPmpVenArt) >= TIMER_RPM_ACC_IN_PRIMING_VEN )
				{
					speed_Venous += DELTA_RPM_ACC_IN_PRIMING_VEN;
					timerCounterRampaAccPmpVenArt = timerCounterModBus;

					if (speed_Venous <= KIDNEY_PRIMING_PMP_OXYG_SPEED )
						setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, speed_Venous);
				}
			}
			else if((GetTherapyType() == LiverTreat) && (perfusionParam.priVolPerfArt > MIN_LIQ_IN_RES_TO_START_OXY_VEN))
			{
				/*blocco l'incremento alla velocit� target*/
				if (speed_Venous >= LIVER_PRIMING_PMP_OXYG_SPEED)
					speed_Venous = LIVER_PRIMING_PMP_OXYG_SPEED;

				if (speed_filter >= LIVER_PPAR_SPEED)
					speed_filter  = LIVER_PPAR_SPEED;

				if (msTick_elapsed(timerCounterRampaAccPmpVenArt) >= TIMER_RPM_ACC_IN_PRIMING_VEN )
				{
					speed_filter += DELTA_RPM_ACC_IN_PRIMING_FLT;
					speed_Venous += DELTA_RPM_ACC_IN_PRIMING_VEN;
					timerCounterRampaAccPmpVenArt = timerCounterModBus;
					if (speed_Venous <= LIVER_PRIMING_PMP_OXYG_SPEED)
						setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, speed_Venous);
					if (speed_filter <= LIVER_PPAR_SPEED)
						setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, speed_filter);
				}

			}
		}
	/*25-10-2018 Vincenzo --> partenza pompe a RAMPA all'inizio del priming END*/

		if(buttonGUITreatment[BUTTON_START_PRIMING].state == GUI_BUTTON_RELEASED )
		{
			// Filippo - messa nel merge del 1-10-2018
			//al rientro dell'allarme, se in allarme era stato spento il frigo, lo riaccendo
			//if (IsFrigoStoppedInAlarm())
			FrigoHeatTempControlTaskNewPID(LIQ_T_CONTR_TASK_RESET_CMD);

			if(ptrCurrentState->state == STATE_PRIMING_RICIRCOLO)
			{
				SetPinchPosInPriming();
				// sono nella fase di ricircolo e si e' verificato un allarme oppure l'utente ha
				// fermato il processo con stop priming e poi e' ripartito
				if(AlarmOrStopInRecircFlag)
					AlarmOrStopInRecircFlag = FALSE;
				TemperatureStateMach(RESTART_CMD);
			}
			else
			{
				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, speed_Arterious);

				//if(((PARAMETER_ACTIVE_TYPE)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_ACTIVE].value) == YES)
				//	setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, (int)((float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / OXYG_FLOW_TO_RPM_CONV * 100.0));
				if((GetTherapyType() == KidneyTreat) && (((PARAMETER_ACTIVE_TYPE)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_ACTIVE].value) == YES) &&
					(perfusionParam.priVolPerfArt > MIN_LIQ_IN_RES_TO_START_OXY_VEN))
				{
					// sono nel priming rene con ossigenatore abilitato ed ho superato una quantita' minima nel reservoir
						setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, speed_Venous);
								                  /*(int)((float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / OXYG_FLOW_TO_RPM_CONV * 100.0));*/
				}
				else if((GetTherapyType() == LiverTreat) && (perfusionParam.priVolPerfArt > MIN_LIQ_IN_RES_TO_START_OXY_VEN))
				{
					// sono nel priming fegato ed ho superato una quantita' minima nel reservoir, quindi, la pompa venosa
					// per il riempimento del disposable di ossigenazione e la pompa di depurazione PPAR
					setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, speed_Venous);
					setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, speed_filter);
				}
			}

			SetPinchPosInPriming();
			// QUESTO CODICE E' STATO SPOSTATO NELLA FUNZIONE SetPinchPosInPriming
//				if(!FilterSelected)
//					setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_LEFT_OPEN);
//				else
//					setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_RIGHT_OPEN);
//				setPinchPositionHighLevel(PINCH_2WPVA, PRIM_PINCH_2WPVA_POS);
//				if(GetTherapyType() == LiverTreat)
//				{
//					// ho selezionato il fegato, quindi devo riempire anche il circuito venoso
//					setPinchPositionHighLevel(PINCH_2WPVV, PRIM_PINCH_2WPVV_POS);
//				}

			releaseGUIButton(BUTTON_START_PRIMING);
			if(!StartPrimingTime)
			{
				// prendo il tempo di start del priming solo se il valore vale 0, cioe' sono partito da IDLE
				StartPrimingTime = (unsigned long)timerCounterModBus;
			}
			else
			{
				// Filippo - inserito nel merge
				//al rientro dell'allarme, se in allarme era stato spento il frigo, lo riaccendo
			//	if (IsFrigoStoppedInAlarm())
				FrigoHeatTempControlTaskNewPID(LIQ_T_CONTR_TASK_RESET_CMD);
			}

			// inizializzo il controllo delle pinch fatto durante l'esecuzione del trattamento
			CheckCurrPinchPosTask(CHECK_CURR_PINCH_POS_INIT_CMD);
			// abilito allarme di pinch posizionate male
			EnableBadPinchPosAlmFunc();
		}
		else if(buttonGUITreatment[BUTTON_START_PERF_PUMP].state == GUI_BUTTON_RELEASED)
		{
			releaseGUIButton(BUTTON_START_PERF_PUMP);

			setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, RPM_IN_PRIMING_PHASES);

			SetPinchPosInPriming();
			// QUESTO CODICE E' STATO SPOSTATO NELLA FUNZIONE SetPinchPosInPriming
//				if(!FilterSelected)
//					setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_LEFT_OPEN);
//				else
//					setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_RIGHT_OPEN);
//				setPinchPositionHighLevel(PINCH_2WPVA, PRIM_PINCH_2WPVA_POS);
//				if(GetTherapyType() == LiverTreat)
//				{
//					// ho selezionato il fegato, quindi devo riempire anche il circuito venoso
//					setPinchPositionHighLevel(PINCH_2WPVV, PRIM_PINCH_2WPVV_POS);
//				}
		}
		else if(buttonGUITreatment[BUTTON_START_OXYGEN_PUMP].state == GUI_BUTTON_RELEASED)
		{
			releaseGUIButton(BUTTON_START_OXYGEN_PUMP);
			//setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, (int)((float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / OXYG_FLOW_TO_RPM_CONV * 100.0));
			if((GetTherapyType() == KidneyTreat) && (((PARAMETER_ACTIVE_TYPE)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_ACTIVE].value) == YES) &&
				(perfusionParam.priVolPerfArt > MIN_LIQ_IN_RES_TO_START_OXY_VEN))
			{
				// sono nel priming rene con ossigenatore abilitato ed ho superato una quantita' minima nel reservoir
					setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress,KIDNEY_PRIMING_PMP_OXYG_SPEED);
							                  /*(int)((float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / OXYG_FLOW_TO_RPM_CONV * 100.0));*/
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

			currentGuard[GUARD_ENT_PAUSE_STATE_PRIM_KIDNEY_1].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			CheckPumpStopTask((CHECK_PUMP_STOP_CMD)INIT_CHECK_SEQ_CMD);
			DebugStringStr("Stop prim.");
			//FilterFlowVal = 0;
			// Disabilto il controllo delle pinch fino a quando non parte il riciclo
			CheckCurrPinchPosTask(CHECK_CURR_PINCH_POS_DISABLE_CMD);
			// disabilito allarme di pinch posizionate male
			DisableBadPinchPosAlmFunc();

			if(ptrCurrentState->state == STATE_PRIMING_RICIRCOLO)
				TimeAtStopRiciclo = timerCounterModBus;
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

			// dato che sono in un cambi
			ClearAlarmState();

			// Disabilto il controllo delle pinch fino a quando non parte il riciclo
			CheckCurrPinchPosTask(CHECK_CURR_PINCH_POS_DISABLE_CMD);
		}
		else if(ptrCurrentState->state == STATE_PRIMING_RICIRCOLO)
		{
			if(TemperatureStateMach(0))
			{
				// ho raggiunto la temperatura ( + o - un grado)richiesta posso passare nello stato di
				// attesa ricezione comando di start trattamento
				// fermo le pompe ed aspetto

				// debugVal le 7 linee di programma che seguono le devo commentare
				// se voglio provare l'allarme delle pompe che non si fermano alla fine
				// del ricircolo
				// commento solo per debug. DA TOGLIERE !!!!
				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
				if(GetTherapyType() == LiverTreat)
				{
					// se sono nel trattamento fegato fermo anche l'altro motore !!
					setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
				}

				//currentGuard[GUARD_ENABLE_WAIT_TREATMENT].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
				//StartTreatmentTime = (unsigned long)timerCounterModBus;

				//currentGuard[GUARD_ENABLE_TREATMENT_KIDNEY_1].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
				// SE NON VADO PIU' INTRATTAMENTO MA DEVO PRIMA CONTROLLARE CHE LE POMPE SIANO FERME E POI CHIUDERE
				// LE PINCH
				currentGuard[GUARD_CHK_FOR_ALL_MOT_STOP].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
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
				//currentGuard[GUARD_ABANDON_PRIMING].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
				SetAbandonGuard();
				// sposto la macchina a stati in TEMP_ABANDONE_CMD per evitare problemi
				TemperatureStateMach(TEMP_ABANDONE_CMD);
			}

		}

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
				if(GetTherapyType() == KidneyTreat)
				{
					// aggiorno il volume che passa attraverso il filtro
					perfusionParam.priVolAdsFilter += (word)((float)speed * CONV_RPMMIN_TO_ML_PER_INTERVAL);
					//FilterFlowVal = CalcFilterFlow(pumpPerist[0].actualSpeed);
				}
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
			if((GetTherapyType() == LiverTreat) /*&& FilterSelected &&
			   ((ptrCurrentState->state == STATE_PRIMING_PH_1) || (ptrCurrentState->state == STATE_PRIMING_PH_2))*/)
			{
				// nella fase di ricircolo non aggiorno piu' il volume
				perfusionParam.priVolAdsFilter += (word)((float)pumpPerist[3].actualSpeed * CONV_RPMMIN_TO_ML_PER_INTERVAL);
				//FilterFlowVal = CalcFilterFlow(pumpPerist[3].actualSpeed);
			}
		}

		// Controllo continuo quando il trattamento e' in corso. Viene confrontata la posizione impostata delle
		// pinch durante il trattamento con quella rilevata dalla protective.
		// Se non coincidono viene generato un allarme
		CheckCurrPinchPosTask(CHECK_CURR_PINCH_POS_NO_CMD);
		break;
	}
	case PARENT_PRIMING_TREAT_KIDNEY_1_ALARM:
		setPumpSpeedValue(pumpPerist[0].pmpMySlaveAddress, 0);
		break;

	case PARENT_PRIMING_TREAT_KIDNEY_1_END:
		setPumpSpeedValue(pumpPerist[0].pmpMySlaveAddress, 0);
		break;

	case PARENT_PRIM_WAIT_PAUSE:
		if(buttonGUITreatment[BUTTON_START_PRIMING].state == GUI_BUTTON_RELEASED)
		{
			// devo fare il release altrimenti in questo posto ci passa piu' volte
			releaseGUIButton(BUTTON_START_PRIMING);
			// ripristino allarmi delle cover
			GlobalFlags.FlagsDef.EnableCoversAlarm = 1;
			currentGuard[GUARD_ENABLE_STATE_KIDNEY_1_PRIM_RUN].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			DebugStringStr("Start (prim)");
		}
		else if(buttonGUITreatment[BUTTON_STOP_PRIMING].state == GUI_BUTTON_RELEASED)
		{
			// se mi arriva uno stop in questo stato lo butto via altrimenti mi rimane memorizzato
			// per quando ritornero' in run
			releaseGUIButton(BUTTON_STOP_PRIMING);
		}
		else if(buttonGUITreatment[BUTTON_PRIMING_ABANDON].state == GUI_BUTTON_RELEASED)
		{
			releaseGUIButton(BUTTON_PRIMING_ABANDON);
			//currentGuard[GUARD_ABANDON_PRIMING].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			SetAbandonGuard();
			// ripristino allarmi delle cover
			GlobalFlags.FlagsDef.EnableCoversAlarm = 1;
		}
		CheckPumpStopTask((CHECK_PUMP_STOP_CMD)NO_CHECK_PUMP_STOP_CMD);
		break;
	default:
		break;
	}

}

// Chiamata all'entry dello stato PARENT_PRIM_WAIT_PAUSE
void manageParentPrimWaitPauseEntry(void)
{
}


//-----------------------------------------------------------------------------------------------------
unsigned short MotStopTicksElapsed;
bool StopMotorTimeOut;
// controllo che tutte le pompe siano ferme altrimenti do un allarme (gestione dello stato PARENT_PRIM_WAIT_MOT_STOP)
void manageParPrimWaitMotStopEntry(void)
{
	CheckPumpStopTask((CHECK_PUMP_STOP_CMD)INIT_CHECK_SEQ_CMD);
	MotStopTicksElapsed = 0;
	StopMotorTimeOut = FALSE;
}

void manageParPrimWaitMotStopEntryAlways(void)
{
	MotStopTicksElapsed++;
	CheckPumpStopTask((CHECK_PUMP_STOP_CMD)NO_CHECK_PUMP_STOP_CMD);
	if(AreAllPumpsStopped() && currentGuard[GUARD_EN_CLOSE_ALL_PINCH].guardEntryValue == GUARD_ENTRY_VALUE_FALSE)
	{
		// passo alla chiusura totale delle due pinch connesse all'organo
		currentGuard[GUARD_EN_CLOSE_ALL_PINCH].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		// disabilito allarme di pompe non ferme
		// disabilito allarme di aria nel filtro durante il priming
		DisablePrimAlmSFAAirDetAlmFunc();

		DebugStringStr("PUMPS STOPPED");
	}
	else if(MotStopTicksElapsed > 100)
	{
		// se sono passati piu' di 5 sec e non sono ancora in allarme ce lo mando io
		StopMotorTimeOut = TRUE;
	}

	if(buttonGUITreatment[BUTTON_PRIMING_ABANDON].state == GUI_BUTTON_RELEASED)
	{
		releaseGUIButton(BUTTON_PRIMING_ABANDON);
		// non dovrebbe servire resettare la macchina a stati
		//CheckPumpStopTask((CHECK_PUMP_STOP_CMD)NO_CHECK_PUMP_STOP_CMD);
		//currentGuard[GUARD_ABANDON_PRIMING].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		SetAbandonGuard();
		// disabilito allarme di pompe non ferme
		// disabilito allarme di aria nel filtro durante il priming
		DisablePrimAlmSFAAirDetAlmFunc();
	}
}
//-----------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------
// conta il numero di volte che le pinch sono rilevate in una posizione non corretta
int PinchCloseAlwaysCnt;
int PinchClosedCnt;

// FUNZIONI PARENT PE RGESTIRE LO STATO PARENT_PRIM_WAIT_PINCH_CLOSE
// controllo che tutte le pinch siano in posizione altrimenti do un allarme
// l'arteriosa e venosa devono essere chiuse per proteggere l'organo quando viene attaccato
void manageParPrimWaitPinchCloseEntry(void)
{
//	CheckPinchPosTask((CHECK_PINCH_CMD)INIT_CHECK_PINCH_SEQ_CMD);

	if(!FilterSelected)
	{
		// il filtro non viene usato quindi devo passare sempre sul bypass
		setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_LEFT_OPEN);
	}
	else
		setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_RIGHT_OPEN);

	// prima di attaccare l'organo vogliono tutte le pinch chiuse
	setPinchPositionHighLevel(PINCH_2WPVA, MODBUS_PINCH_POS_CLOSED);
	if(GetTherapyType() == LiverTreat)
	{
		// ho selezionato il fegato, quindi devo chiudere anche questa
		setPinchPositionHighLevel(PINCH_2WPVV, MODBUS_PINCH_POS_CLOSED);
	}
	PinchCloseAlwaysCnt = 0;
	PinchClosedCnt = 0;
	// abilito allarme di pinch posizionate male
	EnableBadPinchPosAlmFunc();
}

// quando ritorna true posso generare un allarme di pinch posizionate male
// prima di collegare l'organo (devono essere tutte chiuse tranne quella del filter
bool AreAllPinchClose( void )
{
	// aspetto un tempo pari a 50 * 30 = 1500 msec
	// per fare in modo che 3 possibili attuazioni vengano attuate
	if(PinchCloseAlwaysCnt > 30)
	{
		return TRUE;
	}
	return FALSE;
}

// resetto la flag di allarme per permettere l'uscita dallo stato di
// allarme PARENT_PRIMING_END_RECIRC_ALARM
void ResetPrimPinchAlm(void)
{
	PinchCloseAlwaysCnt = 0;
}

void manageParPrimWaitPinchCloseAlways(void)
{
	bool PinchposOK;
	int PinchClosedCntForClose = 10;
	unsigned char PinchPos[3] = {0xff, MODBUS_PINCH_POS_CLOSED, MODBUS_PINCH_POS_CLOSED};
//	CheckPinchPosTask((CHECK_PINCH_CMD)NO_CHECK_PINCH_CMD);

	PinchposOK = IsPinchPosOk(PinchPos);
	if(PinchposOK)
	{
		PinchClosedCnt++;
		PinchCloseAlwaysCnt = 0;
	}
	if((PinchClosedCnt >= PinchClosedCntForClose) &&
	   currentGuard[GUARD_ENABLE_TREATMENT_KIDNEY_1].guardEntryValue == GUARD_ENTRY_VALUE_FALSE)
	{
		// le pinch sono chiuse posso passare al trattamento
		currentGuard[GUARD_ENABLE_TREATMENT_KIDNEY_1].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		// disabilito allarme di pinch non posizionate correttamente
		DisableBadPinchPosAlmFunc();
		DebugStringStr("PINCH CLOSED");
	}
	else if(!PinchposOK)
	{
		PinchCloseAlwaysCnt++;
		PinchClosedCnt = 0;
		// ripeto i comandi sulle pinch
		if(!FilterSelected)
		{
			// il filtro non viene usato quindi devo passare sempre sul bypass
			setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_LEFT_OPEN);
		}
		else
			setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_RIGHT_OPEN);
		// prima di attaccare l'organo vogliono tutte le pinch chiuse
		setPinchPositionHighLevel(PINCH_2WPVA, MODBUS_PINCH_POS_CLOSED);
		if(GetTherapyType() == LiverTreat)
		{
			// ho selezionato il fegato, quindi devo chiudere anche questa
			setPinchPositionHighLevel(PINCH_2WPVV, MODBUS_PINCH_POS_CLOSED);
		}
	}

	if(buttonGUITreatment[BUTTON_PRIMING_ABANDON].state == GUI_BUTTON_RELEASED)
	{
		releaseGUIButton(BUTTON_PRIMING_ABANDON);
		// non dovrebbe servire resettare la macchina a stati
		//CheckPumpStopTask((CHECK_PUMP_STOP_CMD)NO_CHECK_PUMP_STOP_CMD);
		//currentGuard[GUARD_ABANDON_PRIMING].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		SetAbandonGuard();
		// disabilito allarme di pinch non posizionate correttamente
		DisableBadPinchPosAlmFunc();
	}
}
//-----------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------
// gestione allarmi alla fine del ricircolo ( negli stati parent PARENT_PRIM_WAIT_MOT_STOP e PARENT_PRIM_WAIT_PINCH_CLOSE
void  manageParPrimEndRecAlarmEntry(void)
{}
void manageParPrimEndRecAlarmAlways(void)
{}
//-----------------------------------------------------------------------------------------------------


// gestione dello stato parent  PARENT_PRIM_KIDNEY_1_AIR_FILT
void manageParentPrimAirFiltEntry(void)
{
	AirAlarmRecoveryState = INIT_AIR_ALARM_RECOVERY;
}
/*Vincenzo: funzione che gestisce lo sbollamento dopo alarme aria*/
void manageParentPrimAirFiltAlways(void)
{
	AirAlarmRecoveryStateMach();
}

// gestione dello stato parent  PARENT_PRIM_KIDNEY_1_ALM_AIR_REC
void manageParentPrimAirAlmRecEntry(void)
{
}
void manageParentPrimAirAlmRecAlways(void)
{
}

void manageParentPrimingAlarmEntry(void)
{
	// entro in uno stato di allarme durante il priming
	AlarmInPrimingEntered = TRUE;

	if(ptrCurrentState->state == STATE_PRIMING_RICIRCOLO)
		AlarmOrStopInRecircFlag = TRUE;

	AtLeastoneButResRcvd = FALSE;
}


void manageParentPrimingAlarmAlways(void){
}

/*----------------------------------------FINE PARENT PRIMINGLEVEL FUNCTION --------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------------*/

/*----------------------------------------INIZIO PARENT TREATMENTLEVEL FUNCTION -----------------------------------------------*/

void manageParentTreatAlarmEntry(void){
	AtLeastoneButResRcvd = FALSE;
}

void manageParentTreatAlarmAlways(void){
}

// ritorna TRUE se l'allarme e' attivo
bool IsTreatSetPinchPosTaskAlm(void)
{
	TREAT_SET_PINCH_POS_TASK_STATE TSetPinchPosTaskS;
	TSetPinchPosTaskS = TreatSetPinchPosTask((TREAT_SET_PINCH_POS_CMD)T_SET_PINCH_ALARM_CMD);
	if(TSetPinchPosTaskS == T_SET_PINCH_ALARM)
		return TRUE;
	else
		return FALSE;
}

// resetto l'allarme se e' attivo
void ResetTreatSetPinchPosTaskAlm(void)
{
	TREAT_SET_PINCH_POS_TASK_STATE TSetPinchPosTaskS;
	TSetPinchPosTaskS = TreatSetPinchPosTask((TREAT_SET_PINCH_POS_CMD)T_SET_PINCH_ALARM_CMD);
	if(TSetPinchPosTaskS == T_SET_PINCH_ALARM)
		TreatSetPinchPosTask((TREAT_SET_PINCH_POS_CMD)T_SET_PINCH_RESET_ALARM_CMD);
}

// Questo task serve per posizionare le pinch e verificare il loro posizionamento
// prima di iniziare il trattamento. Questo processo deve partire quando l'utente
// preme il tasto BUTTON_START_TREATMENT.
// E' stato fatto in questo modo per evitare di modificare l'interfaccia GUI che, attualmente
// gestisce il bottone BUTTON_START_TREATMENT solo negli stati PARENT_TREAT_KIDNEY_1_INIT,PARENT_TREAT_KIDNEY_1_PUMP_ON,
// PARENT_TREAT_WAIT_START, PARENT_TREAT_WAIT_PAUSE.
// Se la generazione dell'allarme e' disabilitata invia solo i comandi di posizionamento delle pinch e
// poi va nello stato T_SET_PINCH_END che indica la corretta fine del processo.
TREAT_SET_PINCH_POS_TASK_STATE TreatSetPinchPosTask(TREAT_SET_PINCH_POS_CMD cmd)
{
	static TREAT_SET_PINCH_POS_TASK_STATE TreatSetPinchPosTaskState = T_SET_PINCH_IDLE;
	static int TreatSetPinchPosTaskPresc = 0;
	static unsigned char CorrectPosCnt = 0;
	static unsigned char WrongPosCnt = 0;
	unsigned char EndPositioning;
	unsigned char PinchPos[3] = {(FilterSelected) ? MODBUS_PINCH_RIGHT_OPEN : MODBUS_PINCH_LEFT_OPEN, MODBUS_PINCH_LEFT_OPEN, MODBUS_PINCH_LEFT_OPEN};

	THERAPY_TYPE TherType = GetTherapyType();
	if( cmd == T_SET_PINCH_RESET_CMD)
	{
		// metto la macchina nella posizione di inizio controllo posizione pinch
		TreatSetPinchPosTaskState = T_SET_PINCH_IDLE;
		return TreatSetPinchPosTaskState;
	}
	else if( cmd == T_SET_PINCH_DISABLE_CMD)
	{
		// disabilito la macchina a stati che controlla il corretto posizionamento delle pinch
		// prima di entrare in trattamento
		TreatSetPinchPosTaskState = T_SET_PINCH_DISABLE;
		return TreatSetPinchPosTaskState;
	}
	else if( cmd == T_SET_PINCH_ALARM_CMD)
	{
		// ritorno lo stato in modo che posso vedere se sono in allarme
		return TreatSetPinchPosTaskState;
	}
	else if( cmd == T_SET_PINCH_RESET_ALARM_CMD)
	{
		// vado nello stato di fine processo per proseguire nel trattamento
		// il tasto BUTTON_START_TREATMENT lo ho gia' ricevuto, quindi, per il prossimo start
		// devo forzarlo
		setGUIButton(BUTTON_START_TREATMENT);
		TreatSetPinchPosTaskState = T_SET_PINCH_END;
		return TreatSetPinchPosTaskState;
	}

	if(buttonGUITreatment[BUTTON_PRIMING_ABANDON].state == GUI_BUTTON_RELEASED)
	{
		releaseGUIButton(BUTTON_PRIMING_ABANDON);
		TreatSetPinchPosTaskState = T_SET_PINCH_DISABLE;
		//currentGuard[GUARD_ABANDON_PRIMING].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		SetAbandonGuard();
		return TreatSetPinchPosTaskState;
	}

	switch (TreatSetPinchPosTaskState)
	{
	case T_SET_PINCH_IDLE:
		TreatSetPinchPosTaskState = T_SET_PINCH_WAIT_START_BUTT;
		break;
	case T_SET_PINCH_WAIT_START_BUTT:
		if(buttonGUITreatment[BUTTON_START_TREATMENT].state == GUI_BUTTON_RELEASED)
		{
			// NON FACCIO VOLUTAMENTE LA RELEASEBUTTON PERCHE' MI SERVIRA' PER FORZARE LO START
			// DOPO AVER POSIZIONATO LE PINCH
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
			TreatSetPinchPosTaskState = T_SET_PINCH_WAIT_POS;
			DebugStringStr("PINCH TO ORGAN");
		}
		break;
	case T_SET_PINCH_WAIT_POS:
		EndPositioning = TRUE;
		if (PinchWriteTerminated(0) && (modbusData[PINCH_2WPVF-3][17] != 0xaa))
		{
			// posizione impostata non raggiunta
			EndPositioning = FALSE;
		}
		if (PinchWriteTerminated(1) && (modbusData[PINCH_2WPVA-3][17] != 0xaa))
		{
			// posizione impostata non raggiunta
			EndPositioning = FALSE;
		}
		if (PinchWriteTerminated(2) && (modbusData[PINCH_2WPVV-3][17] != 0xaa) && (TherType == LiverTreat))
		{
			// posizione impostata non raggiunta
			EndPositioning = FALSE;
		}
		if(EndPositioning)
		{
			if(!GlobalFlags.FlagsDef.EnableBadPinchPosAlm)
			{
				// la generazione dell'allarme e' disabilitata quindi vado direttamente alla
				// fine del processo di posizionamento delle pinch
				TreatSetPinchPosTaskState = T_SET_PINCH_CHECK_POS;
			}
			else
			{
				// posizionamento delle pinch completate, passo alla verifica con la protective
				TreatSetPinchPosTaskState = T_SET_PINCH_CHECK_POS;
				TreatSetPinchPosTaskPresc = 0;
				CorrectPosCnt = 0;
				WrongPosCnt = 0;
				if(IsPinchPosOk(PinchPos))
					CorrectPosCnt++;
				else
					WrongPosCnt++;
			}
		}
		break;
	case T_SET_PINCH_CHECK_POS:
		TreatSetPinchPosTaskPresc++;
		if(TreatSetPinchPosTaskPresc >= 10)
		{
			// dopo mezzo secondo confronto ancora la posizione con la protective
			TreatSetPinchPosTaskPresc = 0;
			if(IsPinchPosOk(PinchPos))
			{
				WrongPosCnt = 0;
				CorrectPosCnt++;
				if(CorrectPosCnt >= 3)
				{
					// la posizione e' identica a quella del protective per 3 volte consecutive, posso iniziare il trattamento
					// il tasto BUTTON_START_TREATMENT lo ho gia' ricevuto, quindi, per il prossimo start
					// devo forzarlo
					setGUIButton(BUTTON_START_TREATMENT);
					TreatSetPinchPosTaskState = T_SET_PINCH_END;
					DebugStringStr("PINCH TO ORGAN 1");
				}
			}
			else
			{
				CorrectPosCnt = 0;
				WrongPosCnt++;
				if(WrongPosCnt >= 3)
				{
					// la posizione e' diversa da quella del protective per 3 volte consecutive, posso generare un allarme
					TreatSetPinchPosTaskState = T_SET_PINCH_ALARM;
				}
			}
		}
		break;
		case T_SET_PINCH:
			break;
		case T_SET_PINCH_ALARM:
			break;
		case T_SET_PINCH_END:
			break;
		case T_SET_PINCH_DISABLE:
			break;
	}
	return TreatSetPinchPosTaskState;
}


// In questa funzione ci va quando sono nella fase iniziale del trattamento
// Le pompe verranno fatte partire successivamente su richiesta da parte dell'utente
void manageParentTreatEntry(void){
	if(pumpPerist[0].entry == 0)
	{
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);

		//-------------------------------------------------------------------------------------------------------------------------
		// PROBABILMENTE QUESTO CODICE NO SERVE PERCHE' LA RICHIESTA DI CONTROLLARE IL POSIZIONAMENTO DELLE PINCH VIENE FATTO ANCHE
		// IN ALTRI STATI:
		// - RITORNO DA ALLARME
		// - PARENT_TREAT_WAIT_START
		// - PARENT_TREAT_WAIT_PAUSE
		// FORSE SE INSERISCO QUESTA CHIAMATA POSSO ELIMINARE TUTTE LE ALTRE NEGLI ALTRI STATI.        NON LO PROVO PER ORA !!!!
		// preparo la macchina a stati per il controllo delle pinch aperte nella posizione richiesta per lo stato di trattamento
		// TreatSetPinchPosTask((TREAT_SET_PINCH_POS_CMD)T_SET_PINCH_RESET_CMD);
		//------------------------------------------------------------------------------------------------------------------------

//   CODICE USATO SIN DALL'INIZIO PER IL POSIZIONAMENTO DELLE PINCH PRIMA DI PARTIRE PER IL TRATTAMENTO E QUANDO SI
//   RITORNAVA DA UNA SITUAZIONE DI ALLARME. ORA NON SERVE PIU' PERCHE' SI E' DECISO DI UTILIZZARE LA FUNZIONE TreatSetPinchPosTask
//   ALL'INIZIO DELLA manageParentTreatAlways CHE CONTROLLA IL CORRETTO POSIZIONAMENTO DELLE PINCH PRIMA DI PROCEDERE CON IL TRATTAMENTO
//		if(!FilterSelected)
//		{
//			// il filtro non viene usato quindi devo passare sempre sul bypass
//			setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_LEFT_OPEN);
//		}
//		else
//			setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_RIGHT_OPEN);
//		// quando entro in trattamento la pinch deve essere attaccata all'organo
//		setPinchPositionHighLevel(PINCH_2WPVA, MODBUS_PINCH_LEFT_OPEN);
//		if(GetTherapyType() == LiverTreat)
//		{
//			// ho selezionato il fegato, quindi se entro in trattamento devo collegarmi all'organo
//			setPinchPositionHighLevel(PINCH_2WPVV, MODBUS_PINCH_LEFT_OPEN);
//		}
		setPumpPressLoop(0, PRESS_LOOP_OFF);

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

	// abilito allarme di pinch posizionate male
	EnableBadPinchPosAlmFunc();

	// inizializzo il controllo delle pinch fatto durante l'esecuzione del trattamento
	CheckCurrPinchPosTask(CHECK_CURR_PINCH_POS_INIT_CMD);

	// se non sono in allarme (dovrebbe essere sempre cosi') abilito la generazione del prossimo per sicurezza
	if(!IsAlarmActive())
		EnableNextAlarm = TRUE;

}

void manageParentTreatAlways(void)
{
	static char iflag_perf = 0;
	static char iflag_oxyg = 0;
	int speed = 0;
	static int timerCopy = 0;

	//if(GlobalFlags.FlagsDef.EnableBadPinchPosAlm)
	{
		// l'operazione che segue, comanda il posizionamento delle pinch e controlla
		// che sia corretto confrontandolo con quello rilevato dalla protective.
		// Se sono uguali il trattamento inizia, altrimenti viene generato un allarme.
		TREAT_SET_PINCH_POS_TASK_STATE TreatSePinchPosTaskStat;
		TreatSePinchPosTaskStat = TreatSetPinchPosTask((TREAT_SET_PINCH_POS_CMD)T_SET_PINCH_NO_CMD);
		if((TreatSePinchPosTaskStat == T_SET_PINCH_ALARM) ||
			(TreatSePinchPosTaskStat != T_SET_PINCH_END)
				&& (TreatSePinchPosTaskStat != T_SET_PINCH_DISABLE))
		{
			// non ho ancora terminato l'inizializzazione delle pinch oppure c'e' un allarme sul posizionamento delle pinch
			return;
		}
	}

	//manage pump
	switch(ptrCurrentParent->parent)
	{
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
		else if((buttonGUITreatment[BUTTON_STOP_TREATMENT].state == GUI_BUTTON_RELEASED) || (buttonGUITreatment[BUTTON_PRIMING_ABANDON].state == GUI_BUTTON_RELEASED))
		{
			// fermo le pompe ed il conteggio del tempo e volume del trattamento
			setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
			setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
			if(GetTherapyType() == LiverTreat)
			{
				// se sono nel trattamento fegato fermo anche l'altro motore !!
				setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
			}
			setPumpPressLoop(0, PRESS_LOOP_OFF);
			if(GetTherapyType() == LiverTreat)
				setPumpPressLoop(1, PRESS_LOOP_OFF); // pid venoso off
			TotalTreatDuration += TreatDuration;
			TreatDuration = 0;
			StartTreatmentTime = 0;
			StartPrimingTime = 0;
			//GlobalFlags.FlagsDef.EnableAllAlarms = 0;
			//DisableAllAlarm();
			//FilterFlowVal = 0;
			// Filippo - devo spegnere il PID
			FrigoHeatTempControlTaskNewPID((LIQ_TEMP_CONTR_TASK_CMD)TEMP_MANAGER_STOPPED_CMD);

			if(buttonGUITreatment[BUTTON_PRIMING_ABANDON].state == GUI_BUTTON_RELEASED)
			{
				releaseGUIButton(BUTTON_PRIMING_ABANDON);
				//currentGuard[GUARD_ABANDON_PRIMING].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
				SetAbandonGuard();
			}
			else
			{
				releaseGUIButton(BUTTON_STOP_TREATMENT);
				// resetto la macchina a stati che controlla se le pompe sono effettivamente ferme
				CheckPumpStopTask((CHECK_PUMP_STOP_CMD)INIT_CHECK_SEQ_CMD);
				currentGuard[GUARD_ENT_PAUSE_STATE_TREAT_KIDNEY_1_INIT].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
				DebugStringStr("Stop 1");
			}
			if(GetTherapyType() == LiverTreat)
			{
				// blocco aggiornamento della pompa di ossigenazione
				CheckDepurationSpeed(LastDepurationSpeed, FALSE, TRUE);
			}
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
			if(GetTherapyType() == KidneyTreat)
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
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
				// faccio partire la pompa di depurazione ad una velocita' prestabilita
				//setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, LIVER_PPAR_SPEED);

				// sblocco aggiornamento della pompa di ossigenazione
				CheckDepurationSpeed(LastDepurationSpeed, TRUE, FALSE);
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
			//GlobalFlags.FlagsDef.EnableAllAlarms = 1;
			SetAllAlarmEnableFlags();
			EnableFlowAndPressSetAlarmEnableFlags();
			SetFlowHigAlarmEnableFlags();
			EnableBadPinchPosAlmFunc();
			// disabilito allarme di livello alto in trattamento (per ora)
			GlobalFlags.FlagsDef.EnableLevHighAlarm = 0;
			//GlobalFlags.FlagsDef.TankLevelHigh = 0;
			//Filippo - aggiunto nel merge del 1-10-2018
			//al rientro dell'allarme, se in allarme era stato spento il frigo, lo riaccendo
		//	if (IsFrigoStoppedInAlarm())
			FrigoHeatTempControlTaskNewPID(LIQ_T_CONTR_TASK_RESET_CMD);
		}
		else if (buttonGUITreatment[BUTTON_START_PERF_PUMP].state == GUI_BUTTON_RELEASED)
		{
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
		else if(buttonGUITreatment[BUTTON_START_PERF_PUMP].state == GUI_BUTTON_RELEASED)
		{
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
		else if(buttonGUITreatment[BUTTON_START_OXYGEN_PUMP].state == GUI_BUTTON_RELEASED)
		{
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
		if((timerCounterModBus%9) == 7)
		//if(timerCounterPID >=1) //codice originale
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
			if(GetTherapyType() == KidneyTreat)
			{
				perfusionParam.treatVolAdsFilter +=  (word)((float)speed * CONV_RPMMIN_TO_ML_PER_INTERVAL);
				//FilterFlowVal = CalcFilterFlow(pumpPerist[0].actualSpeed);
			}
		}
		if(pumpPerist[1].dataReady == DATA_READY_TRUE)
		{
			pumpPerist[1].actualSpeed = modbusData[pumpPerist[1].pmpMySlaveAddress-2][17] / 100;
			// la pompa 2 e' agganciata alla 1
			pumpPerist[2].actualSpeed = modbusData[pumpPerist[2].pmpMySlaveAddress-2][17] / 100;//pumpPerist[1].actualSpeed;
			// calcolo il volume complessivo di liquido trattato dall'ossigenatore
			perfusionParam.treatVolPerfVenOxy = perfusionParam.treatVolPerfVenOxy +(word)((float)pumpPerist[1].actualSpeed * CONV_RPMMIN_TO_ML_PER_INTERVAL);
			pumpPerist[1].dataReady = DATA_READY_FALSE;
		}
		if(pumpPerist[3].dataReady == DATA_READY_TRUE)
		{
			pumpPerist[3].actualSpeed = modbusData[pumpPerist[3].pmpMySlaveAddress-2][17] / 100;
			pumpPerist[3].dataReady = DATA_READY_FALSE;
			if(GetTherapyType() == LiverTreat)
			{
				//FilterFlowVal = CalcFilterFlow(pumpPerist[3].actualSpeed);
				perfusionParam.treatVolAdsFilter += (word)((float)pumpPerist[3].actualSpeed * CONV_RPMMIN_TO_ML_PER_INTERVAL);
			}
		}

			// Controllo continuo quando il trattamento e' in corso. Viene confrontata la posizione impostata delle
			// pinch durante il trattamento con quella rilevata dalla protective.
			// Se non coincidono viene generato un allarme
			CheckCurrPinchPosTask(CHECK_CURR_PINCH_POS_NO_CMD);
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
		else if((buttonGUITreatment[BUTTON_STOP_TREATMENT].state == GUI_BUTTON_RELEASED) ||
				(buttonGUITreatment[BUTTON_PRIMING_ABANDON].state == GUI_BUTTON_RELEASED))
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
			setPumpPressLoop(0, PRESS_LOOP_OFF);
			TotalTreatDuration += TreatDuration;
			TreatDuration = 0;
			StartTreatmentTime = 0;
			//GlobalFlags.FlagsDef.EnableAllAlarms = 0;
			//DisableAllAlarm();
			//FilterFlowVal = 0;
			// Filippo - devo spegnere il PID
			FrigoHeatTempControlTaskNewPID((LIQ_TEMP_CONTR_TASK_CMD)TEMP_MANAGER_STOPPED_CMD);

			if(buttonGUITreatment[BUTTON_PRIMING_ABANDON].state == GUI_BUTTON_RELEASED)
				{
					releaseGUIButton(BUTTON_PRIMING_ABANDON);
					//currentGuard[GUARD_ABANDON_PRIMING].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
					SetAbandonGuard();
				}
				else
				{
					releaseGUIButton(BUTTON_STOP_TREATMENT);
					// resetto la macchina a stati che controlla se le pompe sono effettivamente ferme
					CheckPumpStopTask((CHECK_PUMP_STOP_CMD)INIT_CHECK_SEQ_CMD);
					currentGuard[GUARD_ENT_PAUSE_STATE_KIDNEY_1_PUMP_ON].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
					DebugStringStr("Stop 2");
				}
				if(GetTherapyType() == LiverTreat)
				{
					// blocco aggiornamento della pompa di ossigenazione
					CheckDepurationSpeed(LastDepurationSpeed, FALSE, TRUE);
				}
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

			if(GetTherapyType() == KidneyTreat)
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
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

				// sblocco aggiornamento della pompa di ossigenazione
				CheckDepurationSpeed(LastDepurationSpeed, TRUE, FALSE);
			}
			//GlobalFlags.FlagsDef.EnableAllAlarms = 1;
			SetAllAlarmEnableFlags();
			EnableBadPinchPosAlmFunc();
			EnableFlowAndPressSetAlarmEnableFlags();
			SetFlowHigAlarmEnableFlags();
			// disabilito allarme di livello alto in trattamento (per ora)
			GlobalFlags.FlagsDef.EnableLevHighAlarm = 0;
			//GlobalFlags.FlagsDef.TankLevelHigh = 0;
			// Filippo - aggiunto nel merge del 1-10-2018
			//al rientro dell'allarme, se in allarme era stato spento il frigo, lo riaccendo
		//	if (IsFrigoStoppedInAlarm())
			FrigoHeatTempControlTaskNewPID(LIQ_T_CONTR_TASK_RESET_CMD);
		}
		else if(buttonGUITreatment[BUTTON_START_PERF_PUMP].state == GUI_BUTTON_RELEASED)
		{
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
		else if(buttonGUITreatment[BUTTON_START_OXYGEN_PUMP].state == GUI_BUTTON_RELEASED)
		{
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
		if((timerCounterModBus%9) == 7)
		//if(timerCounterPID >=1)
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
			if(GetTherapyType() == KidneyTreat)
			{
				perfusionParam.treatVolAdsFilter +=  (word)((float)speed * CONV_RPMMIN_TO_ML_PER_INTERVAL);
				//FilterFlowVal = CalcFilterFlow(pumpPerist[0].actualSpeed);
			}
		}
		if(pumpPerist[1].dataReady == DATA_READY_TRUE)
		{
			pumpPerist[1].actualSpeed = modbusData[pumpPerist[1].pmpMySlaveAddress-2][17] / 100;
			// la pompa 2 e' agganciata alla 1
			pumpPerist[2].actualSpeed = modbusData[pumpPerist[2].pmpMySlaveAddress-2][17] / 100;//pumpPerist[1].actualSpeed;
			// calcolo il volume complessivo di liquido trattato dall'ossigenatore
			perfusionParam.treatVolPerfVenOxy = perfusionParam.treatVolPerfVenOxy +(word)((float)pumpPerist[1].actualSpeed * CONV_RPMMIN_TO_ML_PER_INTERVAL);
			pumpPerist[1].dataReady = DATA_READY_FALSE;
		}
		if(pumpPerist[3].dataReady == DATA_READY_TRUE)
		{
			pumpPerist[3].actualSpeed = modbusData[pumpPerist[3].pmpMySlaveAddress-2][17] / 100;
			pumpPerist[3].dataReady = DATA_READY_FALSE;
			if(GetTherapyType() == LiverTreat)
			{
				perfusionParam.treatVolAdsFilter +=  (word)((float)pumpPerist[3].actualSpeed * CONV_RPMMIN_TO_ML_PER_INTERVAL);
				//FilterFlowVal = CalcFilterFlow(pumpPerist[3].actualSpeed);
			}
		}

		// Controllo continuo quando il trattamento e' in corso. Viene confrontata la posizione impostata delle
		// pinch durante il trattamento con quella rilevata dalla protective.
		// Se non coincidono viene generato un allarme
		CheckCurrPinchPosTask(CHECK_CURR_PINCH_POS_NO_CMD);
		break;

	case PARENT_TREAT_KIDNEY_1_ALARM:
		setPumpSpeedValue(pumpPerist[0].pmpMySlaveAddress, 0);
		break;

	case PARENT_TREAT_KIDNEY_1_END:
		setPumpSpeedValue(pumpPerist[0].pmpMySlaveAddress, 0);
		break;

	case PARENT_TREAT_WAIT_START:
	case PARENT_TREAT_WAIT_PAUSE:
		if(buttonGUITreatment[BUTTON_START_TREATMENT].state == GUI_BUTTON_RELEASED)
		{
			// devo fare il release altrimenti in questo posto ci passa piu' volte
			releaseGUIButton(BUTTON_START_TREATMENT);
			// ripristino allarmi delle cover
			GlobalFlags.FlagsDef.EnableCoversAlarm = 1;
			// ripristino allarmi di temperatura arteriosa
			GlobalFlags.FlagsDef.EnableTempMaxMin = 1;
			GlobalFlags.FlagsDef.EnableTempArtOORAlm = 1;
			if(ptrCurrentParent->parent == PARENT_TREAT_WAIT_START)
			{
				currentGuard[GUARD_ENABLE_STATE_TREAT_KIDNEY_1_INIT].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
				DebugStringStr("Start (init)");
			}
			else
			{
				currentGuard[GUARD_ENABLE_STATE_KIDNEY_1_PUMP_ON].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
				DebugStringStr("Start (run)");
			}

			// Filippo - quando fai partire il trattamento riattivo il PID
			FrigoHeatTempControlTaskNewPID((LIQ_TEMP_CONTR_TASK_CMD)LIQ_T_CONTR_TASK_RESET_CMD);

		}
		else if(buttonGUITreatment[BUTTON_STOP_TREATMENT].state == GUI_BUTTON_RELEASED)
		{
			// se mi arriva uno stop in questo stato lo butto via altrimenti mi rimane memorizzato
			// per quando ritornero' in run
			releaseGUIButton(BUTTON_STOP_TREATMENT);
		}
		else if(buttonGUITreatment[BUTTON_PRIMING_ABANDON].state == GUI_BUTTON_RELEASED)
		{
			releaseGUIButton(BUTTON_PRIMING_ABANDON);
			//currentGuard[GUARD_ABANDON_PRIMING].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			SetAbandonGuard();
			// ripristino allarmi delle cover
			GlobalFlags.FlagsDef.EnableCoversAlarm = 1;
			// ripristino allarmi di temperatura arteriosa
			GlobalFlags.FlagsDef.EnableTempMaxMin = 1;
			GlobalFlags.FlagsDef.EnableTempArtOORAlm = 1;
		}

		CheckPumpStopTask((CHECK_PUMP_STOP_CMD)NO_CHECK_PUMP_STOP_CMD);
		break;

	default:
		break;
	}
}

void setPumpPressLoop(unsigned char pmpId, unsigned char valOnOff){
	pumpPerist[pmpId].pmpPressLoop = valOnOff;
	PidFirstTime[pmpId] = valOnOff;
	if(valOnOff == PRESS_LOOP_OFF)
		pumpPerist[pmpId].actualSpeedOld = 0;
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
	//currentGuard[GUARD_HW_T1T_DONE].guardEntryValue = GUARD_ENTRY_VALUE_TRUE; //messa a true solo se t1 test sono positivi
	//currentGuard[GUARD_COMM_ENABLED].guardEntryValue = GUARD_ENTRY_VALUE_TRUE; //messa a true solo se t1 test sono positivi
	currentGuard[GUARD_T1_NO_DISP_OK].guardEntryValue = GUARD_ENTRY_VALUE_TRUE; //inizio dei t1 test
}


// Processo di svuotamento del disposable----------------------------------------------
word VolumeDischarged = 0;
bool EmptyDisposStartOtherPump = FALSE;
EMPTY_DISPOSABLE_STATE EmptyDispRunAlwaysState = INIT_EMPTY_DISPOSABLE;
unsigned long EmptyWFLOATimeElapsed = 0;
unsigned long EmptyWFLOATimeElapsed_Last;

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

// faccio ripartire le pompe dopo un allarme
void RestartPumpsEmptyState(void)
{
	switch (EmptyDispRunAlwaysState)
	{
	case INIT_EMPTY_DISPOSABLE:
		// in questo stato nessuna pompa e' attiva
		break;
	case WAIT_FOR_1000ML:
		if(GetTherapyType() == LiverTreat)
			setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, LIVER_PPAR_EMPTY_SPEED);
		else if(GetTherapyType() == KidneyTreat)
			setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, KIDNEY_EMPTY_PPAR_SPEED);
		break;
	case WAIT_FOR_AIR_ALARM:
		if(GetTherapyType() == LiverTreat)
		{
			setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, LIVER_PPAR_EMPTY_SPEED);
			// dopo l'ultimo allarme aria le pompe non addette allo scarico non devono piu' ripartire
			if(!IsDisposableEmpty())
			{
				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, LIVER_PPAR_EMPTY_SPEED);
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, LIVER_PPAR_EMPTY_SPEED);
			}
		}
		else if(GetTherapyType() == KidneyTreat)
		{
				// nel caso del rene sono le pompe di ossigenazione che svuotano il recevoir
			setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, KIDNEY_EMPTY_PPAR_SPEED);
			// dopo l'ultimo allarme aria le pompe non addette allo scarico non devono piu' ripartire
			if(!IsDisposableEmpty())
				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, KIDNEY_EMPTY_PPAR_SPEED);
		}
		break;
	case WAIT_FOR_LEVEL_OR_AMOUNT:
		EmptyWFLOATimeElapsed = EmptyWFLOATimeElapsed_Last;
		if(EmptyWFLOATimeElapsed < EMPTY_TIME_PUMPS_ON_AFTER_AIR)
		{
			if(GetTherapyType() == LiverTreat)
			{
				setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, LIVER_PPAR_EMPTY_SPEED);
				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, LIVER_PPAR_EMPTY_SPEED);
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, LIVER_PPAR_EMPTY_SPEED);
			}
			else if(GetTherapyType() == KidneyTreat)
			{
				// nel caso del rene sono le pompe di ossigenazione che svuotano il recevoir
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, KIDNEY_EMPTY_PPAR_SPEED);
				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, KIDNEY_EMPTY_PPAR_SPEED);
			}
		}
		else
		{
			if(GetTherapyType() == LiverTreat)
				setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, LIVER_PPAR_EMPTY_SPEED);
			else if(GetTherapyType() == KidneyTreat)
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, KIDNEY_EMPTY_PPAR_SPEED);
		}
		break;
	}
}

// bottoni da usare
#define BUT_START_EMPTY  BUTTON_START_EMPTY_DISPOSABLE
#define BUT_STOP_EMPTY  BUTTON_STOP_EMPTY_DISPOSABLE

// bottoni usati senza gui per debug
//#define BUT_START_EMPTY  BUTTON_START_PRIMING
//#define BUT_STOP_EMPTY  BUTTON_STOP_PRIMING

// I bottoni usati in questa funzione sono:
//        BUTTON_START_EMPTY_DISPOSABLE per partire o ripartire con lo scaricamento
//        BUTTON_PRIMING_ABANDON        per abbandonare lo stato ed andare in idle
//        BUTTON_STOP_EMPTY_DISPOSABLE  per fermare momentaneamente tutte le pompe
void EmptyDispStateMach(void)
{
	static unsigned long u32 = 0;
	static bool PutPinchInSafetyPosFlag = FALSE;
	static unsigned char EmptyPumpsStopped = 0;
	THERAPY_TYPE TherType = GetTherapyType();
	int StarEmptyDispButId;
	int StopAllPumpButId;
	// quantita' di liquido scaricata prima di iniziare lo scaricamento del disposable
	word DischargeAmountArtPump;

	StarEmptyDispButId = BUT_START_EMPTY;
	StopAllPumpButId = BUT_STOP_EMPTY;

	switch (EmptyDispRunAlwaysState)
	{
	case INIT_EMPTY_DISPOSABLE:
		EmptyWFLOATimeElapsed_Last = 0;
		EmptyWFLOATimeElapsed = 0;
		if(buttonGUITreatment[StarEmptyDispButId].state == GUI_BUTTON_RELEASED)
		{
			// attivo la pompa per iniziare ko svuotamento
			releaseGUIButton(StarEmptyDispButId);
			PutPinchInSafetyPosFlag = PutPinchInSafetyPos();
			// attivazione della pompa di svuotamento
			if(GetTherapyType() == LiverTreat)
			{
				// nel caso del fegato e' la pompa di depurazione che svuota il recevoir
				setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, LIVER_PPAR_EMPTY_SPEED);
			}
			else if(GetTherapyType() == KidneyTreat)
			{
				// nel caso del rene sono le pompe di ossigenazione che svuotano il recevoir
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, KIDNEY_EMPTY_PPAR_SPEED);
			}
			// nel processo di svuotamento non mi servono tutti gli allarmi ma solo quelli di aria
			DisableAllAlarm();
			// abilito anche gli allarmi di pressione alta
			GlobalFlags.FlagsDef.EnablePressSensHighAlm = 1;
			// abilito anche gli allarmi delle cover
			GlobalFlags.FlagsDef.EnableCoversAlarm = 1;
			// abilito gli allarmi su modbus
			GlobalFlags.FlagsDef.EnableModbusNotRespAlm = 1;
			// abilito l'allarme su mancata comunicazione CANBUS
			GlobalFlags.FlagsDef.EnableCANBUSErr = 1;
			// abilito allarmi Hall Machine Covers
			GlobalFlags.FlagsDef.EnableMachineCovers = 1;
			// abilito allarmi Hall ganci contenitore organo
			GlobalFlags.FlagsDef.EnableHooksReservoir = 1;

			EmptyDispRunAlwaysState = WAIT_FOR_1000ML;
		}
		else if(buttonGUITreatment[BUTTON_PRIMING_ABANDON].state == GUI_BUTTON_RELEASED)
		{
			// nessuna pompa e' partita ancora
			releaseGUIButton(BUTTON_PRIMING_ABANDON);
			//currentGuard[GUARD_ABANDON_PRIMING].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			SetAbandonGuard();
		}
		break;
	case WAIT_FOR_1000ML:
		// controllo che le pinch siano in sicurezza
		if(!PutPinchInSafetyPosFlag)
			PutPinchInSafetyPosFlag = PutPinchInSafetyPos();

		//DischargeAmountArtPump = (word)((float)perfusionParam.priVolPerfArt * (float)DISCHARGE_AMOUNT_ART_PUMP / 100.0);
		DischargeAmountArtPump = (word)((float)GetTotalPrimingVolumePerf((int)0) * (float)DISCHARGE_AMOUNT_ART_PUMP / 100.0);
		if(!EmptyDisposStartOtherPump && VolumeDischarged >= DischargeAmountArtPump)
		{
			// faccio partire le altre pompe per svuotare i tubi
			if(TherType == LiverTreat)
			{
				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, LIVER_PPAR_EMPTY_SPEED);
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, LIVER_PPAR_EMPTY_SPEED);
			}
			else if(TherType == KidneyTreat)
				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, KIDNEY_EMPTY_PPAR_SPEED);
			EmptyDisposStartOtherPump = TRUE;

		    ChildEmptyFlags.FlagsVal = 0;
#ifdef	EMPTY_PROC_WITH_ALARM
			// abilito gli allarmi aria
			DisableAllAirAlarm(FALSE);
#endif
			EmptyDispRunAlwaysState = WAIT_FOR_AIR_ALARM;
		}
		else if(buttonGUITreatment[StopAllPumpButId].state == GUI_BUTTON_RELEASED)
		{
			releaseGUIButton(StopAllPumpButId);

			/*tengo traccia che � stato premuto un tasto di stop*/
			emptyStopButtonPressed = TRUE;

			if(GetTherapyType() == LiverTreat)
				setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
			else if(GetTherapyType() == KidneyTreat)
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
		}
		else if(buttonGUITreatment[StarEmptyDispButId].state == GUI_BUTTON_RELEASED)
		{
			if (emptyStopButtonPressed == TRUE)
			{
				emptyStopButtonPressed = FALSE;
				RestartPumpsEmptyState();
			}

			// faccio ripartire le pompe per lo svuotamento
			releaseGUIButton(StarEmptyDispButId);
			if(GetTherapyType() == LiverTreat)
				setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, LIVER_PPAR_EMPTY_SPEED);
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
			//currentGuard[GUARD_ABANDON_PRIMING].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			SetAbandonGuard();
		}
		break;
	case WAIT_FOR_AIR_ALARM:
		if(IsDisposableEmpty() && (ptrCurrentParent->parent != PARENT_EMPTY_DISPOSABLE_ALARM))
		{
			// ho ricevuto tutti gli allarmi aria ed ho risposto con reset
			if(TherType == LiverTreat)
			{
				// NON FERMO LE ALTRE POMPE MA LE FACCIO CONTINUARE FINO ALLA FINE
				// ho rilevato una presenza aria nel circuito di perfusione arteriosa e venosa
				// fermo le pompe e proseguo con lo svuotamento dall'ultimo liquido rimasto
//					setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
//					setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
				u32 = timerCounterModBus;
				EmptyDispRunAlwaysState = WAIT_FOR_LEVEL_OR_AMOUNT;
				EmptyWFLOATimeElapsed = 0;
				EmptyPumpsStopped = 0;
			}
			else if(TherType == KidneyTreat)
			{
				// NON FERMO LE ALTRE POMPE MA LE FACCIO CONTINUARE FINO ALLA FINE
				// ho rilevato una presenza aria nel circuito di perfusione arteriosa
				// fermo la pompa e proseguo con lo svuotamento dall'ultimo liquido rimasto
//					setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
				u32 = timerCounterModBus;
				EmptyDispRunAlwaysState = WAIT_FOR_LEVEL_OR_AMOUNT;
				EmptyWFLOATimeElapsed = 0;
				EmptyPumpsStopped = 0;
			}
		}
		else if(buttonGUITreatment[StopAllPumpButId].state == GUI_BUTTON_RELEASED)
		{
			releaseGUIButton(StopAllPumpButId);

			/*tengo traccia che � stato premuto un tasto di stop*/
			emptyStopButtonPressed = TRUE;

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
			if (emptyStopButtonPressed == TRUE)
			{
				emptyStopButtonPressed = FALSE;
				RestartPumpsEmptyState();
			}

			// faccio ripartire le pompe per lo svuotamento
			releaseGUIButton(StarEmptyDispButId);
			if(GetTherapyType() == LiverTreat)
			{
				setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, LIVER_PPAR_EMPTY_SPEED);
				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, LIVER_PPAR_EMPTY_SPEED);
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, LIVER_PPAR_EMPTY_SPEED);
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
				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
			}
			//currentGuard[GUARD_ABANDON_PRIMING].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			SetAbandonGuard();
		}
		break;
	case WAIT_FOR_LEVEL_OR_AMOUNT:
		EmptyWFLOATimeElapsed = msTick_elapsed(u32);
		if((EmptyWFLOATimeElapsed >= EMPTY_TIME_PUMPS_ON_AFTER_AIR) && !EmptyPumpsStopped)
		{
			EmptyPumpsStopped = 1;
			// scaduti i 10 secondi fermo le altre pompe e lascio solo quella di scarico se,
			// nel frattempo, non ha raggiunto il volume stabilito da scaricare
			if(TherType == LiverTreat)
			{
				// fermo le pompe e proseguo con lo svuotamento dall'ultimo liquido rimasto
				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
			}
			else if(TherType == KidneyTreat)
			{
				// fermo la pompa e proseguo con lo svuotamento dall'ultimo liquido rimasto
				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
			}
		}
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
			//currentGuard[GUARD_ABANDON_PRIMING].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			SetAbandonGuard();
		}
		else if(buttonGUITreatment[StopAllPumpButId].state == GUI_BUTTON_RELEASED)
		{
			releaseGUIButton(StopAllPumpButId);

			/*tengo traccia che � stato premuto un tasto di stop*/
			emptyStopButtonPressed = TRUE;

			if(GetTherapyType() == LiverTreat)
				setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
			else if(GetTherapyType() == KidneyTreat)
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
		}
		else if(buttonGUITreatment[StarEmptyDispButId].state == GUI_BUTTON_RELEASED)
		{
			if (emptyStopButtonPressed == TRUE)
			{
				emptyStopButtonPressed = FALSE;
				RestartPumpsEmptyState();
			}

			// faccio ripartire le pompe per lo svuotamento
			releaseGUIButton(StarEmptyDispButId);
			if(GetTherapyType() == LiverTreat)
				setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, LIVER_PPAR_EMPTY_SPEED);
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
	ChildEmptyFlags.FlagsVal = 0;
	EmptyWFLOATimeElapsed = 0;
}
void manageParentEmptyDisposInitAlways(void)
{
	EmptyDispStateMach();
	CalcVolumeDischarged();
}

void manageParentEmptyDisposRunEntry(void)
{
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

void AirAlarmRecoveryStateMach_originale(void)
{
	static unsigned long StarDelay = 0;
	THERAPY_TYPE TherType = GetTherapyType();
	switch (AirAlarmRecoveryState)
	{
	case INIT_AIR_ALARM_RECOVERY:
		AirAlarmRecoveryState = START_AIR_PUMP;
		TimeoutAirEjection = 0;
		break;
	case START_AIR_PUMP:
		if(recoveryParentState == PARENT_TREAT_KIDNEY_1_AIR_FILT)
		{
			if(TherType == KidneyTreat)
				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, AIR_REJECT_SPEED);
			else if(TherType == LiverTreat)
				setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, AIR_REJECT_SPEED);
			AirAlarmRecoveryState = AIR_CHANGE_START_TIME;
			StarDelay = timerCounterModBus;
		}
		else if(recoveryParentState == PARENT_TREAT_KIDNEY_1_SFV)
		{
			// faccio partire la coppia di pompe per la venosa nel fegato e per l'ossigenazione nel rene
			setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, AIR_REJECT_SPEED);
			AirAlarmRecoveryState = AIR_CHANGE_START_TIME;
			StarDelay = timerCounterModBus;
		}
		else if(recoveryParentState == PARENT_TREAT_KIDNEY_1_SFA)
		{
			// parte la sempre la pompa a cui si riferisce la struttura pumpPerist 0
			setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, AIR_REJECT_SPEED);
			AirAlarmRecoveryState = AIR_CHANGE_START_TIME;
			StarDelay = timerCounterModBus;
		}
		break;
	case AIR_CHANGE_START_TIME:
		if((recoveryParentState == PARENT_TREAT_KIDNEY_1_AIR_FILT) && (Air_1_Status == LIQUID))
		{
			//  la bolla se ne e' andata faccio; ripartire il timer per misurare il tempo a partire da ora
			startTimeToRecovery = timerCounterModBus;
			AirAlarmRecoveryState = STOP_AIR_PUMP;
		}
		else if((recoveryParentState == PARENT_TREAT_KIDNEY_1_SFV) && (sensor_UFLOW[VENOUS_AIR_SENSOR].bubbleSize < MAX_BUBBLE_SIZE))
		{
			//  la bolla se ne e' andata faccio; ripartire il timer per misurare il tempo a partire da ora
			startTimeToRecovery = timerCounterModBus;
			AirAlarmRecoveryState = STOP_AIR_PUMP;
		}
		else if((recoveryParentState == PARENT_TREAT_KIDNEY_1_SFA) && (sensor_UFLOW[ARTERIOUS_AIR_SENSOR].bubbleSize < MAX_BUBBLE_SIZE))
		{
			//  la bolla se ne e' andata faccio; ripartire il timer per misurare il tempo a partire da ora
			startTimeToRecovery = timerCounterModBus;
			AirAlarmRecoveryState = STOP_AIR_PUMP;
		}
		else
		{
			// Dopo un timeout esco comunque e vado avanti dopo aver segnalato alla gui il fatto
			if(StarDelay && ((msTick_elapsed(StarDelay) * 50L) >= 10000L))
			{
				// sono passati 10 secondi la bolla d'aria non si e' ancora spostata
				// forzo il passaggio allo stato successivo.
				startTimeToRecovery = timerCounterModBus;
				AirAlarmRecoveryState = STOP_AIR_PUMP;
				TimeoutAirEjection = 1;
			}
		}
		break;
	case STOP_AIR_PUMP:
		if(startTimeToRecovery && ((msTick_elapsed(startTimeToRecovery) + totalTimeToRecovery) * 50L >= TIME_TO_REJECT_AIR))
		{
			// la pompa per l'espulsione dell'aria ha fatto 10 giri, che ritengo sufficienti per
			// buttare fuori l'aria
			if(recoveryParentState == PARENT_TREAT_KIDNEY_1_AIR_FILT)
			{
				if(TherType == KidneyTreat)
					setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
				else if(TherType == LiverTreat)
					setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
				AirAlarmRecoveryState = AIR_REJECTED;
				//currentGuard[GUARD_AIR_RECOVERY_END].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
				StarDelay = timerCounterModBus;
			}
			else if(recoveryParentState == PARENT_TREAT_KIDNEY_1_SFV)
			{
				// fermo la coppia di pompe per la venosa nel fegato e per l'ossigenazione nel rene
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
				AirAlarmRecoveryState = AIR_REJECTED;
				//currentGuard[GUARD_AIR_RECOVERY_END].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
				StarDelay = timerCounterModBus;
			}
			else if(recoveryParentState == PARENT_TREAT_KIDNEY_1_SFA)
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

void AirAlarmRecoveryStateMach(void)
{
	static unsigned long StarDelay = 0;
	static DELTA_T_HIGH_ALM_RECVR_STATE LastAirAlarmRecoveryState = INIT_AIR_ALARM_RECOVERY;
	static int TimeRemaining = TIME_TO_REJECT_AIR;

	/*flag che mi dice se la procedura di rimozione aria � partita*/
	static bool AirRejectedStarted = FALSE;

	THERAPY_TYPE TherType = GetTherapyType();

	/*se � stato premuto il tasto di stop prima di arrivare qui, non faccio nulla*/
	if (ButtonStopPressed && AirRejectedStarted == FALSE)
		return;

	if(buttonGUITreatment[BUTTON_START_TREATMENT].state == GUI_BUTTON_RELEASED ||
	   buttonGUITreatment[BUTTON_START_PRIMING].state == GUI_BUTTON_RELEASED	 )
	{
		if (buttonGUITreatment[BUTTON_START_TREATMENT].state == GUI_BUTTON_RELEASED)
			releaseGUIButton(BUTTON_START_TREATMENT);
		else //if (buttonGUITreatment[BUTTON_START_PRIMING].state == GUI_BUTTON_RELEASED)
			releaseGUIButton(BUTTON_START_PRIMING);

		if( AirAlarmRecoveryState == AIR_REJECT_STOPPED )
		{
			AirAlarmRecoveryState =	LastAirAlarmRecoveryState;
			LastAirAlarmRecoveryState = INIT_TEMP_ALARM_RECOVERY;
			StarDelay = timerCounterModBus;

			/*originale*/
//			if(TherType == KidneyTreat)
//			{
//				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, TEMP_RESTORE_SPEED);
//				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, TEMP_RESTORE_SPEED);
//			}
//			else if(TherType == LiverTreat)
//			{
//				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, TEMP_RESTORE_SPEED);
//				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, TEMP_RESTORE_SPEED);
//				setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, TEMP_RESTORE_SPEED);
//			}
//
//			if(recoveryParentState == PARENT_TREAT_KIDNEY_1_AIR_FILT)
//			{
//				if(TherType == KidneyTreat)
//					setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, AIR_REJECT_SPEED);
//				else if(TherType == LiverTreat)
//					setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, AIR_REJECT_SPEED);
//			}
//			else if(recoveryParentState == PARENT_TREAT_KIDNEY_1_SFV)
//				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, AIR_REJECT_SPEED);
//			else if(recoveryParentState == PARENT_TREAT_KIDNEY_1_SFA)
//				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, AIR_REJECT_SPEED);
			/*end*/

			/*faccio ripartire tutte le pompe perch� la rimozione d'aria la faccio su
			 * tutte le linee indipendentemente dal sensore che ha rilevato aria*/
			setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, AIR_REJECT_SPEED); // pompa forntale sinistra per kidney e destra per liver

			if ( TherType == LiverTreat ||
			    (TherType == KidneyTreat && (PARAMETER_ACTIVE_TYPE)parameterWordSetFromGUI[PAR_SET_DEPURATION_ACTIVE].value == YES) )
			setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, AIR_REJECT_SPEED);	//pompe OXY
			/*la pompa frontale detsra nel kidney ha indirizzo 3 e la faccio mentre nel liver 0
			 * questa pompa viene usata solo per se il trattamento � il liver*/
			if(TherType == LiverTreat)
				setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, AIR_REJECT_SPEED); // pompa frontale sinistra er liver e non usata per Kidney

		}
	}
	else if(buttonGUITreatment[BUTTON_STOP_TREATMENT].state == GUI_BUTTON_RELEASED ||
			buttonGUITreatment[BUTTON_STOP_PRIMING].state == GUI_BUTTON_RELEASED   )
	{
		if (buttonGUITreatment[BUTTON_STOP_TREATMENT].state == GUI_BUTTON_RELEASED)
			releaseGUIButton(BUTTON_STOP_TREATMENT);
		else //if (buttonGUITreatment[BUTTON_STOP_PRIMING].state == GUI_BUTTON_RELEASED)
			releaseGUIButton(BUTTON_STOP_PRIMING);

		if( (AirAlarmRecoveryState == START_AIR_PUMP) ||
			(AirAlarmRecoveryState == AIR_CHANGE_START_TIME) ||
			(AirAlarmRecoveryState == STOP_AIR_PUMP) )
		{
			LastAirAlarmRecoveryState = AirAlarmRecoveryState;
			AirAlarmRecoveryState = AIR_REJECT_STOPPED;
			TimeRemaining = TIME_TO_REJECT_AIR - msTick_elapsed(StarDelay) * 50L;
			if(TimeRemaining > TIME_TO_REJECT_AIR)
				TimeRemaining = TIME_TO_REJECT_AIR;
			else if(TimeRemaining < 0)
				TimeRemaining = TIME_TO_REJECT_AIR;
/*originale*/
//			if(recoveryParentState == PARENT_TREAT_KIDNEY_1_AIR_FILT)
//			{
//				if(TherType == KidneyTreat)
//					setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
//				else if(TherType == LiverTreat)
//					setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
//			}
//			else if(recoveryParentState == PARENT_TREAT_KIDNEY_1_SFV)
//			{
//				// fermo la coppia di pompe per la venosa nel fegato e per l'ossigenazione nel rene
//				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
//			}
//			else if(recoveryParentState == PARENT_TREAT_KIDNEY_1_SFA)
//			{
//				// fermo la pompa a cui si riferisce la struttura pumpPerist 0
//				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
//			}
/*end*/
			/*fermo tutte le pompe perch� la rimozione d'aria la faccio su
			 * tutte le linee indipendentemente dal sensore che ha rilevato aria*/
			setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0); // pompa forntale sinistra per kidney e destra per liver
			setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);	//pompe OXY
			/*la pompa frontale detsra nel kidney ha indirizzo 3 e la faccio mentre nel liver 0
			 * questa pompa viene usata solo per se il trattamento � il liver*/
			if(TherType == LiverTreat)
				setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0); // pompa frontale sinistra er liver e non usata per Kidney

		}
	}

	switch (AirAlarmRecoveryState)
	{
	case INIT_AIR_ALARM_RECOVERY:
		AirAlarmRecoveryState = START_AIR_PUMP;
		TimeoutAirEjection = 0;
		TimeRemaining = TIME_TO_REJECT_AIR;
		break;
	case START_AIR_PUMP:
/*originale*/
//		if(recoveryParentState == PARENT_TREAT_KIDNEY_1_AIR_FILT)
//		{
//			if(TherType == KidneyTreat)
//				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, AIR_REJECT_SPEED);
//			else if(TherType == LiverTreat)
//				setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, AIR_REJECT_SPEED);
//			AirAlarmRecoveryState = AIR_CHANGE_START_TIME;
//			StarDelay = timerCounterModBus;
//		}
//		else if(recoveryParentState == PARENT_TREAT_KIDNEY_1_SFV)
//		{
//			// faccio partire la coppia di pompe per la venosa nel fegato e per l'ossigenazione nel rene
//			setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, AIR_REJECT_SPEED);
//			AirAlarmRecoveryState = AIR_CHANGE_START_TIME;
//			StarDelay = timerCounterModBus;
//		}
//		else if(recoveryParentState == PARENT_TREAT_KIDNEY_1_SFA)
//		{
//			// parte la sempre la pompa a cui si riferisce la struttura pumpPerist 0
//			setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, AIR_REJECT_SPEED);
//			AirAlarmRecoveryState = AIR_CHANGE_START_TIME;
//			StarDelay = timerCounterModBus;
//		}
/*end*/
		AirRejectedStarted = TRUE;

//		/*Per sicurezza, metto le pinch venosa (solo per liver) e arteriosa sullo scarico e bypass
		/*per la filtro nel caso non fossero nella corretta posizione solo se sono in uno di priming PH 2 opp treatment*/
		if (ptrCurrentState->state == STATE_PRIMING_PH_2 || ptrCurrentState->state == STATE_TREATMENT_KIDNEY_1)
		{
		if ( (modbusData[PINCH_2WPVF-3][0] & 0x0F) != MODBUS_PINCH_LEFT_OPEN)
			setPinchPosValue(PINCH_2WPVF,MODBUS_PINCH_LEFT_OPEN);

		if( (modbusData[PINCH_2WPVA-3][0] & 0x0F) != MODBUS_PINCH_RIGHT_OPEN)
			setPinchPosValue(PINCH_2WPVA,MODBUS_PINCH_RIGHT_OPEN);

		if( (modbusData[PINCH_2WPVV-3][0] & 0x0F) != MODBUS_PINCH_RIGHT_OPEN && GetTherapyType() == LiverTreat)
			setPinchPosValue(PINCH_2WPVV,MODBUS_PINCH_RIGHT_OPEN);
		}
		/*faccio ripartire tutte le pompe perch� la rimozione d'aria la faccio su
		 * tutte le linee indipendentemente dal sensore che ha rilevato aria*/

		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, AIR_REJECT_SPEED); // pompa frontale sinistra per kidney e destra per liver

		if ( TherType == LiverTreat ||
		    (TherType == KidneyTreat && (PARAMETER_ACTIVE_TYPE)parameterWordSetFromGUI[PAR_SET_DEPURATION_ACTIVE].value == YES) )
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, AIR_REJECT_SPEED);	//pompe OXY

		/*la pompa frontale destra nel kidney ha indirizzo 3 e la faccio mentre nel liver 0
		 * questa pompa viene usata solo per se il trattamento � il liver*/
		if(TherType == LiverTreat)
			setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, AIR_REJECT_SPEED); // pompa frontale sinistra er liver e non usata per Kidney

		AirAlarmRecoveryState = AIR_CHANGE_START_TIME;
		StarDelay = timerCounterModBus;

		break;

	case AIR_CHANGE_START_TIME:
/*originale*/
//		if((recoveryParentState == PARENT_TREAT_KIDNEY_1_AIR_FILT) && (Air_1_Status == LIQUID))
//		{
//			//  la bolla se ne e' andata faccio; ripartire il timer per misurare il tempo a partire da ora
//			startTimeToRecovery = timerCounterModBus;
//			AirAlarmRecoveryState = STOP_AIR_PUMP;
//		}
//		else if((recoveryParentState == PARENT_TREAT_KIDNEY_1_SFV) && (sensor_UFLOW[VENOUS_AIR_SENSOR].bubbleSize < 25))
//		{
//			//  la bolla se ne e' andata faccio; ripartire il timer per misurare il tempo a partire da ora
//			startTimeToRecovery = timerCounterModBus;
//			AirAlarmRecoveryState = STOP_AIR_PUMP;
//		}
//		else if((recoveryParentState == PARENT_TREAT_KIDNEY_1_SFA) && (sensor_UFLOW[ARTERIOUS_AIR_SENSOR].bubbleSize < 25))
//		{
//			//  la bolla se ne e' andata faccio; ripartire il timer per misurare il tempo a partire da ora
//			startTimeToRecovery = timerCounterModBus;
//			AirAlarmRecoveryState = STOP_AIR_PUMP;
//		}
//		else
//		{
//			// Dopo un timeout esco comunque e vado avanti dopo aver segnalato alla gui il fatto
//			if(StarDelay && ((msTick_elapsed(StarDelay) * 50L) >= 10000L))
//			{
//				// sono passati 10 secondi la bolla d'aria non si e' ancora spostata
//				// forzo il passaggio allo stato successivo.
//				startTimeToRecovery = timerCounterModBus;
//				AirAlarmRecoveryState = STOP_AIR_PUMP;
//				TimeoutAirEjection = 1;
//			}
//		}
/*end*/
		/*se no vedo pi� aria su tutti e tre i sensori cambio stato e faccio partire il timer per misurare il tempo a partire da ora*/
		if (Air_1_Status == LIQUID && (sensor_UFLOW[VENOUS_AIR_SENSOR].bubbleSize < MAX_BUBBLE_SIZE) && (sensor_UFLOW[VENOUS_AIR_SENSOR].bubbleSize < MAX_BUBBLE_SIZE) )
		{
			startTimeToRecovery = timerCounterModBus;
			AirAlarmRecoveryState = STOP_AIR_PUMP;
		}
		// Dopo un timeout esco comunque e vado avanti dopo aver segnalato alla gui il fatto
		else if(StarDelay && ((msTick_elapsed(StarDelay) * 50L) >= 10000L))
		{
			// sono passati 10 secondi la bolla d'aria non si e' ancora spostata
			// forzo il passaggio allo stato successivo.
			startTimeToRecovery = timerCounterModBus;
			AirAlarmRecoveryState = STOP_AIR_PUMP;
			TimeoutAirEjection = 1;
		}
		break;
	case STOP_AIR_PUMP:
		if(startTimeToRecovery && ((msTick_elapsed(startTimeToRecovery) + totalTimeToRecovery) * 50L >= TimeRemaining))
		{
/*originale*/
//			// la pompa per l'espulsione dell'aria ha fatto 10 giri, che ritengo sufficienti per
//			// buttare fuori l'aria
//			if(recoveryParentState == PARENT_TREAT_KIDNEY_1_AIR_FILT)
//			{
//				if(TherType == KidneyTreat)
//					setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
//				else if(TherType == LiverTreat)
//					setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
//				AirAlarmRecoveryState = AIR_REJECTED;
//				//currentGuard[GUARD_AIR_RECOVERY_END].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
//				StarDelay = timerCounterModBus;
//			}
//			else if(recoveryParentState == PARENT_TREAT_KIDNEY_1_SFV)
//			{
//				// fermo la coppia di pompe per la venosa nel fegato e per l'ossigenazione nel rene
//				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
//				AirAlarmRecoveryState = AIR_REJECTED;
//				//currentGuard[GUARD_AIR_RECOVERY_END].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
//				StarDelay = timerCounterModBus;
//			}
//			else if(recoveryParentState == PARENT_TREAT_KIDNEY_1_SFA)
//			{
//				// fermo la pompa a cui si riferisce la struttura pumpPerist 0
//				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
//				AirAlarmRecoveryState = AIR_REJECTED;
//				//currentGuard[GUARD_AIR_RECOVERY_END].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
//				StarDelay = timerCounterModBus;
//			}
/*end*/
		    // le pompe per l'espulsione dell'aria hanno fatto 10 giri, che ritengo sufficienti per buttare fuori l'aria

			/*fermo tutte le pompe perch� la rimozione d'aria la faccio su
			 * tutte le linee indipendentemente dal sensore che ha rilevato aria*/
			setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0); // pompa forntale sinistra per kidney e destra per liver
			setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);	//pompe OXY
			/*la pompa frontale detsra nel kidney ha indirizzo 3 e la faccio mentre nel liver 0
			 * questa pompa viene usata solo per se il trattamento � il liver*/
			if(TherType == LiverTreat)
				setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0); // pompa frontale sinistra er liver e non usata per Kidney

			AirAlarmRecoveryState = AIR_REJECTED;
			StarDelay = timerCounterModBus;
		}
		break;
	case AIR_REJECTED:
		if(StarDelay && ((msTick_elapsed(StarDelay) * 50L) >= 2000))
		{
			AirAlarmRecoveryState = AIR_REJECTED1;
			currentGuard[GUARD_AIR_RECOVERY_END].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			AirRejectedStarted = FALSE;
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


// procedura per gestire la fase entry nello stato PARENT_TREAT_KIDNEY_1_END
void manageParentTreatEndEntry(void)
{}
// procedura per gestire la fase always nello stato PARENT_TREAT_KIDNEY_1_END
void manageParentTreatEndAlways(void)
{}

// funzione entry dello stato PARENT_TREAT_WAIT_PAUSE
void manageParentTreatWaitPauseEntry(void)
{
	// disabilito allarmi di temperatura arteriosa
	GlobalFlags.FlagsDef.EnableTempMaxMin = 0;
	GlobalFlags.FlagsDef.EnableTempArtOORAlm = 0;
    // Filippo - sospendo il PID in attesa di farlo partire
	FrigoHeatTempControlTaskNewPID((LIQ_TEMP_CONTR_TASK_CMD)TEMP_MANAGER_SUSPEND_CMD);

}

// funzione entry dello stato PARENT_TREAT_WAIT_START
void manageParentTreatWaitStartEntry(void)
{
	// disabilito allarmi di temperatura arteriosa
	GlobalFlags.FlagsDef.EnableTempMaxMin = 0;
	GlobalFlags.FlagsDef.EnableTempArtOORAlm = 0;
	// Filippo - sospendo il PID in attesa di farlo partire
	FrigoHeatTempControlTaskNewPID((LIQ_TEMP_CONTR_TASK_CMD)TEMP_MANAGER_SUSPEND_CMD);

}

// GESTIONE DELLO STATO PER IL RIPRISTINO DELLA TEMPERATURA DEL LIQUIDO-----------------------------------
void DeltaTHighAlarmRecoveryStateMach(void)
{
	static unsigned long StarDelay = 0;
	static int StarTimeToRestoreTemp;
	static DELTA_T_HIGH_ALM_RECVR_STATE LastDeltaTHgAlmRecvrState = INIT_TEMP_ALARM_RECOVERY;
	static int TimeRemaining = TIME_TO_RESTORE_TEMP;
	float TargetT;
	float deltaT;

	THERAPY_TYPE TherType = GetTherapyType();

	TargetT = (float)parameterWordSetFromGUI[PAR_SET_PRIMING_TEMPERATURE_PERFUSION].value / 10;  // (gradi Centigradi * 10)
	// differenza di temperatura tra quella impostata e quella letta dal sensore sulla linea arteriosa
	deltaT = TargetT - sensorIR_TM[0].tempSensValue;
	if(deltaT < 0)
		deltaT = -deltaT;

	if (buttonGUITreatment[BUTTON_START_TREATMENT].state == GUI_BUTTON_RELEASED)
	{
		releaseGUIButton(BUTTON_START_TREATMENT);
		if( DeltaTHighAlarmRecvrState == TEMP_RESTORE_STOPPED )
		{
			DeltaTHighAlarmRecvrState =	LastDeltaTHgAlmRecvrState;
			LastDeltaTHgAlmRecvrState = INIT_TEMP_ALARM_RECOVERY;
			StarTimeToRestoreTemp = timerCounterModBus;

			if(TherType == KidneyTreat)
			{
				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, TEMP_RESTORE_SPEED);
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, TEMP_RESTORE_SPEED);
			}
			else if(TherType == LiverTreat)
			{
				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, TEMP_RESTORE_SPEED);
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, TEMP_RESTORE_SPEED);
				setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, TEMP_RESTORE_SPEED);
			}
		}
	}
	else if(buttonGUITreatment[BUTTON_STOP_TREATMENT].state == GUI_BUTTON_RELEASED)
	{
		releaseGUIButton(BUTTON_STOP_TREATMENT);

		LastDeltaTHgAlmRecvrState = DeltaTHighAlarmRecvrState;
		DeltaTHighAlarmRecvrState = TEMP_RESTORE_STOPPED;
		TimeRemaining = TIME_TO_RESTORE_TEMP - msTick_elapsed(StarTimeToRestoreTemp) * 50L;
		if(TimeRemaining > TIME_TO_RESTORE_TEMP)
			TimeRemaining = TIME_TO_RESTORE_TEMP;
		else if(TimeRemaining < 0)
			TimeRemaining = TIME_TO_RESTORE_TEMP;

		if(TherType == KidneyTreat)
		{
			setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
			setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
		}
		else if(TherType == LiverTreat)
		{
			setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
			setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
			setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
		}
	}

	switch (DeltaTHighAlarmRecvrState)
	{
		case INIT_TEMP_ALARM_RECOVERY:
			DeltaTHighAlarmRecvrState = START_TEMP_PUMP;
			break;
		case START_TEMP_PUMP:
			if(TherType == KidneyTreat)
			{
				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, TEMP_RESTORE_SPEED);
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, TEMP_RESTORE_SPEED);
			}
			else if(TherType == LiverTreat)
			{
				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, TEMP_RESTORE_SPEED);
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, TEMP_RESTORE_SPEED);
				setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, TEMP_RESTORE_SPEED);
			}
			DeltaTHighAlarmRecvrState = TEMP_RESTORE_START_TIME;
			StarTimeToRestoreTemp = timerCounterModBus;
			TimeRemaining = TIME_TO_RESTORE_TEMP;
			break;
		case TEMP_RESTORE_START_TIME:
			if( (StarTimeToRestoreTemp && ((msTick_elapsed(StarTimeToRestoreTemp) * 50L) >= TimeRemaining) ) &&
			   (deltaT < ( (float)DELTA_T_ART_IF_OK - HYSTERESIS) )
			   )
			{
				// e' trascorso il tempo massimo riservato al processo di ripristino della temperatura
				// oppure la temperatura target e' stata raggiunta entro un certo range
				if(TherType == KidneyTreat)
				{
					setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
					setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
				}
				else if(TherType == LiverTreat)
				{
					setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
					setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
					setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
				}
				DeltaTHighAlarmRecvrState = TEMP_RESTORE_END;
				// riabilito allarme temperatura perche' prima di ritornare in trattamento devo riportare il liquido
				// in temperatura e segnalarlo con un ulteriore allarme
				StopAllCntrlAlarm(&DELTA_T_HIGH_RECV_gbf);
				EnableDeltaTHighAlmFunc();
				StarDelay = timerCounterModBus;
			}
			break;
		case TEMP_RESTORE_END:
			// questo delay serve a far si che l'interrupt di temperatura scatti di nuovo se ci sono
			// ancora le condizioni
			if(StarDelay && ((msTick_elapsed(StarDelay) * 50L) >= 2000))
			{
				DeltaTHighAlarmRecvrState = TEMP_RESTORE_END_1;
				currentGuard[GUARD_TEMP_RESTORE_END].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			}
			break;
		case TEMP_RESTORE_END_1:
			break;
		case TEMP_RESTORE_STOPPED:
			break;
	}
}

// faccio partire le pompe per la ricircolazione del liquido per
// riportarlo in temperatura
void manageParentTreatDeltaTHRcvEntry(void)
{
	DeltaTHighAlarmRecvrState = INIT_TEMP_ALARM_RECOVERY;
	FrigoHeatTempControlTaskNewPID(LIQ_T_CONTR_TASK_RESET_CMD);
}

void manageParentTreatDeltaTHRcvAlways(void)
{
	DeltaTHighAlarmRecoveryStateMach();
}


void manageParentTreatAlmDeltaTHRcvEntry(void)
{
	LevelBuzzer = HIGH;//2;
}

void manageParentTreatAlmDeltaTHRcvAlways(void)
{
	if(buttonGUITreatment[BUTTON_RESET_ALARM].state == GUI_BUTTON_RELEASED)
	{
		// volutamente non resetto l'evento di bottone premuto
		LevelBuzzer = SILENT;//0;
	}
}

uint8_t mngParTreatDeltaTHWtState;
void manageParentTreatDeltaTHWaitEntry(void)
{
	StarTimeToRetTeatFromRestTemp = timerCounterModBus;
	mngParTreatDeltaTHWtState = 0;
}

void manageParentTreatDeltaTHWaitAlways(void)
{
	switch (mngParTreatDeltaTHWtState)
	{
		case 0:
			if(StarTimeToRetTeatFromRestTemp && ((msTick_elapsed(StarTimeToRetTeatFromRestTemp) * 50L) >= 1000))
			{
				if(IsAlarmActive())
				{
					// se c'e' un allarme e' sicuramente quello di temperatura
					LevelBuzzer = HIGH;// 2;
					mngParTreatDeltaTHWtState = 1;
				}
				else
					currentGuard[GUARD_TEMP_RESTART_TREAT].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			}
			break;
		case 1:
			// aspetto che l'utente prema reset allarm
			if(buttonGUITreatment[BUTTON_RESET_ALARM].state == GUI_BUTTON_RELEASED)
			{
				releaseGUIButton(BUTTON_RESET_ALARM);
				DisableDeltaTHighAlmFunc();
				StarTimeToRetTeatFromRestTemp = timerCounterModBus;
				mngParTreatDeltaTHWtState = 2;
				LevelBuzzer = SILENT;//0;
				//EnableNextAlarmFunc();
			}
			break;
		case 2:
			// aspetto un po di tempo che l'allarme di temperatura venga tolto
			if(StarTimeToRetTeatFromRestTemp && ((msTick_elapsed(StarTimeToRetTeatFromRestTemp) * 50L) >= 1000))
			{
				RestoreAllCntrlAlarm(&DELTA_T_HIGH_RECV_gbf);
				DisableDeltaTHighAlmFunc();
				if(!IsAlarmActive())
				{
					currentGuard[GUARD_TEMP_NEW_RECOVERY].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
					mngParTreatDeltaTHWtState = 3;
				}
			}
			break;
		case 3:
			break;
	}
}

/*----------------------------------------FINE PARENT TREATMENTLEVEL FUNCTION -----------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/


/* CHILD LEVEL FUNCTION */

void manageChildNull(void)
{
	unsigned short dummy = 0;
}

void manageChildEntry(void)
{
}

void manageChildEntryAlways(void)
{
}

/* CORE FUNCTION */

/*--------------------------------------------------------*/
/*  This function compute the guard value in entry state   */
/*--------------------------------------------------------*/
static void computeMachineStateGuardEntryState(void){

	currentGuard[GUARD_ENABLE_T1_NO_DISP].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;

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
		//currentGuard[GUARD_ABANDON_PRIMING].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		SetAbandonGuard();
	}
	else
	{
		// faccio partire l'ossigenazione e la pompa di depurazione in liver
		if((GetTherapyType() == KidneyTreat) && (((PARAMETER_ACTIVE_TYPE)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_ACTIVE].value) == YES) &&
		   (perfusionParam.priVolPerfArt > MIN_LIQ_IN_RES_TO_START_OXY_VEN) && (StartOxygAndDepState == 0))
		{
			// sono nel priming rene con ossigenatore abilitato ed ho superato una quantita' minima nel reservoir
			setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress,KIDNEY_PRIMING_PMP_OXYG_SPEED);
					                  /*(int)((float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / OXYG_FLOW_TO_RPM_CONV * 100.0));*/
			StartOxygAndDepState = 1;
			// memorizzo la durata del priming prima che vengono fatte partire le pompe di ossigenazione
			PrimDurUntilOxyStart = (word)(TotalPrimingDuration + PrimingDuration);
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
			// memorizzo la durata del priming prima che vengono fatte partire le pompe di ossigenazione
			PrimDurUntilOxyStart = (word)(TotalPrimingDuration + PrimingDuration);
		}

		// controllo se e' cambiata la velocita' della pompa di ossigenazione e la aggiorno
		//CheckOxygenationSpeed(LastOxygenationSpeed);
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
		//currentGuard[GUARD_ABANDON_PRIMING].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		SetAbandonGuard();
	}
	else
	{
		// faccio partire l'ossigenazione e la pompa di depurazione in liver
		if((GetTherapyType() == KidneyTreat) && (((PARAMETER_ACTIVE_TYPE)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_ACTIVE].value) == YES) &&
		   (perfusionParam.priVolPerfArt > MIN_LIQ_IN_RES_TO_START_OXY_VEN) && (StartOxygAndDepState == 0))
		{
			// sono nel priming rene con ossigenatore abilitato ed ho superato una quantita' minima nel reservoir
			setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress,KIDNEY_PRIMING_PMP_OXYG_SPEED);
					                  /*(int)((float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / OXYG_FLOW_TO_RPM_CONV * 100.0));*/
			StartOxygAndDepState = 1;
			// memorizzo la durata del priming prima che vengono fatte partire le pompe di ossigenazione
			PrimDurUntilOxyStart = (word)(TotalPrimingDuration + PrimingDuration);
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
			// memorizzo la durata del priming prima che vengono fatte partire le pompe di ossigenazione
			PrimDurUntilOxyStart = (word)(TotalPrimingDuration + PrimingDuration);
		}
		// controllo se e' cambiata la velocita' della pompa di ossigenazione e la aggiorno
		//CheckOxygenationSpeed(LastOxygenationSpeed);
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
	{
		TreatDuration = msTick_elapsed(StartTreatmentTime) * 50L / 1000;
	/*VINCY-->SOLO PER DEBUG, se voglio accelerare la durata della terapia di 100 volte
	 * commento la riga sopra e decommento quella successiva*/
	//   TreatDuration = msTick_elapsed(StartTreatmentTime) * 50L / 10;
	}


	unsigned long ival = (unsigned long)(parameterWordSetFromGUI[PAR_SET_DESIRED_DURATION].value >> 8);
	unsigned long isec;
	isec = ival * 3600L;      // numero di ore
	ival = (unsigned long)(parameterWordSetFromGUI[PAR_SET_DESIRED_DURATION].value & 0xff);
	isec = isec + ival * 60L; // numero di minuti
	int newSpeedPmp_0 = 0;
	int newSpeedPmp1_2 = 0;
	int newSpeedPmp_3 = 0;

	/*Vincenzo: Gestisco l'abbandona trattamento anche nel caso in cui lo si faccia
	 * senza fare uno stop quindi per sicurezza, indipendentemente dal trattamento
	 * fermo tutte le pompe, poi andr� in svuotamento*/
	if(buttonGUITreatment[BUTTON_PRIMING_ABANDON].state == GUI_BUTTON_RELEASED)
	{
		releaseGUIButton(BUTTON_PRIMING_ABANDON);
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
		setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);

		SetAbandonGuard();
	}
	else if((TotalTreatDuration + TreatDuration) >= isec)
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
			// modificato il comando perche' ora devo andare in un'altro stato parent
			//currentGuard[GUARD_ENABLE_DISPOSABLE_EMPTY].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			ClearAlarmState();
			StopAllCntrlAlarm(&AlarmEnableConf);
			currentGuard[GUARD_ENABLE_WAIT_TREATMENT].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;

			// evito che mi venga fuori in continuazione l'allarme di temperatura se non e' stato
			// risolto durante il trattamento
			DisableDeltaTHighAlmFunc();

			if (PeltierOn && (peltierCell.StopEnable == 0))
			{
				// se erano accese le spengo
				peltierCell.StopEnable = 1;
				peltierCell2.StopEnable = 1;
			}
			// resetto anche il task di controllo per evitare che le peltier ripartano
			PeltierStarted = FALSE;
			LiquidTempContrTask(RESET_LIQUID_TEMP_CONTR_CMD);
			DebugStringStr("TREATMENT END");
		}
		else
		{
			// controllo il tempo trascorso per un eventuale timeout di errore
			if((TotalTreatDuration + TreatDuration) >= (isec + 5))
			{
				// se sono trascorsi 5 secondi dalla fine del trattamento e non sono ancora uscito.
				// la forzo io
				//currentGuard[GUARD_ENABLE_DISPOSABLE_EMPTY].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
				ClearAlarmState();
				StopAllCntrlAlarm(&AlarmEnableConf);
				currentGuard[GUARD_ENABLE_WAIT_TREATMENT].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;

				// evito che mi venga fuori in continuazione l'allarme di temperatura se non e' stato
				// risolto durante il trattamento
				DisableDeltaTHighAlmFunc();

				if (PeltierOn && (peltierCell.StopEnable == 0))
				{
					// se erano accese le spengo
					peltierCell.StopEnable = 1;
					peltierCell2.StopEnable = 1;
				}
				// resetto anche il task di controllo per evitare che le peltier ripartano
				PeltierStarted = FALSE;
				LiquidTempContrTask(RESET_LIQUID_TEMP_CONTR_CMD);
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
			if(isec > 0)
			{
				// controllo se e' cambiata la velocita' della pompa di ossigenazione e la aggiorno
				CheckOxygenationSpeed(LastOxygenationSpeed);
			}
		}
		else if(GetTherapyType() == LiverTreat)
		{
			if(isec > 0)
			{
				// aggiorno la velocita' della pompa di depurazione
				CheckDepurationSpeed(LastDepurationSpeed, FALSE, FALSE);
			}
		}

		/*controllo se mi arriva un comando per spostare la pinch Filter
		 * mentre la terapia sta andando*/

		if(buttonGUITreatment[BUTTON_PINCH_2WPVF_RIGHT_OPEN].state == GUI_BUTTON_RELEASED &&
				parameterWordSetFromGUI[PAR_SET_DEPURATION_ACTIVE].value == YES)
		{
			// pinch filter (pinch in basso aperto a destra)
			releaseGUIButton(BUTTON_PINCH_2WPVF_RIGHT_OPEN);
			HandlePinch(BUTTON_PINCH_2WPVF_RIGHT_OPEN);
		}
		if(buttonGUITreatment[BUTTON_PINCH_2WPVF_LEFT_OPEN].state == GUI_BUTTON_RELEASED &&
				parameterWordSetFromGUI[PAR_SET_DEPURATION_ACTIVE].value == YES)
		{
			// pinch filter (pinch in basso aperto a sinistra)
			releaseGUIButton(BUTTON_PINCH_2WPVF_LEFT_OPEN);
			HandlePinch(BUTTON_PINCH_2WPVF_LEFT_OPEN);
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

// In PARENT_EMPTY_DISPOSABLE_INIT e PARENT_EMPTY_DISPOSABLE_RUN sono attivi i bottoni BUTTON_START_EMPTY_DISPOSABLE
//                                                                                     BUTTON_STOP_EMPTY_DISPOSABLE
//												                                       BUTTON_PRIMING_ABANDON
// In PARENT_EMPTY_DISPOSABLE_ALARM e' attivo solo il tasto BUTTON_RESET
// In PARENT_EMPTY_DISPOSABLE_END non serve alcun tasto
// macchina a stati del processo Parent da usare nello stato STATE_EMPTY_DISPOSABLE
void ParentEmptyDispStateMach(void)
{
	switch(ptrCurrentParent->parent)
	{
	case PARENT_EMPTY_DISPOSABLE_INIT:
		if(VolumeDischarged >= 100)
		{
			// la seconda condizione e' necessaria se premo il tasto abbandona nella fase iniziale del priming
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
			EmptyWFLOATimeElapsed_Last = EmptyWFLOATimeElapsed;
			LevelBuzzer = HIGH;// 2;
		}
		break;

	case PARENT_EMPTY_DISPOSABLE_RUN:
//			if((VolumeDischarged >= (perfusionParam.priVolPerfArt + (float)10.0 * (float)perfusionParam.priVolPerfArt / 100.0)) &&
//			   (PR_LEVEL_mmHg_Filtered <= 0))
		if(((VolumeDischarged >= GetTotalPrimingVolumePerf((int)0)) /*|| (PR_LEVEL_mmHg_Filtered <= 0)*/) && IsDisposableEmpty())
		{
			// ho scaricato tutto, mi fermo
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
			EmptyWFLOATimeElapsed_Last = EmptyWFLOATimeElapsed;
			LevelBuzzer = HIGH;//2;
		}
		break;

	case PARENT_EMPTY_DISPOSABLE_ALARM:
		if((currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_FALSE) &&
		   (buttonGUITreatment[BUTTON_RESET_ALARM].state == GUI_BUTTON_RELEASED))
		{
			// Il ritorno allo svuotamento viene fatto solo dopo la pressione del tasto BUTTON_RESET_ALARM
			releaseGUIButton(BUTTON_RESET_ALARM);
			EnableNextAlarmFunc(); //EnableNextAlarm = TRUE;

			/* faccio ripartire le pompe per lo svuotamento
			 * a patto che non sia stato premuto il tasto di stop */
			if (emptyStopButtonPressed == FALSE)
				RestartPumpsEmptyState();

			ptrFutureParent = &stateParentEmptyDisp[3];
			ptrFutureChild = ptrFutureParent->ptrChild;
			LevelBuzzer = SILENT;//0;
		}
		else if (buttonGUITreatment[BUTTON_RESET_ALARM].state == GUI_BUTTON_RELEASED)
		{
			// Il ritorno allo svuotamento viene fatto solo dopo la pressione del tasto BUTTON_RESET_ALARM
			releaseGUIButton(BUTTON_RESET_ALARM);
			EnableNextAlarmFunc(); //EnableNextAlarm = TRUE;
			// faccio ripartire le pompe per lo svuotamento
			RestartPumpsEmptyState();
			ptrFutureParent = &stateParentEmptyDisp[3];
			ptrFutureChild = ptrFutureParent->ptrChild;
			LevelBuzzer = SILENT;//0;
			break;
		}

		if (ptrCurrentParent->action == ACTION_ON_ENTRY)
		{
			/* (FM) passo alla gestione ACTION_ALWAYS dell'allarme */
			ptrFutureParent = &stateParentEmptyDisp[6];
			ptrFutureChild = ptrFutureParent->ptrChild;
		}
		else if(ptrCurrentParent->action == ACTION_ALWAYS)
		{
			// chiamo la funzione child che gestisce lo stato di allarme durante la fase di svuotamento
			ManageStateChildAlarmEmpty();
		}
		break;

	case PARENT_EMPTY_DISPOSABLE_END:
		if(ptrCurrentParent->action == ACTION_ON_ENTRY)
		{
			/* compute future parent */
			/* FM passo alla gestione ACTION_ALWAYS */
			ptrFutureParent = &stateParentEmptyDisp[8];
			ptrFutureChild = ptrFutureParent->ptrChild;
		}
		else if(ptrCurrentParent->action == ACTION_ALWAYS)
		{
		}
		break;
	}
}

// gestisce il passaggio dallo stato di allarme ad un nuovo stato parent dove si
// cerchera' di eliminare la condizione di allarme.
void GoToRecoveryParentState(int MachineParentState)
{
	switch (MachineParentState)
	{
	case PARENT_TREAT_KIDNEY_1_SFA:
		recoveryParentState = PARENT_TREAT_KIDNEY_1_SFA;
		startTimeToRecovery = timerCounterModBus;
		totalTimeToRecovery = 0;
		/* (FM) passo nello stato per la risoluzione dell'allarme aria nella linea arteriosa*/
		ptrFutureParent = &stateParentTreatKidney1[11];
		ptrFutureChild = ptrFutureParent->ptrChild;
		break;
	case PARENT_TREAT_KIDNEY_1_SFV:
		recoveryParentState = PARENT_TREAT_KIDNEY_1_SFV;
		startTimeToRecovery = timerCounterModBus;
		totalTimeToRecovery = 0;
		/* (FM) passo nello stato per la risoluzione dell'allarme aria nella linea venosa*/
		ptrFutureParent = &stateParentTreatKidney1[9];
		ptrFutureChild = ptrFutureParent->ptrChild;
		break;
	case PARENT_TREAT_KIDNEY_1_AIR_FILT:
		recoveryParentState = PARENT_TREAT_KIDNEY_1_AIR_FILT;
		startTimeToRecovery = timerCounterModBus;
		totalTimeToRecovery = 0;
		/* (FM) passo nello stato per la risoluzione dell'allarme aria nella linea del filtro*/
		ptrFutureParent = &stateParentTreatKidney1[7];
		ptrFutureChild = ptrFutureParent->ptrChild;
		break;

	// vado nello stato di recupero della temperatura del liquido
	case PARENT_TREAT_KIDNEY_1_DELTA_T_HIGH_RECV:
		recoveryParentState = PARENT_TREAT_KIDNEY_1_DELTA_T_HIGH_RECV;
		startTimeToRecovery = timerCounterModBus;
		totalTimeToRecovery = 0;
		/* (FM) passo nello stato per la risoluzione dell'allarme di delta temperatura eccessivo*/
		ptrFutureParent = &stateParentTreatKidney1[21];
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

/*funzione che gestione la pressione del tasto mute a seguito di un allarme
 * se viene premuto, devo silenziare il buzzer per XXX secondi allo scadere dei
 * quali devo riattivare il buzzer; se per� prima dello scadere del timer compare
 * un nuovo allarme devo riattivare il buzzer */
void ManageMuteButton (void)
{
	static unsigned char State = 0;
	static int AlarmCode = 0;
	static unsigned long timeoutMute = 0;

	switch (State)
	{
	 	/*in questo stato controllo se � stato premuto il tasto 'MUTE'
	 	 * in tal caso silenzio il buzzer e cambio stato*/
		case 0:
	 	 {
	 		/*controllo se � stato premuto il tasto 'MUTE'*/
	 		if(buttonGUITreatment[BUTTON_SILENT_ALARM].state == GUI_BUTTON_RELEASED)
	 		{
	 			if(LevelBuzzer != SILENT)
	 				LevelBuzzer = SILENT;

	 			releaseGUIButton(BUTTON_SILENT_ALARM);
	 			/*tengo traccia di quale allarme � stato generato*/
	 			AlarmCode = GetCurrentAlarmActiveListAlm()->code;

	 			/*faccio partire il timer*/
	 			timeoutMute = FreeRunCnt10msec;

	 			/*cambio stato*/
	 			State = 1;
	 		}
	 		 break;
	 	 }
	 	 /*in questo stato controllo se
	 	  * -) non ci son pi� allarmi --> torno alo stato precednete
	 	  * -) C'� un allarme diverso da quello che mi ha fatt venire qui --> riattivo il buzzer e torno allo sttao precedente
	 	  * -) sono trascorsi DELAY_FOR_RESTART_BUZZER/100 secondi --> riattivo il buzzer e torno allo sttao precedente*/
		case 1:
		{
			/*Se non ci sono allrmi attivi perch� � stato fatto un reset torno allo stato precedente*/
			if (GetCurrentAlarmActiveListAlm()->code == 0)
			{
				State = 0;
				AlarmCode = 0;
			}
			/*altrimenti controllo se � arrivato un allarme diverso da quello che mi ha portato qui
			 * in tal caso riattivo il buzzer e torno nello stato precedente aggiornando AlarmCode*/
			else if(AlarmCode != GetCurrentAlarmActiveListAlm()->code)
			{
				LevelBuzzer = HIGH;
				State = 0;
				AlarmCode = GetCurrentAlarmActiveListAlm()->code;
			}
			/*altrimenti controllo se sono trascorsi DELAY_FOR_RESTART_BUZZER/100 secondi nel qual
			 * caso riattivo il buzzer e torno nello stato precedente azzerando AlarmCode*/
			else if (msTick10_elapsed(timeoutMute) >= DELAY_FOR_RESTART_BUZZER)
			{
				LevelBuzzer = HIGH;
				State = 0;
				AlarmCode = 0;
			}
			break;
		}
	 	 default:
		 	 break;
	}

}

// to debug state transitions
int IndexFutureState = 0;
int OldIndexFutureState = 99;

/*----------------------------------------------------------------------------*/
/* This function compute the machine state transition based on guard - state level         */
/*----------------------------------------------------------------------------*/
void SetPtrFutureState(int NFutureState ); // utility fun

void processMachineState(void)
{
	static unsigned short Oldstate = 0;


	if(ptrCurrentState->state != Oldstate)
	{
		// elimino un eventuale allarme pendente quando entro in un nuovo stato
		//ClearAlarmState();
		Oldstate = ptrCurrentState->state;
	}

	// Filippo - se arriva il comando di spegnimento del PC mi metto in idle a prescindere
	if (EMERGENCY_BUTTON_ACTIVATION_PC)
	{
		if (ptrCurrentState->state!=STATE_IDLE)
		{
			// mi porto in idle solo se non c'ero gi�
			SetPtrFutureState(3);
			DebugStringStr("enable state idle");
		}
	}

	// Filippo - messo per gestire lo shut down del PC da tasto
	if (buttonGUITreatment[BUTTON_SHUT_DOWN_PC].state==GUI_BUTTON_RELEASED)
	{
//		buttonGUITreatment[BUTTON_SHUT_DOWN_PC].state=GUI_BUTTON_PRESSED;
		if (ptrCurrentState->state!=STATE_IDLE)
		{
			// mi porto in idle solo se non c'ero gi�
			SetPtrFutureState(3);
			DebugStringStr("enable state idle");

		}
	}

	/* process state structure --> in base alla guard si decide lo stato --> in base allo stato si eseguono certe funzioni in modalit� init o always */
	switch(ptrCurrentState->state)
	{
	case STATE_NULL:
		/* compute future state */
		if(currentGuard[GUARD_START_ENABLE].guardValue == GUARD_VALUE_TRUE)
		{
			/* compute future state */
			SetPtrFutureState(1);
			break; //cambio stato NULL --> ENTRY
		}
		/* execute function state level */
		if(ptrCurrentState->action == ACTION_ON_ENTRY)
		{
			ptrCurrentState->callBackFunct();
			// (FM) faccio in modo che dopo un aprima inizializzazione esca subito e vada
			// nello stato STATE_ENTRY
			if( currentGuard[GUARD_START_ENABLE].guardValue != GUARD_VALUE_TRUE)
				currentGuard[GUARD_START_ENABLE].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		}

		/* execute function parent and child level */
		//NONE
		break;

	case STATE_ENTRY:
		/* compute future state */
		if(currentGuard[GUARD_T1_NO_DISP_OK].guardValue == GUARD_VALUE_TRUE)
		{
			/* (FM) VADO NELLO STATO IDLE,ACTION_ON_ENTRY DATO CHE LA FASE INIZIALE DI TEST E' FINITA */
			/* compute future state */
#ifdef T1_TEST_ENABLED
			SetPtrFutureState(7);
#else
			SetPtrFutureState(3);
#endif
			DebugStringStr("exit from entry");
			break; //cambio stato ENTRY --> T1_TEST_NO_DISP
		}

		/* compute parent......compute child */
		if(ptrCurrentParent->action == ACTION_ON_ENTRY)
		{
			/* execute parent callback function */
			// non serve qui verra' chiamata nella manageStateEntryAndStateAlways
			//ptrCurrentParent->callBackFunct();
			/* compute future parent */
			//ptrFutureParent = &stateParentEntry[2]; //non si pu� attribuire al ON_ENTRY una funzione ALWAYS
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
				//ptrFutureChild = &stateChildEntry[2]; //non si pu� attribuire al ON_ENTRY una funzione ALWAYS
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

	case STATE_T1_NO_DISPOSABLE:
		// Filippo - se arriva un comando di service vado in idle a meno che non sia gi� in allarme
		if (Service)
		{
			if (ptrCurrentParent->parent!=PARENT_T1_NO_DISP_ALARM)
			{
				// se sono in allarme non posso cambiare di stato
				currentGuard[GUARD_HW_T1T_DONE].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
				currentGuard[GUARD_COMM_ENABLED].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			}
		}

		if((currentGuard[GUARD_HW_T1T_DONE].guardValue == GUARD_VALUE_TRUE) &&
			(currentGuard[GUARD_COMM_ENABLED].guardValue == GUARD_VALUE_TRUE))
		{
			SetPtrFutureState(3);
			DebugStringStr("enable state idle");
		}

		manageStateEntryAndStateAlways(8);
		break;

	case STATE_IDLE:
		/*Vincenzo: Potrei essere tornato inn  IDLE alla fine di un trattamento, quindi
		 * per sicurezza resetto i contatori del trattamento e il flagf che mi dice
		 * se ho fatto o meno la tara sui sensori di presisone dopo la connessione dell'organo*/

		/*volume di priming effettuato*/
		if (perfusionParam.priVolPerfArt != 0)
			perfusionParam.priVolPerfArt = 0;

		/*volume di svuotamento effettuato*/
		if (VolumeDischarged != 0)
			VolumeDischarged = 0;

//		/*durata totale effettuata del trattamento*/
//		if (TotalTreatDuration != 0)
//			TotalTreatDuration = 0;

		TARA_PRESS_DONE = FALSE;

		if (EMERGENCY_BUTTON_ACTIVATION_PC)
		{
			//Sono arrivato qui a causa della pressione del tasto di STOP
			//Fermo tutto, l'unica possibilit� � lo spegnimento macchina
			  EN_Clamp_Control(DISABLE);
			  EN_Motor_Control(DISABLE);
		}
		else
		{
			/* compute future state */
			if((currentGuard[GUARD_ENABLE_SELECT_TREAT_PAGE].guardValue == GUARD_VALUE_TRUE))
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
				SetPtrFutureState(9);
				/* compute future parent */
				DebugStringStr("STATE_MOUNTING_DISP");
			}
			/* execute function state level */
			// (FM) DOPO LA PRIMA VOLTA PASSA AUTOMATICAMENTE NELLO STATO IDLE,ACTION_ALWAYS
			manageStateEntryAndStateAlways(4);
		}

	//	DebugStringStr("reach idle state");
		break;

	case STATE_SELECT_TREAT:     // NON E' USATO CON LA GESTIONE DA SERVICE
		/* compute future state */
		if( (currentGuard[GUARD_ENABLE_MOUNT_DISPOSABLE].guardValue == GUARD_VALUE_TRUE) )
		{
			currentGuard[GUARD_ENABLE_MOUNT_DISPOSABLE].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
			/* (FM) E' ARRIVATO UN COMANDO CHE MI CHIEDE DI PASSARE ALLA FASE DI INSTALLAZIONE DEL DISPOSABLE */
			/* compute future state */
			SetPtrFutureState(9);
			DebugStringStr("STATE_MOUNTING_DISP");
		}

		/* execute function state level */
		manageStateEntryAndStateAlways(6);
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
				SetPtrFutureState(11);
				DebugStringStr("STATE_TANK_FILL");
				/*faccio la tara degli ADC del sensore di livello per sapere *
                 * quanti ADC ho con tubo connesso ma senza liquido inserito nel reservoire*/
				SogliaVaschettaVuotaADC = PR_LEVEL_ADC_Filtered;
			}

//TODO da aggiungere l'eventuale tasto di abbandona che porta in somontaggio e non in svuotamento
//			else if(currentGuard[GUARD_ABANDON_PRIMING].guardValue == GUARD_VALUE_TRUE)
//			{
//				currentGuard[GUARD_ABANDON_PRIMING].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
//				/* (VP) HO DECISO DI ABBANDONARE IL PRIMING senza che esso sia mai partito, sono ancora in montaggio, vado in smontaggio
				// se voglio andare in STATE_UNMOUNT_DISPOSABLE e staccare il disposable
//				ptrFutureState = &stateState[27];
//				/* compute future parent */
//				ptrFutureParent = ptrFutureState->ptrParent;
//				/* compute future child */
//				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
//				DebugStringStr("STATE_EMPTY_DISPOSABLE(ABBANDONA)");
//			}

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
				SetPtrFutureState(13);
				DebugStringStr("STATE_PRIMING_PH_1");
			}
			else if((currentGuard[GUARD_ENABLE_PRIMING_PHASE_2].guardValue == GUARD_VALUE_TRUE))
			{
				currentGuard[GUARD_ENABLE_PRIMING_PHASE_2].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				/* VADO DIRETTAMENTE ALLA FASE 2 DEL PRIMING PER POI ANDARE IN RICIRCOLO */
				/* compute future state */
				SetPtrFutureState(15);
				DebugStringStr("STATE_PRIMING_PH_2");
			}

			/* execute function state level */
			manageStateEntryAndStateAlways(12);
			break;

	case STATE_PRIMING_PH_1:
		if(currentGuard[GUARD_ENABLE_STATE_PRIMING_PH_1_WAIT].guardValue == GUARD_VALUE_TRUE)
		{
			// nello stato priming 1 aspetto che mi arrivi il comando di caricamento del filtro
			// quando mi arriva passo subito a priming 2 per completare il 5% di priming che mi rimane
			// le pompe sono ferme e vado nello stato di attesa che l'utente monti il filtro
			currentGuard[GUARD_ENABLE_STATE_PRIMING_PH_1_WAIT].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
			/* compute future state */
			SetPtrFutureState(39);
			DebugStringStr("PRIMING_PH_1_WAIT");
			break;
		}
		else if(currentGuard[GUARD_ABANDON_PRIMING].guardValue == GUARD_VALUE_TRUE)
		{
			currentGuard[GUARD_ABANDON_PRIMING].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
			/* (FM) HO DECISO DI ABBANDONARE IL PRIMING, VADO NELLO STATO DI SVUOTAMENTO */
			/* compute future state */
			SetPtrFutureState(19);
			DebugStringStr("STATE_EMPTY_DISPOSABLE(ABBANDONA)");
		}
		/* execute function state level */
		manageStateEntryAndStateAlways(14);
		break;

	case STATE_PRIMING_PH_1_WAIT:
		// aspetto inserimento del filtro
		if( (currentGuard[GUARD_FILTER_INSTALLED].guardValue == GUARD_VALUE_TRUE) )
		{
			// il filtro e' stato montato passo alla seconda fase di priming
			currentGuard[GUARD_FILTER_INSTALLED].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
			/* (FM) FINITA LA FASE 1 DEL PRIMING POSSO PASSARE ALLA FASE 2 */
			/* compute future state */
			SetPtrFutureState(15);
			DebugStringStr("STATE_PRIMING_PH_2");
			break;
		}
		else if(currentGuard[GUARD_ABANDON_PRIMING].guardValue == GUARD_VALUE_TRUE)
		{
			currentGuard[GUARD_ABANDON_PRIMING].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
			/* (FM) HO DECISO DI ABBANDONARE IL PRIMING, VADO NELLO STATO DI SVUOTAMENTO */
			/* compute future state */
			SetPtrFutureState(19);
			DebugStringStr("STATE_EMPTY_DISPOSABLE(ABBANDONA)");
			break;
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
			SetPtrFutureState(21);
			DebugStringStr("STATE_PRIMING_WAIT");

			/*torno indietro nella macchina a stati quindi resetto la flag di entry sullo stato in cui sono*/
			currentGuard[GUARD_ENABLE_PRIMING_PHASE_2].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
			break;
		}
		else if(currentGuard[GUARD_ABANDON_PRIMING].guardValue == GUARD_VALUE_TRUE)
		{
			currentGuard[GUARD_ABANDON_PRIMING].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
			/* (FM) HO DECISO DI ABBANDONARE IL PRIMING, VADO NELLO STATO DI SVUOTAMENTO */
			/* compute future state */
			SetPtrFutureState(19);
			DebugStringStr("STATE_EMPTY_DISPOSABLE(ABBANDONA)");
			break;
		}
		/* execute function state level */
		manageStateEntryAndStateAlways(16);
		break;

	case STATE_TREATMENT_KIDNEY_1:
		if( currentGuard[GUARD_ENABLE_WAIT_TREATMENT].guardValue == GUARD_VALUE_TRUE )
		{
			// vado nello stato di attesa di un nuovo start trattamento o start svuotamento
			currentGuard[GUARD_ENABLE_WAIT_TREATMENT].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
			SetPtrFutureState(25);
			DebugStringStr("STATE_WAIT_TREATMENT");

			/*per poter tornare indietro dallo stato STATE_WAIT_TREATMENT allo stato STATE_TREATMENT_KIDNEY_1
			 * resetto la flag di entry sullo stato in cui sono*/
			currentGuard[GUARD_ENABLE_TREATMENT_KIDNEY_1].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
			currentGuard[GUARD_ENABLE_TREATMENT_KIDNEY_1].guardValue = GUARD_VALUE_FALSE;

			// disabilito allarme di pinch posizionate male perche' sto uscendo dal trattamento.
			// Il controllo sulle pinch posizionate correttamente viene fatto solo nello stato di trattamento
			DisableBadPinchPosAlmFunc();
			break;
		}
		else if(currentGuard[GUARD_ABANDON_PRIMING].guardValue == GUARD_VALUE_TRUE)
		{
			currentGuard[GUARD_ABANDON_PRIMING].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
			/* (FM) HO DECISO DI ABBANDONARE IL TRATTAMENTO, VADO NELLO STATO DI SVUOTAMENTO */
			/* compute future state */
			SetPtrFutureState(19);
			DebugStringStr("STATE_EMPTY_DISPOSABLE(ABBANDONA)");

			// disabilito allarme di pinch posizionate male perche' sto uscendo dal trattamento.
			// Il controllo sulle pinch posizionate correttamente viene fatto solo nello stato di trattamento
			DisableBadPinchPosAlmFunc();
			break;
		}

		/* execute function state level */
		manageStateEntryAndStateAlways(18);
		break;

	case STATE_PRIMING_WAIT:
		if( currentGuard[GUARD_PRIMING_END].guardValue == GUARD_VALUE_TRUE )
		{
			// ho terminato il riempimento del reservoir vado nello stato di attesa di raggiungimento della temperatura
			currentGuard[GUARD_PRIMING_END].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
			SetPtrFutureState(23);
			DebugStringStr("STATE_RICICLO");
			break;
		}
		else if( currentGuard[GUARD_ABANDON_PRIMING].guardValue == GUARD_VALUE_TRUE )
		{
			// abbandono il priming e VADO NELLO STATO DI SVUOTAMENTO
			currentGuard[GUARD_ABANDON_PRIMING].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
			SetPtrFutureState(19);
			ptrFutureState = &stateState[19];
			DebugStringStr("STATE_EMPTY_DISPOSABLE(ABBANDONA)");
			break;
		}
		else if( currentGuard[GUARD_ENABLE_PRIMING_PHASE_2].guardValue == GUARD_VALUE_TRUE )
		{
			// il volume e' stato modificato ritorno nella fase di riempimento
			currentGuard[GUARD_ENABLE_PRIMING_PHASE_2].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
			SetPtrFutureState(15);
			DebugStringStr("STATE_PRIMING_PHASE_2");

			/*torno indietro nella macchina a stati quindi resetto la flag di entry sullo stato in cui sono*/
			currentGuard[GUARD_ENABLE_PRIMING_WAIT].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
			break;
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
			SetPtrFutureState(17);
			DebugStringStr("STATE_TREATMENT");
			break;
		}
		else if( currentGuard[GUARD_ABANDON_PRIMING].guardValue == GUARD_VALUE_TRUE )
		{
			// abbandono il priming e VADO NELLO STATO DI SVUOTAMENTO
			currentGuard[GUARD_ABANDON_PRIMING].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
			SetPtrFutureState(19);
			DebugStringStr("STATE_EMPTY_DISPOSABLE(ABBANDONA)");
			break;
		}

		/* execute function state level */
		manageStateEntryAndStateAlways(24);
		break;
	case STATE_WAIT_TREATMENT:
		if( currentGuard[GUARD_ENABLE_DISPOSABLE_EMPTY].guardValue == GUARD_VALUE_TRUE )
		{
			currentGuard[GUARD_ENABLE_DISPOSABLE_EMPTY].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
			// se voglio andare in idle e selezionare un nuovo trattamento
			//ptrFutureState = &stateState[3];
			// se voglio andare nella procedura di svuotamento
			SetPtrFutureState(19);
			// ripristino gli enable azzerati alla fine del trattamento
			RestoreAllCntrlAlarm(&AlarmEnableConf);
			DebugStringStr("STATE_EMPTY_DISPOSABLE");
			break;
		}
		else if( currentGuard[GUARD_ENABLE_TREATMENT_KIDNEY_1].guardValue == GUARD_VALUE_TRUE )
		{
			// ritorno direttamente in trattamento per cominciarne uno nuovo
			currentGuard[GUARD_ENABLE_TREATMENT_KIDNEY_1].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
			SetPtrFutureState(17);
			ptrFutureState = &stateState[17];
			// ripristino gli enable azzerati alla fine del trattamento
			RestoreAllCntrlAlarm(&AlarmEnableConf);
			DebugStringStr("STATE_TREATMENT");

			// riparte come se fosse un nuovo trattamento
			StartTreatmentTime = 0;

//			//mantengo la durata che avevo fatto precedentemente commentando la riga seguente
//			TotalTreatDuration = 0;

			TreatDuration = 0;
			setGUIButton(BUTTON_START_TREATMENT);

			/*per poter tornare indietro dallo stato STATE_WAIT_TREATMENT allo stato STATE_TREATMENT_KIDNEY_1
			 * resetto la flag di entry sullo stato in cui sono*/
			currentGuard[GUARD_ENABLE_WAIT_TREATMENT].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
			currentGuard[GUARD_ENABLE_WAIT_TREATMENT].guardValue = GUARD_VALUE_FALSE;
			break;
		}
		/* execute function state level */
		manageStateEntryAndStateAlways(26);
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

		/*in svuotamento fermo la termica*/
		FrigoHeatTempControlTaskNewPID((LIQ_TEMP_CONTR_TASK_CMD)TEMP_MANAGER_STOPPED_CMD);

		if((currentGuard[GUARD_EMPTY_DISPOSABLE_END].guardValue == GUARD_VALUE_TRUE) ||
			(currentGuard[GUARD_ABANDON_EMPTY].guardValue == GUARD_VALUE_TRUE))
		{
			// se voglio andare in idle e selezionare un nuovo trattamento
			//ptrFutureState = &stateState[3];
			// se voglio andare in STATE_UNMOUNT_DISPOSABLE e staccare il disposable
			SetPtrFutureState(27);
			if(currentGuard[GUARD_ABANDON_EMPTY].guardValue == GUARD_VALUE_TRUE)
			{
				// abbandono il priming e VADO NELLO STATO DI STATE_UNMOUNT_DISPOSABLE per smontare il disposable
				currentGuard[GUARD_ABANDON_EMPTY].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				DebugStringStr("UNMOUNT_DISPOS(ABBANDONA)");
			}
			else
			{
				// ho terminato lo svuotamento del reservoir vado nello stato di smontaggio del disposable
				currentGuard[GUARD_EMPTY_DISPOSABLE_END].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				DebugStringStr("STATE_UNMOUNT_DISPOSABLE");
			}
			break;
		}

		/* execute function state level */
		manageStateEntryAndStateAlways(20);
		break;

	case STATE_UNMOUNT_DISPOSABLE:

		/*in smontaggio fermo la termica*/
		FrigoHeatTempControlTaskNewPID((LIQ_TEMP_CONTR_TASK_CMD)TEMP_MANAGER_STOPPED_CMD);

		if((currentGuard[GUARD_ENABLE_UNMOUNT_END].guardValue == GUARD_VALUE_TRUE) ||
			(currentGuard[GUARD_ABANDON_UNMOUNT].guardValue == GUARD_VALUE_TRUE))
		{
			SetPtrFutureState(3);
			ptrFutureState = &stateState[3];
			if(currentGuard[GUARD_ABANDON_UNMOUNT].guardValue == GUARD_VALUE_TRUE)
			{
				// abbandono il priming e VADO NELLO STATO DI SVUOTAMENTO
				currentGuard[GUARD_ABANDON_UNMOUNT].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				DebugStringStr("STATE_IDLE(ABBANDONA)");
			}
			else
			{
				// ho smontato il disposable ritorno in idle
				currentGuard[GUARD_ENABLE_UNMOUNT_END].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				DebugStringStr("STATE_IDLE");
			}
			break;
		}

		/* execute function state level */
		manageStateEntryAndStateAlways(28);
		break;

		case STATE_EMPTY_DISPOSABLE_1:
			/*in STATE_EMPTY_DISPOSABLE_1 fermo la termica*/
			FrigoHeatTempControlTaskNewPID((LIQ_TEMP_CONTR_TASK_CMD)TEMP_MANAGER_STOPPED_CMD);
			break;

		case STATE_EMPTY_DISPOSABLE_2:
			/*in STATE_EMPTY_DISPOSABLE_2 fermo la termica*/
			FrigoHeatTempControlTaskNewPID((LIQ_TEMP_CONTR_TASK_CMD)TEMP_MANAGER_STOPPED_CMD);
			break;

		case STATE_WASHING:
			/*in STATE_WASHING fermo la termica*/
			FrigoHeatTempControlTaskNewPID((LIQ_TEMP_CONTR_TASK_CMD)TEMP_MANAGER_STOPPED_CMD);
			break;

		case STATE_FATAL_ERROR:
			/*in STATE_FATAL_ERROR fermo la termica*/
			FrigoHeatTempControlTaskNewPID((LIQ_TEMP_CONTR_TASK_CMD)TEMP_MANAGER_STOPPED_CMD);
			break;

		default:
			break;
	}

	// chiamo la funzione che gestisce gli stati parent
	if(ptrCurrentState->state == STATE_T1_NO_DISPOSABLE) //chiamo la funzione parent solo quando serve
		ParentFuncT1Test();

	ParentFunc();

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
			//ptrCurrentChild->callBackFunct(); NON SERVE QUESTO
			ptrFutureChild = &stateChildAlarmPriming[2];
		}
		else if(ptrCurrentChild->action == ACTION_ALWAYS){
			/* (FM) esegue la parte ACTION_ALWAYS della gestione dell'allarme */
			//ptrCurrentChild->callBackFunct(); NON SERVE QUESTO
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
        else if(currentGuard[GUARD_ALARM_PUMPS_NOT_STILL].guardValue == GUARD_VALUE_TRUE)
        {
        	/* (FM) risolvo la situazione di allarme andando  a spegnere tutti gli attuatori */
            ptrFutureChild = &stateChildAlarmPriming[17];
        }
        else if(currentGuard[GUARD_ALARM_BAD_PINCH_POS].guardValue == GUARD_VALUE_TRUE)
        {
        	/* (FM) risolvo la situazione di allarme andando  a spegnere tutti gli attuatori */
            ptrFutureChild = &stateChildAlarmPriming[19];
        }
        else if(currentGuard[GUARD_ALARM_SFA_PRIM_AIR_DET].guardValue == GUARD_VALUE_TRUE)
        {
        	/* (FM) risolvo la situazione di allarme andando  a spegnere tutti gli attuatori */
            ptrFutureChild = &stateChildAlarmPriming[21];
        }
        else if(currentGuard[GUARD_ALARM_MOD_BUS_ERROR].guardValue == GUARD_VALUE_TRUE)
        {
        	/* (FM) risolvo la situazione di allarme andando  a spegnere tutti gli attuatori */
            ptrFutureChild = &stateChildAlarmPriming[23];
        }
        ResButInChildFlag = FALSE;
		break;

		case CHILD_PRIMING_ALARM_STOP_PERFUSION:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				//ptrCurrentChild->callBackFunct(); NON SERVE QUESTO

				ptrFutureChild = &stateChildAlarmPriming[4];
			}
            else if(currentGuard[GUARD_ALARM_STOP_PERF_PUMP].guardValue == GUARD_VALUE_FALSE )
                ptrFutureChild = &stateChildAlarmPriming[13]; /* (FM) allarme chiuso */
			else if(ptrCurrentChild->action == ACTION_ALWAYS){
				//ptrCurrentChild->callBackFunct(); NON SERVE QUESTO
			}
			break;

		case CHILD_PRIMING_ALARM_STOP_PURIFICATION:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				//ptrCurrentChild->callBackFunct(); NON SERVE QUESTO

				ptrFutureChild = &stateChildAlarmPriming[6];
			}
            else if((currentGuard[GUARD_ALARM_STOP_PURIF_PUMP].guardValue == GUARD_VALUE_FALSE) ||
                     (currentGuard[GUARD_ALARM_STOP_OXYG_PUMP].guardValue == GUARD_VALUE_FALSE) )
                ptrFutureChild = &stateChildAlarmPriming[13]; /* (FM) allarme chiuso */
			else if(ptrCurrentChild->action == ACTION_ALWAYS){
				//ptrCurrentChild->callBackFunct(); NON SERVE QUESTO
			}
			break;

		case CHILD_PRIMING_ALARM_STOP_ALL_PUMP:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				//ptrCurrentChild->callBackFunct(); NON SERVE QUESTO

				ptrFutureChild = &stateChildAlarmPriming[8];
			}
            else if(currentGuard[GUARD_ALARM_STOP_ALL].guardValue == GUARD_VALUE_FALSE )
                ptrFutureChild = &stateChildAlarmPriming[13]; /* FM allarme chiuso */
			else if(ptrCurrentChild->action == ACTION_ALWAYS){
				//ptrCurrentChild->callBackFunct(); NON SERVE QUESTO
			}
			break;

		case CHILD_PRIMING_ALARM_STOP_PELTIER:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				//ptrCurrentChild->callBackFunct(); NON SERVE QUESTO

				ptrFutureChild = &stateChildAlarmPriming[10];
			}
            else if(currentGuard[GUARD_ALARM_STOP_PELTIER].guardValue == GUARD_VALUE_FALSE )
                ptrFutureChild = &stateChildAlarmPriming[13]; /* FM allarme chiuso */
			else if(ptrCurrentChild->action == ACTION_ALWAYS){
				//ptrCurrentChild->callBackFunct(); NON SERVE QUESTO
			}
			break;

		case CHILD_PRIMING_ALARM_STOP_ALL_ACTUATOR:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				//ptrCurrentChild->callBackFunct(); NON SERVE QUESTO
				ptrFutureChild = &stateChildAlarmPriming[12];
			}
            else if( currentGuard[GUARD_ALARM_STOP_ALL_ACTUATOR].guardValue == GUARD_VALUE_FALSE )
                ptrFutureChild = &stateChildAlarmPriming[13]; /* FM allarme chiuso */
			else if(ptrCurrentChild->action == ACTION_ALWAYS){
				//ptrCurrentChild->callBackFunct(); NON SERVE QUESTO
			}
			break;

		case CHILD_PRIMING_ALARM_END:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				//ptrCurrentChild->callBackFunct(); NON SERVE QUESTO
				ptrFutureChild = &stateChildAlarmPriming[14];
			}
			else if(ptrCurrentChild->action == ACTION_ALWAYS){
                /* (FM) RIMANGO FERMO QUI FINO AL PROSSIMO ALLARME. NON E' NECESSARIO USCIRE DA QUESTO STATO DOPO IL TERMINE
                   DELLA CONDIZIONE DI ALLARME. LA CONDIZIONE DI ALLARME SUCCESSIVA MI FARA' RIPARTIRE DA CHILD_PRIMING_ALARM_INIT */
				//ptrCurrentChild->callBackFunct(); NON SERVE QUESTO
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

		case CHILD_PRIM_ALARM_PUMPS_NOT_STILL:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				ptrFutureChild = &stateChildAlarmPriming[18];
			}
            else if( currentGuard[GUARD_ALARM_PUMPS_NOT_STILL].guardValue == GUARD_VALUE_FALSE )
                ptrFutureChild = &stateChildAlarmPriming[13]; /* FM allarme chiuso */
			else if(ptrCurrentChild->action == ACTION_ALWAYS){}
			break;

		case CHILD_PRIM_ALARM_BAD_PINCH_POS:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				ptrFutureChild = &stateChildAlarmPriming[20];
			}
            else if( currentGuard[GUARD_ALARM_BAD_PINCH_POS].guardValue == GUARD_VALUE_FALSE )
                ptrFutureChild = &stateChildAlarmPriming[13]; /* FM allarme chiuso */
			else if(ptrCurrentChild->action == ACTION_ALWAYS){}
			break;

		case CHILD_PRIM_ALARM_SFA_AIR_DET:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				ptrFutureChild = &stateChildAlarmPriming[22];
			}
            else if( currentGuard[GUARD_ALARM_SFA_PRIM_AIR_DET].guardValue == GUARD_VALUE_FALSE )
                ptrFutureChild = &stateChildAlarmPriming[13]; /* FM allarme chiuso */
			else if(ptrCurrentChild->action == ACTION_ALWAYS){}
			break;

		case CHILD_PRIM_ALARM_MOD_BUS:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				ptrFutureChild = &stateChildAlarmPriming[24];
			}
            else if( currentGuard[GUARD_ALARM_MOD_BUS_ERROR].guardValue == GUARD_VALUE_FALSE )
                ptrFutureChild = &stateChildAlarmPriming[13]; /* FM allarme chiuso */
			else if(ptrCurrentChild->action == ACTION_ALWAYS){}
			break;
		// Filippo - messo per la gestione degli allarmi
		case CHILD_IDLE_ALARM:
            if(currentGuard[GUARD_ALARM_STOP_ALL_ACTUATOR].guardValue == GUARD_VALUE_TRUE)
            {
                ptrFutureChild = &stateChildAlarmIdle[2];
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

void SetPtrFutureState(int NFutureState )
{
	char StatStr[40];
	uint16_t sent_size;

	ptrFutureState = &stateState[NFutureState];
	/* compute future parent */
	ptrFutureParent = ptrFutureState->ptrParent;
	/* compute future child */
	ptrFutureChild = ptrFutureState->ptrParent->ptrChild;

	IndexFutureState = NFutureState;
	if(OldIndexFutureState != NFutureState)
	{
		PC_DEBUG_COMM_SendBlock("Change state index \r\n" , 21  , &sent_size);
		sprintf(StatStr , "\r%s  State transition  %d --> %d \r\n", GetRTCVal() , OldIndexFutureState, NFutureState);
		PC_DEBUG_COMM_SendBlock(StatStr , strlen(StatStr) , &sent_size); //SB 7-2020
		OldIndexFutureState = NFutureState;
	}
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


// Questa cosa deve essere fatta nelle funzioni always e non quando arriva il parametro
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

// Questa funzione deve essere chiamata solo quando sono nel trattamento fegato
// Questa cosa deve essere fatta nelle funzioni always e non quando arriva il parametro
void CheckDepurationSpeed(word value, bool ForceValue, bool DisableUpdateCmd)
{
	static bool EnableUpdate = FALSE;
	if (DisableUpdateCmd)
	{
		EnableUpdate = FALSE;
		return;
	}
    if(ForceValue)
	{
		setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, (int)((float)parameterWordSetFromGUI[PAR_SET_PURIF_FLOW_TARGET].value / DEFAULT_ART_PUMP_GAIN * 100.0));
		LastDepurationSpeed = parameterWordSetFromGUI[PAR_SET_PURIF_FLOW_TARGET].value;
        EnableUpdate = TRUE;
	}
	else if(EnableUpdate && (parameterWordSetFromGUI[PAR_SET_PURIF_FLOW_TARGET].value != value))
	{
		// ho ricevuto un valore del flusso di depurazione diverso dal precedente
		// devo aggiornare la velocita' della pompa
		setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, (int)((float)parameterWordSetFromGUI[PAR_SET_PURIF_FLOW_TARGET].value / DEFAULT_ART_PUMP_GAIN * 100.0));
		LastDepurationSpeed = parameterWordSetFromGUI[PAR_SET_PURIF_FLOW_TARGET].value;
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
// Filippo - tolto il commento alla funzione per il merge del 1-10-2018
//void Cover_Sensor_GetVal() {
//	unsigned char CoverM1, CoverM2, CoverM3, CoverM4, CoverM5;

//	CoverM1 = COVER_M1_GetVal();
//	CoverM2 = COVER_M2_GetVal();
//	CoverM3 = COVER_M3_GetVal();
//	CoverM4 = COVER_M4_GetVal();
//	CoverM5 = COVER_M5_GetVal();

//}


void Voltage_BM_Chk()
{
	unsigned char voltageBoard, voltageMotor;

	voltageBoard = VOLTAGE_B_CHK_GetVal();
	voltageMotor = VOLTAGE_M_CHK_GetVal();
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

	/*Se il CRC calcolato non � uguale a quello letto o la revsione non � uguale a quella attesa
	 * scrivo i parametri di default*/
	if ( config_data.EEPROM_CRC != Calc_CRC_EEPROM || config_data.EEPROM_Revision !=  EEPROM_REVISION)
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

		 config_data.T_Plate_Sensor_Gain_Heat         = GAIN_T_PLATE_SENS_HEAT;
		 config_data.T_Plate_Sensor_Offset_Heat       = OFFSET_T_PLATE_SENS_HEAT;

		 config_data.T_Plate_Sensor_Gain_Cold         = GAIN_T_PLATE_SENS_COLD;
		 config_data.T_Plate_Sensor_Offset_Cold       = OFFSET_T_PLATE_SENS_COLD;

		 config_data.FlowSensor_Ven_Gain              = GAIN_FLOW_SENS_VEN;
		 config_data.FlowSensor_Ven_Offset            = OFFSET_FLOW_SENS_VEN;

		 config_data.FlowSensor_Art_Gain              = GAIN_FLOW_SENS_ART;
		 config_data.FlowSensor_Art_Offset            = OFFSET_FLOW_SENS_ART;

		 //coeff per la calibrazione dei seosnri IR di temperatura
		 config_data.T_sensor_ART_Real_Low 			  = T_SESOR_ART_REAL_LOW_DEF;
		 config_data.T_sensor_ART_Real_Med			  = T_SESOR_ART_REAL_MED_DEF;
		 config_data.T_sensor_ART_Real_High			  = T_SESOR_ART_REAL_HIGH_DEF;
		 config_data.T_sensor_ART_Meas_Low 			  = T_SESOR_ART_MEAS_LOW_DEF;
		 config_data.T_sensor_ART_Meas_Med 			  = T_SESOR_ART_MEAS_MED_DEF;
		 config_data.T_sensor_ART_Meas_High			  = T_SESOR_ART_MEAS_HIGH_DEF;

		 config_data.T_sensor_RIC_Real_Low 			  = T_SESOR_RIC_REAL_LOW_DEF;
		 config_data.T_sensor_RIC_Real_Med 			  = T_SESOR_RIC_REAL_MED_DEF;
		 config_data.T_sensor_RIC_Real_High			  = T_SESOR_RIC_REAL_HIGH_DEF;
		 config_data.T_sensor_RIC_Meas_Low 			  = T_SESOR_RIC_MEAS_LOW_DEF;
		 config_data.T_sensor_RIC_Meas_Med 			  = T_SESOR_RIC_MEAS_MED_DEF;
		 config_data.T_sensor_RIC_Meas_High			  = T_SESOR_RIC_MEAS_HIGH_DEF;

		 config_data.T_sensor_VEN_Real_Low 			  = T_SESOR_VEN_REAL_LOW_DEF;
		 config_data.T_sensor_VEN_Real_Med 			  = T_SESOR_VEN_REAL_MED_DEF;
		 config_data.T_sensor_VEN_Real_High			  = T_SESOR_VEN_REAL_HIGH_DEF;
		 config_data.T_sensor_VEN_Meas_Low 			  = T_SESOR_VEN_MEAS_LOW_DEF;
		 config_data.T_sensor_VEN_Meas_Med 			  = T_SESOR_VEN_MEAS_MED_DEF;
		 config_data.T_sensor_VEN_Meas_High			  = T_SESOR_VEN_MEAS_HIGH_DEF;

		 //revsione della EEPROM
		 config_data.EEPROM_Revision 				  = EEPROM_REVISION;

		 /*carico il CRC della EEPROM (usata la stessa funzione di CRC del MOD_BUS
		  * IL CRC lo clacolo su tutta la struttura meno i due byte ndel CRC stesso*/
		 config_data.EEPROM_CRC = ComputeChecksum(ptr_EEPROM, sizeof(config_data)-2);

		 EEPROM_write((EEPROM_TDataAddress)&config_data, START_ADDRESS_EEPROM, sizeof(config_data));
	}

}

/*funzione che gestione il Button Emergency
 * se viene premuto per almeno TIMER_EMERGENCY_BUTTON * 50 msec,
 * impostiamo EMERGENCY_BUTTON_ACTIVATION = TRUE. Negli allarmi
 * gestiremo EMERGENCY_BUTTON_ACTIVATION e sempre negli allarmi,
 * se dovesse arrivare un reset dopo l'attivazione, metteremo
 * EMERGENCY_BUTTON_ACTIVATION = FALSE.
 * EMERGENCY_BUTTON_OUTPUT viene posto alto e EMERGENCY_BUTTON_INPUT in pull down
 * la pressione del bottone mette in corto i due pin quindi anche l'input sar� alto*/
void Manage_Emergency_Button(void)
{
	static unsigned char Status_Emergency_Button = 0;
	static unsigned long timer_button = 0;

	switch (Status_Emergency_Button)
	{
		case 0:
			if (!EMERGENCY_BUTTON_GetVal() )
			{
				timer_button = timerCounterModBus;
				Status_Emergency_Button = 1;
			}

			break;

		case 1:
			if (EMERGENCY_BUTTON_GetVal())
			{
				// Filippo - inserita la gestione dell'attivazione dell'allarme e dello spegnimento del PC
				if ((msTick_elapsed(timer_button)>=TIMER_EMERGENCY_BUTTON_ALARM) && (msTick_elapsed(timer_button)<TIMER_EMERGENCY_BUTTON) && (!EMERGENCY_BUTTON_ACTIVATION))
				{
					EMERGENCY_BUTTON_ACTIVATION=TRUE;
					Status_Emergency_Button = 0;
				}
				else
				{
					Status_Emergency_Button = 0;
					timer_button = timerCounterModBus;
				}
			}
			else if (msTick_elapsed(timer_button) >= TIMER_EMERGENCY_BUTTON)
				EMERGENCY_BUTTON_ACTIVATION_PC = TRUE;
			break;

		default:
			Status_Emergency_Button = 0;
			break;
	}
}



void SetAbandonGuard(void)
{
	currentGuard[GUARD_ABANDON_PRIMING].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
}

// Filippo - funzioni per aggiungere i test che servono al T1 test
/************************************************************************/
/*																		*/
/*				Funzioni per il T1 test									*/
/*																		*/
/************************************************************************/

void manageLevelSensorTest(void)
{
	// Funzione per il test del sensore di livello



}

// funzione per l'inizializzazione del test del riscaldatore
void manageParentT1HeaterInit(void)
{
	t1Test_heater=0;
	testT1HeatFridge=1;	// comunico che � partito il test heater
	timerCounterT1Test=0;
	protectiveOn=0;
}

// Funzione per la gestione del test riscaldatore
void manageParentT1Heater(void)
{
	LIQ_TEMP_CONTR_TASK_STATE ltcts;
	int tempPlateP = (int)getValTempPlateProt();
	int tempPlateC = (int)T_PLATE_C_GRADI_CENT;
	int deltaTempPlate = 0;

	switch (t1Test_heater)
	{
	case 0:
		// per prima cosa devo provare ad attivare il riscaldatore
		// (senza che la protective dia il consenso) e verificare che il riscaldatore non parta
		// Temperatura desiderata = PIASTRA STP + 5 gradi
		parameterWordSetFromGUI[PAR_SET_PRIMING_TEMPERATURE_PERFUSION].value=(word)((T_PLATE_C_GRADI_CENT + 5)*10);
		t1TestTempPartenza = T_PLATE_C_GRADI_CENT;	// Salvo la temperatura di partenza STP

		EnableHeating();	    // faccio partire il processo di riscaldamento
		t1Test_heater = 1;      // passo al prossimo stato
		timerCounterT1Test = 0; // reset timer test

		break;
	case 1:
		//Differenza tra STP (control) e STP2 (da protective)
		deltaTempPlate = (tempPlateP > tempPlateC) ? (tempPlateP - tempPlateC) : (tempPlateC - tempPlateP);

		//Check congruenza sensori di piastra control e protective
		if (
				(deltaTempPlate > 10) ||  //Al massimo STP e STP2 devono differire di 10 gradi
				(tempPlateP > 70)     ||  //e devono leggere valori ammissibili
				(tempPlateP < -15)    ||
				(tempPlateC > 70)     ||
				(tempPlateC < -15)
			){
			currentGuard[GUARD_ENABLE_T1_ALARM].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			DebugStringStr("alarm from chk heater: temperatura plate Prot e Ctrl");
			testT1HeatFridge = 0;
			protectiveOn = 0;
		}

		// Verifica che il processo di riscaldamento sia partito (lato CONTROL)
		if (IsHeating())
		{
			// OK, passo allo stato successivo
			t1Test_heater = 2;
			timerCounterT1Test = 0;
		}
		/* altrimenti aspetto 10 secondi prima di andare in allarme, per dare il tempo al controllo del riscaldatore di partire */
		else if (timerCounterT1Test > 200)
		{
			currentGuard[GUARD_ENABLE_T1_ALARM].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			DebugStringStr("alarm from chk heater, state 1");
			testT1HeatFridge = 0;
			protectiveOn = 0;
		}
		break;
	case 2:
		// Sono in uno stato in cui la CONTROL sta cercando di riscaldare la piastra ma senza consenso PROTECTIVE -->
		// la temperatura non deve crescere
		if (T_PLATE_C_GRADI_CENT >= t1TestTempPartenza + 3)
		{
			// Se la temperatura cresce di almeno 3 gradi --> allarme
			currentGuard[GUARD_ENABLE_T1_ALARM].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			DebugStringStr("alarm from chk heater, state 2");
			timerCounterT1Test = 0;
			DisableHeating();
			testT1HeatFridge = 0;
			protectiveOn = 0;
		}
		else
		{
			// Attendo 2 minuti
			if (timerCounterT1Test > 2400) //Se ho visto la temperatura NON crescere allora proseguo con il test
			{
				// Spengo il processo di riscaldamento
				DisableHeating();
				t1Test_heater = 3; //passo allo stato successivo
				timerCounterT1Test = 0;
				protectiveOn = 1; // adesso consento l'attivazione della protective
			}
		}

		break;
	case 3:
		//Stato del processo di riscaldamento?
		ltcts = FrigoHeatTempControlTaskNewPID((LIQ_TEMP_CONTR_TASK_CMD)READ_STATE_CMD);
		if (ltcts == LIQ_T_CONTR_HEATING_STOPPED)
		{
			// Se il processo di riscaldamento � stato fermato correttamente, proseguo con il test
			t1Test_heater = 4;
			t1TestTempPartenza = T_PLATE_C_GRADI_CENT;	// salvo la temperatura di partenza
			timerCounterT1Test = 0;                     // reset timer
		}
		/* altrimenti aspetto 10 secondi prima di andare in allarme, per dare il tempo al controllo del riscaldatore di fermarsi */
		else if (timerCounterT1Test > 200)
		{
			// non si � fermato - errore
			currentGuard[GUARD_ENABLE_T1_ALARM].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			DebugStringStr("alarm from chk heater, state 3");
			timerCounterT1Test = 0;
			testT1HeatFridge = 0;
			protectiveOn = 0;
		}

		break;
	case 4:
		//Verifica limite superiore
		if (T_PLATE_C_GRADI_CENT>=55)
		{
			// Se STP � maggiore di 55 gradi per 4 minuti --> allarme
			if (timerCounterT1Test >= 4800)
			{
				// la temperatura non � ancora scesa dopo vado in allarme
				currentGuard[GUARD_ENABLE_T1_ALARM].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
				DebugStringStr("alarm from chk heater, state 4");
				timerCounterT1Test = 0;
				testT1HeatFridge = 0;
				protectiveOn = 0;
			}
			break;
		}
		//Finch� la temperatura � maggiore di 55 gradi non proseguo

		// Ora facciamo partire veramente il riscaldatore,
		// imposto questo parametro per far partire l'algoritmo
		if (T_PLATE_C_GRADI_CENT > sensorIR_TM[1].tempSensValue)
		{
			//Se la piastra ha una temperatura superiore a quella misurata da STA
			//Target = PIASTRA + 5 gradi
			parameterWordSetFromGUI[PAR_SET_PRIMING_TEMPERATURE_PERFUSION].value=(word)((T_PLATE_C_GRADI_CENT + 5)*10);
			t1TestTempPartenza = T_PLATE_C_GRADI_CENT;	// salvo la temperatura di partenza
		}
		else
		{
			//Se la piastra ha una temperatura inferiore a quella misurata da STA
			//Target = STA + 5 gradi
			parameterWordSetFromGUI[PAR_SET_PRIMING_TEMPERATURE_PERFUSION].value=(word)((sensorIR_TM[1].tempSensValue + 5)*10);
			t1TestTempPartenza = sensorIR_TM[1].tempSensValue;	// salvo la temperatura di partenza
		}
		EnableHeating();	// faccio partire il riscaldatore
		t1Test_heater = 5;
		timerCounterT1Test = 0;

		break;
	case 5:
		// Verifica che il processo di riscaldamento sia partito (lato CONTROL)
		if (IsHeating())
		{
			// Il processo di riscaldamento � partito
			t1Test_heater = 6;
			timerCounterT1Test = 0;
		}
		/* altrimenti aspetto 10 secondi prima di andare in allarme, per dare il tempo al controllo del riscaldatore di partire */
		else if (timerCounterT1Test > 200) //10 sec
		{
			//Allarme: controllo del riscaldamento non partito
			currentGuard[GUARD_ENABLE_T1_ALARM].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			DebugStringStr("alarm from chk heater, state 5");
			testT1HeatFridge = 0;
			protectiveOn = 0;
		}
		break;
	case 6:
		// verifico che la temperatura effettivamente sia salita di almeno 3 gradi
		if (T_PLATE_C_GRADI_CENT >= t1TestTempPartenza + 3)
		{
			// ho visto salire la temperatura --> OK
			// Spengo il controllo del riscaldamento
			DisableHeating();
			t1Test_heater = 7;
			timerCounterT1Test = 0;
		}
		else
		{
			// Trascorre un minuto senza avere un incremento di almeno 3 gradi --> allarme
			if (timerCounterT1Test > 2400) //120 sec
			{
				currentGuard[GUARD_ENABLE_T1_ALARM].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
				DebugStringStr("alarm from chk heater, state 6");
				timerCounterT1Test = 0;
				DisableHeating();
				testT1HeatFridge = 0;
				protectiveOn = 0;
			}
		}
		break;
	case 7:
        //Leggo lo stato del controllo
		ltcts = FrigoHeatTempControlTaskNewPID((LIQ_TEMP_CONTR_TASK_CMD)READ_STATE_CMD);
		if (ltcts == LIQ_T_CONTR_HEATING_STOPPED)
		{
			// Se il controllo � in STOP proseguo con il test
			t1Test_heater = 8;
			t1TestTempPartenza = T_PLATE_C_GRADI_CENT;	// salvo la temperatura di partenza
			tempMaxHeatPlate = T_PLATE_C_GRADI_CENT;    // imposto il max
			timerCounterT1Test = 0;
		}
		/* altrimenti aspetto 10 secondi prima di andare in allarme, per dare il tempo al controllo del riscaldatore di fermarsi */
		else if (timerCounterT1Test > 200)
		{
			// non si � fermato - errore
			currentGuard[GUARD_ENABLE_T1_ALARM].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			DebugStringStr("alarm from chk heater, state 7");
			timerCounterT1Test = 0;
			testT1HeatFridge = 0;
			protectiveOn = 0;
		}
		break;
	case 8:
		// prendo il massimo di temperatura, per inerzia termica � possibile che il valore massimo tenda ancora a salire.
		// Allora monitoro la temperatura per 30 secondi
		if (T_PLATE_C_GRADI_CENT > tempMaxHeatPlate)
		{
			//Aggiorno il massimo, potrei avere anche una inerzia termica
			tempMaxHeatPlate = T_PLATE_C_GRADI_CENT;
		}

		//Attendo 30 secondi con heater spento, al termine avr� trovato il massimo o comunque un valore molto prossimo al massimo
		if (timerCounterT1Test > 600)
		{
			t1Test_heater = 81;
			timerCounterT1Test = 0;
		}
		break;

	case 81:
		// verifico che la temperatura non salga di due gradi
		if (T_PLATE_C_GRADI_CENT >= tempMaxHeatPlate + 2)
		{
			// se ci� avviene, vado in allarme
			currentGuard[GUARD_ENABLE_T1_ALARM].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			DebugStringStr("alarm from chk heater, state 81");
			timerCounterT1Test = 0;
			testT1HeatFridge = 0;
			protectiveOn = 0;
		}
		else
		{
			// Se dopo 90 secondi non ho visto salire la temperatura, considero il test OK
			if (timerCounterT1Test > 1800)
			{
				//Resetto il controllo
		      	FrigoHeatTempControlTaskNewPID((LIQ_TEMP_CONTR_TASK_CMD)LIQ_T_CONTR_TASK_RESET_CMD);
		      	t1Test_heater = 9;
			}
		}
		break;
	case 9:
		// il test � finito correttamente
		// ripristino il valore del parametro
		parameterWordSetFromGUI[PAR_SET_PRIMING_TEMPERATURE_PERFUSION].value=0;
		testT1HeatFridge = 0;
		protectiveOn = 0;
		break;

#if 0
	case 10:
		// caso farlocco in cui accendo il riscaldatore e vado oltre i 37 gradi
		parameterWordSetFromGUI[PAR_SET_PRIMING_TEMPERATURE_PERFUSION].value=450;
		t1TestTempPartenza=T_PLATE_C_GRADI_CENT;	// salvo la temperatura di partenza

		EnableHeating();	// faccio partire il riscaldatore
		t1Test_heater=11;
		timerCounterT1Test=0;

		break;
	case 11:
		if (IsHeating())
		{
			// ok il riscaldatore va
			t1Test_heater=12;
			timerCounterT1Test=0;
		}

		break;
	case 12:
		if (T_PLATE_C_GRADI_CENT>=40)
		{
			// ho visto salire la temperatura - fermo tutto
			DisableHeating();
			t1Test_heater=13;
			timerCounterT1Test=0;
		}

		break;
	case 13:
		// qui devo attendere che vada sopra i 38 gradi e poi torno al flusso normale del test
		// se dopo 2 minuti non lo fa torno lo stesso al flusso normale cos� evito tragici piantamenti
		if (T_PLATE_C_GRADI_CENT>38)
		{
			t1Test_heater=4;
			timerCounterT1Test=0;
		}

		if (timerCounterT1Test>=2400)
		{
			timerCounterT1Test=0;
			t1Test_heater=4;
		}
		break;
#endif
	default:

		break;
	}
}

// Funzione per l'inizializzazione del test del frigo
void manageParentT1FridgeInit(void)
{
	t1Test_Frigo=0;
	testT1HeatFridge=1;	// comunico che � partito il test frigo
	protectiveOn=0;
}

// Funzione per la gestione del test del frigo
void manageParentT1Fridge(void)
{
	LIQ_TEMP_CONTR_TASK_STATE ltcts;

	switch (t1Test_Frigo)
	{
	case 0:
		// per prima cosa provo ad attivare il frigo senza il consenso protective e verifico che la temperatura non scenda
		parameterWordSetFromGUI[PAR_SET_PRIMING_TEMPERATURE_PERFUSION].value=(word)((sensorIR_TM[1].tempSensValue - 7) * 10);
		t1TestTempPartenza = sensorIR_TM[1].tempSensValue;	// salvo la temperatura di partenza e prendo la temperatura STA in modo che la piastra vada sotto
		EnableFrigo();	// faccio partire il controllo del raffreddamento
		t1Test_Frigo = 1;
		timerCounterT1Test = 0;

		break;
	case 1:
		if (IsFrigo())
		{
			// ok, il controllo del raffreddamento � partito
			t1Test_Frigo = 2;
			timerCounterT1Test = 0;
		}
		/* altrimenti aspetto 10 secondi prima di andare in allarme, per dare il tempo al controllo del frigo di partire */
		else if (timerCounterT1Test > 100)
		{
			currentGuard[GUARD_ENABLE_T1_ALARM].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			DebugStringStr("alarm from chk fridge, state 1");
			testT1HeatFridge=0;
		}

		break;
	case 2:
		// adesso devo controllare che la temperatura non scenda
		if (T_PLATE_C_GRADI_CENT <= (t1TestTempPartenza - 3))
		{
			currentGuard[GUARD_ENABLE_T1_ALARM].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			DebugStringStr("alarm from chk fridge, state 2");
			timerCounterT1Test = 0;
			DisableFrigo();
			testT1HeatFridge = 0;
			protectiveOn = 0;
		}
		else
		{
			//Attesa 2 minuti, e se la temperatura non scende posso proseguire
			if (timerCounterT1Test > 2400)
			{
				DisableFrigo();
				StopFrigo();
				t1Test_Frigo=3;
				timerCounterT1Test=0;
			}
		}

		break;
	case 3:
		//Check controllo del frigo
		ltcts = FrigoHeatTempControlTaskNewPID((LIQ_TEMP_CONTR_TASK_CMD)READ_STATE_CMD);
		if (ltcts == LIQ_T_CONTR_FRIGO_STOPPED)
		{
			// Il controllo del raffreddamento si � fermato correttamente, proseguo
			t1Test_Frigo = 4;
			t1TestTempPartenza = T_PLATE_C_GRADI_CENT;	// salvo la temperatura di partenza della piastra
			timerCounterT1Test = 0;
			timerCounterT1TestFridge = 0;
			counterFridgeStability = 0;
			protectiveOn = 1;
		}
		/* altrimenti aspetto 10 secondi prima di andare in allarme, per dare il tempo al controllo del frigo di fermarsi */
		else if (timerCounterT1Test > 200)
		{
			// non si � fermato - errore
			currentGuard[GUARD_ENABLE_T1_ALARM].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			DebugStringStr("alarm from chk fridge, state 3");
			timerCounterT1Test = 0;
			timerCounterT1TestFridge = 0;
			testT1HeatFridge = 0;
		}
		break;

	case 4:
		// faccio partire il frigo (questa volta veramente)
		// imposto questo parametro per far partire l'algoritmo
		parameterWordSetFromGUI[PAR_SET_PRIMING_TEMPERATURE_PERFUSION].value=(word)((sensorIR_TM[1].tempSensValue-7)*10);
		t1TestTempPartenza=sensorIR_TM[1].tempSensValue;	// salvo la temperatura di partenza e prendo la temperatura ambiente in modo che la piastra vada sotto
		EnableFrigo();	// faccio partire il raffreddamento
		t1Test_Frigo = 5;
		timerCounterT1Test = 0;

		break;
	case 5:
		// verifico il controllo del raffreddamento � partito
		if (IsFrigo())
		{
			// ok, il controllo del raffreddamento � partito
			t1Test_Frigo = 6;
			timerCounterT1Test = 0;
		}
		/* altrimenti aspetto 10 secondi prima di andare in allarme, per dare il tempo al controllo del frigo di partire */
		else if (timerCounterT1Test > 200)
		{
			currentGuard[GUARD_ENABLE_T1_ALARM].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			DebugStringStr("alarm from chk fridge, state 5");
			testT1HeatFridge=0;
			protectiveOn=0;
		}

		break;
	case 6:
		// verifico che la temperatura scenda sotto la temperatura di partenza di 3 gradi
		if (T_PLATE_C_GRADI_CENT <= (t1TestTempPartenza-3))
		{
			// ho visto scendere la temperatura, proseguo con il test
			DisableFrigo();
			StopFrigo();
			t1Test_Frigo=7;
			timerCounterT1Test=0;
		}
		else
		{
			//Attendo al massimo 90 secondi
			if (timerCounterT1Test > 1800)
			{
				// se non scende la temperatura vado in allarme
				currentGuard[GUARD_ENABLE_T1_ALARM].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
				DebugStringStr("alarm from chk fridge, state 6");
				timerCounterT1Test=0;
				DisableFrigo();
				testT1HeatFridge=0;
				protectiveOn=0;
			}
		}

		break;
	case 7:
		//Verifico lo stato del controllo del raffreddamento: deve essere spento
		ltcts = FrigoHeatTempControlTaskNewPID((LIQ_TEMP_CONTR_TASK_CMD)READ_STATE_CMD);
		if (ltcts == LIQ_T_CONTR_FRIGO_STOPPED)
		{
			// Proseguo con il test
			t1Test_Frigo=8;
			t1TestTempPartenza=T_PLATE_C_GRADI_CENT;	// salvo la temperatura attuale
			timerCounterT1Test = 0;
			timerCounterT1TestFridge = 0;
			counterFridgeStability = 0;
			tempMinFridgePlate = T_PLATE_C_GRADI_CENT;
		}
		/* altrimenti aspetto 10 secondi prima di andare in allarme, per dare il tempo al controllo del frigo di fermarsi */
		else if (timerCounterT1Test > 200)
		{
			// se non scende la temperatura vado in allarme
			currentGuard[GUARD_ENABLE_T1_ALARM].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			DebugStringStr("alarm from chk fridge, state 7");
			timerCounterT1Test=0;
			DisableFrigo();
			testT1HeatFridge=0;
			protectiveOn=0;
		}

		break;

	case 8:
		// prendo il minimo di temperatura, per inerzia termica � possibile che il valore minimo tenda ancora a scendere.
		// Allora monitoro la temperatura per 60 secondi
		if (T_PLATE_C_GRADI_CENT < tempMinFridgePlate)
		{
			//Aggiorno il minimo, potrei avere anche una inerzia termica
			tempMinFridgePlate = T_PLATE_C_GRADI_CENT;
		}

		//Attendo 60 secondi con frigo spento, se il frigo si � arrestato correttamente al termine avr� trovato il minimo o comunque un valore molto prossimo al minimo
		if (timerCounterT1Test > 1200)
		{
			t1Test_Frigo = 81;
			timerCounterT1Test = 0;
		}
		break;

	case 81:
		// verifico che la temperatura non scenda di altri tre gradi
		if (T_PLATE_C_GRADI_CENT <= (tempMinFridgePlate - 3))
		{
			// se ci� avviene, vado in allarme
			currentGuard[GUARD_ENABLE_T1_ALARM].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			DebugStringStr("alarm from chk fridge, state 81");
			timerCounterT1Test=0;
			DisableFrigo();
			testT1HeatFridge=0;
			protectiveOn=0;
		}
		else
		{
			// Se dopo 90 secondi non ho visto scendere la temperatura, considero il test OK
			if (timerCounterT1Test > 1800)
			{
				//Resetto il controllo
		      	FrigoHeatTempControlTaskNewPID((LIQ_TEMP_CONTR_TASK_CMD)LIQ_T_CONTR_TASK_RESET_CMD);
		      	t1Test_Frigo = 9;
			}
		}
		break;
	case 9:
		// test terminato con successo
		// ripristino il valore del parametro
		parameterWordSetFromGUI[PAR_SET_PRIMING_TEMPERATURE_PERFUSION].value=0;
		testT1HeatFridge=0;
		protectiveOn=0;

		break;
	default:
		t1Test_Frigo=0;
		break;
	}
}

// Filippo - funzione di inizializzazione del test aria
void manageParentAirInit(void)
{
	t1TestAir=0;

}

void manageParentAir(void)
{
	// in questa funzione devo verificare il funzionamento del sensore aria
	switch (t1TestAir)
	{
	case 0:
		// vado a vedere se il sensore di aria ha un valore corretto
		if (Air_1_Status==LIQUID)
		{
			// all'inizio non deve essere liquido
			currentGuard[GUARD_ENABLE_T1_ALARM].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			DebugStringStr("alarm from chk air");
			timerCounterT1Test=0;
		}
		else
		{
			// ok il test dell'aria pu� essere fatto
			t1TestAir=1;
		}
		break;
	case 1:
		// attivo il pin di test e poi vado a vedere se cambia stato oppure no
		AIR_T_1_SetVal();
		AIR_T_2_SetVal();
		AIR_T_3_SetVal();
		t1TestAir=2;
		break;
	case 2:
		if (Air_1_Status==LIQUID)
		{
			// ha cambiato stato - ok
			t1TestAir=3;
		}
		else
		{
			// non ha cambiato stato errore
			currentGuard[GUARD_ENABLE_T1_ALARM].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			DebugStringStr("alarm from chk air");
			timerCounterT1Test=0;
			AIR_T_1_ClrVal();
			AIR_T_2_ClrVal();
			AIR_T_3_ClrVal();
		}

		break;
	case 3:
		AIR_T_1_ClrVal();
		AIR_T_2_ClrVal();
		AIR_T_3_ClrVal();
		break;
	}
}

static unsigned char contaErroriTempPlate=0;

// Filippo - devo verificare che le temperature piatto lette dalla control e dalla protective siano le stesse
void verificaTempPlate(void)
{
	// in questa funzione verifico che la temperatura piatto letta dalla control e la temperatura piatto spedita dalla protective non
	// abbiano una differenza superiore a 2 (TBD). Per evitare problemi di sincronizzazione tra la lettura della control e la spedizione
	// della protective l'errore deve avvenire per 3 volte consecutive
	float tempP;

	tempP=getValTempPlateProt();
	tempP-=T_PLATE_C_GRADI_CENT;
	if (tempP<0)
	{
		tempP=-tempP;
	}

	if (tempP>=2)
	{
		contaErroriTempPlate++;
	}
	else
	{
		contaErroriTempPlate=0;
	}

	if (contaErroriTempPlate>3)
	{
		// troppi errori consecutivi - allarme
		contaErroriTempPlate=3;		// evito che la variabile rolli in caso di numero eccessivo di errori




	}
}

/*Vincenzo gestione T0 TEST*/
// Filippo - funzione per eseguire il test del sensore aria T0 TEST
void airSensorTest(void)
{
	unsigned int conta=0;//, conta_debug = 0;

	if (timerCounterTestAirSensor>=200)
	{
		char status = 0xFF;

		// il test lo faccio ogni 10 secondi
		timerCounterTestAirSensor=0;
		if (Air_1_Status == LIQUID)
		{
			// il test lo faccio se all'inizio c'� del liquido
			//AIR_T_1_SetVal(); 	// not connected
			//AIR_T_2_SetVal(); 	//not connected
			AIR_T_3_SetVal();

//			while ((!AIR_SENSOR_GetVal()) && (conta<30000))
//			{
//				status = AIR_SENSOR_GetVal();
//				conta++;
//			}

			while (conta < 30000)
			{
				conta++;
				if (AIR_SENSOR_GetVal())
					break;//conta_debug++;
			}

			status = AIR_SENSOR_GetVal();

			if (conta>=30000)
			{
				// non ritorna normale allarme!!!
				airSensorTestKO=TRUE;
			//	AIR_T_1_ClrVal();
			//	AIR_T_2_ClrVal();
				AIR_T_3_ClrVal();
				return;
			}
			else
			{
				airSensorTestKO=FALSE;
			}

			// visto aria adesso tolgo il pin di test
			AIR_T_1_ClrVal();
			AIR_T_2_ClrVal();
			AIR_T_3_ClrVal();

			conta=0;

			while ((AIR_SENSOR_GetVal()) && (conta<30000))
			{
				conta++;
			}

			if (conta>=30000)
			{
				// non ritorna normale allarme!!!
				airSensorTestKO=TRUE;
			}
			else
			{
				airSensorTestKO=FALSE;
			}
		}
	}
}

void Manage_Frontal_Cover(void)
{
	if (FRONTAL_COVER_1_GetVal())
		FRONTAL_COVER_1_STATUS = TRUE; // cover left opened
	else
		FRONTAL_COVER_1_STATUS = FALSE; // cover left closed

	if (FRONTAL_COVER_2_GetVal())
		FRONTAL_COVER_2_STATUS = TRUE; // cover right opened
	else
		FRONTAL_COVER_2_STATUS = FALSE; // cover right closed

}

void Manage_Hook_Sensors(void)
{
	if (HOOK_SENSOR_1_GetVal())
		HOOK_SENSOR_1_STATUS = TRUE; // vaschetta a destra non inserita
	else
		HOOK_SENSOR_1_STATUS = FALSE; // vaschetta a destra inserita

	if (HOOK_SENSOR_2_GetVal())
		HOOK_SENSOR_2_STATUS = TRUE; // vaschetta a sinistra non inserita
	else
		HOOK_SENSOR_2_STATUS = FALSE; // vaschetta a sinistra inserita
}


