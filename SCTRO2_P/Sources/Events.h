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
#include "AS1_ACTUATOR.h"
#include "ASerialLdd1.h"
#include "LED_GREEN.h"
#include "BitIoLdd1.h"
#include "LED_BLUE.h"
#include "BitIoLdd2.h"
#include "LED_RED.h"
#include "BitIoLdd3.h"
#include "AS2_PC_DEBUG.h"
#include "ASerialLdd2.h"

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
**     Event       :  AS1_ACTUATOR_OnError (module Events)
**
**     Component   :  AS1_ACTUATOR [AsynchroSerial]
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
void AS1_ACTUATOR_OnError(void);

/*
** ===================================================================
**     Event       :  AS1_ACTUATOR_OnRxChar (module Events)
**
**     Component   :  AS1_ACTUATOR [AsynchroSerial]
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
void AS1_ACTUATOR_OnRxChar(void);

/*
** ===================================================================
**     Event       :  AS1_ACTUATOR_OnTxChar (module Events)
**
**     Component   :  AS1_ACTUATOR [AsynchroSerial]
**     Description :
**         This event is called after a character is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void AS1_ACTUATOR_OnTxChar(void);

/*
** ===================================================================
**     Event       :  AS1_ACTUATOR_OnFullRxBuf (module Events)
**
**     Component   :  AS1_ACTUATOR [AsynchroSerial]
**     Description :
**         This event is called when the input buffer is full;
**         i.e. after reception of the last character 
**         that was successfully placed into input buffer.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void AS1_ACTUATOR_OnFullRxBuf(void);

/*
** ===================================================================
**     Event       :  AS1_ACTUATOR_OnFreeTxBuf (module Events)
**
**     Component   :  AS1_ACTUATOR [AsynchroSerial]
**     Description :
**         This event is called after the last character in output
**         buffer is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void AS1_ACTUATOR_OnFreeTxBuf(void);

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
