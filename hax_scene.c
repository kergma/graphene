#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef HAVE_COCOA
# include <OpenGL/gl.h>
#else
# include <GL/gl.h>
#endif

#include <stdlib.h>
#include "hax_scene.h"
#include "hax_map.h"
#include "hax_camera.h"
#include "hax_grid.h"

Scene *scene_create(void)
{
	Scene *s=NEW(Scene);
	if (!s) error_exit("out of memory");

	s->map=map_create();
	map_create_hex(s->map,17);

	s->grid=grid_create(s->map,0.33f);

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
