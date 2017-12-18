#ifndef INCLUDE_LABGEN_RES
#define INCLUDE_LABGEN_RES
	typedef enum { CEMPTY, CWH, CMD, CMDEXIT, CIN, COUT, CWALL } CellStateType;
	typedef enum { N, S, E, W, NE, NW, SE, SW } Direction;
	typedef struct _pt pt;

	char *directionToStr(Direction dir);
#endif