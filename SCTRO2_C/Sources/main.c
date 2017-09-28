/* ###################################################################
**     Filename    : main.c
**     Project     : SCTRO2_C
**     Processor   : MK64FN1M0VLQ12
**     Version     : Driver 01.01
**     Compiler    : GNU C Compiler
**     Date/Time   : 2016-07-19, 16:01, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 01.01
** @brief
**         Main module.
**         This module contains user's application code.
*/         
/*!
**  @addtogroup main_module main module documentation
**  @{
*/         
/* MODULE main */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "Pins1.h"
#include "MODBUS_COMM.h"
#include "ASerialLdd1.h"
#include "AS2_PC_DEBUG.h"
#include "ASerialLdd2.h"
#include "Bit3.h"
#include "BitIoLdd1.h"
#include "Bit4.h"
#include "BitIoLdd2.h"
#include "AD1.h"
#include "AdcLdd1.h"
#include "PELTIER_COMM.h"
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
#include "FLOWSENS_RTS.h"
#include "BitIoLdd5.h"
#include "CI2C1.h"
#include "IntI2cLdd1.h"
#include "TU1.h"
#include "FLASH1.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "PDD_Includes.h"
#include "Init_Config.h"
/* User includes (#include below this line is not maintained by Processor Expert) */
#include "App_Ges.h"
#include "Global.h"
#include "ModBusCommProt.h"
#include "Peltier_Module.h"
#include "Adc_Ges.h"
#include "Alarm_Con.h"
#include "Perfusion.h"
#include "Purificat.h"
#include "Debug_Routine.h"
#include "Flowsens.h"
#include "Temp_sensIR.h"
#include "Comm_Sbc.h"

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */


int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  /* Write your local variable definition here */

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */

  initAllGuard();
  initAllState();
  initGUIButton();
  initSetParamFromGUI();

  alarmConInit();
  initPerfusionParam();
  initPurifParam();

  peltierAssInit();

  initAdcParam();

  initUFlowSensor();
  initTempSensIR();

  timerCounterMState = 0;
  timerCounterUFlowSensor = 0;

  /*#ifdef	DEBUG_I2C_TEMP_SENS
  unsigned char rcvData[20];
  unsigned char * ptrData;
  unsigned char * ptrDatawR;
  unsigned char err;
  unsigned char stateSensTempIR;
  word ret;
  for(char i=0; i<20;i++)
  {
  	 rcvData[i] = 0;
  }
  stateSensTempIR = 0;
 #endif*/

  modBusPmpInit();
  modBusPinchInit();

  AS5_SBC_Enable();
    /**/
  Bit3_SetVal(); /* enable motore */
  Bit4_SetVal();

  static int index = 0;
  //ptrMsgSbcRx = &msg_sbc_rx[0];
  ptrMsgSbcRx = &sbcDebug_rx_data[0];
  ptrSbcCountRx = 0;

  ptrPCDebug = &msgPcDebug[0];
  ptrPCDebugCount = 0;

  ptrMsgPeltierRx = &peltierDebug_rx_data[0];

  ptrPeltierCountRx = 0;

  uint8_t InpData[4];
  uint8_t OutData[4] = "0123";
  LDD_TDeviceData *MySPIPtr;
  LDD_TError Error;
  iflag_spi_rx = IFLAG_IDLE;

  char dummy;

  int timerCounterModBusOld = 0;
  /* For example: for(;;) { } */

  /**********MAIN LOOP START************/
  for(;;) {
	  	  	 /***********DEBUG START*************/
			 #ifdef DEBUG_ENABLE
	  	  	  	  index++;
	         	  if(index%32000 == 0)
	         		 Bit1_NegVal();
	         	  else if(index%65000 == 0)
	         	  {
	         		 Bit2_NegVal();
	         		 index = 0;
	         	  }

				#ifdef	DEBUG_ADC
	         	  //testADCDebug();
	         	 //MySPIPtr = SM1_Init(NULL);
	         	 //Error = SM1_SelectConfiguration(MySPIPtr, 1U, 0U);
	         	 //Error = SM1_ReceiveBlock(MySPIPtr, InpData, 4);
	         	 //Error = SM1_SendBlock(MySPIPtr, OutData, 4);
	         	 //while (!(iflag_spi_rx == IFLAG_SPI_RX_TRUE)) {};
				#endif

				#ifdef	DEBUG_PUMP
	         	 //testPUMPDebug();
				#endif

				#ifdef	DEBUG_CENTRIF_PUMP
	         	//testCENTRFPUMPDebug();
				#endif

				#ifdef	DEBUG_PELTIER
	         	//testPELTIERDebug();
				#endif

				#ifdef DEBUG_FLOW_SENS
	         	  if(index%10000 == 0)
	         	  {
	         		  ptrMsg_UFLOW = buildCmdToFlowSens(FLOW_SENSOR_ONE_ADDR,
	         				  	  	  	  	  	  	  	CMD_GET_VAL_CODE /*CMD_IDENT_CODE*/,
													   0,
													   0,
													   ID_FLOW_VAL_MLMIN);

	         		 FLOWSENS_RTS_SetVal();
	         		  for(char k = 0; k < ptrMsg_UFLOW->bufferToSendLenght; k++)
						{
							//FLOWSENS_RTS_SetVal();
							FLOWSENS_COMM_SendChar(ptrMsg_UFLOW->bufferToSend[k]);
						}

	         		 while(!(iflag_uflow_sens == IFLAG_UFLOW_SENS_RX))
	         			 {
	         			 dummy++;
	         			 }

	         	  }

				/*while(!(iflag_uflow_sens == IFLAG_UFLOW_SENS_RX)){
					iflag_uflow_sens = IFLAG_IDLE;
				};*/
				#endif

				#ifdef	DEBUG_I2C_TEMP_SENS

	         	 //if(index%10000 == 0)
	         	  switch(stateSensTempIR)
	         	  {
	         	  case 0:
	         		 if(iflag_sensTempIR == IFLAG_IDLE)
	         		 	 {
	         		 	   //CI2C1_Disable();
	         		 	    CI2C1_Enable();
	         		 	    ptrData = buildCmdReadTempSensIR(0x01, (EEPROM_ACCESS_COMMAND | SD_TOMIN_E2_ADDRESS), 0);
	         		 	    //ptrData = buildCmdWriteTempSensIR(0x5A, (EEPROM_ACCESS_COMMAND | SD_TOMAX_E2_ADDRESS), 0xBE01);
	         		 	    iflag_sensTempIR = IFLAG_SENS_TEMPIR_WAIT;
	         		 	  }

	         		if(iflag_sensTempIR == IFLAG_SENS_TEMPIR_TX)
	         		{
	         		//CI2C1_SelectSlave(0x5A);
	         			err = CI2C1_RecvBlock(ptrData, 3, &ret);
	         			iflag_sensTempIR = IFLAG_SENS_TEMPIR_WAIT;
	         			stateSensTempIR = 1;
	         		}
	         		  break;

	         	  case 1:
	         		 if(index%10000 == 0)
	         		  {
	         		 	 iflag_sensTempIR = IFLAG_IDLE;
	         		 	 stateSensTempIR = 2;
	         		  }
	         		  break;

	         	  case 2:
	         		  if(iflag_sensTempIR == IFLAG_IDLE)
	         		  {

	         			ptrDatawR = buildCmdWriteTempSensIR(0x01, (EEPROM_ACCESS_COMMAND | SD_TOMIN_E2_ADDRESS), 0x62E5);

	         			iflag_sensTempIR = IFLAG_SENS_TEMPIR_WAIT;
	         		  }

	         		 if(iflag_sensTempIR == IFLAG_SENS_TEMPIR_TX)
	         		 {
	         			stateSensTempIR = 3;
	         			CI2C1_Disable();
	         			//CI2C1_Enable();
	         		 }
	         		  break;

	         	  case 3:
	         		 if(index%10000 == 0)
	         		 {
	         			iflag_sensTempIR = IFLAG_IDLE;
	         			stateSensTempIR = 4;
	         		 }
	         		  break;

	         	 case 4:
	         		if(iflag_sensTempIR == IFLAG_IDLE)
	         		{
	         			CI2C1_Enable();
	         			ptrDatawR = buildCmdWriteTempSensIR(0x01, (EEPROM_ACCESS_COMMAND | SD_TOMIN_E2_ADDRESS), 0x62E5);
	         			iflag_sensTempIR = IFLAG_SENS_TEMPIR_WAIT;
	         		}


	         		if(iflag_sensTempIR == IFLAG_SENS_TEMPIR_TX)
	         		{
	         			stateSensTempIR = 5;
	         			CI2C1_Disable();
	         			//CI2C1_Enable();
	         		}
	         	 	 break;

	         	 case 5:
	         		 if(index%10000 == 0)
	         		{
	         			 iflag_sensTempIR = IFLAG_IDLE;
	         			 stateSensTempIR = 0;
	         			 //Cpu_SystemReset();
	         		}
	         		 break;

	         	  default:
	         		  break;
	         	  }

	         	  //if((iflag_sensTempIR == IFLAG_IDLE))
	         	 //{
	         		 //CI2C1_Disable();
	         		 //CI2C1_Enable();
	         		 //ptrData = buildCmdReadTempSensIR(0x5A, (EEPROM_ACCESS_COMMAND | SD_SMBUS_E2_ADDRESS), 0);
	         		//ptrData = buildCmdWriteTempSensIR(0x5A, (EEPROM_ACCESS_COMMAND | SD_TOMAX_E2_ADDRESS), 0xBE01);
	         		//iflag_sensTempIR = IFLAG_SENS_TEMPIR_WAIT;

	         	 //}
	         	 /*else if((iflag_sensTempIR == IFLAG_SENS_TEMPIR_TX) && (iflag_sensTempIRRW == IFLAG_SENS_TEMPIR_WRITE))
	         	 {
	         		iflag_sensTempIRRW = IFLAG_IDLE;
	         		ptrData = buildCmdReadTempSensIR(0x01, (EEPROM_ACCESS_COMMAND | SD_SMBUS_E2_ADDRESS), 0);
	         		iflag_sensTempIR = IFLAG_SENS_TEMPIR_WAIT;
	         	 }*/

	         	 //if((iflag_sensTempIR == IFLAG_SENS_TEMPIR_TX))
	         	 //{
	         		//CI2C1_SelectSlave(0x5A);
	         		//err = CI2C1_RecvBlock(ptrData, 3, &ret);
	         		//iflag_sensTempIR = IFLAG_SENS_TEMPIR_WAIT;
	         		//iflag_sensTempIR = IFLAG_IDLE;
	         	 //}

	         	#endif

				#ifdef	DEBUG_COMM_SBC
	         	//AS5_SBC_Enable();
	         	testCOMMSbcDebug();

	         	alwaysAdcParam();

	         	if(timerCounterUFlowSensor >= 2){
	         		timerCounterUFlowSensor = 0;

	         		alwaysUFlowSensor();
	         	}
				#endif

		 	 #endif
	         /***********DEBUG END*************/

			 #ifdef DEBUG_TREATMENT
	         /* sbc comm - start */
	         initCommSBC();
	         pollingSBCCommTreat();
	         pollingDataToSBCTreat();
	         /* sbc comm - end */

	         /*****MACHINE STATE UPDATE START****/
	         if(timerCounterMState >= 1)
	         {
	        	timerCounterMState = 0;

	        	computeMachineStateGuard();

	        	processMachineState();

	        	alarmEngineAlways();

	        	//alwaysModBusActuator();
	         }
	         /****MACHINE STATE UPDATE END****/

	         /*******************************/
	         /*UFLOW SENSOR                 */
	         if(timerCounterUFlowSensor >= 2){
	        	 timerCounterUFlowSensor = 0;

	        	 alwaysUFlowSensor();

	         }

	         if(timerCounterPeltier >= 2){
	        	 timerCounterPeltier = 0;

	        	 if(peltierCell.readAlwaysEnable == 0)
	        		 alwaysPeltierActuator();

	         }

	         alwaysIRTempSensRead();
	         /*******************************/
	         /*UFLOW SENSOR                 */

	         /********************************/
	         /*             ADC	             */
	         /********************************/
	         alwaysAdcParam();
	         /********************************/
	         /*				ADC				 */
	         /********************************/

	         /*********PUMP*********/
	         if(timerCounterModBus != timerCounterModBusOld)
	         {
	        	 timerCounterModBusOld = timerCounterModBus;
	        	 alwaysModBusActuator();
	         }
	         /*********PUMP*********/
			 #endif

  }
  /**********MAIN LOOP END**************/

  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
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
