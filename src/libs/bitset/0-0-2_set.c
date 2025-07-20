/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// bitset/set.c                                                             //
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

/** @fn bitset_set
  * @brief sets the bit at the index to the value
  *
  * @param bitset the bitset
  * @param idx the index of the bit
  * @param value 0|1
  *
  * @return the old bit (0|1)
 **/
/*@unused@*/
int
bitset_set(
	/*@reldef@*/ uint8_t *const bitset, const size_t idx, const int value
)
/*@modifies	*bitset@*/
{

	return (value == 0
		? bitset_set_0(bitset, idx)
		: bitset_set_1(bitset, idx)
	);
}

/* EOF //////////////////////////////////////////////////////////////////// */
