%{
#include <stdio.h>
#include <stdlib.h>
%}

%token SE SW NE NW
%token N S W E

%%

cell_00
  : E cell_01
  | S cell_10;

cell_01
  : W cell_00
  | SW cell_10
  | SE cell_12;

cell_10
  : N cell_00
  | NE cell_01
  | SE cell_21;

cell_12
  : S Exit
  | NW cell_01
  | SW cell_21;

cell_21
  : E Exit
  | NW cell_10
  | NE cell_12;

Exit:;

%%

#include "labres.yy.c"
#include "labres_main.c"
