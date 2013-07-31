#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef HAVE_COCOA
# include <OpenGL/gl.h>
#else
# include <GL/gl.h>
#endif

#include <stdlib.h>
#include <string.h>
#include "hax_scene.h"
#include "hax_map.h"
#include "hax_camera.h"
#include "hax_grid.h"

int read_value(char **pos, void *out, char type);
int read_value(char **pos, void *out, char type)
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

int parse_spec(char **pos, void *out, char *type);
int parse_spec(char **pos, void *out, char *type)
{
	int ai,bi;
	float af,bf;
	if (!strcmp(type,"i") || !strcmp(type,"f") || !strcmp(type,"x") || !strcmp(type,"s"))
		return read_value(pos,out,*type);
	if (!strcmp(type,"ri"))
	{
		if (!read_value(pos,&ai,'i')) return 0;
		if (**pos==':')
		{
			if (!read_value(pos,&bi,'i')) return 0;
			*((RandInt*)out)=RandInt_c2(ai,bi);
		}
		else
			*((RandInt*)out)=RandInt_c1(ai);

	};
	if (!strcmp(type,"rf"))
	{
		if (!read_value(pos,&af,'f')) return 0;
		if (**pos==':')
		{
			if (!read_value(pos,&bf,'f')) return 0;
			*((RandFloat*)out)=RandFloat_c2(af,bf);
		}
		else
			*((RandFloat*)out)=RandFloat_c1(af);

	};
	return 1;
}

Scene *scene_create(char *spec)
{
	Scene *s=NEW(Scene);
	char *scene_name=NULL;
	int version=1;
	RandInt map_size;
	RandFloat cell_size;
	unsigned int bgcolor=0x121212;
	int wave_count;
	char *ss=spec;

	if (!s) error_exit("out of memory");

	if (!parse_spec(&ss,&version,"i")) error_exit("error reading scene specificatin");
	printf("version %d\n",version);
	if (!parse_spec(&ss,&map_size,"ri")) error_exit("error reading scene specificatin");
	printf("map_size %d:%d\n",map_size.a,map_size.b);
	if (!parse_spec(&ss,&cell_size,"rf")) error_exit("error reading scene specificatin");
	printf("cell_size %f:%f\n",cell_size.a,cell_size.b);
	if (!parse_spec(&ss,&bgcolor,"x")) error_exit("error reading scene specificatin");
	printf("bgcolor 0x%x\n",bgcolor);
	s->cla=(float)(bgcolor>>24&0xff)/255.f;
	s->clr=(float)(bgcolor>>16&0xff)/255.f;
	s->clg=(float)(bgcolor>>8&0xff)/255.f;
	s->clb=(float)(bgcolor&0xff)/255.f;
	if (!parse_spec(&ss,&wave_count,"i")) error_exit("error reading scene specificatin");
	
	
	parse_spec(&ss,&scene_name,"s");
	if (scene_name) printf("scene_name %s\n",scene_name);

	s->map=map_create();
	map_create_hex(s->map,RandInt_value(&map_size));

	s->grid=grid_create(s->map,RandFloat_value(&cell_size));
	free(scene_name);

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
