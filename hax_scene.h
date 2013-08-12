#ifndef HAX_SCENE_H
#include "hax_map.h"
#include "hax_grid.h"
#include "hax_camera.h"


typedef struct tagBgPoint
{
	GLfloat r,g,b,a;
	float time;
} BgPoint;

typedef struct tagScene
{
	Map *map;
	Grid *grid;
	Camera *camera;
	Array *bgcolors;
	GLfloat clr,clg,clb,cla;
	int current_bg;
	float bgtime;
} Scene;

int scene_render(Scene *s);
Scene *scene_create(char *spec);
int scene_free(Scene *s);
void scene_animate(Scene *s, float delta);

#define HAX_SCENE_H
#endif /* HAX_SCENE_H */
