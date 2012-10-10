/***************************************************************************
                          capteur.h  -  description
                             -------------------
    begin                : 17 janv. 2012
    copyright            : (C) 2012 by alexandre COFFIGNAL
    email                :

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef CAPTEUR_H_
#define CAPTEUR_H_

#include "superviseur.h"
#define NB_CAPTEUR	30

typedef enum
{
	TH=0,
	PE,
	PO,
}tTypeCapteur;

typedef enum
{
	TEMP1=0,
	TEMP2,
	HUMI,
	PRESS,
	VOLT,
	MAX_GP
}tGrandeurPhys;

extern const char * _acGPcolor[];
extern const char * _acGPName[];
extern const char * _acGPRep[];
extern const char * _acGrandeur[];
#define TH_NAME	"ThermRf"
#define PE_NAME	"PicEpeiche"
#define PO_NAME	"PicOlive"

typedef struct
{
	tTypeCapteur tType;
	unsigned int uiNumCapteur;
	float fTemp1;
	float fTemp2;
	unsigned int uiPression;
	unsigned char ucHumi;
	float fBat;
	time_t tTime;

}tCapteur;

#define CAPTEUR_NUM_TEST 666


extern bool Capteur_bFormatTH(tCapteur *stCapteur,int iNumCapteur,int iTemp1);
extern bool Capteur_bFormatTHV2(tCapteur *stCapteur,int iNumCapteur,int iTemp1,unsigned int uiBat);
extern bool Capteur_bFormatPE(tCapteur *stCapteur,int iNumCapteur,int iTemp1,int iPression);
extern bool Capteur_bFormatPO(tCapteur *stCapteur,int iNumCapteur,int iTemp1,int iPression,int iHumi,int iTemp2);
extern bool Capteur_bCompare(tCapteur tCp1,tCapteur tCp2);
extern bool Capteur_IsEnModeTest(tCapteur tCp1);
extern char *Capteur_ac2String(tTypeCapteur tCpt);
extern void Capteur_bPrint(tCapteur stCapteur);
extern void Capteur_vSet(tCapteur * tCp,	tTypeCapteur tType,	unsigned int uiNumCapteur,
		float fTemp1,float fTemp2,unsigned int uiPression,unsigned char ucHumi
		,float fBat,time_t tTime);
#endif /* CAPTEUR_H_ */
