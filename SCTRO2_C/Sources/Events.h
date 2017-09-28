/* ###################################################################
**     Filename    : Events.h
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
** @file Events.h
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup Events_module Events module documentation
**  @{
*/         

#ifndef __Events_H
#define __Events_H
/* MODULE Events */

#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "Pins1.h"
#include "AS1_MODBUS_ACTUATOR.h"
#include "ASerialLdd1.h"
#include "AS2_PC_DEBUG.h"
#include "ASerialLdd2.h"
#include "Bit3.h"
#include "BitIoLdd1.h"
#include "Bit4.h"
#include "BitIoLdd2.h"
#include "AD1.h"
#include "AdcLdd1.h"
#include "AS3_PELTIER.h"
#include "ASerialLdd3.h"
#include "FLOWSENS_COMM.h"
#include "ASerialLdd4.h"
#include "AS5_SBC.h"
#include "ASerialLdd5.h"
#include "Bit1.h"
#include "BitIoLdd3.h"
#include "Bit2.h"
#include "BitIoLdd4.h"
#include "SM1.h"
#include "Uart3_rts.h"
#include "BitIoLdd5.h"
#include "CI2C1.h"
#include "IntI2cLdd1.h"
#include "TU1.h"
#include "FLASH1.h"

#ifdef __cplusplus
extern "C" {
#endif 

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
void Cpu_OnNMI(void);


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
void AS5_SBC_OnError(void);

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
void AS5_SBC_OnRxChar(void);

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
void AS5_SBC_OnTxChar(void);

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
void AS5_SBC_OnFullRxBuf(void);

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
void AS5_SBC_OnFreeTxBuf(void);

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
void FLOWSENS_COMM_OnError(void);

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
void FLOWSENS_COMM_OnRxChar(void);

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
void FLOWSENS_COMM_OnTxChar(void);

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
void FLOWSENS_COMM_OnFullRxBuf(void);

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
void FLOWSENS_COMM_OnFreeTxBuf(void);

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
void AS3_PELTIER_OnError(void);

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
void AS3_PELTIER_OnRxChar(void);

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
void AS3_PELTIER_OnTxChar(void);

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
void AS3_PELTIER_OnFullRxBuf(void);

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
void AS3_PELTIER_OnFreeTxBuf(void);

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
void AS3_PELTIER_OnTxComplete(void);

void AD1_OnEnd(void);
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

void AD1_OnCalibrationEnd(void);
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
void AS2_PC_DEBUG_OnError(void);

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
void AS2_PC_DEBUG_OnRxChar(void);

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
void AS2_PC_DEBUG_OnTxChar(void);

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
void AS2_PC_DEBUG_OnFullRxBuf(void);

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
void AS2_PC_DEBUG_OnFreeTxBuf(void);

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
void AS1_MODBUS_ACTUATOR_OnError(void);

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
void AS1_MODBUS_ACTUATOR_OnRxChar(void);

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
void AS1_MODBUS_ACTUATOR_OnTxChar(void);

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
void AS1_MODBUS_ACTUATOR_OnFullRxBuf(void);

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
void AS1_MODBUS_ACTUATOR_OnFreeTxBuf(void);

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
void SM1_OnBlockSent(LDD_TUserData *UserDataPtr);

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
void SM1_OnBlockReceived(LDD_TUserData *UserDataPtr);

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
void FLOWSENS_COMM_OnTxComplete(void);

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
void CI2C1_OnReceiveData(void);

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
void CI2C1_OnTransmitData(void);

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
void CI2C1_OnNACK(void);

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
void TU1_OnCounterRestart(LDD_TUserData *UserDataPtr);

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
void FLASH1_OnOperationComplete(LDD_TUserData *UserDataPtr);

/* END Events */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

#endif 
/* ifndef __Events_H*/
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
