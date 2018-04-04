/*
 * SWTimer.c
 *
 *  Created on: 03/apr/2018
 *      Author: SB
 */

#include <stdlib.h>
#include <stdbool.h>
#include "stdint.h"
#include "SWTimer.h"


typedef struct{
    uint16_t counter;  // fino a piu di 10 minuti
    uint16_t TimerVal;
    uint8_t DoAct;
    uint8_t Enabled;
    TTimerMode  TimerMode;
    void (*Action)(void);
    int TimerChecksum;
} TSwTimer;

#define MAX_TIMERS 30

TSwTimer Timers[MAX_TIMERS];
int TimersCounter = 0;


uint8_t AddSwTimer( TAction TimerFun, uint16_t Timer10ms, TTimerMode  TimerMode)
{
    uint8_t Index;
    uint8_t ii;
    uint16_t MaxCounter;

    Index = 0xff;
    MaxCounter = Timer10ms-1;
    if(TimersCounter < MAX_TIMERS){
        Timers[TimersCounter].Action = TimerFun;
        Timers[TimersCounter].TimerVal = Timer10ms;
        Timers[TimersCounter].TimerMode = TimerMode;
        if(TimerMode == TM_REPEAT){
            Timers[TimersCounter].Enabled = 1;
        }
        else{
            Timers[TimersCounter].Enabled = 0;
        }
        Index = TimersCounter;
        TimersCounter++;
        for(ii=0; ii<TimersCounter-1; ii++){
            if( Timers[ii].TimerVal == Timer10ms ){
                MaxCounter = (Timers[ii].counter + 1) % Timer10ms;
            }
        }
        // to trigger first time event
        Timers[TimersCounter-1].counter = MaxCounter;
    }
    return Index;
}

void ChangeTimerPeriod( int Index, uint16_t Timer10Ms)
{
    if (Index < TimersCounter){
    	// need mutex
        Timers[Index].counter = 0;
        Timers[Index].TimerVal = Timer10Ms;
    }
}

void StopTimer( int Index )
{
    if (Index < TimersCounter){
    	// need mutex
        Timers[Index].Enabled = 0;
    }
}

void StartTimer( int Index )
{
    if (Index < TimersCounter){

    	// must place a mutex
        Timers[Index].Enabled = 1;
        Timers[Index].counter = 0;
    }
}



void  SwTimerInit(void)
{
	int ii;
	for (ii = 0; ii < MAX_TIMERS; ii++) {
		Timers[ii].Enabled = false;
		Timers[ii].counter = 0;
		Timers[ii].TimerVal = 0;
		Timers[ii].TimerMode == TM_SINGLE;
	}
}


void timerIsr_10ms(void)
{
    int ii;

    for( ii=0 ; ii<TimersCounter; ii++){
        if(Timers[ii].Enabled){
            Timers[ii].counter++;
            if(Timers[ii].counter == Timers[ii].TimerVal){
                Timers[ii].DoAct = 1;
                if(Timers[ii].TimerMode == TM_SINGLE){
                    // single shot
                    Timers[ii].Enabled = 0;
                }
                else{
                    Timers[ii].counter = 0;
                }
            }
        }
    }
}


int TaskIndex = 0;
void RestartTimer0(void);

void ManageSwTimers(void)
{
	int ii;

	for( ii=0 ; ii<TimersCounter; ii++){
		TaskIndex = (TaskIndex + 1)%TimersCounter;
		if(Timers[TaskIndex].DoAct == 1){
			Timers[TaskIndex].DoAct = 0;
			(Timers[TaskIndex].Action)();
			return;
		}

	}
}








