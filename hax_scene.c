#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef HAVE_COCOA
# include <OpenGL/gl.h>
#else
# include <GL/gl.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hax_scene.h"
#include "hax_map.h"
#include "hax_camera.h"
#include "hax_grid.h"

int read_atomic_value(char **pos, void *out, char type);
int read_atomic_value(char **pos, void *out, char type)
{

	char *tm1=calloc(strlen(*pos)+1,sizeof(char));
	char *tm2=tm1;

	char *digits="0123456789.-";
	char *whitespace=" \t\n\r";

	if (type=='i') digits="0123456789-";
	if (type=='f') digits="0123456789.-";
	if (type=='x') digits="0123456789abcdefABCDEFx";
	if (type=='s') digits="0123456789-.";

	if (type=='x')
	{
		while (**pos && strstr(*pos,"0x")!=*pos) (*pos)++;
	};
	if (type=='i' || type=='f' || type=='x')
	{
		while (**pos && !strchr(digits,**pos)) (*pos)++;
		while (**pos && strchr(digits,**pos))
		{
			*tm2=**pos;
			(*pos)++;
			tm2++;
		};
	};
	if (type=='s')
	{
		while (**pos && strchr(whitespace,**pos)) (*pos)++;
		while (**pos && !strchr(digits,**pos))
		{
			*tm2=**pos;
			(*pos)++;
			tm2++;
		};
		while (tm2>tm1 && strchr(whitespace,**pos))
		{
			(*pos)--;
			tm2--;
		};
		tm2++;
	};
	*tm2=0;

	if (*tm1==0)
	{
		free(tm1);
		return 0;
	};
	if (type=='i') *((int*)out)=atoi(tm1);
	if (type=='f') *((float*)out)=atof(tm1);
	if (type=='x') *((unsigned int*)out)=strtoul(tm1,NULL,16);
	if (type=='s')
	{
		*((char**)out)=(char*)calloc(strlen(tm1)+1,sizeof(char));
		strcpy(*((char**)out),tm1);
	};
	free(tm1);
	return 1;
}

int read_value(char **pos, void *out, char *type);
int read_value(char **pos, void *out, char *type)
{
	int ai,bi;
	float af,bf;
	if (!strcmp(type,"i") || !strcmp(type,"f") || !strcmp(type,"x") || !strcmp(type,"s"))
		return read_atomic_value(pos,out,*type);
	if (!strcmp(type,"ri"))
	{
		if (!read_atomic_value(pos,&ai,'i')) return 0;
		if (**pos==':')
		{
			if (!read_atomic_value(pos,&bi,'i')) return 0;
			*((RandInt*)out)=RandInt_c2(ai,bi);
		}
		else
			*((RandInt*)out)=RandInt_c1(ai);

	};
	if (!strcmp(type,"rf"))
	{
		if (!read_atomic_value(pos,&af,'f')) return 0;
		if (**pos==':')
		{
			if (!read_atomic_value(pos,&bf,'f')) return 0;
			*((RandFloat*)out)=RandFloat_c2(af,bf);
		}
		else
			*((RandFloat*)out)=RandFloat_c1(af);

	};
	return 1;
}

void parse_spec(char **pos, void *out, char *type, char *key, int required);
void parse_spec(char **pos, void *out, char *type, char *key, int required)
{
	char buf[512];
	int read=read_value(pos,out,type);
	if (!read && required) 
	{
		snprintf(buf,512,"error reading scene specification, expecting: %s",key);
		error_exit(buf);
	};
	if (!read) return;
	if (!strcmp(type,"i")) snprintf(buf,512,"%d",*((int*)out));
	if (!strcmp(type,"ri") && ((RandInt*)out)->random) snprintf(buf,512,"%d:%d",((RandInt*)out)->a,((RandInt*)out)->b);
	if (!strcmp(type,"ri") && !((RandInt*)out)->random) snprintf(buf,512,"%d",((RandInt*)out)->a);
	if (!strcmp(type,"f")) snprintf(buf,512,"%f",*((float*)out));
	if (!strcmp(type,"rf") && ((RandFloat*)out)->random) snprintf(buf,512,"%f:%f",((RandFloat*)out)->a,((RandFloat*)out)->b);
	if (!strcmp(type,"rf") && !((RandFloat*)out)->random) snprintf(buf,512,"%f",((RandFloat*)out)->a);
	if (!strcmp(type,"x")) snprintf(buf,512,"0x%x",*((unsigned int*)out));
	if (!strcmp(type,"s")) snprintf(buf,512,"%s",*(char**)out);
	printf("%s: %s\n",key,buf);
}

Scene *scene_create(char *spec)
{
	Scene *s=NEW(Scene);
	char *scene_name=NULL;
	int version=1;
	RandInt map_size;
	RandFloat cell_size;
	unsigned int bgcolor=0x121212;
	int wave_count,i;
	Array *waves;
	RandVector source;
	RandFloat amplitude, length, period;
	RandInt replicate;
	char *ss=spec;

	if (!s) error_exit("out of memory");

	parse_spec(&ss,&version,"i","version",1);
	parse_spec(&ss,&map_size,"ri","map size",1);
	parse_spec(&ss,&cell_size,"rf","cell size",1);
	parse_spec(&ss,&bgcolor,"x","bgcolor",1);
	s->cla=(float)(bgcolor>>24&0xff)/255.f;
	s->clr=(float)(bgcolor>>16&0xff)/255.f;
	s->clg=(float)(bgcolor>>8&0xff)/255.f;
	s->clb=(float)(bgcolor&0xff)/255.f;

	waves=array_create(sizeof(GRID_WAVE));
	parse_spec(&ss,&wave_count,"i","wave count",1);
	for (i=0;i<wave_count;i++)
	{
	};
	
	parse_spec(&ss,&scene_name,"s","scene name",0);

	s->map=map_create();
	map_create_hex(s->map,RandInt_value(&map_size));

	s->grid=grid_create(s->map,RandFloat_value(&cell_size));
	free(scene_name);
	
	array_free(waves);

	return s;
}

int scene_render(Scene *s)
{
	glClearColor(s->clr,s->clg,s->clb,s->cla);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	camera_render();
	grid_render(s->grid);

	return 0;
}

int scene_free(Scene *s)
{
	grid_free(s->grid);
	map_free(s->map);
	free(s);
	return 0;
}

void scene_animate(Scene *s, float delta)
{
	grid_animate(s->grid,delta);
}
