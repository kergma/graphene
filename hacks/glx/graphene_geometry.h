#ifndef GRAPHENE_GEOMETRY_H
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

VECTOR3F *hex2rect(float cell_size, VECTOR3F *pointxz, HEXCOORD cc);
HEXCOORD *rect2hex(float cell_size, HEXCOORD *cc, VECTOR3F pointxz);

typedef struct tagMap {
	Array *cells;
	MapCell **array2d;
	int u_low, u_high, v_low, v_high, u_width, v_width;

} Map;

Map *map_create(void);
void map_free(Map *m);

void map_create_hex(Map *m, int size);
void map_create_rect(Map *m, VECTOR3F *clip_rect, float cell_size);
void map_fill_array2d(Map *m);

void map_add_cell(Map *m, MapCell *c);

MapCell *map_cell(Map *m, HEXCOORD cc);
MapCell *map_neighbour(Map *m, MapCell *cell, HEXCOORD cdelta);

HEXCOORD HEXCOORD_c(int u, int v, int f);
HEXCOORD HEXCOORD_mulint(HEXCOORD h, int m);
HEXCOORD HEXCOORD_add(HEXCOORD h, HEXCOORD a);

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
void grid_animate(Grid *g, float delta);

#define GRAPHENE_GEOMETRY_H
#endif /* GRAPHENE_GEOMETRY_H */
