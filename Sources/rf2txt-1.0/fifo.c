/***************************************************************************
                          fifo.c  -  description
                             -------------------
    begin                : 17 janv. 2012
    copyright            : (C) 2012 by alexandre COFFIGNAL
    email                :
	Base on Andrae Muys Demo Producer/Consumer
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
#include <pthread.h>
#include "fifo.h"

fifo *fifo_fInit (void)
{
	fifo *q;

	q = (fifo *)malloc (sizeof (fifo));
	if (q == NULL) return (NULL);

	q->empty = 1;
	q->full = 0;
	q->head = 0;
	q->tail = 0;
	q->mut = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
	pthread_mutex_init (q->mut, NULL);
	q->notFull = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
	pthread_cond_init (q->notFull, NULL);
	q->notEmpty = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
	pthread_cond_init (q->notEmpty, NULL);

	return (q);
}

void fifo_vDelete (fifo *q)
{
	pthread_mutex_destroy (q->mut);
	free (q->mut);
	pthread_cond_destroy (q->notFull);
	free (q->notFull);
	pthread_cond_destroy (q->notEmpty);
	free (q->notEmpty);
	free (q);
}

void fifo_vAdd (fifo *q, tCapteur in)
{
	pthread_mutex_lock (q->mut);
	while (q->full) {

		if(Sup_bIsVerbose(READER)==true)
		{
			printf ("fifo FULL.\n");
		}
		pthread_cond_wait (q->notFull, q->mut);
	}

	q->buf[q->tail] = in;
	q->tail++;
	if (q->tail == FIFOSIZE)
		q->tail = 0;
	if (q->tail == q->head)
		q->full = 1;
	q->empty = 0;

	pthread_mutex_unlock (q->mut);
	pthread_cond_signal (q->notEmpty);
	return;
}

void fifo_vDel (fifo *q, tCapteur *out)
{
	pthread_mutex_lock (q->mut);
	while (q->empty) {
		if(Sup_bIsVerbose(LOGGER)==true)
		{
			printf ("fifo EMPTY.\n");
		}
		pthread_cond_wait (q->notEmpty, q->mut);
	}


	*out = q->buf[q->head];

	q->head++;
	if (q->head == FIFOSIZE)
		q->head = 0;
	if (q->head == q->tail)
		q->empty = 1;
	q->full = 0;
	pthread_mutex_unlock (q->mut);
	pthread_cond_signal (q->notFull);

	return;
}
