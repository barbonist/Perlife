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



word * ReadAdcPr1(void);
word * ReadAdcPr2(void);
word * ReadAdcPr3(void);
//word * ReadAdcTm1(void);
word * ReadAdcTm2(void);

void	initAdcParam(void);
void 	alwaysAdcParam(void);

int meanWA(unsigned char dimNum, int newSensVal);
void ADC1_Calibration(void);
void ADC0_Calibration(void);
void Dip_Switch_ADC_Init(void);
void Voltage_Peltier_ADC_Init(void);
void Manange_ADC0(void);
void Manange_ADC1(void);
void PR_Sens_ADC_Init(void);
#endif /* APPLICATION_ADC_GES_H_ */
