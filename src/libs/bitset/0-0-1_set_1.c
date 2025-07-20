/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// bitset/set_1.c                                                           //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2024-2025, Shane Seelig                                    //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <stddef.h>
#include <stdint.h>

#include "common.h"

/* //////////////////////////////////////////////////////////////////////// */

/** @fn bitset_set_1
  * @brief sets the bit at the index to 1
  *
  * @param bitset the bitset
  * @param idx the index of the bit
  *
  * @return the old bit (0|1)
 **/
/*@unused@*/
int
bitset_set_1(/*@reldef@*/ uint8_t *const bitset, const size_t idx)
/*@modifies	*bitset@*/
{
	const size_t  idx_byte = BITSET_IDX_BYTE(idx);
	const uint8_t idx_bit  = BITSET_IDX_BIT(idx);
	const uint8_t mask     = (uint8_t) (0x1u << idx_bit);
	const uint8_t byte     = bitset[idx_byte];

	bitset[idx_byte] = byte | mask;
	return (int) ((byte & mask) >> idx_bit);
}

/* EOF //////////////////////////////////////////////////////////////////// */
