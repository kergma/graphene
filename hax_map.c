#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include "hax_util.h"
#include "hax_map.h"

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
	if (cc.u<m->u_low) return NULL;
	if (cc.u>m->u_high) return NULL;
	if (cc.v<m->v_low) return NULL;
	if (cc.v>m->v_high) return NULL;

	int ai=COORD_TO_ARRAY_INDEX(cc.u,cc.v);
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

