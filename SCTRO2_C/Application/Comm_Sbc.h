/*
 * Comm_Sbc.h
 *
 *  Created on: 10/nov/2016
 *      Author: W15
 */

#ifndef APPLICATION_COMM_SBC_H_
#define APPLICATION_COMM_SBC_H_

void buildModBusActResponseMsg(char *ptrMsgSbcRx, char *ptrMsgModbusRx);

void buildPeltierResponseMsg(char code);
void buildWritePressSensResponseMsg(char code, char pressSensId);
void buildReadPressSensResponseMsg(char code, char pressSensId);
void buildWriteTempSensResponseMsg(char code, char tempSensId);
void buildReadTempSensResponseMsg(char code, char tempSensId);
void buildReadIRTempRspMsg(char code, char tempIRSensId);
void buildReadFlowArtRspMsg(char code, char flowSensId);

/********************/
/******* TREATMENT - SBC COMM. FUNCTION *********/
/*******************/
void initCommSBC(void);

void pollingSBCCommTreat(void);
void pollingDataToSBCTreat(void);

void buildRDMachineStateResponseMsg(char code, char subcode);
void buildButtonSBCResponseMsg(char code, char subcode, unsigned char buttonId);
void buildParamSetSBCResponseMsg(char code, char subcode, unsigned char paramId, unsigned char param_h, unsigned char param_l);

/*******************/
/******* TREATMENT - SBC COMM. FUNCTION *********/
/********************/



#endif /* APPLICATION_COMM_SBC_H_ */
