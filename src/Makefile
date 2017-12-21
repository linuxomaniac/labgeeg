CFLAGS = -Wall -pedantic -std=c11 -g
LDFLAGS =

all: labgen

labgen.yy.c: labgen.l
	flex -o $@ $<

labgen.c: labgen.y
	bison -o $@ $<

labgen.o: labgen.c labgen.yy.c
	gcc $(CFLAGS) -c -o $@ labgen.c

%.o: %.c %.h
	gcc $(CFLAGS) -c -o $@ $<

labgen: list.o labgen_func.o labgen.o
	gcc $(LDFLAGS) -o $@ $^

clean:
	rm -rf *.o labgen.c labgen.yy.c labgen 2>/dev/null
