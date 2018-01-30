/*
 * App_Ges.c
 *
 *  Created on: 03/nov/2016
 *      Author: W15
 */

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
#include "Alarm_Con.h"
#include "PC_DEBUG_COMM.h"
#include "stdio.h"
#include "string.h"


float pressSample1_Ven = 0;
float pressSample2_Ven = 0;

float pressSample1 = 0;  // FM Questi due vanno inizializzati alla pressione corrente prima di far partire il pid
float pressSample2 = 0;
unsigned char PidFirstTime[4];

// tick di inizio del trattamento
unsigned long StartTreatmentTime = 0;

// VARIABILI USATE DURANTE LA FASE DI MOUNTING
// numero di parametri ricevuti durante la fase di mounting
unsigned char ParamRcvdInMounting[4];
int AllParametersReceived;
// vale 1 se e' arrivato il comando da sbc per impostare il tipo di terapia
char TherapyCmdArrived;

// ferma il conteggio del tempo durante il trattamento
unsigned char StopTreatmentTime;

float volumePriming = 0;
float volumeTreatArt = 0;


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
	TotalTreatDuration = 0;
	StartTreatmentTime = 0;
	TreatDuration = 0;
}




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
		/* treatment init */
		{STATE_NULL, PARENT_TREAT_KIDNEY_1_INIT,    CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[0],        &manageParentTreatEntry},			/* 1 */
		{STATE_NULL, PARENT_TREAT_KIDNEY_1_INIT,    CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[0],        &manageParentTreatAlways},			/* 2 */
		/* treatment run */
		{STATE_NULL, PARENT_TREAT_KIDNEY_1_PUMP_ON, CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[0],        &manageParentTreatEntry},			/* 3 */
		{STATE_NULL, PARENT_TREAT_KIDNEY_1_PUMP_ON, CHILD_IDLE, ACTION_ALWAYS,   &stateChildIdle[0],        &manageParentTreatAlways},			/* 4 */
		/* treatment alarm */
		{STATE_NULL, PARENT_TREAT_KIDNEY_1_ALARM,   CHILD_IDLE, ACTION_ON_ENTRY, &stateChildAlarmTreat1[1], &manageParentTreatAlarmEntry},		/* 5 */
		{STATE_NULL, PARENT_TREAT_KIDNEY_1_ALARM,   CHILD_IDLE, ACTION_ALWAYS,   &stateChildAlarmTreat1[1], &manageParentTreatAlarmAlways},	    /* 6 */
		/* treatment end */
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

		// Questa volta entro direttamente nello stato run del priming perche' non e' prevista la ricezione di un comando BUTTON_CONFIRM
		{STATE_PRIMING_PH_1,       PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY,                &stateParentPrimingTreatKidney1[3], &managePrimingPh1},				/* 13 */
		{STATE_PRIMING_PH_1,       PARENT_NULL, CHILD_NULL, ACTION_ALWAYS,                  &stateParentPrimingTreatKidney1[3], &managePrimingPh1Always},		/* 14 */

		// Questa volta entro direttamente nello stato run del priming perche' non e' prevista la ricezione di un comando BUTTON_CONFIRM
		{STATE_PRIMING_PH_2,       PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY,                &stateParentPrimingTreatKidney1[3], &managePrimingPh2},				/* 15 */
		{STATE_PRIMING_PH_2,       PARENT_NULL, CHILD_NULL, ACTION_ALWAYS,                  &stateParentPrimingTreatKidney1[3], &managePrimingPh2Always},		/* 16 */

		{STATE_TREATMENT_KIDNEY_1, PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY,                &stateParentTreatKidney1[1],        &manageStateTreatKidney1},		/* 17 */
		{STATE_TREATMENT_KIDNEY_1, PARENT_NULL, CHILD_NULL, ACTION_ALWAYS,                  &stateParentTreatKidney1[1],        &manageStateTreatKidney1Always},/* 18 */

		{STATE_EMPTY_DISPOSABLE,   PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY,                &stateParentNull[0],                &manageStateEmptyDisp},			/* 19 */
		{STATE_EMPTY_DISPOSABLE,   PARENT_NULL, CHILD_NULL, ACTION_ALWAYS,                  &stateParentNull[0],                &manageStateEmptyDispAlways},	/* 20 */

		{STATE_PRIMING_WAIT,       PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY,                &stateParentNull[0],                &manageStatePrimingWait},		/* 21 */
		{STATE_PRIMING_WAIT,       PARENT_NULL, CHILD_NULL, ACTION_ALWAYS,                  &stateParentNull[0],                &manageStatePrimingWaitAlways},	/* 22 */

		{STATE_PRIMING_RICIRCOLO,  PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY,                &stateParentPrimingTreatKidney1[3], &manageStatePrimingRicircolo},		 /* 23 */
		{STATE_PRIMING_RICIRCOLO,  PARENT_NULL, CHILD_NULL, ACTION_ALWAYS,                  &stateParentPrimingTreatKidney1[3], &manageStatePrimingRicircoloAlways}, /* 24 */

		{STATE_WAIT_TREATMENT,     PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY,                &stateParentNull[0],                &manageStateWaitTreatment},			 /* 25 */
		{STATE_WAIT_TREATMENT,     PARENT_NULL, CHILD_NULL, ACTION_ALWAYS,                  &stateParentNull[0],                &manageStateWaitTreatmentAlways},	 /* 26 */

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
void manageStateEmptyDisp(void){
	releaseGUIButton(BUTTON_CONFIRM);
}

void manageStateEmptyDispAlways(void){
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
	else if(parameterWordSetFromGUI[PAR_SET_PRIMING_VOL_PERFUSION].value > perfusionParam.priVolPerfArt)
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
//  Codice originale
//	iflag_pmp1_rx = IFLAG_IDLE;
//
//	if(pumpPerist[0].entry == 0)
//	{
//		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
//		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
//		setPinchPositionHighLevel(PNCHVLV1_ADDRESS, MODBUS_PINCH_RIGHT_OPEN);
//		setPinchPositionHighLevel(PNCHVLV3_ADDRESS, MODBUS_PINCH_RIGHT_OPEN);
//
//		startPeltierActuator();
//		peltierCell.readAlwaysEnable  = 1;
//		peltierCell2.readAlwaysEnable = 1;
//
// 		pumpPerist[0].entry = 1;
//	}

	if(pumpPerist[0].entry == 0)
	{
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
		setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_RIGHT_OPEN);
		setPinchPositionHighLevel(PINCH_2WPVA, MODBUS_PINCH_RIGHT_OPEN);

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


	//manage pump
	switch(ptrCurrentParent->parent){
	case PARENT_PRIMING_TREAT_KIDNEY_1_INIT:
	if(buttonGUITreatment[BUTTON_START_PRIMING].state == GUI_BUTTON_RELEASED)
	{
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 2000);
		if(((PARAMETER_ACTIVE_TYPE)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_ACTIVE].value) == YES)
			setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, (int)((float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / OXYG_FLOW_TO_RPM_CONV * 100.0));
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

		setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, (int)((float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / OXYG_FLOW_TO_RPM_CONV * 100.0));
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
		speed = modbusData[pumpPerist[0].pmpMySlaveAddress-2][17] / 100;
		pumpPerist[0].actualSpeed = speed;
		volumePriming = volumePriming + (float)(speed * CONV_RPMMIN_TO_ML_PER_INTERVAL);
		perfusionParam.priVolPerfArt = (int)(volumePriming);
		pumpPerist[0].dataReady = DATA_READY_FALSE;
	}
	break;

	case PARENT_PRIMING_TREAT_KIDNEY_1_RUN:
			if(buttonGUITreatment[BUTTON_START_PRIMING].state == GUI_BUTTON_RELEASED)
			{
				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 2000);
				if(((PARAMETER_ACTIVE_TYPE)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_ACTIVE].value) == YES)
					setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, (int)((float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / OXYG_FLOW_TO_RPM_CONV * 100.0));
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
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, (int)((float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / OXYG_FLOW_TO_RPM_CONV * 100.0));
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
					((float)perfusionParam.priVolPerfArt >= ((float)parameterWordSetFromGUI[PAR_SET_PRIMING_VOL_PERFUSION].value * 50 / 100.0)))
#else
			// nel caso di debug considero il 50%
			else if((ptrCurrentState->state == STATE_PRIMING_PH_1) &&
					((float)perfusionParam.priVolPerfArt >= ((float)parameterWordSetFromGUI[PAR_SET_PRIMING_VOL_PERFUSION].value * PERC_OF_PRIM_FOR_FILTER / 100.0)))
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
					(perfusionParam.priVolPerfArt >= parameterWordSetFromGUI[PAR_SET_PRIMING_VOL_PERFUSION].value))
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
//					parameterWordSetFromGUI[PAR_SET_PRIMING_VOL_PERFUSION].value > perfusionParam.priVolPerfArt)
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
				//timerCopy = timerCounter;
				if(timerCounterModBus != 0)
					timerCopy = timerCounterModBus;
				timerCounter = 0;
				//timerCounterModBus = 0;

				//readPumpSpeedValue(pumpPerist[0].pmpMySlaveAddress);
				readPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress);

//				readPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress);  // per prova
//				readPumpSpeedValueHighLevel(pumpPerist[2].pmpMySlaveAddress);
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
					volumePriming = volumePriming + (float)(speed * CONV_RPMMIN_TO_ML_PER_INTERVAL);
					perfusionParam.priVolPerfArt = (int)(volumePriming);
				}
				pumpPerist[0].dataReady = DATA_READY_FALSE;

//				pumpPerist[1].actualSpeed =modbusData[pumpPerist[1].pmpMySlaveAddress-2][17] / 100; // per prova
//				pumpPerist[2].actualSpeed =modbusData[pumpPerist[2].pmpMySlaveAddress-2][17] / 100;
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
		speed = modbusData[pumpPerist[0].pmpMySlaveAddress-2][17] / 100;
		pumpPerist[0].actualSpeed = speed;
		pumpPerist[0].dataReady = DATA_READY_FALSE;
	}
*/
}

// In questa funzione ci va quando sono nella fase iniziale del trattamento
// Le pompe verranno fatte partire successivamente su richiesta da parte dell'utente
void manageParentTreatEntry(void){
/*
	//iflag_pmp1_rx = IFLAG_IDLE;
	//static unsigned char entry = 0;

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
		//entry = 1;

		startPeltierActuator();
		peltierCell.readAlwaysEnable = 1;
		peltierCell2.readAlwaysEnable = 1;

		pumpPerist[0].entry = 1;
	}
*/

	if(pumpPerist[0].entry == 0)
	{
		setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
		setPinchPositionHighLevel(PINCH_2WPVF, MODBUS_PINCH_RIGHT_OPEN);
		setPinchPositionHighLevel(PINCH_2WPVA, MODBUS_PINCH_RIGHT_OPEN);
		setPumpPressLoop(0, PRESS_LOOP_OFF);

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
		}
		else if(buttonGUITreatment[BUTTON_START_TREATMENT].state == GUI_BUTTON_RELEASED)
		{
			setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 2000); //pump 0: start value = 20 rpm than pressure loop
			//pump 1: start value = 30 rpm than open loop
			if(((PARAMETER_ACTIVE_TYPE)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_ACTIVE].value) == YES)
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, (int)((float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / OXYG_FLOW_TO_RPM_CONV * 100.0));
			releaseGUIButton(BUTTON_START_TREATMENT);

			//if(iflag_perf == 0)
			setPumpPressLoop(0, PRESS_LOOP_ON);
			// FM Questi due vanno inizializzati alla pressione corrente prima di far partire il pid, altrimenti il motore
			// potrebbe partire anche se non dovrebbe.
			pressSample1 = PR_ART_mmHg_Filtered;
			pressSample2 = PR_ART_mmHg_Filtered;
			StartTreatmentTime = (unsigned long)timerCounterModBus;

//			// attivo il pid sull'ossigenazione e perfusione venosa solo se sono in fegato
//			if(GetTherapyType() == LiverTreat)
//			{
//				setPumpPressLoop(1, PRESS_LOOP_ON);
//				pressSample1_Ven = PR_VEN_mmHg_Filtered;
//				pressSample2_Ven = PR_VEN_mmHg_Filtered;
//			}
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
			setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress,(int)((float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / OXYG_FLOW_TO_RPM_CONV * 100.0));

//			// attivo il pid sull'ossigenazione e perfusione venosa solo se sono in fegato
//			if(GetTherapyType() == LiverTreat)
//			{
//				setPumpPressLoop(1, PRESS_LOOP_ON);
//				pressSample1_Ven = PR_VEN_mmHg_Filtered;
//				pressSample2_Ven = PR_VEN_mmHg_Filtered;
//			}
		}

		if(
			(getPumpPressLoop(0) == PRESS_LOOP_ON) &&
			(timerCounterPID >=1)
			)
		{
			timerCounterPID = 0;
			// TODO nel caso del rene lo 0 va bene perche' la pompa arteriosa e' quella
			// con indice 0 nell'array ..HighLevel. Nel caso del fegato la pompa arteriosa
			// e' quella con indice 1 nell'array ..HighLevel.
			// Eseguo l'aggiornamento del pid solo se non ci sono altre richieste lettura scrittura pendenti
			// inoltre la prima volta che lo eseguo devo inizializzare la variabile statica che contiene la velocita' corrente
			// nella funzione alwaysPumpPressLoop
			//if((pumpPerist[0].reqState == REQ_STATE_OFF) && (pumpPerist[0].reqType == REQ_TYPE_IDLE))
			//if(pumpPerist[0].reqState == REQ_STATE_OFF)
			{
				alwaysPumpPressLoop(0, &PidFirstTime[0]);
			}
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
			speed = modbusData[pumpPerist[0].pmpMySlaveAddress-2][17] / 100;
			pumpPerist[0].actualSpeed = speed;
			volumeTreatArt = volumeTreatArt + (float)(speed * CONV_RPMMIN_TO_ML_PER_INTERVAL);
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
			}
			else if(buttonGUITreatment[BUTTON_START_TREATMENT].state == GUI_BUTTON_RELEASED)
			{
				setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 2000); //pump 0: start value = 20 rpm than pressure loop
				//pump 1: start value = 30 rpm than open loop
				if(((PARAMETER_ACTIVE_TYPE)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_ACTIVE].value) == YES)
					setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, (int)((float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / OXYG_FLOW_TO_RPM_CONV * 100.0));
				releaseGUIButton(BUTTON_START_TREATMENT);

				//if(iflag_perf == 0)
				setPumpPressLoop(0, PRESS_LOOP_ON);
				// FM Questi due vanno inizializzati alla pressione corrente prima di far partire il pid, altrimenti il motore
				// potrebbe partire anche se non dovrebbe.
				pressSample1 = PR_ART_mmHg_Filtered;
				pressSample2 = PR_ART_mmHg_Filtered;
				StartTreatmentTime = (unsigned long)timerCounterModBus;

//				// attivo il pid sull'ossigenazione e perfusione venosa solo se sono in fegato
//				if(GetTherapyType() == LiverTreat)
//				{
//					setPumpPressLoop(1, PRESS_LOOP_ON);
//					pressSample1_Ven = PR_VEN_mmHg_Filtered;
//					pressSample2_Ven = PR_VEN_mmHg_Filtered;
//				}
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
				setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, (int)((float)parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / OXYG_FLOW_TO_RPM_CONV * 100.0));

//				// attivo il pid sull'ossigenazione e perfusione venosa solo se sono in fegato
//				if(GetTherapyType() == LiverTreat)
//				{
//					setPumpPressLoop(1, PRESS_LOOP_ON);
//					pressSample1_Ven = PR_VEN_mmHg_Filtered;
//					pressSample2_Ven = PR_VEN_mmHg_Filtered;
//				}
			}

			if(
				(getPumpPressLoop(0) == PRESS_LOOP_ON) &&
				(timerCounterPID >=1)
				)
			{
				timerCounterPID = 0;
				// TODO nel caso del rene lo 0 va bene perche' la pompa arteriosa e' quella
				// con indice 0 nell'array ..HighLevel. Nel caso del fegato la pompa arteriosa
				// e' quella con indice 1 nell'array ..HighLevel.
				// Eseguo l'aggiornamento del pid solo se non ci sono altre richieste lettura scrittura pendenti
				// inoltre la prima volta che lo eseguo devo inizializzare la variabile statica che contiene la velocita' corrente
				// nella funzione alwaysPumpPressLoop
				//if((pumpPerist[0].reqState == REQ_STATE_OFF) && (pumpPerist[0].reqType == REQ_TYPE_IDLE))
				//if(pumpPerist[0].reqState == REQ_STATE_OFF)
					alwaysPumpPressLoop(0, &PidFirstTime[0]);
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
				pumpPerist[0].actualSpeed = speed / 100;
				volumeTreatArt = volumeTreatArt + (float)(speed * CONV_RPMMIN_TO_ML_PER_INTERVAL);
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
	float Max_RPM_for_Flow_Max = ( parameterWordSetFromGUI[PAR_SET_MAX_FLOW_PERFUSION].value / PUMP_ART_GAIN);

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


// Pid per perfusione venosa

float parKITC_Ven = 0.15;
//float parKP_Ven = 0.05;
float parKP_Ven = 0.03;
float parKD_TC_Ven = 0.0015;
float GlobINTEG_Ven;
float GlobPROP_Ven;
float GlobDER_Ven;
int deltaSpeed_Ven = 0;
int actualSpeed_Ven = 0;

int get_Set_Point_Pressure(int Speed)
{
	int Presure_Set_Point;

	Presure_Set_Point = GAIN_PRESSURE * Speed + DELTA_PRESSURE;

	return (Presure_Set_Point);
}

float CalcolaPres(float speed)
{
  float m = ((float)128.0 - (float)50.0) / ((float)100.0 );
  float press = m * (speed) + (float)50.0;
  return press;
}

void alwaysPumpPressLoopVen_old(unsigned char pmpId, unsigned char *PidFirstTime){
	//int deltaSpeed = 0;
	//static int actualSpeed = 0;
	//static int actualSpeedOld = 0;
	//float parKITC = 0.0; //0.2;
	//float parKP = 5.0;   //1;
	//float parKD_TC = 0.0; //0.8;
	float pressSample0_Ven = 0;
	//static float pressSample1 = 0;  // FM Questi due vanno inizializzati alla pressione corrente prima di far partire il pid
	//static float pressSample2 = 0;
	float errPress = 0;
	int Max_RPM_for_Flow_Max = 120; //( parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / (PUMP_ART_GAIN * 2.0));
	int Speed_Media;

	static int Somma_Speed = 0;

	static int Count = 0;
	static int Count2 = 0;

    if(*PidFirstTime == PRESS_LOOP_ON)
    {
    	*PidFirstTime = PRESS_LOOP_OFF;
    	actualSpeed_Ven = (int)pumpPerist[pmpId].actualSpeed;
    }

    pressSample0_Ven = PR_VEN_mmHg_Filtered;
	errPress = parameterWordSetFromGUI[PAR_SET_VENOUS_PRESS_TARGET].value - pressSample0_Ven;

   if (errPress > -15  && errPress < 15 )
   {
	   Count ++;

	   Somma_Speed += actualSpeed_Ven;

		if ( Count >= 100)
		{
			Speed_Media = Somma_Speed/Count;

//			if (Speed_Media <= Speed_Media_old)
//			{
//				Speed_Media+=10;
//				Speed_Media = Speed_Media_old;
//			}

			parameterWordSetFromGUI[PAR_SET_VENOUS_PRESS_TARGET].value  = get_Set_Point_Pressure(Speed_Media);
			Count = 0;
			Somma_Speed = 0;
		}
   }
   else
   {
	   Count = 0;
	   Somma_Speed = 0;
   }



	GlobINTEG_Ven = parKITC_Ven * errPress;
	GlobPROP_Ven = parKP_Ven * (pressSample0_Ven - pressSample1_Ven);
	GlobDER_Ven = parKD_TC_Ven * (pressSample0_Ven - 2*pressSample1_Ven + pressSample2_Ven);

	deltaSpeed_Ven = (int)((parKITC_Ven * errPress) -
			         (parKP_Ven * (pressSample0_Ven - pressSample1_Ven)) -
					 (parKD_TC_Ven * (pressSample0_Ven - 2 * pressSample1_Ven + pressSample2_Ven)));
//	deltaSpeed_Ven = parKP_Ven * errPress;

	if (errPress < 0)
	{
		int a = 0;
	}

	// valutare se mettere il deltaSpeed = 0 nel caso deltaSpeed sia negativo in modo da non far andare actualSpeed a zero troppo in fretta
	// in alternativa il deltaSpeed va considerato solo se è abbastanza negativo
	if((deltaSpeed_Ven < -2) || (deltaSpeed_Ven > 2))
	{
		if (deltaSpeed_Ven < 0)
		{
			actualSpeed_Ven = actualSpeed_Ven + deltaSpeed_Ven;
		}
		else
		{
			if (sensor_UFLOW[1].Average_Flow_Val > ( parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value))

			{
				actualSpeed_Ven = actualSpeed_Ven; /*non aumento la velocità*/
			}
			else if ( (actualSpeed + deltaSpeed  ) > Max_RPM_for_Flow_Max )
			{
				actualSpeed_Ven = Max_RPM_for_Flow_Max;
			}
			else
			{
				if(actualSpeed_Ven > 100)
					actualSpeed_Ven = 100;
				else
					actualSpeed_Ven = actualSpeed_Ven + deltaSpeed_Ven;
			}
		}
	}

	if(actualSpeed_Ven < 0)
		actualSpeed_Ven = 0;

	if(actualSpeed_Ven != pumpPerist[pmpId].actualSpeedOld)
	{
		setPumpSpeedValueHighLevel(pumpPerist[pmpId].pmpMySlaveAddress, (actualSpeed_Ven * 100));
		pumpPerist[pmpId].actualSpeedOld = actualSpeed_Ven;
	}

	pressSample2_Ven = pressSample1_Ven;
	pressSample1_Ven = pressSample0_Ven;

	//DebugStringPID(); // debug
}

int SpeedCostante( int CurrSpeed)
{
	static int SpeedCostanteState = 0;
	static int LastspeedValue;
	static int Cnt = 0;
	int SpeedCostanteFlag = 0;

	if(!SpeedCostanteState)
	{
		LastspeedValue = CurrSpeed;
		SpeedCostanteState = 1;
		Cnt = 0;
	}
	else if(SpeedCostanteState)
	{
		//
		float min = CurrSpeed - CurrSpeed * 10.0;
		float max = CurrSpeed + CurrSpeed * 10.0;
		if(LastspeedValue < min && LastspeedValue > max )
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

void alwaysPumpPressLoopVen(unsigned char pmpId, unsigned char *PidFirstTime){
	//int deltaSpeed = 0;
	//static int actualSpeed = 0;
	//static int actualSpeedOld = 0;
	//float parKITC = 0.0; //0.2;
	//float parKP = 5.0;   //1;
	//float parKD_TC = 0.0; //0.8;
	float pressSample0_Ven = 0;
	//static float pressSample1 = 0;  // FM Questi due vanno inizializzati alla pressione corrente prima di far partire il pid
	//static float pressSample2 = 0;
	float errPress = 0;
	int Max_RPM_for_Flow_Max = 120; //( parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value / (PUMP_ART_GAIN * 2.0));
	int Speed_Media;
	static int Somma_Speed = 0;
	static int Speed_Media_old = 0xfff; /* valore irraggiungibile*/
	static int Increment = 0;
	static int TargetRaggiunto = 0;

	static int Count = 0;
	static int Count2 = 0;

    if(*PidFirstTime == PRESS_LOOP_ON)
    {
    	*PidFirstTime = PRESS_LOOP_OFF;
    	actualSpeed_Ven = (int)pumpPerist[pmpId].actualSpeed;
    }

    pressSample0_Ven = PR_VEN_mmHg_Filtered;
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
			//   TargetRaggiunto = 0;
			}
	   }
	   else
	   {
		   Increment = 0;
		   Count = 0;
		   Somma_Speed = 0;
	   }

		//la velocità del messaggio di stato resta costante per 5 secondi && velocità minore del massimo)
	    // incrementiamo la actual speed di 5 RPM;
	   if (SpeedCostante(actualSpeed_Ven) && (actualSpeed_Ven <= 100))
	   {
		   actualSpeed_Ven += 5;
		   parameterWordSetFromGUI[PAR_SET_VENOUS_PRESS_TARGET].value  = CalcolaPres(actualSpeed_Ven);
	   }


//	   if(TargetRaggiunto)
//	   {
//			if(parameterWordSetFromGUI[PAR_SET_VENOUS_PRESS_TARGET].value < 120 /* target globale*/)
//			{
//				// incremento la velocita' di un piccolo step per cercare di raggiungere il valore necessario per il flusso
//				Speed_Media_old += 10;
//				parameterWordSetFromGUI[PAR_SET_VENOUS_PRESS_TARGET].value  = CalcolaPres(Speed_Media_old);
//			}
//			TargetRaggiunto = 0;
//		    Increment = 0;
//		    Count = 0;
//		    Somma_Speed = 0;
//	   }



	GlobINTEG_Ven = parKITC_Ven * errPress;
	GlobPROP_Ven = parKP_Ven * (pressSample0_Ven - pressSample1_Ven);
	GlobDER_Ven = parKD_TC_Ven * (pressSample0_Ven - 2*pressSample1_Ven + pressSample2_Ven);

	deltaSpeed_Ven = (int)((parKITC_Ven * errPress) -
			         (parKP_Ven * (pressSample0_Ven - pressSample1_Ven)) -
					 (parKD_TC_Ven * (pressSample0_Ven - 2 * pressSample1_Ven + pressSample2_Ven)));
//	deltaSpeed_Ven = parKP_Ven * errPress;

//	if (errPress < 0)
//	{
//		int a = 0;
//	}

	// valutare se mettere il deltaSpeed = 0 nel caso deltaSpeed sia negativo in modo da non far andare actualSpeed a zero troppo in fretta
	// in alternativa il deltaSpeed va considerato solo se è abbastanza negativo
	if((deltaSpeed_Ven < -1) || (deltaSpeed_Ven > 1))
	{
		TargetRaggiunto = 0; // target non raggiunto
		if (deltaSpeed_Ven < 0)
		{
			actualSpeed_Ven = actualSpeed_Ven + deltaSpeed_Ven;
		}
		else
		{
			if (sensor_UFLOW[1].Average_Flow_Val > ( parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value))

			{
				actualSpeed_Ven = actualSpeed_Ven; /*non aumento la velocità*/
			}
			else if ( (actualSpeed + deltaSpeed  ) > Max_RPM_for_Flow_Max )
			{
				actualSpeed_Ven = Max_RPM_for_Flow_Max;
			}
			else
			{
				if(actualSpeed_Ven >= 100)
					actualSpeed_Ven = 100;
				else
					actualSpeed_Ven = actualSpeed_Ven + deltaSpeed_Ven;
			}
		}
	}
	else
		   TargetRaggiunto = 1; // target non raggiunto


	if(actualSpeed_Ven < 0 || PR_VEN_mmHg_Filtered > 160)
		actualSpeed_Ven = 0;

	if(actualSpeed_Ven != pumpPerist[pmpId].actualSpeedOld)
	{
		setPumpSpeedValueHighLevel(pumpPerist[pmpId].pmpMySlaveAddress, (actualSpeed_Ven * 100));
		pumpPerist[pmpId].actualSpeedOld = actualSpeed_Ven;
	}

	pressSample2_Ven = pressSample1_Ven;
	pressSample1_Ven = pressSample0_Ven;



	//DebugStringPID(); // debug
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
		//releaseGUIButton(BUTTON_KIDNEY);
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
//	else if((buttonGUITreatment[BUTTON_KIDNEY].state == GUI_BUTTON_RELEASED) &&
//			(buttonGUITreatment[BUTTON_CONFIRM].state == GUI_BUTTON_RELEASED))  // TODO eliminare, solo per debug
//	{
//		currentGuard[GUARD_ENABLE_SELECT_TREAT_PAGE].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
//	}
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
}

/*--------------------------------------------------------------------*/
/*  This function compute the guard value in priming phase 2 state   */
/*  Controllo quando iniziare il trattamento
/*--------------------------------------------------------------------*/
static void computeMachineStateGuardPrimingPh2(void){
/* vecchia gestione
	if(
		(perfusionParam.priVolPerfArt >= parameterWordSetFromGUI[PAR_SET_PRIMING_VOL_PERFUSION].value) &&
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

// Test portata doppia pompa Davide CAPPI
//void TestPump(unsigned char Adr)
//{
//	static bool MotorOn = 0;
//
//	if (Bubble_Keyboard_GetVal(BUTTON_1) && !MotorOn)
//	{
//	  /*accendo il motore*/
//	  MotorOn = TRUE;
//	  EN_Motor_Control(ENABLE);
//	  setPumpSpeedValueHighLevel(Adr,7500);
//	}
//	else if (Bubble_Keyboard_GetVal(BUTTON_2) && !MotorOn)
//	{
//	  /*accendo il motore*/
//	  MotorOn = TRUE;
//	  EN_Motor_Control(ENABLE);
//	  setPumpSpeedValueHighLevel(Adr,10000);
//	}
//	else if (Bubble_Keyboard_GetVal(BUTTON_3) && !MotorOn)
//	{
//	  /*accendo il motore*/
//	  MotorOn = TRUE;
//	  EN_Motor_Control(ENABLE);
//	  setPumpSpeedValueHighLevel(Adr,13000);
//	}
//	else if (Bubble_Keyboard_GetVal(BUTTON_4) && MotorOn)
//	{
//	  /*spengo il motore*/
//	  MotorOn = FALSE;
//	  EN_Motor_Control(DISABLE);
//	  setPumpSpeedValueHighLevel(Adr,0);
//	}
//	else
//	{
//		PumpAverageCurrent = modbusData[Adr-2][16];
//		PumpSpeedVal = modbusData[Adr-2][17];
//		PumpStatusVal = modbusData[Adr-2][18];
//		//readPumpSpeedValue(pumpPerist[Adr - 2].pmpMySlaveAddress);
//		//readPumpSpeedValue(Adr - 2);
//	}
//}


// Adr 2..5
//void TestPump(unsigned char Adr)
//{
//	static bool MotorOn = 0;
//
//	if (Bubble_Keyboard_GetVal(BUTTON_1) && !MotorOn)
//	{
//	  /*accendo il motore*/
//	  MotorOn = TRUE;
//	  EN_Motor_Control(ENABLE);
//	  setPumpSpeedValueHighLevel(Adr,2000);
//	}
//	else if (Bubble_Keyboard_GetVal(BUTTON_4) && MotorOn)
//	{
//	  /*spengo il motore*/
//	  MotorOn = FALSE;
//	  EN_Motor_Control(DISABLE);
//	  setPumpSpeedValueHighLevel(Adr,0);
//	}
//	else
//	{
//		PumpAverageCurrent = modbusData[Adr-2][16];
//		PumpSpeedVal = modbusData[Adr-2][17];
//		PumpStatusVal = modbusData[Adr-2][18];
//		//readPumpSpeedValue(pumpPerist[Adr - 2].pmpMySlaveAddress);
//		//readPumpSpeedValue(Adr - 2);
//	}
//}

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
				// TODO inrealta' dovrei andare nello stato 17 perche' dovrei fare lo svuotamento
				ptrFutureState = &stateState[3];
				/* compute future parent */
				ptrFutureParent = ptrFutureState->ptrParent;
				/* compute future child */
				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
				DebugStringStr("STATE_IDLE");
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
			}

			break;

		case PARENT_PRIMING_TREAT_KIDNEY_1_RUN:
//			if((float)perfusionParam.priVolPerfArt >= ((float)parameterWordSetFromGUI[PAR_SET_PRIMING_VOL_PERFUSION].value * PERC_OF_PRIM_FOR_FILTER / 100.0))
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
			}
			break;

		case PARENT_PRIMING_TREAT_KIDNEY_1_ALARM:
			if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_FALSE)
			{
				/* (FM) finita la situazione di allarme posso ritornare in PARENT_PRIMING_TREAT_KIDNEY_1_INIT*/
				ptrFutureParent = &stateParentPrimingTreatKidney1[1];
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

/**/
void initGUIButton(void){
	buttonGUITreatment[BUTTON_PINCH_2WPVF_RIGHT_OPEN].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_PINCH_2WPVF_LEFT_OPEN].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_PINCH_2WPVF_BOTH_CLOSED].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_PINCH_2WPVA_RIGHT_OPEN].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_PINCH_2WPVA_LEFT_OPEN].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_PINCH_2WPVA_BOTH_CLOSED].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_PINCH_2WPVV_RIGHT_OPEN].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_PINCH_2WPVV_LEFT_OPEN].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_PINCH_2WPVV_BOTH_CLOSED].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_CONFIRM].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_RESET].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_PRIMING_END_CONFIRM].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_PRIMING_FILT_INS_CONFIRM].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_PRIMING_ABANDON].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_START_PRIMING].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_STOP_PRIMING].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_STOP_ALL_PUMP].state = GUI_BUTTON_NULL;
	buttonGUITreatment[BUTTON_START_TREATMENT].state = GUI_BUTTON_NULL; // viene dato alla fine del ricircolo per far partire il trattamento
	buttonGUITreatment[BUTTON_STOP_TREATMENT].state = GUI_BUTTON_NULL;
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

	buttonGUITreatment[BUTTON_KIDNEY].state = GUI_BUTTON_NULL; // TODO da togliere, messo solo per debug
}

// forzo stato GUI_BUTTON_RELEASED sul tasto buttonId
// Questo e' stato fatto perche' ora si vuol lavorare sugli eventi di release
// e non di press.
// Facendo questa inversione non cambio tutto il resto del codice.
void setGUIButton(unsigned char buttonId){
	//buttonGUITreatment[buttonId].state = GUI_BUTTON_PRESSED;
	buttonGUITreatment[buttonId].state = GUI_BUTTON_RELEASED;
	actionFlag = 2;
}

unsigned char getGUIButton(unsigned char buttonId)
{
	return buttonGUITreatment[buttonId].state;
}

// forzo stato GUI_BUTTON_PRESSED sul tasto buttonId
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
	parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value = 0;
	parameterWordSetFromGUI[PAR_SET_TEMPERATURE].value = 0;
	parameterWordSetFromGUI[PAR_SET_PRESS_ART_TARGET].value = 0;
	parameterWordSetFromGUI[PAR_SET_PRESS_VEN_TARGET].value = 0;
	parameterWordSetFromGUI[PAR_SET_PURIF_FLOW_TARGET].value = 0;
	parameterWordSetFromGUI[PAR_SET_PURIF_UF_FLOW_TARGET].value = 0;
}
void initSetParamInSourceCode(void)
{
	parameterWordSetFromGUI[PAR_SET_VENOUS_PRESS_TARGET].value = SET_POINT_PRESSURE_INIT;
	parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value = 2000;
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
			setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, (int)((float)value / OXYG_FLOW_TO_RPM_CONV * 100.0));
		}
	}
}


void setParamWordFromGUI(unsigned char parId, int value)
{
	if(parId == PAR_SET_PRIMING_VOL_PERFUSION)
	{
		// controllo range del volume di liquido caricato nel reservoir
		if(value > MAX_VOLUME_RESERVOIR)
			value = MAX_VOLUME_RESERVOIR;
		if(value < MIN_VOLUME_RESERVOIR)
			value = MIN_VOLUME_RESERVOIR;
#ifdef DEBUG_WITH_SERVICE_SBC
		if(!ParamRcvdInMounting[0])
			value = 62; // usato per debug con service sbc
		else
			value = 93;
#endif
		ParamRcvdInMounting[0] = 1;
		DebugStringStr("PAR_SET_PRIMING_VOL_PERFUSION");
	}
	else if(parId == PAR_SET_PRIMING_TEMPERATURE_PERFUSION)
	{
//#ifdef DEBUG_WITH_SERVICE_SBC
//		value = value * 10; // usato per debug con service sbc
//#endif
		// controllo range del volume di liquido caricato nel reservoir
		if(value > (MAX_TEMP_PRIMING * 10))
			value = MAX_TEMP_PRIMING * 10;
		if(value < (MIN_TEMP_PRIMING * 10))
			value = MIN_TEMP_PRIMING * 10;
		ParamRcvdInMounting[1] = 1;
		DebugStringStr("PAR_SET_PRIMING_TEMPERATURE_PERFUSION");
	}
	else if(parId == PAR_SET_OXYGENATOR_ACTIVE)
	{
		if((PARAMETER_ACTIVE_TYPE)value == YES)
		{
			ParamRcvdInMounting[2] = 1;
			DebugStringStr("PAR_SET_OXYGENATOR_ACTIVE");
		}
		else if((PARAMETER_ACTIVE_TYPE)value == NO)
		{
			ParamRcvdInMounting[2] = 1;
			DebugStringStr("PAR_SET_OXYGENATOR_NOT_ACTIVE");
		}
		else
			DebugStringStr("PAR_SET_OXYGENATOR_UNDEF");
	}
	else if(parId == PAR_SET_DEPURATION_ACTIVE)
	{
		if((PARAMETER_ACTIVE_TYPE)value == YES)
		{
			ParamRcvdInMounting[3] = 1;
			DebugStringStr("PAR_SET_DEPURATION_ACTIVE");
		}
		else if((PARAMETER_ACTIVE_TYPE)value == NO)
		{
			ParamRcvdInMounting[3] = 1;
			DebugStringStr("PAR_SET_DEPURATION_NOT_ACTIVE");
		}
		else
			DebugStringStr("PAR_SET_DEPURATION_UNDEF");
	}
	else if (parId == PAR_SET_MAX_FLOW_PERFUSION)
	{
		int MaxFlow;
		int MinFlow;

		if (GetTherapyType() == KidneyTreat)
		{
			MaxFlow = MAX_FLOW_ART_KIDNEY;
			MinFlow = MIN_FLOW_ART_KIDNEY;
		}
		else if (GetTherapyType() == LiverTreat)
		{
			MaxFlow = MAX_FLOW_ART_LIVER;
			MinFlow = MIN_FLOW_ART_LIVER;
		}

		// controllo range del volume di liquido caricato nel reservoir
		if(value >= MaxFlow)
			value = MaxFlow;
		if(value <= MinFlow)
			value = MinFlow;
	}
	else if(parId == PAR_SET_OXYGENATOR_FLOW)
	{
		//CheckOxygenationSpeed(value);
	}
	else if(parId == PAR_SET_THERAPY_TYPE)
	{
		TherapyCmdArrived = 1;
	}

//#ifdef DEBUG_WITH_SERVICE_SBC
//  parameterWordSetFromGUI[PAR_SET_DESIRED_DURATION].value = 0x0002; // 0 ore 2 minuti
//	parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value = 183;     // valore di ml/minuto corrispondenti a 10 rpm
//	parameterWordSetFromGUI[PAR_SET_PRESS_ART_TARGET].value = 60;
//	parameterWordSetFromGUI[PAR_SET_MAX_FLOW_PERFUSION].value = 500;
//#endif


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

void Buzzer_Management(BUZZER_LEVEL level)
{
	switch (level)
	{
		case SILENT:
			BUZZER_LOW_C_ClrVal(); 		//disattiva il buzzer low
			BUZZER_MEDIUM_C_ClrVal();	//disattiva il buzzer Medium
			BUZZER_HIGH_C_ClrVal();		//disattiva il buzzer HIGH
			break;

		case LOW:
			BUZZER_LOW_C_SetVal(); 		//attiva il buzzer low
			break;

		case MEDIUM:
			BUZZER_MEDIUM_C_SetVal();	//attiva il buzzer Medium
			break;

		case HIGH:
			BUZZER_HIGH_C_SetVal();		//attiva il buzzer High
			break;

		default:
			break;
	}
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


void DebugString()
{
	static char stringPr[STR_DBG_LENGHT];
//	sprintf(stringPr, "\r %i; %i; %i; %i; %i; %i; %i; %d;",
//							PR_ADS_FLT_mmHg,
//							PR_VEN_mmHg,
//							PR_ART_mmHg,
//							(int) (sensorIR_TM[0].tempSensValue*10),
//							(int) (sensorIR_TM[1].tempSensValue*10),
//							(int) (sensorIR_TM[2].tempSensValue*10),
//							(int) (sensor_PRx[0].prSensValueFilteredWA),
//							(int) pumpPerist[0].actualSpeed
//				);

	sprintf(stringPr, "\r %i; %i; %i; %i; %i; %d; %d;",
							(int) (sensorIR_TM[0].tempSensValue*10)/*PR_ADS_FLT_mmHg*/,
							(int) (sensorIR_TM[1].tempSensValue*10)/*PR_VEN_mmHg*/,
							(int) (sensorIR_TM[2].tempSensValue*10),
							PR_ART_mmHg_Filtered,
							TotalTreatDuration + TreatDuration,
							(int) pumpPerist[0].actualSpeed,
							(int) perfusionParam.priVolPerfArt
				);

//	sprintf(stringPr, "\r %i; %i; %i; - %i; %i; %d; %d;",
//							(int) (sensorIR_TM[0].tempSensValue*10)/*PR_ADS_FLT_mmHg*/,
//							(int) (sensorIR_TM[1].tempSensValue*10)/*PR_VEN_mmHg*/,
//							(int) (sensor_PRx[0].prSensValueFilteredWA),
//							(int) pumpPerist[0].actualSpeed,
//							(int) perfusionParam.priVolPerfArt,
//							(int) pumpPerist[1].actualSpeed,
//							(int) pumpPerist[2].actualSpeed
//				);

	for(int i=0; i<STR_DBG_LENGHT; i++)
	{
		if(stringPr[i])
			PC_DEBUG_COMM_SendChar(stringPr[i]);
		else
			break;
	}
	PC_DEBUG_COMM_SendChar(0x0A);
}

void DebugStringPID()
{
	static char stringPid[STR_DBG_LENGHT];

//	sprintf(stringPid, "\r %i; %i; %i; %i; %i; %d; %d;",
//							(int)(GlobINTEG * 100),
//							(int)(GlobPROP * 100),
//							(int)(GlobDER * 100),
//							PR_ART_mmHg_Filtered,
//							deltaSpeed,
//							(int) actualSpeed,
//							(int) perfusionParam.priVolPerfArt
//				);


	sprintf(stringPid, "\r %i; %i; %i; %i; %i; %d; %d;",
							(int)(GlobINTEG_Ven * 100),
							(int)(GlobPROP_Ven * 100),
							(int)(GlobDER_Ven * 100),
							PR_VEN_mmHg_Filtered,
							deltaSpeed_Ven,
							(int) actualSpeed_Ven,
							(int) perfusionParam.priVolPerfArt
				);

	for(int i=0; i<STR_DBG_LENGHT; i++)
	{
		if(stringPid[i])
			PC_DEBUG_COMM_SendChar(stringPid[i]);
		else
			break;
	}
//	PC_DEBUG_COMM_SendChar(0x0A);
}


void DebugStringStr(char *s)
{
	static char stringPr[STR_DBG_LENGHT];
	sprintf(stringPr, "\r %s;\r\n", s);

	for(int i=0; i<STR_DBG_LENGHT; i++)
	{
		if(stringPr[i])
			PC_DEBUG_COMM_SendChar(stringPr[i]);
		else
			break;
	}
	PC_DEBUG_COMM_SendChar(0x0A);
}


unsigned char ReadKey1(void)
{
	static int Counter = 0;
	static unsigned char state = 0;
	unsigned char Released = 0;
	unsigned char key;

	key = Bubble_Keyboard_GetVal(BUTTON_1);
	if (key && (state == 0))
	{
		Counter++;
		if( Counter > 100)
		{
			Counter = 0;
			state = 1; // tasto premuto
			Released = 0;
		}
	}
	else if (!key && (state == 1))
	{
		Counter++;
		if( Counter > 100)
		{
			state = 0;  // tasto rilasciato
			Counter = 0;
			Released = 1;
		}
	}
	else
	{
		Counter = 0;
	}
	return Released;
}


unsigned char ReadKey2(void)
{
	static int Counter = 0;
	static unsigned char state = 0;
	unsigned char Released = 0;
	unsigned char key;

	key = Bubble_Keyboard_GetVal(BUTTON_2);
	if (key && (state == 0))
	{
		Counter++;
		if( Counter > 100)
		{
			Counter = 0;
			state = 1; // tasto premuto
			Released = 0;
		}
	}
	else if (!key && (state == 1))
	{
		Counter++;
		if( Counter > 100)
		{
			state = 0;  // tasto rilasciato
			Counter = 0;
			Released = 1;
		}
	}
	else
	{
		Counter = 0;
	}
	return Released;
}

unsigned char ReadKey3(void)
{
	static int Counter = 0;
	static unsigned char state = 0;
	unsigned char Released = 0;
	unsigned char key;

	key = Bubble_Keyboard_GetVal(BUTTON_3);
	if (key && (state == 0))
	{
		Counter++;
		if( Counter > 100)
		{
			Counter = 0;
			state = 1; // tasto premuto
			Released = 0;
		}
	}
	else if (!key && (state == 1))
	{
		Counter++;
		if( Counter > 100)
		{
			state = 0;  // tasto rilasciato
			Counter = 0;
			Released = 1;
		}
	}
	else
	{
		Counter = 0;
	}
	return Released;
}


unsigned char Released1;
unsigned char Released2;
unsigned char Released3;


void GenEvntParentPrim(void)
{
	switch(ptrCurrentParent->parent){
		case PARENT_PRIMING_TREAT_KIDNEY_1_INIT:
			if(Released2)
			{
				Released2 = 0;
				if(ptrCurrentParent->action == ACTION_ALWAYS)
				{
					setGUIButton((unsigned char)BUTTON_START_PRIMING);
					parameterWordSetFromGUI[PAR_SET_PRESS_ART_TARGET].value = 60;
					setGUIButton((unsigned char)BUTTON_START_OXYGEN_PUMP);
					// evito che all'entry del parent mi fermi le pompe
					pumpPerist[0].entry = 1;

					if(perfusionParam.priVolPerfArt < parameterWordSetFromGUI[PAR_SET_PRIMING_VOL_PERFUSION].value)
					{
						setGUIButton((unsigned char)BUTTON_CONFIRM);
					}
					else
					{
						DebugStringStr("START POMPA");
					}
				}
			}
			if(Released3)
			{
				setGUIButton((unsigned char)BUTTON_START_PRIMING);
				Released3 = 0;
			}
			break;

		case PARENT_PRIMING_TREAT_KIDNEY_1_RUN:
			if(Released2)
			{
				Released2 = 0;
				if(ptrCurrentParent->action == ACTION_ALWAYS)
				{
					setGUIButton((unsigned char)BUTTON_CONFIRM);
					DebugStringStr("STATE_TREATMENT_KIDNEY_1");
				}
			}
			if(Released3)
			{
				setGUIButton((unsigned char)BUTTON_START_PRIMING);
				Released3 = 0;
			}
			break;

		case PARENT_PRIMING_TREAT_KIDNEY_1_ALARM:
			if(Released3)
			{
				setGUIButton((unsigned char)BUTTON_START_PRIMING);
				Released3 = 0;
			}
			break;

		case PARENT_PRIMING_TREAT_KIDNEY_1_END:
			break;

		default:
			break;
	}
}


void GenEvntParentTreat(void)
{
	switch(ptrCurrentParent->parent){
		case PARENT_TREAT_KIDNEY_1_INIT:
//			if(perfusionParam.treatVolPerfArt >= 200)
//			{
//				/* FM faccio partire la pompa */
//			}
			if(ptrCurrentParent->action == ACTION_ON_ENTRY)
			{
				DebugStringStr("STATE_TREATMENT_KIDNEY_1___");
			}
			break;

		case PARENT_TREAT_KIDNEY_1_PUMP_ON:
			break;

		case PARENT_TREAT_KIDNEY_1_ALARM:
			break;

		case PARENT_TREAT_KIDNEY_1_END:
			break;

		default:
			break;
	}
}


// viene chiamata ad intervalli di 50 msec
// sequenza di tasti 1,2 della tastiera a bolle fino ad arrivare a STATE_PRIMING_1
//  tasto 2 per lo start della pompa nella fase STATE_PRIMING_1 dopo un po..
//          Con il Tasto 3 faccio ripartire le pompe dopo un allarme
//  tasto 1 per passare a STATE_PRIMING_2 dopo un po..
//  tasto 2 per dare lo start alla pompa in STATE_PRIMING_2 una volta
//          raggiunto il volume richiesto di  50 ml premere di nuovo ..
//          Con il Tasto 3 faccio ripartire le pompe dopo un allarme
//  tasto 2 passo alla fase di trattamento poi ...
//  tasto 1 faccio partire le pompe all'inizio del trattamento
//          Con il Tasto 3 faccio ripartire le pompe dopo un allarme
//          DEVO USARE QUESTO TASTO ANCHE PER FAR RIPARTIRE I MOTORI
//          DOPO UN ALLARME E QUANDO SONO IN TRATTAMENTO
void GenerateSBCComm(void)
{
	static int timerCounterGenSBCComm = 0;

//	switch (ptrCurrentState->state)
//	{
//		case STATE_IDLE:
//			if(Released1)
//			{
//				Released1 = 0;
//				setGUIButton((unsigned char)BUTTON_KIDNEY);
//
//				//---------------------------------------------------------------
//				// IMPOSTO ALCUNI PARAMETRI NECESSARI AL TRATTAMENTO
//				// Imposto un volume molto basso per uscire subito dal priming
//				parameterWordSetFromGUI[PAR_SET_PRIMING_VOL_PERFUSION].value = 5;
//				parameterWordSetFromGUI[PAR_SET_PRESS_ART_TARGET].value = 60;
//				// imposto il volume iniziale (NON SERVE)
//				//perfusionParam.priVolPerfArt = 1300;
//				// 10 minuti di trattamento
//				parameterWordSetFromGUI[PAR_SET_DESIRED_DURATION].value = 600;
//				parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FLOW].value = 1000;
//				parameterWordSetFromGUI[PAR_SET_MAX_FLOW_PERFUSION].value = 500;
//			}
//			else if(Released2)
//			{
//				setGUIButton((unsigned char)BUTTON_CONFIRM);
//				Released2 = 0;
//				DebugStringStr("STATE_SELECT_TREAT");
//			}
//			break;
//		case STATE_SELECT_TREAT:
//			if(Released1)
//			{
//				setGUIButton((unsigned char)BUTTON_EN_PERFUSION);
//				setGUIButton((unsigned char)BUTTON_EN_OXYGENATION);
//				Released1 = 0;
//			}
//			else if(Released2)
//			{
//				setGUIButton((unsigned char)BUTTON_CONFIRM);
//				Released2 = 0;
//				DebugStringStr("STATE_MOUNTING_DISP");
//			}
//			break;
//		case STATE_MOUNTING_DISP:
//			if(Released1)
//			{
//				setGUIButton((unsigned char)BUTTON_PERF_DISP_MOUNTED);
//				setGUIButton((unsigned char)BUTTON_OXYG_DISP_MOUNTED);
//				Released1 = 0;
//			}
//			else if(Released2)
//			{
//				setGUIButton((unsigned char)BUTTON_CONFIRM);
//				Released2 = 0;
//				DebugStringStr("STATE_TANK_FILL");
//			}
//			break;
//		case STATE_TANK_FILL:
//			if(Released1)
//			{
//				setGUIButton((unsigned char)BUTTON_PERF_TANK_FILL);
//				Released1 = 0;
//			}
//			else if(Released2)
//			{
//				setGUIButton((unsigned char)BUTTON_CONFIRM);
//				Released2 = 0;
//				DebugStringStr("STATE_PRIMING_PH_1");
//			}
//			break;
//		case STATE_PRIMING_PH_1:
//			if(Released1)
//			{
//				setGUIButton((unsigned char)BUTTON_PERF_FILTER_MOUNT);
//				setGUIButton((unsigned char)BUTTON_CONFIRM);
//				DebugStringStr("STATE_PRIMING_PH_2");
//				Released1 = 0;
//			}
//			else if(Released2)
//			{
//				// Questo tasto viene usato in GenEvntParentPrim per far partire la pompa
//				GenEvntParentPrim();
//				Released2 = 0;
//			}
//			else if(Released3)
//			{
//				// Questo tasto viene usato in GenEvntParentPrim per far ripartire la pompa
//				// dopo un allarme
//				GenEvntParentPrim();
//				Released3 = 0;
//			}
//			break;
//		case STATE_PRIMING_PH_2:
//			if(Released1)
//			{
//				setGUIButton((unsigned char)BUTTON_CONFIRM);
//				DebugStringStr("STATE_TREATMENT_KIDNEY_1");
//				Released1 = 0;
//			}
//			else if(Released2)
//			{
//				// Questo tasto viene usato in GenEvntParentPrim per far ripartire la pompa
//				GenEvntParentPrim();
//				Released2 = 0;
//			}
//			else if(Released3)
//			{
//				// Questo tasto viene usato in GenEvntParentPrim per far partire la pompa
//				// dopo un allarme
//				GenEvntParentPrim();
//				Released3 = 0;
//			}
//			break;
//		case STATE_TREATMENT_KIDNEY_1:
//			if(Released1)
//			{
//				// do lo start al trattamento
//				setGUIButton((unsigned char)BUTTON_START_PRIMING);
//				Released1 = 0;
//			}
//			else if(Released2)
//			{
//				// Questo tasto verra' usato per dare lo stop al trattamento
//				Released2 = 0;
//			}
//			break;
//		case STATE_EMPTY_DISPOSABLE_1:
//			break;
//	}

	if(msTick_elapsed(timerCounterGenSBCComm) >= 20)
	{
		timerCounterGenSBCComm = timerCounterModBus;
		DebugString();
	}

//	// DEBUG visualizzo alcuni dati su seriale di debug ogni secondo
//	if(msTick_elapsed(timerCounterGenSBCComm) >= 20 &&
//
//	  (ptrCurrentState->state != STATE_TREATMENT_KIDNEY_1) )
//	{
//		timerCounterGenSBCComm = timerCounterModBus;
//		DebugString();
//	}
//	else if(msTick_elapsed(timerCounterGenSBCComm) >= 20 &&
//	     (ptrCurrentState->state == STATE_TREATMENT_KIDNEY_1) )
//	{
//		timerCounterGenSBCComm = timerCounterModBus;
//		//DebugStringPID();
//	}
}


// Non so perche' questa da sola non funziona
// Adr 2..5
//void TestPump1(unsigned char Adr )
//{
//	static bool MotorOn = 0;
//	int wait;
//
//	if (Bubble_Keyboard_GetVal(BUTTON_1))
//	{
//	  /*accendo il motore*/
//	  MotorOn = TRUE;
//
//	  wait = FreeRunCnt10msec;
//	  while ((FreeRunCnt10msec - wait) == 0);
//	  setPumpSpeedValue(4, (int) 5000);
//
//	  wait = FreeRunCnt10msec;
//	  while ((FreeRunCnt10msec - wait) == 0);
//	  setPumpSpeedValue(5, (int) 5000);
//	}
//	else if (Bubble_Keyboard_GetVal(BUTTON_2))
//	{
//	  /*spengo il motore*/
//	  MotorOn = TRUE;
//
//	  wait = FreeRunCnt10msec;
//	  while ((FreeRunCnt10msec - wait) == 0);
//	  setPumpSpeedValue(4, (int) 4000);
//	  wait = FreeRunCnt10msec;
//	  while ((FreeRunCnt10msec - wait) >= 2);
//	  setPumpSpeedValue(5, (int) 4000);
//	}
//	else if (Bubble_Keyboard_GetVal(BUTTON_3))
//	{
//	  /*spengo il motore*/
//	  MotorOn = TRUE;
//
//	  wait = FreeRunCnt10msec;
//	  while ((FreeRunCnt10msec - wait) == 0);
//	  setPumpSpeedValue(4, (int) 3000);
//
//	  wait = FreeRunCnt10msec;
//	  while ((FreeRunCnt10msec - wait) >= 2);
//	  setPumpSpeedValue(5, (int) 3000);
//	}
//	else if (Bubble_Keyboard_GetVal(BUTTON_4))
//	{
//		  /*spengo il motore*/
//		  MotorOn = FALSE;
//		  wait = FreeRunCnt10msec;
//		  while ((FreeRunCnt10msec - wait) >= 2);
//		  setPumpSpeedValue(4, (int)0);
//		  wait = FreeRunCnt10msec;
//		  while ((FreeRunCnt10msec - wait) >= 2);
//		  setPumpSpeedValue(5, (int)0);
//	}
//}

/**/
