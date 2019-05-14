
#include "PE_Types.h"
#include "Global.h"

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
#include "statesStructs.h"
#include "Alarm_Con.h"

extern struct machineChild stateChildAlarmEmpty[];
extern struct machineChild stateChildAlarmTreat1[];
extern bool ResButInChildFlag;

//-////////////////////////////////////////////////////////////////////////////////////////////////
/* --------------------------------------------------------------------------------------------
 * CHILD LEVEL FUNCTION FOR PRIMING 1 e 2
 * Per ora le funzioni per la sicurezza nel caso di allarme durante il trattamento sono le
 * stesse del priming.
 * --------------------------------------------------------------------------------------------*/

/* Manage CHILD_PRIMING_ALARM_STOP_PERFUSION entry state */
void manageChildPrimAlarmStopPerfEntry(void)
{
	if(pumpPerist[0].dataReady == DATA_READY_FALSE)
	{
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
	}
	pumpPerist[0].entry = 0;
}

/* Manage CHILD_PRIMING_ALARM_STOP_PERFUSION always state */
void manageChildPrimAlarmStopPerfAlways(void)
{
	static int speed = 0;
	static int timerCopy = 0;

	if((pumpPerist[0].dataReady == DATA_READY_FALSE) && (speed != 0))
	{
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
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
}

/* Manage CHILD_PRIMING_ALARM_STOP_PURIFICATION entry state */
void manageChildPrimAlarmStopPurifEntry(void)
{
	if(pumpPerist[1].dataReady == DATA_READY_FALSE)
	{
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
	}
	pumpPerist[1].entry = 0;
}


/* Manage CHILD_PRIMING_ALARM_STOP_PURIFICATION always state */
void manageChildPrimAlarmStopPurifAlways(void)
{
	static int speed = 0;
	static int timerCopy = 0;

	if((pumpPerist[1].dataReady == DATA_READY_FALSE) && (speed != 0))
	{
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
	}

	if((timerCounterModBus%9) == 8)
	{
		if(timerCounterModBus != 0)
			timerCopy = timerCounterModBus;
		timerCounter = 0;

		readPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress);
	}

	if(pumpPerist[1].dataReady == DATA_READY_TRUE)
	{
		// la velocita' ora posso leggerla direttamente dall'array di registry modbus
		speed = modbusData[pumpPerist[1].pmpMySlaveAddress-2][17];
		pumpPerist[1].actualSpeed = speed;
		pumpPerist[1].dataReady = DATA_READY_FALSE;
	}
}


/* Manage CHILD_PRIMING_ALARM_STOP_ALL_PUMP entry state */
void manageChildPrimAlarmStopAllPumpEntry(void)
{
	if(pumpPerist[0].dataReady == DATA_READY_FALSE)
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
	if(pumpPerist[1].dataReady == DATA_READY_FALSE)
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
	// manca lo stop alla pompa 2 perche' e' legata alla pompa 1
	if((GetTherapyType() == LiverTreat) && (pumpPerist[3].dataReady == DATA_READY_FALSE))
		setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);

	pumpPerist[0].entry = 0;
	pumpPerist[1].entry = 0;
	pumpPerist[3].entry = 0;
}

/* Manage CHILD_PRIMING_ALARM_STOP_ALL_PUMP always state */
void manageChildPrimAlarmStopAllPumpAlways(void)
{
	static int speed0 = 0;
	static int speed1 = 0;
	static int speed3 = 0;
	static int timerCopy = 0;

	if((pumpPerist[0].dataReady == DATA_READY_FALSE) && (speed0 != 0))
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
	if((pumpPerist[1].dataReady == DATA_READY_FALSE) && (speed1 != 0))
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
	if((GetTherapyType() == LiverTreat) && (pumpPerist[3].dataReady == DATA_READY_FALSE) && (speed3 != 0))
		setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);

	if((timerCounterModBus%9) == 8)
	{
		if(timerCounterModBus != 0)
			timerCopy = timerCounterModBus;
		timerCounter = 0;

		readPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress);
		readPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress);
		if(GetTherapyType() == LiverTreat)
			readPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress);
	}

	if(pumpPerist[0].dataReady == DATA_READY_TRUE)
	{
		//speed = ((BYTES_TO_WORD_SIGN(msgToRecvFrame3[3], msgToRecvFrame3[4]))/100)*(timerCopy);
		// la velocita' ora posso leggerla direttamente dall'array di registry modbus
		speed0 = modbusData[pumpPerist[0].pmpMySlaveAddress-2][17];
		pumpPerist[0].actualSpeed = speed0;
		pumpPerist[0].dataReady = DATA_READY_FALSE;
	}
	if(pumpPerist[1].dataReady == DATA_READY_TRUE)
	{
		speed1 = modbusData[pumpPerist[1].pmpMySlaveAddress-2][17];
		pumpPerist[1].actualSpeed = speed1;
		pumpPerist[1].dataReady = DATA_READY_FALSE;
	}
	if((GetTherapyType() == LiverTreat) && pumpPerist[3].dataReady == DATA_READY_TRUE)
	{
		speed3 = modbusData[pumpPerist[3].pmpMySlaveAddress-2][17];
		pumpPerist[3].actualSpeed = speed3;
		pumpPerist[3].dataReady = DATA_READY_FALSE;
	}
}


/* Manage CHILD_PRIMING_ALARM_STOP_PELTIER entry state */
void manageChildPrimAlarmStopPeltEntry(void)
{
	stopPeltierActuator();
	stopPeltier2Actuator();
}

/* Manage CHILD_PRIMING_ALARM_STOP_PELTIER always state */
void manageChildPrimAlarmStopPeltAlways(void)
{
	stopPeltierActuator();
	stopPeltier2Actuator();
}

/* Manage CHILD_PRIMING_ALARM_STOP_ALL_ACTUATOR entry state */
void manageChildPrimAlarmStopAllActEntry(void)
{
	// chiamo la stessa funzione usata nello stato di trattamento, tanto le cose da fare
	// sono le stesse (per ora)
	manageChildTreatAlm1StopAllActEntry();
}

/* Manage CHILD_PRIMING_ALARM_STOP_ALL_ACTUATOR always state */
void manageChildPrimAlarmStopAllActAlways(void)
{
	// chiamo la stessa funzione usata nello stato di trattamento, tanto le cose da fare
	// sono le stesse (per ora)
	manageChildTreatAlm1StopAllActAlways();
}

//--------------------------------------------------------------------------------------------------
// La gestione dell'allarme che segue e' stata fatta il troppo pieno.
// Consiste nel disabilitare per sempre l'allarme senza aspettare che il segnale fisico
// vada a 0 e poi proseguire normalmente
/* Manage CHILD_PRIM_ALARM_1_WAIT_CMD entry state in priming*/
void manageChildPrimAlmAndWaitCmdEntry(void)
{
	// fermo le pompe e metto le pinch in sicurezza
	manageChildTreatAlm1StopAllActEntry();
	GlobalFlags.FlagsDef.ChildAlmAndWaitCmdActive = 1;
}

/* Manage CHILD_PRIM_ALARM_1_WAIT_CMD always state in priming */
void manageChildPrimAlmAndWaitCmdAlways(void)
{
	// apetto che tutte le pompe si siano fermate
	manageChildTreatAlm1StopAllActAlways();
	if(!ResButInChildFlag  && (buttonGUITreatment[BUTTON_RESET_ALARM].state == GUI_BUTTON_RELEASED) && IsSecurityStateActive())
	{
		ResButInChildFlag = TRUE;
		// Questa tipologia di allarmi deve essere forzata in off dal software prima di poter riprendere il lavoro
		ForceCurrentAlarmOff();
		// setto la guard per fare in modo che quando l'allarme risultera' non attivo
		// la macchina a stati parent riprenda il funzionamento normale
		currentGuard[GUARD_ALARM_WAIT_CMD_TO_EXIT].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		// ho raggiunto la condizione di sicurezza ed ho ricevuto un comando reset alarm
		releaseGUIButton(BUTTON_RESET_ALARM);
		EnableNextAlarmFunc(); //EnableNextAlarm = TRUE;
		LevelBuzzer = SILENT;//0;
	}
}
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
// L'allarme viene generato quando per un qualsiasi motivo non si riesce a fermare le pompe.
// In questo caso, lo stop viene forzato togliendo l'enable alle pompe
/* Manage CHILD_PRIM_ALARM_PUMPS_NOT_STILL entry state in priming*/
void manageChildPrimAlmPumpNotStillEntry(void)
{
	EN_Motor_Control(DISABLE);
}

void ClearPumpStopAlarm(void);
void manageChildPrimAlmPumpNotStillAlways(void)
{
	if(!ResButInChildFlag && (buttonGUITreatment[BUTTON_RESET_ALARM].state == GUI_BUTTON_RELEASED))
	{
		ResButInChildFlag = TRUE;
		EN_Motor_Control(ENABLE);
		ClearPumpStopAlarm();
		// in questo caso posso rilasciare il tasto BUTTON_RESET_ALARM perche' per uscire dallo stato
		// di allarme PARENT_PRIMING_END_RECIRC_ALARM controllo solo
		// currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_FALSE e non mi aspetto un comando
		// di reset
		releaseGUIButton(BUTTON_RESET_ALARM);
		EnableNextAlarmFunc(); //EnableNextAlarm = TRUE;
	}
	else if(buttonGUITreatment[BUTTON_OVERRIDE_ALARM].state == GUI_BUTTON_RELEASED)
	{
		EN_Motor_Control(ENABLE);
		ClearPumpStopAlarm();
		ForceCurrentAlarmOff();
		releaseGUIButton(BUTTON_OVERRIDE_ALARM);
		EnableNextAlarmFunc(); //EnableNextAlarm = TRUE;
	}
}
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
// L'allarme viene generato quando viene rilevata una posizione delle pinch sulla
// protective diversa da quella della control
// Per risolvere questo caso non e' possibile fare niente perche' non e' possibile per
// la control leggere la posizione reale delle pinch
/* Manage CHILD_PRIM_ALARM_BAD_PINCH_POS entry state in priming*/
void manageChildPrimAlmBadPinchPosEntry(void)
{
	manageChildTreatAlm1StopAllActEntry();
}

void ResetTreatSetPinchPosTaskAlm(void);
void manageChildPrimAlmBadPinchPosAlways(void)
{
	manageChildTreatAlm1StopAllActAlways();
	if(!ResButInChildFlag && (buttonGUITreatment[BUTTON_RESET_ALARM].state == GUI_BUTTON_RELEASED))
	{
		//ResButInChildFlag = TRUE;
		ResetPrimPinchAlm();
		// se l'allarme era stato generato da qualche problema quando il priming era gia' attivo (pompe in movimento)
		ResetTreatCurrPinchPosOk();

		// in questo caso posso rilasciare il tasto BUTTON_RESET_ALARM perche' per uscire dallo stato
		// di allarme PARENT_PRIMING_END_RECIRC_ALARM controllo solo
		// currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_FALSE e non mi aspetto un comando
		// di reset
		releaseGUIButton(BUTTON_RESET_ALARM);
		EnableNextAlarmFunc(); //EnableNextAlarm = TRUE;
	}
	else if(buttonGUITreatment[BUTTON_OVERRIDE_ALARM].state == GUI_BUTTON_RELEASED)
	{
		ResetPrimPinchAlm();
		// se l'allarme era stato generato da qualche problema quando il priming era gia' attivo (pompe in movimento)
		ResetTreatCurrPinchPosOk();
		ForceCurrentAlarmOff();
		releaseGUIButton(BUTTON_OVERRIDE_ALARM);
		EnableNextAlarmFunc(); //EnableNextAlarm = TRUE;
	}
}
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
// L'allarme viene generato quando viene rilevata aria nella linea del filtro e sono in priming
/* Manage CHILD_PRIM_ALARM_SFA_AIR_DET entry state in priming*/
//void manageChildPrimAlmSFAAirDetEntry(void)
//{
//	manageChildTreatAlm1StopAllActEntry();
//}
//
//void manageChildPrimAlmSFAAirDetAlways(void)
//{
//	manageChildTreatAlm1StopAllActAlways();
//}

void manageChildPrimAlmSFAAirDetEntry(void)
{
	// fermo le pompe e metto le pinch in sicurezza
	manageChildTreatAlm1StopAllActEntry();
	TreatAlm1SafAirFiltActive = TRUE;
}

/* Manage CHILD_PRIM_ALARM_SFA_AIR_DET always state */
void manageChildPrimAlmSFAAirDetAlways(void)
{
	// apetto che tutte le pompe si siano fermate
	manageChildTreatAlm1StopAllActAlways();
	DisablePrimAirAlarm(TRUE); // forzo la chiusura dell'allarme aria
	if(!ResButInChildFlag && (buttonGUITreatment[BUTTON_RESET_ALARM].state == GUI_BUTTON_RELEASED) && IsSecurityStateActive())
	{
		ResButInChildFlag = TRUE;
		// IN QUESTO PUNTO CI PASSA SOLO SE IL TASK CHE GESTISCE GLI ALLARMI NON RIESCE A DISABILITARLI PRIMA DELL'ARRIVO
		// DI BUTTON_RESET_ALARM (PROBABILMENTE NON CI PASSA MAI)
		// setto la guard per fare in modo che quando l'allarme risultera' non attivo
		// la macchina a stati parent vada nello stato di espulsione bolla aria
		currentGuard[GUARD_ALARM_PRIM_AIR_FILT_RECOVERY].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		// ho raggiunto la condizione di sicurezza ed ho ricevuto un comando reset alarm
		releaseGUIButton(BUTTON_RESET_ALARM);
		EnableNextAlarmFunc(); //EnableNextAlarm = TRUE;
		LevelBuzzer = SILENT;//0;
	}
}


/* Manage CHILD_PRIM_ALARM_MOD_BUS always state */
void manageChildPrimAlmModBusEntry(void)
{
	EN_Motor_Control(DISABLE);
}

void manageChildPrimAlmModBusAlways(void)
{
	if(!ResButInChildFlag && (buttonGUITreatment[BUTTON_RESET_ALARM].state == GUI_BUTTON_RELEASED))
	{
		ResButInChildFlag = TRUE;
		EN_Motor_Control(ENABLE);
		ClearModBusAlarm();
		// in questo caso evito di rilasciare il tasto BUTTON_RESET_ALARM perche' per uscire dallo stato
		// di allarme PARENT_PRIMING_TREAT_KIDNEY_1_ALARM controllo
		// currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_FALSE ma aspetto anche un comando
		// di reset
		//releaseGUIButton(BUTTON_RESET_ALARM);
		//EnableNextAlarmFunc(); //EnableNextAlarm = TRUE;
	}
	else if(buttonGUITreatment[BUTTON_OVERRIDE_ALARM].state == GUI_BUTTON_RELEASED)
	{
		EN_Motor_Control(ENABLE);
		ClearModBusAlarm();
		ForceCurrentAlarmOff();
		releaseGUIButton(BUTTON_OVERRIDE_ALARM);
		EnableNextAlarmFunc(); //EnableNextAlarm = TRUE;
	}

}

//--------------------------------------------------------------------------------------------------



//-////////////////////////////////////////////////////////////////////////////////////////////////
/* --------------------------------------------------------------------------------------------
 * CHILD LEVEL FUNCTION FOR TREATMENT_1 (KIDNEY)
 * Per ora le funzioni per la sicurezza nel caso di allarme durante il trattamento sono le
 * stesse del priming.
 * --------------------------------------------------------------------------------------------*/

/* Manage CHILD_TREAT_ALARM_1_INIT always state */
void manageChildTreatAlm1InitAlways(void)
{
	if(currentGuard[GUARD_ALARM_STOP_ALL_ACTUATOR].guardValue == GUARD_VALUE_TRUE)
	{
		/* (FM) risolvo la situazione di allarme andando a spegnere tutti gli attuatori */
		ptrFutureChild = &stateChildAlarmTreat1[11];
	}
	else if(currentGuard[GUARD_ALARM_STOP_ALL].guardValue == GUARD_VALUE_TRUE)
	{
		/* (FM) risolvo la situazione di allarme andando a spegnere tutti gli attuatori e pompe */
		ptrFutureChild = &stateChildAlarmTreat1[7];
	}
	else if(currentGuard[GUARD_ALARM_STOP_PERF_PUMP].guardValue == GUARD_VALUE_TRUE)
	{
		/* (FM) risolvo la situazione di allarme andando a spegnere la PERF PUMP */
		ptrFutureChild = &stateChildAlarmTreat1[3];
	}
	else if(currentGuard[GUARD_ALARM_STOP_PURIF_PUMP].guardValue == GUARD_VALUE_TRUE)
	{
		/* (FM) risolvo la situazione di allarme andando a spegnere la PURIF PUMP */
		ptrFutureChild = &stateChildAlarmTreat1[5];
	}
	else if(currentGuard[GUARD_ALARM_STOP_OXYG_PUMP].guardValue == GUARD_VALUE_TRUE)
	{
		/* (FM) risolvo la situazione di allarme andando a spegnere la OXYG PUMP */
		ptrFutureChild = &stateChildAlarmTreat1[5];
	}
	else if(currentGuard[GUARD_ALARM_STOP_PELTIER].guardValue == GUARD_VALUE_TRUE)
	{
		/* (FM) risolvo la situazione di allarme andando ad agire sulla cella di peltier */
		ptrFutureChild = &stateChildAlarmTreat1[9];
	}
    else if(currentGuard[GUARD_ALARM_STOP_ALL_ACT_WAIT_CMD].guardValue == GUARD_VALUE_TRUE)
    {
    	/* (FM) risolvo la situazione di allarme andando  a spegnere tutti gli attuatori */
        ptrFutureChild = &stateChildAlarmTreat1[21];
    }
    else if(currentGuard[GUARD_ALARM_BAD_PINCH_POS].guardValue == GUARD_VALUE_TRUE)
    {
    	/* (FM) risolvo la situazione di allarme andando  a spegnere tutti gli attuatori */
        ptrFutureChild = &stateChildAlarmTreat1[23];
    }
    else if(currentGuard[GUARD_ALARM_MOD_BUS_ERROR].guardValue == GUARD_VALUE_TRUE)
    {
    	/* (FM) risolvo la situazione di allarme andando  a togliere l'abilitazione alle pompe */
        ptrFutureChild = &stateChildAlarmTreat1[25];
    }
}


/* Manage CHILD_TREAT_ALARM_1_STOP_PERFUSION entry state */
void manageChildTreatAlm1StopPerfEntry(void)
{
	if(pumpPerist[0].dataReady == DATA_READY_FALSE)
	{
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
		if(getPumpPressLoop(0) == PRESS_LOOP_ON)
			setPumpPressLoop(0, PRESS_LOOP_OFF);
	}
	pumpPerist[0].entry = 0;
}

/* Manage CHILD_TREAT_ALARM_1_STOP_PERFUSION always state */
void manageChildTreatAlm1StopPerfAlways(void)
{
	static int speed = 0;
	static int timerCopy = 0;

	if((pumpPerist[0].dataReady == DATA_READY_FALSE) && (speed != 0))
	{
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
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
		speed = modbusData[pumpPerist[0].pmpMySlaveAddress-2][17];
		pumpPerist[0].actualSpeed = speed;
		pumpPerist[0].dataReady = DATA_READY_FALSE;
	}
}

/* Manage CHILD_TREAT_ALARM_1_STOP_PURIFICATION entry state */
void manageChildTreatAlm1StopPurifEntry(void)
{
	if(pumpPerist[1].dataReady == DATA_READY_FALSE)
	{
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
		if(getPumpPressLoop(1) == PRESS_LOOP_ON)
			setPumpPressLoop(1, PRESS_LOOP_OFF);
	}
	pumpPerist[1].entry = 0;
}


/* Manage CHILD_TREAT_ALARM_1_STOP_PURIFICATION always state */
void manageChildTreatAlm1StopPurifAlways(void)
{
	static int speed = 0;
	static int timerCopy = 0;

	if((pumpPerist[1].dataReady == DATA_READY_FALSE) && (speed != 0))
	{
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
	}

	if((timerCounterModBus%9) == 8)
	{
		if(timerCounterModBus != 0)
			timerCopy = timerCounterModBus;
		timerCounter = 0;

		readPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress);
	}

	if(pumpPerist[1].dataReady == DATA_READY_TRUE)
	{
		//speed = ((BYTES_TO_WORD_SIGN(msgToRecvFrame3[3], msgToRecvFrame3[4]))/100)*(timerCopy);
		// la velocita' ora posso leggerla direttamente dall'array di registry modbus
		speed = modbusData[pumpPerist[1].pmpMySlaveAddress-2][17];
		pumpPerist[1].actualSpeed = speed;
		pumpPerist[1].dataReady = DATA_READY_FALSE;
	}
}


/* Manage CHILD_TREAT_ALARM_1_STOP_ALL_PUMP entry state */
void manageChildTreatAlm1StopAllPumpEntry(void)
{
	if(pumpPerist[0].dataReady == DATA_READY_FALSE)
	{
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
		if(getPumpPressLoop(0) == PRESS_LOOP_ON)
			setPumpPressLoop(0, PRESS_LOOP_OFF);
	}
	if(pumpPerist[1].dataReady == DATA_READY_FALSE)
	{
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
		if(getPumpPressLoop(1) == PRESS_LOOP_ON)
			setPumpPressLoop(1, PRESS_LOOP_OFF);
	}

	/*setPumpSpeedValueHighLevel(pumpPerist[2].pmpMySlaveAddress, 0); viene comandata direttamente col comasndo di quella sopra*/

	if((GetTherapyType() == LiverTreat) && pumpPerist[3].dataReady == DATA_READY_FALSE)
		setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);

	pumpPerist[0].entry = 0;
	pumpPerist[1].entry = 0;
	pumpPerist[3].entry = 0;
}

/* Manage CHILD_TREAT_ALARM_1_STOP_ALL_PUMP always state */
void manageChildTreatAlm1StopAllPumpAlways(void)
{
	static int speed0 = 0;
	static int speed1 = 0;
	static int speed3 = 0;
	static int timerCopy = 0;

	if(pumpPerist[0].dataReady == DATA_READY_FALSE && (speed0 != 0))
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
	if(pumpPerist[1].dataReady == DATA_READY_FALSE && (speed1 != 0))
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);

	/*setPumpSpeedValueHighLevel(pumpPerist[2].pmpMySlaveAddress, 0); viene comandata direttamente col comasndo di quella sopra*/

	if((GetTherapyType() == LiverTreat) && (pumpPerist[3].dataReady == DATA_READY_FALSE) && (speed3 != 0))
		setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);


	if((timerCounterModBus%9) == 8)
	{
		if(timerCounterModBus != 0)
			timerCopy = timerCounterModBus;
		timerCounter = 0;

		readPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress);
		readPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress);
		if(GetTherapyType() == LiverTreat)
			readPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress);
	}

	if(pumpPerist[0].dataReady == DATA_READY_TRUE)
	{
		//speed = ((BYTES_TO_WORD_SIGN(msgToRecvFrame3[3], msgToRecvFrame3[4]))/100)*(timerCopy);
		// la velocita' ora posso leggerla direttamente dall'array di registry modbus
		speed0 = modbusData[pumpPerist[0].pmpMySlaveAddress-2][17];
		pumpPerist[0].actualSpeed = speed0;
		pumpPerist[0].dataReady = DATA_READY_FALSE;
	}

	if(pumpPerist[1].dataReady == DATA_READY_TRUE)
	{
		// la velocita' ora posso leggerla direttamente dall'array di registry modbus
		speed1 = modbusData[pumpPerist[1].pmpMySlaveAddress-2][17];
		pumpPerist[1].actualSpeed = speed1;
		pumpPerist[1].dataReady = DATA_READY_FALSE;
	}

	if((GetTherapyType() == LiverTreat) && (pumpPerist[3].dataReady == DATA_READY_TRUE))
	{
		// la velocita' ora posso leggerla direttamente dall'array di registry modbus
		speed3 = modbusData[pumpPerist[3].pmpMySlaveAddress-2][17];
		pumpPerist[3].actualSpeed = speed3;
		pumpPerist[3].dataReady = DATA_READY_FALSE;
	}
}


/* Manage CHILD_TREAT_ALARM_1_STOP_PELTIER entry state */
void manageChildTreatAlm1StopPeltEntry(void)
{
	stopPeltierActuator();
	stopPeltier2Actuator();
}

/* Manage CHILD_TREAT_ALARM_1_STOP_PELTIER always state */
void manageChildTreatAlm1StopPeltAlways(void)
{
	stopPeltierActuator();
	stopPeltier2Actuator();
}

// Filippo gestisco lo stop attuatori in modo diverso nel caso di allarme in test T1
/* Manage CHILD_TREAT_ALARM_1_STOP_ALL_ACTUATOR entry state */
void manageChildTreatAlmT1StopAllActEntry(void)
{
	int speed = 0;
	int timerCopy = 0;

	//gestisco gli attuatori in allarme solo se ho impostato una terapia valida
	THERAPY_TYPE TherType = GetTherapyType();

	if (TherType != Undef)
	{
		if(pumpPerist[0].dataReady == DATA_READY_FALSE)
		{
			setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
			if(getPumpPressLoop(0) == PRESS_LOOP_ON)
				setPumpPressLoop(0, PRESS_LOOP_OFF);
		}
		if(pumpPerist[1].dataReady == DATA_READY_FALSE)
		{
			setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
			if(getPumpPressLoop(1) == PRESS_LOOP_ON)
				setPumpPressLoop(1, PRESS_LOOP_OFF);
		}
		/*viene comandata direttamente col comando di quella sopra*/
//		if(pumpPerist[2].dataReady == DATA_READY_FALSE)
//		{
//			setPumpSpeedValueHighLevel(pumpPerist[2].pmpMySlaveAddress, 0);
//		}
		if((GetTherapyType() == LiverTreat) && (pumpPerist[3].dataReady == DATA_READY_FALSE))
		{
			setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
		}


		// bypasso il filtro
		setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_LEFT_OPEN);   // old MODBUS_PINCH_LEFT_OPEN
		// pich arteriosa collegata al recipiente
		setPinchPositionHighLevel(PINCH_2WPVA, MODBUS_PINCH_RIGHT_OPEN);   // old MODBUS_PINCH_LEFT_OPEN

		if (TherType == LiverTreat)
		{
			// ho selezionato il fegato, quindi devo interrompere anche il circuito venoso e lo collego
			// direttamente al recipiente
			setPinchPositionHighLevel(PINCH_2WPVV, MODBUS_PINCH_RIGHT_OPEN);
		}
		else if (TherType == KidneyTreat)
		{
			//in TherapyType == KidneyTreat non è usata quindi preferisco non muoverla
		}

//		stopPeltierActuator();
//		stopPeltier2Actuator();
	}

	if((timerCounterModBus%9) == 8)
	{
		if(timerCounterModBus != 0)
			timerCopy = timerCounterModBus;
		timerCounter = 0;

		readPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress);
		readPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress);
		if(GetTherapyType() == LiverTreat)
			readPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress);
	}

	if(pumpPerist[0].dataReady == DATA_READY_TRUE)
	{
		//speed = ((BYTES_TO_WORD_SIGN(msgToRecvFrame3[3], msgToRecvFrame3[4]))/100)*(timerCopy);
		// la velocita' ora posso leggerla direttamente dall'array di registry modbus
		speed = modbusData[pumpPerist[0].pmpMySlaveAddress-2][17];
		pumpPerist[0].actualSpeed = speed;
		pumpPerist[0].dataReady = DATA_READY_FALSE;
	}

	if(pumpPerist[1].dataReady == DATA_READY_TRUE)
	{
		// la velocita' ora posso leggerla direttamente dall'array di registry modbus
		speed = modbusData[pumpPerist[1].pmpMySlaveAddress-2][17];
		pumpPerist[1].actualSpeed = speed;
		pumpPerist[1].dataReady = DATA_READY_FALSE;
	}

	if((GetTherapyType() == LiverTreat) && (pumpPerist[3].dataReady == DATA_READY_TRUE))
	{
		// la velocita' ora posso leggerla direttamente dall'array di registry modbus
		speed = modbusData[pumpPerist[3].pmpMySlaveAddress-2][17];
		pumpPerist[3].actualSpeed = speed;
		pumpPerist[3].dataReady = DATA_READY_FALSE;
	}
	pumpPerist[0].entry = 0;
	pumpPerist[1].entry = 0;
	pumpPerist[2].entry = 0;
	pumpPerist[3].entry = 0;

	// Filippo - devo fermare heater e frigo
	DisableHeating();
	DisableFrigo();
//  	FrigoHeatTempControlTask((LIQ_TEMP_CONTR_TASK_CMD)LIQ_T_CONTR_TASK_RESET_CMD);

}

// Filippo - gestisco lo stop attuatori se allarme nel test T1 in modo differente
/* Manage CHILD_TREAT_ALARM_1_STOP_ALL_ACTUATOR always state */
void manageChildTreatAlmT1StopAllActAlways(void)
{
	static int timerCopy = 0;
	THERAPY_TYPE TherType = GetTherapyType();

	//gestisco gli attuatori in allarme solo se ho impostato una terapia valida
	if (TherType != Undef)
	{
		if(pumpPerist[0].dataReady == DATA_READY_FALSE && (pumpPerist[0].actualSpeed != 0))
		{
			setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
		}
		if(pumpPerist[1].dataReady == DATA_READY_FALSE  && ( (pumpPerist[1].actualSpeed != 0) || (pumpPerist[2].actualSpeed != 0)) )
		{
			setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
		}
		/*viene comandata direttamente col comando di quella sopra*/
//		if(pumpPerist[2].dataReady == DATA_READY_FALSE  && (pumpPerist[2].actualSpeed != 0))
//		{
//			setPumpSpeedValueHighLevel(pumpPerist[2].pmpMySlaveAddress, 0);
//		}
		if((TherType == LiverTreat) && (pumpPerist[3].dataReady == DATA_READY_FALSE)  && (pumpPerist[3].actualSpeed != 0))
		{
			setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
		}

		//if (modbusData[PINCH_2WPVF-3][17] != MODBUS_PINCH_LEFT_OPEN)
		if (PinchWriteTerminated(0) && (modbusData[PINCH_2WPVF-3][17] != 0xaa))
		{
			// metto in bypass il filtro per sicurezza
			setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_LEFT_OPEN);
		}

		//if (modbusData[PINCH_2WPVA-3][17] != MODBUS_PINCH_RIGHT_OPEN)
		if (PinchWriteTerminated(1) && (modbusData[PINCH_2WPVA-3][17] != 0xaa))
		{
			setPinchPositionHighLevel(PINCH_2WPVA, MODBUS_PINCH_RIGHT_OPEN);
		}
		/*
		if ( modbusData[PINCH_2WPVV-3][17] != MODBUS_PINCH_RIGHT_OPEN && TherType == LiverTreat)
		{
			setPinchPositionHighLevel(PINCH_2WPVV, MODBUS_PINCH_RIGHT_OPEN);
		}
		else if (modbusData[PINCH_2WPVV-3][17] != MODBUS_PINCH_LEFT_OPEN && TherType == KidneyTreat)
		{
			//in TherapyType == KidneyTreat non è usata quindi preferisco non muoverla
		}
		*/
		if (PinchWriteTerminated(2) && (modbusData[PINCH_2WPVV-3][17] != 0xaa) && (TherType == LiverTreat))
		{
			setPinchPositionHighLevel(PINCH_2WPVV, MODBUS_PINCH_RIGHT_OPEN);
		}
		else if (PinchWriteTerminated(2) && (modbusData[PINCH_2WPVV-3][17] != 0xaa) && (TherType == KidneyTreat))
		{
			//in TherapyType == KidneyTreat non è usata quindi preferisco non muoverla
		}
//		stopPeltierActuator();
//		stopPeltier2Actuator();
	}

//	if((timerCounterModBus%9) == 8)
//	{
//		if(timerCounterModBus != 0)
//			timerCopy = timerCounterModBus;
//		timerCounter = 0;
//		readPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress);
//	}

}


/* Manage CHILD_TREAT_ALARM_1_STOP_ALL_ACTUATOR entry state */
void manageChildTreatAlm1StopAllActEntry(void)
{
	int speed = 0;
	int timerCopy = 0;

	//gestisco gli attuatori in allarme solo se ho impostato una terapia valida
	THERAPY_TYPE TherType = GetTherapyType();

	if (TherType != Undef)
	{
		if(pumpPerist[0].dataReady == DATA_READY_FALSE)
		{
			setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
			if(getPumpPressLoop(0) == PRESS_LOOP_ON)
				setPumpPressLoop(0, PRESS_LOOP_OFF);
		}
		if(pumpPerist[1].dataReady == DATA_READY_FALSE)
		{
			setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
			if(getPumpPressLoop(1) == PRESS_LOOP_ON)
				setPumpPressLoop(1, PRESS_LOOP_OFF);
		}
		/*viene comandata direttamente col comando di quella sopra*/
//		if(pumpPerist[2].dataReady == DATA_READY_FALSE)
//		{
//			setPumpSpeedValueHighLevel(pumpPerist[2].pmpMySlaveAddress, 0);
//		}
		if((GetTherapyType() == LiverTreat) && (pumpPerist[3].dataReady == DATA_READY_FALSE))
		{
			setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
		}


		// bypasso il filtro
		setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_LEFT_OPEN);   // old MODBUS_PINCH_LEFT_OPEN
		// pich arteriosa collegata al recipiente
		setPinchPositionHighLevel(PINCH_2WPVA, MODBUS_PINCH_RIGHT_OPEN);   // old MODBUS_PINCH_LEFT_OPEN

		if (TherType == LiverTreat)
		{
			// ho selezionato il fegato, quindi devo interrompere anche il circuito venoso e lo collego
			// direttamente al recipiente
			setPinchPositionHighLevel(PINCH_2WPVV, MODBUS_PINCH_RIGHT_OPEN);
		}
		else if (TherType == KidneyTreat)
		{
			//in TherapyType == KidneyTreat non è usata quindi preferisco non muoverla
		}

//		stopPeltierActuator();
//		stopPeltier2Actuator();
	}

	if((timerCounterModBus%9) == 8)
	{
		if(timerCounterModBus != 0)
			timerCopy = timerCounterModBus;
		timerCounter = 0;

		readPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress);
		readPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress);
		if(GetTherapyType() == LiverTreat)
			readPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress);
	}

	if(pumpPerist[0].dataReady == DATA_READY_TRUE)
	{
		//speed = ((BYTES_TO_WORD_SIGN(msgToRecvFrame3[3], msgToRecvFrame3[4]))/100)*(timerCopy);
		// la velocita' ora posso leggerla direttamente dall'array di registry modbus
		speed = modbusData[pumpPerist[0].pmpMySlaveAddress-2][17];
		pumpPerist[0].actualSpeed = speed;
		pumpPerist[0].dataReady = DATA_READY_FALSE;
	}

	if(pumpPerist[1].dataReady == DATA_READY_TRUE)
	{
		// la velocita' ora posso leggerla direttamente dall'array di registry modbus
		speed = modbusData[pumpPerist[1].pmpMySlaveAddress-2][17];
		pumpPerist[1].actualSpeed = speed;
		pumpPerist[1].dataReady = DATA_READY_FALSE;
	}

	if((GetTherapyType() == LiverTreat) && (pumpPerist[3].dataReady == DATA_READY_TRUE))
	{
		// la velocita' ora posso leggerla direttamente dall'array di registry modbus
		speed = modbusData[pumpPerist[3].pmpMySlaveAddress-2][17];
		pumpPerist[3].actualSpeed = speed;
		pumpPerist[3].dataReady = DATA_READY_FALSE;
	}
	pumpPerist[0].entry = 0;
	pumpPerist[1].entry = 0;
	pumpPerist[2].entry = 0;
	pumpPerist[3].entry = 0;

	// Filippo - devo fermare heater e frigo
	DisableHeating();
	DisableFrigo();

}

/* Manage CHILD_TREAT_ALARM_1_STOP_ALL_ACTUATOR always state */
void manageChildTreatAlm1StopAllActAlways(void)
{
	static int timerCopy = 0;
	THERAPY_TYPE TherType = GetTherapyType();

	//gestisco gli attuatori in allarme solo se ho impostato una terapia valida
	if (TherType != Undef)
	{
		if(pumpPerist[0].dataReady == DATA_READY_FALSE && (pumpPerist[0].actualSpeed != 0))
		{
			setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
		}
		if(pumpPerist[1].dataReady == DATA_READY_FALSE  && ( (pumpPerist[1].actualSpeed != 0) || (pumpPerist[2].actualSpeed != 0)) )
		{
			setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
		}
		/*viene comandata direttamente col comando di quella sopra*/
//		if(pumpPerist[2].dataReady == DATA_READY_FALSE  && (pumpPerist[2].actualSpeed != 0))
//		{
//			setPumpSpeedValueHighLevel(pumpPerist[2].pmpMySlaveAddress, 0);
//		}
		if((TherType == LiverTreat) && (pumpPerist[3].dataReady == DATA_READY_FALSE)  && (pumpPerist[3].actualSpeed != 0))
		{
			setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, 0);
		}

		//if (modbusData[PINCH_2WPVF-3][17] != MODBUS_PINCH_LEFT_OPEN)
		if (PinchWriteTerminated(0) && (modbusData[PINCH_2WPVF-3][17] != 0xaa))
		{
			// metto in bypass il filtro per sicurezza
			setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_LEFT_OPEN);
		}

		//if (modbusData[PINCH_2WPVA-3][17] != MODBUS_PINCH_RIGHT_OPEN)
		if (PinchWriteTerminated(1) && (modbusData[PINCH_2WPVA-3][17] != 0xaa))
		{
			setPinchPositionHighLevel(PINCH_2WPVA, MODBUS_PINCH_RIGHT_OPEN);
		}
		/*
		if ( modbusData[PINCH_2WPVV-3][17] != MODBUS_PINCH_RIGHT_OPEN && TherType == LiverTreat)
		{
			setPinchPositionHighLevel(PINCH_2WPVV, MODBUS_PINCH_RIGHT_OPEN);
		}
		else if (modbusData[PINCH_2WPVV-3][17] != MODBUS_PINCH_LEFT_OPEN && TherType == KidneyTreat)
		{
			//in TherapyType == KidneyTreat non è usata quindi preferisco non muoverla
		}
		*/
		if (PinchWriteTerminated(2) && (modbusData[PINCH_2WPVV-3][17] != 0xaa) && (TherType == LiverTreat))
		{
			setPinchPositionHighLevel(PINCH_2WPVV, MODBUS_PINCH_RIGHT_OPEN);
		}
		else if (PinchWriteTerminated(2) && (modbusData[PINCH_2WPVV-3][17] != 0xaa) && (TherType == KidneyTreat))
		{
			//in TherapyType == KidneyTreat non è usata quindi preferisco non muoverla
		}
//		stopPeltierActuator();
//		stopPeltier2Actuator();
	}

//	if((timerCounterModBus%9) == 8)
//	{
//		if(timerCounterModBus != 0)
//			timerCopy = timerCounterModBus;
//		timerCounter = 0;
//		readPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress);
//	}

}



/* Manage CHILD_TREAT_ALARM_1_SAF_AIR_FILT entry state */
/* (FM) risolvo la situazione di allarme
 * rene:   attivando il bypass del filtro cioe'
 *         spostando wpwf a sinistra e wpva a destra
 * fegato: attivando il bypass del filtro cioe'
 *         spostando wpwf a sinistra */
void manageChildTreatAlm1SafAirFiltEntry(void)
{
	// fermo le pompe e metto le pinch in sicurezza
	manageChildTreatAlm1StopAllActEntry();
	TreatAlm1SafAirFiltActive = TRUE;
}

/* Manage CHILD_TREAT_ALARM_1_SAF_AIR_FILT always state */
void manageChildTreatAlm1SafAirFiltAlways(void)
{
	// apetto che tutte le pompe si siano fermate
	manageChildTreatAlm1StopAllActAlways();
	DisableAllAirAlarm(TRUE); // forzo la chiusura dell'allarme aria
	if(!ResButInChildFlag && (buttonGUITreatment[BUTTON_RESET_ALARM].state == GUI_BUTTON_RELEASED) && IsSecurityStateActive())
	{
		ResButInChildFlag = TRUE;
		// setto la guard per fare in modo che quando l'allarme risultera' non attivo
		// la macchina a stati parent vada nello stato di espulsione bolla aria
		currentGuard[GUARD_ALARM_AIR_FILT_RECOVERY].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		// ho raggiunto la condizione di sicurezza ed ho ricevuto un comando reset alarm
		releaseGUIButton(BUTTON_RESET_ALARM);
		EnableNextAlarmFunc(); //EnableNextAlarm = TRUE;
		LevelBuzzer = SILENT;//0;
	}
}


/* Manage CHILD_TREAT_ALARM_1_SFV_AIR entry state */
/* (FM) risolvo la situazione di allarme
 * rene:   non faccio niente
 * fegato: sposto wpvv a destra in modo da staccare l'organo
 *         e scaricare sul reservoir */
void manageChildTreatAlm1SFVEntry(void)
{
	/*
	THERAPY_TYPE TherType = GetTherapyType();
	if(TherType == KidneyTreat)
	{
	}
	else if(TherType == LiverTreat)
	{
		HandlePinch(BUTTON_PINCH_2WPVV_RIGHT_OPEN);
	}
	*/
	// fermo le pompe e metto le pinch in sicurezza
	manageChildTreatAlm1StopAllActEntry();
	TreatAlm1SFVActive = TRUE;
}

/* Manage CHILD_TREAT_ALARM_1_SFV_AIR always state */
void manageChildTreatAlm1SFVAlways(void)
{
	// apetto che tutte le pompe si siano fermate
	manageChildTreatAlm1StopAllActAlways();
	DisableAllAirAlarm(TRUE); // forzo la chiusura dell'allarme aria
	if(!ResButInChildFlag && (buttonGUITreatment[BUTTON_RESET_ALARM].state == GUI_BUTTON_RELEASED) && IsSecurityStateActive())
	{
		ResButInChildFlag = TRUE;
		// setto la guard per fare in modo che quando l'allarme risultera' non attivo
		// la macchina a stati parent vada nello stato di espulsione bolla aria
		currentGuard[GUARD_ALARM_AIR_SFV_RECOVERY].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		// ho raggiunto la condizione di sicurezza ed ho ricevuto un comando reset alarm
		releaseGUIButton(BUTTON_RESET_ALARM);
		EnableNextAlarmFunc(); //EnableNextAlarm = TRUE;
		LevelBuzzer = SILENT;//0;
	}
}


/* Manage CHILD_TREAT_ALARM_1_SFA_AIR entry state */
/* (FM) risolvo la situazione di allarme
 * rene:   sposto wpwa a destra in modo da staccare l'organo
 *         e scaricare sul reservoir
 * fegato: sposto wpwa a destra in modo da staccare l'organo
 *         e scaricare sul reservoir */

void manageChildTreatAlm1SFAEntry(void)
{
	// fermo le pompe e metto le pinch in sicurezza
	manageChildTreatAlm1StopAllActEntry();
	TreatAlm1SFAActive = TRUE;
}

/* Manage CHILD_TREAT_ALARM_1_SFA_AIR always state */
void manageChildTreatAlm1SFAAlways(void)
{
	// apetto che tutte le pompe si siano fermate
	manageChildTreatAlm1StopAllActAlways();
	DisableAllAirAlarm(TRUE); // forzo la chiusura dell'allarme aria
	if(!ResButInChildFlag && (buttonGUITreatment[BUTTON_RESET_ALARM].state == GUI_BUTTON_RELEASED) && IsSecurityStateActive())
	{
		ResButInChildFlag = TRUE;
		// setto la guard per fare in modo che quando l'allarme risultera' non attivo
		// la macchina a stati parent vada nello stato di espulsione bolla aria
		currentGuard[GUARD_ALARM_AIR_SFA_RECOVERY].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		// ho raggiunto la condizione di sicurezza ed ho ricevuto un comando reset alarm
		releaseGUIButton(BUTTON_RESET_ALARM);
		EnableNextAlarmFunc(); //EnableNextAlarm = TRUE;
		LevelBuzzer = SILENT;//0;
	}
}


//--------------------------------------------------------------------------------------------------
// La gestione dell'allarme che segue e' stata fatta il troppo pieno.
// Consiste nel disabilitare per sempre l'allarme senza aspettare che il segnale fisico
// vada a 0 e poi proseguire normalmente
/* Manage CHILD_TREAT_ALARM_1_WAIT_CMD entry state */
/* (FM) risolvo la situazione di allarme
 * rene:   sposto wpwa a destra in modo da staccare l'organo
 *         e scaricare sul reservoir
 * fegato: sposto wpwa a destra in modo da staccare l'organo
 *         e scaricare sul reservoir */
void manageChildAlmAndWaitCmdEntry(void)
{
	// fermo le pompe e metto le pinch in sicurezza
	manageChildTreatAlm1StopAllActEntry();
	GlobalFlags.FlagsDef.ChildAlmAndWaitCmdActive = 1;
}

/* Manage CHILD_TREAT_ALARM_1_WAIT_CMD always state */
void manageChildAlmAndWaitCmdAlways(void)
{
	// apetto che tutte le pompe si siano fermate
	manageChildTreatAlm1StopAllActAlways();
	if(!ResButInChildFlag && (buttonGUITreatment[BUTTON_RESET_ALARM].state == GUI_BUTTON_RELEASED) && IsSecurityStateActive())
	{
		ResButInChildFlag = TRUE;
		// Questa tipologia di allarmi deve essere forzata in off dal software prima di poter riprendere il lavoro
		ForceCurrentAlarmOff();
		// setto la guard per fare in modo che quando l'allarme risultera' non attivo
		// la macchina a stati parent riprenda il funzionamento normale
		currentGuard[GUARD_ALARM_WAIT_CMD_TO_EXIT].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		// ho raggiunto la condizione di sicurezza ed ho ricevuto un comando reset alarm
		releaseGUIButton(BUTTON_RESET_ALARM);
		EnableNextAlarmFunc(); //EnableNextAlarm = TRUE;
		LevelBuzzer = SILENT;//0;
	}
}
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
// allarme di pinch posizionate male nella fase iniziale del trattamento
// CHILD_TREAT_ALARM_BAD_PINCH_POS state
void manageChildTreatAlmBadPinchPosEntry(void)
{
	manageChildTreatAlm1StopAllActEntry();
}
void manageChildTreatAlmBadPinchPosAlways(void)
{
	manageChildTreatAlm1StopAllActAlways();
	if(!ResButInChildFlag && (buttonGUITreatment[BUTTON_RESET_ALARM].state == GUI_BUTTON_RELEASED))
	{
		//ResButInChildFlag = TRUE;
		// se l'allarme era stato generato durante il posizionamento iniziale delle pinch prima di iniziare
		// il trattamento (pompe ferme)
		ResetTreatSetPinchPosTaskAlm();
		// se l'allarme era stato generato da qualche problema quando il trattamento era gia' attivo (pompe in movimento)
		ResetTreatCurrPinchPosOk();
		// evito di fare la release perche' il comando mi servira' per uscire dallo stato PARENT_TREAT_KIDNEY_1_ALARM
		//releaseGUIButton(BUTTON_RESET_ALARM);
		EnableNextAlarmFunc(); //EnableNextAlarm = TRUE;
	}
	else if(buttonGUITreatment[BUTTON_OVERRIDE_ALARM].state == GUI_BUTTON_RELEASED)
	{
		ResetTreatSetPinchPosTaskAlm();
		ResetTreatCurrPinchPosOk();
		ForceCurrentAlarmOff();
		releaseGUIButton(BUTTON_OVERRIDE_ALARM);
		EnableNextAlarmFunc(); //EnableNextAlarm = TRUE;

		// forzo un BUTTON_RESET_ALARM perche' il comando mi servira' per uscire dallo stato PARENT_TREAT_KIDNEY_1_ALARM
		// ed andare nello stato di funzionamento normale
		setGUIButton(BUTTON_RESET_ALARM);
	}

}
//--------------------------------------------------------------------------------------------------

void manageChildTreatAlmModBusErrEntry(void)
{
	EN_Motor_Control(DISABLE);
}

/* Manage CHILD_TREAT_ALARM_MOD_BUS_ERROR always state */
void manageChildTreatAlmModBusErrAlways(void)
{
	if(!ResButInChildFlag && (buttonGUITreatment[BUTTON_RESET_ALARM].state == GUI_BUTTON_RELEASED))
	{
		ResButInChildFlag = TRUE;
		EN_Motor_Control(ENABLE);
		ClearModBusAlarm();
		// in questo caso evito di rilasciare il tasto BUTTON_RESET_ALARM perche' per uscire dallo stato
		// di allarme PARENT_PRIMING_TREAT_KIDNEY_1_ALARM controllo
		// currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_FALSE ma aspetto anche un comando
		// di reset
		//releaseGUIButton(BUTTON_RESET_ALARM);
		//EnableNextAlarmFunc(); //EnableNextAlarm = TRUE;
	}
	else if(buttonGUITreatment[BUTTON_OVERRIDE_ALARM].state == GUI_BUTTON_RELEASED)
	{
		EN_Motor_Control(ENABLE);
		ClearModBusAlarm();
		ForceCurrentAlarmOff();
		releaseGUIButton(BUTTON_OVERRIDE_ALARM);
		EnableNextAlarmFunc(); //EnableNextAlarm = TRUE;
	}

}


// Manage CHILD_TREAT_ALARM_1_DELTA_TEMP_HIGH entry state
void manageChildTreatAlmDeltaTempHighEntry(void)
{
	// fermo le pompe e metto le pinch in sicurezza
	manageChildTreatAlm1StopAllActEntry();
}

/* Manage CHILD_TREAT_ALARM_1_DELTA_TEMP_HIGH always state */
void manageChildTreatAlmDeltaTempHighAlways(void)
{
	// apetto che tutte le pompe si siano fermate
	manageChildTreatAlm1StopAllActAlways();
	// disabilito allarmi aria durante la fase di ripristino della temperatura del liquido
	DisableAllAirAlarm(TRUE);
	// forzo uscita da allarme
	DisableDeltaTHighAlmFunc();
	if(!ResButInChildFlag && (buttonGUITreatment[BUTTON_RESET_ALARM].state == GUI_BUTTON_RELEASED) && IsSecurityStateActive())
	{
		ResButInChildFlag = TRUE;
		// setto la guard per fare in modo di passare alla movimentazione del liquido per
		// cercare di rientrare nella temperatura
		currentGuard[GUARD_ALARM_DELTA_TEMP_HIGH_RECV].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		// ho raggiunto la condizione di sicurezza ed ho ricevuto un comando reset alarm
		releaseGUIButton(BUTTON_RESET_ALARM);
		EnableNextAlarmFunc();
		LevelBuzzer = SILENT;//0;
	}
}


// funzione che gestisce gli stati child (allarmi) nel caso di trattamento1 o kidney
// deve essere chiamata dalla gestione del parent nello stato PARENT_TREAT_KIDNEY_1_ALARM
void ManageStateChildAlarmTreat1(void)
{
	switch(ptrCurrentChild->child){
		case CHILD_TREAT_ALARM_1_INIT:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				/* (FM) esegue la parte ACTION_ON_ENTRY della gestione dell'allarme */
				// non serve qui verra' chiamata nella ManageStateChildAlarmTreat1 dello stato principale stateState
				//ptrCurrentChild->callBackFunct();
				ptrFutureChild = &stateChildAlarmTreat1[2];
			}
			else if(ptrCurrentChild->action == ACTION_ALWAYS){
				/* (FM) esegue la parte ACTION_ALWAYS della gestione dell'allarme */
				// non serve qui verra' chiamata nella ManageStateChildAlarmTreat1 dello stato principale stateState
				//ptrCurrentChild->callBackFunct();
			}

            /* (FM) probabilmente cio' che deve essere fatto per completare la gestione dell'allarme e'quello che segue.
               I valori vengono messi a GUARD_VALUE_TRUE nella funzione manageAlarmChildGuard in base alle impostazioni
               fatte per la gestione degli allarmi */
            if(currentGuard[GUARD_ALARM_STOP_ALL_ACTUATOR].guardValue == GUARD_VALUE_TRUE)
            {
                /* (FM) risolvo la situazione di allarme andando a spegnere tutti gli attuatori */
                ptrFutureChild = &stateChildAlarmTreat1[11];
            }
            else if(currentGuard[GUARD_ALARM_STOP_ALL].guardValue == GUARD_VALUE_TRUE)
            {
            	/* (FM) risolvo la situazione di allarme andando a spegnere tutti gli attuatori e pompe */
                ptrFutureChild = &stateChildAlarmTreat1[7];
            }
            else if(currentGuard[GUARD_ALARM_STOP_PERF_PUMP].guardValue == GUARD_VALUE_TRUE)
            {
            	/* (FM) risolvo la situazione di allarme andando a spegnere la PERF PUMP */
                ptrFutureChild = &stateChildAlarmTreat1[3];
            }
            else if(currentGuard[GUARD_ALARM_STOP_PURIF_PUMP].guardValue == GUARD_VALUE_TRUE)
            {
            	/* (FM) risolvo la situazione di allarme andando a spegnere la PURIF PUMP */
                ptrFutureChild = &stateChildAlarmTreat1[5];
            }
            else if(currentGuard[GUARD_ALARM_STOP_OXYG_PUMP].guardValue == GUARD_VALUE_TRUE)
            {
            	/* (FM) risolvo la situazione di allarme andando a spegnere la OXYG PUMP */
                ptrFutureChild = &stateChildAlarmTreat1[5];
            }
            else if(currentGuard[GUARD_ALARM_STOP_PELTIER].guardValue == GUARD_VALUE_TRUE)
            {
            	/* (FM) risolvo la situazione di allarme andando ad agire sulla cella di peltier */
                ptrFutureChild = &stateChildAlarmTreat1[9];
            }

            else if(currentGuard[GUARD_ALARM_SAF_AIR_FILT].guardValue == GUARD_VALUE_TRUE)
            {
            	/* (FM) risolvo la situazione di allarme
            	 * rene:   attivando il bypass del filtro cioe'
            	 *         spostando wpwf a sinistra e wpva a destra
            	 * fegato: attivando il bypass del filtro cioe'
            	 *         spostando wpwf a sinistra */
                ptrFutureChild = &stateChildAlarmTreat1[13];
            }
            else if(currentGuard[GUARD_ALARM_SFV_AIR].guardValue == GUARD_VALUE_TRUE)
            {
            	/* (FM) risolvo la situazione di allarme
            	 * rene:   non faccio niente
            	 * fegato: sposto wpwv a destra in modo da staccare l'organo
            	 *         e scaricare sul reservoir */
                ptrFutureChild = &stateChildAlarmTreat1[15];
            }
            else if(currentGuard[GUARD_ALARM_SFA_AIR].guardValue == GUARD_VALUE_TRUE)
            {
            	/* (FM) risolvo la situazione di allarme
            	 * rene:   sposto wpwa a destra in modo da staccare l'organo
            	 *         e scaricare sul reservoir
            	 * fegato: sposto wpwa a destra in modo da staccare l'organo
            	 *         e scaricare sul reservoir */
                ptrFutureChild = &stateChildAlarmTreat1[17];
            }
            else if(currentGuard[GUARD_ALARM_STOP_ALL_ACT_WAIT_CMD].guardValue == GUARD_VALUE_TRUE)
            {
            	/* (FM) risolvo la situazione di allarme
            	 * rene:   sposto wpwa a destra in modo da staccare l'organo
            	 *         e scaricare sul reservoir
            	 * fegato: sposto wpwa a destra in modo da staccare l'organo
            	 *         e scaricare sul reservoir */
                ptrFutureChild = &stateChildAlarmTreat1[21]; // DA AGGIUNGERE
            }
            else if(currentGuard[GUARD_ALARM_BAD_PINCH_POS].guardValue == GUARD_VALUE_TRUE)
            {
            	/* (FM) risolvo la situazione di allarme andando  a spegnere tutti gli attuatori */
                ptrFutureChild = &stateChildAlarmTreat1[23];
            }
            else if(currentGuard[GUARD_ALARM_MOD_BUS_ERROR].guardValue == GUARD_VALUE_TRUE)
            {
            	/* (FM) risolvo la situazione di allarme andando  a spegnere tutti gli attuatori */
                ptrFutureChild = &stateChildAlarmTreat1[25];
            }
            else if(currentGuard[GUARD_ALARM_DELTA_TEMP_HIGH].guardValue == GUARD_VALUE_TRUE)
            {
            	/* (FM) risolvo la situazione di allarme
            	   escludendo l'organo per poi iniziare una fase di circolazione
            	   del liquido fino a quando la temperatura non ritorna sul target */
                ptrFutureChild = &stateChildAlarmTreat1[27];
            }
            ResButInChildFlag = FALSE;
			break;

		case CHILD_TREAT_ALARM_1_STOP_PERFUSION:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				// non serve qui verra' chiamata nella ManageStateChildAlarmTreat1 dello stato principale stateState
				//ptrCurrentChild->callBackFunct();
				ptrFutureChild = &stateChildAlarmTreat1[4];
			}
            else if(currentGuard[GUARD_ALARM_STOP_PERF_PUMP].guardValue == GUARD_VALUE_FALSE )
                ptrFutureChild = &stateChildAlarmTreat1[19]; /* (FM) allarme chiuso */
			else if(ptrCurrentChild->action == ACTION_ALWAYS){
				// non serve qui verra' chiamata nella ManageStateChildAlarmTreat1 dello stato principale stateState
				//ptrCurrentChild->callBackFunct();
			}
			break;

		case CHILD_TREAT_ALARM_1_STOP_PURIFICATION:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				// non serve qui verra' chiamata nella manageState... dello stato principale stateState
				//ptrCurrentChild->callBackFunct();
				ptrFutureChild = &stateChildAlarmTreat1[6];
			}
            else if((currentGuard[GUARD_ALARM_STOP_PURIF_PUMP].guardValue == GUARD_VALUE_FALSE) ||
                     (currentGuard[GUARD_ALARM_STOP_OXYG_PUMP].guardValue == GUARD_VALUE_FALSE) )
                ptrFutureChild = &stateChildAlarmTreat1[19]; /* (FM) allarme chiuso */
			else if(ptrCurrentChild->action == ACTION_ALWAYS){
				// non serve qui verra' chiamata nella ManageStateChildAlarmTreat1 dello stato principale stateState
				// ptrCurrentChild->callBackFunct();
			}
			break;

		case CHILD_TREAT_ALARM_1_STOP_ALL_PUMP:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				// non serve qui verra' chiamata nella ManageStateChildAlarmTreat1 dello stato principale stateState
				// ptrCurrentChild->callBackFunct();
				ptrFutureChild = &stateChildAlarmTreat1[8];
			}
            else if(currentGuard[GUARD_ALARM_STOP_ALL].guardValue == GUARD_VALUE_FALSE )
                ptrFutureChild = &stateChildAlarmTreat1[19]; /* FM allarme chiuso */
			else if(ptrCurrentChild->action == ACTION_ALWAYS){
				// non serve qui verra' chiamata nella ManageStateChildAlarmTreat1 dello stato principale stateState
				// ptrCurrentChild->callBackFunct();
			}
			break;

		case CHILD_TREAT_ALARM_1_STOP_PELTIER:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				// non serve qui verra' chiamata nella ManageStateChildAlarmTreat1 dello stato principale stateState
				// ptrCurrentChild->callBackFunct();
				ptrFutureChild = &stateChildAlarmTreat1[10];
			}
            else if(currentGuard[GUARD_ALARM_STOP_PELTIER].guardValue == GUARD_VALUE_FALSE )
                ptrFutureChild = &stateChildAlarmTreat1[19]; /* FM allarme chiuso */
			else if(ptrCurrentChild->action == ACTION_ALWAYS){
				// non serve qui verra' chiamata nella ManageStateChildAlarmTreat1 dello stato principale stateState
				// ptrCurrentChild->callBackFunct();
			}
			break;

		case CHILD_TREAT_ALARM_1_STOP_ALL_ACTUATOR:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				// non serve qui verra' chiamata nella ManageStateChildAlarmTreat1 dello stato principale stateState
				// ptrCurrentChild->callBackFunct();
				ptrFutureChild = &stateChildAlarmTreat1[12];
			}
            else if( currentGuard[GUARD_ALARM_STOP_ALL_ACTUATOR].guardValue == GUARD_VALUE_FALSE )
                ptrFutureChild = &stateChildAlarmTreat1[19]; /* FM allarme chiuso */
			else if(ptrCurrentChild->action == ACTION_ALWAYS){
				// non serve qui verra' chiamata nella ManageStateChildAlarmTreat1 dello stato principale stateState
				// ptrCurrentChild->callBackFunct();
			}
			break;


		case CHILD_TREAT_ALARM_1_SAF_AIR_FILT:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				ptrFutureChild = &stateChildAlarmTreat1[14];
			}
            else if( currentGuard[GUARD_ALARM_SAF_AIR_FILT].guardValue == GUARD_VALUE_FALSE )
                ptrFutureChild = &stateChildAlarmTreat1[19]; /* FM allarme chiuso */
			else if(ptrCurrentChild->action == ACTION_ALWAYS)
			{
			}
			break;
		case CHILD_TREAT_ALARM_1_SFV_AIR:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				ptrFutureChild = &stateChildAlarmTreat1[16];
			}
            else if( currentGuard[GUARD_ALARM_SFV_AIR].guardValue == GUARD_VALUE_FALSE )
                ptrFutureChild = &stateChildAlarmTreat1[19]; /* FM allarme chiuso */
			else if(ptrCurrentChild->action == ACTION_ALWAYS)
			{
			}
			break;
		case CHILD_TREAT_ALARM_1_SFA_AIR:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				ptrFutureChild = &stateChildAlarmTreat1[18];
			}
            else if( currentGuard[GUARD_ALARM_SFA_AIR].guardValue == GUARD_VALUE_FALSE )
                ptrFutureChild = &stateChildAlarmTreat1[19]; /* FM allarme chiuso */
			else if(ptrCurrentChild->action == ACTION_ALWAYS)
			{
			}
			break;

		case CHILD_TREAT_ALARM_1_WAIT_CMD:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				ptrFutureChild = &stateChildAlarmTreat1[22];
			}
            else if( currentGuard[GUARD_ALARM_STOP_ALL_ACT_WAIT_CMD].guardValue == GUARD_VALUE_FALSE )
                ptrFutureChild = &stateChildAlarmTreat1[19]; /* FM allarme chiuso */
			else if(ptrCurrentChild->action == ACTION_ALWAYS)
			{
			}
			break;

		case CHILD_TREAT_ALARM_BAD_PINCH_POS:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				ptrFutureChild = &stateChildAlarmTreat1[24];
			}
            else if( currentGuard[GUARD_ALARM_BAD_PINCH_POS].guardValue == GUARD_VALUE_FALSE )
                ptrFutureChild = &stateChildAlarmTreat1[19]; /* FM allarme chiuso */
			else if(ptrCurrentChild->action == ACTION_ALWAYS)
			{
			}
			break;

		case CHILD_TREAT_ALARM_MOD_BUS_ERROR:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				ptrFutureChild = &stateChildAlarmTreat1[26];
			}
            else if( currentGuard[GUARD_ALARM_MOD_BUS_ERROR].guardValue == GUARD_VALUE_FALSE )
                ptrFutureChild = &stateChildAlarmTreat1[19]; /* FM allarme chiuso */
			else if(ptrCurrentChild->action == ACTION_ALWAYS)
			{
			}
			break;

		case CHILD_TREAT_ALARM_1_DELTA_TEMP_HIGH:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				ptrFutureChild = &stateChildAlarmTreat1[28];
			}
            else if( currentGuard[GUARD_ALARM_DELTA_TEMP_HIGH].guardValue == GUARD_VALUE_FALSE )
                ptrFutureChild = &stateChildAlarmTreat1[19]; /* FM allarme chiuso */
			else if(ptrCurrentChild->action == ACTION_ALWAYS)
			{
			}
			break;

		case CHILD_TREAT_ALARM_1_END:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				// non serve qui verra' chiamata nella ManageStateChildAlarmTreat1 dello stato principale stateState
				//ptrCurrentChild->callBackFunct();
				ptrFutureChild = &stateChildAlarmTreat1[20];
			}
			else if(ptrCurrentChild->action == ACTION_ALWAYS){
                /* (FM) RIMANGO FERMO QUI FINO AL PROSSIMO ALLARME. NON E' NECESSARIO USCIRE DA QUESTO STATO DOPO IL TERMINE
                   DELLA CONDIZIONE DI ALLARME. LA CONDIZIONE DI ALLARME SUCCESSIVA MI FARA' RIPARTIRE DA CHILD_PRIMING_ALARM_INIT */
				// non serve qui verra' chiamata nella ManageStateChildAlarmTreat1 dello stato principale stateState
				// ptrCurrentChild->callBackFunct();
			}
			break;

		default:
			break;
	}
}

// idx indice della struttura nell'array pinchActuator
//     0  PINCH_2WPVF
//     1  PINCH_2WPVA
//     2  PINCH_2WPVV
bool PinchWriteTerminated(int idx)
{
	bool WriteTerminated = FALSE;
	if((pinchActuator[idx].reqState == REQ_STATE_ON) && (pinchActuator[1].reqType == REQ_TYPE_WRITE))
	{
		// scrittura in corso
	}
	else if((pinchActuator[idx].reqState == REQ_STATE_OFF) && (pinchActuator[1].reqType == REQ_TYPE_IDLE))
	{
		// scrittura terminata
		WriteTerminated = TRUE;
	}
	return WriteTerminated;
}


// ritorna TRUE se ho raggiunto la condizione di sicurezza
bool IsSecurityStateActive(void)
{
	THERAPY_TYPE TherType = GetTherapyType();

	bool AllMotStopped = FALSE;
	bool PinchOk = TRUE;
	if(pumpPerist[0].actualSpeed == 0 &&
	   pumpPerist[1].actualSpeed == 0 &&
	   pumpPerist[2].actualSpeed == 0 &&
	   pumpPerist[3].actualSpeed == 0)
		AllMotStopped = TRUE;


	if ((PinchOk == TRUE) && PinchWriteTerminated(0) && (modbusData[PINCH_2WPVF-3][17] != 0xaa))
		PinchOk = FALSE;
	if ((PinchOk == TRUE) && PinchWriteTerminated(1) && (modbusData[PINCH_2WPVA-3][17] != 0xaa))
		PinchOk = FALSE;
	if ((PinchOk == TRUE) && PinchWriteTerminated(2) && (modbusData[PINCH_2WPVV-3][17] != 0xaa) && (TherType == LiverTreat))
		PinchOk = FALSE;

	if(AllMotStopped && PinchOk)
		return TRUE;
	else
		return FALSE;
}

// ritorna TRUE se sono in sicurezza
bool PutPinchInSafetyPos(void)
{
	static unsigned char PutPinchInSafetyPos = 0;
	bool InSafetyPos = TRUE;
	THERAPY_TYPE TherType = GetTherapyType();

	if(!PutPinchInSafetyPos)
	{
		setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_LEFT_OPEN);
		setPinchPositionHighLevel(PINCH_2WPVA, MODBUS_PINCH_RIGHT_OPEN);
		setPinchPositionHighLevel(PINCH_2WPVV, MODBUS_PINCH_RIGHT_OPEN);
		PutPinchInSafetyPos = 1;
		InSafetyPos = FALSE;
	}
	else
	{
		if (PinchWriteTerminated(0) && (modbusData[PINCH_2WPVF-3][17] != 0xaa))
		{
			// metto in bypass il filtro per sicurezza
			setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_LEFT_OPEN);
			InSafetyPos = FALSE;
		}
		if (PinchWriteTerminated(1) && (modbusData[PINCH_2WPVA-3][17] != 0xaa))
		{
			setPinchPositionHighLevel(PINCH_2WPVA, MODBUS_PINCH_RIGHT_OPEN);
			InSafetyPos = FALSE;
		}
		if (PinchWriteTerminated(2) && (modbusData[PINCH_2WPVV-3][17] != 0xaa) && (TherType == LiverTreat))
		{
			setPinchPositionHighLevel(PINCH_2WPVV, MODBUS_PINCH_RIGHT_OPEN);
			InSafetyPos = FALSE;
		}
		else if (PinchWriteTerminated(2) && (modbusData[PINCH_2WPVV-3][17] != 0xaa) && (TherType == KidneyTreat))
		{
			//in TherapyType == KidneyTreat non è usata quindi preferisco non muoverla
		}
	}
	return InSafetyPos;
}

//-////////////////////////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------SVUOTAMENTO DISPOSABLE----------------------------------------------------------
// PER LA GESTIONE DEGLI ALLARMI ARIA E PRESSIONE NEL CASO DI SVUOTAMENTO DISPOSABLE

// Flags usati nel processo di svuotamento
CHILD_EMPTY_FLAGS ChildEmptyFlags;

// Filippo - funzione che gestisce l'allarme in idle
void manageChildIdleAlarm(void)
{
	// devo fermare tutti gli attuatori
	if (currentGuard[GUARD_ALARM_ACTIVE].guardValue==GUARD_VALUE_TRUE)
	{





	}
}

//CHILD_TREAT_ALARM_1_INIT quando lo stato principale e' STATE_EMPTY_DISPOSABLE
void manageChildEmptyAlm1InitEntry(void)
{
    if(currentGuard[GUARD_ALARM_SAF_AIR_FILT].guardValue == GUARD_VALUE_TRUE)
    {
        ptrFutureChild = &stateChildAlarmEmpty[3];
    }
    else if(currentGuard[GUARD_ALARM_SFV_AIR].guardValue == GUARD_VALUE_TRUE)
    {
        ptrFutureChild = &stateChildAlarmEmpty[5];
    }
    else if(currentGuard[GUARD_ALARM_SFA_AIR].guardValue == GUARD_VALUE_TRUE)
    {
        ptrFutureChild = &stateChildAlarmEmpty[7];
    }
    else if(currentGuard[GUARD_ALARM_STOP_ALL_ACTUATOR].guardValue == GUARD_VALUE_TRUE)
    {
        ptrFutureChild = &stateChildAlarmEmpty[11];
    }
    else if(currentGuard[GUARD_ALARM_MOD_BUS_ERROR].guardValue == GUARD_VALUE_TRUE)
	{
		/* si e' verificato un allarme di lettura o scrittira su modbus, fermo tutto */
		ptrFutureChild = &stateChildAlarmEmpty[13];
	}
    ResButInChildFlag = FALSE;
}

//CHILD_TREAT_ALARM_1_INIT quando lo stato principale e' STATE_EMPTY_DISPOSABLE
void manageChildEmptyAlm1InitAlways(void)
{

}

//CHILD_TREAT_ALARM_1_SAF_AIR_FILT quando lo stato principale e' STATE_EMPTY_DISPOSABLE
// Allarme aria sul filtro
void manageChildEmptyAlm1SafAirFiltEntry(void)
{
	ChildEmptyFlags.FlagsDef.SAFAirDetected = 1;
	manageChildTreatAlm1StopAllActEntry();
	ForceCurrentAlarmOff();
}

void manageChildEmptyAlm1SafAirFiltAlways(void)
{
	manageChildTreatAlm1StopAllActAlways();
}

//CHILD_TREAT_ALARM_1_SFV_AIR quando lo stato principale e' STATE_EMPTY_DISPOSABLE
// Allarme aria sul circuito venoso
void manageChildEmptyAlm1SFVEntry(void)
{
	ChildEmptyFlags.FlagsDef.SFVAirDetected = 1;
	manageChildTreatAlm1StopAllActEntry();
	ForceCurrentAlarmOff();
}

void manageChildEmptyAlm1SFVAlways(void)
{
	manageChildTreatAlm1StopAllActAlways();
}

//CHILD_TREAT_ALARM_1_SFA_AIR quando lo stato principale e' STATE_EMPTY_DISPOSABLE
// Allarme aria sul circuito arterioso
void manageChildEmptyAlm1SFAEntry(void)
{
	ChildEmptyFlags.FlagsDef.SFAAirDetected = 1;
	manageChildTreatAlm1StopAllActEntry();
	ForceCurrentAlarmOff();
}

void manageChildEmptyAlm1SFAAlways(void)
{
	manageChildTreatAlm1StopAllActAlways();
}

// Filippo - gestisco lo stop di tutti gli attuatori in modo diverso nel caso di allarme in T1 test
void manageChildT1Alm1StAllActEntry(void)
{
	manageChildTreatAlmT1StopAllActEntry();
}

void manageChildT1Alm1StAllActAlways(void)
{
	// fermo tutte le pompe e metto le pinch in sicurezza
	manageChildTreatAlmT1StopAllActAlways();
}

void manageChildIdleAlm1StAllActEntry(void)
{
	manageChildTreatAlm1StopAllActEntry();
}

void manageChildIdleAlm1StAllActAlways(void)
{
	// fermo tutte le pompe e metto le pinch in sicurezza
	manageChildTreatAlm1StopAllActAlways();
}


//CHILD_TREAT_ALARM_1_STOP_ALL_ACTUATOR quando lo stato principale e' STATE_EMPTY_DISPOSABLE
// allarme generato quando viene rilevata una pressione eccessiva su almeno 1
// dei 4 sensori
void manageChildEmptyAlm1StAllActEntry(void)
{
	manageChildTreatAlm1StopAllActEntry();
}

void manageChildEmptyAlm1StAllActAlways(void)
{
	// fermo tutte le pompe e metto le pinch in sicurezza
	manageChildTreatAlm1StopAllActAlways();
}


//CHILD_EMPTY_ALARM_MOD_BUS quando lo stato principale e' STATE_EMPTY_DISPOSABLE
// allarme generato quando viene rilevata un errore nella lettura dello stato o scrittura sulle pompe
// Devo fermare tutto. PROBABILMENTE, IN QUESTO CASO POTREBBE ESSERE MEGLIO TOGLIERE L'ENABLE ALLE POMPE
void manageChildEmptyAlmPumpNotStillEntry(void)
{
//	manageChildTreatAlm1StopAllActEntry();
	manageChildTreatAlmModBusErrEntry();
}

void manageChildEmptyAlmPumpNotStillAlways(void)
{
	manageChildTreatAlmModBusErrAlways();

//	// fermo tutte le pompe e metto le pinch in sicurezza
//	manageChildTreatAlm1StopAllActAlways();
//
//	if(buttonGUITreatment[BUTTON_RESET_ALARM].state == GUI_BUTTON_RELEASED)
//	{
//		EN_Motor_Control(ENABLE);
//		// in questo caso posso rilasciare il tasto BUTTON_RESET_ALARM perche' per uscire dallo stato
//		// di allarme PARENT_PRIMING_END_RECIRC_ALARM controllo solo
//		// currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_FALSE e non mi aspetto un comando
//		// di reset
//		//releaseGUIButton(BUTTON_RESET_ALARM);
//		EnableNextAlarmFunc(); //EnableNextAlarm = TRUE;
//	}
//	else if(buttonGUITreatment[BUTTON_OVERRIDE_ALARM].state == GUI_BUTTON_RELEASED)
//	{
//		EN_Motor_Control(ENABLE);
//		ForceCurrentAlarmOff();
//		releaseGUIButton(BUTTON_OVERRIDE_ALARM);
//		EnableNextAlarmFunc(); //EnableNextAlarm = TRUE;
//	}
}

bool IsDisposableEmptyWithAlm(void)
{
	bool DispEmpty = FALSE;
	if(GetTherapyType() == LiverTreat)
	{
		if(ChildEmptyFlags.FlagsDef.SFAAirDetected && ChildEmptyFlags.FlagsDef.SFVAirDetected && ChildEmptyFlags.FlagsDef.SAFAirDetected)
			DispEmpty = TRUE;
	}
	else if(GetTherapyType() == KidneyTreat)
	{
		if(/*ChildEmptyFlags.FlagsDef.SFAAirDetected &&*/ ChildEmptyFlags.FlagsDef.SFVAirDetected && ChildEmptyFlags.FlagsDef.SAFAirDetected)
			DispEmpty = TRUE;
	}
	return DispEmpty;
}

// Filippo - funzione che gestisce il passaggio dell'allarme per lo stato idle
void manageStateChildAlarmIdle(void)
{
	if (ptrCurrentChild->child==CHILD_TREAT_ALARM_1_STOP_ALL_ACTUATOR)
	{
		if (ptrCurrentChild->action==ACTION_ON_ENTRY)
		{
			ptrFutureChild=&stateChildAlarmT1[3];
		}
	}
}

// Filippo - funzione che gestisce il passaggio dell'allarme per lo stato idle
void manageStateChildAlarmT1(void)
{
	if (ptrCurrentChild->child==CHILD_TREAT_ALARM_1_STOP_ALL_ACTUATOR)
	{
		if (ptrCurrentChild->action==ACTION_ON_ENTRY)
		{
			ptrFutureChild=&stateChildAlarmIdle[3];
		}
	}
}

// Funzione che gestisce gli allarmi nel processo di svuotamento disposable
void ManageStateChildAlarmEmpty(void)
{
	switch(ptrCurrentChild->child){
		case CHILD_TREAT_ALARM_1_INIT:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				ptrFutureChild = &stateChildAlarmEmpty[2];
			}
			else if(ptrCurrentChild->action == ACTION_ALWAYS)
			{
			}

//            if(currentGuard[GUARD_ALARM_SAF_AIR_FILT].guardValue == GUARD_VALUE_TRUE)
//            {
//                ptrFutureChild = &stateChildAlarmEmpty[3];
//            }
//            else if(currentGuard[GUARD_ALARM_SFV_AIR].guardValue == GUARD_VALUE_TRUE)
//            {
//                ptrFutureChild = &stateChildAlarmEmpty[5];
//            }
//            else if(currentGuard[GUARD_ALARM_SFA_AIR].guardValue == GUARD_VALUE_TRUE)
//            {
//                ptrFutureChild = &stateChildAlarmEmpty[7];
//            }
			break;

		case CHILD_TREAT_ALARM_1_SAF_AIR_FILT:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				ptrFutureChild = &stateChildAlarmEmpty[4];
			}
            else if( currentGuard[GUARD_ALARM_SAF_AIR_FILT].guardValue == GUARD_VALUE_FALSE )
                ptrFutureChild = &stateChildAlarmEmpty[9]; /* FM allarme chiuso */
			else if(ptrCurrentChild->action == ACTION_ALWAYS)
			{
			}
			break;
		case CHILD_TREAT_ALARM_1_SFV_AIR:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				ptrFutureChild = &stateChildAlarmEmpty[6];
			}
            else if( currentGuard[GUARD_ALARM_SFV_AIR].guardValue == GUARD_VALUE_FALSE )
                ptrFutureChild = &stateChildAlarmEmpty[9]; /* FM allarme chiuso */
			else if(ptrCurrentChild->action == ACTION_ALWAYS)
			{
			}
			break;
		case CHILD_TREAT_ALARM_1_SFA_AIR:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				ptrFutureChild = &stateChildAlarmEmpty[8];
			}
            else if( currentGuard[GUARD_ALARM_SFA_AIR].guardValue == GUARD_VALUE_FALSE )
                ptrFutureChild = &stateChildAlarmEmpty[9]; /* FM allarme chiuso */
			else if(ptrCurrentChild->action == ACTION_ALWAYS)
			{
			}
			break;

		case CHILD_TREAT_ALARM_1_END:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				ptrFutureChild = &stateChildAlarmEmpty[10];
			}
			else if(ptrCurrentChild->action == ACTION_ALWAYS){
			}
			break;

		case CHILD_TREAT_ALARM_1_STOP_ALL_ACTUATOR:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				ptrFutureChild = &stateChildAlarmEmpty[12];
			}
            else if( currentGuard[GUARD_ALARM_STOP_ALL_ACTUATOR].guardValue == GUARD_VALUE_FALSE )
                ptrFutureChild = &stateChildAlarmEmpty[9]; /* FM allarme chiuso */
			else if(ptrCurrentChild->action == ACTION_ALWAYS)
			{
			}
			break;

		case CHILD_EMPTY_ALARM_MOD_BUS:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				ptrFutureChild = &stateChildAlarmEmpty[14];
			}
            else if( currentGuard[GUARD_ALARM_MOD_BUS_ERROR].guardValue == GUARD_VALUE_FALSE )
                ptrFutureChild = &stateChildAlarmEmpty[9]; /* FM allarme chiuso */
			else if(ptrCurrentChild->action == ACTION_ALWAYS){}
			break;

		default:
			break;
	}
}





