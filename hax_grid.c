#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef HAVE_COCOA
# include <OpenGL/gl.h>
#else
# include <GL/gl.h>
#endif

#include <stdlib.h>
#include "hax_grid.h"
#include "hax_map.h"

int grid_render(void)
{
	glBegin(GL_LINES);
	glColor3f(1.0f,0,0);
	glVertex3f(0.0f,0.0f,0.0f);
	glVertex3f(1.0f,0.0f,0.0f);
	glColor3f(0,1.0f,0);
	glVertex3f(0.0f,0.0f,0.0f);
	glVertex3f(0.0f,1.0f,0.0f);
	glColor3f(0,0,1.0f);
	glVertex3f(0.0f,0.0f,0.0f);
	glVertex3f(0.0f,0.0f,1.0f);
	glEnd();
	return 0;
}

Grid *grid_create(Map *map, float cell_size)
{
	Grid *grid=(Grid*)calloc(1,sizeof(Grid*));
	if (!grid) error_exit("out of memory");

	grid->map=map;
	grid->cell_size=cell_size;

	return grid;


}

int grid_free(Grid *grid)
{
	free(grid);
	return 0;
}

