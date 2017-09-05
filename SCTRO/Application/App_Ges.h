/*
 * App_Ges.h
 *
 *  Created on: 13/giu/2016
 *      Author: W15
 */

#ifndef APPLICATION_APP_GES_H_
#define APPLICATION_APP_GES_H_


/* MACHINE STATE FUNCTION */
void manageNull(void);
void manageStateLevel(void);

/* CORE FUNCTION */
void manageCommWithPc(void);
void manageCommWithProtection(void);
void readAnalogSensor(void);
void readDigitalSensor(void);
void computeMachineStateGuard(void);
void processMachineState(void);

/* STATE LEVEL FUNCTION */
void manageStateEntry(void);
void manageStateEntryAlways(void);

void manageStateIdle(void);
void manageStateIdleAlways(void);

void manageStateT1NoDisp(void);
void manageStateT1NoDispAlways(void);

void manageStateT1WithDisp(void);
void manageStateT1WithDispAlways(void);

void manageStatePrimingTreat1(void);
void manageStatePrimingTreat1Always(void);

void manageStateTreat1(void);
void manageStateTreat1Always(void);

void manageStateEmptyDisp1(void);
void manageStateEmptyDisp1Always(void);

void manageStatePrimingTreat2(void);
void manageStatePrimingTreat2Always(void);

void manageStateTreat2(void);
void manageStateTreat2Always(void);

void manageStateEmptyDisp2(void);
void manageStateEmptyDisp2Always(void);

void manageStateWashing(void);
void manageStateWashingAlways(void);

void manageStateFatalError(void);
void manageStateFatalErrorAlways(void);

/* PARENT LEVEL FUNCTION */
void manageParentEntry(void);
void manageParentEntryAlways(void);

/* CHILD LEVEL FUNCTION */
void manageChildEntry(void);
void manageChildEntryAlways(void);

/* NESTED LEVEL FUNCTION */

/* MACHINE STATE FUNCTION */
void initAllState(void);
void initAllGuard(void);

/*******************/
/* VARIABLE */
/******************/


#endif /* APPLICATION_APP_GES_H_ */
