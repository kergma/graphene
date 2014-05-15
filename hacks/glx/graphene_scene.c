#ifdef HAVE_CONFIG_H
# include "config.h"
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
	RandFloat cell_size;
	int bgcount;
	RandColor bgcolor;
	int wave_count,i,j,k;
	RandVector source;
	RandFloat amplitude, length, period;
	RandInt replicate,wave_color_replicate;
	Array *waves;
	GRID_WAVE wave;
	int cam_count=0;
	RandVector pos,target,up;
	RandFloat fov,time;
	int color_count;
	Array *colors;
	RandFloat contrast,wave_contrast;
	RandColor color1,color2;
	ColorPoint color;

	char *scene_name=NULL;

	if (!s) error_exit("out of memory");
	memset(s,0,sizeof(Scene));

	parse_spec(&ss,&version,"i","version",1);
	parse_spec(&ss,&flags,"x","flags",1);
	parse_spec(&ss,&map_size,"ri","map size",1);
	parse_spec(&ss,&cell_size,"rf","cell size",1);

	s->bganimation.points=array_create(sizeof(BgPoint));
	parse_spec(&ss,&bgcount,"i","bg count",1);
	for (i=0;i<bgcount;i++)
	{
		parse_spec(&ss,&bgcolor,"rc","bgcolor",1);
		parse_spec(&ss,&time,"rf","bgcolor time",1);
		parse_spec(&ss,&replicate,"ri","bgcolor replicate",1);
		for (j=0;j<RandInt_value(&replicate)+1;j++)
			scene_add_bgcolor(s,RandColor_value(&bgcolor),RandFloat_value(&time));
	};

	parse_spec(&ss,&contrast,"rf","grid color contrast",1);
	colors=array_create(sizeof(ColorPoint));
	parse_spec(&ss,&color_count,"i","grid color count",1);
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
	};

	waves=array_create(sizeof(GRID_WAVE));
	parse_spec(&ss,&wave_count,"i","wave count",1);
	for (i=0;i<wave_count;i++)
	{
		parse_spec(&ss,&source,"rv","wave source",1);
		parse_spec(&ss,&amplitude,"rf","wave amplitude",1);
		parse_spec(&ss,&length,"rf","wave length",1);
		parse_spec(&ss,&period,"rf","wave period",1);
		parse_spec(&ss,&wave_contrast,"rf","wave color contrast",1);
		parse_spec(&ss,&color_count,"i","wave color count",1);
		for (j=0;j<color_count;j++)
		{
			parse_spec(&ss,&color1,"rc","wave color1",1);
			parse_spec(&ss,&color2,"rc","wave color2",1);
			parse_spec(&ss,&time,"rf","wave color time",1);
			parse_spec(&ss,&wave_color_replicate,"ri","wave color replicate",1);
		};
		parse_spec(&ss,&replicate,"ri","wave replicate",1);
		for (j=0;j<RandInt_value(&replicate)+1;j++)
		{
			memset(&wave,0,sizeof(wave));
			wave.source=RandVector_value(&source);
			wave.amplitude=RandFloat_value(&amplitude);
			wave.length=RandFloat_value(&length);
			wave.period=RandFloat_value(&period);
			wave.contrast=RandFloat_value(&wave_contrast);
			wave.color_animation.points=array_create(sizeof(ColorPoint));
			for (k=0;k<RandInt_value(&wave_color_replicate)+1;k++)
			{
				color.color1=RandColor_value(&color1);
				color.color2=RandColor_value(&color2);
				color.time=RandFloat_value(&time);
				array_add(wave.color_animation.points,&color);
			};
			array_add(waves,&wave);
		};
	};


	s->camera=camera_create();

	parse_spec(&ss,&cam_count,"i","camera count",1);
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

	};
	
	parse_spec(&ss,&scene_name,"s","scene name",0);

	s->map=map_create();
	map_create_hex(s->map,RandInt_value(&map_size));

	s->grid=grid_create(s->map,RandFloat_value(&cell_size),waves,colors,RandFloat_value(&contrast));
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
