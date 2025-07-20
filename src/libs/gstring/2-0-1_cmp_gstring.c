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
static int cmp_gstring_local(
	const struct GStringLocal *, const struct GStringLocal *, size_t,
	gstring_fnptr_bytecmp
)
/*@*/
;

X_GSTRING_PURE
static int cmp_gstring_alloc(
	const struct GStringAlloc *, const struct GStringAlloc *, size_t,
	gstring_fnptr_bytecmp
)
/*@*/
;

X_GSTRING_PURE
static int cmp_gstring_mixed(
	const struct GStringAlloc *, const struct GStringLocal *, size_t,
	gstring_fnptr_bytecmp
)
/*@*/
;

/* //////////////////////////////////////////////////////////////////////// */

/* returns like memcmp() */
/*@unused@*/
X_GSTRING_PURE
int
gstring_cmp_gstring(
	const struct GString *const s1, const struct GString *const s2,
	const gstring_fnptr_bytecmp my_bytecmp
)
/*@*/
{
	const size_t cmp_len = (size_t) (
		s1->len < s2->len ? s1->len : s2->len
	);
	const uint8_t tt_s1  = (uint8_t) GSTRING_IS_ALLOC(*s1);
	const uint8_t tt_s2  = (uint8_t) GSTRING_IS_ALLOC(*s2);
	const uint8_t tt_cmp = (uint8_t) (tt_s1 | (tt_s2 << 1u));

	assert(tt_cmp <= (uint8_t) 0x03u);
	switch ( tt_cmp ){
	case 0x00u:
		return  cmp_gstring_local(
			(struct GStringLocal *) s1,
			(struct GStringLocal *) s2,
			cmp_len, my_bytecmp
		);
	case 0x01u:
		return  cmp_gstring_mixed(
			(struct GStringAlloc *) s1,
			(struct GStringLocal *) s2,
			cmp_len, my_bytecmp
		);
	case 0x02u:
		return -cmp_gstring_mixed(
			(struct GStringAlloc *) s2,
			(struct GStringLocal *) s1,
			cmp_len, my_bytecmp
		);
	case 0x03u:
	default:
		return  cmp_gstring_alloc(
			(struct GStringAlloc *) s1,
			(struct GStringAlloc *) s2,
			cmp_len, my_bytecmp
		);
	}
}

/* ------------------------------------------------------------------------ */

/* returns like memcmp() */
X_GSTRING_PURE
static int
cmp_gstring_local(
	const struct GStringLocal *const s1,
	const struct GStringLocal *const s2,
	const size_t cmp_len, const gstring_fnptr_bytecmp my_bytecmp
)
/*@*/
{
	int retval;

	assert(GSTRING_IS_LOCAL(*s1));
	assert(GSTRING_IS_LOCAL(*s2));

	retval = my_bytecmp(s1->c8, s2->c8, cmp_len);
	if ( (retval == 0) && (s1->len != s2->len) ){
		retval = (s1->len < s2->len ? -1 : 1);
	}
	return retval;
}

/* returns like memcmp() */
X_GSTRING_PURE
static int
cmp_gstring_alloc(
	const struct GStringAlloc *const s1,
	const struct GStringAlloc *const s2,
	const size_t cmp_len, const gstring_fnptr_bytecmp my_bytecmp
)
/*@*/
{
	const uint8_t *const s1_ptr = s1->ptr;
	const uint8_t *const s2_ptr = s2->ptr;
	/* */
	int retval;

	assert(GSTRING_IS_ALLOC(*s1));
	assert(GSTRING_IS_ALLOC(*s2));
	assert(memcmp(s1->prefix, s1_ptr, GSTRING_PREFIX_SIZE) == 0);
	assert(memcmp(s2->prefix, s2_ptr, GSTRING_PREFIX_SIZE) == 0);

	/* test the prefix first */
	retval = my_bytecmp(s1->prefix, s2->prefix, GSTRING_PREFIX_SIZE);
	if ( retval != 0 ){
		return retval;
	}

	/* test the full string; purposely not skipping the prefix */
	retval = my_bytecmp(s1_ptr, s2_ptr, cmp_len);
	if ( (retval == 0) && (s1->len != s2->len) ){
		retval = (s1->len < s2->len ? -1 : 1);
	}
	return retval;
}

/* returns like memcmp() */
X_GSTRING_PURE
static int
cmp_gstring_mixed(
	const struct GStringAlloc *const s1_alloc,
	const struct GStringLocal *const s2_local,
	size_t cmp_len, const gstring_fnptr_bytecmp my_bytecmp
)
/*@*/
{
	const uint8_t *const s1_ptr    = s1_alloc->ptr;
	const uint8_t *const s2_ptr    = s2_local->c8;
	const uint8_t *const s1_prefix = s1_alloc->prefix;
	/* */
	int retval;

	assert(GSTRING_IS_ALLOC(*s1_alloc));
	assert(GSTRING_IS_LOCAL(*s2_local));
	assert(s1_alloc->len > s2_local->len);
	assert(memcmp(s1_prefix, s1_ptr, GSTRING_PREFIX_SIZE) == 0);

	/* test the prefix first */
	if ( cmp_len >= GSTRING_PREFIX_SIZE ){
		retval = my_bytecmp(s1_prefix, s2_ptr, GSTRING_PREFIX_SIZE);
		if ( (retval != 0) || (cmp_len == GSTRING_PREFIX_SIZE) ){
			return retval;
		}
	}

	/* test the full string; purposely not skipping the prefix */
	retval  = my_bytecmp(s1_ptr, s2_ptr, cmp_len);
	retval += (int) (retval == 0);	/* s1 is always longer */
	return retval;
}

/* EOF //////////////////////////////////////////////////////////////////// */
