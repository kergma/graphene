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

Camera *camera_create(void)
{
	Camera *c=NEW(Camera);
	if (!c) error_exit("out of memory");
	memset(c,0,sizeof(Camera));

	c->aspect=1;
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

void camera_set_viewport(Camera *c, int width, int height)
{
	c->aspect=(float)width/(float)height;
}

void camera_animate_linear(Camera *c, float delta)
{
	CamPoint current,next;
	int next_index;
	float s;

	array_item(c->points,c->current_index,&current);
	c->time+=delta;
	if (c->time>current.time) {
		c->time=0;
		c->current_index++;
		if (c->current_index>=array_count(c->points)) c->current_index=0;
		array_item(c->points,c->current_index,&current);
	};

	next_index=c->current_index+1;
	if (next_index>=array_count(c->points)) next_index=0;
	array_item(c->points,next_index,&next);

	s=c->time/current.time;

	VECTOR3F_lerp(&c->pos,&current.pos,&next.pos,s);
	VECTOR3F_lerp(&c->target,&current.target,&next.target,s);
	VECTOR3F_lerp(&c->up,&current.up,&next.up,s);

	float_lerp(&c->fov,&current.fov,&next.fov,s);
}

void camera_animate(Camera *c, float delta)
{
	camera_animate_linear(c,delta);
}

void camera_render(Camera *c)
{

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(c->pos.x,c->pos.y,c->pos.z, c->target.x,c->target.y,c->target.z, c->up.x,c->up.y,c->up.z);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(c->fov,c->aspect,1,1e35f);

}
