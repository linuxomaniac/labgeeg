#include <stdio.h>
#include <stdlib.h>
#include "lds.h"
#include "top.h"

int gen_l(FILE* lstream);
int gen_ytop(FILE* ystream);
int gen_ybody(Tlds*ds, FILE* ystream);
int gen_ybottom(FILE* ystream, Cstr lcfname);
int gen_ybody_xy(Tlds*ds, FILE *ystream, int x, int y);

Twr ij_to_wr[3][3] = {{LG_WrNW, LG_WrNN, LG_WrNE}, {LG_WrWW, LG_WrUU, LG_WrEE}, {LG_WrSW, LG_WrSS, LG_WrSE}};

extern int lg_gen(Tlds*ds, FILE* lstream, FILE*ystream, Cstr lcfname) {
	int ret;

	ret = gen_l(lstream);
	if(ret < 0) {
		return 1;
	}

	ret = gen_ytop(ystream);
	if(ret < 0) {
		return 1;
	}

	ret = gen_ybody(ds, ystream);
	if(ret < 0) {
		return 1;
	}

	ret = gen_ybottom(ystream, lcfname);
	if(ret < 0) {
		return 1;
	}

	return 0;
}

int gen_l(FILE* lstream) {
	int ret = fputs(
		"%option noyywrap\n"
		"%%\n"
		"\"SE\" {return SE;}\n"
		"\"SW\" {return SW;}\n"
		"\"NE\" {return NE;}\n"
		"\"NW\" {return NW;}\n"
		"\"N\" {return N;}\n"
		"\"S\" {return S;}\n"
		"\"W\" {return W;}\n"
		"\"E\" {return E;}\n"
		"[ \\n\\t] ;\n"
		"#.*\\n ;\n"
		". {fputs(\"syntax error\\n\",stderr);exit(1);}\n"
		, lstream);

	return ret < 0;
}

int gen_ytop(FILE *ystream) {
	int ret = fputs(
		"%{\n"
		"#include <stdio.h>\n"
		"#include <stdlib.h>\n"
		"int yyerror(const char *mess);\n"
		"int yylex();\n"
		"%}\n"
		"%token SE SW NE NW\n"
		"%token N S W E\n"
		"%%\n"
		, ystream);

	return ret < 0;
}

int gen_ybottom(FILE* ystream, Cstr lcfname) {
	int ret;

	ret = fputs(
		"Exit:;\n"
		"%%\n"
		, ystream);
	if(ret < 0) {
		return 1;
	}

	ret = fprintf(ystream, "#include \"%s\"\n", lcfname);
	if(ret < 0) {
		return 1;
	}

	ret = fputs(
		"int yyerror(const char *mess) {}"
		"int main(int argc, char *argv[]) {"
  		"FILE *f = NULL;"
  		"int r;"
		"if(argc == 2) {"
    	"if(strcmp(argv[1], \"-\") != 0) {"
      	"f = fopen(argv[1], \"r\");"
      	"if(f == NULL) {"
        "fprintf(stderr, \"%s: %s\\n\", argv[1], strerror(errno));"
        "exit(1);"
      	"}"
      	"yyin = f;"
    	"}"
  		"} else if(argc > 2) {"
    	"fprintf(stderr, \"Usage: %s [<if>]\\n\", argv[0]);"
    	"exit(1);"
  		"}"
  		"r = yyparse();"
  		"if(f) {"
    	"fclose(f);"
  		"}"
  		"if(!r) {"
    	"fputs(\"gagné\\n\", stdout);"
  		"} else {"
    	"fputs(\"perdu\\n\", stderr);"
  		"}"
		"return r;"
		"}\n"
		, ystream);

	return ret < 0;
}

int gen_ybody(Tlds*ds, FILE *ystream) {
	int x, y, ret;

	/* On commence par traiter l'entrée, ce doit être la première règle. */
	x = ds->in.x;
	y = ds->in.y;

	ret = gen_ybody_xy(ds, ystream, x, y);
	if(ret) {
		return 1;
	}

	for(x = 0; x < ds->dx; x++) {
		for(y = 0; y < ds->dy; y++) {
			if(!(x == ds->in.x && y == ds->in.y) && ds->squares[x][y].kind != LDS_WALL) {
				ret = gen_ybody_xy(ds, ystream, x, y);
				if(ret) {
					return 1;
				}
			}
		}
	}
	
	return 0;
}

/* Renvoie un pointeur vers le point de destination de la magicdoor, ou NULL si inexistant */
Tpoint *md_dest(Tlds *ds, Tsqmd *md, Twr wr) {
	int i;

	if(!md) {
		return NULL;
	}

	for(i = LG_WrFirst; i < LG_WrNb; i++) {
		if(md->t[i].chg && md->t[i].wrd == wr) {
			return &md->t[i].dest;
		}
	}

	return NULL;
}

/* Renvoie le point de destination à travers tous les wh */
Tpoint *wh_dest(Tlds *ds, Tpoint *src) {
	Tsquare *s = &ds->squares[src->x][src->y];

	if(s->opt == LDS_OptWH) {
		return wh_dest(ds, &s->sq_whd);
	}

	return src;
}

int ybody_write_child(Tlds *ds, FILE *ystream, Twr wr, int *started, Tpoint *pt) {
	int ret;

	pt = wh_dest(ds, pt);

	if(ds->squares[pt->x][pt->y].kind == LDS_OUT) {
		ret = fprintf(ystream, "%s%s Exit", (*started?"|":""), wr_uname(wr));
	} else {
		ret = fprintf(ystream, "%s%s square_%d_%d", (*started?"|":""), wr_uname(wr), pt->x, pt->y);
	}

	*started = 1;
	return ret;
}

int gen_ybody_xy(Tlds*ds, FILE *ystream, int x, int y) {
	int ret;
	Tsqmd *md;
	Tpoint *p, pt;
	Twr wr;
	int i, j, xl, yl, started = 0;

	/* Le nom de la règle */
	ret = fprintf(ystream, "square_%d_%d:", x, y);
	if(ret < 0) {
		return 1;
	}

	md = (ds->squares[x][y].kind == LDS_FREE && ds->squares[x][y].opt == LDS_OptMD)?ds->squares[x][y].sq_mdp:NULL;

	/* Boucle pour vérifier les huit directions */
	for(i = 0; i < 3; i++) {
		for(j = 0; j < 3; j++) {
			xl = x + i - 1;
			yl = y + j - 1;

			if(!(i == 1 && j == 1) && xl >= 0 && yl >= 0 && xl < ds->dx && yl < ds->dy) {
				wr = ij_to_wr[j][i];

				p = md_dest(ds, md, wr);
				if(!p && ds->squares[xl][yl].kind != LDS_WALL) {
					pt.x = xl; pt.y = yl;
					p = &pt;
				}
				if(p && ybody_write_child(ds, ystream, wr, &started, p) < 0) {
					return 1;
				}

			}
		}
	}

	/* Le point virgule */
	ret = fputs(";\n", ystream);

	return ret < 0;
}