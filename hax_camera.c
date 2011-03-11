#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef HAVE_COCOA
# include <OpenGL/glu.h>
#else
# include <GL/glu.h>
#endif

#include "hax_camera.h"

int camera_render(void)
{

	glLoadIdentity();
	gluLookAt(3.0f,3.0f,3.0f, 0,0,0, 0,0,1.0f);
	return 0;
}
