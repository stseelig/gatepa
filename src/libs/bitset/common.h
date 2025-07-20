#ifndef BITSET_COMMON_H
#define BITSET_COMMON_H
/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// bitset/common.h                                                          //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2024-2025, Shane Seelig                                    //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <assert.h>
#include <limits.h>
#include <stdint.h>

/* //////////////////////////////////////////////////////////////////////// */

#ifdef __GNUC__

#ifdef __has_attribute
#define X_BITSET_HAS_ATTRIBUTE_GNUC(x)		__has_attribute(x)
#else
#define X_BITSET_HAS_ATTRIBUTE_GNUC(x)		0
#endif

#define X_BITSET_ATTRIBUTE_GNUC_CONST		const
#define X_BITSET_ATTRIBUTE_GNUC_PURE		pure

#else	/* !defined(__GNUC__) */

#define X_BITSET_HAS_ATTRIBUTE_GNUC(x)		0

#define X_BITSET_ATTRIBUTE_GNUC_CONST		nil
#define X_BITSET_ATTRIBUTE_GNUC_PURE		nil

#endif	/* __GNUC__ */

/* ======================================================================== */

#if __STDC_VERSION__ >= 199901L
#define X_BITSET_INLINE		/*@unused@*/ static inline
#elif defined(__GNUC__)
#define X_BITSET_INLINE		/*@unused@*/ static __inline__
#else
#define X_BITSET_INLINE		/*@unused@*/ static
#endif	/* __STDC_VERSION__ */

#if X_BITSET_HAS_ATTRIBUTE_GNUC(X_BITSET_ATTRIBUTE_GNUC_CONST)
#define X_BITSET_CONST		__attribute__((X_BITSET_ATTRIBUTE_GNUC_CONST))
#else
#define X_BITSET_CONST
#endif

#if X_BITSET_HAS_ATTRIBUTE_GNUC(X_BITSET_ATTRIBUTE_GNUC_PURE)
#define X_BITSET_PURE		__attribute__((X_BITSET_ATTRIBUTE_GNUC_PURE))
#else
#define X_BITSET_PURE
#endif

/* //////////////////////////////////////////////////////////////////////// */

#ifdef __GNUC__

#ifdef __has_builtin
#define X_BITSET_HAS_BUILTIN_GNUC(x)		__has_builtin(x)
#else
#define X_BITSET_HAS_BUILTIN_GNUC(x)		0
#endif

#define X_BITSET_BUILTIN_GNUC_POPCOUNT		__builtin_popcount
#define X_BITSET_BUILTIN_GNUC_CTZ		__builtin_ctz

#else	/* !defined(__GNUC__) */

#define X_BITSET_HAS_BUILTIN_GNUC(x)		0

#define X_BITSET_BUILTIN_GNUC_POPCOUNT		nil
#define X_BITSET_BUILTIN_GNUC_CTZ		nil

#endif	/* __GNUC__ */

/* ======================================================================== */

#if X_BITSET_HAS_BUILTIN_GNUC(X_BITSET_BUILTIN_GNUC_CTZ)
#define X_BITSET_TZCNT_U8(x_x)		X_BITSET_BUILTIN_GNUC_CTZ(x_x)
#else
X_BITSET_INLINE X_BITSET_CONST
uint8_t
X_BITSET_TZCNT_U8(const unsigned int x)
/*@*/
{
	uint8_t y = (uint8_t) x;
	uint8_t r = 0;

	if ( (y & 0x01u) == 0 ){
		r  = (uint8_t) 1u;
		if ( (y & 0x0Fu) == 0 ){ r |= 4u, y >>= 4u; }
		if ( (y & 0x03u) == 0 ){ r |= 2u, y >>= 2u; }
		r -=  y & 0x01u;
	}
	return r;
}
#endif
X_BITSET_INLINE X_BITSET_CONST
uint8_t
tzcnt_u8(const unsigned int x)
/*@*/
{
	assert(x != 0);

	return (uint8_t) X_BITSET_TZCNT_U8(x);
}

/* //////////////////////////////////////////////////////////////////////// */

#define BITSET_IDX_BYTE(x_idx)			((size_t)  ((x_idx) >> 3u))
#define BITSET_IDX_BIT(x_idx)			((uint8_t) ((x_idx) & 0x7u))
#define BITSET_IDX_SUM(x_idx_byte, x_idx_bit)	((size_t)  ( \
	(((x_idx_byte) << 3u) | (x_idx_bit)) \
))

#define BITSET_BYTELEN(x_nbits)			((size_t)  ( \
	    BITSET_IDX_BYTE(x_nbits) \
	  + (uint8_t) (BITSET_IDX_BIT(x_nbits) != 0) \
))

/* //////////////////////////////////////////////////////////////////////// */

#undef bitset
/*@external@*/ /*@unused@*/
extern int bitset_set_0(uint8_t *bitset, size_t)
/*@modifies	*bitset@*/
;

#undef bitset
/*@external@*/ /*@unused@*/
extern int bitset_set_1(uint8_t *bitset, size_t)
/*@modifies	*bitset@*/
;

#undef bitset
/*@external@*/ /*@unused@*/
extern void bitset_set_range_0(/*@reldef@*/ uint8_t *bitset, size_t, size_t)
/*@modifies	*bitset@*/
;

#undef bitset
/*@external@*/ /*@unused@*/
extern void bitset_set_range_1(/*@reldef@*/ uint8_t *bitset, size_t, size_t)
/*@modifies	*bitset@*/
;

/*@external@*/ /*@unused@*/
X_BITSET_PURE
extern size_t bitset_find_0(const uint8_t *, size_t, size_t) /*@*/;

/*@external@*/ /*@unused@*/
X_BITSET_PURE
extern size_t bitset_find_1(const uint8_t *, size_t, size_t) /*@*/;

/* EOF //////////////////////////////////////////////////////////////////// */
#endif	/* BITSET_COMMON_H */
