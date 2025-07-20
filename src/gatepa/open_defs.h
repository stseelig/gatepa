#ifndef GATEPA_OPEN_DEFS_H
#define GATEPA_OPEN_DEFS_H
/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// open_defs.h                                                              //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <stdio.h>

#include <libs/nbufio.h>

#include "apetag.h"

/* //////////////////////////////////////////////////////////////////////// */

#define FILE_PATH_SEP	'/'
#define FILE_EXT_SEP	'.'

/* //////////////////////////////////////////////////////////////////////// */

struct OpenFiles {
	/*@temp@*/ /*@relnull@*/
	nbufio_fd		*fd;
	/*@temp@*/ /*@relnull@*/
	struct Gatepa_FileInfo	*info;
	/*@temp@*/ /*@relnull@*/
	struct Gatepa_Tag	*tag;
	/*@temp@*/ /*@relnull@*/
	const char *const	*name;

	unsigned int		nmemb;
};

#define OPENFILES_STATIC_INIT_NULL	{ NULL, NULL, NULL, NULL, 0 }

/* EOF //////////////////////////////////////////////////////////////////// */
#endif	/* GATEPA_OPEN_DEFS_H */
