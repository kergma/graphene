#ifndef HAX_CAMERA_H

#include "hax_util.h"

typedef struct tagCamPoint {
	VECTOR3F pos,target,up;
	float fov;
	float time;
} CamPoint;

typedef struct tagCamera
{

	VECTOR3F pos, target, up;
	float fov;
	int ortho;
	/*D3DMATRIX view,proj;*/
	float aspect;


	Array *points;
	int current_index;
	float time;
} Camera;

Camera *camera_create(int ortho);
void camera_free(Camera *c);
void camera_add_point(Camera *c, VECTOR3F pos, VECTOR3F target, VECTOR3F up, float fov, float time);
void camera_animate_linear(Camera *c, float delta);
void camera_animate(Camera *c, float delta);
void camera_render(Camera *c);

#define HAX_CAMERA_H
#endif /* HAX_CAMERA_H */
