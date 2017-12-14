all: labres_ex1

%: %.y %.l
	bison -o $@.c $<
	flex -o $@.yy.c $(word 2, $^)
	gcc -ansi -o $@ $@.c

clean:
	rm -rf labres_ex1.c labres_ex1.yy.c labres_ex1 2>/dev/null
