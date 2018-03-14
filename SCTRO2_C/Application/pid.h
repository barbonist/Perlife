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
float CalcolaPresVen_with_Speed(float speed);
float CalcolaPresVen_with_Flow(unsigned char type_flow);
float CalcolaPresArt(float speed);
int SpeedCostanteVen( int CurrSpeed);
void alwaysPumpPressLoopVen(unsigned char pmpId, unsigned char *PidFirstTime);

float CalcolaPresArt_with_Flow(unsigned char type_flow);

#endif /* PID_H_ */
