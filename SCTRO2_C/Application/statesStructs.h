/*
 * statesStructs.h
 *
 *  Created on: 05/feb/2018
 *      Author: franco mazzoni
 */

#ifndef APPLICATION_STATESSTRUCTS_H_
#define APPLICATION_STATESSTRUCTS_H_


extern struct machineChild stateChildNull[];
extern struct machineChild stateChildEntry[];
extern struct machineChild stateChildIdle[];
extern struct machineChild stateChildAlarmTreat1[];
extern struct machineChild stateChildAlarmTreat2[];
extern struct machineChild stateChildAlarmPriming[];

extern struct machineParent stateParentNull[];
extern struct machineParent stateParentEntry[];
extern struct machineParent stateParentIdle[];
extern struct machineParent stateParentT1TNoDisposable[];
extern struct machineParent stateParentT1TWithDisposable[];
extern struct machineParent stateParentPrimingTreatKidney1[];
extern struct machineParent stateParentTreatKidney1[];
extern struct machineParent stateParentEmptyDisp[];
extern struct machineState stateState[];
extern struct machineChild stateChildAlarmIdle[];
extern struct machineChild stateChildAlarmT1[];

#endif /* APPLICATION_STATESSTRUCTS_H_ */
