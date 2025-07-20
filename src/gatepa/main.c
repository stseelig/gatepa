/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// main.c                                                                   //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <libs/gbitset.h>
#include <libs/nbufio.h>

#include "alloc.h"
#include "attributes.h"
#include "errors.h"
#include "help.h"
#include "mode.h"
#include "open.h"

/* //////////////////////////////////////////////////////////////////////// */

typedef enum GatepaErr (*gatepa_fnptr_mode)(
	const char *, char, const struct OpenFiles *, struct GBitset *
);

struct ModeInfo {
	/*@temp@*/
	const char		*name;
	/*@temp@*/
	gatepa_fnptr_mode	fn;
	char			sep;
	size_t			range_idx;
};

/* //////////////////////////////////////////////////////////////////////// */

/*@unchecked@*/ /*@observer@*/
const char *f_mode_name[GATEPA_NUM_MODES] = {
	u8"add",
	u8"add-file",
	u8"add-loc",
	u8"append",
	u8"append-loc",
	u8"auto-track",
	u8"clear",
	u8"dump",
	u8"extract",
	u8"print",	/* alias for 'print-short' */
	u8"print-long",
	u8"print-short",
	u8"remove",
	u8"rename",
	u8"sort",	/* alias for 'sort-audio' */
	u8"sort-alpha",
	u8"sort-audio",
	u8"tidy-keys",	/* alias for 'tidy-keys-lo' */
	u8"tidy-keys-1up",
	u8"tidy-keys-lo",
	u8"tidy-keys-up",
	u8"verify",
	u8"write",	/* alias for 'write-long' */
	u8"write-long",
	u8"write-short"
};

/*@unchecked@*/
const uint8_t f_mode_name_len[GATEPA_NUM_MODES] = {
	UINT8_C( 3),	/* add           */
	UINT8_C( 9),	/* add-file      */
	UINT8_C( 7),	/* add-loc       */
	UINT8_C( 6),	/* append        */
	UINT8_C(10),	/* append-loc    */
	UINT8_C(10),	/* auto-track    */
	UINT8_C( 5),	/* clear         */
	UINT8_C( 4),	/* dump          */
	UINT8_C( 7),	/* extract       */
	UINT8_C( 5),	/* print         */
	UINT8_C(10),	/* print-long    */
	UINT8_C(11),	/* print-short   */
	UINT8_C( 6),	/* remove        */
	UINT8_C( 6),	/* rename        */
	UINT8_C( 4),	/* sort          */
	UINT8_C(10),	/* sort-alpha    */
	UINT8_C(10),	/* sort-audio    */
	UINT8_C( 9),	/* tidy-keys     */
	UINT8_C(13),	/* tidy-keys-1up */
	UINT8_C(12),	/* tidy-keys-lo  */
	UINT8_C(12),	/* tidy-keys-up  */
	UINT8_C( 6),	/* verify        */
	UINT8_C( 5),	/* write         */
	UINT8_C(10),	/* write-long    */
	UINT8_C(11)	/* write-short   */
};

/*@unchecked@*/
static const gatepa_fnptr_mode f_mode_fnptr[GATEPA_NUM_MODES] = {
	mode_add,
	mode_addfile,
	mode_addloc,
	mode_append,
	mode_appendloc,
	mode_autotrack,
	mode_clear,
	mode_dump,
	mode_extract,
	mode_print_short,
	mode_print_long,
	mode_print_short,
	mode_remove,
	mode_rename,
	mode_sort_audio,
	mode_sort_alpha,
	mode_sort_audio,
	mode_tidykeys_lo,
	mode_tidykeys_1up,
	mode_tidykeys_lo,
	mode_tidykeys_up,
	mode_verify,
	mode_write_long,
	mode_write_long,
	mode_write_short
};

/* //////////////////////////////////////////////////////////////////////// */

GATEPA_EXTERN int opt_process(const char *)
/*@globals	fileSystem,
		internalState
@*/
/*@modifies	fileSystem,
		internalState
@*/
;

/* //////////////////////////////////////////////////////////////////////// */

#undef info
static int scan_mode(/*@out@*/ struct ModeInfo *info, const char *)
/*@modifies	*info@*/
;

/* ------------------------------------------------------------------------ */

PURE
static unsigned int count_opts(
	unsigned int, const char *const *, unsigned int
)
/*@*/
;

PURE
static unsigned int count_till_dubdash(
	unsigned int, const char *const *, unsigned int
)
/*@*/
;

/* //////////////////////////////////////////////////////////////////////// */

int
main(const int argc, const char *const *const argv)
/*@globals	fileSystem,
		internalState,
		g_argv0
@*/
/*@modifies	fileSystem,
		internalState,
		g_argv0
@*/
{
	struct OpenFiles openfiles;
	struct ModeInfo  modeinfo;
	struct GBitset   range_gbs;
	/* * */
	unsigned int num_opts = 0, num_files = 0;
	unsigned int idx_opt0, idx_file0;
	unsigned int arg_idx = 1u;
	union {	int		i;
		enum GatepaErr	gat;
	} err;
	unsigned int i;

	g_argv0 = argv[0];

	if UNLIKELY ( argc < 2 ){
		help_version(stderr);
		help_license(stderr);
		help_usage(stderr);
		gatepa_error("nothing to do");
		return EXIT_FAILURE;
	}

	/* count the number of opts */
	idx_opt0   = arg_idx;
	num_opts   = count_opts((unsigned int) argc, argv, arg_idx);
	arg_idx   += num_opts;
	if ( arg_idx < (unsigned int) argc ){
		arg_idx += (unsigned int) (strcmp(argv[arg_idx], "--") == 0);
	}

	/* count the number of files */
	idx_file0  = arg_idx;
	num_files  = count_till_dubdash((unsigned int) argc, argv, arg_idx);
	arg_idx   += num_files;

	/* register the cleanup function (exit() handles closing files) */
	(void) atexit(gatepa_alloc_destroy);

	/* process each opt */
	for ( i = 0; i < num_opts; ++i ){
		err.i = opt_process(argv[idx_opt0 + i]);
		if UNLIKELY ( err.i != 0 ){
			gatepa_error("argv[%u]: bad opt string",
				idx_opt0 + i
			);
			exit(EXIT_FAILURE);
		}
	}

	if UNLIKELY ( num_files == 0 ){
		gatepa_error("no files");
		return EXIT_FAILURE;
	}
	if ( arg_idx < (unsigned int) argc ){
		arg_idx += (unsigned int) (strcmp(argv[arg_idx], "--") == 0);
	}
	if UNLIKELY ( arg_idx >= (unsigned int) argc ){
		gatepa_error("no modes");
		return EXIT_FAILURE;
	}

	/* open each file */
	err.i = open_files(&openfiles, num_files, &argv[idx_file0]);
	if ( err.i != 0 ){
		return EXIT_FAILURE;
	}

	/* init the range bitset */
	err.i = gbitset_init(
		&range_gbs, (uint32_t) num_files, &g_myalloc_gbitset
	);
	if UNLIKELY ( err.i != 0 ){
		gatepa_error("%s", gatepa_strerror(GATERR_ALLOCATOR));
	}

	/* process each mode */
	do {	err.i = scan_mode(&modeinfo, argv[arg_idx]);
		if UNLIKELY ( err.i != 0 ){
			gatepa_error("argv[%u]: bad mode string", arg_idx);
			return EXIT_FAILURE;
		}
		err.gat = modeinfo.fn(
			&argv[arg_idx][modeinfo.range_idx],
			modeinfo.sep, &openfiles, &range_gbs
		);
		if UNLIKELY ( err.gat != 0 ){
			gatepa_error("argv[%u] (%s): %s",
				arg_idx, modeinfo.name,
				gatepa_strerror(err.gat)
			);
			return EXIT_FAILURE;
		}
	} while ( ++arg_idx != (unsigned int) argc );

	return EXIT_SUCCESS;
}

/* returns 0 on success */
static int
scan_mode(/*@out@*/ struct ModeInfo *const info, const char *const str)
/*@modifies	*info@*/
{
	const size_t str_len = strlen(str);
	/* * */
	char   sep = '\0';
	size_t sep_idx;
	int mode_idx;

	if ( str_len == 0 ){
		return -1;
	}

	/* get the seperator byte */
	for ( sep_idx = 0; sep_idx < str_len; ++sep_idx ){
		if ( !isalnum((int) str[sep_idx]) && (str[sep_idx] != '-') ){
			sep = str[sep_idx];
			break;
		}
	}
	if ( (sep_idx == 0) || (sep_idx == SIZE_MAX) || (sep == '\0') ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return -1;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}

	/* validation of the seperator byte done in mode parsing functions */

	/* get the mode */
	mode_idx = get_mode_idx((const uint8_t *) str, sep_idx);
	if ( mode_idx < 0 ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return mode_idx;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}

	*info = (struct ModeInfo) {
		f_mode_name[mode_idx], f_mode_fnptr[mode_idx],
		sep, sep_idx + 1u
	};
	return 0;
}

/* ------------------------------------------------------------------------ */

/* returns the number of CLI opts */
PURE
static unsigned int
count_opts(
	const unsigned int argc, const char *const *const argv,
	const unsigned int idx
)
/*@*/
{
	unsigned int count = 0;
	unsigned int i;

	for ( i = idx; i < argc; ++i ){
		if ( argv[i] == NULL ){
			break;
		}
		if ( argv[i][0] == '-' ){
			if ( strcmp(argv[i], "--") == 0 ){
				break;
			}
			count += 1u;
		}
		else {	break; }
	}
	return count;
}

/* returns the number of CLI args before a "--" or NULL */
PURE
static unsigned int
count_till_dubdash(
	const unsigned int argc, const char *const *const argv,
	const unsigned int idx
)
/*@*/
{
	unsigned int count = 0;
	unsigned int i;

	for ( i = idx; i < argc; ++i ){
		if ( argv[i] == NULL ){
			break;
		}
		if ( strcmp(argv[i], "--") == 0 ){
			break;
		}
		count += 1u;
	}
	return count;
}

/* ======================================================================== */

/* returns the index of the mode, or -1 on error */
NOINLINE PURE
GATEPA int
get_mode_idx(const uint8_t *const data, const size_t len)
/*@*/
{
	const int8_t mode_idx_table[64u] = {
	#define T	int8_t
/*$00*/	(T) -1,		(T) M_SORT,	(T) -1,		(T) -1,
	(T) M_TIDY_L,	(T) M_CLEAR,	(T) M_VERIFY,	(T) M_ADD,
	(T) M_RENAME,	(T) -1,		(T) -1,		(T) M_APPEND,
	(T) -1,		(T) -1,		(T) -1,		(T) M_WRITE,
/*$10*/	(T) -1,		(T) -1,		(T) -1,		(T) -1,
	(T) -1,		(T) -1,		(T) -1,		(T) M_PRINT,
	(T) -1,		(T) M_TIDY_U,	(T) M_DUMP,	(T) M_EXTRACT,
	(T) -1,		(T) M_REMOVE,	(T) -1,		(T) -1,
/*$20*/	(T) M_ADD_LOC,	(T) -1,		(T) -1,		(T) -1,
	(T) M_S_ALPHA,	(T) -1,		(T) -1,		(T) M_WRITE_L,
	(T) -1,		(T) M_WRITE_S,	(T) -1,		(T) M_TIDY,
	(T) M_APPEND_L,	(T) -1,		(T) -1,		(T) -1,
/*$30*/	(T) -1,		(T) M_PRINT_S,	(T) -1,		(T) -1,
	(T) -1,		(T) -1,		(T) M_TIDY_1U,	(T) M_S_AUDIO,
	(T) -1,		(T) M_A_TRACK,	(T) -1,		(T) M_ADD_FILE,
	(T) -1,		(T) -1,		(T) -1,		(T) M_PRINT_L,
	#undef T
	};

	uint8_t hash = 0;
	int mode_idx;
	size_t i;

	/* hash */
	for ( i = 0; i < len; ++i ){
		hash ^= data[i] + 0x2Du;
	}
	hash >>= 1u;
	hash  &= 0x3Fu;

	/* verify */
	mode_idx = (int) mode_idx_table[hash];
	if ( (mode_idx < 0)
	    ||
	     ((size_t) f_mode_name_len[mode_idx] != len)
	    ||
	     (memcmp(data, f_mode_name[mode_idx], len) != 0)
	){
		return -1;
	}
	return mode_idx;
}

/* EOF //////////////////////////////////////////////////////////////////// */
