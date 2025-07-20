/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// text.c                                                                   //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// the ascii functions mostly exist to avoid locale bs                      //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <stddef.h>
#include <stdint.h>

#include <libs/ascii-literals.h>

#include "attributes.h"

/* //////////////////////////////////////////////////////////////////////// */

/* utf8 encoding table from wikipedia
 -------------------------------------
 | Byte 1 | Byte 2 | Byte 3 | Byte 4 |
 |--------|--------------------------|
 |0xxxxxxx|                          |
 |--------|--------------------------|
 |110xxxxx|10xxxxxx|                 |
 |--------|--------|-----------------|
 |1110xxxx|10xxxxxx|10xxxxxx|        |
 |--------|--------|--------|--------|
 |11110xxx|10xxxxxx|10xxxxxx|10xxxxxx|
 -------------------------------------
*/
static const uint8_t utf8_cpsize_table[256u] = {
#define T	uint8_t
	/* ascii */
	(T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u,
	(T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u,
	(T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u,
	(T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u,
	(T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u,
	(T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u,
	(T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u,
	(T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u,
	(T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u,
	(T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u,
	(T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u,
	(T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u,
	(T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u,
	(T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u,
	(T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u,
	(T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u, (T) 1u,
	/* continuation */
	(T) 0u, (T) 0u, (T) 0u, (T) 0u, (T) 0u, (T) 0u, (T) 0u, (T) 0u,
	(T) 0u, (T) 0u, (T) 0u, (T) 0u, (T) 0u, (T) 0u, (T) 0u, (T) 0u,
	(T) 0u, (T) 0u, (T) 0u, (T) 0u, (T) 0u, (T) 0u, (T) 0u, (T) 0u,
	(T) 0u, (T) 0u, (T) 0u, (T) 0u, (T) 0u, (T) 0u, (T) 0u, (T) 0u,
	(T) 0u, (T) 0u, (T) 0u, (T) 0u, (T) 0u, (T) 0u, (T) 0u, (T) 0u,
	(T) 0u, (T) 0u, (T) 0u, (T) 0u, (T) 0u, (T) 0u, (T) 0u, (T) 0u,
	(T) 0u, (T) 0u, (T) 0u, (T) 0u, (T) 0u, (T) 0u, (T) 0u, (T) 0u,
	(T) 0u, (T) 0u, (T) 0u, (T) 0u, (T) 0u, (T) 0u, (T) 0u, (T) 0u,
	/* 2-byte codepoint */
	(T) 2u, (T) 2u, (T) 2u, (T) 2u, (T) 2u, (T) 2u, (T) 2u, (T) 2u,
	(T) 2u, (T) 2u, (T) 2u, (T) 2u, (T) 2u, (T) 2u, (T) 2u, (T) 2u,
	(T) 2u, (T) 2u, (T) 2u, (T) 2u, (T) 2u, (T) 2u, (T) 2u, (T) 2u,
	(T) 2u, (T) 2u, (T) 2u, (T) 2u, (T) 2u, (T) 2u, (T) 2u, (T) 2u,
	/* 3-byte codepoint */
	(T) 3u, (T) 3u, (T) 3u, (T) 3u, (T) 3u, (T) 3u, (T) 3u, (T) 3u,
	(T) 3u, (T) 3u, (T) 3u, (T) 3u, (T) 3u, (T) 3u, (T) 3u, (T) 3u,
	/* 4-byte codepoint */
	(T) 4u, (T) 4u, (T) 4u, (T) 4u, (T) 4u, (T) 4u, (T) 4u, (T) 4u,
	/* invalid */
	(T) -1, (T) -1, (T) -1, (T) -1, (T) -1, (T) -1, (T) -1, (T) -1
#undef T
};

/* //////////////////////////////////////////////////////////////////////// */

/* returns the toupper'd character */
CONST
GATEPA uint8_t
ascii_toupper(uint8_t c)
/*@*/
{
	if ( (c >= ASCII_A_LO) && (c <= ASCII_Z_LO) ){
		c -= ASCII_CASE_DIFF;
	}
	return c;
}

/* returns the tolower'd character */
CONST
GATEPA uint8_t
ascii_tolower(uint8_t c)
/*@*/
{
	if ( (c >= ASCII_A_UP) && (c <= ASCII_Z_UP) ){
		c += ASCII_CASE_DIFF;
	}
	return c;
}

/* ======================================================================== */

/* returns 0 if the string is printable ascii */
NOINLINE PURE
GATEPA int
ascii_isprintables(const uint8_t *const str, const size_t len)
/*@*/
{
	uint8_t c;
	size_t i;

	for ( i = 0; i < len; ++i ){
		c = str[i];
		if ( (c < ASCII_SP) || (c > ASCII_TILDE) ){
			return -1;
		}
	}
	return 0;
}

/* returns like memcmp() */
NOINLINE PURE
GATEPA int
ascii_casecmp(
	const uint8_t *const s1, const uint8_t *const s2, const size_t n
)
/*@*/
{
	uint8_t c1, c2;
	size_t i;

	for ( i = 0; i < n; ++i ){
		c1 = ascii_toupper(s1[i]);
		c2 = ascii_toupper(s2[i]);
		if ( c1 != c2 ){
			return ((int) c1) - ((int) c2);
		}
	}
	return 0;
}

/* ======================================================================== */

/* returns 0 if the string is utf8 */
NOINLINE PURE
GATEPA int
utf8_verify(const uint8_t *const str, const size_t len)
/*@*/
{
	uint8_t cpsize, check = 0;
	size_t i;

	for ( i = 0; i < len; i += cpsize ){
		cpsize = utf8_cpsize_table[str[i]];
		if ( len - i < cpsize ){
			return -1;
		}
		switch ( cpsize ){
		case (uint8_t) 4u:
			check |= utf8_cpsize_table[str[i + 3u]];
			/*@fallthrough@*/
		case (uint8_t) 3u:
			check |= utf8_cpsize_table[str[i + 2u]];
			/*@fallthrough@*/
		case (uint8_t) 2u:
			check |= utf8_cpsize_table[str[i + 1u]];
			if ( check != 0 ){
				return -1;
			}
			continue;
		case (uint8_t) 1u:
			continue;
		default:
			return -1;
		}
	}
	return 0;
}

/* EOF //////////////////////////////////////////////////////////////////// */
