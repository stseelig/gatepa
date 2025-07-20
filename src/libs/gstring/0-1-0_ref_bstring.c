/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// gstring/ref_bstring.c                                                    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "common.h"

/* //////////////////////////////////////////////////////////////////////// */

#undef dest
static int ref_bstring_local(
	/*@out@*/ struct GStringLocal *X_GSTRING_RESTRICT dest,
	/*@reldef@*/ const uint8_t *X_GSTRING_RESTRICT, size_t
)
/*@modifies	*dest@*/
;

#undef dest
static int ref_bstring_alloc(
	/*@out@*/ struct GStringAlloc *X_GSTRING_RESTRICT dest,
	/*@reldef@*/ const uint8_t *X_GSTRING_RESTRICT, size_t
)
/*@modifies	*dest@*/
;

/* //////////////////////////////////////////////////////////////////////// */

/* returns 0 on success */
/*@unused@*/
int
gstring_ref_bstring(
	/*@out@*/ struct GString *const X_GSTRING_RESTRICT dest,
	/*@reldef@*/ const uint8_t *const X_GSTRING_RESTRICT src,
	const size_t src_len
)
/*@modifies	*dest@*/
{
	return (src_len <= (size_t) GSTRING_LOCAL_LIMIT
		? ref_bstring_local(
			(struct GStringLocal *) dest, src, src_len
		)
		: ref_bstring_alloc(
			(struct GStringAlloc *) dest, src, src_len
		)
	);
}

/* ------------------------------------------------------------------------ */

/* returns 0 on success */
static int
ref_bstring_local(
	/*@out@*/ struct GStringLocal *const X_GSTRING_RESTRICT dest,
	/*@reldef@*/ const uint8_t *const X_GSTRING_RESTRICT src,
	const size_t src_len
)
/*@modifies	*dest@*/
{
	assert(src_len <= (size_t) GSTRING_LOCAL_LIMIT);

	if ( src_len != 0 ){
		(void) memcpy(dest->c8, src, src_len);
	}
	dest->len = (uint32_t) src_len;

	return 0;
}

/* returns 0 on success */
static int
ref_bstring_alloc(
	/*@out@*/ struct GStringAlloc *const X_GSTRING_RESTRICT dest,
	/*@reldef@*/ const uint8_t *const X_GSTRING_RESTRICT src,
	const size_t src_len
)
/*@modifies	*dest@*/
{
	assert(src_len > (size_t) GSTRING_LOCAL_LIMIT);

	if ( src_len > (size_t) UINT32_MAX ){
		/*@-mustdefine@*/
		return -1;
		/*@=mustdefine@*/
	}

	dest->ptr = (uint8_t *) src;
	(void) memcpy(dest->prefix, src, GSTRING_PREFIX_SIZE);
	dest->len = (uint32_t)  src_len;

	return 0;
}

/* EOF //////////////////////////////////////////////////////////////////// */
