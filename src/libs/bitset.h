#ifndef BITSET_H
#define BITSET_H
/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// bitset.h - bit arrays                                                    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2024-2025, Shane Seelig                                    //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <stddef.h>
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
#undef idx
/*@external@*/ /*@unused@*/
extern int bitset_set_0(uint8_t *bitset, size_t idx)
/*@modifies	*bitset@*/
;

#undef bitset
#undef idx
/*@external@*/ /*@unused@*/
extern int bitset_set_1(uint8_t *bitset, size_t idx)
/*@modifies	*bitset@*/
;

#undef bitset
#undef idx
#undef value
/*@external@*/ /*@unused@*/
extern int bitset_set(uint8_t *bitset, size_t idx, int value)
/*@modifies	*bitset@*/
;

/* ------------------------------------------------------------------------ */

#undef bitset
#undef first
#undef last
/*@external@*/ /*@unused@*/
extern void bitset_set_range_0(
	/*@reldef@*/ uint8_t *bitset, size_t first, size_t last
)
/*@modifies	*bitset@*/
;

#undef bitset
#undef first
#undef last
/*@external@*/ /*@unused@*/
extern void bitset_set_range_1(
	/*@reldef@*/ uint8_t *bitset, size_t first, size_t last
)
/*@modifies	*bitset@*/
;

#undef bitset
#undef first
#undef last
#undef value
/*@external@*/ /*@unused@*/
extern void bitset_set_range(
	/*@reldef@*/ uint8_t *bitset, size_t first, size_t last, int value
)
/*@modifies	*bitset@*/
;

/* ======================================================================== */

#undef bitset
#undef idx
/*@external@*/ /*@unused@*/
extern int bitset_flip(uint8_t *bitset, size_t idx)
/*@modifies	*bitset@*/
;

#undef bitset
#undef first
#undef last
/*@external@*/ /*@unused@*/
extern void bitset_flip_range(
	/*@reldef@*/ uint8_t *bitset, size_t first, size_t last
)
/*@modifies	*bitset@*/
;

/* ======================================================================== */

#undef bitset
#undef idx
/*@external@*/ /*@unused@*/
X_BITSET_PURE
extern int bitset_get(const uint8_t *bitset, size_t idx) /*@*/;

/* ------------------------------------------------------------------------ */

#undef bitset
#undef bitlen
#undef start
/*@external@*/ /*@unused@*/
X_BITSET_PURE
extern size_t bitset_find_0(
	const uint8_t *bitset, size_t bitlen, size_t start
)
/*@*/
;

#undef bitset
#undef bitlen
#undef start
/*@external@*/ /*@unused@*/
X_BITSET_PURE
extern size_t bitset_find_1(
	const uint8_t *bitset, size_t bitlen, size_t start
)
/*@*/
;

#undef bitset
#undef bitlen
#undef start
#undef value
/*@external@*/ /*@unused@*/
X_BITSET_PURE
extern size_t bitset_find(
	const uint8_t *bitset, size_t bitlen, size_t start, int value
)
/*@*/
;

/* ======================================================================== */

#undef bitset
#undef bitlen
/*@external@*/ /*@unused@*/
X_BITSET_PURE
extern size_t bitset_popcount(const uint8_t *bitset, size_t bitlen) /*@*/;

/* EOF //////////////////////////////////////////////////////////////////// */
#endif	/* BITSET_H */
