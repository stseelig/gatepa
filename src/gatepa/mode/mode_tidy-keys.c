/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// mode/mode_tidy-keys.c                                                    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <string.h>

#include <libs/ascii-literals.h>
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

/* tidy-keys$[range][$] */
#define MODE_TIDYKEYS_NFIELDS	((size_t) 1u)

/* //////////////////////////////////////////////////////////////////////// */

typedef void (*tidykeys_fnptr)(struct GString *);

/* //////////////////////////////////////////////////////////////////////// */

#undef openfiles
#undef range_gbs
NOINLINE
static enum GatepaErr
tidykeys_body(
	const char *, char, const struct OpenFiles *openfiles,
	struct GBitset *range_gbs, tidykeys_fnptr
)
/*@modifies	openfiles->tag[],
		*range_gbs
@*/
;

#undef tag
static void tidykeys_single(struct Gatepa_Tag *tag, tidykeys_fnptr)
/*@modifies	tag->key[]@*/
;

#undef key
static void tidykeys_single_lo(struct GString *key)
/*@modifies	*key@*/
;

#undef key
static void tidykeys_single_up(struct GString *key)
/*@modifies	*key@*/
;

#undef key
static void tidykeys_single_1up(struct GString *key)
/*@modifies	*key@*/
;

/* //////////////////////////////////////////////////////////////////////// */

/* returns 0 on success */
GATEPA enum GatepaErr
mode_tidykeys_lo(
	const char *const arg_str, const char arg_sep,
	const struct OpenFiles *const openfiles,
	struct GBitset *const range_gbs
)
/*@modifies	openfiles->tag[],
		*range_gbs
@*/
{
	return tidykeys_body(
		arg_str, arg_sep, openfiles, range_gbs, tidykeys_single_lo
	);
}

/* returns 0 on success */
GATEPA enum GatepaErr
mode_tidykeys_up(
	const char *const arg_str, const char arg_sep,
	const struct OpenFiles *const openfiles,
	struct GBitset *const range_gbs
)
/*@modifies	openfiles->tag[],
		*range_gbs
@*/
{
	return tidykeys_body(
		arg_str, arg_sep, openfiles, range_gbs, tidykeys_single_up
	);
}

/* returns 0 on success */
GATEPA enum GatepaErr
mode_tidykeys_1up(
	const char *const arg_str, const char arg_sep,
	const struct OpenFiles *const openfiles,
	struct GBitset *const range_gbs
)
/*@modifies	openfiles->tag[],
		*range_gbs
@*/
{
	return tidykeys_body(
		arg_str, arg_sep, openfiles, range_gbs, tidykeys_single_1up
	);
}

/* returns 0 on success */
NOINLINE
static enum GatepaErr
tidykeys_body(
	const char *const arg_str, const char arg_sep,
	const struct OpenFiles *const openfiles,
	struct GBitset *const range_gbs, const tidykeys_fnptr fn
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

	MODE_SEP_COUNT(MODE_TIDYKEYS_NFIELDS);

	MODE_RANGE_GET(range_gbs, NULL);

	/* tidy each key */
	idx = 0;
	goto loop_entr;
	do {	tidykeys_single(&openfiles->tag[idx], fn);
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
tidykeys_single(struct Gatepa_Tag *const tag, const tidykeys_fnptr fn)
/*@modifies	tag->key[]@*/
{
	uint32_t i;

	for ( i = 0; i < tag->nmemb; ++i ){
		/*@-noeffectuncon@*/
		fn(&tag->key[i]);
		/*@=noeffectuncon@*/
		gstring_mod_fini(&tag->key[i]);
	}
	return;
}

static void
tidykeys_single_lo(struct GString *const key)
/*@modifies	*key@*/
{
	uint8_t *const ptr = GSTRING_PTR(key);
	uint32_t i;

	for ( i = 0; i < key->len; ++i ){
		ptr[i] = ascii_tolower(ptr[i]);
	}
	return;
}

static void
tidykeys_single_up(struct GString *const key)
/*@modifies	*key@*/
{
	uint8_t *const ptr = GSTRING_PTR(key);
	uint32_t i;

	for ( i = 0; i < key->len; ++i ){
		ptr[i] = ascii_toupper(ptr[i]);
	}
	return;
}

static void
tidykeys_single_1up(struct GString *const key)
/*@modifies	*key@*/
{
	uint8_t *const ptr = GSTRING_PTR(key);
	uint8_t c = ASCII_SP;
	uint32_t i;

	for ( i = 0; i < key->len; ++i ){
		if ( (c == ASCII_SP) || (c == ASCII_USCORE) ){
			ptr[i] = ascii_toupper(ptr[i]);
		}
		else {	ptr[i] = ascii_tolower(ptr[i]); }
		c = ptr[i];
	}
	return;
}

/* EOF //////////////////////////////////////////////////////////////////// */
