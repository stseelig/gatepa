#ifndef GATEPA_HELP_H
#define GATEPA_HELP_H
/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// help.h                                                                   //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include "attributes.h"

/* //////////////////////////////////////////////////////////////////////// */

#undef file
COLD
GATEPA_EXTERN void help_version(FILE *file)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		file
@*/
;

#undef file
COLD
GATEPA_EXTERN void help_license(FILE *file)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		file
@*/
;

#undef file
COLD
GATEPA_EXTERN void help_usage(FILE *file)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		file
@*/
;

#undef file
COLD
GATEPA_EXTERN void help_opts(FILE *file)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		file
@*/
;

#undef file
COLD
GATEPA void help_mode(FILE *file, int)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		file
@*/
;

/* EOF //////////////////////////////////////////////////////////////////// */
#endif	/* GATEPA_HELP_H */
