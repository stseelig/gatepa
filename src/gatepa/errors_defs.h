#ifndef GATEPA_ERRORS_DEFS_H
#define GATEPA_ERRORS_DEFS_H
/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// errors_defs.h                                                            //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

/* MAYBE: bitfield these */
enum TagCheckError {
	TAGCHECK_ERR_OK = 0,

	TAGCHECK_ERR_PREAMBLE,
	TAGCHECK_ERR_VERSION,
	TAGCHECK_ERR_MISMATCHED,
	TAGCHECK_ERR_MALFORMED,

	TAGCHECK_ERR_SIZE_HARDLIMIT,

	TAGCHECK_ERR_SEEK,
	TAGCHECK_ERR_READ_EOF,
	TAGCHECK_ERR_READ
};
#define TAGCHECK_NUM_ERRORS	((unsigned int) TAGCHECK_ERR_READ + 1u)

enum SlurpError {
	SLURP_ERR_OK = 0,

	SLURP_ERR_OVERFLOW,
	SLURP_ERR_ALLOCATOR,
	SLURP_ERR_GSTRING,
	SLURP_ERR_MEMTAG,

	SLURP_ERR_TAG_SIZE_MISMATCH,

	SLURP_ERR_SNPRINTF,

	SLURP_ERR_SEEK,
	SLURP_ERR_READ_EOF,
	SLURP_ERR_READ_SYS
};
#define SLURP_NUM_ERRORS	((unsigned int) SLURP_ERR_READ_SYS + 1u)

enum GatepaErr {
	GATERR_OK = 0,
	GATERR_FAIL,

	GATERR_LIMIT,
	GATERR_OVERFLOW,

	GATERR_ALLOCATOR,
	GATERR_STRING,

	GATERR_IO_OPEN,
	GATERR_IO_READLOCK,
	GATERR_IO_WRITELOCK,
	GATERR_IO_SEEK,
	GATERR_IO_READ_EOF,
	GATERR_IO_READ,
	GATERR_IO_WRITE,
	GATERR_IO_TRUNCATE,

	GATERR_SINGLE_TAG_ONLY,
	GATERR_MISMATCHED_ITEM_TYPES,

	GATERR_MODESTR_NFIELDS,
	GATERR_MODESTR_SEP,
	GATERR_MODESTR_SIZE_ZERO,

	GATERR_RANGESTR_EMPTY,
	GATERR_RANGESTR_MALFORMED,
	GATERR_RANGESTR_STRTOL,
	GATERR_RANGESTR_VALUE,

	GATERR_KEYSTR_BAD,
	GATERR_VALUESTR_BAD
};
#define GATEPA_NUM_ERRORS	((unsigned int) GATERR_VALUESTR_BAD + 1u)

/* =======================================================================+ */

#define TERMCOLOR_DEFAULT	"\033[0m"
#define TERMCOLOR_GRAY		"\033[1;30m"

#define TERMCOLOR_DEFAULT_BOLD	"\033[0;1m"
#define TERMCOLOR_RED_BOLD	"\033[1;31m"
#define TERMCOLOR_YELLOW	"\033[33m"

/* EOF //////////////////////////////////////////////////////////////////// */
#endif	/* GATEPA_ERRORS_DEFS_H */
