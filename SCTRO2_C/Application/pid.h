/*
 * pid.h
 *
 *  Created on: 09/feb/2018
 *      Author: franco mazzoni
 */

#ifndef PID_H_
#define PID_H_



void alwaysPumpPressLoop(unsigned char pmpId, unsigned char *PidFirstTime);
float CalcolaPresArt(float speed);
int SpeedCostanteArt( int CurrSpeed);
void alwaysPumpPressLoopArt(unsigned char pmpId, unsigned char *PidFirstTime);
void alwaysPumpPressLoop(unsigned char pmpId, unsigned char *PidFirstTime);
int get_Set_Point_Pressure(int Speed);
float CalcolaPresVen(float speed);
float CalcolaPresArt(float speed);
int SpeedCostanteVen( int CurrSpeed);
void alwaysPumpPressLoopVen(unsigned char pmpId, unsigned char *PidFirstTime);

#endif /* PID_H_ */