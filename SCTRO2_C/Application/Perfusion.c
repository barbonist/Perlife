/*
 * Perfusion.c
 *
 *  Created on: 11/nov/2016
 *      Author: W15
 */

#include "PE_Types.h"
#include "Global.h"
#include "Perfusion.h"



void initPerfusionParam(void){
	//perfusionParam.diastolicPress = 0xABCD;
	//perfusionParam.systolicPress = 0xFAFC;
	//perfusionParam.meanPress = 0xAFCF;
	sensorsValues.pressSystArt = 0xFAFC;
	sensorsValues.pressDiasArt = 0xABCD;
	sensorsValues.pressMeanArt = 0xAFCF;

}
