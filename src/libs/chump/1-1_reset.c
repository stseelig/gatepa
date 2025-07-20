/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// chump/reset.c                                                            //
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

typedef /*@only@*/ /*@null@*/ void *	void_onlynullptr;

/* //////////////////////////////////////////////////////////////////////// */

#undef chump
static int reset_open(struct Chump *chump, uint32_t)
/*@globals	internalState@*/
/*@modifies	internalState,
		*chump
@*/
;

#undef chump
static void reset_open_nbytes_avail(struct Chump *chump, uint32_t)
/*@globals	internalState@*/
/*@modifies	internalState,
		*chump
@*/
;

/* ------------------------------------------------------------------------ */

#undef array
#undef nmemb
static void reset_realloc_array(
	void_onlynullptr *array, uint32_t *nmemb, uint32_t
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*array,
		*nmemb
@*/
;

#undef array
#undef nmemb
static void reset_realloc_ptrarray(
	uint8_ownedptr_onlynullptr *array, uint32_t *nmemb, uint32_t
)
/*@globals	internalState@*/
/*@modifies	internalState,
		(*array)[],
		*array,
		*nmemb
@*/
;

#undef chump
static int reset_movefrom_full(struct Chump *chump, uint32_t)
/*@globals	internalState@*/
/*@modifies	internalState,
		chump->open,
		chump->open[],
		chump->full[],
		chump->stats.nmemb_open,
		chump->stats.nmemb_full
@*/
;

/* //////////////////////////////////////////////////////////////////////// */

/* returns 0 on success */
/*@unused@*/
int
chump_reset(struct Chump *const chump, const uint32_t blocks_kept_max)
/*@globals	internalState@*/
/*@modifies	internalState,
		*chump
@*/
{
	uint32_t blocks_sum, blocks_kept;
	int err;

	/* calc the number of blocks to keep */
	err = add_u32_overflow(
		&blocks_sum, chump->stats.nmemb_open, chump->stats.nmemb_full
	);
	if ( err != 0 ){
		blocks_sum = UINT32_MAX;
	}
	blocks_kept = (blocks_sum < blocks_kept_max
		? blocks_sum : blocks_kept_max
	);

	/* open */
	if ( chump->open != NULL ){
		assert(chump->open_nbytes_avail != NULL);

		err = reset_open(chump, blocks_kept);
		if ( err != 0 ){
			return err;
		}
		reset_open_nbytes_avail(chump, blocks_kept);
	}

	/* full */
	if ( chump->full != NULL ){
		reset_realloc_ptrarray(
			&chump->full, &chump->stats.nmemb_full, 0
		);
	}

	/* big */
	if ( chump->big != NULL ){
		reset_realloc_ptrarray(
			&chump->big, &chump->stats.nmemb_big, 0
		);
	}

	chump->stats.num_allocs	= 0;
	return 0;
}

/* returns 0 on success */
static int
reset_open(struct Chump *const chump, const uint32_t blocks_kept)
/*@globals	internalState@*/
/*@modifies	internalState,
		*chump
@*/
{
	uint32_t diff, count;
	int err;

	if ( blocks_kept < chump->stats.nmemb_open ){
		reset_realloc_ptrarray(
			&chump->open, &chump->stats.nmemb_open, blocks_kept
		);
		/* if the realloc failed, then everything got freed */
	}
	else {	/* move blocks from full to open */
		diff  = blocks_kept - chump->stats.nmemb_open;
		count = (chump->stats.nmemb_full > diff
			? diff : chump->stats.nmemb_full
		);
		if ( count != 0 ){
			err = reset_movefrom_full(chump, count);
			if ( err != 0 ){
				return err;
			}
		}
	}
	return 0;
}

static void
reset_open_nbytes_avail(struct Chump *const chump, const uint32_t blocks_kept)
/*@globals	internalState@*/
/*@modifies	internalState,
		*chump
@*/
{
	uint32_t i;

	if ( blocks_kept < chump->stats.nmemb_open ){
		reset_realloc_array(
			(void_onlynullptr *) &chump->open_nbytes_avail,
			&chump->stats.nmemb_open,
			(uint32_t) (sizeof chump->open_nbytes_avail[0])
		);
	}
	if ( chump->open_nbytes_avail != NULL ){
		/* reset the array values to 'empty' */
		for ( i = 0; i < chump->stats.nmemb_open; ++i ){
			chump->open_nbytes_avail[i] = (
				chump->config.block_size
			);
		}
	}
	else {	/* open/open_nbytes_avail need to be NULL together */
		free(chump->open);
		chump->open		= NULL;
		chump->stats.nmemb_open	= 0;
	}
	return;
}

/* ------------------------------------------------------------------------ */

static void
reset_realloc_array(
	void_onlynullptr *const array, uint32_t *const nmemb,
	const uint32_t sizeof_array0
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*array,
		*nmemb
@*/
{
	const uint32_t nmemb_realloc = ALIGN_BACKWARDS(
		*nmemb, CHUMP_NMEMB_MOD
	);
	void *result_ptr;

	result_ptr = realloc_addmul_check(
		*array, sizeof_array0, nmemb_realloc
	);
	if ( result_ptr == NULL ){
		/*@-usereleased@*/
		free(*array);
		/*@=usereleased@*/
		*nmemb = 0;
	}
	*array = result_ptr;
	return;
}

static void
reset_realloc_ptrarray(
	uint8_ownedptr_onlynullptr *const array, uint32_t *const nmemb,
	const uint32_t idx_start
)
/*@globals	internalState@*/
/*@modifies	internalState,
		(*array)[],
		*array,
		*nmemb
@*/
{
	const uint32_t limit = *nmemb;
	uint32_t i;

	assert(*array != NULL);

	for ( i = idx_start; i < limit; ++i ){
		free((*array)[i]);
	}
	*nmemb = idx_start;

	reset_realloc_array(
		(void_onlynullptr *) array, nmemb,
		(uint32_t) (sizeof (*array)[0])
	);
	return;
}

/* returns 0 on success */
static int
reset_movefrom_full(struct Chump *const chump, const uint32_t count)
/*@globals	internalState@*/
/*@modifies	internalState,
		chump->open,
		chump->open[],
		chump->full[],
		chump->stats.nmemb_open,
		chump->stats.nmemb_full
@*/
{
	void *result_ptr;
	uint32_t nmemb_realloc, size_memcpy;
	int err;

	assert(chump->open != NULL);
	assert(chump->full != NULL);
	assert(count != 0);

	/* overflow checks */
	err = add_u32_overflow(
		&nmemb_realloc, chump->stats.nmemb_open, count
	);
	if ( err != 0 ){
		return err;
	}
	err = mul_u32_overflow(
		&size_memcpy, (uint32_t) (sizeof chump->open[0]), count
	);
	if ( err != 0 ){
		return err;
	}

	/* resize chump->open */
	result_ptr = realloc_addmul_check(
		chump->open, (uint32_t) (sizeof chump->open[0]), nmemb_realloc
	);
	if ( result_ptr == NULL ){
		/*@-usereleased@*/
		return -1;
		/*@=usereleased@*/
	}
	chump->open = result_ptr;

	/* move block pointers from chump->full to chump->open */
	(void) memcpy(
		&chump->open[chump->stats.nmemb_open],
		&chump->full[chump->stats.nmemb_full - count],
		(size_t) size_memcpy
	);
	chump->stats.nmemb_open += count;

	/* chump->full gets resized elsewhere */
	chump->stats.nmemb_full -= count;

	return 0;
}

/* EOF //////////////////////////////////////////////////////////////////// */
