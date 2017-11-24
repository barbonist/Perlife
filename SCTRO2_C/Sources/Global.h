/*
 * Global.h
 *
 *  Created on: 03/nov/2016
 *      Author: W15
 */

#ifndef SOURCES_GLOBAL_H_
#define SOURCES_GLOBAL_H_

/* Syncronization flag */
char	iflag_pc_rx;
char	iflag_pc_tx;
char	iflag_pmp1_rx;
char	iflag_pmp1_tx;
char	iflag_sbc_rx;
char	iflag_sbc_tx;
char	iflag_peltier_rx;
char	iflag_peltier_tx;
char	iflagPeltierBusy;
char	iflagPeltierMsg;
char	iflag_cntrf_pmp_rx;
char	iflag_cntrf_pmp_tx;
char	iflag_write_press_sensor;
char	iflag_read_press_sensor;
char	iflag_write_temp_sensor;
char	iflag_read_temp_sensor;
char	iflag_spi_rx;
char	iflag_uflow_sens;
char	iflag_sensTempIR;
char	iflag_sensTempIR_Meas_Ready;
char	iflag_sensTempIRRW;

/* DEBUG */
#define DEBUG_ENABLE
#define DEBUG_ADC				0x01
#define DEBUG_PUMP				0x02
#define DEBUG_CENTRIF_PUMP		0x03
#define DEBUG_PELTIER			0x04
//#define DEBUG_COMM_SBC			0x05
#define DEBUG_MACHINE_STATE		0x06
#define DEBUG_CONTROL			0xA5
#define DEBUG_PROTECTION		0x5A
#define DEBUG_LOG_PC			0x01
#define DEBUG_TREATMENT			0xA5
/*#define DEBUG_I2C_TEMP_SENS		0x01;*/
/*#define DEBUG_FLOW_SENS			0x00*/ /*0x01*/
/* DEBUG */

#define	IFLAG_PC_RX				0x01 /* new data on rx 232 pc */
#define IFLAG_PC_TX				0x01 /* new data to rx 232 pc */
#define IFLAG_PMP1_RX			0x01 /* new data on rx 422 pmp1 */
#define IFLAG_PMP1_BUSY			0xA5 /* canale occupato */
#define IFLAG_PMP2_RX			0x01 /* new data on rx 422 pmp2 */
#define IFLAG_PMP3_RX			0x01 /* new data on rx 422 pmp3 */
#define IFLAG_PMP4_RX			0x01 /* new data on rx 422 pmp4 */
#define IFLAG_PMP1_TX			0x01 /* new data on tx 422 pmp1 */
#define IFLAG_PMP2_TX			0x01 /* new data on tx 422 pmp2 */
#define IFLAG_PMP3_TX			0x01 /* new data on tx 422 pmp3 */
#define IFLAG_PMP4_TX			0x01 /* new data on tx 422 pmp4 */
#define IFLAG_SBC_RX			0x01 /* new data from sbc */
#define IFLAG_SBC_TX			0x01 /* new data to sbc */
#define IFLAG_PELTIER_RX		0x01 /* new data from Peltier */
#define IFLAG_PELTIER_HZ		0x05 /* high impedance */
#define IFLAG_PELTIER_TX		0x01 /* new data to Peltier */
#define IFLAG_PELTIER_BUSY		0xA5 /* comm channel to peltier busy */
#define IFLAG_PELTIER_FREE		0x5A /* comm channel to peltier free */
#define IFLAG_PELTIER_MSG_START	0x05 /* Peltier message start */
#define IFLAG_PELTIER_MSG_END	0x55 /* Peltier message end */
#define IFLAG_WRITE_PR_SENSOR	0x03 /* Write pressure sensor parameter */
#define IFLAG_READ_PR_SENSOR	0x01 /* Read pressure sensor */
#define IFLAG_WRITE_TEMP_SENSOR	0x03 /* Write temperature sensor parameter */
#define IFLAG_READ_TEMP_SENSOR	0x01 /* Read temperature sensor */
#define IFLAG_SPI_RX_TRUE		0x01 /* Message received on spi channel */
#define IFLAG_UFLOW_SENS_RX		0x01 /* Message received from ultrasound flowsensor */
#define IFLAG_UFLOW_SENS_TX		0x02 /* Message transmitted to ultrasound flowsensor */
#define IFLAG_SENS_TEMPIR_TX	0x5A /* Message end trasmission */
#define IFLAG_SENS_TEMPIR_RX	0xA5 /* Message end reception */
#define IFLAG_SENS_TEMPIR_WAIT	0xCC /* Wait for rx/tx message */
#define IFLAG_SENS_TEMPIR_WRITE 0xEE /* write request */
#define IFLAG_IRTEMP_MEASURE_READY	0x01 /* valore di temperatura pronto da leggere */
#define IFLAG_IDLE				0x00 /* idle flag */
#define IFLAG_BUSY				0x5A /* channel busy */

/**/
/**/

/**/
#define BYTES_TO_WORD(hi,lo)  (unsigned int) (((unsigned int)(hi) << 8) | (unsigned int)(lo))
#define BYTES_TO_WORD_SIGN(hi,lo)  (int) (((int)(hi) << 8) | (int)(lo))
#define BYTES_TO_WORD_BYTE(hi,lo)	(unsigned short) (((char)(hi) << 8) | (char)(lo))

/* Communication Protocol Variable General */
unsigned char	pcDebug_rx_data[16];				/* received byte from pc */
unsigned char	pcDebug_tx_data[32];				/* transmitted byte to pc */

unsigned char	peltierDebug_rx_data[256];				/* received byte from peltier */
unsigned char	peltierDebug_rx_data_dummy[256];
unsigned char	peltierDebug_tx_data[32];				/* transmitted byte to peltier */

unsigned char	sbcDebug_rx_data[32];				/* received byte from sbc */
//unsigned char	sbcDebug_tx_data[32];				/* transmitted byte to sbc */
unsigned char		sbcDebug_tx_data[32];				/* transmitted byte to sbc */

/**/




//AS1_TComData    msg_pmp1_tx[257];		/* modbus codified pmp1 message transmitted */
//AS1_TComData *  msg_pmp1_tx_ptr;		/* pointer to msg_pmp1_tx */
//AS1_TComData    msg_pmp1_rx[257];		/* modbus codified pmp1 message received */
//AS1_TComData *  msg_pmp1_rx_ptr;		/* pointer to msg_pmp1_rx */

unsigned char * ptr;
//unsigned char * ptrDebug;
char ptr_count;

unsigned char *ptrPCDebug;
unsigned char ptrPCDebugCount;
char msgPcDebug[20];

unsigned char *ptrPeltier;
unsigned char ptrPeltierCountRx;
char *ptrMsgPeltierRx;
char *ptrMsgDataieee754start;
char *ptrMsgDataPeltierInt;
char dataIeee754[10];
char dataIeee754Dummy[10];
char dataIntPeltier[8];
long retIeee754;
unsigned long retIeee754Dummy;
char *ptrMsgPeltierRxDummy;
char *ptrMsgPeltierTx;

char dataFloatWriteLow[10];
char dataFloatWriteHigh[10];
static char msgPeltierToSendWr = 0;
char regIdHigh[4];
char regIdLow[4];

unsigned char *ptrSBC;
unsigned char ptrSBCCount;
//char msg_sbc_rx[20];
char *ptrMsgSbcRx;
char *ptrMsgSbcTx;
unsigned char	ptrSbcCountRx;

unsigned char *ptrPump;
unsigned char ptrPumpCount;

//signed int valModBusArray[6];
unsigned int valModBusArray[6];



struct funcRetStruct
{
	unsigned char *  ptr_msg;
	unsigned char   mstreqRetStructNumByte;
	unsigned char * slvresRetPtr;
	unsigned char   slvresRetNumByte;
	} _funcRetVal;

struct funcRetStruct * _funcRetValPtr;

/* Data Structure */


struct Centrifugal_Pump {
	char	id;
};


struct Pressure_Sensor {
	char	id;
};

struct Temperature_Sensor {
	char	id;
};

struct Flow_Sensor {
	char	id;
};

/* Machine State Structure */
struct machineState {
	unsigned short	state;
	unsigned short  parent;
	unsigned short	child;
	unsigned short  action;
	struct machineParent * ptrParent;
	void (*callBackFunct) (void);
} ;

struct machineParent {
	unsigned short	state;
	unsigned short  parent;
	unsigned short	child;
	unsigned short  action;
	struct machineChild * ptrChild;
	void (*callBackFunct) (void);
} ;

struct machineChild {
	unsigned short	state;
	unsigned short  parent;
	unsigned short	child;
	unsigned short  action;
	struct machineNull * ptrStateNull;
	void (*callBackFunct) (void);
} ;

struct machineNull {

} ;

struct machineState * ptrCurrentState;
struct machineState * ptrPreviousState;
struct machineState * ptrFutureState;

struct machineParent * ptrCurrentParent;
struct machineParent * ptrPreviousParent;
struct machineParent * ptrFutureParent;

struct machineChild * ptrCurrentChild;
struct machineChild * ptrPreviousChild;
struct machineChild * ptrFutureChild;

//struct machineState stateState[26];

unsigned short	actionFlag;
/* Machine State Structure */

/* Machine State Data */
enum State {
	/*********************/
	/* 		STATE  		 */
	/*********************/
	STATE_NULL = 0x0000,
	STATE_ENTRY,
	STATE_IDLE,
	STATE_SELECT_TREAT,
	STATE_T1_NO_DISPOSABLE,
	STATE_MOUNTING_DISP,
	STATE_TANK_FILL,
	STATE_PRIMING_PH_1,
	STATE_PRIMING_PH_2,
	STATE_TREATMENT_KIDNEY_1,
	STATE_EMPTY_DISPOSABLE,
	/*********************/
	/* 		STATE  		 */
	/*********************/

	//STATE_T1_WITH_DISPOSABLE,
	//STATE_PRIMING_TREAT_1,
	//STATE_PRIMING_TREAT_2,
	STATE_TREATMENT_2,
	STATE_EMPTY_DISPOSABLE_1,
	STATE_EMPTY_DISPOSABLE_2,
	STATE_WASHING,
	STATE_FATAL_ERROR,
	STATE_3,
	STATE_END_NUMBER,
};

enum Parent {
	PARENT_NULL = 0x0000,

	/*********************/
	/* 		PARENT		 */
	/*********************/
	PARENT_PRIMING_TREAT_KIDNEY_1_INIT,
	PARENT_PRIMING_TREAT_KIDNEY_1_RUN,
	PARENT_PRIMING_TREAT_KIDNEY_1_ALARM,
	PARENT_PRIMING_TREAT_KIDNEY_1_END,
	PARENT_TREAT_KIDNEY_1_INIT,
	PARENT_TREAT_KIDNEY_1_PUMP_ON,
	PARENT_TREAT_KIDNEY_1_ALARM,
	PARENT_TREAT_KIDNEY_1_END,
	/*********************/
	/* 		PARENT 		 */
	/*********************/

	PARENT_ENTRY,
	PARENT_IDLE,
	PARENT_IDLE_ALARM,
	PARENT_T1_NO_DISP_INIT,
	PARENT_T1_NO_DISP_CHECK_PRESS,
	PARENT_T1_NO_DISP_CHECK_TEMP,
	PARENT_T1_NO_DISP_CHECK_LEVEL,
	PARENT_T1_NO_DISP_CHECK_FLWMTR,
	PARENT_T1_NO_DISP_CHEK_AIR,
	PARENT_T1_NO_DISP_ALARM,
	PARENT_T1_NO_DISP_END,
	PARENT_T1_NO_DISP_FATAL_ERROR,
	PARENT_T1_WITH_DISP_INIT,
	PARENT_T1_WITH_DISP_LEAK,
	PARENT_T1_WITH_DISP_LEVEL,
	PARENT_T1_WITH_DISP_ALARM,
	PARENT_T1_WITH_DISP_END,
	PARENT_T1_WITH_DISP_FATAL_ERROR,
	PARENT_PRIMING_TREAT_1_INIT,
	PARENT_PRIMING_TREAT_1_RUN,
	PARENT_PRIMING_TREAT_1_ALARM,
	PARENT_PRIMING_TREAT_1_END,
	PARENT_PRIMING_TREAT_2_INIT,
	PARENT_PRIMING_TREAT_2_RUN,
	PARENT_PRIMING_TREAT_2_ALARM,
	PARENT_PRIMING_TREAT_2_END,
	PARENT_PRIMING_FATAL_ERROR,
	PARENT_TREAT_1_INIT,
	PARENT_TREAT_1_PUMP_ON,
	PARENT_TREAT_1_STOP_PERFUSION, /* child di alarm */
	PARENT_TREAT_1_STOP_PURIFICAT, /* child di alarm */
	PARENT_TREAT_1_STOP_ALL_PUMP, /* child di alarm */
	PARENT_TREAT_1_ALARM,
	PARENT_TREAT_1_END,
	PARENT_TREAT_2_INIT,
	PARENT_TREAT_2_PUMP_ON,
	PARENT_TREAT_2_STOP_PERFUSION,
	PARENT_TREAT_2_STOP_PURIFICAT,
	PARENT_TREAT_2_STOP_ALL_PUMP,
	PARENT_TREAT_2_ALARM,
	PARENT_TREAT_2_END,
	PARENT_TREAT_FATAL_ERROR,
	PARENT_EMPTY_DISPOSABLE_1_INIT,
	PARENT_EMPTY_DISPOSABLE_1_RUN,
	PARENT_EMPTY_DISPOSABLE_2_INIT,
	PARENT_EMPTY_DISPOSABLE_2_RUN,
	PARENT_EMPTY_DISPOSABLE_ALARM,
	PARENT_EMPTY_DISPOSABLE_END,
	PARENT_EMPTY_DISPOSABLE_FATAL_ERROR,
	PARENT_WASHING_INIT,
	PARENT_WASHING_RUN,
	PARENT_WASHING_ALARM,
	PARENT_WASHING_END,
	PARENT_WASHING_FATAL_ERROR,
	PARENT_0,
	PARENT_1,
	PARENT_2,
	PARENT_3,
	PARENT_END_NUMBER,
};

enum Child {
	/*********************/
	/* 		CHILD		 */
	/*********************/
	CHILD_NULL = 0x0000,
	CHILD_PRIMING_ALARM_INIT,
	CHILD_PRIMING_ALARM_STOP_PERFUSION,
	CHILD_PRIMING_ALARM_STOP_PURIFICATION,
	CHILD_PRIMING_ALARM_STOP_ALL_PUMP,
	CHILD_PRIMING_ALARM_STOP_PELTIER,
	CHILD_PRIMING_ALARM_STOP_ALL_ACTUATOR,
	CHILD_PRIMING_ALARM_END,
	/*********************/
	/* 		CHILD		 */
	/*********************/


	CHILD_ENTRY,
	CHILD_IDLE,
	CHILD_TREAT_ALARM_1_INIT,
	CHILD_TREAT_ALARM_1_STOP_PERFUSION,
	CHILD_TREAT_ALARM_1_STOP_PURIFICATION,
	CHILD_TREAT_ALARM_1_STOP_ALL_PUMP,
	CHILD_TREAT_ALARM_1_STOP_PELTIER,
	CHILD_TREAT_ALARM_1_STOP_ALL_ACTUATOR,
	CHILD_TREAT_ALARM_1_END,
	CHILD_TREAT_ALARM_2_INIT,
	CHILD_TREAT_ALARM_2_STOP_PERFUSION,
	CHILD_TREAT_ALARM_2_STOP_PURIFICATION,
	CHILD_TREAT_ALARM_2_STOP_ALL_PUMP,
	CHILD_TREAT_ALARM_2_STOP_PELTIER,
	CHILD_TREAT_ALARM_2_STOP_ALL_ACTUATOR,
	CHILD_TREAT_ALARM_2_END,
	CHILD_0,
	CHILD_1,
	CHILD_2,
	CHILD_3,
	CHILD_END_NUMBER,
};

enum Action {
	ACTION_NULL = 0x0000,
	ACTION_ON_ENTRY,
	ACTION_ALWAYS,
	ACTION_ON_EXIT,
};
/* Machine State Data */

/* Machine state guard structure */
struct machineStateGuard {
	unsigned short 	guardIdentifier;
	unsigned char	guardValue;
	unsigned char	guardFlagActive;
	unsigned char	guardEntryValue;
};

struct machineStateGuard currentGuard[50]; /* equal to GUARD_END_NUMBER + 1 */

struct machineStateGuard * ptrPreviousGuard;
struct machineStateGuard * ptrCurrentGuard[50]; /* equal to GUARD_END_NUMBER + 1 */
struct machineStateGuard * ptrFutureGuard;

/* Machine state guard structure */

/* Machine state guard */
enum MachineStateGuardId {
	GUARD_NULL = 0x00,
	/* BUTTON GUARD */
	/* servono davvero?.........per ora si fa col button.state */
	/* BUTTON GUARD */

	/*********************/
	/* STATE LEVEL GUARD */
	/*********************/
	GUARD_START_ENABLE,
	GUARD_HW_T1T_DONE,
	GUARD_COMM_ENABLED,
	GUARD_ENABLE_STATE_IDLE,
	GUARD_ENABLE_SELECT_TREAT_PAGE,
	GUARD_ENABLE_T1_NO_DISP,
	GUARD_T1_NO_DISP_OK,
	GUARD_T1_NO_DISP_NOK,
	GUARD_ENABLE_MOUNT_DISPOSABLE,
	GUARD_ENABLE_TANK_FILL,
	GUARD_ENABLE_PRIMING_PHASE_1,
	GUARD_ENABLE_PRIMING_PHASE_2,
	GUARD_ENABLE_TREATMENT_KIDNEY_1,
	GUARD_ENABLE_DISPOSABLE_EMPTY,
	/*********************/
	/* STATE LEVEL GUARD */
	/*********************/

	/*********************/
	/* PARENT LEVEL GUARD */
	/*********************/
	GUARD_ALARM_ACTIVE,

	/*valutare se gestire le azioni di sicurezza con le guard: tutti gli allarmi possono essere ricondotti a 6 tipologie di azioni di sicurezza:
	 ALARM_STOP_ALL_ACTUATOR: tutti gli attuatori devono essere fermati
	 ALARM_STOP_ALL_PUMP: tutte le pompe sangue devono essere fermate
	 ALARM_STOP_PERF_PUMP: solo le pompe perfusione devono essere fermate
	 ALARM_STOP_OXYG_PUMP: solo la pompa ossigenatore deve essere fermata
	 ALARM_STOP_PURIF_PUMP: solo le pompe lato purificazione devono essere fermate
	 ALARM_STOP_PELTIER: solo l'assieme di Peltier deve essere fermato */
	/*********************/
	/* PARENT LEVEL GUARD */
	/*********************/

	/*********************/
	/* CHILD LEVEL GUARD */
	/*********************/
	GUARD_ALARM_STOP_PERF_PUMP,
	GUARD_ALARM_STOP_PURIF_PUMP,
	GUARD_ALARM_STOP_OXYG_PUMP,
	GUARD_ALARM_STOP_ALL,
	GUARD_ALARM_STOP_PELTIER,
	GUARD_ALARM_STOP_ALL_ACTUATOR,
	/*********************/
	/* CHILD LEVEL GUARD */
	/*********************/

	/**/
	GUARD_SELECT_ORGAN,

	GUARD_T1_NO_DISP_START,
	GUARD_T1_NO_DISP_END,
	GUARD_THERAPY_SELECTED,
	GUARD_THERAPY_CONFIRMED,
	GUARD_KIT_MOUNTED_CONFIRMED,
	GUARD_START_LEAK_CONFIRMED,
	GUARD_T1_WITH_DISP_END,
	GUARD_START_PRIMING,
	GUARD_TREAT_1_SELECTED,
	GUARD_TREAT_2_SELECTED,
	GUARD_PRIMING_END,
	GUARD_START_TREAT_CONFIRMED,
	GUARD_STOP_TREAT_SELECTED,
	GUARD_TREAT_1_END,
	GUARD_EMPTY_DISPOSABLE_END,
	GUARD_OPEN_COVER_CONFIRMED,
	GUARD_CLEAN_EQUIP_CONFIRMED,
	/**/



	GUARD_FATAL_ERROR,
	GUARD_END_NUMBER
};

enum MachineStateGuardValue
{
	GUARD_VALUE_NULL = 0x00,
	GUARD_VALUE_NOT_DEFINED = 0xFE,
	GUARD_VALUE_TRUE = 0xA5,
	GUARD_VALUE_FALSE =0x5A
};

enum MachineStateGuardFlag
{
	GUARD_FLAG_NULL = 0x00,
	GUARD_FLAG_TRUE = 0xA5,
	GUARD_FLAG_FALSE = 0x5A,
};

enum MachineStateGuardEntry
{
	GUARD_ENTRY_VALUE_NULL = 0x00,
	GUARD_ENTRY_VALUE_TRUE = 0xA5,
	GUARD_ENTRY_VALUE_FALSE = 0x5A
};
/* Machine state guard */

/* alarm */
struct alarm {
	unsigned char 	code; 			/* alarm code */
	unsigned char	physic;			/* alarm physic condition */
	unsigned char	active;			/* alarm active condition */
	unsigned char	type;			/* alarm type: control, protection */
	unsigned char	secActType;		/* alarm security action: type of secuirty action required */
	unsigned char	priority;		/* alarm priority: low, medium, right */
	unsigned short	entryTime;		/* entry time in ms */
	unsigned short	exitTime;		/* exit time in ms */
	unsigned char	ovrdEnable;		/* override enable: alarm can be overridden when alarm condition is still present */
	unsigned char	resettable;		/* reset property */
	unsigned char	silence;		/* silence property: the alarm acoustic signal can be silenced for a limited period of time */
	unsigned char	memo;			/* memo property: the system remain in the alarm state even if the alarm condition is no longer present */
	void (*prySafetyActionFunc)(void); /* safety action: funzione che esegue la funzione di sicurezza in base alla priorità dell'allarme */
};

struct alarm	alarmCurrent;
struct alarm * ptrAlarmCurrent;
/* alarm */

/* perfusion parameter */
/* word will be converted to float by sbc */
typedef unsigned short	word;
struct perfParam{
	word systolicPress;
	word diastolicPress;
	word meanPress;
	word flowPerfArt;
	word flowPerfVen;
	word flowOxygenat;
	word tempReservOutlet;
	word tempPerfInletCon;
	word tempPerfInletPro;
	word renalResistance;
	word volumePrimingArt;
	word volumePrimingVen;
	word volumePrimingOxygen;
	word volumeTreatArt;
	word volumeTreatVen;
	word volumeTreatOxygen;
};

struct perfParam	perfusionParam;
/* perfusion parameter */

/* purification parameter */
struct purifParam{
	word	pressPlasmaFilt;
	word	pressFractFilt;
	word	pressAdsorbFilt;
	word	flowPlasmaFilt;
	word	flowFractFilt;
	word	flowAdsorbFilt;
	word	volPrimingPlasmaFilt;
	word	volPrimingFractFilt;
	word	volPrimingAdsorbFilt;
	word	volTreatPlasmaFilt;
	word	volTreatFractFilt;
	word	volTreatAdsorbFilt;
	word	volTreatWashFilt;

};

struct purifParam	purificatParam;
/* purification parameter */

/************************************************************************/
/* 						ACTUATOR 										*/
/************************************************************************/
/* Peltier cell */
struct peltier{
	/* data sent to peltier */
	char 	commandString[10];
	char 	commandRegId;
	int	 	commandDataIntToWrite;
	float 	commandDataFloatToWrite;

	/* data coming from peltier */
	char	readyToRcvCommand[4];
	char	msgPeltierRx[256];

	/* set point */
	float myTCDeadBand;
	float myTCLimit;
	float mySet;
	int myRegMode;
	float myOnOffDeadBand;
	float myOnOffHyster;
	int myFanModeSel;
	float myThrsldMainCurrHigh;
	float myNTCSteinCoeff_A;
	float myNTCSteinCoeff_B;
	float myNTCSteinCoeff_C;

	/* varie */
	char readAlwaysEnable;

};

struct peltier	peltierCell;
/* Peltier cell */

/* Peristaltic pump */
#define PRESS_LOOP_ON	0xA5
#define PRESS_LOOP_OFF	0x5A
#define REQ_STATE_ON	0xA5
#define REQ_STATE_OFF	0x5A
#define REQ_TYPE_READ	0xA5
#define REQ_TYPE_WRITE	0x5A
#define REQ_TYPE_IDLE	0xCC
#define ACTUATOR_PUMP_TYPE		0xA5
#define ACTUATOR_PINCH_TYPE		0x5A
#define DATA_READY_TRUE			0xA5
#define DATA_READY_FALSE		0x5A
struct Peristaltic_Pump {
	char					id;
	struct funcRetStruct * 	pmpStructPtr;
	int						pmpSpeed;
	int						pmpGoHomeSpeed;
	unsigned int			pmpAccelSpeed;
	unsigned int			pmpCurrent;
	unsigned int			pmpCruiseSpeed;
	int						pmpStepTarget;
	char					pmpMySlaveAddress;
	char					pmpFuncCode;
	unsigned int			pmpWriteStartAddr;
	unsigned int			pmpReadStartAddr;
	unsigned int			pmpNumeRegWrite;
	unsigned int			pmpNumRegRead;
	unsigned int *			pmpWriteRegValuePtr;
	unsigned char			pmpPressLoop;
	unsigned char			entry;
	unsigned char			reqState; //request pending / request not pending
	unsigned char			reqType; //read / write
	unsigned char			actuatorType; //pump / pinch
	int						value;
	unsigned char			dataReady;
	unsigned char			actualSpeed;
	int						actualSpeedOld;
};

struct Peristaltic_Pump		pumpPerist[4];
/* Peristaltic pump */

/* Pinch */
struct Pinch_Valve {
	char	id;
	struct funcRetStruct * 	pinchStructPtr;
	unsigned int			pinchPosTarget;
	char					pinchMySlaveAddress;
	char					pinchFuncCode;
	unsigned int			pinchWriteStartAddr;
	unsigned int			pinchReadStartAddr;
	unsigned int			pinchNumeRegWrite;
	unsigned int			pinchNumRegRead;
	unsigned int *			pinchWriteRegValuePtr;
	unsigned char			reqState;
	unsigned char			reqType;
	unsigned char			actuatorType;
	int						value;
	unsigned char			dataReady;
};

struct Pinch_Valve			pinchActuator[5];
/* Pinch */
/************************************************************************/
/* 						ACTUATOR 										*/
/************************************************************************/

/************************************************************************/
/* 						SENSORI 										*/
/************************************************************************/
/* SENSORI PRESSIONE */
/* value = gain*(adc_val - offsetVal) + Offset */
struct pressureSensor{
	float prSensOffset;
	float prSensOffsetVal;
	float prSensGain;
	float prSensValue;
	int  prSensValueFilteredWA;
	word  prSensAdc;
	word * prSensAdcPtr;
	float prSensValueOld;
	word * (*readAdctPtr)(void);
};

struct pressureSensor		sensor_PRx[6];
//struct pressureSensor		sensor_PR1;

/* SENSORI TEMPERATURA ANALOGICI */
/* value = gain*(adc_val - offsetVal) + Offset */
struct temperatureSensor{
	float tempSensOffset;
	float tempSensOffsetVal;
	float tempSensGain;
	float tempSensValue;
	word  tempSensAdc;
	word * tempSensAdcPtr;
	float tempSensValueOld;
	word * (*readAdctPtr)(void);
};

struct temperatureSensor	sensor_TMx[6];
//struct temperatureSensor	sensor_TM1;

/* SENSORI TEMPERATURA DIGITALI */
struct tempIRSensor{
	unsigned char sensorId;
	float tempSensOffset;
	float tempSensOffsetVal;
	float tempSensGain;
	float tempSensValue;
	word  tempSensAdc;
	word * tempSensAdcPtr;
	float tempSensValueOld;
	unsigned char bufferToSendLenght;
	unsigned char bufferToSend[4];
	unsigned char bufferReceivedLenght;
	unsigned char bufferReceived[4];
	word * (*readIRTempSensor)(void);
	word * (*writeIRTempSensor)(void);
	word tempSensValToWrite;
	unsigned char * ptrValToRead;
};

struct tempIRSensor sensorIR_TM[6];
struct tempIRSensor * ptrCurrent_IR_TM; /* puntatore a struttura corrente - sensore attualmente interrogato */
struct tempIRSensor * ptrMsg_IR_TM;	/* puntatore utilizzato per spedire il messaggio */


/* SENSORI DI FLUSSO AD ULTRASUONI */
struct ultrsndFlowSens{
	unsigned char sensorId;
	unsigned char sensorAddr;
	unsigned char bufferToSendLenght;
	unsigned char byteSended;
	unsigned char bufferToSend[32];
	unsigned char bufferReceivedLenght;
	unsigned char bufferReceived[128];
	unsigned char * ptrBufferReceived;
	unsigned char * (*sendCmdToSensor)(unsigned char sensId, unsigned char sensorAddress, unsigned char cmdId, unsigned char ctrlZeroAdjust, float valueZeroAdjust, unsigned char valueId);
	float volumeMlTot;
	float volumeDelta;
	unsigned char bubblePhysic;
	unsigned char bubbleSize;
	unsigned char bubblePresence;
};

struct ultrsndFlowSens sensor_UFLOW[2];
struct ultrsndFlowSens * ptrCurrent_UFLOW; /* puntatore a struttura corrente - sensore attualmente interrogato */
struct ultrsndFlowSens * ptrMsg_UFLOW; 	   /* puntatore utilizzato per spedire il messaggio */
/************************************************************************/
/* 						SENSORI 										*/
/************************************************************************/

/************************************************************************/
/* 						PULSANTI GUI 									*/
/************************************************************************/
#define GUI_BUTTON_PRESSED		0xA5
#define GUI_BUTTON_RELEASED		0x5A

enum buttonGUIEnum{
	BUTTON_KIDNEY = 0xA1,
	BUTTON_LIVER = 0xA9,
	BUTTON_CONFIRM = 0xB1,
	BUTTON_RESET = 0xB3,
	BUTTON_BACK = 0xB5,
	BUTTON_START_PRIMING = 0xB7,
	BUTTON_STOP_ALL_PUMP = 0xB9,
	BUTTON_EN_PERFUSION = 0xC1,
	BUTTON_EN_OXYGENATION = 0xC2,
	BUTTON_EN_PURIFICATION = 0xC3,
	BUTTON_START_PERF_PUMP = 0xC4,
	BUTTON_STOP_PERF_PUMP = 0xC5,
	BUTTON_START_OXYGEN_PUMP = 0xC6,
	BUTTON_STOP_OXYGEN_PUMP = 0xC7,
	BUTTON_START_PURIF_PUMP = 0xC8,
	BUTTON_STOP_PURIF_PUMP = 0xC9,
	BUTTON_PERF_DISP_MOUNTED = 0xD1,
	BUTTON_OXYG_DISP_MOUNTED = 0xD2,
	BUTTON_PERF_TANK_FILL = 0xD3,
	BUTTON_PERF_FILTER_MOUNT = 0xD4,
	BUTTON_END_NUMBER = 20
};

struct buttonGUI{
	char id;
	unsigned char state;
};

struct buttonGUI buttonGUITreatment[214];
/************************************************************************/
/* 						PULSANTI GUI 									*/
/************************************************************************/


/************************************************************************/
/* 					PARAMETRI DA IMPOSTARE TRAMITE GUI 					*/
/************************************************************************/
enum paramWordSetFromSBC{
	PAR_SET_PRIMING_VOL_PERFUSION = 0x10,
	PAR_SET_PRIMING_VOL_PURIFICATION = 0x30,
	PAR_SET_PRIMING_VOL_OXYGENATION = 0x50,
	PAR_SET_OXYGENATOR_FOW = 0x70,
	PAR_SET_PRESS_ART_TARGET = 0xB1,
	PAR_SET_PRESS_VEN_TARGET = 0xC2,
	PAR_SET_PURIF_FLOW_TARGET = 0xD3,
	PAR_SET__WORD_END_NUMBER = 0xD3
};

enum paramFloatSetFromSBC{
	PAR_SET_TEMPERATURE = 0xA0,
	PAR_SET_PURIF_UF_FLOW_TARGET = 0xE4,
	PAR_SET_FLOAT_END_NUMBER = 0xE4
};

struct parWordSetFromGUI{
	char id;
	word value;
};
struct parWordSetFromGUI parameterWordSetFromGUI[PAR_SET__WORD_END_NUMBER];


struct parFloatSetFromGUI{
	char id;
	float value;
};
struct parFloatSetFromGUI parameterFloatSetFromGUI[PAR_SET_FLOAT_END_NUMBER];
/************************************************************************/
/* 					PARAMETRI DA IMPOSTARE TRAMITE GUI 					*/
/************************************************************************/


/************************************************************************/
/* 						STRUTTURA COMUNICATOR 							*/
/************************************************************************/
#define DATA_COMM_IDLE					0x5A
#define DATA_COMM_READY_TO_BE_SEND		0xA5
struct communicatorToSBC{
	unsigned short dataPressSensReadyFlag;
	unsigned short dataFlowSensReadyFlag;
	unsigned short dataIRTempSensReadyFlag;
	unsigned short dataPeltierReadyFlag;
	unsigned short dataModBusReadyFlag;
	unsigned short dataMachineStateReadyFlag;
	unsigned short dataPerfParamReadyFlag;

	unsigned short numByteToSend;
};
struct communicatorToSBC myCommunicatorToSBC;
/************************************************************************/
/* 						STRUTTURA COMUNICATOR 							*/
/************************************************************************/


/************************************************************************/
/* 					STRUTTURA VOLUMI TRATTAMENTO 						*/
/************************************************************************/
struct volumePrimingKidney{
	int primingVolPerfTot;
	int primingVolOxygTot;
	int primingVolPurifTot;
	int primingVolPerfPhase_1;
	int primingVolOxygPhase_1;
	int primingVolPurifPhase_1;
	int primingVolPerfPhase_2;
	int primingVolOxygPhase_2;
	int primingVolPurifPhase_2;
};
struct volumePrimingKidney myVolumePrimingKidney;


struct volumeTreatmentKidney{
	int treatVolPerfTot;
	int treatVolOxygTot;
	int treatVolPurifTot;
};
struct volumeTreatmentKidney myVolumeTreatmentKidney;

int timerCounter;
int timerCounterADC;
int timerCounterPID;
int timerCounterMState;
int timerCounterModBus;
int timerCounterUFlowSensor;
int timerCounterPeltier;
/************************************************************************/
/* 					STRUTTURA VOLUMI TRATTAMENTO 						*/
/************************************************************************/


/**/

/**/

#endif /* SOURCES_GLOBAL_H_ */
