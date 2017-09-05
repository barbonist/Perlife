/*
 * ModBusCommProt.h
 *
 *  Created on: 03/nov/2016
 *      Author: W15
 */

#ifndef APPLICATION_MODBUSCOMMPROT_H_
#define APPLICATION_MODBUSCOMMPROT_H_

/* .....Master Request */
char	mstreq_slave_address;			/* slave address */
char	mstreq_func_code;				/* function code */
char	mstreq_data_read_start_add_H;	/**/
char	mstreq_data_read_start_add_L;	/**/
char	mstreq_data_num_reg_rd_H;
char	mstreq_data_num_reg_rd_L;
char	mstreq_data_wr_start_add_H;
char	mstreq_data_wr_start_add_L;
char	mstreq_data_num_reg_wr_H;
char	mstreq_data_num_reg_wr_L;
char	mstreq_data_wr_byte_count;
char	mstreq_data_wr_reg_value_H;
char	mstreq_data_wr_reg_value_L;
char	mstreq_msg_data[252];			/* data field of transmitted message */
char	mstreq_crc_Low;					/* crc low byte */
char	mstreq_crc_High;				/* crc high byte */
unsigned int mstreq_crc;				/* crc */

/* Read function struct */
struct func3RetStruct
{
	unsigned char * mstreqRetStructPtr;
	unsigned char   mstreqRetStructNumByte;
	unsigned char * slvresRetPtr;
	unsigned char   slvresRetNumByte;
	} _func3RetVal;

struct func3RetStruct * _func3RetValPtr;

/* Write function struct */
struct func10RetStruct
{
	unsigned char * mstreqRetStructPtr;
	unsigned char   mstreqRetStructNumByte;
	unsigned char * slvresRetPtr;
	unsigned char   slvresRetNumByte;
	} _func10RetVal;

struct func10RetStruct * _func10RetValPtr;

/* Read and write function struct */
struct func17RetStruct
{
	unsigned char * mstreqRetStructPtr;
	unsigned char   mstreqRetStructNumByte;
	unsigned char * slvresRetPtr;
	unsigned char   slvresRetNumByte;
	} _func17RetVal;

struct func17RetStruct * _func17RetValPtr;

/*......Slave Response */
char	slvres_slave_address;			/* slave address */
char	slvres_func_code;				/* function code */
char	slvres_byte_rd_count;			/* byte read count */
char	slvres_msg_data[252];			/* data field of received message */
char	slvres_crc_Low;					/* crc low byte */
char	slvres_crc_High;				/* crc high byte */

/* Communication Protocol Variable Pump 1 */

unsigned char	msgToSendFrame3[32];
unsigned char * msgToSendFrame3Ptr;
unsigned char	msgToSendFrame10[32];
unsigned char * msgToSendFrame10Ptr;
unsigned char	msgToSendFrame17[32];
unsigned char * msgToSendFrame17Ptr;

unsigned char	msgToRecvFrame3[64];
unsigned char * msgToRecvFrame3Ptr;
unsigned char	msgToRecvFrame10[32];
unsigned char * msgToRecvFrame10Ptr;
unsigned char	msgToRecvFrame17[32];
unsigned char * msgToRecvFrame17Ptr;

int		msg_pmp1_numbyte_tx;	/* modbus codified num byte transmitted */
int		msg_pmp1_numbyte_rx;	/* modbus codified num byte received */
char	data_pmp1[252];			/* pmp1 data field */

/* MODBUS CRC */
#define CRC_POLYNOMIAL		0xA001

/* MODBUS Actuator address */
#define		PMP1_ADDRESS		0x02 /* pump 1 address - pump art */
#define		PMP2_ADDRESS		0x03 /* pump 2 address - pump oxyg */
#define		PMP3_ADDRESS		0x04 /* pump 3 address */
#define		PMP4_ADDRESS		0x05 /* pump 4 address */
#define		PNCHVLV1_ADDRESS	0x07 /* pinch valve 1 address - pinch art */
#define		PNCHVLV2_ADDRESS	0x08 /* pinch valve 2 address - pinch ven */
#define		PNCHVLV3_ADDRESS	0x09 /* pinch valve 2 address - pinch filter */

/* MODBUS Function Code */
#define		FUNC_READ_RGSTR			0x03 /* function read holding registers */
#define		FUNC_WRITE_RGSTR		0x10 /* function write multiple register */
#define		FUNC_RD_AND_WR_RGSTR	0x17 /* function read and write multiple register */

/* MODBUS Address Register Mapping */
#define		RGSTR_PUMP_SPEED_TARGET			0x0000 /* register pump speed target - r/w */
#define		RGSTR_GO_TO_HOME_POSITION		0x0001 /* register go to home position - r/w */
#define		RGSTR_ACCELERATION				0x0002 /* register acceleration - r/w */
#define		RGSTR_CURRENT_LEVEL				0x0003 /* register current level - r/w */
#define		RGSTR_CRUISE_SPEED				0x0004 /* register cruise speed - r/w */
#define		RGSTR_STEPS_TARGET				0x0005 /* register steps target - r/w */
#define		RGSTR_RW_FREE_1					0x0006 /* register free 1 */
#define		RGSTR_RW_FREE_2					0x0007 /* register free 2 */
#define		RGSTR_RW_FREE_3					0x0008 /* register free 3 */
#define		RGSTR_RW_FREE_4					0x0009 /* register free 4 */

#define		RGSTR_PMP_AVRG_CURRENT			0x0010 /* register pump average current */
#define		RGSTR_PMP_SPEED_STATUS			0x0011 /* register pump speed status */
#define		RGSTR_PMP_STATUS				0x0012 /* register pump status */
#define		RGSTR_RD_FREE_1					0x0013 /* register free 1 */
#define		RGSTR_RD_FREE_2					0x0014 /* register free 2 */
#define		RGSTR_RD_FREE_3					0x0015 /* register free 3 */
#define		RGSTR_RD_FREE_4					0x0016 /* register free 4 */
#define		RGSTR_RD_FREE_5					0x0017 /* register free 5 */
#define		RGSTR_RD_FREE_6					0x0018 /* register free 6 */
#define		RGSTR_RD_FREE_7					0x0019 /* register free 7 */
#define		RGSTR_RD_FREE_8					0x001A /* register free 8 */
#define		RGSTR_PMP_PCB_CODE_H			0x001B /* pump pcb code high byte */
#define		RGSTR_PMP_PCB_CODE_L			0x001C /* pump pcb code low byte */
#define		RGSTR_PMP_PCB_CODE_REV			0x001D /* pump pcb code revision */
#define		RGSTR_PMP_SERIAL_NUMBER			0x001E /* pump serial number */
#define 	RGSTR_PMP_FIRMWARE_REV			0x001F /* pump firmware revision */

/* MODBUS message lencht */
#define		MODBUS_FUNC3_FRAME_LENGHT		8		/* read only reques - number of byte */
#define		MODBUS_FUNC10_FRAME_LENGHT		11		/* write only request - number of byte */
#define		MODBUS_FUNC17_FRAME_LENGHT		15

/* MODBUS pinch position */
#define		MODBUS_PINCH_POS_CLOSED		0x0001
#define		MODBUS_PINCH_RIGHT_OPEN		0x0002
#define		MODBUS_PINCH_LEFT_OPEN		0x0004


/* Public interface function */

/* Read function */
struct func3RetStruct * ModBusReadRegisterReq(char slaveAddr,
									  char funcCode,
									  unsigned int readStartAddr,
									  unsigned int numRegisterRead
									  );

/* Write function */
struct func10RetStruct * ModBusWriteRegisterReq(char slaveAddr,
		  	  	  	  	  	  	  	   char funcCode,
									   unsigned int writeStartAddr,
									   unsigned int numRegisterWrite,
									   unsigned int * writeRegisterValue
									  );

/* Read and write function */
struct func17RetStruct * ModBusRWRegisterReq(char slaveAddr,
	  	  	  	   	   	   	   	    char funcCode,
									unsigned int readStartAddr,
									unsigned int numRegisterRead,
									unsigned int writeStartAddr,
									unsigned int numRegisterWrite,
									unsigned int * writeRegisterValue
									);

void modBusPmpInit(void);
void modBusPinchInit(void);

void setPumpSpeedValueHighLevel(unsigned char slaveAddr, int speedValue);
void setPumpSpeedValue(unsigned char slaveAddr, int speedValue);
void readPumpSpeedValueHighLevel(unsigned char slaveAddr);
void readPumpSpeedValue(unsigned char slaveAddr);
void setPinchPositionHighLevel(unsigned char slaveAddr, int posValue);
void setPinchPosValue(unsigned char slaveAddr, int posValue);

void alwaysModBusActuator(void);

/* Public interface function */

/* Private function */
static void PackReadTypeMessage(void);
static void PackWriteTypeMessage(void);
static void PackRWTypeMessage(void);
static void ComputeWriteByteCount(void);
static void FrameInitFunc3(void);
static void FrameInitFunc10(void);
static void FrameInitFunc17(void);
static unsigned int ComputeChecksum(unsigned char * data, int size);
/* Private function */

#endif /* APPLICATION_MODBUSCOMMPROT_H_ */
