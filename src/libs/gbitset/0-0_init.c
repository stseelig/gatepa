/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// gbitset/init.c                                                           //
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

#include "common.h"

/* //////////////////////////////////////////////////////////////////////// */

#define GBITSET_INIT_LOCAL(x_nbits)		(struct GBitsetLocal) { \
	{0,0,0,0,0,0,0,0,0,0,0,0}, (x_nbits) \
}
#define GBITSET_INIT_ALLOC(x_ptr, x_nbits)	(struct GBitsetAlloc) { \
	(x_ptr), GBITSET_PAD_ZERO, (x_nbits) \
}

/* //////////////////////////////////////////////////////////////////////// */

#undef bs
static int init_local(/*@out@*/ struct GBitsetLocal *bs, uint32_t)
/*@globals	internalState@*/
/*@modifies	internalState,
		*bs
@*/
;

#undef bs
static int init_alloc(
	/*@out@*/ struct GBitsetAlloc *bs, uint32_t,
	const struct GBitset_MyAlloc *
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*bs
@*/
;

/* //////////////////////////////////////////////////////////////////////// */

/* returns 0 on success */
/*@unused@*/
int
gbitset_init(
	/*@out@*/ struct GBitset *const bs, const uint32_t nbits,
	const struct GBitset_MyAlloc *const my_alloc
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*bs
@*/
{
	if ( nbits == 0 ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return -1;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}

	return (nbits <= GBITSET_LOCAL_LIMIT
		? init_local((struct GBitsetLocal *) bs, nbits)
		: init_alloc((struct GBitsetAlloc *) bs, nbits, my_alloc)
	);
}

/* ------------------------------------------------------------------------ */

/* returns 0 on success */
static int
init_local(/*@out@*/ struct GBitsetLocal *const bs, const uint32_t nbits)
/*@globals	internalState@*/
/*@modifies	internalState,
		*bs
@*/
{
	assert(nbits <= GBITSET_LOCAL_LIMIT);
	assert(nbits != 0);

	*bs = GBITSET_INIT_LOCAL(nbits);
	return 0;
}

/* returns 0 on success */
static int
init_alloc(
	/*@out@*/ struct GBitsetAlloc *const bs, const uint32_t nbits,
	const struct GBitset_MyAlloc *const my_alloc
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*bs
@*/
{
	void *result_ptr;

	assert(nbits > GBITSET_LOCAL_LIMIT);

	result_ptr = my_alloc->calloc(GBITSET_BYTELEN(nbits), (size_t) 1u);
	if ( result_ptr == NULL ){
		/*@-mustdefine@*/ /*@-mustdefine@*/
		return -1;
		/*@=mustdefine@*/ /*@=mustdefine@*/
	}

	*bs = GBITSET_INIT_ALLOC(result_ptr, nbits);
	return 0;
}

/* EOF //////////////////////////////////////////////////////////////////// */
