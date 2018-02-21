/*
 * Adc_Ges.c
 *
 *  Created on: 03/nov/2016
 *      Author: W15
 */


#include "PE_Types.h"
#include "Global.h"
#include "Adc_Ges.h"
#include "AD0.h"
#include "AD1.h"
#include "AdcLdd1.h"

#include "AIR_SENSOR.h"

#include "PC_DEBUG_COMM.h"
#include "ASerialLdd2.h"

#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "EEPROM.h"
#include "App_Ges.h"
#include "ModBusCommProt.h"
#include "Comm_Sbc.h"

byte adcRes = 0xFF;
word adcValue;
word adcValueDummy;
word * adcValPtr;

word MedForVenousPid = 0;
word MedForArteriousPid = 0;


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

		/*converte i valori ADC in mmHg dei sensori di pressione*/
		Coversion_From_ADC_To_mmHg_Pressure_Sensor();
		/*filtra i valori di mmHg dei sensori di pressione*/
		Pressure_sensor_Fltered();
		/*converte i valori ADC in volt per le tensioni*/
		Coversion_From_ADC_To_Voltage();

		/*resetto il flag di lettura sull'interrupt AD1_OnEnd*/
		END_ADC1 = FALSE;
  	  }
}


void CalcVenSistDiastPress(word Press)
{
  static word CircPressArr[NUMB_OF_SAMPLES_VEN];
  static int CircPressArrIdx = 0;
  static unsigned char BufferFull = 0;
  word min = 0xffff;
  word max = 0;

  CircPressArr[CircPressArrIdx] = Press;
  CircPressArrIdx++;

  if(CircPressArrIdx == NUMB_OF_SAMPLES_VEN)
  {
	  CircPressArrIdx = 0;
	  BufferFull = 1;
  }
  if(BufferFull)
  {
	  MedForVenousPid = 0;
	  for(int i = 0; i < NUMB_OF_SAMPLES_VEN; i++)
	  {
		  MedForVenousPid += CircPressArr[i];
		  if(CircPressArr[i] < min)
			  min = CircPressArr[i];
		  if(CircPressArr[i] > max)
			  max = CircPressArr[i];
	  }
	  MedForVenousPid = MedForVenousPid / NUMB_OF_SAMPLES_VEN;
  }
  else
  {
	  MedForVenousPid = 0;
	  for(int i = 0; i < CircPressArrIdx; i++)
	  {
		  MedForVenousPid += CircPressArr[i];
		  if(CircPressArr[i] < min)
			  min = CircPressArr[i];
		  if(CircPressArr[i] > max)
			  max = CircPressArr[i];
	  }
	  MedForVenousPid = MedForVenousPid / CircPressArrIdx;
  }
	PR_VEN_Diastolyc_mmHg = min;
	PR_VEN_Sistolyc_mmHg  = max;
	PR_VEN_Med_mmHg = (int) ( 2 * PR_VEN_Sistolyc_mmHg + PR_VEN_Diastolyc_mmHg)/3;
}

void CalcArtSistDiastPress(word Press)
{
  static word CircPressArr[NUMB_OF_SAMPLES_VEN];
  static int CircPressArrIdx = 0;
  static unsigned char BufferFull = 0;
  word min = 0xffff;
  word max = 0;

  CircPressArr[CircPressArrIdx] = Press;
  CircPressArrIdx++;

  if(CircPressArrIdx == NUMB_OF_SAMPLES_ART)
  {
	  CircPressArrIdx = 0;
	  BufferFull = 1;
  }
  if(BufferFull)
  {
	  MedForArteriousPid = 0;
	  for(int i = 0; i < NUMB_OF_SAMPLES_ART; i++)
	  {
		  MedForArteriousPid += CircPressArr[i];
		  if(CircPressArr[i] < min)
			  min = CircPressArr[i];
		  if(CircPressArr[i] > max)
			  max = CircPressArr[i];
	  }
	  MedForArteriousPid = MedForArteriousPid / NUMB_OF_SAMPLES_ART;
  }
  else
  {
	  MedForArteriousPid = 0;
	  for(int i = 0; i < CircPressArrIdx; i++)
	  {
		  MedForArteriousPid += CircPressArr[i];
		  if(CircPressArr[i] < min)
			  min = CircPressArr[i];
		  if(CircPressArr[i] > max)
			  max = CircPressArr[i];
	  }
	  MedForArteriousPid = MedForArteriousPid / CircPressArrIdx;
  }
	PR_ART_Diastolyc_mmHg = min;
	PR_ART_Sistolyc_mmHg  = max;
	PR_ART_Med_mmHg = (int) ( 2 * PR_ART_Sistolyc_mmHg + PR_ART_Diastolyc_mmHg)/3;
}


void Coversion_From_ADC_To_mmHg_Pressure_Sensor()
{
	static int Number_Sample = 0;
	static int Pr_Sist_art = 0, Pr_Dia_art = 0, Pr_Sist_ven = 0, Pr_Dia_ven = 0;

	/* TODO deve essere modificata secondo l'equazione y=mx+q; una volta modificata i sensori vanno tutti ricalibraticome segue*/
	PR_OXYG_mmHg 	= config_data.sensor_PRx[OXYG].prSensGain    * PR_OXYG_ADC    + config_data.sensor_PRx[OXYG].prSensOffset;
	PR_LEVEL_mmHg 	= (word)((config_data.sensor_PRx[LEVEL].prSensGain   * PR_LEVEL_ADC   + config_data.sensor_PRx[LEVEL].prSensOffset) * 100.0 + 0.5);
	PR_ADS_FLT_mmHg = config_data.sensor_PRx[ADS_FLT].prSensGain * PR_ADS_FLT_ADC + config_data.sensor_PRx[ADS_FLT].prSensOffset;
	PR_VEN_mmHg 	= config_data.sensor_PRx[VEN].prSensGain     * PR_VEN_ADC     + config_data.sensor_PRx[VEN].prSensOffset;
	PR_ART_mmHg 	= config_data.sensor_PRx[ART].prSensGain     * PR_ART_ADC     + config_data.sensor_PRx[ART].prSensOffset;

	CalcVenSistDiastPress(PR_VEN_mmHg_Filtered);
	CalcArtSistDiastPress(PR_ART_mmHg_Filtered);

}

void Pressure_sensor_Fltered ()
{
	PR_OXYG_mmHg_Filtered		= meanWA(60,PR_OXYG_mmHg,0);

	PR_LEVEL_mmHg_Filtered		= meanWA(60,PR_LEVEL_mmHg,1);

	// calcolo il volume del liquido in vaschetta come percentuale rispetto al suo valore massimo
	LiquidAmount = (word)((float)ConvertMMHgToMl(PR_LEVEL_mmHg_Filtered) / (float)MAX_LIQUID_AMOUNT * 100.0);

	PR_ADS_FLT_mmHg_Filtered	= meanWA(60,PR_ADS_FLT_mmHg,2);
	/*sul venoso e arterioso, che sono usati per i PID, faccio
	 * un filtro a media mobile solo su 8 campioni, tanto poi
	 * farò una media dei campioni filtrati*/
	PR_VEN_mmHg_Filtered		= meanWA(8,PR_VEN_mmHg,3);
	PR_ART_mmHg_Filtered		= meanWA(8,PR_ART_mmHg,4);

	/*
	if(GetTherapyType() == KidneyTreat)
	{
		// calcolo la pressione arteriosa sull'organo
		CalcolaPresArt(actualSpeed_Art);
	}
	else if(GetTherapyType() == LiverTreat)
	{
		CalcolaPresArt(actualSpeed_Art);
	}
	*/


	PR_OXYG_ADC_Filtered		= meanWA(60,PR_OXYG_ADC,5);
	PR_LEVEL_ADC_Filtered		= meanWA(60,PR_LEVEL_ADC,6);
	PR_ADS_FLT_ADC_Filtered		= meanWA(60,PR_ADS_FLT_ADC,7);
	PR_VEN_ADC_Filtered			= meanWA(60,PR_VEN_ADC,8);
	PR_ART_ADC_Filtered			= meanWA(60,PR_ART_ADC,9);
}

void Coversion_From_ADC_To_Voltage()
{
	V24_P1_CHK_VOLT	= (V24_P1_CHK_ADC * V24_P1_CHK_GAIN + V24_P1_CHK_OFFSET);
	V24_P2_CHK_VOLT	= (V24_P2_CHK_ADC * V24_P2_CHK_GAIN + V24_P2_CHK_OFFSET);
}

/*---FUNZIONE DI CALIBRAZIONE DEI SENSORI DI PRESSIONE
 * NAME: Pressure_Sensor_Calibration
 * INPUT: ID_sens (quale sensore), value (valore in mmHg passato dal PC, point (se è il primo o il secondo punto du calibrazione)
 * */
void Pressure_Sensor_Calibration(Press_sens ID_sens, float value, unsigned char point)
{

	static int ADCFirstPoint, ADCSecondPoint;
	static float FirstValue, SecondValue;
	static bool FIRST_POINT = FALSE;
	unsigned char *ptr_EEPROM = (EEPROM_TDataAddress)&config_data;

	/*primo punto di calibrazione*/
	if (point == 0)
	{
		//copy first value sent by SBC
		FirstValue = value;

		switch (ID_sens)
		{
			case OXYG:
				//copy First Point ADC
				ADCFirstPoint = PR_OXYG_ADC_Filtered;
				FIRST_POINT = TRUE;
				break;

			case LEVEL:
				//copy First Point ADC
				 ADCFirstPoint = PR_LEVEL_ADC_Filtered;
				 FIRST_POINT = TRUE;
				break;

			case ADS_FLT:
				//copy First Point ADC
				ADCFirstPoint = PR_ADS_FLT_ADC_Filtered;
				FIRST_POINT = TRUE;
				break;

			case VEN:
				//copy First Point ADC
				ADCFirstPoint = PR_VEN_ADC_Filtered;
				FIRST_POINT = TRUE;
				break;

			case ART:
				//copy First Point ADC
				ADCFirstPoint = PR_ART_ADC_Filtered;
				FIRST_POINT = TRUE;
				break;

			default:
				ADCFirstPoint = 0;
				break;
		}
	}

	/*secondo punto di calibrazione*/
	else if (point == 1 && FIRST_POINT == TRUE)
	{
		//copy Second value sent by SBC
		SecondValue =  value;
		FIRST_POINT = FALSE;

		switch (ID_sens)
		{
			case OXYG:
				//copy Second Point ADC
				ADCSecondPoint = PR_OXYG_ADC_Filtered;

				config_data.sensor_PRx[OXYG].prSensGain 	= ((float) SecondValue - FirstValue) / ((float) ADCSecondPoint - ADCFirstPoint);
				/* TODO deve essere ripristinata questa e poi modificata la funzione  Coversion_From_ADC_To_mmHg_Pressure_Sensor*/
				config_data.sensor_PRx[OXYG].prSensOffset 	= (float) SecondValue - (ADCSecondPoint * config_data.sensor_PRx[OXYG].prSensGain);
				//config_data.sensor_PRx[OXYG].prSensOffset 	= ADCFirstPoint;

				/*carico il CRC della EEPROM (usata la stessa funzione di CRC del MOD_BUS
				 * IL CRC lo clacolo su tutta la struttura meno i due byte ndel CRC stesso*/
				config_data.EEPROM_CRC = ComputeChecksum(ptr_EEPROM, sizeof(config_data)-2);

				/*finita la calibrazione di un sensore la vado subito a salvare in EEPROM*/
				EEPROM_write((EEPROM_TDataAddress)&config_data, START_ADDRESS_EEPROM, sizeof(config_data));
				break;

			case LEVEL:
				//copy Second Point ADC
				ADCSecondPoint = PR_LEVEL_ADC_Filtered;

				config_data.sensor_PRx[LEVEL].prSensGain 	= ((float)SecondValue - FirstValue) / ((float)ADCSecondPoint - ADCFirstPoint);
				/* TODO deve essere ripristinata questa e poi modificata la funzione  Coversion_From_ADC_To_mmHg_Pressure_Sensor*/
				config_data.sensor_PRx[LEVEL].prSensOffset 	= (float) SecondValue - (ADCSecondPoint * config_data.sensor_PRx[LEVEL].prSensGain);
				//config_data.sensor_PRx[LEVEL].prSensOffset 	= ADCFirstPoint;

				/*carico il CRC della EEPROM (usata la stessa funzione di CRC del MOD_BUS
				 * IL CRC lo clacolo su tutta la struttura meno i due byte ndel CRC stesso*/
				config_data.EEPROM_CRC = ComputeChecksum(ptr_EEPROM, sizeof(config_data)-2);

				/*finita la calibrazione di un sensore la vado subito a salvare in EEPROM*/
				EEPROM_write((EEPROM_TDataAddress)&config_data, START_ADDRESS_EEPROM, sizeof(config_data));
				break;

			case ADS_FLT:
				//copy Second Point ADC
				ADCSecondPoint = PR_ADS_FLT_ADC_Filtered;

				config_data.sensor_PRx[ADS_FLT].prSensGain 		= ((float)SecondValue - FirstValue) / ((float)ADCSecondPoint - ADCFirstPoint);
				/* TODO deve essere ripristinata questa e poi modificata la funzione  Coversion_From_ADC_To_mmHg_Pressure_Sensor*/
				config_data.sensor_PRx[ADS_FLT].prSensOffset 	= (float) SecondValue - (ADCSecondPoint * config_data.sensor_PRx[ADS_FLT].prSensGain);
			//	config_data.sensor_PRx[ADS_FLT].prSensOffset 	= ADCFirstPoint;

				/*carico il CRC della EEPROM (usata la stessa funzione di CRC del MOD_BUS
				 * IL CRC lo clacolo su tutta la struttura meno i due byte ndel CRC stesso*/
				config_data.EEPROM_CRC = ComputeChecksum(ptr_EEPROM, sizeof(config_data)-2);

				/*finita la calibrazione di un sensore la vado subito a salvare in EEPROM*/
				EEPROM_write((EEPROM_TDataAddress)&config_data, START_ADDRESS_EEPROM, sizeof(config_data));
				break;

			case VEN:
				//copy Second Point ADC
				ADCSecondPoint = PR_VEN_ADC_Filtered;

				config_data.sensor_PRx[VEN].prSensGain 		= ((float)(SecondValue - FirstValue) ) / ((float)ADCSecondPoint - ADCFirstPoint);
				/* TODO deve essere ripristinata questa e poi modificata la funzione  Coversion_From_ADC_To_mmHg_Pressure_Sensor*/
				config_data.sensor_PRx[VEN].prSensOffset 	= (float) SecondValue - (ADCSecondPoint * config_data.sensor_PRx[VEN].prSensGain);
				//config_data.sensor_PRx[VEN].prSensOffset 	= ADCFirstPoint;

				/*carico il CRC della EEPROM (usata la stessa funzione di CRC del MOD_BUS
				 * IL CRC lo clacolo su tutta la struttura meno i due byte ndel CRC stesso*/
				config_data.EEPROM_CRC = ComputeChecksum(ptr_EEPROM, sizeof(config_data)-2);

				/*finita la calibrazione di un sensore la vado subito a salvare in EEPROM*/
				EEPROM_write((EEPROM_TDataAddress)&config_data, START_ADDRESS_EEPROM, sizeof(config_data));
				break;

			case ART:
				//copy Second Point ADC
				ADCSecondPoint = PR_ART_ADC_Filtered;

				config_data.sensor_PRx[ART].prSensGain 		= ((float)SecondValue - FirstValue) / ((float)ADCSecondPoint - ADCFirstPoint);
				/* TODO deve essere ripristinata questa e poi modificata la funzione  Coversion_From_ADC_To_mmHg_Pressure_Sensor*/
				config_data.sensor_PRx[ART].prSensOffset 	= (float) SecondValue - (ADCSecondPoint * config_data.sensor_PRx[ART].prSensGain);
				//config_data.sensor_PRx[ART].prSensOffset 	= ADCFirstPoint;

				/*carico il CRC della EEPROM (usata la stessa funzione di CRC del MOD_BUS
				 * IL CRC lo clacolo su tutta la struttura meno i due byte ndel CRC stesso*/
				config_data.EEPROM_CRC = ComputeChecksum(ptr_EEPROM, sizeof(config_data)-2);

				/*finita la calibrazione di un sensore la vado subito a salvare in EEPROM*/
				EEPROM_write((EEPROM_TDataAddress)&config_data, START_ADDRESS_EEPROM, sizeof(config_data));
				break;

			default:
				break;
		}
	}

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




int meanWA(unsigned char dimNum, int newSensVal, char IdSens)
{
	static int circularBuffer[10] [64]; //uso una matrice di 10 array, due per ogni sensore così filtro mmHg e ADC
	static int circBuffAdd[10] [64];    //uso una matrice di 10 array, due per ogni sensore così filtro mmHg e ADC
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
