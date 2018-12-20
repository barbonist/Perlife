/*
 * child_gest.h
 *
 *  Created on: 20/dic/2017
 *      Author: franco mazzoni
 */

#ifndef APPLICATION_CHILD_GEST_H_
#define APPLICATION_CHILD_GEST_H_

void manageChildPrimAlarmStopPerfEntry(void);
void manageChildPrimAlarmStopPerfAlways(void);

void manageChildPrimAlarmStopPurifEntry(void);
void manageChildPrimAlarmStopPurifAlways(void);

void manageChildPrimAlarmStopAllPumpEntry(void);
void manageChildPrimAlarmStopAllPumpAlways(void);

void manageChildPrimAlarmStopPeltEntry(void);
void manageChildPrimAlarmStopPeltAlways(void);

void manageChildPrimAlarmStopAllActEntry(void);
void manageChildPrimAlarmStopAllActAlways(void);

void manageChildPrimAlmAndWaitCmdEntry(void);
void manageChildPrimAlmAndWaitCmdAlways(void);

void manageChildPrimAlmPumpNotStillEntry(void);
void manageChildPrimAlmPumpNotStillAlways(void);

void manageChildPrimAlmBadPinchPosEntry(void);
void manageChildPrimAlmBadPinchPosAlways(void);

void manageChildPrimAlmSFAAirDetEntry(void);
void manageChildPrimAlmSFAAirDetAlways(void);

void manageChildPrimAlmModBusEntry(void);
void manageChildPrimAlmModBusAlways(void);

// Filippo - funzione per gestire l'allarme stop button
void manageChildIdleAlarm(void);
void manageChildIdleAlm1StAllActEntry(void);
void manageChildIdleAlm1StAllActAlways(void);
void manageChildT1Alm1StAllActEntry(void);
void manageChildT1Alm1StAllActAlways(void);


/* --------------------------------------------------------------------------------------------
   CHILD LEVEL FUNCTION FOR TREATMENT_1 (KIDNEY)
 * --------------------------------------------------------------------------------------------*/

void manageChildTreatAlm1InitAlways(void);

void manageChildTreatAlm1StopPerfEntry(void);
void manageChildTreatAlm1StopPerfAlways(void);

void manageChildTreatAlm1StopPurifEntry(void);
void manageChildTreatAlm1StopPurifAlways(void);

void manageChildTreatAlm1StopAllPumpEntry(void);
void manageChildTreatAlm1StopAllPumpAlways(void);

void manageChildTreatAlm1StopPeltEntry(void);
void manageChildTreatAlm1StopPeltAlways(void);

void manageChildTreatAlm1StopAllActEntry(void);
void manageChildTreatAlm1StopAllActAlways(void);

void manageChildTreatAlm1SafAirFiltEntry(void);
void manageChildTreatAlm1SafAirFiltAlways(void);

void manageChildTreatAlm1SFVEntry(void);
void manageChildTreatAlm1SFVAlways(void);

void manageChildTreatAlm1SFAEntry(void);
void manageChildTreatAlm1SFAAlways(void);

void manageChildAlmAndWaitCmdEntry(void);
void manageChildAlmAndWaitCmdAlways(void);

void manageChildTreatAlmBadPinchPosEntry(void);
void manageChildTreatAlmBadPinchPosAlways(void);

void manageChildTreatAlmModBusErrEntry(void);
void manageChildTreatAlmModBusErrAlways(void);

void manageChildTreatAlmDeltaTempHighAlways(void);
void manageChildTreatAlmDeltaTempHighEntry(void);

void ManageStateChildAlarmTreat1(void);

bool PinchWriteTerminated(int idx);
bool IsSecurityStateActive(void);
bool PutPinchInSafetyPos(void);

// Filippo - gestite in modo diverso le funzioni di stop attuatori in T1 test
void manageChildTreatAlmT1StopAllActEntry(void);
void manageChildTreatAlmT1StopAllActAlways(void);

/* --------------------------------------------------------------------------------------------
   CHILD LEVEL FUNCTION FOR EMPTY STATE
 * --------------------------------------------------------------------------------------------*/


typedef struct
{
	unsigned int SAFAirDetected : 1;    // aria rilevata su filtro
	unsigned int SFVAirDetected : 1;    // aria rilevata su circuito venoso
	unsigned int SFAAirDetected : 1;    // aria rilevata su circuito arterioso
}CHILD_EMPTY_FLAGS_DEF;

typedef union
{
	CHILD_EMPTY_FLAGS_DEF FlagsDef;
	unsigned int FlagsVal;
}CHILD_EMPTY_FLAGS;


void manageChildEmptyAlm1InitEntry(void);
void manageChildEmptyAlm1InitAlways(void);
void manageChildEmptyAlm1SafAirFiltEntry(void);
void manageChildEmptyAlm1SafAirFiltAlways(void);
void manageChildEmptyAlm1SFVEntry(void);
void manageChildEmptyAlm1SFVAlways(void);
void manageChildEmptyAlm1SFAEntry(void);
void manageChildEmptyAlm1SFAAlways(void);
bool IsDisposableEmptyWithAlm(void);

void ManageStateChildAlarmEmpty(void);

void manageChildEmptyAlm1StAllActEntry(void);
void manageChildEmptyAlm1StAllActAlways(void);
void manageChildEmptyAlmPumpNotStillEntry(void);
void manageChildEmptyAlmPumpNotStillAlways(void);

// Filippo - funzione che gestisce il passaggio dell'allarme per lo stato idle
void manageStateChildAlarmIdle(void);
void manageStateChildAlarmT1(void);


#endif /* APPLICATION_CHILD_GEST_H_ */
