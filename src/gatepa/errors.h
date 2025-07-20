#ifndef GATEPA_ERRORS_H
#define GATEPA_ERRORS_H
/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// errors.h                                                                 //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include "attributes.h"
#include "errors_defs.h"
#include "open.h"

/* //////////////////////////////////////////////////////////////////////// */

/*@unused@*/
extern const char *g_argv0;

/* //////////////////////////////////////////////////////////////////////// */

COLD
GATEPA_EXTERN void gatepa_warning_header(
	const struct OpenFiles *, unsigned int
)
/*@globals	fileSystem@*/
/*@modifies	fileSystem@*/
;

/*@printflike@*/
COLD
GATEPA_EXTERN void gatepa_error(const char *, ...)
/*@globals	fileSystem,
		g_argv0
@*/
/*@modifies	fileSystem@*/
;

/*@observer@*/
COLD CONST
GATEPA_EXTERN const char *gatepa_strerror_tagcheck(enum TagCheckError) /*@*/;

/*@observer@*/
COLD CONST
GATEPA_EXTERN const char *gatepa_strerror_slurp(enum SlurpError) /*@*/;

/*@observer@*/
COLD CONST
GATEPA_EXTERN const char *gatepa_strerror(enum GatepaErr) /*@*/;

/* EOF //////////////////////////////////////////////////////////////////// */
#endif	/* GATEPA_ERRORS_H */
