/*
 * Flowsens.h
 *
 *  Created on: 01/mar/2017
 *      Author: W15
 */

#ifndef APPLICATION_FLOWSENS_H_
#define APPLICATION_FLOWSENS_H_

#define FLOW_SENSOR_ONE_ADDR	0x01 /* address of flowsensor n°1 - arterial line */
#define FLOW_SENSOR_TWO_ADDR	0x0C /* address of flowsensor n°2 - venous line */

#define CMD_PING_CODE			0x29
#define CMD_RESTART_CODE		0x16
#define CMD_RESET_VOL_CODE		0x39
#define CMD_ZERO_CODE			0x33
#define CMD_GET_VAL_CODE		0x3F
#define CMD_IDENT_CODE			0x25
#define CMD_READ_SRAM			0x17
#define CMD_READ_CODE_CODE		0x11

#define START_PATTERN_BASE		0xF0

#define ID_FLOW_VAL_ULSEC		0x81 /* float, 4 byte IEEE 754 */
#define ID_FLOW_VAL_MLMIN		0x82 /* float, 4 byte IEEE 754 */
#define ID_FLOW_VAL_ULSEC_CH1	0x83 /* float, 4 byte IEEE 754 */
#define ID_FLOW_VAL_ULSEC_CH2	0x84 /* float, 4 byte IEEE 754 */
#define ID_FLOW_ZERO_ADJ_ULSEC	0x85 /* float, 4 byte IEEE 754 */
#define ID_TEMP_VAL_CENTIG		0x88 /* float, 4 byte IEEE 754 */
#define ID_ACCUMULATED_VOL_UL	0x8B /* float, 4 byte IEEE 754 */
#define ID_BUBBLE_SIGNAL_ATTEN	0x27 /* unsigned word LL/LH/hl/hh */
#define ID_MAX_BUBBLE_ATTNTN	0x28 /* unsigned word LL/LH/hl/hh */
#define ID_MAX_BUBBLE_SIZE		0x2A /* unsigned byte LL/lh/hl/hh */
#define ID_DEVICE_ERROR			0x42 /* unsigned long LL/LH/HL/HH */
#define ID_FLOW_MEAS_ERROR		0x43 /* unsigned long LL/LH/HL/HH */
#define ID_DET_BUBBLE_ERROR		0x44 /* unsigned long LL/LH/HL/HH */
#define ID_WARNING_MSG			0x45 /* unsigned byte LL/lh/hl/hh */
#define ID_ADC_ZERO				0x22 /* unsigned word LL/LH/hl/hh */
#define ID_ADC_SIGNAL			0x23 /* unsigned word LL/LH/hl/hh */

#define MASK_ERROR_BUBBLE_ALARM		0x01 /* maschera errore per allarme aria */


void initUFlowSensor(void);
void alwaysUFlowSensor(void);
unsigned char computeCRCFlowSens(unsigned char buffer[]);
struct ultrsndFlowSens * buildCmdToFlowSens(unsigned char sensorAddress, unsigned char cmdId, unsigned char ctrlZeroAdjust, float valueZeroAdjust, unsigned char valueId);

#endif /* APPLICATION_FLOWSENS_H_ */
