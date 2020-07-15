/*
 * CommandParser.c
 *
 *  Created on: 13/feb/2019
 *      Author: W32
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "stdint.h"
#include "Global.h"
#include "PC_DEBUG_COMM.h"
#include "App_Ges.h"
#include "general_func.h"
#include "ModBusCommProt.h"
#include "ControlProtectiveInterface_C.h"


void CommandExecute( int argc, char** argv);
char** ExtractTokens(char* CommandNParamsString);
void SetMulti(int NParams, char** Params);
void GetMulti(int NParams, char** Params);
void SetHeater(int NParams, char** Params);
void SetCooler(int NParams, char** Params);
void SetCalibTemp(int NParams, char** Params);
void SetCalibFlowSens(int NParams, char** Params);
void SetDiffTemp(int NParams, char** Params);
void SetDiffTemp(int NParams, char** Params);
void SetAlarm(int NParams, char** Params);
void SetAirValue(int NParams, char** Params);

void ErrorCommandNotFound( int NParams, char** Params);
void ErrorNParamsNotOk( int NParams, char** Params);
void ErrorParamsNotOk( int NParams, char** Params);
void GetDoorsStat(int NParams, char** Params);
void GetReservoirHooks(int NParams, char** Params);
void GetPumps_covers(int NParams, char** Params);
void GetAirSensors(int NParams, char** Params);
void GetCanBus(int NParams, char** Params);
void GetVoltage(int NParams, char** Params);

int strcmp_cr(char* strToCheck, char* strReference );
int str_NoCr(char* strToCheck);

// stubs
void CommandAnswer(char* message);
void HeaterOnOff(int HeaterPower);
void CoolerOnOff(int CoolerPower);

void GetTemp(int NParams, char** Params);
void GetPress(int NParams, char** Params);
void GetPumps(int NParams, char** Params);
void GetPinches(int NParams, char** Params);
void GetErrors(int NParams, char** Params);
void GetPumpsHall(int NParams, char** Params);
void GetFWVersion(int NParams, char** Params);
void GetParamsVal(int NParams, char** Params);
void EnableMulti(int NParams, char** Params);
void EnablePump(int NParams, char** Params);
void EnablePinch(int NParams, char** Params);
void EnablePower(int NParams, char** Params);

void DrawLion(int NParams, char** Params);

static int OffsetTempRes   = 0; // variabile in cui scrivo l'offset aggiunto via command parser per validazione protective
static int OffsetTempArt   = 0; // variabile in cui scrivo l'offset aggiunto via command parser per validazione protective
static int OffsetTempVen   = 0; // variabile in cui scrivo l'offset aggiunto via command parser per validazione protective
static int OffsetTempPlate = 0; // variabile in cui scrivo l'offset aggiunto via command parser per validazione protective
static int AirValueArt	   = 0; // variabile in cui scrivo il valore inviatomi via command parser per il sensore di aria arterioso
static int AirValueVen	   = 0; // variabile in cui scrivo il valore inviatomi via command parser per il sensore di aria Venoso
static int AlarmValue	   = 0; // variabile in cui scrivo il valore inviatomi via command parser per il codice di allarme Control

typedef void(*TCommandAction)(int argc, char** argv);// TAlarmAction;

struct CommandNParams {
	char* Command;
	int NParams; //  min N params
	TCommandAction CmdAction;
};

// da fare
// set heater ... ,  set cooler ... ,  get temp , get temp * , get  pinch , get pupms speed , get pumps hall , ...
// * indica che viene fatto il log continuo
// si esce dal log continuo con esc
// con esc si cancella anche il comando attuale

struct CommandNParams CommandsAvailable[] =
{
		{ "set", 2 , SetMulti },
		{ "get", 2 , GetMulti },
		{ "enable", 2 , EnableMulti },
		{ "DrawLion", 2 , DrawLion }
};


void ParseNExecuteCommand( char* CommadnNParamsString)
{
char** argv;
char** tempargv;
int argc=0;

	argv = ExtractTokens( CommadnNParamsString );
	tempargv = argv;
	while(*argv++ != NULL) argc++;
	CommandExecute(argc, tempargv);
}

void CommandExecute( int argc, char** argv)
{

	int ii;
	for(ii=0; ii<(sizeof(CommandsAvailable) / sizeof(struct CommandNParams)); ii++)
	{
		if(strcmp_cr( *argv , CommandsAvailable[ii].Command) == 0){
			// command match
			CommandsAvailable[ii].CmdAction(argc-1,argv+1);
			break;
		}
	}
	if( ii == (sizeof(CommandsAvailable) / sizeof(struct CommandNParams)) )
	{
		ErrorCommandNotFound( argc , argv);
	}
}



char** ExtractTokens( char* CommandNParamsString)
{

	static char* tokens[10]; // max cmd and 9 params allowed

	tokens[0] = strtok(CommandNParamsString, " ");
	/* walk through other tokens */
	int ii=0;
	while( tokens[ii++] != NULL ) {
	      tokens[ii] = strtok(NULL, " " );
	      if(ii >=9) break;
	}
	return tokens;
}


///////////////////////////////////////////////
// Functions to execute when command received
///////////////////////////////////////////////

void SetMulti(int NParams, char** Params)
{
	// execute
		if(NParams < 1){
			ErrorNParamsNotOk(NParams, Params);
			return;
		}
		// parse what to set
		if(strcmp_cr(Params[0],"heater") == 0) SetHeater(NParams-1, Params+1);
		else if(strcmp_cr(Params[0],"cooler") == 0) SetCooler(NParams-1, Params+1);
		else if (strcmp_cr(Params[0],"calibtemp") == 0) SetCalibTemp(NParams-1, Params+1);
		else if (strcmp_cr(Params[0],"calibflowsens") == 0) SetCalibFlowSens(NParams-1, Params+1);
		else if (strcmp_cr(Params[0],"difftemp") == 0) SetDiffTemp(NParams-1, Params+1);
		else if (strcmp_cr(Params[0],"alarm") == 0) SetAlarm(NParams-1, Params+1);
		else if (strcmp_cr(Params[0],"airvalue") == 0) SetAirValue(NParams-1, Params+1);
		else {
			//ErrorParamsNotOk( NParams, Params);
			CommandAnswer("set heater/cooler/calibtemp/calibflowsens/difftemp/alarm/airvalue");
			return;
		}
}

void GetMulti(int NParams, char** Params)
{
	// execute
		if(NParams < 1){
			ErrorNParamsNotOk(NParams, Params);
			return;
		}
		// parse what to set
		if(strcmp_cr(Params[0],"temp") == 0) GetTemp(NParams-1, Params+1);
		else if(strcmp_cr(Params[0],"press") == 0) GetPress(NParams-1, Params+1) ;
		else if(strcmp_cr(Params[0],"pumps") == 0) GetPumps(NParams-1, Params+1) ;
		else if(strcmp_cr(Params[0],"pinch") == 0) GetPinches(NParams-1, Params+1) ;
		else if(strcmp_cr(Params[0],"errors") == 0) GetErrors(NParams-1, Params+1) ;
		else if(strcmp_cr(Params[0],"covers") == 0) GetPumps_covers(NParams-1, Params+1) ;
		else if(strcmp_cr(Params[0],"air") == 0) GetAirSensors(NParams-1, Params+1) ;
		else if(strcmp_cr(Params[0],"canbus") == 0) GetCanBus(NParams-1, Params+1) ;
		else if(strcmp_cr(Params[0],"voltage") == 0) GetVoltage(NParams-1, Params+1) ;
		else if(strcmp_cr(Params[0],"doors") == 0) GetDoorsStat(NParams-1, Params+1) ;
		else if(strcmp_cr(Params[0],"hooks") == 0) GetReservoirHooks(NParams-1, Params+1) ;
		else if(strcmp_cr(Params[0],"FW_version") == 0) GetFWVersion(NParams-1, Params+1) ;
		else if(strcmp_cr(Params[0],"params") == 0) GetParamsVal(NParams-1, Params+1) ;
		else {
			//ErrorParamsNotOk( NParams, Params);
			CommandAnswer("get temp/press/pumps/pinch/errors/covers/air/canbus/voltage/doors/hooks/FW_version/params ");
			return;
		}
}

void SetHeater(int NParams, char** Params)
{
	bool HeaterOn;
	int HeaterPower;
	char testStr[30];

	// execute
	if((NParams < 1) || (NParams > 2)){
		ErrorNParamsNotOk(NParams, Params);
		return;
	}
	// parse on off
	if(strcmp_cr(Params[0],"on") == 0) HeaterOn = true;
	else if(strcmp_cr(Params[0],"off") == 0) HeaterOn = false;
	else {
		//ErrorParamsNotOk( NParams, Params);
		CommandAnswer("SetHeater on [percent --> 0 for stop]");
		return;
	}
	// param percent , must be a number
	if( NParams == 2 ){
		str_NoCr(Params[1]);
		if(strspn(Params[1], "0123456789") == strlen(Params[1])){
			sscanf(Params[1] , "%d" , &HeaterPower);
			if( HeaterPower > 100) {
				CommandAnswer("percent should be <= 100");
				return;
			}
		}
		else {
			CommandAnswer("set heater on [percent --> 0 for stop]");
			return;
		}
	}
	else {
		HeaterPower = 80;
	}

	HeaterOnOff(HeaterPower );
	CommandAnswer("set heater done");
}
void setOffsetTempRes(int Offset)
{
	OffsetTempRes   = Offset;
}

void setOffsetTempArt(int Offset)
{
	OffsetTempArt   = Offset;
}

void setOffsetTempVen(int Offset)
{
	OffsetTempVen   = Offset;
}

void setOffsetTempPlate(int Offset)
{
	OffsetTempPlate   = Offset;
}

int getOffsetTempRes()
{
	return(OffsetTempRes);
}

int getOffsetTempArt()
{
	return(OffsetTempArt);
}

int getOffsetTempVen()
{
	return(OffsetTempVen);
}

int getOffsetTempPlate()
{
	return(OffsetTempPlate);
}


/*funzione che aggiunge un offset impostabile
 * alla temperatura inviata alla Pro (serve per la validazione PRO)*/
void SetDiffTemp(int NParams, char** Params)
{
	unsigned char SensNum = 0;
	int DiffTemp = 0;

	/*servono due parametri, uno per scegliere il sensore, l'altro per impostare la differenza*/
	if (NParams == 2)
	{
		if (strcmp_cr(Params[0],"1") == 0)
		{
			/*è stato selezionato il sensore di temperatura Arterioso*/
			SensNum = 1;
		}
		else if (strcmp_cr(Params[0],"2") == 0)
		{
			/*è stato selezionato il sensore di temperatura Reservoire*/
			SensNum = 2;
		}
		else if (strcmp_cr(Params[0],"3") == 0)
		{
			/*è stato selezionato il sensore di temperatura Venoso*/
			SensNum = 3;
		}
		else if (strcmp_cr(Params[0],"4") == 0)
		{
			/*è stato selezionato il sensore di temperatura Piastra*/
			SensNum = 4;
		}
		else
		{
			CommandAnswer("Sensor Number should be 1 for Arterious, 2 for Reservoire, 3 for Venous, 4 for Plate");
			return;
		}
		/*per toglie il 'cr'*/
		str_NoCr(Params[1]);

		if(strspn(Params[1], "0123456789.-") == strlen(Params[1])){
			sscanf(Params[1] , "%d" , &DiffTemp);
		}

		switch(SensNum)
		{
			case 1:
				setOffsetTempArt (DiffTemp);
				break;
			case 2:
				setOffsetTempRes (DiffTemp);
				break;
			case 3:
				setOffsetTempVen (DiffTemp);
				break;
			case 4:
				setOffsetTempPlate (DiffTemp);
				break;
			default:
				CommandAnswer("Sensor Number should be 1 for Arterious, 2 for Reservoire, 3 for Venous, 4 for Plate");
				break;
		}

	}
	else
	{
		//messaggio di errore
		CommandAnswer("wrong number of parameters");
	}
}

void setAlarm (int param)
{
	AlarmValue = param;
}

int getAlarm()
{
	return (AlarmValue);
}

/*funzione che imposta un allarme da inviare alla PRO
 * senza che la CON debba andare in allarme --> serve per validazione PRO
 * inviamo solo 1 parametro che è il codice di allarme*/
void SetAlarm(int NParams, char** Params)
{
	int AlaValue = 0;

	/*serve 1 parametro*/
	if (NParams == 1)
	{
		/*per toglie il 'cr'*/
		str_NoCr(Params[0]);

		if(strspn(Params[0], "0123456789") == strlen(Params[0]))
	    {
			sscanf(Params[0] , "%d" , &AlaValue);
			setAlarm (AlaValue);
	    }
	}
	else
	{
		//messaggio di errore
		CommandAnswer("wrong number of parameters");
	}
}


void setArtAir (int param)
{
	AirValueArt = param;
}

int getArtAir()
{
	return (AirValueArt);
}

void setVenAir (int param)
{
	AirValueVen = param;
}

int getVenAir()
{
	return (AirValueVen);
}

/*funzione che imposta un valore di aria su un dei due sensori
 * di aria/flusso per permettere di simulare un allarme da inviare alla PRO
 * Serve per validazione PRO
 * inviamo due parametri: numero sens (1 arterioso e 2 venoso) e il valore numerico da inviare*/
void SetAirValue(int NParams, char** Params)
{
	unsigned char SensNum = 0;
	int AirValue = 0;

	/*servono due parametri, uno per scegliere il sensore, l'altro per impostare il valore*/
	if (NParams == 2)
	{
		if (strcmp_cr(Params[0],"1") == 0)
		{
			/*è stato selezionato il sensore di aria Arterioso*/
			SensNum = 1;
		}
		else if (strcmp_cr(Params[0],"2") == 0)
		{
			/*è stato selezionato il sensore di aria Venoso*/
			SensNum = 2;
		}
		else
		{
			CommandAnswer("Sensor Number should be 1 for Arterious, 2 for Venous");
			return;
		}
		/*per toglie il 'cr'*/
		str_NoCr(Params[1]);

		if(strspn(Params[1], "0123456789") == strlen(Params[1])){
			sscanf(Params[1] , "%d" , &AirValue);
		}

		switch(SensNum)
		{
			case 1:
				setArtAir (AirValue);
				break;
			case 2:
				setVenAir (AirValue);
				break;
			default:
				CommandAnswer("Sensor Number should be 1 for Arterious, 2 for Venous");
				break;
		}
}
	else
	{
		//messaggio di errore
		CommandAnswer("wrong number of parameters");
	}
}

void SetCalibFlowSens(int NParams, char** Params)
{
	unsigned char SensNum = 0;
	float Gain, Offset = 0;
	unsigned char *ptr_EEPROM = (EEPROM_TDataAddress)&config_data;

	if (NParams == 3)
	{
		if (strcmp_cr(Params[0],"1") == 0)
		{
			/*è stato selezionato il sensore di flusso Arterioso*/
			SensNum = 1;
		}
		else if (strcmp_cr(Params[0],"2") == 0)
		{
			/*è stato selezionato il sensore di flusso Venoso*/
			SensNum = 2;
		}
		else
		{
			CommandAnswer("Sensor Number should be 1 for Arterious, or 2 for Venous");
			return;
		}
		/*per toglie il 'cr'*/
		str_NoCr(Params[2]);

		if(strspn(Params[1], "0123456789.") == strlen(Params[1])){
			sscanf(Params[1] , "%f" , &Gain);
		}
		else{
			CommandAnswer("Gain not ok");
			return;
		}
		if(strspn(Params[2], "0123456789.") == strlen(Params[2]))
		{
			sscanf(Params[2] , "%f" , &Offset);
		}
		else
		{
			CommandAnswer("Offset not ok");
			return;
		}

		switch(SensNum)
		{
			case 1:
				config_data.FlowSensor_Art_Gain   = Gain;
				config_data.FlowSensor_Art_Offset = Offset;
				break;
			case 2:
				config_data.FlowSensor_Ven_Gain   = Gain;
				config_data.FlowSensor_Ven_Offset = Offset;
				break;
			default:
				CommandAnswer("Sensor Number should be 1 for Arterious, or 2 for Venous");
				break;
		}
		/*carico il CRC della EEPROM (usata la stessa funzione di CRC del MOD_BUS
		* IL CRC lo clacolo su tutta la struttura meno i due byte ndel CRC stesso*/
		config_data.EEPROM_CRC = ComputeChecksum(ptr_EEPROM, sizeof(config_data)-2);
		/*finita la calibrazione di un sensore la vado subito a salvare in EEPROM*/
		EEPROM_write((EEPROM_TDataAddress)&config_data, START_ADDRESS_EEPROM, sizeof(config_data));
		FlowSensCalibDone = TRUE;
	}
	else
	{
		//messaggio di errore
		CommandAnswer("wrong number of parameters");
	}
}

void SetCalibTemp(int NParams, char** Params)
{
	unsigned char SensNum = 0;
	float TempVal1, TempVal2, TempVal3 = 0;
	unsigned char *ptr_EEPROM = (EEPROM_TDataAddress)&config_data;

	if (NParams == 4)
	{
		if (strcmp_cr(Params[0],"1") == 0)
		{
			SensNum = 1;
		}
		else if (strcmp_cr(Params[0],"2") == 0)
		{
			SensNum = 2;
		}
		else if (strcmp_cr(Params[0],"3") == 0)
		{
			SensNum = 3;
		}
		else
		{
			CommandAnswer("Sensor Number should be 1, 2 or 3");
			return;
		}

		str_NoCr(Params[2]);

		if(strspn(Params[1], "0123456789.") == strlen(Params[1])){
			sscanf(Params[1] , "%f" , &TempVal1);
		}
		else{
			CommandAnswer("Low temperature not ok");
			return;
		}
		if(strspn(Params[2], "0123456789.") == strlen(Params[2]))
		{
			sscanf(Params[2] , "%f" , &TempVal2);
		}
		else
		{
			CommandAnswer("Middle temperature not ok");
			return;
		}
		if(strspn(Params[3], "0123456789.") == strlen(Params[3]))
		{
			sscanf(Params[3] , "%f" , &TempVal3);
		}
		else
		{
			CommandAnswer("High temperature not ok");
			return;
		}

		switch(SensNum){
		case 1:
			config_data.T_sensor_ART_Meas_Low  = TempVal1;
			config_data.T_sensor_ART_Meas_Med  = TempVal2;
			config_data.T_sensor_ART_Meas_High = TempVal3;
			break;
		case 2:
			config_data.T_sensor_RIC_Meas_Low  = TempVal1;
			config_data.T_sensor_RIC_Meas_Med  = TempVal2;
			config_data.T_sensor_RIC_Meas_High = TempVal3;
			break;
		case 3:
			config_data.T_sensor_VEN_Meas_Low  = TempVal1;
			config_data.T_sensor_VEN_Meas_Med  = TempVal2;
			config_data.T_sensor_VEN_Meas_High = TempVal3;
			break;
		}
		/*carico il CRC della EEPROM (usata la stessa funzione di CRC del MOD_BUS
		* IL CRC lo clacolo su tutta la struttura meno i due byte ndel CRC stesso*/
		config_data.EEPROM_CRC = ComputeChecksum(ptr_EEPROM, sizeof(config_data)-2);
		/*finita la calibrazione di un sensore la vado subito a salvare in EEPROM*/
		EEPROM_write((EEPROM_TDataAddress)&config_data, START_ADDRESS_EEPROM, sizeof(config_data));
	}
	else
	{
		//messaggio di errore
		CommandAnswer("wrong number of parameters");
	}
}

void SetCooler(int NParams, char** Params)
{
	// execute
	bool CoolerOn;
	int CoolerPower;

	// execute
	if((NParams < 1) || (NParams > 2)){
		ErrorNParamsNotOk(NParams, Params);
		return;
	}
	// parse on off
	if(strcmp_cr(Params[0],"on") == 0) CoolerOn = true;
	else if(strcmp_cr(Params[0],"off") == 0) CoolerOn = false;
	else {
		//ErrorParamsNotOk( NParams, Params);
		CommandAnswer("set cooler on/off [percent]");
		return;
	}
	// param percent , must be a number
	if( NParams == 2 ){
		str_NoCr(Params[1]);
		if(strspn(Params[1], "0123456789") == strlen(Params[1])){
			sscanf(Params[1] , "%d" , &CoolerPower);
			if( CoolerPower > 100) 		{
				CommandAnswer("percent should be <= 100");
				return;
			}
		}
		else {
			CommandAnswer("set cooler on/off [percent]");
			return;
		}
	}
	else {
		CoolerPower = 80;
	}

	CoolerOnOff(CoolerPower );
	CommandAnswer("set cooler done");

}

void SetLogCommand( uint8_t Command);

void GetTemp(int NParams, char** Params)
{
	SetLogCommand('T');
}

void GetPress(int NParams, char** Params)
{
	SetLogCommand('P');
}

void GetPumps(int NParams, char** Params)
{
	SetLogCommand('M');
}

void GetPinches(int NParams, char** Params)
{
	SetLogCommand('C');
}

void GetErrors(int NParams, char** Params)
{
	SetLogCommand('E');
}

void GetPumps_covers(int NParams, char** Params)
{
	SetLogCommand('1');
}

void GetAirSensors(int NParams, char** Params)
{
	SetLogCommand('2');
}

void GetCanBus(int NParams, char** Params)
{
	SetLogCommand('3');
}

void GetVoltage(int NParams, char** Params)
{
	SetLogCommand('4');
}

void GetDoorsStat(int NParams, char** Params)
{
	SetLogCommand('5');
}

void GetReservoirHooks(int NParams, char** Params)
{
	SetLogCommand('6');
}

void GetFWVersion(int NParams, char** Params)
{
	SetLogCommand('7');
}

void GetParamsVal(int NParams, char** Params)
{
	char	str1[100];
	CommandAnswer("calib press ossigeno\r\n");
	sprintf(str1,"gain = %f , offset = %f\r\n\r\n",
	config_data.sensor_PRx[OXYG].prSensGain,config_data.sensor_PRx[OXYG].prSensOffset);
	CommandAnswer(str1);

	CommandAnswer("calib flussimetro venoso\r\n");
	sprintf(str1,"gain = %f , offset = %f\r\n\r\n",
	config_data.FlowSensor_Ven_Gain,config_data.FlowSensor_Ven_Offset);
	CommandAnswer(str1);

	CommandAnswer("calib flussimetro arterioso\r\n");
	sprintf(str1,"gain = %f , offset = %f\r\n\r\n",
	config_data.FlowSensor_Art_Gain ,config_data.FlowSensor_Art_Offset);
	CommandAnswer(str1);
}


void DrawLion(int NParams, char** Params)
{
	CommandAnswer("\r\n\r\n\r\n");
	CommandAnswer("    /\";;:;;\"\\\r\n");
	CommandAnswer("  (:;/\\,-,/\\;;)\\\r\n");
	CommandAnswer(" (:;{  d b  }:;)\\\r\n");
	CommandAnswer("  (:;\\__Y__/;;)-----------,,_\\\r\n");
	CommandAnswer("   ,..\\  ,..\\      ___/___)__`\\\\\r\n");
	CommandAnswer("   (,,,)~(,,,)`-._##____________)\\\r\n");
	CommandAnswer("\r\n\r\n\r\n");
}

void ErrorCommandNotFound( int NParams, char** Params)
{
	CommandAnswer("Command not found");
}

void ErrorNParamsNotOk( int NParams, char** Params)
{
	CommandAnswer("Number of parameters not ok!");
}

void ErrorParamsNotOk( int NParams, char** Params)
{
	CommandAnswer("Some parameter not ok!");
}

//
//
//	Stubs functions
//
//
void CommandAnswer(char* message)
{
word sent_data;
	PC_DEBUG_COMM_SendBlock(message, strlen(message) , &sent_data);
	// send to serial
}

void HeaterOnOff(int HeaterPower )
{
	// fa partire le resistenze riscaldanti
	if(HeaterPower == 0)
	{
		StopHeating();
		setHeaterOn(FALSE);
		setHeatingPwmPerc(0);
		/*Se do il comando di spegnerlo
		 * disattivo la flag della partenza in debug*/
		START_HEAT_ON_DEBUG  = FALSE;
	}
	else
	{
		/*Flag per identificare la partenza del riscaldatore in debug*/
		START_HEAT_ON_DEBUG  = TRUE;
		START_FRIGO_ON_DEBUG = FALSE;

		setHeaterOn(TRUE);
		/*per sicurezza disabilito il frigo*/
		/*per sicurezza spengo il riscaldatore*/
		CoolerOnOff(0);
		updateMaxTempPlate();
		setHeatingPwmPerc(HeaterPower);
	}
}

void CoolerOnOff(int CoolerPower)
{
	// fa partire le il compressore del frigo
	if(CoolerPower == 0)
	{
		setFrigoOn(FALSE);
		StopFrigo(); //per sicurezza
		setFrigoPercDebug(0);
		Enable_AMS = FALSE;
		/*Se do il comando di spegnerlo
		 * disattivo la flag della partenza in debug*/
		START_FRIGO_ON_DEBUG = FALSE;
	}
	else
	{
		/*Flag per identificare la partenza del riscaldatore in debug*/
		START_HEAT_ON_DEBUG  = FALSE;
		START_FRIGO_ON_DEBUG = TRUE;

		setFrigoOn(TRUE);
		updateMinTempPlate();
		setFrigoPercDebug(CoolerPower);
		Enable_AMS = TRUE;
		/*per sicurezza spengo il riscaldatore*/
		HeaterOnOff(0);

	}

}

void EnableMulti(int NParams, char** Params)

{
       // execute
             if(NParams < 1){
                    ErrorNParamsNotOk(NParams, Params);
                    return;
             }
             // parse what to set
             if(strcmp_cr(Params[0],"pump") == 0) EnablePump(NParams-1, Params+1);
             else if(strcmp_cr(Params[0],"pinch") == 0) EnablePinch(NParams-1, Params+1);
             else if (strcmp_cr(Params[0],"power") == 0) EnablePower(NParams-1, Params+1);

             else {
                    //ErrorParamsNotOk( NParams, Params);
                    CommandAnswer("enable pumps/pinchs/power on/off");
                    return;
             }
}

void EnablePump(int NParams, char** Params)
{
	bool enab_val;

       // parse on off
       if(NParams != 1)
       {
             CommandAnswer("enable pump on/off");
             return;
       }
       if(strcmp_cr(Params[0],"on") == 0) enab_val = TRUE;
       else if(strcmp_cr(Params[0],"off") == 0) enab_val = FALSE;

       else
       {
             //ErrorParamsNotOk( NParams, Params);
             CommandAnswer("enable pump on/off");
             return;
       }

       if( enab_val )
       	   EN_Motor_Control(ENABLE);
       else // if (enab_val == FALSE)
       	   EN_Motor_Control(DISABLE);
}

void EnablePinch(int NParams, char** Params)
{
	bool enab_val;

       // parse on off
       if(NParams != 1){
             CommandAnswer("enable pinch on/off");
             return;
       }
       if(strcmp_cr(Params[0],"on") == 0) enab_val = TRUE;
       else if(strcmp_cr(Params[0],"off") == 0) enab_val = FALSE;

       else
       {
             //ErrorParamsNotOk( NParams, Params);
             CommandAnswer("enable pinch on/off");
             return;
       }

       if( enab_val )
		   EN_Clamp_Control(ENABLE);
	   else // if (enab_val == FALSE)
		   EN_Clamp_Control(DISABLE);
}

void EnablePower(int NParams, char** Params)
{
	bool enab_val;

       // parse on off
       if(NParams != 1){
             CommandAnswer("enable power on/off");
             return;
       }
       if(strcmp_cr(Params[0],"on") == 0) enab_val = TRUE;
       else if(strcmp_cr(Params[0],"off") == 0) enab_val = FALSE;

       else
       {
             //ErrorParamsNotOk( NParams, Params);
             CommandAnswer("enable power on/off");
             return;
       }

       if(enab_val)
    	   EN_24_M_C_Management(ENABLE);
	   else // if (enab_val == FALSE)
		   EN_24_M_C_Management(DISABLE);
}

//
// if str to check include final cr , neglect it to avoid
// check not matching because of final cr
//
int strcmp_cr(char* strToCheck, char* strReference )
{
	int sl = strlen( strToCheck );
	if( strToCheck[sl-1] == '\r' )
		strToCheck[sl-1] = 0;

	return strcmp(strToCheck, strReference);
}

//
// remove cr from end of string
//
int str_NoCr(char* strToCheck)
{
	int sl = strlen( strToCheck );
	if( strToCheck[sl-1] == '\r' )
		strToCheck[sl-1] = 0;
}






