/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// apetag/file_check.c                                                      //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <libs/byteswap.h>
#include <libs/nbufio.h>

#include "../apetag.h"
#include "../attributes.h"
#include "../errors.h"

/* //////////////////////////////////////////////////////////////////////// */

enum ApeTag_TagHFType {
	APETAG_TAG_HEADER		= APEFLAG_IS_HEADER,
	APETAG_TAG_FOOTER		= 0
};

enum ApeTag_QVerify {
	APETAG_QVERIFY_PREAMBLE		= 0x01u,
	APETAG_QVERIFY_VERSION		= 0x02u,
	APETAG_QVERIFY_SIZE_NMEMB	= 0x04u,
	APETAG_QVERIFY_HF_TYPE		= 0x08u,
	APETAG_QVERIFY_RESERVED_BITS	= 0x10u
};
#define APETAG_QVERIFY_MALFORMED_MASK	( \
	  APETAG_QVERIFY_SIZE_NMEMB \
	| APETAG_QVERIFY_HF_TYPE \
	| APETAG_QVERIFY_RESERVED_BITS \
)

/* //////////////////////////////////////////////////////////////////////// */

PURE
static unsigned int apetag_file_tag_hf_qverify(
	const struct ApeTag_TagHF *, enum ApeTag_TagHFType
)
/*@*/
;

CONST
static enum TagCheckError qverify_err(unsigned int) /*@*/;

/* //////////////////////////////////////////////////////////////////////// */

/* returns 0 on success */
GATEPA enum TagCheckError
apetag_file_tag_check_eof(
	/*@out@*/ struct Gatepa_FileInfo *const fileinfo, const nbufio_fd file
)
/*@globals	fileSystem,
		internalState
@*/
/*@modifies	internalState,
		*fileinfo
@*/
{
	struct ApeTag_TagHF tag_hf[2u];
	unsigned int qverify[2u] = {0, 0};
	enum TagCheckError retval = 0;
	uint32_t items_size;
	off_t off_begin, off_end, off_items;
	off_t seek_offset;
	union {	size_t	z;
		off_t	o;
	} err;

	/* seek/read the footer */
	err.o = nbufio_seek(
		file, -((off_t) sizeof(struct ApeTag_TagHF)), SEEK_END
	);
	if ( err.o == NBUFIO_OFF_ERROR ){
		/*@-mustdefine@*/
		return TAGCHECK_ERR_SEEK;
		/*@=mustdefine@*/
	}
	/* * */
	err.z = nbufio_read(file, &tag_hf[0u], sizeof tag_hf[0u]);
	if ( err.z != sizeof tag_hf[0u] ){
		/*@-mustdefine@*/
		return (err.z != NBUFIO_RW_ERROR
			? TAGCHECK_ERR_READ_EOF : TAGCHECK_ERR_READ
		);
		/*@=mustdefine@*/
	}
	off_end = nbufio_tell(file);
	if ( off_end == NBUFIO_OFF_ERROR ){
		/*@-mustdefine@*/
		return TAGCHECK_ERR_SEEK;
		/*@=mustdefine@*/
	}

	/* check if we read an APETAGEX footer */
	qverify[0u] = apetag_file_tag_hf_qverify(
		&tag_hf[0u], APETAG_TAG_FOOTER
	);
	if ( qverify[0u] != 0 ){
		*fileinfo  = gatepa_fileinfo_make(
			0, 0, off_end, NBUFIO_OFF_ERROR, NBUFIO_OFF_ERROR
		);
		return qverify_err(qverify[0u]);
	}

	/* seek/read to check for the optional header */
	seek_offset  = (off_t) byteswap_u32_letoh(tag_hf[0u].size);
	if ( seek_offset <= (off_t) sizeof(struct ApeTag_TagHF) ){
		/* MAYBE: error here */
		off_begin  = off_end - ((off_t) sizeof(struct ApeTag_TagHF));
		off_items  = off_begin;
		goto fn_end;
	}
	seek_offset += ((off_t) sizeof(struct ApeTag_TagHF));
	err.o = nbufio_seek(file, -seek_offset, SEEK_CUR);
	if ( err.o == NBUFIO_OFF_ERROR ){
		/*@-mustdefine@*/
		return TAGCHECK_ERR_SEEK;
		/*@=mustdefine@*/
	}
	/* * */
	err.z = nbufio_read(file, &tag_hf[1u], sizeof tag_hf[1u]);
	if ( err.z != sizeof tag_hf[1u] ){
		/* TODO: check very small (tag-only) files handling */
		/*@-mustdefine@*/
		return (err.z != NBUFIO_RW_ERROR
			? TAGCHECK_ERR_READ_EOF : TAGCHECK_ERR_READ
		);
		/*@=mustdefine@*/
	}
	off_begin = off_end - ((off_t) byteswap_u32_letoh(tag_hf[0u].size));
	off_items = off_begin;

	/* check if we read an APETAGEX header */
	qverify[1u] = apetag_file_tag_hf_qverify(
		&tag_hf[1u], APETAG_TAG_HEADER
	);
	if ( qverify[1u] == 0 ){
		if ( (tag_hf[0u].size  != tag_hf[1u].size)
		    ||
		     (tag_hf[0u].nmemb != tag_hf[1u].nmemb)
		){
			retval = TAGCHECK_ERR_MISMATCHED;
		}
		off_begin -= (off_t) sizeof(struct ApeTag_TagHF);
	}

fn_end:
	if ( qverify[0u] != 0 ){
		; /* MAYBE */
	}
	if ( qverify[1u] != 0 ){
		; /* MAYBE */
	}

	items_size = byteswap_u32_letoh(tag_hf[0u].size);
	*fileinfo  = gatepa_fileinfo_make(
		items_size, byteswap_u32_letoh(tag_hf[0u].nmemb),
		off_begin, off_end, off_items
	);
	return retval;
}

/* returns 0 on success, or a mask of the errors */
PURE
static unsigned int
apetag_file_tag_hf_qverify(
	const struct ApeTag_TagHF *const tag_hf,
	const enum ApeTag_TagHFType tag_type
)
/*@*/
{
	const uint8_t preamble[8u] = APETAG_TAGHF_PREAMBLE;
	const uint8_t blank[8u]    = {0,0,0,0,0,0,0,0};
	/* * */
	unsigned int retval = 0;

	assert(sizeof tag_hf->preamble <= sizeof preamble);
	assert(sizeof tag_hf->pad_0    <= sizeof blank);
	assert(sizeof tag_hf->pad_1    <= sizeof blank);

	/* preamble */
	if ( memcmp(&tag_hf->preamble, preamble, sizeof preamble) != 0 ){
		retval |= APETAG_QVERIFY_PREAMBLE;
	}

	/* version */
	if ( byteswap_u32_letoh(tag_hf->version)
	    !=
	     GATEPA_APETAG_VERSION
	){
		retval |= APETAG_QVERIFY_VERSION;
	}

	/* size, nmemb */
	if ( byteswap_u32_letoh(tag_hf->size)
	    <
	     byteswap_u32_letoh(tag_hf->nmemb)
	){
		retval |= APETAG_QVERIFY_SIZE_NMEMB;
	}

	/* flags - is_readonly */
	if ( (tag_hf->is_readonly & ~APEFLAG_IS_READONLY) != 0 ){
		retval |= APETAG_QVERIFY_RESERVED_BITS;
	}

	/* flags - is_header */
	if ( tag_type
	    !=
	     (enum ApeTag_TagHFType) (tag_hf->is_headfoot & APEFLAG_IS_HEADER)
	){
		retval |= APETAG_QVERIFY_HF_TYPE;
	}

	/* flags - has_header/has_footer */
	if ( (tag_hf->is_headfoot & ~APEFLAG_IS_HEADFOOT_MASK) != 0 ){
		retval |= APETAG_QVERIFY_RESERVED_BITS;
	}

	/* padding / reserved bits */
	if ( (memcmp(tag_hf->pad_0, blank, sizeof tag_hf->pad_0) != 0)
	    ||
	     (memcmp(tag_hf->pad_1, blank, sizeof tag_hf->pad_1) != 0)
	){
		retval |= APETAG_QVERIFY_RESERVED_BITS;
	}

	return retval;
}

CONST
static enum TagCheckError
qverify_err(const unsigned int qverify)
/*@*/
{
	if ( (qverify & APETAG_QVERIFY_PREAMBLE)       != 0 ){
		return TAGCHECK_ERR_PREAMBLE;
	}
	if ( (qverify & APETAG_QVERIFY_VERSION)        != 0 ){
		return TAGCHECK_ERR_VERSION;
	}
	if ( (qverify & APETAG_QVERIFY_MALFORMED_MASK) != 0 ){
		return TAGCHECK_ERR_MALFORMED;
	}
	return 0;
}

/* EOF //////////////////////////////////////////////////////////////////// */
