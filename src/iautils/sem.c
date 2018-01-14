#include "top.h"
#include "lds.h"
#include "pdt.h"

int recurs_wh_check(Tlds *ds, Tpoints *pts, Tpoint pt) {
	Tsquare *sq = &ds->squares[pt.x][pt.y];

	if(sq->opt == LDS_OptWH) {
		if(pts_mem_pt(pts, pt)) {
			return 1;
		} else {
			pts_app_pt(pts, pt);
			return recurs_wh_check(ds, pts, sq->sq_whd);
		}
	}

	return 0;
}

extern int lg_sem(Tlds*ds, const Tpdt*pdt) {
	Tsquare *sq;
	Tpoints *pts;
	int x, y;
	int e_count = 0, o_count = 0;

	/* Vérifications des contraines RS-2 et RS-3 */
	for(x = 0; x < ds->dx; x++) {
		for(y = 0; y < ds->dy; y++) {
			sq = &ds->squares[x][y];

			if(sq->kind == LDS_IN) {
				if(sq->opt == LDS_OptWH) {
					yyerror("An entrance can't be on a WH entrance!");
				}
				e_count += 1;
			} else if(sq->kind == LDS_OUT) {
				if(sq->opt == LDS_OptWH) {
					yyerror("An exit can't be on a WH entrance!");
				}
				o_count += 1;
			}
		}
	}

	if(e_count != 1) {
		yyerror("Exactly one entrance needed!");
	}

	if(o_count == 0) {
		yyerror("At least one exit needed!");
	}

	/* Vérification de la RS-10 */
	for(x = 0; x < pdt->wh->nb && !y; x++) {
		pts = pts_new();
		y = recurs_wh_check(ds, pts, pdt->wh->t[x]);
		pts_free(pts);
	}

	if(y) {
		yyerror("Infinite loop between worm holes!");
	}

	return 0;
}
