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

	glInterleavedArrays(GL_C4UB_V3F,sizeof(GRID_VERTEX),array_data(g->vertices));
	glDrawArrays(GL_LINE_STRIP,0,array_count(g->vertices));
	return 0;
}

void add_vertex(Array *a, float x, float y, float z, GLuint c);
void add_vertex(Array *a, float x, float y, float z, GLuint c)
{
	GRID_VERTEX v;
	v.x=x; v.y=y; v.z=z; v.color=c;
	array_add(a,&v);
}

Grid *grid_create(Map *map, float cell_size)
{
	Grid *g=NEW(Grid);
	if (!g) error_exit("out of memory");

	g->map=map;
	g->cell_size=cell_size;

	g->vertices=array_create(sizeof(GRID_VERTEX));
	add_vertex(g->vertices, 0.0f, 0.0f,0.0f,0x000000);
	add_vertex(g->vertices, 0.0f,-0.2f,0.0f,0x0000ff);
	add_vertex(g->vertices,-0.2f,-0.2f,0.0f,0x00ff00);
	add_vertex(g->vertices,-0.2f,-0.4f,0.0f,0xff0000);
	add_vertex(g->vertices,-0.4f,-0.4f,0.0f,0xffffff);
	add_vertex(g->vertices,-0.4f,-0.6f,0.0f,0xffff00);
	add_vertex(g->vertices,-0.6f,-0.6f,0.0f,0xff00ff);
	add_vertex(g->vertices,-0.6f,-0.8f,0.0f,0x00ffff);
	add_vertex(g->vertices,-0.8f,-0.8f,0.0f,0x000000);
	add_vertex(g->vertices,-0.8f,-1.0f,0.0f,0x0000ff);
	add_vertex(g->vertices,-1.0f,-1.0f,0.0f,0x00ff00);
	add_vertex(g->vertices,-1.0f,-1.2f,0.0f,0xff0000);

	return g;


}

int grid_free(Grid *g)
{
	array_free(g->vertices);
	free(g);
	return 0;
}

