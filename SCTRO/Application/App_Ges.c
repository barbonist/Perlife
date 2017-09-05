/*
 * App_Ges.c
 *
 *  Created on: 13/giu/2016
 *      Author: W15
 */

#include "App_Ges.h"
#include "Global.h"

#include "Pins1.h"
#include "Bit1.h"
#include "BitIoLdd1.h"
#include "Bit2.h"
#include "BitIoLdd2.h"

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


/*********************************/
/*********SATATE LEVEL*************/
/*********************************/

struct machineState stateState[] =
		{
		 {STATE_NULL, PARENT_NULL, CHILD_NULL, ACTION_NULL, &stateParentNull[0], &manageNull},

		 {STATE_ENTRY, PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY, &stateParentEntry[1], &manageStateEntry},
		 {STATE_ENTRY, PARENT_NULL, CHILD_NULL, ACTION_ALWAYS, &stateParentEntry[2], &manageStateEntryAlways},

		 {STATE_IDLE, PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY, &stateParentNull[0], &manageStateIdle},
		 {STATE_IDLE, PARENT_NULL, CHILD_NULL, ACTION_ALWAYS, &stateParentNull[0], &manageStateIdleAlways},

		 {STATE_T1_NO_DISPOSABLE, PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY, &stateParentNull[0], &manageStateT1NoDisp},
		 {STATE_T1_NO_DISPOSABLE, PARENT_NULL, CHILD_NULL, ACTION_ALWAYS, &stateParentNull[0], &manageStateT1NoDispAlways},
		 //{STATE_T1_NO_DISPOSABLE, PARENT_T1_NO_DISP_INIT, CHILD_NULL, ACTION_ON_EXIT, &stateParentT1TNoDisposable[1], &manageNull},

		 {STATE_T1_WITH_DISPOSABLE, PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY, &stateParentNull[0], &manageStateT1WithDisp},
		 {STATE_T1_WITH_DISPOSABLE, PARENT_NULL, CHILD_NULL, ACTION_ALWAYS, &stateParentNull[0], &manageStateT1WithDispAlways},
		 //{STATE_T1_WITH_DISPOSABLE, PARENT_T1_WITH_DISP_INIT, CHILD_NULL, ACTION_ON_EXIT, &stateParentIdle[2], &manageNull},

		 {STATE_PRIMING_TREAT_1, PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY, &stateParentNull[0], &manageStatePrimingTreat1},
		 {STATE_PRIMING_TREAT_1, PARENT_NULL, CHILD_NULL, ACTION_ALWAYS, &stateParentNull[0], &manageStatePrimingTreat1Always},

		 {STATE_PRIMING_TREAT_2, PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY, &stateParentNull[0], &manageStatePrimingTreat2},
		 {STATE_PRIMING_TREAT_2, PARENT_NULL, CHILD_NULL, ACTION_ALWAYS, &stateParentNull[0], &manageStatePrimingTreat2Always},

		 {STATE_TREATMENT_1, PARENT_NULL, CHILD_NULL, ACTION_ON_ENTRY, &stateParentNull[0], &manageStateTreat1},
		 {STATE_TREATMENT_1, PARENT_NULL, CHILD_NULL, ACTION_ALWAYS, &stateParentNull[0], &manageStateTreat1Always},

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
	#ifdef DEBUG_ENABLE
	Bit1_NegVal();
	#endif
}

void manageStateEntryAlways(void)
{
	#ifdef DEBUG_ENABLE
	//Bit2_NegVal();
	#endif
}

/*-----------------------------------------------------------*/
/* This function manages the state idle activity 			 */
/*-----------------------------------------------------------*/
void manageStateIdle(void)
{
	#ifdef DEBUG_ENABLE
	Bit1_NegVal();
	#endif
}

void manageStateIdleAlways(void)
{
	#ifdef DEBUG_ENABLE
	Bit2_NegVal();
	#endif
}

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
/* This function manages the state treatment 1 activity      */
/*-----------------------------------------------------------*/
void manageStateTreat1(void)
{

}

void manageStateTreat1Always(void)
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

/* PARENT LEVEL FUNCTION */

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

/*----------------------------------------------------------------------------*/
/* This function compute the guard to be used for machine state transition    */
/*----------------------------------------------------------------------------*/
void computeMachineStateGuard(void)
{
	/* comopute guard */
	for(int i = 0; i <= GUARD_END_NUMBER; i++)
		{
			if(currentGuard[i].guardEntryValue == GUARD_ENTRY_VALUE_TRUE)
				currentGuard[i].guardValue = GUARD_VALUE_TRUE;
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
			if(ptrCurrentState->action == ACTION_ON_ENTRY)
			{
				/* execute state callback function */
				ptrCurrentState->callBackFunct();
				/* compute future state */
				ptrFutureState = &stateState[2];
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
				ptrCurrentChild->callBackFunct();
			}

			break;

		case STATE_IDLE:

			#ifdef DEBUG_ENABLE
			Bit1_NegVal();
			#endif

			break;

		case STATE_T1_NO_DISPOSABLE:
			break;

		case STATE_T1_WITH_DISPOSABLE:
			break;

		case STATE_PRIMING_TREAT_1:
			break;

		case STATE_PRIMING_TREAT_2:
			break;

		case STATE_TREATMENT_1:
			break;

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















