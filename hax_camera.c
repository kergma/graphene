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

Camera *camera_create(int ortho)
{
	Camera *c=NEW(Camera);
	if (!c) error_exit("out of memory");
	memset(c,0,sizeof(Camera));

	return c;
}

void camera_free(Camera *c)
{
	free(c);
}

void camera_render(Camera *c)
{

	glLoadIdentity();
	gluLookAt(3.0f,3.0f,3.0f, 0,0,0, 0,1,0.0f);
}
