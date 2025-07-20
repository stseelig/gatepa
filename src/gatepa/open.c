/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// open.c                                                                   //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <errno.h>
#include <limits.h>

#include <sys/resource.h>

#include <libs/nbufio.h>
#include <libs/overflow.h>

#include "alloc.h"
#include "apetag.h"
#include "attributes.h"
#include "errors.h"
#include "utility.h"

/* //////////////////////////////////////////////////////////////////////// */

#undef openfiles
static int
open_files_loop_body(
	/*@partial@*/ struct OpenFiles *openfiles, const char *const *,
	unsigned int
)
/*@globals	fileSystem,
		internalState
@*/
/*@modifies	fileSystem,
		internalState,
		*openfiles
@*/
;

/* returns 0 on success */
static int fdlimit_check(void)
/*@globals	internalState@*/
/*@modifies	internalState@*/
;

static void fdlimit_try(void)
/*@globals	internalState@*/
/*@modifies	internalState@*/
;

/* //////////////////////////////////////////////////////////////////////// */

GATEPA void
gatepa_print_filename(
	FILE *const file,
	const struct OpenFiles *const openfiles, const unsigned int idx
)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		*file
@*/
{
	const int pow10 = (int) ilog10p1((uintmax_t) openfiles->nmemb);

	assert(idx < UINT_MAX);

	(void) fprintf(file, "%0*u:%08zX:'%s'\n",
		pow10, idx + 1u, (size_t) openfiles->info[idx].off_begin,
		openfiles->name[idx]
	);
	return;
}

/* returns 0 on success, <0 on allocator err, or the number of file errs */
GATEPA int
open_files(
	/*@out@*/ struct OpenFiles *const openfiles,
	const unsigned int num_files, const char *const *const file0
)
/*@globals	fileSystem,
		internalState
@*/
/*@modifies	fileSystem,
		internalState,
		*openfiles
@*/
{
	int retval = 0;
	void *ptr_fd, *ptr_info, *ptr_tag;
	int err;
	unsigned int i;

	/* alloc */
	ptr_fd = gatepa_alloc_a16(
		sizeof *openfiles->fd, (size_t) num_files
	);
	ptr_info = gatepa_alloc_a16(
		sizeof *openfiles->info, (size_t) num_files
	);
	ptr_tag = gatepa_alloc_a16(
		sizeof *openfiles->tag, (size_t) num_files
	);
	if ( (ptr_fd == NULL) || (ptr_info == NULL) || (ptr_tag == NULL) ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return -1;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}

	/* init */
	*openfiles = (struct OpenFiles) {
		ptr_fd, ptr_info, ptr_tag, file0, 0
	};

	/* fill */
	for ( i = 0; i < num_files; ++i ){
		err = open_files_loop_body(openfiles, file0, i);
		retval += (retval != INT_MAX ? (int) (err != 0) : 0);
	}
	return retval;
}

/* TODO: pass argv_idx for error printing */
static int
open_files_loop_body(
	/*@partial@*/ struct OpenFiles *const openfiles,
	const char *const *const file0, const unsigned int idx
)
/*@globals	fileSystem,
		internalState
@*/
/*@modifies	fileSystem,
		internalState,
		*openfiles
@*/
{
	nbufio_fd fd;
	union {	int			i;
		enum GatepaErr		gat;
		enum TagCheckError	tagcheck;
		enum SlurpError		slurp;
	} err;

	/* open/read-lock the file */
	err.gat = open_file(&fd, file0[idx]);
	if UNLIKELY ( err.gat != 0 ){
		/* MAYBE: use errno */
		gatepa_error("%s: '%s'",
			gatepa_strerror(err.gat), file0[idx]
		);
	}
	openfiles->fd[idx] = fd;
	openfiles->nmemb  += 1u;

	if ( err.gat != 0 ){
		return -1;
	}
	assert(fd != NBUFIO_FD_ERROR);

	/* check the tag */
	err.tagcheck = apetag_file_tag_check_eof(&openfiles->info[idx], fd);
	switch ( err.tagcheck ){
	case TAGCHECK_ERR_PREAMBLE:
		openfiles->tag[idx] = GATEPA_MEMTAG_INIT;
		return 0;
	case TAGCHECK_ERR_MISMATCHED:
		; /* MAYBE */
		/*@switchbreak@*/ break;
	default:
		if UNLIKELY ( err.tagcheck != 0 ){
			gatepa_error("%s: '%s'",
				gatepa_strerror_tagcheck(err.tagcheck),
				file0[idx]
			);
			return -1;
		}
		/*@switchbreak@*/ break;
	}

	/* slurp the tag */
	err.slurp = apetag_slurp_tag(
		&openfiles->tag[idx], &openfiles->info[idx], fd
	);
	if UNLIKELY ( err.slurp != 0 ){
		gatepa_error("%s: '%s'",
			gatepa_strerror_slurp(err.slurp), file0[idx]
		);
		return -1;
	}

	return 0;
}

/* ------------------------------------------------------------------------ */

/* returns 0 on success */
GATEPA enum GatepaErr
open_file(/*@out@*/ nbufio_fd *const fd_out, const char *const pathname)
/*@globals	fileSystem,
		internalState
@*/
/*@modifies	internalState,
		*fd_out
@*/
{
	nbufio_fd fd;
	unsigned int blocked_retries = 3u;
	int err;
	enum GatepaErr retval = 0;

	/* open */
retry:
	fd = nbufio_open(pathname, O_RDWR | O_NONBLOCK);
	if ( fd == NBUFIO_FD_ERROR ){
		if ( (errno == EINTR) && (blocked_retries-- != 0) ){
			goto retry;
		}
		else {	err = fdlimit_check();
			if ( err == 0 ){
				goto retry;
			}
		}
		retval = GATERR_IO_OPEN;
	}

	/* lock */
	if ( fd != NBUFIO_FD_ERROR ){
		err = nbufio_lock(fd, LOCK_SH | LOCK_NB);
		if ( err != 0 ){
			(void) nbufio_close(fd);
			retval = GATERR_IO_READLOCK;
			fd     = NBUFIO_FD_ERROR;
		}
	}

	*fd_out = fd;
	return retval;
}

/* returns 0 on success */
static int
fdlimit_check(void)
/*@globals	internalState@*/
/*@modifies	internalState@*/
{
	static unsigned char tried = 0;

	if ( tried == 0 ){
		fdlimit_try();
		tried = (unsigned char) 1u;
		return 0;
	}
	return -1;
}

static void
fdlimit_try(void)
/*@globals	internalState@*/
/*@modifies	internalState@*/
{
	struct rlimit limit;
	int err;

	err = getrlimit(RLIMIT_NOFILE, &limit);
	if ( err != 0 ){
		return;
	}
	limit.rlim_cur = limit.rlim_max;
	(void) setrlimit(RLIMIT_NOFILE, &limit);

	return;
}

/* EOF //////////////////////////////////////////////////////////////////// */
