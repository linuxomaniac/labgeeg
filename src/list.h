#ifndef _LIST_H
	#define _LIST_H

	#include <stdbool.h>

	typedef struct _cell Cell;
	typedef struct _list *List;

	List listAlloc(void);
	void listFree(List l);
	bool listIsEmpty(List l);
	unsigned int listGetSize(List l);
	Cell *listGetHead(List l);
	Cell *listGetTail(List l);
	void listEnqueue(List l, void *target);
	void listRemoveCell(List l, Cell *cell);
	void *cellGetTarget(Cell *cell);
	void cellSetTarget(Cell *cell, void *target);
	Cell *cellGetNext(Cell *cell);
	Cell *cellGetPrev(Cell *cell);

#endif
