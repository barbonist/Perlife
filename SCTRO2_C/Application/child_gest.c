#include "App_Ges.h"
#include "Global.h"
#include "PE_Types.h"
#include "ModBusCommProt.h"
#include "Peltier_Module.h"

#include "Pins1.h"
#include "Bit1.h"
#include "Bit2.h"
#include "BitIoLdd1.h"

#include "BitIoLdd2.h"
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


extern struct machineChild stateChildAlarmTreat1[];


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
		pumpPerist[0].dataReady = DATA_READY_FALSE;
		speed = ((BYTES_TO_WORD_SIGN(msgToRecvFrame3[3], msgToRecvFrame3[4]))/100)*(timerCopy);
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
		pumpPerist[1].dataReady = DATA_READY_FALSE;
		speed = ((BYTES_TO_WORD_SIGN(msgToRecvFrame3[3], msgToRecvFrame3[4]))/100)*(timerCopy);
	}
}


/* Manage CHILD_PRIMING_ALARM_STOP_ALL_PUMP entry state */
void manageChildPrimAlarmStopAllPumpEntry(void)
{
	if(pumpPerist[0].dataReady == DATA_READY_FALSE)
	{
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
	}
	pumpPerist[0].entry = 0;
}

/* Manage CHILD_PRIMING_ALARM_STOP_ALL_PUMP always state */
void manageChildPrimAlarmStopAllPumpAlways(void)
{
	static int speed = 0;
	static int timerCopy = 0;

	if((pumpPerist[0].dataReady == DATA_READY_FALSE) && (speed != 0))
	{
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
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
		pumpPerist[0].dataReady = DATA_READY_FALSE;
		speed = ((BYTES_TO_WORD_SIGN(msgToRecvFrame3[3], msgToRecvFrame3[4]))/100)*(timerCopy);
	}
}


/* Manage CHILD_PRIMING_ALARM_STOP_PELTIER entry state */
void manageChildPrimAlarmStopPeltEntry(void)
{
	// TODO
	stopPeltierActuator();
}

/* Manage CHILD_PRIMING_ALARM_STOP_PELTIER always state */
void manageChildPrimAlarmStopPeltAlways(void)
{
	// TODO
	stopPeltierActuator();
}


/* Manage CHILD_PRIMING_ALARM_STOP_ALL_ACTUATOR entry state */
void manageChildPrimAlarmStopAllActEntry(void)
{
	if(pumpPerist[0].dataReady == DATA_READY_FALSE)
	{
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
		setPinchPositionHighLevel(PNCHVLV1_ADDRESS, MODBUS_PINCH_POS_CLOSED);
		setPinchPositionHighLevel(PNCHVLV2_ADDRESS, MODBUS_PINCH_POS_CLOSED);
		setPinchPositionHighLevel(PNCHVLV3_ADDRESS, MODBUS_PINCH_POS_CLOSED);

		stopPeltierActuator();
	}

	pumpPerist[0].entry = 0;
}

/* Manage CHILD_PRIMING_ALARM_STOP_ALL_ACTUATOR always state */
void manageChildPrimAlarmStopAllActAlways(void)
{
	static int speed = 0;
	static int timerCopy = 0;

	if((pumpPerist[0].dataReady == DATA_READY_FALSE) && (speed != 0))
	{
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
		setPinchPositionHighLevel(PNCHVLV1_ADDRESS, MODBUS_PINCH_POS_CLOSED); // forse vanno messe in scarico e non chiuse !!!!
		setPinchPositionHighLevel(PNCHVLV2_ADDRESS, MODBUS_PINCH_POS_CLOSED);
		setPinchPositionHighLevel(PNCHVLV3_ADDRESS, MODBUS_PINCH_POS_CLOSED);
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
		speed = ((BYTES_TO_WORD_SIGN(msgToRecvFrame3[3], msgToRecvFrame3[4]))/100)*(timerCopy);

		pumpPerist[0].dataReady = DATA_READY_FALSE;
	}
}



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
}


/* Manage CHILD_TREAT_ALARM_1_STOP_PERFUSION entry state */
void manageChildTreatAlm1StopPerfEntry(void)
{
	if(pumpPerist[0].dataReady == DATA_READY_FALSE)
	{
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
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
		pumpPerist[0].dataReady = DATA_READY_FALSE;
		speed = ((BYTES_TO_WORD_SIGN(msgToRecvFrame3[3], msgToRecvFrame3[4]))/100)*(timerCopy);
	}
}

/* Manage CHILD_TREAT_ALARM_1_STOP_PURIFICATION entry state */
void manageChildTreatAlm1StopPurifEntry(void)
{
	if(pumpPerist[1].dataReady == DATA_READY_FALSE)
	{
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
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
		pumpPerist[1].dataReady = DATA_READY_FALSE;
		speed = ((BYTES_TO_WORD_SIGN(msgToRecvFrame3[3], msgToRecvFrame3[4]))/100)*(timerCopy);
	}
}


/* Manage CHILD_TREAT_ALARM_1_STOP_ALL_PUMP entry state */
void manageChildTreatAlm1StopAllPumpEntry(void)
{
	if(pumpPerist[0].dataReady == DATA_READY_FALSE)
	{
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
	}
	pumpPerist[0].entry = 0;
}

/* Manage CHILD_TREAT_ALARM_1_STOP_ALL_PUMP always state */
void manageChildTreatAlm1StopAllPumpAlways(void)
{
	static int speed = 0;
	static int timerCopy = 0;

	if((pumpPerist[0].dataReady == DATA_READY_FALSE) && (speed != 0))
	{
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
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
		pumpPerist[0].dataReady = DATA_READY_FALSE;
		speed = ((BYTES_TO_WORD_SIGN(msgToRecvFrame3[3], msgToRecvFrame3[4]))/100)*(timerCopy);
	}
}


/* Manage CHILD_TREAT_ALARM_1_STOP_PELTIER entry state */
void manageChildTreatAlm1StopPeltEntry(void)
{
	// TODO
	stopPeltierActuator();
}

/* Manage CHILD_TREAT_ALARM_1_STOP_PELTIER always state */
void manageChildTreatAlm1StopPeltAlways(void)
{
	// TODO
	stopPeltierActuator();
}


/* Manage CHILD_TREAT_ALARM_1_STOP_ALL_ACTUATOR entry state */
void manageChildTreatAlm1StopAllActEntry(void)
{
	if(pumpPerist[0].dataReady == DATA_READY_FALSE)
	{
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
		setPinchPositionHighLevel(PNCHVLV1_ADDRESS, MODBUS_PINCH_POS_CLOSED);
		setPinchPositionHighLevel(PNCHVLV2_ADDRESS, MODBUS_PINCH_POS_CLOSED);
		setPinchPositionHighLevel(PNCHVLV3_ADDRESS, MODBUS_PINCH_POS_CLOSED);

		stopPeltierActuator();
	}

	pumpPerist[0].entry = 0;
}

/* Manage CHILD_TREAT_ALARM_1_STOP_ALL_ACTUATOR always state */
void manageChildTreatAlm1StopAllActAlways(void)
{
	static int speed = 0;
	static int timerCopy = 0;

	if((pumpPerist[0].dataReady == DATA_READY_FALSE) && (speed != 0))
	{
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
		setPinchPositionHighLevel(PNCHVLV1_ADDRESS, MODBUS_PINCH_POS_CLOSED); // forse vanno messe in scarico e non chiuse !!!!
		setPinchPositionHighLevel(PNCHVLV2_ADDRESS, MODBUS_PINCH_POS_CLOSED);
		setPinchPositionHighLevel(PNCHVLV3_ADDRESS, MODBUS_PINCH_POS_CLOSED);
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
		speed = ((BYTES_TO_WORD_SIGN(msgToRecvFrame3[3], msgToRecvFrame3[4]))/100)*(timerCopy);
		pumpPerist[0].dataReady = DATA_READY_FALSE;
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
			break;

		case CHILD_TREAT_ALARM_1_STOP_PERFUSION:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				// non serve qui verra' chiamata nella ManageStateChildAlarmTreat1 dello stato principale stateState
				//ptrCurrentChild->callBackFunct();
				ptrFutureChild = &stateChildAlarmTreat1[4];
			}
            else if(currentGuard[GUARD_ALARM_STOP_PERF_PUMP].guardValue == GUARD_VALUE_FALSE )
                ptrFutureChild = &stateChildAlarmTreat1[13]; /* (FM) allarme chiuso */
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
                ptrFutureChild = &stateChildAlarmTreat1[13]; /* (FM) allarme chiuso */
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
                ptrFutureChild = &stateChildAlarmTreat1[13]; /* FM allarme chiuso */
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
                ptrFutureChild = &stateChildAlarmTreat1[13]; /* FM allarme chiuso */
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
                ptrFutureChild = &stateChildAlarmTreat1[13]; /* FM allarme chiuso */
			else if(ptrCurrentChild->action == ACTION_ALWAYS){
				// non serve qui verra' chiamata nella ManageStateChildAlarmTreat1 dello stato principale stateState
				// ptrCurrentChild->callBackFunct();
			}
			break;

		case CHILD_TREAT_ALARM_1_END:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				// non serve qui verra' chiamata nella ManageStateChildAlarmTreat1 dello stato principale stateState
				//ptrCurrentChild->callBackFunct();
				ptrFutureChild = &stateChildAlarmTreat1[14];
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
