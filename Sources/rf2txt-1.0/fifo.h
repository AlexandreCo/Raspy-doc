/***************************************************************************
                          fifo.h  -  description
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

#ifndef FIFO_H_
#define FIFO_H_
#include <pthread.h>
#include "capteur.h"
#define FIFOSIZE 10

typedef struct {
	tCapteur buf[FIFOSIZE];
	long head, tail;
	int full, empty;
	pthread_mutex_t *mut;
	pthread_cond_t *notFull, *notEmpty;
} fifo;


void fifo_vDel (fifo *q, tCapteur *out);
void fifo_vAdd (fifo *q, tCapteur in);
void fifo_vDelete (fifo *q);
fifo *fifo_fInit (void);

#endif /* FIFO_H_ */
