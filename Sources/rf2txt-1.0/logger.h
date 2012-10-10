/***************************************************************************
                          logger.h  -  description
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

#ifndef LOGGER_H_
#define LOGGER_H_
#include "fifo.h"
#define PATH "/var/www/GnOmeDataCenter"
#define LOG_BRUT_DIR "log_brut"
//#define PATH "/home/alexandre/Bureau/Datacenter/DataCenter/LinuxServer/Demons/rf2txt/log"

extern bool Logger_bInit(char * acPath,pthread_t * logger,fifo *pFifo,bool bEnRam);
#endif /* LOGGER_H_ */
