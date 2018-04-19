/*
 * CalcUtility.c
 *
 *  Created on: 13/apr/2018
 *      Author: W5
 */

#include "stdint.h"




#define NUM_LQUID_LEV_VALUES 20

const uint16_t LiquidLevelTable[NUM_LQUID_LEV_VALUES][2] =
{
		// valore letto dal sensore di pressione mmHg, ml presenti nel recipiente
		105, 100,  // 108
		136, 200,  // 139
		153, 300,  // 156
		174, 400,  // 177
		188, 500,  // 191
		200, 600,  // 203
		206, 700,  // 209
		219, 800,  // 221
		223, 900,  // 226
		234, 1000,  // 237
		245, 1200,  // 248
		262, 1400,  // 265
		268, 1600,  // 271
		305, 1800,  // 308
		316, 2000,  // 319
		330, 2200,  // 333
		353, 2400,  // 356
		363, 2600,  // 366
		376, 2800,  // 379
		391, 3000   // 394
};

// converte il valore mmhg moltiplicato per 10 in ml di contenuto nella baccinella
uint16_t ConvertMMHgToMl( uint16_t mmhg)
{
	uint16_t wd = 0;
	int i;
	for(i = 0; i < NUM_LQUID_LEV_VALUES; i++)
	{
		if(LiquidLevelTable[i][0] > mmhg)
		{
			if(i)
			{
				wd = LiquidLevelTable[i - 1][1];
			}
			else
				wd = 0;
			break;
		}

	}
	if(i == NUM_LQUID_LEV_VALUES)
		wd = 3000;
	return wd;
}
