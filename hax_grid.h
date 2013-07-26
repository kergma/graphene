#ifndef HAX_GRID_H

#include "hax_map.h"

typedef struct tagGrid 
{
	Map *map;
	float cell_size;
	Array *vertices;
} Grid;

extern int grid_render(Grid *g);
Grid *grid_create(Map *map, float cell_size);
int grid_free(Grid *g);

#define HAX_GRID_H
#endif /* HAX_GRID_H */
