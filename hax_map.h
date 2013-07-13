#ifndef HAX_MAP_H

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

Map *map_new(void);
void map_free(Map *map);

#endif /* HAX_MAP_H */

