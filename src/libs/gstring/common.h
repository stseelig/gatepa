#ifndef GSTRING_COMMON_H
#define GSTRING_COMMON_H
/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// gstring/common.h                                                         //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <stddef.h>
#include <stdint.h>

/* //////////////////////////////////////////////////////////////////////// */

#if SIZE_MAX < UINT32_MAX
#error "SIZE_MAX < UINT32_MAX"
#endif

#ifndef S_SPLINT_S

#if !__has_attribute(aligned)
#error "!__has_attribute(aligned)"
#endif

#if !defined(__SIZEOF_POINTER__)
 || ((__SIZEOF_POINTER__ != 8u) && (__SIZEOF_POINTER__ != 4u))
#error "__SIZEOF_POINTER__"
#endif

#endif	/* S_SPLINT_S */

/* //////////////////////////////////////////////////////////////////////// */

#ifdef __GNUC__

#ifdef __has_attribute
#define X_GSTRING_HAS_ATTRIBUTE_GNUC(x)		__has_attribute(x)
#else
#define X_GSTRING_HAS_ATTRIBUTE_GNUC(x)		0
#endif

#define X_GSTRING_ATTRIBUTE_GNUC_PURE		pure

#else	/* !defined(__GNUC__) */

#define X_GSTRING_HAS_ATTRIBUTE_GNUC(x)		0

#define X_GSTRING_ATTRIBUTE_GNUC_PURE		nil

#endif	/* __GNUC__ */

/* ======================================================================== */

#if __STDC_VERSION__ >= 201112L
#define X_GSTRING_ALIGNOF 	_Alignof
#elif defined(__GNUC__)
#define X_GSTRING_ALIGNOF 	__alignof__
#else
#define X_GSTRING_ALIGNOF 	sizeof
#endif

#if __STDC_VERSION__ >= 199901L
#define X_GSTRING_RESTRICT 	restrict
#elif defined(__GNUC__)
#define X_GSTRING_RESTRICT 	__restrict__
#else
#define X_GSTRING_RESTRICT
#endif

#if X_GSTRING_HAS_ATTRIBUTE_GNUC(X_GSTRING_ATTRIBUTE_GNUC_PURE)
#define X_GSTRING_PURE		__attribute__((X_GSTRING_ATTRIBUTE_GNUC_PURE))
#else
#define X_GSTRING_PURE
#endif

/* //////////////////////////////////////////////////////////////////////// */

struct __attribute__((aligned(X_GSTRING_ALIGNOF(uint8_t *)))) GString {
	uint8_t		x_0[12u];
	uint32_t	len;
};

struct __attribute__((aligned(X_GSTRING_ALIGNOF(uint8_t *)))) GStringLocal {
	uint8_t		c8[12u];
	uint32_t	len;
};

#define GSTRING_PREFIX_SIZE	((size_t) ( \
	sizeof(struct GString) - sizeof(uint8_t *) - 4u \
))
struct __attribute__((aligned(X_GSTRING_ALIGNOF(uint8_t *)))) GStringAlloc {
	/*@temp@*/
	uint8_t		*ptr;
	uint8_t		prefix[GSTRING_PREFIX_SIZE];
	uint32_t	len;
};

#define GSTRING_LOCAL_LIMIT	((uint32_t) 12u)
#define GSTRING_IS_LOCAL(x_gs)	((x_gs).len <= GSTRING_LOCAL_LIMIT)
#define GSTRING_IS_ALLOC(x_gs)	(!GSTRING_IS_LOCAL(x_gs))

#define GSTRING_PTR(x_gs_ptr)	( \
	GSTRING_IS_LOCAL(*(x_gs_ptr)) \
		? &((struct GStringLocal *) (x_gs_ptr))->c8 \
		:  ((struct GStringAlloc *) (x_gs_ptr))->ptr \
)

/* ======================================================================== */

/* like <stdlib.h> malloc() */
typedef void *(*gstring_fnptr_malloc)(size_t);
/* like <stdlib.h> free()   */
typedef void  (*gstring_fnptr_free)(void *);

struct GString_MyAlloc {
	/*@temp@*/
	gstring_fnptr_malloc	malloc;
	/*@temp@*/ /*@null@*/
	gstring_fnptr_free	free;
};

#define GSTRING_NULL_FNPTR_FREE		((gstring_fnptr_free) NULL)

/* ------------------------------------------------------------------------ */

/* like <string.h> memcmp(); __attribute__((pure)) */
typedef int (*gstring_fnptr_bytecmp)(
	const uint8_t *, const uint8_t *, size_t
);
/* EOF //////////////////////////////////////////////////////////////////// */
#endif	/* GSTRING_COMMON_H */
