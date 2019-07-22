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
#include "pid.h"

// Filippo - definisco tabella di conversione per una PT1000
TABELLA_PT1000 tabellaPT1000[14]={
		{803.06,-50.0},
		{842.71,-40.0},
		{882.22,-30.0},
		{921.6,-20.0},
		{960.86,-10.0},
		{1000.0,0},
		{1039.0,10.0},
		{1077.9,20.0},
		{1116.7,30.0},
		{1155.4,40.0},
		{1194.0,50.0},
		{1232.4,60.0},
		{1270.8,70.0},
		{1309.0,80.0}

};




byte adcRes = 0xFF;
word adcValue;
word adcValueDummy;
word * adcValPtr;

unsigned int MedForVenousPid	 = 0;
unsigned int MedForArteriousPid	 = 0;


void PR_Sens_ADC_Init()
{
	PR_ADS_FLT_ADC   = 0;		//variabile globale per il valore ADC del sensore di pressione del filtro assorbente --> PTB11
	PR_ADS_FLT_mmHg  = 0;		//variabile globale per il valore in mmHg del sensore di pressione del filtro assorbente
	PR_ART_ADC 		 = 0;		//variabile globale per il valore ADC del sensore di pressione arteriosa --> PTB7
	PR_ART_mmHg		 = 0;		//variabile globale per il valore in mmHg del sensore di pressione arteriosa
	PR_VEN_ADC		 = 0;		//variabile globale per il valore ADC del sensore di pressione Venoso --> PTB6
	PR_VEN_mmHg	 	 = 0;		//variabile globale per il valore in mmHg del sensore di pressione Venoso
	PR_OXYG_ADC		 = 0;		//variabile globale per il valore ADC del sensore di pressione ossigenatore --> PTC10
	PR_OXYG_mmHg	 = 0;		//variabile globale per il valore in mmHg del sensore di pressione ossigenatore
	PR_LEVEL_ADC	 = 0;		//variabile globale per il valore ADC del sensore di pressione di livello vaschetta --> PTC11
	PR_LEVEL_mmHg	 = 0;		//variabile globale per il valore in mmHg del sensore di pressione di livello vaschetta
	PR_VEN_TARA_mmHg = 0;		//variabile globale che serve a fare la tara di pressione dopo la connessione dell'organo, all'inizio del trattamento
	PR_ART_TARA_mmHg = 0;  		//variabile globale che serve a fare la tara di pressione dopo la connessione dell'organo, all'inizio del trattamento
	Soglia_minima_ADC_allarme_Livello = 0; //variabile globale che serve a fare la tara di pressione del livello di liquido in vaschetta in ADC dopo la connessione dell'organo, all'inizio del trattamento
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

void T_PLATE_P_Init(void)
{
	T_PLATE_C_ADC		 = 0;
	T_PLATE_C_GRADI_CENT = 0;
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
		/*T_PLATE_P sta su AD0 channel 6; passando l'indirizzo della variabile, la valorizzo*/
		AD0_GetChanValue16(T_PLATE_P_ADC_CHANNEL, &T_PLATE_C_ADC);

		/*converte il valori ADC in gradi centigradi del sensor di temperatura della piastra*/
		Coversion_From_ADC_To_degree_T_PLATE_Sensor();

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
		/*5 Volt Board sta su AD1 channel 9; passando l'indirizzo della variabile, la valorizzo*/
		AD1_GetChanValue16(V5_B_CHK_ADC_CHANNEL, &V5_B_CHK_ADC);
		/*Peltier Voltage 2 sta su AD1 channel 10; passando l'indirizzo della variabile, la valorizzo*/
		AD1_GetChanValue16(V24_P2_CHK_ADC_CHANNEL, &V24_P2_CHK_ADC);
		/*Peltier Voltage 1 sta su AD1 channel 11; passando l'indirizzo della variabile, la valorizzo*/
		AD1_GetChanValue16(V24_P1_CHK_ADC_CHANNEL, &V24_P1_CHK_ADC);
		/*5 Volt Analogic sta su AD1 channel 12; passando l'indirizzo della variabile, la valorizzo*/
		AD1_GetChanValue16(V5_An_CHK_ADC_CHANNEL, &V5_An_CHK_ADC);
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

void Coversion_From_ADC_To_degree_T_PLATE_Sensor()
{
/* Filippo - calcoliamo la temperatura della PT1000 in altro modo
	if (Frigo_ON)
		T_PLATE_C_GRADI_CENT = config_data.T_Plate_Sensor_Gain_Cold * T_PLATE_C_ADC + config_data.T_Plate_Sensor_Offset_Cold;
	else
		T_PLATE_C_GRADI_CENT = config_data.T_Plate_Sensor_Gain_Heat * T_PLATE_C_ADC + config_data.T_Plate_Sensor_Offset_Heat;

	T_PLATE_C_GRADI_CENT /= 10;
*/
	float appoggioFloat,V1;
	float resPT1000;
	int i;
	float m,q;

	appoggioFloat=(float)T_PLATE_C_ADC/65535;
	appoggioFloat*=3.3;
	appoggioFloat/=10;
	// adesso in appoggioFloat ho la tensione differenziale letta dal front end analogico
	appoggioFloat=appoggioFloat+0.743;	// il valore 0.743 è il valore di tensione del ramo di resistenze fisso
	if (appoggioFloat<0)
	{
		appoggioFloat=-appoggioFloat;
	}
	// adesso in appoggioFloat ho il valore di tensione letto sul partitore variabile in cui è messa la PT1000
	// applico la formula del partitore per conoscere il valore di resistenza della PT1000 x=(V1*4700)/(1-V1) dove V1 è la tensione
	// di partitore diviso l'alimentazione
	V1=appoggioFloat/5;	// divido la tensione di partitore per l'alimentazione dello stesso
	resPT1000=(V1*4700)/(1-V1);

	// adesso in resPT1000 ho il valore della resistenza
	// adesso attraverso la tabella vado a calcolare l'effettiva temperatura
	for (i=1;i<14;i++)
	{
		if ((resPT1000>=tabellaPT1000[i-1].resistenza) && (resPT1000<=tabellaPT1000[i].resistenza))
		{
			// ho trovato l'intervallo della resistenza
			// calcolo adesso il coefficiente angolare ed il termine noto della retta tra i due punti
			m=(tabellaPT1000[i].temperatura-tabellaPT1000[i-1].temperatura)/(tabellaPT1000[i].resistenza-tabellaPT1000[i-1].resistenza);
			q=tabellaPT1000[i].temperatura-(m*tabellaPT1000[i].resistenza);
			break;
		}
	}

	if (i<14)
	{
		T_PLATE_C_GRADI_CENT=m*resPT1000+q;
	}
	else
	{
		T_PLATE_C_GRADI_CENT=tabellaPT1000[13].temperatura;
	}

//	T_PLATE_C_GRADI_CENT+=config_data.T_Plate_Sensor_Offset_Heat;

}
void Coversion_From_ADC_To_mmHg_Pressure_Sensor()
{

	PR_OXYG_mmHg 	= (word)(config_data.sensor_PRx[OXYG].prSensGain    * PR_OXYG_ADC    + config_data.sensor_PRx[OXYG].prSensOffset + 0.5);
	PR_LEVEL_mmHg 	= (word)((config_data.sensor_PRx[LEVEL].prSensGain   * PR_LEVEL_ADC   + config_data.sensor_PRx[LEVEL].prSensOffset) * 100.0 + 0.5);
	PR_ADS_FLT_mmHg = (word)(config_data.sensor_PRx[ADS_FLT].prSensGain * PR_ADS_FLT_ADC + config_data.sensor_PRx[ADS_FLT].prSensOffset + 0.5);
	PR_VEN_mmHg 	= (word)(config_data.sensor_PRx[VEN].prSensGain     * PR_VEN_ADC     + config_data.sensor_PRx[VEN].prSensOffset + 0.5);
	PR_ART_mmHg 	= (word)(config_data.sensor_PRx[ART].prSensGain     * PR_ART_ADC     + config_data.sensor_PRx[ART].prSensOffset + 0.5);

}

void Pressure_sensor_Fltered ()
{
	PR_OXYG_mmHg_Filtered		= meanWA(50,PR_OXYG_mmHg,0);

	PR_LEVEL_mmHg_Filtered		= meanWA(255,PR_LEVEL_mmHg,1);

	// calcolo il volume del liquido in vaschetta come percentuale rispetto al suo valore massimo
	LiquidAmount = (word)((float)ConvertMMHgToMl(PR_LEVEL_mmHg_Filtered) / (float)MAX_LIQUID_AMOUNT * 100.0);

	PR_ADS_FLT_mmHg_Filtered	= meanWA(50,PR_ADS_FLT_mmHg,2);
	/*sul venoso e arterioso, che sono usati per i PID, faccio
	 * un filtro a media mobile solo su 8 campioni, tanto poi
	 * farò una media dei campioni filtrati*/
	PR_VEN_mmHg_Filtered		= meanWA(40,PR_VEN_mmHg,3);
	PR_ART_mmHg_Filtered		= meanWA(40,PR_ART_mmHg,4);


	CalcVenSistDiastPress(PR_VEN_mmHg_Filtered);
	CalcArtSistDiastPress(PR_ART_mmHg_Filtered);

	PR_OXYG_ADC_Filtered		= meanWA(255,PR_OXYG_ADC,5);
	PR_LEVEL_ADC_Filtered		= meanWA(255,PR_LEVEL_ADC,6);
	PR_ADS_FLT_ADC_Filtered		= meanWA(255,PR_ADS_FLT_ADC,7);
	PR_VEN_ADC_Filtered			= meanWA(255,PR_VEN_ADC,8);
	PR_ART_ADC_Filtered			= meanWA(255,PR_ART_ADC,9);
}


void CalcVenSistDiastPress(word Press)
{
  static word CircPressArr[NUMB_OF_SAMPLES_VEN];
  static int CircPressArrIdx = 0;
  static unsigned char BufferFull = 0;
  word min = 0xffff;
  word max = 0;
  int MAX_SAMPLE_FOR_SPEED = NUMB_OF_SAMPLES_VEN;
  float Press_flow_extimated = 0;

  if ( pumpPerist[1].actualSpeed != 0)
  {
	  //MAX_SAMPLE_FOR_SPEED = (int)((float)NUMB_OF_SAMPLES_VEN / (float)pumpPerist[1].actualSpeed * 12.0);
	  // e' la stessa formula
  	  MAX_SAMPLE_FOR_SPEED = (int)((float)NUMB_OF_SAMPLES_VEN / ( (float)pumpPerist[1].actualSpeed / 12.0 ));
  }

  if (MAX_SAMPLE_FOR_SPEED > NUMB_OF_SAMPLES_VEN )
	  MAX_SAMPLE_FOR_SPEED = NUMB_OF_SAMPLES_VEN;

  CircPressArr[CircPressArrIdx] = Press;
  CircPressArrIdx++;
  if(CircPressArrIdx == NUMB_OF_SAMPLES_VEN)
  {
	  CircPressArrIdx = 0;
	  BufferFull = 1;
  }

  if(BufferFull)
  {
	  int i;
	  if(CircPressArrIdx == 0)
		i = NUMB_OF_SAMPLES_VEN - 1;
	  else
		i = CircPressArrIdx - 1;

	  MedForVenousPid = 0;
	  for(int j = 0; j < MAX_SAMPLE_FOR_SPEED; j++)
	  {
		  MedForVenousPid += CircPressArr[i];
		  if(CircPressArr[i] < min)
			  min = CircPressArr[i];
		  if(CircPressArr[i] > max)
			  max = CircPressArr[i];
		  i--;
		  if (i < 0)
			  i = MAX_SAMPLE_FOR_SPEED - 1;
	  }
	  MedForVenousPid = MedForVenousPid / MAX_SAMPLE_FOR_SPEED;
  }
  else
  {
	  int LastIndex;
	  MedForVenousPid = 0;
	  if(CircPressArrIdx > MAX_SAMPLE_FOR_SPEED)
		  LastIndex = CircPressArrIdx - MAX_SAMPLE_FOR_SPEED;
	  else
		  LastIndex = 0;
	  for(int i = CircPressArrIdx - 1; i >= LastIndex; i--)
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

	/*sottraggo la tara col valore che sarà diverso da zero solo dopo la connessione organo,
	 * viene valorizzatasolo all'inizio del trattamento*/
	PR_VEN_Diastolyc_mmHg = PR_VEN_Diastolyc_mmHg - PR_VEN_TARA_mmHg;

	/*controllo se dopo la tara ho un valore negativo e lo metto a zero*/
	if (PR_VEN_Diastolyc_mmHg < 0)
		PR_VEN_Diastolyc_mmHg = 0;

	PR_VEN_Sistolyc_mmHg  = max;

	/*sottraggo la tara col valore che sarà diverso da zero solo dopo la connessione organo,
	 * viene valorizzatasolo all'inizio del trattamento*/
	PR_VEN_Sistolyc_mmHg = PR_VEN_Sistolyc_mmHg - PR_VEN_TARA_mmHg;

	/*controllo se dopo la tara ho un valore negativo e lo metto a zero*/
	if (PR_VEN_Sistolyc_mmHg < 0)
		PR_VEN_Sistolyc_mmHg = 0;

	PR_VEN_Med_mmHg = (int) ( 2 * PR_VEN_Diastolyc_mmHg + PR_VEN_Sistolyc_mmHg )/3;

//	Press_flow_extimated = CalcolaPresVen_with_Flow(1);
//
//	PR_VEN_Diastolyc_mmHg_ORG = PR_VEN_Diastolyc_mmHg - Press_flow_extimated;
//	PR_VEN_Sistolyc_mmHg_ORG  = PR_VEN_Sistolyc_mmHg - Press_flow_extimated;
//	PR_VEN_Med_mmHg_ORG		  = PR_VEN_Med_mmHg - Press_flow_extimated;



	if (sensor_UFLOW[1].Average_Flow_Val_for_GUI <= 100)
		perfusionParam.renalResistanceVenous = 0;
	else
		perfusionParam.renalResistanceVenous = (word)((float)PR_VEN_Med_mmHg /sensor_UFLOW[1].Average_Flow_Val_for_GUI * 1000.0); // mmHg/(ml/min) *10 (SBC poi divide per 10)

	perfusionParam.pulsatilityVenous = PR_VEN_Sistolyc_mmHg - PR_VEN_Diastolyc_mmHg;

	if (PR_VEN_Diastolyc_mmHg_ORG < 0)
		PR_VEN_Diastolyc_mmHg_ORG = 0;
	if (PR_VEN_Sistolyc_mmHg_ORG < 0)
		PR_VEN_Sistolyc_mmHg_ORG = 0;
	if (PR_VEN_Med_mmHg_ORG < 0)
		PR_VEN_Med_mmHg_ORG = 0;

}


/*
void CalcVenSistDiastPress(word Press)
{
  static word CircPressArr[NUMB_OF_SAMPLES_VEN];
  static int CircPressArrIdx = 0;
  static unsigned char BufferFull = 0;
  word min = 0xffff;
  word max = 0;
  int MAX_SAMPLE_FOR_SPEED = NUMB_OF_SAMPLES_VEN;

  if ( pumpPerist[1].actualSpeed != 0)
	  MAX_SAMPLE_FOR_SPEED = NUMB_OF_SAMPLES_VEN / pumpPerist[1].actualSpeed;

  if (MAX_SAMPLE_FOR_SPEED > NUMB_OF_SAMPLES_VEN )
	  MAX_SAMPLE_FOR_SPEED = NUMB_OF_SAMPLES_VEN;

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
	  for(int i = 0; i < MAX_SAMPLE_FOR_SPEED; i++)
	  {
		  MedForVenousPid += CircPressArr[i];
		  if(CircPressArr[i] < min)
			  min = CircPressArr[i];
		  if(CircPressArr[i] > max)
			  max = CircPressArr[i];
	  }
	  MedForVenousPid = MedForVenousPid / MAX_SAMPLE_FOR_SPEED;
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
*/

void CalcArtSistDiastPress(word Press)
{
  static word CircPressArr[NUMB_OF_SAMPLES_ART];
  static int CircPressArrIdx = 0;
  static unsigned char BufferFull = 0;
  word min = 0xffff;
  word max = 0;
  int MAX_SAMPLE_FOR_SPEED = NUMB_OF_SAMPLES_ART;
  float Press_flow_extimated = 0.0;

//  if ( pumpPerist[0].actualSpeed >= 12)
//      MAX_SAMPLE_FOR_SPEED = (int)((float)(NUMB_OF_SAMPLES_ART / 2) / (float)pumpPerist[0].actualSpeed * 12.0);
//  else if ( pumpPerist[0].actualSpeed != 0)
//      MAX_SAMPLE_FOR_SPEED = (int)((float)NUMB_OF_SAMPLES_ART / (float)pumpPerist[0].actualSpeed * 12.0);
  if ( pumpPerist[0].actualSpeed != 0)
        MAX_SAMPLE_FOR_SPEED = (int)((float)NUMB_OF_SAMPLES_ART / (float)pumpPerist[0].actualSpeed * 12.0);

  if (MAX_SAMPLE_FOR_SPEED > NUMB_OF_SAMPLES_ART )
	  MAX_SAMPLE_FOR_SPEED = NUMB_OF_SAMPLES_ART;

  CircPressArr[CircPressArrIdx] = Press;
  CircPressArrIdx++;
  if(CircPressArrIdx == NUMB_OF_SAMPLES_ART)
  {
	  CircPressArrIdx = 0;
	  BufferFull = 1;
  }

  if(BufferFull)
  {
	  int i;
	  if(CircPressArrIdx == 0)
		i = NUMB_OF_SAMPLES_ART - 1;
	  else
		i = CircPressArrIdx - 1;

	  MedForArteriousPid = 0;
	  for(int j = 0; j < MAX_SAMPLE_FOR_SPEED; j++)
	  {
		  MedForArteriousPid += CircPressArr[i];
		  if(CircPressArr[i] < min)
			  min = CircPressArr[i];
		  if(CircPressArr[i] > max)
			  max = CircPressArr[i];
		  i--;
		  if (i < 0)
			  i = MAX_SAMPLE_FOR_SPEED - 1;
	  }
	  MedForArteriousPid = MedForArteriousPid / MAX_SAMPLE_FOR_SPEED;
  }
  else
  {
	  int LastIndex;
	  MedForArteriousPid = 0;
	  if(CircPressArrIdx > MAX_SAMPLE_FOR_SPEED)
		  LastIndex = CircPressArrIdx - MAX_SAMPLE_FOR_SPEED;
	  else
		  LastIndex = 0;
	  for(int i = CircPressArrIdx - 1; i >= LastIndex; i--)
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

	/*sottraggo la tara col valore che sarà diverso da zero solo dopo la connessione organo,
	 * viene valorizzatasolo all'inizio del trattamento*/
	PR_ART_Diastolyc_mmHg = PR_ART_Diastolyc_mmHg - PR_ART_TARA_mmHg;

	/*controllo se dopo la tara ho un valore negativo e lo metto a zero*/
	if (PR_ART_Diastolyc_mmHg < 0)
		PR_ART_Diastolyc_mmHg = 0;

	PR_ART_Sistolyc_mmHg  = max;

	if ( pumpPerist[0].actualSpeed < 12)
	{
		// per velocita' basse forzo la sistolica alla filtered perche, altrimenti, rimane bloccata a valori alti
		// ed il pid non funziona piu' bene
		PR_ART_Sistolyc_mmHg = PR_ART_mmHg_Filtered;
	}

	/*sottraggo la tara col valore che sarà diverso da zero solo dopo la connessione organo,
	 * viene valorizzata solo all'inizio del trattamento*/
	PR_ART_Sistolyc_mmHg = PR_ART_Sistolyc_mmHg - PR_ART_TARA_mmHg;

	/*controllo se dopo la tara ho un valore negativo e lo metto a zero*/
	if (PR_ART_Sistolyc_mmHg < 0)
		PR_ART_Sistolyc_mmHg = 0;

	PR_ART_Med_mmHg = (int) ( 2 * PR_ART_Diastolyc_mmHg + PR_ART_Sistolyc_mmHg )/3;

	//	Press_flow_extimated = CalcolaPresArt_with_Flow(1);
//
//	PR_ART_Diastolyc_mmHg_ORG = PR_ART_Diastolyc_mmHg - Press_flow_extimated;
//	PR_ART_Sistolyc_mmHg_ORG  = PR_ART_Sistolyc_mmHg - Press_flow_extimated;
//	PR_ART_Med_mmHg_ORG		  = PR_ART_Med_mmHg - Press_flow_extimated;

	// RICHIESTO DA ANGELA 18_05_2018
	//perfusionParam.renalResistance = (word)((float)PR_ART_Med_mmHg_ORG /sensor_UFLOW[0].Average_Flow_Val * 100.0);
//	wd = (int) ( 2 * PR_ART_Sistolyc_mmHg_ORG + PR_ART_Diastolyc_mmHg_ORG)/3;

	if (sensor_UFLOW[0].Average_Flow_Val_for_GUI <= 20)
		perfusionParam.renalResistance = 0;
	else
		perfusionParam.renalResistance = (word)((float)PR_ART_Med_mmHg /sensor_UFLOW[0].Average_Flow_Val_for_GUI * 1000.0); // mmHg/(ml/min) * 10 (SBC poi divide per 10)

	if (PR_ART_Diastolyc_mmHg_ORG < 0)
		PR_ART_Diastolyc_mmHg_ORG = 0;
	if (PR_ART_Sistolyc_mmHg_ORG < 0)
		PR_ART_Sistolyc_mmHg_ORG = 0;
	if (PR_ART_Med_mmHg_ORG < 0)
		PR_ART_Med_mmHg_ORG = 0;
}

void Coversion_From_ADC_To_Voltage()
{
	//V24_P1_CHK_VOLT	= (V24_P1_CHK_ADC * V24_P1_CHK_GAIN + V24_P1_CHK_OFFSET);
	// Filippo - modifica per errore di conversione
//	V24_P1_CHK_VOLT = (float)(V24_P1_CHK_ADC * T1_TEST_DIG_TO_VOLT);
	V24_P1_CHK_VOLT = (float)(V24_P1_CHK_ADC) * T1_TEST_DIG_TO_VOLT;
	//V24_P2_CHK_VOLT	= (V24_P2_CHK_ADC * V24_P2_CHK_GAIN + V24_P2_CHK_OFFSET);
	// Filippo - modifica per errore di conversione
	V24_P2_CHK_VOLT = (float)(V24_P2_CHK_ADC) * T1_TEST_DIG_TO_VOLT;
	// Filippo - messo tensioni uguali in modo da passare il test visto che la 24V della Peltier1 non c'è più
	V24_P1_CHK_VOLT=V24_P2_CHK_VOLT;
//	V24_P2_CHK_VOLT = (float)(V24_P2_CHK_ADC * T1_TEST_DIG_TO_VOLT);

	V24_Ever_Motor_Value = V24_P2_CHK_ADC * 3.3;
	V24_Ever_Motor_Value = V24_Ever_Motor_Value * 12.480;
	V24_Ever_Motor_Value = V24_Ever_Motor_Value / 0.680;
	V24_Ever_Motor_Value = V24_Ever_Motor_Value / 65535;
	/*Aggiungo un fattore correttivo per tolleranza su resisternze su scheda sulla scheda*/
	V24_Ever_Motor_Value = V24_Ever_Motor_Value * 0.9764;

	V5_B_Value = V5_B_CHK_ADC * 3.3;
	V5_B_Value = V5_B_Value * 2;
	V5_B_Value = V5_B_Value / 65535;

	V5_An_Value = V5_An_CHK_ADC * 3.3;
	V5_An_Value = V5_An_Value * 2;
	V5_An_Value = V5_An_Value / 65535;



}

// Filippo - funzione per la calibrazione del sensore PT1000
void Plate_Temp_Sensor_Calibration(float value)
{
	unsigned char *ptr_EEPROM = (EEPROM_TDataAddress)&config_data;

	config_data.T_Plate_Sensor_Offset_Heat=value-T_PLATE_C_GRADI_CENT;

	config_data.EEPROM_CRC = ComputeChecksum(ptr_EEPROM, sizeof(config_data)-2);

	/*finita la calibrazione di un sensore la vado subito a salvare in EEPROM*/
	EEPROM_write((EEPROM_TDataAddress)&config_data, START_ADDRESS_EEPROM, sizeof(config_data));
}



/*---FUNZIONE DI CALIBRAZIONE DEI SENSORI DI PRESSIONE
 * NAME: Pressure_Sensor_Calibration
 * INPUT: ID_sens (quale sensore), value (valore in mmHg passato dal PC), point (se è il primo o il secondo punto du calibrazione)
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
				config_data.sensor_PRx[OXYG].prSensOffset 	= (float) SecondValue - (ADCSecondPoint * config_data.sensor_PRx[OXYG].prSensGain);

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
				config_data.sensor_PRx[LEVEL].prSensOffset 	= (float) SecondValue - (ADCSecondPoint * config_data.sensor_PRx[LEVEL].prSensGain);

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
				config_data.sensor_PRx[ADS_FLT].prSensOffset 	= (float) SecondValue - (ADCSecondPoint * config_data.sensor_PRx[ADS_FLT].prSensGain);

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
				config_data.sensor_PRx[VEN].prSensOffset 	= (float) SecondValue - (ADCSecondPoint * config_data.sensor_PRx[VEN].prSensGain);

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
				config_data.sensor_PRx[ART].prSensOffset 	= (float) SecondValue - (ADCSecondPoint * config_data.sensor_PRx[ART].prSensGain);

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
	static int circularBuffer[10] [255]; //uso una matrice di 10 array, due per ogni sensore così filtro mmHg e ADC
	static int circBuffAdd[10] [255];    //uso una matrice di 10 array, due per ogni sensore così filtro mmHg e ADC
	int numSumValue = 0;
	int denValue=0;
	int numTotal=0;
	float numTotalFloat;

	if (dimNum <= 255){
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
	numTotalFloat = ((float)numSumValue/(float)denValue);
	numTotal = (int)(numTotalFloat + 0.5);

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
