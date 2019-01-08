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
#include "EMERGENCY_BUTTON.h"
#include "BitIoLdd17.h"
#include "FRONTAL_COVER_1.h"
#include "BitIoLdd18.h"
#include "FRONTAL_COVER_2.h"
#include "BitIoLdd19.h"
#include "HOOK_SENSOR_1.h"
#include "BitIoLdd20.h"
#include "HOOK_SENSOR_2.h"
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
#include "EEPROM.h"
#include "IntFlashLdd1.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "PDD_Includes.h"
#include "Init_Config.h"
/**/
/* User includes (#include below this line is not maintained by Processor Expert) */
//#include "ModBusCommProt.h"
#include "Machine_State.h"
#include "Global.h"
#include "App_Ges.h"
#include "Uart_utilities.h"

#include "SWTimer.h"
#include "SevenSeg.h"
#include "FlexCanWrapper.h"
#include "Temp_sensIR.h"
#include "SensorAccess.h"
#include "ControlProtectiveInterface.h"
#include "RPMGauge.h"
#include "BubbleKeyboard.h"
#include "Adc_Ges.h"
#include "ActionsProtective.h"
#include "debug_routine.h"

/*prescaler per timer printf di debug*/
unsigned char Timer_printf_debug = 1;

void InitTest(void);
void verificaTempPlate(void);
/*lint -save  -e970 Disable MISRA rule (6.3) checking. */

int Phase = 0x99;
/*Funtion to enable power and actuation of teh actuator*/
void verificaTempPlate(void)
{
	float differenza;

	if (timerConfrontaTempPlate>=100)
	{
		timerConfrontaTempPlate=0;
		differenza=tempPlateControl - T_PLATE_P_GRADI_CENT;
		if (differenza<0)
		{
			differenza=-differenza;
		}
		if (differenza>2)
		{
			// la differenza tra le due temperature lette è eccessiva - allarme
			// per adesso non lo metto perchè la gestione degli allarmi protective la sta facendo Barbonix
		}
	}
	// nella stessa funzione metto anche l'aggiornamento del buffer con il valore di temperatura piatto letto
	onNewTempPlateValue((int16_t)(T_PLATE_P_GRADI_CENT*10));
}

int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
  InitCAN();
  SwTimerInit();
  InitTest();
  initTempSensIR();
  InitSevenSeg();
  InitSensorsAccess();
  InitControlProtectiveInterface();
  InitRPMGauge();
  InitBubbleKeyboard();
  /**/
  ADC0_Calibration();
  ADC1_Calibration();
  timerCounterADC1 = 0;
  timerCounterADC0 = 0;

  Dip_Switch_ADC_Init();
  PR_Sens_ADC_Init();
  // SB T_PLATE_P_Init();
  InitActuators();

  IR_TM_COMM_Enable();
  ptrMsgSbcRx = &sbc_rx_data[0];
  ptrSbcCountRx = 0;
  timerCounter=0;

  /*leggo tutta la struttura dati salvata nella parte di flash usata come EEPROM (ci saranno ad esmepio i coefficienti di claibrazione)*/
   EEPROM_Read(START_ADDRESS_EEPROM, (EEPROM_TDataAddress)&config_data, sizeof(config_data));

  /*scrivo i dati di default sulla flash usata come eeprom
   * (tra cui i coef di calibrazione dei sensori) solo
   * se la flash usata come eeprom non è mai stata scritta.
   * TODO quando sarà fatta la funzione per la calibrazione,
   * dovrà essere fatta in modo simile per sovrascrivere i coefficienti*/
   Set_Data_EEPROM_Default();

   // Filippo - inizializzo il timer per il confronto tra la temperatura di piatto letta dalla protective e la temperatura
   // di piatto spedita dalla control
   timerConfrontaTempPlate=0;

   //printf("provo di scrivere sulla console \r\n");

  for(;;)
  {
  	  if (Service)
  		 Service_SBC();

	  ManageSwTimers();
	  Phase = 2;
	  UpdateActuatorPosition();
	  Phase = 3;
	  Manage_IR_Sens_Temp();
	  /*funzioni per leggere i canali AD*/
	  Phase = 4;
	  Manange_ADC0();
	  Phase = 5;
	  Manange_ADC1();
	  /*END funzioni per leggere i canali AD*/

	 /*faccio lo start della conversione sul canale AD0 ogni 50 msec*/
	 if (timerCounterADC0 >=1)
	 {
		Phase = 6;
		timerCounterADC0 = 0;
		AD0_Start();

		Timer_printf_debug ++;

		/*ogni 500 msec (ovvero una volta ogni 10 ingressi in questo if da 50 msec)
		 * trasmetto sulla seriale di debug tramite la DebugString()*/
		if (Timer_printf_debug >= 10)
		{
			Timer_printf_debug = 1;
			DebugString_TEST();
		}
	 }
	 /*faccio lo start della conversione sul canale AD1 ogni 10 msec
	  * per avere le pressioni a 100 HZ (timerCounterADC1 si incremente ogni msec*/
	 if (timerCounterADC1 >=10)
	 {
		//EnterCritical();
		Phase = 7;
		timerCounterADC1 = 0;
		//ExitCritical();
		AD1_Start();
	 }

     /********************************/
     /*          FRONTAL COVER       */
     /********************************/
	 Manage_Frontal_Cover();

	 /********************************/
	 /*          HOOK SENSORS        */
	 /********************************/
	 Manage_Hook_Sensors();
	 verificaTempPlate();

     /*funzione che aggiorna lo stato del sesnore si aria filtro (digitale)*/
     Manage_Air_Sensor_1();

  }


  /******************************************************
	History protective

	Versione 1.0 19/11/2018

	- bug segnalato : durante fase priming , com T impostata di 37 gradi ,  la protective blocca i motori
	- il debug rivela che il blocco è causato da un blocco motori
	Causa
	- Quando arriva un messaggio di errore da control , la funzione IsVerifyRequired() torna false , e
	  i dati in arrivo da can non vengono processsati. Peroò la gestione degli errori a tempo ,  in VerifyRxCan.c , static void ManageVerificatorAlarms100ms() ,
	  restava in esecuzione . Se si aveva un errore di velocità motori prima dell'arrivo del cod di errore , restava attivo per piu di 7 secondi dato
	  che non veniva aggiornato il flag ErrorConditionPending.  Adesso La ManagerVerificatorAlarms100ms non esegue se la IsVerifyRequired è false.
	- Un altro punto che bloccava le pompe è presente nella IncomingAlarmsManager. Qui , in caso di errore riportato dalla control , veniva forzatamente tolto l'enable
	  alle pompe e alle pinch.
	  		if( IsAlarmTemperatureRelated( controlBoard_AlarmCode ) ){
				DisablePinchNPumps();
				Enable_Heater(false);
	  questo è stato tolto .
	- quando TimeSecsFromControlAlarm andava in timeout , veniva verificato che effettivamente sia le pinch che i motori fossero in sicurezza.
	  la funzione PinchesAreInSafetyMode() era errata dato che si aspettava tutte le pinch aperte verso sinistra , mentre 2 sono aperte a dx e 1 a sx

	Versione 1.0.001 20/11/2018

	modifiche inserite
	Sul missmatch dei sensori di pressione, spostiamo i delta che identificano il missmatch a 20 mmHg; aggiungiamo che questa differenza
	deve restare attiva per almeno 5 secondi.
	L'allarme di temperatura di venosa e arteriosa  ( non ricircolo ) , va bene a 40+1 come adesso ma deve essere attivato solo se almeno
	una tra le pinch arteriuosa e venosa risulta aperta sull'organo (ovvero a sinistra) E SE SIAMO IN TRATTAMENTO --> Stato = ... STATE_TREATMENT_KIDNEY_1, = 9
	Per la piastra --> Allarme SEMPRE se supera 85 gradi.
	Aggiornati anche i valori limite per la pressione
	Temp bassa piastra per allarme , -20

	Versione 1.0.002 20/12/2018
	- Correzione void NotifyCanOnline(bool Online) nel file VerificatorRxCanValues.c , resettava sempre l'errore
	- Correzione in Comm_Sbc.c , non inviava il valore della pressione venosa e arteriosa a SBC
	- Modifica nella gestione dell'errore pinch non corrispondenti:

		 Nel messaggio can inviato da Control a Protective saranno presenti 2 informazioni per ogni PINCH:
		 - La posizione reale della pinch : POS_RE_CONTROL ( LS nibble )
		 - La posizione comandata dalla Control tramite il modbus :POS_CMD  ( MS nibble)

		 La posizione letta dalla protective è POS_RE_PROTE ( letta dai sensori di hall )

		Algoritmo sulla protective:
		 -(PinchVerifierStat = 1)  ENABLE_PINCH = ON :   POS_RE e POS_CMD devono corrispondere tra loro e devono corrispondere con il valore rilevato dalla protective stessa mediante i sensori di Hall.
		   Se non corrispondono --> Allarme

		 -(PinchVerifierStat = 2) ENABLE_PINCH = OFF :   POS_RE_CONTROL e POS_RE_PROTE devono corrispondere . Non valutare POS_CMD perchè sopravanzato da ENABLE= OFF.
		 - se dopo ENABLE_PINC = OFF , torna ENABLE_PINCH = ON vai a (PinchVerifierStat = 3)

		 -(PinchVerifierStat = 3)
		 COntinua a comportarsi come quando ENABLE_PINCH = OFF fino a quando si verifica un nuovo comando che riporta allo stato (PinchVerifierStat = 1)

		 tutto questo per ogni pinch


  */


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
