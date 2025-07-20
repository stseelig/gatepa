/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// mode/mode_clear.c                                                        //
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

#include "../apetag.h"
#include "../attributes.h"
#include "../mode.h"
#include "../open.h"

#include "common.h"

/* //////////////////////////////////////////////////////////////////////// */

/* clear$[range][$] */
#define MODE_CLEAR_NFIELDS	((size_t) 1u)

/* //////////////////////////////////////////////////////////////////////// */

#undef tag
static void clear_single(struct Gatepa_Tag *tag)
/*@modifies	*tag@*/
;

/* //////////////////////////////////////////////////////////////////////// */

/* returns 0 on success */
GATEPA enum GatepaErr
mode_clear(
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
	union {	int		i;
		enum GatepaErr	gat;
	} err;
	size_t idx;

	assert(num_files != 0);

	MODE_SEP_COUNT(MODE_CLEAR_NFIELDS);

	MODE_RANGE_GET(range_gbs, NULL);

	/* clear each tag */
	idx = 0;
	goto loop_entr;
	do {	clear_single(&openfiles->tag[idx]);
		idx += 1u;
loop_entr:
		idx  = bitset_find_1(
			GBITSET_PTR(range_gbs), range_gbs->bitlen, idx
		);
	} while ( idx != SIZE_MAX );

	return 0;
}

/* ------------------------------------------------------------------------ */

static void
clear_single(struct Gatepa_Tag *const tag)
/*@modifies	*tag@*/
{
	apetag_memtag_clear(tag);
	return;
}

/* EOF //////////////////////////////////////////////////////////////////// */
