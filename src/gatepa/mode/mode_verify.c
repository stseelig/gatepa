/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// mode/mode_verify.c                                                       //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <inttypes.h>
#include <stdint.h>
#include <string.h>

#include <libs/bitset.h>
#include <libs/gbitset.h>
#include <libs/gstring.h>
#include <libs/overflow.h>

#include "../alloc.h"
#include "../apetag.h"
#include "../attributes.h"
#include "../mode.h"
#include "../open.h"
#include "../text.h"

#include "common.h"

/* //////////////////////////////////////////////////////////////////////// */

/* verify$[range][$] */
#define MODE_VERIFY_NFIELDS	((size_t) 1u)

/* //////////////////////////////////////////////////////////////////////// */

#undef tag
static enum GatepaErr verify_single(
	struct Gatepa_Tag *tag, unsigned int, const struct OpenFiles *
)
/*@globals	fileSystem,
		internalState
@*/
/*@modifies	fileSystem,
		internalState
@*/
;

#undef size_out
static int verify_tag_items_size(
	/*@out@*/ uint32_t *size_out, const struct Gatepa_Tag *, uint32_t
)
/*@modifies	*size_out@*/
;

PURE
static uint32_t verify_tag_keys_valid(const struct GString *, uint32_t) /*@*/;

PURE
static uint32_t verify_tag_keys_size(
	const struct GString *, uint32_t, uint32_t
)
/*@*/
;

static enum GatepaErr verify_tag_keys_repeat(const struct GString *, uint32_t)
/*@globals	internalState*/
/*@modifies	internalState*/
;

PURE
static int verify_tag_key_repeat_compar(const void *, const void *, void *)
/*@*/
;

PURE
static uint32_t verify_tag_items(const struct Gatepa_Item *, uint32_t) /*@*/;

PURE
static int verify_tag_items_text(const struct Gatepa_Item *) /*@*/;

PURE
static int verify_tag_items_binary(const struct Gatepa_Item *) /*@*/;

PURE
static int verify_tag_items_unknown(const struct Gatepa_Item *) /*@*/;

/* //////////////////////////////////////////////////////////////////////// */

/* verify tag(s) */
/* returns 0 on success */
GATEPA enum GatepaErr
mode_verify(
	const char *const arg_str, const char arg_sep,
	const struct OpenFiles *const openfiles,
	struct GBitset *const range_gbs
)
/*@globals	fileSystem,
		internalState
@*/
/*@modifies	fileSystem,
		internalState,
		openfiles->tag[],
		*range_gbs
@*/
{
	const size_t       arg_len   = strlen(arg_str);
	const unsigned int num_files = openfiles->nmemb;
	/* * */
	enum GatepaErr retval = 0;
	union {	int		i;
		enum GatepaErr	gat;
	} err;
	size_t idx;

	MODE_SEP_COUNT(MODE_VERIFY_NFIELDS);

	MODE_RANGE_GET(range_gbs, NULL);

	/* verify each item in each tag */
	idx   = 0;
	goto loop_entr;
	do {	assert(idx < (size_t) UINT_MAX);
		err.gat = verify_single(
			&openfiles->tag[idx], (unsigned int) idx, openfiles
		);
		if ( err.gat == GATERR_FAIL ){
			retval = GATERR_FAIL;
		}
		else if ( err.gat != 0 ){
			return err.gat;
		} else{;}
		idx += 1u;
loop_entr:
		idx  = bitset_find_1(
			GBITSET_PTR(range_gbs), range_gbs->bitlen, idx
		);
	} while ( idx != SIZE_MAX );

	return retval;
}

/* ------------------------------------------------------------------------ */

/* returns:
	0 if valid,
	GATERR_FAIL if something is invalid,
	or some other error
*/
static enum GatepaErr
verify_single(
	struct Gatepa_Tag *const tag, unsigned int file_idx,
	const struct OpenFiles *const openfiles
)
/*@globals	fileSystem,
		internalState
@*/
/*@modifies	fileSystem,
		internalState
@*/
{
	unsigned int err_count = 0;
	uint32_t items_size;
	uint32_t result_count;
	union {	int		i;
		enum GatepaErr	gat;
	} err;

	if ( tag->nmemb == 0 ){
		return 0;
	}

	err.gat = gatepa_alloc_scratch_reset();
	if ( err.gat != 0 ){
		return GATERR_ALLOCATOR;
	}

	/* check the items size */
	err.i = verify_tag_items_size(
		&items_size, tag, g_apetag.items_size_softlimit
	);
	if UNLIKELY ( err.i != 0 ){
		if ( err_count == 0 ){
			gatepa_warning_header(openfiles, file_idx);
		}
		(void) fputs("\t- items size soft-limit exceeded ", stderr);
		if ( err.i > 0 ){
			(void) fprintf(stderr, "(%"PRIu32"/%"PRIu32")\n",
				items_size, g_apetag.items_size_softlimit
			);
		}
		else {	(void) fputs("(overflow)\n", stderr); }
		err_count += 1u;
	}

	/* check each item key */
	result_count = verify_tag_keys_valid(tag->key, tag->nmemb);
	if UNLIKELY ( result_count != 0 ){
		if ( err_count == 0 ){
			gatepa_warning_header(openfiles, file_idx);
		}
		(void) fprintf(stderr, "\t- %"PRIu32" invalid item key%s\n",
			result_count,
			(result_count == (uint32_t) 1u ? "" : "s")
		);
		err_count += 1u;
	}
	/* * */
	result_count = verify_tag_keys_size(
		tag->key, tag->nmemb, g_apetag.key_size_softlimit
	);
	if UNLIKELY ( result_count != 0 ){
		if ( err_count == 0 ){
			gatepa_warning_header(openfiles, file_idx);
		}
		(void) fprintf(stderr,
			"\t- %"PRIu32" key%s exceed key size soft-limit "
			"(%"PRIu32")\n",
			result_count,
			(result_count == (uint32_t) 1u ? "" : "s"),
			g_apetag.key_size_softlimit
		);
		err_count += 1u;
	}
	/* * */
	err.gat = verify_tag_keys_repeat(tag->key, tag->nmemb);
	if UNLIKELY ( err.gat == GATERR_FAIL ){
		if ( err_count == 0 ){
			gatepa_warning_header(openfiles, file_idx);
		}
		(void) fputs("\t- repeated item keys\n", stderr);
		err_count += 1u;
	}
	else if ( err.gat != 0 ){
		return err.gat;
	} else{;}

	/* check each item value */
	result_count = verify_tag_items(tag->item, tag->nmemb);
	if UNLIKELY ( result_count != 0 ){
		if ( err_count == 0 ){
			gatepa_warning_header(openfiles, file_idx);
		}
		(void) fprintf(stderr,
			"\t- %"PRIu32" invalid or zero-sized item value%s\n",
			result_count,
			(result_count == (uint32_t) 1u ? "" : "s")
		);
		err_count += 1u;
	}

	return (err_count == 0 ? 0 : GATERR_FAIL);
}

/* returns 0 on success, 1 on overlimit, -1 on fail */
static int
verify_tag_items_size(
	/*@out@*/ uint32_t *const size_out,
	const struct Gatepa_Tag *const tag, const uint32_t limit
)
/*@modifies	*size_out@*/
{
	uint32_t size_items, temp;
	union {	int		i;
		enum GatepaErr	gat;
	} err;

	err.gat = apetag_size_items(&size_items, tag);
	if ( err.gat != 0 ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return -1;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}
	err.i = add_u32_overflow(
		&temp, size_items, (uint32_t) sizeof(struct ApeTag_TagHF)
	);
	if ( err.i != 0 ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return -1;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}

	*size_out = size_items;
	return (size_items <= limit ? 0 : 1);
}

/* returns the number of invalid keys */
PURE
static uint32_t
verify_tag_keys_valid(const struct GString *const keys, const uint32_t nmemb)
/*@*/
{
	uint32_t err_count = 0;
	uint32_t i;

	/* check that all keys are valid */
	for ( i = 0; i < nmemb; ++i ){
		err_count += (uint8_t) (
			verify_key(GSTRING_PTR(&keys[i]), keys[i].len) != 0
		);
	}
	return err_count;
}

/* returns the number of key sizes over the soft-limit */
PURE
static uint32_t
verify_tag_keys_size(
	const struct GString *const keys, const uint32_t nmemb,
	const uint32_t key_size_limit
)
/*@*/
{
	uint32_t err_count = 0;
	uint32_t i;

	for ( i = 0; i < nmemb; ++i ){
		err_count += (uint8_t) (keys[i].len > key_size_limit);
	}
	return err_count;
}

/* returns:
	0 if no keys repeat,
	GATERR_FAIL if some do,
	or some other error
*/
static enum GatepaErr
verify_tag_keys_repeat(const struct GString *const keys, const uint32_t nmemb)
/*@globals	internalState*/
/*@modifies	internalState*/
{
	uint32_t *idx_array;
	int cmp;
	uint32_t i;

	assert(nmemb != 0);

	/* create an index array */
	idx_array = gatepa_alloc_scratch(sizeof idx_array[0], (size_t) nmemb);
	if ( idx_array == NULL ){
		return GATERR_ALLOCATOR;
	}
	for ( i = 0; i < nmemb; ++i ){
		idx_array[i] = i;
	}

	/* sort the index array by the real array */
	qsort_r(
		idx_array, (size_t) nmemb, sizeof idx_array[0],
		verify_tag_key_repeat_compar, (void *) keys
	);

	/* check for repeats */
	for ( i = (uint32_t) 1u; i < nmemb; ++i ){
		cmp = gstring_cmp_gstring(
			&keys[i - 1u], &keys[i], ascii_casecmp
		);
		if ( cmp == 0 ){
			return GATERR_FAIL;
		}
	}
	return 0;
}

/* returns like memcmp() */
PURE
static int
verify_tag_key_repeat_compar(
	const void *const a, const void *const b, void *const arg
)
/*@*/
{
	const uint32_t a_idx = *((uint32_t *) a);
	const uint32_t b_idx = *((uint32_t *) b);
	const struct GString *const keys = arg;

	return gstring_cmp_gstring(&keys[a_idx], &keys[b_idx], ascii_casecmp);
}

/* returns the number of bad items */
PURE
static uint32_t
verify_tag_items(const struct Gatepa_Item *const items, const uint32_t nmemb)
/*@*/
{
	uint32_t err_count = 0;
	uint32_t i;

	for ( i = 0; i < nmemb; ++i ){
		switch ( items[i].type ){
		case APEFLAG_ITEMTYPE_TEXT:
		case APEFLAG_ITEMTYPE_LOCATOR:
			err_count += (uint8_t) (
				verify_tag_items_text(&items[i])    != 0
			);
			/*@switchbreak@*/ break;
		case APEFLAG_ITEMTYPE_BINARY:
			err_count += (uint8_t) (
				verify_tag_items_binary(&items[i])  != 0
			);
			/*@switchbreak@*/ break;
		case APEFLAG_ITEMTYPE_UNKNOWN:
			err_count += (uint8_t) (
				verify_tag_items_unknown(&items[i]) != 0
			);
			/*@switchbreak@*/ break;
		default:
			err_count += 1u;
			/*@switchbreak@*/ break;
		}
	}
	return err_count;
}

/* returns 0 on success */
PURE
static int
verify_tag_items_text(const struct Gatepa_Item *const item)
/*@*/
{
	int err;
	uint32_t i;

	assert( (item->type == APEFLAG_ITEMTYPE_TEXT)
	       ||
	        (item->type == APEFLAG_ITEMTYPE_LOCATOR)
	);
	assert(item->nmemb != 0);

	if ( item->nmemb == (uint32_t) 1u ){
		return verify_value_text(
			GSTRING_PTR(&item->value.single),
			item->value.single.len
		);
	}
	else {	for ( i = 0; i < item->nmemb; ++i ){
			err = verify_value_text(
				GSTRING_PTR(&item->value.multi[i]),
				item->value.multi[i].len
			);
			if ( err != 0 ){
				return err;
			}
		}
		return 0;
	}
}

/* returns 0 on success */
PURE
static int
verify_tag_items_binary(const struct Gatepa_Item *const item)
/*@*/
{
	assert(item->type == APEFLAG_ITEMTYPE_BINARY);
	assert(item->nmemb == (uint32_t) 2u);

	if ( (item->value.multi[0u].len == 0)
	    ||
	     (item->value.multi[1u].len == 0)
	){
		return -1;
	}

	return 0;
}

/* returns 0 on success */
PURE
static int
verify_tag_items_unknown(const struct Gatepa_Item *const item)
/*@*/
{
	uint32_t i;

	assert(item->type == APEFLAG_ITEMTYPE_BINARY);
	assert(item->nmemb != 0);

	if ( item->nmemb == (uint32_t) 1u ){
		if ( item->value.single.len == 0 ){
			return -1;
		}
	}
	else {	for ( i = 0; i < item->nmemb; ++i ){
			if ( item->value.multi[i].len == 0 ){
				return -1;
			}
		}
	}
	return 0;
}

/* EOF //////////////////////////////////////////////////////////////////// */
