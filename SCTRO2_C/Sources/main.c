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
#include "Bit3.h"
#include "BitIoLdd1.h"
#include "Bit4.h"
#include "BitIoLdd2.h"
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
#include "Bit1.h"
#include "BitIoLdd20.h"
#include "Bit2.h"
#include "BitIoLdd21.h"
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


 extern unsigned char Released1;
 extern unsigned char Released2;

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
void TestPump(unsigned char Adr); //only for test
void TestPinch(void);
void GenerateSBCComm(void);

int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{

  /* Write your local variable definition here */
  bool MOTORE_ACCESO = FALSE;
  bool MOTORE_ACCESO_2 = FALSE;
  bool Status_Board;

 #ifdef	DEBUG_COMM_SBC
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

  alarmConInit();
  initPerfusionParam();
  initPurifParam();

  peltierAssInit();

  initAdcParam();

  initUFlowSensor();
  initTempSensIR();

  Air_1_Status = AIR;
  slvAddr = FIRST_ACTUATOR;		//metto come indirizzo degli attuatori da leggere il primo

  timerCounterMState = 0;
  timerCounterUFlowSensor = 0;
  timerCounterCheckModBus = 0;
  timerCounterCheckTempIRSens = 0;
  timerCounterLedBoard = 0;

  iFlag_actuatorCheck = IFLAG_IDLE;
  iFlag_modbusDataStorage = FALSE;

  CHANGE_ADDRESS_IR_SENS = FALSE;

  OK_START = FALSE;
  ON_NACK_IR_TM = FALSE;

  #ifdef	DEBUG_I2C_TEMP_SENS
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
 #endif

  modBusPmpInit();
  modBusPinchInit();
  modbusDataInit();


  SBC_COMM_Enable();
    /**/
  Bit3_SetVal(); /* enable motore */
  Bit4_SetVal();

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

  int timerCounterModBusOld = 0;
  /* For example: for(;;) { } */


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

  /*prima di entrare nel loop infinito chiedo i dati di targa agli attuatori
   * devo ricevere i dati di tutte le pompe in massimo un secondo;
   * quando avrò tutti gli attuatori sarà da rimettere TOT_NUMBER_OF_ACTAUTOR al posto di 0x03*/
  while (slvAddr <= 0x03 /*TOT_NUMBER_OF_ACTAUTOR*/ && timerCounterCheckModBus <= 20)
  {
	  /*se ho ricevuto opp sono in IDLE quindi non ho ancora inviato il primo messaggio, invio il messaggio*/
	  if (iFlag_actuatorCheck == IFLAG_COMMAND_RECEIVED || iFlag_actuatorCheck == IFLAG_IDLE)
	  {
		  iFlag_actuatorCheck = IFLAG_COMMAND_SENT;
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
  setPinchPosValue (BOTTOM_PINCH_ID, MODBUS_PINCH_POS_CLOSED);
  while (timerCounterCheckModBus < 1);
  timerCounterCheckModBus = 0;
  setPinchPosValue (LEFT_PINCH_ID, MODBUS_PINCH_POS_CLOSED);
  while (timerCounterCheckModBus < 1);
   timerCounterCheckModBus = 0;
  setPinchPosValue (RIGHT_PINCH_ID, MODBUS_PINCH_POS_CLOSED);
  while (timerCounterCheckModBus < 1);
   timerCounterCheckModBus = 0;


//   setPinchPositionHighLevel(BOTTOM_PINCH_ID, (int)MODBUS_PINCH_POS_CLOSED);
//   setPinchPositionHighLevel(LEFT_PINCH_ID, (int)MODBUS_PINCH_POS_CLOSED);
//   setPinchPositionHighLevel(RIGHT_PINCH_ID, (int)MODBUS_PINCH_POS_CLOSED);

  /**********MAIN LOOP START************/
  for(;;) {
	  	  	 /***********DEBUG START*************/
			 #ifdef DEBUG_ENABLE
//	  	  	  	  index++;
//	         	  if(index%32000 == 0)
//	         		 FLOWSENS_RE_NegVal();
//	         	  else if(index%65000 == 0)
//	         	  {
//	         		 FLOWSENS_DE_NegVal();
//	         		 index = 0;
//	         	  }

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


					 FLOWSENS_RE_SetVal();
					 FLOWSENS_DE_SetVal();
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
	         		 	   //IR_TM_COMM_Disable();
	         		 	    IR_TM_COMM_Enable();
	         		 	    ptrData = buildCmdReadTempSensIR(0x01, (EEPROM_ACCESS_COMMAND | SD_TOMIN_E2_ADDRESS), 0);
	         		 	    //ptrData = buildCmdWriteTempSensIR(0x5A, (EEPROM_ACCESS_COMMAND | SD_TOMAX_E2_ADDRESS), 0xBE01);

	         		 	    iflag_sensTempIR = IFLAG_SENS_TEMPIR_WAIT;
	         		 	  }

	         		if(iflag_sensTempIR == IFLAG_SENS_TEMPIR_TX)
	         		{
	         		//IR_TM_COMM_SelectSlave(0x5A);
	         			err = IR_TM_COMM_RecvBlock(ptrData, 3, &ret);
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
	         			IR_TM_COMM_Disable();
	         			//IR_TM_COMM_Enable();
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
	         			IR_TM_COMM_Enable();
	         			ptrDatawR = buildCmdWriteTempSensIR(0x01, (EEPROM_ACCESS_COMMAND | SD_TOMIN_E2_ADDRESS), 0x62E5);
	         			iflag_sensTempIR = IFLAG_SENS_TEMPIR_WAIT;
	         		}


	         		if(iflag_sensTempIR == IFLAG_SENS_TEMPIR_TX)
	         		{
	         			stateSensTempIR = 5;
	         			IR_TM_COMM_Disable();
	         			//IR_TM_COMM_Enable();
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
	         		 //IR_TM_COMM_Disable();
	         		 //IR_TM_COMM_Enable();
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
	         		//IR_TM_COMM_SelectSlave(0x5A);
	         		//err = IR_TM_COMM_RecvBlock(ptrData, 3, &ret);
	         		//iflag_sensTempIR = IFLAG_SENS_TEMPIR_WAIT;
	         		//iflag_sensTempIR = IFLAG_IDLE;
	         	 //}

	         	#endif

				#ifdef	DEBUG_COMM_SBC
	         	//SBC_COMM_Enable();
	         	testCOMMSbcDebug();

	         	/*da valutare se sreve ancora o può essere sostituita dalla mie Manange_ADC0() e Manange_ADC1()*/
	         	alwaysAdcParam();

	         	/*Gestisco i sensori di Temp IR la funzione viene chiamata a giro
	         	 * di programa ma al suo interno i sensori saranno interrogati
	         	 * ogni 200 msec quindi il giro completo sei tre sensori sarà fatto ogni 600 msec*/
	         	Manage_IR_Sens_Temp();

	         	/*controllo lo stato del sensore d'aria	         	 * e aggiorno la variabile globale
	         	 * Air_1_Status */
	         	Manage_Air_Sensor_1();

		        /*funzioni per leggere i canali AD*/
		        Manange_ADC0();
		        Manange_ADC1();
		        /*END funzioni per leggere i canali AD*/
		        /*converte i valori ADC in mmHg dei sensori di pressione*/
		        Coversion_From_ADC_To_mmHg_Pressure_Sensor();
		        /*converte i valori ADC in volt per le tensioni*/
		        Coversion_From_ADC_To_Voltage();

		        /*faccio lo start per riattivare la conversione sui due canali AD ogni 50 msec*/
		        if (timerCounterADC >= 1)
		        {
		        	timerCounterADC = 0;
		        	AD0_Start();
		        	AD1_Start();
		        }

		        Buzzer_Management(LevelBuzzer);
		         /*******************************/
		         /********UFLOW SENSOR***********/

		        Manage_UFlow_Sens();

		        /*ATTIVA IL MOTORE CON LA TASTIERTA A BOLLE---> SOLO PER TEST*/
//				if (Bubble_Keyboard_GetVal(BUTTON_1) && !MOTORE_ACCESO)
//				{
//					  /*accendo il motore*/
//					  MOTORE_ACCESO = TRUE;
//					 // EN_Motor_Control(ENABLE);
//					  /*faccio partire la pompa con 2 come indirizzo
//					   * corrispondente a 0 come rotary select e 10 RPM come speed*/
//					  setPumpSpeedValue(3,8000);
//				}
//				if (Bubble_Keyboard_GetVal(BUTTON_2) && MOTORE_ACCESO)
//				{
//					  /*spengo il motore*/
//					  MOTORE_ACCESO = FALSE;
//					 // EN_Motor_Control(DISABLE);
//					  /*Fermo la pompa con 2 come indirizzo
//					   * corrispondente a 0 come rotary select e 0 RPM come speed*/
//					  setPumpSpeedValue(3,0);
//				}

//		        if(timerCounterUFlowSensor >= 2)
//		        {
//	         		timerCounterUFlowSensor = 0;
//
//	         		alwaysUFlowSensor();
//	         	}
		        /*******************************/
		        /******UFLOW SENSOR END*********/

		        /*******************************/
		        /***********PELTIER*************/

				if(timerCounterPeltier >= 2){
		        	 timerCounterPeltier = 0;

		        	 if(peltierCell.readAlwaysEnable == 0)
		        		 alwaysPeltierActuator();

		         }
			    /*********PELTIER END***********/
				/*******************************/


	         	Manage_Debug_led(Status_Board);

	         	Manage_and_Storage_ModBus_Actuator_Data();

				#endif

		 	 #endif
	         /***********DEBUG END*************/

			 #ifdef DEBUG_TREATMENT
	         /* sbc comm - start */
	         initCommSBC();
	         pollingSBCCommTreat();
	         pollingDataToSBCTreat();
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

	         /****MACHINE STATE UPDATE END****/

	         /*******************************/
	         /*UFLOW SENSOR                 */
//	         if(timerCounterUFlowSensor >= 2){
//	        	 timerCounterUFlowSensor = 0;
//
//	        	 alwaysUFlowSensor();
//
//	         }
	         Manage_UFlow_Sens();

	         /*
	         TestPump(2); // 2..5 usata per provare le pompe con la tastiera a bolle
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
	        // alwaysIRTempSensRead();
	         Manage_IR_Sens_Temp();

	         /*******************************/
	         /*UFLOW SENSOR                 */


	         /********************************/
	         /*           DEBUG LED          */
	         /********************************/
	         Manage_Debug_led(Status_Board);
	         /********************************/
	         /*           DEBUG LED  END     */
	         /********************************/

	         /********************************/
	         /*             ADC	             */
	         /********************************/
			 /*funzioni per leggere i canali AD*/
			 Manange_ADC0();
			 Manange_ADC1();
			 /*END funzioni per leggere i canali AD*/
		 	 /*converte i valori ADC in mmHg dei sensori di pressione*/
			 Coversion_From_ADC_To_mmHg_Pressure_Sensor();
		     /*converte i valori ADC in volt per le tensioni*/
		     Coversion_From_ADC_To_Voltage();

		     /*faccio lo start della cionversione sui canali AD ogni 50 msec*/
			 if (timerCounterADC >=1)
			 {
			 /*da valutare se sreve ancora o può essere sostituita dalla mie Manange_ADC0() e Manange_ADC1()*/
	         alwaysAdcParam();
				timerCounterADC = 0;
				AD0_Start();
				AD1_Start();
			 }

	         /********************************/
	         /*				ADC				 */
	         /********************************/

	         /*********PUMP*********/
	         /*la gestione del ModBus probabilmente sarà da rifare seguendo la scia di quanto fatto inn Debug*/
	         if(timerCounterModBus != timerCounterModBusOld)
	         {
	        	 timerCounterModBusOld = timerCounterModBus;
	         }

	         // il controllo sul time slot lo fa al suo interno
        	 alwaysModBusActuator();
	         Manage_and_Storage_ModBus_Actuator_Data();
	         /*********PUMP*********/
			 #endif

  }
  /**********MAIN LOOP END**************/

  /*********MAIN LOOP TEST**************/

  /*MAIN loop per la gestione della pinch tramite tastiera a bolle*/
  EN_24_M_C_Management(ENABLE);
  EN_Clamp_Control(ENABLE);
  for(;;)
  {
	  if (Bubble_Keyboard_GetVal(BUTTON_1) && PINCH_POSITION != MODBUS_PINCH_POS_CLOSED)
	  {
		  PINCH_POSITION = MODBUS_PINCH_POS_CLOSED;
		  /*Pinch con rottay select = 5 e position = MODBUS_PINCH_POS_CLOSED*/
		  setPinchPosValue (7,MODBUS_PINCH_POS_CLOSED);
	  }
	  if (Bubble_Keyboard_GetVal(BUTTON_2) && PINCH_POSITION != MODBUS_PINCH_RIGHT_OPEN)
	  {
		  PINCH_POSITION = MODBUS_PINCH_RIGHT_OPEN;
		  /*Pinch con rottay select = 5 e position = MODBUS_PINCH_POS_CLOSED*/
		  setPinchPosValue (7,MODBUS_PINCH_RIGHT_OPEN);
	  }
	  if (Bubble_Keyboard_GetVal(BUTTON_3) && PINCH_POSITION != MODBUS_PINCH_LEFT_OPEN)
	  {
		  PINCH_POSITION = MODBUS_PINCH_LEFT_OPEN;
		  /*Pinch con rottay select = 5 e position = MODBUS_PINCH_POS_CLOSED*/
		  setPinchPosValue (7,MODBUS_PINCH_LEFT_OPEN);
	  }

  }

  /*MAIN loop per la gestione del motore pinch tramite tastiera a bolle*/
  EN_24_M_C_Management(ENABLE);
  for(;;)
  {
	  if (Bubble_Keyboard_GetVal(BUTTON_1) && !MOTORE_ACCESO)
	  {
		  /*accendo il motore*/
		  MOTORE_ACCESO = TRUE;
		  EN_Motor_Control(ENABLE);
		  /*faccio partire la pompa con 2 come indirizzo
		   * corrispondente a 0 come rotary select e 10 RPM come speed*/
		  setPumpSpeedValue(2,1000);
	  }
	  if (Bubble_Keyboard_GetVal(BUTTON_2) && MOTORE_ACCESO)
	  {
		  /*spengo il motore*/
		  MOTORE_ACCESO = FALSE;
		  EN_Motor_Control(DISABLE);
		  /*Fermo la pompa con 2 come indirizzo
		   * corrispondente a 0 come rotary select e 0 RPM come speed*/
		  setPumpSpeedValue(2,0);
	  }

  }

  /*MAIN loop per la gestione di due motori tramite tastiera a bolle*/
  EN_24_M_C_Management(ENABLE);
  EN_Motor_Control(ENABLE);
  EN_Clamp_Control(ENABLE);
  for(;;)
  {
	  if (Bubble_Keyboard_GetVal(BUTTON_1) && Bubble_Keyboard_GetVal(BUTTON_2))
	  {
		  /*do l'enable ai canali pinch e motor*/
		  EN_Motor_Control(ENABLE);
		  EN_Clamp_Control(ENABLE);
	  }
	  else if (Bubble_Keyboard_GetVal(BUTTON_3) && Bubble_Keyboard_GetVal(BUTTON_4))
	  {
		  /*disabilito i canali pinch e motor*/
		  EN_Motor_Control(DISABLE);
		  EN_Clamp_Control(DISABLE);
	  }
	  else if (Bubble_Keyboard_GetVal(BUTTON_1) && !MOTORE_ACCESO)
	  {
		  /*accendo il motore*/
		  MOTORE_ACCESO = TRUE;
		  /*faccio partire la pompa con 2 come indirizzo
		   * corrispondente a 0 come rotary select e 20 RPM come speed*/
		  setPumpSpeedValue(2,5000);
	  }
	  else if (Bubble_Keyboard_GetVal(BUTTON_2) && MOTORE_ACCESO)
	  {
		  /*spengo il motore*/
		  MOTORE_ACCESO = FALSE;
		  /*Fermo la pompa con 2 come indirizzo
		   * corrispondente a 0 come rotary select e 0 RPM come speed*/
		  setPumpSpeedValue(2,0);
	  }
	  else if (Bubble_Keyboard_GetVal(BUTTON_3) && !MOTORE_ACCESO_2)
	  {
		  /*accendo il motore*/
		  MOTORE_ACCESO_2 = TRUE;
		  /*faccio partire la pompa con 3 come indirizzo
		   * corrispondente a 1 come rotary select e 50 RPM come speed*/
		  setPumpSpeedValue(3,2000);
	  }
	  else if (Bubble_Keyboard_GetVal(BUTTON_4) && MOTORE_ACCESO_2)
	  {
		  /*spengo il motore*/
		  MOTORE_ACCESO_2 = FALSE;
		  /*Fermo la pompa con 3 come indirizzo
		   * corrispondente a 1 come rotary select e 0 RPM come speed*/
		  setPumpSpeedValue(3,0);
	  }
  }
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
