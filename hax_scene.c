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

Scene *scene_create(void)
{
	Scene *s=(Scene*)calloc(1,sizeof(Scene*));
	if (!s) error_exit("out of memory");

	s->map=map_create();
	s->grid=grid_create(s->map,1.0);

	return s;
}

int scene_render(Scene *s)
{
	camera_render();
	grid_render();

	return 0;
}

int scene_free(Scene *s)
{
	grid_free(s->grid);
	map_free(s->map);
	free(s);
	return 0;
}
