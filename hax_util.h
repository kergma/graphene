#ifndef HAX_UTIL_H

typedef struct tagHEXAGON_VERTEX {
	float x,z;
} HEXAGON_VERTEX;

typedef struct tagHEXAGON_EDGE {
	HEXAGON_VERTEX *v1, *v2;
} HEXAGON_EDGE;

typedef struct tagGRID_VERTEX {
	float x,y,z;
	unsigned int color;
} GRID_VERTEX;


#define D3DX_2PI 6.28f
#define COS30 0.86602540378f
#define TAN30 0.5773502691f
#define SIN30 0.5f

#define ROUND(x) floorf((x)+0.5f)


#define RANDOM(x) ((x)*rand()/RAND_MAX)
#define RANDOMF(x)   (float(x)*float(rand())/float(RAND_MAX))




#define TRITOXZ(t) t=D3DXVECTOR3(-t.y-t.z,0,TAN30*(t.y-t.z))

/* #define XZTOTRI(t) t=D3DXVECTOR3(t.x,COS30*t.z-SIN30*t.x,-COS30*t.z-SIN30*t.x) */
/* #define TRITOXZ(t) t=D3DXVECTOR3(-t.y-t.z,0,TAN30*(t.y-t.z)) */
#define XZTOHEX(h) (h)=D3DXVECTOR3(COS30*(h).x-SIN30*(h).z,(h).z,-SIN30*(h).z-COS30*(h).x)
#define HEXTOXZ(h) (h)=D3DXVECTOR3(TAN30*((h).x-(h).z),0,-(h).z-(h).x)
/* #define TRITOHEX(t) TRITOXZ(t);XZTOHEX(t) */
/* #define HEXTOTRI(h) HEXTOXZ(h);XZTOTRI(h) */

void error_exit(const char *message);

#define NEW(t) ((t*)calloc(1,sizeof(t)))

typedef struct tagArray
{
	int count;
	int size;
	char *data;
	int item_size;
	int initial_size;
	int growby_size;
} Array;

Array *array_create(int item_size);
void array_reset(Array *a);
void array_free(Array *a);
void array_grow(Array *a);
unsigned int array_add(Array *a, void *item);


#define HAX_UTIL_H
#endif /* HAX_UTIL_H */
