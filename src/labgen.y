%{
#define _POSIX_C_SOURCE 1
#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#include "iautils/top.h"
#include "iautils/vars.h"
#include "iautils/lds.h"
#include "iautils/pdt.h"
#include "iautils/expr.h"

#define MIN(xx,yy) (((xx)<(yy))?(xx):(yy))
#define MAX(xx,yy) (((xx)>(yy))?(xx):(yy))

int yylex();

void add_wormhole(Tpoints *l);
void add_md(Tpoint pt, Tpoint3s *l);
void fill(TdrawOpt dopt);
void draw_ptri(TdrawOpt dopt, Tpoint3s *l);
void draw_rect(Tpoint p1, Tpoint p2, TdrawOpt dopt, bool F);
void check_wall(Tpoint p);
void for_function(TdrawOpt dopt, int level, int max_level, Texpr* e1, Texpr* e2);

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
  Tpoint3 tpt3;
  TdrawOpt insttype;
  Twr direction;
  Texpr* expressionarbreuse;
}

%type <lpts> suite_pt serie_pt
%type <lpt3s> suite_ptri dest_list
%type <tpt> pt
%type <tpt3> range
%type <entier> xcst ri CNUM for_args
%type <chaine> IDENT 
%type <insttype> inst
%type <direction> DIR;
%type <expressionarbreuse> expr;

%%

labyrinthe
  : suite_vars instruction_size suite_instructions
  | instruction_size suite_instructions;

suite_instructions
  : suite_instructions_murs suite_instructions_autres
  | suite_instructions_murs
  | suite_instructions_autres;

instruction_show
  : tk_SHOW                                       { lds_dump(gl_lds, stdout); };

suite_instructions_autres
  : suite_instructions_autres instruction_autre ';'
  | instruction_autre ';'

instruction_autre
  : ';'
  | instruction_show
  | tk_IN pt                                      { if(lds_checkborder_pt(gl_lds, $2)) yyerror("The entry point must be located on the border!"); lds_draw_pt(gl_lds, LG_DrawIn, $2); }
  | tk_OUT suite_pt                               { unsigned int i; for(i = 0; i < $2->nb; i++) { if(lds_checkborder_pt(gl_lds, $2->t[i])) yyerror("Exit points must be located on the border!"); } lds_draw_pts(gl_lds, LG_DrawOut, $2); pts_free($2); }
  | tk_WH serie_pt                                { add_wormhole($2); pts_free($2); }
  | tk_MD pt dest_list                            { add_md($2, $3); pt3s_free($3); };

suite_instructions_murs
  : suite_instructions_murs instruction_mur ';'
  | instruction_mur ';'

instruction_mur
  : inst                                          { fill($1); }
  | inst tk_PTA suite_pt                          { lds_draw_pts(gl_lds, $1, $3); pts_free($3); }
  | inst tk_PTD pt suite_ptri                     { draw_ptri($1, $4); pt3s_free($4); }
  | inst tk_R pt pt                               { draw_rect($3, $4, $1, false); }
  | inst tk_R tk_F pt pt                          { draw_rect($4, $5, $1, true); }
  | inst tk_FOR for_args '(' expr ',' expr ')'    { for_function($1, 0, $3, $5, $7); };

inst
  : tk_WALL   { $$ = LG_DrawWall; }
  | tk_UNWALL { $$ = LG_DrawUnwall; }
  | tk_TOGGLE { $$ = LG_DrawToggle; };

for_args
  : IDENT tk_IN range		{ $$ = 1; gl_pdt->fvars_names[gl_pdt->fvars_index++] = strdup($1); vars_chgOrAddEated(gl_pdt->fvars, $1, 0); pt3s_app_pt3(gl_pdt->frgs, $3); }
  | IDENT for_args range	{ $$ = $2 + 1; gl_pdt->fvars_names[gl_pdt->fvars_index++] = strdup($1); vars_chgOrAddEated(gl_pdt->fvars, $1, 0); pt3s_app_pt3(gl_pdt->frgs, $3); };

instruction_size
  : tk_SIZE xcst ';'          { if($2 < 2 || $2 >= LDS_SIZE) yyerror("%d invalid size", $2); lds_size_set(gl_lds, $2, $2); }
  | tk_SIZE xcst ',' xcst ';' { if($2 < 2 || $2 >= LDS_SIZE || $4 < 0 || $4 >= LDS_SIZE) yyerror("%d invalid size", $2); lds_size_set(gl_lds, $2, $4); };

suite_vars
  : suite_vars var ';'
  | var ';';

var
  : IDENT '=' xcst        { Tvar *v; if(v = vars_get(gl_pdt->vars, $1)) v->val = $3; else vars_chgOrAddEated(gl_pdt->vars, $1, $3); }
  | IDENT '*' '=' xcst    { Tvar *v; if(!(v = vars_get(gl_pdt->vars, $1))) yyerror("%s undefined", $1); v->val *= $4 ;}
  | IDENT '/' '=' xcst    { Tvar *v; if(!(v = vars_get(gl_pdt->vars, $1))) yyerror("%s undefined", $1); v->val /= $4 ;}
  | IDENT '-' '=' xcst    { Tvar *v; if(!(v = vars_get(gl_pdt->vars, $1))) yyerror("%s undefined", $1); v->val -= $4 ;}
  | IDENT '+' '=' xcst    { Tvar *v; if(!(v = vars_get(gl_pdt->vars, $1))) yyerror("%s undefined", $1); v->val += $4 ;}
  | IDENT '%' '=' xcst    { Tvar *v; if(!(v = vars_get(gl_pdt->vars, $1))) yyerror("%s undefined", $1); v->val %= $4 ;};

range
  : '[' CNUM ':' CNUM ']'             { if($2 > $4) yyerror("Value error: %d > %d", $2, $4); $$ = (Tpoint3){.xy.x = $2, .xy.y = $4, .z = 1}; }
  | '[' CNUM ':' CNUM ':' CNUM ']'   { if($2 > $4) yyerror("Value error: %d > %d", $2, $4); if($6 <1) yyerror("Value error: %d must be greater than 0", $6); $$ = (Tpoint3){.xy.x = $2, .xy.y = $4, .z = $6}; }
  | '[' CNUM ':' CNUM '['             { int v = $4 -1; if($2 > v) yyerror("Value error: %d > %d", $2, v); $$ = (Tpoint3){.xy.x = $2, .xy.y = v, .z = 1}; }
  | '[' CNUM ':' CNUM ':' CNUM '['   { int v = $4 - 1; if($2 > v) yyerror("Value error: %d > %d", $2, v); if($6 <1) yyerror("Value error: %d must be greater than 0", $6); $$ = (Tpoint3){.xy.x = $2, .xy.y = v, .z = $6}; };

pt : '(' xcst ',' xcst ')'  { if(lds_check_xy(gl_lds, $2, $4)) yyerror("(%d, %d) out of bounds", $2, $4); $$ = (Tpoint){.x = $2, .y = $4}; };

suite_pt
  : suite_pt pt { pts_app_pt($$ = $1, $2); }
  | pt          { $$ = pts_new_pt($1); };

serie_pt
  : serie_pt suite_tirets '>' pt { pts_app_pt($$ = $1, $4); }
  | pt                           { $$ = pts_new_pt($1); };

suite_tirets
  : suite_tirets '-'
  | '-';

suite_ptri
  : suite_ptri pt         { pt3s_app_p2z($$ = $1, $2, 1); }
  | suite_ptri pt ':' ri  { pt3s_app_p2z($$ = $1, $2, $4); }
  | pt                    { $$ = pt3s_new_p2z($1, 1); }
  | pt ':' ri             { $$ = pt3s_new_p2z($1, $3); };

ri
  : xcst
  | '*'   { $$ = -1; };

dest_list
  : dest_list DIR pt  { pt3s_app_p2z($$ = $1, $3, $2); }
  | DIR pt            { $$ = pt3s_new_p2z($2, $1); };

expr
  : IDENT		  { $$ = expr_varEated($1); }
  | CNUM		  { $$ = expr_cst($1); }
  | expr '+' expr { $$ = expr_binOp(EXPKD_PLUS, $1, $3); }
  | expr '-' expr { $$ = expr_binOp(EXPKD_MINUS, $1, $3); }
  | expr '*' expr { $$ = expr_binOp(EXPKD_TIME, $1, $3); }
  | expr '/' expr { $$ = expr_binOp(EXPKD_DIV, $1, $3); }
  | expr '%' expr { $$ = expr_binOp(EXPKD_MOD, $1, $3); }
  | '(' expr ')'  { $$ = $2; }
  | '+' expr      { $$ = $2; }
  | '-' expr      { $$ = expr_uniOp(EXPKD_NEG, $2); };

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

  check_wall(l->t[0]);

  for(i = 1; i < l->nb; i++) {
    if(lds_md_or_wh_pt(gl_lds, l->t[i - 1])) {
      yyerror("A MD or WH is already present on (%d, %d)", l->t[i - 1].x, l->t[i - 1].y);
    }

    check_wall(l->t[i]);

    pdt_wormhole_add(gl_pdt, l->t[i - 1], l->t[i]);
  }
}

void add_md(Tpoint pt, Tpoint3s *l) {
  unsigned int i;
  Tpoint p;
  Tpoint3 p3;
  Tsqmd* md;

  if(lds_md_or_wh_pt(gl_lds, pt)) {
    yyerror("A MD or WH is already present on (%d, %d)", pt.x, pt.y);
  }

  check_wall(pt);

  md = pdt_magicdoor_getcreate(gl_pdt, gl_lds, pt);

  for(i = 0; i < l->nb; i++) {
    p3 = l->t[i];
    p.x = p3.xy.x;
    p.y = p3.xy.y;

    check_wall(p);

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

void draw_ptri(TdrawOpt dopt, Tpoint3s *l){
  unsigned int i, x = 0, y = 0, k;
  Tpoint3 pt;

  for(i = 0; i < l->nb; i++) {
    pt = l->t[i];

    if(i > 0 && pt.xy.x == 0 && pt.xy.y == 0) {
      yyerror("(0, 0): invalid move");
    }

    for(k = 0; k < pt.z || pt.z == -1; k++) {
      x += pt.xy.x;
      y += pt.xy.y;

      if(lds_check_xy(gl_lds, x, y)) {
        if(pt.z == -1) {
          break;
        } else {
          yyerror("(%d, %d): move out of bounds", x, y);
        }
      }

      lds_draw_xy(gl_lds, dopt, x, y);
    }
  }
}

void draw_rect(Tpoint p1, Tpoint p2, TdrawOpt dopt, bool F) {
  Tpoint pmin, pmax;
  unsigned int x, y;

  pmin.x = MIN(p1.x, p2.x);
  pmin.y = MIN(p1.y, p2.y);
  pmax.x = MAX(p1.x, p2.x);
  pmax.y = MAX(p1.y, p2.y);

  for(x = pmin.x; x <= pmax.x; x++) {
    for(y = pmin.y; x <= pmax.y; y++) {
      if(F || (x == p1.x || x == p2.x || y == p1.y || y == p2.y)) {
        lds_draw_xy(gl_lds, dopt, x, y);
      }
    }
  }
}

void check_wall(Tpoint p) {
  if(lds_check_pt(gl_lds, p) == LDS_WALL) {
    lds_draw_pt(gl_lds, LG_DrawUnwall, p);

    fprintf(stderr, "Warning: a wall was overwritten at (%d, %d)\n", p.x, p.y);
  }
}


int for_function_eval(Texpr *e) {
  const char uv[100];
  int val = 0, ret = expr_eval(e, gl_pdt->fvars, &val, (const char **)&uv);

  switch(ret) {
    case 1:
      yyerror("Expression eval error: Undefined variable %s\n", uv);
      break;

    case 2:
      yyerror("Expression eval error: Zero division\n");
      break;
  }

  return val;
}

void for_function(TdrawOpt dopt, int level, int max_level, Texpr* e1, Texpr* e2) {
  if(level == max_level) {
    int x, y;

    x = for_function_eval(e1);
    y = for_function_eval(e2);

    if(!lds_check_xy(gl_lds, x, y)) {
      lds_draw_xy(gl_lds, dopt, x, y);
    }
  } else {
    /* Fetch the variable value in the bintree by its name in the array given the index (level) */
    int *var = &(vars_get(gl_pdt->fvars, gl_pdt->fvars_names[level])->val);
    Tpoint3 *rg = &(gl_pdt->frgs->t[max_level - level - 1]);

    for(*var = rg->xy.x; *var < rg->xy.y; *var += rg->z) {
      for_function(dopt, level + 1, max_level, e1, e2);
    }
  }
}
