#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "graphene_util.h"
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
	memset(a,0,sizeof(Array));
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

unsigned int array_find_index(Array *a, unsigned int *index, void *item);
unsigned int array_find_index(Array *a, unsigned int *index, void *item)
{
	unsigned i1=0,i2=a->count-1,i=0;
	int diff;
	
	void *itema=a->data+i1*a->item_size,*itemb=a->data+i2*a->item_size,*m;
	if (a->count==0)
	{
		*index=0;
		return 0;
	};
	*index=i2+1;

	diff=a->ordering(item,itemb);
	if (diff>0) return 0;
	if (diff==0) {*index=i2;return 1;}
	*index=i1;
	diff=a->ordering(itema,item);
	if (diff>0) return 0;
	if (diff==0) return 1;

	while (i2-i1>1 && diff)
	{
		i=(i1+i2)/2;
		m=a->data+i*a->item_size;
		diff=a->ordering(m,item);
		if (diff<=0)
		{
			i1=i; itema=m;
		} else {
			i2=i; itemb=m;
		};
	};
	if (diff) {
		*index=i2;
		return 0;
	}
	*index=i;
	return 1;
}

unsigned int array_add(Array *a, void *item)
{
	unsigned int index;
	if (a->count>=a->size) array_grow(a);
	if (a->ordering)
	{
		array_find_index(a,&index,item);
		return array_insert(a,item,index);
	};
	memcpy(a->data+a->count*a->item_size,item,a->item_size);
	a->count++;
	return a->count;
}
unsigned int array_insert(Array *a, void *item, unsigned int index)
{
	if (a->count>=a->size) array_grow(a);
	if (a->count>index)
		memmove(a->data+(index+1)*a->item_size,a->data+index*a->item_size,(a->count-index)*a->item_size);
	memcpy(a->data+index*a->item_size,item,a->item_size);
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
	if (a->ordering)
	{
		if (array_find_index(a,&i,item))
			return i;
		else
			return -1;
	};
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

COLOR COLOR_swaprb(COLOR c)
{
	unsigned char a=c>>24&0xff;
	unsigned char r=c>>16&0xff;
	unsigned char g=c>>8&0xff;
	unsigned char b=c&0xff;
	return a<<24|b<<16|g<<8|r;
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

void COLOR_lerp(COLOR *out, COLOR c1, COLOR c2, float s)
{
	unsigned char a1,r1,g1,b1,a2,r2,g2,b2,a,r,g,b;
	a1=(unsigned char)(c1>>24);
	r1=(unsigned char)(c1>>16);
	g1=(unsigned char)(c1>>8);
	b1=(unsigned char)(c1);
	a2=(unsigned char)(c2>>24);
	r2=(unsigned char)(c2>>16);
	g2=(unsigned char)(c2>>8);
	b2=(unsigned char)(c2);
	a=(unsigned char)(a1+(a2-a1)*s);
	r=(unsigned char)(r1+(r2-r1)*s);
	g=(unsigned char)(g1+(g2-g1)*s);
	b=(unsigned char)(b1+(b2-b1)*s);
	*out=(a<<24)+(r<<16)+(g<<8)+b;
}
void initialize_cit(CIT_EL *cit, float contrast)
{
	int i;
	float x,f;
	for (i=0;i<CIT_COUNT;i++)
	{
		x=(float)i;
		f=contrast*x+127.5f*(1-contrast);
		if (f<0) f=0;
		if (f>CIT_COUNT-1) f=CIT_COUNT-1;
		cit[i]=fast_round(f);
	};
}
void COLOR_cit(COLOR *out, COLOR c1, COLOR c2, float s, CIT_EL *cit)
{
	unsigned char a1,r1,g1,b1,a2,r2,g2,b2,a,r,g,b;
	a1=(unsigned char)(c1>>24);
	r1=(unsigned char)(c1>>16);
	g1=(unsigned char)(c1>>8);
	b1=(unsigned char)(c1);
	a2=(unsigned char)(c2>>24);
	r2=(unsigned char)(c2>>16);
	g2=(unsigned char)(c2>>8);
	b2=(unsigned char)(c2);
	a=(unsigned char)(a1+(a2-a1)*cit[fast_round(s*(CIT_COUNT-1))]/CIT_COUNT);
	r=(unsigned char)(r1+(r2-r1)*cit[fast_round(s*(CIT_COUNT-1))]/CIT_COUNT);
	g=(unsigned char)(g1+(g2-g1)*cit[fast_round(s*(CIT_COUNT-1))]/CIT_COUNT);
	b=(unsigned char)(b1+(b2-b1)*cit[fast_round(s*(CIT_COUNT-1))]/CIT_COUNT);
	*out=(a<<24)+(r<<16)+(g<<8)+b;
}

typedef struct tagWayPoint
{
	float time;
} WayPoint;

void animate_point2(WayAnimation *a, float delta, ANIMATE_POINT2_CB func, void *context)
{
	WayPoint *current, *next;
	int next_index;
	float s;

	if (array_count(a->points)==0) return;

	current=array_pelement(a->points,a->current_index);
	a->time+=delta;
	if (a->time>current->time)
	{
		a->time=0;
		a->current_index++;
		if (a->current_index>=array_count(a->points)) a->current_index=0;
		current=array_pelement(a->points,a->current_index);
	};

	next_index=a->current_index+1;
	if (next_index>=array_count(a->points)) next_index=0;
	next=array_pelement(a->points,next_index);

	s=a->time/current->time;

	if (func)
		func(context,current,next,s);

}

typedef union tagINTFLOAT
{
	int i;
	float f;
} INTFLOAT;

INTFLOAT positive_bias={(23+127)<<23};
INTFLOAT negative_bias={((23+127)<<23)+(1<<22)};

int fast_round(float f)
{
	INTFLOAT n;
	n.f=f;
	if (n.i>0) {
		n.f+=positive_bias.f;
		n.i-=positive_bias.i;
	} else {
		n.f+=negative_bias.f;
		n.i-=negative_bias.i;
	};
	return n.i;
}

float fast_abs(float f)
{
	INTFLOAT n;
	n.f=f;
	n.i&=0x7fffffff;
	return n.f;
}

#define SIN_ENTRIES 256
float sin_table[SIN_ENTRIES];


#define ftoi_bias 12582912.0f

float fast_sin(float x)
{
	int i;
	INTFLOAT tmp;
	tmp.f=x*(256.0f/DOUBLE_PI)+ftoi_bias;
	i=tmp.i&255;
	return sin_table[i];
}



void init_fast_math(void)
{
	int i;
	for (i=0;i<SIN_ENTRIES;i++)
		sin_table[i]=sinf(i*DOUBLE_PI/(float)SIN_ENTRIES);
}

SPEC_DUMPER spec_dumper=SD_NONE;

int read_atomic_value(char **pos, void *out, char type)
{

	char *tm1=calloc(fminl(strlen(*pos)+1,9),sizeof(char)); /* 9 chars is the maximum length of color value */
	char *tm2=tm1;

	char *digits="0123456789.-";
	char *whitespace=" \t\n\r";

	if (type=='i') digits="0123456789-";
	if (type=='f') digits="0123456789.-";
	if (type=='x') digits="0123456789abcdefABCDEFx";
	if (type=='c') digits="0123456789abcdefABCDEF#";
	if (type=='s') digits="0123456789-.";

	if (type=='x')
	{
		while (**pos && strstr(*pos,"0x")!=*pos) (*pos)++;
	};
	if (type=='c')
	{
		while (**pos && strstr(*pos,"#")!=*pos) (*pos)++;
		if (**pos) (*pos)++;
	};
	if (type=='i' || type=='f' || type=='x' || type=='c')
	{
		while (**pos && !strchr(digits,**pos)) (*pos)++;
		while (**pos && strchr(digits,**pos))
		{
			*tm2=**pos;
			(*pos)++;
			tm2++;
		};
	};
	if (type=='s')
	{
		while (**pos && strchr(whitespace,**pos)) (*pos)++;
		while (**pos && !strchr(digits,**pos))
		{
			*tm2=**pos;
			(*pos)++;
			tm2++;
		};
		while (tm2>tm1 && strchr(whitespace,*(tm2-1))) tm2--;
	};
	*tm2=0;

	if (*tm1==0)
	{
		free(tm1);
		return 0;
	};
	if (type=='i') *((int*)out)=atoi(tm1);
	if (type=='f') *((float*)out)=atof(tm1);
	/* shorthand html hexadecimal color */
	if (type=='c' && strlen(tm1)==3)
	{
		strcat(tm1,"xxx");
		tm1[5]=tm1[4]=tm1[2];
		tm1[3]=tm1[2]=tm1[1];
		tm1[1]=tm1[0];
	};
	if (type=='c' && strlen(tm1)==4)
	{
		strcat(tm1,"xxxx");
		tm1[7]=tm1[6]=tm1[3];
		tm1[5]=tm1[4]=tm1[2];
		tm1[3]=tm1[2]=tm1[1];
		tm1[1]=tm1[0];
	};
	if (type=='x' || type=='c') *((unsigned int*)out)=strtoul(tm1,NULL,16);
	if (type=='c')
		*((COLOR*)out)=COLOR_swaprb(*((COLOR*)out));
	if (type=='s')
	{
		*((char**)out)=(char*)calloc(strlen(tm1)+1,sizeof(char));
		strcpy(*((char**)out),tm1);
	};
	free(tm1);
	return 1;
}

int read_value(char **pos, void *out, char *type)
{
	int ai,bi;
	float af,bf;
	COLOR ac,bc;
	if (!strcmp(type,"i") || !strcmp(type,"f") || !strcmp(type,"x") || !strcmp(type,"s") || !strcmp(type,"c"))
		return read_atomic_value(pos,out,*type);
	if (!strcmp(type,"v"))
		return read_atomic_value(pos,&((VECTOR3F*)out)->x,'f') && 
			read_atomic_value(pos,&((VECTOR3F*)out)->y,'f') && 
			read_atomic_value(pos,&((VECTOR3F*)out)->z,'f');
	if (!strcmp(type,"ri"))
	{
		if (!read_atomic_value(pos,&ai,'i')) return 0;
		if (**pos==':')
		{
			if (!read_atomic_value(pos,&bi,'i')) return 0;
			*((RandInt*)out)=RandInt_c2(ai,bi);
		}
		else
			*((RandInt*)out)=RandInt_c1(ai);

	};
	if (!strcmp(type,"rf"))
	{
		if (!read_atomic_value(pos,&af,'f')) return 0;
		if (**pos==':')
		{
			if (!read_atomic_value(pos,&bf,'f')) return 0;
			*((RandFloat*)out)=RandFloat_c2(af,bf);
		}
		else
			*((RandFloat*)out)=RandFloat_c1(af);

	};
	if (!strcmp(type,"rv"))
		return read_value(pos,&((RandVector*)out)->x,"rf") &&
			read_value(pos,&((RandVector*)out)->y,"rf") &&
			read_value(pos,&((RandVector*)out)->z,"rf");
	if (!strcmp(type,"rc"))
	{
		if (!read_atomic_value(pos,&ac,'c')) return 0;
		if (**pos==':')
		{
			if (!read_atomic_value(pos,&bc,'c')) return 0;
			*((RandColor*)out)=RandColor_c2(ac,bc);
		}
		else
			*((RandColor*)out)=RandColor_c1(ac);

	};
	return 1;
}

int snprintf_ri(char *str, size_t size, RandInt v)
{
	if (v.random)
		return snprintf(str,size,"%d:%d",v.a,v.b);
	return snprintf(str,size,"%d",v.a);
}
char *snprintf_float(char *buf, size_t size, float f);
char *snprintf_float(char *buf, size_t size, float f)
{
	snprintf(buf,size,"%f",f);
	while (strchr(buf,'.') && (buf[strlen(buf)-1]=='0' || buf[strlen(buf)-1]=='.' ))
		buf[strlen(buf)-1]=0;
	return buf;

}
int snprintf_rf(char *str, size_t size, RandFloat v)
{
	char buf1[30],buf2[30];
	if (v.random)
		return snprintf(str,size,"%s:%s",snprintf_float(buf1,30,v.a),snprintf_float(buf2,30,v.b));
	return snprintf(str,size,"%s",snprintf_float(buf1,30,v.a));
}
int snprintf_rv(char *str, size_t size, RandVector v)
{
	char x[32],y[32],z[32];
	snprintf_rf(x,32,v.x);
	snprintf_rf(y,32,v.y);
	snprintf_rf(z,32,v.z);
	if (spec_dumper==SD_MINIFIED) return snprintf(str,size,"%s %s %s",x,y,z);
	return snprintf(str,size,"%s, %s, %s",x,y,z);
}
char *snprintf_color(char *buf, size_t size, COLOR c);
char *snprintf_color(char *buf, size_t size, COLOR c)
{
	c=COLOR_swaprb(c);
	snprintf(buf,size,"#%06x",c);
	if (strlen(buf)==7 && buf[1]==buf[2] && buf[3]==buf[4] && buf[5]==buf[6])
	{
		buf[2]=buf[3];
		buf[3]=buf[5];
		buf[4]=0;
	};
	if (strlen(buf)==9 && buf[1]==buf[2] && buf[3]==buf[4] && buf[5]==buf[6] && buf[7]==buf[8])
	{
		buf[2]=buf[3];
		buf[3]=buf[5];
		buf[4]=buf[7];
		buf[5]=0;
	};
	return buf;

}
int snprintf_rc(char *str, size_t size, RandColor v)
{
	char buf1[10],buf2[10];
	if (v.random)
		return snprintf(str,size,"%s:%s",snprintf_color(buf1,10,v.a),snprintf_color(buf2,10,v.b));
	return snprintf(str,size,"%s",snprintf_color(buf1,10,v.a));
}

void parse_spec(char **pos, void *out, char *type, char *key, int required)
{
	char buf[512];
	int read;
	read=read_value(pos,out,type);
	if (!read && required) 
	{
		snprintf(buf,512,"error reading scene specification, expecting: %s",key);
		error_exit(buf);
	};
	if (!read) return;
	if (!strcmp(type,"i")) snprintf(buf,512,"%d",*((int*)out));
	if (!strcmp(type,"ri")) snprintf_ri(buf,512,*((RandInt*)out));
	if (!strcmp(type,"f")) snprintf(buf,512,"%f",*((float*)out));
	if (!strcmp(type,"rf")) snprintf_rf(buf,512,*((RandFloat*)out));
	if (!strcmp(type,"x")) snprintf(buf,512,"0x%x",*((unsigned int*)out));
	if (!strcmp(type,"s")) snprintf(buf,512,"%s",*(char**)out);
	if (!strcmp(type,"v")) snprintf(buf,512,"%f, %f, %f",((VECTOR3F*)out)->x,((VECTOR3F*)out)->y,((VECTOR3F*)out)->z);
	if (!strcmp(type,"rv")) snprintf_rv(buf,512,*((RandVector*)out));
	if (!strcmp(type,"rc")) snprintf_rc(buf,512,*((RandColor*)out));
	if (spec_dumper==SD_MINIFIED) printf("%s ",buf);
	if (spec_dumper==SD_EXPLAINED) printf("%s %s ",key,buf);
	/*
	printf("%s: %s\n",key,buf);
	*/
}
