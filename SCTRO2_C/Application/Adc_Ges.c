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
#include "AIR_SENSOR.h"

#include "PC_DEBUG_COMM.h"
#include "ASerialLdd2.h"

#include "string.h"
#include "stdio.h"
#include "stdlib.h"

byte adcRes = 0xFF;
word adcValue;
word adcValueDummy;
word * adcValPtr;

void PR_Sens_ADC_Init()
{
	PR_ADS_FLT_ADC  = 0;		//variabile globale per il valore ADC del sensore di pressione del filtro assorbente --> PTB11
	PR_ADS_FLT_mmHg = 0;		//variabile globale per il valore in mmHg del sensore di pressione del filtro assorbente
	PR_ART_ADC 		= 0;		//variabile globale per il valore ADC del sensore di pressione arteriosa --> PTB7
	PR_ART_mmHg		= 0;		//variabile globale per il valore in mmHg del sensore di pressione arteriosa
	PR_VEN_ADC		= 0;		//variabile globale per il valore ADC del sensore di pressione Venoso --> PTB6
	PR_VEN_mmHg		= 0;		//variabile globale per il valore in mmHg del sensore di pressione Venoso
	PR_OXYG_ADC		= 0;		//variabile globale per il valore ADC del sensore di pressione ossigenatore --> PTC10
	PR_OXYG_mmHg	= 0;		//variabile globale per il valore in mmHg del sensore di pressione ossigenatore
	PR_LEVEL_ADC	= 0;		//variabile globale per il valore ADC del sensore di pressione di livello vaschetta --> PTC11
	PR_LEVEL_mmHg	= 0;		//variabile globale per il valore in mmHg del sensore di pressione di livello vaschetta
}

void Dip_Switch_ADC_Init(void)
{
	/*inizializzo le variabili globali in cui saranno
	 * presenti i valori ADC dei 3 DIP SWITCH*/
	DipSwitch_0_ADC = 0;
	DipSwitch_1_ADC = 0;
	DipSwitch_2_ADC = 0;
}

void Voltage_Peltier_ADC_Init(void)
{
	V24_P1_CHK_ADC  = 0;
	V24_P2_CHK_ADC  = 0;
	V24_P1_CHK_VOLT = 0;
	V24_P2_CHK_VOLT = 0;
}

void Manange_ADC0(void)
{
	/*dentro l'if seguente posso mettere
	 * tutte le letture del canale ADC0
	 * usando delle variabili globali
	 * che saranno sempre aggiornate*/
	if (END_ADC0)
  	  {
		/*DP_SW0 sta su AD0 channel 4; passando l'indirizzo della variabile, la valorizzo*/
		AD0_GetChanValue16(DipSwitch_0_ADC_CHANNEL, &DipSwitch_0_ADC);
		/*DP_SW1 sta su AD0 channel 5; passando l'indirizzo della variabile, la valorizzo*/
		AD0_GetChanValue16(DipSwitch_1_ADC_CHANNEL, &DipSwitch_1_ADC);

		/*resetto il flag di lettura sull'interrupt AD0_OnEnd*/
		END_ADC0 = FALSE;
  	  }
}

void Manange_ADC1(void)
{
	/*dentro l'if seguente posso mettere
	 * tutte le letture del canale ADC1
	 * usando delle variabili globali
	 * che saranno sempre aggiornate*/
	if (END_ADC1)
  	  {
		/*Peltier Voltage 2 sta su AD1 channel 10; passando l'indirizzo della variabile, la valorizzo*/
		AD1_GetChanValue16(V24_P2_CHK_ADC_CHANNEL, &V24_P2_CHK_ADC);
		/*Peltier Voltage 1 sta su AD1 channel 11; passando l'indirizzo della variabile, la valorizzo*/
		AD1_GetChanValue16(V24_P1_CHK_ADC_CHANNEL, &V24_P1_CHK_ADC);
		/*DP_SW2 sta su AD1 channel 13; passando l'indirizzo della variabile, la valorizzo*/
		AD1_GetChanValue16(DipSwitch_2_ADC_CHANNEL, &DipSwitch_2_ADC);

		/*PR_OXYG sta su AD1 channel 0; passando l'indirizzo della variabile, la valorizzo*/
		AD1_GetChanValue16(PR_OXYG_ADC_CHANNEL, &PR_OXYG_ADC);
		/*PR_LEVEL sta su AD1 channel 1; passando l'indirizzo della variabile, la valorizzo*/
		AD1_GetChanValue16(PR_LEVEL_ADC_CHANNEL, &PR_LEVEL_ADC);
		/*PR_ADS_FLT sta su AD1 channel 2; passando l'indirizzo della variabile, la valorizzo*/
		AD1_GetChanValue16(PR_ADS_FLT_ADC_CHANNEL, &PR_ADS_FLT_ADC);
		/*PR_VEN sta su AD1 channel 4; passando l'indirizzo della variabile, la valorizzo*/
		AD1_GetChanValue16(PR_VEN_ADC_CHANNEL, &PR_VEN_ADC);
		/*PR_ART sta su AD1 channel 5; passando l'indirizzo della variabile, la valorizzo*/
		AD1_GetChanValue16(PR_ART_ADC_CHANNEL, &PR_ART_ADC);

		/*resetto il flag di lettura sull'interrupt AD1_OnEnd*/
		END_ADC1 = FALSE;
  	  }
}

void Coversion_From_ADC_To_mmHg_Pressure_Sensor()
{
	PR_OXYG_mmHg 	= (PR_OXYG_ADC    - PR_OXYG_OFFSET)    * PR_OXYG_GAIN;
	PR_LEVEL_mmHg 	= (PR_LEVEL_ADC   - PR_LEVEL_OFFSET)   * PR_LEVEL_GAIN;
	PR_ADS_FLT_mmHg = (PR_ADS_FLT_ADC - PR_ADS_FLT_OFFSET) * PR_ADS_FLT_GAIN;
	PR_VEN_mmHg 	= (PR_VEN_ADC     - PR_VEN_OFFSET)     * PR_VEN_GAIN;
	PR_ART_mmHg 	= (PR_ART_ADC     - PR_ART_OFFSET)     * PR_ART_GAIN;
}

void Coversion_From_ADC_To_Voltage()
{
	V24_P1_CHK_VOLT	= (V24_P1_CHK_ADC * V24_P1_CHK_GAIN + V24_P1_CHK_OFFSET);
	V24_P2_CHK_VOLT	= (V24_P2_CHK_ADC * V24_P2_CHK_GAIN + V24_P2_CHK_OFFSET);
}

void ADC0_Calibration(void)
{
	int i;
	bool ADC_CALIB_OK = TRUE;

	/*calibriamo il componente AD1 rispetto alla VREF*/
	for (i=0; i<50; i++)
	{
		if (AD0_Calibrate(ADC_CALIB_OK) == ERR_OK)
			break;
	}
	END_ADC0 = FALSE;
}

void ADC1_Calibration(void)
{
	int i;
	bool ADC_CALIB_OK = TRUE;

	/*calibriamo il componente AD1 rispetto alla VREF*/
	for (i=0; i<50; i++)
	{
		if (AD1_Calibrate(ADC_CALIB_OK) == ERR_OK)
			break;
	}
	END_ADC1 = FALSE;
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

void 	alwaysAdcParam(void)
{

	unsigned char * ptrChar;
	static unsigned long timems = 0;

	static char stringPr1[STR_DBG_LENGHT];

	if(timerCounterADC >= 1)
	{
	iflag_read_press_sensor = IFLAG_READ_PR_SENSOR;
	timerCounterADC = 0;
	timems = timems + 50;

	sensor_PRx[0].prSensAdcPtr = sensor_PRx[0].readAdctPtr();
	sensor_PRx[0].prSensAdc = *sensor_PRx[0].prSensAdcPtr;
	//sensor_PRx[0].prSensAdc = PR_ART_ADC;
	sensor_PRx[0].prSensValue = sensor_PRx[0].prSensGain *
								(((float)sensor_PRx[0].prSensAdc/65535)*1617 - sensor_PRx[0].prSensOffsetVal) +
								sensor_PRx[0].prSensOffset;

	sensor_PRx[0].prSensValueFilteredWA = meanWA(8, (sensor_PRx[0].prSensValue),0);

	sensor_PRx[1].prSensAdcPtr = sensor_PRx[1].readAdctPtr();
	sensor_PRx[1].prSensAdc = *sensor_PRx[1].prSensAdcPtr;
	sensor_PRx[1].prSensValue = sensor_PRx[1].prSensGain *
								(((float)sensor_PRx[1].prSensAdc/65535)*1617 - sensor_PRx[1].prSensOffsetVal) +
								sensor_PRx[1].prSensOffset;

	sensor_PRx[1].prSensValueFilteredWA = meanWA(8, (sensor_PRx[1].prSensValue),1);


	//#ifdef DEBUG_LOG_PC
//	sprintf(stringPr1, "\r %i %i; %i; %i; %i; %i; %i; %i; %i; %d;",
//			           /* timems,*/
	//					V24_P1_CHK_VOLT/*PR_OXYG_mmHg*//*(int) sensor_UFLOW[0].Average_Flow_Val*/,
	//						V24_P2_CHK_VOLT/*PR_LEVEL_mmHg*//*(int) sensor_UFLOW[0].Inst_Flow_Value*/,
	//						PR_ADS_FLT_mmHg/*(int) sensor_UFLOW[1].Average_Flow_Val*//*(int) Air_1_Status*//*(int) (sensor_PRx[1].prSensValueFilteredWA)*/,
	//						PR_VEN_mmHg/*(int) sensor_UFLOW[1].Inst_Flow_Value*//*(int) (sensor_UFLOW[0].volumeMlTot)*//*PR_VEN_mmHg*/,
	//						PR_ART_mmHg/*(int) (sensorIR_TM[0].tempSensValue*10)*/,
	//						(int) (sensorIR_TM[0].tempSensValue*10),
	//						(int) (sensorIR_TM[1].tempSensValue*10),
	//						(int) (sensorIR_TM[2].tempSensValue*10),
	//						(int) (sensor_PRx[0].prSensValueFilteredWA),
	//						(int) pumpPerist[0].actualSpeed
	//			);

//		sprintf(stringPr1, "\r %i; %i; %i; %i; %i; %i; %i; %d;",
//				           /* timems,*/
//								PR_ADS_FLT_mmHg/*(int) sensor_UFLOW[1].Average_Flow_Val*//*(int) Air_1_Status*//*(int) (sensor_PRx[1].prSensValueFilteredWA)*/,
//								PR_VEN_mmHg/*(int) sensor_UFLOW[1].Inst_Flow_Value*//*(int) (sensor_UFLOW[0].volumeMlTot)*//*PR_VEN_mmHg*/,
//								PR_ART_mmHg/*(int) (sensorIR_TM[0].tempSensValue*10)*/,
//								(int) (sensorIR_TM[0].tempSensValue*10),
//								(int) (sensorIR_TM[1].tempSensValue*10),
//								(int) (sensorIR_TM[2].tempSensValue*10),
//								(int) (sensor_PRx[0].prSensValueFilteredWA),
//								(int) pumpPerist[0].actualSpeed
//					);


//	sprintf(stringPr1, "\r%d; %i; %i; %i; %i; %i;",
//			            timems,
//						(int) (sensorIR_TM[0].errorNACK),
//						(int) (sensorIR_TM[1].errorNACK),
//						(int) (sensorIR_TM[2].errorNACK),
//						(int) (sensorIR_TM[0].errorPEC),
//						(int) (sensorIR_TM[1].errorPEC),
//						(int) (sensorIR_TM[2].errorPEC)
//			);
//	for(int i=0; i<STR_DBG_LENGHT; i++)
//	{
//		PC_DEBUG_COMM_SendChar(stringPr1[i]);
//	}
//	PC_DEBUG_COMM_SendChar(0x0A);
//	#endif

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

int meanWA(unsigned char dimNum, int newSensVal, char IdSens){
	//fatto così faccio la media fra tutti i sensori contemporaneamente in quanto non specifico
	//su quale sensore fare la media
	/*
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
	*/

	//rifaccio lo stesso codice ma provando a passare per paramento l'id del sensore

	static int circularBuffer[4] [64]; //uso una matrice di 4 array, uno per ogni sensore
	static int circBuffAdd[4] [64];    //uso una matrice di 4 array, uno per ogni sensore
	int numSumValue = 0;
	int denValue=0;
	int numTotal=0;

	if(dimNum <= 64){
	for(int i=(dimNum-1); i>0; i--)
	{
		denValue = denValue + i;

		circularBuffer[IdSens] [i] = circularBuffer [IdSens] [i-1];
		circBuffAdd [IdSens] [i] = circularBuffer[IdSens] [i]*(dimNum-i);
		numSumValue = numSumValue + circBuffAdd [IdSens] [i];

	}
	circularBuffer[IdSens] [0] = newSensVal;
	numSumValue = numSumValue + (circularBuffer [IdSens] [0]*dimNum);
	denValue = denValue + dimNum;
	numTotal = (numSumValue/denValue);

	return numTotal;
	}
	else
		return 0;
}

void Manage_Air_Sensor_1(void)
{
 	if (AIR_SENSOR_GetVal())
 		Air_1_Status = AIR;
 	else
 		Air_1_Status = LIQUID;
}
