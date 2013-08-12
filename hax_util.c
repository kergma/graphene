#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "hax_util.h"
#include "xlockmore.h"

VECTOR3F VECTOR3F_c(float x, float y, float z)
{
	VECTOR3F tmp;
	tmp.x=x; tmp.y=y; tmp.z=z;
	return tmp;
}

VECTOR3F VECTOR3F_mulfloat(VECTOR3F v, float m)
{
	v.x*=m;
	v.y*=m;
	v.z*=m;
	return v;
}
VECTOR3F VECTOR3F_sub(VECTOR3F v, VECTOR3F s)
{
	v.x-=s.x;
	v.y-=s.y;
	v.z-=s.z;
	return v;
}
float VECTOR3F_length(VECTOR3F v)
{
	return sqrtf(v.x*v.x+v.y*v.y+v.z*v.z);
}

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
	memcpy(a->data+a->count*a->item_size,item,a->item_size);
	a->count++;
	return a->count;
}

void *array_data(Array *a)
{
	return a->data;
}

void array_item(Array *a, unsigned int index, void *item)
{
	memcpy(item,a->data+index*a->item_size,a->item_size);
}
void *array_pelement(Array *a, unsigned int index)
{
	return a->data+index*a->item_size;
}
unsigned int array_count(Array *a)
{
	return a->count;
}
unsigned int array_find(Array *a, void *item)
{
	unsigned int i;

	for (i=0;i<array_count(a);i++)
		if (memcmp(item,a->data+i*a->item_size,a->item_size)==0) return i;
	return -1;
}

RandInt RandInt_c2(int a, int b)
{
	RandInt v;
	v.a=a;
	v.b=b;
	v.random=1;
	return v;
}

RandInt RandInt_c1(int a)
{
	RandInt v;
	v.a=a;
	v.random=0;
	return v;
}

int RandInt_value(RandInt *v)
{
	return v->random?(v->b-v->a+1)*(long int)random()/RAND_MAX+v->a:v->a;
}

RandFloat RandFloat_c2(float a, float b)
{
	RandFloat v;
	v.a=a;
	v.b=b;
	v.random=1;
	return v;
}

RandFloat RandFloat_c1(float a)
{
	RandFloat v;
	v.a=a;
	v.random=0;
	return v;
}

float RandFloat_value(RandFloat *v)
{
	return v->random?(v->b-v->a)*(double)random()/RAND_MAX+v->a:v->a;
}

RandVector RandVector_c(RandFloat x, RandFloat y, RandFloat z)
{
	RandVector v;
	v.x=x;
	v.y=y;
	v.z=z;
	return v;
}

VECTOR3F RandVector_value(RandVector *v)
{
	VECTOR3F r;
	r.x=RandFloat_value(&v->x);
	r.y=RandFloat_value(&v->y);
	r.z=RandFloat_value(&v->z);
	return r;
}

RandColor RandColor_c2(COLOR a, COLOR b)
{
	RandColor v;
	v.a=a;
	v.b=b;
	v.random=1;
	return v;
}

RandColor RandColor_c1(COLOR a)
{
	RandColor v;
	v.a=a;
	v.random=0;
	return v;
}

COLOR RandColor_value(RandColor *v)
{
	unsigned char a1=v->a>>24&0xff;
	unsigned char b1=v->b>>24&0xff;
	unsigned char a2=v->a>>16&0xff;
	unsigned char b2=v->b>>16&0xff;
	unsigned char a3=v->a>>8&0xff;
	unsigned char b3=v->b>>8&0xff;
	unsigned char a4=v->a&0xff;
	unsigned char b4=v->b&0xff;
	unsigned char v1=v->random?(b1-a1+1)*(long int)random()/RAND_MAX+a1:a1;
	unsigned char v2=v->random?(b2-a2+1)*(long int)random()/RAND_MAX+a2:a2;
	unsigned char v3=v->random?(b3-a3+1)*(long int)random()/RAND_MAX+a3:a3;
	unsigned char v4=v->random?(b4-a4+1)*(long int)random()/RAND_MAX+a4:a4;
	return v1<<24|v2<<16|v3<<8|v4;
}

float float_lerp(float *pOut, float *f1, float *f2, float s)
{
	*pOut=*f1+(*f2-*f1)*s;
	return *pOut;
}

void VECTOR3F_lerp(VECTOR3F *pOut, VECTOR3F *v1, VECTOR3F *v2, float s)
{
	pOut->x=v1->x+(v2->x-v1->x)*s;
	pOut->y=v1->y+(v2->y-v1->y)*s;
	pOut->z=v1->z+(v2->z-v1->z)*s;
}

void VECTOR3F_hermite(VECTOR3F *out, VECTOR3F *v1, VECTOR3F *t1, VECTOR3F *v2, VECTOR3F *t2, float s)
{
	float h1=2*s*s*s-3*s*s+1;
	float h2=-2*s*s*s+3*s*s;
	float h3=s*s*s-2*s*s+s;
	float h4=s*s*s-s*s;
	out->x=h1*v1->x+h2*v2->x+h3*t1->x+h4*t2->x;
	out->y=h1*v1->y+h2*v2->y+h3*t1->y+h4*t2->y;
	out->z=h1*v1->z+h2*v2->z+h3*t1->z+h4*t2->z;
}
