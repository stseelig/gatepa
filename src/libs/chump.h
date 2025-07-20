#ifndef CHUMP_H
#define CHUMP_H
/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// chump.h - chained bump allocator                                         //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <stddef.h>
#include <stdint.h>

/* //////////////////////////////////////////////////////////////////////// */

typedef /*@only@*/ /*@null@*/ /*@reldef@*/ void * x_gstring_void_onrd;

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
	const x_gstring_void_onrd	x_0[4u];
	const struct Chump_Stats	stats;
	const struct Chump_Config	config;
};

/* ======================================================================== */

#define CHUMP_CONFIG_STATIC_INIT( \
	x_block_size, x_big_alloc_min, x_block_full_margin, \
	x_block_align, x_ptr_align \
) { \
	(x_block_size), (x_big_alloc_min), (x_block_full_margin), \
	(x_block_align), (x_ptr_align) \
}

/* ------------------------------------------------------------------------ */

#define CHUMP_STATIC_INIT_NULL		{ \
	{NULL, NULL, NULL, NULL}, {0,0,0,0}, {0,0,0,0,0} \
}

/* !!! config must be valid, or UB !!! */
#define CHUMP_STATIC_INIT(x_config)	{ \
	{NULL, NULL, NULL, NULL}, {0,0,0,0}, x_config \
}

/* //////////////////////////////////////////////////////////////////////// */

#undef chump
#undef config
/*@external@*/ /*@unused@*/
extern int chump_init(
	/*@out@*/ struct Chump *chump, const struct Chump_Config *config
)
/*@modifies	*chump@*/
;

#undef chump
/*@external@*/ /*@unused@*/
extern void chump_destroy(struct Chump *chump)
/*@globals	internalState@*/
/*@modifies	internalState,
		*chump
@*/
/*@releases	chump->x_0[]@*/
;

#undef chump
#undef blocks_kept_max
/*@external@*/ /*@unused@*/
extern int chump_reset(struct Chump *chump, uint32_t blocks_kept_max)
/*@globals	internalState@*/
/*@modifies	internalState,
		*chump
@*/
;

#undef chump
#undef size
#undef nmemb
/*@external@*/ /*@unused@*/
/*@temp@*/ /*@null@*/ /*@out@*/
extern void *chump_alloc(struct Chump *chump, uint32_t size, uint32_t nmemb)
/*@globals	internalState@*/
/*@modifies	internalState,
		*chump
@*/
;

/* EOF //////////////////////////////////////////////////////////////////// */
#endif	/* CHUMP_H */
