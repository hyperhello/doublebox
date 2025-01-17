#pragma once

/*

	Doublebox is a simplified implementation of encoding data in floating point numbers, or "nanboxing".

	Theory: In IEEE 754 64-bit floating point, a NaN can be represented by the high 16 bits equalling 0xFFF8 through 0xFFFF. In practice, hardware only produces NaN with 0xFFF8. We can use 0xFFF9—0xFFFF to easily encode at least seven things in the other 48 bits:

	0xFFF9 (1001) null
	0xFFFA (1010) undefined
	0xFFFB (1011)	bool, with zero (false) or nonzero (true) in data area
	0xFFFC (1100) integer or other data, 48 bits must be passed as uint64_t.
	0xFFFD (1101) string, up to five chars that will be padded with trailing zeros.
	0xFFFE (1110) custom, available for your adapted use 
	0xFFFF (1111) empty, it's easy to memset an array with 0xFF.
	
	There is no doublebox type. The code all just uses doubles, but you will probably want to typedef in your own project. Double numbers (from -inf to inf inclusive) just work as normal doubles. In some cases, such as the string type, you can reliably compare two doubleboxes with == and !=. 
	
	ToDo: Need to check big endian compatibility.
	
	Note: This encoding technique keeps numerical doubles the same, which is the simplest way to do it. There is another reasonable system available: subtracting 0xFFF9000000000000 to encode, and adding it back to decode. This makes the encoding space go from 0x0000...0x0006. The usefulness of this is that 0x0000... can be a real pointer into 48-bit memory space, if your system would be better optimized in that way, at the cost of the translation to and from doubles.
	
*/

#include <stdio.h>
#include <stdint.h>  
#include <stdbool.h>  
#include <math.h>
#include <string.h>

/* Big endian? (Mostly equivallent to how WebKit does it) */
#if defined(__MIPSEB__) /* MIPS 32-bit */ \
    || defined(__ppc__) || defined(__PPC__) /* CPU(PPC) - PowerPC 32-bit */ \
    || defined(__powerpc__) || defined(__powerpc) || defined(__POWERPC__) \
    || defined(_M_PPC) || defined(__PPC) \
    || defined(__ppc64__) || defined(__PPC64__) /* PowerPC 64-bit */ \
    || defined(__sparc)   /* Sparc 32bit */  \
    || defined(__sparc__) /* Sparc 64-bit */ \
    || defined(__s390x__) /* S390 64-bit */ \
    || defined(__s390__)  /* S390 32-bit */ \
    || defined(__ARMEB__) /* ARM big endian */ \
    || ((defined(__CC_ARM) || defined(__ARMCC__)) /* ARM RealView compiler */ \
        && defined(__BIG_ENDIAN))
	#define DOUBLEBOX_BIG_ENDIAN 1
#endif


#define DOUBLEBOX_TYPEMASK		0xFFFF000000000000
#define DOUBLEBOX_DATAMASK		0x0000FFFFFFFFFFFF
#define DOUBLEBOX_MAX_DOUBLE	0xFFF8FFFFFFFFFFFF		/* this NAN is the largest double */

/* these are returned from get_doublebox_type() */
#define DOUBLEBOX_DOUBLE 			0x0000000000000000	
#define DOUBLEBOX_NULL 				0xFFF9000000000000	
#define DOUBLEBOX_UNDEFINED 	0xFFFA000000000000
#define DOUBLEBOX_BOOL	 			0xFFFB000000000000
#define DOUBLEBOX_INTEGER		 	0xFFFC000000000000		/* 48 bit encoded integer */
#define DOUBLEBOX_STRING	 		0xFFFD000000000000
#define DOUBLEBOX_CUSTOM		 	0xFFFE000000000000
#define DOUBLEBOX_EMPTY 			0xFFFF000000000000

/* quickly create doubleboxes */

double doublebox_bytes(const uint64_t i)
{
	return *(double*)&i;
}
double doublebox_double(double d)
{
	// this will convert an encoded doublebox d to a regular NAN
	if (*(uint64_t*)&d > DOUBLEBOX_MAX_DOUBLE) 
	{
		return NAN;
	}
	return d;
}
double doublebox_null()
{
	uint64_t i = DOUBLEBOX_NULL;
	return *(double*)&i;
}
double doublebox_undefined()
{
	uint64_t i = DOUBLEBOX_UNDEFINED;
	return *(double*)&i;
}
double doublebox_bool(const bool b)
{
	uint64_t i = DOUBLEBOX_BOOL | (b ? 1 : 0);
	return *(double*)&i;
}
double doublebox_integer(const uint64_t data)
{
	// only the top 48 bits are used
	uint64_t i = DOUBLEBOX_INTEGER | (data & DOUBLEBOX_DATAMASK);
	return *(double*)&i;
}
double doublebox_string(const char* str)
{
	uint64_t i = DOUBLEBOX_STRING;
	
	#ifdef DOUBLEBOX_BIG_ENDIAN
		strncpy((char*)&i+2, str, 5);
	#else
		strncpy((char*)&i, str, 5);
	#endif
	
	return *(double*)&i;
}
double doublebox_custom(const uint64_t data)
{
	uint64_t i = DOUBLEBOX_CUSTOM | (data & DOUBLEBOX_DATAMASK);
	return *(double*)&i;
}
double doublebox_empty()
{
	uint64_t i = DOUBLEBOX_EMPTY;
	return *(double*)&i;
}

/* quickly check the type of a doublebox */

bool is_doublebox_double(const double d)
{
	return (*(uint64_t*)&d <= DOUBLEBOX_MAX_DOUBLE);
}
bool is_doublebox_null(const double d)
{
	return (*(uint64_t*)&d & DOUBLEBOX_TYPEMASK) == DOUBLEBOX_NULL;
}
bool is_doublebox_undefined(const double d)
{
	return (*(uint64_t*)&d & DOUBLEBOX_TYPEMASK) == DOUBLEBOX_UNDEFINED;
}
bool is_doublebox_bool(const double d)
{
	return (*(uint64_t*)&d & DOUBLEBOX_TYPEMASK) == DOUBLEBOX_BOOL;
}
bool is_doublebox_false(const double d)
{
	return (*(uint64_t*)&d == DOUBLEBOX_BOOL);
}
bool is_doublebox_true(const double d)
{
	return (*(uint64_t*)&d & DOUBLEBOX_TYPEMASK) == DOUBLEBOX_BOOL && !!(*(uint64_t*)&d & DOUBLEBOX_DATAMASK);
}
bool is_doublebox_integer(const double d)
{
	return (*(uint64_t*)&d & DOUBLEBOX_TYPEMASK) == DOUBLEBOX_INTEGER;
}
bool is_doublebox_string(const double d)
{
	return (*(uint64_t*)&d & DOUBLEBOX_TYPEMASK) == DOUBLEBOX_STRING;
}
bool is_doublebox_custom(const double d)
{
	return (*(uint64_t*)&d & DOUBLEBOX_TYPEMASK) == DOUBLEBOX_CUSTOM;
}
bool is_doublebox_empty(const double d)
{
	return (*(uint64_t*)&d & DOUBLEBOX_TYPEMASK) == DOUBLEBOX_EMPTY;
}

/* quickly request doublebox values */

uint64_t get_doublebox_type(const double d)
{
	uint64_t i = *(uint64_t*)&d;
	return (i <= DOUBLEBOX_MAX_DOUBLE) ? DOUBLEBOX_DOUBLE : (i & DOUBLEBOX_TYPEMASK);
}
uint64_t get_doublebox_bytes(const double d)
{
	return *(uint64_t*)&d;
}
bool get_doublebox_bool(const double d)
{
	return !!(*(uint64_t*)&d & DOUBLEBOX_DATAMASK);
}
uint64_t get_doublebox_integer(const double d)
{
	return (*(uint64_t*)&d & DOUBLEBOX_DATAMASK);
}
void get_doublebox_string(const double d, char* sixchars)
{
	#ifdef DOUBLEBOX_BIG_ENDIAN
		strncpy(sixchars, (char*)&d+2, 5);
	#else
		strncpy(sixchars, (char*)&d, 5);
	#endif
}
