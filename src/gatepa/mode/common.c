/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// mode/common.c                                                            //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <libs/ascii-literals.h>
#include <libs/bitset.h>
#include <libs/gbitset.h>
#include <libs/gstring.h>
#include <libs/overflow.h>

#include "../alloc.h"
#include "../attributes.h"
#include "../errors.h"
#include "../text.h"

#include "common.h"

/* //////////////////////////////////////////////////////////////////////// */

#if   INT_MAX == INT16_MAX
#define NDIGITS_INT_MAX		((size_t)  5u)
#elif INT_MAX == INT32_MAX
#define NDIGITS_INT_MAX		((size_t) 10u)
#elif INT_MAX == INT64_MAX
#define NDIGITS_INT_MAX		((size_t) 19u)
#else
#error "NDIGITS_INT_MAX"
#endif

/* //////////////////////////////////////////////////////////////////////// */

/* ranges range from 1 to INT_MAX */
struct RangeItem {
	unsigned int		first;
	unsigned int		last;		/* UINT_MAX: all of them */
};

/* //////////////////////////////////////////////////////////////////////// */

NOINLINE PURE
static size_t sep_count(const char *, size_t, char) /*@*/;

#undef range_gbs
#undef size_read
static enum GatepaErr arg_range_get_item(
	/*@out@*/ struct RangeItem *range_out, /*@out@*/ size_t *size_read,
	const char *, size_t, char
)
/*@modifies	*range_out,
		*size_read
@*/
;

PURE
static size_t count_digits(const char *, size_t) /*@*/;

#undef value
static enum GatepaErr get_strtoi(/*@out@*/ unsigned int *value, const char *)
/*@modifies	*value@*/
;

/* //////////////////////////////////////////////////////////////////////// */

/* returns 0 on success */
NOINLINE
GATEPA enum GatepaErr
arg_sep_count(
	const char *const arg_str, const size_t arg_len, const char arg_sep,
	const size_t limit
)
/*@*/
{
	const size_t count = sep_count(arg_str, arg_len, arg_sep);

	if ( ((count == limit) && (arg_str[arg_len - 1u] != arg_sep))
	    ||
	     (count > limit)
	){
		return GATERR_MODESTR_NFIELDS;
	}
	return 0;
}

/* returns the number of seperators in the string */
NOINLINE PURE
static size_t
sep_count(
	const char *const arg_str, const size_t arg_len, const char arg_sep
)
/*@*/
{
	size_t count = 0;
	size_t i;

	for ( i = 0; i < arg_len; ++i ){
		if ( arg_str[i] == arg_sep ){
			count += 1u;
		}
	}
	return count;
}

/* returns 0 on success */
NOINLINE
GATEPA enum GatepaErr
arg_range_get(
	struct GBitset *const range_gbs,
	/*@out@*/ /*@null@*/ size_t *const range_len_out,
	const char *const arg_str, const size_t arg_len, const char arg_sep,
	const unsigned int num_files
)
/*@modifies	*range_gbs,
		range_len_out
@*/
{
	struct RangeItem item;
	size_t range_len, range_nmemb, range_idx;
	size_t size_read;
	void *temp_ptr;
	union {	int		i;
		enum GatepaErr	gat;
	} err;
	size_t i;

	/* clear the bitset */
	bitset_set_range_0(GBITSET_PTR(range_gbs), 0, range_gbs->bitlen - 1u);

	/* get the total length and nmemb of the range string */
	temp_ptr = memchr(arg_str, (int) arg_sep, arg_len);
	range_len   = (temp_ptr == NULL
		? arg_len
		: (size_t) (((uintptr_t) temp_ptr) - ((uintptr_t) arg_str))
	);
	range_nmemb = sep_count(arg_str, range_len, ',') + 1u;

	/* add each range to the bitset */
	range_idx = 0;
	for ( i = 0; i < range_nmemb; ++i ){
		err.gat = arg_range_get_item(
			&item, &size_read, &arg_str[range_idx],
			range_len - range_idx, arg_sep
		);
		if ( err.gat != 0 ){
			/*@-mustdefine@*/ /*@-mustmod@*/
			return err.gat;
			/*@=mustdefine@*/ /*@=mustmod@*/
		}
		if ( item.last == UINT_MAX ){
			item.last = num_files;
		}
		if ( (item.first > num_files) || (item.last > num_files) ){
			/*@-mustdefine@*/ /*@-mustmod@*/
			return GATERR_RANGESTR_VALUE;
			/*@=mustdefine@*/ /*@=mustmod@*/
		}
		assert(size_read != SIZE_MAX);
		err.i = add_usize_overflow(
			&range_idx, range_idx, size_read + 1u
		);
		if ( err.i != 0 ){
			/*@-mustdefine@*/ /*@-mustmod@*/
			return GATERR_OVERFLOW;
			/*@=mustdefine@*/ /*@=mustmod@*/
		}

		assert( (item.first > 0) && (item.last > 0)
		       &&
		        (item.first <= item.last)
		);
		bitset_set_range_1(
			GBITSET_PTR(range_gbs),
			(size_t) (item.first - 1u), (size_t) (item.last - 1u)
		);
	}
	if ( range_len != range_idx - 1u ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return GATERR_RANGESTR_MALFORMED;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}

	if ( range_len_out != NULL ){
		*range_len_out = range_idx;
	}
	return 0;
}

/* ranges are 1-based indices */
/* returns 0 on success */
static enum GatepaErr
arg_range_get_item(
	/*@out@*/ struct RangeItem *const range_out,
	/*@out@*/ size_t *const size_read, const char *const arg_str,
	const size_t arg_len, const char arg_sep
)
/*@modifies	*range_out,
		*size_read
@*/
{
	size_t idx = 0;
	unsigned int first = 1u, last = UINT_MAX;
	size_t ndigits;
	enum GatepaErr err;
	char temp_c;

	/* -1 index should be OK, because the string should be offset a bit
	     from its actual start
	*/

	if ( ((arg_sep == '-') || (arg_sep == ','))
	    ||
	     ((arg_str[-1] != arg_sep) && (arg_str[-1] != ','))
	){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return GATERR_MODESTR_SEP;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}

	if ( (arg_len == 0) || (arg_str[0] == arg_sep) ){
		if ( arg_str[-1] == ',' ){
			/*@-mustdefine@*/ /*@-mustmod@*/
			return GATERR_RANGESTR_EMPTY;
			/*@=mustdefine@*/ /*@=mustmod@*/
		}
		goto fn_ret;
	}

	if ( arg_str[0] != '-' ){
		/* '$start[-$end]' */
		if ( arg_len - 1u < idx ){
			/*@-mustdefine@*/ /*@-mustmod@*/
			return GATERR_RANGESTR_MALFORMED;
			/*@=mustdefine@*/ /*@=mustmod@*/
		}
		ndigits = count_digits(&arg_str[idx], arg_len - idx);
		if ( (ndigits == 0) || (ndigits > NDIGITS_INT_MAX) ){
			/*@-mustdefine@*/ /*@-mustmod@*/
			return GATERR_RANGESTR_STRTOL;
			/*@=mustdefine@*/ /*@=mustmod@*/
		}
		err = get_strtoi(&first, &arg_str[idx]);
		if ( err != 0 ){
			/*@-mustdefine@*/ /*@-mustmod@*/
			return err;
			/*@=mustdefine@*/ /*@=mustmod@*/
		}

		idx   += ndigits;
		temp_c = arg_str[idx];
		if ( (arg_len > idx) && (temp_c == '-') ){
			goto get_last;
		}
		else if ( (arg_len == idx)
		         ||
		          ((temp_c == ',') || (temp_c == arg_sep))
		){
			last = first;
		}
		else{;}
	}
	else {	/* '-[$end]' (start at 0) */
	/*
		first   = 1u;
	*/
get_last:
		idx += 1u;
		if ( arg_len - 1u < idx ){
			last = UINT_MAX;
			goto fn_ret;
		}
		ndigits = count_digits(&arg_str[idx], arg_len - idx);
		if ( ndigits > NDIGITS_INT_MAX ){
			/*@-mustdefine@*/ /*@-mustmod@*/
			return GATERR_RANGESTR_STRTOL;
			/*@=mustdefine@*/ /*@=mustmod@*/
		}
		else if ( ndigits == 0 ){
			temp_c = arg_str[idx];
			if ( (idx < arg_len)
			    &&
			     ((temp_c != ',') && (temp_c != arg_sep))
			){
				/*@-mustdefine@*/ /*@-mustmod@*/
				return GATERR_RANGESTR_MALFORMED;
				/*@=mustdefine@*/ /*@=mustmod@*/
			}
		/*
			last = UINT_MAX;
		*/
		}
		else {	err = get_strtoi(&last, &arg_str[idx]);
			if ( err != 0 ){
				/*@-mustdefine@*/ /*@-mustmod@*/
				return err;
				/*@=mustdefine@*/ /*@=mustmod@*/
			}
			idx += ndigits;
		}
	}

	if ( first > last ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return GATERR_RANGESTR_VALUE;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}

fn_ret:
	assert((first != 0) && (last != 0) && (first <= last));

	*range_out = (struct RangeItem) { first, last };
	*size_read = idx;
	return 0;
}

/* ------------------------------------------------------------------------ */

/* returns the number of digits counted */
PURE
static size_t
count_digits(const char *const str, const size_t len)
/*@*/
{
	size_t i;

	for ( i = 0; i < len; ++i ){
		if ( !isdigit((int) str[i]) ){
			break;
		}
	}
	return i;
}

/* returns 0 on success */
static enum GatepaErr
get_strtoi(/*@out@*/ unsigned int *const value, const char *const str)
/*@modifies	*value@*/
{
	long temp;

	errno = 0;
	temp = strtol(str, NULL, 10);
	if ( errno != 0 ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return GATERR_RANGESTR_STRTOL;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}
	if ( (temp <= 0) || (temp > (long) INT_MAX) ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return GATERR_RANGESTR_VALUE;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}

	*value = (unsigned int) temp;
	return 0;
}

/* ======================================================================== */

/* returns 0 on success */
NOINLINE
GATEPA enum GatepaErr
arg_key_get(
	/*@out@*/ struct GString *const key,
	const char *const arg_str, const size_t arg_len, const char arg_sep
)
/*@modifies	*key@*/
{
	enum GatepaErr err;

	err = arg_field_get(key, arg_str, arg_len, arg_sep);
	if ( err != 0 ){
		return err;
	}
	return verify_key(GSTRING_PTR(key), key->len);
}

/* returns 0 on success */
NOINLINE
GATEPA enum GatepaErr
arg_value_get(
	/*@out@*/ struct GString *const value,
	const char *const arg_str, const size_t arg_len, const char arg_sep
)
/*@modifies	*value@*/
{
	enum GatepaErr err;

	err = arg_field_get(value, arg_str, arg_len, arg_sep);
	if ( err != 0 ){
		return err;
	}
	return verify_value_text(GSTRING_PTR(value), value->len);
}

/* returns 0 on success */
NOINLINE
GATEPA enum GatepaErr
arg_path_get(
	/*@out@*/ char **const path,
	const char *const arg_str, const size_t arg_len, const char arg_sep
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*path
@*/
{
	struct GString path_ref;
	char *buf = NULL;
	size_t buf_size;
	union {	int		i;
		enum GatepaErr	gat;
	} err;

	err.gat = arg_field_get(&path_ref, arg_str, arg_len, arg_sep);
	if ( err.gat != 0 ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return err.gat;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}

	/* create a new nul-terminated string */
	buf_size = (size_t) (path_ref.len + 1u);
	if ( buf_size == 0 ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return GATERR_OVERFLOW;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}
	buf = gatepa_alloc_scratch(buf_size, (size_t) 1u);
	if ( buf == NULL ){
		/*@-mustdefine@*/ /*@-mustmod@*/
		return GATERR_ALLOCATOR;
		/*@=mustdefine@*/ /*@=mustmod@*/
	}
	assert(buf != NULL);

	(void) memcpy(buf, GSTRING_PTR(&path_ref), path_ref.len);
	buf[path_ref.len] = '\0';

	*path = buf;
	return 0;
}

/* returns 0 on success */
GATEPA enum GatepaErr
arg_field_get(
	/*@out@*/ struct GString *const field,
	const char *const arg_str, const size_t arg_len, const char arg_sep
)
/*@modifies	*field@*/
{
	uintptr_t sep_addr;
	size_t field_len;
	int err;

	/* ob1's can happen during parsing if the string is malformed */
	if ( (arg_len == 0) || (arg_len == SIZE_MAX) ){
		return GATERR_MODESTR_SIZE_ZERO;
	}

	sep_addr  = (uintptr_t) memchr(arg_str, (int) arg_sep, arg_len);
	field_len = (sep_addr != 0
		? sep_addr - (uintptr_t) arg_str : arg_len
	);

	if ( field_len == 0 ){
		return GATERR_MODESTR_SIZE_ZERO;
	}

	err = gstring_ref_bstring(
		field, (const uint8_t *) arg_str, field_len
	);
	return (err == 0 ? 0 : GATERR_STRING);
}

/* ======================================================================== */

/* returns 0 on success */
PURE
GATEPA enum GatepaErr
verify_key(const uint8_t *const str, const size_t len)
/*@*/
{
	if ( len == 0 ){
		return GATERR_MODESTR_SIZE_ZERO;
	}

	return (ascii_isprintables(str, len) == 0 ? 0 : GATERR_KEYSTR_BAD);
}

/* returns 0 on success */
PURE
GATEPA enum GatepaErr
verify_value_text(const uint8_t *const str, const size_t len)
/*@*/
{
	if ( len == 0 ){
		return GATERR_MODESTR_SIZE_ZERO;
	}

	return (utf8_verify(str, len) == 0 ? 0 : GATERR_VALUESTR_BAD);
}

/* EOF //////////////////////////////////////////////////////////////////// */
