/***************************************************************************
                          superviseur.h  -  description
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
#ifndef SUPERVISEUR_H_
#define SUPERVISEUR_H_

typedef enum {false, true} bool;
typedef enum {MAIN=0,READER,LOGGER,MAX_MODULE}tModule;

#define MAIN_NAME "SUPERVISEUR"
#define READER_NAME "READER"
#define LOGGER_NAME "LOGGER"
#define MAX_PATH	255
#define MAX_DEV_LEN 100
extern bool Sup_bIsBrutMode();
extern bool Sup_bIsVerbose(tModule module);
extern bool Sup_bIsWritable();
#endif /* SUPERVISEUR_H_ */
