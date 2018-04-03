/*
 * Global.h
 *
 *  Created on: 03/nov/2016
 *      Author: W15
 */

#ifndef SOURCES_GLOBAL_H_
#define SOURCES_GLOBAL_H_

// TODO commentare questo define nella versione definitiva perche' deve essere usato solo per il
// debug con il software di service fatto da Luca.
//#define DEBUG_WITH_SERVICE_SBC 0



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

#define DEBUG_MACHINE_STATE		0x06
#define DEBUG_CONTROL			0xA5
#define DEBUG_PROTECTION		0x5A
#define DEBUG_LOG_PC			0x01
#define DEBUG_TREATMENT			0xA5 //alternativa a DEBUG_COMM_SBC
//#define DEBUG_I2C_TEMP_SENS		0x01
/*#define DEBUG_FLOW_SENS			0x00*/ /*0x01*/
/* DEBUG */

#define SERVICE_ACTIVE_TOGETHER_THERAPY

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
#define FIRST_PINCH					0x06
#define TOTAL_ACTUATOR				7
#define TOTAL_MODBUS_DATA			32
#define TOT_DATA_MODBUS_RECEIVED_PUMP	11
#define TOT_DATA_MODBUS_RECEIVED_PINCH	9
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
	STATE_PRIMING_WAIT,
	STATE_PRIMING_RICIRCOLO,
	STATE_WAIT_TREATMENT,
	STATE_EMPTY_DISPOSABLE,
	STATE_UNMOUNT_DISPOSABLE,
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
	STATE_PRIMING_PH_1_WAIT,
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
	PARENT_TREAT_WAIT_START,
	PARENT_TREAT_KIDNEY_1_PUMP_ON,
	PARENT_TREAT_WAIT_PAUSE,
	PARENT_TREAT_KIDNEY_1_ALARM,
	PARENT_TREAT_KIDNEY_1_AIR_FILT,
	PARENT_TREAT_KIDNEY_1_SFV,
	PARENT_TREAT_KIDNEY_1_SFA,
	PARENT_TREAT_KIDNEY_1_ALM_AIR_REC,
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
	PARENT_EMPTY_DISPOSABLE_INIT,
	PARENT_EMPTY_DISPOSABLE_RUN,
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
	// Questo stato viene usato per codificare lo stato di pausa del priming (attivato
	// mediante il tasto BUTTON_STOP_PRIMING) quando gli stati della macchina sono del
	// gruppo PARENT_PRIMING_TREAT_KIDNEY_1....
	PARENT_PRIM_WAIT_PAUSE,
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
	CHILD_PRIM_ALARM_1_WAIT_CMD,
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
	CHILD_TREAT_ALARM_1_SAF_AIR_FILT,
	CHILD_TREAT_ALARM_1_SFV_AIR,
	CHILD_TREAT_ALARM_1_SFA_AIR,
	CHILD_TREAT_ALARM_1_WAIT_CMD,
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
	GUARD_ABANDON_PRIMING,
	GUARD_ENABLE_PRIMING_WAIT,
	GUARD_ENABLE_WAIT_TREATMENT,
	GUARD_ENABLE_UNMOUNT_END,
	GUARD_ENABLE_STATE_PRIMING_PH_1_WAIT,
	GUARD_FILTER_INSTALLED,
	GUARD_ENT_PAUSE_STATE_PRIM_KIDNEY_1,
	GUARD_ENABLE_STATE_KIDNEY_1_PRIM_RUN,
	/*********************/
	/* STATE LEVEL GUARD */
	/*********************/

	/*********************/
	/* PARENT LEVEL GUARD */
	/*********************/
	GUARD_ALARM_ACTIVE,
	GUARD_ALARM_AIR_FILT_RECOVERY,
	GUARD_ALARM_AIR_SFV_RECOVERY,
	GUARD_ALARM_AIR_SFA_RECOVERY,
	GUARD_AIR_RECOVERY_END,
	GUARD_ALARM_WAIT_CMD_TO_EXIT,
	GUARD_ENABLE_STATE_KIDNEY_1_PUMP_ON,
	GUARD_ENABLE_STATE_TREAT_KIDNEY_1_INIT,
	GUARD_ENT_PAUSE_STATE_KIDNEY_1_PUMP_ON,
	GUARD_ENT_PAUSE_STATE_TREAT_KIDNEY_1_INIT,



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
	GUARD_ALARM_SAF_AIR_FILT,
	GUARD_ALARM_SFV_AIR,
	GUARD_ALARM_SFA_AIR,
	GUARD_ALARM_STOP_ALL_ACT_WAIT_CMD,
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

struct machineStateGuard currentGuard[GUARD_END_NUMBER + 1]; /* equal to GUARD_END_NUMBER + 1 */

struct machineStateGuard * ptrPreviousGuard;
struct machineStateGuard * ptrCurrentGuard[GUARD_END_NUMBER + 1]; /* equal to GUARD_END_NUMBER + 1 */
struct machineStateGuard * ptrFutureGuard;


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
	unsigned short	secActType;		/* alarm security action: type of secuirty action required
	                                   (FM modificato da char ad unsigned short perche' ho bisogno di piu' bit per distinguere i vari allarmi*/
	unsigned char	priority;		/* alarm priority: low, medium, right */
	unsigned short	entryTime;		/* entry time in ms */
	unsigned short	exitTime;		/* exit time in ms */
	unsigned char	ovrdEnable;		/* override enable: alarm can be overridden when alarm condition is still present */
	unsigned char	resettable;		/* reset property */
	unsigned char	silence;		/* silence property: the alarm acoustic signal can be silenced for a limited period of time */
	unsigned char	memo;			/* memo property: the system remain in the alarm state even if the alarm condition is no longer present */
	void (*prySafetyActionFunc)(void); /* safety action: funzione che esegue la funzione di sicurezza in base alla priorit� dell'allarme */
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
	char StopEnable;
	char countMsgSent;

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
	float					actualSpeedOld;
	int                     newSpeedValue; // nuovo valore di velocita' arrivato prima della conclusione della scrittura precedente
	unsigned char           ReadRequestPending;  // e' arrivata una richiesta di lettura della velocita' della pompa
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
unsigned char CountErrorModbusMSG[8];
/* Pinch */
/************************************************************************/
/* 						ACTUATOR 										*/
/************************************************************************/

/************************************************************************/
/* 						SENSORI 										*/
/************************************************************************/


typedef enum
{
	OXYG = 0,
	LEVEL,
	ADS_FLT,
	VEN,
	ART,
	TOTAL_PPRESS_SENS

}Press_sens;


/* SENSORI PRESSIONE */
/* value = gain*(adc_val - offsetVal) + Offset */
struct pressureSensor{
	float prSensOffset;
//	float prSensOffsetVal;
	float prSensGain;
//	float prSensValue;
//	int  prSensValueFilteredWA;
//	word  prSensAdc;
//	word * prSensAdcPtr;
//	float prSensValueOld;
//	word * (*readAdctPtr)(void);
};

struct ParSaveTO_EEPROM
{
	struct pressureSensor sensor_PRx[5];
	float  FlowSensor_Ven_Gain;
	float  FlowSensor_Ven_Offset;
	float  FlowSensor_Art_Gain;
	float  FlowSensor_Art_Offset;
	unsigned char EEPROM_Revision;
	word EEPROM_CRC;
};

struct ParSaveTO_EEPROM config_data;

#define EEPROM_REVISION 0x01

/*le variabili globali sottostanti usate perr la pressioner, potrebbero diventare solo 5 variabili della struttura che c'� sopra*/

word  PR_OXYG_ADC;			 	//variabile globale per il valore ADC del sensore di pressione ossigenatore --> PTC10
word  PR_OXYG_mmHg;			 	//variabile globale per il valore in mmHg del sensore di pressione ossigenatore
word  PR_OXYG_mmHg_Filtered;  	//variabile globale per il valore in mmHg del sensore di pressione ossigenatore filtrato
word  PR_OXYG_ADC_Filtered;  	//variabile globale per il valore ADC del sensore di pressione ossigenatore filtrato

word  PR_LEVEL_ADC;			 	//variabile globale per il valore ADC del sensore di pressione di livello vaschetta --> PTC11
word  PR_LEVEL_mmHg;			//variabile globale per il valore in mmHg del sensore di pressione di livello vaschetta
word  PR_LEVEL_mmHg_Filtered; 	//variabile globale per il valore in mmHg del sensore di pressione di livello vaschetta filtrato
word  PR_LEVEL_ADC_Filtered;  	//variabile globale per il valore ADC del sensore di pressione di livello filtrato

word  PR_ADS_FLT_ADC;			//variabile globale per il valore ADC del sensore di pressione del filtro assorbente --> PTB11
word  PR_ADS_FLT_mmHg;			//variabile globale per il valore in mmHg del sensore di pressione del filtro assorbente
word  PR_ADS_FLT_mmHg_Filtered;	//variabile globale per il valore in mmHg del sensore di pressione del filtro assorbente filtrato
word  PR_ADS_FLT_ADC_Filtered;  	//variabile globale per il valore ADC del sensore di pressione del filtro assorbente filtrato

word  PR_VEN_ADC;				//variabile globale per il valore ADC del sensore di pressione Venoso --> PTB6
word  PR_VEN_mmHg;				//variabile globale per il valore in mmHg del sensore di pressione Venoso
word  PR_VEN_mmHg_Filtered;		//variabile globale per il valore in mmHg del sensore di pressione Venoso filtrato
word  PR_VEN_ADC_Filtered;  	//variabile globale per il valore ADC del sensore di pressione Venoso filtrato
word  PR_VEN_Diastolyc_mmHg;	//variabile globale per il valore diastolico  in mmHg del sensore di pressione venosa
word  PR_VEN_Sistolyc_mmHg;	    //variabile globale per il valore sistolico  in mmHg del sensore di pressione venosa
word  PR_VEN_Med_mmHg;			//variabile globale per il valore medio in mmHg del sensore di pressione venosa calcolato come (2 *PR_OXYG_Sistolyc_mmHg + PR_OXYG_Diastolyc_mmHg)/3
int  PR_VEN_Diastolyc_mmHg_ORG;	//variabile globale per il valore diastolico  in mmHg del sensore di pressione venosa stimato sull'organo
int  PR_VEN_Sistolyc_mmHg_ORG;	    //variabile globale per il valore sistolico  in mmHg del sensore di pressione venosa stimato sull'organo
int  PR_VEN_Med_mmHg_ORG;			//variabile globale per il valore medio in mmHg del sensore di pressione venosa calcolato come (2 *PR_OXYG_Sistolyc_mmHg + PR_OXYG_Diastolyc_mmHg)/3 stimato sull'organo



word  PR_ART_ADC;				//variabile globale per il valore ADC del sensore di pressione arteriosa --> PTB7
word  PR_ART_mmHg;				//variabile globale per il valore in mmHg del sensore di pressione arteriosa
word  PR_ART_mmHg_Filtered;		//variabile globale per il valore in mmHg del sensore di pressione arteriosa filtrato
word  PR_ART_ADC_Filtered;  	//variabile globale per il valore ADC del sensore di pressione Arterioso filtrato
word  PR_ART_Diastolyc_mmHg;	//variabile globale per il valore diastolico  in mmHg del sensore di pressione arteriosa
word  PR_ART_Sistolyc_mmHg;	    //variabile globale per il valore sistolico  in mmHg del sensore di pressione arteriosa
word  PR_ART_Med_mmHg;			//variabile globale per il valore medio in mmHg del sensore di pressione arteriosa calcolato come (2 *PR_OXYG_Sistolyc_mmHg + PR_OXYG_Diastolyc_mmHg)/3
int   PR_ART_Diastolyc_mmHg_ORG; //variabile globale per il valore diastolico  in mmHg del sensore di pressione arteriosa
int   PR_ART_Sistolyc_mmHg_ORG;	 //variabile globale per il valore sistolico  in mmHg del sensore di pressione arteriosa
int   PR_ART_Med_mmHg_ORG;		 //variabile globale per il valore medio in mmHg del sensore di pressione arteriosa calcolato come (2 *PR_OXYG_Sistolyc_mmHg + PR_OXYG_Diastolyc_mmHg)/3


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
	unsigned char ErrorMSG;
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
	unsigned char RequestMsgProcessed;  //la incremento quando mand o il messaggio, la decremento quando ricevo al risposta; se supera 25 do allarme
};
#define MASK_Bubble_Alarm					0x01
#define MASK_Error_during_Bubble_Detection	0x10
#define MASK_Error_In_Flow_Meas				0x20
#define MASK_Error_In_Temp_Meas				0x40
#define MASK_Device_Fault					0x80

#define SAMPLE 								64          // se uso un campinamento di 50 msec
//#define SAMPLE 							16 		    //numero di campioni su cui mediare il flusso istantaneo letto
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
	BUTTON_PINCH_2WPVF_RIGHT_OPEN = 0xA0,   // pinch filter (pinch in basso aperto a destra)
	BUTTON_PINCH_2WPVF_LEFT_OPEN = 0xA1,    // pinch filter (pinch in basso aperto a sinistra)
	BUTTON_PINCH_2WPVF_BOTH_CLOSED = 0xA2,  // pinch filter (pinch in basso entrambi chiusi)
	BUTTON_PINCH_2WPVA_RIGHT_OPEN = 0xA3,   // pinch arterial (pinch di sinistra - aperto a destra)
	BUTTON_PINCH_2WPVA_LEFT_OPEN = 0xA4,    // pinch arterial (pinch di sinistra - aperto a sinistra)
	BUTTON_PINCH_2WPVA_BOTH_CLOSED = 0xA5,  // pinch arterial (pinch di sinistra - entrambi chiusi)
	BUTTON_PINCH_2WPVV_RIGHT_OPEN = 0xA6,   // pinch venous (pinch di destra - aperto a destra)
	BUTTON_PINCH_2WPVV_LEFT_OPEN = 0xA7,    // pinch venous (pinch di destra - aperto a sinistra)
	BUTTON_PINCH_2WPVV_BOTH_CLOSED = 0xA8,  // pinch venous (pinch di destra - entrambi chiusi)

	BUTTON_CONFIRM = 0xB1,
	BUTTON_RESET = 0xB3,
	BUTTON_PRIMING_END_CONFIRM = 0xB4,
	BUTTON_PRIMING_FILT_INS_CONFIRM = 0xB5,
	BUTTON_PRIMING_ABANDON = 0xB6,
	BUTTON_START_PRIMING = 0xB7,
	BUTTON_STOP_PRIMING = 0xB8,
	BUTTON_STOP_ALL_PUMP = 0xB9,
	BUTTON_START_TREATMENT = 0xBA, // viene dato alla fine del ricircolo per far partire il trattamento
	BUTTON_STOP_TREATMENT = 0xBB,  // ferma le pompe e ferma il conteggio del trattamento
	BUTTON_END_TREATMENT = 0xBC,

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

	BUTTON_OVERRIDE_ALARM = 0xE0,
	BUTTON_RESET_ALARM = 0xE1,
	BUTTON_SILENT_ALARM = 0xE2,

	BUTTON_START_EMPTY_DISPOSABLE = 0xF0,    // comando per inizio svuotamento
	BUTTON_STOP_EMPTY_DISPOSABLE = 0xF1,     // ferma il processo di svuotamento
	BUTTON_UNMOUNT_END = 0xF2,               // comando per fine smontaggio, posso tornare in idle

	BUTTON_END_NUMBER = 39
};

struct buttonGUI{
	char id;
	unsigned char state;
};

// sono 256 strutture perche' il numero di massimo di bottoni che posso gestire e' 0..255
struct buttonGUI buttonGUITreatment[256];
/************************************************************************/
/* 						PULSANTI GUI 									*/
/************************************************************************/


/************************************************************************/
/* 					PARAMETRI DA IMPOSTARE TRAMITE GUI 					*/
/************************************************************************/
enum paramWordSetFromSBC{
	PAR_SET_PRIMING_VOL_PERFUSION = 0x10,
	PAR_SET_THERAPY_TYPE = 0x20,
	PAR_SET_PRIMING_VOL_PURIFICATION = 0x30,       // ml
	PAR_SET_PRIMING_TEMPERATURE_PERFUSION = 0x40,  // temperatura in �C moltiplicata per 10
	PAR_SET_PRIMING_VOL_OXYGENATION = 0x50,        // ml
	PAR_SET_OXYGENATOR_ACTIVE = 0x60,              // 1 = no, 2 = si, 0 = undefined
	PAR_SET_OXYGENATOR_FLOW = 0x70,                // ml/min
	PAR_SET_DEPURATION_ACTIVE = 0x80,              // 1 = no, 2 = si, 0 = undefined
	PAR_SET_TEMPERATURE = 0xA0,                    // temperatura in �C moltiplicata per 10
	PAR_SET_PRESS_ART_TARGET = 0xB1,               // mmHg
	PAR_SET_DESIRED_DURATION = 0xB3,
	PAR_SET_MAX_FLOW_PERFUSION = 0xB7,             // ml/min
	PAR_SET_PRESS_VEN_TARGET = 0xC2,
	PAR_SET_PURIF_FLOW_TARGET = 0xD3,
	PAR_SET_PURIF_UF_FLOW_TARGET = 0xE4,
	/*da qui in poi parametri non passati dal PC ma
	 * define sul source code definiti con 0xFX*/
	PAR_SET_VENOUS_PRESS_TARGET = 0xF1,
	PAR_SET_WORD_END_NUMBER = PAR_SET_VENOUS_PRESS_TARGET +1
};


struct parWordSetFromGUI{
	char id;
	word value;
};

// sono 256 strutture perche' il numero di massimo di parametri che posso gestire e' 0..255
struct parWordSetFromGUI parameterWordSetFromGUI[256];


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
unsigned int timerCounterADC0;
unsigned int timerCounterADC1;
int timerCounterPID;
int timerCounterMState;
int FreeRunCnt10msec;
int timerCounterModBus;
int timerCounterUFlowSensor;
int timerCounterPeltier;
int timerCounterCheckModBus;
int timerCounterCheckTempIRSens;
int timerCounterLedBoard;
int timerCounterUpdateTargetPressurePid;
int timerCounterUpdateTargetPressPidArt;

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

#define PR_OXYG_GAIN_DEFAULT 		0.0277778
#define PR_OXYG_OFFSET_DEFAULT		-549.583

#define PR_LEVEL_GAIN_DEFAULT 		0.000535045
#define PR_LEVEL_OFFSET_DEFAULT		-10.2772

#define PR_ADS_FLT_GAIN_DEFAULT		0.0272547
#define PR_ADS_FLT_OFFSET_DEFAULT	-537.517

#define PR_VEN_GAIN_DEFAULT 		0.00658244
#define PR_VEN_OFFSET_DEFAULT		-128.147

#define PR_ART_GAIN_DEFAULT 		0.00657
#define PR_ART_OFFSET_DEFAULT		-127.287

/*valore di gain e offset del sensore di flusso venoso calcolati sperimentalmente con prove di flusso con la bilancia*/
#define GAIN_FLOW_SENS_VEN		0.7722f
#define OFFSET_FLOW_SENS_VEN		25
#define GAIN_FLOW_SENS_ART      0.7678f
#define OFFSET_FLOW_SENS_ART		56

#define FREQ_DEBUG_LED 	10
#define SERVICE 		0x01
#define TREAT			0x00

unsigned char Air_1_Status;				//variabile globale per vedere lo stato del sensore di aria SONOTEC; pu� assumere valire AIR opp LIQUID
unsigned char slvAddr;					//variabile globale per l'indirizzo degli attuatori: FIRST_ACTUATOR = 0x02, LAST_ACTUATOR = 0x09, Addr 0x06 not used (last pump 0x05, first pinch 0x07, last pinch 0x09)
unsigned char * ptrDataTemperatureIR;

unsigned char Prescaler_Tick_Timer;
unsigned char Prescaler_Tick_TEST;

/**/

/**/
int pollingDataFromSBC;
int pollingDataToSBC;
unsigned char codeDBG;
unsigned char subcodeDBG;
bool Service;

bool PANIC_BUTTON_ACTIVATION;

// durata globale del trattamento in secondi
unsigned long TreatDuration;
unsigned long TotalTreatDuration;
// durata globale del prtiming in secondi
unsigned long PrimingDuration;
unsigned long TotalPrimingDuration;

typedef enum{Undef = 0, KidneyTreat = 0x10, LiverTreat = 0x50} THERAPY_TYPE;
typedef enum{NOT_DEF = 0, NO = 1, YES = 2} PARAMETER_ACTIVE_TYPE;

// volume massimo in ml trasferito nel reservoir
#define MAX_VOLUME_RESERVOIR     2000
// volume minimo in ml trasferito nel reservoir
#define MIN_VOLUME_RESERVOIR     1000

// temperatura massima nel priming in �C
#define MAX_TEMP_PRIMING         37
// temperatura minima nel priming in �C
#define MIN_TEMP_PRIMING         4

#define MAX_FLOW_ART_KIDNEY		500
#define MAX_FLOW_ART_LIVER		400
#define MIN_FLOW_ART_KIDNEY		50
#define MIN_FLOW_ART_LIVER		50
#define PUMP_ART_GAIN			9 /* ml/GIRO */
#define PUMP_OXY_GAIN			9


// percentuale del priming per l'inserimento del filtro
#define PERC_OF_PRIM_FOR_FILTER    95

// fattore di conversione del flusso in giri al minuto per le pompe dell'ossigenatore
//#define OXYG_FLOW_TO_RPM_CONV 18.3
#define OXYG_FLOW_TO_RPM_CONV 22.0

// definisco il fattore di conversione tra velocita' in rpm/min in ml/interval dove
// interval e' il periodo su cui calcolo la quantita' di liquido pompata.
// Quantita' di liquido pompata nell'intervallo di 450 msec.
// 1 / 60 * 0.450 * 9.3
#define CONV_RPMMIN_TO_ML_PER_INTERVAL  (float)0.06975
// 1 / 60 * 0.450 * 11.0
#define CONV_RPMMIN_TO_ML_PER_INT_OXYG  (float)0.0825
// valore originale del codice
//#define CONV_RPMMIN_TO_ML_PER_INTERVAL  (float)0.00775
// intervallo di tempo in msec per cui devo vedere la temperatura di ricircolo raggiunta prima di chiudere lo stato
// e passare  all trattamento
#define TIMEOUT_TEMPERATURE_RICIRC 2000L

/*
 *  * -----------------CON SPEZZONI POMPA DEL DISPOSABLE ORIGINALI
Abbiamo fatto andare le pompe ad una velocit� fissa da 100 RPM, scoprendo che
la pressione media misurata sul sensore era di circa 130 mmHg.
A questo punto, supponendo pressione atmosferica sull'organo, visto che iul tubo
di uscita � stato lasciato in aria, abbiamo imposto il set point del pid a 120 mmHg.

Usando solo la formula proporzionale:
deltaSpeed = K * errore
Abbiamo trovato il minimo valore di K che mandava in oscillazione la pressione:
Ku = 0.05.
Il periodo dell'oscillazione Pu � stato trovato in 0.4 sec.
a questo punto abbiamo trovato i tre coefficienti del PID:

Kp = 0.6 * Ku = 0.03
Ki = 2Kp/Pu   = 0.15
Kd = Kp*Pu/8  = 0.0015

#define PID_KP_VENOSA (float)0.03
#define PID_KI_VENOSA (float)0.15
#define PID_KD_VENOSA (float)0.0015
*/


/*
 * -----------------CON SPEZZONI POMPA IMN SILICONE PER ALTI FLUSSI
Abbiamo fatto andare le pompe ad una velocit� fissa da 100 RPM, scoprendo che
la pressione media misurata sul sensore era di circa 185 mmHg.
A questo punto, supponendo pressione atmosferica sull'organo, visto che il tubo
di uscita � stato lasciato in aria, abbiamo imposto il set point del pid a 70 mmHg.
per farlo partire da un valore intermedio

Usando solo la formula proporzionale:
deltaSpeed = K * errore
Abbiamo trovato il minimo valore di K che mandava in oscillazione la pressione:
Ku = 0.1.
Il periodo dell'oscillazione Pu � stato trovato in 2.5 sec.
a questo punto abbiamo trovato i tre coefficienti del PID:

Kp = 0.6 * Ku = 0.06
Ki = 2Kp/Pu   = 0.048
Kd = Kp*Pu/8  = 0.01875
*/
// valori usati con il disposable definitivo (calcolati con Vincenzo sul vecchio disposable)
//#define parKITC_Ven 						0.048
//#define parKP_Ven 							0.06
//#define parKD_TC_Ven 						0.01875

//------------------------------------------------------------------------------------------
// parametri per il pid sulla venosa calcolati sul nuovo disposable (franco) e con il tempo
// di aggiornamento del pid di 450 msec
// ku = 0.8, Pu = 12 sec
#define parKITC_Ven 						0.16
#define parKP_Ven 							0.48
#define parKD_TC_Ven 						0.72

// parametri per il pid sulla arteriosa calcolati sul nuovo disposable (franco) e con il tempo
// di aggiornamento del pid di 450 msec
// ku = 0.5, Pu = 3 sec
#define parKITC_Art 						0.2
#define parKP_Art 							0.30
#define parKD_TC_Art 						0.1125
//------------------------------------------------------------------------------------------

// parametri per il nuovo pid sulla arteriosa (calcolati con Vincenzo sul vecchio disposable)
// ku = 0.05, Pu = 8 sec
//#define parKITC_Art 						0.008
//#define parKP_Art 							0.03
//#define parKD_TC_Art 						0.03

// Coefficienti del pid iniziale (calcolati da Davide)
//#define parKP_Art 							1.0
//#define parKITC_Art 						0.2
//#define parKD_TC_Art 						0.8

//----------------------------------------------------------------------------------------------------------
// i valori che seguono sono da considerare nel funzionamento normale
// volume in ml nel reservoir prima di far partire le pompe di ossigenazione e pompa depurazione nel fegato
#define MIN_LIQ_IN_RES_TO_START_OXY_VEN    400
// volume aggiuntivo in ml da considerare in priming per tenere conto
// del riempimento del disposable
#define VOLUME_DISPOSABLE  200
// quantita' di liquido scaricata prima di iniziare lo scaricamento del disposable
#define DISCHARGE_AMOUNT_ART_PUMP  1000
//----------------------------------------------------------------------------------------------------------


/*
//----------------------------------------------------------------------------------------------------------
// i valori che seguono sono da considerare per il debug interno e quindi devono essere commentati
// nel funzionamento normale e ripristinati quelli precedenti
// volume in ml nel reservoir prima di far partire le pompe di ossigenazione e pompa depurazione nel fegato
#define MIN_LIQ_IN_RES_TO_START_OXY_VEN    15
// volume aggiuntivo in ml da considerare in priming per tenere conto
// del riempimento del disposable
#define VOLUME_DISPOSABLE  10
// quantita' di liquido scaricata prima di iniziare lo scaricamento del disposable
#define DISCHARGE_AMOUNT_ART_PUMP  20
//----------------------------------------------------------------------------------------------------------
*/

// volume massimo caricabile in vaschetta in ml
#define MAX_LIQUID_AMOUNT 2500

// velocita' con cui faccio partire, per ora, la pompa di depurazione
#define LIVER_PPAR_SPEED 2000
// velocita' con cui faccio partire, per ora, la pompa di ossigenazione e perfusione venosa nel caso di priming
#define LIVER_PRIMING_PMP_OXYG_SPEED 2000
// definisce il valore massimo di giri che possono raggiungere le pompe di ossigenazione
#define MAX_OXYG_RPM  110
// numero di giri della pompa arteriosa durante lo scarico
#define KIDNEY_EMPTY_PPAR_SPEED 2000
// numero di giri della pompa di ossigenazione durante lo scarico
#define LIVER_PPAR_EMPTY_SPEED 2000
// definisce il valore massimo di giri che puo raggiungere la pompa arteriosa
#define MAX_ART_RPM  60


// velocita' con cui faccio partire, per ora, la pompa per l'espulsione dell'aria
#define AIR_REJECT_SPEED 4000
// tempo in msec necessario per espellere l'aria rilevata in msec
#define TIME_TO_REJECT_AIR 30000L

//start addres FLASH used as EEPROM
#define START_ADDRESS_EEPROM		0xFF000
//stop addres FLASH used as EEPROM
#define STOP_ADDRESS_EEPROM			0xFFFFF

// indice dell'array sensor_UFLOW corrispondente al sensore d'aria arterioso
#define ARTERIOUS_AIR_SENSOR 0
// indice dell'array sensor_UFLOW corrispondente al sensore d'aria venoso
#define VENOUS_AIR_SENSOR 1



typedef enum
{
	INIT_EMPTY_DISPOSABLE = 0,
	WAIT_FOR_1000ML,
	WAIT_FOR_AIR_ALARM,
	WAIT_FOR_LEVEL_OR_AMOUNT
}EMPTY_DISPOSABLE_STATE;

// usata per provare in debug alcuni allarmi.
// non serve nel funzionamento normale
unsigned char CheckAlarmFlag;


typedef enum
{
	SILENT  = 0,
	LOW 	= 1,
	MEDIUM 	= 2,
	HIGH   	= 3,
}BUZZER_LEVEL;

BUZZER_LEVEL LevelBuzzer;

// VARIABILI USATE DURANTE LA FASE DI MOUNTING
// numero di parametri ricevuti durante la fase di mounting
unsigned char ParamRcvdInMounting[4];
int AllParametersReceived;
// vale 1 se e' arrivato il comando da sbc per impostare il tipo di terapia
char TherapyCmdArrived;



// GESTIONE DEGLI STATI PER L'ELIMINAZIONE DELL'ALLARME ARIA-----------------------------------
typedef enum
{
	INIT_AIR_ALARM_RECOVERY = 0,
	START_AIR_PUMP,
	AIR_CHANGE_START_TIME,
	STOP_AIR_PUMP,
	AIR_REJECTED,
	AIR_REJECTED1
}AIR_ALARM_RECOVERY_STATE;

AIR_ALARM_RECOVERY_STATE AirAlarmRecoveryState;

// memorizza lo stato ptrCurrentParent->parent nel momento in cui comincio a cercare
// di uscire dallo stato di allarme aria
unsigned short AirParentState;

// Questa flag viene usata per disabilitare gli allarmi aria durante la fase di recupero da
// un allarme aria precedente
// E' STATA SOSTITUITA CON UNA FUNZIONE CON LO STESSO NOME
//bool DisableAllAirAlarm;

unsigned long StarTimeToRejAir;
// serve per misurare il tempo per l'eliminazione della bolla d'aria
unsigned long TotalTimeToRejAir;

//volume del liquido in vaschetta come percentuale rispetto al suo valore massimo
word LiquidAmount;

// allarme aria su filtro scattato, ma non ho ancora ricevuto il butto_reset per eliminarlo
bool TreatAlm1SafAirFiltActive;
// allarme aria venoso scattato, ma non ho ancora ricevuto il butto_reset per eliminarlo
bool TreatAlm1SFAActive;
// allarme aria arterioso scattato, ma non ho ancora ricevuto il butto_reset per eliminarlo
bool TreatAlm1SFVActive;


// GESTIONE RICIRCOLO VELOCE
typedef enum
{
	START_RECIRC_IDLE,
	START_RECIRC_HIGH_SPEED,    // inizio fase di ricircolo ad alta velocita'
	STOP_RECIRC_HIGH_SPEED,     // fine fase di ricircolo ad alta velocita'
	CALC_PUMPS_GAIN,
	TEMP_START_CHECK_STATE,     // inizio intervallo di controllo temperatura in range
	TEMP_CHECK_DURATION_STATE,  // controllo durata della temperatura in range
	TEMP_ABANDONE_STATE         // ho ricevuto il comando di abbandonare e tornare in idle
}TEMPERATURE_STATE;

typedef enum
{
	NO_TEMP_STATE_CMD ,
	RESTART_CMD,               // restart check sequence  o high speed fase
	TEMP_STATE_RESET,          // riporto la macchina a stati della temperatura nello stato iniziale
	TEMP_ABANDONE_CMD,         // e' stato premuto il tasto abbandona
	TEMP_START_RICIRCOLO
}TEMPERATURE_CMD;

// un minuto di ricircolo veloce per essere sicuro di eliminare tutta l'eventuale aria presente nei tubi
#define HIGH_PUMP_SPEED_DURATION 60000L

// velocita' delle pompe per la fase del ricircolo ad alta velocita'
#define RECIRC_PUMP_HIGH_SPEED 4000
#define RECIRC_PUMP_HIGH_SPEED_ART 2000


// GESTIONE PRIMING AGGIUNTIVO DA DEBUGGARE
typedef enum
{
	INIT_TOT_PRIM_VOL_STATE,
	NEW_TOT_PRIM_VOL_STATE
}TOTAL_PRIMING_VOL_STATE;

typedef enum
{
	NO_CMD_TOT_PRIM_VOL,
	NEW_PRIM_CMD_TOT_PRIM_VOL,
	RESET_CMD_TOT_PRIM_VOL
}TOTAL_PRIMING_VOL_CMD;

// pump gain calcolato in base ai sensori della macchina
float ArteriousPumpGainForPid;
float VenousPumpGainForPid;

#define DEFAULT_ART_PUMP_GAIN 9.3
#define DEFAULT_VEN_PUMP_GAIN 22.0

#define RPM_IN_PRIMING_PHASES  2000


typedef struct
{
	unsigned int EnableAllAlarms            : 1;    // POSIZIONE LIBERA
	unsigned int EnableLevHighAlarm         : 1;    // Abilito allarme di livello alto (troppo pieno)
	unsigned int EnableLevLowAlarm          : 1;    // Abilito allarme di livello basso
	unsigned int EnableCoversAlarm          : 1;    // Abilito allarme di cover
	unsigned int EnablePressSensLowAlm      : 1;    // abilito allarme pressione bassa
	unsigned int EnablePressSensHighAlm     : 1;    // abilito allarme pressione alta
	unsigned int EnableTempArtHighAlm       : 1;    // abilito allarme temperatura alta
	unsigned int EnableDeltaFlowArtAlarm    : 1;    // abilito allarme delta flusso arterioso troppo alto
	unsigned int EnableDeltaFlowVenAlarm    : 1;    // abilito allarme delta flusso venoso troppo alto
	unsigned int EnableDeltaTempRecVenAlarm : 1;    // abilito allarme delta temperatura recipiente e line venosa troppo alta
	unsigned int EnableDeltaTempRecArtAlarm : 1;    // abilito allarme delta temperatura recipiente e line arteriosa troppo alta

	unsigned int EnableSAFAir               : 1;    // abilito allarme aria su filtro
	unsigned int EnableSFVAir               : 1;    // abilito allarme aria su circuito venoso
	unsigned int EnableSFAAir               : 1;    // abilito allarme aria su circuito arterioso

	unsigned int TankLevelHigh              : 1;    // livello del liquido supera il massimo
	unsigned int ChildAlmAndWaitCmdActive   : 1;
}FLAGS_DEF;

typedef union
{
    FLAGS_DEF FlagsDef;
	unsigned int FlagsVal;
}GLOBAL_FLAGS;

GLOBAL_FLAGS GlobalFlags;

// 0 cover della pompa 0 aperto
// 1 cover della pompa 1 aperto
// 2 cover della pompa 2 aperto
// 3 cover della pompa 3 aperto
// 4 cover di tutte le pompe chiuse
unsigned char CoversState;

// quantita' massima di liquido in vaschetta espressa in valore percentuale rispetto al
// massimo di 2500 ml (MAX_LIQUID_AMOUNT)
#define MAX_LIQUID_LEV_IN_PERC   120

// quantita' minim di liquido in vaschetta espressa in valore percentuale rispetto al
// massimo di 2500 ml (MAX_LIQUID_AMOUNT)
#define MIN_LIQUID_LEV_IN_PERC   25

// massima differenza tra flusso arterioso teorico e flusso misurato al di
// sopra del quale do l'allarme
#define MAX_ART_FLOW_DIFF_FROM_CALC_AND_MIS 300.0


// massima differenza tra flusso venoso teorico e flusso misurato al di
// sopra del quale do l'allarme
#define MAX_VEN_FLOW_DIFF_FROM_CALC_AND_MIS 300.0

// massima differenza di temperatura tra recevoir e liquido arterioso
#define MAX_DELTA_TEMP_ART_AND_REC  10.0

// massima differenza di temperatura tra recevoir e liquido venoso
#define MAX_DELTA_TEMP_VEN_AND_REC  10.0

// Quando e' true vuol dire che lo start e' stato inviato
bool Peltier2On;
// Quando e' true vuol dire che lo start e' stato inviato
bool PeltierOn;

unsigned long PeltierDelay;

// stati del task di controllo delle peltier
typedef enum
{
	INIT_LIQTEMPCONTR_STATE,
	READ_LIQTEMPCONTR_STATE,
	WAIT_FOR_SET_T_LIQTEMPCONTR_STATE,
	PELT_NEW_TRGT_CMD_LIQTEMPCONTR_STATE,
	TEMP_START_CHECK_LIQTEMPCONTR_STATE,
	TEMP_CHECK_DUR_LIQTEMPCONTR_STATE
}LIQUID_TEMP_CONTR_STATE;

typedef enum
{
	NO_LIQUID_TEMP_CONTR_CMD,
	WAIT_FOR_NEW_TARGET_T
}LIQUID_TEMP_CONTR_CMD;


//-------------------------------------------------------------------------------
// usati nella funzione CheckPumpStopTask per il controllo delle pompe ferme
typedef enum
{
	CHECK_PUMP_STOP_IDLE,
	WAIT_FOR_NEW_READ,
	READ_PUMP_SPEED,
	PUMP_WRITE_ALARM,
	END_PROCESS
}CHECK_PUMP_STOP_STATE;

typedef enum
{
	NO_CHECK_PUMP_STOP_CMD,
	INIT_CHECK_SEQ_CMD,               // restart check sequence
	RESET_ALARM
}CHECK_PUMP_STOP_CMD;
//-------------------------------------------------------------------------------

// flag usata per disabilitare il task CheckPumpStopTask quando vengono
// usate delle funzioni di service che agiscono sulle pompe.
//  Verra' ripristinato alla ricezione del primo comando INIT_CHECK_SEQ_CM.
char DisableCheckPumpStopTask;

#endif /* SOURCES_GLOBAL_H_ */

