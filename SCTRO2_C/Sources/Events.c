/* ###################################################################
**     Filename    : Events.c
**     Project     : SCTRO2_C
**     Processor   : MK64FN1M0VLQ12
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2016-07-19, 16:01, # CodeGen: 0
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Contents    :
**         Cpu_OnNMI - void Cpu_OnNMI(void);
**
** ###################################################################*/
/*!
** @file Events.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup Events_module Events module documentation
**  @{
*/         
/* MODULE Events */

#include "Cpu.h"
#include "Events.h"
#include "Init_Config.h"
#include "PDD_Includes.h"
#include "Global.h"

#ifdef __cplusplus
extern "C" {
#endif 


/* User includes (#include below this line is not maintained by Processor Expert) */

/*
** ===================================================================
**     Event       :  Cpu_OnNMI (module Events)
**
**     Component   :  Cpu [MK64FN1M0LQ12]
*/
/*!
**     @brief
**         This event is called when the Non maskable interrupt had
**         occurred. This event is automatically enabled when the [NMI
**         interrupt] property is set to 'Enabled'.
*/
/* ===================================================================*/
void Cpu_OnNMI(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  SBC_COMM_OnError (module Events)
**
**     Component   :  SBC_COMM [AsynchroSerial]
**     Description :
**         This event is called when a channel error (not the error
**         returned by a given method) occurs. The errors can be read
**         using <GetError> method.
**         The event is available only when the <Interrupt
**         service/event> property is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void SBC_COMM_OnError(void)
{
  /* Write your code here ... */

}

/*
** ===================================================================
**     Event       :  SBC_COMM_OnRxChar (module Events)
**
**     Component   :  SBC_COMM [AsynchroSerial]
**     Description :
**         This event is called after a correct character is received.
**         The event is available only when the <Interrupt
**         service/event> property is enabled and either the <Receiver>
**         property is enabled or the <SCI output mode> property (if
**         supported) is set to Single-wire mode.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void SBC_COMM_OnRxChar(void)
{
  /* Write your code here ... */
	// SBC_COMM_RecvChar(&msg_sbc_rx[0]);
	SBC_COMM_RecvChar(ptrMsgSbcRx);

	#ifdef	DEBUG_COMM_SBC
	//PC_DEBUG_COMM_SendChar(*ptrMsgSbcRx);
	#endif

	ptrSbcCountRx = ptrSbcCountRx + 1;
	ptrMsgSbcRx = ptrMsgSbcRx + 1;

	//if((ptrSbcCountRx > 20))
	if(
		((*(ptrMsgSbcRx-1)) == 0x5A) &&
		((*(ptrMsgSbcRx-2)) == 0x00) &&
		((*(ptrMsgSbcRx-3)) == 0x00)
		)
	{
		iflag_sbc_rx |= IFLAG_SBC_RX;

		if (sbc_rx_data[5] == 0xCC)
			Service = TRUE;
		else
			Service = FALSE;

		ptrSbcCountRx = 0;
		//ptrMsgSbcRx = &msg_sbc_rx[0];
		ptrMsgSbcRx = &sbc_rx_data[0];
	}
}

/*
** ===================================================================
**     Event       :  SBC_COMM_OnTxChar (module Events)
**
**     Component   :  SBC_COMM [AsynchroSerial]
**     Description :
**         This event is called after a character is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void SBC_COMM_OnTxChar(void)
{
  /* Write your code here ... */

	#ifdef	DEBUG_COMM_SBC
	//PC_DEBUG_COMM_SendChar(*ptrMsgSbcTx);
	//ptrMsgSbcTx = ptrMsgSbcTx + 1;
	#endif
}

/*
** ===================================================================
**     Event       :  SBC_COMM_OnFullRxBuf (module Events)
**
**     Component   :  SBC_COMM [AsynchroSerial]
**     Description :
**         This event is called when the input buffer is full;
**         i.e. after reception of the last character 
**         that was successfully placed into input buffer.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void SBC_COMM_OnFullRxBuf(void)
{
  /* Write your code here ... */

}

/*
** ===================================================================
**     Event       :  SBC_COMM_OnFreeTxBuf (module Events)
**
**     Component   :  SBC_COMM [AsynchroSerial]
**     Description :
**         This event is called after the last character in output
**         buffer is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void SBC_COMM_OnFreeTxBuf(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  FLOWSENS_COMM_OnError (module Events)
**
**     Component   :  FLOWSENS_COMM [AsynchroSerial]
**     Description :
**         This event is called when a channel error (not the error
**         returned by a given method) occurs. The errors can be read
**         using <GetError> method.
**         The event is available only when the <Interrupt
**         service/event> property is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void FLOWSENS_COMM_OnError(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  FLOWSENS_COMM_OnRxChar (module Events)
**
**     Component   :  FLOWSENS_COMM [AsynchroSerial]
**     Description :
**         This event is called after a correct character is received.
**         The event is available only when the <Interrupt
**         service/event> property is enabled and either the <Receiver>
**         property is enabled or the <SCI output mode> property (if
**         supported) is set to Single-wire mode.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void FLOWSENS_COMM_OnRxChar(void)
{
  /* Write your code here ... */

	FLOWSENS_COMM_RecvChar(ptrMsg_UFLOW->ptrBufferReceived);
	ptrMsg_UFLOW->ptrBufferReceived = ptrMsg_UFLOW->ptrBufferReceived + 1;

	ptrMsg_UFLOW->bufferReceivedLenght = ptrMsg_UFLOW->bufferReceivedLenght - 1;
	if(ptrMsg_UFLOW->bufferReceivedLenght == 0)
	{
		iflag_uflow_sens = IFLAG_UFLOW_SENS_RX;
		ptrMsg_UFLOW->ptrBufferReceived = &ptrMsg_UFLOW->bufferReceived[0];

		/*Disable Receiver Enable*/
		FLOWSENS_RE_SetVal();

	}
}

/** ===================================================================
**     Event       :  FLOWSENS_COMM_OnTxComplete (module Events)
**
**     Component   :  FLOWSENS_COMM [AsynchroSerial]
**     Description :
**         This event indicates that the transmitter is finished
**         transmitting all data, preamble, and break characters and is
**         idle. It can be used to determine when it is safe to switch
**         a line driver (e.g. in RS-485 applications).
**         The event is available only when both <Interrupt
**         service/event> and <Transmitter> properties are enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void FLOWSENS_COMM_OnTxComplete(void)
{
  /* Write your code here ... */


	iflag_uflow_sens = IFLAG_UFLOW_SENS_TX;
	FLOWSENS_DE_ClrVal();
	/*Il Receive Enable è attivo basso*/
	FLOWSENS_RE_ClrVal();


//	if(ptrMsg_UFLOW->byteSended >= ptrMsg_UFLOW->bufferToSendLenght)
//	{
////		FLOWSENS_DE_ClrVal();
////		FLOWSENS_RE_ClrVal();
//		//TODO		FLOWSENS_RTS_ClrVal();
//	}

}

/*
** ===================================================================
**     Event       :  FLOWSENS_COMM_OnTxChar (module Events)
**
**     Component   :  FLOWSENS_COMM [AsynchroSerial]
**     Description :
**         This event is called after a character is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void FLOWSENS_COMM_OnTxChar(void)
{
  /* Write your code here ... */
//	ptrMsg_UFLOW->byteSended = ptrMsg_UFLOW->byteSended + 1;
//	if(ptrMsg_UFLOW->byteSended >= ptrMsg_UFLOW->bufferToSendLenght)
//	{
//		iflag_uflow_sens = IFLAG_UFLOW_SENS_TX;
//		FLOWSENS_DE_ClrVal();
//		/*Il Receive Enable è attivo basso*/
//		FLOWSENS_RE_ClrVal();
//	}
}

/*
** ===================================================================
**     Event       :  FLOWSENS_COMM_OnFullRxBuf (module Events)
**
**     Component   :  FLOWSENS_COMM [AsynchroSerial]
**     Description :
**         This event is called when the input buffer is full;
**         i.e. after reception of the last character 
**         that was successfully placed into input buffer.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void FLOWSENS_COMM_OnFullRxBuf(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  FLOWSENS_COMM_OnFreeTxBuf (module Events)
**
**     Component   :  FLOWSENS_COMM [AsynchroSerial]
**     Description :
**         This event is called after the last character in output
**         buffer is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void FLOWSENS_COMM_OnFreeTxBuf(void)
{
  /* Write your code here ... */
	//if(ptrMsg_UFLOW->byteSended >= ptrMsg_UFLOW->bufferToSendLenght)
		//FLOWSENS_RTS_ClrVal();
}

/*
** ===================================================================
**     Event       :  PELTIER_COMM_OnError (module Events)
**
**     Component   :  PELTIER_COMM [AsynchroSerial]
**     Description :
**         This event is called when a channel error (not the error
**         returned by a given method) occurs. The errors can be read
**         using <GetError> method.
**         The event is available only when the <Interrupt
**         service/event> property is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void PELTIER_COMM_OnError(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  PELTIER_COMM_OnRxChar (module Events)
**
**     Component   :  PELTIER_COMM [AsynchroSerial]
**     Description :
**         This event is called after a correct character is received.
**         The event is available only when the <Interrupt
**         service/event> property is enabled and either the <Receiver>
**         property is enabled or the <SCI output mode> property (if
**         supported) is set to Single-wire mode.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void PELTIER_COMM_OnRxChar(void)
{
  /* Write your code here ... */
	/*if(iflagPeltierMsg == IFLAG_PELTIER_MSG_START)
	{*/
	PELTIER_COMM_RecvChar(ptrMsgPeltierRx);

 	#ifdef	DEBUG_COMM_SBC
	//PC_DEBUG_COMM_SendChar(*ptrMsgPeltierRx);
	#endif
	/*}*/

	/* the first who reads the data empty the buffer */
	/*PELTIER_COMM_RecvChar(ptrMsgPeltierRxDummy);

	iflagPeltierMsg |= ((*ptrMsgPeltierRxDummy) == 0x0D) ? 0x01 : 0x00;
	iflagPeltierMsg |= ((*ptrMsgPeltierRxDummy) == 0x0A) ? 0x04 : 0x00;
	iflagPeltierMsg |= ((*ptrMsgPeltierRxDummy) == 0x3E) ? 0x10 : 0x00;
	iflagPeltierMsg |= ((*ptrMsgPeltierRxDummy) == 0x20) ? 0x40 : 0x00;*/

	ptrPeltierCountRx = ptrPeltierCountRx + 1;
	ptrMsgPeltierRx = ptrMsgPeltierRx + 1;

	if(
		((*(ptrMsgPeltierRx-1)) == 0x20) &&
		((*(ptrMsgPeltierRx-2)) == 0x3E) &&
		((*(ptrMsgPeltierRx-3)) == 0x0A) &&
		((*(ptrMsgPeltierRx-4)) == 0x0D)
		)
	//if(iflagPeltierMsg == IFLAG_PELTIER_MSG_END)
	{
  		iflag_peltier_rx = IFLAG_PELTIER_RX;
		iflagPeltierMsg = IFLAG_IDLE;
		ptrMsgPeltierRx = &peltierDebug_rx_data[0];
		ptrMsgDataieee754start = &peltierDebug_rx_data[ptrPeltierCountRx-12];
		ptrMsgDataPeltierInt = &peltierDebug_rx_data[ptrPeltierCountRx-5];
		ptrPeltierCountRx = 0;
	}

}

/*
** ===================================================================
**     Event       :  PELTIER_COMM_OnTxChar (module Events)
**
**     Component   :  PELTIER_COMM [AsynchroSerial]
**     Description :
**         This event is called after a character is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void PELTIER_COMM_OnTxChar(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  PELTIER_COMM_OnFullRxBuf (module Events)
**
**     Component   :  PELTIER_COMM [AsynchroSerial]
**     Description :
**         This event is called when the input buffer is full;
**         i.e. after reception of the last character 
**         that was successfully placed into input buffer.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void PELTIER_COMM_OnFullRxBuf(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  PELTIER_COMM_OnFreeTxBuf (module Events)
**
**     Component   :  PELTIER_COMM [AsynchroSerial]
**     Description :
**         This event is called after the last character in output
**         buffer is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void PELTIER_COMM_OnFreeTxBuf(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  PELTIER_COMM_OnTxComplete (module Events)
**
**     Component   :  PELTIER_COMM [AsynchroSerial]
**     Description :
**         This event indicates that the transmitter is finished
**         transmitting all data, preamble, and break characters and is
**         idle. It can be used to determine when it is safe to switch
**         a line driver (e.g. in RS-485 applications).
**         The event is available only when both <Interrupt
**         service/event> and <Transmitter> properties are enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void PELTIER_COMM_OnTxComplete(void)
{
  /* Write your code here ... */
}
/*
** ===================================================================
**     Event       :  PELTIER2_COMM_OnError (module Events)
**
**     Component   :  PELTIER2_COMM [AsynchroSerial]
**     Description :
**         This event is called when a channel error (not the error
**         returned by a given method) occurs. The errors can be read
**         using <GetError> method.
**         The event is available only when the <Interrupt
**         service/event> property is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void PELTIER2_COMM_OnError(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  PELTIER2_COMM_OnRxChar (module Events)
**
**     Component   :  PELTIER2_COMM [AsynchroSerial]
**     Description :
**         This event is called after a correct character is received.
**         The event is available only when the <Interrupt
**         service/event> property is enabled and either the <Receiver>
**         property is enabled or the <SCI output mode> property (if
**         supported) is set to Single-wire mode.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void PELTIER2_COMM_OnRxChar(void)
{
  /* Write your code here ... */
	/*if(iflagPeltierMsg == IFLAG_PELTIER_MSG_START)
	{*/
	PELTIER2_COMM_RecvChar(ptrMsgPeltier2Rx);

 	#ifdef	DEBUG_COMM_SBC
	//PC_DEBUG_COMM_SendChar(*ptrMsgPeltierRx);
	#endif
	/*}*/

	/* the first who reads the data empty the buffer */
	/*PELTIER_COMM_RecvChar(ptrMsgPeltierRxDummy);

	iflagPeltierMsg |= ((*ptrMsgPeltierRxDummy) == 0x0D) ? 0x01 : 0x00;
	iflagPeltierMsg |= ((*ptrMsgPeltierRxDummy) == 0x0A) ? 0x04 : 0x00;
	iflagPeltierMsg |= ((*ptrMsgPeltierRxDummy) == 0x3E) ? 0x10 : 0x00;
	iflagPeltierMsg |= ((*ptrMsgPeltierRxDummy) == 0x20) ? 0x40 : 0x00;*/

	ptrPeltier2CountRx = ptrPeltier2CountRx + 1;
	ptrMsgPeltier2Rx = ptrMsgPeltier2Rx + 1;

	if(
		((*(ptrMsgPeltier2Rx-1)) == 0x20) &&
		((*(ptrMsgPeltier2Rx-2)) == 0x3E) &&
		((*(ptrMsgPeltier2Rx-3)) == 0x0A) &&
		((*(ptrMsgPeltier2Rx-4)) == 0x0D)
		)
	//if(iflagPeltierMsg == IFLAG_PELTIER_MSG_END)
	{
  		iflag_peltier2_rx = IFLAG_PELTIER_RX;
		iflagPeltier2Msg = IFLAG_IDLE;
		ptrMsgPeltier2Rx = &peltier2Debug_rx_data[0];
		ptrMsgData2ieee754start = &peltier2Debug_rx_data[ptrPeltierCountRx-12];
		ptrMsgDataPeltier2Int = &peltier2Debug_rx_data[ptrPeltierCountRx-5];
		ptrPeltier2CountRx = 0;
	}
}

/*
** ===================================================================
**     Event       :  PELTIER2_COMM_OnTxChar (module Events)
**
**     Component   :  PELTIER2_COMM [AsynchroSerial]
**     Description :
**         This event is called after a character is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void PELTIER2_COMM_OnTxChar(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  PELTIER2_COMM_OnFullRxBuf (module Events)
**
**     Component   :  PELTIER2_COMM [AsynchroSerial]
**     Description :
**         This event is called when the input buffer is full;
**         i.e. after reception of the last character
**         that was successfully placed into input buffer.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void PELTIER2_COMM_OnFullRxBuf(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  PELTIER2_COMM_OnFreeTxBuf (module Events)
**
**     Component   :  PELTIER2_COMM [AsynchroSerial]
**     Description :
**         This event is called after the last character in output
**         buffer is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void PELTIER2_COMM_OnFreeTxBuf(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  PELTIER2_COMM_OnTxComplete (module Events)
**
**     Component   :  PELTIER2_COMM [AsynchroSerial]
**     Description :
**         This event indicates that the transmitter is finished
**         transmitting all data, preamble, and break characters and is
**         idle. It can be used to determine when it is safe to switch
**         a line driver (e.g. in RS-485 applications).
**         The event is available only when both <Interrupt
**         service/event> and <Transmitter> properties are enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void PELTIER2_COMM_OnTxComplete(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  AD1_OnEnd (module Events)
**
**     Component   :  AD1 [ADC]
**     Description :
**         This event is called after the measurement (which consists
**         of <1 or more conversions>) is/are finished.
**         The event is available only when the <Interrupt
**         service/event> property is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/


void AD1_OnEnd(void)
{
  /* Write your code here ... */
	END_ADC1=TRUE;

	AD1_Stop();
}

/*
** ===================================================================
**     Event       :  AD1_OnCalibrationEnd (module Events)
**
**     Component   :  AD1 [ADC]
**     Description :
**         This event is called when the calibration has been finished.
**         User should check if the calibration pass or fail by
**         Calibration status method./nThis event is enabled only if
**         the <Interrupt service/event> property is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void AD1_OnCalibrationEnd(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  PC_DEBUG_COMM_OnError (module Events)
**
**     Component   :  PC_DEBUG_COMM [AsynchroSerial]
**     Description :
**         This event is called when a channel error (not the error
**         returned by a given method) occurs. The errors can be read
**         using <GetError> method.
**         The event is available only when the <Interrupt
**         service/event> property is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void PC_DEBUG_COMM_OnError(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  PC_DEBUG_COMM_OnRxChar (module Events)
**
**     Component   :  PC_DEBUG_COMM [AsynchroSerial]
**     Description :
**         This event is called after a correct character is received.
**         The event is available only when the <Interrupt
**         service/event> property is enabled and either the <Receiver>
**         property is enabled or the <SCI output mode> property (if
**         supported) is set to Single-wire mode.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void PC_DEBUG_COMM_OnRxChar(void)
{
  /* Write your code here ... */

	//ptr = &pc_rx_data[0]; /* questa riga va messa dove si inizialiiza la comunicazione col pc */
	PC_DEBUG_COMM_RecvChar(ptrPCDebug);

	ptrPCDebug = ptrPCDebug + 1;
	ptrPCDebugCount = ptrPCDebugCount + 1;
	if(ptrPCDebugCount >= 16)
	{
		iflag_pc_rx |= IFLAG_PC_RX;
		ptrPCDebug = &msgPcDebug[0];
		ptrPCDebugCount = 0;
	}
		//ptr = ptr + 1;
}

/*
** ===================================================================
**     Event       :  PC_DEBUG_COMM_OnTxChar (module Events)
**
**     Component   :  PC_DEBUG_COMM [AsynchroSerial]
**     Description :
**         This event is called after a character is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void PC_DEBUG_COMM_OnTxChar(void)
{
  /* Write your code here ... */

}

/*
** ===================================================================
**     Event       :  PC_DEBUG_COMM_OnFullRxBuf (module Events)
**
**     Component   :  PC_DEBUG_COMM [AsynchroSerial]
**     Description :
**         This event is called when the input buffer is full;
**         i.e. after reception of the last character 
**         that was successfully placed into input buffer.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void PC_DEBUG_COMM_OnFullRxBuf(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  PC_DEBUG_COMM_OnFreeTxBuf (module Events)
**
**     Component   :  PC_DEBUG_COMM [AsynchroSerial]
**     Description :
**         This event is called after the last character in output
**         buffer is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void PC_DEBUG_COMM_OnFreeTxBuf(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  MODBUS_COMM_OnError (module Events)
**
**     Component   :  MODBUS_COMM [AsynchroSerial]
**     Description :
**         This event is called when a channel error (not the error
**         returned by a given method) occurs. The errors can be read
**         using <GetError> method.
**         The event is available only when the <Interrupt
**         service/event> property is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void MODBUS_COMM_OnError(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  MODBUS_COMM_OnRxChar (module Events)
**
**     Component   :  MODBUS_COMM [AsynchroSerial]
**     Description :
**         This event is called after a correct character is received.
**         The event is available only when the <Interrupt
**         service/event> property is enabled and either the <Receiver>
**         property is enabled or the <SCI output mode> property (if
**         supported) is set to Single-wire mode.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void MODBUS_COMM_OnRxChar(void)
{
  /* Write your code here ... */

	static char byte_received =0; //lo faccio static così non lo inizializza ad ogni ingresso nell' interrupt


	//MODBUS_COMM_RecvChar(msg_pmp1_rx_ptr);
	MODBUS_COMM_RecvChar(_funcRetVal.slvresRetPtr);

	//array [byte_received] = *_funcRetVal.slvresRetPtr; //used only for debug

	byte_received++; // incremento pert sapere quantri byte ho ricevuto


	_funcRetVal.slvresRetPtr = _funcRetVal.slvresRetPtr + 1;

	#ifdef DEBUG_PUMP
	//if(_funcRetVal.slvresRetNumByte > 0)
		//PC_DEBUG_COMM_SendChar(*_funcRetVal.slvresRetPtr);
	#endif

	//msg_pmp1_rx_ptr = msg_pmp1_rx_ptr + 1;

	_funcRetVal.slvresRetNumByte = _funcRetVal.slvresRetNumByte - 1;
	//check when all expected response bytes have been received
	if(_funcRetVal.slvresRetNumByte <= 0)
	{
		iflag_pmp1_rx = IFLAG_IDLE;
		iflag_pmp1_rx |= IFLAG_PMP1_RX;
		_funcRetVal.slvresRetPtr = _funcRetVal.slvresRetPtr - byte_received;	//riporto il puntatore dei valori all'inizio
		byte_received=0;														//resetto byte_received così ad una prossima ricezione
		iFlag_actuatorCheck = IFLAG_COMMAND_RECEIVED;
		iFlag_modbusDataStorage = FALSE;
	}
}

/*
** ===================================================================
**     Event       :  MODBUS_COMM_OnTxChar (module Events)
**
**     Component   :  MODBUS_COMM [AsynchroSerial]
**     Description :
**         This event is called after a character is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void MODBUS_COMM_OnTxChar(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  MODBUS_COMM_OnFullRxBuf (module Events)
**
**     Component   :  MODBUS_COMM [AsynchroSerial]
**     Description :
**         This event is called when the input buffer is full;
**         i.e. after reception of the last character 
**         that was successfully placed into input buffer.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void MODBUS_COMM_OnFullRxBuf(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  MODBUS_COMM_OnFreeTxBuf (module Events)
**
**     Component   :  MODBUS_COMM [AsynchroSerial]
**     Description :
**         This event is called after the last character in output
**         buffer is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void MODBUS_COMM_OnFreeTxBuf(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  SM1_OnBlockReceived (module Events)
**
**     Component   :  SM1 [SPIMaster_LDD]
*/
/*!
**     @brief
**         This event is called when the requested number of data is
**         moved to the input buffer. This method is available only if
**         the ReceiveBlock method is enabled.
**     @param
**         UserDataPtr     - Pointer to the user or
**                           RTOS specific data. The pointer is passed
**                           as the parameter of Init method. 
*/
/* ===================================================================*/
void SM1_OnBlockReceived(LDD_TUserData *UserDataPtr)
{
  /* Write your code here ... */
	iflag_spi_rx = IFLAG_SPI_RX_TRUE;
}

/*

/*
** ===================================================================
**     Event       :  IR_TM_COMM_OnReceiveData (module Events)
**
**     Component   :  IR_TM_COMM [InternalI2C]
**     Description :
**         This event is invoked when I2C finishes the reception of the
**         data successfully. This event is not available for the SLAVE
**         mode and if both RecvChar and RecvBlock are disabled. This
**         event is enabled only if interrupts/events are enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void IR_TM_COMM_OnReceiveData(void)
{
  /* Write your code here ... */

	iflag_sensTempIR_Meas_Ready = IFLAG_IRTEMP_MEASURE_READY;
	IR_TM_COMM_SendStop();
}

/*
** ===================================================================
**     Event       :  IR_TM_COMM_OnTransmitData (module Events)
**
**     Component   :  IR_TM_COMM [InternalI2C]
**     Description :
**         This event is invoked when I2C finishes the transmission of
**         the data successfully. This event is not available for the
**         SLAVE mode and if both SendChar and SendBlock are disabled.
**         This event is enabled only if interrupts/events are enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void IR_TM_COMM_OnTransmitData(void)
{
  /* Write your code here ... */
	unsigned char err;
	word ret;

	err = IR_TM_COMM_RecvBlock(ptrDataTemperatureIR, 3, &ret);
	if(iflag_sensTempIRRW == IFLAG_SENS_TEMPIR_WRITE)
	{
		IR_TM_COMM_SendStop();
	}
}

/*
** ===================================================================
**     Event       :  IR_TM_COMM_OnNACK (module Events)
**
**     Component   :  IR_TM_COMM [InternalI2C]
**     Description :
**         Called when a no slave acknowledge (NAK) occurs during
**         communication. This event is not available for the SLAVE
**         mode. This event is enabled only if interrupts/events are
**         enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void IR_TM_COMM_OnNACK(void)
{
  /* Write your code here ... */
	//iflag_sensTempIR ^= IFLAG_SENS_TEMPIR_TX;
	//iflag_sensTempIR == IFLAG_IDLE

	/*Setto il flag che indica che c'è, stato un NACK sul bus I2C quindi
	 * devo far ripartire la trasmissione dei sensori di temperatura dall'inizio*/
	ON_NACK_IR_TM = TRUE;
}



/*
** ===================================================================
**     Event       :  FLASH1_OnOperationComplete (module Events)
**
**     Component   :  FLASH1 [FLASH_LDD]
*/
/*!
**     @brief
**         Called at the end of the whole write / erase operation. if
**         the event is enabled. See SetEventMask() and GetEventMask()
**         methods.
**     @param
**         UserDataPtr     - Pointer to the user or
**                           RTOS specific data. This pointer is passed
**                           as the parameter of Init method.
*/
/* ===================================================================*/
void FLASH1_OnOperationComplete(LDD_TUserData *UserDataPtr)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  AD0_OnEnd (module Events)
**
**     Component   :  AD0 [ADC]
**     Description :
**         This event is called after the measurement (which consists
**         of <1 or more conversions>) is/are finished.
**         The event is available only when the <Interrupt
**         service/event> property is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void AD0_OnEnd(void)
{
  /* Write your code here ... */

	END_ADC0=TRUE;

	AD0_Stop();
}

/*
** ===================================================================
**     Event       :  AD0_OnCalibrationEnd (module Events)
**
**     Component   :  AD0 [ADC]
**     Description :
**         This event is called when the calibration has been finished.
**         User should check if the calibration pass or fail by
**         Calibration status method./nThis event is enabled only if
**         the <Interrupt service/event> property is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void AD0_OnCalibrationEnd(void)
{
  /* Write your code here ... */
}



/*
** ===================================================================
**     Event       :  CAN_C_P_OnFreeTxBuffer (module Events)
**
**     Component   :  CAN_C_P [CAN_LDD]
*/
/*!
**     @brief
**         This event is called when the buffer is empty after a
**         successful transmit of a message. This event is available
**         only if method SendFrame is enabled.
**     @param
**         UserDataPtr     - Pointer to the user or
**                           RTOS specific data. This pointer is passed
**                           as the parameter of Init method.
**     @param
**         BufferIdx       - Receive message buffer index.
*/
/* ===================================================================*/
void CAN_C_P_OnFreeTxBuffer(LDD_TUserData *UserDataPtr, LDD_CAN_TMBIndex BufferIdx)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  CAN_C_P_OnFullRxBuffer (module Events)
**
**     Component   :  CAN_C_P [CAN_LDD]
*/
/*!
**     @brief
**         This event is called when the buffer is full after a
**         successful receive a message. This event is available only
**         if method ReadFrame or SetRxBufferState is enabled.
**     @param
**         UserDataPtr     - Pointer to the user or
**                           RTOS specific data. This pointer is passed
**                           as the parameter of Init method.
**     @param
**         BufferIdx       - Transmit buffer index.
*/
/* ===================================================================*/
void CAN_C_P_OnFullRxBuffer(LDD_TUserData *UserDataPtr, LDD_CAN_TMBIndex BufferIdx)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  TI1_OnInterrupt (module Events)
**
**     Component   :  TI1 [TimerInt]
**     Description :
**         When a timer interrupt occurs this event is called (only
**         when the component is enabled - <Enable> and the events are
**         enabled - <EnableEvent>). This event is enabled only if a
**         <interrupt service/event> is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void TI1_OnInterrupt(void)
{
  /* Write your code here ... */
	/*funzione per generare il PWM a 200 HZ per il compressore
	 * Se si vuole variare la frequenza (200 HZ dovrebbe essere il massimo
	 * si può mettere una variabile globale impsotabile che funzioni da
	 * divisore di frequenza*/

	COMP_PWM_NegVal();
}

/*
** ===================================================================
**     Event       :  IR_TM_COMM_OnArbitLost (module Events)
**
**     Component   :  IR_TM_COMM [InternalI2C]
**     Description :
**         This event is called when the master lost the bus
**         arbitration or the device detects an error on the bus. This
**         event is enabled only if interrupts/events are enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void IR_TM_COMM_OnArbitLost(void)
{
  /* Write your code here ... */
	int a =0;
}

/*
** ===================================================================
**     Event       :  System_Tick_OnInterrupt (module Events)
**
**     Component   :  System_Tick [TimerInt]
**     Description :
**         When a timer interrupt occurs this event is called (only
**         when the component is enabled - <Enable> and the events are
**         enabled - <EnableEvent>). This event is enabled only if a
**         <interrupt service/event> is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void System_Tick_OnInterrupt(void)
{
  /* Write your code here ... */
	FreeRunCnt10msec++;
	Prescaler_Tick_Timer++;
	Prescaler_Tick_TEST++;


	if (Prescaler_Tick_Timer >=5)
	{
	  Prescaler_Tick_Timer = 0;

	  /*interrupt usato per i timer che scatta ogni 50 ms*/
	  timerCounter = timerCounter + 1;

	  timerCounterADC = timerCounterADC + 1;

	  timerCounterPID = timerCounterPID + 1;

	  timerCounterMState = timerCounterMState + 1;

	  timerCounterModBus = timerCounterModBus + 1;

	  timerCounterUFlowSensor = timerCounterUFlowSensor + 1;

	  timerCounterPeltier = timerCounterPeltier + 1;

	  timerCounterCheckModBus++;

	  timerCounterCheckTempIRSens++;

	  timerCounterLedBoard++;

	}
}

/* END Events */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
