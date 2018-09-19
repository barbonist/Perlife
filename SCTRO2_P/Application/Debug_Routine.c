/*
 * Debug_Routine.c
 *
 *  Created on: 03/set/2018
 *      Author: W32
 */
#include "SBC_COMM.h"
#include "PC_DEBUG_COMM.h"
#include "ASerialLdd2.h"
#include "ASerialLdd1.h"
#include "FLOWSENS_COMM.h"
#include "IR_TM_COMM.h"
#include "Global.h"
#include "Comm_Sbc.h"
#include "ModBusCommProt.h"
#include "Adc_ges.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#include "App_Ges.h"
#include "Temp_sensIR_protective.h"


extern bool WriteActive;
extern bool ReadActive;
extern int MyArrayIdx;

void Service_SBC(void){

	char cmdId;

	union NumFloatUnion{
		uint32 ieee754NUmFormat;
		float numFormatFloat;
	} numFloatSensor;

	char regId[4];
	for(char i=0; i<4; i++)
		regId[i] = 0x00;

	static char regIntId[4];
	static char valueIeee[10];
	int	valueInt;

	unsigned char rcvData[20];
	unsigned char * ptrData;
	unsigned char * ptrDatawR;
	unsigned char err;
	unsigned char stateSensTempIR;
	word ret;
	word snd;

	for(char i=0; i<20;i++)
	{
		rcvData[i] = 0;
	}
//	stateSensTempIR = 0;


	/*porta SBC_COMM - messaggio completo da sbc - contenuto in pcDebug_rx_data*/
	if(iflag_sbc_rx == IFLAG_SBC_RX)
	{
		iflag_sbc_rx = IFLAG_IDLE;

		/*decodifica messaggio*/
		switch(sbc_rx_data[5])
		{
			// Service
			case 0xCA:
			{
				switch(sbc_rx_data[6])
				{

					// Read adc and mmHg pressure values
					case 0x31:
					{
						word snd;
						ptrMsgSbcRx = &sbc_rx_data[0];
						buildPressSensReadValuesResponseMsg(ptrMsgSbcRx);
						ptrMsgSbcTx = &sbc_tx_data[0];
						SBC_COMM_SendBlock(ptrMsgSbcTx,myCommunicatorToSBC.numByteToSend,&snd);
					}
					break;

					// Read pressure sensors calibration values
					case 0x32:
					{
						word snd;
						ptrMsgSbcRx = &sbc_rx_data[0];
						buildPressSensReadParamResponseMsg(ptrMsgSbcRx);
						ptrMsgSbcTx = &sbc_tx_data[0];
						SBC_COMM_SendBlock(ptrMsgSbcTx,myCommunicatorToSBC.numByteToSend,&snd);
					}
					break;

					// Set pressure sensors zero point
					case 0x33:
					{
						//TODO Store zero point for calibration
						word snd;
						ptrMsgSbcRx = &sbc_rx_data[0];

						/*gli ID dei sensori delle GUI partono da 1, mentre i miei partono da 0 quindi sottraggo 1*/
						Press_sens IdSens 	= sbc_rx_data[7];
						//il valore mi arriva su due byte moltiplicato per dieci
						float value 			= BYTES_TO_WORD(sbc_rx_data[8], sbc_rx_data[9]);
						value = value /10;
						unsigned char point = 0;

						Pressure_Sensor_Calibration(IdSens,value,point);

						buildPressSensCalibResponseMsg(ptrMsgSbcRx);
						ptrMsgSbcTx = &sbc_tx_data[0];
						SBC_COMM_SendBlock(ptrMsgSbcTx,myCommunicatorToSBC.numByteToSend,&snd);
					}
					break;

					// Set pressure sensors load point
					case 0x34:
					{
						//TODO Store load point for calibration
						word snd;
						ptrMsgSbcRx = &sbc_rx_data[0];

						/*gli ID dei sensori delle GUI partono da 1, mentre i miei partono da 0 quindi sottraggo 1*/
						Press_sens IdSens 	= sbc_rx_data[7];
						//il valore mi arriva su due byte moltiplicato per dieci
						float value 			= BYTES_TO_WORD(sbc_rx_data[8], sbc_rx_data[9]);
						value = value /10;
						unsigned char point = 1;

						Pressure_Sensor_Calibration(IdSens,value,point);

						buildPressSensCalibResponseMsg(ptrMsgSbcRx);
						ptrMsgSbcTx = &sbc_tx_data[0];
						SBC_COMM_SendBlock(ptrMsgSbcTx,myCommunicatorToSBC.numByteToSend,&snd);
					}
					break;
					// Read temperature IR values
					case 0x40:
					{
						word snd;
						ptrMsgSbcRx = &sbc_rx_data[0];
						buildTempIRSensReadValuesResponseMsg(ptrMsgSbcRx);
						ptrMsgSbcTx = &sbc_tx_data[0];
						SBC_COMM_SendBlock(ptrMsgSbcTx,myCommunicatorToSBC.numByteToSend,&snd);
					}
					break;

					// Read temperature IR register
					case 0x41:
					{
						word snd;
						ptrMsgSbcRx = &sbc_rx_data[0];
						buildTempIRSensReadRegResponseMsg(ptrMsgSbcRx);
						ptrMsgSbcTx = &sbc_tx_data[0];
						SBC_COMM_SendBlock(ptrMsgSbcTx,myCommunicatorToSBC.numByteToSend,&snd);
					}
					break;

					// Write temperature IR register
					case 0x42:
					{
						//TODO write the register

						CHANGE_ADDRESS_IR_SENS = TRUE;
						unsigned char OldAddress = 0x5A;
						word snd;
						ptrMsgSbcRx = &sbc_rx_data[0];
						word NewAddress = BYTES_TO_WORD(ptrMsgSbcRx[9], ptrMsgSbcRx[10]);

						buildTempIRSensWriteRegResponseMsg(ptrMsgSbcRx);
						ptrMsgSbcTx = &sbc_tx_data[0];
						SBC_COMM_SendBlock(ptrMsgSbcTx,myCommunicatorToSBC.numByteToSend,&snd);


						/*resetto l'indirzzo*/
						buildCmdWriteTempSensIR(OldAddress, (EEPROM_ACCESS_COMMAND | SD_SMBUS_E2_ADDRESS), 0x0000);
						timerCounter=0;
						int wait = timerCounter;
						/*attendo 250 msec*/
						while ( timerCounter - wait < 30);

						/*scrivo il nuovo indirizzo*/
						buildCmdWriteTempSensIR(OldAddress, (EEPROM_ACCESS_COMMAND | SD_SMBUS_E2_ADDRESS), NewAddress);

						wait = timerCounter;
						/*attendo 250 msec*/
						while ( timerCounter - wait < 30);

//						/*resetto l'indirzzo*/
//						buildCmdWriteTempSensIR(OldAddress, (RAM_ACCESS_COMMAND | SD_SMBUS_E2_ADDRESS), 0x0000);
//
//		         		int wait = timerCounter;
//		         	    /*attendo 200 msec*/
//		         		while ( timerCounter - wait < 4);
//
//		         		/*scrivo il nuovo indirizzo*/
//		         	    buildCmdWriteTempSensIR(OldAddress, (RAM_ACCESS_COMMAND | SD_SMBUS_E2_ADDRESS), NewAddress);

		         	    /*successivamente dovrò disalimentare il sensore altrimenti il nuovo indirizzo non viene memorizzato*/
					}
					break;
					// Filippo - comando di taratura per il sensore PT1000 sul piatto
					case 0x43:
					{
						word snd1;
						ptrMsgSbcRx = &sbc_rx_data[0];

						/*gli ID dei sensori delle GUI partono da 1, mentre i miei partono da 0 quindi sottraggo 1*/
						//il valore mi arriva su due byte moltiplicato per dieci
						float value = BYTES_TO_WORD(sbc_rx_data[7], sbc_rx_data[8]);
						value = value /10;
						unsigned char point = 0;

						Plate_Temp_Sensor_Calibration(value);

						buildPT1000CalibResponseMsg(ptrMsgSbcRx);

						ptrMsgSbcTx = &sbc_tx_data[0];
						SBC_COMM_SendBlock(ptrMsgSbcTx,myCommunicatorToSBC.numByteToSend,&snd1);


						break;
					}
					// Filippo - comando di lettura dell'offset di calibrazione per il sensore PT1000 sul piatto
					case 0x44:
					{
						word snd;
						ptrMsgSbcRx = &sbc_rx_data[0];
						buildPT1000SensReadParamResponseMsg(ptrMsgSbcRx);
						ptrMsgSbcTx = &sbc_tx_data[0];
						SBC_COMM_SendBlock(ptrMsgSbcTx,myCommunicatorToSBC.numByteToSend,&snd);
					}


						break;

//					// Read temperature IR values
//					case 0x40:
//					{
//						word snd;
//						ptrMsgSbcRx = &sbc_rx_data[0];
//						buildTempIRSensReadValuesResponseMsg(ptrMsgSbcRx);
//						ptrMsgSbcTx = &sbc_tx_data[0];
//						SBC_COMM_SendBlock(ptrMsgSbcTx,myCommunicatorToSBC.numByteToSend,&snd);
//					}
//					break;
//
//					// Read temperature IR register
//					case 0x41:
//					{
//						word snd;
//						ptrMsgSbcRx = &sbc_rx_data[0];
//						buildTempIRSensReadRegResponseMsg(ptrMsgSbcRx);
//						ptrMsgSbcTx = &sbc_tx_data[0];
//						SBC_COMM_SendBlock(ptrMsgSbcTx,myCommunicatorToSBC.numByteToSend,&snd);
//					}
//					break;
//
//					// Write temperature IR register
//					case 0x42:
//					{
//						//TODO write the register
//
//						CHANGE_ADDRESS_IR_SENS = TRUE;
//						unsigned char OldAddress = 0x5A;
//						word snd;
//						ptrMsgSbcRx = &sbc_rx_data[0];
//						word NewAddress = BYTES_TO_WORD(ptrMsgSbcRx[9], ptrMsgSbcRx[10]);
//
//						buildTempIRSensWriteRegResponseMsg(ptrMsgSbcRx);
//						ptrMsgSbcTx = &sbc_tx_data[0];
//						SBC_COMM_SendBlock(ptrMsgSbcTx,myCommunicatorToSBC.numByteToSend,&snd);
//
//
//						/*resetto l'indirzzo*/
//						buildCmdWriteTempSensIR(OldAddress, (EEPROM_ACCESS_COMMAND | SD_SMBUS_E2_ADDRESS), 0x0000);
//
//						int wait = timerCounter;
//						/*attendo 250 msec*/
//						while ( timerCounter - wait < 6);
//
//						/*scrivo il nuovo indirizzo*/
//						buildCmdWriteTempSensIR(OldAddress, (EEPROM_ACCESS_COMMAND | SD_SMBUS_E2_ADDRESS), NewAddress);
//
//						wait = timerCounter;
//						/*attendo 250 msec*/
//						while ( timerCounter - wait < 6);
//
////						/*resetto l'indirzzo*/
////						buildCmdWriteTempSensIR(OldAddress, (RAM_ACCESS_COMMAND | SD_SMBUS_E2_ADDRESS), 0x0000);
////
////		         		int wait = timerCounter;
////		         	    /*attendo 200 msec*/
////		         		while ( timerCounter - wait < 4);
////
////		         		/*scrivo il nuovo indirizzo*/
////		         	    buildCmdWriteTempSensIR(OldAddress, (RAM_ACCESS_COMMAND | SD_SMBUS_E2_ADDRESS), NewAddress);
//
//		         	    /*successivamente dovrò disalimentare il sensore altrimenti il nuovo indirizzo non viene memorizzato*/
//					}
//					break;

					default:
					break;
				}
			}
			break;

			default:
			break;
		}

	}
}




