/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// apetag/memtag.c                                                          //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <assert.h>
#include <stddef.h>
#include <string.h>

#include <libs/overflow.h>

#include "../alloc.h"
#include "../apetag.h"
#include "../attributes.h"
#include "../text.h"

/* //////////////////////////////////////////////////////////////////////// */

/*	optimized for my average use case:

	 1: replaygain_album_gain
	 2: replaygain_album_peak
	 3: replaygain_track_gain
	 4: replaygain_track_peak
	 5: title
	 6: artist
	 7: composer
	 8: album
	 9: year
	10: track
	11: genre
	12: comment
*/
#define MEMTAG_NMEMB_MOD	((uint32_t) 12u)

/* //////////////////////////////////////////////////////////////////////// */

/* returns NULL on failure */
/*@temp@*/ /*@null@*/ /*@reldef@*/
static void *
apetag_memtag_realloc(
	/*@temp@*/ /*@null@*/ /*@reldef@*/ void *const ptr,
	/*@out@*/ /*@null@*/ uint32_t *const nmemb_max_out,
	const size_t size_memb, const uint32_t nmemb_old,
	const uint32_t nmemb_mod
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*nmemb_max_out
@*/
{
	void *retval;
	uint32_t nmemb_new;
	int err;

	err = add_u32_overflow(&nmemb_new, nmemb_old, nmemb_mod);
	if ( err != 0 ){
		if ( nmemb_old == UINT32_MAX ){
			/*@-mustdefine@*/
			return NULL;
			/*@=mustdefine@*/
		}
		nmemb_new = UINT32_MAX;
	}

	retval = gatepa_realloc_a16(
		ptr, size_memb, (size_t) nmemb_old, (size_t) nmemb_new
	);
	if ( (nmemb_max_out != NULL) && (retval != NULL) ){
		*nmemb_max_out = nmemb_new;
	}
	return retval;
}

/* ======================================================================== */

/* returns the index of the item, or UINT32_MAX if the item does not exist */
NOINLINE PURE
GATEPA uint32_t
apetag_memtag_find_item(
	const struct Gatepa_Tag *const tag, const struct GString *const key
)
/*@*/
{
	int cmp;
	uint32_t i;

	for ( i = 0; i < tag->nmemb; ++i ){
		cmp = gstring_cmp_gstring(key, &tag->key[i], ascii_casecmp);
		if ( cmp == 0 ){
			return i;
		}
	}
	return UINT32_MAX;
}

/* returns 0 on success */
NOINLINE
GATEPA enum GatepaErr
apetag_memtag_add_item(
	struct Gatepa_Tag *const tag, const struct GString *const key,
	const struct Gatepa_Item *const item
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*tag
@*/
{
	void *new_key_array, *new_item_array;
	uint32_t temp_nmemb;

	/* check if the arrays need to be resized */
	if ( tag->nmemb == tag->nmemb_max ){
		new_key_array  = apetag_memtag_realloc(
			tag->key, NULL, sizeof tag->key[0],
			tag->nmemb, MEMTAG_NMEMB_MOD
		);
		new_item_array = apetag_memtag_realloc(
			tag->item, &temp_nmemb, sizeof tag->item[0],
			tag->nmemb, MEMTAG_NMEMB_MOD
		);
		if ( (new_key_array == NULL) || (new_item_array == NULL) ){
			return GATERR_ALLOCATOR;
		}
		tag->key	 = new_key_array;
		tag->item	 = new_item_array;
		tag->nmemb_max	 = temp_nmemb;
	}
	assert((tag->key != NULL) && (tag->item != NULL));

	/* update the arrays */
	tag->key [tag->nmemb]	 = *key;
	tag->item[tag->nmemb]	 = *item;
	tag->nmemb		+= 1u;

	return 0;
}

GATEPA void
apetag_memtag_rename_item(
	struct Gatepa_Tag *const tag, const uint32_t item_idx,
	const struct GString *const new_key
)
/*@modifies	tag->key[]@*/
{
	assert(item_idx < tag->nmemb);

	tag->key[item_idx] = *new_key;
	return;
}

GATEPA void
apetag_memtag_replace_item(
	struct Gatepa_Tag *const tag, const uint32_t item_idx,
	const struct Gatepa_Item *const new_item
)
/*@modifies	tag->item[]@*/
{
	tag->item[item_idx] = *new_item;
	return;
}

GATEPA void
apetag_memtag_clear(struct Gatepa_Tag *const tag)
/*@modifies	*tag@*/
{
	tag->nmemb = 0;
	return;
}

NOINLINE
GATEPA enum GatepaErr
apetag_memtag_remove_item(struct Gatepa_Tag *const tag, const uint32_t idx)
/*@modifies	*tag@*/
{
	size_t temp_size;
	int err;

	assert(idx < tag->nmemb);

	/* key */
	err = mul_usize_overflow(
		&temp_size, sizeof tag->key[0],
		(size_t) (tag->nmemb - idx - 1u)
	);
	if ( err != 0 ){
		return GATERR_OVERFLOW;
	}
	if ( temp_size != 0 ){
		(void) memmove(
			&tag->key[idx], &tag->key[idx + 1u], temp_size
		);
	}

	/* item */
	err = mul_usize_overflow(
		&temp_size, sizeof tag->item[0],
		(size_t) (tag->nmemb - idx - 1u)
	);
	if ( err != 0 ){
		return GATERR_OVERFLOW;
	}
	if ( temp_size != 0 ){
		(void) memmove(
			&tag->item[idx], &tag->item[idx + 1u], temp_size
		);
	}

	/* nmemb */
	tag->nmemb -= 1u;

	return 0;
}

/* ======================================================================== */

/* returns 0 on success */
NOINLINE
GATEPA enum GatepaErr
apetag_memitem_add_value(
	struct Gatepa_Item *const item, const struct GString *const gs
)
/*@globals	internalState@*/
/*@modifies	internalState,
		item->value,
		item->nmemb
@*/
{
	struct GString *new_multi;

	if ( item->nmemb == 0 ){
		/* single */
		item->value.single = *gs;
	}
	else {	/* multi */
		new_multi = apetag_memtag_realloc(
			item->value.multi, NULL, sizeof item->value.multi[0],
			(item->nmemb == (uint32_t) 1u ? 0u : item->nmemb),
			(uint32_t) 1u
		);
		if ( new_multi == NULL ){
			return GATERR_ALLOCATOR;
		}
		if ( item->nmemb == (uint32_t) 1u ){
			new_multi[0]	= item->value.single;
		}
		new_multi[item->nmemb]	= *gs;
		item->value.multi	= new_multi;
	}
	item->nmemb += 1u;

	return 0;
}

GATEPA void
apetag_memitem_replace_value(
	struct Gatepa_Item *const item, const struct GString *const value,
	const enum ApeFlag_ItemType type
)
/*@modifies	*item@*/
{
	item->value.single	= *value;
	item->nmemb		= (uint32_t) 1u;
	item->type		= type;
	return;
}

/* EOF //////////////////////////////////////////////////////////////////// */
