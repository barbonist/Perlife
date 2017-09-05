/*
 * Peltier_Module.c
 *
 *  Created on: 05/lug/2016
 *      Author: W15
 */

#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#include "Peltier_Module.h"

#include "AS3_PELTIER.h"
#include "ASerialLdd3.h"

void PeltierAssSendCommand(char command[], char registerId, int dataIntUser, float dataFloatUser)
{
	unsigned char	startChar;
	unsigned char	stopChar;
	int				dataInt;			/* 2 byte */
	float			dataFloat;			/* 4 byte */
	char			stringInt[16];
	char			stringIntlenght;
	char			stringFloat[16];
	char			stringFloatlenght;

	static unsigned char	maskCommLenght_1 = 0x00;
	static unsigned char	maskCommLenght_2 = 0x00;
	static unsigned char	maskCommLenght_3 = 0x00;

	unsigned char msgPeltier[16];
	unsigned char msgLenght;

	for(int i = 0; i < 16; i++)
		msgPeltier[i] = 0x00;

	for(int i = 0; i < 16; i++)
	{
		stringInt[i] = 0x00;
		stringFloat[i] = 0x00;
	}

	/* START CHAR */
	startChar = 0x24;
	msgPeltier[0] = startChar;

	stopChar = 0x0D;

	sprintf(stringInt, "%d", dataIntUser);
	stringIntlenght = strlen(stringInt);

	sprintf(stringFloat, "%3.1f", dataFloatUser);
	stringFloatlenght = strlen(stringFloat);

	if(strlen(command) == 1)
	{
		maskCommLenght_1 = ((strcmp(command,STOP_FLAG) == 0)) |					/* 0x01 */
						   ((strcmp(command,START_FLAG) == 0)<<1) |				/* 0x02 */
						   ((strcmp(command,SHOW_CURRENT_SW) == 0)<<2) |		/* 0x04 */
						   ((strcmp(command,SHOW_CURRENT_SW_INT) == 0)<<3) |	/* 0x08 */
						   ((strcmp(command,GET_STATUS_FLAG) == 0)<<4) |		/* 0x10 */
						   ((strcmp(command,BOOT_LOADER_MODE) == 0)<<5);		/* 0x20 */

		switch(maskCommLenght_1)
		{
		case 0x01:
			msgPeltier[1] = 'Q';
			break;

		case 0x02:
			msgPeltier[1] = 'W';
			break;

		case 0x04:
			msgPeltier[1] = 'V';
			break;

		case 0x08:
			msgPeltier[1] = 'v';
			break;

		case 0x10:
			msgPeltier[1] = 'S';
			break;

		case 0x20:
			msgPeltier[1] = 'B';
			break;

		default:
			break;
		}

		msgPeltier[2] = stopChar;
		msgLenght = 3;

	}
	else if(strlen(command) == 2)
	{
		//maskCommLenght_2
		maskCommLenght_2 = 	((strcmp(command,CLEAR_STATUS_FLAG) == 0)) |					/* 0x01 */
							((strcmp(command,WRITE_REG_VAL_TO_EEPROM) == 0)<<1) |			/* 0x02 */
							((strcmp(command,READ_REG_REGISTER) == 0)<<2) |					/* 0x04 */
							((strcmp(command,REBOOT) == 0)<<3) |							/* 0x08 */
							((strcmp(command,GET_BOARD_INFO_ID) == 0)<<4) |					/* 0x10 */
							((strcmp(command,DISPLAY_LOG_DATA) == 0)<<5) |					/* 0x20 */
							((strcmp(command,LOAD_LOG_EEPROM_TO_RAM) == 0)<<6) |			/* 0x40 */
							((strcmp(command,CLEAR_LOG_EEPROM) == 0)<<7);					/* 0x80 */

		switch(maskCommLenght_2)
				{
				case 0x01:
					msgPeltier[1] = 'S';
					msgPeltier[2] = 'C';
					break;

				case 0x02:
					msgPeltier[1] = 'R';
					msgPeltier[2] = 'W';
					break;

				case 0x04:
					msgPeltier[1] = 'R';
					msgPeltier[2] = 'R';
					break;

				case 0x08:
					msgPeltier[1] = 'B';
					msgPeltier[2] = 'C';
					break;

				case 0x10:
					msgPeltier[1] = 'L';
					msgPeltier[2] = 'I';
					break;

				case 0x20:
					msgPeltier[1] = 'L';
					msgPeltier[2] = 'D';
					break;

				case 0x40:
					msgPeltier[1] = 'L';
					msgPeltier[2] = 'L';
					break;

				case 0x80:
					msgPeltier[1] = 'L';
					msgPeltier[2] = 'C';
					break;

				default:
					break;
				}

			msgPeltier[3] = stopChar;
			msgLenght = 4;
	}
	else
	{
		//maskCommLenght_3
		maskCommLenght_3 =  ((strcmp(command,WRITE_DATA_REGISTER_XX) == 0)) |				/* 0x01 */
							((strcmp(command,READ_DATA_REGISTER_XX) == 0)<<2) |				/* 0x04 */
							((strcmp(command,WRITE_FLOAT_REG_XX) == 0)<<4) |				/* 0x10 */
							((strcmp(command,READ_FLOAT_FROM_REG_XX) == 0)<<6) |			/* 0x40 */
							((strcmp(command,LOG_CONTINUOUS) == 0)<<7);						/* 0x80 */

		switch(maskCommLenght_3)
						{
						case 0x01:
							msgPeltier[1] = 'R';
							msgPeltier[2] = registerId;
							msgPeltier[3] = '=';
							for(int i = 0; i < stringIntlenght; i++)
							{
								msgPeltier[4+i] = stringInt[i];
							}
							msgPeltier[4+stringIntlenght] = stopChar;
							msgLenght = 5 + stringIntlenght;
							break;

						case 0x04:
							msgPeltier[1] = 'R';
							msgPeltier[2] = (char) registerId;
							msgPeltier[3] = '?';
							msgPeltier[4] = stopChar;
							msgLenght = 5;
							break;

						case 0x10:
							msgPeltier[1] = 'R';
							msgPeltier[2] = 'N';
							msgPeltier[3] = (char) registerId;
							msgPeltier[4] = '=';
							for(int i = 0; i < stringFloatlenght; i++)
							{
								msgPeltier[5+i] = stringFloat[i];
							}
							msgPeltier[5+stringFloatlenght] = stopChar;
							msgLenght = 6 + stringFloatlenght;
							break;

						case 0x40:
							msgPeltier[1] = 'R';
							msgPeltier[2] = 'N';
							msgPeltier[3] = (char) registerId;
							msgPeltier[4] = '?';
							msgPeltier[5] = stopChar;
							msgLenght = 6;
							break;

						case 0x80:
							msgPeltier[1] = 'A';
							msgPeltier[2] = (char) registerId;
							msgPeltier[3] = stopChar;
							msgLenght = 4;
							break;

						default:
							break;
						}
	}

	for(int i = 0; i<msgLenght; i++)
	{
		AS3_PELTIER_SendChar(msgPeltier[i]);
	}

}

void PeltierAssWaitReadyToRcv(void)
{

}

void PeltierAssReadResponse(void)
{

}
