%{
#include <stdio.h>
#include <stdlib.h>
%}

%left '+' '-'
%left '/' '*'
%left '%'
%right  '(' ')'


%token IDENT CNUM DIR
%token tk_INST tk_SIZE tk_IN tk_OUT tk_SHOW tk_PTA tk_PTD tk_R tk_F tk_FOR
%token tk_WH tk_MD

%%

labyrinthe : suite_instruction;

suite_instruction
  : suite_instruction instruction
  | instruction ';';

instruction
  : ';'
  | IDENT '=' xcst
  | IDENT '*' '=' xcst
  | IDENT '/' '=' xcst
  | IDENT '-' '=' xcst
  | IDENT '+' '=' xcst
  | IDENT '%' '=' xcst
  | tk_SIZE xcst
  | tk_SIZE xcst ',' xcst
  | tk_IN pt
  | tk_OUT suite_pt
  | tk_SHOW
  | tk_INST
  | tk_INST tk_PTA suite_pt
  | tk_INST tk_PTD pt suite_ptri
  | tk_INST tk_R pt pt
  | tk_INST tk_R tk_F pt pt
  | tk_INST tk_FOR suite_indice tk_IN suite_intervalle '(' expr ',' expr ')'
  | tk_WH serie_pt
  | tk_MD pt dest_list;

suite_indice
  : suite_indice IDENT
  | IDENT;

suite_intervalle
  : suite_intervalle intervalle
  | intervalle;

intervalle
  : '[' expr ':' expr ']'
  | '[' expr ':' expr ':' expr ']';

serie_pt
  : serie_pt '-' '>' pt
  | pt;

dest_list
  : dest_list dest
  | dest;

dest : DIR pt;

expr
  : IDENT
  | CNUM
  | expr '+' expr
  | expr '-' expr
  | expr '*' expr
  | expr '/' expr
  | expr '%' expr
  | '(' expr ')'
  | '+' expr
  | '-' expr;

xcst
  : IDENT
  | CNUM
  | xcst '+' xcst
  | xcst '-' xcst
  | xcst '*' xcst
  | xcst '/' xcst
  | xcst '%' xcst
  | '(' xcst ')'
  | '+' xcst
  | '-' xcst;

pt : '(' xcst ',' xcst ')';


suite_pt
  : suite_pt pt
  | pt;

suite_ptri
  : suite_ptri pt
  | suite_ptri pt ':' ri
  | pt
  | pt ':' ri;

ri
  : xcst
  | '*';

%%

#include "labgen.yy.c"

int yyerror(const char *mess) {
	fprintf(stderr, "FATAL: %s (near %s) at line %d\n", mess, yytext, yylineno + 1);
	exit(1);
}

int main(int argc, char *argv[]) {
  FILE *f;

  switch(argc) {
    case 2:
      if(strcmp(argv[1], "-") != 0) {
        f = fopen(argv[1], "r");
        if(f == NULL) {
          fprintf(stderr, "%s: %s\n", argv[1], strerror(errno));
          exit(1);
        }
        yyin = f;
      }
      break;

    default:
      fprintf(stderr, "Usage: %s [<if> [<of>]]\n", argv[0]);
      exit(1);
  }

	return yyparse();
}
