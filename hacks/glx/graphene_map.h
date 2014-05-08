#ifndef GRAPHENE_MAP_H
#include "graphene_util.h"

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
	MapCell **array2d;
	int u_low, u_high, v_low, v_high, u_width, v_width;
	
} Map;

Map *map_create(void);
void map_free(Map *m);

void map_create_hex(Map *m, int size);
void map_fill_array2d(Map *m);

void map_add_cell(Map *m, MapCell *c);

MapCell *map_cell(Map *m, HEXCOORD cc);
MapCell *map_neighbour(Map *m, MapCell *cell, HEXCOORD cdelta);

HEXCOORD HEXCOORD_c(int u, int v, int f);
HEXCOORD HEXCOORD_mulint(HEXCOORD h, int m);
HEXCOORD HEXCOORD_add(HEXCOORD h, HEXCOORD a);

#define GRAPHENE_MAP_H
#endif /* GRAPHENE_MAP_H */

