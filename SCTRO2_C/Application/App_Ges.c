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
#include "BitIoLdd1.h"
#include "Bit2.h"
#include "BitIoLdd2.h"
#include "MODBUS_COMM.h"
#include "ASerialLdd1.h"

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

struct machineChild stateChildNull[] =
		  {
		  	{STATE_NULL, PARENT_NULL, CHILD_NULL, ACTION_NULL, &stateNull[0], &manageNull},
			{STATE_NULL, PARENT_NULL, CHILD_NULL, ACTION_NULL, &stateNull[0], &manageNull},
		  	{}
		  };

struct machineChild stateChildEntry[] =
		  {
		  	{STATE_NULL, PARENT_NULL, CHILD_ENTRY, ACTION_NULL, &stateNull[0], &manageNull},

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
			{STATE_NULL, PARENT_NULL, CHILD_NULL, ACTION_NULL, &stateNull[0], &manageNull},
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_INIT, ACTION_NULL, &stateNull[0], &manageNull},
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_STOP_PERFUSION, ACTION_NULL, &stateNull[0], &manageNull},
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_STOP_PURIFICATION, ACTION_NULL, &stateNull[0], &manageNull},
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_STOP_ALL_PUMP, ACTION_NULL, &stateNull[0], &manageNull},
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_STOP_PELTIER, ACTION_NULL, &stateNull[0], &manageNull},
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_STOP_ALL_ACTUATOR, ACTION_NULL, &stateNull[0], &manageNull},
			{STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_1_END, ACTION_NULL, &stateNull[0], &manageNull},
			{}
		  };

struct machineChild stateChildAlarmTreat2[] =
		 {
		  {STATE_NULL, PARENT_NULL, CHILD_NULL, ACTION_NULL, &stateNull[0], &manageNull},
		  {STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_2_INIT, ACTION_NULL, &stateNull[0], &manageNull},
		  {STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_2_STOP_PERFUSION, ACTION_NULL, &stateNull[0], &manageNull},
		  {STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_2_STOP_PURIFICATION, ACTION_NULL, &stateNull[0], &manageNull},
		  {STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_2_STOP_ALL_PUMP, ACTION_NULL, &stateNull[0], &manageNull},
		  {STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_2_STOP_PELTIER, ACTION_NULL, &stateNull[0], &manageNull},
		  {STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_2_STOP_ALL_ACTUATOR, ACTION_NULL, &stateNull[0], &manageNull},
		  {STATE_NULL, PARENT_NULL, CHILD_TREAT_ALARM_2_END, ACTION_NULL, &stateNull[0], &manageNull},
		  {}
		 };

/**********************************************/
/***********CHILD ACTIVE***********/
/**********************************************/
struct machineChild stateChildAlarmPriming[] ={
		{STATE_NULL, PARENT_NULL, CHILD_NULL, 								ACTION_NULL, 		&stateNull[0], &manageChildNull},	/* 0 */
		//alarm init
		{STATE_NULL, PARENT_NULL, CHILD_PRIMING_ALARM_INIT, 				ACTION_ON_ENTRY, 	&stateNull[0], &manageChildNull},	/* 1 */
		{STATE_NULL, PARENT_NULL, CHILD_PRIMING_ALARM_INIT, 				ACTION_ALWAYS, 		&stateNull[0], &manageChildNull},	/* 2 */
		//alarm stop perfusion
		{STATE_NULL, PARENT_NULL, CHILD_PRIMING_ALARM_STOP_PERFUSION, 		ACTION_ON_ENTRY, 	&stateNull[0], &manageChildNull},	/* 3 */
		{STATE_NULL, PARENT_NULL, CHILD_PRIMING_ALARM_STOP_PERFUSION, 		ACTION_ALWAYS, 		&stateNull[0], &manageChildNull},	/* 4 */
		//alarm stop purification
		{STATE_NULL, PARENT_NULL, CHILD_PRIMING_ALARM_STOP_PURIFICATION, 	ACTION_ON_ENTRY, 	&stateNull[0], &manageChildNull},	/* 5 */
		{STATE_NULL, PARENT_NULL, CHILD_PRIMING_ALARM_STOP_PURIFICATION, 	ACTION_ALWAYS, 		&stateNull[0], &manageChildNull},	/* 6 */
		//alarm stop all pump
		{STATE_NULL, PARENT_NULL, CHILD_PRIMING_ALARM_STOP_ALL_PUMP, 		ACTION_ON_ENTRY, 	&stateNull[0], &manageChildNull},	/* 7 */
		{STATE_NULL, PARENT_NULL, CHILD_PRIMING_ALARM_STOP_ALL_PUMP, 		ACTION_ALWAYS, 		&stateNull[0], &manageChildNull},	/* 8 */
		//alarm stop peltier
		{STATE_NULL, PARENT_NULL, CHILD_PRIMING_ALARM_STOP_PELTIER, 		ACTION_ON_ENTRY, 	&stateNull[0], &manageChildNull},	/* 9 */
		{STATE_NULL, PARENT_NULL, CHILD_PRIMING_ALARM_STOP_PELTIER, 		ACTION_ALWAYS, 		&stateNull[0], &manageChildNull},	/* 10 */
		//alarm stop all actuator
		{STATE_NULL, PARENT_NULL, CHILD_PRIMING_ALARM_STOP_ALL_ACTUATOR, 	ACTION_ON_ENTRY, 	&stateNull[0], &manageChildNull},	/* 11 */
		{STATE_NULL, PARENT_NULL, CHILD_PRIMING_ALARM_STOP_ALL_ACTUATOR, 	ACTION_ALWAYS, 		&stateNull[0], &manageChildNull},	/* 12 */
		//alarm priming end
		{STATE_NULL, PARENT_NULL, CHILD_PRIMING_ALARM_END, 					ACTION_ON_ENTRY, 	&stateNull[0], &manageChildNull},	/* 13 */
		{STATE_NULL, PARENT_NULL, CHILD_PRIMING_ALARM_END, 					ACTION_ALWAYS, 		&stateNull[0], &manageChildNull},	/* 14 */

		{}
};


/**********************************************/
/**********************************************/
/*********************************************/

/*********************************/
/*********PARENT LEVEL*************/
/*********************************/

struct machineParent stateParentNull[] =
		{
		 {STATE_NULL, PARENT_NULL, CHILD_NULL, ACTION_NULL, &stateChildNull[0], &manageNull},
		 {STATE_NULL, PARENT_NULL, CHILD_NULL, ACTION_NULL, &stateChildNull[0], &manageNull},
		 {}
		};

struct machineParent stateParentEntry[] =
		{
		 {STATE_NULL, PARENT_NULL, CHILD_NULL, ACTION_NULL, &stateChildIdle[0], &manageNull},

		 {STATE_NULL, PARENT_ENTRY, CHILD_NULL, ACTION_ON_ENTRY, &stateChildEntry[1], &manageParentEntry},
		 {STATE_NULL, PARENT_ENTRY, CHILD_NULL, ACTION_ALWAYS, &stateChildEntry[2], &manageParentEntryAlways},

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
		 {STATE_NULL, PARENT_NULL, CHILD_NULL, ACTION_NULL, &stateChildIdle[0], &manageNull},
		 {STATE_NULL, PARENT_T1_NO_DISP_INIT, CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_NO_DISP_INIT, CHILD_IDLE, ACTION_ALWAYS, &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_NO_DISP_CHECK_PRESS, CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_NO_DISP_CHECK_PRESS, CHILD_IDLE, ACTION_ALWAYS, &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_NO_DISP_CHECK_TEMP, CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_NO_DISP_CHECK_TEMP, CHILD_IDLE, ACTION_ALWAYS, &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_NO_DISP_CHECK_FLWMTR, CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_NO_DISP_CHECK_FLWMTR, CHILD_IDLE, ACTION_ALWAYS, &stateChildIdle[1], &manageNull},
	     {STATE_NULL, PARENT_T1_NO_DISP_CHEK_AIR, CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_NO_DISP_CHEK_AIR, CHILD_IDLE, ACTION_ALWAYS, &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_NO_DISP_ALARM, CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_NO_DISP_ALARM, CHILD_IDLE, ACTION_ALWAYS, &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_NO_DISP_END, CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_NO_DISP_END, CHILD_IDLE, ACTION_ALWAYS, &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_NO_DISP_FATAL_ERROR, CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_NO_DISP_FATAL_ERROR, CHILD_IDLE, ACTION_ALWAYS, &stateChildIdle[1], &manageNull},
		 {}
		};

struct machineParent stateParentT1TWithDisposable[] =
		{
		 {STATE_NULL, PARENT_NULL, CHILD_NULL, ACTION_NULL, &stateChildIdle[0], &manageNull},
		 {STATE_NULL, PARENT_T1_WITH_DISP_INIT, CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_WITH_DISP_INIT, CHILD_IDLE, ACTION_ALWAYS, &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_WITH_DISP_LEAK, CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_WITH_DISP_LEAK, CHILD_IDLE, ACTION_ALWAYS, &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_WITH_DISP_LEVEL, CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_WITH_DISP_LEVEL, CHILD_IDLE, ACTION_ALWAYS, &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_WITH_DISP_ALARM, CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_WITH_DISP_ALARM, CHILD_IDLE, ACTION_ALWAYS, &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_WITH_DISP_END, CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_WITH_DISP_END, CHILD_IDLE, ACTION_ALWAYS, &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_WITH_DISP_FATAL_ERROR, CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[1], &manageNull},
		 {STATE_NULL, PARENT_T1_WITH_DISP_FATAL_ERROR, CHILD_IDLE, ACTION_ALWAYS, &stateChildIdle[1], &manageNull},
		 {}
		};

struct machineParent stateParentPrimingTreatKidney1[] =
{
		{STATE_NULL, PARENT_NULL, CHILD_NULL, ACTION_NULL, &stateChildIdle[0], &manageNull},											/* 0 */
		/* priming init */
		{STATE_NULL, PARENT_PRIMING_TREAT_KIDNEY_1_INIT, CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[0], &manageParentPrimingEntry},	/* 1 */
		{STATE_NULL, PARENT_PRIMING_TREAT_KIDNEY_1_INIT, CHILD_IDLE, ACTION_ALWAYS, &stateChildIdle[0], &manageParentPrimingAlways},	/* 2 */
		/* priming run */
		{STATE_NULL, PARENT_PRIMING_TREAT_KIDNEY_1_RUN, CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[0], &manageParentPrimingEntry},	/* 3 */
		{STATE_NULL, PARENT_PRIMING_TREAT_KIDNEY_1_RUN, CHILD_IDLE, ACTION_ALWAYS, &stateChildIdle[0], &manageParentPrimingAlways},		/* 4 */
		/* priming alarm */
		{STATE_NULL, PARENT_PRIMING_TREAT_KIDNEY_1_ALARM, CHILD_IDLE, ACTION_ON_ENTRY, &stateChildAlarmPriming[1], &manageParentPrimingAlarmEntry},	/* 5 */
		{STATE_NULL, PARENT_PRIMING_TREAT_KIDNEY_1_ALARM, CHILD_IDLE, ACTION_ALWAYS, &stateChildAlarmPriming[1], &manageParentPrimingAlarmAlways},	/* 6 */
		/* priming end */
		{STATE_NULL, PARENT_PRIMING_TREAT_KIDNEY_1_END, CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[0], &manageParentPrimingEntry},	/* 7 */
		{STATE_NULL, PARENT_PRIMING_TREAT_KIDNEY_1_END, CHILD_IDLE, ACTION_ALWAYS, &stateChildIdle[0], &manageParentPrimingAlways},		/* 8 */

		{}
};


struct machineParent stateParentTreatKidney1[] =
{
		{STATE_NULL, PARENT_NULL, CHILD_NULL, ACTION_NULL, &stateChildIdle[0], &manageNull},											/* 0 */
		/* priming init */
		{STATE_NULL, PARENT_TREAT_KIDNEY_1_INIT, CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[0], &manageParentTreatEntry},				/* 1 */
		{STATE_NULL, PARENT_TREAT_KIDNEY_1_INIT, CHILD_IDLE, ACTION_ALWAYS, &stateChildIdle[0], &manageParentTreatAlways},				/* 2 */
		/* priming run */
		{STATE_NULL, PARENT_TREAT_KIDNEY_1_PUMP_ON, CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[0], &manageParentTreatEntry},			/* 3 */
		{STATE_NULL, PARENT_TREAT_KIDNEY_1_PUMP_ON, CHILD_IDLE, ACTION_ALWAYS, &stateChildIdle[0], &manageParentTreatAlways},			/* 4 */
		/* priming alarm */
		{STATE_NULL, PARENT_TREAT_KIDNEY_1_ALARM, CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[0], &manageParentTreatAlarmEntry},		/* 5 */
		{STATE_NULL, PARENT_TREAT_KIDNEY_1_ALARM, CHILD_IDLE, ACTION_ALWAYS, &stateChildIdle[0], &manageParentTreatAlarmAlways},		/* 6 */
		/* priming end */
		{STATE_NULL, PARENT_TREAT_KIDNEY_1_END, CHILD_IDLE, ACTION_ON_ENTRY, &stateChildIdle[0], &manageParentTreatEntry},				/* 7 */
		{STATE_NULL, PARENT_TREAT_KIDNEY_1_END, CHILD_IDLE, ACTION_ALWAYS, &stateChildIdle[0], &manageParentTreatAlways},				/* 8 */

		{}
};

/*********************************/
/*********SATATE LEVEL*************/
/*********************************/

struct machineState stateState[] =
		{

		 /**************************************************************************************/
		 /******-----------------------------MACHINE STATE------------------------------********/
		 /**************************************************************************************/
		{STATE_NULL, PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY/*ACTION_NULL*/, &stateParentNull[0], &manageNull},							/* 0 */

		{STATE_ENTRY, PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY, &stateParentEntry[1], &manageStateEntry},									/* 1 */
		{STATE_ENTRY, PARENT_NULL, CHILD_NULL, ACTION_ALWAYS, &stateParentEntry[2], &manageStateEntryAlways},								/* 2 */

		{STATE_IDLE, PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY, &stateParentNull[0], &manageStateIdle},										/* 3 */
		{STATE_IDLE, PARENT_NULL, CHILD_NULL, ACTION_ALWAYS, &stateParentNull[0], &manageStateIdleAlways},									/* 4 */

		{STATE_SELECT_TREAT, PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY, &stateParentNull[0], &manageStateSelTreat},							/* 5 */
		{STATE_SELECT_TREAT, PARENT_NULL, CHILD_NULL, ACTION_ALWAYS, &stateParentNull[0], &manageStateSelTreatAlways},						/* 6 */

		{STATE_T1_NO_DISPOSABLE, PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY, &stateParentNull[0], &manageStateT1NoDisp},						/* 7 */
		{STATE_T1_NO_DISPOSABLE, PARENT_NULL, CHILD_NULL, ACTION_ALWAYS, &stateParentNull[0], &manageStateT1NoDispAlways},					/* 8 */

		{STATE_MOUNTING_DISP, PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY, &stateParentNull[0], &manageStateMountDisp},						/* 9 */
		{STATE_MOUNTING_DISP, PARENT_NULL, CHILD_NULL, ACTION_ALWAYS, &stateParentNull[0], &manageStateMountDispAlways},					/* 10 */

		{STATE_TANK_FILL, PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY, &stateParentNull[0], &manageStateTankFill},								/* 11 */
		{STATE_TANK_FILL, PARENT_NULL, CHILD_NULL, ACTION_ALWAYS, &stateParentNull[0], &manageStateTankFillAlways},							/* 12 */

		{STATE_PRIMING_PH_1, PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY, &stateParentPrimingTreatKidney1[1], &managePrimingPh1},				/* 13 */
		{STATE_PRIMING_PH_1, PARENT_NULL, CHILD_NULL, ACTION_ALWAYS, &stateParentPrimingTreatKidney1[1], &managePrimingPh1Always},			/* 14 */

		{STATE_PRIMING_PH_2, PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY, &stateParentPrimingTreatKidney1[1], &managePrimingPh2},				/* 15 */
		{STATE_PRIMING_PH_2, PARENT_NULL, CHILD_NULL, ACTION_ALWAYS, &stateParentPrimingTreatKidney1[1], &managePrimingPh2Always},			/* 16 */

		{STATE_TREATMENT_KIDNEY_1, PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY, &stateParentTreatKidney1[1], &manageStateTreatKidney1},		/* 17 */
		{STATE_TREATMENT_KIDNEY_1, PARENT_NULL, CHILD_NULL, ACTION_ALWAYS, &stateParentTreatKidney1[1], &manageStateTreatKidney1Always},	/* 18 */

		{STATE_EMPTY_DISPOSABLE, PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY, &stateParentNull[0], &manageStateEmptyDisp},						/* 19 */
		{STATE_EMPTY_DISPOSABLE, PARENT_NULL, CHILD_NULL, ACTION_ALWAYS, &stateParentNull[0], &manageStateEmptyDispAlways},					/* 20 */

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


		 {STATE_TREATMENT_2, PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY, &stateParentNull[0], &manageStateTreat2},
		 {STATE_TREATMENT_2, PARENT_NULL, CHILD_NULL, ACTION_ALWAYS, &stateParentNull[0], &manageStateTreat2},

		 {STATE_EMPTY_DISPOSABLE_1, PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY, &stateParentNull[0], &manageStateEmptyDisp1},
		 {STATE_EMPTY_DISPOSABLE_1, PARENT_NULL, CHILD_NULL, ACTION_ALWAYS, &stateParentNull[0], &manageStateEmptyDisp1Always},

		 {STATE_EMPTY_DISPOSABLE_2, PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY, &stateParentNull[0], &manageStateEmptyDisp2},
		 {STATE_EMPTY_DISPOSABLE_2, PARENT_NULL, CHILD_NULL, ACTION_ALWAYS, &stateParentNull[0], &manageStateEmptyDisp2Always},

		 {STATE_WASHING, PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY, &stateParentNull[0], &manageStateWashing},
		 {STATE_WASHING, PARENT_NULL, CHILD_NULL, ACTION_ALWAYS, &stateParentNull[0], &manageStateWashingAlways},

		 {STATE_FATAL_ERROR, PARENT_NULL, CHILD_NULL, ACTION_NULL, &stateParentNull[0], &manageStateFatalError},
		 {STATE_FATAL_ERROR, PARENT_NULL, CHILD_NULL, ACTION_NULL, &stateParentNull[0], &manageStateFatalErrorAlways},

		 {}
};


/********************************/
/* general purpose function     */
/********************************/

void manageNull(void)
{
	#ifdef DEBUG_TREATMENT
	currentGuard[GUARD_START_ENABLE].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
	currentGuard[GUARD_HW_T1T_DONE].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
	currentGuard[GUARD_COMM_ENABLED].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
	#endif
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
	static float myTempValue = 20;

	if(myTempValue != parameterFloatSetFromGUI[PAR_SET_TEMPERATURE].value){
		myTempValue = parameterFloatSetFromGUI[PAR_SET_TEMPERATURE].value;

		if(myTempValue == 4.0)
		{
			peltierCell.mySet = myTempValue - 8.0;
		}
		else if(myTempValue == 36.0)
		{
			peltierCell.mySet = myTempValue + 6.0;
		}
		else
		{
			peltierCell.mySet = 20.0;
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
	//RMAIN: ENABLE, TCDB: 2, TCLIMIT: 100, SET: -6�C
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
		speed = ((BYTES_TO_WORD_SIGN(msgToRecvFrame3[3], msgToRecvFrame3[4]))/100)*(timerCopy);
		volumePriming = volumePriming + (float)(speed * 0.00775);
		perfusionParam.volumePrimingArt = (int)(volumePriming);
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
			(buttonGUITreatment[BUTTON_START_PRIMING].state == GUI_BUTTON_RELEASED) ||
			(buttonGUITreatment[BUTTON_START_PERF_PUMP].state == GUI_BUTTON_RELEASED)
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
				(buttonGUITreatment[BUTTON_STOP_ALL_PUMP].state == GUI_BUTTON_RELEASED) ||
				(buttonGUITreatment[BUTTON_STOP_PERF_PUMP].state == GUI_BUTTON_RELEASED)
				)
		{
			releaseGUIButton(BUTTON_STOP_ALL_PUMP);
			releaseGUIButton(BUTTON_STOP_PERF_PUMP);

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
				speed = ((BYTES_TO_WORD_SIGN(msgToRecvFrame3[3], msgToRecvFrame3[4]))/100)*(timerCopy);
				volumePriming = volumePriming + (float)(speed * 0.00775);
				perfusionParam.volumePrimingArt = (int)(volumePriming);
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
}

void manageParentTreatAlarmEntry(void){

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
}



void manageParentPrimingAlarmAlways(void){
	//static unsigned char oneShot = 0;
	static int speed = 0;
	static int timerCopy = 0;

	if((pumpPerist[0].dataReady == DATA_READY_FALSE) && (speed != 0))
	{
	setPumpSpeedValueHighLevel(pumpPerist[0].pmpMySlaveAddress, 0);
	setPumpSpeedValueHighLevel(pumpPerist[1].pmpMySlaveAddress, 0);
	setPinchPositionHighLevel(PNCHVLV1_ADDRESS, MODBUS_PINCH_POS_CLOSED);
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
		speed = ((BYTES_TO_WORD_SIGN(msgToRecvFrame3[3], msgToRecvFrame3[4]))/100)*(timerCopy);

		pumpPerist[0].dataReady = DATA_READY_FALSE;
	}
}

void manageParentTreatAlarmAlways(void){

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
		speed = ((BYTES_TO_WORD_SIGN(msgToRecvFrame3[3], msgToRecvFrame3[4]))/100)*(timerCopy);

		pumpPerist[0].dataReady = DATA_READY_FALSE;
	}

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
			speed = ((BYTES_TO_WORD_SIGN(msgToRecvFrame3[3], msgToRecvFrame3[4]))/100)*(timerCopy);
			pumpPerist[0].actualSpeed = speed;
			volumeTreatArt = volumeTreatArt + (float)(speed * 0.00775);
			perfusionParam.volumeTreatArt = (int)(volumeTreatArt);
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
				speed = ((BYTES_TO_WORD_SIGN(msgToRecvFrame3[3], msgToRecvFrame3[4]))/100)*(timerCopy);
				pumpPerist[0].actualSpeed = speed;
				volumeTreatArt = volumeTreatArt + (float)(speed * 0.00775);
				perfusionParam.volumeTreatArt = (int)(volumeTreatArt);
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
	// in alternativa il deltaSpeed va considerato solo se � abbastanza negativo
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

	//se provengo da un allarme la pompa � ferma ed il controllo deve ripartire
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
}

void manageParentEntryAlways(void)
{
	#ifdef DEBUG_ENABLE
	static int index3 = 0;
	index3++;

	if(index3%14000 == 0)
		Bit1_NegVal();
	#endif
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
	if(
		(currentGuard[GUARD_HW_T1T_DONE].guardValue == GUARD_VALUE_TRUE) &&
		(currentGuard[GUARD_COMM_ENABLED].guardValue == GUARD_VALUE_TRUE)
			){
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
/*--------------------------------------------------------------------*/
static void computeMachineStateGuardPrimingPh2(void){
	if(
		(perfusionParam.volumePrimingArt >= parameterWordSetFromGUI[PAR_SET_PRIMING_VOL_PERFUSION].value) &&
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
/*--------------------------------------------------------------------*/
static void computeMachineStateGuardTreatment(void){
	if(
			(buttonGUITreatment[BUTTON_STOP_ALL_PUMP].state == GUI_BUTTON_RELEASED) &&
			(buttonGUITreatment[BUTTON_CONFIRM].state == GUI_BUTTON_RELEASED)
			)
		{
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

/*----------------------------------------------------------------------------*/
/* This function compute the machine state transition based on guard - state level         */
/*----------------------------------------------------------------------------*/
void processMachineState(void)
{
	/* process state structure --> in base alla guard si decide lo stato --> in base allo stato si eseguono certe funzioni in modalit� init o always */
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
			if(
				(currentGuard[GUARD_HW_T1T_DONE].guardValue == GUARD_VALUE_TRUE) &&
				(currentGuard[GUARD_COMM_ENABLED].guardValue == GUARD_VALUE_TRUE)
				)
			{
				/* compute future state */
				ptrFutureState = &stateState[3];
				/* compute future parent */
				ptrFutureParent = ptrFutureState->ptrParent;
				/* compute future child */
				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
			}

			/* compute parent......compute child */

			/* execute function state level */
			manageStateEntryAndStateAlways(2);

			/*if(ptrCurrentState->action == ACTION_ON_ENTRY)
			{
				// execute state callback function
				ptrCurrentState->callBackFunct();
				// compute future state
				ptrFutureState = &stateState[2];
				// execute parent callback function
				ptrCurrentParent->callBackFunct();
				// compute future parent
				ptrFutureParent = ptrFutureState->ptrParent;
				// execute child callback function
				ptrCurrentChild->callBackFunct();
				// compute future child
				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
				actionFlag = 1;
			}
			else if(ptrCurrentState->action == ACTION_ALWAYS)
			{
				ptrCurrentState->callBackFunct();
				ptrCurrentParent->callBackFunct();
				ptrCurrentChild->callBackFunct();
			}*/

			break;

		case STATE_IDLE:
			/* compute future state */
			if(
				(currentGuard[GUARD_ENABLE_SELECT_TREAT_PAGE].guardValue == GUARD_VALUE_TRUE)
				)
			{
				/* compute future state */
				ptrFutureState = &stateState[5];
				/* compute future parent */
				ptrFutureParent = ptrFutureState->ptrParent;
				/* compute future child */
				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
			}
			/* execute function state level */
			manageStateEntryAndStateAlways(4);

			/*if(ptrCurrentState->action == ACTION_ON_ENTRY)
			{
				// execute state callback function
				ptrCurrentState->callBackFunct();
				// compute future state
				ptrFutureState = &stateState[4];
				// execute parent callback function
				ptrCurrentParent->callBackFunct();
				// compute future parent
				ptrFutureParent = ptrFutureState->ptrParent;
				// execute child callback function
				ptrCurrentChild->callBackFunct();
				// compute future child
				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
				actionFlag = 1;
			}
			else if(ptrCurrentState->action == ACTION_ALWAYS)
			{
				ptrCurrentState->callBackFunct();
				ptrCurrentParent->callBackFunct();
				ptrCurrentChild->callBackFunct();
			}*/


			#ifdef DEBUG_ENABLE
			Bit1_NegVal();
			#endif

			break;

		case STATE_SELECT_TREAT:
			/* compute future state */
			if(
					(currentGuard[GUARD_ENABLE_MOUNT_DISPOSABLE].guardValue == GUARD_VALUE_TRUE)
				)
			{
				/* compute future state */
				ptrFutureState = &stateState[9];
				/* compute future parent */
				ptrFutureParent = ptrFutureState->ptrParent;
				/* compute future child */
				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
			}

			/* execute function state level */
			manageStateEntryAndStateAlways(6);

			/*if(ptrCurrentState->action == ACTION_ON_ENTRY)
			{
				// execute state callback function
				ptrCurrentState->callBackFunct();
				// compute future state
				ptrFutureState = &stateState[6];
				// execute parent callback function
				ptrCurrentParent->callBackFunct();
				// compute future parent
				ptrFutureParent = ptrFutureState->ptrParent;
				// execute child callback function
				ptrCurrentChild->callBackFunct();
				// compute future child
				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
				actionFlag = 1;
			}
			else if(ptrCurrentState->action == ACTION_ALWAYS)
			{
				ptrCurrentState->callBackFunct();
				ptrCurrentParent->callBackFunct();
				ptrCurrentChild->callBackFunct();
			}*/

			break;

		case STATE_T1_NO_DISPOSABLE:
			break;

		case STATE_MOUNTING_DISP:
			if(
				(currentGuard[GUARD_ENABLE_TANK_FILL].guardValue == GUARD_VALUE_TRUE)
				)
			{
				/* compute future state */
				ptrFutureState = &stateState[11];
				/* compute future parent */
				ptrFutureParent = ptrFutureState->ptrParent;
				/* compute future child */
				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
			}

			/* execute function state level */
			manageStateEntryAndStateAlways(10);

			/*if(ptrCurrentState->action == ACTION_ON_ENTRY)
			{
				// execute state callback function
				ptrCurrentState->callBackFunct();
				// compute future state
				ptrFutureState = &stateState[10];
				// execute parent callback function
				ptrCurrentParent->callBackFunct();
				// compute future parent
				ptrFutureParent = ptrFutureState->ptrParent;
				// execute child callback function
				ptrCurrentChild->callBackFunct();
				// compute future child
				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
				actionFlag = 1;
			}
			else if(ptrCurrentState->action == ACTION_ALWAYS)
			{
				ptrCurrentState->callBackFunct();
				ptrCurrentParent->callBackFunct();
				ptrCurrentChild->callBackFunct();
			}*/

			break;

		case STATE_TANK_FILL:
			if(
				(currentGuard[GUARD_ENABLE_PRIMING_PHASE_1].guardValue == GUARD_VALUE_TRUE)
				)
			{
				/* compute future state */
				ptrFutureState = &stateState[13];
				/* compute future parent */
				ptrFutureParent = ptrFutureState->ptrParent;
				/* compute future child */
				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
			}
			/* execute function state level */
			manageStateEntryAndStateAlways(12);

			/*if(ptrCurrentState->action == ACTION_ON_ENTRY)
			{
				// execute state callback function
				ptrCurrentState->callBackFunct();
				// compute future state
				ptrFutureState = &stateState[12];
				// execute parent callback function
				ptrCurrentParent->callBackFunct();
				// compute future parent
				ptrFutureParent = ptrFutureState->ptrParent;
				// execute child callback function
				ptrCurrentChild->callBackFunct();
				// compute future child
				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
				actionFlag = 1;
			}
			else if(ptrCurrentState->action == ACTION_ALWAYS)
			{
				ptrCurrentState->callBackFunct();
				ptrCurrentParent->callBackFunct();
				ptrCurrentChild->callBackFunct();
			}*/

			break;

		case STATE_PRIMING_PH_1:
			if(
				(currentGuard[GUARD_ENABLE_PRIMING_PHASE_2].guardValue == GUARD_VALUE_TRUE)
				)
			{
				/* compute future state */
				ptrFutureState = &stateState[15];
				/* compute future parent */
				ptrFutureParent = ptrFutureState->ptrParent;
				/* compute future child */
				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
			}
			/* execute function state level */
			manageStateEntryAndStateAlways(14);

			/*if(ptrCurrentState->action == ACTION_ON_ENTRY)
			{
				// execute state callback function
				ptrCurrentState->callBackFunct();
				// compute future state
				ptrFutureState = &stateState[14];
				// execute parent callback function
				ptrCurrentParent->callBackFunct();
				// compute future parent
				ptrFutureParent = ptrFutureState->ptrParent;
				// execute child callback function
				ptrCurrentChild->callBackFunct();
				// compute future child
				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
				actionFlag = 1;
			}
			else if(ptrCurrentState->action == ACTION_ALWAYS)
			{
				ptrCurrentState->callBackFunct();
				ptrCurrentParent->callBackFunct();
				ptrCurrentChild->callBackFunct();
			}*/

			break;

		case STATE_PRIMING_PH_2:
			if(
				(currentGuard[GUARD_ENABLE_TREATMENT_KIDNEY_1].guardValue == GUARD_VALUE_TRUE)
				)
			{
				/* compute future state */
				ptrFutureState = &stateState[17];
				/* compute future parent */
				ptrFutureParent = ptrFutureState->ptrParent;
				/* compute future child */
				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
			}
			/* execute function state level */
			manageStateEntryAndStateAlways(16);

			/*if(ptrCurrentState->action == ACTION_ON_ENTRY)
			{
				// execute state callback function
				ptrCurrentState->callBackFunct();
				// compute future state
				ptrFutureState = &stateState[16];
				// execute parent callback function
				ptrCurrentParent->callBackFunct();
				// compute future parent
				ptrFutureParent = ptrFutureState->ptrParent;
				// execute child callback function
				ptrCurrentChild->callBackFunct();
				// compute future child
				ptrFutureChild = ptrFutureState->ptrParent->ptrChild;
				actionFlag = 1;
			}
			else if(ptrCurrentState->action == ACTION_ALWAYS)
			{
				ptrCurrentState->callBackFunct();
				ptrCurrentParent->callBackFunct();
				ptrCurrentChild->callBackFunct();
			}*/

			break;

		case STATE_TREATMENT_KIDNEY_1:
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

	//questo switch andr� suddiviso e portato dentro i singoli case dello switch sopra........
	switch(ptrCurrentParent->parent){
		case PARENT_PRIMING_TREAT_KIDNEY_1_INIT:
			if(buttonGUITreatment[BUTTON_CONFIRM].state == GUI_BUTTON_RELEASED)
			{
				releaseGUIButton(BUTTON_CONFIRM);

				ptrFutureParent = &stateParentPrimingTreatKidney1[3];
			}

			if(ptrCurrentParent->action == ACTION_ON_ENTRY)
				{
					/* execute parent callback function */
					ptrCurrentParent->callBackFunct();
					/* compute future parent */
					ptrFutureParent = &stateParentPrimingTreatKidney1[2];
				}
			else if(ptrCurrentState->action == ACTION_ALWAYS)
				{
					ptrCurrentParent->callBackFunct();
				}

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
			else if(ptrCurrentState->action == ACTION_ALWAYS)
			{
				ptrCurrentParent->callBackFunct();
			}

			if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_TRUE)
			{
				ptrFutureParent = &stateParentPrimingTreatKidney1[5];
				ptrFutureChild = ptrFutureParent->ptrChild;
			}
			break;

		case PARENT_PRIMING_TREAT_KIDNEY_1_ALARM:
			if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_FALSE)
			{
				ptrFutureParent = &stateParentPrimingTreatKidney1[1];
			}

			if(ptrCurrentParent->action == ACTION_ON_ENTRY)
			{
				/* execute parent callback function */
				ptrCurrentParent->callBackFunct();
				/* compute future parent */
				ptrFutureParent = &stateParentPrimingTreatKidney1[6];
			}
			else if(ptrCurrentState->action == ACTION_ALWAYS)
			{
				ptrCurrentParent->callBackFunct();
			}
			break;

		case PARENT_PRIMING_TREAT_KIDNEY_1_END:
			break;


		case PARENT_TREAT_KIDNEY_1_INIT:
			if(perfusionParam.volumeTreatArt >= 200)
			{
				ptrFutureParent = &stateParentTreatKidney1[3];
			}

			if(ptrCurrentParent->action == ACTION_ON_ENTRY)
			{
				/* execute parent callback function */
				ptrCurrentParent->callBackFunct();
				/* compute future parent */
				ptrFutureParent = &stateParentTreatKidney1[2];
			}
			else if(ptrCurrentState->action == ACTION_ALWAYS)
			{
				ptrCurrentParent->callBackFunct();
			}

			if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_TRUE)
			{
				ptrFutureParent = &stateParentTreatKidney1[5];
			}
			break;

		case PARENT_TREAT_KIDNEY_1_PUMP_ON:
			if(ptrCurrentParent->action == ACTION_ON_ENTRY)
			{
				/* execute parent callback function */
				ptrCurrentParent->callBackFunct();
				/* compute future parent */
				ptrFutureParent = &stateParentTreatKidney1[4];
			}
			else if(ptrCurrentState->action == ACTION_ALWAYS)
			{
				ptrCurrentParent->callBackFunct();
			}

			if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_TRUE)
			{
				ptrFutureParent = &stateParentTreatKidney1[5];
			}
			break;

		case PARENT_TREAT_KIDNEY_1_ALARM:
			if(currentGuard[GUARD_ALARM_ACTIVE].guardValue == GUARD_VALUE_FALSE)
			{
				ptrFutureParent = &stateParentTreatKidney1[1];
			}

			if(ptrCurrentParent->action == ACTION_ON_ENTRY)
			{
				/* execute parent callback function */
				ptrCurrentParent->callBackFunct();
				/* compute future parent */
				ptrFutureParent = &stateParentTreatKidney1[6];
			}
			else if(ptrCurrentState->action == ACTION_ALWAYS)
			{
				ptrCurrentParent->callBackFunct();
			}
			break;

		case PARENT_TREAT_KIDNEY_1_END:
			break;


		default:
			break;
	}


	switch(ptrCurrentChild->child){
		static unsigned char maskGuard;

		case CHILD_PRIMING_ALARM_INIT:

			maskGuard = (currentGuard[GUARD_ALARM_STOP_ALL_ACTUATOR].guardValue == GUARD_VALUE_TRUE) 		|
						((currentGuard[GUARD_ALARM_STOP_ALL].guardValue == GUARD_VALUE_TRUE) << 1)			|
						((currentGuard[GUARD_ALARM_STOP_PERF_PUMP].guardValue == GUARD_VALUE_TRUE) << 2)	|
						((currentGuard[GUARD_ALARM_STOP_PURIF_PUMP].guardValue == GUARD_VALUE_TRUE) << 3)	|
						((currentGuard[GUARD_ALARM_STOP_OXYG_PUMP].guardValue == GUARD_VALUE_TRUE) << 4)	|
						((currentGuard[GUARD_ALARM_STOP_PELTIER].guardValue == GUARD_VALUE_TRUE) << 5);

			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				ptrCurrentChild->callBackFunct();

				ptrFutureChild = &stateChildAlarmPriming[2];
			}
			else if(ptrCurrentChild->action == ACTION_ALWAYS){
				ptrCurrentChild->callBackFunct();
			}
			break;

		case CHILD_PRIMING_ALARM_STOP_PERFUSION:
			if(ptrCurrentChild->action == ACTION_ON_ENTRY)
			{
				ptrCurrentChild->callBackFunct();

				ptrFutureChild = &stateChildAlarmPriming[4];
			}
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
	buttonGUITreatment[buttonId].state = GUI_BUTTON_RELEASED;
}
/**/

/**/
void initSetParamFromGUI(void){
	parameterWordSetFromGUI[PAR_SET_PRIMING_VOL_PERFUSION].value = 0;
	parameterWordSetFromGUI[PAR_SET_PRIMING_VOL_PURIFICATION].value = 0;
	parameterWordSetFromGUI[PAR_SET_PRIMING_VOL_OXYGENATION].value = 0;
	parameterWordSetFromGUI[PAR_SET_OXYGENATOR_FOW].value = 0;
	parameterWordSetFromGUI[PAR_SET_PRESS_ART_TARGET].value = 0;
	parameterWordSetFromGUI[PAR_SET_PRESS_VEN_TARGET].value = 0;
	parameterWordSetFromGUI[PAR_SET_PURIF_FLOW_TARGET].value = 0;

	parameterFloatSetFromGUI[PAR_SET_TEMPERATURE].value = 0;
	parameterFloatSetFromGUI[PAR_SET_PURIF_UF_FLOW_TARGET].value = 0;
}

void setParamWordFromGUI(unsigned char parId, int value){
	parameterWordSetFromGUI[parId].value = value;
}

void setParamFloatFromGUI(unsigned char parId, float value){
	parameterFloatSetFromGUI[parId].value = value;
}

void resetParamWordFromGUI(unsigned char parId){
	parameterWordSetFromGUI[parId].value = 0;
}

void resetParamFloatFromGUI(unsigned char parId){
	parameterFloatSetFromGUI[parId].value = 0;
}

char checkParTypeFromGUI(unsigned char parId){
	switch(parId)
	{
	case PAR_SET_PRIMING_VOL_PERFUSION:
	case PAR_SET_PRIMING_VOL_PURIFICATION:
	case PAR_SET_PRIMING_VOL_OXYGENATION:
	case PAR_SET_OXYGENATOR_FOW:
	case PAR_SET_PRESS_ART_TARGET:
	case PAR_SET_PRESS_VEN_TARGET:
	case PAR_SET_PURIF_FLOW_TARGET:
		return 0x01;
		break;

	case PAR_SET_TEMPERATURE:
	case PAR_SET_PURIF_UF_FLOW_TARGET:
		return 0x03;
		break;

	default:
		return 0;
		break;

	}
}
/**/
