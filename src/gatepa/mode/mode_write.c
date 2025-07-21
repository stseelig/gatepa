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
#include <libs/overflow.h>

#include "../alloc.h"
#include "../apetag.h"
#include "../attributes.h"
#include "../mode.h"
#include "../open.h"

#include "common.h"

/* //////////////////////////////////////////////////////////////////////// */

/* write$[range][$] */
#define MODE_WRITE_NFIELDS	((size_t) 1u)

/* //////////////////////////////////////////////////////////////////////// */

#undef openfiles
#undef range_gbs
NOINLINE
static enum GatepaErr write_body(
	const char *, char, const struct OpenFiles *openfiles,
	struct GBitset *range_gbs, enum Write_TagType
)
/*@globals	fileSystem,
		internalState
@*/
/*@modifies	fileSystem,
		internalState,
		openfiles->tag[],
		*range_gbs
@*/
;

#undef info
static enum GatepaErr write_single(
	nbufio_fd, struct Gatepa_FileInfo *info, const struct Gatepa_Tag *,
	enum Write_TagType
)
/*@globals	fileSystem,
		internalState
@*/
/*@modifies	fileSystem,
		internalState,
		*info
@*/
;

/* //////////////////////////////////////////////////////////////////////// */

/* returns 0 on success */
GATEPA enum GatepaErr
mode_write_long(const char *const arg_str, const char arg_sep,
	const struct OpenFiles *const openfiles,
	struct GBitset *const range_gbs
)
/*@globals	fileSystem,
		internalState
@*/
/*@modifies	fileSystem,
		internalState,
		openfiles->tag[],
		*range_gbs
@*/
{
	return write_body(
		arg_str, arg_sep, openfiles, range_gbs, TAGTYPE_LONG
	);
}

GATEPA enum GatepaErr
mode_write_short(const char *const arg_str, const char arg_sep,
	const struct OpenFiles *const openfiles,
	struct GBitset *const range_gbs
)
/*@globals	fileSystem,
		internalState
@*/
/*@modifies	fileSystem,
		internalState,
		openfiles->tag[],
		*range_gbs
@*/
{
	return write_body(
		arg_str, arg_sep, openfiles, range_gbs, TAGTYPE_SHORT
	);
}

/* ------------------------------------------------------------------------ */

/* returns 0 on success */
NOINLINE
static enum GatepaErr
write_body(
	const char *const arg_str, const char arg_sep,
	const struct OpenFiles *const openfiles,
	struct GBitset *const range_gbs, enum Write_TagType type
)
/*@globals	fileSystem,
		internalState
@*/
/*@modifies	fileSystem,
		internalState,
		openfiles->tag[],
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

	MODE_SEP_COUNT(MODE_WRITE_NFIELDS);

	MODE_RANGE_GET(range_gbs, NULL);

	/* write each tag */
	idx = 0;
	goto loop_entr;
	do {	err.gat = write_single(
			openfiles->fd[idx], &openfiles->info[idx],
			&openfiles->tag[idx], type
		);
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

static enum GatepaErr
write_single(
	const nbufio_fd fd, struct Gatepa_FileInfo *const info,
	const struct Gatepa_Tag *const tag, const enum Write_TagType type
)
/*@globals	fileSystem,
		internalState
@*/
/*@modifies	fileSystem,
		internalState,
		*info
@*/
{
	uint8_t *buf = NULL;
	uint32_t size_items;
	off_t off_items, off_end;
	union {	int		i;
		size_t		z;
		off_t		o;
		enum GatepaErr	gat;
	} err;

	/* calculate the collective size of the tag items */
	err.gat = apetag_size_items(&size_items, tag);
	if ( err.gat != 0 ){
		/*@-mustmod@*/
		return err.gat;
		/*@=mustmod@*/
	}
	err.i = add_u32_overflow(
		&size_items, size_items,
		(uint32_t) sizeof(struct ApeTag_TagHF)
	);
	if ( err.i != 0 ){
		/*@-mustmod@*/
		return GATERR_OVERFLOW;
		/*@=mustmod@*/
	}

	/* write-lock file */
	err.i = nbufio_lock(fd, LOCK_EX | LOCK_NB);
	if ( err.i != 0 ){
		/*@-mustmod@*/
		return GATERR_IO_WRITELOCK;
		/*@=mustmod@*/
	}

	if ( tag->nmemb == 0 ){
		if ( info->off_end != NBUFIO_OFF_ERROR ){
			/* remove the old tag */
			err.i = nbufio_truncate(fd, info->off_begin);
			if ( err.i != 0 ){
				/*@-mustmod@*/
				return GATERR_IO_TRUNCATE;
				/*@=mustmod@*/
			}

			*info = gatepa_fileinfo_make(
				0, 0, info->off_begin, NBUFIO_OFF_ERROR,
				NBUFIO_OFF_ERROR
			);
		}
		/*@-mustmod@*/
		return 0;
		/*@=mustmod@*/
	}

	/* allocate a buffer */
	err.i = gatepa_alloc_scratch_reset();
	if ( err.i != 0 ){
		/*@-mustmod@*/
		return GATERR_ALLOCATOR;
		/*@=mustmod@*/
	}
	buf = gatepa_alloc_scratch((size_t) size_items, (size_t) 1u);
	if ( buf == NULL ){
		/*@-mustmod@*/
		return GATERR_ALLOCATOR;
		/*@=mustmod@*/
	}
	assert(buf != NULL);

	/* construct the new tag */
	err.z = apetag_construct_tag(buf, (size_t) size_items, tag, type);
	assert(err.z == (size_t) size_items);

	if ( info->off_end != NBUFIO_OFF_ERROR ){
		/* remove the old tag */
		err.i = nbufio_truncate(fd, info->off_begin);
		if ( err.i != 0 ){
			/*@-mustmod@*/
			return GATERR_IO_TRUNCATE;
			/*@=mustmod@*/
		}
	}

	/* seek to the end of the file */
	err.o = nbufio_seek(fd, 0, SEEK_END);
	if ( err.o == NBUFIO_OFF_ERROR ){
		/*@-mustmod@*/
		return GATERR_IO_SEEK;
		/*@=mustmod@*/
	}
	assert(nbufio_tell(fd) == info->off_begin);

	/* if we fail beyond here, remove the tag.
	   saving the old tag beforehand and then rewriting it on error may be
	     viable, but if writing failed, more writing will likely also fail
	*/

	if ( type == TAGTYPE_LONG ){
		/* write the new tag header */
		err.gat = apetag_write_tag_header(fd, size_items, tag->nmemb);
		if ( err.gat != 0 ){
			(void) nbufio_truncate(fd, info->off_begin);
			/*@-mustmod@*/
			return err.gat;
			/*@=mustmod@*/
		}
	}
	off_items = nbufio_tell(fd);
	assert(off_items != NBUFIO_OFF_ERROR);

	/* write the new tag items+footer */
	err.z = nbufio_write(fd, buf, size_items);
	if ( err.z == NBUFIO_RW_ERROR ){
		(void) nbufio_truncate(fd, info->off_begin);
		/*@-mustmod@*/
		return GATERR_IO_WRITE;
		/*@=mustmod@*/
	}
	off_end = nbufio_tell(fd);
	assert(off_items != NBUFIO_OFF_ERROR);

	*info = gatepa_fileinfo_make(
		size_items, tag->nmemb, info->off_begin, off_end, off_items
	);
	return 0;
}

/* EOF //////////////////////////////////////////////////////////////////// */
