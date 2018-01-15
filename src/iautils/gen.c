#include <stdio.h>
#include <stdlib.h>
#include "lds.h"
#include "top.h"

int gen_l(FILE* lstream);
int gen_ytop(FILE* ystream);
int gen_ybody(Tlds*ds, FILE* ystream);
int gen_ybottom(FILE* ystream, Cstr lcfname);
int gen_ybody_xy(Tlds*ds, FILE *ystream, int x, int y);


extern int lg_gen(Tlds*ds, FILE* lstream, FILE*ystream, Cstr lcfname) {
	int ret;

	ret = gen_l(lstream);
	if(ret < 0) {
		return 1;
	}

	ret = gen_ytop(lstream);
	if(ret < 0) {
		return 1;
	}

	ret = gen_ybody(ds, lstream);
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
			if(x != ds->in.x && y != ds->in.y) {
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

	for(i = 0; i < LG_WrNb; i++) {
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

int ybody_write_child(Tlds *ds, FILE *ystream, Twr wr, Tpoint *pt) {
	pt = wh_dest(ds, pt);
	return fprintf(ystream, "|%s square_%d%d", wr_uname(wr), pt->x, pt->y);
}

int gen_ybody_xy(Tlds*ds, FILE *ystream, int x, int y) {
	int ret;
	Tsquare *s;
	Tsqmd *md;
	Tpoint *p, pt;

	/* Le nom de la règle */
	ret = fprintf(ystream, "square_%d%d:", x, y);
	if(ret < 0) {
		return 1;
	}

	s = &ds->squares[x][y];
	md = s->sq_mdp;

	/* Le nord */
	if((p = md_dest(ds, md, LG_WrNN))) {
		if(!p && y > 0 && ds->squares[x][y - 1].kind != LDS_WALL) {
			pt.x = x; pt.y = y - 1;
			p = &pt;
		}

		if(p && ybody_write_child(ds, ystream, LG_WrNN, p) < 0) {
			return 1;
		}
	}

	/* L'ouest */
	if((p = md_dest(ds, md, LG_WrWW))) {
		if(!p && x > 0 && ds->squares[x - 1][y].kind != LDS_WALL) {
			pt.x = x - 1; pt.y = y;
			p = &pt;
		}

		if(p && ybody_write_child(ds, ystream, LG_WrWW, p) < 0) {
			return 1;
		}
	}

	/* L'est */
	if((p = md_dest(ds, md, LG_WrEE))) {
		if(!p && x < ds->dx - 1 && ds->squares[x + 1][y].kind != LDS_WALL) {
			pt.x = x + 1; pt.y = y;
			p = &pt;
		}

		if(p && ybody_write_child(ds, ystream, LG_WrEE, p) < 0) {
			return 1;
		}
	}

	/* Le sud */
	if((p = md_dest(ds, md, LG_WrSS))) {
		if(!p && y < ds->dy - 1 && ds->squares[x][y + 1].kind != LDS_WALL) {
			pt.x = x; pt.y = y + 1;
			p = &pt;
		}

		if(p && ybody_write_child(ds, ystream, LG_WrSS, p) < 0) {
			return 1;
		}
	}

	/* Le nord-ouest */
	if((p = md_dest(ds, md, LG_WrNW))) {
		if(p && x > 0 && y > 0 && ds->squares[x - 1][y - 1].kind != LDS_WALL) {
			pt.x = x - 1; pt.y = y - 1;
			p = &pt;
		}

		if(p && ybody_write_child(ds, ystream, LG_WrNW, p) < 0) {
			return 1;
		}
	}

	/* Le nord-est */
	if((p = md_dest(ds, md, LG_WrNE))) {
		if(p && x < ds->dx - 1 && y > 0 && ds->squares[x + 1][y - 1].kind != LDS_WALL) {
			pt.x = x + 1; pt.y = y - 1;
			p = &pt;
		}

		if(p && ybody_write_child(ds, ystream, LG_WrNE, p) < 0) {
			return 1;
		}
	}

	/* Les Landes */
	if((p = md_dest(ds, md, LG_WrSW))) {
		if(p && x > 0 && y < ds->dx - 1 && ds->squares[x - 1][y + 1].kind != LDS_WALL) {
			pt.x = x - 1; pt.y = y + 1;
			p = &pt;
		}

		if(p && ybody_write_child(ds, ystream, LG_WrSW, p) < 0) {
			return 1;
		}
	}

	/* Le sud-est */
	if((p = md_dest(ds, md, LG_WrSE))) {
		if(p && x < ds->dx - 1 && y < ds->dx - 1 && ds->squares[x + 1][y + 1].kind != LDS_WALL) {
			pt.x = x + 1; pt.y = y + 1;
			p = &pt;
		}

		if(p && ybody_write_child(ds, ystream, LG_WrSE, p) < 0) {
			return 1;
		}
	}

	/* Le point virgule */
	ret = fputs(";\n", ystream);

	return ret < 0;
}