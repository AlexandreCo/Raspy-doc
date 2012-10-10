/*
 * png.c
 *
 *  Created on: 23 janv. 2012
 *      Author: alexandre
 */

#include <time.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <libgen.h>
#include "gnuplot_i.h"
#include "png.h"
#include "logger.h"

typedef struct
{
	tCapteur * pstCpt;
	unsigned int uiNbCpt;
}t_log;

static char _acPath[MAX_PATH];

static void vDisplay(tInfo info);
static void vDisplayMinMax(tInfo info);
void vWritePng(gnuplot_ctrl * h,tInfo info);

//pretrace la premiere courbe
gnuplot_ctrl *vDrawLine(time_t * pdTime,double * pdData,tInfo info,bool bFilled);
//affiche la courbe complete
void vDrawFull(time_t * pdTime,double * pdData,tInfo info,bool bFilled);



//Affiche les courbes Min et Max de la grandeur physique
void vDrawMinMax(time_t * pdTime,double * pdDataMin,double * pdDataMax,tInfo info)
{

	gnuplot_ctrl * h;
	char tmp[255];

	char acTitle[255];
	struct tm *tFirst;
	struct tm *tLast;
/*	int i;
	printf("%d %d\n",info.uiNbJour,info.uiNumCpt);
	for( i=0 ; i<info.uiNbData;i++)
	{
		printf("%g\t%g\n",pdDataMin[i],pdDataMax[i]);
	}*/
	tFirst = localtime(&pdTime[0]);
	tLast = localtime(&pdTime[info.uiNbData-1]);

	h = gnuplot_init() ;
	sprintf(acTitle,"%s du %02d/%02d/%02d au %02d/%02d/%02d",_acGPName[info.tGp],
			tFirst->tm_mday,tFirst->tm_mon+1,tFirst->tm_year-100,
			tLast->tm_mday,tLast->tm_mon+1,tLast->tm_year-100

	);

	gnuplot_setfilledstylefilled( h,(char*)_acGPcolor[info.tGp],"above");

	sprintf(tmp,"set title \"%s\"",acTitle);
	gnuplot_cmd(h, tmp) ;

	if(info.uiNbJour==1)
	{
		gnuplot_set_xlabel	( h," Heure (HH:MM)");
		gnuplot_set_format_x(h,"%H:%M");
	}
	else
	{
		gnuplot_set_xlabel	( h," Date (DD/MM)");
		gnuplot_set_format_x(h,"%d/%m");
	}
	sprintf(tmp, "%s en %s",_acGPName[info.tGp],_acGrandeur[info.tGp]);
	gnuplot_set_ylabel	( h,tmp);
	gnuplot_cmd(h, "set autoscale") ;
	gnuplot_cmd(h, "set ytics 1") ;
	gnuplot_cmd(h, "set grid");
	gnuplot_cmd(h, "set key off");

	gnuplot_plot_timey(h, pdTime,pdDataMax, info.uiNbData, NULL) ;
	gnuplot_setfilledstylefilled( h,"white","below") 	;
	gnuplot_plot_timey(h,pdTime,pdDataMin,info.uiNbData,NULL);
	vWritePng(h,info);

}

void vWritePng(gnuplot_ctrl * h,tInfo info)
{
	char filename[255];
	char tmp[255];
	gnuplot_cmd(h, "set term png") ;
	sprintf(filename,"%s/%s/%s/%d_%d_data.txt.png",_acPath,PNG_PATH,_acGPRep[info.tGp],info.uiNumCpt,info.uiNbJour);
	sprintf(tmp,"set output  \"%s\"",filename);
	gnuplot_cmd(h, tmp) ;
	gnuplot_cmd(h, "replot") ;
	gnuplot_close(h);
}



void vDrawFull(time_t * pdTime,double * pdData,tInfo info,bool bFilled)
{
	gnuplot_ctrl * h;
	h= vDrawLine(pdTime,pdData,info,bFilled);
	vWritePng(h,info);
}


gnuplot_ctrl *vDrawLine(time_t * pdTime,double * pdData,tInfo info,bool bFilled)
{

	gnuplot_ctrl * h ;
	char tmp[255];
	char acTitle[255];
	struct tm *tFirst;
	struct tm *tLast;

	tFirst = localtime(&pdTime[0]);
	tLast = localtime(&pdTime[info.uiNbData-1]);

	h = gnuplot_init() ;
	sprintf(acTitle,"%s du %02d/%02d/%02d au %02d/%02d/%02d",_acGPName[info.tGp],
			tFirst->tm_mday,tFirst->tm_mon+1,tFirst->tm_year-100,
			tLast->tm_mday,tLast->tm_mon+1,tLast->tm_year-100

	);
	if(bFilled)
	{
		gnuplot_setfilledstylefilled( h,(char*)_acGPcolor[info.tGp],"below") 	;
	}
	else
	{
		gnuplot_setstyle(h,"lines");
	}

	sprintf(tmp,"set title \"%s\"",acTitle);
	gnuplot_cmd(h, tmp) ;

	if(info.uiNbJour==1)
	{
		gnuplot_set_xlabel	( h," Heure (HH:MM)");
		gnuplot_set_format_x(h,"%H:%M");
	}
	else
	{
		gnuplot_set_xlabel	( h," Date (DD/MM)");
		gnuplot_set_format_x(h,"%d/%m");
	}
	sprintf(tmp, "%s en %s",_acGPName[info.tGp],_acGrandeur[info.tGp]);
	gnuplot_set_ylabel	( h,tmp);
	gnuplot_cmd(h, "set autoscale") ;
	gnuplot_cmd(h, "set ytics 1") ;
	gnuplot_cmd(h, "set grid");
	gnuplot_cmd(h, "set key off");

	gnuplot_plot_timey(h, pdTime,pdData, info.uiNbData, "Temp") ;
	return h;

}

// Renverse le tableau de capteur
void vRevert(tCapteur * ptCpt,unsigned int uiNb)
{
	unsigned int i;
	tCapteur * pstCptTemp = malloc(sizeof(tCapteur) * (uiNb));

	for(i=0;i<(uiNb);i++)
	{
		pstCptTemp[i]=ptCpt[i];
	}

	for(i=0;i<(uiNb);i++)
	{
		ptCpt[i]=pstCptTemp[(uiNb-1)-i];
	}
	free(pstCptTemp);
}


unsigned int uiLoadFile(unsigned int uiday,tCapteur * ptCpt)
{
	time_t result;
	result = time(NULL)-(24*60*60*uiday);

	char acTimeName[80];
	char acFile[255];
	tCapteur capteur;
	unsigned int year,month,day,hour,min,sec;
	unsigned int uiNbSample=0;
	unsigned int uiHumi;
	int iRet=0;
	strftime (acTimeName,80,"%Y-%m-%d",localtime ( &result ));
	sprintf(acFile,"%s/%s/%s_data.txt",_acPath,LOG_BRUT_DIR,acTimeName);

	FILE *f=fopen(acFile,"r");
	struct tm stTm;
	if(f!=NULL)
	{
		//printf("Read data %s\n",acFile);
		while(!feof(f))
		{

			iRet = fscanf(f, "%d;%d-%d-%d;%d:%d:%d;%f;%d;%d;%f;%f\n",
					&capteur.uiNumCapteur,
					&year,&month,&day,
					&hour,&min,&sec,
					&capteur.fTemp1,
					&capteur.uiPression,
					&uiHumi,
					&capteur.fTemp2,
					&capteur.fBat);
			capteur.ucHumi=(unsigned char)uiHumi;
			stTm.tm_sec=sec;
			stTm.tm_min=min;
			stTm.tm_hour=hour;
			stTm.tm_mday=day;
			stTm.tm_mon=month-1;
			stTm.tm_year=year-1900;
			//correction
			switch(capteur.uiNumCapteur)
			{
			case 0:
				capteur.fTemp1-=0.56;
				break;
			case 1:
				capteur.fTemp1-=0;
				break;
			case 11:
				capteur.fTemp1-=1.5;
				break;
			case 21:
				capteur.fTemp1-=0.5;
				capteur.fTemp2-=2.0;
				capteur.uiPression+=3;
				break;
			}
			capteur.tTime=mktime(&stTm);
			ptCpt[uiNbSample++]=capteur;

		}
		vRevert(ptCpt,uiNbSample);
		//printf("%d sample read\n",uiNbSample);
		fclose(f);
	}
	else
	{
		//printf("%s no data found\n",acFile);
	}

	return uiNbSample;
}

#define MAX_CPT_JOUR 1200
tCapteur *pstLoadFiles(unsigned int * puiNbCapteur,unsigned int uiNbJour)
{
	tCapteur * astCpt;
	tCapteur * pstTemp;

	unsigned int uiNbSampleDay,uiNbSampleTotal=0,i;
	unsigned int uiFileRead=0;
	unsigned int uiTailleTab=0;


	uiTailleTab=MAX_CPT_JOUR;
	astCpt = malloc(sizeof(tCapteur) * uiTailleTab);
	for(i=0;i<uiNbJour;i++)
	{
		uiNbSampleDay=uiLoadFile(i,&astCpt[uiNbSampleTotal]);
		if(uiNbSampleDay)
		{
			//sample present dans le fichier
			uiNbSampleTotal+=uiNbSampleDay;
			uiFileRead++;
			if((uiNbSampleTotal+MAX_CPT_JOUR)>uiTailleTab)
			{
				uiTailleTab+=MAX_CPT_JOUR;
				//printf("reallocation %d",uiTailleTab);
				pstTemp = realloc(astCpt, sizeof(tCapteur) * (uiTailleTab + 1));
				if(pstTemp!=NULL)
				{
					astCpt=pstTemp;
				}
			}
		}
		//printf("Sample read %d\tNb Sample %d ",uiNbSampleDay,uiNbSampleTotal);
		//printf("\n");
	}
	//	printf("%d sample read in %d file(s) (moy=%d)\n",uiNbSampleTotal,uiFileRead,uiNbSampleTotal/uiFileRead);


	*puiNbCapteur=uiNbSampleTotal;
	return astCpt;
}

bool Png_bInit(char * acPath,bool bMount)
{
	bool bRet=true;
	printf("initialisation du module png\n");
	int i;
	char buf[255];
	char acRamPath[255];
	char acPngPath[255];

	strncpy(_acPath,acPath,MAX_PATH);

	sprintf(acRamPath,"%s/%s/",RAM_PATH,PNG_PATH);
	printf("ram path %s\n",acRamPath);

	if(mkdir(acRamPath,0777)==0)
	{
		sprintf(acPngPath,"%s/%s",acPath,PNG_PATH);
		if(bMount==true)
		{
			if(mount(acRamPath, acPngPath, NULL, MS_BIND, NULL)!=0)
			{
				printf("Probleme de mountage de %s dans la ram %s\n",acPngPath,acRamPath);
				bRet=false;
			}
			else
			{
				printf("mountage %s dans la ram %s ok\n",acPngPath,acRamPath);
			}
		}
		else
		{
			printf("creation du repertoire %s\n",acPngPath);
			mkdir(acPngPath,0777);
		}
		for(i=0;i<MAX_GP;i++)
		{
			sprintf(buf,"%s/%s/",acPngPath,_acGPRep[i]);
			mkdir(buf,0777);
			printf("creation du repertoire %s\n",buf);
		}
	}
	else
	{
		perror("mkdir");
		printf("Repertoires deja presents\n");
	}

	printf("fin initialisation du module png\n");
	return bRet;
}


void Png_vCapteur(unsigned int uiCapteur)
{
	tInfo info;
	switch(uiCapteur)
	{
	case 0 :
		info.tGp=TEMP1;
		info.uiNumCpt=uiCapteur;
		info.uiNbJour=1;
		vDisplay(info);
		info.uiNbJour=7;
		vDisplay(info);
		info.uiNbJour=30;
		vDisplayMinMax(info);
		break;
	case 1 :

		info.tGp=TEMP1;
		info.uiNumCpt=uiCapteur;
		info.uiNbJour=1;
		vDisplay(info);
		info.uiNbJour=7;
		vDisplay(info);
		info.uiNbJour=30;
		vDisplayMinMax(info);

		info.tGp=VOLT;
		info.uiNbJour=1;
		vDisplay(info);
		info.uiNbJour=7;
		vDisplay(info);
		info.uiNbJour=30;
		vDisplayMinMax(info);

		break;
	case 11:
		info.tGp=TEMP1;
		info.uiNumCpt=uiCapteur;
		info.uiNbJour=1;
		vDisplay(info);
		info.uiNbJour=7;
		vDisplay(info);
		info.uiNbJour=30;
		vDisplayMinMax(info);

		break;
	case 21:
		info.tGp=TEMP2;
		info.uiNumCpt=uiCapteur;

		info.uiNbJour=1;
		vDisplay(info);
		info.uiNbJour=7;
		vDisplay(info);
		info.uiNbJour=30;
		vDisplayMinMax(info);

		//--------------------------
		info.tGp=HUMI;
		info.uiNbJour=1;
		vDisplay(info);
		info.uiNbJour=7;
		vDisplay(info);
		info.uiNbJour=30;
		vDisplayMinMax(info);

		//--------------------------
		info.tGp=PRESS;
		info.uiNbJour=1;
		vDisplay(info);
		info.uiNbJour=7;
		vDisplay(info);
		info.uiNbJour=30;
		vDisplayMinMax(info);

		break;
	}
}

static unsigned int  uiGetMinMax(tInfo info, time_t * pdTime,double * pdDataMin,double * pdDataMax)
{
	t_log stListCpt;
	double dMin=65535;
	double dMax=-65535;
	double dCurrent=0;
	unsigned int uiNbData=0;
	int i;
	stListCpt.pstCpt=pstLoadFiles(&stListCpt.uiNbCpt,info.uiNbJour);
	time_t tTmOld=stListCpt.pstCpt[stListCpt.uiNbCpt-1].tTime;
	if(stListCpt.uiNbCpt>0)
	{
		for(i=(stListCpt.uiNbCpt-1);i>0;i--)
		{
			if(stListCpt.pstCpt[i].uiNumCapteur==info.uiNumCpt)
			{
				if(tTmOld+(24*60*60)<stListCpt.pstCpt[i].tTime)
				{
					pdDataMin[uiNbData]=dMin;
					pdDataMax[uiNbData]=dMax;
					pdTime[uiNbData]=tTmOld;
					tTmOld=stListCpt.pstCpt[i].tTime;
					if(dMin!=65535)
					{
						uiNbData++;
						dMin=65535;
						dMax=-65535;
					}
				}

				switch(info.tGp)
				{
				case TEMP1:
					dCurrent=(double)stListCpt.pstCpt[i].fTemp1;
					break;
				case TEMP2:
					dCurrent=(double)stListCpt.pstCpt[i].fTemp2;
					break;
				case HUMI:
					dCurrent=(double)stListCpt.pstCpt[i].ucHumi;
					break;
				case PRESS:
					dCurrent=(double)stListCpt.pstCpt[i].uiPression;
					break;
				case VOLT:
					dCurrent=(double)stListCpt.pstCpt[i].fBat;
					break;
				default :
					break;
				}
				if(dCurrent<dMin)
				{
					dMin=dCurrent;
				}
				if(dCurrent>dMax)
				{
					dMax=dCurrent;
				}
				//printf("%ld\t%ld\t%d\t%f\t%f\t%f\n",tTmOld,stListCpt.pstCpt[i].tTime,uiNbData,dCurrent,dMin,dMax);

			}
		}
	}
	free(stListCpt.pstCpt);
	return uiNbData;
}

static void vDisplayMinMax(tInfo info)
{
	time_t * pdTime;
	double * pdDataMin;
	double * pdDataMax;

	t_log stListCpt;
	stListCpt.pstCpt=pstLoadFiles(&stListCpt.uiNbCpt,info.uiNbJour);
	pdTime= malloc(sizeof(time_t) * info.uiNbJour);
	pdDataMin= malloc(sizeof(double) * info.uiNbJour); //todo alloc nb jour
	pdDataMax= malloc(sizeof(double) * info.uiNbJour); //todo alloc nb jour

	info.uiNbData=uiGetMinMax(info, pdTime,pdDataMin,pdDataMax);

	vDrawMinMax(pdTime,pdDataMin,pdDataMax,info);

	free(pdTime);
	free(pdDataMin);
	free(pdDataMax);

}


static void vDisplay(tInfo info)
{
	unsigned int i;
	unsigned int uiNbData=0;

	time_t * pdTime;
	double * pdData;

	t_log stListCpt;
	stListCpt.pstCpt=pstLoadFiles(&stListCpt.uiNbCpt,info.uiNbJour);
	pdTime= malloc(sizeof(time_t) * stListCpt.uiNbCpt);
	pdData= malloc(sizeof(double) * stListCpt.uiNbCpt);

	if(stListCpt.uiNbCpt>0)
	{
		for(i=(stListCpt.uiNbCpt-1);i>0;i--)
		{
			if(stListCpt.pstCpt[i].uiNumCapteur==info.uiNumCpt)
			{
				pdTime[uiNbData]=stListCpt.pstCpt[i].tTime;
				switch(info.tGp)
				{
				case TEMP1:
					pdData[uiNbData]=(double)stListCpt.pstCpt[i].fTemp1;
					break;
				case TEMP2:
					pdData[uiNbData]=(double)stListCpt.pstCpt[i].fTemp2;
					break;
				case HUMI:
					pdData[uiNbData]=(double)stListCpt.pstCpt[i].ucHumi;
					break;
				case PRESS:
					pdData[uiNbData]=(double)stListCpt.pstCpt[i].uiPression;
					break;
				case VOLT:
					pdData[uiNbData]=(double)stListCpt.pstCpt[i].fBat;
					break;
				default :
					break;
				}
				uiNbData++;
			}
		}
		info.uiNbData=uiNbData;
		vDrawFull(pdTime,pdData,info,true);

	}

	free(pdTime);
	free(pdData);
	free(stListCpt.pstCpt);
}
