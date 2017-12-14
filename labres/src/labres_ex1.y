%{
#include <stdio.h>
#include <stdlib.h>
%}

%token SE SW NE NW
%token N S W E
%token EXIT

%%

cell_00
  : E cell_01
  | S cell_10;

cell_01
  : W cell_00
  | E cell_02
  | SW cell_20
  | SE cell_12;

cell_02
  : W cell_01
  | S cell_12;

cell_10
  : N cell_00
  | S cell_20
  | NE cell_01
  | SE cell_21;

cell_12
  : N cell_02
  | S EXIT
  | NW cell_01
  | SW cell_21;

cell_20
  : N cell_10
  | E cell_21;

cell_21
  : W cell_20
  | E EXIT
  | NW cell_10
  | NE cell_12;

%%

#include "labres_ex1.yy.c"

int yyerror(const char *mess) {}

int main(int argc, char *argv[]) {
  FILE *f = NULL;
  int r;

  if(argc > 1) {
    if(strcmp(argv[1], "-") != 0) {
      f = fopen(argv[1], "r");
      if(f == NULL) {
        fprintf(stderr, "%s: %s\n", argv[1], strerror(errno));
        exit(1);
      }
      yyin = f;
    }
  } else if(argc > 2) {
    fprintf(stderr, "Usage: %s [<if>]\n", argv[0]);
    exit(1);
  }

  r = yyparse();

  if(f) {
    fclose(f);
  }

  if(!r) {
    fputs("gagné\n", stdout);
  } else {
    fputs("perdu\n", stderr);
  }

	return r;
}
