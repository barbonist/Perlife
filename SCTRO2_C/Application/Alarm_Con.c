/*
 * Alarm_Con.c
 *
 *  Created on: 10/nov/2016
 *      Author: W15
 */

#include "Alarm_Con.h"
#include "Global.h"
#include "Flowsens.h"


struct alarm alarmList[] =
{
		//{CODE_ALARM0, PHYSIC_TRUE, TYPE_ALARM_CONTROL, PRIORITY_LOW, OVRD_ENABLE, SILENCE_ALLOWED},
		{CODE_ALARM_PRESS_ART_HIGH, PHYSIC_TRUE, ACTIVE_FLASE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL, PRIORITY_HIGH, 2000, 2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull}, 		/* 0 */
		{CODE_ALARM_PRESS_ART_LOW, PHYSIC_FLASE, ACTIVE_FLASE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL, PRIORITY_HIGH, 2000, 2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull}, 		/* 1 */
		{CODE_ALARM_AIR_PRES_ART, PHYSIC_FLASE, ACTIVE_FLASE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL, PRIORITY_HIGH, 1000, 2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_ALLOWED, &alarmManageNull}, 				/* 2 */
		{CODE_ALARM_TEMP_ART_HIGH, PHYSIC_FLASE, ACTIVE_FLASE, ALARM_TYPE_CONTROL, SECURITY_STOP_PERF_PUMP, PRIORITY_HIGH, 5000, 2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull}, 	/* 3 */
		{CODE_ALARM_PRESS_ADS_FILTER_HIGH, PHYSIC_FLASE, ACTIVE_FLASE, ALARM_TYPE_CONTROL, SECURITY_STOP_ALL, PRIORITY_HIGH, 2000, 2000, OVRD_NOT_ENABLED, RESET_ALLOWED, SILENCE_ALLOWED, MEMO_NOT_ALLOWED, &alarmManageNull},	/* 4 */
		{}																																																						/* 5 */

};

void alarmConInit(void){
	ptrAlarmCurrent = &alarmList[0];
}

void alarmEngineAlways(void){

	//verifica physic pressioni
	manageAlarmPhysicPressSens();

	//verifica physic flow sensor
	manageAlarmPhysicUFlowSens();

	//verifica physic ir temp sens
	manageAlarmPhysicTempSens();

	for(int i=0; i<ALARM_ACTIVE_IN_STRUCT; i++)
	{
		if((alarmList[i].physic == PHYSIC_TRUE) && (alarmList[i].active != ACTIVE_TRUE))
		{
			alarmList[i].prySafetyActionFunc();
			ptrAlarmCurrent = &alarmList[i];
		}
		else if((alarmList[i].active == ACTIVE_TRUE) && (alarmList[i].physic == PHYSIC_FLASE))
		{
			alarmList[i].prySafetyActionFunc();
			ptrAlarmCurrent = &alarmList[i];
		}
	}
}

void manageAlarmPhysicPressSens(void){
	if(sensor_PRx[0].prSensValue > 80)
		{
			alarmList[0].physic = PHYSIC_TRUE;
		}
	else
		{
			alarmList[0].physic = PHYSIC_FLASE;
		}

	if(sensor_PRx[1].prSensValue > 100)
		{
			alarmList[4].physic = PHYSIC_TRUE;
		}
	else
		{
			alarmList[4].physic = PHYSIC_FLASE;
		}
}

void manageAlarmPhysicTempSens(void){
	if((sensorIR_TM[0].tempSensValue > 40.0)){
		alarmList[3].physic = PHYSIC_TRUE;
	}
	else
	{
		alarmList[3].physic = PHYSIC_FLASE;
	}
}

void manageAlarmPhysicUFlowSens(void){
	if(
		(sensor_UFLOW[0].bubbleSize >= 25) ||
		(sensor_UFLOW[0].bubblePresence == MASK_ERROR_BUBBLE_ALARM)
		)
	{
		alarmList[2].physic = PHYSIC_TRUE;
	}
	else
	{
		alarmList[2].physic = PHYSIC_FLASE;
	}
}

void alarmManageNull(void)
{
	static unsigned char dummy = 0;
	static unsigned short elapsedEntryTime = 0;
	static unsigned short elapsedExitTime = 0;

	elapsedEntryTime = elapsedEntryTime + 50;
	elapsedExitTime = elapsedExitTime + 50;
	if((ptrAlarmCurrent->active != ACTIVE_TRUE) && (elapsedEntryTime > ptrAlarmCurrent->entryTime))
	{
		elapsedEntryTime = 0;
		elapsedExitTime = 0;
		ptrAlarmCurrent->active = ACTIVE_TRUE;
		currentGuard[GUARD_ALARM_ACTIVE].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;

		manageAlarmChildGuard(ptrAlarmCurrent);
	}
	else if((ptrAlarmCurrent->active == ACTIVE_TRUE) && (elapsedExitTime > ptrAlarmCurrent->exitTime)){
		elapsedEntryTime = 0;
		elapsedExitTime = 0;
		ptrAlarmCurrent->active = ACTIVE_FLASE;
		currentGuard[GUARD_ALARM_ACTIVE].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;

		manageAlarmChildGuard(ptrAlarmCurrent);
	}


	dummy = dummy + 1;
}

void manageAlarmChildGuard(struct alarm * ptrAlarm){
	struct alarm * myAlarmPointer;

	myAlarmPointer = ptrAlarm;

	//setting child guard depending on alarm security action
	switch(myAlarmPointer->secActType){
	case SECURITY_STOP_ALL_ACTUATOR:
		if(myAlarmPointer->active == ACTIVE_TRUE)
			currentGuard[GUARD_ALARM_STOP_ALL_ACTUATOR].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		else
			currentGuard[GUARD_ALARM_STOP_ALL_ACTUATOR].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
		break;

	case SECURITY_STOP_ALL:
		if(myAlarmPointer->active == ACTIVE_TRUE)
			currentGuard[GUARD_ALARM_STOP_ALL].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		else
			currentGuard[GUARD_ALARM_STOP_ALL].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
		break;

	case SECURITY_STOP_PERF_PUMP:
		if(myAlarmPointer->active == ACTIVE_TRUE)
			currentGuard[GUARD_ALARM_STOP_PERF_PUMP].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		else
			currentGuard[GUARD_ALARM_STOP_PERF_PUMP].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
		break;

	case SECURITY_STOP_PURIF_PUMP:
		if(myAlarmPointer->active == ACTIVE_TRUE)
			currentGuard[GUARD_ALARM_STOP_PURIF_PUMP].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		else
			currentGuard[GUARD_ALARM_STOP_PURIF_PUMP].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
		break;

	case SECURITY_STOP_OXYG_PUMP:
		if(myAlarmPointer->active == ACTIVE_TRUE)
			currentGuard[GUARD_ALARM_STOP_OXYG_PUMP].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		else
			currentGuard[GUARD_ALARM_STOP_OXYG_PUMP].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
		break;

	case SECURITY_STOP_PELTIER:
		if(myAlarmPointer->active == ACTIVE_TRUE)
			currentGuard[GUARD_ALARM_STOP_PELTIER].guardEntryValue = GUARD_ENTRY_VALUE_TRUE;
		else
			currentGuard[GUARD_ALARM_STOP_PELTIER].guardEntryValue = GUARD_ENTRY_VALUE_FALSE;
		break;

	default:
		break;
	}

}
