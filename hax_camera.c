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

	if (array_count(c->points)==0) return;

	array_item(c->points,c->current_index,&current);
	c->time+=delta;
	if (c->time>current.time)
	{
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

void camera_animate_hermite(Camera *c, float delta)
{
	CamPoint current,next,next2,prev;
	int next_index, next2_index, prev_index;
	float s;

	VECTOR3F pos_t1,target_t1,up_t1, pos_t2,target_t2,up_t2;

	if (array_count(c->points)==0) return;

	array_item(c->points,c->current_index,&current);
	c->time+=delta;
	
	if (c->time>current.time)
	{
		c->time=0;
		c->current_index++;
		if (c->current_index>=array_count(c->points)) c->current_index=0;
		array_item(c->points,c->current_index,&current);
	};

	next_index=c->current_index+1;
	if (next_index>=array_count(c->points)) next_index=0;

	next2_index=next_index+1;
	if (next2_index>=array_count(c->points)) next2_index=0;

	prev_index=c->current_index-1;
	if (prev_index<0) prev_index=array_count(c->points)-1;

	array_item(c->points,next_index,&next);
	array_item(c->points,next2_index,&next2);
	array_item(c->points,prev_index,&prev);

	s=c->time/current.time;

	pos_t1=VECTOR3F_sub(next.pos,current.pos);
	target_t1=VECTOR3F_sub(next.target,current.target);
	up_t1=VECTOR3F_sub(next.up,current.up);
	pos_t2=VECTOR3F_sub(next2.pos,next.pos);
	target_t2=VECTOR3F_sub(next2.target,next.target);
	up_t2=VECTOR3F_sub(next2.up,next.up);

	VECTOR3F_hermite(&c->pos,&current.pos,&pos_t1,&next.pos,&pos_t2,s);
	VECTOR3F_hermite(&c->target,&current.target,&target_t1,&next.target,&target_t2,s);
	VECTOR3F_hermite(&c->up,&current.up,&up_t1,&next.up,&up_t2,s);

	float_lerp(&c->fov,&current.fov,&next.fov,s);
}

void camera_animate(Camera *c, float delta)
{
	camera_animate_hermite(c,delta);
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
