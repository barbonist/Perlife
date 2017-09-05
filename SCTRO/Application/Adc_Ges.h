/*
 * Adc_Ges.h
 *
 *  Created on: 12/lug/2016
 *      Author: W15
 */

#include "PE_Types.h"

#ifndef APPLICATION_ADC_GES_H_
#define APPLICATION_ADC_GES_H_

#define ADC_CHANNEL_PR1			0U /* pr1 measured on adc channel 0 */
#define ADC_CHANNEL_PR2			1U /* pr2 measured on adc channel 0 */
#define ADC_CHANNEL_TM1			2U /* pr1 measured on adc channel 0 */
#define ADC_CHANNEL_TM2			3U /* pr1 measured on adc channel 0 */



word * ReadAdcPr1(void);
word * ReadAdcPr2(void);
word * ReadAdcTm1(void);
word * ReadAdcTm2(void);


#endif /* APPLICATION_ADC_GES_H_ */
