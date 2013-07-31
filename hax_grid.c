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
	glDrawElements(GL_LINES,array_count(g->indices),GL_UNSIGNED_INT,array_data(g->indices));
	return 0;
}

void add_vertex(Array *a, float x, float y, float z, GLuint c);
void add_vertex(Array *a, float x, float y, float z, GLuint c)
{
	GRID_VERTEX v;
	v.x=x; v.y=y; v.z=z; v.color=c;
	array_add(a,&v);
}

VECTOR3F *grid_hex2rect(Grid *g, VECTOR3F *pointxz, HEXCOORD cc)
{
	float hex_delta=2*g->cell_size*COS30;

	*pointxz=VECTOR3F_c((float)cc.u,(float)cc.v,(float)cc.f);
	*pointxz=VECTOR3F_mulfloat(*pointxz,hex_delta);
	HEXTOXZ(*pointxz);

	return pointxz;
}

HEXAGON_VERTEX *get_vertex(HEXAGON_VERTEX v, HEXAGON_EDGE *e1, HEXAGON_EDGE *e2);
HEXAGON_VERTEX *get_vertex(HEXAGON_VERTEX v, HEXAGON_EDGE *e1, HEXAGON_EDGE *e2)
{
	#define VERTICES_EQ(v1,v2) ((v1).x-(v2).x==0 && (v1).z-(v2).z==0)
	if (e1 && e1->v1 && VERTICES_EQ(*e1->v1,v)) return e1->v1;
	if (e1 && e1->v2 && VERTICES_EQ(*e1->v2,v)) return e1->v2;
	if (e2 && e2->v1 && VERTICES_EQ(*e2->v1,v)) return e2->v1;
	if (e2 && e2->v2 && VERTICES_EQ(*e2->v2,v)) return e2->v2;
	return NULL;
	#undef VERTICES_EQ
}

Grid *grid_create(Map *map, float cell_size)
{
	Array *vertices;
	Array *edges;
	int i,e;
	MapCell *cell;
	HEXAGON_EDGE *edge,*edge1,*edge2,*edge3,*edge4;
	MapCell *n;
	VECTOR3F vc[6];
	VECTOR3F cell_coord;
	HEXAGON_VERTEX v, *vertex;
	GLuint index;

	Grid *g=NEW(Grid);
	if (!g) error_exit("out of memory");


	g->map=map;
	g->cell_size=cell_size;

	vertices=array_create(sizeof(HEXAGON_VERTEX*));
	edges=array_create(sizeof(HEXAGON_EDGE*));

	#define OPPOSITE_EDGE(e) ((e)+3>5?(e)-3:(e)+3)

	/* loop through map cells */
	for (i=0;i<array_count(g->map->cells);i++)
	{
		/* the current cell */
		array_item(g->map->cells,i,&cell);
		/* loop through cell sides */
		for (e=0;e<6;e++) {
			/* if side wasnt processed yet */
			if (cell->edges[e]==NULL)
			{
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

	for (i=0;i<6;i++)
	{
		vc[i].x=(float)hex_direction[i].u;
		vc[i].y=(float)hex_direction[i].v;
		vc[i].z=(float)hex_direction[i].f;
		vc[i]=VECTOR3F_mulfloat(vc[i],g->cell_size);
		TRITOXZ(vc[i]);
	};
	#define PREV_EDGE(e) ((e)-1<0?(e)+5:(e)-1)
	#define NEXT_EDGE(e) ((e)+1>5?(e)-5:(e)+1)
	for (i=0;i<array_count(g->map->cells);i++)
	{
		/* take next cell */
		array_item(g->map->cells,i,&cell);
		grid_hex2rect(g,&cell_coord,cell->Coord);
		/* loop througg its edges */
		for (e=0;e<6;e++)
		{
			/* next edge */
			edge=cell->edges[e];
			n=map_neighbour(g->map,cell,hex_direction[e]);
			/* the four adjacent edges */
			edge1=n?n->edges[OPPOSITE_EDGE(e+1)]:NULL;
			edge2=cell->edges[PREV_EDGE(e)];
			edge3=n?n->edges[OPPOSITE_EDGE(e-1)]:NULL;
			edge4=cell->edges[NEXT_EDGE(e)];

			v.x=cell_coord.x+vc[PREV_EDGE(e)].x;
			v.z=cell_coord.z+vc[PREV_EDGE(e)].z;
			if (edge->v1==NULL) 
				edge->v1=get_vertex(v,edge1,edge2);
			if (edge->v1==NULL)
			{
				vertex=NEW(HEXAGON_VERTEX);
				*vertex=v;
				array_add(vertices,&vertex);
				edge->v1=vertex;
			};

			v.x=cell_coord.x+vc[e].x;
			v.z=cell_coord.z+vc[e].z;
			if (edge->v2==NULL) 
				edge->v2=get_vertex(v,edge3,edge4);
			if (edge->v2==NULL)
			{
				HEXAGON_VERTEX *vertex=NEW(HEXAGON_VERTEX);
				*vertex=v;
				array_add(vertices,&vertex);
				edge->v2=vertex;
			};
		};
	};

	#undef NEXT_EDGE
	#undef PREV_EDGE
	#undef OPPOSITE_EDGE
	grid_clear(g);

	g->vertices=array_create(sizeof(GRID_VERTEX));
	for (i=0;i<array_count(vertices);i++) {
		array_item(vertices,i,&vertex);
		add_vertex(g->vertices,vertex->x,0,vertex->z,0xffffff);
	};

	g->indices=array_create(sizeof(GLuint));
	for (i=0;i<array_count(edges);i++) {
		array_item(edges,i,&edge);
		index=array_find(vertices,&edge->v1);
		array_add(g->indices,&index);
		index=array_find(vertices,&edge->v2);
		array_add(g->indices,&index);
	};

	for (i=0;i<array_count(vertices);i++)
	{
		array_item(vertices,i,&vertex);
		free(vertex);
	};
	for (i=0;i<array_count(edges);i++)
	{
		array_item(edges,i,&edge);
		free(edge);
	};
	array_free(vertices);
	array_free(edges);

	g->waves=array_create(sizeof(GRID_WAVE));
	return g;


}
void grid_clear(Grid *g)
{
}

int grid_free(Grid *g)
{
	array_free(g->waves);
	array_free(g->indices);
	array_free(g->vertices);
	free(g);
	return 0;
}

void grid_animate(Grid *g, float delta)
{
	int i;
	GRID_VERTEX *v=array_data(g->vertices);

	for (i=0;i<array_count(g->vertices);i++)
	{
		v->y=((float)(rand()%10))/100.0f;
		v++;
	};
}
