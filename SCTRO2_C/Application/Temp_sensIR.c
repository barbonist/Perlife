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

 		/*se l'indirizzo supera l'ultimo sensore, resetto l'indirizzo al primo sensore per rifare il giro dei 3*/
 		if (Address > LAST_IR_TEMP_SENSOR)
 			Address = FIRST_IR_TEMP_SENSOR;

 		/*costruisco il comando di lettura per il sensore di temperatura con indirizzo pari ad Address
 		 * Quando scatter� l'interrupt di trasmissione 'void IR_TM_COMM_OnTransmitData(void)'
 		 * andr� ad effettuare la lettura con 'IR_TM_COMM_RecvBlock(ptrDataTemperatureIR, 3, &ret);'*/
 		ptrDataTemperatureIR = buildCmdReadTempSensIR(Address, (RAM_ACCESS_COMMAND | SD_TOBJ1_RAM_ADDRESS), 0);
 		/*incremento i possibili errori dpovuti a non risposte o a risposte con PEC errate; se ricevo benbe resetto questo contatore*/
 		sensorIR_TM[Address-1].ErrorMSG++;
 		/*incremento l'indirizzo cos� al prossimo giro interrogo il sensore successivo*/
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
 			/*Copio il valore ricevuto*/
 			sensorIR_TM[index_array].tempSensValue = (float)((BYTES_TO_WORD(sensorIR_TM[index_array].bufferReceived[1], sensorIR_TM[index_array].bufferReceived[0]))*((float)0.02)) - (float)273.15;
 			/*correggo il valore di OfFset se sono sotto LOWER_RANGE_IR_CORRECTION o sopra HIGHER_RANGE_IR_CORRECTION*/
 			//sensorIR_TM[index_array].tempSensValue = Ir_Temperature_correction_offset (sensorIR_TM[index_array].tempSensValue);
 			/*correggo la temperatura sulla base dell'algoritmo di SB vedi mail del 21/6/2019*/
 			sensorIR_TM[index_array].tempSensValue = Ir_Measured_Temperature_correction(index_array);
 			/*aggiorno il valore filtrato di temperatura, filtro su 25 campioni quindi tengo la storia su 15 secondi (ogni sensore viene letto ogni 600 msec*/
 			sensorIR_TM[index_array].tempSensValueFiltered = meanWATempSensorIR(25,sensorIR_TM[index_array].tempSensValue,index_array);
 			/*ho ricevuto bene, resetto il contatore consecutivo di errore*/
 			sensorIR_TM[index_array].ErrorMSG = 0;
 		}

 		iflag_sensTempIR_Meas_Ready = IFLAG_IDLE;

 	}
}


float Ir_Measured_Temperature_correction(int index_array)

{
	float MeasuredTemp;
	float E8;  // error at 8 degrees
	float E27;  // error at 27 degrees
	float E37; //error at 37 degrees
	float a,b,c,d,Err;

    MeasuredTemp = sensorIR_TM[index_array].tempSensValue;

    switch(index_array)
    {
		case 0:
			E8  = config_data.T_sensor_ART_Meas_Low  - config_data.T_sensor_ART_Real_Low;
			E27 = config_data.T_sensor_ART_Meas_Med  - config_data.T_sensor_ART_Real_Med;
			E37 = config_data.T_sensor_ART_Meas_High - config_data.T_sensor_ART_Real_High;
			break;

		case 1:
			E8  = config_data.T_sensor_RIC_Meas_Low  - config_data.T_sensor_RIC_Real_Low;
			E27 = config_data.T_sensor_RIC_Meas_Med  - config_data.T_sensor_RIC_Real_Med;
			E37 = config_data.T_sensor_RIC_Meas_High - config_data.T_sensor_RIC_Real_High;
			break;

		case 2:
			E8  = config_data.T_sensor_VEN_Meas_Low  - config_data.T_sensor_VEN_Real_Low;
			E27 = config_data.T_sensor_VEN_Meas_Med  - config_data.T_sensor_VEN_Real_Med;
			E37 = config_data.T_sensor_VEN_Meas_High - config_data.T_sensor_VEN_Real_High;
			break;
    }

    a = (E27 - E8)/19;
    b = (27*E8 - 8*E27)/19;
    c = (E37 - E27)/10;
    d = (37*E27 - 27*E37)/10;

 //vedi SC 111_008C_19
    if (MeasuredTemp <= 27.0)
    	Err = MeasuredTemp*a + b;
    else //
    	Err = MeasuredTemp*c + d;

    return (MeasuredTemp + Err);
}


/*funzione che serve a correggere la temperatura letta col sensore IR con un
 * Offset via via crescente quando la temperatura letta � <= 10 opp >= 31*/
float Ir_Temperature_correction_offset (float Temp_value)
{
	float Temp_correct = Temp_value;

//	/*se la temperaura � compresa tra LOWER_RANGE_IR_CORRECTION e HIGHER_RANGE_IR_CORRECTION non facciuo nessuna converisone*/
//	if (Temp_value < HIGHER_RANGE_IR_CORRECTION && Temp_value > LOWER_RANGE_IR_CORRECTION)
//	{
//		Temp_correct = Temp_value;
//	}
	/*se la temperatura � <= LOWER_RANGE_IR_CORRECTION correggo in negativo*/
	/*else*/ if (Temp_value <= LOWER_RANGE_IR_CORRECTION)
	{
		Temp_correct = Temp_value - (LOWER_RANGE_IR_CORRECTION - Temp_value ) * DELTA_CORRECTION;
	}
	/*se la temperatura � >= HIGHER_RANGE_IR_CORRECTION correggo in positivo*/
	else if (Temp_value >= HIGHER_RANGE_IR_CORRECTION)
	{
		Temp_correct = Temp_value + (Temp_value - HIGHER_RANGE_IR_CORRECTION) * DELTA_CORRECTION;
	}

	return (Temp_correct);
}

float meanWATempSensorIR(unsigned char dimNum, float newSensVal, char IdSens)
{
	static float circularBuffer[3] [255]; //uso una matrice di 3 array, uno per ogni sensore
	static float circBuffAdd[3] [255];    //uso una matrice di 3 array, uno per ogni sensore
	float numSumValue = 0;
	float denValue=0;
	float numTotal=0;

	if(dimNum <= 255){
	for(int i=(dimNum-1); i>0; i--)
	{
		denValue = denValue + i;

		circularBuffer[IdSens] [i] = circularBuffer [IdSens] [i-1];
		circBuffAdd [IdSens] [i] = circularBuffer[IdSens] [i]*(dimNum-i);
		numSumValue = numSumValue + circBuffAdd [IdSens] [i];

	}
	circularBuffer[IdSens] [0] = newSensVal;
	numSumValue = numSumValue + (circularBuffer [IdSens] [0]*dimNum);
	denValue = denValue + dimNum;
	numTotal = (numSumValue/denValue);

	return numTotal;
	}
	else
		return 0;
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
//		/*Se � arrivato un NACK sul bus I2C con cui comunico con  i sensori IR,
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
//			/*resetto il flag cos� da non ricadere pi� in questo if
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
//				 * vorrei impostare il controllo della PEC in ricezione ma non riesco a calcolarla correttamente e non � mai uguale a quella ricevuta
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

	/* Restart + slave address + received data */
	//IR_TM_COMM_SelectSlave(tempSensAddress);
	//if(err == ERR_OK)
		//IR_TM_COMM_RecvBlock(&rcvData[0], NUM_BYTE_RCV, &ret);
	//ptrData = &rcvData[0];
	ptrData = &sensorIR_TM[tempSensAddress-1].bufferReceived[0];
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

