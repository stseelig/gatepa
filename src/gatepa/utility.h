#ifndef GATEPA_UTILITY_H
#define GATEPA_UTILITY_H
/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// utility.h                                                                //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <stddef.h>
#include <stdint.h>

/* //////////////////////////////////////////////////////////////////////// */

/* returns the address of the last occurence of 'c', or NULL if no 'c' */
/*@temp@*/ /*@null@*/
PURE
INLINE void *
findrchar(/*@returned@*/ const void *const s, const int c, size_t n)
/*@*/
{
	const uint8_t *s_u8 = s;
	const uint8_t  c_u8 = (uint8_t) c;

	while ( n-- != 0 ){
		if ( s_u8[n] == c_u8 ){
			return (void *) &s_u8[n];
		}
	}
	return NULL;
}

/* returns the number of characters needed to print 'x' in decimal */
CONST
ALWAYS_INLINE unsigned int
ilog10p1(uintmax_t x)
/*@*/
{
	unsigned int retval = 1u;

	while ( (x /= 10u) != 0 ){
		retval += 1u;
	}
	return retval;
}

/* EOF //////////////////////////////////////////////////////////////////// */
#endif	/* GATEPA_UTILITY_H */
