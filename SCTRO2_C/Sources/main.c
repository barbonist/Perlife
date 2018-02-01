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
#include "RTS_MOTOR.h"
#include "BitIoLdd5.h"
#include "EN_CLAMP_CONTROL.h"
#include "BitIoLdd6.h"
#include "EN_MOTOR_CONTROL.h"
#include "BitIoLdd7.h"
#include "EN_24_M_C.h"
#include "BitIoLdd8.h"
#include "COVER_M1.h"
#include "BitIoLdd9.h"
#include "COVER_M2.h"
#include "BitIoLdd10.h"
#include "COVER_M3.h"
#include "BitIoLdd12.h"
#include "COVER_M4.h"
#include "BitIoLdd11.h"
#include "COVER_M5.h"
#include "BitIoLdd13.h"
#include "PC_DEBUG_COMM.h"
#include "ASerialLdd2.h"
#include "AD1.h"
#include "AdcLdd1.h"
#include "VOLTAGE_B_CHK.h"
#include "BitIoLdd36.h"
#include "VOLTAGE_M_CHK.h"
#include "BitIoLdd37.h"
#include "PELTIER_COMM.h"
#include "PELTIER2_COMM.h"
#include "ASerialLdd3.h"
#include "PELTIER2_COMM.h"
#include "ASerialLdd6.h"
#include "EN_P_2_C.h"
#include "BitIoLdd14.h"
#include "EN_P_1_C.h"
#include "BitIoLdd15.h"
#include "EN_FAN_1.h"
#include "DacLdd1.h"
#include "EN_FAN_2.h"
#include "DacLdd2.h"
#include "FLOWSENS_COMM.h"
#include "ASerialLdd4.h"
#include "SBC_COMM.h"
#include "ASerialLdd5.h"
#include "AD0.h"
#include "AdcLdd2.h"
#include "FLOWSENS_RE.h"
#include "BitIoLdd3.h"
#include "FLOWSENS_DE.h"
#include "BitIoLdd4.h"
#include "IR_TM_COMM.h"
#include "IntI2cLdd1.h"
#include "TU1.h"
#include "FLASH1.h"
#include "LAMP_HIGH.h"
#include "BitIoLdd33.h"
#include "LAMP_MEDIUM.h"
#include "BitIoLdd34.h"
#include "LAMP_LOW.h"
#include "BitIoLdd35.h"
#include "CAN_C_P.h"
#include "AIR_SENSOR.h"
#include "BitsIoLdd1.h"
#include "AIR_T_1.h"

#include "BitIoLdd38.h"
#include "AIR_T_2.h"

#include "BitIoLdd39.h"
#include "AIR_T_3.h"

#include "BitIoLdd40.h"
#include "COMP_PWM.h"
#include "BitIoLdd41.h"
#include "HEAT_ON_C.h"
#include "BitIoLdd42.h"
#include "TI1.h"
#include "TimerIntLdd1.h"
#include "TU2.h"
#include "System_Tick.h"
#include "TimerIntLdd2.h"
#include "D_7S_DP.h"
#include "BitIoLdd22.h"
#include "D_7S_G.h"
#include "BitIoLdd23.h"
#include "D_7S_F.h"
#include "BitIoLdd24.h"
#include "D_7S_E.h"
#include "BitIoLdd25.h"
#include "D_7S_D.h"
#include "BitIoLdd26.h"
#include "D_7S_C.h"
#include "BitIoLdd27.h"
#include "D_7S_B.h"
#include "BitIoLdd28.h"
#include "D_7S_A.h"
#include "BitIoLdd29.h"
#include "BUZZER_LOW_C.h"
#include "BitIoLdd30.h"
#include "BUZZER_MEDIUM_C.h"
#include "BitIoLdd31.h"
#include "BUZZER_HIGH_C.h"
#include "BitIoLdd32.h"
#include "BUBBLE_KEYBOARD_BUTTON1.h"
#include "BitIoLdd16.h"
#include "BUBBLE_KEYBOARD_BUTTON2.h"
#include "BitIoLdd17.h"
#include "BUBBLE_KEYBOARD_BUTTON3.h"
#include "BitIoLdd18.h"
#include "BUBBLE_KEYBOARD_BUTTON4.h"
#include "BitIoLdd19.h"
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
#include "Debug_Routine.h"


extern unsigned char PidFirstTime[4];
extern float pressSample1_Ven;
extern float pressSample2_Ven;
extern bool WriteActive;
extern unsigned char Released1;
extern unsigned char Released2;
extern unsigned char Released3;

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
void TestPump(unsigned char Adr); //only for test
void TestPinch(void);
void GenerateSBCComm(void);

int FreeRunCnt10msecOld;

int timerCounterModBusOld = 0;
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{

  /* Write your local variable definition here */
  bool MOTORE_ACCESO = FALSE;
  bool MOTORE_ACCESO_2 = FALSE;
  bool Status_Board;
  THERAPY_TYPE TreatType = KidneyTreat;
  pollingDataFromSBC = 0;
  pollingDataToSBC = 0;
  codeDBG = 0;
  subcodeDBG = 0;
  Released1 = 0;
  Released2 = 0;
  Released3 = 0;



 #ifdef	SERVICE_ACTIVE_TOGETHER_THERAPY
  Status_Board = SERVICE;
 #else
  Status_Board = TREAT;
 #endif

  unsigned char PINCH_POSITION 				= 0;
  unsigned char funcCode 					= 0x03;
  word			readAddrStartReadRevision	= 0x0000;
  unsigned char	numberOfAddressReadRevision	= 0x20; //allo start leggo 32 registri

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /**/
  ADC0_Calibration();
  ADC1_Calibration();

  Dip_Switch_ADC_Init();
  Voltage_Peltier_ADC_Init();
  PR_Sens_ADC_Init();

  /* Write your code here */

  LevelBuzzer = SILENT;
  initAllGuard();
  initAllState();
  initGUIButton();
  initSetParamFromGUI();
  initSetParamInSourceCode();

  alarmConInit();
  initPerfusionParam();
  initPurifParam();

  peltierAssInit();

  initUFlowSensor();
  initTempSensIR();

  Air_1_Status = AIR;
  slvAddr = FIRST_ACTUATOR;		//metto come indirizzo degli attuatori da leggere il primo

  timerCounterMState = 0;
  timerCounterUFlowSensor = 0;
  timerCounterCheckModBus = 0;
  timerCounterCheckTempIRSens = 0;
  timerCounterLedBoard = 0;
  Prescaler_Tick_Timer = 0;
  Prescaler_Tick_TEST=0;
  FreeRunCnt10msec = 0;
  Service = FALSE;

  iFlag_actuatorCheck = IFLAG_IDLE;
  iFlag_modbusDataStorage = FALSE;

  CHANGE_ADDRESS_IR_SENS = FALSE;

  OK_START = FALSE;
  ON_NACK_IR_TM = FALSE;

  modBusPmpInit(TreatType);
  modBusPinchInit();
  modbusDataInit();


  SBC_COMM_Enable();
  initCommSBC();
    /**/
  /*abilito l'RTS per la trasmissionme verso i motori;
   * la lascio sempre attiva tanto ogni 50 msec al massimo
   *  sarò sempre io a fare la trasmisisone interrogando i driver*/

  RTS_MOTOR_SetVal();

  static int index = 0;
  //ptrMsgSbcRx = &msg_sbc_rx[0];
  ptrMsgSbcRx = &sbc_rx_data[0];
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

  //int timerCounterModBusOld = 0;

  /*faccio lo start dei canali AD per far scattare l'interrupt
   * dentro l'interrupt farò lo stop e poi lo start lo rifarò
   * nella Manange_ADC0 e Manange_ADC1 oppure nel main loop sotto timer*/
  AD0_Start();
  AD1_Start();

  /*abilito la 24V sui motori*/
  EN_24_M_C_Management(ENABLE);
  /*do l'enable sui motori*/
  EN_Clamp_Control(ENABLE);
  EN_Motor_Control(ENABLE);
  IR_TM_COMM_Enable();

  /*abilitp la 24 V sui driver delle celle di peltier*/
  EN_P_1_C_SetVal();
  EN_P_2_C_SetVal();


  /*attendo 5 s prima di entrare nell main loop
   * per dare il tempo ai sensori di alimentarsi e
   * ativarsi e soprattutto ai driver delle pompe e
   * delle pinch di essere pronti dopo avergli dato
   * l'alimentazione; la variabile timerCounterCheckModBus
   * si incrementa di una unità nella TU1_OnCounterRestart
   * interrupt che scatta ogni 50 ms, quindi quando arriva
   * a 100 avrò contato 5000 msec ovvero 5 sec*/
  while (!OK_START)
  {
	  if (timerCounterCheckModBus >= 100)
	  {
		  OK_START = TRUE;
		  timerCounterCheckModBus = 0;
	  }
  }

//  QUESTO CODICE POTREBBE ESSERE NECESSARIO SCOMMENTARLO SE NON FOSSE SUFFICIENTE LA
//  CORRENTE DI PILOTAGGIO DELLE POMPE
  timerCounterCheckModBus = 0;
  setPumpCurrentValue((unsigned char)2, (int)31);
  while (timerCounterCheckModBus < 1);
  timerCounterCheckModBus = 0;
  setPumpCurrentValue((unsigned char)3, (int)31);
  while (timerCounterCheckModBus < 1);
  timerCounterCheckModBus = 0;
  setPumpCurrentValue((unsigned char)4, (int)31);    // imposto la CURRENT LEVEL a 24
  while (timerCounterCheckModBus < 1);
  timerCounterCheckModBus = 0;
  setPumpCurrentValue((unsigned char)5, (int)31);
  while (timerCounterCheckModBus < 1);

  setPumpAccelerationValue((unsigned char)4, (int)30);    // imposto la CURRENT LEVEL a 24
  while (timerCounterCheckModBus < 1);
  timerCounterCheckModBus = 0;
  setPumpAccelerationValue((unsigned char)5, (int)30);
  while (timerCounterCheckModBus < 1);


  /*prima di entrare nel loop infinito chiedo i dati di targa agli attuatori
   * devo ricevere i dati di tutte le pompe in massimo un secondo;
   * quando avrò tutti gli attuatori sarà da rimettere TOT_NUMBER_OF_ACTAUTOR al posto di 0x03*/
  slvAddr = FIRST_ACTUATOR;

  while (slvAddr <= LAST_ACTUATOR && timerCounterCheckModBus <= 20)
  {
	  /*se ho ricevuto opp sono in IDLE quindi non ho ancora inviato il primo messaggio, invio il messaggio*/
	  if (iFlag_actuatorCheck == IFLAG_COMMAND_RECEIVED || iFlag_actuatorCheck == IFLAG_IDLE)
	  {
		  iFlag_actuatorCheck = IFLAG_COMMAND_SENT;

		  /*L'indirizzo slvAddr = 6 non è usato*/
		  if (slvAddr == 6)
				slvAddr= 7;

		/*chiamo la funzione col corretto number of address dipendentemente dall'attuatore (pump/pinch)*/
		if (slvAddr <= LAST_PUMP)
			/*funzione che mi legge lo stato delle pompe*/
			Check_Actuator_Status (slvAddr,funcCode,readAddrStartReadRevision,numberOfAddressReadRevision);
		else
			/*funzione che mi legge lo stato delle pinch*/
			Check_Actuator_Status (slvAddr,funcCode,readAddrStartReadRevision,numberOfAddressReadRevision);
	  }
	  /*Se ho ricevuto vado a salvarmi i dati nella matrice 'modbusData'*/
	  if (iFlag_actuatorCheck == IFLAG_COMMAND_RECEIVED)
	  {
		  StorageModbusDataInit();
		  /*dopo che ho memorizzato la risposta posso passare al prossimo attuatore*/

	        slvAddr++;
	  }
  }

  timerCounterCheckModBus = 0; 	//resetto il timer di check sul modbus
  slvAddr = FIRST_ACTUATOR;		//rimetto come indirizzo da leggere il primo
  timerCounterADC = 0;


  // al reset metto tutti irami delle pich chiusi
  setPinchPosValue (PINCH_2WPVF, MODBUS_PINCH_POS_CLOSED);
  while (timerCounterCheckModBus < 1);
  timerCounterCheckModBus = 0;
  setPinchPosValue (PINCH_2WPVA, MODBUS_PINCH_POS_CLOSED);
  while (timerCounterCheckModBus < 1);
   timerCounterCheckModBus = 0;
  setPinchPosValue (PINCH_2WPVV, MODBUS_PINCH_POS_CLOSED);
  while (timerCounterCheckModBus < 1);
   timerCounterCheckModBus = 0;

//   setPinchPositionHighLevel(BOTTOM_PINCH_ID, (int)MODBUS_PINCH_POS_CLOSED);
//   setPinchPositionHighLevel(LEFT_PINCH_ID, (int)MODBUS_PINCH_POS_CLOSED);
//   setPinchPositionHighLevel(RIGHT_PINCH_ID, (int)MODBUS_PINCH_POS_CLOSED);

//#ifdef TUNING_PID_VEN_LIVER
//   // attivo il pid sull'ossigenazione e perfusione venosa per provare i
//   // coefficienti
//	setPumpPressLoop(1, PRESS_LOOP_ON);
//	pressSample1_Ven = 0;
//	pressSample2_Ven = 0;
//	FreeRunCnt10msecOld = 0;
//#endif


  /**********MAIN LOOP START************/
  for(;;) {


#ifdef SERVICE_ACTIVE_TOGETHER_THERAPY
	  	  if (Service)
	  		 Service_SBC();

	         else
	         {
				 /* sbc comm - start */
				 pollingSBCCommTreat();
				 pollingDataToSBCTreat();
	         }
#else

		 /* sbc comm - start */
		 pollingSBCCommTreat();
		 pollingDataToSBCTreat();
#endif
	         /* sbc comm - end */

			 /*controllo lo stato del sensore d'aria
			  * e aggiorno la variabile globale
			  * Air_1_Status */

			 Manage_Air_Sensor_1();


	         /*****MACHINE STATE UPDATE START****/
        	 if(timerCounterMState >= 1)
	         {
	        	timerCounterMState = 0;

	        	computeMachineStateGuard();

	        	processMachineState();

	        	alarmEngineAlways();
		        GenerateSBCComm();


	         }

	         if(ReadKey1()) // per debug con la tastiera a bolle
	        	 Released1 = 1;
	         if(ReadKey2()) // per debug con la tastiera a bolle
	        	 Released2 = 1;
	         if(ReadKey3()) // per debug con la tastiera a bolle
	        	 Released3 = 1;

	         /****MACHINE STATE UPDATE END****/

	         /*******************************/
	         /*UFLOW SENSOR                 */
	         Manage_UFlow_Sens();

	         /*TestPump(4); // 2..5 usata per provare le pompe con la tastiera a bolle
	         if( (pumpPerist[0].reqState == REQ_STATE_OFF) && (pumpPerist[0].reqType == REQ_TYPE_IDLE) &&
	             (pumpPerist[1].reqState == REQ_STATE_OFF) && (pumpPerist[1].reqType == REQ_TYPE_IDLE) &&
				 (pumpPerist[2].reqState == REQ_STATE_OFF) && (pumpPerist[2].reqType == REQ_TYPE_IDLE) &&
				 (pumpPerist[3].reqState == REQ_STATE_OFF) && (pumpPerist[3].reqType == REQ_TYPE_IDLE))
	         {
	        	 TestPinch();     // BOTTOM_PINCH_ID = 7, LEFT_PINCH_ID = 8, RIGHT_PINCH_ID = 9
	         }
	         */
			Buzzer_Management(LevelBuzzer);

			if(timerCounterPeltier >= 2)
			{
	        	 timerCounterPeltier = 0;

	        	 if(peltierCell.readAlwaysEnable == 0)
	        		 alwaysPeltierActuator();

	         }
	         /********************************/
	         /*             I2C	             */
	         /********************************/
	         Manage_IR_Sens_Temp();


	         /********************************/
	         /*           DEBUG LED          *
	         /********************************/

	         Manage_Debug_led(Status_Board);

	         /********************************/
	         /*           DEBUG LED  END     */
	         /********************************/

	         /********************************/
			 /*      CHECK COVER PUMP        */
			 /********************************/
	         /*a giro di programma controllo lo stato dei cover delle pompe
	          * Poi nella gestione degli allarmi, in funzione dello stato
	          * in cui sono, se un cover è aperto, fermo tutte le pompe, metto le pinch in sicurezza
	          * (vaschetta quelle in alto ovvero posizione a destra)
	          * e SBC metterà a video 'cover pump open'. quando tutti
	          * i coperchi saranno chiusi (la funzione restituisce 4)
	          * tolgo la condizione di allarme, SBC metterà a video il retry
	          * e solo allora potrò far ripartire le pompe e riprendere con la macchina a stati*/
	         CheckCoverPump();

	         /********************************/
	         /*             ADC	             */
	         /********************************/

			 /*funzioni per leggere i canali AD*/
			 Manange_ADC0();
			 Manange_ADC1();
			 /*END funzioni per leggere i canali AD*/


		     /*faccio lo start della cionversione sui canali AD ogni 50 msec*/
			 if (timerCounterADC >=1)
			 {
				timerCounterADC = 0;
				AD0_Start();
				AD1_Start();
			 }

	         /********************************/
	         /*				ADC				 */
	         /********************************/

	         /*********PUMP*********/
	         /*la gestione del ModBus probabilmente sarà da rifare seguendo la scia di quanto fatto inn Debug*/

	         alwaysModBusActuator();

	         // si possono verificare delle chiamate alla setPumpSpeedValueHighLevel quando ci sono
	         // delle scritture in corso. Per evitare di perderle e per evitare di corrompere le flag devo
	         // chiamare questa funzione
	         UpdatePumpSpeed();

	         if (!iflag_sbc_rx && !WriteActive);
	        	Manage_and_Storage_ModBus_Actuator_Data();


	         /*********PUMP*********/

  }
  /**********MAIN LOOP END**************/


  /*********MAIN LOOP TEST END **************/

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
