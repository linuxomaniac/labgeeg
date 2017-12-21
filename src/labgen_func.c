#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "list.h"
#include "labgen_func.h"

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

void labAlloc(unsigned int width, unsigned int height) {
	unsigned int i, j;

	lab.height = height + 1;
	lab.width = width + 1;
	lab.wh_count = 0;
	lab.md_count = 0;

	lab.tab = (List **)malloc(sizeof(List *) * lab.height);
	for(i = 0; i < lab.height; i++) {
		lab.tab[i] = (List *)malloc(sizeof(List) * lab.width);
		for(j = 0; j < lab.width; j++) {
			lab.tab[i][j] = listAlloc();
		}
	}
}

void labFree() {
	unsigned int i, j;

	if(labIsAllocated()) {
		for(i = 0; i < lab.height; i++) {
			for(j = 0; j < lab.width; j++) {
				/* TODO boucler dans les éléments de la boucle pour les free */
				listFree(lab.tab[i][j]);
			}
			free(lab.tab[i]);
		}

		free(lab.tab);
	}
}

bool labIsAllocated() {
	return lab.tab != NULL;
}

void labAssertAllocated() {
	if(!labIsAllocated()) {
		fputs("Invalid syntax! Please use SIZE first.\n", stderr);
		/* Pas besoin  de free vu qu'on n'a rien alloc. */
		exit(1);
	}
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
						/* 3 : 1 pour "A" et 2 au max pour la direction */
						wcomp = intlen(state->parentid)
								+ strlen(directionToStr(state->parentdir)) + 1;
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

void showBorder(unsigned int *colsWidth, unsigned int firstColWidth,
				char *lineBuffer, unsigned int lineBufferSize) {
	unsigned int j, processed;

	memset(lineBuffer, '-', lineBufferSize - 1);

	lineBuffer[0] = '+';

	processed = firstColWidth + 1;
	for(j = 0; j < lab.width; j++) {
		processed += colsWidth[j] + 1;
		lineBuffer[processed] = '+';
	}

	lineBuffer[lineBufferSize - 3] = '+';
	lineBuffer[lineBufferSize - 2] = '\n';
	lineBuffer[lineBufferSize - 1] = '\0';
}

unsigned int showCell(List l, unsigned int colWidth, char *buf) {
	Cell *cell;
	CellState *state;
	unsigned int processed = 0;
	char *dir;

	cell = listGetHead(l);
	while(cell != NULL) {
		state = cellGetTarget(cell);

		switch(state->type) {
			case CEMPTY:
				break;

			case CWALL:
				buf[processed] = 'W';
				processed += 1;
				break;
			case CIN:
				buf[processed] = 'E';
				processed += 1;
				break;
			case COUT:
				buf[processed] = 'O';
				processed += 1;
				break;

			case CWH:
			/* Normalement on ne dépassera pas le buffer */
				processed += sprintf(buf + processed, "*%d", state->id);
				break;
			case CMD:
				processed += sprintf(buf + processed, "A%d", state->id);
				break;

			case CMDEXIT:
				dir = directionToStr(state->parentdir);
				processed += sprintf(buf + processed, "a%d%s",
										state->id, dir);
				break;
		}

		cell = cellGetNext(cell);
	}

	if(processed < colWidth) {
		// On complète par des espaces
		memset(buf + processed, ' ', colWidth - processed);
		processed = colWidth;
	}

	buf[processed++] = '|';

	return processed;
}

void labShow(FILE *fout) {
	unsigned int i, j, processed;
	unsigned int *colsWidth;
	unsigned int lineBufferSize, firstColWidth;
	char *lineBuffer;

	labAssertAllocated();

	/* On a lab.width colonnes donc (lab.width + 1) signes "+", et le \0 final,
	 * le \n, mais aussi la largeur de la colonne zéro, celle qui contient les
	 * chiffres.
	 */
	firstColWidth = intlen(lab.height);
	lineBufferSize = lab.width + 3 + firstColWidth;

	/* On calcule les largeurs des colonnes */
	colsWidth = (unsigned int *)malloc(sizeof(unsigned int) * lab.width);
	for(j = 0; j < lab.width; j++) {
		colsWidth[j] = labRowWidth(j);
		/* On ajoute les largeurs indviduelles des colonnes */
		lineBufferSize += colsWidth[j];
	}

	lineBuffer = (char *)malloc(lineBufferSize);

	/* Afficher la bordure supérieure */
	showBorder(colsWidth, firstColWidth, lineBuffer, lineBufferSize);
	fputs(lineBuffer, fout);

	for(i = 0; i < lab.height; i++) {
		memset(lineBuffer, ' ', lineBufferSize - 1);
		lineBuffer[0] = '|';

		processed = 1;
		for(j = 0; j < lab.width; j++) {
			processed += showCell(lab.tab[i][j], colsWidth[j],
									lineBuffer + processed);
		}

		lineBuffer[lineBufferSize - 2] = '\n';
		lineBuffer[lineBufferSize - 1] = '\0';

		/* On afficher ce qu'on vient de calculer */
		fputs(lineBuffer, fout);

		/* Afficher la bordure inférieure */
		showBorder(colsWidth, firstColWidth, lineBuffer, lineBufferSize);
		fputs(lineBuffer, fout);
	}

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