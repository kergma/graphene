#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef HAVE_COCOA
# include <OpenGL/gl.h>
#else
# include <GL/gl.h>
#endif

#include <stdlib.h>
#include <string.h>
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
	Array *edges;
	int i,e;
	MapCell *cell;
	HEXAGON_EDGE *edge;
	MapCell *n;

	Grid *g=NEW(Grid);
	if (!g) error_exit("out of memory");


	g->map=map;
	g->cell_size=cell_size;

	edges=array_create(sizeof(HEXAGON_EDGE*));

	#define OPPOSITE_EDGE(e) ((e)+3>5?(e)-3:(e)+3)

	/* loop throug map cells */
	for (i=0;i<array_count(g->map->cells);i++) {
		/* the current cell */
		array_item(g->map->cells,i,&cell);
		/* loop through cell sides */
		for (e=0;e<6;e++) {
			/* if side didnt processed yet */
			if (cell->edges[e]==NULL) {
				/* create new edge for current side */
				edge=NEW(HEXAGON_EDGE);
				memset(edge,0,sizeof(*edge));
				array_add(edges,&edge);
				/* link it to cell */
				cell->edges[e]=edge;
				/* find another adjacing cell */
				n=map_neighbour(g->map,cell,hex_direction[e]);
				/* and link edge to it too */
				if (n) n->edges[OPPOSITE_EDGE(e)]=edge;
			};
		};
	};

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

	array_free(edges);
	return g;


}

int grid_free(Grid *g)
{
	array_free(g->vertices);
	free(g);
	return 0;
}

