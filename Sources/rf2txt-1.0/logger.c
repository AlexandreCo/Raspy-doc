/***************************************************************************
                          logger.c  -  description
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
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "capteur.h"
#include "superviseur.h"
#ifdef PNG
#include "png.h"
#endif
#include "logger.h"

static tCapteur _atOldCapteurs[NB_CAPTEUR];

static bool bDayLog(tCapteur stCapteur);


static bool bDayLog(tCapteur stCapteur);

static char _acPath[MAX_PATH];

#ifdef PNG
static void vGeneratePngFiles()
{
	Png_vCapteur(0);
	Png_vCapteur(1);
	Png_vCapteur(11);
	Png_vCapteur(21);


}
#endif

void *pvLogger (void *q)
{
	fifo *fFifo;
	tCapteur stCapteur;

	fFifo = (fifo *)q;

	if(Sup_bIsVerbose(LOGGER)==true)
	{
		printf("[Logger] Démarrage thread logger ...\n");
	}
#ifdef PNG
	vGeneratePngFiles();
#endif
	while(1)
	{
		fifo_vDel (fFifo, &stCapteur);
		if(Sup_bIsVerbose(LOGGER)==true)
		{
			printf("[Logger] recu capteur %d de type  %s\n",stCapteur.uiNumCapteur,Capteur_ac2String(stCapteur.tType));
		}
		bDayLog(stCapteur);
	}
}


bool Logger_bInit(char * acPath,pthread_t * logger,fifo *pFifo,bool bEnRam)
{
	bool bRet=true;
	printf("Initialisation du Logger\n");


	char acLogBrutPath[255];

	strncpy(_acPath,acPath,MAX_PATH);
	sprintf(acLogBrutPath,"%s/%s/",acPath,LOG_BRUT_DIR);
	mkdir(acLogBrutPath,0777);
#ifdef PNG
	if(Png_bInit(acPath,bEnRam)==false)
	{
		bRet=false;
	}
#endif
	strncpy(_acPath,acPath,MAX_PATH);

	if(pthread_create (logger, NULL, pvLogger, pFifo)!=0)
	{
		bRet=false;
	}

	printf("Fin initialisation du Logger\n");
	return bRet;
}




//Stock sur la flash le resultat du capteur recu par le reader
static bool bDayLog(tCapteur stCapteur)
{
	char acTime[80];
	char acTimeName[80];
	char acFile[255];


	strftime (acTime,80,"%Y-%m-%d;%H:%M:%S",localtime ( &stCapteur.tTime ));


	if(Sup_bIsWritable()==true)
	{
		unsigned char acNumCapteur=stCapteur.tType*10+stCapteur.uiNumCapteur;
		if(acNumCapteur<NB_CAPTEUR)
		{
			if(Capteur_bCompare(_atOldCapteurs[acNumCapteur],stCapteur)==false)
			{
				//nouvelle valeur : on mets a jour le fichier
				strftime (acTimeName,80,"%Y-%m-%d",localtime ( &stCapteur.tTime ));
				sprintf(acFile,"%s/%s/%s_data.txt",_acPath,LOG_BRUT_DIR,acTimeName);
				printf("write data %s\n",acFile);
				FILE *f=fopen(acFile,"a+");
				if (f==NULL)
				{
					perror("fopen");
				}
				else
				{
					fprintf(f,"%d;%s;%f;%d;%d;%f;%f\n",acNumCapteur,
							acTime,
							stCapteur.fTemp1,
							stCapteur.uiPression,
							stCapteur.ucHumi,
							stCapteur.fTemp2,
							stCapteur.fBat
					);
					fflush(f);
					fclose(f);
//#define EMONCMS3
//#ifdef EMONCMS3
#define URL	"http://127.0.0.1/emoncms3/emoncms3/"
#define API_KEY "be257fcfd02cf344e99ef6a4fcad9c7b"
					char acCmd[255];
					sprintf(acCmd,"wget \"%s/api/post?apikey=%s&node=%d&json={t1:%f,p:%d,h:%d,t2:%f}\" -qO - >/dev/null",URL,API_KEY,acNumCapteur,stCapteur.fTemp1,stCapteur.uiPression,stCapteur.ucHumi,stCapteur.fTemp2);
					
					system(acCmd);
					printf("%s\n",acCmd);
//#endif

#ifdef PNG
					Png_vCapteur(stCapteur.uiNumCapteur+stCapteur.tType*10);


#endif
				}
			}
			Capteur_vSet(&_atOldCapteurs[acNumCapteur],	stCapteur.tType,stCapteur.uiNumCapteur,
					stCapteur.fTemp1,stCapteur.fTemp2,stCapteur.uiPression,stCapteur.ucHumi
					,stCapteur.fBat,stCapteur.tTime);
		}
	}
	else
	{
		if(Sup_bIsVerbose(LOGGER)==true)
		{
			printf("Log non actif\n");
		}
	}

	if(Sup_bIsVerbose(LOGGER)==true)
	{
		Capteur_bPrint(stCapteur);
	}
	return true;
}
