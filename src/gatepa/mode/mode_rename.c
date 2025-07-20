/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// mode/mode_rename.c                                                       //
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

/* rename$[range]$old_key$new_key[$] */
#define MODE_RENAME_NFIELDS	((size_t) 3u)

/* //////////////////////////////////////////////////////////////////////// */

#undef tag
static enum GatepaErr rename_single(
	struct Gatepa_Tag *tag, const struct GString *, const struct GString *
)
/*@modifies	*tag@*/
;

/* //////////////////////////////////////////////////////////////////////// */

/* rename an item key */
/* returns 0 on success */
GATEPA enum GatepaErr
mode_rename(
	const char *const arg_str, const char arg_sep,
	const struct OpenFiles *const openfiles,
	struct GBitset *const range_gbs
)
/*@modifies	openfiles->tag[],
		*range_gbs
@*/
{
	const size_t       arg_len   = strlen(arg_str);
	const unsigned int num_files = openfiles->nmemb;
	/* * */
	struct GString old_key, new_key;
	/* * */
	size_t arg_idx, size_read;
	union {	int		i;
		enum GatepaErr	gat;
	} err;
	size_t idx;

	MODE_SEP_COUNT(MODE_RENAME_NFIELDS);

	MODE_RANGE_GET(range_gbs, &size_read);
	arg_idx  = size_read;

	MODE_KEY_GET_NOVERIFY(&old_key);
	arg_idx += old_key.len + 1u;

	MODE_KEY_GET(&new_key);

	/* rename the item key in each tag */
	idx = 0;
	goto loop_entr;
	do {	err.gat = rename_single(
			&openfiles->tag[idx], &old_key, &new_key
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

/* ------------------------------------------------------------------------ */

/* returns 0 on success */
static enum GatepaErr
rename_single(
	struct Gatepa_Tag *const tag,
	const struct GString *const old_key,
	const struct GString *const new_key
)
/*@modifies	*tag@*/
{
	const uint32_t item_idx = apetag_memtag_find_item(tag, old_key);

	if ( item_idx != UINT32_MAX ){
		apetag_memtag_rename_item(tag, item_idx, new_key);
	}
	/*@-mustmod@*/
	return 0;
}
	/*@=mustmod@*/

/* EOF //////////////////////////////////////////////////////////////////// */
