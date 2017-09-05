/* ###################################################################
**     Filename    : main.c
**     Project     : SCTRO
**     Processor   : MK64FN1M0VLQ12
**     Version     : Driver 01.01
**     Compiler    : GNU C Compiler
**     Date/Time   : 2016-06-07, 18:03, # CodeGen: 0
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
#include "Bit1.h"
#include "BitIoLdd1.h"
#include "Bit2.h"
#include "BitIoLdd2.h"
#include "AS1_MODBUS_ACTUATOR.h"
#include "ASerialLdd1.h"
#include "AS2_PC_DEBUG.h"
#include "ASerialLdd2.h"
#include "Bit3.h"
#include "BitIoLdd3.h"
#include "Bit4.h"
#include "BitIoLdd4.h"
#include "TI1.h"
#include "TimerIntLdd1.h"
#include "TU1.h"
#include "AD1.h"
#include "AdcLdd1.h"
#include "AS3_PELTIER.h"
#include "ASerialLdd3.h"
#include "AS4_C2P.h"
#include "ASerialLdd4.h"
#include "AS5_SBC.h"
#include "ASerialLdd5.h"
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
#include "CRC_Routine.h"
#include "ModBusCommProt.h"
#include "Peltier_Module.h"
#include "Adc_Ges.h"


char msg_rx[32];
unsigned int wr_reg_value[6] = {0x0000,
								0x0000,
								0x0000,
								0x0000,
								0x0000,
								0x0000};

unsigned int * wr_reg_value_ptr;
unsigned int write_start_addr;
unsigned int read_start_address;

unsigned char	debug_guard_value;

unsigned char peltier_debug[8] = {0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38};
unsigned char peltier_debug_2[8] = {0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48};

word * snd;
unsigned char * ptrSnd;
unsigned char ptrSndChar;
unsigned char result;

word * valAdc = NULL;
int valAdcRaw = 0xFFFF;
unsigned char errAs2Tx;
unsigned char msgDebugPC[8];

//AS1_MODBUS_ACTUATOR_TComData * ptr_msg;

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  /* Write your local variable definition here */

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */

  /**/
  initAllGuard();
  initAllState();
  /**/
  Bit3_SetVal(); /* enable motore */
  Bit4_SetVal();

  static int index = 0;
  static int index2 = 0;
  ptr = &pc_rx_data[0];
  ptr_count = 0;

  for(;;) {
				  #ifndef DEBUG_ENABLE
	  	  	  	  index++;
	         	  if(index%32000 == 0)
	         		 Bit1_NegVal();
	         	  else if(index%65000 == 0)
	         	  {
	         		 Bit2_NegVal();
	         		 index = 0;
	         	  }
				  #endif

	         	 computeMachineStateGuard();

	         	 processMachineState();



				 #ifdef DEBUG_ENABLE
	         	 static char	indSel = 0;
	         	 index++;
	         	 if(index%8000 == 0)
	         	 {
	         	  ptrSndChar = 0x24;
	         	  ptrSnd = &peltier_debug_2[0];

	         	 /**/
	         	  switch(indSel)
	         	  {
	         	  case 0:
	         		  valAdc = ReadAdcPr1();
	         		  valAdcRaw = *valAdc;
	         		  indSel++;

	         		 PeltierAssSendCommand(WRITE_DATA_REGISTER_XX, '4', valAdcRaw, 0);

	         		 msgDebugPC[0] = 0xB0;
	         		 msgDebugPC[1] = 0xB0;
	         		 msgDebugPC[2] = (unsigned char) valAdcRaw;
	         		 msgDebugPC[3] = (unsigned char) (valAdcRaw>>8);
	         		 msgDebugPC[4] = 0x00;
	         		 msgDebugPC[5] = 0x00;
	         		 msgDebugPC[6] = 0x00;
	         		 msgDebugPC[7] = 0x00;

	         		 //for(int i = 0; i<8; i++)
	         		 ptrDebug = &msgDebugPC[0];
	         		 //AS2_PC_DEBUG_SendChar(*ptrDebug);
	         		  break;

	         	  case 1:
	         		 valAdc = ReadAdcPr2();
	         		 valAdcRaw = *valAdc;
	         		 indSel++;

	         		 PeltierAssSendCommand(WRITE_DATA_REGISTER_XX, '4', valAdcRaw, 0);

	         		 msgDebugPC[0] = 0xB1;
	         		 msgDebugPC[1] = 0xB0;
	         		 msgDebugPC[2] = (unsigned char) valAdcRaw;
	         		 msgDebugPC[3] = (unsigned char) (valAdcRaw>>8);
	         		 msgDebugPC[4] = 0x00;
	         		 msgDebugPC[5] = 0x00;
	         		 msgDebugPC[6] = 0x00;
	         		 msgDebugPC[7] = 0x00;

	         		 ptrDebug = &msgDebugPC[0];
	         		 //AS2_PC_DEBUG_SendChar(*ptrDebug);
	         		  break;

	         	  case 2:
	         		 valAdc = ReadAdcTm1();
	         		 valAdcRaw = *valAdc;
	         		 indSel++;

	         		PeltierAssSendCommand(WRITE_DATA_REGISTER_XX, '4', valAdcRaw, 0);

	         		msgDebugPC[0] = 0xC0;
	         		msgDebugPC[1] = 0xB0;
	         		msgDebugPC[2] = (unsigned char) valAdcRaw;
	         		msgDebugPC[3] = (unsigned char) (valAdcRaw>>8);
	         		msgDebugPC[4] = 0x00;
	         		msgDebugPC[5] = 0x00;
	         		msgDebugPC[6] = 0x00;
	         		msgDebugPC[7] = 0x00;

	         		//for(int i = 0; i<8; i++)
	         		ptrDebug = &msgDebugPC[0];
	         		//AS2_PC_DEBUG_SendChar(*ptrDebug);
	         		  break;

	         	  case 3:
	         		 valAdc = ReadAdcTm2();
	         		 valAdcRaw = *valAdc;
	         		 indSel = 0;

	         		PeltierAssSendCommand(WRITE_DATA_REGISTER_XX, '4', valAdcRaw, 0);

	         		 msgDebugPC[0] = 0xC1;
	         		 msgDebugPC[1] = 0xB0;
	         		 msgDebugPC[2] = (unsigned char) valAdcRaw;
	         	     msgDebugPC[3] = (unsigned char) (valAdcRaw>>8);
	         		 msgDebugPC[4] = 0x00;
	         		 msgDebugPC[5] = 0x00;
	         		 msgDebugPC[6] = 0x00;
	         		 msgDebugPC[7] = 0x00;

	         	     //for(int i = 0; i<8; i++)
	         		 ptrDebug = &msgDebugPC[0];
	         		 //AS2_PC_DEBUG_SendChar(*ptrDebug);
	         		  break;
	         	  }

	         	 //PeltierAssSendCommand(WRITE_DATA_REGISTER_XX, '4', valAdcRaw, 0);
	         	 /**/

	         	  //result = AS3_PELTIER_SendBlock(ptrSnd, 8, snd);
				 #ifndef DEBUG_ENABLE

	         	 switch(index2)
	         	 {
	         	 case 0:
	         		PeltierAssSendCommand(STOP_FLAG, 0, 0, 0);
	         	 	index2 = index2 + 1;
	         	 	 break;

	         	 case 1:
	         		PeltierAssSendCommand(START_FLAG, 0, 0, 0);
	         		index2 = index2 + 1;
	         		 break;

	         	 case 2:
	         		PeltierAssSendCommand(SHOW_CURRENT_SW, 0, 0, 0);
	         	 	index2 = index2 + 1;
	         	 	 break;

	         	 case 3:
	         		PeltierAssSendCommand(SHOW_CURRENT_SW_INT, 0, 0, 0);
	         		index2 = index2 + 1;
	         		 break;

	         	case 4:
	         		PeltierAssSendCommand(WRITE_DATA_REGISTER_XX, '4', 20, 0);
	         		index2 = index2 + 1;
	         		 break;

	         	case 5:
	         		PeltierAssSendCommand(READ_DATA_REGISTER_XX, '5', 20, 0);
	         		index2 = index2 + 1;
	         		 break;

	         	case 6:
	         		PeltierAssSendCommand(WRITE_FLOAT_REG_XX, '6', 0, 10.3);
	         		index2 = index2 + 1;
	         		break;

	         	case 7:
	         		PeltierAssSendCommand(READ_FLOAT_FROM_REG_XX, '7', 0, 10.3);
	         	 	index2 = 0;
	         	 	break;

	         	default:
	         		break;
	         	 }
				 #endif
	         	  //for(int i = 0; i<8; i++)
	         	  //{
	         		 //AS3_PELTIER_SendChar(peltier_debug[i]);
	         	  //}
	         	 }
				 #endif
	         	  /*msg_rx[index%32] = 0x00 + (index%32);
	         	  AS2_PC_DEBUG_Enable();
	         	  AS2_PC_DEBUG_SendChar(msg_rx[index%32]);
	         	  AS2_PC_DEBUG_Disable();*/

	         	  if(iflag_pc_rx == IFLAG_PC_RX)
	         	  {
	         		iflag_pc_rx = IFLAG_IDLE;
	         		ptr_count = 0;
	         		ptr = &pc_rx_data[0];

	         		//Bit3_SetVal(); /* enable motore */
	         		//Bit4_SetVal(); /* rts max 491 */
	         		AS1_MODBUS_ACTUATOR_Enable();
	         		AS2_PC_DEBUG_Enable();
	         		AS5_SBC_Enable();

	         		switch(pc_rx_data[0])
					{
	         			/* write */
	         			case 0x31:
	         				wr_reg_value_ptr = &wr_reg_value[0];
	         				wr_reg_value[0] = 0x07D0;

							_funcRetValPtr = ModBusWriteRegisterReq(PMP1_ADDRESS,
	         												 FUNC_WRITE_RGSTR,
															 RGSTR_PUMP_SPEED_TARGET,
															 0x0001,
															 wr_reg_value_ptr);

	         				_funcRetVal.ptr_msg = _funcRetValPtr->ptr_msg;
	         				_funcRetVal.mstreqRetStructNumByte = _funcRetValPtr->mstreqRetStructNumByte;

	         				//msg_pmp1_rx_ptr = &msg_pmp1_rx[0];
	         				_funcRetVal.slvresRetPtr = _funcRetValPtr->slvresRetPtr;
	         				_funcRetVal.slvresRetNumByte = _funcRetValPtr->slvresRetNumByte;

	         				for(char k = 0; k < _funcRetVal.mstreqRetStructNumByte; k++)
	         				{
	         					AS1_MODBUS_ACTUATOR_SendChar(*(_funcRetVal.ptr_msg+k));
	         				}

	         				AS2_PC_DEBUG_TComData ptr_snd;
	         				ptr_snd = 33;
	         				//AS2_PC_DEBUG_SendChar(ptr_snd);
	         				ptr_snd = 55;
	         				//AS2_PC_DEBUG_SendChar(ptr_snd);
	         				break;

	         			/* stop */
	         			case 0x39:
	         				wr_reg_value_ptr = &wr_reg_value[0];
	         				wr_reg_value[0] = 0x0000;

	         				_funcRetValPtr = ModBusWriteRegisterReq(PMP1_ADDRESS,
	         					         							FUNC_WRITE_RGSTR,
																	RGSTR_PUMP_SPEED_TARGET,
																	0x0001,
	         														wr_reg_value_ptr);

	         				_funcRetVal.ptr_msg = _funcRetValPtr->ptr_msg;
	         				_funcRetVal.mstreqRetStructNumByte = _funcRetValPtr->mstreqRetStructNumByte;

	         				//msg_pmp1_rx_ptr = &msg_pmp1_rx[0];
	         				_funcRetVal.slvresRetPtr = _funcRetValPtr->slvresRetPtr;
	         				_funcRetVal.slvresRetNumByte = _funcRetValPtr->slvresRetNumByte;

	         				for(char k = 0; k < _funcRetVal.mstreqRetStructNumByte; k++)
	         				{
	         					AS1_MODBUS_ACTUATOR_SendChar(*(_funcRetVal.ptr_msg+k));
	         				}

	         				ptr_snd = 33;
	         				//AS2_PC_DEBUG_SendChar(ptr_snd);
	         				break;

	         			/* read */
	         			case 0x35:
	         				/* costruisci messaggio raw */


	         				//AS1_MODBUS_ACTUATOR_SendChar(*ptr_msg);

	         				ptr_snd = 33;
	         				//AS2_PC_DEBUG_SendChar(ptr_snd);
	         				ptr_snd = 55;
	         				//AS2_PC_DEBUG_SendChar(ptr_snd);
	         				ptr_snd = mstreq_data_read_start_add_L;
	         				//AS2_PC_DEBUG_SendChar(ptr_snd);
							break;

						#ifdef DEBUG_ENABLE
							/*PUMP 1*/
							case 0x02:
							switch(pc_rx_data[1])
							{
								case	0x10:
									write_start_addr = BYTES_TO_WORD(pc_rx_data[2] , pc_rx_data[3]);
									wr_reg_value_ptr = &wr_reg_value[0];
									wr_reg_value[0] = BYTES_TO_WORD(pc_rx_data[5] , pc_rx_data[4]);

									_funcRetValPtr = ModBusWriteRegisterReq(PMP1_ADDRESS,
																			pc_rx_data[1],
																			write_start_addr,
																			0x0001,
																			wr_reg_value_ptr);

									_funcRetVal.ptr_msg = _funcRetValPtr->ptr_msg;
									_funcRetVal.mstreqRetStructNumByte = _funcRetValPtr->mstreqRetStructNumByte;

									//msg_pmp1_rx_ptr = &msg_pmp1_rx[0];
									_funcRetVal.slvresRetPtr = _funcRetValPtr->slvresRetPtr;
									_funcRetVal.slvresRetNumByte = _funcRetValPtr->slvresRetNumByte;

									for(char k = 0; k < _funcRetVal.mstreqRetStructNumByte; k++)
									{
										AS1_MODBUS_ACTUATOR_SendChar(*(_funcRetVal.ptr_msg+k));
									}
									break;

								    case	0x03:
								        read_start_address = BYTES_TO_WORD(pc_rx_data[10] , pc_rx_data[11]);

								         _funcRetValPtr = ModBusReadRegisterReq(PMP1_ADDRESS,
								         										pc_rx_data[1],
																				read_start_address,
																				0x0001);

								         _funcRetVal.ptr_msg = _funcRetValPtr->ptr_msg;
								         _funcRetVal.mstreqRetStructNumByte = _funcRetValPtr->mstreqRetStructNumByte;

								         //msg_pmp1_rx_ptr = &msg_pmp1_rx[0];
								         _funcRetVal.slvresRetPtr = _funcRetValPtr->slvresRetPtr;
								         _funcRetVal.slvresRetNumByte = _funcRetValPtr->slvresRetNumByte;

								         for(char k = 0; k < _funcRetVal.mstreqRetStructNumByte; k++)
								         {
								         	AS1_MODBUS_ACTUATOR_SendChar(*(_funcRetVal.ptr_msg+k));
								         }
										break;
								}
								 break;

	         			/*PUMP 2*/
	         			case 0x03:
	         				switch(pc_rx_data[1])
	         				{
	         				case	0x10:
	         					write_start_addr = BYTES_TO_WORD(pc_rx_data[2] , pc_rx_data[3]);
	         					wr_reg_value_ptr = &wr_reg_value[0];
	         					wr_reg_value[0] = BYTES_TO_WORD(pc_rx_data[5] , pc_rx_data[4]);

	         					_funcRetValPtr = ModBusWriteRegisterReq(PMP2_ADDRESS,
	         															pc_rx_data[1],
	         															write_start_addr,
	         															0x0001,
	         															wr_reg_value_ptr);

	         					_funcRetVal.ptr_msg = _funcRetValPtr->ptr_msg;
	         					_funcRetVal.mstreqRetStructNumByte = _funcRetValPtr->mstreqRetStructNumByte;

	         					//msg_pmp1_rx_ptr = &msg_pmp1_rx[0];
	         					_funcRetVal.slvresRetPtr = _funcRetValPtr->slvresRetPtr;
	         					_funcRetVal.slvresRetNumByte = _funcRetValPtr->slvresRetNumByte;

	         					for(char k = 0; k < _funcRetVal.mstreqRetStructNumByte; k++)
	         					{
	         						AS1_MODBUS_ACTUATOR_SendChar(*(_funcRetVal.ptr_msg+k));
	         					}
	         					break;

	         				case	0x03:
	         					read_start_address = BYTES_TO_WORD(pc_rx_data[10] , pc_rx_data[11]);

	         					_funcRetValPtr = ModBusReadRegisterReq(PMP2_ADDRESS,
	         												           pc_rx_data[1],
	         															read_start_address,
	         															0x0001);

	         					_funcRetVal.ptr_msg = _funcRetValPtr->ptr_msg;
	         					_funcRetVal.mstreqRetStructNumByte = _funcRetValPtr->mstreqRetStructNumByte;

	         					//msg_pmp1_rx_ptr = &msg_pmp1_rx[0];
	         					_funcRetVal.slvresRetPtr = _funcRetValPtr->slvresRetPtr;
	         					_funcRetVal.slvresRetNumByte = _funcRetValPtr->slvresRetNumByte;

	         					for(char k = 0; k < _funcRetVal.mstreqRetStructNumByte; k++)
	         					{
	         						AS1_MODBUS_ACTUATOR_SendChar(*(_funcRetVal.ptr_msg+k));
	         					}
	         					break;
	         				}
	         				break;

	         			/*PINCH 1*/
	         			case 0x09:
	         				switch(pc_rx_data[1])
	         					{
	         					case	0x10:
	         					write_start_addr = BYTES_TO_WORD(pc_rx_data[2] , pc_rx_data[3]);
	         					wr_reg_value_ptr = &wr_reg_value[0];
	         					wr_reg_value[0] = BYTES_TO_WORD(pc_rx_data[5] , pc_rx_data[4]);

	         					_funcRetValPtr = ModBusWriteRegisterReq(PNCHVLV3_ADDRESS,
	         															pc_rx_data[1],
	         															write_start_addr,
	         															0x0001,
	         															wr_reg_value_ptr);

	         					_funcRetVal.ptr_msg = _funcRetValPtr->ptr_msg;
	         					_funcRetVal.mstreqRetStructNumByte = _funcRetValPtr->mstreqRetStructNumByte;

	         					//msg_pmp1_rx_ptr = &msg_pmp1_rx[0];
	         					_funcRetVal.slvresRetPtr = _funcRetValPtr->slvresRetPtr;
	         					_funcRetVal.slvresRetNumByte = _funcRetValPtr->slvresRetNumByte;

	         					for(char k = 0; k < _funcRetVal.mstreqRetStructNumByte; k++)
	         					{
	         					AS1_MODBUS_ACTUATOR_SendChar(*(_funcRetVal.ptr_msg+k));
	         					}
	         					break;

	         					case	0x03:
	         						read_start_address = BYTES_TO_WORD(pc_rx_data[10] , pc_rx_data[11]);

	         						_funcRetValPtr = ModBusReadRegisterReq(PNCHVLV3_ADDRESS,
	         												         		pc_rx_data[1],
	         																read_start_address,
	         																0x0001);

	         						_funcRetVal.ptr_msg = _funcRetValPtr->ptr_msg;
	         						_funcRetVal.mstreqRetStructNumByte = _funcRetValPtr->mstreqRetStructNumByte;

	         						//msg_pmp1_rx_ptr = &msg_pmp1_rx[0];
	         						_funcRetVal.slvresRetPtr = _funcRetValPtr->slvresRetPtr;
	         						_funcRetVal.slvresRetNumByte = _funcRetValPtr->slvresRetNumByte;

	         						for(char k = 0; k < _funcRetVal.mstreqRetStructNumByte; k++)
	         						{
	         							AS1_MODBUS_ACTUATOR_SendChar(*(_funcRetVal.ptr_msg+k));
	         						}
	         						break;
	         					}
	         				break;

	         			/*PELTIER*/
	         			case 0x90:
	         				break;

	         			/* GUARD */
	         			case 0xA0:

							#ifdef DEBUG_ENABLE

	         				if(pc_rx_data[1] == 0xA0)
	         				{
	         					switch(pc_rx_data[13])
	         					{
	         					case 0x00:
	         						debug_guard_value = GUARD_VALUE_NULL;
	         						break;

	         					case 0xA5:
	         						debug_guard_value = GUARD_VALUE_TRUE;
	         						break;

	         					case 0x5A:
	         						debug_guard_value = GUARD_VALUE_FALSE;
	         						break;

	         					default:
	         						break;
	         					}

	         					switch(pc_rx_data[12])
	         					{
	         					case 0x00:
	         						currentGuard[GUARD_NULL].guardValue = debug_guard_value;
	         						break;

	         					case 0x01:
	         						currentGuard[GUARD_START_ENABLE].guardValue = debug_guard_value;
	         						break;

	         					case 0x02:
	         						currentGuard[GUARD_HW_T1T_DONE].guardValue = debug_guard_value;
	         						break;

	         					case 0x03:
	         						currentGuard[GUARD_COMM_ENABLED].guardValue = debug_guard_value;
	         						break;

	         					case 0x04:
	         						currentGuard[GUARD_THERAPY_SELECTED].guardValue = debug_guard_value;
	         						break;

	         					case 0x05:
	         						currentGuard[GUARD_THERAPY_CONFIRMED].guardValue = debug_guard_value;
	         						break;

	         					case 0x06:
	         						currentGuard[GUARD_T1_NO_DISP_END].guardValue = debug_guard_value;
	         						break;

	         					case 0x07:
	         						currentGuard[GUARD_KIT_MOUNTED_CONFIRMED].guardValue = debug_guard_value;
	         						break;

	         					case 0x08:
	         						currentGuard[GUARD_START_LEAK_CONFIRMED].guardValue = debug_guard_value;
	         						break;

	         					case 0x09:
	         						currentGuard[GUARD_T1_WITH_DISP_END].guardValue = debug_guard_value;
	         						break;

	         					case 0x0A:
	         						currentGuard[GUARD_START_PRIMING].guardValue = debug_guard_value;
	         						break;

	         					case 0x0B:
	         						currentGuard[GUARD_TREAT_1_SELECTED].guardValue = debug_guard_value;
	         						break;

	         					case 0x0C:
	         						currentGuard[GUARD_TREAT_2_SELECTED].guardValue = debug_guard_value;
	         						break;

	         					case 0x0D:
	         						currentGuard[GUARD_PRIMING_END].guardValue = debug_guard_value;
	         						break;

	         					case 0x0E:
	         						currentGuard[GUARD_START_TREAT_CONFIRMED].guardValue = debug_guard_value;
	         						break;

	         					case 0x0F:
	         						currentGuard[GUARD_STOP_TREAT_SELECTED].guardValue = debug_guard_value;
	         						break;

	         					case 0x10:
	         						currentGuard[GUARD_TREAT_1_END].guardValue = debug_guard_value;
	         						break;

	         					case 0x11:
	         						currentGuard[GUARD_EMPTY_DISPOSABLE_END].guardValue = debug_guard_value;
	         						break;

	         					case 0x12:
	         						currentGuard[GUARD_OPEN_COVER_CONFIRMED].guardValue = debug_guard_value;
	         						break;

	         					case 0x13:
	         						currentGuard[GUARD_CLEAN_EQUIP_CONFIRMED].guardValue = debug_guard_value;
	         						break;

	         					case 0x14:
	         						currentGuard[GUARD_FATAL_ERROR].guardValue = debug_guard_value;
	         						break;

	         					default:
	         						break;
	         					}
	         				}
							#endif

	         				break;

	         			/* PR1 SENSOR */
	         			case 0xB0:
	         				pc_rx_data[0] = 0x00;
	         				valAdc = ReadAdcPr1();
	         				valAdcRaw = *valAdc;
	         				msgDebugPC[0] = 0xB0;
	         				msgDebugPC[1] = 0xB0;
	         				msgDebugPC[2] = (unsigned char) valAdcRaw;
							msgDebugPC[3] = (unsigned char) (valAdcRaw>>8);
							msgDebugPC[4] = 0x00;
							msgDebugPC[5] = 0x00;
							msgDebugPC[6] = 0x00;
							msgDebugPC[7] = 0x00;

							//for(int i = 0; i<8; i++)
							ptrDebug = &msgDebugPC[0];
								//AS2_PC_DEBUG_SendChar(*ptrDebug);
	         				break;
	         			/* PR2 SENSOR */
	         			case 0xB1:
	         				pc_rx_data[0] = 0x00;
	         				valAdc = ReadAdcPr2();
	         				valAdcRaw = *valAdc;
	         				msgDebugPC[0] = 0xB1;
	         				msgDebugPC[1] = 0xB0;
	         				msgDebugPC[2] = (unsigned char) valAdcRaw;
	         				msgDebugPC[3] = (unsigned char) (valAdcRaw>>8);
	         				msgDebugPC[4] = 0x00;
	         				msgDebugPC[5] = 0x00;
	         				msgDebugPC[6] = 0x00;
	         				msgDebugPC[7] = 0x00;

	         				//for(int i = 0; i<8; i++)
	         				ptrDebug = &msgDebugPC[0];
	         					//AS2_PC_DEBUG_SendChar(*ptrDebug);
	         				break;
	         			/* TM1 SENSOR */
	         			case 0xC0:
	         				pc_rx_data[0] = 0x00;
	         				valAdc = ReadAdcTm1();
	         				valAdcRaw = *valAdc;
	         				msgDebugPC[0] = 0xC0;
	         				msgDebugPC[1] = 0xB0;
	         				msgDebugPC[2] = (unsigned char) valAdcRaw;
	         				msgDebugPC[3] = (unsigned char) (valAdcRaw>>8);
	         				msgDebugPC[4] = 0x00;
	         				msgDebugPC[5] = 0x00;
	         				msgDebugPC[6] = 0x00;
	         				msgDebugPC[7] = 0x00;

	         				//for(int i = 0; i<8; i++)
	         				ptrDebug = &msgDebugPC[0];
	         					//AS2_PC_DEBUG_SendChar(*ptrDebug);
	         				break;
	         			/* TM2 SENSOR */
	         			case 0xC1:
	         				pc_rx_data[0] = 0x00;
	         				valAdc = ReadAdcTm2();
	         				valAdcRaw = *valAdc;
	         				msgDebugPC[0] = 0xC1;
	         				msgDebugPC[1] = 0xB0;
	         				msgDebugPC[2] = (unsigned char) valAdcRaw;
	         				msgDebugPC[3] = (unsigned char) (valAdcRaw>>8);
	         				msgDebugPC[4] = 0x00;
	         				msgDebugPC[5] = 0x00;
	         				msgDebugPC[6] = 0x00;
	         				msgDebugPC[7] = 0x00;

	         				//for(int i = 0; i<8; i++)
	         				ptrDebug = &msgDebugPC[0];
	         					//AS2_PC_DEBUG_SendChar(*ptrDebug);
	         				break;

	         			break;

						#endif

	         			/* tolgo coppia al motore */
	         			default:
	         				Bit3_ClrVal();
	         				break;
					}

	         	  }


	         	  /* attesa per comando da PC */
	         	  /* se lettura sensore --> acquisizione sensore e visualizzazione valore su terminale pc */
	         	  /* se pilotaggio attuatore --> invia/ricevi messaggio su rs422 e visualizza valore encoder su terminale pc */
	         	  /* altrimenti: arresta attuatore --> invia/ricevi messaggio su rs422 e visualizza valore encoder su terminale pc */

  }
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
