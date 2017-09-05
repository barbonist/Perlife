/*
 * Debug_Routine.h
 *
 *  Created on: 03/nov/2016
 *      Author: W15
 */

#ifndef APPLICATION_DEBUG_ROUTINE_H_
#define APPLICATION_DEBUG_ROUTINE_H_

#include "App_Ges.h"
#include "Global.h"
#include "ModBusCommProt.h"
#include "Peltier_Module.h"
#include "Adc_Ges.h"

#ifdef DEBUG_ADC
void testADCDebug(void);
#endif

#ifdef	DEBUG_PUMP
void testPUMPDebug(void);
#endif

#ifdef	DEBUG_CENTRIF_PUMP
void testCENTRFPUMPDebug(void);
#endif

#ifdef	DEBUG_PELTIER
void testPELTIERDebug(void);
#endif

#ifdef	DEBUG_COMM_SBC
void testCOMMSbcDebug(void);
#endif

#endif /* APPLICATION_DEBUG_ROUTINE_H_ */
