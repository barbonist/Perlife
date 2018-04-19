/*
 * FlexCANWrapper.h
 *
 *  Created on: 18/apr/2018
 *      Author: W5
 */

#ifndef FLEXCANWRAPPER_H_
#define FLEXCANWRAPPER_H_
#include "CAN_C_P.h"

void InitCAN(void);
LDD_TError  SendCAN(uint8_t *txbuff, int txsize, LDD_CAN_TMBIndex ChIndex);

#endif /* FLEXCANWRAPPER_H_ */
