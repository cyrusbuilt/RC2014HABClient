CC := zcc
CFLAGS := +cpm -lm -create-app
SOURCES := src/rc2014hb.c lib/sc129.c

all: clean rc2014hb.com

clean:
	rm -rf -v *.COM *.com *.o *.map *.err *.lis

rc2014hb.com: $(SOURCES) Makefile
	$(CC) $(CFLAGS) -o $@ $(SOURCES) -DAMALLOC -v