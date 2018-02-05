/*
 * general_func.h
 *
 *  Created on: 05/feb/2018
 *      Author: franco mazzoni
 */

#ifndef APPLICATION_GENERAL_FUNC_H_
#define APPLICATION_GENERAL_FUNC_H_



unsigned char ReadKey1(void);
unsigned char ReadKey2(void);
unsigned char ReadKey3(void);

void GenEvntParentPrim(void);
void GenEvntParentTreat(void);

void DebugStringStr(char *s);
void DebugStringPID();
void DebugString();

unsigned char Bubble_Keyboard_GetVal(unsigned char Button);

void Heater_ON();
void Set_Lamp(unsigned char level);
void Reset_Lamp(unsigned char level);



#endif /* APPLICATION_GENERAL_FUNC_H_ */
