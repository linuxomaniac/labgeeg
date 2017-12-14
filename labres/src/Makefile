all: labres_ex1 labres_ex2

labres.yy.c: labres.l
	flex -o $@ $<
	
%: %.y labres.yy.c labres_main.c
	bison -o $@.c $<
	gcc -ansi -o $@ $@.c

clean:
	rm -rf labres_ex1.c labres_ex1.c labres.yy.c labres_ex1 labres_ex2 2>/dev/null
