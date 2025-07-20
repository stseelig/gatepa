/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// bitset/find_0.c                                                          //
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

X_BITSET_PURE
static size_t nextish_0(const uint8_t *, size_t, size_t) /*@*/;

/* //////////////////////////////////////////////////////////////////////// */

/** @fn bitset_find_0
  * @brief finds the first 0 in the bitset starting at an index
  *
  * @param bitset[in] the bitset
  * @param bitlen the number of bits in the bitset
  * @param start the index to start at
  *
  * @return the index of the first 0 from the start
  * @retval SIZE_MAX - not found
 **/
/*@unused@*/
X_BITSET_PURE
size_t
bitset_find_0(
	const uint8_t *const bitset, const size_t bitlen, const size_t start
)
/*@*/
{
	const size_t  idx_byte = BITSET_IDX_BYTE(start);
	const uint8_t idx_bit  = BITSET_IDX_BIT(start);
	const uint8_t nbits    = tzcnt_u8(
		(unsigned int) ((0xFF00u | ~bitset[idx_byte]) >> idx_bit)
	);

	assert(bitlen >= start);

	return (idx_bit + nbits < (uint8_t) 8u
		? (start + nbits < bitlen ? start + nbits : SIZE_MAX)
		: nextish_0(bitset, bitlen, idx_byte + 1u)
	);
}

/* ------------------------------------------------------------------------ */

X_BITSET_PURE
static size_t
nextish_0(const uint8_t *const bitset, const size_t bitlen, size_t idx_byte)
/*@*/
{
	size_t  idx_sum;
	uint8_t idx_bit;

	for ( ; idx_byte < BITSET_BYTELEN(bitlen); ++idx_byte ){
		if ( bitset[idx_byte] != UINT8_MAX ){
			idx_bit = tzcnt_u8(
				~((unsigned int) bitset[idx_byte])
			);
			idx_sum = BITSET_IDX_SUM(idx_byte, idx_bit);
			return (idx_sum < bitlen ? idx_sum : SIZE_MAX);
		}
	}
	return SIZE_MAX;
}

/* EOF //////////////////////////////////////////////////////////////////// */
