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
#include "colors.h"
#include "sphere.h"
#include "tube.h"
#include "rotator.h"
#include "gltrackball.h"
#include "hax_scene.h"
#include "hax_camera.h"
#include <ctype.h>

#ifdef USE_GL /* whole file */


#define DEF_SPIN        "True"
#define DEF_WANDER      "True"
#define DEF_SPEED       "0.05"

#define SPIKE_FACES   12  /* how densely to render spikes */
#define SMOOTH_SPIKES True
#define SPHERE_SLICES 32  /* how densely to render spheres */
#define SPHERE_STACKS 16


Scene *scene=NULL;
static SceneSpec clspec;

typedef struct {
  GLXContext *glx_context;
  rotator *rot;
  trackball_state *trackball;
  Bool button_down_p;

  GLuint ball_list;
  GLuint spike_list;

  GLfloat pos;
  int *spikes;

  int ncolors;
  XColor *colors;
  int ccolor;
  int color_shift;

} ball_configuration;

static ball_configuration *bps = NULL;

static Bool do_spin;
static GLfloat speed;
static Bool do_wander;

static XrmOptionDescRec opts[] =
{
	{ "-map-size",   ".map_size",   XrmoptionSepArg, "17" },
	{ "-cell-size",  ".cell_size",  XrmoptionSepArg, "0.3" },
  { "-spin",   ".spin",   XrmoptionNoArg, "True" },
  { "+spin",   ".spin",   XrmoptionNoArg, "False" },
  { "-speed",  ".speed",  XrmoptionSepArg, 0 },
  { "-wander", ".wander", XrmoptionNoArg, "True" },
  { "+wander", ".wander", XrmoptionNoArg, "False" }
};

static argtype vars[] =
{
	{&clspec.map_size,   "map_size",   "Size of the map",   "3",   t_Int},
	{&clspec.cell_size,  "cell_size",  "Size of the cell",  "0.3", t_Float},

  {&do_spin,   "spin",   "Spin",   DEF_SPIN,   t_Bool},
  {&do_wander, "wander", "Wander", DEF_WANDER, t_Bool},
  {&speed,     "speed",  "Speed",  DEF_SPEED,  t_Float},
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


/* Window management, etc
 */
ENTRYPOINT void
reshape_hax (ModeInfo *mi, int width, int height)
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

ENTRYPOINT Bool
hax_handle_event (ModeInfo *mi, XEvent *event)
{
  ball_configuration *bp = &bps[MI_SCREEN(mi)];

  if (event->xany.type == ButtonPress &&
      event->xbutton.button == Button1)
    {
      bp->button_down_p = True;
      gltrackball_start (bp->trackball,
                         event->xbutton.x, event->xbutton.y,
                         MI_WIDTH (mi), MI_HEIGHT (mi));
      return True;
    }
  else if (event->xany.type == ButtonRelease &&
           event->xbutton.button == Button1)
    {
      bp->button_down_p = False;
      return True;
    }
  else if (event->xany.type == ButtonPress &&
           (event->xbutton.button == Button4 ||
            event->xbutton.button == Button5 ||
            event->xbutton.button == Button6 ||
            event->xbutton.button == Button7))
    {
      gltrackball_mousewheel (bp->trackball, event->xbutton.button, 10,
                              !!event->xbutton.state);
      return True;
    }
  else if (event->xany.type == MotionNotify &&
           bp->button_down_p)
    {
      gltrackball_track (bp->trackball,
                         event->xmotion.x, event->xmotion.y,
                         MI_WIDTH (mi), MI_HEIGHT (mi));
      return True;
    }

  return False;
}


ENTRYPOINT void 
init_hax (ModeInfo *mi)
{
  ball_configuration *bp;
  int wire = MI_IS_WIREFRAME(mi);

  if (!bps) {
    bps = (ball_configuration *)
      calloc (MI_NUM_SCREENS(mi), sizeof (ball_configuration));
    if (!bps) {
      fprintf(stderr, "%s: out of memory\n", progname);
      exit(1);
    }
  }

  bp = &bps[MI_SCREEN(mi)];

  bp->glx_context = init_GL(mi);
 	scene=scene_create(&clspec);
	start_time=current_time();

  reshape_hax (mi, MI_WIDTH(mi), MI_HEIGHT(mi));

  if (!wire)
    {
      GLfloat pos[4] = {1.0, 1.0, 1.0, 0.0};
      GLfloat amb[4] = {0.0, 0.0, 0.0, 1.0};
      GLfloat dif[4] = {1.0, 1.0, 1.0, 1.0};
      GLfloat spc[4] = {0.0, 1.0, 1.0, 1.0};

      glEnable(GL_LIGHTING);
      glEnable(GL_LIGHT0);
      glEnable(GL_DEPTH_TEST);
      glEnable(GL_CULL_FACE);

      glLightfv(GL_LIGHT0, GL_POSITION, pos);
      glLightfv(GL_LIGHT0, GL_AMBIENT,  amb);
      glLightfv(GL_LIGHT0, GL_DIFFUSE,  dif);
      glLightfv(GL_LIGHT0, GL_SPECULAR, spc);
    }
	glEnable(GL_COLOR_MATERIAL);

  {
    double spin_speed   = 10.0;
    double wander_speed = 0.12;
    double spin_accel   = 2.0;

    bp->rot = make_rotator (do_spin ? spin_speed : 0,
                            do_spin ? spin_speed : 0,
                            do_spin ? spin_speed : 0,
                            spin_accel,
                            do_wander ? wander_speed : 0,
                            True);
    bp->trackball = gltrackball_init ();
  }

  bp->ncolors = 128;
  bp->colors = (XColor *) calloc(bp->ncolors, sizeof(XColor));
  make_smooth_colormap (0, 0, 0,
                        bp->colors, &bp->ncolors,
                        False, 0, False);

  bp->spikes = (int *) calloc(MI_COUNT(mi), sizeof(*bp->spikes) * 2);

  bp->ball_list = glGenLists (1);
  bp->spike_list = glGenLists (1);

  glNewList (bp->ball_list, GL_COMPILE);
  unit_sphere (SPHERE_STACKS, SPHERE_SLICES, wire);
  glEndList ();

  glNewList (bp->spike_list, GL_COMPILE);
  cone (0, 0, 0,
        0, 1, 0,
        1, 0, SPIKE_FACES, SMOOTH_SPIKES, False, wire);
  glEndList ();

}


ENTRYPOINT void
draw_hax (ModeInfo *mi)
{
  ball_configuration *bp = &bps[MI_SCREEN(mi)];
  Display *dpy = MI_DISPLAY(mi);
  Window window = MI_WINDOW(mi);
	double cur_time;

  if (!bp->glx_context)
    return;

  glXMakeCurrent(MI_DISPLAY(mi), MI_WINDOW(mi), *(bp->glx_context));

  glShadeModel(GL_SMOOTH);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_NORMALIZE);
  glEnable(GL_CULL_FACE);

	glClearColor(0.5f,0.5f,0.5f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  /*glPushMatrix ();*/

  /*glScalef(1.1, 1.1, 1.1);*/


  bp->ccolor++;
  if (bp->ccolor >= bp->ncolors) bp->ccolor = 0;

  mi->polygon_count = 0;

  /*glScalef (2.0, 2.0, 2.0);*/


 	
	cur_time=current_time()-start_time;

	scene_animate(scene,cur_time-last_drawn_time);
	scene_render(scene);

	last_drawn_time=cur_time;
	

  if (mi->fps_p) do_fps (mi);
  glFinish();

  glXSwapBuffers(dpy, window);
}

ENTRYPOINT void release_hax (ModeInfo *mi)
{
	scene_free(scene);
}

XSCREENSAVER_MODULE ("Hax", hax)

#endif /* USE_GL */
