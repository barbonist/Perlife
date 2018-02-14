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


void ManageStateChildAlarmTreat1(void);

bool PinchWriteTerminated(int idx);
bool IsSecurityStateActive(void);

#endif /* APPLICATION_CHILD_GEST_H_ */
