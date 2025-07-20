/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// bitset/get.c                                                             //
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

/** @fn bitset_get
  * @brief gets the bit at the index
  *
  * @param bitset[in] the bitset
  * @param idx the index of the bit
  *
  * @retval 0|1 - the bit
 **/
/*@unused@*/
X_BITSET_PURE
int
bitset_get(const uint8_t *const bitset, const size_t idx)
/*@*/
{
	const size_t  idx_byte = BITSET_IDX_BYTE(idx);
	const uint8_t idx_bit  = BITSET_IDX_BIT(idx);

	return (int) ((bitset[idx_byte] >> idx_bit) & 0x1u);
}

/* EOF //////////////////////////////////////////////////////////////////// */
