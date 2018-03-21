/* ###################################################################
**     Filename    : Events.h
**     Project     : SCTRO2_P
**     Processor   : MK64FN1M0VLQ12
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2016-07-19, 16:06, # CodeGen: 0
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
#include "ASerialLdd1.h"
#include "FLOWSENS_RE.h"
#include "BitIoLdd22.h"
#include "FLOWSENS_DE.h"
#include "BitIoLdd23.h"
#include "PC_DEBUG_COMM.h"
#include "EN_24_M_P.h"
#include "BitIoLdd1.h"
#include "D_7S_DP.h"
#include "BitIoLdd9.h"
#include "D_7S_F.h"
#include "BitIoLdd2.h"
#include "D_7S_G.h"
#include "BitIoLdd3.h"
#include "D_7S_E.h"
#include "BitIoLdd4.h"
#include "D_7S_D.h"
#include "BitIoLdd5.h"
#include "D_7S_C.h"
#include "BitIoLdd6.h"
#include "D_7S_B.h"
#include "BitIoLdd7.h"
#include "D_7S_A.h"
#include "BitIoLdd8.h"
#include "BUBBLE_KEYBOARD_BUTTON1.h"
#include "BitIoLdd10.h"
#include "BUBBLE_KEYBOARD_BUTTON2.h"
#include "BitIoLdd11.h"
#include "BUBBLE_KEYBOARD_BUTTON3.h"
#include "BitIoLdd12.h"
#include "BUBBLE_KEYBOARD_BUTTON4.h"
#include "BitIoLdd13.h"
#include "BUZZER_LOW_P.h"
#include "BitIoLdd14.h"
#include "BUZZER_MEDIUM_P.h"
#include "BitIoLdd15.h"
#include "BUZZER_HIGH_P.h"
#include "BitIoLdd16.h"
#include "AD0.h"
#include "AdcLdd1.h"
#include "AD1.h"
#include "AdcLdd2.h"
#include "COVER_M1.h"
#include "BitIoLdd17.h"
#include "COVER_M2.h"
#include "BitIoLdd18.h"
#include "COVER_M3.h"
#include "BitIoLdd19.h"
#include "COVER_M4.h"
#include "BitIoLdd20.h"
#include "COVER_M5.h"
#include "BitIoLdd21.h"
#include "FLOWSENS_COMM.h"
#include "ASerialLdd2.h"
#include "SBC_COMM.h"
#include "ASerialLdd3.h"
#include "IR_TM_COMM.h"
#include "IntI2cLdd1.h"
#include "AIR_SENSOR_3.h"
#include "BitIoLdd24.h"
#include "AIR_T_3.h"
#include "BitIoLdd27.h"
#include "AIR_SENSOR_2.h"
#include "BitIoLdd28.h"
#include "AIR_T_2.h"
#include "BitIoLdd26.h"
#include "AIR_SENSOR_1.h"
#include "BitIoLdd29.h"
#include "AIR_T_1.h"
#include "BitIoLdd25.h"
#include "LAMP_HIGH.h"
#include "BitIoLdd30.h"
#include "LAMP_MEDIUM.h"
#include "BitIoLdd31.h"
#include "LAMP_LOW.h"
#include "BitIoLdd32.h"
#include "HEAT_ON_P.h"
#include "BitIoLdd33.h"
#include "COMP_ENABLE.h"
#include "BitIoLdd34.h"
#include "CAN_C_P.h"
#include "VOLTAGE_B_CHK.h"
#include "BitIoLdd35.h"
#include "VOLTAGE_M_CHK.h"
#include "BitIoLdd36.h"
#include "EN_P_2_C.h"
#include "BitIoLdd37.h"
#include "EN_P_1_C.h"
#include "BitIoLdd38.h"

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
void IR_TM_COMM_OnReceiveData(void);

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
void IR_TM_COMM_OnTransmitData(void);

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
void IR_TM_COMM_OnArbitLost(void);

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
void IR_TM_COMM_OnNACK(void);

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
void SBC_COMM_OnError(void);

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
void SBC_COMM_OnRxChar(void);

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
void SBC_COMM_OnTxChar(void);

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
void SBC_COMM_OnFullRxBuf(void);

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
void SBC_COMM_OnFreeTxBuf(void);

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
void PC_DEBUG_COMM_OnError(void);

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
void PC_DEBUG_COMM_OnRxChar(void);

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
void PC_DEBUG_COMM_OnTxChar(void);

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
void PC_DEBUG_COMM_OnFullRxBuf(void);

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
void PC_DEBUG_COMM_OnFreeTxBuf(void);

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

void AD0_OnEnd(void);
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

void AD0_OnCalibrationEnd(void);
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
void CAN_C_P_OnFreeTxBuffer(LDD_TUserData *UserDataPtr, LDD_CAN_TMBIndex BufferIdx);

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
void CAN_C_P_OnFullRxBuffer(LDD_TUserData *UserDataPtr, LDD_CAN_TMBIndex BufferIdx);

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
