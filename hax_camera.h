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
void camera_render(Camera *c);

#define HAX_CAMERA_H
#endif /* HAX_CAMERA_H */
