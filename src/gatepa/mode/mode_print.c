/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// mode/mode_print.c                                                        //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <stdio.h>
#include <string.h>

#include <libs/bitset.h>
#include <libs/gbitset.h>

#include "../apetag.h"
#include "../attributes.h"
#include "../mode.h"
#include "../open.h"

#include "common.h"

/* //////////////////////////////////////////////////////////////////////// */

/* print$[range][$] */
#define MODE_PRINT_NFIELDS	((size_t) 1u)

/* //////////////////////////////////////////////////////////////////////// */

#undef openfiles
#undef range_gbs
NOINLINE
static enum GatepaErr print_body(
	const char *, char, const struct OpenFiles *openfiles,
	struct GBitset *range_gbs, enum Print_Type
)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		openfiles->tag[],
		*range_gbs
@*/
;

static void print_single(
	const struct OpenFiles *, unsigned int, enum Print_Type
)
/*@globals	fileSystem@*/
/*@modifies	fileSystem@*/
;

/* //////////////////////////////////////////////////////////////////////// */

GATEPA enum GatepaErr
mode_print_short(
	const char *const arg_str, const char arg_sep,
	const struct OpenFiles *const openfiles,
	struct GBitset *const range_gbs
)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		*range_gbs
@*/
{
	return print_body(
		arg_str, arg_sep, openfiles, range_gbs, PRINTTYPE_SHORT
	);
}

GATEPA enum GatepaErr
mode_print_long(
	const char *const arg_str, const char arg_sep,
	const struct OpenFiles *const openfiles,
	struct GBitset *const range_gbs
)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		*range_gbs
@*/
{
	return print_body(
		arg_str, arg_sep, openfiles, range_gbs, PRINTTYPE_LONG
	);
}

/* ------------------------------------------------------------------------ */

/* returns 0 on success */
NOINLINE
static enum GatepaErr
print_body(
	const char *const arg_str, const char arg_sep,
	const struct OpenFiles *const openfiles,
	struct GBitset *const range_gbs, enum Print_Type type
)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
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

	MODE_SEP_COUNT(MODE_PRINT_NFIELDS);

	MODE_RANGE_GET(range_gbs, NULL);

	/* print each tag */
	idx   = 0;
	goto loop_entr;
	do {	print_single(openfiles, (unsigned int) idx, type);
		idx += 1u;
loop_entr:
		idx  = bitset_find_1(
			GBITSET_PTR(range_gbs), range_gbs->bitlen, idx
		);
	} while ( idx != SIZE_MAX );
	(void) fputc('\n', stdout);

	return 0;
}

static void
print_single(
	const struct OpenFiles *const openfiles, const unsigned int idx,
	enum Print_Type type
)
/*@globals	fileSystem@*/
/*@modifies	fileSystem@*/
{
	(void) fputc('\n', stdout);
	gatepa_print_filename(stdout, openfiles, idx);
	if ( openfiles->tag[idx].nmemb != 0 ){
		if ( type == PRINTTYPE_SHORT ){
			gatepa_print_short(stdout, &openfiles->tag[idx]);
		}
		else {	gatepa_print_long(stdout, &openfiles->tag[idx]); }
	}
	else {	(void) fprintf(stdout, "\t(%s)\n",
			openfiles->info[idx].items_nmemb == 0
				? "tagless" : "empty"
		);
	}
	return;
}

/* EOF //////////////////////////////////////////////////////////////////// */
