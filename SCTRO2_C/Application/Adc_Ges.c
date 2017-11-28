/*
 * Adc_Ges.c
 *
 *  Created on: 03/nov/2016
 *      Author: W15
 */

#include "Global.h"
#include "Adc_Ges.h"
#include "AD0.h"
#include "AD1.h"
#include "AdcLdd1.h"
#include "PE_Types.h"

#include "PC_DEBUG_COMM.h"
#include "ASerialLdd2.h"

#include "string.h"
#include "stdio.h"
#include "stdlib.h"

byte adcRes = 0xFF;
word adcValue;
word adcValueDummy;
word * adcValPtr;

void ADC0_Init(void)
{
	int i;
	bool ADC_CALIB_OK = TRUE;

	/*calibriamo il componente AD1 rispetto alla VREF*/
	for (int i=0; i<50; i++)
	{
		if (AD0_Calibrate(ADC_CALIB_OK) == ERR_OK)
			break;
	}

	/*faccio lo start per attivare la conversione*/
	AD0_Start();
}

void ADC1_Init(void)
{
	int i;
	bool ADC_CALIB_OK = TRUE;

	/*calibriamo il componente AD1 rispetto alla VREF*/
	for (int i=0; i<50; i++)
	{
		if (AD1_Calibrate(ADC_CALIB_OK) == ERR_OK)
			break;
	}

	/*faccio lo start per attivare la conversione*/
	AD1_Start();
}
word * ReadAdcPr1(void)
{
	adcRes = AD1_MeasureChan(1, ADC_CHANNEL_PR1);

	if(adcRes == ERR_OK)
		adcRes = AD1_GetChanValue16(ADC_CHANNEL_PR1, &adcValue);

	if(adcRes == ERR_OK)
		adcValPtr = &adcValue;

	adcValueDummy = adcValue;
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

word * ReadAdcPr3(void)
{
	adcRes = AD1_MeasureChan(1, ADC_CHANNEL_PR3);

	if(adcRes == ERR_OK)
		adcRes = AD1_GetChanValue16(ADC_CHANNEL_PR3, &adcValue);

	adcValPtr = &adcValue;
	/* valuatre se introdurre un buffer circolare per mediare i campioni */
	return adcValPtr;
}

/*word * ReadAdcTm1(void)
{
	adcRes = AD1_MeasureChan(1, ADC_CHANNEL_TM1);

	if(adcRes == ERR_OK)
		adcRes = AD1_GetChanValue16(ADC_CHANNEL_TM1, &adcValue);

	adcValPtr = &adcValue;

	return adcValPtr;
}*/

word * ReadAdcTm2(void)
{
	adcRes = AD1_MeasureChan(1, ADC_CHANNEL_TM2);

	if(adcRes == ERR_OK)
		adcRes = AD1_GetChanValue16(ADC_CHANNEL_TM2, &adcValue);

	adcValPtr = &adcValue;
	/* valuatre se introdurre un buffer circolare per mediare i campioni */
	return adcValPtr;
}

void initAdcParam(void)
{
	iflag_read_press_sensor = IFLAG_IDLE;
	timerCounterADC = 0;

	sensor_PRx[0].prSensGain = 1;
	sensor_PRx[0].prSensOffset = 0;
	sensor_PRx[0].prSensOffsetVal = 0;
	sensor_PRx[0].prSensValue = 0;
	sensor_PRx[0].prSensValueFilteredWA = 0;
	sensor_PRx[0].readAdctPtr = ReadAdcPr1;

	sensor_PRx[1].prSensGain = 1;
	sensor_PRx[1].prSensOffset = 0;
	sensor_PRx[1].prSensOffsetVal = 0;
	sensor_PRx[1].prSensValue = 0;
	sensor_PRx[1].prSensValueFilteredWA = 0;
	sensor_PRx[1].readAdctPtr = ReadAdcPr2;

	sensor_PRx[2].prSensGain = 1;
	sensor_PRx[2].prSensOffset = 0;
	sensor_PRx[2].prSensOffsetVal = 0;
	sensor_PRx[2].prSensValue = 0;
	sensor_PRx[2].prSensValueFilteredWA = 0;
	sensor_PRx[2].readAdctPtr = ReadAdcPr3;

	/*iflag_write_temp_sensor = IFLAG_IDLE;
	sensor_TMx[0].tempSensGain = 0;
	sensor_TMx[0].tempSensOffset = 0;
	sensor_TMx[0].tempSensOffsetVal = 0;
	sensor_TMx[0].tempSensValue = 0;
	sensor_TMx[0].readAdctPtr = ReadAdcTm1;*/

	sensor_TMx[1].tempSensGain = 0;
	sensor_TMx[1].tempSensOffset = 0;
	sensor_TMx[1].tempSensOffsetVal = 0;
	sensor_TMx[1].tempSensValue = 0;
	sensor_TMx[1].readAdctPtr = ReadAdcTm2;

	/***********************/
	sensor_PRx[0].prSensAdcPtr = sensor_PRx[0].readAdctPtr();
	sensor_PRx[0].prSensAdc = *sensor_PRx[0].prSensAdcPtr;
	sensor_PRx[0].prSensValue = sensor_PRx[0].prSensGain *
								(((float)((float)sensor_PRx[0].prSensAdc/65535))*3.3 - sensor_PRx[0].prSensOffsetVal) +
								sensor_PRx[0].prSensOffset;
	sensor_PRx[0].prSensOffset = - sensor_PRx[0].prSensValue * ((float)(1000/2.04));


	sensor_PRx[1].prSensAdcPtr = sensor_PRx[1].readAdctPtr();
	sensor_PRx[1].prSensAdc = *sensor_PRx[1].prSensAdcPtr;
	sensor_PRx[1].prSensValue = sensor_PRx[1].prSensGain *
								(((float)((float)sensor_PRx[1].prSensAdc/65535))*3.3 - sensor_PRx[1].prSensOffsetVal) +
								sensor_PRx[1].prSensOffset;
	sensor_PRx[1].prSensOffset = - sensor_PRx[1].prSensValue * ((float)(1000/2.04));


	sensor_PRx[2].prSensAdcPtr = sensor_PRx[2].readAdctPtr();
	sensor_PRx[2].prSensAdc = *sensor_PRx[2].prSensAdcPtr;
	sensor_PRx[2].prSensValue = sensor_PRx[2].prSensGain *
								(((float)((float)sensor_PRx[2].prSensAdc/65535))*3.3 - sensor_PRx[2].prSensOffsetVal) +
								sensor_PRx[2].prSensOffset;
	sensor_PRx[2].prSensOffset = - sensor_PRx[2].prSensValue * ((float)(1000/2.04));
	/***********************/

}

void 	alwaysAdcParam(void){
	int valPressure = 0;
	int dummy = 0;
	unsigned char * ptrChar;
	static unsigned long timems = 0;

	static char stringPr1[20];

	if(timerCounterADC >= 1)
	{
	iflag_read_press_sensor = IFLAG_READ_PR_SENSOR;
	timerCounterADC = 0;
	timems = timems + 50;

	sensor_PRx[0].prSensAdcPtr = sensor_PRx[0].readAdctPtr();
	sensor_PRx[0].prSensAdc = *sensor_PRx[0].prSensAdcPtr;
	sensor_PRx[0].prSensValue = sensor_PRx[0].prSensGain *
								(((float)sensor_PRx[0].prSensAdc/65535)*1617 - sensor_PRx[0].prSensOffsetVal) +
								sensor_PRx[0].prSensOffset;

	sensor_PRx[0].prSensValueFilteredWA = meanWA(8, (sensor_PRx[0].prSensValue));
	dummy = dummy + 1;

	#ifdef DEBUG_LOG_PC
	sprintf(stringPr1, "%d; %i; %i;\r", timems, (int)(sensor_UFLOW[0].volumeMlTot), (sensor_PRx[0].prSensValueFilteredWA));
	for(int i=0; i<20; i++)
	{
		PC_DEBUG_COMM_SendChar(stringPr1[i]);
	}
	PC_DEBUG_COMM_SendChar(0x0A);
	#endif

	sensor_PRx[1].prSensAdcPtr = sensor_PRx[1].readAdctPtr();
	sensor_PRx[1].prSensAdc = *sensor_PRx[1].prSensAdcPtr;
	sensor_PRx[1].prSensValue = sensor_PRx[1].prSensGain *
								(((float)sensor_PRx[1].prSensAdc/65535)*1617 - sensor_PRx[1].prSensOffsetVal) +
								sensor_PRx[1].prSensOffset;

	sensor_PRx[2].prSensAdcPtr = sensor_PRx[2].readAdctPtr();
	sensor_PRx[2].prSensAdc = *sensor_PRx[2].prSensAdcPtr;
	sensor_PRx[2].prSensValue = sensor_PRx[2].prSensGain *
								(((float)sensor_PRx[2].prSensAdc/65535)*1617 - sensor_PRx[2].prSensOffsetVal) +
								sensor_PRx[2].prSensOffset;
	}
}

int meanWA(unsigned char dimNum, int newSensVal){
	static int circularBuffer[64];
	static int circBuffAdd[64];
	int numSumValue = 0;
	int denValue=0;
	int numTotal=0;

	if(dimNum <= 64){
	for(int i=(dimNum-1); i>0; i--)
	{
		denValue = denValue + i;

		circularBuffer[i] = circularBuffer[i-1];
		circBuffAdd[i] = circularBuffer[i]*(dimNum-i);
		numSumValue = numSumValue + circBuffAdd[i];

	}
	circularBuffer[0] = newSensVal;
	numSumValue = numSumValue + (circularBuffer[0]*dimNum);
	denValue = denValue + dimNum;
	numTotal = (numSumValue/denValue);

	return numTotal;
	}
	else
		return 0;
}
