/*
 * Temp_sensIR.c
 *
 *  Created on: 06/mar/2017
 *      Author: W15
 */


#include "PE_Types.h"
#include "Global.h"
#include "Temp_sensIR.h"
#include "IR_TM_COMM.h"
#include "ControlProtectiveInterface.h"

#define NUM_BYTE_CRC	4
#define NUM_BYTE_CRC_RX 5
#define NUM_BYTE_RCV	3
#define NUM_SIZE_BLOCK_WRITE 4

void initTempSensIR(void){

	for (char i=0; i<3; i++)
	{
		sensorIR_TM[i].sensorId 		 = 0;
		sensorIR_TM[i].tempSensGain 	 = 1;
		sensorIR_TM[i].tempSensOffset 	 = 0;
		sensorIR_TM[i].tempSensOffsetVal = 0;
		sensorIR_TM[i].tempSensValue 	 = 0;
		sensorIR_TM[i].tempSensValueOld  = 0;
		sensorIR_TM[i].tempSensAdc 		 = 0;
		sensorIR_TM[i].errorNACK		 = 0;
		sensorIR_TM[i].errorPEC			 = 0;
		sensorIR_TM[i].ErrorMSG			 = 0;

		for(char j = 0; j<4; j++)
		{
			sensorIR_TM[i].bufferReceived[j] = 0;
			sensorIR_TM[i].bufferToSend[j]	 = 0;
		}

	}

	iflag_sensTempIRRW = IFLAG_IDLE;
	iflag_sensTempIR_Meas_Ready = IFLAG_IDLE;

}
/*funzione che chiede ogni 200 msec ad un sensore di temperatura IR il valoire; se il sensore risponde correttamente (con la corretta PEC)
 * il valore sa� memorizzato; ogni sensore sar� interrogato ogni 600 msec in quanto abbiamo 3 sensori, i valori saranno
 * sensorIR_TM[0].tempSensValue --> temperatura del flusso di perfusione arteriosa
 * sensorIR_TM[1].tempSensValue --> temperatura del liquido vaschetta
 * sensorIR_TM[2].tempSensValue --> temperatura del flusso di perfusione venosa */
void Manage_IR_Sens_Temp(void)
{
   static unsigned char Address = FIRST_IR_TEMP_SENSOR; /*lo metto static cos� non la inizializza tutte le vote*/

	/*interrogo su bus I2C ogni 200 msec, quindi ogni sensore sar� interrogato ogni 600 msec perch� abbiamo 3 sensori*/
   if (CHANGE_ADDRESS_IR_SENS == FALSE && timerCounterCheckTempIRSens >= 4)
    {
 		timerCounterCheckTempIRSens = 0;

 		/*se prima avevo ricevuto un NACK, resetto il flag altrimenti non ricevo pi�*/
 		if (ON_NACK_IR_TM)
 		{
 			/*incremento il contatore di errori NACK*/
 			sensorIR_TM[Address-2].errorNACK++;
 			ON_NACK_IR_TM = FALSE;
 		}
 		if (Address > LAST_IR_TEMP_SENSOR)
 			Address = FIRST_IR_TEMP_SENSOR;

 		/*costruisco e invio !! il comando di lettura per il sensore di temperatura con indirizzo pari ad Address
 		 * Quando scatter� l'interrupt di trasmissione 'void IR_TM_COMM_OnTransmitData(void)'
 		 * andr� ad effettuare la lettura con 'IR_TM_COMM_RecvBlock(ptrDataTemperatureIR, 3, &ret);'*/
 		ptrDataTemperatureIR = buildCmdReadTempSensIR(Address, (RAM_ACCESS_COMMAND | SD_TOBJ1_RAM_ADDRESS), 0);
 		/*incremento i possibili errori dpovuti a non risposte o a risposte con PEC errate; se ricevo benbe resetto questo contatore*/
 		sensorIR_TM[Address].ErrorMSG++;
 		Address++;
    }

	/* Quando scatter� l'interrupt di ricezione 'IR_TM_COMM_OnReceiveData(void) sar� comandata
	 * la send Stop 'IR_TM_COMM_SendStop() e alzato il flag iflag_sensTempIR_Meas_Ready = IFLAG_IRTEMP_MEASURE_READY
	 * con cui potr� copiare il dato ricevuto*/

 	if (iflag_sensTempIR_Meas_Ready == IFLAG_IRTEMP_MEASURE_READY && ON_NACK_IR_TM == FALSE)
 	{
 		/*mi vado a memorizzare quale lo slave Address su cui ho inviato l'ultimo comando
 		 * in quanto dopo l'invio del comando l'Address � stato incrementato*/
 		unsigned char Slave_Address_Sent = Address -1;

 		/*se Ho spedito il comando al sensore con indirizzo pari a 'Addres' ma nell'if precedente
 		 * Address � gi� stato incrementato per essere pronto alla nuova spedzione;io per�
 		 * sto sempre aspettando la ricezione del precedente sensore quindi
 		 * l'indice Array (che parte da 0) lo metto a Slave_Address_Sent (che parte da 1) -1 */
 		unsigned char index_array = Slave_Address_Sent -1;
 		unsigned char * ptrChar;

 		/* index_array == 0 :
 		 * sensore con conettore 4 pin, indirizzo '0x01'. montato nela parte anteriore vicino al sensore di flusso,
 		 * Misura la temperatura del flusso di perfusione arteriosa
 		 * index_array == 1 :
 		 * sensore con conettore 5 pin, indirizzo '0x02'. montato nela parte anteriore vicino le pinch,
 		 * Misura la temperatura del flusido della vaschetta
 		 * index_array == 2 :
 		 * sensore con conettore 6 pin, indirizzo '0x03'. montato nela parte laterale vicino al sensore di flusso,
 		 * Misura la temperatura del flusso di perfusione Venosa
 		 *   */
 		ptrChar = &sensorIR_TM[index_array].bufferReceived[0];

 		/*devo salvare il dato solo dopo aver fatto il controllo della PEC Packet Error Code*/
 		/*Vado a copiarmi il dato ricevuto dal sensore IR solo se non ho ricevuto un NACK
 		 * e se torna LA PEC ricevuta con quella calcolata*/
 		if (computeCRC8TempSensRx(ptrChar,(RAM_ACCESS_COMMAND | SD_TOBJ1_RAM_ADDRESS),Slave_Address_Sent) )
 		{
 			sensorIR_TM[index_array].tempSensValue = (float)((BYTES_TO_WORD(sensorIR_TM[index_array].bufferReceived[1], sensorIR_TM[index_array].bufferReceived[0]))*((float)0.02)) - (float)273.15;
 			/*ho ricevuto bene, resetto il contatore consecutivo di errore*/
 			sensorIR_TM[index_array].ErrorMSG = 0;
 			// invio le temperature
 			switch(index_array){
 			case 0:	// T perf. arteriosa
 				onNewTPerfArteriosa(sensorIR_TM[index_array].tempSensValue);
 				break;
 			case 1:	// T liquido di ricircolo
 				onNewTPerfRicircolo(sensorIR_TM[index_array].tempSensValue);
 				break;
 			case 2: // T perfusione venosa
 				onNewTPerfVenosa(sensorIR_TM[index_array].tempSensValue);
 				break;
 			}
 		}

 		iflag_sensTempIR_Meas_Ready = IFLAG_IDLE;

 	}
}


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

bool computeCRC8TempSensRx(unsigned char buffer[],unsigned char command, unsigned char TempSensAddr){
	word registerCRC = 0x0000;
	word polinomial = 0x0107;
	word maskByte = 0x8000;
	unsigned char bufferPEC[NUM_BYTE_CRC_RX];
	unsigned char PEC_CALCULATED, PEC_RX;

	/*Il sensore IR calcola e invia la PEC nel terzo byte (i primi due sono i valori di temp)
	 * il calcolo lo fa usando 5 byte con:
	 * 1) Slave Address *2
	 * 2) Command (nel caso di comando di lettura si ha (RAM_ACCESS_COMMAND | SD_TOBJ1_RAM_ADDRESS) ovvero 0x0007
	 * 3) (Slave Address *2) +1
	 * 4) Byte di dato LSB
	 * 5) Byte di dato MSB */
	bufferPEC[0] = TempSensAddr * 2;
	bufferPEC[1] = command;
	bufferPEC[2] = (TempSensAddr * 2) + 1;
	bufferPEC[3] = buffer[0];
	bufferPEC[4] = buffer[1];

	PEC_RX = buffer[2];

	for(int a = 0; a<NUM_BYTE_CRC_RX; a++)
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

	PEC_CALCULATED = (unsigned char) (registerCRC & 0x00FF);

	if (PEC_CALCULATED == PEC_RX)
		return (TRUE);
	else
	{
		/*incremento il contatore di errori sulla PEC*/
		sensorIR_TM[TempSensAddr-1].errorPEC++;
		return (FALSE);
	}
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
		sensTempIRId = sensorIR_TM[tempSensAddress-1].sensorId;

	/* Start + slave address + write command */
	IR_TM_COMM_SelectSlave(tempSensAddress);
	err = IR_TM_COMM_SendChar(command);
    // Filippo - in caso di errore provo a resettare la periferica
    if (err!=ERR_OK)
    {
        IR_TM_COMM_Disable();
        IR_TM_COMM_Enable();
    }
	ptrData = &sensorIR_TM[tempSensAddress-1].bufferReceived[0];
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
