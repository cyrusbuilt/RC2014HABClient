CC := zcc
CFLAGS := +cpm -m -SO3 --list -create-app -v
#CFLAGS := +rc2014 -subtype=cpm -m -SO3 --max-allocs-per-node200000 --c-code-in-asm --list -create-app
SOURCES := src/rc2014hb.c lib/sc129.c

all: clean rc2014hb.com

clean:
	rm -rf -v *.COM *.com *.o *.map *.err *.lis
	find . -name \*.lis -type f -delete

rc2014hb.com: $(SOURCES) Makefile
	$(CC) $(CFLAGS) -o $@ $(SOURCES) -DAMALLOC -v