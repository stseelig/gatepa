/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// mode/mode_autotrack.c                                                    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <libs/ascii-literals.h>
#include <libs/bitset.h>
#include <libs/gbitset.h>
#include <libs/gstring.h>

#include "../apetag.h"
#include "../attributes.h"
#include "../mode.h"
#include "../open.h"
#include "../utility.h"

#ifndef NDEBUG
#include "../text.h"	/* assert */
#endif

#include "common.h"

/* //////////////////////////////////////////////////////////////////////// */

/* auto-track$[range][$] */
#define MODE_AUTOTRACK_NFIELDS	((size_t) 1u)

/* //////////////////////////////////////////////////////////////////////// */

#undef tag
static enum GatepaErr
autotrack_single(
	struct Gatepa_Tag *tag, unsigned int, unsigned int, unsigned int
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*tag
@*/
;

/* //////////////////////////////////////////////////////////////////////// */

/* add the track number(s) to a group of tag(s) */
/* returns 0 on success */
GATEPA enum GatepaErr
mode_autotrack(
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
	const size_t       arg_len   = strlen(arg_str);
	const unsigned int num_files = openfiles->nmemb;
	/* * */
	unsigned int track_total, track_curr, pow10;
	union {	int		i;
		enum GatepaErr	gat;
	} err;
	size_t idx;

	MODE_SEP_COUNT(MODE_AUTOTRACK_NFIELDS);

	MODE_RANGE_GET(range_gbs, NULL);

	/* autotrack each tag */
	track_total = (unsigned int) bitset_popcount(
		GBITSET_PTR(range_gbs), range_gbs->bitlen
	);
	pow10       = ilog10p1((uintmax_t) track_total);
	track_curr  = 1u;
	idx = 0;
	goto loop_entr;
	do {	err.gat = autotrack_single(
			&openfiles->tag[idx], pow10, track_curr, track_total
		);
		if ( err.gat != 0 ){
			return err.gat;
		}
		track_curr += 1u;
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
autotrack_single(
	struct Gatepa_Tag *const tag, const unsigned int pow10,
	const unsigned int track_curr, const unsigned int track_total
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*tag
@*/
{
	const struct GString key = {
		{ASCII_T_LO, ASCII_R_LO, ASCII_A_LO, ASCII_C_LO,
		 ASCII_K_LO, 0,0,0 ,0,0,0,0
		},
		(uint32_t) 5u
	};
	const uint32_t item_idx = apetag_memtag_find_item(tag, &key);
	/* * */
	struct Gatepa_Item item;
	struct GString value;
	uint8_t buf[64u];
	int num_printed;
	union {	int		i;
		enum GatepaErr	gat;
	} err;

	/* create value */
	num_printed = snprintf((char *) buf, sizeof buf, u8"%0*u/%0*u",
		(int) pow10, track_curr, (int) pow10, track_total
	);
	if ( (num_printed < 3) || (num_printed >= (int) (sizeof buf)) ){
		/*@-mustmod@*/
		return GATERR_LIMIT;
		/*@=mustmod@*/
	}
	err.i = gstring_ref_bstring(&value, buf, (size_t) num_printed);
	if ( err.i != 0 ){
		/*@-mustmod@*/
		return GATERR_STRING;
		/*@=mustmod@*/
	}

	if ( item_idx != UINT32_MAX ){
		/* replace */
		apetag_memitem_replace_value(
			&tag->item[item_idx], &value, APEFLAG_ITEMTYPE_TEXT
		);
	}
	else {	/* add */
		item = gatepa_memitem_init(APEFLAG_ITEMTYPE_TEXT);
		err.gat = apetag_memitem_add_value(&item, &value);
		if ( err.gat != 0 ){
			return err.gat;
		}
		err.gat = apetag_memtag_add_item(tag, &key, &item);
		if ( err.gat != 0 ){
			return err.gat;
		}
	}
	return 0;
}

/* EOF //////////////////////////////////////////////////////////////////// */
