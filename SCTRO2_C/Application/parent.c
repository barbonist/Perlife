/*
 * parent.c
 *
 *  Created on: 18/apr/2018
 *      Author: franco mazzoni
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



void ParentFunc(void)
{
	static unsigned short ParentStateGenAlarm = 0;  // mantiene il valore dello stato parent che ha generato l'allarme

// ------------------GESTIONE STATI PARENT PRIMING-------------------------------------------------------------------
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
				//ptrCurrentParent->callBackFunct(); NON SERVE QUESTO
				/* compute future parent */
				ptrFutureParent = &stateParentPrimingTreatKidney1[2];
				ptrFutureChild = ptrFutureParent->ptrChild;
			}
			else if(ptrCurrentParent->action == ACTION_ALWAYS)
			{
				//ptrCurrentParent->callBackFunct(); NON SERVE QUESTO
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
			// per sicurezza resetto la flag di reset alarm premuto, nel caso mi fosse rimasto settato
			releaseGUIButton(BUTTON_RESET_ALARM);
			break;

		case PARENT_PRIMING_TREAT_KIDNEY_1_RUN:
			if(ptrCurrentParent->action == ACTION_ON_ENTRY)
			{
				/* execute parent callback function */
				// ptrCurrentParent->callBackFunct(); NON SERVE QUESTO
				/* compute future parent */
				ptrFutureParent = &stateParentPrimingTreatKidney1[4];
				ptrFutureChild = ptrFutureParent->ptrChild;
			}
			else if(ptrCurrentParent->action == ACTION_ALWAYS)
			{
				// ptrCurrentParent->callBackFunct(); NON SERVE QUESTO
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
			else if(currentGuard[GUARD_ENT_PAUSE_STATE_PRIM_KIDNEY_1].guardValue == GUARD_VALUE_TRUE)
			{
				// VADO NELLO STATO DI PAUSA DEL PRIMING RUN
				ptrFutureParent = &stateParentPrimingTreatKidney1[9];
				ptrFutureChild = ptrFutureParent->ptrChild;
				currentGuard[GUARD_ENT_PAUSE_STATE_PRIM_KIDNEY_1].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				currentGuard[GUARD_ENT_PAUSE_STATE_PRIM_KIDNEY_1].guardValue = GUARD_VALUE_FALSE;
			}
			else if(currentGuard[GUARD_CHK_FOR_ALL_MOT_STOP].guardValue == GUARD_VALUE_TRUE)
			{
				// VADO NELLO STATO DI CONTROLLO CHE TUTTE LE POMPE SIANO FERME
				ptrFutureParent = &stateParentPrimingTreatKidney1[11];
				ptrFutureChild = ptrFutureParent->ptrChild;
				currentGuard[GUARD_CHK_FOR_ALL_MOT_STOP].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				currentGuard[GUARD_CHK_FOR_ALL_MOT_STOP].guardValue = GUARD_VALUE_FALSE;
			}
			// per sicurezza resetto la flag di reset alarm premuto, nel caso mi fosse rimasto settato
			releaseGUIButton(BUTTON_RESET_ALARM);
			break;

		case PARENT_PRIMING_TREAT_KIDNEY_1_ALARM:
			if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_FALSE)
			{
				if((currentGuard[GUARD_ALARM_PRIM_AIR_FILT_RECOVERY].guardValue == GUARD_VALUE_TRUE) ||
				   ((buttonGUITreatment[BUTTON_RESET_ALARM].state == GUI_BUTTON_RELEASED) && TreatAlm1SafAirFiltActive))
				{
					if((buttonGUITreatment[BUTTON_RESET_ALARM].state == GUI_BUTTON_RELEASED))
					{
						releaseGUIButton(BUTTON_RESET_ALARM);
						EnableNextAlarmFunc(); //EnableNextAlarm = TRUE;
						LevelBuzzer = 0;
					}
					// ho ricevuto un BUTTON_RESET_ALARM e l'allarme era ancora attivo oppure
					// ho ricevuto un BUTTON_RESET_ALARM dopo che l'allarme e' stato cancellato perche' la condizione
					// fisica di allarme e' venuta meno
					TreatAlm1SafAirFiltActive = FALSE;
					// vado nello stato parent dove posso cercare di recuperare la condizione di allarme
					currentGuard[GUARD_ALARM_PRIM_AIR_FILT_RECOVERY].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
					currentGuard[GUARD_ALARM_PRIM_AIR_FILT_RECOVERY].guardValue = GUARD_VALUE_FALSE;
					AirParentState = PARENT_TREAT_KIDNEY_1_AIR_FILT;
					StarTimeToRejAir = timerCounterModBus;
					TotalTimeToRejAir = 0;
					/* (FM) passo nello stato per la risoluzione dell'allarme aria nella linea arteriosa*/
					ptrFutureParent = &stateParentPrimingTreatKidney1[17];
					ptrFutureChild = ptrFutureParent->ptrChild;
				}
				else if(currentGuard[GUARD_ALARM_WAIT_CMD_TO_EXIT].guardValue == GUARD_VALUE_TRUE)
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

					if( (perfusionParam.priVolPerfArt == 0) && ((ptrCurrentState->state == STATE_PRIMING_PH_1) || (ptrCurrentState->state == STATE_PRIMING_PH_2)))
					{
						// mi e' arrivato un segnale di troppo pieno ancora prima di cominciare il priming
						// forzo l'uscita dal priming e vado subito in ricircolo
						perfusionParam.priVolPerfArt = MAX_LIQUID_AMOUNT;
					}

					// forzo anche una pressione del tasto BUTTON_PRIMING_END_CONFIRM per fare in modo che
					// il riempimento termini subito e si vada alla fase di riciclo
					setGUIButton(BUTTON_PRIMING_END_CONFIRM);
				}
				else if(buttonGUITreatment[BUTTON_RESET_ALARM].state == GUI_BUTTON_RELEASED)
				{
					// Il ritorno al priming viene fatto solo dopo la pressione del tasto BUTTON_RESET_ALARM
					releaseGUIButton(BUTTON_RESET_ALARM);
					EnableNextAlarmFunc(); //EnableNextAlarm = TRUE;
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
			else if(buttonGUITreatment[BUTTON_RESET_ALARM].state == GUI_BUTTON_RELEASED)
			{
				// ho ricevuto un reset allarme ma la condizione fisica di allarme non e' ancora andata via
				//  se lo stato child non e' in SECURITY_STOP_ALL_ACTUATOR il tasto reset potrebbe servire a lui per
				// disabilitare l'allarme.
				if(!IsButtResUsedByChild())
				{
					releaseGUIButton(BUTTON_RESET_ALARM);
					// qui non serve perche' l'allarme non e' ancora terminato
					//EnableNextAlarmFunc(); //EnableNextAlarm = TRUE;
					ptrFutureParent = &stateParentPrimingTreatKidney1[3];
					ptrFutureChild = ptrFutureParent->ptrChild;
					LevelBuzzer = 0;
				}
				break;
			}

			if(ptrCurrentParent->action == ACTION_ON_ENTRY)
			{
				/* execute parent callback function */
				//ptrCurrentParent->callBackFunct(); NON SERVE QUESTO
				/* compute future parent */
				ptrFutureParent = &stateParentPrimingTreatKidney1[6];
				ptrFutureChild = ptrFutureParent->ptrChild;
			}
			else if(ptrCurrentParent->action == ACTION_ALWAYS)
			{
				//ptrCurrentParent->callBackFunct(); NON SERVE QUESTO
			}
			break;

		case PARENT_PRIMING_TREAT_KIDNEY_1_END:
			break;

		case PARENT_PRIM_WAIT_PAUSE:
			if(currentGuard[GUARD_ENABLE_STATE_KIDNEY_1_PRIM_RUN].guardValue == GUARD_VALUE_TRUE)
			{
				ptrFutureParent = &stateParentPrimingTreatKidney1[3];
				ptrFutureChild = ptrFutureParent->ptrChild;
				// forzo anche una pressione del tasto TREATMENT START per fare in modo che
				// il trattamento riprenda automaticamente
				setGUIButton(BUTTON_START_PRIMING);
				currentGuard[GUARD_ENABLE_STATE_KIDNEY_1_PRIM_RUN].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				currentGuard[GUARD_ENABLE_STATE_KIDNEY_1_PRIM_RUN].guardValue = GUARD_VALUE_FALSE;
				break;
			}
			if(ptrCurrentParent->action == ACTION_ON_ENTRY)
			{
				/* compute future parent */
				/* FM passo alla gestione ACTION_ALWAYS */
				ptrFutureParent = &stateParentPrimingTreatKidney1[10];
				ptrFutureChild = ptrFutureParent->ptrChild;
			}
			else if(ptrCurrentParent->action == ACTION_ALWAYS)
			{
			}
			break;

		// STATO PER LA GESTIONE DELL'ATTESA DELLE POMPE FERME. SE NON SI FERMANO
		// VERRA' GENERATO UN ALLARME GESTITO NELLO STATO PARENT_PRIMING_END_RECIRC_ALARM
		case PARENT_PRIM_WAIT_MOT_STOP:
			if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_TRUE)
			{
				ptrFutureParent = &stateParentPrimingTreatKidney1[15];
				ptrFutureChild = ptrFutureParent->ptrChild;
				LevelBuzzer = 2;
				ParentStateGenAlarm = PARENT_PRIM_WAIT_MOT_STOP;
				break;
			}
			else if(currentGuard[GUARD_EN_CLOSE_ALL_PINCH].guardValue == GUARD_VALUE_TRUE)
			{
				// tutte le pompe sono ferme posso passare alla chiusura delle pinch prima di collegare l'organo
				ptrFutureParent = &stateParentPrimingTreatKidney1[13];
				ptrFutureChild = ptrFutureParent->ptrChild;
				currentGuard[GUARD_EN_CLOSE_ALL_PINCH].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				currentGuard[GUARD_EN_CLOSE_ALL_PINCH].guardValue = GUARD_VALUE_FALSE;
				break;
			}

			if(ptrCurrentParent->action == ACTION_ON_ENTRY)
			{
				/* compute future parent */
				/* FM passo alla gestione ACTION_ALWAYS */
				ptrFutureParent = &stateParentPrimingTreatKidney1[12];
				ptrFutureChild = ptrFutureParent->ptrChild;
			}
			else if(ptrCurrentParent->action == ACTION_ALWAYS)
			{
			}
			break;
		// STATO PER LA GESTIONE DELL'ATTESA DELLE PINCH TUTTE CHIUSE. sE NON SI DOVESSERO CHIUDERE VERRA'
		// GENERATO UN ALLARME GESTITO NELLO STATO PARENT_PRIMING_END_RECIRC_ALARM
		case PARENT_PRIM_WAIT_PINCH_CLOSE:
			if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_TRUE)
			{
				ptrFutureParent = &stateParentPrimingTreatKidney1[15];
				ptrFutureChild = ptrFutureParent->ptrChild;
				LevelBuzzer = 2;
				ParentStateGenAlarm = PARENT_PRIM_WAIT_PINCH_CLOSE;
			}
			if(ptrCurrentParent->action == ACTION_ON_ENTRY)
			{
				/* compute future parent */
				/* FM passo alla gestione ACTION_ALWAYS */
				ptrFutureParent = &stateParentPrimingTreatKidney1[14];
				ptrFutureChild = ptrFutureParent->ptrChild;
			}
			else if(ptrCurrentParent->action == ACTION_ALWAYS)
			{
			}
			break;
		case PARENT_PRIMING_END_RECIRC_ALARM:
			if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_FALSE)
			{
				// Il BUTTON_RESET_ALARM e' stato usato nelle funzioni child per resettare
				// l'allarme e proseguire
				//if(buttonGUITreatment[BUTTON_RESET_ALARM].state == GUI_BUTTON_RELEASED)
				{
					// Il ritorno allo stato di partenza del priming viene fatto solo dopo la pressione del tasto BUTTON_RESET_ALARM
					//releaseGUIButton(BUTTON_RESET_ALARM);
					EnableNextAlarmFunc(); //EnableNextAlarm = TRUE;
					if(ParentStateGenAlarm == PARENT_PRIM_WAIT_PINCH_CLOSE)
					{
						ptrFutureParent = &stateParentPrimingTreatKidney1[13];
						ptrFutureChild = ptrFutureParent->ptrChild;
					}
					else if(ParentStateGenAlarm == PARENT_PRIM_WAIT_MOT_STOP)
					{
						ptrFutureParent = &stateParentPrimingTreatKidney1[11];
						ptrFutureChild = ptrFutureParent->ptrChild;
					}
					LevelBuzzer = 0;
				}
			}

			if(ptrCurrentParent->action == ACTION_ON_ENTRY)
			{
				/* compute future parent */
				ptrFutureParent = &stateParentPrimingTreatKidney1[16];
				ptrFutureChild = ptrFutureParent->ptrChild;
			}
			else if(ptrCurrentParent->action == ACTION_ALWAYS){}
			break;

			// SE DURANTE IL PRIMING CON FILTRO ABILITATO SI VERIFICA UN ALLARME ARIA SUL FILTRO SI
			// ARRIVA IN QUESTO STATO DOVE VIENE ATTIVATA LA PROCEDURA DI ESPULSIONE DELL'ARIA E CONTEMPORANEAMENTE
			// VENGONO CONTROLLATI EVENTUALI ALLARMI DI PRESSIONE NEL CASO LE LINEE SIANO OTTURATE

			// STATI PER LA GESTIONE DELLA PROCEDURA DI RIMOZIONE DELL'ARIA DAL CIRCUITO -----------------------
			case PARENT_PRIM_KIDNEY_1_AIR_FILT:
				if(currentGuard[GUARD_AIR_RECOVERY_END].guardValue == GUARD_VALUE_TRUE)
				{
					currentGuard[GUARD_AIR_RECOVERY_END].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
					currentGuard[GUARD_AIR_RECOVERY_END].guardValue = GUARD_VALUE_FALSE;
					// ho rimosso l'aria dal circuito, posso ritornare nello stato di allarme in cui sono partito
					// oppure nello stato di lavoro normale (dopo aver riabilitato gli allarmi).
					// Decido di ripartire dalla fase iniziale del primin.
					DisablePrimAirAlarm(FALSE);
					ptrFutureParent = &stateParentPrimingTreatKidney1[3];
					ptrFutureChild = ptrFutureParent->ptrChild;
					setGUIButton(BUTTON_START_PRIMING);
					break;
				}

				if(ptrCurrentParent->action == ACTION_ON_ENTRY)
				{
					/* compute future parent */
					/* FM passo alla gestione ACTION_ALWAYS */
					ptrFutureParent = &stateParentPrimingTreatKidney1[18];
					ptrFutureChild = ptrFutureParent->ptrChild;
				}
				else if(ptrCurrentParent->action == ACTION_ALWAYS)
				{
				}

				if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_TRUE)
				{
					/* (FM) si e' verificato un allarme, durante la procedura di recupero dell'allarme aria.
					 * Per la sua gestione uso un nuovo stato 19  */
					ptrFutureParent = &stateParentPrimingTreatKidney1[19];
					ptrFutureChild = ptrFutureParent->ptrChild;
					// guardando a questo valore posso vedere il tipo di azione di sicurezza
					// e quindi posso decidere di andare anche in un qualche altro stato ad hoc
					// di allarme
					//ptrAlarmCurrent->secActType
					//TotalTimeToRejAir += msTick_elapsed(StarTimeToRejAir);
				}
				break;
			case PARENT_PRIM_KIDNEY_1_ALM_AIR_REC:
				if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_FALSE)
				{
					/* FM allarme finito posso ritornare nello stato di partenza
					 * quando si e' verificato l'allarme */
					if(AirParentState == PARENT_TREAT_KIDNEY_1_AIR_FILT)
					{
						// ero nello stato recupero da allarme aria nel sensore aria on/off
						ptrFutureParent = &stateParentPrimingTreatKidney1[17];
						ptrFutureChild = ptrFutureParent->ptrChild;
						StarTimeToRejAir = timerCounterModBus;
						break;
					}
					else
					{
						ptrFutureParent = &stateParentPrimingTreatKidney1[3];
						ptrFutureChild = ptrFutureParent->ptrChild;
						break;
					}
				}

				if(ptrCurrentParent->action == ACTION_ON_ENTRY)
				{
					/* compute future parent */
					/* (FM) passo alla gestione ACTION_ALWAYS dell'allarme */
					ptrFutureParent = &stateParentPrimingTreatKidney1[20];
					ptrFutureChild = ptrFutureParent->ptrChild;
				}
				else if(ptrCurrentParent->action == ACTION_ALWAYS)
				{
					// (FM) chiamo la funzione child che gestisce lo stato di allarme
					//ManageStateChildAlarmTreat1();
					// LA FUNZIONE CHILD CHE GESTISCE LO STATO DI ALLARME, IN QUESTO CASO, SI TROVA NELLA FUNZIONE
					// ProcessMachineState
				}
				break;
			//---------------------------------------------------------------------------------------------



// ------------------GESTIONE STATI PARENT TRATTAMENTO-------------------------------------------------------------------
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
				//ptrCurrentParent->callBackFunct(); NON SERVE QUESTO
				/* compute future parent */
				/* FM entro nello stato in cui l'azione e' di tipo ACTION_ALWAYS */
				ptrFutureParent = &stateParentTreatKidney1[2];
				ptrFutureChild = ptrFutureParent->ptrChild;
			}
			else if(ptrCurrentParent->action == ACTION_ALWAYS)
			{
				//ptrCurrentParent->callBackFunct(); NON SERVE QUESTO
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
			else if(currentGuard[GUARD_ENT_PAUSE_STATE_TREAT_KIDNEY_1_INIT].guardValue == GUARD_VALUE_TRUE)
			{
				ptrFutureParent = &stateParentTreatKidney1[17];
				ptrFutureChild = ptrFutureParent->ptrChild;
				currentGuard[GUARD_ENT_PAUSE_STATE_TREAT_KIDNEY_1_INIT].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				currentGuard[GUARD_ENT_PAUSE_STATE_TREAT_KIDNEY_1_INIT].guardValue = GUARD_VALUE_FALSE;
			}
			// per sicurezza resetto la flag di reset alarm premuto, nel caso mi fosse rimasto settato
			releaseGUIButton(BUTTON_RESET_ALARM);
			break;

		case PARENT_TREAT_KIDNEY_1_PUMP_ON:
			if(ptrCurrentParent->action == ACTION_ON_ENTRY)
			{
				/* execute parent callback function */
				//ptrCurrentParent->callBackFunct(); NON SERVE QUESTO
				/* compute future parent */
				/* FM passo alla gestione ACTION_ALWAYS */
				ptrFutureParent = &stateParentTreatKidney1[4];
				ptrFutureChild = ptrFutureParent->ptrChild;
			}
			else if(ptrCurrentParent->action == ACTION_ALWAYS)
			{
				//ptrCurrentParent->callBackFunct(); NON SERVE QUESTO
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
			else if(currentGuard[GUARD_ENT_PAUSE_STATE_KIDNEY_1_PUMP_ON].guardValue == GUARD_VALUE_TRUE)
			{
				ptrFutureParent = &stateParentTreatKidney1[19];
				ptrFutureChild = ptrFutureParent->ptrChild;
				currentGuard[GUARD_ENT_PAUSE_STATE_KIDNEY_1_PUMP_ON].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				currentGuard[GUARD_ENT_PAUSE_STATE_KIDNEY_1_PUMP_ON].guardValue = GUARD_VALUE_FALSE;
			}
			// per sicurezza resetto la flag di reset alarm premuto, nel caso mi fosse rimasto settato
			releaseGUIButton(BUTTON_RESET_ALARM);
			break;

		case PARENT_TREAT_KIDNEY_1_ALARM:
			if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_FALSE)
			{
				bool ButtonResetRcvd = FALSE;
				if(buttonGUITreatment[BUTTON_RESET_ALARM].state == GUI_BUTTON_RELEASED)
				{
					releaseGUIButton(BUTTON_RESET_ALARM);
					EnableNextAlarmFunc(); //EnableNextAlarm = TRUE;
					ButtonResetRcvd = TRUE;
					LevelBuzzer = 0;
					// preparo la macchina a stati per il controllo delle pinch aperte nella posizione richiesta per lo stato di trattamento
					TreatSetPinchPosTask((TREAT_SET_PINCH_POS_CMD)T_SET_PINCH_RESET_CMD);
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
			else if(buttonGUITreatment[BUTTON_RESET_ALARM].state == GUI_BUTTON_RELEASED)
			{
				// ho ricevuto un reset allarme ma la condizione fisica di allarme non e' ancora andata via.
				// Se lo stato child non e' in SECURITY_STOP_ALL_ACTUATOR il tasto reset potrebbe servire a lui per
				// disabilitare l'allarme.
				if(!IsButtResUsedByChild())
				{
					releaseGUIButton(BUTTON_RESET_ALARM);
					// qui non serve perche' l'allarme non e' ancora terminato
					//EnableNextAlarmFunc(); //EnableNextAlarm = TRUE;
					ptrFutureParent = &stateParentTreatKidney1[1];
					ptrFutureChild = ptrFutureParent->ptrChild;
					LevelBuzzer = 0;

					// preparo la macchina a stati per il controllo delle pinch aperte nella posizione richiesta
					// per lo stato di trattamento
					TreatSetPinchPosTask((TREAT_SET_PINCH_POS_CMD)T_SET_PINCH_RESET_CMD);
					// forzo anche una pressione del tasto TREATMENT START per fare in modo che
					// il trattamento riprenda automaticamente
					setGUIButton(BUTTON_START_TREATMENT);
				}
				break;
			}

			if(ptrCurrentParent->action == ACTION_ON_ENTRY)
			{
				/* execute parent callback function */
				//ptrCurrentParent->callBackFunct(); NON SERVE QUESTO
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
				//ptrCurrentParent->callBackFunct(); NON SERVE QUESTO
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


		case PARENT_TREAT_WAIT_START:
			if(currentGuard[GUARD_ENABLE_STATE_TREAT_KIDNEY_1_INIT].guardValue == GUARD_VALUE_TRUE)
			{
				ptrFutureParent = &stateParentTreatKidney1[1];
				ptrFutureChild = ptrFutureParent->ptrChild;
				// forzo anche una pressione del tasto TREATMENT START per fare in modo che
				// il trattamento riprenda automaticamente
				setGUIButton(BUTTON_START_TREATMENT);
				// preparo la macchina a stati per il controllo delle pinch aperte nella posizione richiesta per lo stato di trattamento
				TreatSetPinchPosTask((TREAT_SET_PINCH_POS_CMD)T_SET_PINCH_RESET_CMD);
				currentGuard[GUARD_ENABLE_STATE_TREAT_KIDNEY_1_INIT].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				currentGuard[GUARD_ENABLE_STATE_TREAT_KIDNEY_1_INIT].guardValue = GUARD_VALUE_FALSE;
				break;
			}
			if(ptrCurrentParent->action == ACTION_ON_ENTRY)
			{
				/* compute future parent */
				/* FM passo alla gestione ACTION_ALWAYS */
				ptrFutureParent = &stateParentTreatKidney1[18];
				ptrFutureChild = ptrFutureParent->ptrChild;
			}
			else if(ptrCurrentParent->action == ACTION_ALWAYS)
			{
			}
			break;

		case PARENT_TREAT_WAIT_PAUSE:
			if(currentGuard[GUARD_ENABLE_STATE_KIDNEY_1_PUMP_ON].guardValue == GUARD_VALUE_TRUE)
			{
				ptrFutureParent = &stateParentTreatKidney1[3];
				ptrFutureChild = ptrFutureParent->ptrChild;
				// forzo anche una pressione del tasto TREATMENT START per fare in modo che
				// il trattamento riprenda automaticamente
				setGUIButton(BUTTON_START_TREATMENT);
				// preparo la macchina a stati per il controllo delle pinch aperte nella posizione richiesta per lo stato di trattamento
				TreatSetPinchPosTask((TREAT_SET_PINCH_POS_CMD)T_SET_PINCH_RESET_CMD);
				currentGuard[GUARD_ENABLE_STATE_KIDNEY_1_PUMP_ON].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				currentGuard[GUARD_ENABLE_STATE_KIDNEY_1_PUMP_ON].guardValue = GUARD_VALUE_FALSE;
				break;
			}
			if(ptrCurrentParent->action == ACTION_ON_ENTRY)
			{
				/* compute future parent */
				/* FM passo alla gestione ACTION_ALWAYS */
				ptrFutureParent = &stateParentTreatKidney1[20];
				ptrFutureChild = ptrFutureParent->ptrChild;
			}
			else if(ptrCurrentParent->action == ACTION_ALWAYS)
			{
			}
			break;

		default:
			break;
	}

}



void ParentFuncT1Test(void)
{
#ifdef DEBUG_T1_TEST
	switch(ptrCurrentParent->parent){
	case PARENT_T1_NO_DISP_INIT:
		if(currentGuard[GUARD_ENABLE_T1_CONFIG].guardValue == GUARD_VALUE_TRUE)
		{
		ptrFutureParent = &stateParentT1TNoDisposable[3];
		ptrFutureChild = ptrFutureParent->ptrChild;
		DebugStringStr("parent chk config");
		break;
		}

		if(ptrCurrentParent->action == ACTION_ON_ENTRY)
		{
		ptrFutureParent = &stateParentT1TNoDisposable[2];
		ptrFutureChild = ptrFutureParent->ptrChild;
		}
		else if(ptrCurrentParent->action == ACTION_ALWAYS)
		{
			currentGuard[GUARD_ENABLE_T1_CONFIG].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		}
		break;

	case PARENT_T1_NO_DISP_CHK_CONFG:
		if(currentGuard[GUARD_ENABLE_T1_24VBRK].guardValue == GUARD_VALUE_TRUE)
		{
		ptrFutureParent = &stateParentT1TNoDisposable[5];
		ptrFutureChild = ptrFutureParent->ptrChild;
		DebugStringStr("parent chk 24vbrk");
		break;
		}

		if(ptrCurrentParent->action == ACTION_ON_ENTRY)
		{
		ptrFutureParent = &stateParentT1TNoDisposable[4];
		ptrFutureChild = ptrFutureParent->ptrChild;
		}
		else if(ptrCurrentParent->action == ACTION_ALWAYS)
		{
			currentGuard[GUARD_ENABLE_T1_24VBRK].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		}
		break;

	case PARENT_T1_NO_DISP_CHK_24VBRK:
		if(currentGuard[GUARD_ENABLE_T1_PRESS].guardValue == GUARD_VALUE_TRUE)
		{
		ptrFutureParent = &stateParentT1TNoDisposable[7];
		ptrFutureChild = ptrFutureParent->ptrChild;
		DebugStringStr("parent chk press");
		break;
		}

		if(ptrCurrentParent->action == ACTION_ON_ENTRY)
		{
		ptrFutureParent = &stateParentT1TNoDisposable[6];
		ptrFutureChild = ptrFutureParent->ptrChild;
		}
		else if(ptrCurrentParent->action == ACTION_ALWAYS)
		{
			currentGuard[GUARD_ENABLE_T1_PRESS].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		}
		break;

	case PARENT_T1_NO_DISP_CHECK_PRESS:
		if(currentGuard[GUARD_ENABLE_T1_TEMPIR].guardValue == GUARD_VALUE_TRUE)
		{
		ptrFutureParent = &stateParentT1TNoDisposable[9];
		ptrFutureChild = ptrFutureParent->ptrChild;
		DebugStringStr("parent chk temp");
		break;
		}

		if(ptrCurrentParent->action == ACTION_ON_ENTRY)
		{
		ptrFutureParent = &stateParentT1TNoDisposable[8];
		ptrFutureChild = ptrFutureParent->ptrChild;
		}
		else if(ptrCurrentParent->action == ACTION_ALWAYS)
		{
			currentGuard[GUARD_ENABLE_T1_TEMPIR].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		}
		break;

	case PARENT_T1_NO_DISP_CHECK_TEMP:
		if(currentGuard[GUARD_ENABLE_T1_LEVEL].guardValue == GUARD_VALUE_TRUE)
		{
		ptrFutureParent = &stateParentT1TNoDisposable[11];
		ptrFutureChild = ptrFutureParent->ptrChild;
		DebugStringStr("parent chk level");
		break;
		}

		if(ptrCurrentParent->action == ACTION_ON_ENTRY)
		{
		ptrFutureParent = &stateParentT1TNoDisposable[10];
		ptrFutureChild = ptrFutureParent->ptrChild;
		}
		else if(ptrCurrentParent->action == ACTION_ALWAYS)
		{
			currentGuard[GUARD_ENABLE_T1_LEVEL].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		}
		break;

	case PARENT_T1_NO_DISP_CHECK_LEVEL:
		if(currentGuard[GUARD_ENABLE_T1_FLOWMTR].guardValue == GUARD_VALUE_TRUE)
		{
		ptrFutureParent = &stateParentT1TNoDisposable[13];
		ptrFutureChild = ptrFutureParent->ptrChild;
		DebugStringStr("parent chk flwmtr");
		break;
		}

		if(ptrCurrentParent->action == ACTION_ON_ENTRY)
		{
		ptrFutureParent = &stateParentT1TNoDisposable[12];
		ptrFutureChild = ptrFutureParent->ptrChild;
		}
		else if(ptrCurrentParent->action == ACTION_ALWAYS)
		{
			currentGuard[GUARD_ENABLE_T1_FLOWMTR].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		}
		break;

	case PARENT_T1_NO_DISP_CHECK_FLWMTR:
		if(currentGuard[GUARD_ENABLE_T1_AIRSENS].guardValue == GUARD_VALUE_TRUE)
		{
		ptrFutureParent = &stateParentT1TNoDisposable[15];
		ptrFutureChild = ptrFutureParent->ptrChild;
		DebugStringStr("parent chk air");
		break;
		}

		if(ptrCurrentParent->action == ACTION_ON_ENTRY)
		{
		ptrFutureParent = &stateParentT1TNoDisposable[14];
		ptrFutureChild = ptrFutureParent->ptrChild;
		}
		else if(ptrCurrentParent->action == ACTION_ALWAYS)
		{
			currentGuard[GUARD_ENABLE_T1_AIRSENS].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		}
		break;

	case PARENT_T1_NO_DISP_CHEK_AIR:
		if(currentGuard[GUARD_ENABLE_T1_PINCH].guardValue == GUARD_VALUE_TRUE)
		{
		ptrFutureParent = &stateParentT1TNoDisposable[17];
		ptrFutureChild = ptrFutureParent->ptrChild;
		DebugStringStr("parent chk pinch");
		break;
		}

		if(ptrCurrentParent->action == ACTION_ON_ENTRY)
		{
		ptrFutureParent = &stateParentT1TNoDisposable[16];
		ptrFutureChild = ptrFutureParent->ptrChild;
		}
		else if(ptrCurrentParent->action == ACTION_ALWAYS)
		{
			currentGuard[GUARD_ENABLE_T1_PINCH].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		}
		break;

	case PARENT_T1_NO_DISP_CHEK_PINCH:
		if(currentGuard[GUARD_ENABLE_T1_PUMP].guardValue == GUARD_VALUE_TRUE)
		{
		ptrFutureParent = &stateParentT1TNoDisposable[19];
		ptrFutureChild = ptrFutureParent->ptrChild;
		DebugStringStr("parent chk pump");
		break;
		}

		if(ptrCurrentParent->action == ACTION_ON_ENTRY)
		{
		ptrFutureParent = &stateParentT1TNoDisposable[18];
		ptrFutureChild = ptrFutureParent->ptrChild;
		}
		else if(ptrCurrentParent->action == ACTION_ALWAYS)
		{
			currentGuard[GUARD_ENABLE_T1_PUMP].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		}
		break;

	case PARENT_T1_NO_DISP_CHEK_PUMP:
		if(currentGuard[GUARD_ENABLE_T1_TERMO].guardValue == GUARD_VALUE_TRUE)
		{
		ptrFutureParent = &stateParentT1TNoDisposable[21];
		ptrFutureChild = ptrFutureParent->ptrChild;
		DebugStringStr("parent chk termo");
		break;
		}

		if(ptrCurrentParent->action == ACTION_ON_ENTRY)
		{
		ptrFutureParent = &stateParentT1TNoDisposable[20];
		ptrFutureChild = ptrFutureParent->ptrChild;
		}
		else if(ptrCurrentParent->action == ACTION_ALWAYS)
		{
			currentGuard[GUARD_ENABLE_T1_TERMO].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		}
		break;

	case PARENT_T1_NO_DISP_CHEK_PELTIER:
		if(currentGuard[GUARD_ENABLE_T1_END].guardValue == GUARD_VALUE_TRUE)
		{
		ptrFutureParent = &stateParentT1TNoDisposable[23];
		ptrFutureChild = ptrFutureParent->ptrChild;
		DebugStringStr("parent to end");
		break;
		}

		if(ptrCurrentParent->action == ACTION_ON_ENTRY)
		{
		ptrFutureParent = &stateParentT1TNoDisposable[22];
		ptrFutureChild = ptrFutureParent->ptrChild;
		}
		else if(ptrCurrentParent->action == ACTION_ALWAYS)
		{
			currentGuard[GUARD_ENABLE_T1_END].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		}
		break;

	case PARENT_T1_NO_DISP_END:
		currentGuard[GUARD_HW_T1T_DONE].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		currentGuard[GUARD_COMM_ENABLED].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		DebugStringStr("parent to idle state");
		break;

	case PARENT_T1_NO_DISP_ALARM:
		break;

	case PARENT_T1_NO_DISP_FATAL_ERROR:
		break;

	default:
		break;
	}
#endif
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



extern bool FilterSelected;

// controlla il posizionamento delle pinch nei vari stati
CHECK_CURR_PINCH_POS_TASK_STATE CheckCurrPinchPosTask(CHECK_CURR_PINCH_POS_TASK_CMD cmd)
{
	static CHECK_CURR_PINCH_POS_TASK_STATE CheckCurrPinchPosTaskState = T_SET_PINCH_IDLE;
	static unsigned char CorrectPosCnt = 0;
	static unsigned char WrongPosCnt = 0;
	static unsigned char PinchPos[3] = {0xff, 0xff, 0xff};
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
				PinchPos[0] = (FilterSelected) ? MODBUS_PINCH_RIGHT_OPEN : MODBUS_PINCH_LEFT_OPEN;
				PinchPos[1] = MODBUS_PINCH_LEFT_OPEN;
				PinchPos[2] = MODBUS_PINCH_LEFT_OPEN;
				CheckCurrPinchPosTaskState = CHK_PINCH_POS_RUN;
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


