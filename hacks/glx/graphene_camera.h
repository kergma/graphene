#ifndef GRAPHENE_CAMERA_H

#include "graphene_util.h"

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

#define GRAPHENE_CAMERA_H
#endif /* GRAPHENE_CAMERA_H */
