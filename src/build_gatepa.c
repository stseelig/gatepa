/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// build_gatepa.c                                                           //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#if __STDC_VERSION__ < 201112L
#error "__STDC_VERSION__ < 201112L"
#endif	/* __STDC_VERSION__ */

#if _POSIX_C_SOURCE < 200112L || !defined(_POSIX_C_SOURCE)
#undef  _POSIX_C_SOURCE
#define _POSIX_C_SOURCE		200112L
#endif	/* _POSIX_C_SOURCE */

/* qsort_r() */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#if _FILE_OFFSET_BITS < 64 || !defined(_FILE_OFFSET_BITS)
#undef  _FILE_OFFSET_BITS
#define	_FILE_OFFSET_BITS	64
#endif	/* _FILE_OFFSET_BITS */

/* //////////////////////////////////////////////////////////////////////// */

#include "gatepa/main.c"

#include "gatepa/alloc.c"
#include "gatepa/errors.c"
#include "gatepa/help.c"
#include "gatepa/open.c"
#include "gatepa/opts.c"
#include "gatepa/text.c"

#include "gatepa/apetag/file_check.c"
#include "gatepa/apetag/file_slurp.c"
#include "gatepa/apetag/file_write.c"
#include "gatepa/apetag/memtag.c"
#include "gatepa/apetag/print.c"
#include "gatepa/apetag/sort.c"

#include "gatepa/mode/common.c"
#include "gatepa/mode/mode_add.c"
#include "gatepa/mode/mode_add-file.c"
#include "gatepa/mode/mode_append.c"
#include "gatepa/mode/mode_auto-track.c"
#include "gatepa/mode/mode_clear.c"
#include "gatepa/mode/mode_dump.c"
#include "gatepa/mode/mode_extract.c"
#include "gatepa/mode/mode_print.c"
#include "gatepa/mode/mode_remove.c"
#include "gatepa/mode/mode_rename.c"
#include "gatepa/mode/mode_sort.c"
#include "gatepa/mode/mode_tidy-keys.c"
#include "gatepa/mode/mode_verify.c"
#include "gatepa/mode/mode_write.c"

/* EOF //////////////////////////////////////////////////////////////////// */
