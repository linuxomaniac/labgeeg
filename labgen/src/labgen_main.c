#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "labgen_main.h"

/* Structure globale du labyrinthe */
struct {
	List **tab;
	unsigned int width, height;
	unsigned int wh_count, md_count;
} lab = {0};

typedef struct {
	/* Énumérateur du type de cellule */
	CellStateType type;

	/* identifiant unique du type (MD n°x, WH n°y etc) */
	unsigned int id;

	/* Sert à identifier d'où on vient */
	unsigned int parentid;

	/* Set à identifier la direction parente (utile pour print les magic
	 * doors)
	 */
	Direction parentdir;

	/* Liste des cellules filles (dans le cas des magic doors).
	 * Sert à stocker en mémoire une liste de directions mappées
	 * avec des cellules.
	 */
	List child;
} CellState;

struct _pt {
	unsigned int x, y;
};

unsigned int intlen(int n) {
	unsigned int ret = 0;

	while(n >= 1) {
		n = n / 10;
		ret++;
	}

	return ret;
}

/* Calcule la largeur max d'une colonne pour la show() */
unsigned int labRowWidth(unsigned int j) {
	unsigned int i, w = 0, wcomp;
	Cell *cell;
	CellState *state;

	for(i = 0; i < lab.height; i++) {
		if(lab.tab[i][j] != NULL) {
			cell = listGetHead(lab.tab[i][j]);
			while(cell != NULL) {
				state = cellGetTarget(cell);

				switch(state->type) {
					case CEMPTY:
						break;

					case CWALL:
					case CIN:
					case COUT:
						wcomp = 1;
						break;

					case CWH:
					case CMD:
						/* 1 pour "A" (ou "W") */
						wcomp = intlen(state->id) + 1;
						break;

					case CMDEXIT:
						/* 3 : 1 pour "A" et 2 pour la direction */
						wcomp = intlen(state->parentid) + 3;
						break;
				}

				if(wcomp > w) {
					w = wcomp;
				}

				cell = cellGetNext(cell);
			}
		}
	}

	return w;
}

void labAlloc(unsigned int width, unsigned int height) {
	unsigned int i, j;

	lab.height = height + 1;
	lab.width = width + 1;
	lab.wh_count = 0;
	lab.md_count = 0;

	lab.tab = (List **)malloc(sizeof(List *) * lab.height);
	for(i = 0; i < lab.height; i++) {
		lab.tab[i] = (List *)malloc(sizeof(List) * lab.width);
		for(j = 0; j < lab.height; j++) {
			lab.tab[i][j] = listAlloc();
		}
	}
}

void labFree() {
	unsigned int i, j;

	if(lab.tab != NULL) {
		for(i = 0; i < lab.height; i++) {
			for(j = 0; j < lab.height; j++) {
				listFree(lab.tab[i][j]);
			}
			free(lab.tab[i]);
		}

		free(lab.tab);
	}
}

void show() {
	unsigned int i, j;
	unsigned int *colsWidth;
	unsigned int lineBufferSize;
	unsigned int cellBufferSize = 0;
	char *lineBuffer, *cellBuffer;

	/* On a lab.width colonnes donc lab.width + 1 signes "+", et le \0 final */
	lineBufferSize = lab.width + 2;

	/* On calcule les largeurs des colonnes */
	colsWidth = (unsigned int *)malloc(sizeof(unsigned int) * lab.width);
	for(j = 0; j < lab.width; j++) {
		colsWidth[j] = labRowWidth(j);
		/* On ajoute les largeurs indviduelles des colonnes */
		lineBufferSize += colsWidth[j];
		if(colsWidth[j] > cellBufferSize) {
			cellBufferSize = colsWidth[j];
		}
	}

	lineBuffer = (char *)malloc(lineBufferSize);
	cellBuffer = (char *)malloc(cellBufferSize);

	free(cellBuffer);
	free(lineBuffer);
	free(colsWidth);
}

char *directionToStr(Direction dir) {
	char *ret = NULL;

	switch(dir) {
		case N:
			ret = "N";
			break;
		case S:
			ret = "S";
			break;
		case E:
			ret = "E";
			break;
		case W:
			ret = "W";
			break;
		case NE:
			ret = "NE";
			break;
		case NW:
			ret = "NW";
			break;
		case SE:
			ret = "SE";
			break;
		case SW:
			ret = "SW";
			break;
	}

	return ret;
}