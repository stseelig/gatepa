/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// apetag/sort.c                                                            //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <libs/ascii-literals.h>
#include <libs/gstring.h>

#include "../apetag.h"
#include "../attributes.h"
#include "../text.h"

/* //////////////////////////////////////////////////////////////////////// */

PURE
static int cmp_item(
	const struct Gatepa_Tag *, uint32_t, uint32_t, enum Sort_TagCompar
)
/*@*/
;

PURE
static int cmp_item_score(
	const struct Gatepa_Tag *, uint32_t, enum Sort_TagCompar
)
/*@*/
;

PURE
static int cmp_item_score_text(
	const struct Gatepa_Tag *, uint32_t, enum Sort_TagCompar
)
/*@*/
;

PURE
static int cmp_item_score_text_audio(const struct Gatepa_Tag *, uint32_t)
/*@*/
;

PURE
static int get_stak_idx(const uint8_t *, size_t) /*@*/;

PURE
static int cmp_item_fbu(const struct Gatepa_Tag *, uint32_t, uint32_t) /*@*/;

PURE
static uint32_t cmp_item_fbu_size(const struct Gatepa_Tag *, uint32_t) /*@*/;

/* //////////////////////////////////////////////////////////////////////// */

#define ITEMCMPSCORE_TEXT_USCORE	-20
#define ITEMCMPSCORE_TEXT_AUDIO_BASE	-19
#define ITEMCMPSCORE_TEXT		  0
#define ITEMCMPSCORE_LOCATOR		  1
#define ITEMCMPSCORE_FBU	 	  2

/* //////////////////////////////////////////////////////////////////////// */

/* returns like a qsort comparison function */
PURE
GATEPA int
apetag_compar_audio(
	const void *const a, const void *const b, void *const arg
)
/*@*/
{
	const uint32_t a_idx = *((uint32_t *) a);
	const uint32_t b_idx = *((uint32_t *) b);
	const struct Gatepa_Tag *const tag = arg;

	return cmp_item(tag, a_idx, b_idx, TAGCOMPAR_AUDIO);
}

/* returns like a qsort comparison function */
PURE
GATEPA int
apetag_compar_alpha(
	const void *const a, const void *const b, void *const arg
)
/*@*/
{
	const uint32_t a_idx = *((uint32_t *) a);
	const uint32_t b_idx = *((uint32_t *) b);
	const struct Gatepa_Tag *const tag = arg;

	return cmp_item(tag, a_idx, b_idx, TAGCOMPAR_ALPHA);
}

/* ------------------------------------------------------------------------ */

/* returns like a qsort comparison function */
PURE
static int
cmp_item(
	const struct Gatepa_Tag *const tag,
	const uint32_t a_idx, const uint32_t b_idx,
	const enum Sort_TagCompar type
)
/*@*/
{
	const int score_a = cmp_item_score(tag, a_idx, type);
	const int score_b = cmp_item_score(tag, b_idx, type);
	/* * */
/*	int cmp = ((int) (score_a > score_b)) - ((int) (score_a < score_b));
*/	int cmp = score_a - score_b;

	if ( cmp == 0 ){
		switch ( score_a ){
		case ITEMCMPSCORE_FBU:
			cmp = cmp_item_fbu(tag, a_idx, b_idx);
			if ( cmp != 0 ){
				break;
			}
			/*@fallthrough@*/
		default:
			cmp = gstring_cmp_gstring(
				&tag->key[a_idx], &tag->key[b_idx],
				ascii_casecmp
			);
			break;
		}
	}
	return cmp;
}

/*
   0:	custom ordering (not implemented)
   1.a: [TAGCOMPAR_AUDIO] TEXT with an underscore in the key
   1.b:	TEXT: alpha|audio
   2:	LOCATOR: alpha
   3:	FILES, BINARY, & UNKNOWN (FBU): size then alpha
*/
PURE
static int
cmp_item_score(
	const struct Gatepa_Tag *const tag, const uint32_t idx,
	const enum Sort_TagCompar type
)
/*@*/
{
	switch ( tag->item[idx].type ){
	case APEFLAG_ITEMTYPE_TEXT:
		return cmp_item_score_text(tag, idx, type);
	case APEFLAG_ITEMTYPE_LOCATOR:
		return ITEMCMPSCORE_LOCATOR;
	case APEFLAG_ITEMTYPE_BINARY:
	case APEFLAG_ITEMTYPE_UNKNOWN:
	default:
		return ITEMCMPSCORE_FBU;
	}
}

/* returns the item score */
PURE
static int
cmp_item_score_text(
	const struct Gatepa_Tag *const tag, const uint32_t idx,
	const enum Sort_TagCompar type
)
/*@*/
{
	switch ( type ){
	default:
	case TAGCOMPAR_AUDIO:
		return cmp_item_score_text_audio(tag, idx);
	case TAGCOMPAR_ALPHA:
		return ITEMCMPSCORE_TEXT;
	}
}

/* returns the item score */
PURE
static int
cmp_item_score_text_audio(
	const struct Gatepa_Tag *const tag, const uint32_t idx
)
/*@*/
{
	const uint8_t *const key_str = GSTRING_PTR(&tag->key[idx]);
	const size_t         key_len = (size_t) tag->key[idx].len;
	/* * */
	void *temp_ptr;
	int stak_idx;

	/* MAYBE: check if the item is a text file */

	/* check if it is one of the special keys */
	stak_idx = get_stak_idx(key_str, key_len);
	if ( stak_idx >= 0 ){
		return ITEMCMPSCORE_TEXT_AUDIO_BASE + stak_idx;
	}

	/* check for an underscore */
	temp_ptr = memchr(key_str, (int) ASCII_USCORE, key_len);
	if ( temp_ptr != NULL ){
		return ITEMCMPSCORE_TEXT_USCORE;
	}
	else {	return ITEMCMPSCORE_TEXT; }
}

/* returns the index of the key, or -1 on error */
PURE
static int
get_stak_idx(const uint8_t *const data, const size_t len)
/*@*/
{
	#define STAK_TITLE	0u
	#define STAK_ARTST	1u
	#define STAK_CMPSR	2u
	#define STAK_ALBUM	3u
	#define STAK_YEAR	4u
	#define STAK_TRACK	5u
	#define STAK_GENRE	6u
	#define STAK_CMMNT	7u
	/* * */
	#define SCORE_TEXT_AUDIO_KEY_NUM	(STAK_CMMNT + 1u)
	/*@observer@*/
	const uint8_t *stak_key[SCORE_TEXT_AUDIO_KEY_NUM] = {
		(const uint8_t *) u8"title",
		(const uint8_t *) u8"artist",
		(const uint8_t *) u8"composer",
		(const uint8_t *) u8"album",
		(const uint8_t *) u8"year",
		(const uint8_t *) u8"track",
		(const uint8_t *) u8"genre",
		(const uint8_t *) u8"comment"
	};
	const uint8_t  stak_key_len[SCORE_TEXT_AUDIO_KEY_NUM] = {
		UINT8_C(5),	/* title    */
		UINT8_C(6),	/* artist   */
		UINT8_C(8),	/* composer */
		UINT8_C(5),	/* album    */
		UINT8_C(4),	/* year     */
		UINT8_C(5),	/* track    */
		UINT8_C(5),	/* genre    */
		UINT8_C(7),	/* comment  */
	};
	const int8_t stak_idx_table[16u] = {
	#define T	int8_t
	(T) STAK_TITLE,	(T) -1,		(T) STAK_CMMNT,	(T) STAK_GENRE,
	(T) STAK_TRACK,	(T) STAK_TRACK,	(T) -1,		(T) -1,
	(T) STAK_CMPSR,	(T) STAK_ALBUM,	(T) -1,		(T) STAK_ARTST,
	(T) -1,		(T) -1,		(T) STAK_YEAR,	(T) -1
	#undef T
	};

	uint8_t hash = 0;
	int stak_idx;
	size_t i;

	/* hash */
	for ( i = 0; i < len; ++i ){
		hash ^= ascii_tolower(data[i]);
	}
	hash  *= (uint8_t) i;
	hash >>= 1u;
	hash  &= 0xFu;

	/* verify */
	stak_idx = (int) stak_idx_table[hash];
	if ( (stak_idx < 0)
	    ||
	     ((size_t) stak_key_len[stak_idx] != len)
	    ||
	     (memcmp(data, stak_key[stak_idx], len) != 0)
	){
		return -1;
	}
	return stak_idx;
}

/* returns like a qsort comparison function */
PURE
static int
cmp_item_fbu(
	const struct Gatepa_Tag *const tag,
	const uint32_t a_idx, const uint32_t b_idx
)
/*@*/
{
	const uint32_t size_a = cmp_item_fbu_size(tag, a_idx);
	const uint32_t size_b = cmp_item_fbu_size(tag, b_idx);
	/* * */
	int cmp = ((int) (size_a > size_b)) - ((int) (size_a < size_b));

	return cmp;
}

/* returns the size of the item */
PURE
static uint32_t
cmp_item_fbu_size(const struct Gatepa_Tag *const tag, const uint32_t idx)
/*@*/
{
	switch ( tag->item[idx].type ){
	case APEFLAG_ITEMTYPE_BINARY:
		assert(tag->item[idx].nmemb == (uint32_t) 2u);
		return tag->item[idx].value.multi[1u].len;
	default:
		assert(tag->item[idx].nmemb == (uint32_t) 1u);
		return tag->item[idx].value.single.len;
	}
}

/* EOF //////////////////////////////////////////////////////////////////// */
