PROCESSOR:=$(shell uname -m)
ifeq ($(PROCESSOR), armv6l)
CFLAGS=-g -c -Wall #-O2
else ifeq ($(PROCESSOR), armv7l)
CFLAGS=-g -c -mfpu=neon -Wall -D__NEON__ #-O2
else
CFLAGS=-c -Wall #-O2
endif

all: libspi_lcd.a

libspi_lcd.a: spi_lcd.o fonts.o
	ar -rc libspi_lcd.a spi_lcd.o fonts.o ;\
	sudo cp libspi_lcd.a /usr/local/lib ;\
	sudo cp spi_lcd.h /usr/local/include

fonts.o: fonts.c
	$(CC) $(CFLAGS) fonts.c

spi_lcd.o: spi_lcd.c
	$(CC) $(CFLAGS) spi_lcd.c

clean:
	rm *.o libspi_lcd.a

