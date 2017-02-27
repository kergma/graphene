#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_COCOA
# include <OpenGL/glu.h>
#else
# include <GL/glu.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "graphene_scene.h"
#include "graphene_geometry.h"
#include "graphene_util.h"

void animate_bg(void *context, void *current, void *next, float s);

void scene_add_bgcolor(Scene *s, unsigned int bgcolor, float time);
void scene_add_bgcolor(Scene *s, unsigned int bgcolor, float time)
{
	BgPoint bg;
	bg.time=time;
	bg.a=(float)(bgcolor>>24&0xff)/255.f;
	bg.b=(float)(bgcolor>>16&0xff)/255.f;
	bg.g=(float)(bgcolor>>8&0xff)/255.f;
	bg.r=(float)(bgcolor&0xff)/255.f;
	
	array_add(s->bganimation.points,&bg);
}

#define SF_STATIC 0x1
#define SF_PLAIN 0x2
Scene *scene_create(char *spec)
{
	Scene *s=NEW(Scene);
	char *ss=spec;
	int version=1;
	unsigned int flags;
	RandInt map_size;
	int _map_size;
	RandFloat cell_size;
	float _cell_size;
	VECTOR3F clip_rect[2];
	int bgcount;
	RandColor bgcolor;
	int wave_count,i,j;
	RandVector source;
	RandFloat amplitude, length, period;
	RandInt replicate;
	Array *waves;
	GRID_WAVE wave;
	int cam_count=0;
	RandVector pos,target,up;
	RandFloat fov,time;
	int color_count;
	Array *colors;
	RandFloat contrast;
	RandColor color1,color2;
	ColorPoint color;

	char *scene_name=NULL;

	if (!s) error_exit("out of memory");
	memset(s,0,sizeof(Scene));

	parse_spec(&ss,&version,"i","version",1);
	parse_spec(&ss,&flags,"x","flags",1);
	if (spec_dumper==SD_EXPLAINED) printf("\n");
	parse_spec(&ss,&map_size,"ri","map size",1);
	parse_spec(&ss,&cell_size,"rf","cell size",1);
	if (spec_dumper==SD_EXPLAINED) printf("\n");

	s->bganimation.points=array_create(sizeof(BgPoint));
	parse_spec(&ss,&bgcount,"i","bg count",1);
	if (spec_dumper==SD_EXPLAINED) printf("\n");
	for (i=0;i<bgcount;i++)
	{
		parse_spec(&ss,&bgcolor,"rc","bgcolor",1);
		parse_spec(&ss,&time,"rf","bgcolor time",1);
		parse_spec(&ss,&replicate,"ri","bgcolor replicate",1);
		for (j=0;j<RandInt_value(&replicate)+1;j++)
			scene_add_bgcolor(s,RandColor_value(&bgcolor),RandFloat_value(&time));
		if (spec_dumper==SD_EXPLAINED) printf("\n");
	};

	parse_spec(&ss,&contrast,"rf","grid color contrast",1);
	colors=array_create(sizeof(ColorPoint));
	parse_spec(&ss,&color_count,"i","grid color count",1);
	if (spec_dumper==SD_EXPLAINED) printf("\n");
	for (i=0;i<color_count;i++)
	{
		parse_spec(&ss,&color1,"rc","grid color1",1);
		parse_spec(&ss,&color2,"rc","grid color2",1);
		parse_spec(&ss,&time,"rf","grid color time",1);
		parse_spec(&ss,&replicate,"ri","grid color replicate",1);
		for (j=0;j<RandInt_value(&replicate)+1;j++)
		{
			color.color1=RandColor_value(&color1);
			color.color2=RandColor_value(&color2);
			color.time=RandFloat_value(&time);
			array_add(colors,&color);
		};
		if (spec_dumper==SD_EXPLAINED) printf("\n");
	};

	waves=array_create(sizeof(GRID_WAVE));
	parse_spec(&ss,&wave_count,"i","wave count",1);
	if (spec_dumper==SD_EXPLAINED) printf("\n");
	for (i=0;i<wave_count;i++)
	{
		parse_spec(&ss,&source,"rv","wave source",1);
		parse_spec(&ss,&amplitude,"rf","wave amplitude",1);
		parse_spec(&ss,&length,"rf","wave length",1);
		parse_spec(&ss,&period,"rf","wave period",1);
		parse_spec(&ss,&replicate,"ri","wave replicate",1);
		for (j=0;j<RandInt_value(&replicate)+1;j++)
		{
			memset(&wave,0,sizeof(wave));
			wave.source=RandVector_value(&source);
			wave.amplitude=RandFloat_value(&amplitude);
			wave.length=RandFloat_value(&length);
			wave.period=RandFloat_value(&period);
			array_add(waves,&wave);
		};
		if (spec_dumper==SD_EXPLAINED) printf("\n");
	};


	s->camera=camera_create();

	parse_spec(&ss,&cam_count,"i","camera count",1);
	if (spec_dumper==SD_EXPLAINED) printf("\n");
	for (i=0;i<cam_count;i++)
	{
		parse_spec(&ss,&pos,"rv","camera pos",1);
		parse_spec(&ss,&target,"rv","camera target",1);
		parse_spec(&ss,&up,"rv","camera up",1);
		parse_spec(&ss,&fov,"rf","camera fov",1);
		parse_spec(&ss,&time,"rf","camera time",1);
		parse_spec(&ss,&replicate,"ri","camera replicate",1);
		for (j=0;j<RandInt_value(&replicate)+1;j++)
			camera_add_point(s->camera,RandVector_value(&pos),RandVector_value(&target),RandVector_value(&up),RandFloat_value(&fov),RandFloat_value(&time));
		if (spec_dumper==SD_EXPLAINED) printf("\n");
	};
	
	if (spec_dumper==SD_MINIFIED) printf("\n");
	if (spec_dumper!=SD_NONE) fflush(0);

	s->map=map_create();
	_map_size=RandInt_value(&map_size);
	_cell_size=RandFloat_value(&cell_size);
	if (_map_size>0)
		map_create_hex(s->map,_map_size);
	else if (_map_size==0)
	{
		camera_calculate_cliprect(s->camera,clip_rect);
		map_create_rect(s->map,clip_rect,_cell_size);
	};

	s->grid=grid_create(s->map,_cell_size,waves,colors,RandFloat_value(&contrast));
	free(scene_name);
	
	array_free(colors);
	array_free(waves);

	if ((flags&SF_PLAIN)!=0)
		glDisable(GL_DEPTH_TEST);
	else
		glEnable(GL_DEPTH_TEST);

	s->enable_grid_animation=!(flags&SF_STATIC);
	if (!s->enable_grid_animation)
	{
		animate_point2(&s->bganimation,0,&animate_bg,s);
		grid_animate(s->grid,0);
	};

	return s;
}

int scene_render(Scene *s)
{
	glClearColor(s->clr,s->clg,s->clb,s->cla);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	camera_render(s->camera);
	grid_render(s->grid);

	return 0;
}

int scene_free(Scene *s)
{
	camera_free(s->camera);
	grid_free(s->grid);
	map_free(s->map);
	array_free(s->bganimation.points);
	free(s);
	return 0;
}

void animate_bg(void *context, void *current, void *next, float s)
{
	Scene *scene=(Scene*)context;
	BgPoint *c=(BgPoint*)current;
	BgPoint *n=(BgPoint*)next;

	float_lerp(&scene->clr,&c->r,&n->r,s);
	float_lerp(&scene->clg,&c->g,&n->g,s);
	float_lerp(&scene->clb,&c->b,&n->b,s);
	float_lerp(&scene->cla,&c->a,&n->a,s);
}
void scene_animate(Scene *s, float delta)
{
	animate_point2(&s->bganimation,delta,&animate_bg,s);
	camera_animate(s->camera,delta);
	if (s->enable_grid_animation)
		grid_animate(s->grid,delta);
}

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

void screen_to_xz(GLdouble wx, GLdouble wy, const GLdouble *model, const GLdouble *proj, const GLint *view, float *x, float *z);
void screen_to_xz(GLdouble wx, GLdouble wy, const GLdouble *model, const GLdouble *proj, const GLint *view, float *x, float *z)
{
	GLdouble x1,y1,z1, x2,y2,z2;
	gluUnProject(wx,wy,100000,model,proj,view,&x1,&y1,&z1);
	gluUnProject(wx,wy,-100000,model,proj,view,&x2,&y2,&z2);
	*x=x1-y1*(x2-x1)/(y2-y1);
	*z=z1-y1*(z2-z1)/(y2-y1);
}

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
void camera_calculate_cliprect(Camera *c, VECTOR3F *clip_rect)
{
	int i;
	CamPoint p;
	GLint view[4];
	GLdouble proj[16], model[16];
	float x,z;
	glGetIntegerv(GL_VIEWPORT,view);
	camera_set_viewport(c,view[2]-view[0],view[3]-view[1]);
	for (i=0;i<array_count(c->points);i++)
	{
		array_item(c->points,i,&p);
		c->pos=p.pos;
		c->target=p.target;
		c->up=p.up;
		c->fov=p.fov;
		camera_render(c);
		glGetDoublev(GL_PROJECTION_MATRIX,proj);
		glGetDoublev(GL_MODELVIEW_MATRIX,model);
		screen_to_xz(view[0],view[1],model,proj,view,&x,&z);
		clip_rect[0].x=MIN(x,clip_rect[0].x); clip_rect[0].z=MIN(z,clip_rect[0].z);
		clip_rect[1].x=MAX(x,clip_rect[1].x); clip_rect[1].z=MAX(z,clip_rect[1].z);
		screen_to_xz(view[0],view[3],model,proj,view,&x,&z);
		clip_rect[0].x=MIN(x,clip_rect[0].x); clip_rect[0].z=MIN(z,clip_rect[0].z);
		clip_rect[1].x=MAX(x,clip_rect[1].x); clip_rect[1].z=MAX(z,clip_rect[1].z);
		screen_to_xz(view[2],view[3],model,proj,view,&x,&z);
		clip_rect[0].x=MIN(x,clip_rect[0].x); clip_rect[0].z=MIN(z,clip_rect[0].z);
		clip_rect[1].x=MAX(x,clip_rect[1].x); clip_rect[1].z=MAX(z,clip_rect[1].z);
		screen_to_xz(view[2],view[1],model,proj,view,&x,&z);
		clip_rect[0].x=MIN(x,clip_rect[0].x); clip_rect[0].z=MIN(z,clip_rect[0].z);
		clip_rect[1].x=MAX(x,clip_rect[1].x); clip_rect[1].z=MAX(z,clip_rect[1].z);
	};
}
