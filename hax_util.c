#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string.h>
#include "hax_util.h"
#include "xlockmore.h"

void error_exit(const char *message)
{
	fprintf(stderr, "%s: %s\n", progname, message);
	exit(1);
}

Array *array_create(int item_size)
{
	Array *a=NEW(Array);
	if (!a) error_exit("out of memory");
	memset(a,0,sizeof(Array*));
	a->item_size=item_size;
	array_reset(a);
	return a;
}

void array_reset(Array *a)
{
	if (a->data) free(a->data);
	a->data=NULL;
	a->size=a->initial_size;
	a->data=a->size?calloc(a->size,a->item_size):NULL;
	a->count=0;
}

void array_free(Array *a)
{
	if (a->data) free(a->data);
	a->data=NULL;
	free(a);
}

void array_grow(Array *a)
{
	int new_size=a->size;
	char *new_data;
	if (a->growby_size) new_size+=a->growby_size; else if (new_size) new_size*=2; else new_size=1;
	new_data=calloc(new_size,a->item_size);
	if (a->data) memcpy(new_data,a->data,a->size*a->item_size);
	if (a->data) free(a->data);
	a->data=new_data;
	a->size=new_size;
}
unsigned int array_add(Array *a, void *item)
{
	if (a->count>=a->size) array_grow(a);
	memcpy(a->data+a->count,item,a->item_size);
	a->count++;
	return a->count;
}
