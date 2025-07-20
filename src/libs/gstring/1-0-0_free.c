/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// gstring/free_unchecked.c                                                 //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <assert.h>

#include "common.h"

/* //////////////////////////////////////////////////////////////////////// */

#undef gs
static void free_alloc(
	struct GStringAlloc *gs, const struct GString_MyAlloc *
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*gs
@*/
;

/* //////////////////////////////////////////////////////////////////////// */

/*@unused@*/
void
gstring_free(
	struct GString *const gs,
	const struct GString_MyAlloc *const my_alloc
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*gs
@*/
{
	if ( GSTRING_IS_ALLOC(*gs) ){
		free_alloc((struct GStringAlloc *) gs, my_alloc);
	}
	/*@-mustmod@*/
	return;
}
	/*@=mustmod@*/

/* ------------------------------------------------------------------------ */

static void
free_alloc(
	struct GStringAlloc *const gs,
	const struct GString_MyAlloc *const my_alloc
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*gs
@*/
{
	assert(GSTRING_IS_ALLOC(*gs));

	if ( my_alloc->free != GSTRING_NULL_FNPTR_FREE ){
		/*@-noeffectuncon@*/
		my_alloc->free(gs->ptr);
		/*@=noeffectuncon@*/
	}
	return;
}

/* EOF //////////////////////////////////////////////////////////////////// */
