/*
 * App_Ges.c
 *
 *  Created on: 03/nov/2016
 *      Author: W15
 */

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
#include "child_gest.h"

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
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_STOP_PURIFICATION, ACTION_NULL,     &stateNull[0], &manageChildTreatAlm1StopPurifEntry},       /* 5 */
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_STOP_PURIFICATION, ACTION_ALWAYS,   &stateNull[0], &manageChildTreatAlm1StopPurifAlways},      /* 6 */
			//alarm stop all pump
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_STOP_ALL_PUMP,     ACTION_NULL,     &stateNull[0], &manageChildTreatAlm1StopAllPumpEntry},     /* 7 */
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_STOP_ALL_PUMP,     ACTION_ALWAYS,   &stateNull[0], &manageChildTreatAlm1StopAllPumpAlways},    /* 8 */
			//alarm stop peltier
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_STOP_PELTIER,      ACTION_NULL,     &stateNull[0], &manageChildTreatAlm1StopPeltEntry},        /* 9 */
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_STOP_PELTIER,      ACTION_ALWAYS,   &stateNull[0], &manageChildTreatAlm1StopPeltAlways},       /* 10 */
			//alarm stop all actuator
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_STOP_ALL_ACTUATOR, ACTION_NULL,     &stateNull[0], &manageChildTreatAlm1StopAllActEntry},      /* 11 */
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_STOP_ALL_ACTUATOR, ACTION_ALWAYS,   &stateNull[0], &manageChildTreatAlm1StopAllActAlways},     /* 12 */
			//alarm priming end
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_END,               ACTION_NULL,     &stateNull[0], &manageNull},                               /* 13 */
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_END,               ACTION_ALWAYS,   &stateNull[0], &manageNull},                               /* 14 */
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
		 {STATE_NULL, PARENT_NULL, CHILD_NULL, ACTION_NULL, &stateChildIdle[0], &manageNull},
		 {STATE_NULL, PARENT_IDLE, CHILD_IDLE, ACTION_NULL, &stateChildIdle[1], &manageNull},
		 {}
		};

struct machineParent stateParentT1TNoDisposable[] =
		{
		 {STATE_NULL, PARENT_NULL,                    CHILD_NULL, ACTION_NULL,     &stateChildIdle[0], &manageNull},
		 {STATE_NULL, PARENT_T1_NO_DISP_INIT,         CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_NO_DISP_INIT,         CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_NO_DISP_CHECK_PRESS,  CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_NO_DISP_CHECK_PRESS,  CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_NO_DISP_CHECK_TEMP,   CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_NO_DISP_CHECK_TEMP,   CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_NO_DISP_CHECK_FLWMTR, CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_NO_DISP_CHECK_FLWMTR, CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[1], &manageNull},
	     {STATE_NULL, PARENT_T1_NO_DISP_CHEK_AIR,     CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_NO_DISP_CHEK_AIR,     CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_NO_DISP_ALARM,        CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_NO_DISP_ALARM,        CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_NO_DISP_END,          CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_NO_DISP_END,          CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_NO_DISP_FATAL_ERROR,  CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_NO_DISP_FATAL_ERROR,  CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[1], &manageNull},
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

		{}
};


struct machineParent stateParentTreatKidney1[] =
{
		{STATE_NULL, PARENT_NULL,                   CHILD_NULL, ACTION_NULL,     &stateChildIdle[0],        &manageNull},						/* 0 */
		/* priming init */
		{STATE_NULL, PARENT_TREAT_KIDNEY_1_INIT,    CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[0],        &manageParentTreatEntry},			/* 1 */
		{STATE_NULL, PARENT_TREAT_KIDNEY_1_INIT,    CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[0],        &manageParentTreatAlways},			/* 2 */
		/* priming run */
		{STATE_NULL, PARENT_TREAT_KIDNEY_1_PUMP_ON, CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[0],        &manageParentTreatEntry},			/* 3 */
		{STATE_NULL, PARENT_TREAT_KIDNEY_1_PUMP_ON, CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[0],        &manageParentTreatAlways},			/* 4 */
		/* priming alarm */
		{STATE_NULL, PARENT_TREAT_KIDNEY_1_ALARM,   CHILD_IDLE, ACTION_ON_ENTRY, &stateChildAlarmTreat1[1], &manageParentTreatAlarmEntry},		/* 5 */
		{STATE_NULL, PARENT_TREAT_KIDNEY_1_ALARM,   CHILD_IDLE, ACTION_ALWAYS,   &stateChildAlarmTreat1[1], &manageParentTreatAlarmAlways},	    /* 6 */
		/* priming end */
		{STATE_NULL, PARENT_TREAT_KIDNEY_1_END,     CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[0],        &manageParentTreatEntry},			/* 7 */
		{STATE_NULL, PARENT_TREAT_KIDNEY_1_END,     CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[0],        &manageParentTreatAlways},		    /* 8 */

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

		{STATE_IDLE,               PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY,                &stateParentNull[0],                &manageStateIdle},				/* 3 */
		{STATE_IDLE,               PARENT_NULL, CHILD_NULL, ACTION_ALWAYS,                  &stateParentNull[0],                &manageStateIdleAlways},		/* 4 */

		{STATE_SELECT_TREAT,       PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY,                &stateParentNull[0],                &manageStateSelTreat},			/* 5 */
		{STATE_SELECT_TREAT,       PARENT_NULL, CHILD_NULL, ACTION_ALWAYS,                  &stateParentNull[0],                &manageStateSelTreatAlways},	/* 6 */

		{STATE_T1_NO_DISPOSABLE,   PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY,                &stateParentNull[0],                &manageStateT1NoDisp},			/* 7 */
		{STATE_T1_NO_DISPOSABLE,   PARENT_NULL, CHILD_NULL, ACTION_ALWAYS,                  &stateParentNull[0],                &manageStateT1NoDispAlways},	/* 8 */

		{STATE_MOUNTING_DISP,      PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY,                &stateParentNull[0],                &manageStateMountDisp},			/* 9 */
		{STATE_MOUNTING_DISP,      PARENT_NULL, CHILD_NULL, ACTION_ALWAYS,                  &stateParentNull[0],                &manageStateMountDispAlways},	/* 10 */

		{STATE_TANK_FILL,          PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY,                &stateParentNull[0],                &manageStateTankFill},			/* 11 */
		{STATE_TANK_FILL,          PARENT_NULL, CHILD_NULL, ACTION_ALWAYS,                  &stateParentNull[0],                &manageStateTankFillAlways},	/* 12 */

		{STATE_PRIMING_PH_1,       PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY,                &stateParentPrimingTreatKidney1[1], &managePrimingPh1},				/* 13 */
		{STATE_PRIMING_PH_1,       PARENT_NULL, CHILD_NULL, ACTION_ALWAYS,                  &stateParentPrimingTreatKidney1[1], &managePrimingPh1Always},		/* 14 */

		{STATE_PRIMING_PH_2,       PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY,                &stateParentPrimingTreatKidney1[1], &managePrimingPh2},				/* 15 */
		{STATE_PRIMING_PH_2,       PARENT_NULL, CHILD_NULL, ACTION_ALWAYS,                  &stateParentPrimingTreatKidney1[1], &managePrimingPh2Always},		/* 16 */

		{STATE_TREATMENT_KIDNEY_1, PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY,                &stateParentTreatKidney1[1],        &manageStateTreatKidney1},		/* 17 */
		{STATE_TREATMENT_KIDNEY_1, PARENT_NULL, CHILD_NULL, ACTION_ALWAYS,                  &stateParentTreatKidney1[1],        &manageStateTreatKidney1Always},/* 18 */

		{STATE_EMPTY_DISPOSABLE,   PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY,                &stateParentNull[0],                &manageStateEmptyDisp},			/* 19 */
		{STATE_EMPTY_DISPOSABLE,   PARENT_NULL, CHILD_NULL, ACTION_ALWAYS,                  &stateParentNull[0],                &manageStateEmptyDispAlways},	/* 20 */

		/**************************************************************************************/
		/******-----------------------------MACHINE STATE------------------------------********/
		/**************************************************************************************/


		 //{STATE_T1_NO_DISPOSABLE, PARENT_T1_NO_DISP_INIT, CHILD_NULL, ACTION_ON_EXIT, &stateParentT1TNoDisposable[1], &manageNull},

		 //{STATE_T1_WITH_DISPOSABLE, PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY, &stateParentNull[0], &manageStateT1WithDisp},
		 //{STATE_T1_WITH_DISPOSABLE, PARENT_NULL, CHILD_NULL, ACTION_ALWAYS, &stateParentNull[0], &manageStateT1WithDispAlways},
		 //{STATE_T1_WITH_DISPOSABLE, PARENT_T1_WITH_DISP_INIT, CHILD_NULL, ACTION_ON_EXIT, &stateParentIdle[2], &manageNull},

		 //{STATE_PRIMING_TREAT_1, PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY, &stateParentNull[0], &manageStatePrimingTreat1},
		 //{STATE_PRIMING_TREAT_1, PARENT_NULL, CHILD_NULL, ACTION_ALWAYS, &stateParentNull[0], &manageStatePrimingTreat1Always},

		 //{STATE_PRIMING_TREAT_2, PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY, &stateParentNull[0], &manageStatePrimingTreat2},
		 //{STATE_PRIMING_TREAT_2, PARENT_NULL, CHILD_NULL, ACTION_ALWAYS, &stateParentNull[0], &manageStatePrimingTreat2Always},


		 {STATE_TREATMENT_2,        PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY, &stateParentNull[0], &manageStateTreat2},
		 {STATE_TREATMENT_2,        PARENT_NULL, CHILD_NULL, ACTION_ALWAYS,   &stateParentNull[0], &manageStateTreat2},

		 {STATE_EMPTY_DISPOSABLE_1, PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY, &stateParentNull[0], &manageStateEmptyDisp1},
		 {STATE_EMPTY_DISPOSABLE_1, PARENT_NULL, CHILD_NULL, ACTION_ALWAYS,   &stateParentNull[0], &manageStateEmptyDisp1Always},

		 {STATE_EMPTY_DISPOSABLE_2, PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY, &stateParentNull[0], &manageStateEmptyDisp2},
		 {STATE_EMPTY_DISPOSABLE_2, PARENT_NULL, CHILD_NULL, ACTION_ALWAYS,   &stateParentNull[0], &manageStateEmptyDisp2Always},

		 {STATE_WASHING,            PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY, &stateParentNull[0], &manageStateWashing},
		 {STATE_WASHING,            PARENT_NULL, CHILD_NULL, ACTION_ALWAYS,   &stateParentNull[0], &manageStateWashingAlways},

		 {STATE_FATAL_ERROR,        PARENT_NULL, CHILD_NULL, ACTION_NULL,     &stateParentNull[0], &manageStateFatalError},
		 {STATE_FATAL_ERROR,        PARENT_NULL, CHILD_NULL, ACTION_NULL,     &stateParentNull[0], &manageStateFatalErrorAlways},

		 {}
};


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
	releaseGUIButton(BUTTON_CONFIRM);

	#ifdef DEBUG_ENABLE
	//Bit1_NegVal();
	#endif
}

void manageStateIdleAlways(void)
{
	computeMachineStateGuardIdle();

	#ifdef DEBUG_ENABLE
	//Bit2_NegVal();
	#endif
}

/*-----------------------------------------------------------*/
/* This function manages the state select treatment activity */
/*-----------------------------------------------------------*/
void manageStateSelTreat(void)
{
	releaseGUIButton(BUTTON_CONFIRM);

	#ifdef DEBUG_ENABLE

	#endif
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
	releaseGUIButton(BUTTON_CONFIRM);

	#ifdef DEBUG_ENABLE

	#endif
}

void manageStateMountDispAlways(void)
{
	computeMachineStateGuardMountDisp();

	#ifdef DEBUG_ENABLE

	#endif
}

/*--------------------------------------------------------------*/
/* This function manages the state tank fill activity           */
/*--------------------------------------------------------------*/
void manageStateTankFill(void)
{
	releaseGUIButton(BUTTON_CONFIRM);

	#ifdef DEBUG_ENABLE

	#endif
}

void manageStateTankFillAlways(void)
{
	static float myTempValue = 200;

	if(myTempValue != parameterWordSetFromGUI[PAR_SET_TEMPERATURE].value){
		myTempValue = parameterWordSetFromGUI[PAR_SET_TEMPERATURE].value;

		if(myTempValue == 40)
		{
			peltierCell.mySet = myTempValue - 80;
		}
		else if(myTempValue == 360)
		{
			peltierCell.mySet = myTempValue + 60;
		}
		else
		{
			peltierCell.mySet = 200;
		}
	}

	computeMachineStateGuardTankFill();

	#ifdef DEBUG_ENABLE

	#endif
}

/*--------------------------------------------------------------*/
/* This function manages the state priming phase 1 activity     */
/*--------------------------------------------------------------*/
void managePrimingPh1(void)
{
	releaseGUIButton(BUTTON_CONFIRM);

	timerCounter = 0;

	pumpPerist[0].entry = 0;

	#ifdef DEBUG_ENABLE

	#endif
}

void managePrimingPh1Always(void)
{
	static char iflag_perf = 0;
	static char iflag_oxyg = 0;

	//guard macchina a stati
	computeMachineStateGuardPrimingPh1();

	//gestione attuatori modbus (pump e pinch)
	//pump 1: speed 300 ml/min, open loop
	//pump 2: speed 300 ml/min, open loop
	//pinch 1: bypass su filtro
	//pinch 2: bypass su organo

	//gestione assieme Peltier
	//RMAIN: ENABLE, TCDB: 2, TCLIMIT: 100, SET: -6°C
	//RMODE: ON/OFF, DB:0.5, HYST: 0.5
	//MDFAN: 1
	//CALT1: NTC
	//CUR_H: 30
	//RUN
	//STOP

	//

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

	#ifdef DEBUG_ENABLE

	#endif
}

/*--------------------------------------------------------------*/
/* This function manages the state priming phase 2 activity     */
/*--------------------------------------------------------------*/
void managePrimingPh2(void)
{
	releaseGUIButton(BUTTON_CONFIRM);

	pumpPerist[0].entry = 0;

	#ifdef DEBUG_ENABLE

	#endif
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

	#ifdef DEBUG_ENABLE

	#endif
}

/*-----------------------------------------------------------*/
/* This function manages the state treatment 1 activity      */
/*-----------------------------------------------------------*/
void manageStateTreatKidney1(void)
{
	releaseGUIButton(BUTTON_CONFIRM);

	pumpPerist[0].entry = 0;
}

void manageStateTreatKidney1Always(void)
{
	computeMachineStateGuardTreatment();

	#ifdef DEBUG_ENABLE

	#endif
}

/*-----------------------------------------------------------*/
/* This function manages the state empty disposable activity */
/* Entrata nello stato di svutamento del disposable          */
/*-----------------------------------------------------------*/
void manageStateEmptyDisp(void){
	releaseGUIButton(BUTTON_CONFIRM);
}

void manageStateEmptyDispAlways(void){
	#ifdef DEBUG_ENABLE

	#endif
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

	iflag_pmp1_rx = IFLAG_IDLE;

	if(pumpPerist[0].entry == 0)
	{
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
		setPinchPositionHighLevel(PNCHVLV1_ADDRESS, MODBUS_PINCH_RIGHT_OPEN);
		setPinchPositionHighLevel(PNCHVLV3_ADDRESS, MODBUS_PINCH_RIGHT_OPEN);

		startPeltierActuator();
		peltierCell.readAlwaysEnable = 1;

 		pumpPerist[0].entry = 1;
	}
}

void manageParentPrimingAlways(void){

	static char iflag_perf = 0;
	static char iflag_oxyg = 0;
	int speed = 0;
	static int timerCopy = 0;
	static float volumePriming = 0;

	//manage pump
	switch(ptrCurrentParent->parent){
	case PARENT_PRIMING_TREAT_KIDNEY_1_INIT:
	if(buttonGUITreatment[BUTTON_START_PRIMING].state == GUI_BUTTON_RELEASED)
	{
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 2000);
		//setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 3000);
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 10*parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FOW].value);
		setPinchPositionHighLevel(PNCHVLV1_ADDRESS, MODBUS_PINCH_RIGHT_OPEN);
		setPinchPositionHighLevel(PNCHVLV3_ADDRESS, MODBUS_PINCH_RIGHT_OPEN);

		releaseGUIButton(BUTTON_START_PRIMING);
	}
	else if(buttonGUITreatment[BUTTON_START_PERF_PUMP].state == GUI_BUTTON_RELEASED)
	{
		releaseGUIButton(BUTTON_START_PERF_PUMP);

		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 2000);
		setPinchPositionHighLevel(PNCHVLV1_ADDRESS, MODBUS_PINCH_RIGHT_OPEN);
		setPinchPositionHighLevel(PNCHVLV3_ADDRESS, MODBUS_PINCH_RIGHT_OPEN);
	}
	else if(buttonGUITreatment[BUTTON_START_OXYGEN_PUMP].state == GUI_BUTTON_RELEASED)
	{
		releaseGUIButton(BUTTON_START_OXYGEN_PUMP);

		//setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 3000);
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 10*parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FOW].value);
	}
	else if(buttonGUITreatment[BUTTON_STOP_ALL_PUMP].state == GUI_BUTTON_RELEASED)
	{
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
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

	if((timerCounterModBus%9) == 8)
	{
		//timerCopy = timerCounter;
		if(timerCounterModBus != 0)
			timerCopy = timerCounterModBus;
		timerCounter = 0;
		//timerCounterModBus = 0;

		//readPumpSpeedValue(pumpPerist[0].pmpMySlaveAddress);
		readPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress);
	}

	//if(iflag_pmp1_rx == IFLAG_PMP1_RX)
	if(pumpPerist[0].dataReady == DATA_READY_TRUE)
	{
		//iflag_pmp1_rx = IFLAG_IDLE;
		//speed = ((BYTES_TO_WORD_SIGN(msgToRecvFrame3[3], msgToRecvFrame3[4]))/100)*(timerCopy);
		// la velocita' ora posso leggerla direttamente dall'array di registry modbus
		speed = modbusData[pumpPerist[0].pmpMySlaveAddress-2][17];
		volumePriming = volumePriming + (float)(speed * 0.00775);
		perfusionParam.priVolPerfArt = (int)(volumePriming);
		pumpPerist[0].dataReady = DATA_READY_FALSE;
	}
	break;

	case PARENT_PRIMING_TREAT_KIDNEY_1_RUN:
			if(buttonGUITreatment[BUTTON_START_PRIMING].state == GUI_BUTTON_RELEASED)
			{
				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 2000);
				//setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 3000);
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 10*parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FOW].value);
				setPinchPositionHighLevel(PNCHVLV1_ADDRESS, MODBUS_PINCH_RIGHT_OPEN);
				setPinchPositionHighLevel(PNCHVLV3_ADDRESS, MODBUS_PINCH_RIGHT_OPEN);

				releaseGUIButton(BUTTON_START_PRIMING);
			}
			else if(buttonGUITreatment[BUTTON_START_PERF_PUMP].state == GUI_BUTTON_RELEASED)
			{
				releaseGUIButton(BUTTON_START_PERF_PUMP);

				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 2000);
				setPinchPositionHighLevel(PNCHVLV1_ADDRESS, MODBUS_PINCH_RIGHT_OPEN);
				setPinchPositionHighLevel(PNCHVLV3_ADDRESS, MODBUS_PINCH_RIGHT_OPEN);
			}
			else if(buttonGUITreatment[BUTTON_START_OXYGEN_PUMP].state == GUI_BUTTON_RELEASED)
			{
				releaseGUIButton(BUTTON_START_OXYGEN_PUMP);

				//setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 3000);
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 10*parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FOW].value);
			}
			else if(buttonGUITreatment[BUTTON_STOP_ALL_PUMP].state == GUI_BUTTON_RELEASED)
			{
				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
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
				//timerCopy = timerCounter;
				if(timerCounterModBus != 0)
					timerCopy = timerCounterModBus;
				timerCounter = 0;
				//timerCounterModBus = 0;

				//readPumpSpeedValue(pumpPerist[0].pmpMySlaveAddress);
				readPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress);
			}

		if(pumpPerist[0].dataReady == DATA_READY_TRUE)
			{
				//iflag_pmp1_rx = IFLAG_IDLE;
				//speed = ((BYTES_TO_WORD_SIGN(msgToRecvFrame3[3], msgToRecvFrame3[4]))/100)*(timerCopy);
				// la velocita' ora posso leggerla direttamente dall'array di registry modbus
				speed = modbusData[pumpPerist[0].pmpMySlaveAddress-2][17];
				volumePriming = volumePriming + (float)(speed * 0.00775);
				perfusionParam.priVolPerfArt = (int)(volumePriming);
				pumpPerist[0].dataReady = DATA_READY_FALSE;
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
	setPinchPositionHighLevel(PNCHVLV1_ADDRESS, MODBUS_PINCH_POS_CLOSED);
	setPinchPositionHighLevel(PNCHVLV2_ADDRESS, MODBUS_PINCH_POS_CLOSED);
	setPinchPositionHighLevel(PNCHVLV3_ADDRESS, MODBUS_PINCH_POS_CLOSED);
	//oneShot = 1;

	stopPeltierActuator();
	}

	pumpPerist[0].entry = 0;
	*/
}

void manageParentTreatAlarmEntry(void){

	/*
	 * (FM) per ora commento tutto questo perche' per la gestione dell'allarme puo' essere fatta in diversi modi ed e'
	 * meglio farla dentro lo switch del child
	if(pumpPerist[0].dataReady == DATA_READY_FALSE)
	{
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
		setPinchPositionHighLevel(PNCHVLV1_ADDRESS, MODBUS_PINCH_LEFT_OPEN);
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
		setPinchPositionHighLevel(PNCHVLV1_ADDRESS, MODBUS_PINCH_POS_CLOSED); // forse vanno messe in scarico e non chiuse !!!!
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
		speed = modbusData[pumpPerist[0].pmpMySlaveAddress-2][17];
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
		setPinchPositionHighLevel(PNCHVLV1_ADDRESS, MODBUS_PINCH_LEFT_OPEN);
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
		speed = modbusData[pumpPerist[0].pmpMySlaveAddress-2][17];
		pumpPerist[0].dataReady = DATA_READY_FALSE;
	}
*/
}

void manageParentTreatEntry(void){
	//iflag_pmp1_rx = IFLAG_IDLE;
	static unsigned char entry = 0;

	//if((iflag_pmp1_rx == IFLAG_PMP1_RX) || (timerCounter >= 10))
	//if(entry == 0)
	if(pumpPerist[0].entry == 0)
	{
		timerCounter = 0;
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
		setPinchPositionHighLevel(PNCHVLV1_ADDRESS, MODBUS_PINCH_RIGHT_OPEN);
		setPinchPositionHighLevel(PNCHVLV3_ADDRESS, MODBUS_PINCH_RIGHT_OPEN);
		setPumpPressLoop(0, PRESS_LOOP_OFF);
		entry = 1;

		startPeltierActuator();
		peltierCell.readAlwaysEnable = 1;

		pumpPerist[0].entry = 1;
	}


}


void manageParentTreatAlways(void){
		static char iflag_perf = 0;
		static char iflag_oxyg = 0;
		int speed = 0;
		static int timerCopy = 0;
		static float volumeTreatArt = 0;

		//manage pump
		switch(ptrCurrentParent->parent){
		case PARENT_TREAT_KIDNEY_1_INIT:
		if(buttonGUITreatment[BUTTON_STOP_ALL_PUMP].state == GUI_BUTTON_RELEASED)
		{
			setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
			setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
			releaseGUIButton(BUTTON_STOP_ALL_PUMP);

			setPumpPressLoop(0, PRESS_LOOP_OFF);
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
		}
		else if(buttonGUITreatment[BUTTON_START_PRIMING].state == GUI_BUTTON_RELEASED)
		{
			setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 2000); //pump 0: start value = 20 rpm than pressure loop
			//setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 3000); //pump 1: start value = 30 rpm than open loop
			setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 10*parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FOW].value);
			releaseGUIButton(BUTTON_START_PRIMING);

			//if(iflag_perf == 0)
			setPumpPressLoop(0, PRESS_LOOP_ON);
		}
		else if(buttonGUITreatment[BUTTON_START_PERF_PUMP].state == GUI_BUTTON_RELEASED){
			releaseGUIButton(BUTTON_START_PERF_PUMP);

			setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 2000); //pump 0: start value = 20 rpm than pressure loop

			setPumpPressLoop(0, PRESS_LOOP_ON);
		}
		else if(buttonGUITreatment[BUTTON_START_OXYGEN_PUMP].state == GUI_BUTTON_RELEASED){
			releaseGUIButton(BUTTON_START_OXYGEN_PUMP);

			//setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 3000); //pump 1: start value = 30 rpm than open loop
			setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 10*parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FOW].value);
		}

		if(
			(getPumpPressLoop(0) == PRESS_LOOP_ON) &&
			(timerCounterPID >=1)
			)
		{
			timerCounterPID = 0;
			alwaysPumpPressLoop(0);
		}
		else
		{
			//setPumpSpeedValue(pumpPerist[0].pmpMySlaveAddress, 10);
		}


		if((timerCounterModBus%9) == 8)
		{
			//timerCopy = timerCounter;
			if(timerCounterModBus != 0)
				timerCopy = timerCounterModBus;
			timerCounter = 0;

			readPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress);
		}

		if(pumpPerist[0].dataReady == DATA_READY_TRUE)
		{
			//iflag_pmp1_rx = IFLAG_IDLE;
			//speed = ((BYTES_TO_WORD_SIGN(msgToRecvFrame3[3], msgToRecvFrame3[4]))/100)*(timerCopy);
			// la velocita' ora posso leggerla direttamente dall'array di registry modbus
			speed = modbusData[pumpPerist[0].pmpMySlaveAddress-2][17];
			pumpPerist[0].actualSpeed = speed;
			volumeTreatArt = volumeTreatArt + (float)(speed * 0.00775);
			perfusionParam.treatVolPerfArt = (int)(volumeTreatArt);
			pumpPerist[0].dataReady = DATA_READY_FALSE;
		}
		break;

		case PARENT_TREAT_KIDNEY_1_PUMP_ON:
			if(buttonGUITreatment[BUTTON_STOP_ALL_PUMP].state == GUI_BUTTON_RELEASED)
			{
				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
				releaseGUIButton(BUTTON_STOP_ALL_PUMP);

				setPumpPressLoop(0, PRESS_LOOP_OFF);
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
			}
			else if(buttonGUITreatment[BUTTON_START_PRIMING].state == GUI_BUTTON_RELEASED)
			{
				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 2000); //pump 0: start value = 20 rpm than pressure loop
				//setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 3000); //pump 1: start value = 30 rpm than open loop
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 10*parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FOW].value);
				releaseGUIButton(BUTTON_START_PRIMING);

				//if(iflag_perf == 0)
				setPumpPressLoop(0, PRESS_LOOP_ON);
			}
			else if(buttonGUITreatment[BUTTON_START_PERF_PUMP].state == GUI_BUTTON_RELEASED){
				releaseGUIButton(BUTTON_START_PERF_PUMP);

				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 2000); //pump 0: start value = 20 rpm than pressure loop

				setPumpPressLoop(0, PRESS_LOOP_ON);
			}
			else if(buttonGUITreatment[BUTTON_START_OXYGEN_PUMP].state == GUI_BUTTON_RELEASED){
				releaseGUIButton(BUTTON_START_OXYGEN_PUMP);

				//setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 3000); //pump 1: start value = 30 rpm than open loop
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 10*parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FOW].value);
			}

			if(
				(getPumpPressLoop(0) == PRESS_LOOP_ON) &&
				(timerCounterPID >=1)
				)
			{
				timerCounterPID = 0;
				alwaysPumpPressLoop(0);
			}
			else
			{
				//setPumpSpeedValue(pumpPerist[0].pmpMySlaveAddress, 10);
			}


			if((timerCounterModBus%9) == 8)
			{
			//timerCopy = timerCounter;
				if(timerCounterModBus != 0)
					timerCopy = timerCounterModBus;
				timerCounter = 0;

				readPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress);
			}

			if(pumpPerist[0].dataReady == DATA_READY_TRUE)
			{
				//iflag_pmp1_rx = IFLAG_IDLE;
				//speed = ((BYTES_TO_WORD_SIGN(msgToRecvFrame3[3], msgToRecvFrame3[4]))/100)*(timerCopy);
				// la velocita' ora posso leggerla direttamente dall'array di registry modbus
				speed = modbusData[pumpPerist[0].pmpMySlaveAddress-2][17];
				pumpPerist[0].actualSpeed = speed;
				volumeTreatArt = volumeTreatArt + (float)(speed * 0.00775);
				perfusionParam.treatVolPerfArt = (int)(volumeTreatArt);
				pumpPerist[0].dataReady = DATA_READY_FALSE;
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
}

unsigned char getPumpPressLoop(unsigned char pmpId){
	return pumpPerist[pmpId].pmpPressLoop;
}

void alwaysPumpPressLoop(unsigned char pmpId){
	int deltaSpeed = 0;
	static int actualSpeed = 0;
	static int actualSpeedOld = 0;
	float parKITC = 0.2;
	float parKP = 1;
	float parKD_TC = 0.8;
	float pressSample0 = 0;
	static float pressSample1 = 0;
	static float pressSample2 = 0;
	float errPress = 0;


	pressSample0 = sensor_PRx[0].prSensValueFilteredWA;
	errPress = parameterWordSetFromGUI[PAR_SET_PRESS_ART_TARGET].value - pressSample0;

	deltaSpeed = (int)((parKITC*errPress) - (parKP*(pressSample0 - pressSample1)) - (parKD_TC*(pressSample0 - 2*pressSample1 + pressSample2)));

	// valutare se mettere il deltaSpeed = 0 nel caso deltaSpeed sia negativo in modo da non far andare actualSpeed a zero troppo in fretta
	// in alternativa il deltaSpeed va considerato solo se è abbastanza negativo
	if((deltaSpeed < -2) || (deltaSpeed > 2))
		actualSpeed = actualSpeed + deltaSpeed;

	if(actualSpeed >= 50)
		actualSpeed = 50;
	else if(actualSpeed <= 0)
		actualSpeed = 0;

	if(actualSpeed != pumpPerist[0].actualSpeedOld){
		//setPumpSpeedValue(pumpPerist[pmpId].pmpMySlaveAddress, (actualSpeed*100));
		setPumpSpeedValueHighLevel(pumpPerist[pmpId].pmpMySlaveAddress, (actualSpeed*100));
		pumpPerist[0].actualSpeedOld = actualSpeed;
	}

	//se provengo da un allarme la pompa è ferma ed il controllo deve ripartire
	/*if(pumpPerist[0].actualSpeed == 0){
		actualSpeedOld = 0;
	}*/

	pressSample2 = pressSample1;
	pressSample1 = pressSample0;


}

void manageParentEntry(void)
{
	#ifdef DEBUG_ENABLE
	Bit1_NegVal();
	#endif

	// (FM) qui posso far partire il T1 test e l'inizializzazione della comm
	// TODO
}

void manageParentEntryAlways(void)
{
	#ifdef DEBUG_ENABLE
	static int index3 = 0;
	index3++;

	if(index3%14000 == 0)
		Bit1_NegVal();
	#endif

	// TODO
	// (FM) qui posso gestire il T1 test e l'inizializzazione della comm ed alla fine
	// devo eseguire le due istruzioni
	currentGuard[GUARD_HW_T1T_DONE].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
	currentGuard[GUARD_COMM_ENABLED].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
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
	#ifdef DEBUG_ENABLE
	Bit1_NegVal();
	#endif
}

void manageChildEntryAlways(void)
{
	#ifdef DEBUG_ENABLE
	static int index2 = 0;
	index2++;

	if(index2%12000 == 0)
		Bit2_NegVal();
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
	if(
		(buttonGUITreatment[BUTTON_KIDNEY].state == GUI_BUTTON_RELEASED) &&
		(buttonGUITreatment[BUTTON_CONFIRM].state == GUI_BUTTON_RELEASED)
		)
	{
		currentGuard[GUARD_ENABLE_SELECT_TREAT_PAGE].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
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
	}
}

/*--------------------------------------------------------------------*/
/*  This function compute the guard value in mounting disposable state   */
/*--------------------------------------------------------------------*/
static void computeMachineStateGuardMountDisp(void){
	if(
		(buttonGUITreatment[BUTTON_PERF_DISP_MOUNTED].state == GUI_BUTTON_RELEASED) &&
		(buttonGUITreatment[BUTTON_OXYG_DISP_MOUNTED].state == GUI_BUTTON_RELEASED) &&
		(buttonGUITreatment[BUTTON_CONFIRM].state == GUI_BUTTON_RELEASED)
			)
	{
		currentGuard[GUARD_ENABLE_TANK_FILL].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
	}
}

/*--------------------------------------------------------------------*/
/*  This function compute the guard value in tank fill state   */
/*--------------------------------------------------------------------*/
static void computeMachineStateGuardTankFill(void){
	if(
		(buttonGUITreatment[BUTTON_PERF_TANK_FILL].state == GUI_BUTTON_RELEASED) &&
		(buttonGUITreatment[BUTTON_CONFIRM].state == GUI_BUTTON_RELEASED)
			)
	{
		currentGuard[GUARD_ENABLE_PRIMING_PHASE_1].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		releaseGUIButton(BUTTON_CONFIRM);
	}
}

/*--------------------------------------------------------------------*/
/*  This function compute the guard value in priming phase 1 state   */
/*--------------------------------------------------------------------*/
static void computeMachineStateGuardPrimingPh1(void){
	if(
		(buttonGUITreatment[BUTTON_PERF_FILTER_MOUNT].state == GUI_BUTTON_RELEASED) &&
		(buttonGUITreatment[BUTTON_CONFIRM].state == GUI_BUTTON_RELEASED)
			)
	{
		currentGuard[GUARD_ENABLE_PRIMING_PHASE_2].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		releaseGUIButton(BUTTON_CONFIRM);
		//setPumpSpeedValue(pumpPerist[0].pmpMySlaveAddress, 0);
	}

}

/*--------------------------------------------------------------------*/
/*  This function compute the guard value in priming phase 2 state   */
/*  Controllo quando iniziare il trattamento
/*--------------------------------------------------------------------*/
static void computeMachineStateGuardPrimingPh2(void){
	if(
		(perfusionParam.priVolPerfArt >= parameterWordSetFromGUI[PAR_SET_PRIMING_VOL_PERFUSION].value) &&
		(buttonGUITreatment[BUTTON_CONFIRM].state == GUI_BUTTON_RELEASED) /*&&
		(iflag_pmp1_rx == IFLAG_PMP1_RX)*/
		)
	{
		currentGuard[GUARD_ENABLE_TREATMENT_KIDNEY_1].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		releaseGUIButton(BUTTON_CONFIRM);
		//setPumpSpeedValue(pumpPerist[0].pmpMySlaveAddress, 0);
	}
}

/*--------------------------------------------------------------------*/
/*  This function compute the guard value in treatment kidney 1 state   */
/*  Controllo la fine del trattamento
/*--------------------------------------------------------------------*/
static void computeMachineStateGuardTreatment(void){
	if(
			(buttonGUITreatment[BUTTON_STOP_ALL_PUMP].state == GUI_BUTTON_RELEASED) &&
			(buttonGUITreatment[BUTTON_CONFIRM].state == GUI_BUTTON_RELEASED)
			)
		{
			// passo allo svuotamento del circuito
			currentGuard[GUARD_ENABLE_DISPOSABLE_EMPTY].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		}
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
				currentGuard[i].guardValue = GUARD_VALUE_TRUE;
			else if(currentGuard[i].guardEntryValue == GUARD_ENTRY_VALUE_FALSE){
				currentGuard[i].guardValue = GUARD_VALUE_FALSE;
			}
		}
}


// Adr 7..9
void TestPinch(void)
{
	static unsigned char PinchPos = 0;
	static unsigned char Adr = 7;   // BOTTOM_PINCH_ID = 7, LEFT_PINCH_ID = 8, RIGHT_PINCH_ID = 9
	static int Counter = 0;
	static unsigned char state = 0;
	if (Bubble_Keyboard_GetVal(BUTTON_1) && PinchPos != MODBUS_PINCH_POS_CLOSED)
	{
		PinchPos = MODBUS_PINCH_POS_CLOSED;
		//setPinchPosValue (Adr,MODBUS_PINCH_POS_CLOSED);
		setPinchPositionHighLevel(Adr, (int)MODBUS_PINCH_POS_CLOSED);
		Counter = 0;
		state = 0;
	}
	else if (Bubble_Keyboard_GetVal(BUTTON_2) && PinchPos != MODBUS_PINCH_RIGHT_OPEN)
	{
		PinchPos = MODBUS_PINCH_RIGHT_OPEN;
		//setPinchPosValue (Adr,MODBUS_PINCH_RIGHT_OPEN);
		setPinchPositionHighLevel(Adr, (int)MODBUS_PINCH_RIGHT_OPEN);
		Counter = 0;
		state = 0;
	}
	else if (Bubble_Keyboard_GetVal(BUTTON_3) && PinchPos != MODBUS_PINCH_LEFT_OPEN)
	{
		PinchPos = MODBUS_PINCH_LEFT_OPEN;
		//setPinchPosValue (Adr,MODBUS_PINCH_LEFT_OPEN);
		setPinchPositionHighLevel(Adr, (int)MODBUS_PINCH_LEFT_OPEN);
		Counter = 0;
		state = 0;
	}
	else if (Bubble_Keyboard_GetVal(BUTTON_4) && (state == 0))
	{
		Counter++;
		if( Counter > 100)
		{
			Adr++;
			if(Adr == 10)
				Adr = 7;
			Counter = 0;
			state = 1;
		}
	}
	else if (!Bubble_Keyboard_GetVal(BUTTON_4) && (state == 1))
	{
		Counter++;
		if( Counter > 100)
		{
			state = 0;
			Counter = 0;
		}
	}
	else
	{
		Counter = 0;
	}

}




unsigned int PumpAverageCurrent;
unsigned int PumpSpeedVal;
unsigned int PumpStatusVal;

// Adr 2..5
void TestPump(unsigned char Adr)
{
	static bool MotorOn = 0;

	if (Bubble_Keyboard_GetVal(BUTTON_1) && !MotorOn)
	{
	  /*accendo il motore*/
	  MotorOn = TRUE;
	  EN_Motor_Control(ENABLE);
	  setPumpSpeedValueHighLevel(Adr,2000);
	}
	else if (Bubble_Keyboard_GetVal(BUTTON_2) && MotorOn)
	{
	  /*spengo il motore*/
	  MotorOn = FALSE;
	  EN_Motor_Control(DISABLE);
	  setPumpSpeedValueHighLevel(Adr,0);
	}
	else
	{
		PumpAverageCurrent = modbusData[Adr-2][16];
		PumpSpeedVal = modbusData[Adr-2][17];
		PumpStatusVal = modbusData[Adr-2][18];
		//readPumpSpeedValue(pumpPerist[Adr - 2].pmpMySlaveAddress);
		//readPumpSpeedValue(Adr - 2);
	}
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

			if(index%32000 == 0)
				Bit1_NegVal();
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
			    /* (FM) HO RICEVUTO UN COMANDO (DA TASTIERA O SERIALE) CHE MI CHIEDE DI ENTRARE NELLO STATO DI
			       SELEZIONE DEL TRATTAMENTO */
				/* compute future state */
				ptrFutureState = &stateState[5];
				/* compute future parent */
				ptrFutureParent = ptrFutureState->ptrParent;
				/* compute future child */
				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
			}
			/* execute function state level */
			// (FM) DOPO LA PRIMA VOLTA PASSA AUTOMATICAMENTE NELLO STATO IDLE,ACTION_ALWAYS
			manageStateEntryAndStateAlways(4);

//			#ifdef DEBUG_ENABLE
//			Bit1_NegVal();
//			TestPump(3, 1000 );
//			#endif

			break;

		case STATE_SELECT_TREAT:
			/* compute future state */
			if( (currentGuard[GUARD_ENABLE_MOUNT_DISPOSABLE].guardValue == GUARD_VALUE_TRUE) )
			{
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
				(currentGuard[GUARD_ENABLE_TANK_FILL].guardValue == GUARD_VALUE_TRUE)
				)
			{
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

		case STATE_TANK_FILL:
			if(
				(currentGuard[GUARD_ENABLE_PRIMING_PHASE_1].guardValue == GUARD_VALUE_TRUE)
				)
			{
				/* (FM) FINITA LA FASE DI RIEMPIMENTO POSSO PASSARE ALLA FASE 1 DEL PRIMING */
				/* compute future state */
				ptrFutureState = &stateState[13];
				/* compute future parent */
				ptrFutureParent = ptrFutureState->ptrParent;
				/* compute future child */
				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
			}
			/* execute function state level */
			manageStateEntryAndStateAlways(12);
			break;

		case STATE_PRIMING_PH_1:
			if(
				(currentGuard[GUARD_ENABLE_PRIMING_PHASE_2].guardValue == GUARD_VALUE_TRUE)
				)
			{
				/* (FM) FINITA LA FASE 1 DEL PRIMING POSSO PASSARE ALLA FASE 2 */
				/* compute future state */
				ptrFutureState = &stateState[15];
				/* compute future parent */
				ptrFutureParent = ptrFutureState->ptrParent;
				/* compute future child */
				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
			}
			/* execute function state level */
			manageStateEntryAndStateAlways(14);
			break;

		case STATE_PRIMING_PH_2:
			if(
				(currentGuard[GUARD_ENABLE_TREATMENT_KIDNEY_1].guardValue == GUARD_VALUE_TRUE)
				)
			{
				/* (FM) FINITA LA FASE 2 DEL PRIMING POSSO PASSARE AL TRATTAMENTO ..KIDNEY_1 */
				/* compute future state */
				ptrFutureState = &stateState[17];
				/* compute future parent */
				ptrFutureParent = ptrFutureState->ptrParent;
				/* compute future child */
				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
			}
			/* execute function state level */
			manageStateEntryAndStateAlways(16);
			break;

		case STATE_TREATMENT_KIDNEY_1:
            /* (FM) SONO IN TRATTAMENTO A QUESTO PUNTO FUNZIONANO GLI ALLARMI E SONO IN ATTESA DELL'ATTIVAZIONE DI
               currentGuard[GUARD_ENABLE_DISPOSABLE_EMPTY].guardEntryValue (AVVERRA' QUANDO L'UTENTE PREMERA' LO STOP ALLE POMPE
               O SEMPLICEMENTE IL TASTO ENTER (VEDI FUNZIONE manageStateTreatKidney1Always).
               QUANDO ARRIVERA', POTRO' TORNARE NELLO STATE_ENTRY INIZIALE (DA AGGIUNGERE).
               TODO */

			/* execute function state level */
			manageStateEntryAndStateAlways(18);
			break;

		/*case STATE_T1_WITH_DISPOSABLE:
			break;

		case STATE_PRIMING_TREAT_1:
			break;

		case STATE_PRIMING_TREAT_2:
			break;*/


		case STATE_TREATMENT_2:
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
			}

			if(ptrCurrentParent->action == ACTION_ON_ENTRY)
			{
				/* execute parent callback function */
				ptrCurrentParent->callBackFunct();
				/* compute future parent */
				ptrFutureParent = &stateParentPrimingTreatKidney1[2];
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
			}

			break;

		case PARENT_PRIMING_TREAT_KIDNEY_1_RUN:
			if(ptrCurrentParent->action == ACTION_ON_ENTRY)
			{
				/* execute parent callback function */
				ptrCurrentParent->callBackFunct();
				/* compute future parent */
				ptrFutureParent = &stateParentPrimingTreatKidney1[4];
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
			}
			break;

		case PARENT_PRIMING_TREAT_KIDNEY_1_ALARM:
			if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_FALSE)
			{
				/* (FM) finita la situazione di allarme posso ritornare in PARENT_PRIMING_TREAT_KIDNEY_1_INIT*/
				ptrFutureParent = &stateParentPrimingTreatKidney1[1];
			}

			if(ptrCurrentParent->action == ACTION_ON_ENTRY)
			{
				/* execute parent callback function */
				ptrCurrentParent->callBackFunct();
				/* compute future parent */
				ptrFutureParent = &stateParentPrimingTreatKidney1[6];
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
			}

			if(ptrCurrentParent->action == ACTION_ON_ENTRY)
			{
				/* execute parent callback function */
				ptrCurrentParent->callBackFunct();
				/* compute future parent */
				/* FM entro nello stato in cui l'azione e' di tipo ACTION_ALWAYS */
				ptrFutureParent = &stateParentTreatKidney1[2];
			}
			else if(ptrCurrentParent->action == ACTION_ALWAYS)
			{
				ptrCurrentParent->callBackFunct();
			}

			if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_TRUE)
			{
				/* (FM) si e' verificato un allarme, passo alla sua gestione */
				ptrFutureParent = &stateParentTreatKidney1[5];
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
			}
			else if(ptrCurrentParent->action == ACTION_ALWAYS)
			{
				ptrCurrentParent->callBackFunct();
			}

			if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_TRUE)
			{
				/* (FM) si e' verificato un allarme, passo alla sua gestione */
				ptrFutureParent = &stateParentTreatKidney1[5];
			}
			break;

		case PARENT_TREAT_KIDNEY_1_ALARM:
			if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_FALSE)
			{
				/* FM allarme finito posso ritornare nella fase iniziale del trattamento */
				ptrFutureParent = &stateParentTreatKidney1[1];
			}

			if(ptrCurrentParent->action == ACTION_ON_ENTRY)
			{
				/* execute parent callback function */
				ptrCurrentParent->callBackFunct();
				/* compute future parent */
				/* (FM) passo alla gestione ACTION_ALWAYS dell'allarme */
				ptrFutureParent = &stateParentTreatKidney1[6];
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

/**/
void initGUIButton(void){

	buttonGUITreatment[BUTTON_KIDNEY].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_LIVER].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_CONFIRM].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_RESET].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_BACK].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_START_PRIMING].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_STOP_ALL_PUMP].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_EN_PERFUSION].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_EN_OXYGENATION].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_EN_PURIFICATION].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_START_PERF_PUMP].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_STOP_PERF_PUMP].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_START_OXYGEN_PUMP].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_STOP_OXYGEN_PUMP].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_START_PURIF_PUMP].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_STOP_PURIF_PUMP].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_PERF_DISP_MOUNTED].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_OXYG_DISP_MOUNTED].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_PERF_TANK_FILL].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_PERF_FILTER_MOUNT].state = GUI_BUTTON_NULL;

}

void setGUIButton(unsigned char buttonId){
	buttonGUITreatment[buttonId].state = GUI_BUTTON_PRESSED;
	actionFlag = 2;
}

unsigned char getGUIButton(unsigned char buttonId)
{
	return buttonGUITreatment[buttonId].state;
}

void releaseGUIButton(unsigned char buttonId){
	// FM visto che ora si lavora sugli eventi release, l'assenza di evento tasto diventa GUI_BUTTON_PRESSED !!!
	//buttonGUITreatment[buttonId].state = GUI_BUTTON_RELEASED;
	buttonGUITreatment[buttonId].state = GUI_BUTTON_PRESSED;
}
/**/

/**/
void initSetParamFromGUI(void){
	parameterWordSetFromGUI[PAR_SET_PRIMING_VOL_PERFUSION].value = 0;
	parameterWordSetFromGUI[PAR_SET_PRIMING_VOL_PURIFICATION].value = 0;
	parameterWordSetFromGUI[PAR_SET_PRIMING_VOL_OXYGENATION].value = 0;
	parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FOW].value = 0;
	parameterWordSetFromGUI[PAR_SET_TEMPERATURE].value = 0;
	parameterWordSetFromGUI[PAR_SET_PRESS_ART_TARGET].value = 0;
	parameterWordSetFromGUI[PAR_SET_PRESS_VEN_TARGET].value = 0;
	parameterWordSetFromGUI[PAR_SET_PURIF_FLOW_TARGET].value = 0;
	parameterWordSetFromGUI[PAR_SET_PURIF_UF_FLOW_TARGET].value = 0;
}

void setParamWordFromGUI(unsigned char parId, int value){
	parameterWordSetFromGUI[parId].value = value;
}

void resetParamWordFromGUI(unsigned char parId){
	parameterWordSetFromGUI[parId].value = 0;
}

void Manage_Debug_led(bool Status)
{
 	unsigned char Freq = FREQ_DEBUG_LED;
 	static bool Status_Led_Board = FALSE;

 	if (Status)
 		Freq = Freq * 2;

	if (timerCounterLedBoard >= Freq)
 	{
 		timerCounterLedBoard = 0;
 		if (Status_Led_Board)
 		{
 			Status_Led_Board = FALSE;
 			D_7S_DP_SetVal(); //spegne puntino led
 		}
 		else
 		{
 			Status_Led_Board = TRUE;
 			D_7S_DP_ClrVal(); //accende puntino led
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

unsigned char Bubble_Keyboard_GetVal(unsigned char Button)
{
	unsigned char button_Value = 0;

		switch (Button)
		{
			case BUTTON_1:
				 button_Value = BUBBLE_KEYBOARD_BUTTON1_GetVal();
				 break;

			case BUTTON_2:
				 button_Value = BUBBLE_KEYBOARD_BUTTON2_GetVal();
				 break;

			case BUTTON_3:
				 button_Value = BUBBLE_KEYBOARD_BUTTON3_GetVal();
				 break;

			case BUTTON_4:
				 button_Value = BUBBLE_KEYBOARD_BUTTON4_GetVal();
				 break;

			default:
				break;
		}
	return (button_Value);
}

void Buzzer_Management()
{
//	BUZZER_LOW_C_SetVal(); 		//attiva il buzzer low
//	BUZZER_LOW_C_ClrVal(); 		//disattiva il buzzer low

//	BUZZER_MEDIUM_C_SetVal();	//attiva il buzzer Medium
//	BUZZER_MEDIUM_C_ClrVal();	//disattiva il buzzer Medium

//	BUZZER_HIGH_C_SetVal();		//attiva il buzzer High
//	BUZZER_HIGH_C_ClrVal();		//disattiva il buzzer Medium
}

void Heater_ON()
{
	unsigned char HeaterStatus =  HEAT_ON_C_GetVal();

	if (!HeaterStatus)
		HEAT_ON_C_SetVal();
}

void Set_Lamp(unsigned char level)
{
	switch (level)
	{
	case LAMP_LEVEL_LOW:
		LAMP_LOW_SetVal();
		break;

	case LAMP_LEVEL_MEDIUM:
		LAMP_MEDIUM_SetVal();
		break;

	case LAMP_LEVEL_HIGH:
		LAMP_HIGH_SetVal();
		break;

	default:
		break;
	}
}

void Reset_Lamp(unsigned char level)
{
	switch (level)
	{
	case LAMP_LEVEL_LOW:
		LAMP_LOW_ClrVal();
		break;

	case LAMP_LEVEL_MEDIUM:
		LAMP_MEDIUM_ClrVal();
		break;

	case LAMP_LEVEL_HIGH:
		LAMP_HIGH_ClrVal();
		break;

	default:
		break;
	}
}

void RTS_Motor_Management(unsigned char action)
{
	switch (action)
	{
	case ENABLE:
		RTS_MOTOR_SetVal();
		break;

	case DISABLE:
		RTS_MOTOR_ClrVal();
		break;

	default:
		break;
	}

}

void EN_Clamp_Control (unsigned char action)
{
	switch (action)
	{
	case ENABLE:
		EN_CLAMP_CONTROL_SetVal();
		break;

	case DISABLE:
		EN_CLAMP_CONTROL_ClrVal();
		break;

	default:
		break;
	}
}

void EN_Motor_Control(unsigned char action)
{
	switch (action)
	{
	case ENABLE:
		EN_MOTOR_CONTROL_SetVal();
		break;

	case DISABLE:
		EN_MOTOR_CONTROL_ClrVal();
		break;

	default:
		break;
	}
}

void EN_24_M_C_Management(unsigned char action)
{
	switch (action)
	{
	case ENABLE:
		EN_24_M_C_SetVal();
		break;

	case DISABLE:
		EN_24_M_C_ClrVal();
		break;

	default:
		break;
	}
}

/**/
