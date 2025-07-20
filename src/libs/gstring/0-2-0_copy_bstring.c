/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// gstring/copy_bstring.c                                                   //
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
static int copy_bstring_local(
	/*@out@*/ struct GStringLocal *X_GSTRING_RESTRICT dest,
	const uint8_t *X_GSTRING_RESTRICT, size_t
)
/*@modifies	*dest@*/
;

#undef dest
static int copy_bstring_alloc(
	/*@out@*/ struct GStringAlloc *X_GSTRING_RESTRICT dest,
	const uint8_t *X_GSTRING_RESTRICT, size_t,
	const struct GString_MyAlloc *
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*dest
@*/
;

/* //////////////////////////////////////////////////////////////////////// */

/* returns 0 on success */
/*@unused@*/
int
gstring_copy_bstring(
	/*@out@*/ struct GString *const X_GSTRING_RESTRICT dest,
	const uint8_t *const X_GSTRING_RESTRICT src, const size_t src_len,
	const struct GString_MyAlloc *const my_alloc
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*dest
@*/
{
	return (src_len <= (size_t) GSTRING_LOCAL_LIMIT
		? copy_bstring_local(
			(struct GStringLocal *) dest, src, src_len
		)
		: copy_bstring_alloc(
			(struct GStringAlloc *) dest, src, src_len, my_alloc
		)
	);
}

/* ------------------------------------------------------------------------ */

/* returns 0 on success */
static int
copy_bstring_local(
	/*@out@*/ struct GStringLocal *const X_GSTRING_RESTRICT dest,
	const uint8_t *const X_GSTRING_RESTRICT src, const size_t src_len
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
copy_bstring_alloc(
	/*@out@*/ struct GStringAlloc *const X_GSTRING_RESTRICT dest,
	const uint8_t *const X_GSTRING_RESTRICT src, const size_t src_len,
	const struct GString_MyAlloc *const my_alloc
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*dest
@*/
{
	uint8_t *buf = NULL;

	assert(src_len > (size_t) GSTRING_LOCAL_LIMIT);

	if ( src_len > (size_t) UINT32_MAX ){
		/*@-mustdefine@*/
		return -1;
		/*@=mustdefine@*/
	}

	buf = my_alloc->malloc(src_len);
	if ( buf == NULL ){
		/*@-mustdefine@*/
		return -1;
		/*@=mustdefine@*/
	}
	assert(buf != NULL);

	(void) memcpy(buf, src, src_len);
	dest->ptr = buf;
	(void) memcpy(dest->prefix, src, GSTRING_PREFIX_SIZE);
	dest->len = (uint32_t) src_len;

	return 0;
}

/* EOF //////////////////////////////////////////////////////////////////// */
