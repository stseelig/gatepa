/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// mode/mode_remove.c                                                       //
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

/* remove$[range]$key[$] */
#define MODE_REMOVE_NFIELDS	((size_t) 2u)

/* //////////////////////////////////////////////////////////////////////// */

#undef tag
static enum GatepaErr remove_single(
	struct Gatepa_Tag *tag, const struct GString *
)
/*@modifies	*tag@*/
;

/* //////////////////////////////////////////////////////////////////////// */

/* remove an item from tag(s) */
/* returns 0 on success */
GATEPA enum GatepaErr
mode_remove(
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
	struct GString key;
	/* * */
	size_t arg_idx, size_read;
	union {	int		i;
		enum GatepaErr	gat;
	} err;
	size_t idx;

	MODE_SEP_COUNT(MODE_REMOVE_NFIELDS);

	MODE_RANGE_GET(range_gbs, &size_read);
	arg_idx = size_read;

	MODE_KEY_GET_NOVERIFY(&key);

	/* remove the item in each tag */
	idx = 0;
	goto loop_entr;
	do {	err.gat = remove_single(&openfiles->tag[idx], &key);
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
remove_single(struct Gatepa_Tag *const tag, const struct GString *const key)
/*@modifies	*tag@*/
{
	const uint32_t item_idx = apetag_memtag_find_item(tag, key);
	enum GatepaErr err;

	if ( item_idx != UINT32_MAX ){
		err = apetag_memtag_remove_item(tag, item_idx);
		if ( err != 0 ){
			return err;
		}
	}
	/*@=mustmod@*/
	return 0;
}
	/*@-mustmod@*/

/* EOF //////////////////////////////////////////////////////////////////// */
