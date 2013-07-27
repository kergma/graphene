#ifndef HAX_GRID_H

#include "hax_map.h"

typedef struct tagGrid 
{
	Map *map;
	float cell_size;
	Array *vertices;
	Array *indices;
} Grid;

extern int grid_render(Grid *g);
Grid *grid_create(Map *map, float cell_size);
int grid_free(Grid *g);
void grid_clear(Grid *g);
VECTOR3F *grid_hex2rect(Grid *g, VECTOR3F *pointxz, HEXCOORD cc);

#define HAX_GRID_H
#endif /* HAX_GRID_H */
