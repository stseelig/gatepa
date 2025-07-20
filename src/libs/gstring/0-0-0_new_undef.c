/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// gstring/new_undef.c                                                      //
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
static int new_undef_local(/*@reldef@*/ struct GStringLocal *dest, size_t)
/*@modifies	*dest@*/
;

#undef dest
static int new_undef_alloc(
	/*@reldef@*/ struct GStringAlloc *dest, size_t,
	const struct GString_MyAlloc *
)
/*@modifies	*dest@*/
;

/* //////////////////////////////////////////////////////////////////////// */

/* allocs an undefined string.
   use 'gstring_ref_bstring(&str, GSTRING_PTR(&str), str.len)' after you are
     finished defining the string (for the alloc prefix)
*/
/* returns 0 on success */
/*@unused@*/
int
gstring_new_undef(
	/*@reldef@*/ struct GString *const dest, const size_t len,
	const struct GString_MyAlloc *const my_alloc
)
/*@modifies	*dest@*/
{
	return (len <= (size_t) GSTRING_LOCAL_LIMIT
		? new_undef_local((struct GStringLocal *) dest, len)
		: new_undef_alloc((struct GStringAlloc *) dest, len, my_alloc)
	);
}

/* ------------------------------------------------------------------------ */

/* returns 0 on success */
static int
new_undef_local(
	/*@reldef@*/ struct GStringLocal *const dest, const size_t len
)
/*@modifies	*dest@*/
{
	assert(len <= (size_t) GSTRING_LOCAL_LIMIT);

	dest->len = (uint32_t) len;
	return 0;
}

/* returns 0 on success */
static int
new_undef_alloc(
	/*@reldef@*/ struct GStringAlloc *const dest, const size_t len,
	const struct GString_MyAlloc *const my_alloc
)
/*@modifies	*dest@*/
{
	uint8_t *buf = NULL;

	assert(len > (size_t) GSTRING_LOCAL_LIMIT);

	if ( len > (size_t) UINT32_MAX ){
		/*@-mustdefine@*/
		return -1;
		/*@=mustdefine@*/
	}

	buf = my_alloc->malloc(len);
	if ( buf == NULL ){
		/*@-mustdefine@*/
		return -1;
		/*@=mustdefine@*/
	}
	assert(buf != NULL);

	dest->ptr = buf;
	/* prefix left undefined */
	dest->len = (uint32_t) len;

	return 0;
}

/* EOF //////////////////////////////////////////////////////////////////// */
