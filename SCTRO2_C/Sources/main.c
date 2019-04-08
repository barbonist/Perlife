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
#include "string.h"
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
#include "EMERGENCY_BUTTON.h"

#include "BitIoLdd9.h"
#include "FRONTAL_COVER_1.h"

#include "BitIoLdd10.h"
#include "FRONTAL_COVER_2.h"

#include "BitIoLdd12.h"
#include "HOOK_SENSOR_1.h"

#include "BitIoLdd11.h"
#include "HOOK_SENSOR_2.h"

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
#include "PANIC_BUTTON_INPUT.h"

//#include "PANIC_BUTTON_INPUT.h"
#include "BitIoLdd1.h"
#include "PANIC_BUTTON_OUTPUT.h"
#include "BitIoLdd2.h"
#include "LAMP_HIGH.h"
#include "BitIoLdd33.h"
#include "LAMP_MEDIUM.h"
#include "BitIoLdd34.h"
#include "LAMP_LOW.h"
#include "BitIoLdd35.h"
#include "CAN_C_P.h"
#include "AIR_SENSOR.h"
#include "BitIoLdd20.h"
#include "Timer_1msec.h"
#include "TimerIntLdd3.h"
#include "TU3.h"
#include "EEPROM.h"
#include "IntFlashLdd1.h"
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
#include "Global.h"
#include "App_Ges.h"
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
#include "general_func.h"
#include "SWTimer.h"
#include "SevenSeg.h"
#include "ControlProtectiveInterface_C.h"

extern uint8_t AlarmCheckFlag;
extern bool EnableHeatingFromControl;
extern bool EnableFrigoFromPlate;
extern bool EnableFrigoFromControl;
extern unsigned char PidFirstTime[4];
extern float pressSample1_Ven;
extern float pressSample2_Ven;
extern bool WriteActive;
extern unsigned char Released1;
extern unsigned char Released2;
extern unsigned char Released3;
extern unsigned char Released4;

#ifdef ENABLE_PROTECTIVE_ALARM_RESET
void HandleProtectiveAlarm(void);
#endif

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
void TestPump(unsigned char Adr); //only for test
void TestPinch(void);
void GenerateSBCComm(void);
void ProtectiveTask(void);
void InitCAN(void);
void InitTest(void);
int FreeRunCnt10msecOld;

int timerCounterModBusOld = 0;



void Manage_Debug_led(bool Status)
{
 	unsigned char Freq = FREQ_DEBUG_LED;
 	static bool Status_Led_Board = FALSE;

 	if (Status)
 		Freq = Freq * 2;

	if (timerCounterLedBoard >= Freq)
 	{
 		timerCounterLedBoard = 0;
 		if (Status_Led_Board)
 		{
 			Status_Led_Board = FALSE;
 			D_7S_DP_SetVal(); //spegne puntino led
 		}
 		else
 		{
 			Status_Led_Board = TRUE;
 			D_7S_DP_ClrVal(); //accende puntino led
 		}
 	}
}


#ifdef DEBUG_FRIGO_AMS
word TempPerfVal;    // per debug
word TempLiquido;
word TempLiquidoDecimi;
#endif

int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  /* Write your local variable definition here */

  bool MOTORE_ACCESO   = FALSE;
  bool MOTORE_ACCESO_2 = FALSE;
  bool Status_Board;
  THERAPY_TYPE TreatType = Undef;
  EnableNextAlarm = FALSE;

  pollingDataFromSBC = 0;
  pollingDataToSBC = 0;
  codeDBG = 0;
  subcodeDBG = 0;
  Released1 = 0;
  Released2 = 0;
  Released3 = 0;
  CheckAlarmFlag = 0;
  Peltier2On = FALSE;
  PeltierOn = FALSE;
  GlobalFlags.FlagsVal = 0;
  Prescaler_Freq_Signal_AMS = 10;
  Enable_AMS = 0;
  ptrT1Test = &T1TEST;

  unsigned char Prescaler_CheckStopPump = 0;

  /*inizializzo il flag che mi dice che non ho ancora fatto la tara delle pressioni che va fatta dopo la connessione dell'organo*/
  TARA_PRESS_DONE = FALSE;

  // Filippo - inizializzo il flag di test del frigo e riscaldatore
  testT1HeatFridge=0;
  airSensorTestKO=FALSE;

  CoversState = 4; // all covers chiusi
  ArteriousPumpGainForPid = DEFAULT_ART_PUMP_GAIN;
  VenousPumpGainForPid = DEFAULT_VEN_PUMP_GAIN;

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
  T_PLATE_P_Init();

  /* Write your code here */

  TreatAlm1SafAirFiltActive = FALSE;
  TreatAlm1SFAActive = FALSE;
  TreatAlm1SFVActive = FALSE;

  LiquidAmount = 0;
  StarTimeToRejAir = 0;
  TotalTimeToRejAir = 0;
  AirAlarmRecoveryState = INIT_AIR_ALARM_RECOVERY;
  DisableAllAirAlarm(FALSE);
  DisableCheckPumpStopTask = 0;

  LevelBuzzer = SILENT;
  initAllGuard();
  initAllState();
  initGUIButton();
  initSetParamFromGUI();
  initSetParamInSourceCode();

  initT1Test();

  alarmConInit();
  warningConInit();
  InitWarningsStates();
  InitAlarmsStates();
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
  timerCounterUpdateTargetPressurePid = 0;
  timerCounterUpdateTargetPressPidArt = 0;
  Prescaler_Tick_Timer = 0;
  Prescaler_Tick_TEST=0;
  FreeRunCnt10msec = 0;
  Service = FALSE;
  // Filippo - timer per gestire l'eventuale riaccensione del frigo
  timerCounterFrigoOn=ATTESA_FRIGO_OFF_NEW_PID;	// forzo il timer già scaduto perchè alla prima accensione deve partire subito

  // Filippo - inizializzo l'allarme del T1 test
  allarmeTestT1Attivo=FALSE;
  // inizializzo la variabile che decide se fare il T1Test del frigo prima del riscaldatore
  T1_Test_Frigo_Before_Heater = FALSE;

  Frigo_ON = FALSE;
  Heat_ON  = FALSE;

  MAX_PLATE_TEMP = 0;
  MIN_PLATE_TEMP = 0;
  START_HEAT_ON_DEBUG = FALSE;
  START_FRIGO_ON_DEBUG = FALSE;

  iFlag_actuatorCheck = IFLAG_IDLE;
  iFlag_modbusDataStorage = FALSE;

  CHANGE_ADDRESS_IR_SENS = FALSE;

  OK_START = FALSE;
  ON_NACK_IR_TM = FALSE;

  Stop_Button_clicked = FALSE;

  modBusPmpInit(TreatType);
  modBusPinchInit();
  modbusDataInit();


  SBC_COMM_Enable();
  initCommSBC();
    /**/
#ifndef PUMP_EVER
  /*abilito l'RTS per la trasmissionme verso i motori;
   * la lascio sempre attiva tanto ogni 50 msec al massimo
   *  sarò sempre io a fare la trasmisisone interrogando i driver*/

  RTS_MOTOR_SetVal();
#else
  /*disabilito l'RTS per la trasmissionme verso i motori;
   * sarà attivato prima di effettuare una trasmissione e
   * disattivato alla fine della stessa*/
  RTS_MOTOR_ClrVal();
#endif

  //ptrMsgSbcRx = &msg_sbc_rx[0];
  ptrMsgSbcRx = &sbc_rx_data[0];
  ptrSbcCountRx = 0;

  ptrPCDebug = &msgPcDebug[0];
  ptrPCDebugCount = 0;

  ptrMsgPeltierRx = &peltierDebug_rx_data[0];

  ptrPeltierCountRx = 0;

  ptrMsgPeltier2Rx = &peltier2Debug_rx_data[0];

  ptrPeltier2CountRx = 0;

  uint8_t InpData[4];
  uint8_t OutData[4] = "0123";
  LDD_TDeviceData *MySPIPtr;
  LDD_TError Error;
  iflag_spi_rx = IFLAG_IDLE;

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

  /*NEL VECCHIO HW abilitava la 24 V sui driver delle celle di peltier.
   * CON LE MODIFICHE HW PER LE EVER ORA DIVENTA L'ENABLE AL 48 PER LE POMPE EVER*/
  EN_P_1_C_SetVal();
  // questo servirebbe solo con le celle di peltier. Ora con le modifiche hw per le
  // EVER non servirebbe piu' ma, io lo abilito lo stesso. Con le modifiche hw (48 V)
  // fatte per le ever non e' piu' possibile far funzionare di nuovo le peltier solo
  // ripristinando il sw.
  EN_P_2_C_SetVal();

  /*leggo tutta la struttura dati salvata nella parte di flash usata come EEPROM (ci saranno ad esmepio i coefficienti di claibrazione)*/
   EEPROM_Read(START_ADDRESS_EEPROM, (EEPROM_TDataAddress)&config_data, sizeof(config_data));

  /*scrivo i dati di default sulla flash usata come eeprom
   * (tra cui i coef di calibrazione dei sensori) solo
   * se la flash usata come eeprom non è mai stata scritta.
   * TODO quando sarà fatta la funzione per la calibrazione,
   * dovrà essere fatta in modo simile per sovrascrivere i coefficienti*/
   Set_Data_EEPROM_Default();

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
#ifdef PUMP_EVER
	MOD_BUS_RESPONSE result;
	result = WaitForModBusResponseTask((WAIT_FOR_MB_RESP_TASK_CMD)WAIT_MB_RESP_TASK_RESET_CMD);
	setPumpSpeedValueEVER (PPV1,0,0, INIT_PUMP);
	//setPumpSpeedValue (PPV1, 1000);
 	while(result == MOD_BUS_ANSW_NO_ANSW)
 		result = WaitForModBusResponseTask((WAIT_FOR_MB_RESP_TASK_CMD)WAIT_MB_RESP_TASK_NO_CMD);

  // Se uso queste funzioni prima di andare avanti dovrei controllare la risposta e
  // poi passare alla seconda pompa
	result = WaitForModBusResponseTask((WAIT_FOR_MB_RESP_TASK_CMD)WAIT_MB_RESP_TASK_RESET_CMD);
	setPumpSpeedValueEVER (PPV2,0,0, INIT_PUMP);
	//setPumpSpeedValue (PPV1, 0);
 	while(result == MOD_BUS_ANSW_NO_ANSW)
 		result = WaitForModBusResponseTask((WAIT_FOR_MB_RESP_TASK_CMD)WAIT_MB_RESP_TASK_NO_CMD);
#endif

//  QUESTO CODICE POTREBBE ESSERE NECESSARIO SCOMMENTARLO SE NON FOSSE SUFFICIENTE LA
//  CORRENTE DI PILOTAGGIO DELLE POMPE
  timerCounterCheckModBus = 0;
  setPumpCurrentValue((unsigned char)2, (int)31);
  while (timerCounterCheckModBus < 1);
  timerCounterCheckModBus = 0;
  setPumpCurrentValue((unsigned char)3, (int)31);
  while (timerCounterCheckModBus < 1);
  timerCounterCheckModBus = 0;
#ifndef PUMP_EVER
  setPumpCurrentValue((unsigned char)4, (int)31);    // imposto la CURRENT LEVEL a 24
  while (timerCounterCheckModBus < 1);
  timerCounterCheckModBus = 0;
  setPumpCurrentValue((unsigned char)5, (int)31);
  while (timerCounterCheckModBus < 1);

  setPumpAccelerationValue((unsigned char)4, (int)30);    // imposto accelerazione 30 rpm/sec
  while (timerCounterCheckModBus < 1);
  timerCounterCheckModBus = 0;
  setPumpAccelerationValue((unsigned char)5, (int)30);
  while (timerCounterCheckModBus < 1);
#endif

  /*prima di entrare nel loop infinito chiedo i dati di targa agli attuatori
   * devo ricevere i dati di tutte le pompe in massimo un secondo;
   * quando avrò tutti gli attuatori sarà da rimettere TOT_NUMBER_OF_ACTAUTOR al posto di 0x03*/
  slvAddr = FIRST_ACTUATOR;

  unsigned char Ritrasmissioni = 0;

  /*accetto un numero massimo di ritrasmissioni pari a 40, altrimenti esco dal while*/
  while (slvAddr <= LAST_ACTUATOR && Ritrasmissioni <= 40)
  {
	  /*se ho ricevuto opp sono in IDLE quindi non ho ancora inviato il primo messaggio, invio il messaggio*/
	  /*Se è scaduto il timer da 1 sec (timerCounterCheckModBus >= 20) e non ho ricevuto risposta, ritrasmetto*/
	  if (iFlag_actuatorCheck == IFLAG_COMMAND_RECEIVED || iFlag_actuatorCheck == IFLAG_IDLE || timerCounterCheckModBus >= 20)
	  {
		  iFlag_actuatorCheck = IFLAG_COMMAND_SENT;

		  timerCounterCheckModBus = 0;

		  /*L'indirizzo slvAddr = 6 non è usato*/
		  if (slvAddr == 6)
				slvAddr= 7;
#ifdef PUMP_EVER
		  if (slvAddr == 4)
				slvAddr= 7;
#endif
		/*chiamo la funzione col corretto number of address dipendentemente dall'attuatore (pump/pinch)*/
		if (slvAddr <= LAST_PUMP)
			/*funzione che mi legge lo stato delle pompe*/
			Check_Actuator_Status (slvAddr,funcCode,readAddrStartReadRevision,numberOfAddressReadRevision);
		else
			/*funzione che mi legge lo stato delle pinch*/
			Check_Actuator_Status (slvAddr,funcCode,readAddrStartReadRevision,numberOfAddressReadRevision);

		Ritrasmissioni++;
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
  timerCounterADC0 = 0;
  timerCounterADC1 = 0;


  // al reset metto tutti i rami delle pinch chiusi
  setPinchPosValue (PINCH_2WPVF, MODBUS_PINCH_POS_CLOSED);
  while (timerCounterCheckModBus < 1);
  timerCounterCheckModBus = 0;
  setPinchPosValue (PINCH_2WPVA, MODBUS_PINCH_POS_CLOSED);
  while (timerCounterCheckModBus < 1);
   timerCounterCheckModBus = 0;
  setPinchPosValue (PINCH_2WPVV, MODBUS_PINCH_POS_CLOSED);
  while (timerCounterCheckModBus < 1);
   timerCounterCheckModBus = 0;

   // abilito tutti gli allarmi previsti
   SetAllAlarmEnableFlags();

   // non faccio partire le peltier subito
   peltierCell.readAlwaysEnable = 1;
   peltierCell2.readAlwaysEnable = 1;
   ArteriousPumpGainForPid = DEFAULT_ART_PUMP_GAIN;
   VenousPumpGainForPid = DEFAULT_VEN_PUMP_GAIN;

   InitCAN();
   SwTimerInit();
   InitTest();
   InitSevenSeg();
   EnableNextAlarmFunc(); //EnableNextAlarm = TRUE;
   PeltierStarted = FALSE;
   LiquidTempContrTask(RESET_LIQUID_TEMP_CONTR_CMD);
   ExpectedPrimDuration = 0;
   FilterFlowVal = 0;
   OxygenFlowRpm = 0;
   PumpStoppedCnt = 0;
   InitControlProtectiveInterface();
   SuspendInvioAlarmCode = 0;
   TimeoutAirEjection = 0;
#ifdef ENABLE_PROTECTIVE_ALARM_RESET
   ProAlmCodeToreset = 0;                         // codice dell'allarme protective da resettare (RxBuffCanP[2]->SRxCan2.AlarmCode).
		                                          // 0 nessun allarme da resettare
   memset(&ProtectiveAlarmStruct, 0, sizeof(struct alarm));
#endif

#ifdef DEBUG_FRIGO_AMS
   T_PLATE_C_GRADI_CENT = 0.0;
   TempPerfVal = 25;    // per debug
   TempLiquido = 25;
   TempLiquidoDecimi = 250;
#endif

   AlarmCheckFlag = 0;
   InitDigitalT1Test(); //Inizializzazione dei T1 Test segnali digitali

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

#ifdef ENABLE_PROTECTIVE_ALARM_RESET
			// Controllo se e' attivo un allarme della protective e, nel caso, invio eventuali button reset
			// su canbus
	      	HandleProtectiveAlarm();
#endif

	         /*****MACHINE STATE UPDATE START****/
        	 if(timerCounterMState >= 1)
	         {
	        	timerCounterMState = 0;


	        	computeMachineStateGuard();

	        	processMachineState();

	        	ManageMuteButton();

	        	alarmEngineAlways();
	        	warningsEngineAlways();
		        GenerateSBCComm();
		        ProtectiveTask();
		        // Filippo - devo verificare che le temperature piatto lette dalla control e dalla protective siano le stesse
		        verificaTempPlate();
		        manageAlarmT1Test();

		        updateDurationToSendSBC();

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
		        CoversState = CheckCoverPump();

				/********************************/
				/*      CHECK STOP PUMP        */
				/********************************/
		        /*funzione che controlla se ad ogni singola pompa è stato inviato un comando di stop
		         * e la stessa sta ancora girando, nel qual caso si reinoltra il comando di stop
		         * il controllo lo facciamo una volta ogni 1 secondo per dar modo alla pompa eventualmente di eseguire il comando*/
		        Prescaler_CheckStopPump++;
		        if (Prescaler_CheckStopPump > 20)
		        {
					/*Se è sttaa selezionata almeno una terapia*/
		        	if (GetTherapyType() != Undef)
						CheckStopPump();

					Prescaler_CheckStopPump = 0;

			        updateMaxTempPlate();
			        updateMinTempPlate();
		        }
	         }

	         if(ReadKey1()) // per debug con la tastiera a bolle
	         {
	        //	 setPumpSpeedValueHighLevel(0x04,2000);
	        //	 setPumpSpeedValueHighLevel(0x04,4000);
	        	 Released1 = 1;
	         }
	         if(ReadKey2()) // per debug con la tastiera a bolle
	         {
	        //	 setPumpSpeedValueHighLevel(0x04,8000);
	        //	 setPumpSpeedValueHighLevel(0x04,0000);
	        	 Released2 = 1;
	         }
	         if(ReadKey3()) // per debug con la tastiera a bolle
	         {
	        //	 EN_Motor_Control(DISABLE);
	        //	 setPumpSpeedValueHighLevel(0x05,4000);
	        	 Released3 = 1;
	         }
	         if(ReadKey4()) // per debug con la tastiera a bolle
	         {
//#ifdef DEBUG_FRIGO_AMS
//	        	 TempLiquidoDecimi--;
//#endif
	        //	 EN_Motor_Control(ENABLE);
	       // 	 setPumpSpeedValueHighLevel(0x05,0000);
	        	 Released4 = 1;
	         }

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

#ifndef	 DEBUG_FRIGO_AMS
	        	 // per il controllo della temperatura tramite le peltier
		         if(PeltierStarted && (peltierCell.readAlwaysEnable == 1) && (peltierCell2.readAlwaysEnable == 1))
	        	 {
        			 LiquidTempContrTask(NO_LIQUID_TEMP_CONTR_CMD);
	        	 }

	        	 // *********************DISATTIVAZIONE PELTIER SE NON RISPONDONO **************************
	        	 // messo qui soloper debug, dopo sarà da gestire con un allarme specifco
	        	 // Nel Test, se mando 50 msg senza avere una loro risposta corretta (quindi 5 secondi), disabilitp la 24 Volt e le spengo
	        	 if (peltierCell.countMsgSent > 50)
	        		  EN_P_1_C_ClrVal();

	        	 if (peltierCell2.countMsgSent > 50)
	        		  EN_P_2_C_ClrVal();
	        	 // *********************DISATTIVAZIONE PELTIER SE NON RISPONDONO END **************************

				 if(peltierCell.readAlwaysEnable == 0)
				 {
					 // funzione che gestisce tutta la comunicazione, serve solo una volta
					 // per impostare i parametri sui driver delle peltier
					 // alwaysPeltierActuator();

					 // funzione che serve a impostare un nuovo valore di temp, e gestire
					 // lo start se non è stato fattio e l'abilitazione delle ventole
					 // sulla cella di peltier
					 ManagePeltierActuator();
				 }

				 if(peltierCell2.readAlwaysEnable == 0)
				 {
					// funzione che gestisce tutta la comunicazione, serve solo una volta
					// per impostare i parametri sui driver delle peltier
				    // alwaysPeltier2Actuator();

					 // funzione che serve a impostare un nuovo valore di temp, e gestire
					 // lo start se non è stato fattio e l'abilitazione delle ventole
					 // sulla cella di peltier
					 ManagePeltier2Actuator();
				 }

				 //funzione cher gestiscer lo stop della cella di peltier 1
				 if (peltierCell.StopEnable == 1)
					 stopPeltierActuator();

				 //funzione cher gestiscer lo stop della cella di peltier 1
				 if (peltierCell2.StopEnable == 1)
					 stopPeltier2Actuator();
#endif

				 perfusionParam.pulsatility = (word)(modbusData[pumpPerist[0].pmpMySlaveAddress-2][17] / 100) * 2;
		     }

	         /********************************/
	         /*      EMERGENCY BUTTON        */
	         /********************************/
			 Manage_Emergency_Button();

	         /********************************/
	         /*          FRONTAL COVER       */
	         /********************************/
			 Manage_Frontal_Cover();

			 /********************************/
			 /*          HOOK SENSORS        */
			 /********************************/
			 Manage_Hook_Sensors();

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
	         /*             ADC	             */
	         /********************************/

			 /*funzioni per leggere i canali AD*/
			 Manange_ADC0();
			 Manange_ADC1();
			 /*END funzioni per leggere i canali AD*/


		     /*faccio lo start della conversione sul canale AD0 ogni 50 msec*/
			 if (timerCounterADC0 >=1)
			 {
				timerCounterADC0 = 0;
				AD0_Start();
			 }
			 /*faccio lo start della conversione sul canale AD1 ogni 10 msec
			  * per avere le pressioni a 100 HZ (timerCounterADC1 si incremente ogni msec*/
			 if (timerCounterADC1 >=10)
			 {
				EnterCritical();
				timerCounterADC1 = 0;
				ExitCritical();
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

        	 Manage_and_Storage_ModBus_Actuator_Data();


	         /*********PUMP*********/

			// solo per fare il priming senza disposable
	        //if(ptrCurrentState->state != STATE_EMPTY_DISPOSABLE)
	        //	DisableAllAlarm();

	      	ManageSwTimers();

	      	// aggiorno il valore del flusso sul filtro
	      	UpdateFilterFlowVal();

//#ifdef	 DEBUG_FRIGO_AMS
	      	// task di controllo della temperatura del liquido nel reservoir
	      	// non posso mettere questo codice al posto di quello delle peltier perche' non
	      	// lavorerebbe bene il pwm del riscaldatore
	      	// Filippo - cambio funzione per gestire nuovo PID che usa frigo e riscaldatore insieme
//	      	FrigoHeatTempControlTask((LIQ_TEMP_CONTR_TASK_CMD)LIQ_T_CONTR_TASK_NO_CMD);
	      	/*se non sono state comandate le accensione ne di frigo ne di riscaldatore in debug*/
	      	if (START_HEAT_ON_DEBUG == FALSE && START_FRIGO_ON_DEBUG == FALSE)
	      	{
	      		FrigoHeatTempControlTaskNewPID((LIQ_TEMP_CONTR_TASK_CMD)LIQ_T_CONTR_TASK_NO_CMD);
	      	}
	      	/*se è stata comandata l'accensione del riscaldatore in debug*/
	      	else if (START_HEAT_ON_DEBUG == TRUE)
	      	{
	      		HeatingPwmDebug(GetHeatingPwmPerc());
	      	}
	      	/*se è stata comandata l'accensione del FRIGO in debug*/
	      	else if (START_FRIGO_ON_DEBUG == TRUE)
	      	{
	      		Start_Frigo_AMSDebug(GetFrigoPercDebug());
	      	}
//#endif
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
