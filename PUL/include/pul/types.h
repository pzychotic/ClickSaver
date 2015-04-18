/* Poly
   Useful
   Lib */

#ifndef __PUL_TYPES_H__
#define __PUL_TYPES_H__

typedef unsigned long PULID;
typedef unsigned long PULTAG;

typedef unsigned long PUU32;
typedef signed long PUS32;

typedef unsigned short PUU16;
typedef signed short PUS16;

typedef unsigned char PUU8;
typedef signed char PUS8;

typedef struct
{
	PUS32 X, Y;
	PUS32 Width, Height;
} pusRect;

#ifndef NULL
#define NULL 0L
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#endif