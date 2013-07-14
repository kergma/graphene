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
	Array *a;
	int i,j;
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
	/*
	FillArray();

	*/
}

