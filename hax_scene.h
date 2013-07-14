#ifndef HAX_SCENE_H
#include "hax_map.h"
#include "hax_grid.h"
typedef struct tagScene
{
	Map *map;
	Grid *grid;
} Scene;

int scene_render(Scene *s);
Scene *scene_create(void);
int scene_free(Scene *s);

#define HAX_SCENE_H
#endif /* HAX_SCENE_H */
