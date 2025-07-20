/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// chump/destroy.c                                                          //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <assert.h>
#include <stdlib.h>

#include "common.h"

/* //////////////////////////////////////////////////////////////////////// */

/*@unused@*/
void
chump_destroy(struct Chump *const chump)
/*@globals	internalState@*/
/*@modifies	internalState,
		*chump
@*/
/*@releases	chump->open[],
		chump->full[],
		chump->big[],
		chump->open,
		chump->open_nbytes_avail,
		chump->full,
		chump->big
@*/
{
	uint32_t i;

	/* inner */
	for( i = 0; i < chump->stats.nmemb_open; ++i ){
		assert(chump->open != NULL);
		free(chump->open[i]);
	}
	for( i = 0; i < chump->stats.nmemb_full; ++i ){
		assert(chump->full != NULL);
		free(chump->full[i]);
	}
	for( i = 0; i < chump->stats.nmemb_big; ++i ){
		assert(chump->big != NULL);
		free(chump->big[i]);
	}

	/* outer */
	free(chump->open);
	free(chump->open_nbytes_avail);
	free(chump->full);
	free(chump->big);

	/*@-mustfreeonly@*/ /*@-usereleased@*/ /*@-compdef@*/
	return;
	/*@=mustfreeonly@*/ /*@=usereleased@*/ /*@=compdef@*/
}

/* EOF //////////////////////////////////////////////////////////////////// */
