/*
 * ModBusCommProt.c
 *
 *  Created on: 20/giu/2016
 *      Author: W15
 */

#include "ModBusCommProt.h"
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

	for(int num_byte = 0; num_byte < (mstreq_data_wr_byte_count/2); num_byte++)
		{
			/*pos_10plus_numbyte: write value - High */
			msgToSendFrame10[10+(1+2*num_byte)] = (_writeRegValueArray[num_byte] & 0xFF00)>>8;
			/*pos_11plus_numbyte: write value - Low */
			msgToSendFrame10[10+(2+2*num_byte)] = (_writeRegValueArray[num_byte] & 0x00FF);
		}

	msgToSendFrame17Ptr = &msgToSendFrame17[0];

	mstreq_crc = ComputeChecksum(msgToSendFrame17Ptr, (10+mstreq_data_wr_byte_count+1));
	/*pos_: crc Low*/
	mstreq_crc_Low = (mstreq_crc & 0x00FF);
	msgToSendFrame17[10+mstreq_data_wr_byte_count+1] = mstreq_crc_Low;
	/*pos_: crc High*/
	mstreq_crc_High = (mstreq_crc & 0xFF00)>>8;
	msgToSendFrame17[10+mstreq_data_wr_byte_count+2] = mstreq_crc_High;

	msgToSendFrame17Ptr = &msgToSendFrame17[0];
	_func17RetVal.mstreqRetStructPtr = msgToSendFrame17Ptr;
	_func17RetVal.mstreqRetStructNumByte = 10+mstreq_data_wr_byte_count+2+1;
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
/* Public function */



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




