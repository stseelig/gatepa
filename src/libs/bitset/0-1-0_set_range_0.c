/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// bitset/set_range_0.c                                                     //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2024-2025, Shane Seelig                                    //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "common.h"

/* //////////////////////////////////////////////////////////////////////// */

#undef bitset
static void set_range_0_short(
	/*@reldef@*/ uint8_t *bitset, size_t, uint8_t, uint8_t
)
/*@modifies	*bitset@*/
;

#undef bitset
static void set_range_0_long(
	/*@reldef@*/ uint8_t *bitset, size_t, size_t, uint8_t, uint8_t
)
/*@modifies	*bitset@*/
;

/* //////////////////////////////////////////////////////////////////////// */

/** @fn bitset_set_range_0
  * @brief sets the bits from first to last to 0
  *
  * @param bitset the bitset
  * @param first the index of the first bit
  * @param last the index of the last bit
 **/
/*@unused@*/
void
bitset_set_range_0(
	/*@reldef@*/ uint8_t *const bitset,
	const size_t first, const size_t last
)
/*@modifies	*bitset@*/
{
	const size_t first_idx_byte = BITSET_IDX_BYTE(first);
	const size_t last_idx_byte  = BITSET_IDX_BYTE(last);
	/* * */
	const uint8_t mask_first = (uint8_t) (
		UINT8_MAX << BITSET_IDX_BIT(first)
	);
	const uint8_t mask_last  = (uint8_t) (
		UINT8_MAX >> (7u - BITSET_IDX_BIT(last))
	);

	assert(first <= last);

	if ( first_idx_byte == last_idx_byte ){
		set_range_0_short(
			bitset, first_idx_byte, mask_first, mask_last
		);
	}
	else {	set_range_0_long(
			bitset, first_idx_byte, last_idx_byte,
			mask_first, mask_last
		);
	}
	return;
}

/* ------------------------------------------------------------------------ */

static void
set_range_0_short(
	/*@reldef@*/ uint8_t *const bitset, const size_t byte_idx,
	const uint8_t mask_first, const uint8_t mask_last
)
/*@modifies	*bitset@*/
{
	bitset[byte_idx] &= ~(mask_first & mask_last);
	return;
}

static void
set_range_0_long(
	/*@reldef@*/ uint8_t *const bitset,
	const size_t first_idx_byte, const size_t last_idx_byte,
	const uint8_t mask_first, const uint8_t mask_last
)
/*@modifies	*bitset@*/
{
	const size_t set_len = last_idx_byte - first_idx_byte - 1u;

	assert(first_idx_byte < last_idx_byte);

	bitset[first_idx_byte] &= ~mask_first;
	memset(&bitset[first_idx_byte + 1u], 0x00, set_len);
	bitset[last_idx_byte ] &= ~mask_last;
	return;
}

/* EOF //////////////////////////////////////////////////////////////////// */
