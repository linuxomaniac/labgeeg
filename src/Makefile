CFLAGS = -g
LDFLAGS =

all: labgen

labgen.yy.c: labgen.l
	flex -o $@ $<

labgen.c: labgen.y
	bison -o $@ $<

labgen.o: labgen.c labgen.yy.c
	gcc $(CFLAGS) -c -o $@ labgen.c

# %.o: %.c %.h
# 	gcc $(CFLAGS) -c -o $@ $<

iautils/iautils.o:
	make -C iautils iautils.o

labgen: iautils/iautils.o labgen.o
	gcc $(LDFLAGS) -o $@ $^

clean:
	make -C iautils clean
	rm -rf *.o labgen.c labgen.yy.c labgen 2>/dev/null
