#ifndef HAX_SCENE_H
#include "hax_map.h"
#include "hax_grid.h"
typedef struct tagScene
{
	Map *map;
	Grid *grid;
} Scene;

typedef struct tag_SceneSpec
{
	int map_size;
	float cell_size;
} SceneSpec;

int scene_render(Scene *s);
Scene *scene_create(SceneSpec *spec);
int scene_free(Scene *s);
void scene_animate(Scene *s, float delta);

#define HAX_SCENE_H
#endif /* HAX_SCENE_H */
