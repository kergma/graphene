#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include "hax_util.h"
#include "hax_map.h"

HEXCOORD hex_direction[7]={{1,-1,0}, {1,0,-1}, {0,1,-1}, {-1,1,0}, {-1,0,1}, {0,-1,1}, {1,-1,0}};


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
	if (m->cells) array_free(m->cells);
	free(m);
}

void map_create_hex(int size)
{

}

