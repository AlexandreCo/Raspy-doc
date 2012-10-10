/***************************************************************************
                          reader.c  -  description
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
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include "reader.h"
#include "fifo.h"
#include "capteur.h"
#include "superviseur.h"

#define MIN_DETECT	7

static char _acDev[MAX_DEV_LEN];
static unsigned int _uiSpeed=0;

void F_COM_Conf(int fd)
{
	static struct termios	TermioParms;

	bzero(&TermioParms, sizeof(TermioParms));

	TermioParms.c_iflag = IGNBRK;

	TermioParms.c_cflag = _uiSpeed | CS8 |CREAD;

	TermioParms.c_oflag = 0;
	TermioParms.c_lflag = ICANON;

	//	time out d'attente et la taille Min de la chaine de reception attendue

	TermioParms.c_cc[VINTR]    = 0;     /* Ctrl-c */
	TermioParms.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
	TermioParms.c_cc[VERASE]   = 0;     /* del */
	TermioParms.c_cc[VKILL]    = 0;     /* @ */
	TermioParms.c_cc[VEOF]     = 4;     /* Ctrl-d */
	//	time out d'attente et la taille Min de la chaine de reception attendue
	TermioParms.c_cc[VTIME]    = 0;     /* compteur inter-caractère non utilisé */
	TermioParms.c_cc[VMIN]     = 1;     /* read bloquant jusqu'à l'arrivée d'1 caractère */
	TermioParms.c_cc[VSWTC]    = 0;     /* '\0' */
	TermioParms.c_cc[VSTART]   = 0;     /* Ctrl-q */
	TermioParms.c_cc[VSTOP]    = 0;     /* Ctrl-s */
	TermioParms.c_cc[VSUSP]    = 0;     /* Ctrl-z */
	TermioParms.c_cc[VEOL]     = 0;     /* '\0' */
	TermioParms.c_cc[VREPRINT] = 0;     /* Ctrl-r */
	TermioParms.c_cc[VDISCARD] = 0;     /* Ctrl-u */
	TermioParms.c_cc[VWERASE]  = 0;     /* Ctrl-w */
	TermioParms.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
	TermioParms.c_cc[VEOL2]    = 0;     /* '\0' */

	// flush tty
	tcflush(fd, TCIFLUSH);
	//Configure tty
	tcsetattr(fd,TCSANOW,&TermioParms);

}

int bCrc(char * p,unsigned char ucCrc)
{
	int i=0;
	unsigned char ucCrcCal=0;
	for(i=0;i<strlen(p);i++)
	{
		ucCrcCal+=p[i];
	}
	if(Sup_bIsVerbose(READER)==true)
	{
		printf("CRC %d (%d)\n",ucCrcCal,ucCrc);
	}
	if(ucCrcCal==ucCrc)
		return true;
	else
		return false;
}

bool bAnalyse(char *acBuf,int iNbCar,tCapteur * stCapteur)
{
	char * p=&acBuf[0];
	char acBufTemp[255];
	int cNbArg=0;
	int iNumCapteur=0;
	int iTemp1=0;
	int iTemp2=0;
	int iPression=0;
	int iHumi=0;
	unsigned int uiBat=0;

	int iCrc=0;
	bool bRet=false;
	if(iNbCar>MIN_DETECT)
	{
		//sinon pas assez de caractére pour une dectection (optimisation)
		if(acBuf[0]==0x0D)
		{
			//typique d'une nouvelle trame ok  (optimisation)
			p=&acBuf[1];
			//ThermRF v2
			cNbArg=sscanf(p,"TH %d %d %d %d\n",&iNumCapteur,&iTemp1,&uiBat,&iCrc);
			if(cNbArg==4)
			{
				sprintf(acBufTemp,"%d %d %d",iNumCapteur,iTemp1,uiBat);
				if(bCrc(acBufTemp,iCrc)==true)
				{
					Capteur_bFormatTHV2(stCapteur,iNumCapteur,iTemp1,uiBat);
					bRet=true;
				}
			}
			else
			{
				//ThermRF v1
				cNbArg=sscanf(p,"TH %d %d %d\n",&iNumCapteur,&iTemp1,&iCrc);
				if(cNbArg==3)
				{
					sprintf(acBufTemp,"%d %d",iNumCapteur,iTemp1);
					if(bCrc(acBufTemp,iCrc)==true)
					{
						Capteur_bFormatTH(stCapteur,iNumCapteur,iTemp1);
						bRet=true;
					}
				}
				else
				{
					cNbArg=sscanf(p,"PE %d %d %d %d\n",&iNumCapteur,&iPression,&iTemp1,&iCrc);
					if(cNbArg==4)
					{
						sprintf(acBufTemp,"%d %d %d",iNumCapteur,iPression,iTemp1);
						if(bCrc(acBufTemp,iCrc)==true)
						{
							Capteur_bFormatPE(stCapteur,iNumCapteur,iTemp1,iPression);
							bRet=true;
						}
					}
					else
					{
						cNbArg=sscanf(p,"PO %d %d %d %d %d %d\n",&iNumCapteur,&iTemp1,&iTemp2,&iHumi,&iPression,&iCrc);
						if(cNbArg==6)
						{
							sprintf(acBufTemp,"%d %d %d %d %d",iNumCapteur,iTemp1,iTemp2,iHumi,iPression);
							if(bCrc(acBufTemp,iCrc)==true)
							{
								Capteur_bFormatPO(stCapteur,iNumCapteur,iTemp1,iPression,iHumi,iTemp2);
								bRet=true;
							}
						}
						else
						{

						}
					}
				}
			}
		}
	}
	return bRet;
}

void *pvReader (void *q)
{
	fifo *fFifo;
	fFifo = (fifo *)q;

	tCapteur stCapteur;
	int fd,ret;
	char  acRecu[MAX_BUF];

	if(Sup_bIsVerbose(READER)==true)
	{
		printf("Ouverture %s\n",_acDev);
	}

	fd=open(_acDev,O_RDONLY);
	if(fd<=0)
	{
		perror("open");
		if(Sup_bIsVerbose(READER)==true)
		{
			printf("Erreur lecture sur %s ...\n",_acDev);
		}
		return 0;
	}
	F_COM_Conf(fd);
	while(1)
	{

		if(Sup_bIsVerbose(READER)==true)
		{
			printf("Attente lecture sur %s ...\n",_acDev);
		}
		ret=read(fd,acRecu,MAX_BUF);
		if(ret>0)
		{

			if(bAnalyse(acRecu,ret,&stCapteur)==true)
			{
				//send to logger
				if(Sup_bIsVerbose(READER)==true)
				{
					printf("Envoi capteur %d de type %s au logger\n",stCapteur.uiNumCapteur,Capteur_ac2String(stCapteur.tType));
				}

				if(Capteur_IsEnModeTest(stCapteur)==false)
				{
					fifo_vAdd (fFifo, stCapteur);
				}
			}
			if(Sup_bIsBrutMode()==true)
			{
				printf("%s",acRecu);
			}
			bzero(acRecu, sizeof(unsigned char)*MAX_BUF);
		}
		else
		{
			perror("read");
			if(Sup_bIsVerbose(READER)==true)
			{
				printf("Erreur read sur %s\n",_acDev);
			}
		}
	}
	close(fd);
}

void vSetSpeed(unsigned int uiSpeed)
{
	switch(uiSpeed)
	{
	case 4800 : _uiSpeed=B4800; break;
	case 57600 :
	default : _uiSpeed=B57600; break;
	}
}

bool Reader_bInit(pthread_t * reader,fifo *pFifo,unsigned int uiSpeed,char *acDev)
{
	vSetSpeed(uiSpeed);
	strncpy(_acDev,acDev,MAX_DEV_LEN);
	if(pthread_create (reader, NULL, pvReader, pFifo)==0)
	{
		return true;
	}
	return false;


}
