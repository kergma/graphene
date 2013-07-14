#ifndef HAX_MAP_H
#include "hax_util.h"

typedef struct tagHEXCOORD {
	int u,v,f;
} HEXCOORD;

typedef struct tagMapCell {
	HEXCOORD Coord;
/*	WORD Index; */
	HEXAGON_EDGE *edges[6];
} MapCell, *LPMapCell;		

extern HEXCOORD hex_direction[];

typedef struct tagMap {
	LPMapCell *cells;
} Map;

Map *map_create(void);
void map_free(Map *map);

#define HAX_MAP_H
#endif /* HAX_MAP_H */

