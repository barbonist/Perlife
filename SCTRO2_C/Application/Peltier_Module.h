/*
 * Peltier_Module.h
 *
 *  Created on: 03/nov/2016
 *      Author: W15
 */

#ifndef APPLICATION_PELTIER_MODULE_H_
#define APPLICATION_PELTIER_MODULE_H_

/* command type */
#define START_CHAR				"$"		/* start character */
#define STOP_FLAG				"Q"		/* clear run flag */
#define START_FLAG				"W"		/* set run flag */
#define SHOW_CURRENT_SW			"V"		/* show current sw version */
#define SHOW_CURRENT_SW_INT		"v"		/* show current sw and interface */
#define GET_STATUS_FLAG			"S"		/* get status flag */
#define CLEAR_STATUS_FLAG		"SC"	/* clear status flag */
#define WRITE_REG_VAL_TO_EEPROM	"RW"	/* write regulator register to eeprom */
#define READ_REG_REGISTER		"RR"	/* display regulator register */
#define WRITE_DATA_REGISTER_XX	"RXX"	/* write data to register xx */
#define READ_DATA_REGISTER_XX	"RXX?"	/* read data from register xx */
#define WRITE_FLOAT_REG_XX		"RNXX"	/* write float to register xx */
#define READ_FLOAT_FROM_REG_XX	"RNXX?" /* read float from register xx */
#define EQUAL_SYMBOL			"="		/* equal symbol */
#define QUESTION_MARK_SYMBOL	"?"		/* question mark symbol */
#define WRITE_FLOAT				"RN"	/* generic float register */
#define LOG_CONTINUOUS			"Ax"	/* continuous log of values */
#define BOOT_LOADER_MODE		"B"		/* enter boot loader mode */
#define REBOOT					"BC"	/* reboot the board */
#define GET_BOARD_INFO_ID		"LI"	/* get board info and id name */
#define DISPLAY_LOG_DATA		"LD"	/* display log data */
#define LOAD_LOG_EEPROM_TO_RAM	"LL"	/* load log data from eeprom to ram */
#define CLEAR_LOG_EEPROM		"LC"	/* clear log dtat in eeprom */
#define STOP_CHAR_CR			0x0D	/* perform last command */

/* register - read / write */
#define REG_IDLE						0xFF
#define REG_0_SET_POINT					"0"		/* set point - main temperature reference; default value (float): 20.0; R/W */
#define REG_1_PID_KP					"1"		/* pid kp; default value (float): 20.0; R/W */
#define REG_2_PID_KI					"2"		/* pid ki; default value (float): 2.0; R/W */
#define REG_3_PID_KD					"3"		/* pid kd; default value (float): 5.0; R/W */
#define REG_4_PID_KLPA					4		/* pid klpa; default value (float): 2.0; R/W */
#define REG_5_PID_KLPB					5		/* pid klpb; default value (float): 3.0; R/W */
#define REG_6_MAIN_TCLIMIT				"6"		/* tcLimit; default value (float): 100.0 - range [0 - 100]; R/W */
#define REG_7_MAIN_TCDEADBAND			"7"		/* tcDeadBand; default value (float): 3.0 - range [0 - 100]; R/W */
#define REG_8_PID_CURRENT_LIMIT			8		/* limit I value; default value (float): 100.0 - range [0 - 100]; R/W */
#define REG_9_SAMPLE_RATE				9		/* sample rate ts; default value (float): 0.05; R */
#define REG_10_COOL_GAIN				10		/* cool gain of tc signal; default value (float): 1.0; R/W */
#define REG_11_HEAT_GAIN				11		/* heat gain of tc signal; default value (float): 1.0; R/W */
#define REG_12_PID_CURRENT_DECAY		12		/* decay of I; default value (float): 0.1; R/W */
#define REG_13_REGULATOR_MODE			"13"		/* control regiater - regulation mode; default value (int): 128; R/W */
												/* 0 : no regulator mode */
												/* 1 : POWER MODE */
												/* 2 : ON/OFF MODE */
												/* 3 : P REGULATOR MODE */
												/* 4 : PI REGULATOR MODE */
												/* 5 : PD REGULATOR MODE */
												/* 6 : PID REGULATOR MODE */
#define REG_14_ON_OFF_DEAD_BAND			"14"		/* on/off dead band; default value (float): 5.0; R/W */
#define REG_15_ON_OFF_HYSTER			"15" 		/* on/off hysteresis; default value (float): 5.0; R/W */
#define REG_16_FAN1_MOD_SEL				"16"		/* fan1 mode select; default value (int): 0; R/W */
												/* 0 : always off */
												/* 1 : always on */
												/* 2 : COOL mode */
												/* 3 : HEAT mode */
												/* 4 : COOL/HEAT mode */
												/* 5 : FAN regulator mode */
#define REG_17_FAN1_SET_TEMP			17		/* fan1 set temperature; default value (float): 20.0; R/W */
#define REG_18_FAN1_DEAD_BAND			18		/* fan1 dead band; default value (float): 8.0; R/W */
#define REG_19_FAN1_LOW_SPEED_HYST		19		/* fan1 low speed hysteresis; default value (float): 4.0; R/W */
#define REG_20_FAN1_HIGH_SPEED_HYST		20		/* fan1 high speed hysteresis; default value (float): 2.0; R/w */
#define REG_21_FAN1_LOW_SPEED_VOLT		"21"		/* fan1 low speed voltage; default value (float): 30.0; R/W */
#define REG_22_FAN1_HIGH_SPEED_VOLT		"22"		/* fan1 high speed voltage; default value (float): 30.0; R/W */
#define REG_23_FAN2_MOD_SEL				23		/* fan2 mode select; default value (int): 0; R/W */
#define REG_24_FAN2_SET_TEMP			24		/* fan2 set temperature; default value (float): 20.0; R/W */
#define REG_25_FAN2_DEAD_BAND			25		/* fan2 dead band; default value (float): 8.0; R/W */
#define REG_26_FAN2_LOW_SPEED_HYST		26		/* fan2 low speed hysteresis; default value (float): 4.0; R/W */
#define REG_27_FAN2_HIGH_SPEED_HYST		27		/* fan2 high speed hysteresis; default value (float): 2.0; R/w */
#define REG_28_FAN2_LOW_SPEED_VOLT		28		/* fan2 low speed voltage; default value (float): 30.0; R/W */
#define REG_29_FAN2_HIGH_SPEED_VOLT		29		/* fan2 high speed voltage; default value (float): 30.0; R/W */
#define REG_30_POT_IN_AD_OFFSET			30		/* pot input ad offset; default value (float): 0; R/W */
#define REG_31_POT_IN_OFFSET			31		/* pot input offset; default value (float): 0; R/W */
#define REG_32_POT_IN_GAIN				32		/* pot input gain; default value (float): 1; R/W */
#define REG_33_AD_OUT_OFFSET			33		/* expansion port ad out offset; default value (float): 0; R/W */
#define REG_34_AD_OUT_GAIN				34		/* expansion port ad out gain; default value (float): 1; R/W */
#define REG_35_TEMP1_GAIN				35		/* temp1 gain; default value (float): 1; R/W */
#define REG_36_TEMP1_OFFSET				36		/* temp1 offset; default value (float): 0; R/W */
#define REG_37_TEMP2_GAIN				37		/* temp2 gain; default value (float): 1; R/W */
#define REG_38_TEMP2_OFFSET				38		/* temp2 offset; default value (float): 0; R/W */
#define REG_39_TEMP3_GAIN				39		/* temp3 gain; default value (float): 1; R/W */
#define REG_40_TEMP3_OFFSET				40		/* temp3 offset; default value (float): 0; R/W */
#define REG_41_TEMP_FET_GAIN			41		/* temp fet gain; default value (float): 1; R/W */
#define REG_42_TEMP_FET_OFFSET			42		/* temp fet offset; default value (float): 0; R/W */
#define REG_43_TEMP1_DIG_POT_GAIN		43		/* temp 1 dig pot gain; default value (float): - range [0 - 255]; R/W */
#define REG_44_TEMP1_DIG_POT_OFFSET		44		/* temp 1 dig pot offset; default value (float): - range [0 - 255]; R/W */
#define REG_45_ALARM_VOLT_HIGH			"45"		/* alarm level voltage high; default value (float): 30.0; R/W */
#define REG_46_ALARM_VOLT_LOW		    "46"		/* alarm level voltage low; default value (float): 10.0; R/W */
#define REG_47_ALARM_CURRENT_HIGH		"47"		/* alarm level main current high; default value (float): 15.0; R/W */
#define REG_48_ALARM_CURRENT_LOW		"48"		/* alarm level main current low; default value (float): 0.1; R/W */
#define REG_49_ALARM_FAN1_CURRENT_HIGH	"49"		/* alarm level fan1 current high; default value (float): 2.0; R/W */
#define REG_50_ALARM_FAN2_CURRENT_LOW	"50"		/* alarm level fan1 current low; default value (float): 0.1; R/W */
#define REG_51_ALARM_FAN2_CURRENT_HIGH	51		/* alarm level fan2 current high; default value (float): 2.0; R/W */
#define REG_52_ALARM_FAN2_CURRENT_LOW	52		/* alarm level fan2 current low; default value (float): 0.1; R/W */
#define REG_53_ALARM_INT12V_HIGH		"53"		/* alarm level internal 12 v high; default value (float): 13.0; R/W */
#define REG_54_ALARM_INT12V_LOW			"54"		/* alarm level internal 12 v low; default value (float): 7.0; R/W */
#define REG_55_TEMP1_MODE				55		/* temp1 mode; default value (int): 12; R/W */
#define REG_56_TEMP2_MODE				56		/* temp2 mode; default value (int): 4; R/W */
#define REG_57_TEMP3_MODE				57		/* temp3 mode; default value (int); 4; R/W */
#define REG_58_TEMP4_MODE				58		/* temp4 mode; default value (int); 4; R/W */
#define REG_59_TEMP1_STEIN_COEFF_A		"59"		/* temp1 steinhart coefficient a; default value (float): 1.396917e-3; R/W */
#define REG_60_TEMP1_STEIN_COEFF_B		"60"		/* temp1 steinhart coefficient b; default value (float): 2.378257e-4; R/W */
#define REG_61_TEMP1_STEIN_COEFF_C		"61"		/* temp1 steinhart coefficient c; default value (float): 9.372652e-8; R/W */
#define REG_62_TEMP2_STEIN_COEFF_A		62		/* temp2 steinhart coefficient a; default value (float): 1.396917e-3; R/W */
#define REG_63_TEMP2_STEIN_COEFF_B		63		/* temp2 steinhart coefficient b; default value (float): 2.378257e-5; R/W */
#define REG_64_TEMP2_STEIN_COEFF_C		64		/* temp2 steinhart coefficient c; default value (float): 9.372652e-7; R/W */
#define REG_65_TEMP3_STEIN_COEFF_A		65		/* temp3 steinhart coefficient a; default value (float): 1.396917e-3; R/W */
#define REG_66_TEMP3_STEIN_COEFF_B		66		/* temp3 steinhart coefficient b; default value (float): 2.378257e-5; R/W */
#define REG_67_TEMP3_STEIN_COEFF_C		67		/* temp3 steinhart coefficient c; default value (float): 9.372652e-7; R/W */
#define REG_68_TEMP1_STEIN_COEFF_A		68		/* temp4 steinhart coefficient a; default value (float): 6.843508e-3; R/W */
#define REG_69_TEMP1_STEIN_COEFF_B		69		/* temp4 steinhart coefficient b; default value (float): 2.895852e-4; R/W */
#define REG_70_TEMP1_STEIN_COEFF_C		70		/* temp4 steinhart coefficient c; default value (float): -8.177021e-8; R/W */
#define REG_71_ALARM_TEMP1_HIGH			"71"		/* alarm temp 1 high; default value (float): 80.0; R/W */
#define REG_72_ALARM_TEMP1_LOW			"72"		/* alarm temp 1 low; default value (float): -40.0; R/W */
#define REG_73_ALARM_TEMP2_HIGH			73		/* alarm temp 2 high; default value (float): 50.0; R/W */
#define REG_74_ALARM_TEMP2_LOW			74		/* alarm temp 2 low; default value (float): -10.0; R/W */
#define REG_75_ALARM_TEMP3_HIGH			75		/* alarm temp 3 high; default value (float): 50.0; R/W */
#define REG_76_ALARM_TEMP3_LOW			76		/* alarm temp 3 low; default value (float): -10.0; R/W */
#define REG_77_ALARM_TEMP4_HIGH			77		/* alarm temp 4 high; default value (float): 60.0; R/W */
#define REG_78_ALARM_TEMP4_LOW			78		/* alarm temp 4 low; default value (float): -10.0; R/W */
#define REG_79_TEMP1_STEIN_VAL_H		79		/* temp 1 steinhart resistance value h; default value (float): 759.4; R/W */
#define REG_80_TEMP1_STEIN_VAL_M		80		/* temp 1 steinhart resistance value l; default value (float): 3057.7; R/W */
#define REG_81_TEMP1_STEIN_VAL_L		81		/* temp 1 steinhart resistance value m; default value (float): 29875.8; R/W */
#define REG_82_TEMP2_STEIN_VAL_H		82		/* temp 2 steinhart resistance value h; default value (float): 759.4; R/W */
#define REG_83_TEMP2_STEIN_VAL_M		83		/* temp 2 steinhart resistance value l; default value (float): 3057.7; R/W */
#define REG_84_TEMP2_STEIN_VAL_L		84		/* temp 2 steinhart resistance value m; default value (float): 29875.8; R/W */
#define REG_85_TEMP3_STEIN_VAL_H		85		/* temp 3 steinhart resistance value h; default value (float): 759.4; R/W */
#define REG_86_TEMP3_STEIN_VAL_M		86		/* temp 3 steinhart resistance value l; default value (float): 3057.7; R/W */
#define REG_87_TEMP3_STEIN_VALL			87		/* temp 3 steinhart resistance value m; default value (float): 29875.8; R/W */
#define REG_88_TEMP4_STEIN_VAL_H		88		/* temp 4 steinhart resistance value h; default value (float): 2965.14; R/W */
#define REG_89_TEMP4_STEIN_VAL_M		89		/* temp 4 steinhart resistance value l; default value (float): 28836.8; R/W */
#define REG_90_TEMP4_STEIN_VALL			90		/* temp 4 steinhart resistance value m; default value (float): 78219; R/W */
#define REG_91_ALARM_ENABLE_LOW			91		/* alarm enable bits low; default value (uint): 351; R/W */
#define REG_92_ALARM_ENABLE_HIGH		92		/* alarm enable bits high; default value (uint): 255; R/W */
#define REG_93_TST_SETPOINT_2			93		/* test loop mode -setpoint 2; default value (float): 8.0; R/W*/
#define REG_94_TIME_HIGH				94		/* time high when in loop mode; default value (uint): 300; R/W */
#define REG_95_TIME_LOW					95		/* time low when in loop mode; default value (uint): 200; R/W */
#define REG_96_SENSOR_ALARM_MASK		96		/* sensor alarm mask - select warning - alarm of sensor; default value (uint): 65532; R/W */

/* register - read */
#define REG_99_EVENT_COUNT				99		/* regulator event count; (float); R */
#define REG_100_TEMP_1_VALUE			"100"		/* temp1 value (AN5); (float); R */
#define REG_101_TEMP_2_VALUE			101		/* temp2 value (AN6); (float); R */
#define REG_102_TEMP_3_VALUE			102		/* temp3 value (AN7); (float); R */
#define REG_103_TEMP_4_VALUE			"103"		/* temp fet value (AN8); (float); R */
#define REG_104_TEMP_PT_REF				104		/* temp pot reference; (float); R */
#define REG_105_TEMP_REF				"105"		/* temp reference; (float); R */
#define REG_106_MAIN_TEMP_OUT_VAL		"106"		/* temperature output value; (float); R */
#define REG_107_FAN1_OUT_VAL			107		/* fan 1 output value; (float); R */
#define REG_108_FAN2_OUT_VAL			108		/* fan 2 output value; (float); R */
#define REG_110_PID_TA					110		/* pid ta; (float); R */
#define REG_111_PID_TE					111		/* pid te; (float); R */
#define REG_112_PID_TP					112		/* pid tp; (float); R */
#define REG_113_PID_TI					113		/* pid ti; (float); R */
#define REG_114_PID_TD					114		/* pid td; (float); R */
#define REG_117_PID_TLP_A				117		/* pid tlpa; (float); R */
#define REG_118_PID_TLP_B				118		/* pid tlpb; (float); R */
#define REG_122_ON_OFF_RUN_STATE		"122"		/* on/off runtime state; (int); R */
#define REG_123_ON_OFF_RUN_MAX			"123"		/* on/off runtime max; (float); R */
#define REG_124_ON_OFF_RUN_MIN			"124"		/* on/off runtime min; (float); R */
#define REG_125_FAN1_RUN_STATE			125		/* fan1 runtime state; (int); R */
#define REG_126_FAN1_RUN_MAX			126		/* fan1 runtime max; (float); R */
#define REG_127_FAN1_RUN_MIN			127		/* fan1 runtime min;(float); R */
#define REG_128_FAN2_RUN_STATE			128		/* fan2 runtime state; (int); R */
#define REG_129_FAN2_RUN_MAX			129		/* fan2 runtime max; (float); R */
#define REG_130_FAN2_RUN_MIN			130		/* fan2 runtime min; (float); R */
#define REG_150_AN1_IN_VOLTAGE			"150"		/* an1 input voltage; (float); R */
#define REG_151_AN10_12V_INT			"151"		/* an10 internal 12v; (float); R */
#define REG_152_AN9_MAIN_CURRENT		"152"		/* an9 main current; (float); R */
#define REG_153_AN11_FAN1_CURRENT		"153"		/* an11 fan 1 current; (float); R */
#define REG_154_AN4_FAN2_CURRENT		154		/* an4 fan 2 current; (float); R */







//void PeltierAssSendCommand(char command[], char registerId, int dataIntUser, float dataFloatUser);
void PeltierAssSendCommand(char command[], char registerId[], int dataIntUser, char dataFloatUser[8], unsigned char channel);
void PeltierAssWaitReadyToRcv(void);
void PeltierAssReadResponse(void);
void peltierAssInit(void);
void alwaysPeltierActuator(void);
void alwaysPeltier2Actuator(void);
void startPeltierActuator(void);
void stopPeltierActuator(void);
void NewSetTempPeltierActuator();
void NewSetTempPeltier2Actuator();


#endif /* APPLICATION_PELTIER_MODULE_H_ */
