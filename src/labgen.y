%{
#define _POSIX_C_SOURCE 1
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "iautils/top.h"
#include "iautils/vars.h"

%}

%left '+' '-'
%left '/' '*'
%left '%'
%right  '(' ')'


%token IDENT CNUM DIR
%token tk_INST tk_SIZE tk_IN tk_OUT tk_SHOW tk_PTA tk_PTD tk_R tk_F tk_FOR
%token tk_WH tk_MD

%union {
  int entier;
  char* chaine;
  Tpoints* lpts;
  Tpoint tpt;
}

%type <lpts> suite_pt
%type <tpt> pt
%type <entier> xcst
%type <chaine> IDENT CNUM

%%

labyrinthe
  : suite_vars inst_size suite_instructions
  | inst_size suite_instructions;

suite_instructions
  : suite_instructions instruction ';'
  | instruction ';'
  | tk_SHOW                             { lds_dump(gl_lds, stdout); };

instruction
  : ';'
  | tk_IN pt                                      { lds_draw_pt(gl_lds, LDS_IN, $2); }
  | tk_OUT suite_pt                               { lds_draw_pts(gl_lds, LDS_OUT, $2); pts_free($2); }
  | tk_INST
  | tk_INST tk_PTA suite_pt
  | tk_INST tk_PTD pt suite_ptri
  | tk_INST tk_R pt pt
  | tk_INST tk_R tk_F pt pt
  | tk_INST tk_FOR for_args '(' expr ',' expr ')'
  | tk_WH serie_pt
  | tk_MD pt dest_list;

inst_size
  : tk_SIZE xcst ';'          { if($2 < 0 || $2 >= LDS_SIZE) yyerrror("%d invalid size", $2); lds_size_set(gl_lds, $2, $2); }
  | tk_SIZE xcst ',' xcst ';' { if($2 < 0 || $2 >= LDS_SIZE || $4 < 0 || $4 >= LDS_SIZE) yyerrror("%d invalid size", $2); lds_size_set(gl_lds, $2, $4); };

suite_vars
  : suite_vars var ';'
  | var ';';

var
  : IDENT '=' xcst        { Tvar *v; if(v = vars_get(gl_pdt->vars, $1)) v->val = $1; else vars_chgOrAddEated(gl_pdt->vars, $1, $3); $$ = $1; }
  | IDENT '*' '=' xcst    { Tvar *v; if(!(v = vars_get(gl_pdt->vars, $1))) yyerror("%s undefined", $1); $$->val *= $4 }
  | IDENT '/' '=' xcst    { Tvar *v; if(!(v = vars_get(gl_pdt->vars, $1))) yyerror("%s undefined", $1); $$->val /= $4 }
  | IDENT '-' '=' xcst    { Tvar *v; if(!(v = vars_get(gl_pdt->vars, $1))) yyerror("%s undefined", $1); $$->val -= $4 }
  | IDENT '+' '=' xcst    { Tvar *v; if(!(v = vars_get(gl_pdt->vars, $1))) yyerror("%s undefined", $1); $$->val += $4 }
  | IDENT '%' '=' xcst    { Tvar *v; if(!(v = vars_get(gl_pdt->vars, $1))) yyerror("%s undefined", $1); $$->val %= $4 };

for_args
  : IDENT "IN" intervalle
  | IDENT for_args intervalle;

intervalle
  : '[' expr ':' expr ']'
  | '[' expr ':' expr ':' expr ']';

pt : '(' xcst ',' xcst ')'  { if(!lds_check_xy(gl_lds, $2, $4)) yyerror("(%d, %d) out of bounds", $2, $4); $$ = (Tpoint){.x = $2, .y = $4}; };

suite_pt
  : suite_pt pt { pts_app_pt($$ = $1, $2); }
  | pt          { $$ = pts_new_pt($1); };

serie_pt
  : serie_pt suite_tirets '>' pt { pts_app_pt($$ = $1, $2); }
  | pt                           { $$ = pts_new_pt($1); };

suite_tirets
  : suite_tirets '-'
  | '-';

suite_ptri
  : suite_ptri pt
  | suite_ptri pt ':' ri
  | pt
  | pt ':' ri;

ri
  : xcst
  | '*';

dest_list
  : dest_list dest
  | dest;

dest : DIR pt;

expr
  : IDENT
  | CNUM
  | expr '+' expr { $$ = $1 + $3; }
  | expr '-' expr { $$ = $1 - $3; }
  | expr '*' expr { $$ = $1 * $3; }
  | expr '/' expr { $$ = $1 / $3; }
  | expr '%' expr { $$ = $1 % $3; }
  | '(' expr ')'  { $$ = $2; }
  | '+' expr      { $$ = $2; }
  | '-' expr      { $$ = - $2; };

xcst
  : IDENT         { Tvar *v; if(!(v = vars_get(gl_pdt->vars, $1))) yyerror("%s undefined", $1); $$ = v->val; }
  | CNUM          { $$ = $1; }
  | xcst '+' xcst { $$ = $1 + $3; }
  | xcst '-' xcst { $$ = $1 - $3; }
  | xcst '*' xcst { $$ = $1 * $3; }
  | xcst '/' xcst { $$ = $1 / $3; }
  | xcst '%' xcst { $$ = $1 % $3; }
  | '(' xcst ')'  { $$ = $2; }
  | '+' xcst      { $$ = $2; }
  | '-' xcst      { $$ = - $2; };

%%

#include "labgen.yy.c"

extern void yyerror(const char* fmt, ...) {
  char buf[10000];
  va_list ap;
  va_start(ap,fmt);
  vsprintf(buf,fmt,ap);
  va_end(ap);
  fprintf(stderr,"%s:%d: %s (near '%s')\n",gl_infname,yylineno,buf,yytext);
  exit(1);
}
