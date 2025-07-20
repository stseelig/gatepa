/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// mode/mode_sort.c                                                         //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <string.h>

#include <libs/bitset.h>
#include <libs/gbitset.h>
#include <libs/gstring.h>

#include "../alloc.h"
#include "../apetag.h"
#include "../attributes.h"
#include "../mode.h"
#include "../open.h"
#include "../text.h"

#include "common.h"

/* //////////////////////////////////////////////////////////////////////// */

/* sort$[range][$] */
#define MODE_SORT_NFIELDS	((size_t) 1u)

/* //////////////////////////////////////////////////////////////////////// */

#undef openfiles
#undef range_gbs
NOINLINE
static enum GatepaErr sort_body(
	const char *, char, const struct OpenFiles *openfiles,
	struct GBitset *range_gbs, enum Sort_TagCompar
)
/*@globals	internalState@*/
/*@modifies	internalState,
		openfiles->tag[],
		*range_gbs
@*/
;

#undef tag
static enum GatepaErr sort_single(struct Gatepa_Tag *tag, enum Sort_TagCompar)
/*@globals	internalState@*/
/*@modifies	internalState,
		*tag
@*/
;

#undef key_array
#undef temp_sorted
static void psort_key(
	struct GString *restrict key_array,
	struct GString *restrict temp_sorted,
	const uint32_t *, size_t
)
/*@modifies	key_array[],
		temp_sorted[]
@*/
;

#undef item_array
#undef temp_sorted
static void psort_item(
	struct Gatepa_Item *item_array, struct Gatepa_Item *temp_sorted,
	const uint32_t *, size_t
)
/*@modifies	item_array[],
		temp_sorted[]
@*/
;

/* //////////////////////////////////////////////////////////////////////// */

/* returns 0 on success */
GATEPA enum GatepaErr
mode_sort_audio(
	const char *const arg_str, const char arg_sep,
	const struct OpenFiles *const openfiles,
	struct GBitset *const range_gbs
)
/*@globals	internalState@*/
/*@modifies	internalState,
		openfiles->tag[],
		*range_gbs
@*/
{
	return sort_body(
		arg_str, arg_sep, openfiles, range_gbs, TAGCOMPAR_AUDIO
	);
}

/* returns 0 on success */
GATEPA enum GatepaErr
mode_sort_alpha(
	const char *const arg_str, const char arg_sep,
	const struct OpenFiles *const openfiles,
	struct GBitset *const range_gbs
)
/*@globals	internalState@*/
/*@modifies	internalState,
		openfiles->tag[],
		*range_gbs
@*/
{
	return sort_body(
		arg_str, arg_sep, openfiles, range_gbs, TAGCOMPAR_ALPHA
	);
}

/* returns 0 on success */
NOINLINE
static enum GatepaErr
sort_body(
	const char *const arg_str, const char arg_sep,
	const struct OpenFiles *const openfiles,
	struct GBitset *const range_gbs, const enum Sort_TagCompar sorttype
)
/*@globals	internalState@*/
/*@modifies	internalState,
		openfiles->tag[],
		*range_gbs
@*/
{
	const size_t       arg_len   = strlen(arg_str);
	const unsigned int num_files = openfiles->nmemb;
	/* * */
	union {	int		i;
		enum GatepaErr	gat;
	} err;
	size_t idx;

	assert(num_files != 0);

	MODE_SEP_COUNT(MODE_SORT_NFIELDS);

	MODE_RANGE_GET(range_gbs, NULL);

	/* sort each tag */
	idx = 0;
	goto loop_entr;
	do {	err.gat = sort_single(&openfiles->tag[idx], sorttype);
		if ( err.gat != 0 ){
			return err.gat;
		}
		idx += 1u;
loop_entr:
		idx  = bitset_find_1(
			GBITSET_PTR(range_gbs), range_gbs->bitlen, idx
		);
	} while ( idx != SIZE_MAX );

	return 0;
}

/* ------------------------------------------------------------------------ */

/* returns 0 on success */
static enum GatepaErr
sort_single(struct Gatepa_Tag *const tag, const enum Sort_TagCompar sorttype)
/*@globals	internalState@*/
/*@modifies	internalState,
		*tag
@*/
{
	void *temp_sorted;
	uint32_t *idx_array;
	int (*compar_fn)(const void *, const void *, void *);
	union {	int		i;
		enum GatepaErr	gat;
	} err;
	size_t temp_size;
	uint32_t i;

	/* create an index array */
	err.i = gatepa_alloc_scratch_reset();
	if ( err.i != 0 ){
		/*@-mustmod@*/
		return GATERR_ALLOCATOR;
		/*@=mustmod@*/
	}
	idx_array = gatepa_alloc_scratch(
		sizeof *idx_array, (size_t) tag->nmemb
	);
	if ( idx_array == NULL ){
		/*@-mustmod@*/
		return GATERR_ALLOCATOR;
		/*@=mustmod@*/
	}
	for ( i = 0; i < tag->nmemb; ++i ){
		idx_array[i] = i;
	}

	/* sort the index array */
	switch ( sorttype ){
	case TAGCOMPAR_AUDIO:
		compar_fn = apetag_compar_audio;
		break;
	case TAGCOMPAR_ALPHA:
		compar_fn = apetag_compar_alpha;
		break;
	}
	qsort_r(
		idx_array, (size_t) tag->nmemb, sizeof idx_array[0],
		compar_fn, tag
	);

	/* sort the parallel arrays by the index array using a temp array */
	temp_size   = (sizeof tag->item[0] > sizeof tag->key[0]
		? sizeof tag->item[0] : sizeof tag->key[0]
	);
	temp_sorted = gatepa_alloc_scratch(temp_size, (size_t) tag->nmemb);
	if ( temp_sorted == NULL ){
		/*@-mustmod@*/
		return GATERR_ALLOCATOR;
		/*@=mustmod@*/
	}
	psort_key(tag->key, temp_sorted, idx_array, tag->nmemb);
	psort_item(tag->item, temp_sorted, idx_array, tag->nmemb);

	return 0;
}

static void
psort_key(
	struct GString *restrict const key_array,
	struct GString *restrict const temp_sorted,
	const uint32_t *const idx_array, const size_t nmemb
)
/*@modifies	key_array[],
		temp_sorted[]
@*/
{
	uint32_t i;
	for ( i = 0; i < nmemb; ++i ){
		temp_sorted[i] = key_array[idx_array[i]];
	}
	(void) memcpy(	/* would have overflown earlier */
		key_array, temp_sorted, (size_t) (sizeof key_array[0] * nmemb)
	);
	return;
}

static void
psort_item(
	struct Gatepa_Item *const item_array,
	struct Gatepa_Item *const temp_sorted,
	const uint32_t *const idx_array, const size_t nmemb
)
/*@modifies	item_array[],
		temp_sorted[]
@*/
{
	uint32_t i;
	for ( i = 0; i < nmemb; ++i ){
		temp_sorted[i] = item_array[idx_array[i]];
	}
	(void) memcpy(	/* would have overflown earlier */
		item_array, temp_sorted,
		(size_t) (sizeof item_array[0] * nmemb)
	);
	return;
}

/* EOF //////////////////////////////////////////////////////////////////// */
