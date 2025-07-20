/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// mode/mode_extract.c                                                      //
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

#include "../apetag.h"
#include "../attributes.h"
#include "../mode.h"
#include "../open.h"

#include "common.h"

/* //////////////////////////////////////////////////////////////////////// */

/* extract$[range]$key[$] */
#define MODE_EXTRACT_NFIELDS	((size_t) 2u)

/* //////////////////////////////////////////////////////////////////////// */

static void extract_single(const struct Gatepa_Tag *, const struct GString *)
/*@globals	fileSystem@*/
/*@modifies	fileSystem@*/
;

static void extract_single_print(const struct Gatepa_Item *)
/*@globals	fileSystem@*/
/*@modifies	fileSystem@*/
;

/* //////////////////////////////////////////////////////////////////////// */

/* returns 0 on success */
GATEPA enum GatepaErr
mode_extract(
	const char *const arg_str, const char arg_sep,
	const struct OpenFiles *const openfiles,
	struct GBitset *const range_gbs
)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		openfiles->tag[],
		*range_gbs
@*/
{
	const size_t       arg_len   = strlen(arg_str);
	const unsigned int num_files = openfiles->nmemb;
	/* * */
	struct GString key;
	/* * */
	size_t arg_idx, size_read;
	union {	int		i;
		enum GatepaErr	gat;
	} err;
	size_t idx;

	assert(num_files != 0);

	MODE_SEP_COUNT(MODE_EXTRACT_NFIELDS);

	MODE_RANGE_GET(range_gbs, &size_read);
	arg_idx = size_read;

	MODE_KEY_GET(&key);

	/* check that we are only extracting from one tag/file */
	if ( bitset_popcount(GBITSET_PTR(range_gbs), range_gbs->bitlen)
	   !=
	    (size_t) 1u
	){
		return GATERR_SINGLE_TAG_ONLY;
	}

	/* extract the item */
	idx = bitset_find_1(GBITSET_PTR(range_gbs), range_gbs->bitlen, 0);
	assert(idx != SIZE_MAX);
	extract_single(&openfiles->tag[idx], &key);

	return 0;
}

/* ------------------------------------------------------------------------ */

static void
extract_single(
	const struct Gatepa_Tag *const tag, const struct GString *const key
)
/*@globals	fileSystem@*/
/*@modifies	fileSystem@*/
{
	const uint32_t item_idx = apetag_memtag_find_item(tag, key);

	if ( item_idx != UINT32_MAX ){
		extract_single_print(&tag->item[item_idx]);
	}
	/*@-mustmod@*/
	return;
}
	/*@=mustmod@*/

static void
extract_single_print(const struct Gatepa_Item *const item)
/*@globals	fileSystem@*/
/*@modifies	fileSystem@*/
{
	uint32_t i;

	switch ( item->type ){
	case APEFLAG_ITEMTYPE_TEXT:
	case APEFLAG_ITEMTYPE_LOCATOR:
		if ( item->nmemb == (uint32_t) 1u ){
			(void) fwrite(
				GSTRING_PTR(&item->value.single),
				item->value.single.len, (size_t) 1u, stdout
			);
		}
		else {	for ( i = 0; i < item->nmemb; ++i ){
				(void) fwrite(
					GSTRING_PTR(&item->value.multi[i]),
					item->value.multi[i].len, (size_t) 1u,
					stdout
				);
				if ( i + 1u != item->nmemb ){
					(void) putchar('\0');
				}
			}
		}
		break;
	case APEFLAG_ITEMTYPE_BINARY:
		assert(item->nmemb == (uint32_t) 2u);
		(void) fwrite(
			GSTRING_PTR(&item->value.multi[1u]),
			item->value.multi[1u].len, (size_t) 1u, stdout
		);
		break;
	case APEFLAG_ITEMTYPE_UNKNOWN:
		assert(item->nmemb == (uint32_t) 1u);
		(void) fwrite(
			GSTRING_PTR(&item->value.single),
			item->value.single.len, (size_t) 1u, stdout
		);
		break;
	}
}

/* EOF //////////////////////////////////////////////////////////////////// */
