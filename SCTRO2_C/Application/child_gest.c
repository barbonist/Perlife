
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
}

/* Manage CHILD_PRIMING_ALARM_STOP_PELTIER always state */
void manageChildPrimAlarmStopPeltAlways(void)
{
	stopPeltierActuator();
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
}

/* Manage CHILD_TREAT_ALARM_1_STOP_PELTIER always state */
void manageChildTreatAlm1StopPeltAlways(void)
{
	stopPeltierActuator();
}


/* Manage CHILD_TREAT_ALARM_1_STOP_ALL_ACTUATOR entry state */
void manageChildTreatAlm1StopAllActEntry(void)
{
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

		stopPeltierActuator();
	}

	pumpPerist[0].entry = 0;
	pumpPerist[1].entry = 0;
	pumpPerist[2].entry = 0;
	pumpPerist[3].entry = 0;
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

		if (modbusData[PINCH_2WPVF-3][17] != MODBUS_PINCH_LEFT_OPEN)
		{
			// metto in bypass il filtro per sicurezza
			setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_LEFT_OPEN);
		}

		if (modbusData[PINCH_2WPVA-3][17] != MODBUS_PINCH_RIGHT_OPEN)
		{
			setPinchPositionHighLevel(PINCH_2WPVA, MODBUS_PINCH_RIGHT_OPEN);
		}

		if ( modbusData[PINCH_2WPVV-3][17] != MODBUS_PINCH_RIGHT_OPEN && TherType == LiverTreat)
		{
			setPinchPositionHighLevel(PINCH_2WPVV, MODBUS_PINCH_RIGHT_OPEN);
		}
		else if (modbusData[PINCH_2WPVV-3][17] != MODBUS_PINCH_LEFT_OPEN && TherType == KidneyTreat)
		{
			//in TherapyType == KidneyTreat non è usata quindi preferisco non muoverla
		}
		stopPeltierActuator();
	}

//	if((timerCounterModBus%9) == 8)
//	{
//		if(timerCounterModBus != 0)
//			timerCopy = timerCounterModBus;
//		timerCounter = 0;
//		readPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress);
//	}

	pumpPerist[0].actualSpeed =  modbusData[pumpPerist[0].pmpMySlaveAddress-2][17];
	pumpPerist[1].actualSpeed = modbusData[pumpPerist[1].pmpMySlaveAddress-2][17];
	pumpPerist[2].actualSpeed = modbusData[pumpPerist[2].pmpMySlaveAddress-2][17];
	pumpPerist[3].actualSpeed = modbusData[pumpPerist[3].pmpMySlaveAddress-2][17];
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
