/*
 * Debug_Routine.c
 *
 *  Created on: 03/nov/2016
 *      Author: W15
 */

#include "SBC_COMM.h"
#include "ASerialLdd5.h"
#include "PC_DEBUG_COMM.h"
#include "ASerialLdd2.h"
#include "MODBUS_COMM.h"
#include "ASerialLdd1.h"


#include "FLOWSENS_COMM.h"
#include "IR_TM_COMM.h"

#include "Global.h"
#include "Comm_Sbc.h"
#include "Peltier_Module.h"
#include "ModBusCommProt.h"
#include "Adc_ges.h"
#include "Flowsens.h"
#include "Temp_sensIR.h"

#include "string.h"
#include "stdio.h"
#include "stdlib.h"


void Service_SBC(void){

	char cmdId;
	unsigned int * valModBusArrayPtr;

	static char stCoeffA[10];
	static char stCoeffB[10];
	static char stCoeffC[10];
	static char regCoeffA[4];
	static char regCoeffB[4];
	static char regCoeffC[4];
	static char msgPeltierCalT1Wr = 0;

	static char tcDBand[10];
	static char tcLimit[10];
	static char setTemp[10];
	static char regTcDBand[4];
	static char regTcLimit[4];
	static char regSetTemp[4];
	static char msgPeltierDbLimSet = 0;

	static char onOffDBand[10];
	static char onOffHyst[10];
	static char	onOffValue = 255;
	static char regOnOffDBand[4];
	static char regOnOffHyst[4];
	static char regOnOffValue[4];
	static char msgPeltierOnOff = 0;

	static char fanHighSpeedVal[10];
	static char fanLowSpeedVal[10];
	static char fanValue = 255;
	static char regFanHighSpeed[4];
	static char regFanLowSpeed[4];
	static char regFanValue[4];
	static char msgPeltierFan = 0;

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
	for(char i=0; i<20;i++)
	{
		rcvData[i] = 0;
	}
	stateSensTempIR = 0;


	/*porta SBC_COMM - messaggio completo da sbc - contenuto in pcDebug_rx_data*/
	if(iflag_sbc_rx == IFLAG_SBC_RX)
	{
		iflag_sbc_rx = IFLAG_IDLE;

		/*decodifica messaggio*/
		switch(sbc_rx_data[5])
		{
			// Service
			case 0xCC:
			{
				switch(sbc_rx_data[6])
				{
					// Modbus write
					case 0x15:
					{
						/*mando il messaggio ricevuto dall SBC alle pompe/pinch solo se il buffer è libero
						 * ovvero se ho avuto l'ultima ricezione*/
						if(iFlag_actuatorCheck == IFLAG_COMMAND_RECEIVED)
						{
							iFlag_actuatorCheck = IFLAG_COMMAND_SENT;

							//build command for actuator
							byte slvAddr = sbc_rx_data[7];
							byte funcCode = 0x10;
							word wrAddrStart = BYTES_TO_WORD(sbc_rx_data[8], sbc_rx_data[9]);
							word wrAddrStop  = BYTES_TO_WORD(sbc_rx_data[10], sbc_rx_data[11]);
							word numberOfAddress = wrAddrStop - wrAddrStart + 1;

							if (numberOfAddress == 1)
							{
								int dato = BYTES_TO_WORD(sbc_rx_data[12], sbc_rx_data[13]);

								if (slvAddr >=2 && slvAddr <=5)
									setPumpSpeedValueHighLevel(slvAddr, dato);
								else if (slvAddr >=7 && slvAddr <=9)
									setPinchPositionHighLevel(slvAddr, dato);
							}
							else
							{
								for(int i = 0 ; i < numberOfAddress ; i++)
								{
									valModBusArray[i] = (sbc_rx_data[12+2*i] << 8)  + sbc_rx_data[13+2*i];
								}
								valModBusArrayPtr = &valModBusArray[0];

								_funcRetValPtr = ModBusWriteRegisterReq(slvAddr,
																		funcCode,
																		wrAddrStart,
																		numberOfAddress,
																		valModBusArrayPtr);
								//send command to actuator
								_funcRetVal.ptr_msg = _funcRetValPtr->ptr_msg;
								_funcRetVal.mstreqRetStructNumByte = _funcRetValPtr->mstreqRetStructNumByte;
								_funcRetVal.slvresRetPtr = _funcRetValPtr->slvresRetPtr;
								_funcRetVal.slvresRetNumByte = _funcRetValPtr->slvresRetNumByte;
							}


							word snd;
							MODBUS_COMM_SendBlock(_funcRetVal.ptr_msg,
												  _funcRetVal.mstreqRetStructNumByte,
												  &snd);

							//send answer to sbc
							ptrMsgSbcRx = &sbc_rx_data;
							buildModBusWriteRegActResponseMsg(ptrMsgSbcRx);
							ptrMsgSbcTx = &sbc_tx_data[0];
							SBC_COMM_SendBlock(ptrMsgSbcTx,myCommunicatorToSBC.numByteToSend,&snd);
						}
						else //altrimenti provo a rimandare il messaggio
							iflag_sbc_rx = IFLAG_SBC_RX;
					}
					break;

					// Modbus read
					case 0x16:
					{

						iFlag_actuatorCheck = IFLAG_COMMAND_SENT;

						byte slvAddr = sbc_rx_data[7];
						byte funcCode = 0x03;
						word readAddrStart = BYTES_TO_WORD(sbc_rx_data[8], sbc_rx_data[9]);
						word readAddrStop  = BYTES_TO_WORD(sbc_rx_data[10], sbc_rx_data[11]);
						word numberOfAddress = readAddrStop - readAddrStart + 1;

//						_funcRetValPtr = ModBusReadRegisterReq(slvAddr,
//															   funcCode,
//															   readAddrStart,
//															   numberOfAddress);
//
//						_funcRetVal.ptr_msg = _funcRetValPtr->ptr_msg;
//						_funcRetVal.mstreqRetStructNumByte = _funcRetValPtr->mstreqRetStructNumByte;
//						_funcRetVal.slvresRetPtr = _funcRetValPtr->slvresRetPtr;
//						_funcRetVal.slvresRetNumByte = _funcRetValPtr->slvresRetNumByte;
//
//						word snd;
//						MODBUS_COMM_SendBlock(_funcRetVal.ptr_msg,
//											  _funcRetVal.mstreqRetStructNumByte,
//											  &snd);

						//send answer to sbc
						word snd;
						ptrMsgSbcRx = &sbc_rx_data;
						buildModBusReadRegActResponseMsg(ptrMsgSbcRx, slvAddr, readAddrStart, numberOfAddress);
						ptrMsgSbcTx = &sbc_tx_data[0];
						SBC_COMM_SendBlock(ptrMsgSbcTx,myCommunicatorToSBC.numByteToSend,&snd);

					}
					break;

					case 0x17:
					{
						//send answer to sbc
						word snd;
						ptrMsgSbcRx = &sbc_rx_data;
						buildModBusReadStatusResponseMsg(ptrMsgSbcRx);
						ptrMsgSbcTx = &sbc_tx_data[0];

						SBC_COMM_SendBlock(ptrMsgSbcTx,myCommunicatorToSBC.numByteToSend,&snd);
					}
					break;

					// Modbus read/write
					/*case 0x17:
					{
						byte slvAddr = sbc_rx_data[7];
						byte funcCode = sbc_rx_data[8];

						unsigned int readAddr2 = BYTES_TO_WORD(sbc_rx_data[9], sbc_rx_data[10]);
						unsigned int numRegRead2 = BYTES_TO_WORD(sbc_rx_data[11], sbc_rx_data[12]);

						valModBusArray[1] = 0x14;
						valModBusArrayPtr = &valModBusArray[1];

						_funcRetValPtr = ModBusRWRegisterReq(slvAddr,
								   	   	   	   	   	   	   	 funcCode,
															 readAddr2,
															 numRegRead2,
															 0x0003,
															 1,
															 valModBusArrayPtr);

						_funcRetVal.ptr_msg = _funcRetValPtr->ptr_msg;
						_funcRetVal.mstreqRetStructNumByte = _funcRetValPtr->mstreqRetStructNumByte;
						_funcRetVal.slvresRetPtr = _funcRetValPtr->slvresRetPtr;
						_funcRetVal.slvresRetNumByte = _funcRetValPtr->slvresRetNumByte;

						for(char k = 0; k < _funcRetVal.mstreqRetStructNumByte; k++)
						{
							MODBUS_COMM_SendChar(*(_funcRetVal.ptr_msg+k));
						}
					}
					break;*/

					// Read adc and mmHg pressure values
					case 0x31:
					{
						word snd;
						ptrMsgSbcRx = &sbc_rx_data;
						buildPressSensReadValuesResponseMsg(ptrMsgSbcRx);
						ptrMsgSbcTx = &sbc_tx_data[0];
						SBC_COMM_SendBlock(ptrMsgSbcTx,myCommunicatorToSBC.numByteToSend,&snd);
					}
					break;

					// Read pressure sensors calibration values
					case 0x32:
					{
						word snd;
						ptrMsgSbcRx = &sbc_rx_data;
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
						ptrMsgSbcRx = &sbc_rx_data;
					//	buildPressSensCalibResponseMsg(ptrMsgSbcRx);
						ptrMsgSbcTx = &sbc_tx_data[0];
						SBC_COMM_SendBlock(ptrMsgSbcTx,myCommunicatorToSBC.numByteToSend,&snd);
					}
					break;

					// Set pressure sensors load point
					case 0x34:
					{
						//TODO Store load point for calibration
						word snd;
						ptrMsgSbcRx = &sbc_rx_data;
					//	buildPressSensCalibResponseMsg(ptrMsgSbcRx);
						ptrMsgSbcTx = &sbc_tx_data[0];
						SBC_COMM_SendBlock(ptrMsgSbcTx,myCommunicatorToSBC.numByteToSend,&snd);
					}
					break;

					// Read ADC and mmHg temperature IR values
					case 0x40:
					{
						word snd;
						ptrMsgSbcRx = &sbc_rx_data;
						buildTempIRSensReadValuesResponseMsg(ptrMsgSbcRx);
						ptrMsgSbcTx = &sbc_tx_data[0];
						SBC_COMM_SendBlock(ptrMsgSbcTx,myCommunicatorToSBC.numByteToSend,&snd);
					}
					break;

					// Read temperature IR register
					case 0x41:
					{
						word snd;
						ptrMsgSbcRx = &sbc_rx_data;
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
						ptrMsgSbcRx = &sbc_rx_data;
						word NewAddress = BYTES_TO_WORD(ptrMsgSbcRx[9], ptrMsgSbcRx[10]);

						buildTempIRSensWriteRegResponseMsg(ptrMsgSbcRx);
						ptrMsgSbcTx = &sbc_tx_data[0];
						SBC_COMM_SendBlock(ptrMsgSbcTx,myCommunicatorToSBC.numByteToSend,&snd);

						/*resetto l'indirzzo*/
						buildCmdWriteTempSensIR(OldAddress, (RAM_ACCESS_COMMAND | SD_SMBUS_E2_ADDRESS), 0x0000);

		         		int wait = timerCounter;
		         	    /*attendo 200 msec*/
		         		while ( timerCounter - wait < 4);

		         		/*scrivo il nuovo indirizzo*/
		         	    buildCmdWriteTempSensIR(OldAddress, (RAM_ACCESS_COMMAND | SD_SMBUS_E2_ADDRESS), NewAddress);

		         	    /*successivamente dovrò disalimentare il sensore altrimenti il nuovo indirizzo non viene memorizzato*/
					}
					break;
					// Flow sensor read values
					case 0x50:
					{
						word snd;
						ptrMsgSbcRx = &sbc_rx_data;
						buildReadFlowAirResponseMsg(ptrMsgSbcRx);
						ptrMsgSbcTx = &sbc_tx_data[0];
						SBC_COMM_SendBlock(ptrMsgSbcTx,myCommunicatorToSBC.numByteToSend,&snd);

					}
					// Flow sensor reset
					case 0x51:
					{
						word snd;
						ptrMsgSbcRx = &sbc_rx_data;
						buildReadFlowResetResponseMsg(ptrMsgSbcRx);
						ptrMsgSbcTx = &sbc_tx_data[0];
						SBC_COMM_SendBlock(ptrMsgSbcTx,myCommunicatorToSBC.numByteToSend,&snd);
					}
					break;
					// Peltier read float
					case 0x26:
					{
						sprintf(regId, "%u", sbc_rx_data[7]);
						PeltierAssSendCommand(READ_FLOAT_FROM_REG_XX,regId,0,"0",2);

						word snd;
						ptrMsgSbcRx = &sbc_rx_data;
						buildPeltierReadFloatResponseMsg(ptrMsgSbcRx, ptrMsgDataieee754start);
						ptrMsgSbcTx = &sbc_tx_data[0];
						SBC_COMM_SendBlock(ptrMsgSbcTx,myCommunicatorToSBC.numByteToSend,&snd);
					}
					break;
					// Peltier read int
					case 0x28:
					{
						sprintf(regId, "%u", sbc_rx_data[7]);
						PeltierAssSendCommand(READ_DATA_REGISTER_XX,regId,0,"0",2);

						word snd;
						ptrMsgSbcRx = &sbc_rx_data;
						buildPeltierReadIntResponseMsg(ptrMsgSbcRx, ptrMsgDataPeltierInt);
						ptrMsgSbcTx = &sbc_tx_data[0];
						SBC_COMM_SendBlock(ptrMsgSbcTx,myCommunicatorToSBC.numByteToSend,&snd);
					}
					break;
					// Peltier write float
					case 0x27:
					{
						sprintf(regId, "%u", sbc_rx_data[7]);
						sprintf(valueIeee, "%2X%2X%2X%2X", sbc_rx_data[8], sbc_rx_data[9], sbc_rx_data[10], sbc_rx_data[11]);
						PeltierAssSendCommand(WRITE_FLOAT_REG_XX,regId,0,valueIeee,2);

						word snd;
						ptrMsgSbcRx = &sbc_rx_data;
						buildPeltierWriteFloatResponseMsg(ptrMsgSbcRx);
						ptrMsgSbcTx = &sbc_tx_data[0];
						SBC_COMM_SendBlock(ptrMsgSbcTx,myCommunicatorToSBC.numByteToSend,&snd);
					}
					break;
					// Peltier write int
					case 0x29:
					{
						sprintf(regId, "%u", sbc_rx_data[7]);
						valueInt = sbc_rx_data[8];
						PeltierAssSendCommand(WRITE_DATA_REGISTER_XX,regId,valueInt,"0",2);

						word snd;
						ptrMsgSbcRx = &sbc_rx_data;
						buildPeltierWriteIntResponseMsg(ptrMsgSbcRx);
						ptrMsgSbcTx = &sbc_tx_data[0];
						SBC_COMM_SendBlock(ptrMsgSbcTx,myCommunicatorToSBC.numByteToSend,&snd);
					}
					break;
					// Peltier start
					case 0x20:
					{
						PeltierAssSendCommand(START_FLAG,"0",0,"0",2);

						word snd;
						ptrMsgSbcRx = &sbc_rx_data;
						buildPeltierStartResponseMsg(ptrMsgSbcRx);
						ptrMsgSbcTx = &sbc_tx_data[0];
						SBC_COMM_SendBlock(ptrMsgSbcTx,myCommunicatorToSBC.numByteToSend,&snd);
					}
					break;
					// Peltier stop
					case 0x21:
					{
						PeltierAssSendCommand(STOP_FLAG,"0",0,"0",2);

						word snd;
						ptrMsgSbcRx = &sbc_rx_data;
						buildPeltierStopResponseMsg(ptrMsgSbcRx);
						ptrMsgSbcTx = &sbc_tx_data[0];
						SBC_COMM_SendBlock(ptrMsgSbcTx,myCommunicatorToSBC.numByteToSend,&snd);
					}
					break;
					// Peltier write ee
					case 0x22:
					{
						PeltierAssSendCommand(WRITE_REG_VAL_TO_EEPROM, "0",0,"0",2);

						word snd;
						ptrMsgSbcRx = &sbc_rx_data;
						buildPeltierWriteEEResponseMsg(ptrMsgSbcRx);
						ptrMsgSbcTx = &sbc_tx_data[0];
						SBC_COMM_SendBlock(ptrMsgSbcTx,myCommunicatorToSBC.numByteToSend,&snd);
					}
					break;

/******************************************************************/
/******************************************************************/
/******************************************************************/

					case 0x01:
					{
						//build command for peltier
						//send command to peltier
						PeltierAssSendCommand(SHOW_CURRENT_SW,0,0,"0",2);
						//wait for response
					}
					break;

					case 0x02:
					{
						//build command for peltier
						//send command to peltier
						//PeltierAssSendCommand(SHOW_CURRENT_SW_INT,0,0,0);
						//PeltierAssSendCommand(READ_REG_REGISTER,0,0,0);
						for(char i=0; i<1; i++)
						{
							//while(iflagPeltierBusy == IFLAG_PELTIER_BUSY);
							PeltierAssSendCommand(READ_FLOAT_FROM_REG_XX,REG_0_SET_POINT+i,0,"0",2);
						}
						//wait for response
					}
					break;

					case 0x03:
					{
						/* when using the ieee 754 format the hex number shall be written as a string */
						PeltierAssSendCommand(WRITE_FLOAT_REG_XX, REG_0_SET_POINT, 0, "40133333",2);
					}
					break;

//					//peltier; write tcdband, tclimit, set temp
//					case 0x21:
//					{
//						msgPeltierDbLimSet = 3;
//						iflag_peltier_rx = IFLAG_PELTIER_RX;
//						sprintf(tcDBand, "%2X%2X%2X%2X", sbc_rx_data[8], sbc_rx_data[9], sbc_rx_data[10], sbc_rx_data[11]);
//						for(char i=0; i<8; i++)
//						{
//							if(tcDBand[i] == 0x20)
//								tcDBand[i] = 0x30;
//						}
//						sprintf(regTcDBand, "%u", sbc_rx_data[7]);
//
//						sprintf(tcLimit, "%2X%2X%2X%2X", sbc_rx_data[13], sbc_rx_data[14], sbc_rx_data[15], sbc_rx_data[16]);
//						for(char i=0; i<8; i++)
//						{
//							if(tcLimit[i] == 0x20)
//								tcLimit[i] = 0x30;
//						}
//						sprintf(regTcLimit, "%u", sbc_rx_data[12]);
//
//						sprintf(setTemp, "%2X%2X%2X%2X", sbc_rx_data[18], sbc_rx_data[19], sbc_rx_data[20], sbc_rx_data[21]);
//						for(char i=0; i<8; i++)
//						{
//							if(setTemp[i] == 0x20)
//								setTemp[i] = 0x30;
//						}
//						sprintf(regSetTemp, "%u", sbc_rx_data[17]);
//					}
//					break;
//
//					//peltier write 3 float; calibration of t1 ntc
//					case 0x22:
//					{
//						msgPeltierCalT1Wr = 3;
//						iflag_peltier_rx = IFLAG_PELTIER_RX;
//						sprintf(stCoeffA, "%2X%2X%2X%2X", sbc_rx_data[8], sbc_rx_data[9], sbc_rx_data[10], sbc_rx_data[11]);
//						for(char i=0; i<8; i++)
//						{
//							if(stCoeffA[i] == 0x20)
//								stCoeffA[i] = 0x30;
//						}
//						sprintf(regCoeffA, "%u", sbc_rx_data[7]);
//
//						sprintf(stCoeffB, "%2X%2X%2X%2X", sbc_rx_data[13], sbc_rx_data[14], sbc_rx_data[15], sbc_rx_data[16]);
//						for(char i=0; i<8; i++)
//						{
//							if(stCoeffB[i] == 0x20)
//								stCoeffB[i] = 0x30;
//						}
//						sprintf(regCoeffB, "%u", sbc_rx_data[12]);
//
//						sprintf(stCoeffC, "%2X%2X%2X%2X", sbc_rx_data[18], sbc_rx_data[19], sbc_rx_data[20], sbc_rx_data[21]);
//						for(char i=0; i<8; i++)
//						{
//							if(stCoeffC[i] == 0x20)
//								stCoeffC[i] = 0x30;
//						}
//						sprintf(regCoeffC, "%u", sbc_rx_data[17]);
//					}
//					break;
//
//					//peltier; write parameters for on / off mode
//					case 0x23:
//					{
//						msgPeltierOnOff = 3;
//						iflag_peltier_rx = IFLAG_PELTIER_RX;
//						sprintf(onOffDBand, "%2X%2X%2X%2X", sbc_rx_data[8], sbc_rx_data[9], sbc_rx_data[10], sbc_rx_data[11]);
//						for(char i=0; i<8; i++)
//						{
//							if(onOffDBand[i] == 0x20)
//								onOffDBand[i] = 0x30;
//						}
//						sprintf(regOnOffDBand, "%u", sbc_rx_data[7]);
//
//						sprintf(onOffHyst, "%2X%2X%2X%2X", sbc_rx_data[13], sbc_rx_data[14], sbc_rx_data[15], sbc_rx_data[16]);
//						for(char i=0; i<8; i++)
//						{
//							if(onOffHyst[i] == 0x20)
//								onOffHyst[i] = 0x30;
//						}
//						sprintf(regOnOffHyst, "%u", sbc_rx_data[12]);
//
//						onOffValue = sbc_rx_data[19];
//						sprintf(regOnOffValue, "%u", sbc_rx_data[17]);
//					}
//					break;
//
//					//peltier write 2 float - ad alarm thresholds
//					case 0x24:
//					{
//						msgPeltierToSendWr = 2;
//						iflag_peltier_rx = IFLAG_PELTIER_RX;
//						sprintf(dataFloatWriteHigh, "%2X%2X%2X%2X", sbc_rx_data[8], sbc_rx_data[9], sbc_rx_data[10], sbc_rx_data[11]);
//						for(char i=0; i<8; i++)
//						{
//							if(dataFloatWriteHigh[i] == 0x20)
//								dataFloatWriteHigh[i] = 0x30;
//						}
//						sprintf(regIdHigh, "%u", sbc_rx_data[7]);
//						msgPeltierToSendWr = 2;
//
//						sprintf(dataFloatWriteLow, "%2X%2X%2X%2X", sbc_rx_data[13], sbc_rx_data[14], sbc_rx_data[15], sbc_rx_data[16]);
//						for(char i=0; i<8; i++)
//						{
//							if(dataFloatWriteLow[i] == 0x20)
//								dataFloatWriteLow[i] = 0x30;
//						}
//						sprintf(regIdLow, "%u", sbc_rx_data[12]);
//
//						/*if(sbcDebug_rx_data[7] == 0x05)
//						{
//							sprintf(dataFloatWriteHigh, "%2x%2x%2x%2x", sbcDebug_rx_data[8], sbcDebug_rx_data[9], sbcDebug_rx_data[10], sbcDebug_rx_data[11]);
//							for(char i=0; i<8; i++)
//							{
//								if(dataFloatWriteHigh[i] == 0x20)
//									dataFloatWriteHigh[i] = 0x30;
//							}
//							msgPeltierToSend = 2;
//							PeltierAssSendCommand(WRITE_FLOAT_REG_XX, REG_72_ALARM_TEMP1_LOW, 0, dataFloatWrite);
//
//
//							sprintf(dataFloatWriteLow, "%2x%2x%2x%2x", sbcDebug_rx_data[13], sbcDebug_rx_data[14], sbcDebug_rx_data[15], sbcDebug_rx_data[16]);
//							for(char i=0; i<8; i++)
//							{
//								if(dataFloatWriteLow[i] == 0x20)
//									dataFloatWriteLow[i] = 0x30;
//							}
//
//							//PeltierAssSendCommand(WRITE_FLOAT_REG_XX, REG_71_ALARM_TEMP1_HIGH, 0, dataFloatWrite);
//							//PeltierAssSendCommand(WRITE_FLOAT_REG_XX, REG_72_ALARM_TEMP1_LOW, 0, dataFloatWrite);
//						}*/
//					}
//					break;
//
//					//Peltier fan
//					case 0x25:
//					{
//						msgPeltierFan = 3;
//						iflag_peltier_rx = IFLAG_PELTIER_RX;
//						sprintf(fanHighSpeedVal, "%2X%2X%2X%2X", sbc_rx_data[8], sbc_rx_data[9], sbc_rx_data[10], sbc_rx_data[11]);
//						for(char i=0; i<8; i++)
//						{
//							if(fanHighSpeedVal[i] == 0x20)
//								fanHighSpeedVal[i] = 0x30;
//						}
//						sprintf(regFanHighSpeed, "%u", sbc_rx_data[7]);
//
//						sprintf(fanLowSpeedVal, "%2X%2X%2X%2X", sbc_rx_data[13], sbc_rx_data[14], sbc_rx_data[15], sbc_rx_data[16]);
//						for(char i=0; i<8; i++)
//						{
//							if(fanLowSpeedVal[i] == 0x20)
//								fanLowSpeedVal[i] = 0x30;
//						}
//						sprintf(regFanLowSpeed, "%u", sbc_rx_data[12]);
//
//						fanValue = sbc_rx_data[19];
//						sprintf(regFanValue, "%u", sbc_rx_data[17]);
//					}
//					break;
//
//					//cal temperature sensr
//					case 0x35:
//					{
//						if(sbc_rx_data[7] == 0x10)
//						{
//							numFloatSensor.ieee754NUmFormat = 	(sbc_rx_data[8] << 24) |
//																(sbc_rx_data[9] << 16) |
//																(sbc_rx_data[10] << 8) |
//																(sbc_rx_data[11]);
//
//							sensor_TMx[sbc_rx_data[22]-6].tempSensGain = numFloatSensor.numFormatFloat;
//						}
//
//						if(sbc_rx_data[12] == 0x20)
//						{
//							numFloatSensor.ieee754NUmFormat = 	(sbc_rx_data[13] << 24) |
//																(sbc_rx_data[14] << 16) |
//																(sbc_rx_data[15] << 8) |
//																(sbc_rx_data[16]);
//
//							sensor_TMx[sbc_rx_data[22]-6].tempSensOffset = numFloatSensor.numFormatFloat;
//						}
//
//						if(sbc_rx_data[17] == 0x30)
//						{
//							numFloatSensor.ieee754NUmFormat = 	(sbc_rx_data[18] << 24) |
//																(sbc_rx_data[19] << 16) |
//																(sbc_rx_data[20] << 8) |
//																(sbc_rx_data[21]);
//
//							sensor_TMx[sbc_rx_data[22]-6].tempSensOffsetVal = numFloatSensor.numFormatFloat;
//						}
//
//						sensor_TMx[sbc_rx_data[22]-6].tempSensAdcPtr = sensor_TMx[sbc_rx_data[22]-6].readAdctPtr();
//						sensor_TMx[sbc_rx_data[22]-6].tempSensAdc = *sensor_TMx[sbc_rx_data[22]-6].tempSensAdcPtr;
//						sensor_TMx[sbc_rx_data[22]-6].tempSensValue = sensor_TMx[sbc_rx_data[22]-6].tempSensGain *
//																		((((float)sensor_TMx[sbc_rx_data[22]-6].tempSensAdc)/65535)*3.3 - sensor_TMx[sbc_rx_data[22]-6].tempSensOffsetVal) +
//																		sensor_TMx[sbc_rx_data[22]-6].tempSensOffset;
//
//						iflag_write_temp_sensor = IFLAG_WRITE_TEMP_SENSOR;
//					}
//					break;
//
//					//read adc temp sens
//					case 0x36:
//					{
//						iflag_read_temp_sensor = IFLAG_READ_TEMP_SENSOR;
//					}
//					break;
//
//					case 0x80:
//					{
//						if(sbc_rx_data[7] == 0x5A)
//							PeltierAssSendCommand(STOP_FLAG,"0",0,"0");
//					}
//					break;
//
//					case 0x90:
//					{
//						if(sbc_rx_data[7] == 0xA5)
//							PeltierAssSendCommand(START_FLAG,"0",0,"0");
//					}
//					break;

					default:{}
					break;
				}
			}
			break;

			default:{}
			break;
		}
		/*confezione risposta - contenuto in pcDebug_tx_data*/
		/*invio risposta*/
	}

/*************************************************************/
/*************************************************************/
/*************************************************************/

//	if((msgPeltierToSendWr != 0) && (iflag_peltier_rx == IFLAG_PELTIER_RX))
//	{
//		switch(msgPeltierToSendWr)
//		{
//		case 1:
//			msgPeltierToSendWr = msgPeltierToSendWr - 1;
//			PeltierAssSendCommand(WRITE_FLOAT_REG_XX, regIdHigh, 0, dataFloatWriteHigh);
//			iflag_peltier_rx = IFLAG_IDLE;
//			break;
//
//		case 2:
//			msgPeltierToSendWr = msgPeltierToSendWr - 1;
//			PeltierAssSendCommand(WRITE_FLOAT_REG_XX, regIdLow, 0, dataFloatWriteLow);
//			iflag_peltier_rx = IFLAG_IDLE;
//			break;
//
//		default:
//			break;
//		}
//
//	}
//
//	if((msgPeltierCalT1Wr != 0) && (iflag_peltier_rx == IFLAG_PELTIER_RX))
//	{
//		switch(msgPeltierCalT1Wr)
//		{
//		case 3:
//			msgPeltierCalT1Wr = msgPeltierCalT1Wr - 1;
//			PeltierAssSendCommand(WRITE_FLOAT_REG_XX, regCoeffA, 0, stCoeffA);
//			iflag_peltier_rx = IFLAG_IDLE;
//			break;
//
//		case 2:
//			msgPeltierCalT1Wr = msgPeltierCalT1Wr - 1;
//			PeltierAssSendCommand(WRITE_FLOAT_REG_XX, regCoeffB, 0, stCoeffB);
//			iflag_peltier_rx = IFLAG_IDLE;
//			break;
//
//		case 1:
//			msgPeltierCalT1Wr = msgPeltierCalT1Wr - 1;
//			PeltierAssSendCommand(WRITE_FLOAT_REG_XX, regCoeffC, 0, stCoeffC);
//			iflag_peltier_rx = IFLAG_IDLE;
//			break;
//
//		default:
//			break;
//		}
//	}
//
//	if((msgPeltierDbLimSet != 0) && (iflag_peltier_rx == IFLAG_PELTIER_RX))
//	{
//		switch(msgPeltierDbLimSet)
//		{
//		case 3:
//			msgPeltierDbLimSet = msgPeltierDbLimSet - 1;
//			PeltierAssSendCommand(WRITE_FLOAT_REG_XX, regTcDBand, 0, tcDBand);
//			iflag_peltier_rx = IFLAG_IDLE;
//			break;
//
//		case 2:
//			msgPeltierDbLimSet = msgPeltierDbLimSet - 1;
//			PeltierAssSendCommand(WRITE_FLOAT_REG_XX, regTcLimit, 0, tcLimit);
//			iflag_peltier_rx = IFLAG_IDLE;
//			break;
//
//		case 1:
//			msgPeltierDbLimSet = msgPeltierDbLimSet - 1;
//			PeltierAssSendCommand(WRITE_FLOAT_REG_XX, regSetTemp, 0, setTemp);
//			iflag_peltier_rx = IFLAG_IDLE;
//			break;
//
//		default:
//			break;
//		}
//	}
//
//	if((msgPeltierOnOff != 0) && (iflag_peltier_rx == IFLAG_PELTIER_RX))
//	{
//		switch(msgPeltierOnOff)
//		{
//		case 3:
//			msgPeltierOnOff = msgPeltierOnOff - 1;
//			PeltierAssSendCommand(WRITE_FLOAT_REG_XX, regOnOffDBand, 0, onOffDBand);
//			iflag_peltier_rx = IFLAG_IDLE;
//			break;
//
//		case 2:
//			msgPeltierOnOff = msgPeltierOnOff - 1;
//			PeltierAssSendCommand(WRITE_FLOAT_REG_XX, regOnOffHyst, 0, onOffHyst);
//			iflag_peltier_rx = IFLAG_IDLE;
//			break;
//
//		case 1:
//			msgPeltierOnOff = msgPeltierOnOff - 1;
//			PeltierAssSendCommand(WRITE_DATA_REGISTER_XX, regOnOffValue, onOffValue, 0);
//			iflag_peltier_rx = IFLAG_IDLE;
//			break;
//
//		default:
//			break;
//		}
//	}
//
//	if((msgPeltierFan != 0) && (iflag_peltier_rx == IFLAG_PELTIER_RX))
//	{
//		switch(msgPeltierFan){
//		case 3:
//			msgPeltierFan = msgPeltierFan - 1;
//			PeltierAssSendCommand(WRITE_FLOAT_REG_XX, regFanHighSpeed, 0, fanHighSpeedVal);
//			iflag_peltier_rx = IFLAG_IDLE;
//			break;
//
//		case 2:
//			msgPeltierFan = msgPeltierFan - 1;
//			PeltierAssSendCommand(WRITE_FLOAT_REG_XX, regFanLowSpeed, 0, fanLowSpeedVal);
//			iflag_peltier_rx = IFLAG_IDLE;
//			break;
//
//		case 1:
//			msgPeltierFan = msgPeltierFan - 1;
//			PeltierAssSendCommand(WRITE_DATA_REGISTER_XX, regFanValue, fanValue, 0);
//			iflag_peltier_rx = IFLAG_IDLE;
//			break;
//
//		default:
//			break;
//		}
//	}
//
//	if(iflag_peltier_rx == IFLAG_PELTIER_RX)
//	{
//		iflag_peltier_rx = IFLAG_IDLE;
//
//		//build peltier response
//		cmdId = sbc_rx_data[6] & 0x66;
//		buildPeltierResponseMsg(cmdId);
//		ptrMsgSbcTx = &sbc_tx_data[0];
//
//		for(char i = 0; i < 14 ; i++)
//		{
//			SBC_COMM_SendChar(*(ptrMsgSbcTx+i));
//
//			#ifdef	DEBUG_COMM_SBC
//			//PC_DEBUG_COMM_SendChar(*(ptrMsgSbcTx+i));
//			#endif
//		}
//	}
//
//	//temp sensor write
//	if(iflag_write_temp_sensor == IFLAG_WRITE_TEMP_SENSOR){
//		iflag_write_temp_sensor = IFLAG_IDLE;
//		cmdId = sbc_rx_data[6];
//		buildWriteTempSensResponseMsg(cmdId, (sbc_rx_data[22]-6));
//		for(char i = 0; i < 14 ; i++)
//		{
//			SBC_COMM_SendChar(*(ptrMsgSbcTx+i));
//
//			#ifdef	DEBUG_COMM_SBC
//			//PC_DEBUG_COMM_SendChar(*(ptrMsgSbcTx+i));
//			#endif
//		}
//	}
//
//	//temp sensor read
//	if(iflag_read_temp_sensor == IFLAG_READ_TEMP_SENSOR){
//		iflag_read_temp_sensor = IFLAG_IDLE;
//		cmdId = sbc_rx_data[6];
//		buildReadTempSensResponseMsg(cmdId, (sbc_rx_data[7]-6));
//		for(char i = 0; i < 24 ; i++)
//		{
//			SBC_COMM_SendChar(*(ptrMsgSbcTx+i));
//
//			#ifdef	DEBUG_COMM_SBC
//			//PC_DEBUG_COMM_SendChar(*(ptrMsgSbcTx+i));
//			#endif
//		}
//	}
}
