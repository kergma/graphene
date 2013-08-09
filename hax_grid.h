#ifndef HAX_GRID_H

#include "hax_map.h"

typedef float GRID_PARAM;

typedef struct tagGRID_WAVE {
	VECTOR3F source;
	float amplitude;
	float length;
	float period;
} GRID_WAVE;

typedef struct tagGrid 
{
	Map *map;
	float cell_size;
	Array *vertices;
	Array *indices;
	Array *waves;
	float amplitudes_sum;
	GRID_PARAM *data;
	float time;
} Grid;

extern int grid_render(Grid *g);
Grid *grid_create(Map *map, Array *waves, float cell_size);
int grid_free(Grid *g);
void grid_clear(Grid *g);
VECTOR3F *grid_hex2rect(Grid *g, VECTOR3F *pointxz, HEXCOORD cc);
void grid_animate(Grid *g, float delta);

#define HAX_GRID_H
#endif /* HAX_GRID_H */
