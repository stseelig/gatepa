/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// errors.c                                                                 //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <stdarg.h>
#include <stdio.h>

#include "attributes.h"
#include "errors.h"
#include "open.h"

/* //////////////////////////////////////////////////////////////////////// */

/*@-readonlytrans@*/
const char *g_argv0 = "gatepa";
/*@=readonlytrans@*/

/* //////////////////////////////////////////////////////////////////////// */

/* MAYBE: merge all of the errors */
/* MAYBE: opt errors */

/*@unchecked@*/ /*@observer@*/
static const char *const f_tagcheck_strerror[TAGCHECK_NUM_ERRORS] = {
	"ok (BUG)",

	"no 'APETAGEX' preamble",
	"unsupported tag version",
	"mismatched tag header/footer pair",
	"malformed tag",

	"tag size larger than hard-limit",

	"i/o seek error",
	"i/o read unexpected EOF",
	"i/o read error"
};

/*@unchecked@*/ /*@observer@*/
static const char *const f_slurp_strerror[SLURP_NUM_ERRORS] = {
	"ok (BUG)",

	"integer overflow (generic)",
	"allocator failure (generic)",
	"string error (generic)",
	"error updating tag (generic)",

	"tag size mismatch",

	"snprintf error",

	"i/o seek error",
	"i/o read unexpected EOF",
	"i/o read error"
};

/*@unchecked@*/ /*@observer@*/
static const char *const f_gatepa_strerror[GATEPA_NUM_ERRORS] = {
	"ok (BUG)",
	"failure",

	"limit reached (generic)",
	"integer overflow (generic)",

	"allocator failure (generic)",
	"string error (generic)",

	"i/o open-file error",
	"i/o read-lock error",
	"i/o write-lock error",
	"i/o seek error",
	"i/o read unexpected EOF",
	"i/o read error",
	"i/o write error",
	"i/o truncate error",

	"only one tag may be selected for this mode",
	"mismatched item types",

	"too many fields in mode string",
	"unallowed seperator byte",
	"zero-sized field",

	"empty range field member",
	"malformed range string",
	"bad range string (strtol)",
	"bad range value",

	"key string must be printable ASCII ($20 - $7E)",
	"value string must be UTF-8"
};

/* //////////////////////////////////////////////////////////////////////// */

COLD
GATEPA void
gatepa_warning_header(
	const struct OpenFiles *const openfiles, const unsigned int file_idx
)
/*@globals	fileSystem@*/
/*@modifies	fileSystem@*/
{
	(void) fputs(TERMCOLOR_YELLOW, stderr);
	gatepa_print_filename(stderr, openfiles, file_idx);
	(void) fputs(TERMCOLOR_DEFAULT, stderr);
	return;
}

/*@printflike@*/
COLD
GATEPA void
gatepa_error(const char *const format, ...)
/*@globals	fileSystem,
		g_argv0
@*/
/*@modifies	fileSystem@*/
{
	va_list arg;

	va_start(arg, format);

	(void) fprintf(stderr,
		TERMCOLOR_RED_BOLD "%s: " TERMCOLOR_DEFAULT_BOLD, g_argv0
	);
	(void) vfprintf(stderr, format, arg);
	(void) fputs( TERMCOLOR_RED_BOLD " !" TERMCOLOR_DEFAULT "\n", stderr);

	va_end(arg);
	return;
}

/* ======================================================================== */

/*@observer@*/
COLD CONST
GATEPA const char *
gatepa_strerror_tagcheck(const enum TagCheckError type)
/*@*/
{
	return f_tagcheck_strerror[type];
}

/*@observer@*/
COLD CONST
GATEPA const char *
gatepa_strerror_slurp(const enum SlurpError type)
/*@*/
{
	return f_slurp_strerror[type];
}

/*@observer@*/
COLD CONST
GATEPA const char *
gatepa_strerror(const enum GatepaErr type)
/*@*/
{
	return f_gatepa_strerror[type];
}

/* EOF //////////////////////////////////////////////////////////////////// */
