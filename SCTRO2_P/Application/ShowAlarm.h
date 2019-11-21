/*
 * ShowAlarms.h
 *
 *  Created on: 02/mag/2018
 *      Author: W5
 */

#ifndef APPLICATION_SHOWALARM_H_
#define APPLICATION_SHOWALARM_H_


void RetriggerNoCANRxTxAlarm(void);
void ShowNewAlarmError(uint16_t AlarmCode);
void SetBuzzerMode(int val);

#endif /* APPLICATION_SHOWALARM_H_ */
