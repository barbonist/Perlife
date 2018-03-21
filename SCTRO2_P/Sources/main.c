/* ###################################################################
**     Filename    : main.c
**     Project     : SCTRO2_P
**     Processor   : MK64FN1M0VLQ12
**     Version     : Driver 01.01
**     Compiler    : GNU C Compiler
**     Date/Time   : 2016-07-19, 16:06, # CodeGen: 0
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
#include "TI1_10ms.h"
#include "TimerIntLdd1.h"
#include "TU1.h"
#include "TI1_1ms.h"
#include "TimerIntLdd2.h"
#include "TU2.h"
#include "TI1_100ms.h"
#include "TimerIntLdd3.h"
#include "TU3.h"
#include "EN_MOTOR_P_1.h"
#include "BitIoLdd39.h"
#include "EN_MOTOR_P_2.h"
#include "BitIoLdd40.h"
#include "EN_MOTOR_P_3.h"
#include "BitIoLdd41.h"
#include "EN_MOTOR_P_4.h"
#include "BitIoLdd42.h"
#include "EN_MOTOR_P_5.h"
#include "BitIoLdd47.h"
#include "EN_CLAMP_P_1.h"
#include "BitIoLdd43.h"
#include "EN_CLAMP_P_2.h"
#include "BitIoLdd44.h"
#include "EN_CLAMP_P_3.h"
#include "BitIoLdd45.h"
#include "EN_CLAMP_P_4.h"
#include "BitIoLdd46.h"
#include "M1_HALL_A.h"
#include "BitIoLdd48.h"
#include "M1_HALL_B.h"
#include "BitIoLdd49.h"
#include "M2_HALL_A.h"
#include "BitIoLdd50.h"
#include "M2_HALL_B.h"
#include "BitIoLdd51.h"
#include "M3_HALL_A.h"
#include "BitIoLdd52.h"
#include "M3_HALL_B.h"
#include "BitIoLdd53.h"
#include "M4_HALL_A.h"
#include "BitIoLdd54.h"
#include "M4_HALL_B.h"
#include "BitIoLdd55.h"
#include "C1_HALL_L.h"
#include "BitIoLdd56.h"
#include "C1_HALL_R.h"
#include "BitIoLdd57.h"
#include "C2_HALL_L.h"
#include "BitIoLdd58.h"
#include "C2_HALL_R.h"
#include "BitIoLdd59.h"
#include "C3_HALL_L.h"
#include "BitIoLdd60.h"
#include "C3_HALL_R.h"
#include "BitIoLdd61.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "PDD_Includes.h"
#include "Init_Config.h"
/**/
/* User includes (#include below this line is not maintained by Processor Expert) */
#include "ModBusCommProt.h"
#include "Machine_State.h"
#include "Global.h"
#include "App_Ges.h"
#include "Uart_utilities.h"

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  /* Write your local variable definition here */
	static int index = 0;
	//ptr_count = 0x00;
	ptr = &pc_rx_data[0];
	ptrCount = 0;
	iflag_pc_rx = IFLAG_IDLE;
  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */

  /**************************************/
  /*****Initialisation******************/

  /* Init machine state */
  initAllState();

  /* Init guard */
  initAllGuard();

  /*****End Initialisation**************/
  /* For example: for(;;) { } */
  while(1)
  {
	#ifdef DEBUG_ENABLE
	  index++;
	  if(index%64000 == 0)
	  {
		  EN_24_M_P_NegVal();
	  }
	#endif

	  /*ADC reading */
	  /*ADC reading */

	  /*Machine state:
	   	   	   	   	   - guard computing
	   	   	   	   	   - state transition computing */
	  computeMachineStateGuard();
	  processMachineState();
	  /*Machine state */

	  /*UART 0 - actuator reading */
		#ifdef	DEBUG_ENABLE

		#endif
	  //ManageCommActuator();
	  /*UART 0 - actuator reading */

	  /*UART 1 - pc debug */
	  ManageCommPcDebug();
	  /*UART 1 - pc debug */

	  /*UART 2 - sbc */
	  //ManageCommSBC();
	  /*UART 2 - SBC */

	  /*UART3 - C2P */
	  //ManageCommC2P();
	  /*UART3 - C2P */

	  /**/
  }


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
