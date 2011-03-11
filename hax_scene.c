#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef HAVE_COCOA
# include <OpenGL/gl.h>
#else
# include <GL/gl.h>
#endif

#include "hax_scene.h"
#include "hax_camera.h"
#include "hax_grid.h"

int scene_render(void)
{
	camera_render();
	grid_render();

	return 0;
}
