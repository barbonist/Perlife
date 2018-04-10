/*
 * SWTimer.h
 *
 *  Created on: 03/apr/2018
 *      Author: W5
 */

#ifndef SWTIMER_H_
#define SWTIMER_H_

typedef enum { TM_REPEAT = 1, TM_SINGLE = 2 } TTimerMode;
typedef void(*TAction)(void);// TAction;
void  SwTimerInit(void);
void  ManageSwTimers(void);
void  ChangeTimerPeriod( int Index, uint16_t Timer10Ms);
uint8_t AddSwTimer( TAction TimerFun,  uint16_t Timer10ms, TTimerMode  TimerMode);


#endif /* SWTIMER_H_ */
