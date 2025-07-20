/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// bitset/popcount.c                                                        //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2024-2025, Shane Seelig                                    //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "common.h"

/* //////////////////////////////////////////////////////////////////////// */

#if X_BITSET_HAS_BUILTIN_GNUC(X_BITSET_BUILTIN_GNUC_POPCOUNT)

X_BITSET_INLINE X_BITSET_CONST
unsigned int
popcount_uint(const unsigned int x)
/*@*/
{
	return X_BITSET_BUILTIN_GNUC_POPCOUNT(x);
}

#else	/* !X_BITSET_HAS_BUILTIN_GNUC(X_BITSET_BUILTIN_GNUC_POPCOUNT) */

X_BITSET_INLINE X_BITSET_CONST
uint8_t
popcount_u8(const uint8_t x)
/*@*/
{
	const uint8_t popcount_u8_table[256u] = {
	#define T	uint8_t
	(T) 0u, (T) 1u, (T) 1u, (T) 2u, (T) 1u, (T) 2u, (T) 2u, (T) 3u,
	(T) 1u, (T) 2u, (T) 2u, (T) 3u, (T) 2u, (T) 3u, (T) 3u, (T) 4u,
	(T) 1u, (T) 2u, (T) 2u, (T) 3u, (T) 2u, (T) 3u, (T) 3u, (T) 4u,
	(T) 2u, (T) 3u, (T) 3u, (T) 4u, (T) 3u, (T) 4u, (T) 4u, (T) 5u,
	(T) 1u, (T) 2u, (T) 2u, (T) 3u, (T) 2u, (T) 3u, (T) 3u, (T) 4u,
	(T) 2u, (T) 3u, (T) 3u, (T) 4u, (T) 3u, (T) 4u, (T) 4u, (T) 5u,
	(T) 2u, (T) 3u, (T) 3u, (T) 4u, (T) 3u, (T) 4u, (T) 4u, (T) 5u,
	(T) 3u, (T) 4u, (T) 4u, (T) 5u, (T) 4u, (T) 5u, (T) 5u, (T) 6u,
	(T) 1u, (T) 2u, (T) 2u, (T) 3u, (T) 2u, (T) 3u, (T) 3u, (T) 4u,
	(T) 2u, (T) 3u, (T) 3u, (T) 4u, (T) 3u, (T) 4u, (T) 4u, (T) 5u,
	(T) 2u, (T) 3u, (T) 3u, (T) 4u, (T) 3u, (T) 4u, (T) 4u, (T) 5u,
	(T) 3u, (T) 4u, (T) 4u, (T) 5u, (T) 4u, (T) 5u, (T) 5u, (T) 6u,
	(T) 2u, (T) 3u, (T) 3u, (T) 4u, (T) 3u, (T) 4u, (T) 4u, (T) 5u,
	(T) 3u, (T) 4u, (T) 4u, (T) 5u, (T) 4u, (T) 5u, (T) 5u, (T) 6u,
	(T) 3u, (T) 4u, (T) 4u, (T) 5u, (T) 4u, (T) 5u, (T) 5u, (T) 6u,
	(T) 4u, (T) 5u, (T) 5u, (T) 6u, (T) 5u, (T) 6u, (T) 6u, (T) 7u,
	(T) 1u, (T) 2u, (T) 2u, (T) 3u, (T) 2u, (T) 3u, (T) 3u, (T) 4u,
	(T) 2u, (T) 3u, (T) 3u, (T) 4u, (T) 3u, (T) 4u, (T) 4u, (T) 5u,
	(T) 2u, (T) 3u, (T) 3u, (T) 4u, (T) 3u, (T) 4u, (T) 4u, (T) 5u,
	(T) 3u, (T) 4u, (T) 4u, (T) 5u, (T) 4u, (T) 5u, (T) 5u, (T) 6u,
	(T) 2u, (T) 3u, (T) 3u, (T) 4u, (T) 3u, (T) 4u, (T) 4u, (T) 5u,
	(T) 3u, (T) 4u, (T) 4u, (T) 5u, (T) 4u, (T) 5u, (T) 5u, (T) 6u,
	(T) 3u, (T) 4u, (T) 4u, (T) 5u, (T) 4u, (T) 5u, (T) 5u, (T) 6u,
	(T) 4u, (T) 5u, (T) 5u, (T) 6u, (T) 5u, (T) 6u, (T) 6u, (T) 7u,
	(T) 2u, (T) 3u, (T) 3u, (T) 4u, (T) 3u, (T) 4u, (T) 4u, (T) 5u,
	(T) 3u, (T) 4u, (T) 4u, (T) 5u, (T) 4u, (T) 5u, (T) 5u, (T) 6u,
	(T) 3u, (T) 4u, (T) 4u, (T) 5u, (T) 4u, (T) 5u, (T) 5u, (T) 6u,
	(T) 4u, (T) 5u, (T) 5u, (T) 6u, (T) 5u, (T) 6u, (T) 6u, (T) 7u,
	(T) 3u, (T) 4u, (T) 4u, (T) 5u, (T) 4u, (T) 5u, (T) 5u, (T) 6u,
	(T) 4u, (T) 5u, (T) 5u, (T) 6u, (T) 5u, (T) 6u, (T) 6u, (T) 7u,
	(T) 4u, (T) 5u, (T) 5u, (T) 6u, (T) 5u, (T) 6u, (T) 6u, (T) 7u,
	(T) 5u, (T) 6u, (T) 6u, (T) 7u, (T) 6u, (T) 7u, (T) 7u, (T) 8u
	#undef T
	};

	return popcount_u8_table[x];
}

#endif	/* X_BITSET_HAS_BUILTIN_GNUC(X_BITSET_BUILTIN_GNUC_POPCOUNT) */

/* //////////////////////////////////////////////////////////////////////// */

/** @fn bitset_popcount
  * @brief counts the number of 1 bits in the bitset
  *
  * @param bitset[in] the bitset
  * @param bitlen the number of bits in the bitset
  *
  * @retval the number of 1 bits
 **/
/*@unused@*/
X_BITSET_PURE
size_t
bitset_popcount(const uint8_t *const bitset, const size_t bitlen)
/*@*/
{
#if X_BITSET_HAS_BUILTIN_GNUC(X_BITSET_BUILTIN_GNUC_POPCOUNT)

	const size_t bytelen = BITSET_BYTELEN(bitlen);
	size_t head_size, tail_size, body_len;
	size_t count = 0;
	size_t i, j;

	head_size = ((uintptr_t) bitset % sizeof(int) != 0
		? sizeof(int) - ((uintptr_t) bitset % sizeof(int)) : 0
	);
	if ( head_size > bytelen ){
		head_size = bytelen;
	}

	tail_size = (bytelen - head_size) % sizeof(int);

	body_len  = bytelen - head_size - tail_size;
	assert(body_len % sizeof(int) == 0);
	body_len /= sizeof(int);

	i = 0;
	for ( j = 0; j < head_size; ++j, i += 1u ){
		count += popcount_uint((unsigned int) bitset[i]);
	}
	assert(((uintptr_t) &bitset[i]) % sizeof(int) == 0);
	for ( j = 0; j < body_len ; ++j, i += sizeof(int) ){
		count += popcount_uint(*((unsigned int *) &bitset[i]));
	}
	for ( j = 0; j < tail_size; ++j, i += 1u ){
		count += popcount_uint((unsigned int) bitset[i]);
	}
	return count;

#else	/* ! X_BITSET_HAS_BUILTIN_GNUC(X_BITSET_BUILTIN_GNUC_POPCOUNT) */

	size_t count = 0;
	size_t i;

	for ( i = 0; i < BITSET_BYTELEN(bitlen); ++i ){
		count += popcount_u8(bitset[i]);
	}
	return count;

#endif	/*  X_BITSET_HAS_BUILTIN_GNUC(X_BITSET_BUILTIN_GNUC_POPCOUNT) */
}

/* EOF //////////////////////////////////////////////////////////////////// */
