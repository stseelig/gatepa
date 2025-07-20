/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// apetag/file_slurp.c                                                      //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <assert.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <libs/ascii-literals.h>
#include <libs/byteswap.h>
#include <libs/gstring.h>
#include <libs/nbufio.h>
#include <libs/overflow.h>

#include "../alloc.h"
#include "../apetag.h"
#include "../attributes.h"
#include "../errors.h"
#include "../utility.h"

/* //////////////////////////////////////////////////////////////////////// */

/* opt_g_apetag_strtol() depends on field order */
/*@checkmod@*/
struct ApeTag_Globals g_apetag = {
	.items_size_softlimit	= UINT32_C(1048576),
	.key_size_softlimit	= UINT32_C(     24),

	.binary_name_limit	= UINT32_C(    256),
	.binary_fext_limit	= UINT32_C(     32)
};

/* //////////////////////////////////////////////////////////////////////// */

#undef tag
#undef size_read_out
#undef file
static enum SlurpError slurp_item_blob(
	struct Gatepa_Tag *tag, /*@out@*/ uint32_t *size_read_out,
	const uint8_t *, uint32_t, uint32_t, enum ApeFlag_ItemType, uint32_t
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*size_read_out,
		*tag
@*/
;

#undef gs
#undef size_read_out
static enum SlurpError slurp_item_key(
	/*@out@*/ struct GString *gs, /*@out@*/ uint32_t *size_read_out,
	const uint8_t *, uint32_t, uint32_t
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*gs,
		*size_read_out
@*/
;

#undef item
#undef size_read_out
static enum SlurpError slurp_item_value(
	/*@in@*/ struct Gatepa_Item *item,
	/*@out@*/ uint32_t *size_read_out, const uint8_t *, uint32_t,
	enum ApeFlag_ItemType
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*item,
		*size_read_out
@*/
;

#undef item
#undef size_read_out
static enum SlurpError slurp_item_value_text(
	/*@in@*/ struct Gatepa_Item *item,
	/*@out@*/ uint32_t *size_read_out, const uint8_t *, uint32_t,
	enum ApeFlag_ItemType
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*item,
		*size_read_out
@*/
;

#undef item
#undef size_read_out
static enum SlurpError slurp_item_value_binary(
	/*@in@*/ struct Gatepa_Item *item,
	/*@out@*/ uint32_t *size_read_out, const uint8_t *, uint32_t
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*item,
		*size_read_out
@*/
;

#undef item
#undef size_read_out
static enum SlurpError slurp_item_value_unknown(
	/*@in@*/ struct Gatepa_Item *item,
	/*@out@*/ uint32_t *size_read_out, const uint8_t *, uint32_t
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*item,
		*size_read_out
@*/
;

/* //////////////////////////////////////////////////////////////////////// */

/* TODO: enum returns */
/* returns 0 on success */
GATEPA enum SlurpError
apetag_slurp_tag(
	/*@out@*/ struct Gatepa_Tag *const tag_out,
	const struct Gatepa_FileInfo *const file_info, const nbufio_fd file
)
/*@globals	fileSystem,
		internalState
@*/
/*@modifies	internalState,
		*tag_out
@*/
{
	struct Gatepa_Tag tag = GATEPA_MEMTAG_INIT;
	struct ApeTag_ItemH itemh;
	uint8_t *blob = NULL;
	uint32_t blob_idx, new_idx;
	uint32_t size_read;
	size_t size_result, temp_size, target_size;
	uint32_t item_idx;
	off_t seek_err;
	int err;

	assert(file_info->items_size >= sizeof(struct ApeTag_TagHF));

	/* seek to the start of the items blob */
	seek_err = nbufio_seek(file, file_info->off_items, SEEK_SET);
	if ( seek_err == NBUFIO_OFF_ERROR ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return SLURP_ERR_SEEK;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}

	/* read the blob into a buffer */

	blob = gatepa_alloc_a1(file_info->items_size, (size_t) 1u);
	if ( blob == NULL ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return SLURP_ERR_ALLOCATOR;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}
	assert(blob != NULL);
	temp_size   = (size_t) file_info->items_size;
	size_result = nbufio_read(file, blob, temp_size);
	if ( size_result != temp_size ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return (size_result != NBUFIO_RW_ERROR
			? SLURP_ERR_READ_EOF : SLURP_ERR_READ_SYS
		);
		/*@=mustdefine@*/ /*@=mustmod@*/
	}

	blob_idx = 0;
	for ( item_idx = 0; item_idx < file_info->items_nmemb; ++item_idx ){
		/* item header */
		err = add_u32_overflow(
			&new_idx, blob_idx, (uint32_t) sizeof itemh
		);
		if ( err != 0 ){
			/*@-mustdefine@*/ /*@-mustmod@*/
			return SLURP_ERR_OVERFLOW;
			/*@=mustdefine@*/ /*@=mustmod@*/
		}
		if ( new_idx >= file_info->items_size ){
			/*@-mustdefine@*/ /*@-mustmod@*/
			return SLURP_ERR_TAG_SIZE_MISMATCH;
			/*@=mustdefine@*/ /*@=mustmod@*/
		}
		(void) memcpy(&itemh, &blob[blob_idx], sizeof itemh);
		/* MAYBE: verify item header */
		itemh.size = byteswap_u32_letoh(itemh.size);
		blob_idx   = new_idx;

		/* item */
		assert(file_info->items_size > blob_idx);
		err = slurp_item_blob(
			&tag, &size_read, &blob[blob_idx],
			file_info->items_size - blob_idx, itemh.size,
			APETAG_ITEM_TYPE(itemh.type), item_idx
		);
		if ( err != 0 ){
			/*@-mustdefine@*/ /*@-mustmod@*/
			return err;
			/*@=mustdefine@*/ /*@=mustmod@*/
		}
		err = add_u32_overflow(&blob_idx, blob_idx, size_read);
		if ( err != 0 ){
			/*@-mustdefine@*/ /*@-mustmod@*/
			return SLURP_ERR_OVERFLOW;
			/*@=mustdefine@*/ /*@=mustmod@*/
		}
		if ( blob_idx > file_info->items_size ){
			/*@-mustdefine@*/ /*@-mustmod@*/
			return SLURP_ERR_TAG_SIZE_MISMATCH;
			/*@=mustdefine@*/ /*@=mustmod@*/
		}
	}

	target_size = file_info->items_size - sizeof(struct ApeTag_TagHF);
	if ( blob_idx != target_size ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return SLURP_ERR_TAG_SIZE_MISMATCH;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}

	*tag_out = tag;
	return 0;
}

/* ------------------------------------------------------------------------ */

/* the file should be at the beginning of an item header */
/* returns 0 on success */
static enum SlurpError
slurp_item_blob(
	struct Gatepa_Tag *const tag,
	/*@out@*/ uint32_t *const size_read_out, const uint8_t *const blob,
	const uint32_t blob_limit, const uint32_t value_size,
	const enum ApeFlag_ItemType type, const uint32_t item_idx
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*tag,
		*size_read_out
@*/
{
	struct GString key;
	struct Gatepa_Item item = gatepa_memitem_init(type);
	uint32_t size_blob_read = 0, size_blob_left = blob_limit;
	uint32_t size_read;
	int err;

	/* key */
	err = slurp_item_key(
		&key, &size_read, blob, size_blob_left, item_idx
	);
	if ( err != 0 ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return err;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}
	err = sub_u32_overflow(&size_blob_left, size_blob_left, size_read);
	if ( err != 0 ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return SLURP_ERR_OVERFLOW;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}
	size_blob_read += size_read;

	/* values */
	if ( value_size > size_blob_left ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return SLURP_ERR_TAG_SIZE_MISMATCH;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}
	if ( size_blob_left != 0 ){
		err = slurp_item_value(
			&item, &size_read, &blob[size_blob_read], value_size,
			type
		);
		if ( err != 0 ){
			/*@-mustdefine@*/ /*@-mustmod@*/
			return err;
			/*@=mustdefine@*/ /*@=mustmod@*/
		}
		err = add_u32_overflow(
			&size_blob_read, size_blob_read, size_read
		);
		if ( err != 0 ){
			/*@-mustdefine@*/ /*@-mustmod@*/
			return SLURP_ERR_OVERFLOW;
			/*@=mustdefine@*/ /*@=mustmod@*/
		}
	}

	/* add the item to the tag */
	err = (int) apetag_memtag_add_item(tag, &key, &item);
	if ( err != 0 ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return err;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}

	*size_read_out = size_blob_read;
	return 0;
}

/* returns 0 on success */
static enum SlurpError
slurp_item_key(
	/*@out@*/ struct GString *const gs,
	/*@out@*/ uint32_t *const size_read_out, const uint8_t *const blob,
	const uint32_t blob_limit, const uint32_t item_idx
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*gs,
		*size_read_out
@*/
{
	const uintptr_t nulbyte = (uintptr_t) memchr(
		blob, (int) ASCII_NUL, (size_t) blob_limit
	);
	uint32_t size = (nulbyte == 0
		? blob_limit : (uint32_t) (nulbyte - ((uintptr_t) blob))
	);
	const uint8_t *key = blob;
	uint8_t buf[32u];
	int err;

	if ( nulbyte == 0 ){
		/* give the key a name */
		err = snprintf(
			(char *) buf, sizeof buf, u8"key-%"PRIu32, item_idx
		);
		if ( (err < 0) || (err > (int) sizeof buf) ){
			/*@-mustdefine@*/ /*@-mustmod@*/
			return SLURP_ERR_SNPRINTF;
			/*@=mustdefine@*/ /*@=mustmod@*/
		}
		key  = buf;
		size = (uint32_t) err;
	}

	err = gstring_ref_bstring(gs, key, (size_t) size);
	if ( err != 0 ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return SLURP_ERR_GSTRING;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}

	*size_read_out = size + 1u;
	return 0;
}

/* returns 0 on success */
static enum SlurpError
slurp_item_value(
	/*@in@*/ struct Gatepa_Item *const item,
	/*@out@*/ uint32_t *const size_read_out, const uint8_t *const blob,
	const uint32_t value_size, const enum ApeFlag_ItemType type
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*item,
		*size_read_out
@*/
{
	switch ( type ){
	case APEFLAG_ITEMTYPE_TEXT:
	case APEFLAG_ITEMTYPE_LOCATOR:
		return slurp_item_value_text(
			item, size_read_out, blob, value_size, type
		);
	case APEFLAG_ITEMTYPE_BINARY:
		return slurp_item_value_binary(
			item, size_read_out, blob, value_size
		);
	case APEFLAG_ITEMTYPE_UNKNOWN:
	default:
		return slurp_item_value_unknown(
			item, size_read_out, blob, value_size
		);
	}
}

/* returns 0 on success */
static enum SlurpError
slurp_item_value_text(
	/*@in@*/ struct Gatepa_Item *const item,
	/*@out@*/ uint32_t *const size_read_out, const uint8_t *const blob,
	const uint32_t value_size_total, const enum ApeFlag_ItemType type
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*item,
		*size_read_out
@*/
{
	struct GString gs;
	uintptr_t nulbyte;
	uint32_t value_size;
	uint32_t size_read = 0;
	int err;

	assert( (type == APEFLAG_ITEMTYPE_TEXT)
	       ||
	        (type == APEFLAG_ITEMTYPE_LOCATOR)
	);

	while ( size_read < value_size_total ){
		/* find the size of the value */
		nulbyte = (uintptr_t) memchr(
			&blob[size_read], (int) ASCII_NUL,
			value_size_total - size_read
		);
		value_size = (nulbyte == 0
			? value_size_total - size_read
			: (uint32_t) (nulbyte - (uintptr_t) &blob[size_read])
		);

		/* add the value to the item */
		err = gstring_ref_bstring(&gs, &blob[size_read], value_size);
		if ( err != 0 ){
			/*@-mustdefine@*/ /*@-mustmod@*/
			return SLURP_ERR_GSTRING;
			/*@=mustdefine@*/ /*@=mustmod@*/
		}
		err = (int) apetag_memitem_add_value(item, &gs);
		if ( err != 0 ){
			/*@-mustdefine@*/ /*@-mustmod@*/
			return SLURP_ERR_MEMTAG;/* TODO: change/enum */
			/*@=mustdefine@*/ /*@=mustmod@*/
		}

		size_read += value_size + (uint8_t) (nulbyte != 0);
		if ( value_size > size_read ){
			size_read += 1u;
		}
	}
	assert(size_read == value_size_total);

	item->type = type;
	*size_read_out = size_read;
	return 0;
}

/* returns 0 on success */
static enum SlurpError
slurp_item_value_binary(
	/*@in@*/ struct Gatepa_Item *const item,
	/*@out@*/ uint32_t *const size_read_out, const uint8_t *const blob,
	const uint32_t value_size_total
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*item,
		*size_read_out
@*/
{
	/* needs to be 'static' for gstring_ref_bstring() */
	static const uint8_t ext_fallback[] = GATEPA_FEXT_FALLBACK;
	/* * */
	struct GString gs;
	uintptr_t nulbyte;
	const uint8_t *data_start;
	const uint8_t *fext;
	size_t fname_size = 0, fext_size;
	size_t limit;
	int err;

	/* search for the filename/extension */
	limit = g_apetag.binary_name_limit;
	limit = (limit < (size_t) value_size_total
		? limit : (size_t) value_size_total
	);
	nulbyte = (uintptr_t) memchr(blob, (int) ASCII_NUL, limit);
	/* * */
	data_start = (uint8_t *) (nulbyte + 1u);
	fname_size = (size_t) (nulbyte - ((uintptr_t) blob) + 1u);
	if ( (nulbyte == 0)
	    ||
	     (fname_size > (size_t) g_apetag.binary_name_limit)
	){
		data_start = blob;
		fname_size = 0;
	}

	limit = g_apetag.binary_fext_limit;
	limit = (limit < fname_size ? limit : fname_size);
	fext  = findrchar(blob, (int) ASCII_PERIOD, limit);
	fext_size = (size_t) (nulbyte - ((uintptr_t) fext));
	if ( (nulbyte == 0) || (fext == NULL) || (fext_size <= (size_t) 1u)
	    ||
	     (fext_size > (size_t) g_apetag.binary_fext_limit)
	){
		fext      = ext_fallback;
		fext_size = sizeof ext_fallback;
	}

	/* add the extension to the item */
	err = gstring_ref_bstring(&gs, fext, fext_size);
	if ( err != 0 ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return SLURP_ERR_GSTRING;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}
	err = (int) apetag_memitem_add_value(item, &gs);
	if ( err != 0 ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return SLURP_ERR_MEMTAG;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}

	/* add the data to the item */
	err = gstring_ref_bstring(
		&gs, data_start, value_size_total - fname_size
	);
	if ( err != 0 ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return SLURP_ERR_GSTRING;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}
	err = (int) apetag_memitem_add_value(item, &gs);
	if ( err != 0 ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return SLURP_ERR_MEMTAG;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}

	*size_read_out = value_size_total;
	return 0;
}

/* returns 0 on success */
static enum SlurpError
slurp_item_value_unknown(
	/*@in@*/ struct Gatepa_Item *const item,
	/*@out@*/ uint32_t *const size_read_out, const uint8_t *const blob,
	const uint32_t value_size_total
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*item,
		*size_read_out
@*/
{
	struct GString gs;
	int err;

	/* add the data to the item */
	err = gstring_ref_bstring(&gs, blob, value_size_total);
	if ( err != 0 ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return SLURP_ERR_GSTRING;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}
	err = (int) apetag_memitem_add_value(item, &gs);
	if ( err != 0 ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return SLURP_ERR_MEMTAG;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}

	*size_read_out = value_size_total;
	return 0;
}

/* EOF //////////////////////////////////////////////////////////////////// */
