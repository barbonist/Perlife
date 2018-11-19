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

//#define DEMO_AFERETICA

#ifdef DEMO_AFERETICA
	// define di correzione del valore di temp. sensori Ir
	#define LOWER_RANGE_IR_CORRECTION 19
	#define HIGHER_RANGE_IR_CORRECTION 31
	#define DELTA_CORRECTION 0.5
#else
// define di correzione del valore di temp. sensori Ir
	#define LOWER_RANGE_IR_CORRECTION 10
	#define HIGHER_RANGE_IR_CORRECTION 31
	#define DELTA_CORRECTION 0.5
#endif






void initTempSensIR(void);
unsigned char computeCRC8TempSens(unsigned char buffer[]);
bool computeCRC8TempSensRx(unsigned char buffer[],unsigned char command, unsigned char TempSensAddr);
void Manage_IR_Sens_Temp(void);
unsigned char * buildCmdReadTempSensIR(unsigned char  tempSensAddress,
										   unsigned char  command,
										   word dataWordTx);
unsigned char * buildCmdWriteTempSensIR(unsigned char  tempSensAddress,
		   	   	   	   	   	   	   	    unsigned char  command,
										word dataWordTx);

void alwaysIRTempSensRead(void);

#endif /* APPLICATION_TEMP_SENSIR_H_ */
