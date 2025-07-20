#ifndef GATEPA_MODE_H
#define GATEPA_MODE_H
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
#include "errors.h"
#include "open.h"

/* //////////////////////////////////////////////////////////////////////// */

enum ModeIdx {
	M_ADD,
	M_ADD_FILE,
	M_ADD_LOC,
	M_APPEND,
	M_APPEND_L,
	M_A_TRACK,
	M_CLEAR,
	M_DUMP,
	M_EXTRACT,
	M_PRINT,
	M_PRINT_L,
	M_PRINT_S,
	M_REMOVE,
	M_RENAME,
	M_SORT,
	M_S_ALPHA,
	M_S_AUDIO,
	M_TIDY,
	M_TIDY_1U,
	M_TIDY_L,
	M_TIDY_U,
	M_VERIFY,
	M_WRITE,
	M_WRITE_L,
	M_WRITE_S
};
#define GATEPA_NUM_MODES	((unsigned int) M_WRITE_S + 1u)

/* //////////////////////////////////////////////////////////////////////// */

NOINLINE PURE
GATEPA_EXTERN int get_mode_idx(const uint8_t *, size_t) /*@*/;

/* ======================================================================== */

#undef openfiles
#undef range_gbs
GATEPA_EXTERN enum GatepaErr mode_add(
	const char *, char, const struct OpenFiles *openfiles,
	struct GBitset *range_gbs
)
/*@globals	internalState@*/
/*@modifies	internalState,
		openfiles->tag[],
		*range_gbs
@*/
;

#undef openfiles
#undef range_gbs
GATEPA_EXTERN enum GatepaErr mode_addfile(
	const char *, char, const struct OpenFiles *openfiles,
	struct GBitset *range_gbs
)
/*@globals	fileSystem,
		internalState
@*/
/*@modifies	internalState,
		openfiles->tag[],
		*range_gbs
@*/
;

#undef openfiles
#undef range_gbs
GATEPA_EXTERN enum GatepaErr mode_addloc(
	const char *, char, const struct OpenFiles *openfiles,
	struct GBitset *range_gbs
)
/*@globals	internalState@*/
/*@modifies	internalState,
		openfiles->tag[],
		*range_gbs
@*/
;

#undef openfiles
#undef range_gbs
GATEPA_EXTERN enum GatepaErr mode_append(
	const char *, char, const struct OpenFiles *openfiles,
	struct GBitset *range_gbs
)
/*@globals	internalState@*/
/*@modifies	internalState,
		openfiles->tag[],
		*range_gbs
@*/
;

#undef openfiles
#undef range_gbs
GATEPA_EXTERN enum GatepaErr mode_appendloc(
	const char *, char, const struct OpenFiles *openfiles,
	struct GBitset *range_gbs
)
/*@globals	internalState@*/
/*@modifies	internalState,
		openfiles->tag[],
		*range_gbs
@*/
;

#undef openfiles
#undef range_gbs
GATEPA_EXTERN enum GatepaErr mode_autotrack(
	const char *, char, const struct OpenFiles *openfiles,
	struct GBitset *range_gbs
)
/*@globals	internalState@*/
/*@modifies	internalState,
		openfiles->tag[],
		*range_gbs
@*/
;

#undef openfiles
#undef range_gbs
GATEPA_EXTERN enum GatepaErr mode_clear(
	const char *, char, const struct OpenFiles *openfiles,
	struct GBitset *range_gbs
)
/*@modifies	openfiles->tag[],
		*range_gbs
@*/
;

#undef openfiles
#undef range_gbs
GATEPA_EXTERN enum GatepaErr mode_dump(
	const char *, char, const struct OpenFiles *openfiles,
	struct GBitset *range_gbs
)
/*@globals	fileSystem,
		internalState
@*/
/*@modifies	fileSystem,
		internalState,
		*range_gbs
@*/
;

#undef openfiles
#undef range_gbs
GATEPA_EXTERN enum GatepaErr mode_extract(
	const char *, char, const struct OpenFiles *openfiles,
	struct GBitset *range_gbs
)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		openfiles->tag[],
		*range_gbs
@*/
;

#undef range_gbs
GATEPA_EXTERN enum GatepaErr mode_print_long(
	const char *, char, const struct OpenFiles *,
	struct GBitset *range_gbs
)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		*range_gbs
@*/
;

#undef range_gbs
GATEPA_EXTERN enum GatepaErr mode_print_short(
	const char *, char, const struct OpenFiles *,
	struct GBitset *range_gbs
)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		*range_gbs
@*/
;

#undef openfiles
#undef range_gbs
GATEPA_EXTERN enum GatepaErr mode_remove(
	const char *, char, const struct OpenFiles *openfiles,
	struct GBitset *range_gbs
)
/*@modifies	openfiles->tag[],
		*range_gbs
@*/
;

#undef openfiles
#undef range_gbs
GATEPA_EXTERN enum GatepaErr mode_rename(
	const char *, char, const struct OpenFiles *openfiles,
	struct GBitset *range_gbs
)
/*@modifies	openfiles->tag[],
		*range_gbs
@*/
;

#undef openfiles
#undef range_gbs
GATEPA_EXTERN enum GatepaErr mode_sort_alpha(
	const char *, char, const struct OpenFiles *openfiles,
	struct GBitset *range_gbs
)
/*@globals	internalState@*/
/*@modifies	internalState,
		openfiles->tag[],
		*range_gbs
@*/
;

#undef openfiles
#undef range_gbs
GATEPA_EXTERN enum GatepaErr mode_sort_audio(
	const char *, char, const struct OpenFiles *openfiles,
	struct GBitset *range_gbs
)
/*@globals	internalState@*/
/*@modifies	internalState,
		openfiles->tag[],
		*range_gbs
@*/
;

#undef openfiles
#undef range_gbs
GATEPA_EXTERN enum GatepaErr mode_tidykeys_1up(
	const char *, char, const struct OpenFiles *openfiles,
	struct GBitset *range_gbs
)
/*@modifies	openfiles->tag[],
		*range_gbs
@*/
;

#undef openfiles
#undef range_gbs
GATEPA_EXTERN enum GatepaErr mode_tidykeys_lo(
	const char *, char, const struct OpenFiles *openfiles,
	struct GBitset *range_gbs
)
/*@modifies	openfiles->tag[],
		*range_gbs
@*/
;

#undef openfiles
#undef range_gbs
GATEPA_EXTERN enum GatepaErr mode_tidykeys_up(
	const char *, char, const struct OpenFiles *openfiles,
	struct GBitset *range_gbs
)
/*@modifies	openfiles->tag[],
		*range_gbs
@*/
;

#undef openfiles
#undef range_gbs
GATEPA_EXTERN enum GatepaErr mode_verify(
	const char *, char, const struct OpenFiles *openfiles,
	struct GBitset *range_gbs
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

#undef openfiles
#undef range_gbs
GATEPA_EXTERN enum GatepaErr mode_write_long(
	const char *, char, const struct OpenFiles *openfiles,
	struct GBitset *range_gbs
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

#undef openfiles
#undef range_gbs
GATEPA_EXTERN enum GatepaErr mode_write_short(
	const char *, char, const struct OpenFiles *openfiles,
	struct GBitset *range_gbs
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

/* EOF //////////////////////////////////////////////////////////////////// */
#endif	/* GATEPA_MODE_H */
