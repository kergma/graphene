/* dangerball, Copyright (c) 2001-2008 Jamie Zawinski <jwz@jwz.org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  No representations are made about the suitability of this
 * software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 */

#define DEFAULTS	"*delay:	30000       \n" \
			"*count:        30          \n" \
			"*showFPS:      False       \n" \
			"*wireframe:    False       \n" \

# define refresh_graphene 0
#undef countof
#define countof(x) (sizeof((x))/sizeof((*x)))

#include "xlockmore.h"
#include "graphene_scene.h"
#include <ctype.h>

#ifdef USE_GL /* whole file */

Scene *scene=NULL;
static char *cl_scene_spec;
static char *cl_spec_dumper;


typedef struct tagGrapheneInfo
{
	GLXContext *glx_context;
	char *scene_spec;
} GrapheneInfo;

static GrapheneInfo *graphene_info=NULL;

static XrmOptionDescRec opts[] =
{
	{ "-scene", ".scene", XrmoptionSepArg, NULL },
	{ "-dump", ".dump", XrmoptionSepArg, NULL },
};

static argtype vars[] =
{
	{&cl_scene_spec, "scene",  "Scene specification",  "random", t_String},
	{&cl_spec_dumper, "dump",  "Dump scene specification",  "none", t_String},
};

ENTRYPOINT ModeSpecOpt graphene_opts = {countof(opts), opts, countof(vars), vars, NULL};

static double start_time=0.0;
static double last_drawn_time=0.0;

static double current_time(void)
{
	struct timeval tv;
# ifdef GETTIMEOFDAY_TWO_ARGS
	struct timezone tzp;
	gettimeofday(&tv, &tzp);
# else
	gettimeofday(&tv);
# endif

	return (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
}

ENTRYPOINT void reshape_graphene (ModeInfo *mi, int width, int height)
{
	glViewport(0, 0, width, height);
	camera_set_viewport(scene->camera,width,height);
	camera_render(scene->camera);
}

ENTRYPOINT Bool graphene_handle_event (ModeInfo *mi, XEvent *event)
{
	/*GrapheneInfo *gi=&graphene_info[MI_SCREEN(mi)];*/
	return False;
}

char *read_scene_spec(char *s);

ENTRYPOINT void init_graphene (ModeInfo *mi)
{

	GrapheneInfo *gi;
	
	if (graphene_info==NULL)
	{
		graphene_info=(GrapheneInfo*)calloc(MI_NUM_SCREENS(mi),sizeof(GrapheneInfo));
		if (graphene_info==NULL) error_exit("out of memory");
	};

	init_fast_math();
	gi=&graphene_info[MI_SCREEN(mi)];
	gi->scene_spec=cl_scene_spec;
	gi->glx_context=init_GL(mi);

	if (!strcmp(cl_spec_dumper,"minified")) spec_dumper=SD_MINIFIED;
	if (!strcmp(cl_spec_dumper,"explained")) spec_dumper=SD_EXPLAINED;
	scene=scene_create(read_scene_spec(cl_scene_spec));
	start_time=current_time();
	reshape_graphene (mi, MI_WIDTH(mi), MI_HEIGHT(mi));
}


ENTRYPOINT void draw_graphene (ModeInfo *mi)
{
	GrapheneInfo *gi=&graphene_info[MI_SCREEN(mi)];
	Display *display = MI_DISPLAY(mi);
	Window window = MI_WINDOW(mi);
	double cur_time;

	if (!gi->glx_context) return;

	glXMakeCurrent(MI_DISPLAY(mi), MI_WINDOW(mi), *(gi->glx_context));

	cur_time=current_time()-start_time;
	scene_animate(scene,cur_time-last_drawn_time);
	scene_render(scene);
	last_drawn_time=cur_time;

	if (mi->fps_p) do_fps (mi);
	glFinish();
	glXSwapBuffers(display, window);
}

ENTRYPOINT void release_graphene (ModeInfo *mi)
{
	scene_free(scene);
	free(cl_scene_spec);
	if (graphene_info) free(graphene_info);
	graphene_info=NULL;
}

XSCREENSAVER_MODULE ("Graphene", graphene)

typedef struct tagSCENE_SPEC {
	char *name;
	char *spec;
} SCENE_SPEC;


SCENE_SPEC scenes[]={
	{"demo", "1 0x0 17 0.2 bg 1 #888 10 0 color 1 1 #fff #fff 10 0 wav 1 0 0 0 0.1 1 1 wc 1 0 0 cam 1 3 3 3 0 0 0 0 1 0 45 10 0"},
	{"curves","1 0x0 55 50 1 #ff000080 10 0 11 1 #ff000000 #ffffffff 10 0 2 -3000:3000 0 -3000:3000 10 1000:2000 1:4 1 0 0 0 0 0 20:50 300:1000 1:5 1 0  1 8 0 3000 0 0 0 0 0 0 1 90 30 0 0 3000 0 0 0 0 0 0 1 90 1 0 0 2000 0 0 0 0 1 0 0 90 30 0 0 2000 0 0 0 0 1 0 0 90 1 0 0 2500 0 0 0 0 0 0 1 90 30 0 0 2500 0 0 0 0 0 0 1 90 1 0 1000 2000 0 1000 0 0 -1 0 0 90 30 0 1000 2000 0 1000 0 0 -1 0 0 90 1 0"},
};

char *read_scene_spec(char *s)
{
	int i=0;

	if (!strcmp(s,"random")) return scenes[(sizeof(scenes)/sizeof(scenes[0]))*(long int)random()/RAND_MAX].spec;
	for (i=0;i<sizeof(scenes)/sizeof(scenes[0]);i++)
	{
		if (!strcmp(scenes[i].name,s)) return scenes[i].spec;
	};
	return s;
}

#endif /* USE_GL */
