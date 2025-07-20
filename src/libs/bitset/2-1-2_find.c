/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// bitset/find.c                                                            //
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

/** @fn bitset_find
  * @brief finds the first 0|1 in the bitset starting at an index
  *
  * @param bitset[in] the bitset
  * @param bitlen the number of bits in the bitset
  * @param start the index to start at
  * @param value 0|1
  *
  * @return the index of the first 0|1 from the start
  * @retval SIZE_MAX - not found
 **/
/*@unused@*/
X_BITSET_PURE
size_t
bitset_find(
	const uint8_t *const bitset, const size_t bitlen, const size_t start,
	const int value
)
/*@*/
{
	return (value == 0
		? bitset_find_0(bitset, bitlen, start)
		: bitset_find_1(bitset, bitlen, start)
	);
}

/* EOF //////////////////////////////////////////////////////////////////// */
