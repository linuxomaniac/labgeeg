%{
#define _POSIX_C_SOURCE 1
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "iautils/top.h"
#include "iautils/vars.h"

void add_wormhole(Tpoints *l);
void add_md(Tpoint pt, Tpoint3s *l);
void fill(TdrawOpt dopt);
void draw_ptri(TdrawOpt dopt, Tpoint3s *l);

%}

%left '+' '-'
%left '/' '*'
%left '%'
%right  '(' ')'


%token IDENT CNUM DIR
%token tk_SIZE tk_IN tk_OUT tk_SHOW tk_PTA tk_PTD tk_R tk_F tk_FOR
%token tk_WH tk_MD tk_WALL tk_UNWALL tk_TOGGLE

%union {
  int entier;
  char* chaine;
  Tpoints* lpts;
  Tpoint3s* lpt3s;
  Tpoint tpt;
  Tdrawopt insttype;
  Twr direction;
}

%type <lpts> suite_pt
%type <lpt3s> suite_ptri dest_list
%type <tpt> pt
%type <entier> xcst
%type <chaine> IDENT CNUM
%type <insttype> inst
%type <direction> DIR;

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
  | tk_IN pt                                      { if(lds_checkborder_pt(gl_lds, $2)) yyerror("The entry point must be located on the border!"); lds_draw_pt(gl_lds, LG_DrawIn, $2); }
  | tk_OUT suite_pt                               { unsigned int i; for(i = 0; i < $2->nb; i++) { if(lds_checkborder_pt(gl_lds, $2->t[i])) yyerror("Exit points must be located on the border!"); } lds_draw_pts(gl_lds, LG_DrawOut, $2); pts_free($2); }
  | inst                                          { fill($1); }
  | inst tk_PTA suite_pt                          { lds_draw_pts(gl_lds, $1, $3); pts_free($3); }
  | inst tk_PTD pt suite_ptri                     { draw_ptri($1, $3); pt3s_free($3); }
  | inst tk_R pt pt
  | inst tk_R tk_F pt pt
  | inst tk_FOR for_args '(' expr ',' expr ')'
  | tk_WH serie_pt                                { add_wormhole($2); pts_free($2); }
  | tk_MD pt dest_list;                           { add_md($2, $3); pt3s_free($3); }

inst
  : tk_WALL   { $$ = LG_DrawWall; }
  | tk_UNWALL { $$ = LG_DrawUnwall; }
  | tk_TOGGLE { $$ = LG_DrawToggle; };

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
  : IDENT tk_IN intervalle
  | IDENT for_args intervalle;

intervalle
  : '[' expr ':' expr ']'
  | '[' expr ':' expr ':' expr ']';

pt : '(' xcst ',' xcst ')'  { if(lds_check_xy(gl_lds, $2, $4)) yyerror("(%d, %d) out of bounds", $2, $4); $$ = (Tpoint){.x = $2, .y = $4}; };

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
  : suite_ptri pt         { pt3s_app_p2z($$ = $1, $2, 1); }
  | suite_ptri pt ':' ri  { pt3s_app_p2z($$ = $1, $2, $4); }
  | pt                    { $$ = pt3s_new_p2z($1, 1); }
  | pt ':' ri;            { $$ = pt3s_new_p2z($1, $3); }

ri
  : xcst
  | '*';

dest_list
  : dest_list DIR pt  { pt3s_app_p2z($$ = $1, $3, $2); }
  | DIR pt;           { $$ = pt3s_new_p2z($2, $1); };

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

void add_wormhole(Tpoints *l) {
  unsigned int i;
  /* On va ajouter les points deux à deux */
  if(l->nb <= 1) {
    yyerror("Not enough points specified (wanted at least 2, got %d", l->nb);
  }

  for(i = 1; i < l->nb; i++) {
    pdt_wormhole_add(gl_pdt, l->t[i - 1], l->t[i]);
  }
}

void add_md(Tpoint pt, Tpoint3s *l) {
  unsigned int i;
  Tpoint p;
  Tpoint3 p3;
  Tsqmd* md = pdt_magicdoor_getcreate(gl_pdt, gl_lds, pt);

  for(i = 0; i < l->nb; i++) {
    p3 = l->t[i];
    p.x = p3.x;
    p.y = p3.y;

    lds_sqmd_update(md, p3.z, p);
  }
}

void fill(TdrawOpt dopt) {
  unsigned int i, j;

  for(i = 0; !lds_check_xy(gl_lds, i, j); i++) {
    for(j = 0; !lds_check_xy(gl_lds, i, j); j++) {
      lds_draw_xy(gl_lds, dopt, i, j);
    }
  }
}

/* TODO : fix ça pour gérer les étoiles '*' */
void draw_ptri(TdrawOpt dopt, Tpoint3s *l){
  unsigned int i, x = 0, y = 0;
  Tpoint p;

  for(i = 0; i < l->nb; i++) {
    p = l->t[i];

    if(i > 0 && p.x == 0 && p.y == 0) {
      yyerror("(0, 0): invalid move");
    }

    x += p.x;
    y += p.y;

    if(lds_check_xy(gl_lds, x, y)) {
      yyerror("(%d, %d): move out of bounds", x, y);
    }

    lds_draw_xy(gl_lds, dopt, x, y);
  }
}
