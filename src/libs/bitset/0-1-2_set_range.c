/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// gbitset/set_range.c                                                      //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2024-2025, Shane Seelig                                    //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <stddef.h>

#include "common.h"

/* //////////////////////////////////////////////////////////////////////// */

/** @fn bitset_set_range
  * @brief sets the bits from first to last
  *
  * @param bitset the bitset
  * @param first the index of the first bit
  * @param last the index of the last bit
  * @param value 0|1
 **/
/*@unused@*/
void
bitset_set_range(
	/*@reldef@*/ uint8_t *const bitset,
	const size_t first, const size_t last, const int value
)
/*@modifies	*bitset@*/
{
	if ( value == 0 ){
		bitset_set_range_0(bitset, first, last);
	}
	else {	bitset_set_range_1(bitset, first, last); }

	return;
}

/* EOF //////////////////////////////////////////////////////////////////// */
