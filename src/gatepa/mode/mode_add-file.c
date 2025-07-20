/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// mode/mode_add-file.c                                                     //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <stdint.h>
#include <string.h>

#include <libs/bitset.h>
#include <libs/gbitset.h>
#include <libs/gstring.h>

#include "../alloc.h"
#include "../apetag.h"
#include "../attributes.h"
#include "../mode.h"
#include "../open.h"
#include "../utility.h"

#include "common.h"

/* //////////////////////////////////////////////////////////////////////// */

/* add-file$[range]$key$path[$] */
#define MODE_ADDFILE_NFIELDS	((size_t) 3u)

/* //////////////////////////////////////////////////////////////////////// */

#undef item
static enum GatepaErr addfile_item_construct(
	/*@out@*/ struct Gatepa_Item *item, const char *
)
/*@globals	fileSystem,
		internalState
@*/
/*@modifies	internalState,
		*item
@*/
;

#undef file_ext
static enum GatepaErr addfile_ext_get(
	/*@out@*/ struct GString *file_ext, const char *, size_t
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*file_ext
@*/
;

#undef tag
static enum GatepaErr addfile_single(
	struct Gatepa_Tag *tag, struct Gatepa_Item *item,
	const struct GString *
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*tag
@*/
;

/* //////////////////////////////////////////////////////////////////////// */

/* add/replace a file/binary-item to tag(s) */
/* returns 0 on success */
GATEPA enum GatepaErr
mode_addfile(
	const char *const arg_str, const char arg_sep,
	const struct OpenFiles *const openfiles,
	struct GBitset *const range_gbs
)
/*@globals	fileSystem,
		internalState
@*/
/*@modifies	internalState,
		openfiles->tag[],
		*range_gbs
@*/
{
	const size_t       arg_len   = strlen(arg_str);
	const unsigned int num_files = openfiles->nmemb;
	/* * */
	struct GString key;
	struct Gatepa_Item item;
	char *path = NULL;
	/* * */
	size_t arg_idx, size_read;
	union {	int		i;
		enum GatepaErr	gat;
	} err;
	size_t idx;

	if ( arg_sep == (char) FILE_PATH_SEP ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return GATERR_MODESTR_SEP;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}

	err.i = gatepa_alloc_scratch_reset();	/* for *path */
	if ( err.i != 0 ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return GATERR_ALLOCATOR;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}

	MODE_SEP_COUNT(MODE_ADDFILE_NFIELDS);

	MODE_RANGE_GET(range_gbs, &size_read);
	arg_idx  = size_read;

	MODE_KEY_GET(&key);
	arg_idx += key.len + 1u;

	MODE_PATH_GET(&path);

	err.gat = addfile_item_construct(&item, path);
	if ( err.gat != 0 ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return err.gat;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}

	/* add/replace the item in each tag */
	idx = 0;
	goto loop_entr;
	do {	err.gat = addfile_single(&openfiles->tag[idx], &item, &key);
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

static enum GatepaErr
addfile_item_construct(
	/*@out@*/ struct Gatepa_Item *const item, const char *const path
)
/*@globals	fileSystem,
		internalState
@*/
/*@modifies	internalState,
		*item
@*/
{
	struct GString file_ext, file_data;
	nbufio_fd fd;
	uint8_t *buf = NULL;
	size_t buf_size;
	union {	int		i;
		size_t		z;
		off_t		o;
		enum GatepaErr	gat;
	} err;

	/* open the file */
	err.gat = open_file(&fd, path);
	if ( err.gat != 0 ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return err.gat;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}
	err.o = nbufio_seek(fd, 0, SEEK_END);
	if ( err.o == NBUFIO_OFF_ERROR ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return GATERR_IO_SEEK;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}
	buf_size = (size_t) err.o;
	if ( buf_size > (size_t) UINT32_MAX ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return GATERR_LIMIT;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}

	/* read the file into a buffer */
	buf = gatepa_alloc_a1(buf_size, (size_t) 1u);
	if ( buf == NULL ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return GATERR_ALLOCATOR;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}
	assert(buf != NULL);
	err.o = nbufio_seek(fd, 0, SEEK_SET);
	if ( err.o == NBUFIO_OFF_ERROR ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return GATERR_IO_SEEK;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}
	err.z = nbufio_read(fd, buf, buf_size);
	if ( err.z == NBUFIO_RW_ERROR ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return GATERR_IO_READ;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}
	(void) nbufio_close(fd);

	/* stringify the buffer */
	err.gat = addfile_ext_get(&file_ext, path, strlen(path));
	if ( err.gat != 0 ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return err.gat;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}
	err.i   = gstring_ref_bstring(&file_data, buf, buf_size);
	if ( err.i != 0 ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return GATERR_STRING;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}

	/* create the item */
	*item = gatepa_memitem_init(APEFLAG_ITEMTYPE_BINARY);
	err.gat = apetag_memitem_add_value(item, &file_ext);
	if ( err.gat != 0 ){
		return err.gat;
	}
	err.gat = apetag_memitem_add_value(item, &file_data);
	if ( err.gat != 0 ){
		return err.gat;
	}

	return 0;
}

/* MAYBE: share this function with file_slurp.c */
static enum GatepaErr
addfile_ext_get(
	/*@out@*/ struct GString *const file_ext,
	const char *const path, size_t path_size
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*file_ext
@*/
{
	const uint8_t ext_fallback[]  = GATEPA_FEXT_FALLBACK;
	const uintptr_t addr_ext_sep  = (uintptr_t) (
		findrchar(path, FILE_EXT_SEP, path_size)
	);
	const uintptr_t addr_path_sep = (uintptr_t) (
		findrchar(path, FILE_PATH_SEP, path_size)
	);
	/* * */
	uint8_t *str     = (void *) addr_ext_sep;
	size_t   str_len = (size_t) (
		((uintptr_t) &path[path_size]) - addr_ext_sep
	);
	int err;

	if ( (addr_ext_sep == 0) || (addr_ext_sep <= addr_path_sep)
	    ||
	     (str_len > g_apetag.binary_fext_limit)
	){
		str     = (uint8_t *) ext_fallback;
		str_len	= sizeof ext_fallback;
	}
	assert(str != NULL);
	err = gstring_copy_bstring(
		file_ext, str, str_len, &g_myalloc_gstring
	);

	return (err == 0 ? 0 : GATERR_STRING);
}

/* returns 0 on success */
static enum GatepaErr
addfile_single(
	struct Gatepa_Tag *const tag, struct Gatepa_Item *const item,
	const struct GString *const key
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*tag
@*/
{
	const uint32_t item_idx = apetag_memtag_find_item(tag, key);
	enum GatepaErr err;

	if ( item_idx != UINT32_MAX ){
		/* replace */
		apetag_memtag_replace_item(tag, item_idx, item);
	}
	else {	/* add */
		err = apetag_memtag_add_item(tag, key, item);
		if ( err != 0 ){
			return err;
		}
	}
	return 0;
}

/* EOF //////////////////////////////////////////////////////////////////// */
