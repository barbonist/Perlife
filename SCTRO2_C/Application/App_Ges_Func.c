

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
#include "COMP_PWM.h"
#include "HEAT_ON_C.h"


extern bool StopMotorTimeOut;

// Task di controllo della temperatura
// Se supero di 1 grado la temperatura target spengo le Peltier e le riaccendo quando
// la temperatura ha raggiunto di nuovo il target.
// Questo avviene solo quando sono in trattamento
void LiquidTempContrTask(LIQUID_TEMP_CONTR_CMD LiqTempContrCmd)
{
	static unsigned long TempOkTimeout;
	static LIQUID_TEMP_CONTR_STATE LiquidTempContrState = INIT_LIQTEMPCONTR_STATE;
	static LIQ_PELTIER_STATE PeltierState = UNDEF;
	int CurrTemp;
	int myTempValue;

//	if(ptrCurrentState->state != STATE_TREATMENT_KIDNEY_1)
//		return;

	if(LiqTempContrCmd == RESET_LIQUID_TEMP_CONTR_CMD)
	{
		LiquidTempContrState = INIT_LIQTEMPCONTR_STATE;
	}

	CurrTemp = (int)(sensorIR_TM[1].tempSensValue * 10.0);
	myTempValue = (int)parameterWordSetFromGUI[PAR_SET_PRIMING_TEMPERATURE_PERFUSION].value;
	switch (LiquidTempContrState)
	{
		// controllo del mantenimento della temperatura precedentemente raggiunta
		case INIT_LIQTEMPCONTR_STATE:
			LiquidTempContrState = READ_LIQTEMPCONTR_STATE;
			break;
		case READ_LIQTEMPCONTR_STATE:
			if(CurrTemp >= (myTempValue + DELTA_TEMP_TO_STOP_PELTIER))
			{
				if(PeltierState != OFF)
				{
					// le celle NON SONO SPENTE
					// la temperatura del reservoir e' superiore a quella impostata di almeno un grado
					// passo a controllare che questo si mantenga vero per un certo periodo di tempo
					LiquidTempContrState = CHECK_TEMP_HIGH;
					TempOkTimeout = timerCounterModBus;
				}
			}
			else if(CurrTemp <= (myTempValue + DELTA_TEMP_TO_RESTART_PELTIER))
			{
				if(PeltierState != ON)
				{
					// le celle NON SONO ACCESE
					// la temperatura e' ritornata vicino al valore target posso far ripartire le Peltier
					LiquidTempContrState = CHECK_TEMP_LOW;
					TempOkTimeout = timerCounterModBus;
				}
			}
			break;
		case CHECK_TEMP_HIGH:
			// verifico che la temperatura si mantenga al di sopra del valore di controllo per un certo
			// periodo di tempo
			if(CurrTemp >= (myTempValue + DELTA_TEMP_TO_STOP_PELTIER))
			{
				// ho raggiunto la temperatura target
				if(msTick_elapsed(TempOkTimeout) * 50L >= LIQUID_TEMP_CONTR_TASK_TIME)
				{
					// la temperatura del reservoir e' superiore a quella impostata di almeno mezzo grado
					// fermo le Peltier e mi metto in attesa che si raffreddino
					peltierCell.StopEnable = 1;
					peltierCell2.StopEnable = 1;
					PeltierState = OFF;
					LiquidTempContrState = INIT_LIQTEMPCONTR_STATE;
				}
			}
			else
			{
				LiquidTempContrState = READ_LIQTEMPCONTR_STATE;
			}
			break;
		case CHECK_TEMP_LOW:
			// verifico che la temperatura si mantenga al di sotto del valore di controllo per un certo
			// periodo di tempo
			if(CurrTemp <= (myTempValue + DELTA_TEMP_TO_RESTART_PELTIER))
			{
				// ho raggiunto la temperatura target
				if(msTick_elapsed(TempOkTimeout) * 50L >= LIQUID_TEMP_CONTR_TASK_TIME)
				{
					// la temperatura e' ritornata vicino al valore target posso far ripartire le Peltier
					// la differenza di 2 decimi di grado serve per poter far ripartire le peltier
					peltierCell.readAlwaysEnable = 0;
					peltierCell2.readAlwaysEnable = 0;
					PeltierState = ON;
					LiquidTempContrState = INIT_LIQTEMPCONTR_STATE;
				}
			}
			else
			{
				LiquidTempContrState = READ_LIQTEMPCONTR_STATE;
			}
			break;
	}
}




bool IsPumpStopAlarmActive(void)
{
	CHECK_PUMP_STOP_STATE st;
	st = CheckPumpStopTask((CHECK_PUMP_STOP_CMD)NO_CHECK_PUMP_READ_ALM_CMD);
	if((st == PUMP_WRITE_ALARM) || StopMotorTimeOut)
		return TRUE;
	else
		return FALSE;
}

void ClearPumpStopAlarm(void)
{
	CheckPumpStopTask((CHECK_PUMP_STOP_CMD)RESET_ALARM);
}

// Ritorna TRUE se tutte le 4 pompe sono ferme
bool AreAllPumpsStopped( void )
{
	if(PumpStoppedCnt >= 3)
		return TRUE;
	else
		return FALSE;
}

int debugVal = 0;
// cmd comando per eventuare riposizionamento della macchina a stati
CHECK_PUMP_STOP_STATE CheckPumpStopTask(CHECK_PUMP_STOP_CMD cmd)
{
	static CHECK_PUMP_STOP_STATE CheckPumpStopTaskMach = CHECK_PUMP_STOP_IDLE;
	static int Delay = 0;
	static int CheckPumpStopCnt = 0;
	char PompeInMovimento = 0;

	if(cmd == INIT_CHECK_SEQ_CMD)
	{
		CheckPumpStopTaskMach = WAIT_FOR_NEW_READ;
		Delay = 0;
		CheckPumpStopCnt = 0;
		DisableCheckPumpStopTask = 0;
		PumpStoppedCnt = 0;
		StopMotorTimeOut = FALSE;
		return CheckPumpStopTaskMach;
	}
	else if(cmd == RESET_ALARM)
	{
		// lo metto in uno stato di inattivita'
		CheckPumpStopTaskMach = CHECK_PUMP_STOP_IDLE;
		StopMotorTimeOut = FALSE;
		return CheckPumpStopTaskMach;
	}
	else if(cmd == NO_CHECK_PUMP_READ_ALM_CMD)
	{
		return CheckPumpStopTaskMach;
	}

	// dal momento in cui viene fatto un accesso alle pompe da service, questo task non viene piu'
	// eseguito. Verra' ripristinato alla ricezione del primo comando INIT_CHECK_SEQ_CMD
	if(DisableCheckPumpStopTask)
		return CheckPumpStopTaskMach;

	if(!( ((ptrCurrentState->state == STATE_TREATMENT_KIDNEY_1) &&
	      ((ptrCurrentParent->parent == PARENT_TREAT_WAIT_START) || (ptrCurrentParent->parent == PARENT_TREAT_WAIT_PAUSE))) ||
		  ((ptrCurrentState->state == STATE_PRIMING_RICIRCOLO) && (ptrCurrentParent->parent == PARENT_PRIM_WAIT_MOT_STOP)) ||
		  (ptrCurrentState->state == STATE_IDLE) ||
		  (ptrCurrentState->state == STATE_UNMOUNT_DISPOSABLE) ||
		  ((ptrCurrentState->state == STATE_PRIMING_PH_1) && (ptrCurrentParent->parent == PARENT_PRIM_WAIT_PAUSE)) ||
		  ((ptrCurrentState->state == STATE_PRIMING_PH_2) && (ptrCurrentParent->parent == PARENT_PRIM_WAIT_PAUSE))
		))
	{
		// se il task viene chiamato in uno stato non corretto
		return CheckPumpStopTaskMach;
	}

	switch (CheckPumpStopTaskMach)
	{
		case CHECK_PUMP_STOP_IDLE:
			break;

		case WAIT_FOR_NEW_READ:
			// in questo stato ci va all'inizio del priming ed in attesa di iniziare la fase di ricircolo
			Delay++;
			if(Delay > 20)
			{
				// aspetto 1 secondo poi vado a controllare le velocita' delle pompe
				CheckPumpStopTaskMach = READ_PUMP_SPEED;
				Delay = 0;
			}
			break;

		case READ_PUMP_SPEED:
			PompeInMovimento = 0;
			if(GetTherapyType() == KidneyTreat)
			{
				for(int i = 0; i <= 3; i++)
				{
					if(i == 1)
						continue;
					if(modbusData[i][17] != 0)
					{
						// la pompa non e' ferma
						PompeInMovimento = 1;
						break;
					}
				}

				if(PompeInMovimento)
				{
					setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, (int)0);
					setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, (int)0);
					//setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, (int)0);
					CheckPumpStopCnt++;
				}
			}
			else if((GetTherapyType() == LiverTreat))
			{
				for(int i = 0; i <= 3; i++)
				{
					if(modbusData[i][17] != 0)
					{
						// la pompa non e' ferma
						PompeInMovimento = 1;
						break;
					}
				}

				if(PompeInMovimento)
				{
					// l'if che segue puo' essere inserito, insieme al codice alla linea 1606,
					// solo se devo controllare in debug l'allarme di pompe che non si
					// fermano alla fine del ricircolo
					//if(debugVal) // solo per debug. DA TOGLIERE !!!!
					{
					setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, (int)0);
					setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, (int)0);
					setPumpSpeedValueHighLevel(pumpPerist[3].pmpMySlaveAddress, (int)0);
					}
					CheckPumpStopCnt++;
				}
			}
			if(PompeInMovimento)
			{
				if(CheckPumpStopCnt >= 3)
				{
					// ho superato il numero massimo di tentativi senza successo, devo generare un allarme
					CheckPumpStopTaskMach = PUMP_WRITE_ALARM;
				}
				else
				{
					CheckPumpStopTaskMach = WAIT_FOR_NEW_READ;
					Delay = 0;
				}
				PumpStoppedCnt = 0;
			}
			else
			{
				CheckPumpStopTaskMach = WAIT_FOR_NEW_READ;
				Delay = 0;
				CheckPumpStopCnt = 0;
				PumpStoppedCnt++;
			}
			break;

		case PUMP_WRITE_ALARM:
			break;

		case END_PROCESS:
			break;
	}
	return CheckPumpStopTaskMach;
}

// ritorna TRUE quando sono in uno stato dove la struttura parent prevede la gestione dell'allarme
// QUESTA FUZIONE DOVREBBE ESSERE AGGIORNATA OGNI VOLTA CHE SI AGGIUNGE UN NUOVO STATO CHE PREVEDE
// LA GESTIONE DEGLI ALLARMI
bool AmJInAlarmHandledState(void)
{
	bool InAlarmState = FALSE;
	if((ptrCurrentParent->parent == PARENT_EMPTY_DISPOSABLE_INIT) ||
	   (ptrCurrentParent->parent == PARENT_EMPTY_DISPOSABLE_RUN) ||
	   (ptrCurrentParent->parent == PARENT_PRIMING_TREAT_KIDNEY_1_INIT) ||
	   (ptrCurrentParent->parent == PARENT_PRIMING_TREAT_KIDNEY_1_RUN) ||
	   (ptrCurrentParent->parent == PARENT_PRIM_WAIT_MOT_STOP) ||
	   (ptrCurrentParent->parent == PARENT_PRIM_WAIT_PINCH_CLOSE) ||
	   (ptrCurrentParent->parent == PARENT_PRIM_KIDNEY_1_AIR_FILT) ||
	   (ptrCurrentParent->parent == PARENT_TREAT_KIDNEY_1_INIT) ||
	   (ptrCurrentParent->parent == PARENT_TREAT_KIDNEY_1_PUMP_ON) ||
	   (ptrCurrentParent->parent == PARENT_TREAT_KIDNEY_1_AIR_FILT) ||
	   (ptrCurrentParent->parent == PARENT_TREAT_KIDNEY_1_SFV) ||
	   (ptrCurrentParent->parent == PARENT_TREAT_KIDNEY_1_SFA)
	   )
		InAlarmState = TRUE;
	return InAlarmState;
}

// ritorna TRUE quando sono in uno stato di gestione allarme (le pompe e pinch sono gia' in sicurezza)
// QUESTA FUZIONE DOVREBBE ESSERE AGGIORNATA OGNI VOLTA CHE SI AGGIUNGE UN NUOVO STATO PER LA GESTIONE
// DI ALLARMI
bool AmJInAlarmState(void)
{
	bool InAlarmState = FALSE;
	if((ptrCurrentParent->parent == PARENT_EMPTY_DISPOSABLE_ALARM) ||
	   (ptrCurrentParent->parent == PARENT_TREAT_KIDNEY_1_ALM_AIR_REC) ||
	   (ptrCurrentParent->parent == PARENT_TREAT_KIDNEY_1_ALARM) ||
	   (ptrCurrentParent->parent == PARENT_PRIM_KIDNEY_1_ALM_AIR_REC) ||
	   (ptrCurrentParent->parent == PARENT_PRIMING_END_RECIRC_ALARM) ||
	   (ptrCurrentParent->parent == PARENT_PRIMING_TREAT_KIDNEY_1_ALARM))
		InAlarmState = TRUE;
	return InAlarmState;
}

// Se sono in uno stato non di allarme ma l'allarme verso la GUI e' diverso da 0, allora
// forzo annullamento allarme perche' e' una situazione di errore.
// Vuol dire che per un qualche motivo il reset dell'allarme non ha funzionato
void CheckAlarmForGuiStateMsg(void)
{
	if(!AmJInAlarmState())
	{
		if(alarmCurrent.code)
			memset(&alarmCurrent, 0, sizeof(struct alarm));
	}
}

// ritorna TRUE se lo stato child ha bisogno del button reset per fare qualcosa
// QUESTA FUZIONE DOVREBBE ESSERE AGGIORNATA OGNI VOLTA CHE SI AGGIUNGE UN NUOVO STATO PER LA GESTIONE
// DI ALLARMI DOVE LO STATO CHILD HA BISOGNO DI UN BUTTON RESET
bool IsButtResUsedByChild(void)
{
	bool ButtResUsedByChild = FALSE;
	if((ptrCurrentChild->child == CHILD_PRIM_ALARM_1_WAIT_CMD) ||
	   (ptrCurrentChild->child == CHILD_PRIM_ALARM_PUMPS_NOT_STILL) ||
	   (ptrCurrentChild->child == CHILD_PRIM_ALARM_BAD_PINCH_POS) ||
	   (ptrCurrentChild->child == CHILD_PRIM_ALARM_SFA_AIR_DET) ||
	   (ptrCurrentChild->child == CHILD_PRIM_ALARM_MOD_BUS) ||
	   (ptrCurrentChild->child == CHILD_TREAT_ALARM_1_SAF_AIR_FILT) ||
	   (ptrCurrentChild->child == CHILD_TREAT_ALARM_1_SFA_AIR) ||
	   (ptrCurrentChild->child == CHILD_TREAT_ALARM_1_SFV_AIR) ||
	   (ptrCurrentChild->child == CHILD_TREAT_ALARM_1_WAIT_CMD) ||
	   (ptrCurrentChild->child == CHILD_TREAT_ALARM_BAD_PINCH_POS) ||
	   (ptrCurrentChild->child == CHILD_TREAT_ALARM_MOD_BUS_ERROR) ||
	   (ptrCurrentChild->child == CHILD_EMPTY_ALARM_MOD_BUS))
	{
		ButtResUsedByChild = TRUE;
	}
	return ButtResUsedByChild;
}



//-------------------------------------------------------------------------------------------------------------------------
extern bool FilterSelected;

// controlla il posizionamento delle pinch nei vari stati
CHECK_CURR_PINCH_POS_TASK_STATE CheckCurrPinchPosTask(CHECK_CURR_PINCH_POS_TASK_CMD cmd)
{
	static CHECK_CURR_PINCH_POS_TASK_STATE CheckCurrPinchPosTaskState = T_SET_PINCH_IDLE;
	static unsigned char CorrectPosCnt = 0;
	static unsigned char WrongPosCnt = 0;
	static unsigned char PinchPos[3] = {0xff, 0xff, 0xff};
	static int DelayCnt = 0;
	bool TreatCurrPinchPosOkFlag = TRUE;

	if( cmd == CHECK_CURR_PINCH_POS_INIT_CMD)
	{
		// metto la macchina nella posizione di inizio controllo posizione pinch
		CheckCurrPinchPosTaskState = CHK_PINCH_POS_INIT;
	}
	else if( cmd == CHECK_CURR_PINCH_POS_DISABLE_CMD)
	{
		// disabilito la macchina a stati che controlla il corretto posizionamento delle pinch
		CheckCurrPinchPosTaskState = CHK_PINCH_POS_IDLE;
		return CheckCurrPinchPosTaskState;
	}
	else if( cmd == CHECK_CURR_PINCH_POS_READ_CMD)
	{
		// ritorno lo stato in modo che posso vedere se sono in allarme
		return CheckCurrPinchPosTaskState;
	}
	else if( cmd == CHECK_CURR_PINCH_POS_RESET_ALARM_CMD)
	{
		// se ero in allarme ripristino il controllo della posizione
		if(CheckCurrPinchPosTaskState == CHK_PINCH_POS_ALARM)
			CheckCurrPinchPosTaskState = CHK_PINCH_POS_INIT;
		return CheckCurrPinchPosTaskState;
	}

	switch (CheckCurrPinchPosTaskState)
	{
		case CHK_PINCH_POS_IDLE:
			break;
		case CHK_PINCH_POS_INIT:
			if((ptrCurrentParent->parent == PARENT_TREAT_KIDNEY_1_INIT) || (ptrCurrentParent->parent == PARENT_TREAT_KIDNEY_1_PUMP_ON))
			{
				WrongPosCnt = 0;
				CorrectPosCnt = 0;
				PinchPos[0] = (FilterSelected) ? MODBUS_PINCH_RIGHT_OPEN : MODBUS_PINCH_LEFT_OPEN;
				PinchPos[1] = MODBUS_PINCH_LEFT_OPEN;
				PinchPos[2] = MODBUS_PINCH_LEFT_OPEN;
				CheckCurrPinchPosTaskState = CHK_PINCH_POS_RUN;
			}
			else if(((ptrCurrentState->state == STATE_PRIMING_PH_1) || (ptrCurrentState->state == STATE_PRIMING_PH_2)) &&
					((ptrCurrentParent->parent == PARENT_PRIMING_TREAT_KIDNEY_1_RUN) || (ptrCurrentParent->parent == PARENT_PRIMING_TREAT_KIDNEY_1_ALARM)))
			{
				WrongPosCnt = 0;
				CorrectPosCnt = 0;
				PinchPos[0] = (FilterSelected) ? MODBUS_PINCH_RIGHT_OPEN : MODBUS_PINCH_LEFT_OPEN;
				PinchPos[1] = PRIM_PINCH_2WPVA_POS; // aperta a destra
				PinchPos[2] = PRIM_PINCH_2WPVV_POS; // aperta a destra
				CheckCurrPinchPosTaskState = CHK_PINCH_POS_PRIM_DELAY;
				DelayCnt = 0;
			}
			else if((ptrCurrentState->state == STATE_PRIMING_RICIRCOLO) &&
					((ptrCurrentParent->parent == PARENT_PRIMING_TREAT_KIDNEY_1_RUN) || (ptrCurrentParent->parent == PARENT_PRIMING_TREAT_KIDNEY_1_ALARM)))
			{
				WrongPosCnt = 0;
				CorrectPosCnt = 0;
				PinchPos[0] = (FilterSelected) ? MODBUS_PINCH_RIGHT_OPEN : MODBUS_PINCH_LEFT_OPEN;
				PinchPos[1] = MODBUS_PINCH_LEFT_OPEN; // aperta a sinistra
				PinchPos[2] = MODBUS_PINCH_LEFT_OPEN; // aperta a sinistra
				CheckCurrPinchPosTaskState = CHK_PINCH_POS_PRIM_DELAY;
				DelayCnt = 0;
			}
			break;
		case CHK_PINCH_POS_RUN:
			if(IsPinchPosOk(PinchPos))
			{
				WrongPosCnt = 0;
				CorrectPosCnt++;
				if(CorrectPosCnt >= 3)
				{
					// pinch posizionata correttamente
					TreatCurrPinchPosOkFlag = TRUE;
					CheckCurrPinchPosTaskState = CHK_PINCH_POS_RESTART;
				}
			}
			else
			{
				CorrectPosCnt = 0;
				WrongPosCnt++;
				if(WrongPosCnt >= 3)
				{
					// pinch posizionata in modo errato
					TreatCurrPinchPosOkFlag = FALSE;
					CheckCurrPinchPosTaskState = CHK_PINCH_POS_ALARM;
				}
			}
			break;
		case CHK_PINCH_POS_RESTART:
			WrongPosCnt = 0;
			CorrectPosCnt = 0;
			CheckCurrPinchPosTaskState = CHK_PINCH_POS_RUN;
			break;
		case CHK_PINCH_POS_ALARM:
			break;
		case CHK_PINCH_POS_PRIM_DELAY:
			// aspetto un po' per dare il tempo alle pinch di posizionarsi prima
			// iniziare a fare il controllo
			DelayCnt++;
			if(DelayCnt >= 40)
			{
				// sono passati 2 secondi le pinch dovrebbero aver terminato il posizionamento
				// impostato con il comando start priming. Posso cominciare a fare
				// il controllo con la protective
				CheckCurrPinchPosTaskState = CHK_PINCH_POS_RUN;
			}
			break;
	}
	return TreatCurrPinchPosOkFlag;
}

// ritorna TRUE se la posizione e' corretta
bool IsTreatCurrPinchPosOk(void)
{
	bool TreatCurrPinchPosOkFlag;
	CHECK_CURR_PINCH_POS_TASK_STATE st = CheckCurrPinchPosTask((CHECK_CURR_PINCH_POS_TASK_CMD)CHECK_CURR_PINCH_POS_READ_CMD);
	if(st == CHK_PINCH_POS_ALARM)
		TreatCurrPinchPosOkFlag = FALSE;
	else
		TreatCurrPinchPosOkFlag = TRUE;
	return TreatCurrPinchPosOkFlag;
}

// fa uscire la macchina a stati dalla posizione di allarme e la rimette nello
// stato di controllo della posizione
void ResetTreatCurrPinchPosOk(void)
{
	CHECK_CURR_PINCH_POS_TASK_STATE st = CheckCurrPinchPosTask((CHECK_CURR_PINCH_POS_TASK_CMD)CHECK_CURR_PINCH_POS_READ_CMD);
	if(st == CHK_PINCH_POS_ALARM)
		CheckCurrPinchPosTask((CHECK_CURR_PINCH_POS_TASK_CMD)CHECK_CURR_PINCH_POS_INIT_CMD);
}


//-------------------------------------------------------------------------------------------------------------------------
// Questo task puo' essere usato per controllare la risposta su modbus.
// Viene ritornato uno dei valori:
// 	MOD_BUS_ANSW_NO_ANSW  = risposta non ancora completa
//  MOD_BUS_ANSW_OK       = risposta completa e corretta (anche il CRC)
//  MOD_BUS_ANSW_CRC_ERR  = risposta completa ma CRC errato
//  MOD_BUS_ANSW_TOUT_ERR = risposta non completa entro 20 msec
MOD_BUS_RESPONSE WaitForModBusResponseTask(WAIT_FOR_MB_RESP_TASK_CMD WaitForMBRespTskCmd)
{
	static WAIT_FOR_MB_RESP_TASK_STATE WaitForModBusResponseTaskSt = WAIT_MB_RESP_TASK_IDLE;
	static unsigned char *ptr_msg = 0;
	static uint32_t startTimeInterv = 0;
	static int CrcLen = 0;
	word CRC_RX,CRC_CALC;
	MOD_BUS_RESPONSE Ok = MOD_BUS_ANSW_NO_ANSW;


	if(WaitForMBRespTskCmd == WAIT_MB_RESP_TASK_RESET_CMD)
	{
		// controllo la risposta del comando di lettura pompe lean
		WaitForModBusResponseTaskSt = WAIT_MB_RESP_TASK_IDLE;
	}
	switch (WaitForModBusResponseTaskSt)
	{
		case WAIT_MB_RESP_TASK_IDLE:
			WaitForModBusResponseTaskSt = WAIT_MB_RESP_TASK_INIT_GENERIC;
			break;
		case WAIT_MB_RESP_TASK_INIT_GENERIC:
			iFlag_actuatorCheck = IFLAG_COMMAND_SENT;
			WaitForModBusResponseTaskSt = WAIT_MB_RESP_TASK_RUN_3;
			ptr_msg = _funcRetValPtr->slvresRetPtr;
			startTimeInterv = FreeRunCnt10msec;
			CrcLen = _funcRetValPtr->slvresRetNumByte;
			break;

		case WAIT_MB_RESP_TASK_RUN_3:
			if(iFlag_actuatorCheck == IFLAG_COMMAND_RECEIVED)
			{
				// ho ricevuto la risposta su MODBUS
				CRC_CALC = ComputeChecksum(ptr_msg, CrcLen - 2);
				CRC_RX = BYTES_TO_WORD(ptr_msg[CrcLen - 1], ptr_msg[CrcLen - 2]);
				iFlag_actuatorCheck = IFLAG_IDLE;
				WaitForModBusResponseTaskSt = WAIT_MB_RESP_TASK_IDLE;
				if (CRC_RX != CRC_CALC)
					Ok = MOD_BUS_ANSW_CRC_ERR;
				else
					Ok = MOD_BUS_ANSW_OK;
			}
			else if(startTimeInterv && (msTick10_elapsed(startTimeInterv) >= 2))
			{
				// sono passati 20 msec e non ho avuto ancora risposta, restituisco errore
				Ok = MOD_BUS_ANSW_TOUT_ERR;
			}
			break;
	}
	return Ok;
}


//-------------------------------------------------------------------------------------------------------------------------

void EnableAllTempAlarm(void)
{
	GlobalFlags.FlagsDef.EnableDeltaTempRecArtAlarm = 1;
}

void DisableAllTempAlarm(void)
{
	GlobalFlags.FlagsDef.EnableDeltaTempRecArtAlarm = 0;
}

void EnableTempAlarm(unsigned char code)
{
	switch (code)
	{
		case CODE_ALARM_DELTA_TEMP_REC_ART:
			GlobalFlags.FlagsDef.EnableDeltaTempRecArtAlarm = 1;
			break;
	}
}

void DisableTempAlarm(unsigned char code)
{
	switch (code)
	{
		case CODE_ALARM_DELTA_TEMP_REC_ART:
			GlobalFlags.FlagsDef.EnableDeltaTempRecArtAlarm = 0;
			break;
	}
}



// Questo task controlla l'abilitazione degli allarmi di temperatura
void TempAlarmEnableTask(TEMP_ALARM_EN_TASK_CMD cmd)
{
	static TEMP_ALARM_EN_TASK_STATE TempAlarmEnTaskSt = T_AL_EN_IDLE;
	static unsigned long timeInterval10 = 0;
	static word OldTempTrgt = 0;
	static bool TrgtChgd = FALSE;
	static bool PumpChangedState = FALSE;
	static bool ResetReceived = FALSE;
//	static unsigned long timeIntrv = 0;
//	static CHECK_PUMP_STATE ChkPumpState = CHK_PUMP_SPEED_IDLE;
//	static PUMP_STATE PumpState;
	word temp_trgt = parameterWordSetFromGUI[PAR_SET_PRIMING_TEMPERATURE_PERFUSION].value;
	float tmpr = ((int)(sensorIR_TM[1].tempSensValue * 10));

	if(OldTempTrgt != temp_trgt)
	{
		OldTempTrgt = temp_trgt;
		TrgtChgd = 1;
	}

	// controllo lo stato della pompa
//	switch (ChkPumpState)
//	{
//		case CHK_PUMP_SPEED_IDLE:
//			ChkPumpState = CHK_PUMP_SPEED;
//			break;
//		case CHK_PUMP_SPEED:
//			if(pumpPerist[0].actualSpeed && PumpState == PUMP_STOPPED)
//				ChkPumpState = CHK_PUMP_SPEED_ON;
//			else if(!pumpPerist[0].actualSpeed && PumpState == PUMP_MOVING)
//				ChkPumpState = CHK_PUMP_SPEED_OFF;
//			timeIntrv = FreeRunCnt10msec;
//			break;
//		case CHK_PUMP_SPEED_ON:
//			if(pumpPerist[0].actualSpeed)
//			{
//				if(timeIntrv && (msTick10_elapsed(timeIntrv) >= 300))
//				{
//					// in movimento da 3 secondi
//					PumpState = PUMP_MOVING;
//					ChkPumpState = CHK_PUMP_SPEED;
//					PumpChangedState = TRUE;
//				}
//			}
//			break;
//		case CHK_PUMP_SPEED_OFF:
//			if(!pumpPerist[0].actualSpeed)
//			{
//				if(timeIntrv && (msTick10_elapsed(timeIntrv) >= 300))
//				{
//					// in movimento da 3 secondi
//					PumpState = PUMP_STOPPED;
//					ChkPumpState = CHK_PUMP_SPEED;
//					PumpChangedState = TRUE;
//				}
//			}
//			break;
//	}

	switch (TempAlarmEnTaskSt)
	{
		case T_AL_EN_IDLE:
			// aspetto di entrare nello stato di trattamento
			if(ptrCurrentState->state == STATE_TREATMENT_KIDNEY_1)
			{
				TempAlarmEnTaskSt = T_AL_EN_TREAT_DELAY;
				timeInterval10 = FreeRunCnt10msec;
			}
			break;

		case T_AL_EN_TREAT_DELAY:
			if(timeInterval10 && (msTick10_elapsed(timeInterval10) >= DELAY_FOR_START_TEMP_AL_CHECK))
			{
				TempAlarmEnTaskSt = T_AL_EN_RUN;
			}
			break;

		case T_AL_EN_RUN:
			if(GlobalFlags.FlagsDef.EnableDeltaTempRecArtAlarm && IsAlarmCodeActive((unsigned char)CODE_ALARM_DELTA_TEMP_REC_ART))
				break; // l'allarme e' gia' attivo
			else
			{
				if(ResetReceived)
				{
					ResetReceived = FALSE;
					GlobalFlags.FlagsDef.EnableDeltaTempRecArtAlarm = 0;
					TempAlarmEnTaskSt = T_AL_EN_WAIT_INTERVAL;
					timeInterval10 = FreeRunCnt10msec;
				}
				else if(TrgtChgd)
				{
					TrgtChgd = FALSE;
					GlobalFlags.FlagsDef.EnableDeltaTempRecArtAlarm = 0;
					TempAlarmEnTaskSt = T_AL_EN_WAIT_TARGET;
				}
				else if(PumpChangedState)
				{
					PumpChangedState = FALSE;
					if(pumpPerist[0].actualSpeed)
					{
						// si e' messa in movimento abilito l'allarme
						GlobalFlags.FlagsDef.EnableDeltaTempRecArtAlarm = 1;
					}
					else
						GlobalFlags.FlagsDef.EnableDeltaTempRecArtAlarm = 0;
				}
			}
			break;

		case T_AL_EN_WAIT_TARGET:
			// aspetto che la temperatura nel reservoir raggiunga il nuovo target
			// prima di riprendere il controllo di temperatura
			if((tmpr >= (float)(temp_trgt - DELTA_TEMP_TARGET)) && (tmpr <= (float)(temp_trgt + DELTA_TEMP_TARGET)))
			{
				// ho raggiunto la temperatura target
				TempAlarmEnTaskSt = T_AL_EN_WAIT_TARGET_1;
				timeInterval10 = FreeRunCnt10msec;
			}
			break;

		case T_AL_EN_WAIT_TARGET_1:
			if((tmpr >= (float)(temp_trgt - DELTA_TEMP_TARGET)) && (tmpr <= (float)(temp_trgt + DELTA_TEMP_TARGET)))
			{
				// nel reservoir ho raggiunto la temperatura target da 2 secondi
				if(timeInterval10 && (msTick10_elapsed(timeInterval10) >= 200))
				{
					GlobalFlags.FlagsDef.EnableDeltaTempRecArtAlarm = 1;
					TempAlarmEnTaskSt = T_AL_EN_RUN;
				}
			}
			else
				TempAlarmEnTaskSt = T_AL_EN_WAIT_TARGET;
			break;

		case T_AL_EN_WAIT_INTERVAL:
			// aspetto un po di tempo prima di riprendere il controllo di temperatura
			if(timeInterval10 && (msTick10_elapsed(timeInterval10) >= DIS_TEMP_ALM_TIME_AFTER_RESET))
			{
				GlobalFlags.FlagsDef.EnableDeltaTempRecArtAlarm = 1;
				TempAlarmEnTaskSt = T_AL_EN_RUN;
			}
			break;
	}
}




//-------------------------------------------------------------------------------------------------------------------------

// abilitazione all'accensione del riscaldatore da parte del task di controllo
// della temperatura massima del piatto
bool EnableHeatingFromPlate = FALSE;
// abilitazione all'accensione del riscaldatore da parte del task di controllo
bool EnableHeatingFromControl = FALSE;
bool EnableFrigoFromPlate = FALSE;
bool EnableFrigoFromControl = FALSE;
int HeatingPwmPerc = 0;

/*funzione che accende con una data potenza o spegne il frigo con potenza '0'
 * power = 10 --> 200 Hz max
 * power = 40 --> 50 Hz --> 25%*/
// ritorna TRUE se il frigo e' partito
bool Start_Frigo_AMS(float DeltaT)
{
	int DeltaTInt;
	unsigned char power = 10;
	bool StartFlag = FALSE;

	if(!(EnableFrigoFromPlate && EnableFrigoFromControl))
		return StartFlag;

	if(DeltaT < 0.0)
	{
		// la temperatura da corrente e' inferiore a quella target
		// Impossibile con il raffreddamento
		power = 0;
	}
	else if(DeltaT == 0.0)
	{
		power = 0;  // spento
	}
	else
	{
		// la temperatura  da raggiungere e' inferiore a quella corrente, quindi
		// devo accendere il frigo con una potenza che dipende dal DeltaT
		DeltaTInt = DeltaT / 10.0 + 0.5;
		if(DeltaTInt >= 12)
			power = 10;           // massima potenza
		else if(DeltaTInt >= 10)
			power = 15;
		else if(DeltaTInt >= 8)
			power = 20;
		else if(DeltaTInt >= 6)
			power = 25;
		else if(DeltaTInt >= 4)
			power = 30;
		else if(DeltaTInt >= 2)
			power = 35;
		else
			power = 40;           // minima potenza
	}

	if (power == 0)
	{
		Enable_AMS = FALSE;
	    COMP_PWM_ClrVal();
	    StartFlag = FALSE;
	}
	else
	{
		Enable_AMS = TRUE;

		Prescaler_Freq_Signal_AMS = power;
		/* a power = 10 corrisponde la massima frequenza pari
		 * a 200 Hz quindi non posso andare sotto 10*/
		if (power <= 10)
			power = 10;
		StartFlag = TRUE;
	}
	return StartFlag;
}

void StopFrigo(void)
{
	Start_Frigo_AMS((float)0.0);
}


// Perc  0..100
void HeatingPwm(int Perc)
{
	static int OldPerc = 0;
	static int TOn = 0;
	static int TOff = 0;
	static unsigned long timeIntervalHeating = 0;
	static HEAT_PWM_STATE HeatingPwmState = HEAT_PWM_ALWAYS_OFF;

	if(!(EnableHeatingFromPlate && EnableHeatingFromControl))
		return;

	if(OldPerc != Perc)
	{
		OldPerc = Perc;
		if(Perc == 100)
		{
			HEAT_ON_C_SetVal();
			HeatingPwmState = HEAT_PWM_ALWAYS_ON;
		}
		else if(Perc == 0)
		{
			HEAT_ON_C_ClrVal();
			HeatingPwmState = HEAT_PWM_ALWAYS_OFF;
		}
		else
		{
			TOn = Perc;
			TOff = 100 - Perc;
			if((HeatingPwmState == HEAT_PWM_ALWAYS_OFF) || (HeatingPwmState == HEAT_PWM_ALWAYS_ON))
				HeatingPwmState = HEAT_PWM_IDLE;
		}
	}

	switch (HeatingPwmState)
	{
		case HEAT_PWM_ALWAYS_OFF:
			break;
		case HEAT_PWM_IDLE:
			timeIntervalHeating = FreeRunCnt10msec;
			HeatingPwmState = HEAT_PWM_ON;
			HEAT_ON_C_SetVal();
			break;
		case HEAT_PWM_ON:
			// On
			if(timeIntervalHeating && (msTick10_elapsed(timeIntervalHeating) >= TOn))
			{
				HEAT_ON_C_ClrVal();
				HeatingPwmState = HEAT_PWM_OFF;
				timeIntervalHeating = FreeRunCnt10msec;
			}
			break;
		case HEAT_PWM_OFF:
			// off
			if(timeIntervalHeating && (msTick10_elapsed(timeIntervalHeating) >= TOff))
			{
				HEAT_ON_C_SetVal();
				HeatingPwmState = HEAT_PWM_ON;
				timeIntervalHeating = FreeRunCnt10msec;
			}
			break;
		case HEAT_PWM_ALWAYS_ON:
			break;
	}
}

// percentuale di potenza con cui faccio partire il riscaldatore
// temperatura corrente - temperatura target in decimi di grado
// ritorna TRUE se il riscaldatore e' partito
bool StartHeating(float DeltaT)
{
	bool StartHeatingFlag = FALSE;
	int DeltaTInt;

	// fa partire le resistenze riscaldanti
	if(!(EnableHeatingFromPlate && EnableHeatingFromControl))
		return StartHeatingFlag;

	if(DeltaT > 0.0)
	{
		// la temperatura da raggiungere e' inferiore a quella corrente
		// Impossibile con il riscaldamento
		HeatingPwmPerc = 0;
	}
	else if(DeltaT == 0.0)
	{
		HeatingPwmPerc = 0;
		HeatingPwm((int)0); // spento
	}
	else
	{
		// la temperatura  da raggiungere e' superiore a quella corrente, quindi
		// devo accendere il riscaldatore con un pwm che dipende dal DeltaT
		// DeltaT in questo caso e' sicuramente negativa
		DeltaT = -DeltaT;
		DeltaTInt = DeltaT / 10.0 + 0.5;
		if(DeltaTInt >= 9)
			HeatingPwmPerc = 90; // 90%
		else if(DeltaTInt >= 8)
			HeatingPwmPerc = 80; // 80%
		else if(DeltaTInt >= 7)
			HeatingPwmPerc = 70; // 70%
		else if(DeltaTInt >= 6)
			HeatingPwmPerc = 60; // 60%
		else if(DeltaTInt >= 5)
			HeatingPwmPerc = 50; // 50%
		else if(DeltaTInt >= 4)
			HeatingPwmPerc = 40; // 40%
		else if(DeltaTInt >= 3)
			HeatingPwmPerc = 30; // 30%
		else if(DeltaTInt >= 2)
			HeatingPwmPerc = 20; // 20%
		else
			HeatingPwmPerc = 10; // 10%
		HeatingPwmPerc = 10;
		HeatingPwm((int)10); // 10 %
		StartHeatingFlag = TRUE;
	}
	return StartHeatingFlag;
}

// spengo il riscaldatore
void StopHeating(void)
{
	HEAT_ON_C_ClrVal();
}


void PlateStateFrigo(void)
{
	static unsigned long timeIntervalPlate10 = 0;
	static PLATE_STATE PlateState = PLATE_STATE_IDLE;

	switch (PlateState)
	{
		case PLATE_STATE_IDLE:
			timeIntervalPlate10 = FreeRunCnt10msec;
			EnableFrigoFromPlate = TRUE;
			PlateState = PLATE_STATE_DIS_FRIGO;
			break;
		case PLATE_STATE_DIS_FRIGO:
			// controllo disabilitazione raffreddamento
			if(T_PLATE_C_GRADI_CENT < MIN_PLATE_TEMP)
			{
				if(timeIntervalPlate10 && (msTick10_elapsed(timeIntervalPlate10) >= 300))
				{
					StopFrigo();
					EnableFrigoFromPlate = FALSE;
					PlateState = PLATE_STATE_EN_FRIGO;
					timeIntervalPlate10 = FreeRunCnt10msec;
				}
			}
			else
				timeIntervalPlate10 = FreeRunCnt10msec;
			break;
		case PLATE_STATE_EN_FRIGO:
			// controllo abilitazione raffreddamento
			if(T_PLATE_C_GRADI_CENT > MIN_PLATE_TEMP)
			{
				if(timeIntervalPlate10 && (msTick10_elapsed(timeIntervalPlate10) >= 300))
				{
					EnableFrigoFromPlate = TRUE;
					PlateState = PLATE_STATE_DIS_FRIGO;
					timeIntervalPlate10 = FreeRunCnt10msec;
				}
			}
			else
				timeIntervalPlate10 = FreeRunCnt10msec;
			break;
	}
}

void PlateStateHeating(void)
{
	static unsigned long timeIntervalPlate10_r = 0;
	static PLATE_HEAT_STATE PlateStateHeating = PLATE_HEAT_IDLE;

	switch (PlateStateHeating)
	{
		case PLATE_HEAT_IDLE:
			timeIntervalPlate10_r = FreeRunCnt10msec;
			PlateStateHeating = PLATE_HEAT_DIS;
			EnableHeatingFromPlate = TRUE;
			break;
		case PLATE_HEAT_DIS:
			// controllo disabilitazione riscaldamento, raffreddamento
			if(T_PLATE_C_GRADI_CENT > MAX_PLATE_TEMP)
			{
				if(timeIntervalPlate10_r && (msTick10_elapsed(timeIntervalPlate10_r) >= 300))
				{
					StopHeating();
					EnableHeatingFromPlate = FALSE;
					PlateStateHeating = PLATE_HEAT_EN;
					timeIntervalPlate10_r = FreeRunCnt10msec;
				}
			}
			else
				timeIntervalPlate10_r = FreeRunCnt10msec;
			break;
		case PLATE_HEAT_EN:
			// controllo abilitazione riscaldamento, raffreddamento
			if(T_PLATE_C_GRADI_CENT < MAX_PLATE_TEMP)
			{
				if(timeIntervalPlate10_r && (msTick10_elapsed(timeIntervalPlate10_r) >= 300))
				{
					EnableHeatingFromPlate = TRUE;
					PlateStateHeating = PLATE_HEAT_DIS;
					timeIntervalPlate10_r = FreeRunCnt10msec;
				}
			}
			else
				timeIntervalPlate10_r = FreeRunCnt10msec;
			break;
	}
}

// mi consente di inserire o disinserire il controllo della temperatura in runtime per passare
// da riscaldamento a frigo
bool EnableRunTimeCheckTemp = FALSE;

// task di controllo della temperatura del liquido mediante frigo e resistenze riscaldanti
void FrigoHeatTempControlTask(LIQ_TEMP_CONTR_TASK_CMD LiqTempContrTaskCmd)
{
	static LIQ_TEMP_CONTR_TASK_STATE LiqTempContrTaskSt = LIQ_T_CONTR_IDLE;
	static unsigned long timeInterval10 = 0;
	static unsigned long timeInterval10_r = 0;
	static unsigned long timeInterval10_max = 0;
	static word OldTargetTemp = 0;

	float tmpr, MaxThrsh, MinThrsh;
	word tmpr_trgt;
	// temperatura raggiunta dal reservoir
	tmpr = ((int)(sensorIR_TM[1].tempSensValue*10));
	// temperatura da raggiungere moltiplicata per 10
	tmpr_trgt = parameterWordSetFromGUI[PAR_SET_PRIMING_TEMPERATURE_PERFUSION].value;

	PlateStateHeating();
	PlateStateFrigo();
	// gestisco accensione e spegnimento alimentazione resistenza di riscaldamento
	// se e' abilitata
	HeatingPwm(HeatingPwmPerc);

	//return;    // per ora
	if(OldTargetTemp != tmpr_trgt)
	{
		// se il target di temperatura cambia mi riporto nello stato di
		// scelta tra frigo o riscaldamento
		OldTargetTemp = tmpr_trgt;
		if(EnableRunTimeCheckTemp)
		{
			if(LiqTempContrTaskSt > LIQ_T_CONTR_CHECK_TEMP)
			{
				// forzo di nuovo il controllo frigo o riscaldatore
				LiqTempContrTaskSt = LIQ_T_CONTR_CHECK_TEMP;
			}
		}
	}

	// Se sto per iniziare una nuova fase di priming mi metto in attesa di rilevare la presenza del liquido
	// nel disposable
	if((ptrCurrentState->state < STATE_PRIMING_PH_1) && (LiqTempContrTaskSt != LIQ_T_CONTR_DETECT_LIQ_IN_DISP))
		LiqTempContrTaskSt = LIQ_T_CONTR_IDLE;

	switch (LiqTempContrTaskSt)
	{
		case LIQ_T_CONTR_IDLE:
			LiqTempContrTaskSt = LIQ_T_CONTR_DETECT_LIQ_IN_DISP;
			timeInterval10 = FreeRunCnt10msec;
			EnableHeatingFromPlate = FALSE;
			EnableHeatingFromControl = FALSE;
			EnableFrigoFromControl = FALSE;
			EnableFrigoFromPlate = FALSE;
			HeatingPwmPerc = 0;
			StopHeating();
			StopFrigo();
			break;

		// rilevo la presenza del liquido nel disposable
		case LIQ_T_CONTR_DETECT_LIQ_IN_DISP:
			if((Air_1_Status == LIQUID) && (ptrCurrentState->state >= STATE_PRIMING_PH_1))
			{
				if(timeInterval10 && (msTick10_elapsed(timeInterval10) >= 300))
				{
					// sono passati 3 secondi con rilevazione continua di liquido
					LiqTempContrTaskSt = LIQ_T_CONTR_WAIT_FOR_START_CNTR;
					timeInterval10 = FreeRunCnt10msec;
				}
			}
			else
				timeInterval10 = FreeRunCnt10msec;
			break;

		// ritardo dopo il rilevamento del liquido nel disposable prima di attivare
		// la fase di controllo della temperatura del reservoir
		case LIQ_T_CONTR_WAIT_FOR_START_CNTR:
			if(timeInterval10 && (msTick10_elapsed(timeInterval10) >= DELAY_FOR_START_T_CONTROL))
			{
				// e' trascorso del tempo da quando ho rilevato la presenza del liquido
				LiqTempContrTaskSt = LIQ_T_CONTR_CHECK_TEMP;
				timeInterval10 = FreeRunCnt10msec;
				timeInterval10_r = FreeRunCnt10msec;
			}
			break;

		// controllo la temperatura attuale per decidere riscaldamento o raffreddamento
		case LIQ_T_CONTR_CHECK_TEMP:
			EnableHeatingFromControl = FALSE;
			EnableFrigoFromControl = FALSE;
			StopHeating();
			StopFrigo();
			if(tmpr > (float)tmpr_trgt)
			{
				if(timeInterval10 && (msTick10_elapsed(timeInterval10) >= FRIGO_DELAY))
				{
					// temperatura superiore al target usero' sempre il frigo
					LiqTempContrTaskSt = LIQ_T_CONTR_RUN_FRIGO;
					timeInterval10 = FreeRunCnt10msec;
					timeInterval10_max = FreeRunCnt10msec;
					EnableFrigoFromControl = TRUE;
				}
			}
			else
				timeInterval10 = FreeRunCnt10msec;
			if(tmpr < (float)tmpr_trgt)
			{
				if(timeInterval10_r && (msTick10_elapsed(timeInterval10_r) >= FRIGO_DELAY))
				{
					// temperatura inferiore al target usero' sempre il riscaldatore
					LiqTempContrTaskSt = LIQ_T_CONTR_RUN_HEATING;
					timeInterval10_r = FreeRunCnt10msec;
					timeInterval10_max = FreeRunCnt10msec;
					EnableHeatingFromControl = TRUE;
				}
			}
			else
				timeInterval10_r = FreeRunCnt10msec;
			break;

		// stati di controllo del frigo
		case LIQ_T_CONTR_RUN_FRIGO:
			if(tmpr >= ((float)tmpr_trgt + (float)DELTA_TEMP_FOR_FRIGO_ON))
			{
				if(timeInterval10 && (msTick10_elapsed(timeInterval10) >= FRIGO_DELAY))
				{
					if(Start_Frigo_AMS(tmpr - (float)tmpr_trgt))
					{
						LiqTempContrTaskSt = LIQ_T_CONTR_WAIT_STOP_FRIGO;
						timeInterval10 = FreeRunCnt10msec;
					}
				}
			}
			else
				timeInterval10 = FreeRunCnt10msec;

			if(EnableRunTimeCheckTemp)
			{
				if(timeInterval10_max && (msTick10_elapsed(timeInterval10_max) >= MAX_TEMP_CNTRL_DELAY))
				{
					// E' trascorso troppo tempo senza una ripartenza del frigo vado a controllare se la
					// selezione del frigo e' corretta
					LiqTempContrTaskSt = LIQ_T_CONTR_CHECK_TEMP;
					timeInterval10 = FreeRunCnt10msec;
					timeInterval10_r = FreeRunCnt10msec;
				}
			}
			break;
		case LIQ_T_CONTR_WAIT_STOP_FRIGO:
			if(tmpr <= ((float)tmpr_trgt + (float)DELTA_TEMP_FOR_FRIGO_OFF))
			{
				if(timeInterval10 && (msTick10_elapsed(timeInterval10) >= HEATING_DELAY))
				{
					// fermo il raffreddamento
					StopFrigo();
					LiqTempContrTaskSt = LIQ_T_CONTR_RUN_FRIGO;
					timeInterval10_max = FreeRunCnt10msec;
				}
			}
			else
				timeInterval10 = FreeRunCnt10msec;
			break;


		// stati di controllo del riscaldamento
		case LIQ_T_CONTR_RUN_HEATING:
			if(tmpr <= ((float)tmpr_trgt  + (float)DELTA_TEMP_FOR_RESISTOR_ON))
			{
				if(timeInterval10_r && (msTick10_elapsed(timeInterval10_r) >= HEATING_DELAY))
				{
					// faccio partire il riscaldamento
					StartHeating(tmpr - (float)tmpr_trgt);
					LiqTempContrTaskSt = LIQ_T_CONTR_WAIT_STOP_HEATING;
					timeInterval10_r = FreeRunCnt10msec;
				}
			}
			else
				timeInterval10_r = FreeRunCnt10msec;
			if(EnableRunTimeCheckTemp)
			{
				if(timeInterval10_max && (msTick10_elapsed(timeInterval10_max) >= MAX_TEMP_CNTRL_DELAY))
				{
					// E' trascorso troppo tempo senza una ripartenza del riscaldatore vado a controllare se la
					// selezione del frigo e' corretta
					LiqTempContrTaskSt = LIQ_T_CONTR_CHECK_TEMP;
					timeInterval10 = FreeRunCnt10msec;
					timeInterval10_r = FreeRunCnt10msec;
				}
			}
			break;
		case LIQ_T_CONTR_WAIT_STOP_HEATING:
			if(tmpr >= ((float)tmpr_trgt  + (float)DELTA_TEMP_FOR_RESISTOR_OFF))
			{
				if(timeInterval10_r && (msTick10_elapsed(timeInterval10_r) >= HEATING_DELAY))
				{
					// fermo il riscaldamento
					StopHeating();
					LiqTempContrTaskSt = LIQ_T_CONTR_RUN_HEATING;
					timeInterval10_max = FreeRunCnt10msec;
				}
			}
			else
				timeInterval10_r = FreeRunCnt10msec;
			break;
	}
}

