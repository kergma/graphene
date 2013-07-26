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

int grid_render(Grid *g)
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

	glEnable (GL_TEXTURE_2D);
	glColor3f (1, 1, 1);
	glEnableClientState (GL_VERTEX_ARRAY);
	glVertexPointer (3, GL_FLOAT, 0, array_data(g->vertices));
	glDrawArrays(GL_LINE_STRIP,0,array_count(g->vertices));
	return 0;
}

void add_vertex(Array *a, float x, float y, float z);
void add_vertex(Array *a, float x, float y, float z)
{
	GRID_VERTEX v;
	v.x=x; v.y=y; v.z=z;
	array_add(a,&v);
}

Grid *grid_create(Map *map, float cell_size)
{
	Grid *g=NEW(Grid);
	if (!g) error_exit("out of memory");

	g->map=map;
	g->cell_size=cell_size;

	g->vertices=array_create(sizeof(GRID_VERTEX));
	add_vertex(g->vertices,0.0f,0.0f,0.0f);
	add_vertex(g->vertices,0.0f,0.0f,1.0f);
	add_vertex(g->vertices,1.0f,0.0f,1.0f);
	add_vertex(g->vertices,1.0f,1.0f,1.0f);

	return g;


}

int grid_free(Grid *g)
{
	array_free(g->vertices);
	free(g);
	return 0;
}

