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
**     Event       :  AS5_SBC_OnError (module Events)
**
**     Component   :  AS5_SBC [AsynchroSerial]
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
void AS5_SBC_OnError(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  AS5_SBC_OnRxChar (module Events)
**
**     Component   :  AS5_SBC [AsynchroSerial]
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
void AS5_SBC_OnRxChar(void)
{
  /* Write your code here ... */
	// AS5_SBC_RecvChar(&msg_sbc_rx[0]);
	AS5_SBC_RecvChar(ptrMsgSbcRx);

	#ifdef	DEBUG_COMM_SBC
	//AS2_PC_DEBUG_SendChar(*ptrMsgSbcRx);
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
		ptrSbcCountRx = 0;
		//ptrMsgSbcRx = &msg_sbc_rx[0];
		ptrMsgSbcRx = &sbcDebug_rx_data[0];
	}
}

/*
** ===================================================================
**     Event       :  AS5_SBC_OnTxChar (module Events)
**
**     Component   :  AS5_SBC [AsynchroSerial]
**     Description :
**         This event is called after a character is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void AS5_SBC_OnTxChar(void)
{
  /* Write your code here ... */
	#ifdef	DEBUG_COMM_SBC
	//AS2_PC_DEBUG_SendChar(*ptrMsgSbcTx);
	//ptrMsgSbcTx = ptrMsgSbcTx + 1;
	#endif
}

/*
** ===================================================================
**     Event       :  AS5_SBC_OnFullRxBuf (module Events)
**
**     Component   :  AS5_SBC [AsynchroSerial]
**     Description :
**         This event is called when the input buffer is full;
**         i.e. after reception of the last character 
**         that was successfully placed into input buffer.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void AS5_SBC_OnFullRxBuf(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  AS5_SBC_OnFreeTxBuf (module Events)
**
**     Component   :  AS5_SBC [AsynchroSerial]
**     Description :
**         This event is called after the last character in output
**         buffer is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void AS5_SBC_OnFreeTxBuf(void)
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
	if(ptrMsg_UFLOW->bufferReceivedLenght <= 0)
	{
		iflag_uflow_sens = IFLAG_UFLOW_SENS_RX;
		ptrMsg_UFLOW->ptrBufferReceived = &ptrMsg_UFLOW->bufferReceived[0];
	}
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
	ptrMsg_UFLOW->byteSended = ptrMsg_UFLOW->byteSended + 1;
	if(ptrMsg_UFLOW->byteSended >= ptrMsg_UFLOW->bufferToSendLenght)
	{
		iflag_uflow_sens = IFLAG_UFLOW_SENS_TX;
	}
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
		//Uart3_rts_ClrVal();
}

/*
** ===================================================================
**     Event       :  AS3_PELTIER_OnError (module Events)
**
**     Component   :  AS3_PELTIER [AsynchroSerial]
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
void AS3_PELTIER_OnError(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  AS3_PELTIER_OnRxChar (module Events)
**
**     Component   :  AS3_PELTIER [AsynchroSerial]
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
void AS3_PELTIER_OnRxChar(void)
{
  /* Write your code here ... */
	/*if(iflagPeltierMsg == IFLAG_PELTIER_MSG_START)
	{*/
	AS3_PELTIER_RecvChar(ptrMsgPeltierRx);

 	#ifdef	DEBUG_COMM_SBC
	//AS2_PC_DEBUG_SendChar(*ptrMsgPeltierRx);
	#endif
	/*}*/

	/* the first who reads the data empty the buffer */
	/*AS3_PELTIER_RecvChar(ptrMsgPeltierRxDummy);

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
**     Event       :  AS3_PELTIER_OnTxChar (module Events)
**
**     Component   :  AS3_PELTIER [AsynchroSerial]
**     Description :
**         This event is called after a character is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void AS3_PELTIER_OnTxChar(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  AS3_PELTIER_OnFullRxBuf (module Events)
**
**     Component   :  AS3_PELTIER [AsynchroSerial]
**     Description :
**         This event is called when the input buffer is full;
**         i.e. after reception of the last character 
**         that was successfully placed into input buffer.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void AS3_PELTIER_OnFullRxBuf(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  AS3_PELTIER_OnFreeTxBuf (module Events)
**
**     Component   :  AS3_PELTIER [AsynchroSerial]
**     Description :
**         This event is called after the last character in output
**         buffer is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void AS3_PELTIER_OnFreeTxBuf(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  AS3_PELTIER_OnTxComplete (module Events)
**
**     Component   :  AS3_PELTIER [AsynchroSerial]
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
void AS3_PELTIER_OnTxComplete(void)
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
**     Event       :  AS2_PC_DEBUG_OnError (module Events)
**
**     Component   :  AS2_PC_DEBUG [AsynchroSerial]
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
void AS2_PC_DEBUG_OnError(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  AS2_PC_DEBUG_OnRxChar (module Events)
**
**     Component   :  AS2_PC_DEBUG [AsynchroSerial]
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
void AS2_PC_DEBUG_OnRxChar(void)
{
  /* Write your code here ... */

	//ptr = &pc_rx_data[0]; /* questa riga va messa dove si inizialiiza la comunicazione col pc */
	AS2_PC_DEBUG_RecvChar(ptrPCDebug);

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
**     Event       :  AS2_PC_DEBUG_OnTxChar (module Events)
**
**     Component   :  AS2_PC_DEBUG [AsynchroSerial]
**     Description :
**         This event is called after a character is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void AS2_PC_DEBUG_OnTxChar(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  AS2_PC_DEBUG_OnFullRxBuf (module Events)
**
**     Component   :  AS2_PC_DEBUG [AsynchroSerial]
**     Description :
**         This event is called when the input buffer is full;
**         i.e. after reception of the last character 
**         that was successfully placed into input buffer.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void AS2_PC_DEBUG_OnFullRxBuf(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  AS2_PC_DEBUG_OnFreeTxBuf (module Events)
**
**     Component   :  AS2_PC_DEBUG [AsynchroSerial]
**     Description :
**         This event is called after the last character in output
**         buffer is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void AS2_PC_DEBUG_OnFreeTxBuf(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  AS1_MODBUS_ACTUATOR_OnError (module Events)
**
**     Component   :  AS1_MODBUS_ACTUATOR [AsynchroSerial]
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
void AS1_MODBUS_ACTUATOR_OnError(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  AS1_MODBUS_ACTUATOR_OnRxChar (module Events)
**
**     Component   :  AS1_MODBUS_ACTUATOR [AsynchroSerial]
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
void AS1_MODBUS_ACTUATOR_OnRxChar(void)
{
  /* Write your code here ... */

	//AS1_MODBUS_ACTUATOR_RecvChar(msg_pmp1_rx_ptr);
	AS1_MODBUS_ACTUATOR_RecvChar(_funcRetVal.slvresRetPtr);
	_funcRetVal.slvresRetPtr = _funcRetVal.slvresRetPtr + 1;

	#ifdef DEBUG_PUMP
	//if(_funcRetVal.slvresRetNumByte > 0)
		//AS2_PC_DEBUG_SendChar(*_funcRetVal.slvresRetPtr);
	#endif

	//msg_pmp1_rx_ptr = msg_pmp1_rx_ptr + 1;

	_funcRetVal.slvresRetNumByte = _funcRetVal.slvresRetNumByte - 1;
	//check when all expected response bytes have been received
	if(_funcRetVal.slvresRetNumByte <= 0)
	{
		iflag_pmp1_rx = IFLAG_IDLE;
		iflag_pmp1_rx |= IFLAG_PMP1_RX;
	}
}

/*
** ===================================================================
**     Event       :  AS1_MODBUS_ACTUATOR_OnTxChar (module Events)
**
**     Component   :  AS1_MODBUS_ACTUATOR [AsynchroSerial]
**     Description :
**         This event is called after a character is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void AS1_MODBUS_ACTUATOR_OnTxChar(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  AS1_MODBUS_ACTUATOR_OnFullRxBuf (module Events)
**
**     Component   :  AS1_MODBUS_ACTUATOR [AsynchroSerial]
**     Description :
**         This event is called when the input buffer is full;
**         i.e. after reception of the last character 
**         that was successfully placed into input buffer.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void AS1_MODBUS_ACTUATOR_OnFullRxBuf(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  AS1_MODBUS_ACTUATOR_OnFreeTxBuf (module Events)
**
**     Component   :  AS1_MODBUS_ACTUATOR [AsynchroSerial]
**     Description :
**         This event is called after the last character in output
**         buffer is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void AS1_MODBUS_ACTUATOR_OnFreeTxBuf(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  SM1_OnBlockSent (module Events)
**
**     Component   :  SM1 [SPIMaster_LDD]
*/
/*!
**     @brief
**         This event is called after the last character from the
**         output buffer is moved to the transmitter. This event is
**         available only if the SendBlock method is enabled.
**     @param
**         UserDataPtr     - Pointer to the user or
**                           RTOS specific data. The pointer is passed
**                           as the parameter of Init method. 
*/
/* ===================================================================*/
void SM1_OnBlockSent(LDD_TUserData *UserDataPtr)
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
** ===================================================================
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
	if(ptrMsg_UFLOW->byteSended >= ptrMsg_UFLOW->bufferToSendLenght)
			Uart3_rts_ClrVal();
}

/*
** ===================================================================
**     Event       :  CI2C1_OnReceiveData (module Events)
**
**     Component   :  CI2C1 [InternalI2C]
**     Description :
**         This event is invoked when I2C finishes the reception of the
**         data successfully. This event is not available for the SLAVE
**         mode and if both RecvChar and RecvBlock are disabled. This
**         event is enabled only if interrupts/events are enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void CI2C1_OnReceiveData(void)
{
  /* Write your code here ... */
	iflag_sensTempIR = IFLAG_IDLE;
	iflag_sensTempIR_Meas_Ready = IFLAG_IRTEMP_MEASURE_READY;
	CI2C1_SendStop();
}

/*
** ===================================================================
**     Event       :  CI2C1_OnTransmitData (module Events)
**
**     Component   :  CI2C1 [InternalI2C]
**     Description :
**         This event is invoked when I2C finishes the transmission of
**         the data successfully. This event is not available for the
**         SLAVE mode and if both SendChar and SendBlock are disabled.
**         This event is enabled only if interrupts/events are enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void CI2C1_OnTransmitData(void)
{
  /* Write your code here ... */
	iflag_sensTempIR = IFLAG_SENS_TEMPIR_TX;
	if(iflag_sensTempIRRW == IFLAG_SENS_TEMPIR_WRITE)
	{
		//iflag_sensTempIRRW = IFLAG_IDLE;
		//CI2C1_Disable();
		CI2C1_SendStop();
		//CI2C1_Enable();
	}
}

/*
** ===================================================================
**     Event       :  CI2C1_OnNACK (module Events)
**
**     Component   :  CI2C1 [InternalI2C]
**     Description :
**         Called when a no slave acknowledge (NAK) occurs during
**         communication. This event is not available for the SLAVE
**         mode. This event is enabled only if interrupts/events are
**         enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void CI2C1_OnNACK(void)
{
  /* Write your code here ... */
	//iflag_sensTempIR ^= IFLAG_SENS_TEMPIR_TX;
	//iflag_sensTempIR == IFLAG_IDLE;
}

/*
** ===================================================================
**     Event       :  TU1_OnCounterRestart (module Events)
**
**     Component   :  TU1 [TimerUnit_LDD]
*/
/*!
**     @brief
**         Called if counter overflow/underflow or counter is
**         reinitialized by modulo or compare register matching.
**         OnCounterRestart event and Timer unit must be enabled. See
**         [SetEventMask] and [GetEventMask] methods. This event is
**         available only if a [Interrupt] is enabled.
**     @param
**         UserDataPtr     - Pointer to the user or
**                           RTOS specific data. The pointer passed as
**                           the parameter of Init method.
*/
/* ===================================================================*/
void TU1_OnCounterRestart(LDD_TUserData *UserDataPtr)
{
  /* Write your code here ... */
  timerCounter = timerCounter + 1;

  timerCounterADC = timerCounterADC + 1;

  timerCounterPID = timerCounterPID + 1;

  timerCounterMState = timerCounterMState + 1;

  timerCounterModBus = timerCounterModBus + 1;

  timerCounterUFlowSensor = timerCounterUFlowSensor + 1;

  timerCounterPeltier = timerCounterPeltier + 1;
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
