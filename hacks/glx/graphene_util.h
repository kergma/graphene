#ifndef GRAPHENE_UTIL_H

typedef struct tagHEXAGON_VERTEX {
	float x,z;
} HEXAGON_VERTEX;

typedef struct tagHEXAGON_EDGE {
	HEXAGON_VERTEX *v1, *v2;
} HEXAGON_EDGE;

typedef struct tagVECTOR3F
{
	float x,y,z;
} VECTOR3F;

VECTOR3F VECTOR3F_c(float x, float y, float z);
VECTOR3F VECTOR3F_mulfloat(VECTOR3F v, float m);
VECTOR3F VECTOR3F_sub(VECTOR3F v, VECTOR3F s);
float VECTOR3F_length(VECTOR3F v);

typedef struct tagRandInt
{
	int a,b;
	char random;
} RandInt;

RandInt RandInt_c2(int a, int b);
RandInt RandInt_c1(int a);
int RandInt_value(RandInt *v);

typedef struct tagRandFloat
{
	float a,b;
	char random;
} RandFloat;

RandFloat RandFloat_c2(float a, float b);
RandFloat RandFloat_c1(float a);
float RandFloat_value(RandFloat *v);

typedef struct tagRandVector {
	RandFloat x,y,z;
}  RandVector;

RandVector RandVector_c(RandFloat x, RandFloat y, RandFloat z);
VECTOR3F RandVector_value(RandVector *v);

typedef unsigned int COLOR;
typedef struct tagRandColor
{
	COLOR a,b;
	char random;
} RandColor;

RandColor RandColor_c2(COLOR a, COLOR b);
RandColor RandColor_c1(COLOR a);
COLOR RandColor_value(RandColor *v);
COLOR COLOR_swaprb(COLOR c);


#define THE_PI 3.14159f
#define DOUBLE_PI 6.28f
#define COS30 0.86602540378f
#define TAN30 0.5773502691f
#define SIN30 0.5f

#define ROUND(x) floorf((x)+0.5f)


#define RANDOM(x) ((x)*rand()/RAND_MAX)
#define RANDOMF(x)   (float(x)*float(rand())/float(RAND_MAX))




#define XZTOTRI(t) t=VECTOR3F_c(t.x,COS30*t.z-SIN30*t.x,-COS30*t.z-SIN30*t.x)
#define TRITOXZ(t) t=VECTOR3F_c(-t.y-t.z,0,TAN30*(t.y-t.z))
#define XZTOHEX(h) (h)=VECTOR3F_c(COS30*(h).x-SIN30*(h).z,(h).z,-SIN30*(h).z-COS30*(h).x)
#define HEXTOXZ(h) (h)=VECTOR3F_c(TAN30*((h).x-(h).z),0,-(h).z-(h).x)
/* #define TRITOHEX(t) TRITOXZ(t);XZTOHEX(t) */
/* #define HEXTOTRI(h) HEXTOXZ(h);XZTOTRI(h) */

void error_exit(const char *message);

#define NEW(t) ((t*)calloc(1,sizeof(t)))

typedef int (*ORDEDING_FUNC) (void *item1, void *item2);

typedef struct tagArray
{
	int count;
	int size;
	char *data;
	int item_size;
	int initial_size;
	int growby_size;
	ORDEDING_FUNC ordering;
} Array;

Array *array_create(int item_size);
void array_reset(Array *a);
void array_free(Array *a);
void array_grow(Array *a);
unsigned int array_add(Array *a, void *item);
unsigned int array_insert(Array *a, void *item, unsigned int index);

void *array_data(Array *a);
void array_item(Array *a, unsigned int index, void *item);
void *array_pelement(Array *a, unsigned int index);
unsigned int array_count(Array *a);
unsigned int array_find(Array *a, void *item);

float float_lerp(float *pOut, float *f1, float *f2, float s);
void VECTOR3F_lerp(VECTOR3F *pOut, VECTOR3F *v1, VECTOR3F *v2, float s);
void VECTOR3F_hermite(VECTOR3F *pOut, VECTOR3F *v1, VECTOR3F *t1, VECTOR3F *v2, VECTOR3F *t2, float s);
void COLOR_lerp(COLOR *out, COLOR c1, COLOR c2, float s);
#define CIT_COUNT 256
typedef unsigned char CIT_EL;
void initialize_cit(CIT_EL *cit, float contrast);
void COLOR_cit(COLOR *out, COLOR c1, COLOR c2, float s, CIT_EL *cit);

typedef void (*ANIMATE_POINT2_CB) (void *context, void *current, void *next, float s);
typedef struct tagWayAnimation
{
	Array *points;
	int current_index;
	float time;
} WayAnimation;

void animate_point2(WayAnimation *a, float delta, ANIMATE_POINT2_CB func, void *context);

int fast_round(float f);
float fast_abs(float f);
float fast_sin(float x);

void init_fast_math(void);

typedef enum tag_SPEC_DUMPER
{
	SD_NONE,
	SD_MINIFIED,
	SD_EXPLAINED
} SPEC_DUMPER;
extern SPEC_DUMPER spec_dumper;

int read_atomic_value(char **pos, void *out, char type);
int read_value(char **pos, void *out, char *type);
int snprintf_ri(char *str, size_t size, RandInt v);
int snprintf_rf(char *str, size_t size, RandFloat v);
int snprintf_rv(char *str, size_t size, RandVector v);
int snprintf_rc(char *str, size_t size, RandColor v);
void parse_spec(char **pos, void *out, char *type, char *key, int required);

#define GRAPHENE_UTIL_H
#endif /* GRAPHENE_UTIL_H */
