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

int parse_spec(char **pos, void *out, char type);
int parse_spec(char **pos, void *out, char type)
{
#define IS_DIGIT(c) (((c)>='0' && (c)<='9') || (c)=='-' || (c)=='.')
#define IS_DELIMITER(c) ((c)==' ' || (c)==';' || (c)==':' || (c)=='\r' || (c)=='\n')

	char *tm1=calloc(strlen(*pos)+1,sizeof(char));
	char *tm2=tm1;
	if (type=='i' || type=='f')
	{
		while (**pos && !IS_DIGIT(**pos)) (*pos)++;
		while (**pos && IS_DIGIT(**pos))
		{
			*tm2=**pos;
			(*pos)++;
			tm2++;
		};
	};
	if (type=='s')
	{
		while (**pos && IS_DELIMITER(**pos)) (*pos)++;
		while (**pos && !IS_DIGIT(**pos))
		{
			*tm2=**pos;
			(*pos)++;
			tm2++;
		};
		while (tm2>tm1 && IS_DELIMITER(**pos))
		{
			(*pos)--;
			tm2--;
		};
	};
	*tm2=0;

	if (*tm1==0)
	{
		free(tm1);
		return 0;
	};
	if (type=='i') *((int*)out)=atoi(tm1);
	if (type=='f') *((float*)out)=atof(tm1);
	if (type=='s')
	{
		*((char**)out)=(char*)calloc(strlen(tm1)+1,sizeof(char));
		strcpy(*((char**)out),tm1);
	};
	free(tm1);
	return 1;
#undef IS_NUMBER
}


Scene *scene_create(char *spec)
{
	Scene *s=NEW(Scene);
	char *scene_name=NULL;
	int map_size;
	float cell_size;
	char *ss=spec;

	if (!s) error_exit("out of memory");

	if (!parse_spec(&ss,&map_size,'i')) error_exit("error reading scene specificatin");
	printf("map_size %d\n",map_size);
	if (!parse_spec(&ss,&cell_size,'f')) error_exit("error reading scene specificatin");
	printf("cell_size %f\n",cell_size);
	parse_spec(&ss,&scene_name,'s');
	if (scene_name) printf("scene_name %s\n",scene_name);

	s->map=map_create();
	map_create_hex(s->map,map_size);

	s->grid=grid_create(s->map,cell_size);
	free(scene_name);

	return s;
}

int scene_render(Scene *s)
{
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
