/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// gstring/new_fini.c                                                       //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "common.h"

/* //////////////////////////////////////////////////////////////////////// */

#undef gs
static void mod_fini_alloc(/*@partial@*/ struct GStringAlloc *gs)
/*@modifies	gs->prefix@*/
;

/* //////////////////////////////////////////////////////////////////////// */

/*@unused@*/
void
gstring_mod_fini(/*@partial@*/ struct GString *const gs)
/*@modifies	*gs@*/
{
	if ( GSTRING_IS_ALLOC(*gs) ){
		mod_fini_alloc((struct GStringAlloc *) gs);
	}
	/*@-mustmod@*/
	return;
}
	/*@=mustmod@*/

/* ------------------------------------------------------------------------ */

static void
mod_fini_alloc(/*@partial@*/ struct GStringAlloc *const gs)
/*@modifies	gs->prefix@*/
{
	assert(GSTRING_IS_ALLOC(*gs));

	(void) memcpy(gs->prefix, gs->ptr, GSTRING_PREFIX_SIZE);
	return;
}

/* EOF //////////////////////////////////////////////////////////////////// */
