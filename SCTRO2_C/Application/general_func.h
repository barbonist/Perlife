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
unsigned char ReadKey4(void);

//void GenEvntParentPrim(void);
//void GenEvntParentTreat(void);

void DebugStringStr(char *s);
void DebugStringPID();
void DebugString();

unsigned char Bubble_Keyboard_GetVal(unsigned char Button);

void Heater_ON();
void Set_Lamp(unsigned char level);
void Reset_Lamp(unsigned char level);

void updateDurationToSendSBC(void);

void ShowButtonStr(struct buttonGUI *ButtonGuiArray, unsigned char id);
void ShowParameterStr(struct parWordSetFromGUI *ParamGuiArray, unsigned char id);

/*funzione che controlla se è stato inviato lo stop ad una pompa ma la pompa
 * sta ancora girando, nel qual caso reinotlra il comando con velocità pari a '0'*/
void CheckStopPump(void);

void updateMaxTempPlate(void);
void updateMinTempPlate(void);

#endif /* APPLICATION_GENERAL_FUNC_H_ */
