/* ###################################################################
**     Filename    : Events.c
**     Project     : SCTRO
**     Processor   : MK64FN1M0VLQ12
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2016-06-07, 18:03, # CodeGen: 0
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
	//new_byte_rx = 1;
	iflag_pmp1_rx |= IFLAG_PMP1_RX;
	//AS1_MODBUS_ACTUATOR_RecvChar(msg_pmp1_rx_ptr);
	AS1_MODBUS_ACTUATOR_RecvChar(_funcRetVal.slvresRetPtr);

	//AS2_PC_DEBUG_SendChar(*msg_pmp1_rx_ptr);
	if(_funcRetVal.slvresRetNumByte > 0)
		AS2_PC_DEBUG_SendChar(*_funcRetVal.slvresRetPtr);

	//msg_pmp1_rx_ptr = msg_pmp1_rx_ptr + 1;
	_funcRetVal.slvresRetPtr = _funcRetVal.slvresRetPtr + 1;
	_funcRetVal.slvresRetNumByte = _funcRetVal.slvresRetNumByte - 1;
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
	/*new_byte_rx = 1;*/
	/*char ch;
	AS2_PC_DEBUG_RecvChar(ch);*/
	//unsigned char * ptr;
	//ptr = &pc_rx_data[0]; /* questa riga va messa dove si inizialiiza la comunicazione col pc */
	AS2_PC_DEBUG_RecvChar(ptr);
	//unsigned int num;
	//num = atoi(ptr);
	ptr = ptr + 1;
	ptr_count = ptr_count + 1;
	if(ptr_count >= 16)
	{
		iflag_pc_rx |= IFLAG_PC_RX;
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
	static char countByte = 0;

	countByte++;
	//ptrDebug = ptrDebug + 1;
	if(countByte <= 7);
		//AS2_PC_DEBUG_SendChar(*ptrDebug);
	else
	{
		countByte = 0;
	}

	Bit1_NegVal();
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
	Bit1_NegVal();
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
**     Event       :  AS4_C2P_OnError (module Events)
**
**     Component   :  AS4_C2P [AsynchroSerial]
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
void AS4_C2P_OnError(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  AS4_C2P_OnRxChar (module Events)
**
**     Component   :  AS4_C2P [AsynchroSerial]
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
void AS4_C2P_OnRxChar(void)
{
  /* Write your code here ... */

}

/*
** ===================================================================
**     Event       :  AS4_C2P_OnTxChar (module Events)
**
**     Component   :  AS4_C2P [AsynchroSerial]
**     Description :
**         This event is called after a character is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void AS4_C2P_OnTxChar(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  AS4_C2P_OnFullRxBuf (module Events)
**
**     Component   :  AS4_C2P [AsynchroSerial]
**     Description :
**         This event is called when the input buffer is full;
**         i.e. after reception of the last character 
**         that was successfully placed into input buffer.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void AS4_C2P_OnFullRxBuf(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  AS4_C2P_OnFreeTxBuf (module Events)
**
**     Component   :  AS4_C2P [AsynchroSerial]
**     Description :
**         This event is called after the last character in output
**         buffer is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void AS4_C2P_OnFreeTxBuf(void)
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
  AS5_SBC_RecvChar(&msg_sbc_rx[0]);
  AS5_SBC_SendChar(msg_sbc_rx[0]+0x02);
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
