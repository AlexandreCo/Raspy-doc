/***************************************************************************
                          rf2txt.c  -  description
                             -------------------
    begin                : jun 06 2011
    copyright            : (C) 2011 by alexandre COFFIGNAL
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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>


#include <sys/types.h>

#include "superviseur.h"
#include "fifo.h"
#include "reader.h"
#include "logger.h"
#define MAX_BUF	30
#define MIN_DETECT	7
#define VRAI 1
#define FAUX 0

static bool _bVerbose[MAX_MODULE];
static bool _bVerboseBrut=false;
static bool _bWritable=true;
static char _acDev[MAX_DEV_LEN];

char *acModule2String(tModule tMod)
{
	switch(tMod)
	{
	case 	MAIN : return MAIN_NAME; break;
	case 	READER : return READER_NAME; break;
	case 	LOGGER : return LOGGER_NAME; break;
	default : return "inconnu"; break;
	}

}

char *acBool2String(bool bState)
{
	switch(bState)
	{
	case 	true : return "true"; break;
	case 	false : return "false"; break;
	default : return "inconnu"; break;
	}

}

bool Sup_bIsWritable()
{
	return _bWritable;
}

void vSetWritable(bool bWritable)
{
	_bWritable=bWritable;
}

bool Sup_bIsBrutMode()
{
	return _bVerboseBrut;
}

void vSetVerboseBrutMode(bool bVerbose)
{
	_bVerboseBrut=bVerbose;
}

bool Sup_bIsVerbose(tModule module)
{
	return _bVerbose[module];
}

void vInitVerboseMode()
{
	int i=0;
	for( i=0 ; i<MAX_MODULE;i++)
	{
		_bVerbose[i]=false;
	}
}

void vSetVerboseMode(bool bVerbose,tModule module)
{
	printf("verbose %s thread %s\n",acBool2String(bVerbose),acModule2String(module));
	_bVerbose[module]=bVerbose;
}




void vSetDev(char * acDev)
{

	strncpy(_acDev,acDev,MAX_DEV_LEN);
	printf("dev : %s\n",_acDev);
}

static void _vUsage(char * acName)
{
	printf("usage : %s [-v] [-l!] [-h]\n",acName);
	printf("-h, --help : aide\n");
	printf("-v num, --verbose : verbose Superviseur=1,Reader=2,Logger=4\n");
	printf("-l!, --log_off : pas de log\n");
	printf("-b, --brut : affiche les données brutes\n");
	printf("-s num, --speed : vitesse RS232\n");
	printf("-d dev_name, --dev : dev /dev/ttyUSB0\n");
	printf("-r!, --ram_off : desactivation du montage en ram\n");
	printf("-p path_name, --path : path /var/www/GnOmeDataCenter\n");
}

int main (int argc, char *argv[])
{

	int i;
	unsigned int uiSpeed=RS_DEFAULT_SPEED;
	char acPath[MAX_PATH];
	bool bEnRam=true;	//Montage du repertoire des png en ram
	strncpy(acPath,PATH,MAX_PATH);
	vSetDev("/dev/ttyUSB0");
	for(i=0;i<argc;i++)
	{
		//mode verbeux ou pas
		if((!strcmp(argv[i],"-v"))||(!strcmp(argv[i],"--verbose")))
		{
			char cMode=atoi(argv[++i]);
			if(cMode&0x01)
			{
				vSetVerboseMode(true,MAIN);
			}
			if(cMode&0x02)
			{
				vSetVerboseMode(true,READER);
			}
			if(cMode&0x04)
			{
				vSetVerboseMode(true,LOGGER);
			}

		}

		if((!strcmp(argv[i],"-l!"))||(!strcmp(argv[i],"--log_off")))
		{
			printf("mode log non actif\n");
			vSetWritable(false);
		}

		if((!strcmp(argv[i],"-r!"))||(!strcmp(argv[i],"--ram_off")))
		{
			printf("montage en ram non actif\n");
			bEnRam=false;
		}


		if((!strcmp(argv[i],"-b"))||(!strcmp(argv[i],"--brut")))
		{
			printf("mode log brut actif\n");
			vSetVerboseBrutMode(true);
		}

		if((!strcmp(argv[i],"-d"))||(!strcmp(argv[i],"--dev")))
		{
			vSetDev(argv[++i]);
		}

		if((!strcmp(argv[i],"-s"))||(!strcmp(argv[i],"--speed")))
		{
			uiSpeed=atoi(argv[++i]);
			printf("Rs232 speed : %d\n",uiSpeed);
		}

		if((!strcmp(argv[i],"-h"))||(!strcmp(argv[i],"--help")))
		{
			_vUsage(argv[0]);
			return 0;
		}

		if((!strcmp(argv[i],"-p"))||(!strcmp(argv[i],"--path")))
		{
			strncpy(acPath,argv[++i],MAX_PATH);
			printf("path : %s\n",acPath);
		}

	}

	fifo *pFifo;
	pthread_t reader, logger;

	pFifo = fifo_fInit ();
	if (pFifo ==  NULL) {
		fprintf (stderr, "main: Fifo Init failed.\n");
		return -1;
	}
	if(Reader_bInit(&reader,pFifo,uiSpeed,_acDev)==false)
	{
		printf("probleme d'initiatlisation du module reader\n");
		fifo_vDelete (pFifo);
		return -1;
	}
	if(Logger_bInit(acPath,&logger,pFifo,bEnRam)==false)
	{
		printf("probleme d'initiatlisation du module reader\n");
		fifo_vDelete (pFifo);
		return -1;
	}
	pthread_join (reader, NULL);
	pthread_join (logger, NULL);
	fifo_vDelete (pFifo);
	return 0;
}
