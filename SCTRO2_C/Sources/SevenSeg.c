//
//
//	filename:   SevenSeg.c
//	date:		23/3/2018
//	author:		SB
//
//


#include <stdio.h>
#include <string.h>
#include "D_7S_DP.h"
#include "D_7S_A.h"
#include "D_7S_B.h"
#include "D_7S_C.h"
#include "D_7S_D.h"
#include "D_7S_E.h"
#include "D_7S_F.h"
#include "D_7S_G.h"
#include "SWTimer.h"



//
// +--1--+
// 2     3
// +--4--+
// 5     6
// +--7--+
//

byte SegCf[17][7] =  { // off , 0 1 2 3 4 5 6 7 8 9 A B C D E F
		{FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE},	{ TRUE, TRUE, TRUE,FALSE, TRUE, TRUE, TRUE},
		{FALSE,FALSE, TRUE,FALSE,FALSE, TRUE,FALSE},    { TRUE,FALSE, TRUE, TRUE, TRUE,FALSE, TRUE},
		{ TRUE,FALSE, TRUE, TRUE,FALSE, TRUE, TRUE}, 	{FALSE, TRUE, TRUE, TRUE,FALSE, TRUE,FALSE},
		{ TRUE, TRUE,FALSE, TRUE,FALSE, TRUE, TRUE}, 	{ TRUE, TRUE,FALSE, TRUE, TRUE, TRUE, TRUE},
		{ TRUE,FALSE, TRUE,FALSE,FALSE, TRUE,FALSE}, 	{ TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE},
		{ TRUE, TRUE, TRUE, TRUE,FALSE, TRUE, TRUE}, 	{ TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,FALSE},
		{FALSE, TRUE,FALSE, TRUE, TRUE, TRUE, TRUE}, 	{ TRUE,FALSE, TRUE,FALSE,FALSE, TRUE, TRUE},
		{FALSE,FALSE, TRUE, TRUE, TRUE, TRUE, TRUE}, 	{ TRUE, TRUE,FALSE, TRUE, TRUE,FALSE, TRUE},
		{ TRUE, TRUE,FALSE, TRUE, TRUE,FALSE,FALSE}
};


typedef void (*TSegmentDrive)(bool val);

void DriveSegment1(bool val) {  D_7S_A_PutVal(!val); }
void DriveSegment3(bool val) {  D_7S_B_PutVal(!val); }
void DriveSegment6(bool val) {  D_7S_C_PutVal(!val); }
void DriveSegment7(bool val) {  D_7S_D_PutVal(!val); }
void DriveSegment5(bool val) {  D_7S_E_PutVal(!val); }
void DriveSegment2(bool val) {  if(!val) D_7S_F_SetVal(); else  D_7S_F_ClrVal(); }  // missing some functions in auto generated code
void DriveSegment4(bool val) {  if(!val) D_7S_G_SetVal(); else  D_7S_G_ClrVal(); }  // missing some functions in auto generated code ( ?? SB )



TSegmentDrive SetSegment[7] =
{
		DriveSegment1 , DriveSegment2 , DriveSegment3 ,  DriveSegment4 , DriveSegment5 , DriveSegment6 , DriveSegment7
};

char String2Load[20] = "";
char String2Show[20] = "";
int  String2ShowIdx = 0;
void Set7SegAscii(char aval);
void ManageSevenSeg200ms(void);
void Dot(bool Stat);

void InitSevenSeg(void)
{
	AddSwTimer(ManageSevenSeg200ms,20,TM_REPEAT);
}

bool DotStat = FALSE;
void ManageSevenSeg200ms(void)
{
	if( DotStat){
		if(strlen(String2Show) != 0){
			Set7SegAscii(String2Show[String2ShowIdx]);
			String2ShowIdx = (String2ShowIdx + 1) % strlen(String2Show);
		//	Dot(TRUE);
		}
		else{
			//Dot(FALSE);
		}
	}
	else {
		Set7SegAscii(' ');
	}
	DotStat = !DotStat;

	// if previous value shown , load new string if present
	if((String2ShowIdx == 0) && (strlen(String2Load) != 0) ){
		strcpy(String2Show,String2Load);
		String2Load[0] = 0;
	}
}


int ShowErrorSevenSeg(int errnum)
{
	strcpy(String2Load,"");
	//String2ShowIdx = 0;
	sprintf(String2Load,"E%03u    ",errnum);
}

#define CHR_BLANK 0x20

void Set7SegVal(int val)
{
int ii;
	if( val == CHR_BLANK ){
		for(ii=0;ii<7;ii++){
			SetSegment[ii](SegCf[0][ii]);
		}
	}
	else{
		for(ii=0;ii<7;ii++){
			SetSegment[ii](SegCf[val+1][ii]);
		}
	}
}


void Set7SegAscii(char aval)
{
	if((aval >= '0') && (aval <= '9')) {
		Set7SegVal(aval - '0');
	}
	else if((aval >= 'A')  && (aval <= 'F')){
		Set7SegVal(aval - 'A' + 0x0A);
	}
	else if( aval == ' '){
		Set7SegVal(CHR_BLANK);
	}
}

void Dot(bool Stat)
{
	if(Stat){
		D_7S_DP_ClrVal();
	}
	else{
		D_7S_DP_SetVal();
	}
}




