/*
 * Purificat.c
 *
 *  Created on: 11/nov/2016
 *      Author: W15
 */

#include "Purificat.h"
#include "Global.h"

void initPurifParam(void){
	purificatParam.flowAbsorbFilt = 0x0000;
	purificatParam.flowFractFilt = 0x0000;
	purificatParam.flowPlasmaFilt = 0x0000;
	purificatParam.pressAbsorbFilt = 0xFAFC;
	purificatParam.pressFractFilt = 0x0000;
	purificatParam.pressPlasmaFilt = 0x0000;
	purificatParam.volPrimingAbsorbFilt = 0x0000;
	purificatParam.volPrimingFractFilt = 0x0000;
	purificatParam.volPrimingPlasmaFilt = 0x0000;
	purificatParam.volTreatAbsorbFilt = 0x0000;
	purificatParam.volTreatFractFilt = 0x0000;
	purificatParam.volTreatPlasmaFilt = 0x0000;
	purificatParam.volTreatWashFilt = 0x0000;

}
