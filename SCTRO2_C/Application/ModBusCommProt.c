/*
 * ModBusCommProt.c
 *
 *  Created on: 03/nov/2016
 *      Author: W15
 */

#include "ModBusCommProt.h"
#include "Global.h"

#include "AS1_MODBUS_ACTUATOR.h"
#include "ASerialLdd1.h"


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

void modBusPmpInit(void)
{
	pumpPerist[0].id = 0;
	pumpPerist[0].pmpSpeed = 0x0000;
	pumpPerist[0].pmpGoHomeSpeed = 0x0000;
	pumpPerist[0].pmpAccelSpeed = 0x0000;
	pumpPerist[0].pmpCurrent = 0x0000;
	pumpPerist[0].pmpCruiseSpeed = 0x0000;
	pumpPerist[0].pmpStepTarget = 0x0000;
	pumpPerist[0].pmpMySlaveAddress = 0x02;		//rotary selctor = 0 - pump art
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

	pumpPerist[1].id = 1;
	pumpPerist[1].pmpSpeed = 0x0000;
	pumpPerist[1].pmpGoHomeSpeed = 0x0000;
	pumpPerist[1].pmpAccelSpeed = 0x0000;
	pumpPerist[1].pmpCurrent = 0x0000;
	pumpPerist[1].pmpCruiseSpeed = 0x0000;
	pumpPerist[1].pmpStepTarget = 0x0000;
	pumpPerist[1].pmpMySlaveAddress = 0x03;		//rotary selctor = 1 - pump oxyg
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

	pumpPerist[2].id = 2;
	pumpPerist[2].pmpSpeed = 0x0000;
	pumpPerist[2].pmpGoHomeSpeed = 0x0000;
	pumpPerist[2].pmpAccelSpeed = 0x0000;
	pumpPerist[2].pmpCurrent = 0x0000;
	pumpPerist[2].pmpCruiseSpeed = 0x0000;
	pumpPerist[2].pmpStepTarget = 0x0000;
	pumpPerist[2].pmpMySlaveAddress = 0x04;		//rotary selctor = 2
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
}

void modBusPinchInit(void)
{
	pinchActuator[0].id = 0;
	pinchActuator[0].pinchPosTarget = 0x0000;
	pinchActuator[0].pinchMySlaveAddress = 0x07; 	//rotary selctor = 5 - pinch art
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

	pinchActuator[1].id = 1;
	pinchActuator[1].pinchPosTarget = 0x0000;
	pinchActuator[1].pinchMySlaveAddress = 0x08;		//rotary selctor = 6 - pinch ven
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

	pinchActuator[2].id = 2;
	pinchActuator[2].pinchPosTarget = 0x0000;
	pinchActuator[2].pinchMySlaveAddress = 0x09; 		//rotary selctor = 7 - pinch filter
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

void setPumpSpeedValueHighLevel(unsigned char slaveAddr, int speedValue){
	switch((slaveAddr - 2))
	{
	case 0:
		pumpPerist[0].reqState = REQ_STATE_ON;
		pumpPerist[0].reqType = REQ_TYPE_WRITE;
		pumpPerist[0].actuatorType = ACTUATOR_PUMP_TYPE;
		pumpPerist[0].value = speedValue;
		break;

	case 1:
		pumpPerist[1].reqState = REQ_STATE_ON;
		pumpPerist[1].reqType = REQ_TYPE_WRITE;
		pumpPerist[1].actuatorType = ACTUATOR_PUMP_TYPE;
		pumpPerist[1].value = speedValue;
		break;

	case 2:
		pumpPerist[2].reqState = REQ_STATE_ON;
		pumpPerist[2].reqType = REQ_TYPE_WRITE;
		pumpPerist[2].actuatorType = ACTUATOR_PUMP_TYPE;
		pumpPerist[2].value = speedValue;
		break;

	default:
		break;
	}
}

void setPumpSpeedValue(unsigned char slaveAddr, int speedValue){
	unsigned int mySpeedValue = 0;
	unsigned int * valModBusArrayPtr;
	char	slvAddr;
	char	funcCode;
	unsigned int	wrAddr;

	slvAddr = slaveAddr;
	funcCode = 0x10;
	wrAddr = 0x0000; /* speed */

	if((slvAddr >= 2) && (slvAddr <= 4)) /* pump */
		mySpeedValue = (unsigned int)speedValue;

	valModBusArrayPtr = &mySpeedValue;

	_funcRetValPtr = ModBusWriteRegisterReq(slvAddr,
											funcCode,
											wrAddr,
											0x0001,
											valModBusArrayPtr);
	//send command to actuator
	_funcRetVal.ptr_msg = _funcRetValPtr->ptr_msg;
	_funcRetVal.mstreqRetStructNumByte = _funcRetValPtr->mstreqRetStructNumByte;
	_funcRetVal.slvresRetPtr = _funcRetValPtr->slvresRetPtr;
	_funcRetVal.slvresRetNumByte = _funcRetValPtr->slvresRetNumByte;

	for(char k = 0; k < _funcRetVal.mstreqRetStructNumByte; k++)
	{
		AS1_MODBUS_ACTUATOR_SendChar(*(_funcRetVal.ptr_msg+k));
	}
}

void readPumpSpeedValue(unsigned char slaveAddr){
	char	slvAddr;
	char	funcCode;

	slvAddr = slaveAddr;
	funcCode = 0x03;
	unsigned int readAddr = 0x0011;
	unsigned int numRegRead = 0x0001;

	_funcRetValPtr = ModBusReadRegisterReq(slvAddr,
										   funcCode,
											readAddr,
											numRegRead);

	_funcRetVal.ptr_msg = _funcRetValPtr->ptr_msg;
	_funcRetVal.mstreqRetStructNumByte = _funcRetValPtr->mstreqRetStructNumByte;
	_funcRetVal.slvresRetPtr = _funcRetValPtr->slvresRetPtr;
	_funcRetVal.slvresRetNumByte = _funcRetValPtr->slvresRetNumByte;

	for(char k = 0; k < _funcRetVal.mstreqRetStructNumByte; k++)
	{
		AS1_MODBUS_ACTUATOR_SendChar(*(_funcRetVal.ptr_msg+k));
	}
}

void readPumpSpeedValueHighLevel(unsigned char slaveAddr){
	switch((slaveAddr - 2))
		{
		case 0:
			if((pumpPerist[0].reqState == REQ_STATE_OFF) && (pumpPerist[0].reqType != REQ_TYPE_WRITE))
			{
				pumpPerist[0].reqState = REQ_STATE_ON;
				pumpPerist[0].reqType = REQ_TYPE_READ;
				pumpPerist[0].actuatorType = ACTUATOR_PUMP_TYPE;
				pumpPerist[0].dataReady = DATA_READY_FALSE;
			}
			break;

		case 1:
			if((pumpPerist[1].reqState == REQ_STATE_OFF) && (pumpPerist[1].reqType == REQ_TYPE_IDLE))
			{
				pumpPerist[1].reqState = REQ_STATE_ON;
				pumpPerist[1].reqType = REQ_TYPE_READ;
				pumpPerist[1].actuatorType = ACTUATOR_PUMP_TYPE;
				pumpPerist[1].dataReady = DATA_READY_FALSE;
			}
			break;

		case 2:
			if((pumpPerist[2].reqState == REQ_STATE_OFF) && (pumpPerist[2].reqType == REQ_TYPE_IDLE))
			{
				pumpPerist[2].reqState = REQ_STATE_ON;
				pumpPerist[2].reqType = REQ_TYPE_READ;
				pumpPerist[2].actuatorType = ACTUATOR_PUMP_TYPE;
				pumpPerist[2].dataReady = DATA_READY_FALSE;
			}
			break;

		default:
			break;
		}
}

void setPinchPositionHighLevel(unsigned char slaveAddr, int posValue){
	switch(slaveAddr)
		{
		case 0x07:
			pinchActuator[0].reqState = REQ_STATE_ON;
			pinchActuator[0].reqType = REQ_TYPE_WRITE;
			pinchActuator[0].actuatorType = ACTUATOR_PINCH_TYPE;
			pinchActuator[0].value = posValue;
			break;

		case 0x08:
			pinchActuator[1].reqState = REQ_STATE_ON;
			pinchActuator[1].reqType = REQ_TYPE_WRITE;
			pinchActuator[1].actuatorType = ACTUATOR_PINCH_TYPE;
			pinchActuator[1].value = posValue;
			break;

		case 0x09:
			pinchActuator[2].reqState = REQ_STATE_ON;
			pinchActuator[2].reqType = REQ_TYPE_WRITE;
			pinchActuator[2].actuatorType = ACTUATOR_PINCH_TYPE;
			pinchActuator[2].value = posValue;
			break;

		default:
			break;
		}
}

void setPinchPosValue(unsigned char slaveAddr, int posValue){
	unsigned int myPinchPosValue = 0;
	unsigned int * valModBusArrayPtr;
	char	slvAddr;
	char	funcCode;
	unsigned int	wrAddr;

	slvAddr = slaveAddr;
	funcCode = 0x10;
	wrAddr = 0x0000; /* pinch position */

		if((slvAddr >= 7) && (slvAddr <= 9)) /* pinch */
			myPinchPosValue = (unsigned int)posValue;

		valModBusArrayPtr = &myPinchPosValue;

		_funcRetValPtr = ModBusWriteRegisterReq(slvAddr,
												funcCode,
												wrAddr,
												0x0001,
												valModBusArrayPtr);
		//send command to actuator
		_funcRetVal.ptr_msg = _funcRetValPtr->ptr_msg;
		_funcRetVal.mstreqRetStructNumByte = _funcRetValPtr->mstreqRetStructNumByte;
		_funcRetVal.slvresRetPtr = _funcRetValPtr->slvresRetPtr;
		_funcRetVal.slvresRetNumByte = _funcRetValPtr->slvresRetNumByte;

		for(char k = 0; k < _funcRetVal.mstreqRetStructNumByte; k++)
		{
			AS1_MODBUS_ACTUATOR_SendChar(*(_funcRetVal.ptr_msg+k));
		}
}

void alwaysModBusActuator(void){
	switch((timerCounterModBus%9))
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
			pumpPerist[0].reqType = REQ_TYPE_IDLE; //la scrittura ha la precedenza sulla lettura; leggo solo se il reqType � idle......questo reqType va messo in idle dopo che � stata letta tutta la risposta
			iflag_pmp1_rx = IFLAG_PMP1_BUSY;
			//timerCounterModBus = 0; //da verificare.....serve azzerare il contatore prima che arrivi la risposta?
			setPumpSpeedValue(pumpPerist[0].pmpMySlaveAddress, pumpPerist[0].value);
		}
		else if(
				(pumpPerist[0].reqState == REQ_STATE_ON) &&
				(pumpPerist[0].reqType == REQ_TYPE_READ) &&
				(iflag_pmp1_rx != IFLAG_PMP1_BUSY)
				) //read
		{
			pumpPerist[0].reqState = REQ_STATE_OFF;
			pumpPerist[0].dataReady = DATA_READY_FALSE;
			pumpPerist[0].reqType = REQ_TYPE_IDLE;
			iflag_pmp1_rx = IFLAG_PMP1_BUSY;
			//timerCounterModBus = 0; //da verificare.....serve azzerare il contatore prima che arrivi la risposta?
			readPumpSpeedValue(pumpPerist[0].pmpMySlaveAddress);
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
			pumpPerist[1].reqType = REQ_TYPE_IDLE;
			iflag_pmp1_rx = IFLAG_PMP1_BUSY;
			//pumpPerist[1].reqType = REQ_TYPE_IDLE;
			setPumpSpeedValue(pumpPerist[1].pmpMySlaveAddress, pumpPerist[1].value);
		}
		else if(
				(pumpPerist[1].reqState == REQ_STATE_ON) &&
				(pumpPerist[1].reqType == REQ_TYPE_READ) &&
				(iflag_pmp1_rx != IFLAG_PMP1_BUSY)
				) //read
		{
			pumpPerist[1].reqState = REQ_STATE_OFF;
			pumpPerist[1].dataReady = DATA_READY_FALSE;
			pumpPerist[1].reqType = REQ_TYPE_IDLE;
			iflag_pmp1_rx = IFLAG_PMP1_BUSY;
			//pumpPerist[1].reqType ==REQ_TYPE_IDLE;
			readPumpSpeedValue(pumpPerist[1].pmpMySlaveAddress);
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
			pumpPerist[2].reqType = REQ_TYPE_IDLE;
			iflag_pmp1_rx = IFLAG_PMP1_BUSY;
			//pumpPerist[2].reqType = REQ_TYPE_IDLE;
			setPumpSpeedValue(pumpPerist[2].pmpMySlaveAddress, pumpPerist[2].value);
		}
		else if(
				(pumpPerist[2].reqState == REQ_STATE_ON) &&
				(pumpPerist[2].reqType == REQ_TYPE_READ) &&
				(iflag_pmp1_rx != IFLAG_PMP1_BUSY)
				) //read
		{
			pumpPerist[2].reqState = REQ_STATE_OFF;
			pumpPerist[2].dataReady = DATA_READY_FALSE;
			pumpPerist[2].reqType = REQ_TYPE_IDLE;
			iflag_pmp1_rx = IFLAG_PMP1_BUSY;
			//pumpPerist[2].reqType = REQ_TYPE_IDLE;
			readPumpSpeedValue(pumpPerist[2].pmpMySlaveAddress);
		}
		break;

	//pump 3
	case 3:
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
			pinchActuator[0].reqType = REQ_TYPE_IDLE; //la scrittura ha la precedenza sulla lettura; leggo solo se il reqType � idle......questo reqType va messo in idle dopo che � stata letta tutta la risposta
			iflag_pmp1_rx = IFLAG_PMP1_BUSY;
			//timerCounterModBus = 0; //da verificare.....serve azzerare il contatore prima che arrivi la risposta?
			setPinchPosValue(pinchActuator[0].pinchMySlaveAddress, pinchActuator[0].value);
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
			pinchActuator[1].reqType = REQ_TYPE_IDLE; //la scrittura ha la precedenza sulla lettura; leggo solo se il reqType � idle......questo reqType va messo in idle dopo che � stata letta tutta la risposta
			iflag_pmp1_rx = IFLAG_PMP1_BUSY;
			//timerCounterModBus = 0; //da verificare.....serve azzerare il contatore prima che arrivi la risposta?
			setPinchPosValue(pinchActuator[1].pinchMySlaveAddress, pinchActuator[1].value);
		}
		break;

	//pinch 3 - adsorbent filter
	case 6:
		if(
			(pinchActuator[2].reqState == REQ_STATE_ON) &&
			(pinchActuator[2].reqType == REQ_TYPE_WRITE) &&
			(iflag_pmp1_rx != IFLAG_PMP1_BUSY)
			) //write
		{
			pinchActuator[2].reqState = REQ_STATE_OFF;
			pinchActuator[2].reqType = REQ_TYPE_IDLE; //la scrittura ha la precedenza sulla lettura; leggo solo se il reqType � idle......questo reqType va messo in idle dopo che � stata letta tutta la risposta
			iflag_pmp1_rx = IFLAG_PMP1_BUSY;
			//timerCounterModBus = 0; //da verificare.....serve azzerare il contatore prima che arrivi la risposta?
			setPinchPosValue(pinchActuator[2].pinchMySlaveAddress, pinchActuator[2].value);
		}
		break;

	//pinch 4
	case 7:
		break;

	default:
		break;
	}

	if(iflag_pmp1_rx == IFLAG_PMP1_RX){
		for(int i=0; i<=2; i++)
		{
			//if(pumpPerist[i].reqType != REQ_TYPE_IDLE)
			//{
				//pumpPerist[i].reqState = REQ_STATE_OFF;
				//pumpPerist[i].reqType = REQ_TYPE_IDLE;

				//queste due righe andranno condizionate in base all'indirizzo dello slave che risponde
 				pumpPerist[i].dataReady = DATA_READY_TRUE;
 				pinchActuator[i].dataReady = DATA_READY_TRUE;
			//}
		}
		iflag_pmp1_rx = IFLAG_IDLE;
		timerCounterModBus = 0; //da verificare......qui il canale � sicuramente libero
	}
}
/* Public function */

/* Private function */
static unsigned int ComputeChecksum(unsigned char * data, int size)
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
