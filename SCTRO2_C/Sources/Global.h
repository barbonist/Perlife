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
char	iflag_peltier2_rx;
char	iflag_peltier2_tx;
char	iflagPeltierBusy;
char	iflagPeltierMsg;
char	iflagPeltier2Busy;
char	iflagPeltier2Msg;
char	iflag_cntrf_pmp_rx;
char	iflag_cntrf_pmp_tx;
char	iflag_write_press_sensor;
char	iflag_read_press_sensor;
char	iflag_write_temp_sensor;
char	iflag_read_temp_sensor;
char	iflag_spi_rx;
char	iflag_uflow_sens;
//char	iflag_sensTempIR;
char	iflag_sensTempIR_Meas_Ready;
char	iflag_sensTempIRRW;
char 	iFlag_actuatorCheck;
char    iFlag_modbusDataStorage;

/* DEBUG */
#define DEBUG_ENABLE
#define DEBUG_ADC				0x01
#define DEBUG_PUMP				0x02
#define DEBUG_CENTRIF_PUMP		0x03
#define DEBUG_PELTIER			0x04
//#define DEBUG_COMM_SBC			0x05  //alternativa a DEBUG_TREATMENT
#define DEBUG_MACHINE_STATE		0x06
#define DEBUG_CONTROL			0xA5
#define DEBUG_PROTECTION		0x5A
#define DEBUG_LOG_PC			0x01
#define DEBUG_TREATMENT			0xA5 //alternativa a DEBUG_COMM_SBC
//#define DEBUG_I2C_TEMP_SENS		0x01
/*#define DEBUG_FLOW_SENS			0x00*/ /*0x01*/
/* DEBUG */

#define STR_DBG_LENGHT				100

#define	IFLAG_PC_RX					0x01 /* new data on rx 232 pc */
#define IFLAG_PC_TX					0x01 /* new data to rx 232 pc */
#define IFLAG_PMP1_RX				0x01 /* new data on rx 422 pmp1 */
#define IFLAG_PMP1_BUSY				0xA5 /* canale occupato */
#define IFLAG_PMP2_RX				0x01 /* new data on rx 422 pmp2 */
#define IFLAG_PMP3_RX				0x01 /* new data on rx 422 pmp3 */
#define IFLAG_PMP4_RX				0x01 /* new data on rx 422 pmp4 */
#define IFLAG_PMP1_TX				0x01 /* new data on tx 422 pmp1 */
#define IFLAG_PMP2_TX				0x01 /* new data on tx 422 pmp2 */
#define IFLAG_PMP3_TX				0x01 /* new data on tx 422 pmp3 */
#define IFLAG_PMP4_TX				0x01 /* new data on tx 422 pmp4 */
#define IFLAG_SBC_RX				0x01 /* new data from sbc */
#define IFLAG_SBC_TX				0x01 /* new data to sbc */
#define IFLAG_PELTIER_RX			0x01 /* new data from Peltier */
#define IFLAG_PELTIER_HZ			0x05 /* high impedance */
#define IFLAG_PELTIER_TX			0x01 /* new data to Peltier */
#define IFLAG_PELTIER_BUSY			0xA5 /* comm channel to peltier busy */
#define IFLAG_PELTIER_FREE			0x5A /* comm channel to peltier free */
#define IFLAG_PELTIER_MSG_START		0x05 /* Peltier message start */
#define IFLAG_PELTIER_MSG_END		0x55 /* Peltier message end */
#define IFLAG_WRITE_PR_SENSOR		0x03 /* Write pressure sensor parameter */
#define IFLAG_READ_PR_SENSOR		0x01 /* Read pressure sensor */
#define IFLAG_WRITE_TEMP_SENSOR		0x03 /* Write temperature sensor parameter */
#define IFLAG_READ_TEMP_SENSOR		0x01 /* Read temperature sensor */
#define IFLAG_SPI_RX_TRUE			0x01 /* Message received on spi channel */
#define IFLAG_UFLOW_SENS_RX			0x01 /* Message received from ultrasound flowsensor */
#define IFLAG_UFLOW_SENS_TX			0x02 /* Message transmitted to ultrasound flowsensor */
#define IFLAG_SENS_TEMPIR_TX		0x5A /* Message end trasmission */
#define IFLAG_SENS_TEMPIR_RX		0xA5 /* Message end reception */
#define IFLAG_SENS_TEMPIR_WAIT		0xCC /* Wait for rx/tx message */
#define IFLAG_SENS_TEMPIR_WRITE		0xEE /* write request */
#define IFLAG_IRTEMP_MEASURE_READY	0x01 /* valore di temperatura pronto da leggere */
#define IFLAG_IDLE					0x00 /* idle flag */
#define IFLAG_BUSY					0x5A /* channel busy */
#define LAMP_LEVEL_LOW				0x00
#define LAMP_LEVEL_MEDIUM			0x01
#define LAMP_LEVEL_HIGH				0x02
#define ENABLE						0x00
#define DISABLE						0x01
#define IFLAG_IDLE					0x00
#define IFLAG_COMMAND_SENT			0x01
#define IFLAG_COMMAND_RECEIVED		0x02
#define LAST_ACTUATOR  				0x09 //gli attuatori sono 7 ma ho un offset di due per gli indirizzi (il primo attuatore ha indirizzo 0x02, l'ultimo ha indirizzo 0x08
#define FIRST_ACTUATOR				0x02
#define LAST_PUMP					0x05
#define TOTAL_ACTUATOR				7
#define TOTAL_MODBUS_DATA			32
#define MAX_DATA_MODBUS_RECEIVED	11
#define MAX_DATA_MODBUS_RX 		    67 //64byte di dati + 3 byte iniziali con slv Addr, fun code and byte Read Count
#define AIR							0x00
#define LIQUID						0x01

static char array [70];

/**/
/**/

/**/
#define BYTES_TO_WORD(hi,lo)  (unsigned int) (((unsigned char)(hi) << 8) | (unsigned char)(lo)) //i caratteri passi come argomento (hi,lo) devono essere UNSIGNED char e non char
#define BYTES_TO_WORD_SIGN(hi,lo)  (int) (((int)(hi) << 8) | (int)(lo))
#define BYTES_TO_WORD_BYTE(hi,lo)	(unsigned short) (((char)(hi) << 8) | (char)(lo))

/* Communication Protocol Variable General */
unsigned char	pcDebug_rx_data[16];				/* received byte from pc */
unsigned char	pcDebug_tx_data[32];				/* transmitted byte to pc */

unsigned char	peltierDebug_rx_data[256];				/* received byte from peltier */
unsigned char	peltierDebug_rx_data_dummy[256];
unsigned char	peltierDebug_tx_data[32];				/* transmitted byte to peltier */
unsigned char	peltier2Debug_rx_data[256];				/* received byte from peltier */
unsigned char	peltier2Debug_rx_data_dummy[256];
unsigned char	peltier2Debug_tx_data[32];				/* transmitted byte to peltier */

unsigned char	sbc_rx_data[256];				/* received byte from sbc */
unsigned char	sbc_tx_data[256];				/* transmitted byte to sbc */

#define SBC_RX_DATA_POS_CODE	5
#define SBC_RX_DATA_POS_SUBCODE	6

/**/


enum CommandId {
	COMMAND_ID_NULL		= 0x00,
	COMMAND_ID_ST      	= 0x20,
	COMMAND_ID_PAR_SET 	= 0xC0,
	COMMAND_ID_BUT_SBC 	= 0xC2
};

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

unsigned char *ptrPeltier2;
unsigned char ptrPeltier2CountRx;
char *ptrMsgPeltier2Rx;
char *ptrMsgData2ieee754start;
char *ptrMsgDataPeltier2Int;
char data2Ieee754[10];
char data2Ieee754Dummy[10];
char dataIntPeltier2[8];
long ret2Ieee754;
unsigned long ret2Ieee754Dummy;
char *ptrMsgPeltier2RxDummy;
char *ptrMsgPeltier2Tx;


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
	unsigned short  parent;  /* (FM) PER ORA E' SEMPRE PARENT_NULL E NON VIENE MAI USATO */
	unsigned short	child;   /* (FM) PER ORA E' SEMPRE CHILD_NULL E NON VIENE MAI USATO */
	unsigned short  action;
	struct machineParent * ptrParent;
	void (*callBackFunct) (void);
} ;

struct machineParent {
	unsigned short	state;
	unsigned short  parent;
	unsigned short	child;            /* (FM) PER ORA NON VIENE MAI USATO */
	unsigned short  action;
	struct machineChild * ptrChild;
	void (*callBackFunct) (void);
} ;

struct machineChild {
	unsigned short	state;
	unsigned short  parent;          /* (FM) PER ORA NON VIENE MAI USATO */
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


	GUARD_ALARM_WAIT_CONFIRM,  // aspetto una conferma dall'operatore per rimuovere l'allarme non dovuto a cause fisiche
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

/* sensors values */
typedef unsigned short	word;
struct sensValues{
	word pressAdsFilter;
	word pressArt;
	word pressVen;
	word pressOxy;
	word pressLevel;
	word pressSystArt;
	word pressDiasArt;
	word pressMeanArt;
	word pressSystVen;
	word pressDiasVen;
	word pressMeanVen;
	word flowArt;
	word flowVenOxy;
	word tempResOut;
	word tempOrganIn;
	word tempVenOxy;
};

struct sensValues	sensorsValues;
/* sensors values */

/* perfusion parameter */
/* word will be converted to float by sbc */
struct perfParam{
	word priVolAdsFilter;
	word priVolPerfArt;
	word priVolPerfVenOxy;
	word priDurPerfArt;
	word priDurPerVenOxy;
	word treatVolAdsFilter;
	word treatVolPerfArt;
	word treatVolPerfVenOxy;
	word treatDurPerfArt;
	word treatDurPerVenOxy;
	word unlVolAdsFilter;
	word unlVolPerfArt;
	word unlVolPerfVenOxy;
	word unlVolRes;
	word unlDurPerfArt;
	word unlDurPerVenOxy;
	word renalResistance;
	word pulsatility;
	word pressDropAdsFilter;
};

struct perfParam	perfusionParam;
/* perfusion parameter */

/* purification parameter */
struct purifParam{
	word	pressPlasmaFilt;
	word	pressFractFilt;
	word	pressAbsorbFilt;
	word	flowPlasmaFilt;
	word	flowFractFilt;
	word	flowAbsorbFilt;
	word	volPrimingPlasmaFilt;
	word	volPrimingFractFilt;
	word	volPrimingAbsorbFilt;
	word	volTreatPlasmaFilt;
	word	volTreatFractFilt;
	word	volTreatAbsorbFilt;
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
struct peltier	peltierCell2;
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

word modbusData [TOTAL_ACTUATOR] [TOTAL_MODBUS_DATA];
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

struct pressureSensor		sensor_PRx[5];

/*le variabili globali sottostanti usate perr la pressioner, potrebbero diventare solo 5 variabili della struttura che c'è sopra*/

word PR_OXYG_ADC;			 	//variabile globale per il valore ADC del sensore di pressione ossigenatore --> PTC10
word PR_OXYG_mmHg;			 	//variabile globale per il valore in mmHg del sensore di pressione ossigenatore
word PR_OXYG_mmHg_Filtered;  	//variabile globale per il valore in mmHg del sensore di pressione ossigenatore filtrato

word PR_LEVEL_ADC;			 	//variabile globale per il valore ADC del sensore di pressione di livello vaschetta --> PTC11
word PR_LEVEL_mmHg;			 	//variabile globale per il valore in mmHg del sensore di pressione di livello vaschetta
word PR_LEVEL_mmHg_Filtered; 	//variabile globale per il valore in mmHg del sensore di pressione di livello vaschetta filtrato

word PR_ADS_FLT_ADC;			//variabile globale per il valore ADC del sensore di pressione del filtro assorbente --> PTB11
word PR_ADS_FLT_mmHg;			//variabile globale per il valore in mmHg del sensore di pressione del filtro assorbente
word PR_ADS_FLT_mmHg_Filtered;	//variabile globale per il valore in mmHg del sensore di pressione del filtro assorbente filtrato

word PR_VEN_ADC;				//variabile globale per il valore ADC del sensore di pressione Venoso --> PTB6
word PR_VEN_mmHg;				//variabile globale per il valore in mmHg del sensore di pressione Venoso
word PR_VEN_mmHg_Filtered;		//variabile globale per il valore in mmHg del sensore di pressione Venoso filtrato

word PR_ART_ADC;				//variabile globale per il valore ADC del sensore di pressione arteriosa --> PTB7
word PR_ART_mmHg;				//variabile globale per il valore in mmHg del sensore di pressione arteriosa
word PR_ART_mmHg_Filtered;		//variabile globale per il valore in mmHg del sensore di pressione arteriosa filtrato

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

struct temperatureSensor	sensor_TMx[3];


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
	word errorNACK;
	word errorPEC;
};

struct tempIRSensor sensorIR_TM[3];
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
	float Inst_Flow_Value;
	float Average_Flow_Val;
	float Temperature;
	float Accumulated_Volume_ul;
	unsigned char Bubble_Alarm;
	unsigned char Error_during_Bubble_Detection;
	unsigned char Error_In_Flow_Meas;
	unsigned char Error_In_Temp_Meas;
	unsigned char Device_Fault;
};
#define MASK_Bubble_Alarm					0x01
#define MASK_Error_during_Bubble_Detection	0x10
#define MASK_Error_In_Flow_Meas				0x20
#define MASK_Error_In_Temp_Meas				0x40
#define MASK_Device_Fault					0x80

#define SAMPLE 								16			//numero di campioni su cui mediare il flusso istantaneo letto
#define TOT_UF_SENSOR 						2
#define BYTE_COUNT_GET_VAL_CODE 			0x17 		//numero di byte che mi aspetto in ricezione col comando di GET_VAL_CODE customizzato con 3 byte di richesta 0x82 -- 0x88 -- 0x8B
float buffer_flow_value [TOT_UF_SENSOR][SAMPLE];

struct ultrsndFlowSens sensor_UFLOW[2];
struct ultrsndFlowSens * ptrCurrent_UFLOW; /* puntatore a struttura corrente - sensore attualmente interrogato */
struct ultrsndFlowSens * ptrMsg_UFLOW; 	   /* puntatore utilizzato per spedire il messaggio */
/************************************************************************/
/* 						SENSORI 										*/
/************************************************************************/

/************************************************************************/
/* 						PULSANTI GUI 									*/
/************************************************************************/
#define GUI_BUTTON_NULL			0x00
//#define GUI_BUTTON_PRESSED		0x01
//#define GUI_BUTTON_RELEASED		0x00
// (FM) ho scambiato gli eventi perche' ora si devono prendere in considerazione solo gli eventi
// di release
#define GUI_BUTTON_PRESSED		0x00
#define GUI_BUTTON_RELEASED		0x01

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
	PAR_SET_TEMPERATURE = 0xA0,
	PAR_SET_PRESS_ART_TARGET = 0xB1,
	PAR_SET_DESIRED_DURATION = 0xB3,
	PAR_SET_MAX_FLOW_PERFUSION = 0xB7,
	PAR_SET_PRESS_VEN_TARGET = 0xC2,
	PAR_SET_PURIF_FLOW_TARGET = 0xD3,
	PAR_SET_PURIF_UF_FLOW_TARGET = 0xE4,
	PAR_SET_WORD_END_NUMBER = PAR_SET_PURIF_UF_FLOW_TARGET
};

struct parWordSetFromGUI{
	char id;
	word value;
};
struct parWordSetFromGUI parameterWordSetFromGUI[PAR_SET_WORD_END_NUMBER];


/*struct parFloatSetFromGUI{
	char id;
	float value;
};
struct parFloatSetFromGUI parameterFloatSetFromGUI[PAR_SET_FLOAT_END_NUMBER];*/
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
	unsigned short dataButtonSBCReadyFlag;
	unsigned short dataParamSetSBCReadyFlag;
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
unsigned int timerCounterADC;
int timerCounterPID;
int timerCounterMState;
int timerCounterModBus;
int timerCounterUFlowSensor;
int timerCounterPeltier;
int timerCounterCheckModBus;
int timerCounterCheckTempIRSens;
int timerCounterLedBoard;

/************************************************************************/
/* 					STRUTTURA VOLUMI TRATTAMENTO 						*/
/************************************************************************/

/************************************************************************/
/* 					VARIABILI CANALI ADC		 						*/
/************************************************************************/

word DipSwitch_0_ADC;		//Variabile globale col valore ADC del DIP_SWITCH_1
word DipSwitch_1_ADC;		//Variabile globale col valore ADC del DIP_SWITCH_2
word DipSwitch_2_ADC;		//Variabile globale col valore ADC del DIP_SWITCH_3

#define DipSwitch_0_ADC_CHANNEL 	4
#define DipSwitch_1_ADC_CHANNEL 	5
#define DipSwitch_2_ADC_CHANNEL 	13

word V24_P1_CHK_ADC;
word V24_P1_CHK_VOLT;
word V24_P2_CHK_ADC;
word V24_P2_CHK_VOLT;

/*V24_P1_CHK:	 to 24 Volt --> 49764 ADC count; to 22 Volt 45576 ADC count*/
#define V24_P1_CHK_GAIN 		0.00047755492
#define V24_P1_CHK_OFFSET		0.25

/*V24_P2_CHK:	 to 24 Volt --> 49600 ADC count; to 22 Volt 45523 ADC count*/
#define V24_P2_CHK_GAIN 		0.00049055678
#define V24_P2_CHK_OFFSET		-0.33

#define V24_P2_CHK_ADC_CHANNEL 		10
#define V24_P1_CHK_ADC_CHANNEL 		11


#define PR_OXYG_ADC_CHANNEL		0
#define PR_LEVEL_ADC_CHANNEL	1
#define PR_ADS_FLT_ADC_CHANNEL 	2
#define PR_VEN_ADC_CHANNEL		4
#define PR_ART_ADC_CHANNEL		5

char ON_NACK_IR_TM;			//variabile globale che viene messa ad 1 se ricevo un NACK da un sensore di Temp IR

unsigned char END_ADC0;
unsigned char END_ADC1;

unsigned char OK_START;
unsigned char CHANGE_ADDRESS_IR_SENS;

#define BUTTON_1 0x01
#define BUTTON_2 0x02
#define BUTTON_3 0x03
#define BUTTON_4 0x04


/*PR_OXYG Sensor calibration:	 to 0 mmHg --> 19687 ADC count; to 50 mmHg 21439 ADC count*/
#define PR_OXYG_GAIN 		0.028539
#define PR_OXYG_OFFSET		19687

/*PR_LEVEL Sensor calibration:	 to 0 mmHg --> 18694 ADC count; to 20 mmHg 26650 ADC count*/
#define PR_LEVEL_GAIN 		0.00251383
#define PR_LEVEL_OFFSET		18694

/*PR_ADS_FLT Sensor calibration: to 0 mmHg --> 19785 ADC count; to 50 mmHg 21561 ADC count*/
#define PR_ADS_FLT_GAIN		0.028153
#define PR_ADS_FLT_OFFSET	19785

/*PR_VEN Sensor calibration:	 to 0 mmHg --> 19624 ADC count; to 50 mmHg 22231 ADC count*/
#define PR_VEN_GAIN 		0.019179
#define PR_VEN_OFFSET		19624

/*PR_ART Sensor calibration: 	 to 0 mmHg --> 19672 ADC count; to 50 mmHg 22347 ADC count*/
#define PR_ART_GAIN 		0.018691
#define PR_ART_OFFSET		19672

#define FREQ_DEBUG_LED 	10
#define SERVICE 		0x01
#define TREAT			0x00

unsigned char Air_1_Status;				//variabile globale per vedere lo stato del sensore di aria SONOTEC; può assumere valire AIR opp LIQUID
unsigned char slvAddr;					//variabile globale per l'indirizzo degli attuatori: FIRST_ACTUATOR = 0x02, LAST_ACTUATOR = 0x08
unsigned char * ptrDataTemperatureIR;

unsigned char Prescaler_Tick_Timer;

/**/

/**/

typedef enum{KidneyTreat = 0, LiverTreat} TREATMENT_TYPE;

#endif /* SOURCES_GLOBAL_H_ */
