/*
 * Comm_Sbc.h
 *
 *  Created on: 10/nov/2016
 *      Author: W15
 */

#ifndef APPLICATION_COMM_SBC_H_
#define APPLICATION_COMM_SBC_H_

void buildModBusWriteRegActResponseMsg(char *ptrMsgSbcRx);
void buildModBusReadRegActResponseMsg(char *ptrMsgSbcRx,
									  char slaveAddr,
									  unsigned int readStartAddr,
									  unsigned int numRegisterRead);
void buildModBusReadStatusResponseMsg(char *ptrMsgSbcRx);
void buildPressSensReadValuesResponseMsg(char *ptrMsgSbcRx);
void buildPressSensReadParamResponseMsg(char *ptrMsgSbcRx);
void buildPressSensCalibResponseMsg(char *ptrMsgSbcRx);
void buildTempIRSensReadValuesResponseMsg(char *ptrMsgSbcRx);
void buildTempIRSensReadRegResponseMsg(char *ptrMsgSbcRx);
void buildTempIRSensWriteRegResponseMsg(char *ptrMsgSbcRx);
void buildReadFlowAirResponseMsg(char *ptrMsgSbcRx);
void buildReadFlowResetResponseMsg(char *ptrMsgSbcRx);
void buildPeltierReadFloatResponseMsg(char *ptrMsgSbcRx, char *ieee754ptr);
void buildPeltierReadIntResponseMsg(char *ptrMsgSbcRx, char *intptr);
void buildPeltierWriteFloatResponseMsg(char *ptrMsgSbcRx);
void buildPeltierWriteIntResponseMsg(char *ptrMsgSbcRx);
void buildPeltierStartResponseMsg(char *ptrMsgSbcRx);
void buildPeltierStopResponseMsg(char *ptrMsgSbcRx);
void buildPeltierWriteEEResponseMsg(char *ptrMsgSbcRx);
// Filippo - costruisco la risposta al comando di calibrazione sonda PT1000
void buildPT1000CalibResponseMsg(char *ptrMsgSbcRx);
// Filippo - funzione che risponde al comando di lettura parametro di calibrazione per il sensore PT1000
void buildPT1000SensReadParamResponseMsg(char *ptrMsgSbcRx);


//void buildPeltierResponseMsg(char code);
//void buildWriteTempSensResponseMsg(char code, char tempSensId);
//void buildReadTempSensResponseMsg(char code, char tempSensId);
//void buildReadIRTempRspMsg(char code, char tempIRSensId);

/********************/
/******* TREATMENT - SBC COMM. FUNCTION *********/
/*******************/
void initCommSBC(void);

void pollingSBCCommTreat(void);
void pollingDataToSBCTreat(void);

void buildRDMachineStateResponseMsg(char code, char subcode);
void buildButtonSBCResponseMsg(char code, char subcode, unsigned char buttonId);
void buildParamSetSBCResponseMsg(char code, char subcode, unsigned char paramId, unsigned char param_h, unsigned char param_l);

void initSetParamFromGUI(void);
void initSetParamInSourceCode(void);
void setParamWordFromGUI(unsigned char parId, int value);
void resetParamWordFromGUI(unsigned char parId);
word ConvertMMHgToMl( word mmhg);
int CalcPrimingDuration(word volume);
int CalcFilterFlow(unsigned char PumpSpeed);
word CalcHoursMin(int seconds);
void UpdateFilterFlowVal(void);

/*******************/
/******* TREATMENT - SBC COMM. FUNCTION *********/
/********************/

// Se definito, aggiunge alla risposta ai comandi di stato la lista dei valori di alcuni parametri
// attualmente impostati da utente
#define NEW_STATE_RESPONSE_MSG
#ifdef NEW_STATE_RESPONSE_MSG
	#define STATUS_MSG_LENGTH 0x87
#else
	#define STATUS_MSG_LENGTH 0x5F
#endif


#endif /* APPLICATION_COMM_SBC_H_ */
