/*
 * ModBusCommProt.c
 *
 *  Created on: 03/nov/2016
 *      Author: W15
 */

#include "PE_Types.h"
#include "Global.h"
#include "ModBusCommProt.h"
#include "EN_24_M_C.h"
#include "EN_MOTOR_CONTROL.h"
#include "EN_CLAMP_CONTROL.h"
#include "RTS_MOTOR.h"
#include "MODBUS_COMM.h"
#include "ASerialLdd1.h"
#include "Alarm_Con.h"
#include "EEPROM.h"
#include "string.h"
#include "App_Ges.h"


extern int MyArrayIdx;

THERAPY_TYPE TherapyType = KidneyTreat;
bool WriteActive = FALSE;
bool ReadActive = FALSE;
char LasActuatorWriteID = 0;      // id dell'attuatore modbus con scrittura in corso

extern int timerCounterModBusOld;


uint32_t msTick_elapsed( uint32_t last );


THERAPY_TYPE GetTherapyType(void)
{
	return TherapyType;
}

void SetTherapyType(THERAPY_TYPE tt)
{
	TherapyType = tt;
}

/* Public function */
struct func3RetStruct * ModBusReadRegisterReq(char slaveAddr,
									  char funcCode,
									  unsigned int readStartAddr,
									  unsigned int numRegisterRead
									  )
{
	/* Init frame - All frame byte equal to 0xFF */
	FrameInitFunc3();

	/*pos_0: slave address */
	msgToSendFrame3[0] = slaveAddr;
	/*pos_1: function code */
	msgToSendFrame3[1] = funcCode;
	/*pos_2: read start address - High */
	msgToSendFrame3[2] = (readStartAddr & 0xFF00)>>8;
	/*pos_3: read start address - Low */
	msgToSendFrame3[3] = (readStartAddr & 0x00FF);
	/*pos_4: number of register to be read - High*/
	msgToSendFrame3[4] = (numRegisterRead & 0xFF00)>>8;
	/*pos_5: number of register to be read - Low*/
	msgToSendFrame3[5] = (numRegisterRead & 0x00FF);

	msgToSendFrame3Ptr =&msgToSendFrame3[0];

	mstreq_crc = ComputeChecksum(msgToSendFrame3Ptr, 6);

	/*pos_6: crc Low*/
	mstreq_crc_Low = (mstreq_crc & 0x00FF);
	msgToSendFrame3[6] = mstreq_crc_Low;
	/*pos 7: crc High*/
	mstreq_crc_High = (mstreq_crc & 0xFF00)>>8;
	msgToSendFrame3[7] = mstreq_crc_High;

	msgToSendFrame3Ptr =&msgToSendFrame3[0];
	_func3RetVal.mstreqRetStructPtr = msgToSendFrame3Ptr;
	_func3RetVal.mstreqRetStructNumByte = 8;
	_func3RetVal.slvresRetNumByte = 1 + 							/* slave address */
									1 +								/* function code */
									1 +								/* byte read count */
									(2*msgToSendFrame3[5]) +		/* data byte */
									1 + 							/* crc low */
									1;								/* crc high */
	msgToRecvFrame3Ptr = &msgToRecvFrame3[0];
	_func3RetVal.slvresRetPtr = msgToRecvFrame3Ptr;

	/* return the pointer to the struct: position 0 --> pointer to message; position 1: number of byte to send */
	_func3RetValPtr = &_func3RetVal;

	//return msgToSendFrame3Ptr;
	return _func3RetValPtr;
}




#ifdef PUMP_EVER
struct func3RetStruct * ModBusReadRegisterReqEVER(char slaveAddr,
									              char funcCode,
									              unsigned int readStartAddr,
									              unsigned int numRegisterRead
									             )
{
	/* Init frame - All frame byte equal to 0xFF */
	FrameInitFunc3();

	/*pos_0: slave address */
	msgToSendFrame3[0] = slaveAddr;
	/*pos_1: function code */
	msgToSendFrame3[1] = funcCode;
	/*pos_2: read start address - High */
	msgToSendFrame3[2] = (readStartAddr & 0xFF00)>>8;
	/*pos_3: read start address - Low */
	msgToSendFrame3[3] = (readStartAddr & 0x00FF);
	/*pos_4: number of register to be read - High*/
	msgToSendFrame3[4] = (numRegisterRead & 0xFF00)>>8;
	/*pos_5: number of register to be read - Low*/
	msgToSendFrame3[5] = (numRegisterRead & 0x00FF);

	msgToSendFrame3Ptr =&msgToSendFrame3[0];

	mstreq_crc = ComputeChecksum(msgToSendFrame3Ptr, 6);

	/*pos_6: crc Low*/
	mstreq_crc_Low = (mstreq_crc & 0x00FF);
	msgToSendFrame3[6] = mstreq_crc_Low;
	/*pos 7: crc High*/
	mstreq_crc_High = (mstreq_crc & 0xFF00)>>8;
	msgToSendFrame3[7] = mstreq_crc_High;

	msgToSendFrame3Ptr =&msgToSendFrame3[0];
	_func3RetVal.mstreqRetStructPtr = msgToSendFrame3Ptr;
	_func3RetVal.mstreqRetStructNumByte = 8;
	_func3RetVal.slvresRetNumByte = 1 + 							/* slave address */
									1 +								/* function code */
									1 +								/* byte read count */
									(2*msgToSendFrame3[5]) +		/* data byte */
									1 + 							/* crc low */
									1;								/* crc high */
	msgToRecvFrame3Ptr = &msgToRecvFrame3[0];
	_func3RetVal.slvresRetPtr = msgToRecvFrame3Ptr;

	/* return the pointer to the struct: position 0 --> pointer to message; position 1: number of byte to send */
	_func3RetValPtr = &_func3RetVal;

	//return msgToSendFrame3Ptr;
	return _func3RetValPtr;
}
#endif

struct func10RetStruct * ModBusWriteRegisterReq(char slaveAddr,
		  	  	  	  	  	  	  	   char funcCode,
									   unsigned int writeStartAddr,
									   unsigned int numRegisterWrite,
									   unsigned int * writeRegisterValue
									  )
{
	/*Internal variable*/
		unsigned int _writeRegValueArray[6] = {0x0000,
											   0x0000,
											   0x0000,
											   0x0000,
											   0x0000,
											   0x0000};

		unsigned int * _writeRegValuePtr;
		_writeRegValuePtr = writeRegisterValue;

		/* Update array values with current one starting from pos 0*/
		for(int a = 0; a<6; a++){
			_writeRegValueArray[a] = *(_writeRegValuePtr+a);
		}

	/* Init frame - All frame byte equal to 0xFF */
	FrameInitFunc10();

	/*pos_0: slave address */
	msgToSendFrame10[0] = slaveAddr;
	/*pos_1: function code */
	msgToSendFrame10[1] = funcCode;
	/*pos_2: write start address - High */
	msgToSendFrame10[2] = (writeStartAddr & 0xFF00)>>8;
	/*pos_3: write start address - Low */
	msgToSendFrame10[3] = (writeStartAddr & 0x00FF);
	/*pos_4: number of register to write - High*/
	msgToSendFrame10[4] = (numRegisterWrite & 0xFF00)>>8;
	/*pos_5: number of register to write - Low*/
	msgToSendFrame10[5] = (numRegisterWrite & 0x00FF);
	/*pos_6: write byte count = 2 * num register to write */
	mstreq_data_wr_byte_count = (2 * msgToSendFrame10[5]);
	msgToSendFrame10[6] = mstreq_data_wr_byte_count;

	for(int num_byte = 0; num_byte < (mstreq_data_wr_byte_count/2); num_byte++)
	{
		/*pos_6plus_numbyte: write value - High */
		msgToSendFrame10[6+(1+2*num_byte)] = (_writeRegValueArray[num_byte] & 0xFF00)>>8;
		/*pos_7plus_numbyte: write value - Low */
		msgToSendFrame10[6+(2+2*num_byte)] = (_writeRegValueArray[num_byte] & 0x00FF);
	}

	msgToSendFrame10Ptr = &msgToSendFrame10[0];

	mstreq_crc = ComputeChecksum(msgToSendFrame10Ptr, (6+mstreq_data_wr_byte_count+1));
	/*pos_: crc Low*/
	mstreq_crc_Low = (mstreq_crc & 0x00FF);
	msgToSendFrame10[6+mstreq_data_wr_byte_count+1] = mstreq_crc_Low;
	/*pos_: crc High*/
	mstreq_crc_High = (mstreq_crc & 0xFF00)>>8;
	msgToSendFrame10[6+mstreq_data_wr_byte_count+2] = mstreq_crc_High;

	msgToSendFrame10Ptr = &msgToSendFrame10[0];
	_func10RetVal.mstreqRetStructPtr = msgToSendFrame10Ptr;
	_func10RetVal.mstreqRetStructNumByte = 6+mstreq_data_wr_byte_count+2+1;
	_func10RetVal.slvresRetNumByte = 	1 + 							/* slave address */
										1 +								/* function code */
										1 +								/* write start address High */
										1 +								/* write start address Low */
										1 +								/* num reg written High */
										1 +								/* num reg written Low */
										1 + 							/* crc low */
										1;								/* crc high */
	msgToRecvFrame10Ptr = &msgToRecvFrame10[0];
	_func10RetVal.slvresRetPtr = msgToRecvFrame10Ptr;
	/* return the pointer to the struct: position 0 --> pointer to message; position 1: number of byte to send */
	_func10RetValPtr = &_func10RetVal;

	//return msgToSendFrame10Ptr;
	return _func10RetValPtr;
}

#ifdef PUMP_EVER
struct func10RetStruct * ModBusWriteRegisterReqEVER(char slaveAddr,
		  	  	  	  	  	  	  	   char funcCode,
									   unsigned int writeStartAddr,
									   unsigned int numRegisterWrite,
									   word * writeRegisterValue
									  )
{


	/* Init frame - All frame byte equal to 0xFF */
	FrameInitFunc10();

	/*pos_0: slave address */
	msgToSendFrame10[0] = slaveAddr;
	/*pos_1: function code */
	msgToSendFrame10[1] = funcCode;
	/*pos_2: write start address - High */
	msgToSendFrame10[2] = (writeStartAddr & 0xFF00)>>8;
	/*pos_3: write start address - Low */
	msgToSendFrame10[3] = (writeStartAddr & 0x00FF);
	/*pos_4: number of register to write - High*/
	msgToSendFrame10[4] = (numRegisterWrite & 0xFF00)>>8;
	/*pos_5: number of register to write - Low*/
	msgToSendFrame10[5] = (numRegisterWrite & 0x00FF);
	/*pos_6: write byte count = 2 * num register to write */
	mstreq_data_wr_byte_count = (2 * numRegisterWrite);
	msgToSendFrame10[6] = mstreq_data_wr_byte_count;

	for(int num_byte = 0; num_byte < (mstreq_data_wr_byte_count/2); num_byte++)
	{
		/*pos_6plus_numbyte: write value - High */
		msgToSendFrame10[6+(1+2*num_byte)] = (writeRegisterValue[num_byte] & 0xFF00)>>8;
		/*pos_7plus_numbyte: write value - Low */
		msgToSendFrame10[6+(2+2*num_byte)] = (writeRegisterValue[num_byte] & 0x00FF);
	}

	msgToSendFrame10Ptr = &msgToSendFrame10[0];

	mstreq_crc = ComputeChecksum(msgToSendFrame10Ptr, (6+mstreq_data_wr_byte_count+1));

	/*pos_: crc Low*/
	mstreq_crc_Low = (mstreq_crc & 0x00FF);
	msgToSendFrame10[6+mstreq_data_wr_byte_count+1] = mstreq_crc_Low;
	/*pos_: crc High*/
	mstreq_crc_High = (mstreq_crc & 0xFF00)>>8;
	msgToSendFrame10[6+mstreq_data_wr_byte_count+2] = mstreq_crc_High;



	msgToSendFrame10Ptr = &msgToSendFrame10[0];
	_func10RetVal.mstreqRetStructPtr = msgToSendFrame10Ptr;
	_func10RetVal.mstreqRetStructNumByte = 6+mstreq_data_wr_byte_count+2+1;
	_func10RetVal.slvresRetNumByte = 	1 + 							/* slave address */
										1 +								/* function code */
										1 +								/* write start address High */
										1 +								/* write start address Low */
										1 +								/* num reg written High */
										1 +								/* num reg written Low */
										1 + 							/* crc high */
										1;								/* crc low */
	msgToRecvFrame10Ptr = &msgToRecvFrame10[0];
	_func10RetVal.slvresRetPtr = msgToRecvFrame10Ptr;
	/* return the pointer to the struct: position 0 --> pointer to message; position 1: number of byte to send */
	_func10RetValPtr = &_func10RetVal;

	//return msgToSendFrame10Ptr;
	return _func10RetValPtr;
}
#endif

struct func17RetStruct * ModBusRWRegisterReq(char slaveAddr,
	  	  	  	   	   	   	   	    char funcCode,
									unsigned int readStartAddr,
									unsigned int numRegisterRead,
									unsigned int writeStartAddr,
									unsigned int numRegisterWrite,
									unsigned int * writeRegisterValue
									)
{
	/*Internal variable*/
	unsigned int _writeRegValueArray[6] = {0x0000,
										   0x0000,
										   0x0000,
										   0x0000,
										   0x0000,
										   0x0000};

	unsigned int * _writeRegValuePtr;
	_writeRegValuePtr = writeRegisterValue;
	char endIndexMsgToSend = 0;

	/* Update array values with current one starting from pos 0*/
	for(int a = 0; a<6; a++){
		_writeRegValueArray[a] = *(_writeRegValuePtr+a);
	}
	/* Init frame - All frame byte equal to 0xFF */
	FrameInitFunc17();

	/*pos_0: slave address */
	msgToSendFrame17[0] = slaveAddr;
	/*pos_1: function code */
	msgToSendFrame17[1] = funcCode;
	/*pos_2: read start address - High */
	msgToSendFrame17[2] = (readStartAddr & 0xFF00)>>8;
	/*pos_3: read start address - Low */
	msgToSendFrame17[3] = (readStartAddr & 0x00FF);
	/*pos_4: number of register to be read - High*/
	msgToSendFrame17[4] = (numRegisterRead & 0xFF00)>>8;
	/*pos_5: number of register to be read - Low*/
	msgToSendFrame17[5] = (numRegisterRead & 0x00FF);
	/*pos_6: write start address - High */
	msgToSendFrame17[6] = (writeStartAddr & 0xFF00)>>8;
	/*pos_7: write start address - Low */
	msgToSendFrame17[7] = (writeStartAddr & 0x00FF);
	/*pos_8: number of register to write - High*/
	msgToSendFrame17[8] = (numRegisterWrite & 0xFF00)>>8;
	/*pos_9: number of register to write - Low*/
	msgToSendFrame17[9] = (numRegisterWrite & 0x00FF);
	/*pos_10: write byte count = 2 * num register to write */
	mstreq_data_wr_byte_count = (2 * msgToSendFrame17[9]);
	msgToSendFrame17[10] = mstreq_data_wr_byte_count;

	if(mstreq_data_wr_byte_count == 0)
	{
		/*pos_10plus_numbyte: write value - High */
		msgToSendFrame17[10+(1+2*mstreq_data_wr_byte_count)] = 0x00;
		/*pos_11plus_numbyte: write value - Low */
		msgToSendFrame17[10+(2+2*mstreq_data_wr_byte_count)] = 0x00;

		endIndexMsgToSend = 0;
	}
	else
	{
	for(int num_byte = 0; num_byte <= (mstreq_data_wr_byte_count - 2)/2; num_byte=num_byte+1)
		{
			/*pos_10plus_numbyte: write value - High */
			msgToSendFrame17[10+(1+2*num_byte)] = (_writeRegValueArray[num_byte] & 0xFF00)>>8;
			/*pos_11plus_numbyte: write value - Low */
			msgToSendFrame17[10+(2+2*num_byte)] = (_writeRegValueArray[num_byte] & 0x00FF);
		}

	endIndexMsgToSend = (mstreq_data_wr_byte_count-2);
	}

	msgToSendFrame17Ptr = &msgToSendFrame17[0];

	mstreq_crc = ComputeChecksum(msgToSendFrame17Ptr, (10+mstreq_data_wr_byte_count+1));
	/*pos_: crc Low*/
	mstreq_crc_Low = (mstreq_crc & 0x00FF);
	msgToSendFrame17[12+endIndexMsgToSend+1] = mstreq_crc_Low;
	/*pos_: crc High*/
	mstreq_crc_High = (mstreq_crc & 0xFF00)>>8;
	msgToSendFrame17[12+endIndexMsgToSend+2] = mstreq_crc_High;

	msgToSendFrame17Ptr = &msgToSendFrame17[0];
	_func17RetVal.mstreqRetStructPtr = msgToSendFrame17Ptr;
	_func17RetVal.mstreqRetStructNumByte = 12+endIndexMsgToSend+2+1;
	_func17RetVal.slvresRetNumByte = 	1 + 							/* slave address */
										1 +								/* function code */
										1 +								/* byte read count */
										(2*msgToSendFrame17[5]) +		/* data byte */
										1 + 							/* crc low */
										1;								/* crc high */
	msgToRecvFrame17Ptr = &msgToRecvFrame17[0];
	_func17RetVal.slvresRetPtr = msgToRecvFrame17Ptr;
	/* return the pointer to the struct: position 0 --> pointer to message; position 1: number of byte to send */
	_func17RetValPtr = &_func17RetVal;

	//return msgToSendFrame17Ptr;
	return _func17RetValPtr;
}

void UpdatePmpAddress(THERAPY_TYPE tt)
{
	if(tt == KidneyTreat)
		pumpPerist[0].pmpMySlaveAddress = PPAR ;   // 0x02;	 rotary selctor = 0 - pump art
	else if(TherapyType == LiverTreat)
		pumpPerist[0].pmpMySlaveAddress = PPAF;	   // 0x03  rotary selctor = 1 - pump art
	else
	{
		// se il parametro e' indefinito metto il rene
		pumpPerist[0].pmpMySlaveAddress = PPAR ;   // 0x02;	 rotary selctor = 0 - pump art
	}

	if(tt == KidneyTreat)
		pumpPerist[3].pmpMySlaveAddress = PPAF ;   // 0x03;	 rotary selctor = 1 - noy used fot kidney therapy
	else if(TherapyType == LiverTreat)
		pumpPerist[3].pmpMySlaveAddress = PPAR;	   // 0x02  rotary selctor = 0 - pump art
	else
	{
		// se il parametro e' indefinito metto il rene
		pumpPerist[3].pmpMySlaveAddress = PPAF ;   // 0x03;	 rotary selctor = 1 - noy used fot kidney therapy
	}
}


void modBusPmpInit(THERAPY_TYPE tt)
{
	for (int i=0; i<8; i++)
		CountErrorModbusMSG[i] = 0;
	for (int i = 0; i < LAST_ACTUATOR; i++)
		ActuatorWriteCnt[i] = 0;

	/***************** PMP 1********************/
	pumpPerist[0].id = 0;
	pumpPerist[0].pmpSpeed = 0x0000;
	pumpPerist[0].pmpGoHomeSpeed = 0x0000;
	pumpPerist[0].pmpAccelSpeed = 0x0000;
	pumpPerist[0].pmpCurrent = 0x0000;
	pumpPerist[0].pmpCruiseSpeed = 0x0000;
	pumpPerist[0].pmpStepTarget = 0x0000;

	if(tt == KidneyTreat)
		pumpPerist[0].pmpMySlaveAddress = PPAR ;   // 0x02;	 rotary selctor = 0 - pump art
	else if(TherapyType == LiverTreat)
		pumpPerist[0].pmpMySlaveAddress = PPAF;	   // 0x03  rotary selctor = 1 - pump art
	else
	{
		// se il parametro e' indefinito metto il rene
		pumpPerist[0].pmpMySlaveAddress = PPAR ;   // 0x02;	 rotary selctor = 0 - pump art
	}

	pumpPerist[0].pmpFuncCode = 0xFF;
	pumpPerist[0].pmpWriteStartAddr = 0xFFFF;
	pumpPerist[0].pmpReadStartAddr = 0xFFFF;
	pumpPerist[0].pmpNumeRegWrite = 0x0001;
	pumpPerist[0].pmpNumRegRead = 0x0001;
	pumpPerist[0].pmpWriteRegValuePtr = 0;
	pumpPerist[0].pmpPressLoop = PRESS_LOOP_OFF;
	pumpPerist[0].entry = 0;
	pumpPerist[0].reqState = REQ_STATE_OFF;
	pumpPerist[0].reqType = REQ_TYPE_IDLE;
	pumpPerist[0].actuatorType = ACTUATOR_PUMP_TYPE;
	pumpPerist[0].dataReady = DATA_READY_FALSE;
	pumpPerist[0].actualSpeed = 0;
	pumpPerist[0].actualSpeedOld = 0;

	/***************** PMP 2********************/
	pumpPerist[1].id = 1;
	pumpPerist[1].pmpSpeed = 0x0000;
	pumpPerist[1].pmpGoHomeSpeed = 0x0000;
	pumpPerist[1].pmpAccelSpeed = 0x0000;
	pumpPerist[1].pmpCurrent = 0x0000;
	pumpPerist[1].pmpCruiseSpeed = 0x0000;
	pumpPerist[1].pmpStepTarget = 0x0000;

	/* rotary selctor = 2 - pump oxyg per rene e fegato
	 *	(nel fegato e' collegata anche alla linea venosa)*/

	pumpPerist[1].pmpMySlaveAddress = PPV1;

	pumpPerist[1].pmpFuncCode = 0xFF;
	pumpPerist[1].pmpWriteStartAddr = 0xFFFF;
	pumpPerist[1].pmpReadStartAddr = 0xFFFF;
	pumpPerist[1].pmpNumeRegWrite = 0x0001;
	pumpPerist[1].pmpNumRegRead = 0x0001;
	pumpPerist[1].pmpWriteRegValuePtr = 0;
	pumpPerist[1].pmpPressLoop = PRESS_LOOP_OFF;
	pumpPerist[1].entry = 0;
	pumpPerist[1].reqState = REQ_STATE_OFF;
	pumpPerist[1].reqType = REQ_TYPE_IDLE;
	pumpPerist[1].actuatorType = ACTUATOR_PUMP_TYPE;
	pumpPerist[1].dataReady = DATA_READY_FALSE;
	pumpPerist[1].actualSpeed = 0;
	pumpPerist[1].actualSpeedOld = 0;

	/***************** PMP 3********************/
	pumpPerist[2].id = 2;
	pumpPerist[2].pmpSpeed = 0x0000;
	pumpPerist[2].pmpGoHomeSpeed = 0x0000;
	pumpPerist[2].pmpAccelSpeed = 0x0000;
	pumpPerist[2].pmpCurrent = 0x0000;
	pumpPerist[2].pmpCruiseSpeed = 0x0000;
	pumpPerist[2].pmpStepTarget = 0x0000;
	/* rotary selctor = 2 - pump oxyg per rene e fegato
	 *	(nel fegato e' collegata anche alla linea venosa)*/
	pumpPerist[2].pmpMySlaveAddress = PPV2;		//rotary selctor = 3
	pumpPerist[2].pmpFuncCode = 0xFF;
	pumpPerist[2].pmpWriteStartAddr = 0xFFFF;
	pumpPerist[2].pmpReadStartAddr = 0xFFFF;
	pumpPerist[2].pmpNumeRegWrite = 0x0001;
	pumpPerist[2].pmpNumRegRead = 0x0001;
	pumpPerist[2].pmpWriteRegValuePtr = 0;
	pumpPerist[2].pmpPressLoop = PRESS_LOOP_OFF;
	pumpPerist[2].entry = 0;
	pumpPerist[2].reqState = REQ_STATE_OFF;
	pumpPerist[2].reqType = REQ_TYPE_IDLE;
	pumpPerist[2].actuatorType = ACTUATOR_PUMP_TYPE;
	pumpPerist[2].dataReady = DATA_READY_FALSE;
	pumpPerist[2].actualSpeed = 0;
	pumpPerist[2].actualSpeedOld = 0;

	/***************** PMP 4********************/
	pumpPerist[3].id = 3;
	pumpPerist[3].pmpSpeed = 0x0000;
	pumpPerist[3].pmpGoHomeSpeed = 0x0000;
	pumpPerist[3].pmpAccelSpeed = 0x0000;
	pumpPerist[3].pmpCurrent = 0x0000;
	pumpPerist[3].pmpCruiseSpeed = 0x0000;
	pumpPerist[3].pmpStepTarget = 0x0000;

	if(tt == KidneyTreat)
		pumpPerist[3].pmpMySlaveAddress = PPAF ;   // 0x03;	 rotary selctor = 1 - noy used fot kidney therapy
	else if(TherapyType == LiverTreat)
		pumpPerist[3].pmpMySlaveAddress = PPAR;	   // 0x02  rotary selctor = 0 - pump art
	else
	{
		// se il parametro e' indefinito metto il rene
		pumpPerist[3].pmpMySlaveAddress = PPAF ;   // 0x03;	 rotary selctor = 1 - noy used fot kidney therapy
	}

	pumpPerist[3].pmpFuncCode = 0xFF;
	pumpPerist[3].pmpWriteStartAddr = 0xFFFF;
	pumpPerist[3].pmpReadStartAddr = 0xFFFF;
	pumpPerist[3].pmpNumeRegWrite = 0x0001;
	pumpPerist[3].pmpNumRegRead = 0x0001;
	pumpPerist[3].pmpWriteRegValuePtr = 0;
	pumpPerist[3].pmpPressLoop = PRESS_LOOP_OFF;
	pumpPerist[3].entry = 0;
	pumpPerist[3].reqState = REQ_STATE_OFF;
	pumpPerist[3].reqType = REQ_TYPE_IDLE;
	pumpPerist[3].actuatorType = ACTUATOR_PUMP_TYPE;
	pumpPerist[3].dataReady = DATA_READY_FALSE;
	pumpPerist[3].actualSpeed = 0;
	pumpPerist[3].actualSpeedOld = 0;
}

void modBusPinchInit(void)
{
	/***************** PINCH 1********************/
	pinchActuator[0].id = 0;
	pinchActuator[0].pinchPosTarget = 0x0000;
	pinchActuator[0].pinchMySlaveAddress = PINCH_2WPVF; 	//rotary selctor = 5 - pinch art
	pinchActuator[0].pinchFuncCode = 0xFF;
	pinchActuator[0].pinchWriteStartAddr = 0xFFFF;
	pinchActuator[0].pinchReadStartAddr = 0xFFFF;
	pinchActuator[0].pinchNumeRegWrite = 0x0001;
	pinchActuator[0].pinchNumRegRead = 0x0001;
	pinchActuator[0].pinchWriteRegValuePtr = 0;
	pinchActuator[0].reqState = REQ_STATE_OFF;
	pinchActuator[0].reqType = REQ_TYPE_IDLE;
	pinchActuator[0].actuatorType = ACTUATOR_PINCH_TYPE;
	pinchActuator[0].dataReady = DATA_READY_FALSE;

	/***************** PINCH 2********************/
	pinchActuator[1].id = 1;
	pinchActuator[1].pinchPosTarget = 0x0000;
	pinchActuator[1].pinchMySlaveAddress = PINCH_2WPVA;		//rotary selctor = 6 - pinch ven
	pinchActuator[1].pinchFuncCode = 0xFF;
	pinchActuator[1].pinchWriteStartAddr = 0xFFFF;
	pinchActuator[1].pinchReadStartAddr = 0xFFFF;
	pinchActuator[1].pinchNumeRegWrite = 0x0001;
	pinchActuator[1].pinchNumRegRead = 0x0001;
	pinchActuator[1].pinchWriteRegValuePtr = 0;
	pinchActuator[1].reqState = REQ_STATE_OFF;
	pinchActuator[1].reqType = REQ_TYPE_IDLE;
	pinchActuator[1].actuatorType = ACTUATOR_PINCH_TYPE;
	pinchActuator[1].dataReady = DATA_READY_FALSE;

	/***************** PINCH 3********************/
	pinchActuator[2].id = 2;
	pinchActuator[2].pinchPosTarget = 0x0000;
	pinchActuator[2].pinchMySlaveAddress = PINCH_2WPVV; 		//rotary selctor = 7 - pinch filter
	pinchActuator[2].pinchFuncCode = 0xFF;
	pinchActuator[2].pinchWriteStartAddr = 0xFFFF;
	pinchActuator[2].pinchReadStartAddr = 0xFFFF;
	pinchActuator[2].pinchNumeRegWrite = 0x0001;
	pinchActuator[2].pinchNumRegRead = 0x0001;
	pinchActuator[2].pinchWriteRegValuePtr = 0;
	pinchActuator[2].reqState = REQ_STATE_OFF;
	pinchActuator[2].reqType = REQ_TYPE_IDLE;
	pinchActuator[2].actuatorType = ACTUATOR_PINCH_TYPE;
	pinchActuator[2].dataReady = DATA_READY_FALSE;

	/***************** NOT USED********************/
	pinchActuator[3].id = 3;
	pinchActuator[3].pinchPosTarget = 0x0000;
	pinchActuator[3].pinchMySlaveAddress = 0xA0;		//rotary selctor = 8
	pinchActuator[3].pinchFuncCode = 0xFF;
	pinchActuator[3].pinchWriteStartAddr = 0xFFFF;
	pinchActuator[3].pinchReadStartAddr = 0xFFFF;
	pinchActuator[3].pinchNumeRegWrite = 0x0001;
	pinchActuator[3].pinchNumRegRead = 0x0001;
	pinchActuator[3].pinchWriteRegValuePtr = 0;
}
void modbusDataInit (void)
{
	int i,j;

	for (i=0; i<TOTAL_ACTUATOR; i++)
	{
		for (j=0; j<TOTAL_MODBUS_DATA;j++)
			modbusData [TOTAL_ACTUATOR] [TOTAL_MODBUS_DATA] = 0;
	}

	TherapyType = KidneyTreat;
	WriteActive = FALSE;
	ReadActive = FALSE;
	LasActuatorWriteID = 0;      // id dell'attuatore modbus con scrittura in corso
	memset(ActuatorWriteCnt, 0, LAST_ACTUATOR);
	memset(ActuatorWrRepeatCmd, 0, LAST_ACTUATOR);

}

int SelectStruct(unsigned char slaveAddr)
{
	int StructId = 20; // devo impostarlo ad un numero che non esiste nei case
	if(pumpPerist[0].pmpMySlaveAddress == slaveAddr)
		StructId = 0;
	else if(pumpPerist[1].pmpMySlaveAddress == slaveAddr)
		StructId = 1;
	else if(pumpPerist[2].pmpMySlaveAddress == slaveAddr)
		StructId = 2;
	else if(pumpPerist[3].pmpMySlaveAddress == slaveAddr)
		StructId = 3;
	return StructId;
}

// se ci sono dei valori di velocita' da aggiornare lo faccio
void UpdatePumpSpeed(void)
{
	if(!WriteActive)
	{
		for(int i = 0; i < 4; i++)
		{
			if(pumpPerist[i].newSpeedValue != pumpPerist[i].value)
			{
				setPumpSpeedValueHighLevel(pumpPerist[i].pmpMySlaveAddress, pumpPerist[i].newSpeedValue);
			}
		}
	}
}


void setPumpSpeedValueHighLevel(unsigned char slaveAddr, int speedValue){

	int StructId = SelectStruct(slaveAddr);

	pumpPerist[StructId].newSpeedValue = speedValue;
	if(StructId == 1)
		pumpPerist[2].newSpeedValue = speedValue;
	if(WriteActive && (LasActuatorWriteID == StructId))
	{
		// c'e' una scrittura in corso devo aspettare che finisca
		return;
	}

	// QUESTO CONTROLLO E' STATO COMMENTATO PERCHE' LA VELOCITA' LETTA DA modbusData VARIA MOLTO
	// PIU' LENTAMENTE DI newSpeedValue. LA VELOCITA' VIENE IMPOSTATA SOLO A TIME SLOT FISSI, QUINDI,
	// SI PUO' MODIFICARE IL TARGET PIU' VOLTE.
	/*se la velocità che mi da lo slave è uguale a quella che vorrei impostare non attuo il comando*/
	//if (pumpPerist[StructId].newSpeedValue == modbusData[slaveAddr-2][17] && pumpPerist[StructId].newSpeedValue != 0)
	//	return;

	if(speedValue == 0)
	{
		int i = 0;
	}

	//switch((slaveAddr - 2))
	switch(StructId)
	{
	case 0:
		pumpPerist[0].reqState = REQ_STATE_ON;
		pumpPerist[0].reqType = REQ_TYPE_WRITE;
		pumpPerist[0].actuatorType = ACTUATOR_PUMP_TYPE;
		pumpPerist[0].value = speedValue;
		if(!ActuatorWrRepeatCmd[0])
		{
			// l'indice coincide con LasActuatorWriteID di alwaysModBusActuator
			// il comando high level non e' una ripetizione di un comando precedente quindi resetto il contatore di tentativi
			ActuatorWriteCnt[0] = 0;
		}
		break;

	case 1:
		pumpPerist[1].reqState = REQ_STATE_ON;
		pumpPerist[1].reqType = REQ_TYPE_WRITE;
		pumpPerist[1].actuatorType = ACTUATOR_PUMP_TYPE;
		pumpPerist[1].value = speedValue;
		if(!ActuatorWrRepeatCmd[1])
		{
			// l'indice coincide con LasActuatorWriteID di alwaysModBusActuator
			// il comando high level non e' una ripetizione di un comando precedente quindi resetto il contatore di tentativi
			ActuatorWriteCnt[1] = 0;
		}
		// il break e' stato tolto volutamente perche' le due pump devono lavorare insieme --> a livello logico sono un unica pompa
		//break;

	case 2:
		pumpPerist[2].reqState = REQ_STATE_ON;
		pumpPerist[2].reqType = REQ_TYPE_WRITE;
		pumpPerist[2].actuatorType = ACTUATOR_PUMP_TYPE;
		pumpPerist[2].value = speedValue;
		if(!ActuatorWrRepeatCmd[2])
		{
			// l'indice coincide con LasActuatorWriteID di alwaysModBusActuator
			// il comando high level non e' una ripetizione di un comando precedente quindi resetto il contatore di tentativi
			ActuatorWriteCnt[2] = 0;
		}
		break;

	case 3:
		pumpPerist[3].reqState = REQ_STATE_ON;
		pumpPerist[3].reqType = REQ_TYPE_WRITE;
		pumpPerist[3].actuatorType = ACTUATOR_PUMP_TYPE;
		pumpPerist[3].value = speedValue;
		if(!ActuatorWrRepeatCmd[3])
		{
			// l'indice coincide con LasActuatorWriteID di alwaysModBusActuator
			// il comando high level non e' una ripetizione di un comando precedente quindi resetto il contatore di tentativi
			ActuatorWriteCnt[3] = 0;
		}
		break;

	default:
		break;
	}
}

// imposta il valore di CURRENT LEVEL dell'attuatore
void setPumpCurrentValue(unsigned char slaveAddr, int currValue){
	unsigned int mycurrValue = 0;
	unsigned int * valModBusArrayPtr;
	char	funcCode;
	unsigned int	wrAddr;

	funcCode = 0x10;
	wrAddr = 0x0003; /* current level */

	if((slaveAddr >= 2) && (slaveAddr <= 6)) /* pump */
		mycurrValue = (unsigned int)currValue;

	valModBusArrayPtr = &mycurrValue;

	_funcRetValPtr = (struct funcRetStruct *)ModBusWriteRegisterReq(slaveAddr,
											                        funcCode,
											                        wrAddr,
											                        0x0001,
											                        valModBusArrayPtr);
	//send command to actuator
	_funcRetVal.ptr_msg = _funcRetValPtr->ptr_msg;
	_funcRetVal.mstreqRetStructNumByte = _funcRetValPtr->mstreqRetStructNumByte;
	_funcRetVal.slvresRetPtr = _funcRetValPtr->slvresRetPtr;
	_funcRetVal.slvresRetNumByte = _funcRetValPtr->slvresRetNumByte;

	MODBUS_COMM_ClearRxBuf();
	/*prima di ogni spedizione abilito la seriale del modbus perchè
	 * potrebbe essere stata disabilita da una non corretta ricezione
	 * per resettare la perifericare e riallinearla; all'interno della
	 * funzione MODBUS_COMM_Enable() viene controllata la flag EnUser
	 * che esegue l'abilitazione della serrtiale solo se essa è disabilitata*/
	MODBUS_COMM_Enable();
#ifdef PUMP_EVER
	RX_ENABLE = FALSE;
	RTS_MOTOR_SetVal();
#endif
	for(char k = 0; k < _funcRetVal.mstreqRetStructNumByte; k++)
	{
		MODBUS_COMM_SendChar(*(_funcRetVal.ptr_msg+k));
	}
}

// imposta il valore di CURRENT LEVEL dell'attuatore
void setPumpAccelerationValue(unsigned char slaveAddr, int acc)
{
	unsigned int mycurrValue = 0;
	unsigned int * valModBusArrayPtr;
	char	funcCode = 0x10;
	unsigned int	wrAddr = 0x0002; /* current level */

	if((slaveAddr >= 2) && (slaveAddr <= 6)) /* pump */
		mycurrValue = (unsigned int)acc;

	valModBusArrayPtr = &mycurrValue;

	_funcRetValPtr = (struct funcRetStruct *)ModBusWriteRegisterReq(slaveAddr,
											                        funcCode,
											                        wrAddr,
											                        0x0001,
											                        valModBusArrayPtr);
	//send command to actuator
	_funcRetVal.ptr_msg = _funcRetValPtr->ptr_msg;
	_funcRetVal.mstreqRetStructNumByte = _funcRetValPtr->mstreqRetStructNumByte;
	_funcRetVal.slvresRetPtr = _funcRetValPtr->slvresRetPtr;
	_funcRetVal.slvresRetNumByte = _funcRetValPtr->slvresRetNumByte;

	MODBUS_COMM_ClearRxBuf();
	/*prima di ogni spedizione abilito la seriale del modbus perchè
	 * potrebbe essere stata disabilita da una non corretta ricezione
	 * per resettare la perifericare e riallinearla; all'interno della
	 * funzione MODBUS_COMM_Enable() viene controllata la flag EnUser
	 * che esegue l'abilitazione della serrtiale solo se essa è disabilitata*/
	MODBUS_COMM_Enable();
#ifdef PUMP_EVER
	RX_ENABLE = FALSE;
	RTS_MOTOR_SetVal();
#endif
	for(char k = 0; k < _funcRetVal.mstreqRetStructNumByte; k++)
	{
		MODBUS_COMM_SendChar(*(_funcRetVal.ptr_msg+k));
	}
}

/*Funzione che riceve in ingresso l'indirizzo con un offset di 2
 * ad esempio se ho rotary select = 0 devo mandare due e la velocità
 * moltiplicata per 100...se devo mandare 10 RPM devo inviare 1000
 * alla funzione. Questa funzione fa muovere la pompa inviando già
 * il comando su MODBUS*/
void setPumpSpeedValue(unsigned char slaveAddr, int speedValue){
	unsigned int mySpeedValue = 0;
	unsigned int * valModBusArrayPtr;
//	char	Address;
	char	funcCode;
	unsigned int	wrAddr;

	int StructId = SelectStruct(slaveAddr);
    pumpPerist[StructId].pmpSpeed = speedValue;

	//Address = slaveAddr;
	funcCode = 0x10;
	wrAddr = 0x0000; /* speed */

	if((slaveAddr >= 2) && (slaveAddr <= 6)) /* pump */
		mySpeedValue = (unsigned int)speedValue;

	valModBusArrayPtr = &mySpeedValue;

	_funcRetValPtr = (struct funcRetStruct *)ModBusWriteRegisterReq(slaveAddr,
											                        funcCode,
											                        wrAddr,
											                        0x0001,
											                        valModBusArrayPtr);
	//send command to actuator
	_funcRetVal.ptr_msg = _funcRetValPtr->ptr_msg;
	_funcRetVal.mstreqRetStructNumByte = _funcRetValPtr->mstreqRetStructNumByte;
	_funcRetVal.slvresRetPtr = _funcRetValPtr->slvresRetPtr;
	_funcRetVal.slvresRetNumByte = _funcRetValPtr->slvresRetNumByte;

	MODBUS_COMM_ClearRxBuf();
	/*prima di ogni spedizione abilito la seriale del modbus perchè
	 * potrebbe essere stata disabilita da una non corretta ricezione
	 * per resettare la perifericare e riallinearla; all'interno della
	 * funzione MODBUS_COMM_Enable() viene controllata la flag EnUser
	 * che esegue l'abilitazione della serrtiale solo se essa è disabilitata*/
	MODBUS_COMM_Enable();
#ifdef PUMP_EVER
	RX_ENABLE = FALSE;
	RTS_MOTOR_SetVal();
#endif
	for(char k = 0; k < _funcRetVal.mstreqRetStructNumByte; k++)
	{
		MODBUS_COMM_SendChar(*(_funcRetVal.ptr_msg+k));
	}
}
#ifdef PUMP_EVER
/*
 le funzioni ad oggi di start stop della pompa saranno:
  setPumpSpeedValueEVER(0x03,0, START);
setPumpSpeedValueEVER(0x03,0, STOP);
0x03 = Address
0 (speed )NOT USED

la funzione per cambiare velocità:

setPumpSpeedValueEVER(0x03,1600, CHANGE_PROFILE_VELOCITY);
0x03 = Address
1600 (speed di esempio, 1600 Hz sarebbero 60 RPM)NOT USED

prima di entrare nel main loop bisogna chiamare la funzione:

setPumpSpeedValueEVER (0x03,0, INIT_PUMP); (scrive 5 indirizzi da 4101H a 4105H)
0x03 = Address
0 (speed )NOT USED
*/
void setPumpSpeedValueEVER(unsigned char slaveAddr, int speedValue,ActionPumpEver Action)
{
	word data[5];
	word * valModBusArrayPtr;
//	char	Address;
	char	funcCode;
	unsigned int	wrAddr;
	unsigned int    numberRegister;

	int StructId = SelectStruct(slaveAddr);
    /*TODO la velocità cambia non solo se chiamo la funzione
     * con CHANGE_PORIFILE_VELOCITY ma anche se poi hiamo la funzione
     * con START, altriemnti la pompa non parte */
	pumpPerist[StructId].pmpSpeed = speedValue;

	for (int i=0; i<5; i++)
	{
		data[i] = 0;
	}

	//Address = slaveAddr;
	funcCode = 0x10;

	if (Action == STOP)
	{
		/*per fare uno stop devo scirvere il registro con cmd = 0 che nl mio caso sarebbe al speed value */
		wrAddr = 0x4105; /* speed */
		numberRegister = 0x0001;
		data[0] = 0;
	}
	else if (Action == START)
	{
		/*per fare uno start devo scirvere il registro con cmd = 1 che nl mio caso sarebbe al speed value */
		wrAddr = 0x4105; /* speed */
		numberRegister = 0x0001;
		data[0] = 1;
	}
	else if (Action == CHANGE_PROFILE_VELOCITY)
	{
		/*per fare una modofica di velocità devo scrivere il registro 1017*/
		wrAddr = 0x1017; /* speed */
		numberRegister = 0x0002;
		data [0] = (word) speedValue >> 16;
		data [1] = (word) speedValue;
	}
	else  if (Action == INIT_PUMP)
	{
		wrAddr = 0x4101; /* speed */
		numberRegister = 0x0005;
		for (int i = 0; i <5; i++)
		{
			if (i==4)
				data[i]=1;
			else
				data[i] = 0;
		}
	}
	else if (Action == CHANGE_VELOCITY)
	{
		wrAddr = 0x0000;
		numberRegister = 0x0004;
		if (speedValue > 0)
		{
			int i = 0;

			data[i++] = 0;
			data[i++] = 1;
			data[i++] = 0; //assumiamo cone max velociutà 65535 ossia 655.35 RPM
			data[i++] = (word) speedValue;

		}
		else
		{
			for (int i = 0; i <4; i++)
				data[i] = 0;
		}


	}

	valModBusArrayPtr = &data[0];

	_funcRetValPtr = (struct funcRetStruct *)ModBusWriteRegisterReqEVER(slaveAddr,
											                        funcCode,
											                        wrAddr,
																	numberRegister,
											                        valModBusArrayPtr);
	//send command to actuator
	_funcRetVal.ptr_msg = _funcRetValPtr->ptr_msg;
	_funcRetVal.mstreqRetStructNumByte = _funcRetValPtr->mstreqRetStructNumByte;
	_funcRetVal.slvresRetPtr = _funcRetValPtr->slvresRetPtr;
	_funcRetVal.slvresRetNumByte = _funcRetValPtr->slvresRetNumByte;

	RX_ENABLE = FALSE;
	RTS_MOTOR_SetVal();

	MODBUS_COMM_ClearRxBuf();
	/*prima di ogni spedizione abilito la seriale del modbus perchè
	 * potrebbe essere stata disabilita da una non corretta ricezione
	 * per resettare la perifericare e riallinearla; all'interno della
	 * funzione MODBUS_COMM_Enable() viene controllata la flag EnUser
	 * che esegue l'abilitazione della serrtiale solo se essa è disabilitata*/
	MODBUS_COMM_Enable();

	for(char k = 0; k < _funcRetVal.mstreqRetStructNumByte; k++)
	{
		MODBUS_COMM_SendChar(*(_funcRetVal.ptr_msg+k));
	}
}
#endif

void readPumpSpeedValue(unsigned char slaveAddr){
	//char	slvAddr;
	char	funcCode;

	//slvAddr = slaveAddr;
	funcCode = 0x03;
	unsigned int readAddr = 0x0011;
	unsigned int numRegRead = 0x0001;

	_funcRetValPtr = (struct funcRetStruct *)ModBusReadRegisterReq(slaveAddr,
										                           funcCode,
											                       readAddr,
											                       numRegRead);

	_funcRetVal.ptr_msg = _funcRetValPtr->ptr_msg;
	_funcRetVal.mstreqRetStructNumByte = _funcRetValPtr->mstreqRetStructNumByte;
	_funcRetVal.slvresRetPtr = _funcRetValPtr->slvresRetPtr;
	_funcRetVal.slvresRetNumByte = _funcRetValPtr->slvresRetNumByte;

	MODBUS_COMM_ClearRxBuf();
	/*prima di ogni spedizione abilito la seriale del modbus perchè
	 * potrebbe essere stata disabilita da una non corretta ricezione
	 * per resettare la perifericare e riallinearla; all'interno della
	 * funzione MODBUS_COMM_Enable() viene controllata la flag EnUser
	 * che esegue l'abilitazione della serrtiale solo se essa è disabilitata*/
	MODBUS_COMM_Enable();
#ifdef PUMP_EVER
	RX_ENABLE = FALSE;
	RTS_MOTOR_SetVal();
#endif
	for(char k = 0; k < _funcRetVal.mstreqRetStructNumByte; k++)
	{
		MODBUS_COMM_SendChar(*(_funcRetVal.ptr_msg+k));
	}
}

void readPumpSpeedValueHighLevel(unsigned char slaveAddr){
	int StructId = SelectStruct(slaveAddr);

	pumpPerist[StructId].ReadRequestPending = 1;

	/*
	//switch((slaveAddr - 2))
	switch(StructId)
		{
		case 0:
			if((pumpPerist[0].reqState == REQ_STATE_OFF) && (pumpPerist[0].reqType == REQ_TYPE_IDLE))
			//if(pumpPerist[0].reqState == REQ_STATE_OFF)
			{
				pumpPerist[0].reqState = REQ_STATE_ON;
				pumpPerist[0].reqType = REQ_TYPE_READ;
				pumpPerist[0].actuatorType = ACTUATOR_PUMP_TYPE;
				pumpPerist[0].dataReady = DATA_READY_FALSE;
			}
			break;

		case 1:
			if((pumpPerist[1].reqState == REQ_STATE_OFF) && (pumpPerist[1].reqType == REQ_TYPE_IDLE))
			//if(pumpPerist[1].reqState == REQ_STATE_OFF)
			{
				pumpPerist[1].reqState = REQ_STATE_ON;
				pumpPerist[1].reqType = REQ_TYPE_READ;
				pumpPerist[1].actuatorType = ACTUATOR_PUMP_TYPE;
				pumpPerist[1].dataReady = DATA_READY_FALSE;
			}
			break;

		case 2:
			if((pumpPerist[2].reqState == REQ_STATE_OFF) && (pumpPerist[2].reqType == REQ_TYPE_IDLE))
			//if(pumpPerist[2].reqState == REQ_STATE_OFF)
			{
				pumpPerist[2].reqState = REQ_STATE_ON;
				pumpPerist[2].reqType = REQ_TYPE_READ;
				pumpPerist[2].actuatorType = ACTUATOR_PUMP_TYPE;
				pumpPerist[2].dataReady = DATA_READY_FALSE;
			}
			break;

		case 3:
			if((pumpPerist[3].reqState == REQ_STATE_OFF) && (pumpPerist[3].reqType == REQ_TYPE_IDLE))
			//if(pumpPerist[3].reqState == REQ_STATE_OFF)
			{
				pumpPerist[3].reqState = REQ_STATE_ON;
				pumpPerist[3].reqType = REQ_TYPE_READ;
				pumpPerist[3].actuatorType = ACTUATOR_PUMP_TYPE;
				pumpPerist[3].dataReady = DATA_READY_FALSE;
			}
			break;

		default:
			break;
		}
		*/
}

void setPinchPositionHighLevel(unsigned char slaveAddr, int posValue){
	switch(slaveAddr)
		{
		case 0x07:  // pinchActuator[0].pinchMySlaveAddress
			pinchActuator[0].reqState = REQ_STATE_ON;
			pinchActuator[0].reqType = REQ_TYPE_WRITE;
			pinchActuator[0].actuatorType = ACTUATOR_PINCH_TYPE;
			pinchActuator[0].value = posValue;
			if(!ActuatorWrRepeatCmd[4])
			{
				// l'indice coincide con LasActuatorWriteID di alwaysModBusActuator
				// il comando high level non e' una ripetizione di un comando precedente quindi resetto il contatore di tentativi
				ActuatorWriteCnt[4] = 0;
			}
			break;

		case 0x08:  // pinchActuator[1].pinchMySlaveAddress
			pinchActuator[1].reqState = REQ_STATE_ON;
			pinchActuator[1].reqType = REQ_TYPE_WRITE;
			pinchActuator[1].actuatorType = ACTUATOR_PINCH_TYPE;
			pinchActuator[1].value = posValue;
			if(!ActuatorWrRepeatCmd[5])
			{
				// l'indice coincide con LasActuatorWriteID di alwaysModBusActuator
				// il comando high level non e' una ripetizione di un comando precedente quindi resetto il contatore di tentativi
				ActuatorWriteCnt[5] = 0;
			}
			break;

		case 0x09:  // pinchActuator[2].pinchMySlaveAddress
			pinchActuator[2].reqState = REQ_STATE_ON;
			pinchActuator[2].reqType = REQ_TYPE_WRITE;
			pinchActuator[2].actuatorType = ACTUATOR_PINCH_TYPE;
			pinchActuator[2].value = posValue;
			if(!ActuatorWrRepeatCmd[6])
			{
				// l'indice coincide con LasActuatorWriteID di alwaysModBusActuator
				// il comando high level non e' una ripetizione di un comando precedente quindi resetto il contatore di tentativi
				ActuatorWriteCnt[6] = 0;
			}
			break;

		default:
			break;
		}
}


void setPinchPosValue(unsigned char slaveAddr, int posValue){
	unsigned int myPinchPosValue = 0;
	unsigned int * valModBusArrayPtr;
//	char	slvAddr;
	char	funcCode;
	unsigned int	wrAddr;

//	slvAddr = slaveAddr;
	funcCode = 0x10;
	wrAddr = 0x0000; /* pinch position */

		if((slaveAddr >= 7) && (slaveAddr <= 9)) /* pinch */
		{
			myPinchPosValue = (unsigned int)posValue;
			pinchActuator[slaveAddr - 7].pinchPosTarget = posValue;
		}

		valModBusArrayPtr = &myPinchPosValue;

		_funcRetValPtr = (struct funcRetStruct *)ModBusWriteRegisterReq(slaveAddr,
												                        funcCode,
												                        wrAddr,
												                        0x0001,
												                        valModBusArrayPtr);
		//send command to actuator
		_funcRetVal.ptr_msg = _funcRetValPtr->ptr_msg;
		_funcRetVal.mstreqRetStructNumByte = _funcRetValPtr->mstreqRetStructNumByte;
		_funcRetVal.slvresRetPtr = _funcRetValPtr->slvresRetPtr;
		_funcRetVal.slvresRetNumByte = _funcRetValPtr->slvresRetNumByte;

		MODBUS_COMM_ClearRxBuf();		/*prima di ogni spedizione abilito la seriale del modbus perchè
		 * potrebbe essere stata disabilita da una non corretta ricezione
		 * per resettare la perifericare e riallinearla; all'interno della
		 * funzione MODBUS_COMM_Enable() viene controllata la flag EnUser
		 * che esegue l'abilitazione della serrtiale solo se essa è disabilitata*/
		MODBUS_COMM_Enable();
#ifdef PUMP_EVER
	RX_ENABLE = FALSE;
	RTS_MOTOR_SetVal();
#endif
		for(char k = 0; k < _funcRetVal.mstreqRetStructNumByte; k++)
		{
			MODBUS_COMM_SendChar(*(_funcRetVal.ptr_msg+k));
		}
}

// in base all'indice dell'ultima scrittura fatta risalgo all'indirizzo fisico
// dell'attuatore a cui era rivolta
// ritorna 2..9
unsigned char GetActuatorPhysAddr(char LasActuatorWriteIDVal)
{
	unsigned char ActId = 0;
	switch (LasActuatorWriteIDVal)
	{
		case 0:
			ActId = pumpPerist[0].pmpMySlaveAddress;
			break;
		case 1:
			ActId = pumpPerist[1].pmpMySlaveAddress;
			break;
		case 2:
			ActId = pumpPerist[2].pmpMySlaveAddress;
			break;
		case 3:
			ActId = pumpPerist[3].pmpMySlaveAddress;
			break;
		case 4:
			ActId = pinchActuator[0].pinchMySlaveAddress;
			break;
		case 5:
			ActId = pinchActuator[1].pinchMySlaveAddress;
			break;
		case 6:
			ActId = pinchActuator[2].pinchMySlaveAddress;
			break;
		case 7:
			break;
	}
}


// ogni 50 msec controlla un attuatore ed esegue l'operazione richiesta.
// Una volta terminata l'operazione mette la flag iflag_pmp1_rx a IFLAG_IDLE
void alwaysModBusActuator(void)
{
	static int timerCounterModBusStart = 0;
	static unsigned long timerCounterModBusStartWr;
	unsigned char *ptrMsg;


	//if( iFlag_modbusDataStorage != TRUE )
	if( ReadActive == TRUE)
	{
		// in questa condizione il modbus e' occupato dalla funzione che legge
		// le velocita' delle pompe
		return;
	}

    if(timerCounterModBus != timerCounterModBusOld)
    {
		timerCounterModBusOld = timerCounterModBus;

		switch((timerCounterModBus%9))
		//switch((FreeRunCnt10msec%9))
		{
		//pump 0
		case 0:
			if(
				(pumpPerist[0].reqState == REQ_STATE_ON) &&
				(pumpPerist[0].reqType == REQ_TYPE_WRITE) &&
				(iflag_pmp1_rx != IFLAG_PMP1_BUSY)
				) //write
			{
				pumpPerist[0].reqState = REQ_STATE_OFF;
				//pumpPerist[0].reqType = REQ_TYPE_IDLE; //la scrittura ha la precedenza sulla lettura; leggo solo se il reqType è idle......questo reqType va messo in idle dopo che è stata letta tutta la risposta
				iflag_pmp1_rx = IFLAG_PMP1_BUSY;
				//timerCounterModBus = 0; //da verificare.....serve azzerare il contatore prima che arrivi la risposta?
				WriteActive = TRUE;
				timerCounterModBusStartWr = FreeRunCnt10msec; //timerCounterModBus;
				setPumpSpeedValue(pumpPerist[0].pmpMySlaveAddress, pumpPerist[0].value);
				LasActuatorWriteID = 0;
			}
			else if(
					(pumpPerist[0].reqState == REQ_STATE_ON) &&
					(pumpPerist[0].reqType == REQ_TYPE_READ) /*&&
					(iflag_pmp1_rx != IFLAG_PMP1_BUSY)*/
					) //read
			{
				pumpPerist[0].reqState = REQ_STATE_OFF;
				pumpPerist[0].dataReady = DATA_READY_TRUE;
				pumpPerist[0].reqType = REQ_TYPE_IDLE;
				/*
				pumpPerist[0].reqState = REQ_STATE_OFF;
				pumpPerist[0].dataReady = DATA_READY_FALSE;
				pumpPerist[0].reqType = REQ_TYPE_IDLE;
				iflag_pmp1_rx = IFLAG_PMP1_BUSY;
				//timerCounterModBus = 0; //da verificare.....serve azzerare il contatore prima che arrivi la risposta?
				readPumpSpeedValue(pumpPerist[0].pmpMySlaveAddress);
				*/
			}

			if(pumpPerist[0].ReadRequestPending)
			{
				pumpPerist[0].dataReady = DATA_READY_TRUE;
				pumpPerist[0].ReadRequestPending = 0;
			}
			break;

		//pump 1
		case 1:
			if(
				(pumpPerist[1].reqState == REQ_STATE_ON) &&
				(pumpPerist[1].reqType == REQ_TYPE_WRITE) &&
				(iflag_pmp1_rx != IFLAG_PMP1_BUSY)
				) //write
			{
				pumpPerist[1].reqState = REQ_STATE_OFF;
				//pumpPerist[1].reqType = REQ_TYPE_IDLE;
				iflag_pmp1_rx = IFLAG_PMP1_BUSY;
				WriteActive = TRUE;
				timerCounterModBusStartWr = FreeRunCnt10msec;  //timerCounterModBus;
#ifdef PUMP_EVER
				setPumpSpeedValueEVER(pumpPerist[1].pmpMySlaveAddress,pumpPerist[1].value, CHANGE_VELOCITY);
#else
				setPumpSpeedValue(pumpPerist[1].pmpMySlaveAddress, pumpPerist[1].value);
#endif
				LasActuatorWriteID = 1;
			}
			else if(
					(pumpPerist[1].reqState == REQ_STATE_ON) &&
					(pumpPerist[1].reqType == REQ_TYPE_READ) /*&&
					(iflag_pmp1_rx != IFLAG_PMP1_BUSY)*/
					) //read
			{
				pumpPerist[1].reqState = REQ_STATE_OFF;
				// il valore della velocita' e' gia' pronto
				pumpPerist[1].dataReady = DATA_READY_TRUE;
				pumpPerist[1].reqType = REQ_TYPE_IDLE;

				/*
				pumpPerist[1].reqState = REQ_STATE_OFF;
				pumpPerist[1].dataReady = DATA_READY_FALSE;
				pumpPerist[1].reqType = REQ_TYPE_IDLE;
				iflag_pmp1_rx = IFLAG_PMP1_BUSY;
				readPumpSpeedValue(pumpPerist[1].pmpMySlaveAddress);
				*/
			}
			if(pumpPerist[1].ReadRequestPending)
			{
				pumpPerist[1].dataReady = DATA_READY_TRUE;
				pumpPerist[1].ReadRequestPending = 0;
			}
			break;

		//pump 2
		case 2:
			if(
				(pumpPerist[2].reqState == REQ_STATE_ON) &&
				(pumpPerist[2].reqType == REQ_TYPE_WRITE) &&
				(iflag_pmp1_rx != IFLAG_PMP1_BUSY)
				) //write
			{
				pumpPerist[2].reqState = REQ_STATE_OFF;
				//pumpPerist[2].reqType = REQ_TYPE_IDLE;
				iflag_pmp1_rx = IFLAG_PMP1_BUSY;
				WriteActive = TRUE;
				timerCounterModBusStartWr = FreeRunCnt10msec;  //timerCounterModBus;
#ifdef PUMP_EVER
				setPumpSpeedValueEVER(pumpPerist[2].pmpMySlaveAddress,pumpPerist[2].value, CHANGE_VELOCITY);
#else
				setPumpSpeedValue(pumpPerist[2].pmpMySlaveAddress, pumpPerist[2].value);
#endif
				LasActuatorWriteID = 2;
			}
			else if(
					(pumpPerist[2].reqState == REQ_STATE_ON) &&
					(pumpPerist[2].reqType == REQ_TYPE_READ) /*&&
					(iflag_pmp1_rx != IFLAG_PMP1_BUSY)*/
					) //read
			{
				pumpPerist[2].reqState = REQ_STATE_OFF;
				// il valore della velocita' e' gia' pronto
				pumpPerist[2].dataReady = DATA_READY_TRUE;
				pumpPerist[2].reqType = REQ_TYPE_IDLE;
				/*
				pumpPerist[2].reqState = REQ_STATE_OFF;
				pumpPerist[2].dataReady = DATA_READY_FALSE;
				pumpPerist[2].reqType = REQ_TYPE_IDLE;
				iflag_pmp1_rx = IFLAG_PMP1_BUSY;
				readPumpSpeedValue(pumpPerist[2].pmpMySlaveAddress);
				*/
			}
			if(pumpPerist[2].ReadRequestPending)
			{
				pumpPerist[2].dataReady = DATA_READY_TRUE;
				pumpPerist[2].ReadRequestPending = 0;
			}
			break;

		//pump 3
		case 3:
			if(
				(pumpPerist[3].reqState == REQ_STATE_ON) &&
				(pumpPerist[3].reqType == REQ_TYPE_WRITE) &&
				(iflag_pmp1_rx != IFLAG_PMP1_BUSY)
				) //write
			{
				pumpPerist[3].reqState = REQ_STATE_OFF;
				//pumpPerist[3].reqType = REQ_TYPE_IDLE;
				iflag_pmp1_rx = IFLAG_PMP1_BUSY;
				WriteActive = TRUE;
				timerCounterModBusStartWr = FreeRunCnt10msec; //timerCounterModBus;
				setPumpSpeedValue(pumpPerist[3].pmpMySlaveAddress, pumpPerist[3].value);
				LasActuatorWriteID = 3;
			}
			else if(
					(pumpPerist[3].reqState == REQ_STATE_ON) &&
					(pumpPerist[3].reqType == REQ_TYPE_READ) /*&&
					(iflag_pmp1_rx != IFLAG_PMP1_BUSY)*/
					) //read
			{
				pumpPerist[3].reqState = REQ_STATE_OFF;
				// il valore della velocita' e' gia' pronto
				pumpPerist[3].dataReady = DATA_READY_TRUE;
				pumpPerist[3].reqType = REQ_TYPE_IDLE;
				/*
				pumpPerist[3].reqState = REQ_STATE_OFF;
				pumpPerist[3].dataReady = DATA_READY_FALSE;
				pumpPerist[3].reqType = REQ_TYPE_IDLE;
				iflag_pmp1_rx = IFLAG_PMP1_BUSY;
				readPumpSpeedValue(pumpPerist[3].pmpMySlaveAddress);
				*/
			}
			if(pumpPerist[3].ReadRequestPending)
			{
				pumpPerist[3].dataReady = DATA_READY_TRUE;
				pumpPerist[3].ReadRequestPending = 0;
			}
			break;

		//pinch 1 - arterial
		case 4:
			if(
				(pinchActuator[0].reqState == REQ_STATE_ON) &&
				(pinchActuator[0].reqType == REQ_TYPE_WRITE) &&
				(iflag_pmp1_rx != IFLAG_PMP1_BUSY)
				) //write
			{
				pinchActuator[0].reqState = REQ_STATE_OFF;
				//pinchActuator[0].reqType = REQ_TYPE_IDLE; //la scrittura ha la precedenza sulla lettura; leggo solo se il reqType è idle......questo reqType va messo in idle dopo che è stata letta tutta la risposta
				iflag_pmp1_rx = IFLAG_PMP1_BUSY;
				//timerCounterModBus = 0; //da verificare.....serve azzerare il contatore prima che arrivi la risposta?
				WriteActive = TRUE;
				timerCounterModBusStartWr = FreeRunCnt10msec; //timerCounterModBus;
				setPinchPosValue(pinchActuator[0].pinchMySlaveAddress, pinchActuator[0].value);
				LasActuatorWriteID = 4;
			}
			break;

		//pinch 2 - venous
		case 5:
			if(
				(pinchActuator[1].reqState == REQ_STATE_ON) &&
				(pinchActuator[1].reqType == REQ_TYPE_WRITE) &&
				(iflag_pmp1_rx != IFLAG_PMP1_BUSY)
				) //write
			{
				pinchActuator[1].reqState = REQ_STATE_OFF;
				//pinchActuator[1].reqType = REQ_TYPE_IDLE; //la scrittura ha la precedenza sulla lettura; leggo solo se il reqType è idle......questo reqType va messo in idle dopo che è stata letta tutta la risposta
				iflag_pmp1_rx = IFLAG_PMP1_BUSY;
				//timerCounterModBus = 0; //da verificare.....serve azzerare il contatore prima che arrivi la risposta?
				WriteActive = TRUE;
				timerCounterModBusStartWr = FreeRunCnt10msec;  //timerCounterModBus;
				setPinchPosValue(pinchActuator[1].pinchMySlaveAddress, pinchActuator[1].value);
				LasActuatorWriteID = 5;
			}
			break;

		//pinch 3 - absorbent filter
		case 6:
			if(
				(pinchActuator[2].reqState == REQ_STATE_ON) &&
				(pinchActuator[2].reqType == REQ_TYPE_WRITE) &&
				(iflag_pmp1_rx != IFLAG_PMP1_BUSY)
				) //write
			{
				pinchActuator[2].reqState = REQ_STATE_OFF;
				//pinchActuator[2].reqType = REQ_TYPE_IDLE; //la scrittura ha la precedenza sulla lettura; leggo solo se il reqType è idle......questo reqType va messo in idle dopo che è stata letta tutta la risposta
				iflag_pmp1_rx = IFLAG_PMP1_BUSY;
				//timerCounterModBus = 0; //da verificare.....serve azzerare il contatore prima che arrivi la risposta?
				WriteActive = TRUE;
				timerCounterModBusStartWr = FreeRunCnt10msec;  //timerCounterModBus;
				setPinchPosValue(pinchActuator[2].pinchMySlaveAddress, pinchActuator[2].value);
				LasActuatorWriteID = 6;
			}
			break;

		//pinch 4 NOT USED
		case 7:
			break;

		default:
			break;
		}
    }

	if((iflag_pmp1_rx == IFLAG_PMP1_RX) && (WriteActive == TRUE))
	{
		unsigned char Adr;
		WriteActive = FALSE;
		ptrMsg = msgToRecvFrame10;
		Adr = *ptrMsg;
		if((*(ptrMsg + 1) == 0x10) && (Adr >= 7) && (GetActuatorPhysAddr(LasActuatorWriteID) == Adr))
		{
			// ho ricevuto la risposta da un pinch
			//pinchActuator[Adr - 7].dataReady = DATA_READY_TRUE;
			pinchActuator[Adr - 7].reqType = REQ_TYPE_IDLE;
			ActuatorWriteCnt[LasActuatorWriteID] = 0;  // reset del contatore di errore
			ActuatorWrRepeatCmd[LasActuatorWriteID] = 0;
		}
		else if((*(ptrMsg + 1) == 0x10) && (GetActuatorPhysAddr(LasActuatorWriteID) == Adr))
		{
			// ho ricevuto la risposta dalla pompa
			//pumpPerist[Adr - 2].dataReady = DATA_READY_TRUE;
			//pumpPerist[Adr - 2].reqType = REQ_TYPE_IDLE;
			pumpPerist[LasActuatorWriteID].reqType = REQ_TYPE_IDLE;
			ActuatorWriteCnt[LasActuatorWriteID] = 0; // reset del contatore di errore
			ActuatorWrRepeatCmd[LasActuatorWriteID] = 0;
		}
//		if((*(ptrMsg + 1) == 0x10) && (Adr > 7) && ((Adr - 7) == (LasActuatorWriteID - 4)))
//		{
//			// ho ricevuto la risposta da un pinch
//			//pinchActuator[Adr - 7].dataReady = DATA_READY_TRUE;
//			pinchActuator[Adr - 7].reqType = REQ_TYPE_IDLE;
//			ActuatorWriteCnt[LasActuatorWriteID] = 0;  // reset del contatore di errore
//			ActuatorWrRepeatCmd[LasActuatorWriteID] = 0;
//		}
//		else if((*(ptrMsg + 1) == 0x10) && ((Adr - 2) == LasActuatorWriteID))
//		{
//			// ho ricevuto la risposta dalla pompa
//			//pumpPerist[Adr - 2].dataReady = DATA_READY_TRUE;
//			pumpPerist[Adr - 2].reqType = REQ_TYPE_IDLE;
//			ActuatorWriteCnt[LasActuatorWriteID] = 0; // reset del contatore di errore
//			ActuatorWrRepeatCmd[LasActuatorWriteID] = 0;
//		}
		else
		{
			ActuatorWriteCnt[LasActuatorWriteID] += 1;
			if(ActuatorWriteCnt[LasActuatorWriteID] >= MAX_MSG_CONSECUTIVE_ACTUATOR_MODBUS_NOT_RESPOND)
			{
				// termino la sequenza di scritture e posso generare un allarme
				if(LasActuatorWriteID <= 3)
					pumpPerist[LasActuatorWriteID].reqType = REQ_TYPE_IDLE;
				else if(LasActuatorWriteID <= 6)
					pinchActuator[LasActuatorWriteID - 4].reqType = REQ_TYPE_IDLE;
			}
			else
			{
				// risposta errata, ritento l'ultima scrittura
				if(LasActuatorWriteID <= 3)
				{
					// l'ultima scrittura era una pompa
					ActuatorWrRepeatCmd[LasActuatorWriteID] = 1;
					setPumpSpeedValueHighLevel(pumpPerist[LasActuatorWriteID].pmpMySlaveAddress, pumpPerist[LasActuatorWriteID].value);
					ActuatorWrRepeatCmd[LasActuatorWriteID] = 0;
					//pumpPerist[LasActuatorWriteID].dataReady = DATA_READY_TRUE;
					//pumpPerist[LasActuatorWriteID].reqType = REQ_TYPE_IDLE;
				}
				else if(LasActuatorWriteID <= 6)
				{
					// l'ultima scrittura era una pinch
					ActuatorWrRepeatCmd[LasActuatorWriteID] = 1;
					setPinchPositionHighLevel(pinchActuator[LasActuatorWriteID - 4].pinchMySlaveAddress, pinchActuator[LasActuatorWriteID - 4].value);
					ActuatorWrRepeatCmd[LasActuatorWriteID] = 0;
					//pinchActuator[LasActuatorWriteID - 4].dataReady = DATA_READY_TRUE; //non serve questo
					//pinchActuator[LasActuatorWriteID - 4].reqType = REQ_TYPE_IDLE;
				}
			}
		}

		iflag_pmp1_rx = IFLAG_IDLE; // resetto la flag di pacchetto modbus completo
		timerCounterModBusStartWr = 0;

	}
	else //if(iflag_pmp1_rx == IFLAG_PMP1_BUSY)
	{
		// sono in attesa della risposta da un attuatore pinch o pompa
		if(timerCounterModBusStartWr && (msTick10_elapsed(timerCounterModBusStartWr) >= 3))
		{
			// sono passati 30 msec e non ho ancora ricevuto alcuna risposta
			WriteActive = FALSE; // termino la fase di scritture che era rimasta aperta
			timerCounterModBusStartWr = 0;
			iflag_pmp1_rx = IFLAG_IDLE;
			// incremento il contatore delle scritture errate
			ActuatorWriteCnt[LasActuatorWriteID] += 1;

			if(ActuatorWriteCnt[LasActuatorWriteID] >= MAX_MSG_CONSECUTIVE_ACTUATOR_MODBUS_NOT_RESPOND)
			{
				// termino la sequenza di scritture e posso generare un allarme
				if(LasActuatorWriteID <= 3)
					pumpPerist[LasActuatorWriteID].reqType = REQ_TYPE_IDLE;
				else if(LasActuatorWriteID <= 6)
					pinchActuator[LasActuatorWriteID - 4].reqType = REQ_TYPE_IDLE;
			}
			else
			{
				// dato che non ho avuto alcuna risposta, ritento l'ultima scrittura
				if(LasActuatorWriteID <= 3)
				{
					// l'ultima scrittura era una pompa
					ActuatorWrRepeatCmd[LasActuatorWriteID] = 1;
					setPumpSpeedValueHighLevel(pumpPerist[LasActuatorWriteID].pmpMySlaveAddress, pumpPerist[LasActuatorWriteID].value);
					ActuatorWrRepeatCmd[LasActuatorWriteID] = 0;
				}
				else if(LasActuatorWriteID <= 6)
				{
					// l'ultima scrittura era una pinch
					ActuatorWrRepeatCmd[LasActuatorWriteID] = 1;
					setPinchPositionHighLevel(pinchActuator[LasActuatorWriteID - 4].pinchMySlaveAddress, pinchActuator[LasActuatorWriteID - 4].value);
					ActuatorWrRepeatCmd[LasActuatorWriteID] = 0;
				}
			}
		}
	}
}

void alwaysModBusActuator_old(void)
{
	static char ActuatorWriteCnt = 0;
	static int timerCounterModBusStart = 0;
	static char AlarmSet = 0;
	unsigned char *ptrMsg;



	//if( iFlag_modbusDataStorage != TRUE )
	if( ReadActive == TRUE)
	{
		// in questa condizione il modbus e' occupato dalla funzione che legge
		// le velocita' delle pompe
		return;
	}

    if(timerCounterModBus != timerCounterModBusOld)
    {
		timerCounterModBusOld = timerCounterModBus;

		switch((timerCounterModBus%9))
		//switch((FreeRunCnt10msec%9))
		{
		//pump 0
		case 0:
			if(
				(pumpPerist[0].reqState == REQ_STATE_ON) &&
				(pumpPerist[0].reqType == REQ_TYPE_WRITE) &&
				(iflag_pmp1_rx != IFLAG_PMP1_BUSY)
				) //write
			{
				pumpPerist[0].reqState = REQ_STATE_OFF;
				//pumpPerist[0].reqType = REQ_TYPE_IDLE; //la scrittura ha la precedenza sulla lettura; leggo solo se il reqType è idle......questo reqType va messo in idle dopo che è stata letta tutta la risposta
				iflag_pmp1_rx = IFLAG_PMP1_BUSY;
				//timerCounterModBus = 0; //da verificare.....serve azzerare il contatore prima che arrivi la risposta?
				WriteActive = TRUE;
				setPumpSpeedValue(pumpPerist[0].pmpMySlaveAddress, pumpPerist[0].value);
				LasActuatorWriteID = 0;
				ActuatorWriteCnt = 0;
			}
			else if(
					(pumpPerist[0].reqState == REQ_STATE_ON) &&
					(pumpPerist[0].reqType == REQ_TYPE_READ) /*&&
					(iflag_pmp1_rx != IFLAG_PMP1_BUSY)*/
					) //read
			{
				pumpPerist[0].reqState = REQ_STATE_OFF;
				pumpPerist[0].dataReady = DATA_READY_TRUE;
				pumpPerist[0].reqType = REQ_TYPE_IDLE;
				/*
				pumpPerist[0].reqState = REQ_STATE_OFF;
				pumpPerist[0].dataReady = DATA_READY_FALSE;
				pumpPerist[0].reqType = REQ_TYPE_IDLE;
				iflag_pmp1_rx = IFLAG_PMP1_BUSY;
				//timerCounterModBus = 0; //da verificare.....serve azzerare il contatore prima che arrivi la risposta?
				readPumpSpeedValue(pumpPerist[0].pmpMySlaveAddress);
				*/
			}

			if(pumpPerist[0].ReadRequestPending)
			{
				pumpPerist[0].dataReady = DATA_READY_TRUE;
				pumpPerist[0].ReadRequestPending = 0;
			}
			break;

		//pump 1
		case 1:
			if(
				(pumpPerist[1].reqState == REQ_STATE_ON) &&
				(pumpPerist[1].reqType == REQ_TYPE_WRITE) &&
				(iflag_pmp1_rx != IFLAG_PMP1_BUSY)
				) //write
			{
				pumpPerist[1].reqState = REQ_STATE_OFF;
				//pumpPerist[1].reqType = REQ_TYPE_IDLE;
				iflag_pmp1_rx = IFLAG_PMP1_BUSY;
				WriteActive = TRUE;
				setPumpSpeedValue(pumpPerist[1].pmpMySlaveAddress, pumpPerist[1].value);
				LasActuatorWriteID = 1;
				ActuatorWriteCnt = 0;
			}
			else if(
					(pumpPerist[1].reqState == REQ_STATE_ON) &&
					(pumpPerist[1].reqType == REQ_TYPE_READ) /*&&
					(iflag_pmp1_rx != IFLAG_PMP1_BUSY)*/
					) //read
			{
				pumpPerist[1].reqState = REQ_STATE_OFF;
				// il valore della velocita' e' gia' pronto
				pumpPerist[1].dataReady = DATA_READY_TRUE;
				pumpPerist[1].reqType = REQ_TYPE_IDLE;

				/*
				pumpPerist[1].reqState = REQ_STATE_OFF;
				pumpPerist[1].dataReady = DATA_READY_FALSE;
				pumpPerist[1].reqType = REQ_TYPE_IDLE;
				iflag_pmp1_rx = IFLAG_PMP1_BUSY;
				readPumpSpeedValue(pumpPerist[1].pmpMySlaveAddress);
				*/
			}
			if(pumpPerist[1].ReadRequestPending)
			{
				pumpPerist[1].dataReady = DATA_READY_TRUE;
				pumpPerist[1].ReadRequestPending = 0;
			}
			break;

		//pump 2
		case 2:
			if(
				(pumpPerist[2].reqState == REQ_STATE_ON) &&
				(pumpPerist[2].reqType == REQ_TYPE_WRITE) &&
				(iflag_pmp1_rx != IFLAG_PMP1_BUSY)
				) //write
			{
				pumpPerist[2].reqState = REQ_STATE_OFF;
				//pumpPerist[2].reqType = REQ_TYPE_IDLE;
				iflag_pmp1_rx = IFLAG_PMP1_BUSY;
				WriteActive = TRUE;
				setPumpSpeedValue(pumpPerist[2].pmpMySlaveAddress, pumpPerist[2].value);
				LasActuatorWriteID = 2;
				ActuatorWriteCnt = 0;
			}
			else if(
					(pumpPerist[2].reqState == REQ_STATE_ON) &&
					(pumpPerist[2].reqType == REQ_TYPE_READ) /*&&
					(iflag_pmp1_rx != IFLAG_PMP1_BUSY)*/
					) //read
			{
				pumpPerist[2].reqState = REQ_STATE_OFF;
				// il valore della velocita' e' gia' pronto
				pumpPerist[2].dataReady = DATA_READY_TRUE;
				pumpPerist[2].reqType = REQ_TYPE_IDLE;
				/*
				pumpPerist[2].reqState = REQ_STATE_OFF;
				pumpPerist[2].dataReady = DATA_READY_FALSE;
				pumpPerist[2].reqType = REQ_TYPE_IDLE;
				iflag_pmp1_rx = IFLAG_PMP1_BUSY;
				readPumpSpeedValue(pumpPerist[2].pmpMySlaveAddress);
				*/
			}
			if(pumpPerist[2].ReadRequestPending)
			{
				pumpPerist[2].dataReady = DATA_READY_TRUE;
				pumpPerist[2].ReadRequestPending = 0;
			}
			break;

		//pump 3
		case 3:
			if(
				(pumpPerist[3].reqState == REQ_STATE_ON) &&
				(pumpPerist[3].reqType == REQ_TYPE_WRITE) &&
				(iflag_pmp1_rx != IFLAG_PMP1_BUSY)
				) //write
			{
				pumpPerist[3].reqState = REQ_STATE_OFF;
				//pumpPerist[3].reqType = REQ_TYPE_IDLE;
				iflag_pmp1_rx = IFLAG_PMP1_BUSY;
				WriteActive = TRUE;
				setPumpSpeedValue(pumpPerist[3].pmpMySlaveAddress, pumpPerist[3].value);
				LasActuatorWriteID = 3;
				ActuatorWriteCnt = 0;
			}
			else if(
					(pumpPerist[3].reqState == REQ_STATE_ON) &&
					(pumpPerist[3].reqType == REQ_TYPE_READ) /*&&
					(iflag_pmp1_rx != IFLAG_PMP1_BUSY)*/
					) //read
			{
				pumpPerist[3].reqState = REQ_STATE_OFF;
				// il valore della velocita' e' gia' pronto
				pumpPerist[3].dataReady = DATA_READY_TRUE;
				pumpPerist[3].reqType = REQ_TYPE_IDLE;
				/*
				pumpPerist[3].reqState = REQ_STATE_OFF;
				pumpPerist[3].dataReady = DATA_READY_FALSE;
				pumpPerist[3].reqType = REQ_TYPE_IDLE;
				iflag_pmp1_rx = IFLAG_PMP1_BUSY;
				readPumpSpeedValue(pumpPerist[3].pmpMySlaveAddress);
				*/
			}
			if(pumpPerist[3].ReadRequestPending)
			{
				pumpPerist[3].dataReady = DATA_READY_TRUE;
				pumpPerist[3].ReadRequestPending = 0;
			}
			break;

		//pinch 1 - arterial
		case 4:
			if(
				(pinchActuator[0].reqState == REQ_STATE_ON) &&
				(pinchActuator[0].reqType == REQ_TYPE_WRITE) &&
				(iflag_pmp1_rx != IFLAG_PMP1_BUSY)
				) //write
			{
				pinchActuator[0].reqState = REQ_STATE_OFF;
				//pinchActuator[0].reqType = REQ_TYPE_IDLE; //la scrittura ha la precedenza sulla lettura; leggo solo se il reqType è idle......questo reqType va messo in idle dopo che è stata letta tutta la risposta
				iflag_pmp1_rx = IFLAG_PMP1_BUSY;
				//timerCounterModBus = 0; //da verificare.....serve azzerare il contatore prima che arrivi la risposta?
				WriteActive = TRUE;
				setPinchPosValue(pinchActuator[0].pinchMySlaveAddress, pinchActuator[0].value);
				LasActuatorWriteID = 4;
				ActuatorWriteCnt = 0;
			}
			break;

		//pinch 2 - venous
		case 5:
			if(
				(pinchActuator[1].reqState == REQ_STATE_ON) &&
				(pinchActuator[1].reqType == REQ_TYPE_WRITE) &&
				(iflag_pmp1_rx != IFLAG_PMP1_BUSY)
				) //write
			{
				pinchActuator[1].reqState = REQ_STATE_OFF;
				//pinchActuator[1].reqType = REQ_TYPE_IDLE; //la scrittura ha la precedenza sulla lettura; leggo solo se il reqType è idle......questo reqType va messo in idle dopo che è stata letta tutta la risposta
				iflag_pmp1_rx = IFLAG_PMP1_BUSY;
				//timerCounterModBus = 0; //da verificare.....serve azzerare il contatore prima che arrivi la risposta?
				WriteActive = TRUE;
				setPinchPosValue(pinchActuator[1].pinchMySlaveAddress, pinchActuator[1].value);
				LasActuatorWriteID = 5;
				ActuatorWriteCnt = 0;
			}
			break;

		//pinch 3 - absorbent filter
		case 6:
			if(
				(pinchActuator[2].reqState == REQ_STATE_ON) &&
				(pinchActuator[2].reqType == REQ_TYPE_WRITE) &&
				(iflag_pmp1_rx != IFLAG_PMP1_BUSY)
				) //write
			{
				pinchActuator[2].reqState = REQ_STATE_OFF;
				//pinchActuator[2].reqType = REQ_TYPE_IDLE; //la scrittura ha la precedenza sulla lettura; leggo solo se il reqType è idle......questo reqType va messo in idle dopo che è stata letta tutta la risposta
				iflag_pmp1_rx = IFLAG_PMP1_BUSY;
				//timerCounterModBus = 0; //da verificare.....serve azzerare il contatore prima che arrivi la risposta?
				WriteActive = TRUE;
				setPinchPosValue(pinchActuator[2].pinchMySlaveAddress, pinchActuator[2].value);
				LasActuatorWriteID = 6;
				ActuatorWriteCnt = 0;
			}
			break;

		//pinch 4 NOT USED
		case 7:
			break;

		default:
			break;
		}
    }

	if((iflag_pmp1_rx == IFLAG_PMP1_RX) && (WriteActive == TRUE))
	{
		unsigned char Adr;
		WriteActive = FALSE;
		ptrMsg = msgToRecvFrame10;
		Adr = *ptrMsg;
		//if( (*(ptrMsg + 1) == 0x10) && ((Adr - 2) == LasActuatorWriteID) )
		if( (*(ptrMsg + 1) == 0x10) && (GetActuatorPhysAddr(LasActuatorWriteID) == Adr) )
		{
			// ho ricevuto la risposta dalla pompa
			//pumpPerist[Adr - 2].dataReady = DATA_READY_TRUE;
			pumpPerist[LasActuatorWriteID].reqType = REQ_TYPE_IDLE;
		}
		else if((*(ptrMsg + 1) == 0x10) && (Adr >= 7) && ((Adr - 7) == (LasActuatorWriteID - 4)))
		{
			// ho ricevuto la risposta da un pinch
			//pinchActuator[Adr - 7].dataReady = DATA_READY_TRUE;
			pinchActuator[Adr - 7].reqType = REQ_TYPE_IDLE;
		}
		else
		{
			// risposta errata, ritento l'ultima scrittura
			if(LasActuatorWriteID <= 3)
			{
				// l'ultima scrittura era una pompa
				setPumpSpeedValueHighLevel(pumpPerist[LasActuatorWriteID].pmpMySlaveAddress, pumpPerist[LasActuatorWriteID].value);
				//pumpPerist[LasActuatorWriteID].dataReady = DATA_READY_TRUE;
				//pumpPerist[LasActuatorWriteID].reqType = REQ_TYPE_IDLE;
			}
			else if(LasActuatorWriteID <= 6)
			{
				// l'ultima scrittura era una pinch
				setPinchPositionHighLevel(pinchActuator[LasActuatorWriteID - 4].pinchMySlaveAddress, pinchActuator[LasActuatorWriteID - 4].value);
				//pinchActuator[LasActuatorWriteID - 4].dataReady = DATA_READY_TRUE; //non serve questo
				//pinchActuator[LasActuatorWriteID - 4].reqType = REQ_TYPE_IDLE;
			}

//			if(ActuatorWriteCnt >= 2)
//			{
//				if(!AlarmSet)
//				{
//					SetNonPhysicalAlm((int)CODE_ALARM_MODBUS_ACTUATOR_SEND);
//					timerCounterModBusStart = timerCounterModBus;
//					AlarmSet = 1;
//				}
//				ActuatorWriteCnt = 0;
//			}
		}

		iflag_pmp1_rx = IFLAG_IDLE; // resetto la flag di pacchetto modbus completo

	}
	else
	{

//		for(int i=0; i<=3; i++)
//		{
//			if((pumpPerist[i].reqState == REQ_STATE_OFF) && (pumpPerist[i].reqType != REQ_TYPE_IDLE))
//			{
//				if(pumpIdleTimeout)
//				pumpPerist[i].reqType = REQ_TYPE_IDLE;
//			}
//		}

	}

//	if( AlarmSet && (msTick_elapsed(timerCounterModBusStart) >= 25))
//	{
//		// dopo 250 msec tolgo automaticamente l'allarme generato per l'errore di retry su modbus
//		ClearNonPhysicalAlm( (int)CODE_ALARM_MODBUS_ACTUATOR_SEND);
//		AlarmSet = 0;
//	}
}

bool CommandModBusPMPExecute(int SpeedPMP_0, int SpeedPMP_1_2, int SpeedPMP_3)
{

	unsigned int Speed_Pmp_0 = modbusData [0][17];
	unsigned int Speed_Pmp_1 = modbusData [1][17];
	unsigned int Speed_Pmp_2 = modbusData [2][17];
	unsigned int Speed_Pmp_3 = modbusData [3][17];

	if (Speed_Pmp_0 == SpeedPMP_0    &&
		Speed_Pmp_1 == SpeedPMP_1_2 &&
		Speed_Pmp_2 == SpeedPMP_1_2 &&
		Speed_Pmp_3 == SpeedPMP_3
		)
	{
		return (TRUE);
	}
	else
		return(FALSE);
}

#ifdef PUMP_EVER
/*funzione per controllare lo stato dei motori*/
void Check_Actuator_Status_Ever (char slaveAddr,
							     char funcCode,
							     int readAddrStart,
							     int numberOfAddress)
{
	word snd;

	_funcRetValPtr = (struct funcRetStruct *)ModBusReadRegisterReqEVER(slaveAddr,
										                               funcCode,
										                               readAddrStart,
										                               numberOfAddress);

	_funcRetVal.ptr_msg = _funcRetValPtr->ptr_msg;
	_funcRetVal.mstreqRetStructNumByte = _funcRetValPtr->mstreqRetStructNumByte;
	_funcRetVal.slvresRetPtr = _funcRetValPtr->slvresRetPtr;
	_funcRetVal.slvresRetNumByte = _funcRetValPtr->slvresRetNumByte;




	/* prima di ogni spedizione abilito la seriale del modbus perchè
	 * potrebbe essere stata disabilita da una non corretta ricezione
	 * per resettare la perifericare e riallinearla; all'interno della
	 * funzione MODBUS_COMM_Enable() viene controllata la flag EnUser
	 * che esegue l'abilitazione della serrtiale solo se essa è disabilitata*/
	MODBUS_COMM_Enable();
	RX_ENABLE = FALSE;
	RTS_MOTOR_SetVal();
	for(char k = 0; k < _funcRetVal.mstreqRetStructNumByte; k++)
	{
		MODBUS_COMM_SendChar(*(_funcRetVal.ptr_msg+k));
	}
}
#endif


/*funzione per controllare lo stato dei motori*/
void Check_Actuator_Status (char slaveAddr,
							char funcCode,
							int readAddrStart,
							int numberOfAddress)
{
	word snd;

	_funcRetValPtr = (struct funcRetStruct *)ModBusReadRegisterReq(slaveAddr,
										                           funcCode,
										                           readAddrStart,
										                           numberOfAddress);

	_funcRetVal.ptr_msg = _funcRetValPtr->ptr_msg;
	_funcRetVal.mstreqRetStructNumByte = _funcRetValPtr->mstreqRetStructNumByte;
	_funcRetVal.slvresRetPtr = _funcRetValPtr->slvresRetPtr;
	_funcRetVal.slvresRetNumByte = _funcRetValPtr->slvresRetNumByte;




	/* prima di ogni spedizione abilito la seriale del modbus perchè
	 * potrebbe essere stata disabilita da una non corretta ricezione
	 * per resettare la perifericare e riallinearla; all'interno della
	 * funzione MODBUS_COMM_Enable() viene controllata la flag EnUser
	 * che esegue l'abilitazione della serrtiale solo se essa è disabilitata*/
	MODBUS_COMM_Enable();
#ifdef PUMP_EVER
	RX_ENABLE = FALSE;
	RTS_MOTOR_SetVal();
#endif
	for(char k = 0; k < _funcRetVal.mstreqRetStructNumByte; k++)
	{
		MODBUS_COMM_SendChar(*(_funcRetVal.ptr_msg+k));
	}
}

void Manage_and_Storage_ModBus_Actuator_Data(void)
{
	  static unsigned char LastActuatslvAddr = 0;  // indirizzo dell'ultima pompa o pinch  che ho usato in Check_Actuator_Status
	  word 			readAddrStart		 		= 0x0010;
	  unsigned char numberOfAddressCheckPump	= 0x03;
	  unsigned char numberOfAddressCheckPinch	= 0x02;
	  unsigned char funcCode 					= 0x03;
#ifdef PUMP_EVER
	  unsigned char numberOfAddressCheckPumpEver = 0x03;
	  word 			readAddrStartEver	 		 = 0x1008; // registro Current_actual_value di Ever
#endif

	 if(WriteActive == TRUE)
	 {
		 /* controllo anche che l'eventuale operazione di scrittura fatta in alwaysModBusActuator sia terminata */
		 return;
	 }

 	/*se ho ricevuto un dato me lo vado a memorizzare nella mia struttura globale: 'modbusData'*/
 	if (iFlag_actuatorCheck == IFLAG_COMMAND_RECEIVED && iFlag_modbusDataStorage == FALSE)
 	{
 		StorageModbusData(LastActuatslvAddr);
 		iFlag_modbusDataStorage = TRUE;
 		ReadActive = FALSE;
 		iflag_pmp1_rx = IFLAG_IDLE; //libero la flafg di ricezione per la alwaysModBusActuator
 	}
 	/*chiamo la funzione ogni 50 msec*/
 	if (timerCounterCheckModBus >= 1)
    {
        ReadActive = TRUE;
     	if (iFlag_actuatorCheck != IFLAG_COMMAND_RECEIVED )
     	{
     		//TODO aggiungere controllo mancata risposta
     	}

     	iFlag_actuatorCheck = IFLAG_COMMAND_SENT;
     	iFlag_modbusDataStorage = FALSE;
		timerCounterCheckModBus = 0;

		/*L'indirizzo slvAddr = 6 non è usato*/
		if (slvAddr == 6)
        	slvAddr= 7;

		CountErrorModbusMSG[slvAddr - 2]++;

        /*chiamo la funzione col corretto number of address dipendentemente dall'attuatore (pump/pinch)*/
#ifdef PUMP_EVER
		if(slvAddr == (LAST_PUMP - 1) || slvAddr == LAST_PUMP)
		{
			/*funzione che mi legge lo stato delle pompe*/
			Check_Actuator_Status_Ever (slvAddr, funcCode, readAddrStartEver, numberOfAddressCheckPumpEver);
			LastActuatslvAddr = slvAddr;
		}
		else if (slvAddr <= (LAST_PUMP - 2))
#else
		if (slvAddr <= LAST_PUMP)
#endif
		{
			/*funzione che mi legge lo stato delle pompe*/
			Check_Actuator_Status (slvAddr,funcCode,readAddrStart,numberOfAddressCheckPump);
			LastActuatslvAddr = slvAddr;
		}
		else
		{
			/*funzione che mi legge lo stato delle pinch*/
			Check_Actuator_Status (slvAddr,funcCode,readAddrStart,numberOfAddressCheckPinch);
			LastActuatslvAddr = slvAddr;
		}

	   /* se supero l'uiltimo attuatore, rifaccio il giro da capo*/
        /*incremento l'indirizzo per interrogare tutti gli attuatori*/

        	slvAddr++;

        if (slvAddr > LAST_ACTUATOR)
			slvAddr = FIRST_ACTUATOR;
    }
}

//char st[STR_DBG_LENGHT];
//int SelPump = 0;
/*In questa funzione memorizzo i dati ricevuti in seguito
 *  alla Check_Actuator_Status nella matrice modbusData*/
void StorageModbusData(unsigned char LastActuatslvAddr)
{
	/*in questa funzione suppongo di usare sempre il msgToRecvFrame3 in quanto usata solo per messaggi di stato e non di impostazione
	 * ecco perchè posso fare Address = msgToRecvFrame3[0] e funCode = msgToRecvFrame3[1]*/
#ifdef PUMP_EVER
	unsigned char dataTemp[TOT_DATA_MODBUS_RECEIVED_PUMP_EVER];
#else
	unsigned char dataTemp[TOT_DATA_MODBUS_RECEIVED_PUMP];
#endif
	unsigned char i,Address = msgToRecvFrame3[0],funCode = msgToRecvFrame3[1];
	unsigned int  Pump_Average_Current	= 0,
				  Pump_Speed_Status		= 0,
				  Pump_Status			= 0,
				  Pinch_Average_Current = 0,
				  Pinch_Status			= 0;

	int Tot_ModBus_Data_RX = 0;
	word CRC_RX,CRC_CALC;
	unsigned char *ptr_msg;

	/* controllo che il function code si esatto, se non lo
	 * è non processo il messaggio e spengo la seriale
	 * prima di ogni spedizione abilito la seriale tanto
	 * esiste la flag 'EnUser' che abilita la seriale se non lo è
	 * */
	if (funCode != 0x03)
	{
		MODBUS_COMM_Disable();
		MODBUS_COMM_ClearRxBuf();
		return;
	}

	ptr_msg =&msgToRecvFrame3[0];
#ifdef PUMP_EVER
	/*controllo i dati rievuti e calcolo il relativo CRC cofrontandolo con quello ricevuto*/
	if(Address == (LAST_PUMP - 1) || Address == LAST_PUMP)
	{
		// ho letto lo stato delle pompe EVER
		Tot_ModBus_Data_RX = TOT_DATA_MODBUS_RECEIVED_PUMP_EVER;
		CRC_CALC = ComputeChecksum(ptr_msg, TOT_DATA_MODBUS_RECEIVED_PUMP - 2);
		CRC_RX = BYTES_TO_WORD(msgToRecvFrame3[10], msgToRecvFrame3[9]);
	}
	else if (Address >= FIRST_ACTUATOR && Address <= (LAST_PUMP - 2))
#else
	/*controllo i dati rievuti e calcolo il relativo CRC cofrontandolo con quello ricevuto*/
	if (Address >= FIRST_ACTUATOR && Address <= LAST_PUMP)
#endif
	{
		Tot_ModBus_Data_RX = TOT_DATA_MODBUS_RECEIVED_PUMP;
		CRC_CALC = ComputeChecksum(ptr_msg, TOT_DATA_MODBUS_RECEIVED_PUMP -2);
		CRC_RX = BYTES_TO_WORD(msgToRecvFrame3[10], msgToRecvFrame3[9]);
	}
	else if (Address >= FIRST_PINCH && Address <= LAST_ACTUATOR)
	{
		Tot_ModBus_Data_RX = TOT_DATA_MODBUS_RECEIVED_PINCH;
		CRC_CALC = ComputeChecksum(ptr_msg, TOT_DATA_MODBUS_RECEIVED_PINCH -2);
		CRC_RX = BYTES_TO_WORD(msgToRecvFrame3[8], msgToRecvFrame3[7]);
	}
	else
	{
		/*ho ricevuto qualcosa con un indirizzo non valido
		 * non processo il messaggio e spengo la seriale
		 * prima di ogni spedizione abilito la seriale tanto
	 	 * esiste la flag 'EnUser' che abilita la seriale se non lo è*/
		MODBUS_COMM_Disable();
		MODBUS_COMM_ClearRxBuf();
		return;
	}

	/*se il CRC calcolato sulla base dei byte ricevuti e il CRC ricevuto non sono uguali
	 * non processo il messaggio e spengo la seriale
	 * prima di ogni spedizione abilito la seriale tanto
	 * esiste la flag 'EnUser' che abilita la seriale se non lo è*/
	if (CRC_RX != CRC_CALC)
	{
		MODBUS_COMM_Disable();
		MODBUS_COMM_ClearRxBuf();
		return;
	}

	/*ho superato tutti i controlli di sicurezza sul messaggio, ossia fun code,
	 *  Address e CRC  quindi posso copiare il messaggio*/
	/*copio nell'array temporaneo i dati ricevuti*/
	for (i=0; i<Tot_ModBus_Data_RX; i++)
	{
		/*sposto il puntatore nella posizione che devo leggere*/
		//_funcRetVal.slvresRetPtr = _funcRetVal.slvresRetPtr + i;

		/*copio il dato partrendo dall'ultimo byte fino al primo tornando indietro di Tot_ModBus_Data_RX
		 * che può essere 11 nel caso di una pompa oppure 9 nel caso di una pinch */
		//dataTemp[Tot_ModBus_Data_RX - 1 - i] = * (_funcRetVal.slvresRetPtr - i - 1); //così non sposto il puntatore, ma leggo direttamente quello che mi serve senza spostarlo

		dataTemp[i] = * (ptr_msg + i);
	}

	/*se ho l'indirizzo di una pompa*/
#ifdef PUMP_EVER
	if(Address == (LAST_PUMP - 1) || Address == LAST_PUMP)
	{
		/*devo trasfomare i dati ricevuti da byte in word*/
		Pump_Average_Current = BYTES_TO_WORD(dataTemp[3], dataTemp[4]);
		Pump_Speed_Status	 = BYTES_TO_WORD(dataTemp[7], dataTemp[8]);
		Pump_Speed_Status = (Pump_Speed_Status * 60)/1600 ;
		Pump_Status 		 = 0; //BYTES_TO_WORD(dataTemp[7], dataTemp[8]);
	}
	else if (Address >= FIRST_ACTUATOR && Address <= (LAST_PUMP - 2))
#else
	if (Address >= FIRST_ACTUATOR && Address <= LAST_PUMP)
#endif
	{
		/*devo trasfomare i dati ricevuti da byte in word*/
		Pump_Average_Current = BYTES_TO_WORD(dataTemp[3], dataTemp[4]);
		Pump_Speed_Status	 = BYTES_TO_WORD(dataTemp[5], dataTemp[6]);
		Pump_Status 		 = BYTES_TO_WORD(dataTemp[7], dataTemp[8]);
//		if(SelPump == Address)
//		{
//		sprintf(st, "*%x*", Pump_Status & 0x200);
//		DebugStringStr(st);
//		}
		if(Address == FIRST_ACTUATOR && !Pump_Speed_Status)
			i = 0;

	}
	/*se ho l'indirizzo di una pinch*/
	else if (Address >= FIRST_PINCH && Address <= LAST_ACTUATOR)
	{
		/*devo trasfomare i dati ricevuti da byte in word*/
		Pinch_Average_Current = BYTES_TO_WORD(dataTemp[3], dataTemp[4]);
		Pinch_Status	 	  = BYTES_TO_WORD(dataTemp[5], dataTemp[6]);
	}

	    /* uso l'Address come indice per la matrice
		 * ma lo decremento di due in quanto pompa con
		 * selettore '0' corrisposnde a indirizzo '2'*/
		/*se ho l'indirizzo di una pompa*/
#ifdef PUMP_EVER
	if(Address == (LAST_PUMP - 1) || Address == LAST_PUMP)
	{
		modbusData[Address-2][16]= Pump_Average_Current;
		modbusData[Address-2][17]= Pump_Speed_Status;
		modbusData[Address-2][18]= Pump_Status;
		/* azzero per quello slave il contatore di messaggi
		 * che non hanno avuto risposta in modo da contare
		 * le mancate risposte consecutive*/
		CountErrorModbusMSG[Address-2] = 0;
	}
	else if (Address >= FIRST_ACTUATOR && Address <= (LAST_PUMP - 2))
#else
		if (Address >= FIRST_ACTUATOR && Address <= LAST_PUMP)
#endif
		{
			modbusData[Address-2][16]= Pump_Average_Current;
			modbusData[Address-2][17]= Pump_Speed_Status;
			modbusData[Address-2][18]= Pump_Status;
			/*azzero per quello slave il contatore di messaggi
			 * che onn hano avuto risposta in modo da contare
			 * le mancate risposte consecutive*/
			CountErrorModbusMSG[Address-2] = 0;
		}
	/*se ho l'indirizzo di una pinch decremento l'address di 3
	 *- come indice dell'array  in quanto address 6 non è usato
	 -* e le pinch hano addres  7-8-9  */
		else if (Address >= FIRST_PINCH && Address <= LAST_ACTUATOR)
		{
			modbusData[Address-3][16]= Pinch_Average_Current;
			modbusData[Address-3][17]= Pinch_Status;
			/*azzero per quello slave il contatore di messaggi
			 * che onn hano avuto risposta in modo da contare
			 * le mancate risposte consecutive*/
			CountErrorModbusMSG[Address-2] = 0;
		}

}

void StorageModbusDataInit(void)
{
	unsigned char dataTemp[MAX_DATA_MODBUS_RX],i,Address,funCode;
	unsigned int  Target 				= 0,
			      Go_To_Home_Position 	= 0,
				  Acceleration 			= 0,
				  Current_level 		= 0,
				  Cruise_Speed 			= 0,
				  Steps_Target 			= 0,
				  Average_Current 		= 0,
				  Pump_Speed_Status 	= 0,
				  Status 				= 0,
				  BL_Version			= 0,
				  PCB_Code_H			= 0,
				  PCB_Code_L			= 0,
				  PCB_Code_REV  		= 0,
				  Serial_Number 		= 0,
				  FW_Version    		= 0;

	/*copio nell'array temporaneo i dati ricevuti*/
	for (i=0; i<MAX_DATA_MODBUS_RX; i++)
	{
		/*copio il dato*/
		dataTemp[i] = * (_funcRetVal.slvresRetPtr+i); //così sposto non sposto il puntatore, ma leggo direttamente quello che mi serve senza spostarlo
	}

	Address = dataTemp[0];
	funCode = dataTemp[1];


	/*devo trasfomare i dati ricevuti da byte in word*/
	Target				= BYTES_TO_WORD(dataTemp[3], dataTemp[4]);
	Average_Current		= BYTES_TO_WORD(dataTemp[35], dataTemp[36]);
	BL_Version 	 		= BYTES_TO_WORD(dataTemp[55], dataTemp[56]);
	PCB_Code_H			= BYTES_TO_WORD(dataTemp[57], dataTemp[58]);
	PCB_Code_L 	 		= BYTES_TO_WORD(dataTemp[59], dataTemp[60]);
	PCB_Code_REV 		= BYTES_TO_WORD(dataTemp[61], dataTemp[62]);
	Serial_Number		= BYTES_TO_WORD(dataTemp[63], dataTemp[64]);
	FW_Version 	 		= BYTES_TO_WORD(dataTemp[65], dataTemp[66]);

	/*se ho l'indirizzo di una pompa memorizzo anche...*/
	if (Address <= LAST_PUMP) //sono nel caso di una pompa
	{
		Go_To_Home_Position = BYTES_TO_WORD(dataTemp[5], dataTemp[6]);
		Acceleration		= BYTES_TO_WORD(dataTemp[7], dataTemp[8]);
		Current_level		= BYTES_TO_WORD(dataTemp[9], dataTemp[10]);
		Cruise_Speed		= BYTES_TO_WORD(dataTemp[11], dataTemp[12]);
		Steps_Target		= BYTES_TO_WORD(dataTemp[13], dataTemp[14]);
		Pump_Speed_Status	= BYTES_TO_WORD(dataTemp[37], dataTemp[38]);
		Status				= BYTES_TO_WORD(dataTemp[39], dataTemp[40]);

	}
	else //sono nel caso di uan pinch, lo Status è messo in un posto diverso
		Status = BYTES_TO_WORD(dataTemp[37], dataTemp[38]);

	/*memorizzo i dati solo se il function code contenuto
	 * nel secondo byte ricevuto è pari a 0x03 ossia
	 * quella è una risposta al mio comando di Read Revision*/
	if (funCode == 0x03)
	{
		/*uso lo Address come indice per la matrice
		 * ma lo decremento di due in quanto pompa con
		 * selettore '0' corrisponde a indirizzo '2'*/



		/*se ho l'indirizzo di una pompa memorizzo anche...*/
		if (Address <= LAST_PUMP) //sono nel caso di una pompa
		{
			modbusData[Address-2][0]  = Target;
			modbusData[Address-2][1]  = Go_To_Home_Position;
			modbusData[Address-2][2]  = Acceleration;
			modbusData[Address-2][3]  = Current_level;
			modbusData[Address-2][4]  = Cruise_Speed;
			modbusData[Address-2][5]  = Steps_Target;
			modbusData[Address-2][16] = Average_Current;
			modbusData[Address-2][17] = Pump_Speed_Status;
			modbusData[Address-2][18] = Status;
			modbusData[Address-2][26] = BL_Version;
			modbusData[Address-2][27] = PCB_Code_H;
			modbusData[Address-2][28] = PCB_Code_L;
			modbusData[Address-2][29] = PCB_Code_REV;
			modbusData[Address-2][30] = Serial_Number;
			modbusData[Address-2][31] = FW_Version;

		}
		else //sono nel caso di uan pinch, lo Status è messo in un posto diverso
		{

			modbusData[Address-3][0]  = Target;
			modbusData[Address-3][16] = Average_Current;
			modbusData[Address-3][17]= Status;
			modbusData[Address-3][26] = BL_Version;
			modbusData[Address-3][27] = PCB_Code_H;
			modbusData[Address-3][28] = PCB_Code_L;
			modbusData[Address-3][29] = PCB_Code_REV;
			modbusData[Address-3][30] = Serial_Number;
			modbusData[Address-3][31] = FW_Version;
		}
	}
}


/*funzione che ritorna il numero della pompa con il
 * cover perto oppure 4 se tutti i cover sono chiusi*/
//unsigned char CheckCoverPump()
//{
//	for (unsigned char i= 0; i<4; i++)
//	{
//		if (modbusData[i][18] & 0x0200)
//			return(i);
//	}
//
//	return(4);
//}

unsigned char CoverOpenCnt[4] = {0, 0, 0, 0};
unsigned char CoverCloseCnt[4] = {0, 0, 0, 0};
unsigned char CoverState[4] = {0, 0, 0, 0};   // 0 chiuse 1 aperte
unsigned char CoverPresc = 0;

/* funzione che ritorna il numero della pompa con il
 * cover aperto oppure 4 se tutti i cover sono chiusi*/
unsigned char CheckCoverPump()
{
	CoverPresc++;
	if(CoverPresc >= 10)
	{
		CoverPresc = 0;
		for (unsigned char i= 0; i<4; i++)
		{
			if (modbusData[i][18] & 0x0200)
			{
				CoverOpenCnt[i]++;
				if(CoverOpenCnt[i] >= 2)
				{
					CoverOpenCnt[i] = 3;
					CoverState[i] = 1;
				}
				CoverCloseCnt[i] = 0;
			}
			else
			{
				CoverCloseCnt[i]++;
				if(CoverCloseCnt[i] > 6)
				{
					CoverCloseCnt[i] = 3;
					CoverState[i] = 0;
				}
				CoverOpenCnt[i] = 0;
			}
		}
	}

	for (unsigned char i= 0; i<4; i++)
	{
		if (CoverState[i] == 1)
			return(i);
	}
	return(4);
}

/*funzione del CheckSum usata anche per la EEPROM e per il protocollo con SBC*/
unsigned int ComputeChecksum(unsigned char * data, int size)
{
	unsigned int CRC16 = 0xFFFF;
	unsigned int CRCLsb = 0x0000;
	unsigned int byte = 0x0000;

	for(int i = 0; i < size; i++)
	{
		byte = *(data+i) & 0x00FF;
		CRC16 = CRC16 ^ byte;

		for(int j = 0; j < 8; j++)
		{
			CRCLsb = CRC16 & 0x0001;
			CRC16 = (CRC16 >> 1) & 0x7FFF;

			if(CRCLsb == 0x0001)
			{
				CRC16 = CRC16 ^ CRC_POLYNOMIAL;
			}
		}
	}

	return CRC16;
}
/* Public function */

/* Private function */


static void FrameInitFunc3(void)
{
	for(int i = 0; i < MODBUS_FUNC3_FRAME_LENGHT; i++)
	{
		msgToSendFrame3[i] = 0xFF;
	}
}

static void FrameInitFunc10(void)
{
	for(int i = 0; i < MODBUS_FUNC10_FRAME_LENGHT; i++)
		{
			msgToSendFrame10[i] = 0xFF;
		}
}

static void FrameInitFunc17(void)
{
	for(int i = 0; i < MODBUS_FUNC17_FRAME_LENGHT; i++)
		{
			msgToSendFrame17[i] = 0xFF;
		}
}
/* Private function */

void RTS_Motor_Management(unsigned char action)
{
	switch (action)
	{
	case ENABLE:
		RTS_MOTOR_SetVal();
		break;

	case DISABLE:
		RTS_MOTOR_ClrVal();
		break;

	default:
		break;
	}

}

void EN_Clamp_Control (unsigned char action)
{
	switch (action)
	{
	case ENABLE:
		EN_CLAMP_CONTROL_SetVal();
		break;

	case DISABLE:
		EN_CLAMP_CONTROL_ClrVal();
		break;

	default:
		break;
	}
}

void EN_Motor_Control(unsigned char action)
{
	switch (action)
	{
	case ENABLE:
		EN_MOTOR_CONTROL_SetVal();
		break;

	case DISABLE:
		EN_MOTOR_CONTROL_ClrVal();
		break;

	default:
		break;
	}
}

void EN_24_M_C_Management(unsigned char action)
{
	switch (action)
	{
	case ENABLE:
		EN_24_M_C_SetVal();
		break;

	case DISABLE:
		EN_24_M_C_ClrVal();
		break;

	default:
		break;
	}
}

