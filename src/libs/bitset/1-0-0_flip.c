/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// bitset/flip.c                                                            //
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

/** @fn bitset_flip
  * @brief flips the bit at the index
  *
  * @param bitset[in] the bitset
  * @param idx the index of the bit
  *
  * @retval 0|1 - the new bit
 **/
/*@unused@*/
int
bitset_flip(uint8_t *const bitset, const size_t idx)
/*@modifies	*bitset@*/
{
	const size_t  idx_byte = BITSET_IDX_BYTE(idx);
	const uint8_t idx_bit  = BITSET_IDX_BIT(idx);
	const uint8_t mask     = (uint8_t) (0x1u << idx_bit);
	const uint8_t byte     = bitset[idx_byte] ^ mask;

	bitset[idx_byte] = byte;
	return (int) ((byte >> idx_bit) & 0x1u);
}

/* EOF //////////////////////////////////////////////////////////////////// */
