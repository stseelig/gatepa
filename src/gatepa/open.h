#ifndef GATEPA_OPEN_H
#define GATEPA_OPEN_H
/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// open.h                                                                   //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <stdio.h>

#include <libs/nbufio.h>

#include "open_defs.h"

/* //////////////////////////////////////////////////////////////////////// */

#undef file
GATEPA_EXTERN void gatepa_print_filename(
	FILE *file, const struct OpenFiles *, unsigned int
)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		*file
@*/
;

#undef openfiles
GATEPA_EXTERN int open_files(
	/*@out@*/ struct OpenFiles *openfiles,
	unsigned int, const char *const *
)
/*@globals	fileSystem,
		internalState
@*/
/*@modifies	fileSystem,
		internalState,
		*openfiles
@*/
;

#undef fd_out
GATEPA_EXTERN enum GatepaErr open_file(
	/*@out@*/ nbufio_fd *fd_out, const char *
)
/*@globals	fileSystem,
		internalState
@*/
/*@modifies	internalState,
		*fd_out
@*/
;

/* EOF //////////////////////////////////////////////////////////////////// */
#endif	/* GATEPA_OPEN_H */
