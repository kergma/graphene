#ifndef GRAPHENE_SCENE_H
#include "graphene_geometry.h"

typedef struct tagCamPoint {
	VECTOR3F pos,target,up;
	float fov;
	float time;
} CamPoint;

typedef struct tagCamera
{

	VECTOR3F pos, target, up;
	float fov;
	float aspect;


	Array *points;
	int current_index;
	float time;
} Camera;

Camera *camera_create(void);
void camera_free(Camera *c);
void camera_add_point(Camera *c, VECTOR3F pos, VECTOR3F target, VECTOR3F up, float fov, float time);
void camera_set_viewport(Camera *c, int width, int height);
void camera_animate_hermite(Camera *c, float delta);
void camera_animate(Camera *c, float delta);
void camera_render(Camera *c);
void camera_calculate_cliprect(Camera *c, VECTOR3F *clip_rect);

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
