/***************************************************************************
                          capteur.c  -  description
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

#include <stdio.h>
#include <time.h>
#include "capteur.h"
#include "superviseur.h"

// Grandeur Physique TEMP1 TEMP2, HUMI, PRESS, VOLT
const char * _acGPcolor[]= {"red","red","blue","green","grey"};
const char * _acGPName[]= {"Temperature","Temperature","Humidite","Pression","Tension"};
const char * _acGPRep[]= {"temp","temp2","humi","press","volt"};
const char * _acGrandeur[]= {"°C","°C","%%","hPa","volt"};


char *Capteur_ac2String(tTypeCapteur tCpt)
{
	switch(tCpt)
	{
	case 	TH : return TH_NAME; break;
	case 	PE : return PE_NAME; break;
	case 	PO : return PO_NAME; break;
	default : return "inconnu"; break;
	}

}
bool Capteur_bFormatTH(tCapteur *stCapteur,int iNumCapteur,int iTemp1)
{
	stCapteur->tType=TH;
	stCapteur->uiNumCapteur=iNumCapteur;
	stCapteur->ucHumi=0;
	stCapteur->uiPression=0;

	stCapteur->fTemp1=(float)((short)iTemp1)/256;
	stCapteur->fTemp2=0;
	stCapteur->fBat=0;
	stCapteur->tTime = time(NULL);

	return true;
}

bool Capteur_bFormatTHV2(tCapteur *stCapteur,int iNumCapteur,int iTemp1,unsigned int uiBat)
{
	Capteur_bFormatTH(stCapteur,iNumCapteur,iTemp1);
	stCapteur->fBat=(float)(uiBat*5)/1024;
	stCapteur->tTime = time(NULL);

	return true;
}


bool Capteur_bFormatPE(tCapteur *stCapteur,int iNumCapteur,int iTemp1,int iPression)
{
	stCapteur->tType=PE;
	stCapteur->uiNumCapteur=iNumCapteur;
	stCapteur->ucHumi=0;
	stCapteur->uiPression=iPression;
	stCapteur->fTemp1=(float)iTemp1/2;
	stCapteur->fTemp2=0;
	stCapteur->fBat=0;
	stCapteur->tTime = time(NULL);
	return true;
}


bool Capteur_bFormatPO(tCapteur *stCapteur,int iNumCapteur,int iTemp1,int iPression,int iHumi,int iTemp2)
{
	stCapteur->tType=PO;
	stCapteur->uiNumCapteur=iNumCapteur;
	stCapteur->ucHumi=iHumi;
	stCapteur->uiPression=iPression;
	stCapteur->fTemp1=(float)iTemp1/2;
	stCapteur->fTemp2=(float)iTemp2/10;
	stCapteur->fBat=0;
	stCapteur->tTime = time(NULL);
	return true;
}


void Capteur_vSet(tCapteur * tCp,
		tTypeCapteur tType,
		unsigned int uiNumCapteur,
		float fTemp1,
		float fTemp2,
		unsigned int uiPression,
		unsigned char ucHumi,
		float fBat,
		time_t tTime
)
{
	tCp->tType=tType=tType;
	tCp->uiNumCapteur=uiNumCapteur;
	tCp->fTemp1=fTemp1;
	tCp->fTemp2=fTemp2;
	tCp->uiPression=uiPression;
	tCp->ucHumi=ucHumi;
	tCp->fBat=fBat;
	tCp->tTime=tTime;
}

bool Capteur_bCompare(tCapteur tCp1,tCapteur tCp2)
{
	if(tCp1.uiNumCapteur!=tCp2.uiNumCapteur)
		return false;
	if(tCp1.tType!=tCp2.tType)
		return false;
	if(tCp1.fTemp1!=tCp2.fTemp1)
		return false;
	if(tCp1.fTemp2!=tCp2.fTemp2)
		return false;
	if(tCp1.ucHumi!=tCp2.ucHumi)
		return false;
	if(tCp1.uiPression!=tCp2.uiPression)
		return false;
	if(tCp1.fBat!=tCp2.fBat)
		return false;
	return true;
}
unsigned int uiNbCptTest=0;
time_t tTimeStartMoy=0;
bool Capteur_IsEnModeTest(tCapteur stCapteur)
{
	time_t tTimeNow=0;
	unsigned int uiNbSec=1,uiSamplePerMin=0;
	if(stCapteur.uiNumCapteur!=CAPTEUR_NUM_TEST)
	{
		return false;
	}
	else
	{
		//printf("Capteur %s mode test\t%g volt\t%g C\t%s",Capteur_ac2String(stCapteur.tType),stCapteur.fBat,stCapteur.fTemp1,ctime (& stCapteur.tTime));
		uiNbCptTest++;
		if(!tTimeStartMoy)
		{
			tTimeStartMoy=time(NULL);

		}
		else
		{
			tTimeNow=time(NULL);
			uiNbSec=tTimeNow-tTimeStartMoy;
			if(uiNbSec==0)
			{
				uiNbSec=1;
			}
		}
		uiSamplePerMin=(uiNbCptTest*60)/uiNbSec;
		printf("%d;sample en;%d;sec;%d;/min Capteur %s test;%g;volt;%g;C;%s",
				uiNbCptTest,
				uiNbSec,
				uiSamplePerMin,
				Capteur_ac2String(stCapteur.tType),
				stCapteur.fBat,
				stCapteur.fTemp1,
				ctime (& stCapteur.tTime)

		);

		return true;
	}
}

void Capteur_bPrint(tCapteur stCapteur)
{
	char acTime[80];
	strftime (acTime,80,"%Y-%m-%d;%H:%M:%S",localtime ( &stCapteur.tTime ));
	switch(stCapteur.tType)
	{
	case TH:
		printf("%s TH%d: T1=%f°C BAT=%fv\n",acTime,stCapteur.uiNumCapteur,stCapteur.fTemp1,stCapteur.fBat);
		break;
	case PE:
		printf("%s PE%d: T1=%f°C P=%dhPa\n",acTime,stCapteur.uiNumCapteur,stCapteur.fTemp1,stCapteur.uiPression);
		break;
	case PO:
		printf("%s PO%d: T1=%f°C T2=%f°C P=%dhPa H=%dpct\n",acTime,stCapteur.uiNumCapteur,stCapteur.fTemp1,stCapteur.fTemp2,stCapteur.uiPression,stCapteur.ucHumi);
		break;
	default:
		printf("%s capteur inconnu (%d:%d): T1=%f°C T2=%f°C P=%dhPa H=%dpct\n",acTime,stCapteur.tType,stCapteur.uiNumCapteur,stCapteur.fTemp1,stCapteur.fTemp2,stCapteur.uiPression,stCapteur.ucHumi);
		break;
	}
}
