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
		while (tm2>tm1 && strchr(whitespace,*(tm2-1))) tm2--;
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
	if (!strcmp(type,"v"))
		return read_atomic_value(pos,&((VECTOR3F*)out)->x,'f') && 
			read_atomic_value(pos,&((VECTOR3F*)out)->y,'f') && 
			read_atomic_value(pos,&((VECTOR3F*)out)->z,'f');
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
	if (!strcmp(type,"rv"))
		return read_value(pos,&((RandVector*)out)->x,"rf") &&
			read_value(pos,&((RandVector*)out)->y,"rf") &&
			read_value(pos,&((RandVector*)out)->z,"rf");
	return 1;
}

int snprintf_ri(char *str, size_t size, RandInt v);
int snprintf_ri(char *str, size_t size, RandInt v)
{
	if (v.random)
		return snprintf(str,size,"%d:%d",v.a,v.b);
	return snprintf(str,size,"%d",v.a);
}
int snprintf_rf(char *str, size_t size, RandFloat v);
int snprintf_rf(char *str, size_t size, RandFloat v)
{
	if (v.random)
		return snprintf(str,size,"%f:%f",v.a,v.b);
	return snprintf(str,size,"%f",v.a);
}
int snprintf_rv(char *str, size_t size, RandVector v);
int snprintf_rv(char *str, size_t size, RandVector v)
{
	char x[32],y[32],z[32];
	snprintf_rf(x,32,v.x);
	snprintf_rf(y,32,v.y);
	snprintf_rf(z,32,v.z);
	return snprintf(str,size,"%s, %s, %s",x,y,z);
}

void parse_spec(char **pos, void *out, char *type, char *key, int required);
void parse_spec(char **pos, void *out, char *type, char *key, int required)
{
	char buf[512];
	int read;
	read=read_value(pos,out,type);
	if (!read && required) 
	{
		snprintf(buf,512,"error reading scene specification, expecting: %s",key);
		error_exit(buf);
	};
	if (!read) return;
	if (!strcmp(type,"i")) snprintf(buf,512,"%d",*((int*)out));
	if (!strcmp(type,"ri")) snprintf_ri(buf,512,*((RandInt*)out));
	if (!strcmp(type,"f")) snprintf(buf,512,"%f",*((float*)out));
	if (!strcmp(type,"rf")) snprintf_rf(buf,512,*((RandFloat*)out));
	if (!strcmp(type,"x")) snprintf(buf,512,"0x%x",*((unsigned int*)out));
	if (!strcmp(type,"s")) snprintf(buf,512,"%s",*(char**)out);
	if (!strcmp(type,"v")) snprintf(buf,512,"%f, %f, %f",((VECTOR3F*)out)->x,((VECTOR3F*)out)->y,((VECTOR3F*)out)->z);
	if (!strcmp(type,"rv")) snprintf_rv(buf,512,*((RandVector*)out));
	printf("%s: %s\n",key,buf);
}

Scene *scene_create(char *spec)
{
	Scene *s=NEW(Scene);
	char *ss=spec;
	int version=1;
	RandInt map_size;
	RandFloat cell_size;
	unsigned int bgcolor=0x121212;
	int wave_count,i,j;
	RandVector source;
	RandFloat amplitude, length, period;
	RandInt replicate;
	Array *waves;
	GRID_WAVE wave;
	char *scene_name=NULL;

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
		parse_spec(&ss,&source,"rv","wave source",1);
		parse_spec(&ss,&amplitude,"rf","wave amplitude",1);
		parse_spec(&ss,&length,"rf","wave length",1);
		parse_spec(&ss,&period,"rf","wave period",1);
		parse_spec(&ss,&replicate,"ri","wave replicate",1);
		for (j=0;j<RandInt_value(&replicate)+1;j++)
		{
			wave.source=RandVector_value(&source);
			wave.amplitude=RandFloat_value(&amplitude);
			wave.length=RandFloat_value(&length);
			wave.period=RandFloat_value(&period);
			array_add(waves,&wave);
		};
	};
	
	parse_spec(&ss,&scene_name,"s","scene name",0);

	s->map=map_create();
	map_create_hex(s->map,RandInt_value(&map_size));

	s->grid=grid_create(s->map,waves,RandFloat_value(&cell_size));
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
