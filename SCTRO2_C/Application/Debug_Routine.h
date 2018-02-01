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


#ifdef	SERVICE_ACTIVE_TOGETHER_THERAPY
void Service_SBC(void);
#endif

#endif /* APPLICATION_DEBUG_ROUTINE_H_ */
