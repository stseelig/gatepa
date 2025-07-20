/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// apetag/file_write.c                                                      //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <libs/ascii-literals.h>
#include <libs/gstring.h>
#include <libs/nbufio.h>
#include <libs/overflow.h>

#include "../apetag.h"
#include "../attributes.h"
#include "../errors.h"

/* //////////////////////////////////////////////////////////////////////// */

#undef size_out
static enum GatepaErr apetag_size_item(
	/*@out@*/ uint32_t *size_out, const struct Gatepa_Tag *, uint32_t idx
)
/*@modifies	*size_out@*/
;

#undef size_out
static enum GatepaErr apetag_size_item_values(
	/*@out@*/ uint32_t *size_out, const struct Gatepa_Item *
)
/*@modifies	*size_out@*/
;

#undef buf
static size_t apetag_construct_item(
	/*@out@*/ uint8_t *buf, size_t, const struct Gatepa_Tag *, uint32_t
)
/*@modifies	*buf@*/
;

/* //////////////////////////////////////////////////////////////////////// */

/* returns 0 on success */
GATEPA enum GatepaErr
apetag_size_items(
	/*@out@*/ uint32_t *const size_out, const struct Gatepa_Tag *const tag
)
/*@modifies	*size_out@*/
{
	uint32_t size_total = 0;
	uint32_t size_item;
	union {	int		i;
		enum GatepaErr	gat;
	} err;
	uint32_t i;

	for ( i = 0; i < tag->nmemb; ++i ){
		err.gat = apetag_size_item(&size_item, tag, i);
		if ( err.gat != 0 ){
			/*@-mustdefine@*/ /*@-mustmod@*/
			return err.gat;
			/*@=mustdefine@*/ /*@=mustmod@*/
		}
		err.i = add_u32_overflow(&size_total, size_total, size_item);
		if ( err.i != 0 ){
			/*@-mustdefine@*/ /*@-mustmod@*/
			return GATERR_OVERFLOW;
			/*@=mustdefine@*/ /*@=mustmod@*/
		}
	}

	*size_out = size_total;
	return 0;
}

/* returns 0 on success */
static enum GatepaErr
apetag_size_item(
	/*@out@*/ uint32_t *const size_out,
	const struct Gatepa_Tag *const tag, const uint32_t idx
)
/*@modifies	*size_out@*/
{
	uint32_t size_total = 0;
	uint32_t size_values;
	union {	int		i;
		enum GatepaErr	gat;
	} err;

	/* header */
	err.i = add_u32_overflow(
		&size_total, size_total,
		(uint32_t) sizeof(struct ApeTag_ItemH)
	);
	if ( err.i != 0 ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return GATERR_OVERFLOW;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}

	/* key */
	err.i = add_u32_overflow(&size_total, size_total, tag->key[idx].len);
	if ( err.i != 0 ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return GATERR_OVERFLOW;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}
	size_total += 1u;	/* nul-byte */
	if ( size_total == 0 ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return GATERR_OVERFLOW;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}

	/* value(s) */
	err.gat = apetag_size_item_values(&size_values, &tag->item[idx]);
	if ( err.gat != 0 ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return err.gat;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}
	err.i = add_u32_overflow(&size_total, size_total, size_values);
	if ( err.i != 0 ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return GATERR_OVERFLOW;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}

	*size_out = size_total;
	return 0;
}

/* returns 0 on success */
static enum GatepaErr
apetag_size_item_values(
	/*@out@*/ uint32_t *const size_out,
	const struct Gatepa_Item *const item
)
/*@modifies	*size_out@*/
{
	uint32_t size_total = 0;
	uint32_t size_value;
	int err;
	uint32_t i;

	assert(item->nmemb != 0);

	if ( item->nmemb == (uint32_t) 1u ){
		size_value = item->value.single.len;
		assert(size_value != 0);
		size_total = size_value;
	}
	else {	for ( i = 0; i < item->nmemb; ++i ){
			if ( i != 0 ){
				size_total += 1u;	/* nul-byte */
				if ( size_total == 0 ){
					/*@-mustdefine@*/ /*@-mustmod@*/
					return GATERR_OVERFLOW;
					/*@=mustdefine@*/ /*@=mustmod@*/
				}
			}
			size_value = item->value.multi[i].len;
			assert(size_value != 0);
			err = add_u32_overflow(
				&size_total, size_total, size_value
			);
			if ( err != 0 ){
				/*@-mustdefine@*/ /*@-mustmod@*/
				return GATERR_OVERFLOW;
				/*@=mustdefine@*/ /*@=mustmod@*/
			}
		}
	}

	*size_out = size_total;
	return 0;
}

/* ======================================================================== */

/* returns 0 on success */
GATEPA enum GatepaErr
apetag_write_tag_header(
	const nbufio_fd fd, const uint32_t tag_size, const uint32_t tag_nmemb
)
/*@globals	fileSystem,
		internalState
@*/
/*@modifies	fileSystem,
		internalState
@*/
{
	struct ApeTag_TagHF header;
	size_t err;

	header = apetag_taghf_make(	/* TODO: short tag opt */
		tag_size, tag_nmemb, APEFLAG_NO_READONLY,
		APEFLAG_IS_HEADER | APEFLAG_HAS_FOOTER | APEFLAG_HAS_HEADER
	);
	err = nbufio_write(fd, &header, sizeof header);

	return (err != NBUFIO_RW_ERROR ? 0 : GATERR_IO_WRITE);
}

/* returns the number of bytes written to the buffer */
/* not overflow checking here, because we should be good */
GATEPA size_t
apetag_construct_tag(
	/*@out@*/ uint8_t *const buf, const size_t buf_size,
	const struct Gatepa_Tag *const tag, const enum Write_TagType type
)
/*@modifies	*buf@*/
{
	struct ApeTag_TagHF hf;
	unsigned int has_header;
	size_t buf_idx = 0;
	uint32_t i;

	/* write each item */
	for ( i = 0; i < tag->nmemb; ++i ){
		buf_idx = apetag_construct_item(buf, buf_idx, tag, i);
	}

	/* create/write tag footer */
	has_header = (type == TAGTYPE_LONG ? APEFLAG_HAS_HEADER : 0);
	hf = apetag_taghf_make(
		buf_size, tag->nmemb, APEFLAG_NO_READONLY,
		APEFLAG_IS_FOOTER | APEFLAG_HAS_FOOTER | has_header
	);
	(void) memcpy(&buf[buf_idx], &hf, sizeof hf);
	buf_idx += sizeof hf;

	return buf_idx;
}

/* ------------------------------------------------------------------------ */

/* returns the new 'buf_idx' */
/* not overflow checking here, because we should be good */
static size_t
apetag_construct_item(
	/*@out@*/ uint8_t *const buf, size_t buf_idx,
	const struct Gatepa_Tag *const tag, const uint32_t idx
)
/*@modifies	*buf@*/
{
	const struct GString     *const key  = &tag->key[idx];
	const struct Gatepa_Item *const item = &tag->item[idx];
	/* * */
	struct ApeTag_ItemH header;
	uint32_t item_size;
	enum GatepaErr err;
	uint32_t i;

	assert(item->nmemb != 0);

	/* size */
	err = apetag_size_item_values(&item_size, &tag->item[idx]);
	if ( err != 0 ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return 0;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}

	/* header */
	header = apetag_itemh_make(item_size, item->type);
	(void) memcpy(&buf[buf_idx], &header, sizeof header);
	buf_idx += sizeof header;

	/* key */
	(void) memcpy(&buf[buf_idx], GSTRING_PTR(key), (size_t) key->len);
	buf_idx += key->len;
	buf[buf_idx++] = ASCII_NUL;

	/* value(s) */
	if ( item->nmemb == (uint32_t) 1u ){
		(void) memcpy(
			&buf[buf_idx], GSTRING_PTR(&item->value.single),
			item->value.single.len
		);
		buf_idx += item->value.single.len;
	}
	else {	for ( i = 0; i < item->nmemb; ++i ){
			if ( i != 0 ){
				buf[buf_idx++] = ASCII_NUL;
			}
			(void) memcpy(
				&buf[buf_idx],
				GSTRING_PTR(&item->value.multi[i]),
				item->value.multi[i].len
			);
			buf_idx += item->value.multi[i].len;
		}
	}

	return buf_idx;
}

/* EOF //////////////////////////////////////////////////////////////////// */
