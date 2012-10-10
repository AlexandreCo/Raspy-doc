/***************************************************************************
                          reader.h  -  description
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


#ifndef READER_H_
#define READER_H_
#include <termios.h>
#include "fifo.h"
#define MAX_BUF	30
#define RS_DEFAULT_SPEED	57200
extern bool Reader_bInit(pthread_t * reader,fifo *pFifo,unsigned int uiSpeed,char *acDev);

#endif /* READER_H_ */
