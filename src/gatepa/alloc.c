/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// alloc.c                                                                  //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <libs/chump.h>
#include <libs/gbitset.h>
#include <libs/gstring.h>
#include <libs/overflow.h>

#include "alloc.h"
#include "attributes.h"

/* //////////////////////////////////////////////////////////////////////// */

/* for tag items/values (text strings and binary data) */
#define CONFIG_A1	CHUMP_CONFIG_STATIC_INIT( \
	UINT32_C(4096), \
	UINT32_C(4096), \
	UINT32_C(  16), \
	UINT16_C(4096), \
	UINT16_C(   1) \
)
#define CONFIG_A16	CHUMP_CONFIG_STATIC_INIT( \
	UINT32_C(4096), \
	UINT32_C(4096), \
	UINT32_C(  32), \
	UINT16_C(4096), \
	UINT16_C(  16) \
)

/* for tag items/values (text strings and binary data) */
static struct Chump f_chump_a1	    = CHUMP_STATIC_INIT(CONFIG_A1);

/* for other tag/item stuff */
static struct Chump f_chump_a16     = CHUMP_STATIC_INIT(CONFIG_A16);

/* for temp data */
static struct Chump f_chump_scratch = CHUMP_STATIC_INIT(CONFIG_A16);

/* //////////////////////////////////////////////////////////////////////// */

/*@temp@*/ /*@null@*/ /*@reldef@*/
static void *gatepa_calloc_a16(size_t, size_t)
/*@globals	internalState,
		f_chump_a16
@*/
/*@modifies	internalState,
		f_chump_a16
@*/
;

/* ======================================================================== */

/*@unchecked@*/
const struct GBitset_MyAlloc g_myalloc_gbitset = {
	gatepa_calloc_a16, GBITSET_NULL_FNPTR_FREE
};

/*@unchecked@*/
const struct GString_MyAlloc g_myalloc_gstring = {
	gatepa_alloc_a1_gstring, GSTRING_NULL_FNPTR_FREE
};

/* //////////////////////////////////////////////////////////////////////// */

GATEPA void
gatepa_alloc_destroy(void)
/*@globals	internalState,
		f_chump_a1,
		f_chump_a16,
		f_chump_scratch
@*/
/*@modifies	internalState,
		f_chump_a1,
		f_chump_a16,
		f_chump_scratch
@*/
{
	chump_destroy(&f_chump_a1);
	chump_destroy(&f_chump_a16);
	chump_destroy(&f_chump_scratch);
	return;
}

/* ======================================================================== */

/* returns NULL on failure */
/*@temp@*/ /*@null@*/ /*@reldef@*/
GATEPA void *
gatepa_alloc_a1(const size_t size, const size_t nmemb)
/*@globals	internalState,
		f_chump_a1
@*/
/*@modifies	internalState,
		f_chump_a1
@*/
{
	if ( (size > UINT32_MAX) || (nmemb > UINT32_MAX) ){
		return NULL;
	}

	return chump_alloc(&f_chump_a1, (uint32_t) size, (uint32_t) nmemb);
}

/* returns NULL on failure */
/*@temp@*/ /*@null@*/ /*@reldef@*/
GATEPA void *
gatepa_alloc_a1_gstring(const size_t size)
/*@globals	internalState,
		f_chump_a1
@*/
/*@modifies	internalState,
		f_chump_a1
@*/
{
	if ( size > UINT32_MAX ){
		return NULL;
	}

	return chump_alloc(&f_chump_a1, (uint32_t) size, (uint32_t) 1u);
}

/* returns NULL on failure */
/*@temp@*/ /*@null@*/ /*@reldef@*/
GATEPA void *
gatepa_alloc_a16(const size_t size, const size_t nmemb)
/*@globals	internalState,
		f_chump_a16
@*/
/*@modifies	internalState,
		f_chump_a16
@*/
{
	if ( (size > UINT32_MAX) || (nmemb > UINT32_MAX) ){
		return NULL;
	}

	return chump_alloc(&f_chump_a16, (uint32_t) size, (uint32_t) nmemb);
}

/* returns NULL on failure */
/*@temp@*/ /*@null@*/ /*@reldef@*/
static void *
gatepa_calloc_a16(const size_t size, const size_t nmemb)
/*@globals	internalState,
		f_chump_a16
@*/
/*@modifies	internalState,
		f_chump_a16
@*/
{
	void *retval;

	if ( (size > UINT32_MAX) || (nmemb > UINT32_MAX) ){
		return NULL;
	}

	retval = chump_alloc(&f_chump_a16, (uint32_t) size, (uint32_t) nmemb);
	if ( retval == NULL ){
		return NULL;
	}
	assert(retval != NULL);

	memset(retval, 0x00, (size_t) (size * nmemb));	/* NULL if overflow */
	return retval;
}

/* returns NULL on failure */
/*@temp@*/ /*@null@*/ /*@reldef@*/
GATEPA void *
gatepa_realloc_a16(
	/*@temp@*/ /*@null@*/ /*@reldef@*/ void *const ptr_old,
	const size_t size, const size_t nmemb_old, const size_t nmemb_new
)
/*@globals	internalState,
		f_chump_a16
@*/
/*@modifies	internalState,
		f_chump_a16
@*/
{
	void *ptr_new;
	size_t size_old;
	int err;

	err = mul_usize_overflow(&size_old, size, nmemb_old);
	if ( (err != 0) || (size_old > UINT32_MAX) || (size > UINT32_MAX)
	    || (nmemb_new > UINT32_MAX) || (nmemb_old > UINT32_MAX)
	){
		return NULL;
	}

	ptr_new = chump_alloc(&f_chump_a16, size, nmemb_new);
	if ( (ptr_new != NULL) && (ptr_old != NULL) && (size_old != 0) ){
		return memcpy(ptr_new, ptr_old, size_old);
	}
	return ptr_new;
}

/* returns NULL on failure */
/*@temp@*/ /*@null@*/ /*@reldef@*/
GATEPA void *
gatepa_alloc_scratch(const size_t size, const size_t nmemb)
/*@globals	internalState,
		f_chump_scratch
@*/
/*@modifies	internalState,
		f_chump_scratch
@*/
{
	if ( (size > UINT32_MAX) || (nmemb > UINT32_MAX) ){
		return NULL;
	}

	return chump_alloc(
		&f_chump_scratch, (uint32_t) size, (uint32_t) nmemb
	);
}

/* returns 0 on success */
GATEPA int
gatepa_alloc_scratch_reset(void)
/*@globals	internalState,
		f_chump_scratch
@*/
/*@modifies	internalState,
		f_chump_scratch
@*/
{
	return chump_reset(&f_chump_scratch, (uint32_t) 1u);
}

/* EOF //////////////////////////////////////////////////////////////////// */
