/*
 * png.h
 *
 *  Created on: 23 janv. 2012
 *      Author: alexandre
 */

#ifndef PNG_H_
#define PNG_H_
#include "capteur.h"
#define PNG_PATH "out"
#define RAM_PATH "/root"
typedef struct
{
	tGrandeurPhys tGp;
	unsigned int uiNumCpt;
	unsigned int uiNbJour;
	unsigned int uiNbData;
}tInfo;


void Png_vCapteur(unsigned int uiCapteur);
bool Png_bInit(char * acPath,bool bMount);
#endif /* PNG_H_ */
