#ifndef HAX_GRID_H

#include "hax_map.h"

typedef struct tagGrid 
{
	Map *map;
	float cell_size;
} Grid;

extern int grid_render(void);
Grid *grid_create(Map *map, float cell_size);
int grid_free(Grid *grid);

#define HAX_GRID_H
#endif /* HAX_GRID_H */
