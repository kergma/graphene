#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "hax_util.h"
#include "hax_map.h"

HEXCOORD hex_direction[7]={{1,-1,0}, {1,0,-1}, {0,1,-1}, {-1,1,0}, {-1,0,1}, {0,-1,1}, {1,-1,0}};


Map *new_Map(void)
{
	Map *map=(Map*)calloc(1,sizeof(Map *));
	if (!map) error_exit("out of memory");
	return map;
}

void free_Map(Map *map)
{
	free(map);
}
