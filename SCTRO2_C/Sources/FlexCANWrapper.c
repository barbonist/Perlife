//
//
//	filename:   FlexCANWrapper.c
//	date:		26/3/2018
//	author:		SB
//
#include "CAN_C_P.h"
#include "string.h"

LDD_TUserData	*pUserData;
//LDD_CAN_TFrame* FrameInP;
//uint8_t InBuffer[8];
uint8_t TxData[10];
LDD_CAN_TFrame TxFrame;

uint8_t RxData[10];
LDD_CAN_TFrame RxFrame;



void InitCAN(void)
{
	TxFrame.FrameType = LDD_CAN_DATA_FRAME;
	TxFrame.Data = TxData;

	RxFrame.FrameType = LDD_CAN_DATA_FRAME;
	RxFrame.Data = RxData;

	pUserData = CAN_C_P_Init(NULL);
}


/*
** ===================================================================
**     Method      :  void SendCAN
*/
/*!
**     @brief
**         Wrapper 2 send a frame via the CAN device.
**     @param
**         txbuff   -  data buffer , maximum 8 bytes allowed
**     @param
**         txsize   -  buffer size ( 1..8. )
**     @param
**         ChIndex	- 0 to 7
**     @return
**                         - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_DISABLED - This component is disabled
**                           by user
**                           ERR_SPEED - This device does not work in
**                           the active clock configuration
**                           ERR_PARAM_RANGE - Value of buffer index is
**                           out of range.
**                           ERR_PARAM_INDEX - Index of message buffer
**                           is not for transmit.
**                           ERR_PARAM_LENGTH - Number of data in the
**                           frame is greater than MaxDataLength.
**                           ERR_PARAM_ATTRIBUTE_SET - Frame type isn't
**                           supported.
**                           ERR_PARAM_VALUE - Value of Tx priority is
**                           fail.
**                           ERR_BUSY - CAN module is busy.
*/
/* ===================================================================*/
LDD_TError  SendCAN(uint8_t *txbuff, int txsize, LDD_CAN_TMBIndex ChIndex)
{

	if((txsize > 8) || ( ChIndex > 7)){
		return ERR_PARAM_VALUE;
	}
	TxFrame.MessageID = 0x0701U + ChIndex;
	TxFrame.FrameType = LDD_CAN_DATA_FRAME;
	TxFrame.Data = txbuff;
	TxFrame.Length = txsize;
	LDD_TError error = CAN_C_P_SendFrame(pUserData, 8U + ChIndex, &TxFrame);
	return error;
}


uint8_t RxBuff004[10];
void ReceivedCanData(uint8_t *rxbuff, int rxlen, int RxChannel);
void TestReceiveCan( LDD_CAN_TMBIndex BufferIdx ){

	CAN_C_P_ReadFrame(pUserData, BufferIdx, &RxFrame);
	uint8_t LenData = RxFrame.Length;
	memcpy(RxBuff004,RxFrame.Data,LenData);
	ReceivedCanData(RxBuff004, LenData, BufferIdx);
}



