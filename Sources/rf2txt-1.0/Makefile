#
# makefile
#
			
#
# add more flags here,
# but don't remove the "+="
#
CFLAGS += -Wall -O2 -g

CPPFLAGS +=
LDFLAGS += -lpthread


all: rf2txt

#si gestion de la generation en interne :rf2txt:  fifo.c reader.c capteur.c logger.c gnuplot_i.c png.c
#sinon :
rf2txt:  fifo.c reader.c capteur.c logger.c

clean:
	rm -f *.o rf2txt

