/*
 * parent.c
 *
 *  Created on: 18/apr/2018
 *      Author: franco mazzoni
 */

#include "PE_Types.h"
#include "Global.h"
//#include "PANIC_BUTTON_INPUT.h"
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
#include "statesStructs.h"
#include "general_func.h"
#include "pid.h"
#include "Comm_Sbc.h"

extern bool AtLeastoneButResRcvd;

void ParentFunc(void)
{
	static unsigned short ParentStateGenAlarm = 0;  // mantiene il valore dello stato parent che ha generato l'allarme

// ------------------GESTIONE STATI PARENT PRIMING-------------------------------------------------------------------
	//questo switch andrà suddiviso e portato dentro i singoli case dello switch sopra........
	/* (FM) POSSO FARE UNA FUNZIONE CHIAMATA stateParentPrimingTreatKidney1_func ED INSERIRE TUTTI I CASE
	   RELATIVI AL PRIMING E CHIAMARLA SEMPRE NEI CASE STATE_PRIMING_PH_1, STATE_PRIMING_PH_2 DELLO SWITCH PRECEDENTE
	   L'ALTRA FUNZIONE, ANALOGAMENTE POTRA' ESSERE CHIAMATA stateParentTreatKidney1_func, CONTERRA' TUTTI I CASE DAL PARENT_TREAT_KIDNEY_1_INIT
	   IN POI. LA FUNZIONE CREATA DOVRA' ESSERE CHIAMATA NEL CASE STATE_TREATMENT_KIDNEY_1. */
	switch(ptrCurrentParent->parent)
	{
		// Filippo - inserito caso per poter gestire l'allarme stop button anche in idle
		case PARENT_IDLE:
			// si è verificato un allarme lo attivo
			if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_TRUE)
			{
				// ho premuto il tasto di stop - gestisco l'allarme
				ptrFutureParent = &stateParentIdle[2];
				ptrFutureChild = ptrFutureParent->ptrChild;
				LevelBuzzer = HIGH;//2;
			}
			releaseGUIButton(BUTTON_RESET_ALARM);
			break;
		// Filippo - aggiunto il caso perchè devo gestire l'allarme in idle
		case PARENT_IDLE_ALARM:
			if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_FALSE)
			{
				// se hanno resettato l'allarme torno indietro
				if(buttonGUITreatment[BUTTON_RESET_ALARM].state == GUI_BUTTON_RELEASED)
				{
					releaseGUIButton(BUTTON_RESET_ALARM);
					EnableNextAlarmFunc(); //EnableNextAlarm = TRUE;
					LevelBuzzer = SILENT;//0;
					ptrFutureParent = &stateParentIdle[1];
					ptrFutureChild = ptrFutureParent->ptrChild;
					currentGuard[GUARD_ALARM_ACTIVE].guardValue = GUARD_VALUE_FALSE;
				}
			}
			break;
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
				LevelBuzzer = HIGH;//2;
			}
			// per sicurezza resetto la flag di reset alarm premuto, nel caso mi fosse rimasto settato //NOTA_ML: privo di senso
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
				LevelBuzzer = HIGH;//2;
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

			//Il Reset è un evento gestito ora in modo asincrono, per gli warning
			//cancello eventuali reset pendenti se e solo se non ho warning in lista d'attesa
			if (NoWarningsPresent())
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
						LevelBuzzer = SILENT;//0;
						AtLeastoneButResRcvd = TRUE;
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
					if (currentGuard[GUARD_PRIMING_STOPPED].guardValue == GUARD_VALUE_TRUE)
					{
						currentGuard[GUARD_PRIMING_STOPPED].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
						currentGuard[GUARD_PRIMING_STOPPED].guardValue = GUARD_VALUE_FALSE;

						ptrFutureParent = &stateParentPrimingTreatKidney1[9];
						ptrFutureChild = ptrFutureParent->ptrChild;
					}
					else
					{
						setGUIButton(BUTTON_START_PRIMING);

						ptrFutureParent = &stateParentPrimingTreatKidney1[3];
						ptrFutureChild = ptrFutureParent->ptrChild;
					}

					releaseGUIButton(BUTTON_RESET_ALARM);
					AtLeastoneButResRcvd = TRUE;
					EnableNextAlarmFunc();
					LevelBuzzer = SILENT;//0;
				}
			}
			else if(buttonGUITreatment[BUTTON_RESET_ALARM].state == GUI_BUTTON_RELEASED)
			{
				if (currentGuard[GUARD_PRIMING_STOPPED].guardValue == GUARD_VALUE_TRUE)
				{
					currentGuard[GUARD_PRIMING_STOPPED].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
					currentGuard[GUARD_PRIMING_STOPPED].guardValue = GUARD_VALUE_FALSE;

					ptrFutureParent = &stateParentPrimingTreatKidney1[9];
					ptrFutureChild = ptrFutureParent->ptrChild;
				}
				else
				{
					setGUIButton(BUTTON_START_PRIMING);

					ptrFutureParent = &stateParentPrimingTreatKidney1[3];
					ptrFutureChild = ptrFutureParent->ptrChild;
				}

				releaseGUIButton(BUTTON_RESET_ALARM);
				AtLeastoneButResRcvd = TRUE;
				EnableNextAlarmFunc();
				LevelBuzzer = SILENT;//0;
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
			currentGuard[GUARD_PRIMING_STOPPED].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			if(currentGuard[GUARD_ENABLE_STATE_KIDNEY_1_PRIM_RUN].guardValue == GUARD_VALUE_TRUE)
			{
				ptrFutureParent = &stateParentPrimingTreatKidney1[3];
				ptrFutureChild = ptrFutureParent->ptrChild;
				// forzo anche una pressione del tasto TREATMENT START per fare in modo che
				// il trattamento riprenda automaticamente
				setGUIButton(BUTTON_START_PRIMING);

				currentGuard[GUARD_ENABLE_STATE_KIDNEY_1_PRIM_RUN].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				currentGuard[GUARD_ENABLE_STATE_KIDNEY_1_PRIM_RUN].guardValue = GUARD_VALUE_FALSE;

				currentGuard[GUARD_PRIMING_STOPPED].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				currentGuard[GUARD_PRIMING_STOPPED].guardValue = GUARD_VALUE_FALSE;
				break;
			}
			else if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_TRUE)
			{
				ptrFutureParent = &stateParentPrimingTreatKidney1[5];
				ptrFutureChild = ptrFutureParent->ptrChild;
				LevelBuzzer = HIGH;//2;
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

			//Il Reset è un evento gestito ora in modo asincrono, per gli warning
			//cancello eventuali reset pendenti se e solo se non ho warning in lista d'attesa
			if (NoWarningsPresent())
				releaseGUIButton(BUTTON_RESET_ALARM);
			break;

		// STATO PER LA GESTIONE DELL'ATTESA DELLE POMPE FERME. SE NON SI FERMANO
		// VERRA' GENERATO UN ALLARME GESTITO NELLO STATO PARENT_PRIMING_END_RECIRC_ALARM
		case PARENT_PRIM_WAIT_MOT_STOP:
			if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_TRUE)
			{
				ptrFutureParent = &stateParentPrimingTreatKidney1[15];
				ptrFutureChild = ptrFutureParent->ptrChild;
				LevelBuzzer = HIGH;//2;
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
				LevelBuzzer = HIGH;//2;
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
					LevelBuzzer = SILENT;//0;
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
					LevelBuzzer = HIGH;
				}
				break;
			case PARENT_PRIM_KIDNEY_1_ALM_AIR_REC:
				if (buttonGUITreatment[BUTTON_RESET_ALARM].state == GUI_BUTTON_RELEASED)
				{
					releaseGUIButton(BUTTON_RESET_ALARM);
					EnableNextAlarmFunc();
					LevelBuzzer = SILENT;//0;
				}

				if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_FALSE)
				{
					/* FM allarme finito posso ritornare nello stato di partenza
					 * quando si e' verificato l'allarme */
					if (AirParentState == PARENT_TREAT_KIDNEY_1_AIR_FILT)
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
				LevelBuzzer = HIGH;//2;
			}
			else if(currentGuard[GUARD_ENT_PAUSE_STATE_TREAT_KIDNEY_1_INIT].guardValue == GUARD_VALUE_TRUE)
			{
				ptrFutureParent = &stateParentTreatKidney1[17];
				ptrFutureChild = ptrFutureParent->ptrChild;
				currentGuard[GUARD_ENT_PAUSE_STATE_TREAT_KIDNEY_1_INIT].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				currentGuard[GUARD_ENT_PAUSE_STATE_TREAT_KIDNEY_1_INIT].guardValue = GUARD_VALUE_FALSE;
			}

			//Il Reset è un evento gestito ora in modo asincrono, per gli warning
			//cancello eventuali reset pendenti se e solo se non ho warning in lista d'attesa
			if (NoWarningsPresent())
				releaseGUIButton(BUTTON_RESET_ALARM);
			break;

		case PARENT_TREAT_KIDNEY_1_PUMP_ON:
			if (ptrCurrentParent->action == ACTION_ON_ENTRY)
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

			if (currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_TRUE)
			{
				/* (FM) si e' verificato un allarme, passo alla sua gestione */
				ptrFutureParent = &stateParentTreatKidney1[5];
				ptrFutureChild = ptrFutureParent->ptrChild;
				DisableAllAirAlarm(FALSE);
				LevelBuzzer = HIGH;//2;
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

			//Il Reset è un evento gestito ora in modo asincrono, per gli warning
			//cancello eventuali reset pendenti se e solo se non ho warning in lista d'attesa
			if (NoWarningsPresent())
				releaseGUIButton(BUTTON_RESET_ALARM);
			break;

		case PARENT_TREAT_KIDNEY_1_ALARM:
			if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_FALSE)
			{
				bool ButtonResetRcvd = FALSE;
				if(buttonGUITreatment[BUTTON_RESET_ALARM].state == GUI_BUTTON_RELEASED)
				{
					releaseGUIButton(BUTTON_RESET_ALARM);
					// Filippo - devo rirpistinare l'algoritmo di gestione della temperatura
					FrigoHeatTempControlTaskNewPID((LIQ_TEMP_CONTR_TASK_CMD)LIQ_T_CONTR_TASK_RESET_CMD);
					EnableNextAlarmFunc(); //EnableNextAlarm = TRUE;
					ButtonResetRcvd = TRUE;
					LevelBuzzer = SILENT;//0;
					// preparo la macchina a stati per il controllo delle pinch aperte nella posizione richiesta per lo stato di trattamento
					TreatSetPinchPosTask((TREAT_SET_PINCH_POS_CMD)T_SET_PINCH_RESET_CMD);
					AtLeastoneButResRcvd = TRUE;
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
				else if(currentGuard[GUARD_ALARM_DELTA_TEMP_HIGH_RECV].guardValue == GUARD_VALUE_TRUE)
				{
					// vado nello stato parent dove posso cercare di recuperare la temperatura del liquido
					currentGuard[GUARD_ALARM_DELTA_TEMP_HIGH_RECV].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
					currentGuard[GUARD_ALARM_DELTA_TEMP_HIGH_RECV].guardValue = GUARD_VALUE_FALSE;
					GoToRecoveryParentState(PARENT_TREAT_KIDNEY_1_DELTA_T_HIGH_RECV);
				}
			}
			else if(buttonGUITreatment[BUTTON_RESET_ALARM].state == GUI_BUTTON_RELEASED)
			{
				if (currentGuard[GUARD_TREATMENT_STOPPED].guardValue == GUARD_VALUE_TRUE)
				{
					currentGuard[GUARD_TREATMENT_STOPPED].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
					currentGuard[GUARD_TREATMENT_STOPPED].guardValue = GUARD_VALUE_FALSE;

					ptrFutureParent = &stateParentTreatKidney1[17];
					ptrFutureChild = ptrFutureParent->ptrChild;
				}
				else
				{
					ptrFutureParent = &stateParentTreatKidney1[1];
					ptrFutureChild = ptrFutureParent->ptrChild;
					TreatSetPinchPosTask((TREAT_SET_PINCH_POS_CMD)T_SET_PINCH_RESET_CMD);
					setGUIButton(BUTTON_START_TREATMENT);
				}

				releaseGUIButton(BUTTON_RESET_ALARM);
				EnableNextAlarmFunc();
				LevelBuzzer = SILENT;//0;
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

				// preparo la macchina a stati per il posizionamento e controllo delle pinch aperte nella posizione richiesta
				// per lo stato di trattamento
				TreatSetPinchPosTask((TREAT_SET_PINCH_POS_CMD)T_SET_PINCH_RESET_CMD);
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

				// preparo la macchina a stati per il posizionamento e controllo delle pinch aperte nella posizione richiesta
				// per lo stato di trattamento
				TreatSetPinchPosTask((TREAT_SET_PINCH_POS_CMD)T_SET_PINCH_RESET_CMD);
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

				// preparo la macchina a stati per il posizionamento e controllo delle pinch aperte nella posizione richiesta
				// per lo stato di trattamento
				TreatSetPinchPosTask((TREAT_SET_PINCH_POS_CMD)T_SET_PINCH_RESET_CMD);
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
			if (buttonGUITreatment[BUTTON_RESET_ALARM].state == GUI_BUTTON_RELEASED)
			{
				releaseGUIButton(BUTTON_RESET_ALARM);
				EnableNextAlarmFunc();
				LevelBuzzer = SILENT;//0;
			}

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

		// STATI PER LA GESTIONE DELLA PROCEDURA DI RIPRISTINO DELLA TEMPERATURA DEL LIQUIDO -----------------------
		case PARENT_TREAT_KIDNEY_1_DELTA_T_HIGH_RECV:
			if(currentGuard[GUARD_TEMP_RESTORE_END].guardValue == GUARD_VALUE_TRUE)
			{
				// vado nello stato in cui controllero' che l'allarme di temperatura non venga visualizzato
				currentGuard[GUARD_TEMP_RESTORE_END].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				currentGuard[GUARD_TEMP_RESTORE_END].guardValue = GUARD_VALUE_FALSE;
				ptrFutureParent = &stateParentTreatKidney1[25];
				ptrFutureChild = ptrFutureParent->ptrChild;
				break;
			}

			if(ptrCurrentParent->action == ACTION_ON_ENTRY)
			{
				/* compute future parent */
				/* FM passo alla gestione ACTION_ALWAYS */
				ptrFutureParent = &stateParentTreatKidney1[22];
				ptrFutureChild = ptrFutureParent->ptrChild;
			}
			else if(ptrCurrentParent->action == ACTION_ALWAYS)
			{
			}

			if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_TRUE)
			{
				/* (FM) si e' verificato un allarme, durante la procedura di recupero dell'allarme delta temperatura eccessivo.
				 * Per la sua gestione uso un nuovo stato 23  */
				ptrFutureParent = &stateParentTreatKidney1[23];
				ptrFutureChild = ptrFutureParent->ptrChild;
			}
			break;

		case PARENT_TREAT_KIDNEY_1_ALM_DELTA_T_H_RECV:
			if (buttonGUITreatment[BUTTON_RESET_ALARM].state == GUI_BUTTON_RELEASED)
			{
				releaseGUIButton(BUTTON_RESET_ALARM);
				EnableNextAlarmFunc();
				LevelBuzzer = SILENT;//0;
			}

			if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_FALSE)
			{
				/* FM allarme finito posso ritornare nello stato di partenza
				 * quando si e' verificato l'allarme */
				if(AirParentState == PARENT_TREAT_KIDNEY_1_DELTA_T_HIGH_RECV)
				{
					ptrFutureParent = &stateParentTreatKidney1[21];
					ptrFutureChild = ptrFutureParent->ptrChild;
					StarTimeToRejAir = timerCounterModBus;
					break;
				}
				else
				{
					// per sicurezza, non dovrebbe andarci mai
					ptrFutureParent = &stateParentTreatKidney1[1];
					ptrFutureChild = ptrFutureParent->ptrChild;
					break;
				}
			}

			if(ptrCurrentParent->action == ACTION_ON_ENTRY)
			{
				/* compute future parent */
				/* (FM) passo alla gestione ACTION_ALWAYS dell'allarme */
				ptrFutureParent = &stateParentTreatKidney1[24];
				ptrFutureChild = ptrFutureParent->ptrChild;
			}
			else if(ptrCurrentParent->action == ACTION_ALWAYS)
			{
				// (FM) chiamo la funzione child che gestisce lo stato di allarme
				ManageStateChildAlarmTreat1();
			}
			break;

		case PARENT_TREAT_KIDNEY_1_DELTA_T_HIGH_WAIT:
			if(currentGuard[GUARD_TEMP_NEW_RECOVERY].guardValue == GUARD_VALUE_TRUE)
			{
				currentGuard[GUARD_TEMP_NEW_RECOVERY].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				currentGuard[GUARD_TEMP_NEW_RECOVERY].guardValue = GUARD_VALUE_FALSE;
				/* la condizione di allarme dovuta alla temperatura non e' stata risolta eseguo un'altro
				 * ciclo di recupero non senza prima aver disabilitato l'allarme di temperatura che per ora non mi
				 * serve piu'*/
				ptrFutureParent = &stateParentTreatKidney1[21];
				ptrFutureChild = ptrFutureParent->ptrChild;
			}
			else if(currentGuard[GUARD_TEMP_RESTART_TREAT].guardValue == GUARD_VALUE_TRUE)
			{
				// vado nello stato in cui controllero' che l'allarme di temperatura non venga visualizzato
				currentGuard[GUARD_TEMP_RESTART_TREAT].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				currentGuard[GUARD_TEMP_RESTART_TREAT].guardValue = GUARD_VALUE_FALSE;

				//l'ALLARME NON E' SCATTATO, HO RAGGIUNTO LA TEMPERATURA TARGET POSSO RIPARTIRE CON IL TRATTAMENTO

				// ho terminato la fase di ripristino della temperatura del liquido, posso
				// ritornare nello stato di allarme in cui sono partito oppure nello stato di
				// lavoro normale (dopo aver riabilitato gli allarmi).
				// Decido di ripartire dalla fase iniziale del trattamento.

				// ripristino le abilitazioni degli allarmi
				RestoreAllCntrlAlarm(&DELTA_T_HIGH_RECV_gbf);
				EnableDeltaTHighAlmFunc();  // riabilito allarme temperatura
				DisableAllAirAlarm(FALSE);  // riabilito allarmi aria
				ptrFutureParent = &stateParentTreatKidney1[1];
				ptrFutureChild = ptrFutureParent->ptrChild;

				// preparo la macchina a stati per il posizionamento e controllo delle pinch aperte nella posizione richiesta
				// per lo stato di trattamento
				TreatSetPinchPosTask((TREAT_SET_PINCH_POS_CMD)T_SET_PINCH_RESET_CMD);
				// forzo anche una pressione del tasto TREATMENT START per fare in modo che
				// il trattamento riprenda automaticamente
				setGUIButton(BUTTON_START_TREATMENT);
				break;
			}

			if(ptrCurrentParent->action == ACTION_ON_ENTRY)
			{
				ptrFutureParent = &stateParentTreatKidney1[26];
				ptrFutureChild = ptrFutureParent->ptrChild;
			}
			else if(ptrCurrentParent->action == ACTION_ALWAYS)
			{
			}
			break;

		//---------------------------------------------------------------------------------------------


		case PARENT_TREAT_KIDNEY_1_END:
            /* (FM) fine del trattamento  devo rimanere fermo qui, fino a quando non ricevo un nuovo
             comando di inizio trattamento */
			break;


		case PARENT_TREAT_WAIT_START:
			currentGuard[GUARD_TREATMENT_STOPPED].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
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

				currentGuard[GUARD_TREATMENT_STOPPED].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
				currentGuard[GUARD_TREATMENT_STOPPED].guardValue = GUARD_VALUE_FALSE;
				break;
			}
			else if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_TRUE)
			{
				ptrFutureParent = &stateParentTreatKidney1[5];
				ptrFutureChild = ptrFutureParent->ptrChild;
				LevelBuzzer = HIGH;//2;
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
			currentGuard[GUARD_TREATMENT_STOPPED].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_TRUE)
			{
				ptrFutureParent = &stateParentTreatKidney1[5];
				ptrFutureChild = ptrFutureParent->ptrChild;

				LevelBuzzer = HIGH;//2;
				break;
			}
			else if(currentGuard[GUARD_ENABLE_STATE_KIDNEY_1_PRIM_RUN].guardValue == GUARD_VALUE_TRUE)
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

	switch(ptrCurrentParent->parent)
#ifdef DEBUG_T1_TEST
	{
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
		else if(currentGuard[GUARD_ENABLE_T1_ALARM].guardValue == GUARD_VALUE_TRUE){
			ptrFutureParent = &stateParentT1TNoDisposable[25]; //alarm
			ptrFutureChild = ptrFutureParent->ptrChild;
			DebugStringStr("alarm t1 test");
			break;
		}

		if(ptrCurrentParent->action == ACTION_ON_ENTRY)
		{
		ptrFutureParent = &stateParentT1TNoDisposable[4];
		ptrFutureChild = ptrFutureParent->ptrChild;
		}
		else if((ptrCurrentParent->action == ACTION_ALWAYS) && (ptrT1Test->result_T1_cfg_data == T1_TEST_OK))
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
		else if(currentGuard[GUARD_ENABLE_T1_ALARM].guardValue == GUARD_VALUE_TRUE){
			ptrFutureParent = &stateParentT1TNoDisposable[25]; //alarm
			ptrFutureChild = ptrFutureParent->ptrChild;
			DebugStringStr("alarm t1 test");
			break;
		}

		if(ptrCurrentParent->action == ACTION_ON_ENTRY)
		{
		ptrFutureParent = &stateParentT1TNoDisposable[6];
		ptrFutureChild = ptrFutureParent->ptrChild;
		}
		else if((ptrCurrentParent->action == ACTION_ALWAYS) && (ptrT1Test->result_T1_24vbrk == T1_TEST_OK))
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
		else if(currentGuard[GUARD_ENABLE_T1_ALARM].guardValue == GUARD_VALUE_TRUE){
		ptrFutureParent = &stateParentT1TNoDisposable[25]; //alarm
		ptrFutureChild = ptrFutureParent->ptrChild;
		DebugStringStr("alarm t1 test");
		break;
		}

		if(ptrCurrentParent->action == ACTION_ON_ENTRY)
		{
		ptrFutureParent = &stateParentT1TNoDisposable[8];
		ptrFutureChild = ptrFutureParent->ptrChild;
		}
		else if((ptrCurrentParent->action == ACTION_ALWAYS) && (ptrT1Test->result_T1_press == T1_TEST_OK))
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
		else if(currentGuard[GUARD_ENABLE_T1_ALARM].guardValue == GUARD_VALUE_TRUE){
		ptrFutureParent = &stateParentT1TNoDisposable[25]; //alarm
		ptrFutureChild = ptrFutureParent->ptrChild;
		DebugStringStr("alarm t1 test");
		break;
		}

		if(ptrCurrentParent->action == ACTION_ON_ENTRY)
		{
		ptrFutureParent = &stateParentT1TNoDisposable[10];
		ptrFutureChild = ptrFutureParent->ptrChild;
		}
		else if((ptrCurrentParent->action == ACTION_ALWAYS) && (ptrT1Test->result_T1_tempIR == T1_TEST_OK))
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
		else if(currentGuard[GUARD_ENABLE_T1_ALARM].guardValue == GUARD_VALUE_TRUE){
		ptrFutureParent = &stateParentT1TNoDisposable[25]; //alarm
		ptrFutureChild = ptrFutureParent->ptrChild;
		DebugStringStr("alarm t1 test");
		break;
		}

		if(ptrCurrentParent->action == ACTION_ON_ENTRY)
		{
		ptrFutureParent = &stateParentT1TNoDisposable[14];
		ptrFutureChild = ptrFutureParent->ptrChild;
		}
		else if((ptrCurrentParent->action == ACTION_ALWAYS) && (ptrT1Test->result_T1_flwmtr == T1_TEST_OK))
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
		else if((ptrCurrentParent->action == ACTION_ALWAYS) && (t1Test_pinch_state == 4))
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
		else if((ptrCurrentParent->action == ACTION_ALWAYS) && (t1Test_pump_state == 4))
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
#else
	{
		/*Vincenzo gestione T1 TEST*/
	case PARENT_T1_NO_DISP_INIT:
#ifdef T1_TEST_ENABLED
		// Filippo - tolto per fargli eseguire il T1 test
		ptrFutureParent = &stateParentT1TNoDisposable[3];	// lo mando al test della EEPROM quindi eseguo il T1 TEST

		/*se la temperatura di piastra supera i 38 gradi
		 * decido di fare prima il T1_Test del frigo,
		 * il riscaldatore lo farò per ultimo*/
		if (T_PLATE_C_GRADI_CENT>=38)
			T1_Test_Frigo_Before_Heater = TRUE;
#else
		ptrFutureParent = &stateParentT1TNoDisposable[23]; //con il 23 salto di netto tutti i T1 TEST e vado nel case: PARENT_T1_NO_DISP_END
		// lo mando al test dell'heater
//		ptrFutureParent = &stateParentT1TNoDisposable[29]; //con il 29 passo al T1 TEST dsel FRIGO
//		ptrFutureParent = &stateParentT1TNoDisposable[3];	// lo mando al test della EEPROM quindi eseguo il T1 TEST
#endif // TI_TEST_ENABLED
		break;

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
		else if(currentGuard[GUARD_ENABLE_T1_ALARM].guardValue == GUARD_VALUE_TRUE)
		{
			ptrFutureParent = &stateParentT1TNoDisposable[25]; //alarm
			ptrFutureChild = ptrFutureParent->ptrChild;
			DebugStringStr("alarm t1 test");
			allarmeTestT1Attivo=TRUE;
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
		else if(currentGuard[GUARD_ENABLE_T1_ALARM].guardValue == GUARD_VALUE_TRUE){
			ptrFutureParent = &stateParentT1TNoDisposable[25]; //alarm
			ptrFutureChild = ptrFutureParent->ptrChild;
			DebugStringStr("alarm t1 test");
			allarmeTestT1Attivo=TRUE;
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
		else if(currentGuard[GUARD_ENABLE_T1_ALARM].guardValue == GUARD_VALUE_TRUE)
		{
			ptrFutureParent = &stateParentT1TNoDisposable[25]; //alarm
			ptrFutureChild = ptrFutureParent->ptrChild;
			DebugStringStr("alarm t1 test");
			allarmeTestT1Attivo=TRUE;
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
		else if(currentGuard[GUARD_ENABLE_T1_ALARM].guardValue == GUARD_VALUE_TRUE)
		{
			ptrFutureParent = &stateParentT1TNoDisposable[25]; //alarm
			ptrFutureChild = ptrFutureParent->ptrChild;
			DebugStringStr("alarm t1 test");
			allarmeTestT1Attivo=TRUE;
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
		else if(currentGuard[GUARD_ENABLE_T1_ALARM].guardValue == GUARD_VALUE_TRUE)
		{
			ptrFutureParent = &stateParentT1TNoDisposable[25]; //alarm
			ptrFutureChild = ptrFutureParent->ptrChild;
			DebugStringStr("alarm t1 test");
			allarmeTestT1Attivo=TRUE;
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
//			if (t1TestAir==3)
//			{
				currentGuard[GUARD_ENABLE_T1_PINCH].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
//			}
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
			// Filippo - il test deve terminare con successo, altrimenti allarme!!!!
			if (t1Test_pinch_state==4)
			{
				currentGuard[GUARD_ENABLE_T1_PUMP].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			}
			else if (t1Test_pinch_state == 5)
			{
				ptrFutureParent = &stateParentT1TNoDisposable[25]; //alarm
				ptrFutureChild = ptrFutureParent->ptrChild;
				DebugStringStr("alarm t1 test");
				allarmeTestT1Attivo=TRUE;
				break;
			}
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
			// Filippo - messo il controllo per la fine del test pompe
			if (t1Test_pump_state==4)
			{
				currentGuard[GUARD_ENABLE_T1_TERMO].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			}
			else if (t1Test_pump_state == 5)
			{
				ptrFutureParent = &stateParentT1TNoDisposable[25]; //alarm
				ptrFutureChild = ptrFutureParent->ptrChild;
				DebugStringStr("alarm t1 test");
				allarmeTestT1Attivo=TRUE;
				break;
			}
		}
		break;

	case PARENT_T1_NO_DISP_CHEK_PELTIER:
		if(currentGuard[GUARD_ENABLE_T1_HEATER].guardValue == GUARD_VALUE_TRUE)
		{
			/*se devo afr partire prima il riscaldatore*/
			if (T1_Test_Frigo_Before_Heater == FALSE)
			{
				ptrFutureParent = &stateParentT1TNoDisposable[29];
	//		ptrFutureParent = &stateParentT1TNoDisposable[23];
				ptrFutureChild = ptrFutureParent->ptrChild;
				DebugStringStr("parent to chk heater");
			}
			else //if (T1_Test_Frigo_Before_Heater == TRUE)
			{
				ptrFutureParent = &stateParentT1TNoDisposable[31];	// finisco il test
				ptrFutureChild = ptrFutureParent->ptrChild;
				DebugStringStr("parent chk fridge");
			}
			break;
		}

		if(ptrCurrentParent->action == ACTION_ON_ENTRY)
		{
		ptrFutureParent = &stateParentT1TNoDisposable[22];
		ptrFutureChild = ptrFutureParent->ptrChild;
		}
		else if(ptrCurrentParent->action == ACTION_ALWAYS)
		{
			currentGuard[GUARD_ENABLE_T1_HEATER].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		}
		break;

	case PARENT_T1_NO_DISP_END:
		currentGuard[GUARD_HW_T1T_DONE].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		currentGuard[GUARD_COMM_ENABLED].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		DebugStringStr("parent to idle state");
		break;

	case PARENT_T1_NO_DISP_ALARM:
		// Filippo - quando entro qui devo sollevare un allarme non ripristinabile
		if(ptrCurrentParent->action == ACTION_ON_ENTRY)
		{
			// quando entro in allarme aggiorno i figli per far in modo di fermare tutti gli attuatori e rimango in allarme per
			// sempre
			ptrFutureParent = &stateParentT1TNoDisposable[26];
			ptrFutureChild = ptrFutureParent->ptrChild;
			LevelBuzzer = HIGH;//2;

		}
		else if(ptrCurrentParent->action == ACTION_ALWAYS)
		{
			// qui devo controllare se qualcuno ha spedito il tasto di reset per annullarne l'effetto e fare in modo che rimanga in
			// allarme
//			if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_FALSE)
//			{
//				// se hanno resettato l'allarme lo ripristino e resto lì
//				if(buttonGUITreatment[BUTTON_RESET_ALARM].state == GUI_BUTTON_RELEASED)
//				{
//					releaseGUIButton(BUTTON_RESET_ALARM);
//				}
//			}
		}

		break;

	case PARENT_T1_NO_DISP_FATAL_ERROR:
		break;
	// Filippo - casistica aggiunta per inserire il test del riscaldatore
	case PARENT_T1_NO_DISP_CHECK_HEATER:
		// adesso metto l'end, però man mano che aggiungo test potrebbe cambiare
		if(currentGuard[GUARD_ENABLE_T1_FRIDGE].guardValue == GUARD_VALUE_TRUE)
		{
			if (T1_Test_Frigo_Before_Heater == FALSE)
			{
				//			ptrFutureParent = &stateParentT1TNoDisposable[23];	// finisco il test
				ptrFutureParent = &stateParentT1TNoDisposable[31];	// finisco il test
				ptrFutureChild = ptrFutureParent->ptrChild;
				DebugStringStr("parent chk fridge");
			}
			else //if (T1_Test_Frigo_Before_Heater == TRUE)
			{
				ptrFutureParent = &stateParentT1TNoDisposable[23];	// finisco il test
				ptrFutureChild = ptrFutureParent->ptrChild;
				DebugStringStr("parent to end");
			}
			break;
		}
		else if (currentGuard[GUARD_ENABLE_T1_ALARM].guardValue == GUARD_VALUE_TRUE)
		{
			ptrFutureParent = &stateParentT1TNoDisposable[25]; //alarm
			ptrFutureChild = ptrFutureParent->ptrChild;
			DebugStringStr("alarm t1 test");
			allarmeTestT1Attivo=TRUE;
			break;
		}

		if(ptrCurrentParent->action == ACTION_ON_ENTRY)
		{
			ptrFutureParent = &stateParentT1TNoDisposable[30];
			ptrFutureChild = ptrFutureParent->ptrChild;
		}
		else if(ptrCurrentParent->action == ACTION_ALWAYS)
		{
			// Filippo - messo il controllo per la fine del test pompe
			if (t1Test_heater==9)
			{
				// per adesso metto l'end ma poi dovrò aggiungere altri test
				currentGuard[GUARD_ENABLE_T1_FRIDGE].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			}
		}

		break;
	// Filippo - casistica aggiunta per inserire il test del riscaldatore
	case PARENT_T1_NO_DISP_CHECK_FRIDGE:
		// adesso metto l'end, però man mano che aggiungo test potrebbe cambiare
		if(currentGuard[GUARD_ENABLE_T1_END].guardValue == GUARD_VALUE_TRUE)
		{
			if (T1_Test_Frigo_Before_Heater == FALSE)
			{
				ptrFutureParent = &stateParentT1TNoDisposable[23];	// finisco il test
				ptrFutureChild = ptrFutureParent->ptrChild;
				DebugStringStr("parent to end");
			}
			else //if (T1_Test_Frigo_Before_Heater == TRUE)
			{
				ptrFutureParent = &stateParentT1TNoDisposable[29];
				ptrFutureChild = ptrFutureParent->ptrChild;
				DebugStringStr("parent to chk heater");
			}
			break;
		}
		else if (currentGuard[GUARD_ENABLE_T1_ALARM].guardValue == GUARD_VALUE_TRUE)
		{
			ptrFutureParent = &stateParentT1TNoDisposable[25]; //alarm
			ptrFutureChild = ptrFutureParent->ptrChild;
			DebugStringStr("alarm t1 test");
			allarmeTestT1Attivo=TRUE;
			break;
		}

		if(ptrCurrentParent->action == ACTION_ON_ENTRY)
		{
			ptrFutureParent = &stateParentT1TNoDisposable[32];
			ptrFutureChild = ptrFutureParent->ptrChild;
		}
		else if(ptrCurrentParent->action == ACTION_ALWAYS)
		{
			// Filippo - messo il controllo per la fine del test pompe
			if (t1Test_Frigo==9)
			{
				// per adesso metto l'end ma poi dovrò aggiungere altri test
				currentGuard[GUARD_ENABLE_T1_END].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
			}
		}

		break;
	default:
		break;
	}

	// Filippo - il controllo sull'allarme di stop lo faccio per tutti i test
	if (ptrCurrentParent->parent!=PARENT_T1_NO_DISP_ALARM)
	{
		if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_TRUE)
		{
			// ho premuto il tasto di stop - gestisco l'allarme
			ptrFutureParent = &stateParentT1TNoDisposable[25]; //alarm
			ptrFutureChild = ptrFutureParent->ptrChild;
			DebugStringStr("alarm t1 test");
			LevelBuzzer = HIGH;//2;
		}
	}



#endif
}



