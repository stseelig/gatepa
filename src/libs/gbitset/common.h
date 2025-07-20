#ifndef GBITSET_COMMON_H
#define GBITSET_COMMON_H
/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// gbitset/common.h                                                         //
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

#if __STDC_VERSION__ >= 201112L
#define X_GBITSET_ALIGNOF 	_Alignof
#elif defined(__GNUC__)
#define X_GBITSET_ALIGNOF 	__alignof__
#else
#define X_GBITSET_ALIGNOF 	sizeof
#endif

/* //////////////////////////////////////////////////////////////////////// */

struct __attribute__((aligned(X_GBITSET_ALIGNOF(uint8_t *)))) GBitset {
	uint8_t		x_0[12u];
	uint32_t	bitlen;
};

struct __attribute__((aligned(X_GBITSET_ALIGNOF(uint8_t *)))) GBitsetLocal {
	uint8_t		d8[12u];
	uint32_t	bitlen;
};

#define GBITSET_PAD_SIZE	(12u - __SIZEOF_POINTER__)
struct __attribute__((aligned(X_GBITSET_ALIGNOF(uint8_t *)))) GBitsetAlloc {
	/*@temp@*/
	uint8_t		*ptr;
	uint8_t		pad0[GBITSET_PAD_SIZE];
	uint32_t	bitlen;
};

#if   GBITSET_PAD_SIZE == 4u || defined(S_SPLINT_S)
#define GBITSET_PAD_ZERO	{0,0,0,0}
#elif GBITSET_PAD_SIZE == 8u
#define GBITSET_PAD_ZERO	{0,0,0,0,0,0,0,0}
#else
#error "GBITSET_PAD_ZERO"
#endif

#define GBITSET_LOCAL_LIMIT	((uint32_t) 96u)
#define GBITSET_IS_LOCAL(x_bs)	((x_bs).bitlen <= GBITSET_LOCAL_LIMIT)
#define GBITSET_IS_ALLOC(x_bs)	(!GBITSET_IS_LOCAL(x_bs))

#define GBITSET_PTR(x_bs_ptr)	( \
	GBITSET_IS_LOCAL(*(x_bs_ptr)) \
		? &((struct GBitsetLocal *) (x_bs_ptr))->d8 \
		:  ((struct GBitsetAlloc *) (x_bs_ptr))->ptr \
)

#define GBITSET_IDX_BYTE(x_idx)		((size_t) ((x_idx) >> 3u))
#define GBITSET_IDX_BIT(x_idx)		((uint8_t)  ((x_idx) & 0x7u))

#define GBITSET_BYTELEN(x_bitlen)	((size_t) ( \
	  GBITSET_IDX_BYTE(x_bitlen) \
	  + (uint8_t) (GBITSET_IDX_BIT(x_bitlen) != 0) \
))

/* ======================================================================== */

/* like <stdlib.h> calloc() */
typedef void *(*gbitset_fnptr_calloc)(size_t, size_t);
/* like <stdlib.h> free()   */
typedef void  (*gbitset_fnptr_free)(void *);

struct GBitset_MyAlloc {
	/*@temp@*/
	gbitset_fnptr_calloc	calloc;
	/*@temp@*/ /*@null@*/
	gbitset_fnptr_free	free;
};

#define GBITSET_NULL_FNPTR_FREE		((gbitset_fnptr_free) NULL)

/* EOF //////////////////////////////////////////////////////////////////// */
#endif	/* GBITSET_COMMON_H */
