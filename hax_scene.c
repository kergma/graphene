#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef HAVE_COCOA
# include <OpenGL/gl.h>
#else
# include <GL/gl.h>
#endif

#include "hax_scene.h"
#include "hax_map.h"
#include "hax_camera.h"
#include "hax_grid.h"

Map *map=NULL;


int scene_create(void)
{
	if (map) map_free(map);
	map=map_new();
	return 0;
}

int scene_render(void)
{
	camera_render();
	grid_render();

	return 0;
}

int scene_free(void)
{
	if (map) map_free(map);
	map=NULL;
	return 0;
}
