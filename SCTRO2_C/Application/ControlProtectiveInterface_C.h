/*
 * ControlProtectiveInterface.h
 *
 *  Created on: 18/apr/2018
 *      Author: W5
 */

#ifndef CONTROLPROTECTIVEINTERFACE_H_
#define CONTROLPROTECTIVEINTERFACE_H_

void onNewSensPressVal(uint16_t PressFilt, uint16_t PressArt ,
		               uint16_t PressVen, uint16_t PressLev);
void onNewPumpSpeed(uint16_t Pump0Speed, uint16_t Pump1Speed ,
		            uint16_t Pump2Speed, uint16_t Pump3Speed);
void onNewState( struct machineState* MSp, struct machineParent* MPp ,
				  struct machineChild* MCp, uint16_t Guard   );
void InitControlProtectiveInterface(void);
void onNewSensTempVal(uint16_t PressOxyg, float TempRes,
		               float TempArt, float TempVen);
void onNewPinchVal(uint8_t AirFiltStat, uint16_t AlarmCode,
		           uint8_t Pinch2WPVF, uint8_t Pinch2WPVA, uint8_t Pinch2WPVV);
void onNewOffsetPressInlet(uint8_t Offset_Press_Ven, uint8_t Offset_Press_Art);
void onNewTherapyType(void);

/*definizione dei comandi da dare alla PRO per il T1_TEST*/
typedef  enum {

       C2PCOM_NONE = 00 ,
       C2PCOM_ENABPUMPS_OFF = 01 ,
       C2PCOM_ENABPUMPS_ON = 02 ,
       C2PCOM_ENABPINCH_OFF = 20 ,
       C2PCOM_ENABPINCH_ON = 21 ,
       C2PCOM_POWER_OFF = 30 ,
       C2PCOM_POWER_ON = 31 ,
} TControl2ProtCommands;

void onNewCommadT1TEST(TControl2ProtCommands Command);

void setOffsetTempRes (int Offset); 	// funzione che scrive l'offset aggiunto via command parser per validazione protective
int getOffsetTempRes();					// funzione che restituisce l'offset aggiunto via command parser per validazione protective
void setOffsetTempArt (int Offset); 	// funzione che scrive l'offset aggiunto via command parser per validazione protective
int getOffsetTempArt();					// funzione che restituisce l'offset aggiunto via command parser per validazione protective
void setOffsetTempVen (int Offset); 	// funzione che scrive l'offset aggiunto via command parser per validazione protective
int getOffsetTempVen();					// funzione che restituisce l'offset aggiunto via command parser per validazione protective
void setOffsetTempPlate (int Offset); 	// funzione che scrive l'offset aggiunto via command parser per validazione protective
int getOffsetTempPlate();				// funzione che restituisce l'offset aggiunto via command parser per validazione protective

void setAlarm (int param); 	// funzione che scrive il valore inviatomi via command parser per il codice di allarme Control
int getAlarm();						// funzione che restituisce il valore inviatomi via command parser per il codice di allarme Control
void setArtAir (int param); 	// funzione che scrive il valore inviatomi via command parser per il sensore di aria arterioso
int getArtAir();					// funzione che restituisce il valore inviatomi via command parser per il sensore di aria arterioso
void setVenAir (int param); 	// funzione che scrive il valore inviatomi via command parser per il sensore di aria venoso
int getVenAir();					// funzione che restituisce il valore inviatomi via command parser per il sensore di aria venoso

#ifdef ENABLE_PROTECTIVE_ALARM_RESET
void onNewAlmToResetMsg(uint16_t AlmCodeToreset);
#endif

extern uint8_t *FilterPinchPos;
extern uint8_t *ArtPinchPos;
extern uint8_t *OxygPinchPos;

//Valori di pressione in arrivo dalla Protective
extern uint16_t *PressOxy      ;
extern uint16_t *PressFilter   ;
extern uint16_t *PressArt      ;
extern uint16_t *PressVen      ;
extern uint16_t *PressLevelx100;

extern uint16_t *SpeedPump1Rpmx10;
extern uint16_t *SpeedPump2Rpmx10;
extern uint16_t *SpeedPump3Rpmx10;
extern uint16_t *SpeedPump4Rpmx10;

//Temperature in arrivo dalla Protective
extern uint16_t *STA2;
extern uint16_t *STV2;
extern uint16_t *STF2;

uint16_t GetAlarmCodeProt(void);
uint16_t GetRevisionFWProt(void);


#endif /* CONTROLPROTECTIVEINTERFACE_H_ */
