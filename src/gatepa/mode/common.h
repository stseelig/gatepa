#ifndef GATEPA_MODE_COMMON_H
#define GATEPA_MODE_COMMON_H
/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// mode/common.h                                                            //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <assert.h>
#include <stddef.h>

#include <libs/bitset.h>

#include "../attributes.h"
#include "../errors.h"

/* //////////////////////////////////////////////////////////////////////// */

#define MODE_SEP_COUNT(x_nfields)	do { \
	err.gat = arg_sep_count(arg_str, arg_len, arg_sep, (x_nfields)); \
	if ( err.gat != 0 ){ \
		/*@-mustmod@*/ \
		return err.gat; \
		/*@=mustmod@*/ \
	} \
} while ( /*@-predboolptr@*/ 0 /*@=predboolptr@*/ );

#define MODE_RANGE_GET(x_gbs_ptr, x_size_read_ptr)	do { \
	err.gat = arg_range_get( \
		(x_gbs_ptr), (x_size_read_ptr), arg_str, arg_len, arg_sep, \
		num_files \
	); \
	if ( err.gat != 0 ){ \
		/*@-mustmod@*/ \
		return err.gat; \
		/*@=mustmod@*/ \
	} \
} while ( /*@-predboolptr@*/ 0 /*@=predboolptr@*/ );

#define MODE_KEY_GET(x_key_ptr)		do { \
	err.gat = arg_key_get( \
		(x_key_ptr), &arg_str[arg_idx], arg_len - arg_idx, arg_sep \
	); \
	if ( err.gat != 0 ){ \
		/*@-mustmod@*/ \
		return err.gat; \
		/*@=mustmod@*/ \
	} \
} while ( /*@-predboolptr@*/ 0 /*@=predboolptr@*/ );

#define MODE_KEY_GET_NOVERIFY(x_key_ptr)		do { \
	err.gat = arg_field_get( \
		(x_key_ptr), &arg_str[arg_idx], arg_len - arg_idx, arg_sep \
	); \
	if ( err.gat != 0 ){ \
		/*@-mustmod@*/ \
		return err.gat; \
		/*@=mustmod@*/ \
	} \
} while ( /*@-predboolptr@*/ 0 /*@=predboolptr@*/ );

#define MODE_VALUE_GET(x_value_ptr)	do { \
	err.gat = arg_value_get( \
		(x_value_ptr), &arg_str[arg_idx], arg_len - arg_idx, arg_sep \
	); \
	if ( err.gat != 0 ){ \
		/*@-mustmod@*/ \
		return err.gat; \
		/*@=mustmod@*/ \
	} \
} while ( /*@-predboolptr@*/ 0 /*@=predboolptr@*/ );

#define MODE_PATH_GET(x_path_ptr)	do { \
	err.gat = arg_path_get( \
		(x_path_ptr), &arg_str[arg_idx], arg_len - arg_idx, arg_sep \
	); \
	if ( err.gat != 0 ){ \
		/*@-mustmod@*/ \
		return err.gat; \
		/*@=mustmod@*/ \
	} \
	assert((x_path_ptr) != NULL); \
} while ( /*@-predboolptr@*/ 0 /*@=predboolptr@*/ );

/* //////////////////////////////////////////////////////////////////////// */

NOINLINE
GATEPA_EXTERN enum GatepaErr arg_sep_count(const char *, size_t, char, size_t)
/*@*/
;

#undef range_gbs
NOINLINE
GATEPA_EXTERN enum GatepaErr arg_range_get(
	struct GBitset *range_gbs, /*@out@*/ /*@null@*/ size_t *range_len_out,
	const char *, size_t, char, unsigned int
)
/*@modifies	*range_gbs,
		*range_len_out
@*/
;

#undef key
NOINLINE
GATEPA_EXTERN enum GatepaErr arg_key_get(
	/*@out@*/ struct GString *key, const char *, size_t, char
)
/*@modifies	*key@*/
;

#undef value
NOINLINE
GATEPA_EXTERN enum GatepaErr arg_value_get(
	/*@out@*/ struct GString *value, const char *, size_t, char
)
/*@modifies	*value@*/
;

#undef value
NOINLINE
GATEPA_EXTERN enum GatepaErr arg_path_get(
	/*@out@*/ char **path, const char *, size_t, char
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*path
@*/
;

#undef field
GATEPA_EXTERN enum GatepaErr arg_field_get(
	/*@out@*/ struct GString *field, const char *, size_t, char
)
/*@modifies	*field@*/
;

/* ------------------------------------------------------------------------ */

PURE
GATEPA_EXTERN enum GatepaErr verify_key(const uint8_t *, size_t) /*@*/;

PURE
GATEPA_EXTERN enum GatepaErr verify_value_text(const uint8_t *, size_t) /*@*/;

/* EOF //////////////////////////////////////////////////////////////////// */
#endif	/* GATEPA_MODE_COMMON_H */
