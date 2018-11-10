/*
 * Temp_sensIR.h
 *
 *  Created on: 06/mar/2017
 *      Author: W15
 */

#ifndef APPLICATION_TEMP_SENSIR_H_
#define APPLICATION_TEMP_SENSIR_H_

#include "PE_Types.h"

#define SD_DEFAULT_E2_ADDRESS_VAL		0x5A

#define SD_SMBUS_E2_ADDRESS				0x0E /* read / write */
#define SD_TOMAX_E2_ADDRESS				0x00 /* read / write */
#define SD_TOMIN_E2_ADDRESS				0x01 /* read / write */
#define SD_PWMCTRL_E2_ADDRESS			0x02 /* read / write */
#define SD_TARANGE_E2_ADDRESS			0x03 /* read / write */
#define SD_EMISSIVITY_E2_ADDRESS		0x04 /* read / write */
#define SD_CONFREG_E2_ADDRESS			0x05 /* read / write */
#define SD_REG_RD_WR_1_ADDRESS			0x0F /* read / write */
#define SD_REG_RD_WR_2_ADDRESS			0x19 /* read / write */
#define SD_IDNUM1_E2_ADDRESS			0x1C /* read */
#define SD_IDNUM2_E2_ADDRESS			0x1D /* read */
#define SD_IDNUM3_E2_ADDRESS			0x1E /* read */
#define SD_IDNUM4_E2_ADDRESS			0x1F /* read */

#define SD_RAWDATACH1_RAM_ADDRESS		0x04 /* raw data on channel 1 */
#define SD_RAWDATACH2_RAM_ADDRESS		0X05 /* raw data on channel 2 */
#define SD_TAMB_VAL_RAM_ADDRESS			0x06 /* Tambient */
#define SD_TOBJ1_RAM_ADDRESS			0x07 /* Tobject 1 */
#define SD_TOBJ2_RAM_ADDRESS			0x08 /* Tobject 2 */

#define EEPROM_ACCESS_COMMAND			0x20 /* final address = eeprom_access_command + sd_xxx_e2_address */
											 /* es: access to smbus address = 0x20 + 0x0E = 0x2E */

#define RAM_ACCESS_COMMAND				0x00 /* final address = ram_access_command + sd_xxx_ram_address */
#define FIRST_IR_TEMP_SENSOR			0x01 /*corrisponde al sensore con connettore a 4 pin*/
#define LAST_IR_TEMP_SENSOR				0x03 /*corrisponde al sensore con connettore a 6 pin, l'indirizzo 0x02 corrisponde al sensore con connettore a 5 pin*/

/*se inizio a correggere da 10, da 10 a 4 ho 6 °C;
 * con 0.5 per 1°C quando leggo dal sensore 6 °C,
 * avrò come correzione T = 6 - (10-6) * 0.5 = 4 °C
 * ma siccome se devo arrivare a 4, accetto anche 4.5 °, avrò 4.5 quando in realtà ho 6.3333
 * La formula inversa per rovare quale valore corretto serve per avere un determinato valore reale
 * è:  Temp_value =  ( (LOWER_RANGE_IR_CORRECTION * DELTA_CORRECTION) + Temp_correct )/ (1+DELTA_CORRECTION)
 * Analogamente da 31 a 37 ho 6°C e ho un offset totale di 2 °C
 * Se correggo da 19, da 19 a 4 ho 15 °C quindi con 0.5 per 1 °C
 * Così facendo ho 4 °C quando leggo dal sensore 9 °C,
 * ma siccome se devo arrivare a 4, accetto anche 4.5 °, avrò 4.5 quando in realtà ho 9.3333
 * */
#define LOWER_RANGE_IR_CORRECTION		19 //10 TODO da rimettere 10 nella versione definitiva---19 solo per Bergamo/Pisa
#define HIGHER_RANGE_IR_CORRECTION		31
#define DELTA_CORRECTION				0.5 //delta di correzione tmeperatura per 1 °C


void initTempSensIR(void);
unsigned char computeCRC8TempSens(unsigned char buffer[]);
bool computeCRC8TempSensRx(unsigned char buffer[],unsigned char command, unsigned char TempSensAddr);
void Manage_IR_Sens_Temp(void);
float Ir_Temperature_correction_offset (float Temp_value);
unsigned char * buildCmdReadTempSensIR(unsigned char  tempSensAddress,
										   unsigned char  command,
										   word dataWordTx);
unsigned char * buildCmdWriteTempSensIR(unsigned char  tempSensAddress,
		   	   	   	   	   	   	   	    unsigned char  command,
										word dataWordTx);

void alwaysIRTempSensRead(void);

#endif /* APPLICATION_TEMP_SENSIR_H_ */
