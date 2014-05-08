#ifndef GRAPHENE_SCENE_H
#include "graphene_map.h"
#include "graphene_grid.h"
#include "graphene_camera.h"


typedef struct tagBgPoint
{
	float time;
	GLfloat r,g,b,a;
} BgPoint;

typedef struct tagScene
{
	Map *map;
	Grid *grid;
	Camera *camera;
	GLfloat clr,clg,clb,cla;
	WayAnimation bganimation;
	int enable_grid_animation;
} Scene;

int scene_render(Scene *s);
Scene *scene_create(char *spec);
int scene_free(Scene *s);
void scene_animate(Scene *s, float delta);

#define GRAPHENE_SCENE_H
#endif /* GRAPHENE_SCENE_H */
