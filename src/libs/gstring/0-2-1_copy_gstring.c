/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// gstring/copy_gstring.c                                                   //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "common.h"

/* //////////////////////////////////////////////////////////////////////// */

#undef dest
static int copy_gstring_local(
	/*@out@*/ struct GStringLocal *X_GSTRING_RESTRICT dest,
	const struct GStringLocal *X_GSTRING_RESTRICT
)
/*@modifies	*dest@*/
;

#undef dest
static int copy_gstring_alloc(
	/*@out@*/ struct GStringAlloc *X_GSTRING_RESTRICT dest,
	const struct GStringAlloc *X_GSTRING_RESTRICT,
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
gstring_copy_gstring(
	/*@out@*/ struct GString *const X_GSTRING_RESTRICT dest,
	const struct GString *const X_GSTRING_RESTRICT src,
	const struct GString_MyAlloc *const my_alloc
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*dest
@*/
{
	return (GSTRING_IS_LOCAL(*src)
		? copy_gstring_local(
			(struct GStringLocal *) dest,
			(struct GStringLocal *) src
		)
		: copy_gstring_alloc(
			(struct GStringAlloc *) dest,
			(struct GStringAlloc *) src,
			my_alloc
		)
	);
}

/* ------------------------------------------------------------------------ */

/* returns 0 on success */
static int
copy_gstring_local(
	/*@out@*/ struct GStringLocal *const X_GSTRING_RESTRICT dest,
	const struct GStringLocal *const X_GSTRING_RESTRICT src
)
/*@modifies	*dest@*/
{
	assert(GSTRING_IS_LOCAL(*src));

	*dest = *src;

	return 0;
}

/* returns 0 on success */
static int
copy_gstring_alloc(
	/*@out@*/ struct GStringAlloc *const X_GSTRING_RESTRICT dest,
	const struct GStringAlloc *const X_GSTRING_RESTRICT src,
	const struct GString_MyAlloc *const my_alloc
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*dest
@*/
{
	uint8_t *buf = NULL;

	assert(GSTRING_IS_ALLOC(*src));

	buf = my_alloc->malloc((size_t) src->len);
	if ( buf == NULL ){
		/*@-mustdefine@*/
		return -1;
		/*@=mustdefine@*/
	}
	assert(buf != NULL);

	(void) memcpy(buf, src->ptr, (size_t) src->len);
	dest->ptr = buf;
	(void) memcpy(dest->prefix, src->prefix, GSTRING_PREFIX_SIZE);
	dest->len = src->len;

	return 0;
}

/* EOF //////////////////////////////////////////////////////////////////// */
