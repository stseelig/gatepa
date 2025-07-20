/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// chump/alloc.c                                                            //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

/* //////////////////////////////////////////////////////////////////////// */

#if __STDC_VERSION__ < 201112L
#error "aligned_alloc(): -std=c11"
#endif

/* //////////////////////////////////////////////////////////////////////// */

#undef chump
/*@temp@*/ /*@null@*/ /*@out@*/
static void *alloc_small(struct Chump *chump, uint32_t)
/*@globals	internalState@*/
/*@modifies	internalState,
		*chump
@*/
;

#undef chump
/*@temp@*/ /*@null@*/ /*@out@*/
static void *alloc_small_old(struct Chump *chump, uint32_t, uint32_t)
/*@globals	internalState@*/
/*@modifies	internalState,
		*chump
@*/
;

#undef chump
/*@temp@*/ /*@null@*/ /*@out@*/
static void *alloc_small_new(struct Chump *chump, uint32_t)
/*@globals	internalState@*/
/*@modifies	internalState,
		*chump
@*/
;

#undef chump
/*@temp@*/ /*@null@*/ /*@out@*/
static void *alloc_big(struct Chump *chump, uint32_t)
/*@globals	internalState@*/
/*@modifies	internalState,
		chump->big,
		chump->big[],
		chump->stats.nmemb_big
@*/
;

/* ------------------------------------------------------------------------ */

#undef chump
static int alloc_addto_open_nbytes_avail(struct Chump *chump, uint32_t)
/*@globals	internalState@*/
/*@modifies	internalState,
		chump->open_nbytes_avail,
		chump->open_nbytes_avail[]
@*/
;

#undef chump
static int alloc_moveto_full(struct Chump *chump, uint32_t)
/*@globals	internalState@*/
/*@modifies	internalState,
		chump->open[],
		chump->open_nbytes_avail[],
		chump->full,
		chump->full[],
		chump->stats.nmemb_open,
		chump->stats.nmemb_full
@*/
;

#undef array
#undef nmemb
static int alloc_addto_ptrarray(
	uint8_ownedptr_onlynullptr *array, uint32_t *nmemb,
	/*@owned@*/ /*@reldef@*/ const uint8_t *
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*array,
		(*array)[],
		*nmemb
@*/
;

/* //////////////////////////////////////////////////////////////////////// */

/* returns NULL on failure */
/*@unused@*/
/*@temp@*/ /*@null@*/ /*@out@*/
void *
chump_alloc(struct Chump *const chump, uint32_t size, const uint32_t nmemb)
/*@globals	internalState@*/
/*@modifies	internalState,
		*chump
@*/
{
	void *retval;
	int err_add, err_mul;

	/* align the requested size for aligned_alloc() */
	err_add = mul_u32_overflow(&size, size, nmemb);
	size    = ALIGN_BACKWARDS(size, chump->config.ptr_align);
	err_mul = add_u32_overflow(
		&size, size, (uint32_t) chump->config.ptr_align
	);
	if ( (err_add != 0) || (err_mul != 0) || (size == 0) ){
		return NULL;
	}

	retval = (size < chump->config.big_alloc_min
		? alloc_small(chump, size)
		: alloc_big(chump, size)
	);

	if ( chump->stats.num_allocs != UINT32_MAX ){
		chump->stats.num_allocs += (uint8_t) (retval != NULL);
	}

	return retval;
}

/* ------------------------------------------------------------------------ */

/* returns NULL on failure */
/*@temp@*/ /*@null@*/ /*@out@*/
static void *
alloc_small(struct Chump *const chump, const uint32_t size)
/*@globals	internalState@*/
/*@modifies	internalState,
		*chump
@*/
{
	uint32_t i;

	assert(size != 0);

	/* check the open blocks */
	for ( i = 0; i < chump->stats.nmemb_open; ++i ){
		assert(chump->open_nbytes_avail != NULL);
		if ( size <= chump->open_nbytes_avail[i] ){
			return alloc_small_old(chump, size, i);
		}
	}
	return alloc_small_new(chump, size);
}

/* returns NULL on failure */
/*@temp@*/ /*@null@*/ /*@out@*/
static void *
alloc_small_old(
	struct Chump *const chump, const uint32_t size,
	const uint32_t idx
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*chump
@*/
{
	/*@temp@*/
	void *retval;
	uint32_t nbytes_avail;
	int err;

	assert(popcount_u32((uint32_t) chump->config.ptr_align) == 1u);
	assert(chump->open_nbytes_avail != NULL);
	assert(chump->open != NULL);
	assert(size != 0);
	assert(size % chump->config.ptr_align == 0);

	nbytes_avail = chump->open_nbytes_avail[idx];
	retval = &chump->open[idx][chump->config.block_size - nbytes_avail];

	assert(nbytes_avail >= size);
	nbytes_avail -= size;
	assert(nbytes_avail % chump->config.ptr_align == 0);

	if ( (nbytes_avail >= chump->config.block_full_margin) ){
		chump->open_nbytes_avail[idx] = nbytes_avail;
	}
	else {	err = alloc_moveto_full(chump, idx);
		if ( err != 0 ){
			return NULL;
		}
	}

	assert(((uintptr_t) retval) % chump->config.ptr_align == 0);
	return retval;
}

/* returns NULL on failure */
/*@temp@*/ /*@null@*/ /*@out@*/
static void *
alloc_small_new(struct Chump *const chump, const uint32_t size)
/*@globals	internalState@*/
/*@modifies	internalState,
		*chump
@*/
{
	/*@temp@*/
	void *retval;
	void *result_ptr;
	int err;

	assert(popcount_u32((uint32_t) chump->config.block_size)  == 1u);
	assert(popcount_u32((uint32_t) chump->config.block_align) == 1u);
	assert(popcount_u32((uint32_t) chump->config.ptr_align)   == 1u);
	assert(chump->config.block_size % chump->config.block_align == 0);
	assert(size != 0);
	assert(size % chump->config.ptr_align   == 0);

	/* open_nbytes_avail */
	err = alloc_addto_open_nbytes_avail(chump, size);
	if ( err != 0 ){
		return NULL;
	}
	assert(chump->open_nbytes_avail != NULL);

	/* open */
	result_ptr = aligned_alloc(
		(size_t) chump->config.block_align,
		(size_t) chump->config.block_size
	);
	if ( result_ptr == NULL ){
		return NULL;
	}
	retval = result_ptr;

	/* check if the block is already full (very large 'stats.ptr_align'),
	     then add it to the appropriate array
	*/
	if ( chump->open_nbytes_avail[chump->stats.nmemb_open]
	    >=
	     chump->config.block_full_margin
	){
		err = alloc_addto_ptrarray(
			&chump->open, &chump->stats.nmemb_open, result_ptr
		);
	}
	else {	err = alloc_addto_ptrarray(
			&chump->full, &chump->stats.nmemb_full, result_ptr
		);
	}
	if ( err != 0 ){
		free(retval);
		return NULL;
	}

	assert(((uintptr_t) retval) % chump->config.block_align == 0);
	assert(((uintptr_t) retval) % chump->config.ptr_align   == 0);
	return retval;
}

/* returns NULL on failure */
/*@temp@*/ /*@null@*/ /*@out@*/
static void *
alloc_big(struct Chump *const chump, const uint32_t size)
/*@globals	internalState@*/
/*@modifies	internalState,
		chump->big,
		chump->big[],
		chump->stats.nmemb_big
@*/
{
	/*@temp@*/
	void *retval;
	void *result_ptr;
	int err;

	assert(popcount_u32((uint32_t) chump->config.ptr_align) == 1u);
	assert(size >= chump->config.big_alloc_min);
	assert(size % chump->config.ptr_align == 0);

	result_ptr = aligned_alloc(
		(size_t) chump->config.ptr_align, (size_t) size
	);
	if ( result_ptr == NULL ){
		return NULL;
	}
	retval = result_ptr;
	err    = alloc_addto_ptrarray(
		&chump->big, &chump->stats.nmemb_big, result_ptr
	);
	if ( err != 0 ){
		free(retval);
		return NULL;
	}

	assert(((uintptr_t) retval) % chump->config.ptr_align == 0);
	return retval;
}

/* ------------------------------------------------------------------------ */

/* returns 0 on success */
static int
alloc_addto_open_nbytes_avail(
	struct Chump *const chump, const uint32_t size
)
/*@globals	internalState@*/
/*@modifies	internalState,
		chump->open_nbytes_avail,
		chump->open_nbytes_avail[]
@*/
{
	void *result_ptr;

	/* check if the array needs to be resized (or initialized) */
	if ( ( (chump->stats.nmemb_open != 0)
	      &&
	       (chump->stats.nmemb_open % CHUMP_NMEMB_MOD == 0)
	     )
	    ||
	     (chump->open_nbytes_avail == NULL)
	){
		result_ptr = realloc_addmul_check(
			chump->open_nbytes_avail,
			(uint32_t) (sizeof chump->open_nbytes_avail[0]),
			chump->stats.nmemb_open
		);
		if ( result_ptr == NULL ){
			/*@-usereleased@*/
			return -1;
			/*@=usereleased@*/
		}
		chump->open_nbytes_avail                  = result_ptr;
	}
	assert(chump->open_nbytes_avail != NULL);

	/* update the array */
	assert(chump->config.block_size >= size);
	chump->open_nbytes_avail[chump->stats.nmemb_open] = ALIGN_BACKWARDS(
		chump->config.block_size - size, chump->config.ptr_align
	);

	/* not updating 'stats.nmemb_open' here, in case of some allocator
	     failure later. alloc_addto_ptrarray() handles it
	*/

	return 0;
}

/* returns 0 on success */
static int
alloc_moveto_full(struct Chump *const chump, const uint32_t idx)
/*@globals	internalState@*/
/*@modifies	internalState,
		chump->open[],
		chump->open_nbytes_avail[],
		chump->full,
		chump->full[],
		chump->stats.nmemb_open,
		chump->stats.nmemb_full
@*/
{
	size_t size_memcpy;
	int err;

	assert(chump->open_nbytes_avail != NULL);
	assert(chump->open != NULL);
	assert(chump->stats.nmemb_open != 0);

	/* move the block from 'open' to 'full' */
	err = alloc_addto_ptrarray(
		&chump->full, &chump->stats.nmemb_full, chump->open[idx]
	);
	if ( err != 0 ){
		return err;
	}

	/* reorder the arrays */
	size_memcpy = (size_t) (
		  (sizeof chump->open_nbytes_avail[0])
		* (chump->stats.nmemb_open - idx - 1u)
	);
	if ( size_memcpy != 0 ){
		(void) memmove(
			&chump->open_nbytes_avail[idx],
			&chump->open_nbytes_avail[idx + 1u],
			size_memcpy
		);
	}
	size_memcpy = (size_t) (
		(sizeof chump->open[0]) * (chump->stats.nmemb_open - idx - 1u)
	);
	if ( size_memcpy != 0 ){
		(void) memmove(
			&chump->open[idx], &chump->open[idx + 1u], size_memcpy
		);
	}

	/* update nmemb */
	chump->stats.nmemb_open -= 1u;

	return 0;
}

/* returns 0 on success */
static int
alloc_addto_ptrarray(
	uint8_ownedptr_onlynullptr *const array, uint32_t *const nmemb,
	/*@owned@*/ /*@reldef@*/ const uint8_t *const ptr
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*array,
		(*array)[],
		*nmemb
@*/
{
	void *result_ptr;

	if ( *nmemb == UINT32_MAX ){
		/*@-mustfreeonly@*/
		return -1;
		/*@=mustfreeonly@*/
	}

	/* check if the array needs to be resized (or initialized) */
	if ( ((*nmemb != 0) && (*nmemb % CHUMP_NMEMB_MOD == 0))
	    ||
	     (*array == NULL)
	){
		result_ptr = realloc_addmul_check(
			*array, (uint32_t) (sizeof (*array)[0]), *nmemb
		);
		if ( result_ptr == NULL ){
			/*@-usereleased@*/ /*@-mustfreeonly@*/
			return -1;
			/*@=usereleased@*/ /*@=mustfreeonly@*/
		}
		*array   = result_ptr;
	}
	assert(*array != NULL);

	/* update the array */
	(*array)[*nmemb] = (uint8_t *) ptr;
	*nmemb 		+= 1u;

	return 0;
}

/* EOF //////////////////////////////////////////////////////////////////// */
