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

# define refresh_hax 0
#undef countof
#define countof(x) (sizeof((x))/sizeof((*x)))

#include "xlockmore.h"
#include "hax_scene.h"
#include "hax_camera.h"
#include <ctype.h>

#ifdef USE_GL /* whole file */

Scene *scene=NULL;
static char *cl_scene_spec;

typedef struct tagHaxInfo
{
	GLXContext *glx_context;
	char *scene_spec;
} HaxInfo;

static HaxInfo *hax_info=NULL;

static XrmOptionDescRec opts[] =
{
	{ "-scene", ".scene", XrmoptionSepArg, "don't know why this field is needed" },
};

static argtype vars[] =
{
	{&cl_scene_spec, "scene",  "Scene specification",  "test 1 17 0.3 0x808080 1", t_String},
};

ENTRYPOINT ModeSpecOpt hax_opts = {countof(opts), opts, countof(vars), vars, NULL};

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

ENTRYPOINT void reshape_hax (ModeInfo *mi, int width, int height)
{
	GLfloat h = (GLfloat) height / (GLfloat) width;

	glViewport (0, 0, (GLint) width, (GLint) height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective (30.0, 1/h, 1.0, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt( 0.0, 0.0, 30.0,
			0.0, 0.0, 0.0,
			0.0, 1.0, 0.0);

	camera_render();
	glClear(GL_COLOR_BUFFER_BIT);
}

ENTRYPOINT Bool hax_handle_event (ModeInfo *mi, XEvent *event)
{
	/*HaxInfo *hi=&hax_info[MI_SCREEN(mi)];*/
	return False;
}


ENTRYPOINT void init_hax (ModeInfo *mi)
{

	HaxInfo *hi;
	
	if (hax_info==NULL)
	{
		hax_info=(HaxInfo*)calloc(MI_NUM_SCREENS(mi),sizeof(HaxInfo));
		if (hax_info==NULL) error_exit("out of memory");
	};

	hi=&hax_info[MI_SCREEN(mi)];
	hi->scene_spec=cl_scene_spec;
	hi->glx_context=init_GL(mi);


	reshape_hax (mi, MI_WIDTH(mi), MI_HEIGHT(mi));

 	scene=scene_create(cl_scene_spec);
	start_time=current_time();
}


ENTRYPOINT void draw_hax (ModeInfo *mi)
{
	HaxInfo *hi=&hax_info[MI_SCREEN(mi)];
	Display *display = MI_DISPLAY(mi);
	Window window = MI_WINDOW(mi);
	double cur_time;

	if (!hi->glx_context) return;

	glXMakeCurrent(MI_DISPLAY(mi), MI_WINDOW(mi), *(hi->glx_context));

	cur_time=current_time()-start_time;
	scene_animate(scene,cur_time-last_drawn_time);
	scene_render(scene);
	last_drawn_time=cur_time;

	if (mi->fps_p) do_fps (mi);
	glFinish();
	glXSwapBuffers(display, window);
}

ENTRYPOINT void release_hax (ModeInfo *mi)
{
	scene_free(scene);
	free(cl_scene_spec);
	if (hax_info) free(hax_info);
	hax_info=NULL;
}

XSCREENSAVER_MODULE ("Hax", hax)

#endif /* USE_GL */
