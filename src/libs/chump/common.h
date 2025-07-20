#ifndef CHUMP_COMMON_H
#define CHUMP_COMMON_H
/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// chump/common.h                                                           //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>

#include "builtins.h"

/* //////////////////////////////////////////////////////////////////////// */

#if SIZE_MAX < UINT32_MAX
#error "SIZE_MAX < UINT32_MAX"
#endif

/* //////////////////////////////////////////////////////////////////////// */

#define CHUMP_NMEMB_MOD		((uint32_t) 16u)

/* //////////////////////////////////////////////////////////////////////// */

/* x_align is power of 2 (popcount of 1) */
#define ALIGN_BACKWARDS(x_size, x_align)	( \
	(assert(popcount_u32(x_align) == 1u)), \
	((x_size) & (UINTMAX_MAX ^ ((x_align) - 1u))) \
)

/* //////////////////////////////////////////////////////////////////////// */

typedef /*@owned@*/ /*@reldef@*/ uint8_t *	uint8_ownedptr;
typedef /*@only@*/ /*@null@*/ uint8_ownedptr *	uint8_ownedptr_onlynullptr;

/* ======================================================================== */

struct Chump_Config {
	uint32_t	block_size;
	uint32_t	big_alloc_min;
	uint32_t	block_full_margin;
	uint16_t	block_align;
	uint16_t	ptr_align;
};

struct Chump_Stats {
	uint32_t	nmemb_open;
	uint32_t	nmemb_full;
	uint32_t	nmemb_big;
	uint32_t	num_allocs;	/* saturates */
};

struct Chump {
	/*@only@*/ /*@null@*/ /*@reldef@*/
	uint8_ownedptr		*open;
	/*@only@*/ /*@null@*/ /*@reldef@*/
	uint32_t		*open_nbytes_avail;
	/*@only@*/ /*@null@*/ /*@reldef@*/
	uint8_ownedptr		*full;
	/*@only@*/ /*@null@*/ /*@reldef@*/
	uint8_ownedptr		*big;

	struct Chump_Stats	stats;
	struct Chump_Config	config;
};

/* //////////////////////////////////////////////////////////////////////// */

/*@unused@*/
/*@only@*/ /*@null@*/ /*@reldef@*/
ALWAYS_INLINE void *
realloc_addmul_check(
	/*@only@*/ /*@null@*/ /*@reldef@*/ void *const ptr,
	uint32_t size, const uint32_t nmemb_old
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*ptr
@*/
{
	uint32_t nmemb_new;
	int err;

	err = add_u32_overflow(&nmemb_new, nmemb_old, CHUMP_NMEMB_MOD);
	if ( err != 0 ){
		if ( nmemb_old == UINT32_MAX ){
			/*@-mustfreeonly@*/
			return NULL;
			/*@=mustfreeonly@*/
		}
		nmemb_new = UINT32_MAX;
	}

	err = mul_u32_overflow(&size, size, nmemb_new);
	if ( err != 0 ){
		/*@-mustfreeonly@*/
		return NULL;
		/*@=mustfreeonly@*/
	}

	return realloc(ptr, size);
}

/* EOF //////////////////////////////////////////////////////////////////// */
#endif	/* CHUMP_COMMON_H */
