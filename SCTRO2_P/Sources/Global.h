/*
 * Global.h
 *
 *  Created on: 15/giu/2016
 *      Author: W15
 */

#ifndef SOURCES_GLOBAL_H_
#define SOURCES_GLOBAL_H_

/* Syncronization flag */
char	iflag_pc_rx;
char	iflag_pc_tx;
char	iflag_pmp1_rx;
char	iflag_pmp1_tx;

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

struct funcRetStruct
{
	unsigned char *  ptr_msg;
	unsigned char   mstreqRetStructNumByte;
	unsigned char * slvresRetPtr;
	unsigned char   slvresRetNumByte;
	} _funcRetVal;

struct funcRetStruct * _funcRetValPtr;

/* Data Structure */
struct Peristaltic_Pump {
	char	id;
};

struct Centrifugal_Pump {
	char	id;
};

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

#endif /* SOURCES_GLOBAL_H_ */
