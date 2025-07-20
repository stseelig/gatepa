/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// chump/init.c                                                             //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <stddef.h>
#include <stdint.h>

#include "common.h"

/* //////////////////////////////////////////////////////////////////////// */

#define CHUMP_INIT(x_config)	(struct Chump) { \
	NULL, NULL, NULL, NULL, {0,0,0,0}, x_config \
}

/* //////////////////////////////////////////////////////////////////////// */

/* returns 0 on success */
/*@unused@*/
int
chump_init(
	/*@out@*/ struct Chump *const chump,
	const struct Chump_Config *const config
)
/*@modifies	*chump@*/
{
	if ( (config->block_size < config->big_alloc_min)
	    ||
	     (config->block_size < config->block_full_margin)
	    ||
	     (config->block_size < config->ptr_align)
	){
		return -1;
	}
	if ( (popcount_u32((uint32_t) config->block_size)  != 1u)
	    ||
	     (popcount_u32((uint32_t) config->block_align) != 1u)
	    ||
	     (popcount_u32((uint32_t) config->ptr_align)   != 1u)
	){
		return -1;
	}
	if ( ((config->block_size  & (config->block_align - 1u)) != 0)
	    ||
	     ((config->block_align & (config->ptr_align   - 1u)) != 0) ){
		return -1;
	}

	*chump = CHUMP_INIT(*config);

	return 0;
}

/* EOF //////////////////////////////////////////////////////////////////// */
