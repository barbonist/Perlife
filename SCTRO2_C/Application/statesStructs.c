/*
 * statesStructs.c
 *
 *  Created on: 05/feb/2018
 *      Author: franco mazzoni
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



/********************************/
/* machine state initialization */
/********************************/

struct machineNull stateNull[] =
		  {
		  		{},
		  		{}
		  };


/*********************************/
/*********CHILD LEVEL*************/
/*********************************/

// (FM)
// machineChild    struttura usata per descrivere le attuazioni da fare in conseguenza di comandi ricevuti nello stato
//                 parent
// {STATE_NULL,    questo valore e' sempre STATE_NULL perche' questo campo della struttura non e' mai usato
//  PARENT_NULL,   questo campo e' inutilizzato
//  CHILD_NULL,    identifica il tipo di operazione da eseguire nell'ultimo switch della funzione processMachineState
//                 normalmente dopo aver eseguito la funzione prevista in ACTION_ON_ENTRY si ferma e loop all'infinito
//                 nella funzione individuata da ACTION_ALWAYS
//  ACTION_NULL,  ACTION_ON_ENTRY o ACTION_ALWAYS (*)
//  &stateNull[0], questa struttura normalmente non prevede un cambio di array di strutture
//  &manageNull},  operazioni da eseguire in base a (*)

struct machineChild stateChildNull[] =
		  {
		  	{STATE_NULL, PARENT_NULL, CHILD_NULL, ACTION_NULL, &stateNull[0], &manageNull},
			{STATE_NULL, PARENT_NULL, CHILD_NULL, ACTION_NULL, &stateNull[0], &manageNull},
		  	{}
		  };

struct machineChild stateChildEntry[] =
		  {
		  	{STATE_NULL, PARENT_NULL, CHILD_NULL, ACTION_NULL, &stateNull[0], &manageNull},

			{STATE_NULL, PARENT_NULL, CHILD_ENTRY, ACTION_ON_ENTRY, &stateNull[0], &manageChildEntry},
			{STATE_NULL, PARENT_NULL, CHILD_ENTRY, ACTION_ALWAYS, &stateNull[0], &manageChildEntryAlways},

		  	{}
		  };

struct machineChild stateChildIdle[] =
		  {
		  	{STATE_NULL, PARENT_NULL, CHILD_NULL, ACTION_NULL, &stateNull[0], &manageNull},
			{STATE_NULL, PARENT_NULL, CHILD_IDLE, ACTION_NULL, &stateNull[0], &manageNull},
		  	{}
		  };

// Filippo - aggiunto stato di allarme in idle
struct machineChild stateChildAlarmIdle[] =
		  {
		  	{STATE_NULL, PARENT_NULL, CHILD_IDLE_ALARM, ACTION_ON_ENTRY, &stateNull[0], &manageChildIdleAlarm},
			{STATE_NULL, PARENT_NULL, CHILD_IDLE_ALARM, ACTION_ALWAYS, &stateNull[0], &manageNull},
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_STOP_ALL_ACTUATOR, ACTION_ON_ENTRY, &stateNull[0], &manageChildIdleAlm1StAllActEntry},      /* 11 */
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_STOP_ALL_ACTUATOR, ACTION_ALWAYS,   &stateNull[0], &manageChildIdleAlm1StAllActAlways},     /* 12 */
		  	{}
		  };


// Filippo - aggiunto stato di allarme in idle
struct machineChild stateChildAlarmT1[] =
		  {
		  	{STATE_NULL, PARENT_NULL, CHILD_IDLE_ALARM, ACTION_ON_ENTRY, &stateNull[0], &manageChildIdleAlarm},
			{STATE_NULL, PARENT_NULL, CHILD_IDLE_ALARM, ACTION_ALWAYS, &stateNull[0], &manageNull},
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_STOP_ALL_ACTUATOR, ACTION_ON_ENTRY, &stateNull[0], &manageChildT1Alm1StAllActEntry},      /* 11 */
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_STOP_ALL_ACTUATOR, ACTION_ALWAYS,   &stateNull[0], &manageChildT1Alm1StAllActAlways},     /* 12 */
		  	{}
		  };

struct machineChild stateChildAlarmEmpty[] =
		  {
			{STATE_NULL, PARENT_NULL, CHILD_NULL,                            ACTION_NULL,     &stateNull[0], &manageNull},                             /* 0 */
			//alarm init
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_INIT,              ACTION_ON_ENTRY, &stateNull[0], &manageChildEmptyAlm1InitEntry},          /* 1 */
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_INIT,              ACTION_ALWAYS,   &stateNull[0], &manageChildEmptyAlm1InitAlways},         /* 2 */

			//allarme di aria nel filtro di depurazione
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_SAF_AIR_FILT,      ACTION_ON_ENTRY, &stateNull[0], &manageChildEmptyAlm1SafAirFiltEntry},    /* 3 */
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_SAF_AIR_FILT,      ACTION_ALWAYS,   &stateNull[0], &manageChildEmptyAlm1SafAirFiltAlways},   /* 4 */
			//allarme di aria nel circuito venoso
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_SFV_AIR,           ACTION_ON_ENTRY, &stateNull[0], &manageChildEmptyAlm1SFVEntry},           /* 5 */
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_SFV_AIR,           ACTION_ALWAYS,   &stateNull[0], &manageChildEmptyAlm1SFVAlways},          /* 6 */
			//allarme di aria nel circuito arterioso
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_SFA_AIR,           ACTION_ON_ENTRY, &stateNull[0], &manageChildEmptyAlm1SFAEntry},           /* 7 */
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_SFA_AIR,           ACTION_ALWAYS,   &stateNull[0], &manageChildEmptyAlm1SFAAlways},          /* 8 */

			//alarm in empty state finished
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_END,               ACTION_ON_ENTRY, &stateNull[0], &manageNull},                             /* 9 */
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_END,               ACTION_ALWAYS,   &stateNull[0], &manageNull},                             /* 10 */

			//alarm stop all actuator
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_STOP_ALL_ACTUATOR, ACTION_ON_ENTRY, &stateNull[0], &manageChildEmptyAlm1StAllActEntry},      /* 11 */
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_STOP_ALL_ACTUATOR, ACTION_ALWAYS,   &stateNull[0], &manageChildEmptyAlm1StAllActAlways},     /* 12 */

			// gestisce un eventuale allarme generato quando non si riesce a comunicare su MODBUS in scrittura o lettura
			{STATE_NULL, PARENT_NULL, CHILD_EMPTY_ALARM_MOD_BUS,            ACTION_ON_ENTRY, &stateNull[0], &manageChildEmptyAlmPumpNotStillEntry},   /* 13 */
			{STATE_NULL, PARENT_NULL, CHILD_EMPTY_ALARM_MOD_BUS,            ACTION_ALWAYS,   &stateNull[0], &manageChildEmptyAlmPumpNotStillAlways},  /* 14 */
			{}
		  };


struct machineChild stateChildAlarmTreat1[] =
		  {
			{STATE_NULL, PARENT_NULL, CHILD_NULL,                            ACTION_NULL,     &stateNull[0], &manageNull},                               /* 0 */
			//alarm init
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_INIT,              ACTION_ON_ENTRY, &stateNull[0], &manageNull},                               /* 1 */
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_INIT,              ACTION_ALWAYS,   &stateNull[0], &manageChildTreatAlm1InitAlways},           /* 2 */
			//alarm stop perfusion
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_STOP_PERFUSION,    ACTION_ON_ENTRY, &stateNull[0], &manageChildTreatAlm1StopPerfEntry},        /* 3 */
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_STOP_PERFUSION,    ACTION_ALWAYS,   &stateNull[0], &manageChildTreatAlm1StopPerfAlways},       /* 4 */
			//alarm stop purification
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_STOP_PURIFICATION, ACTION_ON_ENTRY, &stateNull[0], &manageChildTreatAlm1StopPurifEntry},       /* 5 */
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_STOP_PURIFICATION, ACTION_ALWAYS,   &stateNull[0], &manageChildTreatAlm1StopPurifAlways},      /* 6 */
			//alarm stop all pump
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_STOP_ALL_PUMP,     ACTION_ON_ENTRY, &stateNull[0], &manageChildTreatAlm1StopAllPumpEntry},     /* 7 */
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_STOP_ALL_PUMP,     ACTION_ALWAYS,   &stateNull[0], &manageChildTreatAlm1StopAllPumpAlways},    /* 8 */
			//alarm stop peltier
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_STOP_PELTIER,      ACTION_ON_ENTRY, &stateNull[0], &manageChildTreatAlm1StopPeltEntry},        /* 9 */
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_STOP_PELTIER,      ACTION_ALWAYS,   &stateNull[0], &manageChildTreatAlm1StopPeltAlways},       /* 10 */
			//alarm stop all actuator
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_STOP_ALL_ACTUATOR, ACTION_ON_ENTRY, &stateNull[0], &manageChildTreatAlm1StopAllActEntry},      /* 11 */
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_STOP_ALL_ACTUATOR, ACTION_ALWAYS,   &stateNull[0], &manageChildTreatAlm1StopAllActAlways},     /* 12 */

			//allarme di aria nel filtro di depurazione
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_SAF_AIR_FILT,      ACTION_ON_ENTRY, &stateNull[0], &manageChildTreatAlm1SafAirFiltEntry},      /* 13 */
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_SAF_AIR_FILT,      ACTION_ALWAYS,   &stateNull[0], &manageChildTreatAlm1SafAirFiltAlways},     /* 14 */
			//allarme di aria nel circuito venoso
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_SFV_AIR,           ACTION_ON_ENTRY, &stateNull[0], &manageChildTreatAlm1SFVEntry},             /* 15 */
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_SFV_AIR,           ACTION_ALWAYS,   &stateNull[0], &manageChildTreatAlm1SFVAlways},            /* 16 */
			//allarme di aria nel circuito arterioso
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_SFA_AIR,           ACTION_ON_ENTRY, &stateNull[0], &manageChildTreatAlm1SFAEntry},             /* 17 */
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_SFA_AIR,           ACTION_ALWAYS,   &stateNull[0], &manageChildTreatAlm1SFAAlways},            /* 18 */

			//alarm priming end
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_END,               ACTION_ON_ENTRY, &stateNull[0], &manageNull},                               /* 19 */
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_END,               ACTION_ALWAYS,   &stateNull[0], &manageNull},                               /* 20 */

			//gestisce una tipologia di allarmi in trattamento dove vengono fermati tutti gli attuatori poi l'utente
			// premendo BUTTON_RESET forza l'uscita dalla condizione di allarme senza fare nessun'altra operazione
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_WAIT_CMD,          ACTION_ON_ENTRY, &stateNull[0], &manageChildAlmAndWaitCmdEntry},            /* 21 */
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_WAIT_CMD,          ACTION_ALWAYS,   &stateNull[0], &manageChildAlmAndWaitCmdAlways},           /* 22 */

			// gestisce un eventuale allarme generato quando la posizione delle pinch confrontata con quella delle protective non coincide
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_BAD_PINCH_POS,       ACTION_ON_ENTRY, &stateNull[0], &manageChildTreatAlmBadPinchPosEntry},      /* 23 */
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_BAD_PINCH_POS,       ACTION_ALWAYS,   &stateNull[0], &manageChildTreatAlmBadPinchPosAlways},     /* 24 */

			// gestisce un eventuale allarme generato quando le ripetizioni di un comando su modbus superano un certo valore
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_MOD_BUS_ERROR,       ACTION_ON_ENTRY, &stateNull[0], &manageChildTreatAlmModBusErrEntry},        /* 25 */
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_MOD_BUS_ERROR,       ACTION_ALWAYS,   &stateNull[0], &manageChildTreatAlmModBusErrAlways},       /* 26 */
			{}
		  };

struct machineChild stateChildAlarmTreat2[] =
		 {
		  {STATE_NULL, PARENT_NULL, CHILD_NULL,                            ACTION_NULL, &stateNull[0], &manageNull},
		  {STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_2_INIT,              ACTION_NULL, &stateNull[0], &manageNull},
		  {STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_2_STOP_PERFUSION,    ACTION_NULL, &stateNull[0], &manageNull},
		  {STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_2_STOP_PURIFICATION, ACTION_NULL, &stateNull[0], &manageNull},
		  {STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_2_STOP_ALL_PUMP,     ACTION_NULL, &stateNull[0], &manageNull},
		  {STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_2_STOP_PELTIER,      ACTION_NULL, &stateNull[0], &manageNull},
		  {STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_2_STOP_ALL_ACTUATOR, ACTION_NULL, &stateNull[0], &manageNull},
		  {STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_2_END,               ACTION_NULL, &stateNull[0], &manageNull},
		  {}
		 };

/**********************************************/
/***********CHILD ACTIVE***********/
/**********************************************/
struct machineChild stateChildAlarmPriming[] ={
		{STATE_NULL, PARENT_NULL, CHILD_NULL, 								ACTION_NULL, 		&stateNull[0], &manageChildNull},	                    /* 0 */
		//alarm init
		{STATE_NULL, PARENT_NULL, CHILD_PRIMING_ALARM_INIT, 				ACTION_ON_ENTRY, 	&stateNull[0], &manageChildNull},	                    /* 1 */
		{STATE_NULL, PARENT_NULL, CHILD_PRIMING_ALARM_INIT, 				ACTION_ALWAYS, 		&stateNull[0], &manageChildNull},	                    /* 2 */
		//alarm stop perfusion
		{STATE_NULL, PARENT_NULL, CHILD_PRIMING_ALARM_STOP_PERFUSION, 		ACTION_ON_ENTRY, 	&stateNull[0], &manageChildPrimAlarmStopPerfEntry},	    /* 3 */
		{STATE_NULL, PARENT_NULL, CHILD_PRIMING_ALARM_STOP_PERFUSION, 		ACTION_ALWAYS, 		&stateNull[0], &manageChildPrimAlarmStopPerfAlways},	/* 4 */
		//alarm stop purification
		{STATE_NULL, PARENT_NULL, CHILD_PRIMING_ALARM_STOP_PURIFICATION, 	ACTION_ON_ENTRY, 	&stateNull[0], &manageChildPrimAlarmStopPurifEntry},	/* 5 */
		{STATE_NULL, PARENT_NULL, CHILD_PRIMING_ALARM_STOP_PURIFICATION, 	ACTION_ALWAYS, 		&stateNull[0], &manageChildPrimAlarmStopPurifAlways},	/* 6 */
		//alarm stop all pump
		{STATE_NULL, PARENT_NULL, CHILD_PRIMING_ALARM_STOP_ALL_PUMP, 		ACTION_ON_ENTRY, 	&stateNull[0], &manageChildPrimAlarmStopAllPumpEntry},	/* 7 */
		{STATE_NULL, PARENT_NULL, CHILD_PRIMING_ALARM_STOP_ALL_PUMP, 		ACTION_ALWAYS, 		&stateNull[0], &manageChildPrimAlarmStopAllPumpAlways},	/* 8 */
		//alarm stop peltier
		{STATE_NULL, PARENT_NULL, CHILD_PRIMING_ALARM_STOP_PELTIER, 		ACTION_ON_ENTRY, 	&stateNull[0], &manageChildPrimAlarmStopPeltEntry},	    /* 9 */
		{STATE_NULL, PARENT_NULL, CHILD_PRIMING_ALARM_STOP_PELTIER, 		ACTION_ALWAYS, 		&stateNull[0], &manageChildPrimAlarmStopPeltAlways},	/* 10 */
		//alarm stop all actuator
		{STATE_NULL, PARENT_NULL, CHILD_PRIMING_ALARM_STOP_ALL_ACTUATOR, 	ACTION_ON_ENTRY, 	&stateNull[0], &manageChildPrimAlarmStopAllActEntry},	/* 11 */
		{STATE_NULL, PARENT_NULL, CHILD_PRIMING_ALARM_STOP_ALL_ACTUATOR, 	ACTION_ALWAYS, 		&stateNull[0], &manageChildPrimAlarmStopAllActAlways},	/* 12 */
		//alarm priming end
		{STATE_NULL, PARENT_NULL, CHILD_PRIMING_ALARM_END, 					ACTION_ON_ENTRY, 	&stateNull[0], &manageChildNull},	                    /* 13 */
		{STATE_NULL, PARENT_NULL, CHILD_PRIMING_ALARM_END, 					ACTION_ALWAYS, 		&stateNull[0], &manageChildNull},	                    /* 14 */

		//gestisce una tipologia di allarmi in trattamento dove vengono fermati tutti gli attuatori poi l'utente
		// premendo BUTTON_RESET forza l'uscita dalla condizione di allarme senza fare nessun'altra operazione
		{STATE_NULL, PARENT_NULL, CHILD_PRIM_ALARM_1_WAIT_CMD,             ACTION_ON_ENTRY,    &stateNull[0], &manageChildPrimAlmAndWaitCmdEntry},     /* 15 */
		{STATE_NULL, PARENT_NULL, CHILD_PRIM_ALARM_1_WAIT_CMD,             ACTION_ALWAYS,      &stateNull[0], &manageChildPrimAlmAndWaitCmdAlways},    /* 16 */

		// gestisce un eventuale allarme generato quando non si riesce a fermare le pompe
		{STATE_NULL, PARENT_NULL, CHILD_PRIM_ALARM_PUMPS_NOT_STILL,        ACTION_ON_ENTRY,    &stateNull[0], &manageChildPrimAlmPumpNotStillEntry},   /* 17 */
		{STATE_NULL, PARENT_NULL, CHILD_PRIM_ALARM_PUMPS_NOT_STILL,        ACTION_ALWAYS,      &stateNull[0], &manageChildPrimAlmPumpNotStillAlways},  /* 18 */

		// gestisce un eventuale allarme generato quando la posizione delle pinch confrontata con quella delle protective non coincide
		{STATE_NULL, PARENT_NULL, CHILD_PRIM_ALARM_BAD_PINCH_POS,          ACTION_ON_ENTRY,    &stateNull[0], &manageChildPrimAlmBadPinchPosEntry},    /* 19 */
		{STATE_NULL, PARENT_NULL, CHILD_PRIM_ALARM_BAD_PINCH_POS,          ACTION_ALWAYS,      &stateNull[0], &manageChildPrimAlmBadPinchPosAlways},   /* 20 */

		// gestisce un allarme generato dal pericolo di pompare aria nel filtro durante il priming
		{STATE_NULL, PARENT_NULL, CHILD_PRIM_ALARM_SFA_AIR_DET,            ACTION_ON_ENTRY,    &stateNull[0], &manageChildPrimAlmSFAAirDetEntry},      /* 21 */
		{STATE_NULL, PARENT_NULL, CHILD_PRIM_ALARM_SFA_AIR_DET,            ACTION_ALWAYS,      &stateNull[0], &manageChildPrimAlmSFAAirDetAlways},     /* 22 */

		// gestisce un allarme generato da un errore grave nella comunicazione modbus
		{STATE_NULL, PARENT_NULL, CHILD_PRIM_ALARM_MOD_BUS,                ACTION_ON_ENTRY,    &stateNull[0], &manageChildPrimAlmModBusEntry},         /* 23 */
		{STATE_NULL, PARENT_NULL, CHILD_PRIM_ALARM_MOD_BUS,                ACTION_ALWAYS,      &stateNull[0], &manageChildPrimAlmModBusAlways},        /* 24 */
		{}
};


/**********************************************/
/**********************************************/
/*********************************************/

/*********************************/
/*********PARENT LEVEL*************/
/*********************************/

// (FM)
// guard sono comandi che possono essere ricevuti dalla seriale, dalla tastiera, dal sistema di allarme

// machineParent         struttura usata per la gestione di tutti i comandi ricevuti nel particolare stateState in cui mi trovo
// {STATE_NULL,          questo valore e' sempre STATE_NULL perche' questo campo della struttura non e' mai usato
//  PARENT_NULL,         questo campo identifica uno stato del parent ( cioe' del sw che gestisce i comendi ricevuti(guard))
//                       Il codice che esegue e' quello per gestire i comandi (tastiera, seriale, ecc) a cui deve rispondere
//  CHILD_NULL,          questo campo e' sempre fisso a CHILD_NULL perche' non viene mai usato
//  ACTION_NULL,         questo campo indica quando (ACTION_ON_ENTRY, ACTION_ALWAYS) devo effettuare le operazioni fatte nella
//                       funzione manageNull (*)
//  &stateChildNull[0],  questo e' l'indirizzo dell'array di strutture machineChild associate a questo stato della macchina
//                       Sono operazioni da eseguire in conseguenza di comandi ricevuti.
//  &manageNull},        Operazioni da eseguire in base al valore del campo (*)

struct machineParent stateParentNull[] =
		{
		 {STATE_NULL, PARENT_NULL, CHILD_NULL, ACTION_NULL, &stateChildNull[0], &manageNull},
		 {STATE_NULL, PARENT_NULL, CHILD_NULL, ACTION_NULL, &stateChildNull[0], &manageNull},
		 {}
		};

struct machineParent stateParentEntry[] =
		{
		 {STATE_NULL, PARENT_NULL,      CHILD_NULL, ACTION_NULL,     &stateChildIdle[0],  &manageNull},              /* 0 */

		 {STATE_NULL, PARENT_ENTRY,     CHILD_NULL, ACTION_ON_ENTRY, &stateChildEntry[1], &manageParentEntry},       /* 1 */
		 {STATE_NULL, PARENT_ENTRY,     CHILD_NULL, ACTION_ALWAYS,   &stateChildEntry[2], &manageParentEntryAlways}, /* 2 */

		 {}
		};

struct machineParent stateParentIdle[] =
		{
		 {STATE_NULL, PARENT_IDLE, CHILD_IDLE, ACTION_NULL, &stateChildIdle[0], &manageNull},
		 {STATE_NULL, PARENT_IDLE, CHILD_IDLE, ACTION_NULL, &stateChildIdle[1], &manageNull},
		 // Filippo - aggiunto stato per gestire l'allarme in IDLE
		 {STATE_NULL, PARENT_IDLE_ALARM, CHILD_IDLE, ACTION_NULL, &stateChildAlarmIdle[2], &manageNull},
 		 {}
		};

struct machineParent stateParentT1TNoDisposable[] =
		{
		 {STATE_NULL, PARENT_NULL,                    CHILD_NULL, ACTION_NULL,     &stateChildIdle[0], &manageNull}, /* 0 */
		 {STATE_NULL, PARENT_T1_NO_DISP_INIT,         CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageNull}, /* 1 */ /* init */
		 {STATE_NULL, PARENT_T1_NO_DISP_INIT,         CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[1], &manageNull}, /* 2 */ /* init */
		 {STATE_NULL, PARENT_T1_NO_DISP_CHK_CONFG,    CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageParentChkConfig}, /* 3 */ /* t1 check confg data */
		 {STATE_NULL, PARENT_T1_NO_DISP_CHK_CONFG,    CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[1], &manageNull}, /* 4 */ /* t1 check confg data */
		 {STATE_NULL, PARENT_T1_NO_DISP_CHK_24VBRK,   CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageParentChk24Vbrk}, /* 5 */ /* t1 check 24VBRK */
		 {STATE_NULL, PARENT_T1_NO_DISP_CHK_24VBRK,   CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[1], &manageNull}, /* 6 */ /* t1 check 24VBRK */
		 {STATE_NULL, PARENT_T1_NO_DISP_CHECK_PRESS,  CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageParentChkPress}, /* 7 */ /* t1 chk press. sensor */
		 {STATE_NULL, PARENT_T1_NO_DISP_CHECK_PRESS,  CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[1], &manageNull}, /* 8 */ /* t1 chk press. sensor */
		 {STATE_NULL, PARENT_T1_NO_DISP_CHECK_TEMP,   CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageParentTempSensIR}, /* 9 */ /* t1 chk temp. sensor */
		 {STATE_NULL, PARENT_T1_NO_DISP_CHECK_TEMP,   CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[1], &manageNull}, /* 10 */ /* t1 chk temp. sensor */
		 {STATE_NULL, PARENT_T1_NO_DISP_CHECK_LEVEL,  CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageLevelSensorTest}, /* 11 */ /* t1 chk level sensor */
		 {STATE_NULL, PARENT_T1_NO_DISP_CHECK_LEVEL,  CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[1], &manageNull}, /* 12 */ /* t1 chk level sensor */
		 {STATE_NULL, PARENT_T1_NO_DISP_CHECK_FLWMTR, CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &mangeParentUFlowSens}, /* 13 */ /* t1 chk flowmeter sensor */
		 {STATE_NULL, PARENT_T1_NO_DISP_CHECK_FLWMTR, CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[1], &manageNull}, /* 14 */ /* t1 chk flowmeter sensor */
	     {STATE_NULL, PARENT_T1_NO_DISP_CHEK_AIR,     CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageNull}, /* 15 */ /* t1 chk air sensor */
		 {STATE_NULL, PARENT_T1_NO_DISP_CHEK_AIR,     CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[1], &manageNull}, /* 16 */ /* t1 chk air sensor */
		 {STATE_NULL, PARENT_T1_NO_DISP_CHEK_PINCH,   CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageParenT1PinchInit}, /* 17 */ /* t1 chk pinch */
		 {STATE_NULL, PARENT_T1_NO_DISP_CHEK_PINCH,   CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[1], &manageParenT1Pinch}, /* 18 */ /* t1 chk pinch */
		 {STATE_NULL, PARENT_T1_NO_DISP_CHEK_PUMP,    CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageParentT1PumpInit}, /* 19 */ /* t1 chk pump */
		 {STATE_NULL, PARENT_T1_NO_DISP_CHEK_PUMP,    CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[1], &manageParentT1Pump}, /* 20 */ /* t1 chk pump */
		 {STATE_NULL, PARENT_T1_NO_DISP_CHEK_PELTIER, CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageNull}, /* 21 */ /* t1 chk Peltier */
		 {STATE_NULL, PARENT_T1_NO_DISP_CHEK_PELTIER, CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[1], &manageNull}, /* 22 */ /* t1 chk Peltier */
		 {STATE_NULL, PARENT_T1_NO_DISP_END,          CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageNull}, /* 23 */ /* t1 test end */
		 {STATE_NULL, PARENT_T1_NO_DISP_END,          CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[1], &manageNull}, /* 24 */ /* t1 test end */
		 {STATE_NULL, PARENT_T1_NO_DISP_ALARM,        CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageNull}, /* 25 */ /* t1 test alarm */
		 {STATE_NULL, PARENT_T1_NO_DISP_ALARM,        CHILD_IDLE, ACTION_ALWAYS,   &stateChildAlarmT1[2], &manageNull}, /* 26 */ /* t1 test alarm */
		 {STATE_NULL, PARENT_T1_NO_DISP_FATAL_ERROR,  CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageNull}, /* 27 */ /* t1 test error */
		 {STATE_NULL, PARENT_T1_NO_DISP_FATAL_ERROR,  CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[1], &manageNull}, /* 28 */ /* t1 test error */
		 {STATE_NULL, PARENT_T1_NO_DISP_CHECK_HEATER, CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageParentT1HeaterInit}, /* 29 */ /* t1 chk pump */
		 {STATE_NULL, PARENT_T1_NO_DISP_CHECK_HEATER, CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[1], &manageParentT1Heater}, /* 30 */ /* t1 chk pump */
		 {STATE_NULL, PARENT_T1_NO_DISP_CHECK_FRIDGE, CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageParentT1FridgeInit}, /* 29 */ /* t1 chk pump */
		 {STATE_NULL, PARENT_T1_NO_DISP_CHECK_FRIDGE, CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[1], &manageParentT1Fridge}, /* 30 */ /* t1 chk pump */

		 {}
		};

struct machineParent stateParentT1TWithDisposable[] =
		{
		 {STATE_NULL, PARENT_NULL,                     CHILD_NULL, ACTION_NULL,     &stateChildIdle[0], &manageNull},
		 {STATE_NULL, PARENT_T1_WITH_DISP_INIT,        CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_WITH_DISP_INIT,        CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_WITH_DISP_LEAK,        CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_WITH_DISP_LEAK,        CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_WITH_DISP_LEVEL,       CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_WITH_DISP_LEVEL,       CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_WITH_DISP_ALARM,       CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_WITH_DISP_ALARM,       CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_WITH_DISP_END,         CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_WITH_DISP_END,         CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_WITH_DISP_FATAL_ERROR, CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_WITH_DISP_FATAL_ERROR, CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[1], &manageNull},
		 {}
		};

struct machineParent stateParentPrimingTreatKidney1[] =
{
		{STATE_NULL, PARENT_NULL,                         CHILD_NULL, ACTION_NULL,     &stateChildIdle[0],         &manageNull},					  /* 0 */
		/* priming init */
		{STATE_NULL, PARENT_PRIMING_TREAT_KIDNEY_1_INIT,  CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[0],         &manageParentPrimingEntry},	      /* 1 */
		{STATE_NULL, PARENT_PRIMING_TREAT_KIDNEY_1_INIT,  CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[0],         &manageParentPrimingAlways},	      /* 2 */
		/* priming run */
		{STATE_NULL, PARENT_PRIMING_TREAT_KIDNEY_1_RUN,   CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[0],         &manageParentPrimingEntry},	      /* 3 */
		{STATE_NULL, PARENT_PRIMING_TREAT_KIDNEY_1_RUN,   CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[0],         &manageParentPrimingAlways},		  /* 4 */
		/* priming alarm */
		{STATE_NULL, PARENT_PRIMING_TREAT_KIDNEY_1_ALARM, CHILD_IDLE, ACTION_ON_ENTRY, &stateChildAlarmPriming[1], &manageParentPrimingAlarmEntry},	  /* 5 */
		{STATE_NULL, PARENT_PRIMING_TREAT_KIDNEY_1_ALARM, CHILD_IDLE, ACTION_ALWAYS,   &stateChildAlarmPriming[1], &manageParentPrimingAlarmAlways},  /* 6 */
		/* priming end */
		{STATE_NULL, PARENT_PRIMING_TREAT_KIDNEY_1_END,   CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[0],         &manageParentPrimingEntry},	      /* 7 */
		{STATE_NULL, PARENT_PRIMING_TREAT_KIDNEY_1_END,   CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[0],         &manageParentPrimingAlways},		  /* 8 */

		/* stato di pausa del priming attivato partendo da PARENT_PRIMING_TREAT_KIDNEY_1_RUN mediante pressione del tasto priming stop */
		{STATE_NULL, PARENT_PRIM_WAIT_PAUSE,              CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[0],         &manageParentPrimWaitPauseEntry},  /* 9 */
		{STATE_NULL, PARENT_PRIM_WAIT_PAUSE,              CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[0],         &manageParentPrimingAlways},       /* 10 */

		/* stato attivato alla fine del ricircolo per aspettare che i motori siano completamente fermi. Alla fine del ricircolo ora non va piu'
		 * in trattamento diretto
		 * Se i motori non si fermano entro un certo tempo genero un allarme */
		{STATE_NULL, PARENT_PRIM_WAIT_MOT_STOP,          CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[0],          &manageParPrimWaitMotStopEntry},	      /* 11 */
		{STATE_NULL, PARENT_PRIM_WAIT_MOT_STOP,          CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[0],          &manageParPrimWaitMotStopEntryAlways}, /* 12 */
		/* stato attesa di chusura delle pinch una volta fermati i motori */
		{STATE_NULL, PARENT_PRIM_WAIT_PINCH_CLOSE,       CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[0],          &manageParPrimWaitPinchCloseEntry},	  /* 13 */
		{STATE_NULL, PARENT_PRIM_WAIT_PINCH_CLOSE,       CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[0],          &manageParPrimWaitPinchCloseAlways},   /* 14 */
		/* stato per la gestione degli allarmi che si possono verificare negli stati PARENT_PRIM_WAIT_MOT_STOP, PARENT_PRIM_WAIT_PINCH_CLOSE*/
		{STATE_NULL, PARENT_PRIMING_END_RECIRC_ALARM,    CHILD_IDLE, ACTION_ON_ENTRY, &stateChildAlarmPriming[1],  &manageParPrimEndRecAlarmEntry},	      /* 15 */
		{STATE_NULL, PARENT_PRIMING_END_RECIRC_ALARM,    CHILD_IDLE, ACTION_ALWAYS,   &stateChildAlarmPriming[1],  &manageParPrimEndRecAlarmAlways},      /* 16 */

		/* treatment aria rilevata dal sensore digitale, cerco di svuotare */
		{STATE_NULL, PARENT_PRIM_KIDNEY_1_AIR_FILT,     CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[0],           &manageParentPrimAirFiltEntry},	      /* 17 */
		{STATE_NULL, PARENT_PRIM_KIDNEY_1_AIR_FILT,     CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[0],           &manageParentPrimAirFiltAlways},	      /* 18 */
		/* treatment allarme durante la procedura di recupero da un allarme aria */
		{STATE_NULL, PARENT_PRIM_KIDNEY_1_ALM_AIR_REC,  CHILD_IDLE, ACTION_ON_ENTRY, &stateChildAlarmPriming[1],   &manageParentPrimAirAlmRecEntry},	  /* 19 */
		{STATE_NULL, PARENT_PRIM_KIDNEY_1_ALM_AIR_REC,  CHILD_IDLE, ACTION_ALWAYS,   &stateChildAlarmPriming[1],   &manageParentPrimAirAlmRecAlways},     /* 20 */
		{}
};


struct machineParent stateParentTreatKidney1[] =
{
		{STATE_NULL, PARENT_NULL,                        CHILD_NULL, ACTION_NULL,     &stateChildIdle[0],        &manageNull},						 /* 0 */
		/* treatment init */
		{STATE_NULL, PARENT_TREAT_KIDNEY_1_INIT,         CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[0],        &manageParentTreatEntry},			 /* 1 */
		{STATE_NULL, PARENT_TREAT_KIDNEY_1_INIT,         CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[0],        &manageParentTreatAlways},			 /* 2 */
		/* treatment run */
		{STATE_NULL, PARENT_TREAT_KIDNEY_1_PUMP_ON,      CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[0],        &manageParentTreatEntry},			 /* 3 */
		{STATE_NULL, PARENT_TREAT_KIDNEY_1_PUMP_ON,      CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[0],        &manageParentTreatAlways},			 /* 4 */
		/* treatment alarm */
		{STATE_NULL, PARENT_TREAT_KIDNEY_1_ALARM,        CHILD_IDLE, ACTION_ON_ENTRY, &stateChildAlarmTreat1[1], &manageParentTreatAlarmEntry},		 /* 5 */
		{STATE_NULL, PARENT_TREAT_KIDNEY_1_ALARM,        CHILD_IDLE, ACTION_ALWAYS,   &stateChildAlarmTreat1[1], &manageParentTreatAlarmAlways},	 /* 6 */


		/* treatment aria rilevata dal sensore digitale, cerco di svuotare */
		{STATE_NULL, PARENT_TREAT_KIDNEY_1_AIR_FILT,     CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[0],        &manageParentTreatAirFiltEntry},	 /* 7 */
		{STATE_NULL, PARENT_TREAT_KIDNEY_1_AIR_FILT,     CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[0],        &manageParentTreatAirFiltAlways},	 /* 8 */
		/* treatment aria rilevata dal sensore venos, cerco di svuotareo */
		{STATE_NULL, PARENT_TREAT_KIDNEY_1_SFV,          CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[0],        &manageParentTreatSFVEntry},		 /* 9 */
		{STATE_NULL, PARENT_TREAT_KIDNEY_1_SFV,          CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[0],        &manageParentTreatSFVAlways},		 /* 10 */
		/* treatment aria rilevata dal sensore arterioso, cerco di svuotare */
		{STATE_NULL, PARENT_TREAT_KIDNEY_1_SFA,          CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[0],        &manageParentTreatSFAEntry},		 /* 11 */
		{STATE_NULL, PARENT_TREAT_KIDNEY_1_SFA,          CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[0],        &manageParentTreatSFAAlways},		 /* 12 */
		/* treatment allarme durante la procedura di recupero da un allarme aria */
		{STATE_NULL, PARENT_TREAT_KIDNEY_1_ALM_AIR_REC,  CHILD_IDLE, ACTION_ON_ENTRY, &stateChildAlarmTreat1[1], &manageParentTreatAirAlmRecEntry},	 /* 13 */
		{STATE_NULL, PARENT_TREAT_KIDNEY_1_ALM_AIR_REC,  CHILD_IDLE, ACTION_ALWAYS,   &stateChildAlarmTreat1[1], &manageParentTreatAirAlmRecAlways}, /* 14 */

		/* treatment end */
		{STATE_NULL, PARENT_TREAT_KIDNEY_1_END,          CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[0],        &manageParentTreatEndEntry},		 /* 15 */
		{STATE_NULL, PARENT_TREAT_KIDNEY_1_END,          CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[0],        &manageParentTreatEndAlways},	     /* 16 */

		/* stop treatment ricevuto nella fase PARENT_TREAT_KIDNEY_1_INIT*/
		{STATE_NULL, PARENT_TREAT_WAIT_START,            CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[0],        &manageParentTreatWaitStartEntry},	 /* 17 */
		{STATE_NULL, PARENT_TREAT_WAIT_START,            CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[0],        &manageParentTreatAlways},		     /* 18 */

		/* stop treatment ricevuto nella fase PARENT_TREAT_KIDNEY_1_PUMP_ON*/
		{STATE_NULL, PARENT_TREAT_WAIT_PAUSE,           CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[0],         &manageParentTreatWaitPauseEntry},  /* 19 */
		{STATE_NULL, PARENT_TREAT_WAIT_PAUSE,           CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[0],         &manageParentTreatAlways},		     /* 20 */
		{}
};

struct machineParent stateParentEmptyDisp[] =
{
		 {STATE_NULL, PARENT_NULL,                   CHILD_NULL, ACTION_NULL,     &stateChildIdle[0],       &manageNull},                         /* 0 */

		 {STATE_NULL, PARENT_EMPTY_DISPOSABLE_INIT,  CHILD_NULL, ACTION_ON_ENTRY, &stateChildIdle[0],       &manageParentEmptyDisposInitEntry},   /* 1 */
		 {STATE_NULL, PARENT_EMPTY_DISPOSABLE_INIT,  CHILD_NULL, ACTION_ALWAYS,   &stateChildIdle[0],       &manageParentEmptyDisposInitAlways},  /* 2 */

		 {STATE_NULL, PARENT_EMPTY_DISPOSABLE_RUN,   CHILD_NULL, ACTION_ON_ENTRY, &stateChildIdle[0],       &manageParentEmptyDisposRunEntry},    /* 3 */
		 {STATE_NULL, PARENT_EMPTY_DISPOSABLE_RUN,   CHILD_NULL, ACTION_ALWAYS,   &stateChildIdle[0],       &manageParentEmptyDisposRunAlways},   /* 4 */

		 {STATE_NULL, PARENT_EMPTY_DISPOSABLE_ALARM, CHILD_NULL, ACTION_ON_ENTRY, &stateChildAlarmEmpty[1], &manageParentEmptyDisposAlarmEntry},  /* 5 */
		 {STATE_NULL, PARENT_EMPTY_DISPOSABLE_ALARM, CHILD_NULL, ACTION_ALWAYS,   &stateChildAlarmEmpty[1], &manageParentEmptyDisposAlarmAlways}, /* 6 */

		 {STATE_NULL, PARENT_EMPTY_DISPOSABLE_END,   CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[0],        &manageParentEmptyDisposEndEntry},	   /* 7 */
		 {STATE_NULL, PARENT_EMPTY_DISPOSABLE_END,   CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[0],        &manageParentEmptyDisposEndAlways},   /* 8 */
		 {}
};


/*********************************/
/*********SATATE LEVEL*************/
/*********************************/

/*
  (FM)
  descrizione singola struttura      machineState
  { STATE_NULL,                      stato reale della macchina
    PARENT_NULL,                     valore sempre nullo per ora (mai usato)
    CHILD_NULL,                      valore sempre nullo per ora (mai usato)
    ACTION_ON_ENTRY(ACTION_NULL),    quando deve avvenire l'azione specificata nell'ultimo parametro (*)
    &stateParentNull[0],             insirizzo dell'array di strutture machineParent da utilizzare in questo stato
    &manageNull},                    azione da effettuare quando si entra nello stato o sempre a secondo
                                     di quanto specificato in (*)
*/
struct machineState stateState[] =
		{

		 /**************************************************************************************/
		 /******-----------------------------MACHINE STATE------------------------------********/
		 /**************************************************************************************/
		{STATE_NULL,               PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY/*ACTION_NULL*/, &stateParentNull[0],                &manageNull},					/* 0 */

		{STATE_ENTRY,              PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY,                &stateParentEntry[1],               &manageStateEntry},				/* 1 */
		{STATE_ENTRY,              PARENT_NULL, CHILD_NULL, ACTION_ALWAYS,                  &stateParentEntry[2],               &manageStateEntryAlways},		/* 2 */

		{STATE_IDLE,               PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY,                &stateParentIdle[1],                &manageStateIdle},				/* 3 */
		{STATE_IDLE,               PARENT_NULL, CHILD_NULL, ACTION_ALWAYS,                  &stateParentIdle[1],                &manageStateIdleAlways},		/* 4 */

		{STATE_SELECT_TREAT,       PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY,                &stateParentNull[0],                &manageStateSelTreat},			/* 5 */
		{STATE_SELECT_TREAT,       PARENT_NULL, CHILD_NULL, ACTION_ALWAYS,                  &stateParentNull[0],                &manageStateSelTreatAlways},	/* 6 */

		{STATE_T1_NO_DISPOSABLE,   PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY,                &stateParentT1TNoDisposable[1],     &manageStateT1NoDisp},			/* 7 */
		{STATE_T1_NO_DISPOSABLE,   PARENT_NULL, CHILD_NULL, ACTION_ALWAYS,                  &stateParentT1TNoDisposable[2],     &manageStateT1NoDispAlways},	/* 8 */

		{STATE_MOUNTING_DISP,      PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY,                &stateParentNull[0],                &manageStateMountDisp},			/* 9 */
		{STATE_MOUNTING_DISP,      PARENT_NULL, CHILD_NULL, ACTION_ALWAYS,                  &stateParentNull[0],                &manageStateMountDispAlways},	/* 10 */

		{STATE_TANK_FILL,          PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY,                &stateParentNull[0],                &manageStateTankFill},			/* 11 */
		{STATE_TANK_FILL,          PARENT_NULL, CHILD_NULL, ACTION_ALWAYS,                  &stateParentNull[0],                &manageStateTankFillAlways},	/* 12 */

		// Questa volta entro direttamente nello stato run del priming perche' non e' prevista la ricezione di un comando BUTTON_CONFIRM
		{STATE_PRIMING_PH_1,       PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY,                &stateParentPrimingTreatKidney1[3], &managePrimingPh1},				/* 13 */
		{STATE_PRIMING_PH_1,       PARENT_NULL, CHILD_NULL, ACTION_ALWAYS,                  &stateParentPrimingTreatKidney1[3], &managePrimingPh1Always},		/* 14 */

		// Questa volta entro direttamente nello stato run del priming perche' non e' prevista la ricezione di un comando BUTTON_CONFIRM
		{STATE_PRIMING_PH_2,       PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY,                &stateParentPrimingTreatKidney1[3], &managePrimingPh2},				/* 15 */
		{STATE_PRIMING_PH_2,       PARENT_NULL, CHILD_NULL, ACTION_ALWAYS,                  &stateParentPrimingTreatKidney1[3], &managePrimingPh2Always},		/* 16 */

		// stato di trattamento kidney o liver. Co le ultime modifiche che mi sono state richieste (partenza con BUTTON_START_TREATMENT dopo
		// che le pompe sono ferme e le pinch chiuse) non posso andare allo stato 17 (PARENT_TREAT_WAIT_START) perche', in quello stato, aspetterebbe
		// ancora un'altro BUTTON_START_TREATMENT. Quindi, devo andare per forza nello stato 1 (PARENT_TREAT_KIDNEY_1_INIT).
		// Dato che non si puo' modificare la gui sono costretto a ritornare allo stato 17 (02__05_2018)
		{STATE_TREATMENT_KIDNEY_1, PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY,                &stateParentTreatKidney1[17]/*[1]*/, &manageStateTreatKidney1},		/* 17 */
		{STATE_TREATMENT_KIDNEY_1, PARENT_NULL, CHILD_NULL, ACTION_ALWAYS,                  &stateParentTreatKidney1[17]/*[1]*/, &manageStateTreatKidney1Always},/* 18 */

		// stato di svuotamento del disposable
		{STATE_EMPTY_DISPOSABLE,   PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY,                &stateParentEmptyDisp[1],           &manageStateEmptyDisp},			/* 19 */
		{STATE_EMPTY_DISPOSABLE,   PARENT_NULL, CHILD_NULL, ACTION_ALWAYS,                  &stateParentEmptyDisp[1],           &manageStateEmptyDispAlways},	/* 20 */

		// stato di attesa del tasto priming end, abbandona o di un nuovo volumr per continuare il priming
		{STATE_PRIMING_WAIT,       PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY,                &stateParentNull[0],                &manageStatePrimingWait},		/* 21 */
		{STATE_PRIMING_WAIT,       PARENT_NULL, CHILD_NULL, ACTION_ALWAYS,                  &stateParentNull[0],                &manageStatePrimingWaitAlways},	/* 22 */

		{STATE_PRIMING_RICIRCOLO,  PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY,                &stateParentPrimingTreatKidney1[3], &manageStatePrimingRicircolo},		 /* 23 */
		{STATE_PRIMING_RICIRCOLO,  PARENT_NULL, CHILD_NULL, ACTION_ALWAYS,                  &stateParentPrimingTreatKidney1[3], &manageStatePrimingRicircoloAlways}, /* 24 */

		// Questo stato non viene usato per ora
		{STATE_WAIT_TREATMENT,     PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY,                &stateParentNull[0],                &manageStateWaitTreatment},			 /* 25 */
		{STATE_WAIT_TREATMENT,     PARENT_NULL, CHILD_NULL, ACTION_ALWAYS,                  &stateParentNull[0],                &manageStateWaitTreatmentAlways},	 /* 26 */

		// stato di smontaggio del disposable
		{STATE_UNMOUNT_DISPOSABLE, PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY,                &stateParentNull[0],                &manageStateUnmountDisposableEntry},  /* 27 */
		{STATE_UNMOUNT_DISPOSABLE, PARENT_NULL, CHILD_NULL, ACTION_ALWAYS,                  &stateParentNull[0],                &manageStateUnmountDisposableAlways}, /* 28 */


		/**************************************************************************************/
		/******-----------------------------MACHINE STATE------------------------------********/
		/**************************************************************************************/

		 //{STATE_T1_WITH_DISPOSABLE, PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY, &stateParentNull[0], &manageStateT1WithDisp},
		 //{STATE_T1_WITH_DISPOSABLE, PARENT_NULL, CHILD_NULL, ACTION_ALWAYS, &stateParentNull[0], &manageStateT1WithDispAlways},
		 //{STATE_T1_WITH_DISPOSABLE, PARENT_T1_WITH_DISP_INIT, CHILD_NULL, ACTION_ON_EXIT, &stateParentIdle[2], &manageNull},

		 //{STATE_PRIMING_TREAT_1, PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY, &stateParentNull[0], &manageStatePrimingTreat1},
		 //{STATE_PRIMING_TREAT_1, PARENT_NULL, CHILD_NULL, ACTION_ALWAYS, &stateParentNull[0], &manageStatePrimingTreat1Always},

		 //{STATE_PRIMING_TREAT_2, PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY, &stateParentNull[0], &manageStatePrimingTreat2},
		 //{STATE_PRIMING_TREAT_2, PARENT_NULL, CHILD_NULL, ACTION_ALWAYS, &stateParentNull[0], &manageStatePrimingTreat2Always},


		 {STATE_TREATMENT_2,        PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY, &stateParentNull[0], &manageStateTreat2},                                               /* 29 */
		 {STATE_TREATMENT_2,        PARENT_NULL, CHILD_NULL, ACTION_ALWAYS,   &stateParentNull[0], &manageStateTreat2},                                               /* 30 */

		 {STATE_EMPTY_DISPOSABLE_1, PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY, &stateParentNull[0], &manageStateEmptyDisp1},                                           /* 31 */
		 {STATE_EMPTY_DISPOSABLE_1, PARENT_NULL, CHILD_NULL, ACTION_ALWAYS,   &stateParentNull[0], &manageStateEmptyDisp1Always},                                     /* 32 */

		 {STATE_EMPTY_DISPOSABLE_2, PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY, &stateParentNull[0], &manageStateEmptyDisp2},                                           /* 33 */
		 {STATE_EMPTY_DISPOSABLE_2, PARENT_NULL, CHILD_NULL, ACTION_ALWAYS,   &stateParentNull[0], &manageStateEmptyDisp2Always},                                     /* 34 */

		 {STATE_WASHING,            PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY, &stateParentNull[0], &manageStateWashing},                                              /* 35 */
		 {STATE_WASHING,            PARENT_NULL, CHILD_NULL, ACTION_ALWAYS,   &stateParentNull[0], &manageStateWashingAlways},                                        /* 36 */

		 {STATE_FATAL_ERROR,        PARENT_NULL, CHILD_NULL, ACTION_NULL,     &stateParentNull[0], &manageStateFatalError},                                           /* 37 */
		 {STATE_FATAL_ERROR,        PARENT_NULL, CHILD_NULL, ACTION_NULL,     &stateParentNull[0], &manageStateFatalErrorAlways},                                     /* 38 */

		 // stato di attesa caricamento del filtro
		 {STATE_PRIMING_PH_1_WAIT,  PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY, &stateParentNull[0], &manageStatePriming_1_WaitEntry},			                      /* 39 */
		 {STATE_PRIMING_PH_1_WAIT,  PARENT_NULL, CHILD_NULL, ACTION_ALWAYS,   &stateParentNull[0], &manageStatePriming_1_WaitAlways},		                          /* 40 */
		 {}
};

