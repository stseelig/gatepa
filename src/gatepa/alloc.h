#ifndef GATEPA_ALLOC_H
#define GATEPA_ALLOC_H
/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// alloc.h                                                                  //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <stddef.h>
#include <stdint.h>

#include <libs/gbitset.h>
#include <libs/gstring.h>

#include "attributes.h"

/* //////////////////////////////////////////////////////////////////////// */

/*@unchecked@*/ /*@unused@*/
extern const struct GBitset_MyAlloc g_myalloc_gbitset;

/*@unchecked@*/ /*@unused@*/
extern const struct GString_MyAlloc g_myalloc_gstring;

/* //////////////////////////////////////////////////////////////////////// */

GATEPA_EXTERN void gatepa_alloc_destroy(void)
/*@globals	internalState@*/
/*@modifies	internalState@*/
;

/*@temp@*/ /*@null@*/ /*@reldef@*/
GATEPA_EXTERN void *gatepa_alloc_a1(size_t, size_t)
/*@globals	internalState@*/
/*@modifies	internalState@*/
;

/*@temp@*/ /*@null@*/ /*@reldef@*/
GATEPA_EXTERN void *gatepa_alloc_a1_gstring(size_t)
/*@globals	internalState@*/
/*@modifies	internalState@*/
;

/*@temp@*/ /*@null@*/ /*@reldef@*/
GATEPA_EXTERN void *gatepa_alloc_a16(size_t, size_t)
/*@globals	internalState@*/
/*@modifies	internalState@*/
;

#undef ptr_old
#undef size
#undef nmemb_old
#undef nmemb_new
/*@temp@*/ /*@null@*/ /*@reldef@*/
GATEPA_EXTERN void *gatepa_realloc_a16(
	/*@temp@*/ /*@null@*/ /*@reldef@*/ void *ptr_old,
	size_t size, size_t nmemb_old, size_t nmemb_new
)
/*@globals	internalState@*/
/*@modifies	internalState@*/
;

/*@temp@*/ /*@null@*/ /*@reldef@*/
GATEPA_EXTERN void *gatepa_alloc_scratch(size_t, size_t)
/*@globals	internalState@*/
/*@modifies	internalState@*/
;

GATEPA_EXTERN int gatepa_alloc_scratch_reset(void)
/*@globals	internalState@*/
/*@modifies	internalState@*/
;

/* EOF //////////////////////////////////////////////////////////////////// */
#endif	/* GATEPA_ALLOC_H */
