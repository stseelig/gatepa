/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// opts.c                                                                   //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "apetag.h"
#include "help.h"
#include "mode.h"

/* //////////////////////////////////////////////////////////////////////// */

/*@maynotreturn@*/
COLD
static int opt_help(unsigned int, /*@null@*/ const char *, size_t)
/*@globals	fileSystem@*/
/*@modifies	fileSystem@*/
;

static int opt_g_apetag_strtol(unsigned int, /*@null@*/ const char *, size_t)
/*@globals	g_apetag@*/
/*@modifies	g_apetag@*/
;

/* //////////////////////////////////////////////////////////////////////// */

typedef int (*gatepa_fnptr_opt)(
	unsigned int, /*@null@*/ const char *, size_t
);

#define GATEPA_NUM_OPTS			5u

#define OPT_G_APETAG_STRTOL_START	1u
#define OPT_G_APETAG_STRTOL_END		4u

/*@unchecked@*/ /*@observer@*/
static const char *f_opt_name[GATEPA_NUM_OPTS] = {
	"help",
	"softlimit-items-size",
	"softlimit-key-size",
	"limit-binary-name",
	"limit-binary-fext"
};

static const uint8_t f_opt_name_len[GATEPA_NUM_OPTS] = {
	UINT8_C( 4),	/* help                 */
	UINT8_C(20),	/* softlimit-items-size */
	UINT8_C(18),	/* softlimit-key-size   */
	UINT8_C(17),	/* limit-binary-name    */
	UINT8_C(17)	/* limit-binary-fext    */
};

static const gatepa_fnptr_opt f_opt_fn[GATEPA_NUM_OPTS] = {
	opt_help,
	opt_g_apetag_strtol,
	opt_g_apetag_strtol,
	opt_g_apetag_strtol,
	opt_g_apetag_strtol,
};

/* //////////////////////////////////////////////////////////////////////// */

GATEPA int
opt_process(const char *const arg)
/*@globals	fileSystem,
		internalState
@*/
/*@modifies	fileSystem,
		internalState
@*/
{
	const size_t      arg_len = strlen(arg);
	/*@null@*/
	const char *const eqs_ptr = memchr(arg, '=', arg_len);
	/*@null@*/
	const char *const opt_arg = (eqs_ptr != NULL ? &eqs_ptr[1u] : NULL);
	const size_t  opt_arg_len = (size_t) (opt_arg != NULL
		? arg_len - (size_t) ((uintptr_t) opt_arg - (uintptr_t) arg)
		: 0
	);
	const size_t arg_name_len = (
		arg_len - opt_arg_len - 2u - (uint8_t) (eqs_ptr != NULL)
	);
	unsigned int i;

	if ( (arg_len <= (size_t) 2u)
	    ||
	     (memcmp(arg, "--", (size_t) 2u) != 0)
	){
		return -1;
	}

	for ( i = 0; i < GATEPA_NUM_OPTS; ++i ){
		if ( (arg_name_len == f_opt_name_len[i])
		    &&
		     (memcmp(&arg[2u], f_opt_name[i], arg_name_len) == 0)
		){
			return f_opt_fn[i](i, opt_arg, opt_arg_len);
		}
	}
	return -1;
}

/* ======================================================================== */

/* on success: prints program help, than exits with status EXIT_FAILURE
   on failure: returns -1
*/
/*@maynotreturn@*/
COLD
static int
opt_help(
	/*@unused@*/ const unsigned int opt_idx,
	/*@null@*/ const char *const arg, const size_t arg_len
)
/*@globals	fileSystem@*/
/*@modifies	fileSystem@*/
{
	int mode_idx;

	/*@-noeffect@*/
	(void) opt_idx;
	/*@=noeffect@*/

	if ( arg != NULL ){
		mode_idx = get_mode_idx((const uint8_t *) arg, arg_len);
		if ( mode_idx < 0 ){
			return -1;
		}
		help_mode(stderr, mode_idx);
	}
	else {	help_usage(stderr);
		help_opts(stderr);
	}

	exit(EXIT_FAILURE);
}

/* returns 0 on success */
static int
opt_g_apetag_strtol(
	const unsigned int opt_idx,
	/*@null@*/ const char *const arg, const size_t arg_len
)
/*@globals	g_apetag@*/
/*@modifies	g_apetag@*/
{
	const unsigned int opt_base = OPT_G_APETAG_STRTOL_START;
	/* * */
	long value;
	char *endptr;
	size_t size_read;

	assert((opt_idx >= OPT_G_APETAG_STRTOL_START)
	      &&
	       (opt_idx <= OPT_G_APETAG_STRTOL_END)
	);

	/* read value */
	if ( (arg == NULL) || (arg_len == 0) ){
		/*@-mustmod@*/
		return -1;
		/*@=mustmod@*/
	}
	errno = 0;
	value = strtol(arg, &endptr, 10);
	if ( errno != 0 ){
		/*@-mustmod@*/
		return -1;
		/*@=mustmod@*/
	}
	if ( (value < 0) || (value > (long) UINT32_MAX) ){
		/*@-mustmod@*/
		return -1;
		/*@=mustmod@*/
	}
	size_read = (size_t) (((uintptr_t) endptr) - ((uintptr_t) arg));
	if ( size_read != arg_len ){
		/*@-mustmod@*/
		return -1;
		/*@=mustmod@*/
	}

	/* set value */
	if ( value == 0 ){
		value = UINT32_MAX;
	}
	((uint32_t *) &g_apetag)[opt_idx - opt_base] = (uint32_t) value;

	return 0;
}

/* EOF //////////////////////////////////////////////////////////////////// */
