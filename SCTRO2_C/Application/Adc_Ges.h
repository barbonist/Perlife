/*
 * Adc_Ges.h
 *
 *  Created on: 03/nov/2016
 *      Author: W15
 */

#include "PE_Types.h"

#ifndef APPLICATION_ADC_GES_H_
#define APPLICATION_ADC_GES_H_

#define ADC_CHANNEL_PR1			0U /* pr1 measured on adc channel 0 */
#define ADC_CHANNEL_PR2			1U /* pr2 measured on adc channel 0 */
#define ADC_CHANNEL_PR3			2U /* pr2 measured on adc channel 0 */
//#define ADC_CHANNEL_TM1			2U /* pr1 measured on adc channel 0 */
#define ADC_CHANNEL_TM2			3U /* pr1 measured on adc channel 0 */

// numero di campioni di pressione arteriosa presi per il calcolo della media dei
// campioni gia' mediati dopo la conversione
#define NUMB_OF_SAMPLES_ART     500 * 2
// numero di campioni di pressione venosa presi per il calcolo della media dei
// campioni gia' mediati dopo la conversione
#define NUMB_OF_SAMPLES_VEN     500

//Peso del filtraggio per il valore acquisito STP di piastra
#define PESO_FILTRO_STP			100

// Filippo - definizione della tabella di conversione PT1000
typedef struct
{
	float resistenza;
	float temperatura;
} TABELLA_PT1000;

typedef struct
{
	int count;
	int press;
} TABELLA_PRESSVEN;

typedef struct
{
	int count;
	int press;
} TABELLA_PRESSART;

#define PR_ART_MAX_SATURAZIONE 450 // mmHg
#define PR_VEN_MAX_SATURAZIONE 450 // mmHg

word * ReadAdcPr1(void);
word * ReadAdcPr2(void);
word * ReadAdcPr3(void);
//word * ReadAdcTm1(void);
word * ReadAdcTm2(void);


int meanWA(unsigned char dimNum, int newSensVal, char IdSens);
void ADC1_Calibration(void);
void ADC0_Calibration(void);
void Dip_Switch_ADC_Init(void);
void Voltage_Peltier_ADC_Init(void);
void Manange_ADC0(void);
void Manange_ADC1(void);
void PR_Sens_ADC_Init(void);
void Coversion_From_ADC_To_mmHg_Pressure_Sensor(void);
word ConversionePressioneExtraVen(word countVen);
word ConversionePressioneExtraArt(word countArt);
void Coversion_From_ADC_To_Voltage();
void Pressure_sensor_Fltered ();
void Manage_Air_Sensor_1(void);
void Manage_Debug_led(bool Status);
void Pressure_Sensor_Calibration(Press_sens ID_sens, float value, unsigned char point);

void CalcVenSistDiastPress(word Press);
void CalcArtSistDiastPress(word Press);
void T_PLATE_P_Init(void);
void Coversion_From_ADC_To_degree_T_PLATE_Sensor();

// Filippo - funzione per la calibrazione del sensore PT1000
void Plate_Temp_Sensor_Calibration(float value);

#endif /* APPLICATION_ADC_GES_H_ */
