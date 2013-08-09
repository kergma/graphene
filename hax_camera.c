#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef HAVE_COCOA
# include <OpenGL/glu.h>
#else
# include <GL/glu.h>
#endif

#include <stdlib.h>
#include <string.h>
#include "hax_camera.h"

Camera *camera_create(int ortho)
{
	Camera *c=NEW(Camera);
	if (!c) error_exit("out of memory");
	memset(c,0,sizeof(Camera));

	c->points=array_create(sizeof(CamPoint));
	return c;
}

void camera_free(Camera *c)
{
	array_free(c->points);
	free(c);
}

void camera_add_point(Camera *c, VECTOR3F pos, VECTOR3F target, VECTOR3F up, float fov, float time)
{
	CamPoint cp;
	cp.pos=pos; cp.target=target; cp.up=up; cp.fov=fov; cp.time=time;
	array_add(c->points,&cp);
}

void camera_animate_linear(Camera *c, float delta)
{
}

void camera_animate(Camera *c, float delta)
{
	camera_animate_linear(c,delta);
}

void camera_render(Camera *c)
{

	glLoadIdentity();
	gluLookAt(3.0f,3.0f,3.0f, 0,0,0, 0,1,0.0f);
}
