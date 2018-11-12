/*
 * App_Ges.c
 *
 *  Created on: 13/giu/2016
 *      Author: W15
 */


#include "PE_Types.h"
#include "Global.h"
#include "App_Ges.h"
#include "VOLTAGE_B_CHK.h"
#include "BitIoLdd36.h"
#include "VOLTAGE_M_CHK.h"
#include "BitIoLdd37.h"
#include "Pins1.h"
#include "ASerialLdd1.h"
#include "EEPROM.h"
#include "IntFlashLdd1.h"
#include "string.h"


#include "M1_HALL_A.h"
#include "BitIoLdd48.h"
#include "M1_HALL_B.h"
#include "BitIoLdd49.h"
#include "M2_HALL_A.h"
#include "BitIoLdd50.h"
#include "M2_HALL_B.h"
#include "BitIoLdd51.h"
#include "M3_HALL_A.h"
#include "BitIoLdd52.h"
#include "M3_HALL_B.h"
#include "BitIoLdd53.h"
#include "M4_HALL_A.h"
#include "BitIoLdd54.h"
#include "M4_HALL_B.h"
#include "BitIoLdd55.h"
#include "C1_HALL_L.h"
#include "BitIoLdd56.h"
#include "C1_HALL_R.h"
#include "BitIoLdd57.h"
#include "C2_HALL_L.h"
#include "BitIoLdd58.h"
#include "C2_HALL_R.h"
#include "BitIoLdd59.h"
#include "C3_HALL_L.h"
#include "BitIoLdd60.h"
#include "C3_HALL_R.h"
#include "BitIoLdd61.h"
#include "ControlProtectiveInterface.h"
#include "FRONTAL_COVER_1.h"
#include "FRONTAL_COVER_2.h"
#include "HOOK_SENSOR_1.h"
#include "HOOK_SENSOR_2.h"


/********************************/
/* machine state initialization */
/********************************/


void Manage_Frontal_Cover(void)
{
	if (FRONTAL_COVER_1_GetVal())
		FRONTAL_COVER_1_STATUS = TRUE; // cover left opened
	else
		FRONTAL_COVER_1_STATUS = FALSE; // cover left closed

	if (FRONTAL_COVER_2_GetVal())
		FRONTAL_COVER_2_STATUS = TRUE; // cover right opened
	else
		FRONTAL_COVER_2_STATUS = FALSE; // cover right closed

}

void Manage_Hook_Sensors(void)
{
	if (HOOK_SENSOR_1_GetVal())
		HOOK_SENSOR_1_STATUS = TRUE; // vaschetta a destra non inserita
	else
		HOOK_SENSOR_1_STATUS = FALSE; // vaschetta a destra inserita

	if (HOOK_SENSOR_2_GetVal())
		HOOK_SENSOR_2_STATUS = TRUE; // vaschetta a sinistra non inserita
	else
		HOOK_SENSOR_2_STATUS = FALSE; // vaschetta a sinistra inserita
}


void EEPROM_Read(LDD_FLASH_TAddress Source, LDD_TData *Dest, LDD_FLASH_TDataSize Count)
{
	EEPROM_GetFlash(Source,Dest,Count);
}

/* Src   --> indirizzo della variabile o array da cui prendiamo i dati da scrivere
 * Dst   --> indirizzo di partenza da cui si scrive in flash (es 0xFF000)
 * Count --> numero di byte che andiamo a scrivere
 */
void EEPROM_write(EEPROM_TDataAddress Src, EEPROM_TAddress Dst, word Count)
{
	EEPROM_SetFlash(Src,Dst,Count);
}


void Set_Data_EEPROM_Default(void)
{

	unsigned char *ptr_EEPROM = (EEPROM_TDataAddress)&config_data;

	/*Calcolo il CRC sui dati letti dalla EEPROM
	 * * IL CRC lo clacolo su tutta la struttura meno i due byte ndel CRC stesso*/
	unsigned int Calc_CRC_EEPROM = ComputeChecksum(ptr_EEPROM, sizeof(config_data)-2);

	/*Se il CRC calcolato non è uguale a quello letto o la revsione non è uguale a quella attesa
	 * scrivo i parametri di default*/
	if ( config_data.EEPROM_CRC != Calc_CRC_EEPROM || config_data.EEPROM_Revision != EEPROM_REVISION)
	{
		 config_data.sensor_PRx[OXYG].prSensGain      = PR_OXYG_GAIN_DEFAULT;
		 config_data.sensor_PRx[OXYG].prSensOffset    = PR_OXYG_OFFSET_DEFAULT;

		 config_data.sensor_PRx[LEVEL].prSensGain     = PR_LEVEL_GAIN_DEFAULT;
		 config_data.sensor_PRx[LEVEL].prSensOffset   = PR_LEVEL_OFFSET_DEFAULT;

		 config_data.sensor_PRx[ADS_FLT].prSensGain   = PR_ADS_FLT_GAIN_DEFAULT;
		 config_data.sensor_PRx[ADS_FLT].prSensOffset = PR_ADS_FLT_OFFSET_DEFAULT;

		 config_data.sensor_PRx[VEN].prSensGain       = PR_VEN_GAIN_DEFAULT;
		 config_data.sensor_PRx[VEN].prSensOffset     = PR_VEN_OFFSET_DEFAULT;

		 config_data.sensor_PRx[ART].prSensGain       = PR_ART_GAIN_DEFAULT;
		 config_data.sensor_PRx[ART].prSensOffset     = PR_ART_OFFSET_DEFAULT;

		 config_data.T_Plate_Sensor_Gain_Heat         = GAIN_T_PLATE_SENS_HEAT;
		 config_data.T_Plate_Sensor_Offset_Heat       = OFFSET_T_PLATE_SENS_HEAT;

		 config_data.T_Plate_Sensor_Gain_Cold         = GAIN_T_PLATE_SENS_COLD;
		 config_data.T_Plate_Sensor_Offset_Cold       = OFFSET_T_PLATE_SENS_COLD;

		 config_data.FlowSensor_Ven_Gain              = GAIN_FLOW_SENS_VEN;
		 config_data.FlowSensor_Ven_Offset            = OFFSET_FLOW_SENS_VEN;

		 config_data.FlowSensor_Art_Gain              = GAIN_FLOW_SENS_ART;
		 config_data.FlowSensor_Art_Offset            = OFFSET_FLOW_SENS_ART;

		 //revsione della EEPROM
		 config_data.EEPROM_Revision 				  = EEPROM_REVISION;

		 /*carico il CRC della EEPROM (usata la stessa funzione di CRC del MOD_BUS
		  * IL CRC lo clacolo su tutta la struttura meno i due byte ndel CRC stesso*/
		 config_data.EEPROM_CRC = ComputeChecksum(ptr_EEPROM, sizeof(config_data)-2);

		 EEPROM_write((EEPROM_TDataAddress)&config_data, START_ADDRESS_EEPROM, sizeof(config_data));
	}

}

/*funzione del CheckSum usata anche per la EEPROM e per il protocollo con SBC*/

ActuatorHallStatus OldHallSens;

void UpdateActuatorPosition()
{
	  /*i nomi L/R delle pinch sulle get val non sono attendibili*/
int aaa =  0;


	  HallSens.PumpFilter_HSens1	 = M1_HALL_A_GetVal();
	  HallSens.PumpFilter_HSens2	 = M1_HALL_B_GetVal();
	  HallSens.PumpArt_Liver_HSens1  = M2_HALL_A_GetVal();
	  HallSens.PumpArt_Liver_HSens2	 = M2_HALL_B_GetVal();
	  HallSens.PumpOxy_1_HSens1 	 = M3_HALL_A_GetVal();
	  HallSens.PumpOxy_1_HSens2 	 = M3_HALL_B_GetVal();
	  HallSens.PumpOxy_2_HSens1 	 = M4_HALL_A_GetVal();
	  HallSens.PumpOxy_2_HSens2 	 = M4_HALL_B_GetVal();
	  HallSens.PinchFilter_Left  	 = C2_HALL_R_GetVal();
	  HallSens.PinchFilter_Right 	 = C2_HALL_L_GetVal();
	  HallSens.PinchArt_Left 	 	 = C3_HALL_R_GetVal();
	  HallSens.PinchArt_Right 	 	 = C3_HALL_L_GetVal();
	  HallSens.PinchVen_Left 	 	 = C1_HALL_L_GetVal();
	  HallSens.PinchVen_Right 	 	 = C1_HALL_R_GetVal();


		if( memcmp( &OldHallSens , &HallSens, sizeof(HallSens) ) != 0  ){
			// some change

			if( HallSens.PumpFilter_HSens1 != OldHallSens.PumpFilter_HSens1)
			{
				aaa = 1;
			}
			if( HallSens.PumpFilter_HSens2 != OldHallSens.PumpFilter_HSens2 )
			{
				aaa = 1;
			}
			if( HallSens.PumpArt_Liver_HSens1 != OldHallSens.PumpArt_Liver_HSens1 )
			{
				aaa = 1;
			}
			if( HallSens.PumpArt_Liver_HSens2 != OldHallSens.PumpArt_Liver_HSens2 )
			{
				aaa = 1;
			}
			if( HallSens.PumpOxy_1_HSens1 != OldHallSens.PumpOxy_1_HSens1 )
			{
				aaa = 1;
			}
			if( HallSens.PumpOxy_1_HSens2 != OldHallSens.PumpOxy_1_HSens2 )
			{
				aaa = 1;
			}
			if( HallSens.PumpOxy_2_HSens1 != OldHallSens.PumpOxy_2_HSens1 )
			{
				aaa = 1;
			}
			if( HallSens.PumpOxy_2_HSens2 != OldHallSens.PumpOxy_2_HSens2 )
			{
				aaa = 1;
			}
			OldHallSens = HallSens;
		}


	  onNewPinchStat(HallSens);

}
