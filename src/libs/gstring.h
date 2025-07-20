#ifndef GSTRING_H
#define GSTRING_H
/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// gstring.h - german strings                                               //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// https://cedardb.com/blog/german_strings                                  //
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
	uint8_t		x_gstring_0[12u];
	uint32_t	len;
};

#define GSTRING_INIT_NULL		{{0,0,0,0,0,0,0,0,0,0,0,0}, 0}

#define X_GSTRING_LOCAL_LIMIT		((uint32_t) 12u)
#define X_GSTRING_IS_LOCAL(x_gs)	((x_gs).len <= X_GSTRING_LOCAL_LIMIT)

#define GSTRING_PTR(x_gs_ptr)		( \
	X_GSTRING_IS_LOCAL(*(x_gs_ptr)) \
		? &(x_gs_ptr)->x_gstring_0[0] \
		: *((uint8_t **) (&(x_gs_ptr)->x_gstring_0[0])) \
)

/* ======================================================================== */

/* like <stdlib.h> malloc() */
typedef void *(*gstring_fnptr_malloc)(size_t);
/* like <stdlib.h> free()   */
typedef void  (*gstring_fnptr_free)(void *);

struct GString_MyAlloc {
	/*@temp@*/
	gstring_fnptr_malloc		malloc;
	/*@temp@*/ /*@null@*/
	gstring_fnptr_free		free;
};

#define GSTRING_NULL_FNPTR_FREE		((gstring_fnptr_free) NULL)

/* ------------------------------------------------------------------------ */

/* like <string.h> memcmp(); __attribute__((pure)) */
typedef int (*gstring_fnptr_bytecmp)(
	const uint8_t *, const uint8_t *, size_t
);

/* //////////////////////////////////////////////////////////////////////// */

#undef dest
#undef len
#undef my_alloc
/*@external@*/ /*@unused@*/
extern int gstring_new_undef(
	/*@reldef@*/
	struct GString *dest,
	size_t len,
	const struct GString_MyAlloc *my_alloc
)
/*@modifies	*dest@*/
;

#undef gs
/*@external@*/ /*@unused@*/
extern void gstring_mod_fini(/*@partial@*/ struct GString *gs)
/*@modifies	*gs@*/
;

#undef dest
#undef src
#undef src_len
/*@external@*/ /*@unused@*/
extern int gstring_ref_bstring(
	/*@out@*/
	struct GString *X_GSTRING_RESTRICT dest,
	/*@reldef@*/
	const uint8_t *X_GSTRING_RESTRICT src,
	size_t src_len
)
/*@modifies	*dest@*/
;

#undef dest
#undef src
#undef src_len
#undef my_alloc
/*@external@*/ /*@unused@*/
extern int gstring_copy_bstring(
	/*@out@*/
	struct GString *X_GSTRING_RESTRICT dest,
	const uint8_t *X_GSTRING_RESTRICT src,
	size_t src_len,
	const struct GString_MyAlloc *my_alloc
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*dest
@*/
;

#undef dest
#undef src
#undef my_alloc
/*@external@*/ /*@unused@*/
extern int gstring_copy_gstring(
	/*@out@*/ struct GString *X_GSTRING_RESTRICT dest,
	const struct GString *X_GSTRING_RESTRICT src,
	const struct GString_MyAlloc *my_alloc
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*dest
@*/
;

#undef gs
#undef my_alloc
/*@external@*/ /*@unused@*/
extern void gstring_free(
	struct GString *gs, const struct GString_MyAlloc *my_alloc
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*gs
@*/
;

#undef s1_gs
#undef s2_bs
#undef s2_bs_len
#undef my_bytecmp
/*@external@*/ /*@unused@*/
X_GSTRING_PURE
extern int gstring_cmp_bstring(
	const struct GString *s1_gs,
	const uint8_t *s2_bs,
	size_t s2_bs_len,
	gstring_fnptr_bytecmp my_bytecmp
)
/*@*/
;

#undef s1
#undef s2
#undef my_bytecmp
/*@external@*/ /*@unused@*/
X_GSTRING_PURE
extern int gstring_cmp_gstring(
	const struct GString *s1,
	const struct GString *s2,
	gstring_fnptr_bytecmp my_bytecmp
)
/*@*/
;

/* EOF //////////////////////////////////////////////////////////////////// */
#endif	/* GSTRING_H */
