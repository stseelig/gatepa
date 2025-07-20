/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// apetag/print.c                                                           //
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

#include <libs/gstring.h>

#include "../apetag.h"
#include "../attributes.h"
#include "../errors.h"
#include "../open.h"
#include "../utility.h"

/* //////////////////////////////////////////////////////////////////////// */

#define PRINT_SHORT_KEY_LIMIT		((size_t)  24u)
#define PRINT_SHORT_VALUE_START		((size_t)  32u)

/* MAYBE: opt this */
#define PRINT_SHORT_VALUE_TEXT_LIMIT	((size_t) 192u)

enum PrintRepeat {
	PRINT_REPEAT_SPACE,
	PRINT_REPEAT_PERIOD,
	PRINT_REPEAT_BACKSPACE
};

/* //////////////////////////////////////////////////////////////////////// */

#undef file
static void print_short_key(FILE *file, const struct Gatepa_Tag *, uint32_t)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		*file
@*/
;

#undef file
static void print_short_value(
	FILE *file, const struct Gatepa_Tag *, uint32_t
)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		*file
@*/
;

#undef file
static void print_short_value_multi(
	FILE *file, const struct Gatepa_Tag *, uint32_t
)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		*file
@*/
;

/* ------------------------------------------------------------------------ */

#undef file
static void print_long_key(FILE *file, const struct Gatepa_Tag *, uint32_t)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		*file
@*/
;

#undef file
static void print_long_value(
	FILE *file, const struct Gatepa_Tag *, uint32_t
)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		*file
@*/
;

#undef file
static void print_long_value_multi(
	FILE *file, const struct Gatepa_Tag *, uint32_t
)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		*file
@*/
;

/* ------------------------------------------------------------------------ */

#undef file
static void print_value_switch(
FILE *file, const struct Gatepa_Tag *, uint32_t, uint32_t, uint32_t,
enum Print_Type
)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		*file
@*/
;

#undef file
static void print_value_descript(
	FILE *file, const enum ApeFlag_ItemType, uint32_t
)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		*file
@*/
;

#undef file
static void print_value_binary(
	FILE *file, const struct Gatepa_Tag *, uint32_t
)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		*file
@*/
;

/* ------------------------------------------------------------------------ */

#undef file
static size_t print_repeat(FILE *file, size_t, enum PrintRepeat)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		*file
@*/
;

/* //////////////////////////////////////////////////////////////////////// */

GATEPA void
gatepa_print_short(FILE *const file, const struct Gatepa_Tag *const tag)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		*file
@*/
{
	uint32_t i;

	for ( i = 0; i < tag->nmemb; ++i ){
		print_short_key(file, tag, i);
		print_short_value(file, tag, i);
	}
	return;
}

/* ------------------------------------------------------------------------ */

static void
print_short_key(
	FILE *const file, const struct Gatepa_Tag *const tag,
	const uint32_t idx
)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		*file
@*/
{
	const size_t str_len  = (tag->key[idx].len > PRINT_SHORT_KEY_LIMIT
		? (size_t) (PRINT_SHORT_KEY_LIMIT - 1u)
		: (size_t) tag->key[idx].len
	);
	const void *const str = GSTRING_PTR(&tag->key[idx]);
	size_t temp_size = 8u + str_len;
	size_t nperiods;

	(void) fputs("        ", file);
	(void) fwrite(str, str_len, (size_t) 1u, file);

	if ( tag->key[idx].len > PRINT_SHORT_KEY_LIMIT ){
		(void) fputs(TERMCOLOR_GRAY"+"TERMCOLOR_DEFAULT, file);
		temp_size += 1u;
	}

	if ( temp_size < PRINT_SHORT_VALUE_START ){
		nperiods = PRINT_SHORT_VALUE_START - temp_size;
		(void) fputs(TERMCOLOR_GRAY, file);
		(void) print_repeat(file, nperiods, PRINT_REPEAT_PERIOD);
		(void) fputs(TERMCOLOR_DEFAULT, file);
	}

	return;
}

static void
print_short_value(
	FILE *const file, const struct Gatepa_Tag *const tag,
	const uint32_t item_idx
)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		*file
@*/
{
	if ( tag->item[item_idx].nmemb == 0 ){
		(void) fputc('\n', file);
	}
	else if ( tag->item[item_idx].nmemb == (uint32_t) 1u ){
		assert(tag->item[item_idx].type != APEFLAG_ITEMTYPE_BINARY);
		print_value_switch(
			file, tag, item_idx, 0, PRINT_SHORT_VALUE_TEXT_LIMIT,
			PRINTTYPE_SHORT
		);
		(void) fputc('\n', file);
	}
	else {	print_short_value_multi(file, tag, item_idx); }

	return;
}

static void
print_short_value_multi(
	FILE *const file, const struct Gatepa_Tag *const tag,
	const uint32_t item_idx
)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		*file
@*/
{
	size_t bs_count, sp_count;
	uint32_t value_idx;

	if ( tag->item[item_idx].type == APEFLAG_ITEMTYPE_BINARY ){
		print_value_binary(file, tag, item_idx);
		return;
	}

	(void) fputs("\b\b\b",file);
	if ( tag->key[item_idx].len
	    >=
	     (uint32_t) (PRINT_SHORT_KEY_LIMIT - 3u)
	){
		(void) fputs("\b"TERMCOLOR_GRAY"+",file);
	}

	value_idx = 0;
	goto loop_entr;
	for ( ; value_idx < tag->item[item_idx].nmemb; ++value_idx ){
		bs_count = (size_t) (ilog10p1((uintmax_t) value_idx) + 1u);
		sp_count = PRINT_SHORT_VALUE_START - bs_count - 1u;
		(void) print_repeat(file, sp_count, PRINT_REPEAT_SPACE);
loop_entr:
		(void) fprintf(file,
			TERMCOLOR_GRAY"[%"PRIu32"]"TERMCOLOR_DEFAULT,
			value_idx
		);
		print_value_switch(
			file, tag, item_idx, value_idx,
			PRINT_SHORT_VALUE_TEXT_LIMIT, PRINTTYPE_SHORT
		);
		(void) fputc('\n', file);
	}

	return;
}

/* ======================================================================== */

GATEPA void
gatepa_print_long(FILE *const file, const struct Gatepa_Tag *const tag)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		*file
@*/
{
	uint32_t i;

	for ( i = 0; i < tag->nmemb; ++i ){
		print_long_key(file, tag, i);
		print_long_value(file, tag, i);
	}
	return;
}

/* ------------------------------------------------------------------------ */

static void
print_long_key(
	FILE *const file, const struct Gatepa_Tag *const tag,
	const uint32_t idx
)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		*file
@*/
{
	(void) fputs("        ""'", file);
	(void) fwrite(
		GSTRING_PTR(&tag->key[idx]), tag->key[idx].len, (size_t) 1u,
		file
	);
	(void) fputs("'\n", file);
	return;
}

static void
print_long_value(
	FILE *const file, const struct Gatepa_Tag *const tag,
	const uint32_t item_idx
)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		*file
@*/
{
	if ( tag->item[item_idx].nmemb == 0 ){
		(void) fputc('\n', file);
	}
	else if ( tag->item[item_idx].nmemb == (uint32_t) 1u ){
		(void) fputs("        ""        ", file);
		assert(tag->item[item_idx].type != APEFLAG_ITEMTYPE_BINARY);
		print_value_switch(
			file, tag, item_idx, 0, UINT32_MAX, PRINTTYPE_LONG
		);
		(void) fputc('\n', file);
	}
	else {	print_long_value_multi(file, tag, item_idx); }

	return;
}

static void
print_long_value_multi(
	FILE *const file, const struct Gatepa_Tag *const tag,
	const uint32_t item_idx
)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		*file
@*/
{
	uint32_t value_idx;

	if ( tag->item[item_idx].type == APEFLAG_ITEMTYPE_BINARY ){
		(void) fputs("        ""        ", file);
		print_value_binary(file, tag, item_idx);
		return;
	}

	value_idx = 0;
	for ( ; value_idx < tag->item[item_idx].nmemb; ++value_idx ){
		(void) fputs("        ""        ", file);
		print_value_switch(
			file, tag, item_idx, value_idx, UINT32_MAX,
			PRINTTYPE_LONG
		);
		(void) fputc('\n', file);
	}

	return;
}

/* ======================================================================== */

static void
print_value_switch(
FILE *const file, const struct Gatepa_Tag *const tag,
	const uint32_t item_idx, const uint32_t value_idx,
	const uint32_t limit, const enum Print_Type print_type
)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		*file
@*/
{
	const enum ApeFlag_ItemType type = tag->item[item_idx].type;
	const void *str;
	uint32_t    str_len;

	assert(tag->item[item_idx].nmemb != 0);
	assert(type != APEFLAG_ITEMTYPE_BINARY);

	str = GSTRING_PTR(tag->item[item_idx].nmemb == (uint32_t) 1u
		? &tag->item[item_idx].value.single
		: &tag->item[item_idx].value.multi[value_idx]
	);
	str_len = (tag->item[item_idx].nmemb == (uint32_t) 1u
		? tag->item[item_idx].value.single.len
		: tag->item[item_idx].value.multi[value_idx].len
	);

	switch ( type ){
	case APEFLAG_ITEMTYPE_TEXT:
		if ( str_len <= limit ){
			(void) fputs(": ", file);
			if ( print_type == PRINTTYPE_LONG ){
				(void) fputc('\'', file);
			}
			(void) fwrite(str, str_len, (size_t) 1u, file);
			if ( print_type == PRINTTYPE_LONG ){
				(void) fputc('\'', file);
			}
		}
		else {	print_value_descript(file, type, str_len); }
		break;
	case APEFLAG_ITEMTYPE_BINARY:
		assert(0);
		break;
	case APEFLAG_ITEMTYPE_LOCATOR:
		if ( str_len <= limit ){
			(void) fputs("@ ", file);
			if ( print_type == PRINTTYPE_LONG ){
				(void) fputc('\'', file);
			}
			(void) fwrite(str, str_len, (size_t) 1u, file);
			if ( print_type == PRINTTYPE_LONG ){
				(void) fputc('\'', file);
			}
		}
		else {	print_value_descript(file, type, str_len); }
		break;
	default:
	case APEFLAG_ITEMTYPE_UNKNOWN:
		print_value_descript(file, type, str_len);
		break;
	}
	return;
}

static void
print_value_descript(
	FILE *const file, const enum ApeFlag_ItemType type,
	const uint32_t size
)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		*file
@*/
{
	const char *name = NULL;

	assert(type != APEFLAG_ITEMTYPE_BINARY);

	switch ( type ){
	case APEFLAG_ITEMTYPE_TEXT:
		name = "TEXT";
		break;
	case APEFLAG_ITEMTYPE_BINARY:
		assert(0);
		break;
	case APEFLAG_ITEMTYPE_LOCATOR:
		name = "LOCATOR";
		break;
	default:
	case APEFLAG_ITEMTYPE_UNKNOWN:
		name = "UNKNOWN";
		break;
	}

	(void) fprintf(file, "# %s - %"PRIu32" bytes", name, size);
	return;
}

static void
print_value_binary(
	FILE *const file, const struct Gatepa_Tag *const tag,
	const uint32_t item_idx
)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		*file
@*/
{
	const struct Gatepa_Item *const item = &tag->item[item_idx];

	assert(item->type  == APEFLAG_ITEMTYPE_BINARY);
	assert(item->nmemb == (uint32_t) 2u);

	(void) fputs("# BINARY (" , file);
	(void) fwrite(
		GSTRING_PTR(&item->value.multi[0u]),
		item->value.multi[0u].len, (size_t) 1u, file
	);
	(void) fprintf(file, ") - %"PRIu32" bytes\n",
		item->value.multi[1u].len
	);
	return;
}

static size_t
print_repeat(
	FILE *const file, const size_t count, const enum PrintRepeat mode
)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		*file
@*/
{
	const char spaces[]     =
		"        ""        ""        ""        "
		"        ""        ""        ""        "
	;
	const char periods[]    =
		"........""........""........""........"
		"........""........""........""........"
	;
	const char backspaces[] = "\b\b\b\b\b\b\b\b""\b\b\b\b\b\b\b\b";
	const char *array;
	size_t limit;

	switch ( mode ){
	case PRINT_REPEAT_SPACE:
		array = spaces;
		limit = (sizeof spaces) - 1u;
		break;
	case PRINT_REPEAT_PERIOD:
		array = periods;
		limit = (sizeof periods) - 1u;
		break;
	case PRINT_REPEAT_BACKSPACE:
		array = backspaces;
		limit = (sizeof backspaces) - 1u;
		break;
	}

	return fwrite(
		array, (count < limit ? count : limit), (size_t) 1u, file
	);
}

/* EOF //////////////////////////////////////////////////////////////////// */
