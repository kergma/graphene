#ifndef HAX_MAP_H
#include "hax_util.h"

typedef struct tagHEXCOORD {
	int u,v,f;
} HEXCOORD;

typedef struct tagMapCell {
	HEXCOORD Coord;
/*	WORD Index; */
	HEXAGON_EDGE *edges[6];
} MapCell;		

extern HEXCOORD hex_direction[];

typedef struct tagMap {
	Array *cells;
	
} Map;

Map *map_create(void);
void map_free(Map *m);

void map_create_hex(Map *m, int size);

void map_add_cell(Map *m, MapCell *c);

HEXCOORD HEXCOORD_c(int u, int v, int f);
HEXCOORD HEXCOORD_mulint(HEXCOORD h, int m);
HEXCOORD HEXCOORD_add(HEXCOORD h, HEXCOORD a);

#define HAX_MAP_H
#endif /* HAX_MAP_H */

