/*
 * BubbleKeyboard.c
 *
 *  Created on: 03/mag/2018
 *      Author: W5
 */
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "stdint.h"
#include "SwTimer.h"
#include "Global.h"
#include "BUBBLE_KEYBOARD_BUTTON1.h"
#include "BUBBLE_KEYBOARD_BUTTON2.h"
#include "BUBBLE_KEYBOARD_BUTTON3.h"
#include "BUBBLE_KEYBOARD_BUTTON4.h"

void onNewKey(char key);
void ManageBubbleKeyboard20ms(void);

void InitBubbleKeyboard(void)
{
	AddSwTimer(ManageBubbleKeyboard20ms, 2 , TM_REPEAT);
}

void ManageBubbleKeyboard20ms(void)
{
static bool OldBut1 = FALSE ,OldBut2 = FALSE ,OldBut3 = FALSE ,OldBut4 = FALSE;
static bool But1,But2,But3,But4;

	But1 = BUBBLE_KEYBOARD_BUTTON1_GetVal();
	But2 = BUBBLE_KEYBOARD_BUTTON2_GetVal();
	But3 = BUBBLE_KEYBOARD_BUTTON3_GetVal();
	But4 = BUBBLE_KEYBOARD_BUTTON4_GetVal();

	if( But1 && (!OldBut1)) onNewKey('1');
	if( But2 && (!OldBut2)) onNewKey('2');
	if( But3 && (!OldBut3)) onNewKey('3');
	if( But4 && (!OldBut4)) onNewKey('4');

	OldBut1 = But1;
	OldBut2 = But2;
	OldBut3 = But3;
	OldBut4 = But4;
}

