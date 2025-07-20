/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// mode/mode_add.c                                                          //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <stdint.h>
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

/* add$[range]$key$value[$] */
#define MODE_ADD_NFIELDS	((size_t) 3u)

/* //////////////////////////////////////////////////////////////////////// */

#undef openfiles
#undef range_gbs
NOINLINE
static enum GatepaErr add_body(
	const char *, char, const struct OpenFiles *openfiles,
	struct GBitset *range_gbs, enum ApeFlag_ItemType
)
/*@globals	internalState@*/
/*@modifies	internalState,
		openfiles->tag[],
		*range_gbs
@*/
;

#undef tag
static enum GatepaErr add_single(
	struct Gatepa_Tag *tag, struct Gatepa_Item *item,
	const struct GString *, const struct GString *, enum ApeFlag_ItemType
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*tag,
		*item
@*/
;

/* //////////////////////////////////////////////////////////////////////// */

/* returns 0 on success */
GATEPA enum GatepaErr
mode_add(
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
	return add_body(
		arg_str, arg_sep, openfiles, range_gbs, APEFLAG_ITEMTYPE_TEXT
	);
}

/* returns 0 on success */
GATEPA enum GatepaErr
mode_addloc(
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
	return add_body(
		arg_str, arg_sep, openfiles, range_gbs,
		APEFLAG_ITEMTYPE_LOCATOR
	);
}

/* ------------------------------------------------------------------------ */

/* add/replace a text item to tag(s) */
/* returns 0 on success */
NOINLINE
static enum GatepaErr
add_body(
	const char *const arg_str, const char arg_sep,
	const struct OpenFiles *const openfiles,
	struct GBitset *const range_gbs, const enum ApeFlag_ItemType type
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
	struct Gatepa_Item item = gatepa_memitem_init(type);
	struct GString key;
	struct GString value;
	/* * */
	size_t arg_idx, size_read;
	union {	int		i;
		enum GatepaErr	gat;
	} err;
	size_t idx;

	MODE_SEP_COUNT(MODE_ADD_NFIELDS);

	MODE_RANGE_GET(range_gbs, &size_read);
	arg_idx  = size_read;

	MODE_KEY_GET(&key);
	arg_idx += key.len + 1u;

	MODE_VALUE_GET(&value);

	/* add/replace the item in each tag */
	idx = 0;
	goto loop_entr;
	do {	err.gat = add_single(
			&openfiles->tag[idx], &item, &key, &value, type
		);
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

/* returns 0 on success */
static enum GatepaErr
add_single(
	struct Gatepa_Tag *const tag, struct Gatepa_Item *const item,
	const struct GString *const key, const struct GString *const value,
	const enum ApeFlag_ItemType type
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*tag,
		*item
@*/
{
	const uint32_t item_idx = apetag_memtag_find_item(tag, key);
	enum GatepaErr err;

	if ( item_idx != UINT32_MAX ){
		/* replace */
		apetag_memitem_replace_value(
			&tag->item[item_idx], value, type
		);
	}
	else {	/* add */
		if ( item->nmemb == 0 ){
			err = apetag_memitem_add_value(item, value);
			if ( err != 0 ){
				return err;
			}
		}
		err = apetag_memtag_add_item(tag, key, item);
		if ( err != 0 ){
			return err;
		}
	}
	return 0;
}

/* EOF //////////////////////////////////////////////////////////////////// */
