/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// gbitset/free.c                                                           //
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

#undef bs
static void free_alloc(
	struct GBitsetAlloc *bs, const struct GBitset_MyAlloc *
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*bs
@*/
;

/* //////////////////////////////////////////////////////////////////////// */

/*@unused@*/
void
gbitset_free(
	struct GBitset *const bs, const struct GBitset_MyAlloc *const my_alloc
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*bs
@*/
{
	if ( GBITSET_IS_ALLOC(*bs) ){
		free_alloc((struct GBitsetAlloc *) bs, my_alloc);
	}
	return;
}

/* ------------------------------------------------------------------------ */

static void
free_alloc(
	struct GBitsetAlloc *const bs,
	const struct GBitset_MyAlloc *const my_alloc
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*bs
@*/
{
	assert(GBITSET_IS_ALLOC(*bs));

	if ( my_alloc->free != GBITSET_NULL_FNPTR_FREE ){
		/*@-noeffectuncon@*/
		my_alloc->free(bs->ptr);
		/*@=noeffectuncon@*/
	}
	return;
}

/* EOF //////////////////////////////////////////////////////////////////// */
