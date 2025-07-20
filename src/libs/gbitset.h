#ifndef GBITSET_H
#define GBITSET_H
/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// gbitset.h - german string inspired bitsets                               //
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

#ifndef S_SPLINT_S

#if !__has_attribute(aligned)
#error "!__has_attribute(aligned)"
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
	uint8_t		x_gbitset_0[12u];
	uint32_t	bitlen;
};

#define GBITSET_INIT_NULL		{{0,0,0,0,0,0,0,0,0,0,0,0}, 0}

#define X_GBITSET_LOCAL_LIMIT		((uint32_t) 96u)
#define X_GBITSET_IS_LOCAL(x_bs)	( \
	(x_bs).bitlen <= X_GBITSET_LOCAL_LIMIT \
)

#define GBITSET_PTR(x_bs_ptr)		( \
	X_GBITSET_IS_LOCAL(*(x_bs_ptr)) \
		? &(x_bs_ptr)->x_gbitset_0[0] \
		: *((uint8_t **) (&(x_bs_ptr)->x_gbitset_0[0])) \
)

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

/* //////////////////////////////////////////////////////////////////////// */

#undef bs
#undef nbits
#undef my_alloc
/*@external@*/ /*@unused@*/
extern int gbitset_init(
	/*@out@*/
	struct GBitset *bs,
	uint32_t nbits,
	const struct GBitset_MyAlloc *my_alloc
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*bs
@*/
;

#undef bs
#undef my_alloc
/*@external@*/ /*@unused@*/
extern void gbitset_free(
	struct GBitset *bs, const struct GBitset_MyAlloc *my_alloc
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*bs
@*/
;

/* EOF //////////////////////////////////////////////////////////////////// */
#endif	/* GBITSET_H */
