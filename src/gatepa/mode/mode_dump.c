/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// mode/mode_dump.c                                                         //
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

/* dump$[range][$] */
#define MODE_DUMP_NFIELDS	((size_t) 1u)

/* //////////////////////////////////////////////////////////////////////// */

static enum GatepaErr dump_single(nbufio_fd, const struct Gatepa_FileInfo *)
/*@globals	fileSystem,
		internalState
@*/
/*@modifies	fileSystem,
		internalState
@*/
;

/* //////////////////////////////////////////////////////////////////////// */

/* returns 0 on success */
GATEPA enum GatepaErr
mode_dump(
	const char *const arg_str, const char arg_sep,
	const struct OpenFiles *const openfiles,
	struct GBitset *const range_gbs
)
/*@globals	fileSystem,
		internalState
@*/
/*@modifies	fileSystem,
		internalState,
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

	MODE_SEP_COUNT(MODE_DUMP_NFIELDS);

	MODE_RANGE_GET(range_gbs, NULL);

	/* check that we are only extracting from one tag/file */
	if ( bitset_popcount(GBITSET_PTR(range_gbs), range_gbs->bitlen)
	   !=
	    (size_t) 1u
	){
		return GATERR_SINGLE_TAG_ONLY;
	}

	/* dump the tag */
	idx = bitset_find_1(GBITSET_PTR(range_gbs), range_gbs->bitlen, 0);
	assert(idx != SIZE_MAX);
	err.gat = dump_single(openfiles->fd[idx], &openfiles->info[idx]);

	return err.gat;
}

/* ------------------------------------------------------------------------ */

/* dumps the tag as it currently is in the file */
/* returns 0 on success */
static enum GatepaErr
dump_single(const nbufio_fd fd, const struct Gatepa_FileInfo *const info)
/*@globals	fileSystem,
		internalState
@*/
/*@modifies	fileSystem,
		internalState
@*/
{
	uint8_t buf[BUFSIZ];
	size_t nbytes_remain = (size_t) (
		((size_t) info->off_end) - ((size_t) info->off_begin)
	);
	size_t nbytes_curr;
	union {	size_t	z;
		off_t	o;
	} err;

	if ( info->items_nmemb == 0 ){
		/*@-mustmod@*/
		return 0;
		/*@=mustmod@*/
	}

	assert((size_t) info->off_end > (size_t) info->off_begin);
	assert(nbytes_remain != 0);

	err.o = nbufio_seek(fd, info->off_begin, SEEK_SET);
	if ( err.o == NBUFIO_OFF_ERROR ){
		/*@-mustmod@*/
		return GATERR_IO_SEEK;
		/*@=mustmod@*/
	}

	do {	nbytes_curr    = (nbytes_remain > sizeof buf
			? sizeof buf : nbytes_remain
		);
		nbytes_remain -= nbytes_curr;

		err.z = nbufio_read(fd, buf, nbytes_curr);
		if ( err.z == NBUFIO_RW_ERROR ){
			/*@-mustmod@*/
			return GATERR_IO_READ;
			/*@=mustmod@*/
		}
		(void) fwrite(buf, nbytes_curr, (size_t) 1u, stdout);
	}
	while ( nbytes_remain != 0 );

	return 0;
}

/* EOF //////////////////////////////////////////////////////////////////// */
