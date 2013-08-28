#ifndef HAX_GRID_H

#include "hax_map.h"

typedef float GRID_PARAM;

typedef struct tagColorPoint
{
	float time;
	COLOR color1,color2;
} ColorPoint;

typedef struct tagGRID_WAVE
{
	VECTOR3F source;
	float amplitude;
	float length;
	float period;
	WayAnimation color_animation;
	float contrast;
	CIT_EL *cit;
	COLOR color1, color2;
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
	WayAnimation color_animation;
	float contrast;
	CIT_EL *cit;
	COLOR color1,color2;
} Grid;

extern int grid_render(Grid *g);
Grid *grid_create(Map *map, float cell_size, Array *waves, Array *colors, float contrast);
int grid_free(Grid *g);
void grid_clear(Grid *g);
VECTOR3F *grid_hex2rect(Grid *g, VECTOR3F *pointxz, HEXCOORD cc);
void grid_animate(Grid *g, float delta);

#define HAX_GRID_H
#endif /* HAX_GRID_H */
