all: labgen

%: %.y %.l
	bison -o $@.c $<
	flex -o $@.yy.c $(word 2, $^)
	gcc -ansi -o $@ $@.c

clean:
	rm -rf labgen.c labgen.yy.c labgen 2>/dev/null
