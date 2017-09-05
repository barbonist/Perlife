/*
 * Adc_Ges.c
 *
 *  Created on: 12/lug/2016
 *      Author: W15
 */


#include "Adc_Ges.h"
#include "AD1.h"
#include "AdcLdd1.h"
#include "PE_Types.h"

byte adcRes = 0xFF;
word adcValue;
word * adcValPtr;

word * ReadAdcPr1(void)
{
	adcRes = AD1_MeasureChan(1, ADC_CHANNEL_PR1);

	if(adcRes == ERR_OK)
		adcRes = AD1_GetChanValue16(ADC_CHANNEL_PR1, &adcValue);

	adcValPtr = &adcValue;
	/* valuatre se introdurre un buffer circolare per mediare i campioni */
	return adcValPtr;
}

word * ReadAdcPr2(void)
{
	adcRes = AD1_MeasureChan(1, ADC_CHANNEL_PR2);

	if(adcRes == ERR_OK)
		adcRes = AD1_GetChanValue16(ADC_CHANNEL_PR2, &adcValue);

	adcValPtr = &adcValue;
	/* valuatre se introdurre un buffer circolare per mediare i campioni */
	return adcValPtr;
}

word * ReadAdcTm1(void)
{
	adcRes = AD1_MeasureChan(1, ADC_CHANNEL_TM1);

	if(adcRes == ERR_OK)
		adcRes = AD1_GetChanValue16(ADC_CHANNEL_TM1, &adcValue);

	adcValPtr = &adcValue;
	/* valuatre se introdurre un buffer circolare per mediare i campioni */
	return adcValPtr;
}

word * ReadAdcTm2(void)
{
	adcRes = AD1_MeasureChan(1, ADC_CHANNEL_TM2);

	if(adcRes == ERR_OK)
		adcRes = AD1_GetChanValue16(ADC_CHANNEL_TM2, &adcValue);

	adcValPtr = &adcValue;
	/* valuatre se introdurre un buffer circolare per mediare i campioni */
	return adcValPtr;
}
