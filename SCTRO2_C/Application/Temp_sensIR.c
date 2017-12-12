/*
 * Temp_sensIR.c
 *
 *  Created on: 06/mar/2017
 *      Author: W15
 */

#include "Temp_sensIR.h"
#include "Global.h"
#include "PE_Types.h"
#include "IR_TM_COMM.h"

#define NUM_BYTE_CRC	4
#define NUM_BYTE_RCV	3
#define NUM_SIZE_BLOCK_WRITE 4

void initTempSensIR(void){
	sensorIR_TM[0].sensorId = 0;
	sensorIR_TM[0].tempSensGain = 1;
	sensorIR_TM[0].tempSensOffset = 0;
	sensorIR_TM[0].tempSensOffsetVal = 0;
	sensorIR_TM[0].tempSensValue = 0;
	sensorIR_TM[0].tempSensValueOld = 0;
	sensorIR_TM[0].tempSensAdc = 0;

	for(char i = 0; i<4; i++)
	{
		sensorIR_TM[0].bufferReceived[i] = 0;
		sensorIR_TM[0].bufferToSend[i] = 0;
	}

	/*unsigned char testChar;
	unsigned char buffer[NUM_BYTE_CRC];
	unsigned char * ptrChar;
	buffer[0] = 0xFF;
	buffer[1] = 0xFF;
	buffer[2] = 0xFF;
	buffer[3] = 0xFF;
	ptrChar = &buffer[0];
	testChar = computeCRC8TempSens(ptrChar);
	ptrChar = ptrChar + 1;*/
	iflag_sensTempIRRW = IFLAG_IDLE;
	iflag_sensTempIR_Meas_Ready = IFLAG_IDLE;
	/*unsigned char * ptrData;
	ptrData = buildCmdReadTempSensIR(0x5A, SD_TOBJ1_RAM_ADDRESS, 0);
	ptrData = ptrData + 1;*/
}

void Manage_IR_Sens_Temp(void)
{
   static unsigned char Address = FIRST_IR_TEMP_SENSOR; /*lo metto static così non la inizializza tutte le vote*/

	/*interrogo su bus I2C ogni 200 msec, quindi ogni sensore sarà interrogato ogni 600 msec perché abbiamo 3 sensori*/
   if (CHANGE_ADDRESS_IR_SENS == FALSE && timerCounterCheckTempIRSens >=4)
    {
    //	alwaysIRTempSensRead();
 		timerCounterCheckTempIRSens = 0;

 		/*se prima avevo ricevuto un NACK, resetto il flag altrimenti non ricevo più*/
 		if (ON_NACK_IR_TM)
 			ON_NACK_IR_TM = FALSE;

 		/*costruisco il comando di lettura per il sensore di temperatura con indirizzo pari ad Address
 		 * Quando scatterà l'interrupt di trasmissione 'void IR_TM_COMM_OnTransmitData(void)'
 		 * andrò ad effettuare la lettura con 'IR_TM_COMM_RecvBlock(ptrDataTemperatureIR, 3, &ret);'*/
 		ptrDataTemperatureIR = buildCmdReadTempSensIR(Address, (RAM_ACCESS_COMMAND | SD_TOBJ1_RAM_ADDRESS), 0);
 		/*incremento l'indirizzo così al prossimo giro interrogo il sensore successivo*/
 		Address++;
    }

	/* Quando scatterà l'interrupt di ricezione 'IR_TM_COMM_OnReceiveData(void) sarà comandata
	 * la send Stop 'IR_TM_COMM_SendStop() e alzato il flag iflag_sensTempIR_Meas_Ready = IFLAG_IRTEMP_MEASURE_READY
	 * con cui potrò copiare il dato ricevuto*/

 	if (iflag_sensTempIR_Meas_Ready == IFLAG_IRTEMP_MEASURE_READY && ON_NACK_IR_TM == FALSE)
 	{
 		/*se Ho spedito il comando al sensore con address 0x01, nell'if precedente
 		 * Address è già stato incrementato per essere pronto alla nuova spedzione, quindi
 		 * sarà 0x02 ma sto sempre aspettando la ricezione del primo sensore quindi
 		 * con indice dell'array pari a 0 e devo quindi sottrarre due all'address*/
 		unsigned char index_array = Address -2;

 		/*devo salvare il dato solo dopo aver fatto il controllo della PEC Packet Error Code*/

 		/*Vado a copiarmi il dato ricevuto dal sensore IR solo se non ho ricevuto un NACK*/
 		sensorIR_TM[index_array].tempSensValue = (float)((BYTES_TO_WORD(sensorIR_TM[0].bufferReceived[1], sensorIR_TM[0].bufferReceived[0]))*((float)0.02)) - (float)273.15;

 		iflag_sensTempIR_Meas_Ready = IFLAG_IDLE;

 		/*se l'indirizzo supera l'ultimo sensore, resetto l'indirizzo al primo sensore per rifare il giro dei 3*/
 		if (Address > LAST_IR_TEMP_SENSOR)
 			Address = FIRST_IR_TEMP_SENSOR;
 	}
}

//void alwaysIRTempSensRead(void){
//	static unsigned char tempState = 0;
//	unsigned char err;
//	unsigned char * ptrCRC;
////	static unsigned char * ptrDataTemp; //se non lo metto static, alla prossima chiamata della funzione lui me lo inizializza a null
//	word ret;
//	static int wait = 0;
//
//	void * ptrDataOne;
//	static unsigned char dummy = 0;
//
//		/*Se è arrivato un NACK sul bus I2C con cui comunico con  i sensori IR,
//		 * devo far ripartire la macchina a stati dall'inizio*/
//		if (ON_NACK_IR_TM)
//		{
//			/*Nel caso in cui il bus I2C mi abbia risposto
//			 * con un NACK mando uno stop per ricominicare
//			 * la trasmisisone altrimenti si inchioda*/
//			IR_TM_COMM_SendStop();
//
//			tempState = 0;
//			/*resetto anche tutti i flag per ricominciare daccapo*/
//			iflag_sensTempIR = IFLAG_IDLE;
//			iflag_sensTempIRRW = IFLAG_IDLE;
//			iflag_sensTempIR_Meas_Ready = IFLAG_IDLE;
//			/*resetto il flag così da non ricadere più in questo if
//			 * se non allo scattare di un nuovo NACK*/
//			ON_NACK_IR_TM = FALSE;
//		}
//
//		switch(tempState){
//		case 0:
//			if(iflag_sensTempIR == IFLAG_IDLE)
//			{
//				//IR_TM_COMM_Enable();
//				//id sensore
//				//indirizzo da leggere
//				ptrDataTemperatureIR = buildCmdReadTempSensIR(/*0x5A*/0x02, (RAM_ACCESS_COMMAND | SD_TOBJ1_RAM_ADDRESS), 0);
//				//ptrDataTemp = buildCmdWriteTempSensIR(0x5A, (EEPROM_ACCESS_COMMAND | SD_SMBUS_E2_ADDRESS), 0x05);
//				//ptrDataOne = ptrDataTemp;
//				iflag_sensTempIR = IFLAG_SENS_TEMPIR_WAIT;
//				tempState = 1;
//			}
//			break;
//
//		case 1:
//			dummy++;
//			if(iflag_sensTempIR == IFLAG_SENS_TEMPIR_TX)
//			{
//				//IR_TM_COMM_SelectSlave(0x01);
//				//ptrDataOne = &sensorIR_TM[0].bufferReceived[0];
//			//	err = IR_TM_COMM_RecvBlock(ptrDataTemperatureIR, 3, &ret);
//
//				if (err != ERR_OK)
//				{
//					int a = 0;
//				}
//
//				iflag_sensTempIR = IFLAG_IDLE;
//
//				tempState = 2;
//				dummy = 0;
//			}
//			if (dummy >4)
//			{
//				ON_NACK_IR_TM = TRUE;
//				dummy = 0;
//			}
//			//dummy = dummy + 1;
//			break;
//
//		case 2:
//			dummy++;
//			if(iflag_sensTempIR_Meas_Ready == IFLAG_IRTEMP_MEASURE_READY)
//			{
//				/*
//				 * vorrei impostare il controllo della PEC in ricezione ma non riesco a calcolarla correttamente e non è mai uguale a quella ricevuta
//				unsigned char PEC;
//				unsigned char PEC_RX;
//				unsigned char command = RAM_ACCESS_COMMAND | SD_TOBJ1_RAM_ADDRESS;
//				unsigned char TempSensAddr;
//
//				TempSensAddr = 0x02;
//				PEC_RX = sensorIR_TM[0].bufferReceived[2];
//				ptrCRC = &sensorIR_TM[0].bufferReceived[0];
//				PEC = computeCRC8TempSensRx(ptrCRC,command,TempSensAddr);
//
//				if (PEC_RX == PEC)
//
//			 */
//				/*copio ilm dato solo se non ho ricevuto tutti FF ossia il sensore non mi ha risposto
//				 * e ho campionato la risposta con un segnale sempre alto*/
////				if (sensorIR_TM[0].bufferReceived[0] != 0xFF ||
////					sensorIR_TM[0].bufferReceived[1] != 0xFF ||
////					sensorIR_TM[0].bufferReceived[2] != 0xFF)
//
//				//	sensorIR_TM[0].tempSensValue = (float)((BYTES_TO_WORD(sensorIR_TM[0].bufferReceived[1], sensorIR_TM[0].bufferReceived[0]))*((float)0.02)) - (float)273.15;
//
//				if (sensorIR_TM[0].tempSensValue > 1000)
//				{
//					IR_Overflow++;
//				}
//				else
//					IR_OK++;
//
//				iflag_sensTempIR_Meas_Ready = IFLAG_IDLE;
//
//				tempState = 0;
//				dummy = 0;
//
//			}
//			if (dummy >4)
//			{
//				dummy = 0;
//				ON_NACK_IR_TM = TRUE;
//			}
//			/*va aggiunto un time out che se entro 50 ms non ricevo risposta, ricomincio da capo esempio impostando il NACK*/
//			break;
//
//		case 3:
////			wait = timerCounter;
////			tempState = 4;
//		//	tempState = 0;
//			break;
//
//		case 4:
//			/*inizio una nuova trasmissione solo se sono passati almeno 50 msec*/
////			if (timerCounter - wait >= 0)
////				tempState = 0;
//			break;
//
//		default:
//			tempState = 0;
//		break;
//		}
//
//}

unsigned char computeCRC8TempSens(unsigned char buffer[]){
	word registerCRC = 0x0000;
	word polinomial = 0x0107;
	word maskByte = 0x8000;


	for(int a = 0; a<NUM_BYTE_CRC; a++)
	{
		registerCRC = ((registerCRC ^ buffer[a]) << 8);

		for(int i = 0; i<8; i++)
		{
			if(registerCRC & (maskByte >> i))
			{
				registerCRC = registerCRC ^ (polinomial<<(8-i-1));
			}
		}
	}

	return (unsigned char) (registerCRC & 0x00FF);
}

unsigned char computeCRC8TempSensRx(unsigned char buffer[],unsigned char command, unsigned char TempSensAddr){
	word registerCRC = 0x0000;
	word polinomial = 0x0107;
	word maskByte = 0x8000;
	unsigned char bufferPEC[3];

	bufferPEC[0] = TempSensAddr * 2;
	bufferPEC[1] = command;
	bufferPEC[2] = buffer[0];
	bufferPEC[3] = buffer[1];

	for(int a = 0; a<4; a++)
	{
		registerCRC = ((registerCRC ^ bufferPEC[a]) << 8);

		for(int i = 0; i<8; i++)
		{
			if(registerCRC & (maskByte >> i))
			{
				registerCRC = registerCRC ^ (polinomial<<(8-i-1));
			}
		}
	}

	return (unsigned char) (registerCRC & 0x00FF);
}

unsigned char * buildCmdReadTempSensIR(unsigned char  tempSensAddress,
									   unsigned char  command,
									   word dataWordTx)
{
	unsigned char sensTempIRId;
	unsigned char err;
	unsigned char rcvData[NUM_BYTE_RCV];
	unsigned char * ptrData;
	word ret;


	if(tempSensAddress == 0x5A)
		sensTempIRId = sensorIR_TM[0].sensorId;

	/* Start + slave address + write command */
	IR_TM_COMM_SelectSlave(tempSensAddress);
	err = IR_TM_COMM_SendChar(command);
	/* Restart + slave address + received data */
	//IR_TM_COMM_SelectSlave(tempSensAddress);
	//if(err == ERR_OK)
		//IR_TM_COMM_RecvBlock(&rcvData[0], NUM_BYTE_RCV, &ret);
	//ptrData = &rcvData[0];
	ptrData = &sensorIR_TM[0].bufferReceived[0];
	//IR_TM_COMM_RecvBlock(ptrData, 3, &ret);

	return	ptrData;

}

unsigned char * buildCmdWriteTempSensIR(unsigned char  tempSensAddress,
		   	   	   	   	   	   	   	    unsigned char  command,
										word dataWordTx)
{
	unsigned char	byteTx_L;
	unsigned char	byteTx_H;
	unsigned char	byteCRC;
	unsigned char * ptrBuffer;
	unsigned char * ptrCRC;
	unsigned char	error;
	unsigned char   nullByte;
	unsigned char	buffCRC[5];
	word sizeBlock;
	word  dataSent;

	sizeBlock = NUM_SIZE_BLOCK_WRITE;
	for(char i = 0; i<5; i++)
		buffCRC[i] = 0x00;


	/* Start + slave address + write command */
	IR_TM_COMM_SelectSlave(tempSensAddress);
	/**/
	buffCRC[0] = tempSensAddress*2;

	sensorIR_TM[0].bufferToSend[0] = command;
	buffCRC[1] = sensorIR_TM[0].bufferToSend[0];
//	error = IR_TM_COMM_SendChar(sensorIR_TM[0].bufferToSend[0]);

	byteTx_L = (unsigned char) (dataWordTx & 0x00FF);
	sensorIR_TM[0].bufferToSend[1] = byteTx_L;
	buffCRC[2] = sensorIR_TM[0].bufferToSend[1];
//	error = IR_TM_COMM_SendChar(sensorIR_TM[0].bufferToSend[1]);

	byteTx_H = (unsigned char) ((dataWordTx >> 8) & 0x00FF);
	sensorIR_TM[0].bufferToSend[2] = byteTx_H;
	buffCRC[3] = sensorIR_TM[0].bufferToSend[2];
//	error = IR_TM_COMM_SendChar(sensorIR_TM[0].bufferToSend[2]);

	ptrCRC = &buffCRC[0];
	byteCRC = computeCRC8TempSens(ptrCRC);
	sensorIR_TM[0].bufferToSend[3] = byteCRC;
//	error = IR_TM_COMM_SendChar(sensorIR_TM[0].bufferToSend[3]);

	ptrBuffer = &sensorIR_TM[0].bufferToSend[0];
	error = IR_TM_COMM_SendBlock(ptrBuffer, sizeBlock, &dataSent);
	//if(error == ERR_OK)
		//IR_TM_COMM_SendStop();

	iflag_sensTempIRRW = IFLAG_SENS_TEMPIR_WRITE;


	return ptrBuffer;
}

