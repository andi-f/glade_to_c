# Makefile
VERSION = 3.02
CC      = /usr/bin/gcc
OBJ	 = glade_to_c.o 
CFLAGS  = -Wall -ggdb
LDFLAGS = -lm -lexpat `pkg-config --cflags --libs gtk+-3.0`
PRG 	= glade_to_c

all: $(OBJ)
	$(CC) $(CFLAGS) -o $(PRG) $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< $(LDFLAGS)

clean:
	rm -f $(PRG) *~ *.o a.out core *protot*

install:
	sudo cp $(PRG) /usr/local/bin
