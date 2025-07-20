/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// gstring/cmp_bstring.c                                                    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#ifndef NDEBUG
#include <string.h>	/* assertions with memcmp() */
#endif	/* NDEBUG */

#include <assert.h>
#include <stddef.h>

#include "common.h"

/* //////////////////////////////////////////////////////////////////////// */

X_GSTRING_PURE
static int cmp_bstring_local(
	const struct GStringLocal *, const uint8_t *, size_t, size_t,
	gstring_fnptr_bytecmp
)
/*@*/
;

X_GSTRING_PURE
static int cmp_bstring_alloc(
	const struct GStringAlloc *, const uint8_t *, size_t, size_t,
	gstring_fnptr_bytecmp
)
/*@*/
;

/* //////////////////////////////////////////////////////////////////////// */

/* returns like memcmp() */
/*@unused@*/
X_GSTRING_PURE
int
gstring_cmp_bstring(
	const struct GString *const s1_gs,
	const uint8_t *const s2_bs, const size_t s2_bs_len,
	const gstring_fnptr_bytecmp my_bytecmp
)
/*@*/
{
	const size_t cmp_len = ((size_t) s1_gs->len < s2_bs_len
		? (size_t) s1_gs->len : s2_bs_len
	);

	return (GSTRING_IS_LOCAL(*s1_gs)
		? cmp_bstring_local(
			(struct GStringLocal *) s1_gs,
			s2_bs, s2_bs_len,
			cmp_len, my_bytecmp
		)
		: cmp_bstring_alloc(
			(struct GStringAlloc *) s1_gs,
			s2_bs, s2_bs_len,
			cmp_len, my_bytecmp
		)
	);
}

/* ------------------------------------------------------------------------ */

/* returns like memcmp() */
X_GSTRING_PURE
static int
cmp_bstring_local(
	const struct GStringLocal *const s1_gs,
	const uint8_t *const s2_bs, const size_t s2_bs_len,
	const size_t cmp_len, const gstring_fnptr_bytecmp my_bytecmp
)
/*@*/
{
	int retval;

	assert(GSTRING_IS_LOCAL(*s1_gs));

	retval = (cmp_len != 0 ? my_bytecmp(s1_gs->c8, s2_bs, cmp_len) : 0);
	if ( (retval == 0)  && (s1_gs->len != s2_bs_len) ){
		retval = ((size_t) s1_gs->len < s2_bs_len ? -1 : 1);
	}
	return retval;
}

/* returns like memcmp() */
X_GSTRING_PURE
static int
cmp_bstring_alloc(
	const struct GStringAlloc *const s1_gs,
	const uint8_t *const s2_bs, const size_t s2_bs_len,
	size_t cmp_len, const gstring_fnptr_bytecmp my_bytecmp
)
/*@*/
{
	const uint8_t *const s1_ptr    = s1_gs->ptr;
	const uint8_t *const s1_prefix = s1_gs->prefix;
	/* */
	int retval;

	assert(GSTRING_IS_ALLOC(*s1_gs));
	assert(memcmp(s1_prefix, s1_ptr, GSTRING_PREFIX_SIZE) == 0);

	/* test the prefix first */
	if ( cmp_len >= GSTRING_PREFIX_SIZE ){
		retval = my_bytecmp(s1_prefix, s2_bs, GSTRING_PREFIX_SIZE);
		if ( (retval != 0) || (cmp_len == GSTRING_PREFIX_SIZE) ){
			return retval;
		}
	}

	/* test the full string; purposely not skipping the prefix */
	retval = (cmp_len != 0 ? my_bytecmp(s1_ptr, s2_bs, cmp_len) : 0);
	if ( retval == 0 ){
		retval = ((size_t) s1_gs->len < s2_bs_len ? -1 : 1);
	}
	return retval;
}

/* EOF //////////////////////////////////////////////////////////////////// */
