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
#include <math.h>
#include "graphene_geometry.h"

HEXCOORD hex_direction[7]={{1,-1,0}, {1,0,-1}, {0,1,-1}, {-1,1,0}, {-1,0,1}, {0,-1,1}, {1,-1,0}};

HEXCOORD HEXCOORD_c(int u, int v, int f)
{
	HEXCOORD h;
	h.u=u;
	h.v=v;
	h.f=f;
	return h;
}

HEXCOORD HEXCOORD_mulint(HEXCOORD h, int m)
{
	h.u*=m;
	h.v*=m;
	h.f*=m;
	return h;
}

HEXCOORD HEXCOORD_add(HEXCOORD h, HEXCOORD a)
{
	h.u+=a.u;
	h.v+=a.v;
	h.f+=a.f;
	return h;
}

Map *map_create(void)
{
	Map *m=NEW(Map);
	if (!m) error_exit("out of memory");
	memset(m,0,sizeof(*m));
	m->cells=array_create(sizeof(MapCell*));

	return m;
}

void map_free(Map *m)
{
	int i;
	MapCell *cell;
	if (m->cells)
		for (i=0;i<array_count(m->cells);i++)
		{
			array_item(m->cells,i,&cell);
			free(cell);
		};

	if (m->cells) array_free(m->cells);
	if (m->array2d) free(m->array2d);
	free(m);
}

void map_create_hex(Map *m, int size)
{
	MapCell *cell=NEW(MapCell);
	int i,j,k;
	HEXCOORD h,d;

	memset(cell,0,sizeof(*cell));
	cell->Coord=HEXCOORD_c(0,0,0);
	array_add(m->cells,&cell);
	for (i=1;i<size;i++) {
		for (j=0;j<6;j++) {
			h=hex_direction[j];
			h=HEXCOORD_mulint(h,i);
			d=hex_direction[j+2>5?j-4:j+2];
			for (k=0;k<i;k++) {
				cell=NEW(MapCell);
				memset(cell,0,sizeof(*cell));
				cell->Coord=h;
				array_add(m->cells,&cell);
				h=HEXCOORD_add(h,d);
			};
		};
	};
	map_fill_array2d(m);
}
void map_create_rect(Map *m, VECTOR3F *clip_rect, float cell_size)
{
	MapCell *cell;
	int i,j,a,b;
	HEXCOORD h1,h2,h3,d;

	rect2hex(cell_size,&h1,clip_rect[0]);
	rect2hex(cell_size,&h2,VECTOR3F_c(clip_rect[0].x,0,clip_rect[1].z));
	rect2hex(cell_size,&h3,VECTOR3F_c(clip_rect[1].x,0,clip_rect[0].z));

	b=h3.u-h1.u+1;
	a=h2.v-h1.v+1;

	d=hex_direction[3];
	for (i=0;i<a;i++)
	{
		h2=h1;
		for (j=0;j<b;j++)
		{
			cell=NEW(MapCell);
			memset(cell,0,sizeof(*cell));
			cell->Coord=h2;
			array_add(m->cells,&cell);
			h2=HEXCOORD_add(h2,hex_direction[1]);
		};
		h1=HEXCOORD_add(h1,d);
		d=HEXCOORD_add(HEXCOORD_c(-1,2,-1),HEXCOORD_mulint(d,-1));
	};

	map_fill_array2d(m);
}

/* macro to be used inside Map "members" with Map referenced as m */
#define COORD_TO_ARRAY_INDEX(c1,c2) ((c2-m->v_low)*m->u_width+(c1-m->u_low))
void map_fill_array2d(Map *m)
{
	MapCell *tmp;
	int i;
	if (m->array2d) free(m->array2d);
	if (array_count(m->cells)==0) {
		m->u_low=m->v_low=0;
		m->u_high=m->v_high=-1;
		m->u_width=m->u_high-m->u_low+1;
		m->v_width=m->v_high-m->v_low+1;
		return;
	};
	array_item(m->cells,0,&tmp);
	m->u_low=m->u_high=tmp->Coord.u;
	m->v_low=m->v_high=tmp->Coord.v;

	for (i=0;i<array_count(m->cells);i++) {
		array_item(m->cells,i,&tmp);
		if (tmp->Coord.u<m->u_low) m->u_low=tmp->Coord.u;
		if (tmp->Coord.u>m->u_high) m->u_high=tmp->Coord.u;
		if (tmp->Coord.v<m->v_low) m->v_low=tmp->Coord.v;
		if (tmp->Coord.v>m->v_high) m->v_high=tmp->Coord.v;
	};
	m->u_width=m->u_high-m->u_low+1;
	m->v_width=m->v_high-m->v_low+1;
	m->array2d=calloc((1+m->u_high-m->u_low)*(1+m->v_high-m->v_low),sizeof(MapCell*));
	memset(m->array2d,0,m->u_width*m->v_width*sizeof(MapCell*));
	for (i=0;i<array_count(m->cells);i++) {
		array_item(m->cells,i,&tmp);
		m->array2d[COORD_TO_ARRAY_INDEX(tmp->Coord.u,tmp->Coord.v)]=tmp;
	};
}

MapCell *map_cell(Map *m, HEXCOORD cc)
{
	int ai;
	if (cc.u<m->u_low) return NULL;
	if (cc.u>m->u_high) return NULL;
	if (cc.v<m->v_low) return NULL;
	if (cc.v>m->v_high) return NULL;

	ai=COORD_TO_ARRAY_INDEX(cc.u,cc.v);
	if (ai<0) return NULL;
	if (ai>=(1+m->u_high-m->u_low)*(1+m->v_high-m->v_low)) return NULL;
	return m->array2d[ai];
}

MapCell *map_neighbour(Map *m, MapCell *cell, HEXCOORD cdelta)
{
	HEXCOORD c=cell->Coord;
	if (cell==NULL) return NULL;
	return map_cell(m,HEXCOORD_add(c,cdelta));
}


typedef struct tagGRID_VERTEX {
	unsigned int color;
	float x,y,z;
} GRID_VERTEX;


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

VECTOR3F *hex2rect(float cell_size, VECTOR3F *pointxz, HEXCOORD cc)
{
	float hex_delta=2*cell_size*COS30;

	*pointxz=VECTOR3F_c((float)cc.u,(float)cc.v,(float)cc.f);
	*pointxz=VECTOR3F_mulfloat(*pointxz,hex_delta);
	HEXTOXZ(*pointxz);

	return pointxz;
}

HEXCOORD *rect2hex(float cell_size, HEXCOORD *cc, VECTOR3F pointxz)
{
	VECTOR3F tmp;
	float hex_delta=2*cell_size*COS30;
	float dx,dy,dz;

	pointxz.x/=hex_delta;
	pointxz.z/=hex_delta;

	tmp.x=COS30*pointxz.x-SIN30*pointxz.z;
	tmp.y=pointxz.z;
	tmp.z=-SIN30*pointxz.z-COS30*pointxz.x;

	dx=fast_abs(tmp.x-ROUND(tmp.x));
	dy=fast_abs(tmp.y-ROUND(tmp.y));
	dz=fast_abs(tmp.z-ROUND(tmp.z));

	cc->u=fast_round(tmp.x);
	cc->v=fast_round(tmp.y);
	cc->f=fast_round(tmp.z);

	if (dx>dy && dx>dz)
	{
		cc->u=-cc->v-cc->f;
	}
	else if (dy>dx && dy>dz)
	{
		cc->v=-cc->u-cc->f;
	}
	else
	{
		cc->f=-cc->u-cc->v;
	};
	return cc;
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

int vertices_ordering(void *a, void *b);
int vertices_ordering(void *a, void *b)
{
	return (long int)*(void**)a-(long int)*(void**)b;
}

Grid *grid_create(Map *map, float cell_size, Array *waves, Array *colors, float contrast)
{
	Array *vertices;
	Array *edges;
	int i,j,e;
	MapCell *cell;
	HEXAGON_EDGE *edge,*edge1,*edge2,*edge3,*edge4;
	MapCell *n;
	VECTOR3F vc[6];
	VECTOR3F cell_coord;
	HEXAGON_VERTEX v, *vertex;
	GLuint index;
	GRID_WAVE wave;
	GRID_PARAM *param;
	ColorPoint color;

	Grid *g=NEW(Grid);
	if (!g) error_exit("out of memory");
	memset(g,0,sizeof(Grid));


	g->map=map;
	g->cell_size=cell_size;

	vertices=array_create(sizeof(HEXAGON_VERTEX*));
	vertices->ordering=&vertices_ordering;
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
		hex2rect(g->cell_size,&cell_coord,cell->Coord);
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

	g->waves=array_create(sizeof(GRID_WAVE));
	g->amplitudes_sum=0;
	for (i=0;i<array_count(waves);i++)
	{

		array_item(waves,i,&wave);
		wave.period=DOUBLE_PI/wave.period;
		array_add(g->waves,&wave);
		g->amplitudes_sum+=fabsf(wave.amplitude);
	};
	if (g->amplitudes_sum==0) g->amplitudes_sum=1;

	g->contrast=contrast;
	g->cit=(CIT_EL*)calloc(CIT_COUNT,sizeof(CIT_EL));
	initialize_cit(g->cit,g->contrast);
	g->color_animation.points=array_create(sizeof(ColorPoint));
	for (i=0;i<array_count(colors);i++)
	{
		array_item(colors,i,&color);
		array_add(g->color_animation.points,&color);
	};

	param=g->data=(GRID_PARAM*)calloc(array_count(vertices)*array_count(waves),sizeof(GRID_PARAM));

	g->vertices=array_create(sizeof(GRID_VERTEX));
	for (i=0;i<array_count(vertices);i++) {
		array_item(vertices,i,&vertex);
		add_vertex(g->vertices,vertex->x,0,vertex->z,0xffffff);
		for (j=0;j<array_count(g->waves);j++)
		{
			array_item(g->waves,j,&wave);
			*param=(GRID_PARAM)VECTOR3F_length(VECTOR3F_sub(VECTOR3F_c(vertex->x,0,vertex->z),wave.source))*DOUBLE_PI/wave.length;
			param++;
		};
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


	return g;


}
void grid_clear(Grid *g)
{
}

int grid_free(Grid *g)
{
	free(g->data);
	free(g->cit);
	array_free(g->color_animation.points);
	array_free(g->waves);
	array_free(g->indices);
	array_free(g->vertices);
	free(g);
	return 0;
}

void animate_colors(void *context, void *current, void *next, float s);
void animate_colors(void *context, void *current, void *next, float s)
{
	Grid *g=(Grid*)context;
	ColorPoint *c=(ColorPoint*)current;
	ColorPoint *n=(ColorPoint*)next;

	COLOR_lerp(&g->color1,c->color1,n->color1,s);
	COLOR_lerp(&g->color2,c->color2,n->color2,s);
}
void grid_animate(Grid *g, float delta)
{
	int i,j;
	register float phase;
	GRID_VERTEX *v=array_data(g->vertices);
	GRID_PARAM *param=g->data;
	register int wcount=array_count(g->waves);

	animate_point2(&g->color_animation,delta,&animate_colors,g);

	g->time+=delta;

	for (i=0;i<array_count(g->vertices);i++)
	{
		phase=0;
		for (j=0;j<wcount;j++) {
			register GRID_WAVE *wave=(GRID_WAVE*)array_pelement(g->waves,j);
			phase+=wave->amplitude*fast_sin(wave->period*g->time-*param);
			param++;
		};
		v->y=phase;
		phase/=g->amplitudes_sum;
		COLOR_cit(&v->color,g->color1,g->color2,(phase+1)/2,g->cit);
		v++;
	};

}
