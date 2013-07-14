#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdlib.h>
#include "hax_util.h"
#include "hax_map.h"

HEXCOORD hex_direction[7]={{1,-1,0}, {1,0,-1}, {0,1,-1}, {-1,1,0}, {-1,0,1}, {0,-1,1}, {1,-1,0}};


Map *map_create(void)
{
	Map *map=(Map*)calloc(1,sizeof(Map *));
	if (!map) error_exit("out of memory");
	return map;
}

void map_free(Map *map)
{
	free(map);
}
