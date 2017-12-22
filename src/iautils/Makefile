CFLAGS = -g
LDFLAGS =

%.o: %.c %.h
	gcc $(CFLAGS) -c -o $@ $<

iautils.o: lds.o pdt.o points.o util.o vars.o wr.o main.o
	ld -r -o $@ $^

clean:
	rm -rf *.o 2>/dev/null