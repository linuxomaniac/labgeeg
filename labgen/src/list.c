#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "list.h"

/*!	\brief Structure de cellule.
 *	Contient 4 pointeurs vers : la valeur, la liste qui contient la cellule, la cellule précédente et la cellule suivante.
 */
struct _cell {
	void *target; /**< Un pointeur vers le contenu de la cellule. */
	struct _list *ownership; /**< Un pointeur vers la liste qui contient la celule. */
	struct _cell *next; /**< Un pointeur vers la cellule suivante. */
	struct _cell *prev; /**< Un pointeur vers la cellule précédente. */
 };

/*!	\brief Structure de liste doublement chainée.
 *	Contient la taille, un pointeur vers la première cellule et un pointeur vers la dernnière.
 */
struct _list {
	unsigned int n; /**< Taille de la liste */
	struct _cell *first; /**< Adresse de la première cellule de la liste */
	struct _cell *last; /**< Adresse de la dernière cellule de la liste */
 };

/*!	\fn List listAlloc(void)
 *	\brief Alloue la mémoire pour une liste
 *	\return La liste en question
 */
List listAlloc(void) {
 	List list = (List)calloc(1, sizeof(struct _list));

 	assert(list != NULL);

 	return list;
 }

/*!	\fn void listFree(List l)
 *	\brief Libère la liste en mémoire.
 *	Bien penser à free les targets des cellules, parce que sinon ça fait des memleaks.
 *	\param l une List
 */
void listFree(List l) {
 	Cell *cell, *cell_next;

 	assert(l != NULL);

	/* Si la liste est vide, cell vaut NULL, donc on ne rentre pas dans le while */
 	cell = listGetHead(l);
 	while(cell != NULL) {
 		cell_next = cellGetNext(cell);
 		free(cell);
 		cell = cell_next;
 	}

 	free(l);
 }

/*!	\fn bool listIsEmpty(List l)
 *	\brief Détermine si une liste est vide.
 *	\param l une List
 *	\return true si la liste est vide et flase sinon
 */

bool listIsEmpty(List l) {
 	assert(l != NULL);

 	return l->n == 0;
 }

/*!	\fn unsigned int listGetSize(List l)
 *	\brief Renvoie la taile d'une liste.
 *	\param l une Liste
 *	\return Un entier étant la taille de la liste
 */
unsigned int listGetSize(List l) {
 	assert(l != NULL);

 	return l->n;
 }

/*!	\fn Cell *listGetHead(List l)
 *	\brief Donne la tête de la liste.
 *	\param l une List
 *	\return un pointeur vers la première cellule Cell de la liste
 */
Cell *listGetHead(List l) {
 	assert(l != NULL);

 	return l->first;
 }

/*!	\fn Cell *listGetTail(List l)
 *	\brief Donne la queue de la liste (le dernier élément).
 *	\param l une List
 *	\return un pointeur vers la dernière cellule Cell de la liste
 */
Cell *listGetTail(List l) {
 	assert(l != NULL);

 	return l->last;
 }

/*!	\fn void listEnqueue(List l, void *target)
 *	\brief Ajoute un élément à la fin de la liste.
 *	\param l une List
 *	\param target Un pointeur vers une entité à ajouter
 */
void listEnqueue(List l, void *target) {
 	Cell *cell;

 	assert(l != NULL);

 	cell = (Cell *)calloc(1, sizeof(Cell));
 	assert(cell != NULL);

 	cell->target = target;
	/* Pour être sûr qu'on modifie bien un élément de la liste et pas qu'on passe un élément Cell au pif */
 	cell->ownership = l;

 	if(listIsEmpty(l)) {
 		l->first = cell;
 		l->last = cell;
 	} else {
 		cell->prev = l->last;
 		l->last->next = cell;
 		l->last = cell;
 	}

 	l->n++;
 }

/*!	\fn void listRemoveCell(List l, Cell *cell)
 *	\brief Retire une cellule d'une liste.
 *	\param l une List
 *	\param cell un pointeur vers une cellule Cell *
 */
void listRemoveCell(List l, Cell *cell) {
	assert(l != NULL && cell != NULL);
	assert(cell->ownership == l);

	if(cell == l->first) {/* C'est le premier élément */
		if(cell->next != NULL) {/* S'il y a du monde après nous */
			l->first = cell->next;
			cell->next->prev = NULL;
		} else {/* C'était le seul élément de la liste */
			l->first = NULL;
			l->last = NULL;
		}
	} else if(cell == l->last) {/* On était le dernier élément */
		if(cell->prev != NULL) {/* Il y a des cellules avant nous */
			cell->prev->next = NULL;
			l->last = cell->prev;
		} else {/* C'était la seule cellule de la liste */
			l->first = NULL;
			l->last = NULL;
		}
	} else {/* La cellule est en plein milieu de la liste */
		cell->prev->next = cell->next;
		cell->next->prev = cell->prev;
	}

	l->n--;

	free(cell);
}


/*!	\fn void *cellGetTarget(Cell *cell)
 *	\brief Retourne la valeur d'une cellule.
 *	\param cell un pointeur vers une cellule Cell
 *	\return le pointeur vers l'élément qui a été ajouté à la liste
 */
void *cellGetTarget(Cell *cell) {
 	assert(cell != NULL);

 	return cell->target;
 }

/*!	\fn void cellSetTarget(Cell *cell, void *target)
 *	\brief Définit la valeur d'une cellule.
 *	\param cell une Cell *
 *	\param target un pointeur vers l'élément à ajouter à la liste
 */
void cellSetTarget(Cell *cell, void *target) {
 	assert(cell != NULL);

 	cell->target = target;
 }

/*!	\fn Cell *cellGetNext(Cell *cell)
 *	\brief Donne la cellule suivante.
 	Sert à itérer sur les cellules
 *	\param cell une Cell *
 *	\return un pointeur vers la cellule suivante de cell, NULL si elle est inexistante
 */
Cell *cellGetNext(Cell *cell) {
 	assert(cell != NULL);

 	return cell->next;
 }

/*!	\fn Cell *cellGetPrev(Cell *cell)
 *	\brief Donne la cellule précédente.
 	Sert à itérer sur les cellules
 *	\param cell une Cell *
 *	\return un pointeur vers la cellule précédente de cell, NULL si elle est inexistante
 */
Cell *cellGetPrev(Cell *cell) {
 	assert(cell != NULL);

 	return cell->prev;
 }
