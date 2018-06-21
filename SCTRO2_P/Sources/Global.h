/*
 * Global.h
 *
 *  Created on: 15/giu/2016
 *      Author: W15
 */

#ifndef SOURCES_GLOBAL_H_
#define SOURCES_GLOBAL_H_

#include "PE_Types.h"
typedef unsigned short	word;

/* Syncronization flag */
char	iflag_pc_rx;
char	iflag_pc_tx;
char	iflag_pmp1_rx;
char	iflag_pmp1_tx;
char	iflag_sensTempIR_Meas_Ready;
char	iflag_sensTempIRRW;
unsigned char * ptrDataTemperatureIR;

/* DEBUG */
#define DEBUG_ENABLE
#define DEBUG_CONTROL			0xA5
#define DEBUG_PROTECTION		0x5A
/* DEBUG */

#define	IFLAG_PC_RX				0x01 /* new data on rx 232 pc */
#define IFLAG_PC_TX				0x01 /* new data to rx 232 pc */
#define IFLAG_PMP1_RX			0x01 /* new data on rx 422 pmp1 */
#define IFLAG_PMP2_RX			0x01 /* new data on rx 422 pmp2 */
#define IFLAG_PMP3_RX			0x01 /* new data on rx 422 pmp3 */
#define IFLAG_PMP4_RX			0x01 /* new data on rx 422 pmp4 */
#define IFLAG_PMP1_TX			0x01 /* new data on tx 422 pmp1 */
#define IFLAG_PMP2_TX			0x01 /* new data on tx 422 pmp2 */
#define IFLAG_PMP3_TX			0x01 /* new data on tx 422 pmp3 */
#define IFLAG_PMP4_TX			0x01 /* new data on tx 422 pmp4 */
#define IFLAG_IDLE				0x00 /* idle flag */
#define IFLAG_SENS_TEMPIR_TX		0x5A /* Message end trasmission */
#define IFLAG_SENS_TEMPIR_RX		0xA5 /* Message end reception */
#define IFLAG_SENS_TEMPIR_WAIT		0xCC /* Wait for rx/tx message */
#define IFLAG_SENS_TEMPIR_WRITE		0xEE /* write request */
#define IFLAG_IRTEMP_MEASURE_READY	0x01 /* valore di temperatura pronto da leggere */

/**/
#define BYTES_TO_WORD(hi,lo)  (unsigned int) (((unsigned int)(hi) << 8) | (unsigned int)(lo))
#define BYTES_TO_WORD_SIGN(hi,lo)  (int) (((int)(hi) << 8) | (int)(lo))

/* Communication Protocol Variable General */
unsigned char	pc_rx_data[16];				/* received byte from pc */
unsigned char	pc_tx_data[32];				/* transmitted byte to pc */

//AS1_TComData    msg_pmp1_tx[257];		/* modbus codified pmp1 message transmitted */
//AS1_TComData *  msg_pmp1_tx_ptr;		/* pointer to msg_pmp1_tx */
//AS1_TComData    msg_pmp1_rx[257];		/* modbus codified pmp1 message received */
//AS1_TComData *  msg_pmp1_rx_ptr;		/* pointer to msg_pmp1_rx */

unsigned char * ptr;
unsigned char * ptrDebug;
char ptrCount;

/************************************************************************/
/* 					VARIABILI CANALI ADC		 						*/
/************************************************************************/

word DipSwitch_0_ADC;		//Variabile globale col valore ADC del DIP_SWITCH_1
word DipSwitch_1_ADC;		//Variabile globale col valore ADC del DIP_SWITCH_2
word DipSwitch_2_ADC;		//Variabile globale col valore ADC del DIP_SWITCH_3

#define DipSwitch_0_ADC_CHANNEL 	4
#define DipSwitch_1_ADC_CHANNEL 	5
#define T_PLATE_P_ADC_CHANNEL		6
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

struct funcRetStruct
{
	unsigned char *  ptr_msg;
	unsigned char   mstreqRetStructNumByte;
	unsigned char * slvresRetPtr;
	unsigned char   slvresRetNumByte;
	} _funcRetVal;

struct funcRetStruct * _funcRetValPtr;

/* Data Structure */
typedef enum
{
	OXYG = 0,
	LEVEL,
	ADS_FLT,
	VEN,
	ART,
	TOTAL_PPRESS_SENS

}Press_sens;


typedef enum
{
	SILENT  = 0,
	LOW 	= 1,
	MEDIUM 	= 2,
	HIGH   	= 3,
}BUZZER_LEVEL;

BUZZER_LEVEL LevelBuzzer;


struct Pinch_Valve {
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
	STATE_NULL = 0x0000,
	STATE_ENTRY,
	STATE_IDLE,
	STATE_T1_NO_DISPOSABLE,
	STATE_T1_WITH_DISPOSABLE,
	STATE_PRIMING_TREAT_1,
	STATE_PRIMING_TREAT_2,
	STATE_TREATMENT_1,
	STATE_TREATMENT_2,
	STATE_EMPTY_DISPOSABLE_1,
	STATE_EMPTY_DISPOSABLE_2,
	STATE_WASHING,
	STATE_FATAL_ERROR,
	STATE_3,
	STATE_DEBUG_0,
	STATE_DEBUG_1,
	STATE_DEBUG_2,
	STATE_DEBUG_3,
	STATE_END_NUMBER,
};

enum Parent {
	PARENT_NULL = 0x0000,
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
	PARENT_DEBUG_0,
	PARENT_DEBUG_1,
	PARENT_DEBUG_2,
	PARENT_DEBUG_3,
	PARENT_END_NUMBER,
};

enum Child {
	CHILD_NULL = 0x0000,
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
	CHILD_DEBUG_0,
	CHILD_DEBUG_1,
	CHILD_DEBUG_2,
	CHILD_DEBUG_3,
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

struct machineStateGuard currentGuard[21]; /* equal to GUARD_END_NUMBER + 1 */

struct machineStateGuard * ptrPreviousGuard;
struct machineStateGuard * ptrCurrentGuard[21]; /* equal to GUARD_END_NUMBER + 1 */
struct machineStateGuard * ptrFutureGuard;

/* Machine state guard structure */

/* Machine state guard */
enum MachineStateGuardId {
	GUARD_NULL = 0x0000,
	/* STATE LEVEL GUARD */
	GUARD_START_ENABLE,
	GUARD_HW_T1T_DONE,
	GUARD_COMM_ENABLED,
	GUARD_THERAPY_SELECTED,
	GUARD_THERAPY_CONFIRMED,
	GUARD_T1_NO_DISP_END,
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
	/* STATE LEVEL GUARD */

	/* PARENT LEVEL GUARD */
	/* PARENT LEVEL GUARD */

	GUARD_FATAL_ERROR,

	GUARD_DEBUG_01,
	GUARD_DEBUG_12,
	GUARD_DEBUG_23,

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

typedef struct
{
	unsigned char PumpFilter_HSens1;
	unsigned char PumpFilter_HSens2;
	unsigned char PumpArt_Liver_HSens1;
	unsigned char PumpArt_Liver_HSens2;
	unsigned char PumpOxy_1_HSens1;
	unsigned char PumpOxy_1_HSens2;
	unsigned char PumpOxy_2_HSens1;
	unsigned char PumpOxy_2_HSens2;
	unsigned char PinchFilter_Left;
	unsigned char PinchFilter_Right;
	unsigned char PinchArt_Left;
	unsigned char PinchArt_Right;
	unsigned char PinchVen_Left;
	unsigned char PinchVen_Right;

}ActuatorHallStatus;

ActuatorHallStatus HallSens;

/*le variabili globali sottostanti usate perr la pressioner, potrebbero diventare solo 5 variabili della struttura che c'è sopra*/

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
word  T_PLATE_P_ADC;			// variabile globale per il valore in ADC della temperatura di piastra letta dalla protective
float  T_PLATE_P_GRADI_CENT;		// variabile globale per il valore in gradi centigradi della temperatura di piastra letta dalla protective
int   PR_ART_Diastolyc_mmHg_ORG; //variabile globale per il valore diastolico  in mmHg del sensore di pressione arteriosa
int   PR_ART_Sistolyc_mmHg_ORG;	 //variabile globale per il valore sistolico  in mmHg del sensore di pressione arteriosa
int   PR_ART_Med_mmHg_ORG;		 //variabile globale per il valore medio in mmHg del sensore di pressione arteriosa calcolato come (2 *PR_OXYG_Sistolyc_mmHg + PR_OXYG_Diastolyc_mmHg)/3

unsigned char Air_1_Status;				//variabile globale per vedere lo stato del sensore di aria SONOTEC; può assumere valire AIR opp LIQUID

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

/*valori di GAIN e OFFSET calcolati con: R1 = 931  ohm  = -17.9°C a cui corrispondono 21345 ADC
 * 										 R2 = 1238 ohm = 61.8 °C   a cui corrispondono 51464 ADC
 * 										 le temperature le ho considerate tutte moltiplicate per 10
 * 										 per avere la risoluzione del decimo di grado
 * con una pt100 montata e una temp misurata con termometro esterno pari a 25.6 si è visto
 * che era necessario ridurre l'offset di altri 2.6 °C (da -743.8  a -769.8)							 */
//#define GAIN_T_PLATE_SENS			0.016864
//#define OFFSET_T_PLATE_SENS			-384.935

/*valori di GAIN e OFFSET calcolati con: R1 = 931  ohm  = -10.8°C a cui corrispondono 17535 ADC
 * 										 R2 = 1102.5 ohm = 26.6 °C   a cui corrispondono 40927 ADC
 * 										 le temperature le ho considerate tutte moltiplicate per 10
 * 										 per avere la risoluzione del decimo di grado							 */

#define GAIN_T_PLATE_SENS_COLD			0.015988
#define OFFSET_T_PLATE_SENS_COLD		-388.3496

bool Heat_ON;	//variabile globale che mi dice se è partito il riscaldatore

/*valori di GAIN e OFFSET ricalcolati con in data 19-6-2018 con pt100 in classe A poggiata sulla piastra:
 * 				 R1 = 1131  ohm  = 34°C a cui corrispondono 43637 ADC
 * 				 R2 = 1171  ohm  = 44.4 °C   a cui corrispondono 46752 ADC
 * 				 le temperature le ho considerate tutte moltiplicate per 10
 * 				 per avere la risoluzione del decimo di grado							 */

bool Frigo_ON;	//variabile globale che mi dice se è partito il firgo

#define GAIN_T_PLATE_SENS_HEAT			0.033386838
#define OFFSET_T_PLATE_SENS_HEAT			-1117

#define AIR							0x00
#define LIQUID						0x01

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
	float  T_Plate_Sensor_Gain_cold;
	float  T_Plate_Sensor_Offset_cold;
	float  T_Plate_Sensor_Gain_heat;
	float  T_Plate_Sensor_Offset_heat;
	unsigned char EEPROM_Revision;
	word EEPROM_CRC;
};

//volume del liquido in vaschetta come percentuale rispetto al suo valore massimo
word LiquidAmount;

// volume massimo caricabile in vaschetta in ml
#define MAX_LIQUID_AMOUNT 2500


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

struct ParSaveTO_EEPROM config_data;
#define EEPROM_REVISION 0x01

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

struct tempIRSensor sensorIR_TM5A;

int timerCounterCheckTempIRSens;
unsigned int timerCounterADC0;
unsigned int timerCounterADC1;

//start addres FLASH used as EEPROM
#define START_ADDRESS_EEPROM		0xFF000
//stop addres FLASH used as EEPROM
#define STOP_ADDRESS_EEPROM			0xFFFFF


void Enable_Power_EVER_PUMP(bool status);
void Enable_Power_Motor(bool status);
void Enable_Pump_filter(bool status);
void Enable_Pump_Art_Livcer(bool status);
void Enable_Pump_OXY(bool status);
void Enable_Pinch_Filter(bool status);
void Enable_Pinch_Arterial(bool status);
void Enable_Pinch_Venous(bool status);
void Enable_Heat(bool status);
void Enable_Frigo (bool status);

#endif /* SOURCES_GLOBAL_H_ */
