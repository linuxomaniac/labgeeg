#ifndef INCLUDE_LABGEN_RES
#define INCLUDE_LABGEN_RES
	#include <stdio.h>
	#include <stdbool.h>

	typedef enum { CEMPTY, CWH, CMD, CMDEXIT, CIN, COUT, CWALL } CellStateType;
	typedef enum { N, S, E, W, NE, NW, SE, SW } Direction;
	typedef struct _pt pt;

	void labAlloc(unsigned int width, unsigned int height);
	void labFree();
	bool labIsAllocated();
	void labShow(FILE *fout);
	char *directionToStr(Direction dir);
#endif