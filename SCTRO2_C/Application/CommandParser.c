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



void CommandExecute( int argc, char** argv);
char** ExtractTokens(char* CommandNParamsString);
void SetMulti(int NParams, char** Params);
void GetMulti(int NParams, char** Params);
void SetHeater(int NParams, char** Params);
void SetCooler(int NParams, char** Params);

void ErrorCommandNotFound( int NParams, char** Params);
void ErrorNParamsNotOk( int NParams, char** Params);
void ErrorParamsNotOk( int NParams, char** Params);
void GetDoorsStat(int NParams, char** Params);
void GetReservoirHooks(int NParams, char** Params);

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

void DrawLion(int NParams, char** Params);

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
		if(strcmp_cr(Params[0],"heater") == 0) SetHeater(NParams-1, Params+1) ;
		else if(strcmp_cr(Params[0],"cooler") == 0) SetCooler(NParams-1, Params+1) ;
		else {
			//ErrorParamsNotOk( NParams, Params);
			CommandAnswer("set heater/cooler");
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
		else if(strcmp_cr(Params[0],"doors") == 0) GetDoorsStat(NParams-1, Params+1) ;
		else if(strcmp_cr(Params[0],"reservoir_hooks") == 0) GetReservoirHooks(NParams-1, Params+1) ;
		else if(strcmp_cr(Params[0],"pumps_hall") == 0) GetPumpsHall(NParams-1, Params+1) ;
		else {
			//ErrorParamsNotOk( NParams, Params);
			CommandAnswer("get temp/press/pumps/pinch/errors/doors/reservoir_hooks  params ");
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
			CommandAnswer("set heater onf [percent --> 0 for stop]");
			return;
		}
	}
	else {
		HeaterPower = 80;
	}

	HeaterOnOff(HeaterPower );
	CommandAnswer("set heater done");
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

void GetPumpsHall(int NParams, char** Params)
{
	SetLogCommand('H');
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

void GetDoorsStat(int NParams, char** Params)
{
	if (FRONTAL_COVER_1_STATUS == FALSE && FRONTAL_COVER_2_STATUS == FALSE)
		CommandAnswer("Doors are CLOSED");
	else if (FRONTAL_COVER_1_STATUS == TRUE && FRONTAL_COVER_2_STATUS == FALSE)
		CommandAnswer("Doors --> left is OPENED, right is CLOSED ");
	else if (FRONTAL_COVER_1_STATUS == FALSE && FRONTAL_COVER_2_STATUS == TRUE)
		CommandAnswer("Doors --> left is CLOSED, right is OPENED ");
	else if (FRONTAL_COVER_1_STATUS == TRUE && FRONTAL_COVER_2_STATUS == TRUE)
		CommandAnswer("Doors are OPENED");
}

void GetReservoirHooks(int NParams, char** Params)
{
	if (HOOK_SENSOR_1_STATUS == FALSE && HOOK_SENSOR_2_STATUS == FALSE)
		CommandAnswer("Reservoir hooks are hooked");
	else if (HOOK_SENSOR_1_STATUS == TRUE && HOOK_SENSOR_2_STATUS == FALSE)
		CommandAnswer("Reservoir hook --> left is hooked, right is NOT hooked ");
	else if (HOOK_SENSOR_1_STATUS == FALSE && HOOK_SENSOR_2_STATUS == TRUE)
		CommandAnswer("Reservoir hook --> left is NOT hooked, right is hooked ");
	else if (HOOK_SENSOR_1_STATUS == TRUE && HOOK_SENSOR_2_STATUS == TRUE)
		CommandAnswer("Reservoir hooks are NOT hooked");
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






