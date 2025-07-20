#ifndef APETAG_H
#define APETAG_H
/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// apetag.h - ApeTag v2.0 metadata format                                   //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <libs/ascii-literals.h>
#include <libs/byteswap.h>
#include <libs/gstring.h>
#include <libs/nbufio.h>

#include "attributes.h"
#include "errors.h"

/* //////////////////////////////////////////////////////////////////////// */

struct ApeTag_Globals {
	uint32_t	items_size_softlimit;
	uint32_t	key_size_softlimit;

	uint32_t	binary_name_limit;
	uint32_t	binary_fext_limit;
};

/*@checkmod@*/ /*@unused@*/
extern struct ApeTag_Globals g_apetag;

/* //////////////////////////////////////////////////////////////////////// */

#define GATEPA_APETAG_VERSION		UINT32_C(2000)

#define GATEPA_FEXT_FALLBACK	{ \
	ASCII_PERIOD, ASCII_B_LO, ASCII_I_LO, ASCII_N_LO \
}

/* //////////////////////////////////////////////////////////////////////// */

/* file structs */

#define APEFLAG_NO_READONLY		  0x00u
#define APEFLAG_IS_READONLY		  0x01u
enum ApeFlag_ItemType {
	APEFLAG_ITEMTYPE_TEXT		= 0x00u,
	APEFLAG_ITEMTYPE_BINARY		= 0x02u,
	APEFLAG_ITEMTYPE_LOCATOR	= 0x04u,
	APEFLAG_ITEMTYPE_UNKNOWN	= 0x06u
};
#define APEFLAG_ITEMTYPE_MASK		  0x06u

#define APEFLAG_IS_HEADER		  0x20u
#define APEFLAG_IS_FOOTER		  0x00u
#define APEFLAG_HAS_FOOTER		  0x40u	/* not reliable in the wild */
#define APEFLAG_HAS_HEADER		  0x80u	/* ~                        */
#define APEFLAG_HAS_HF_MASK		  0xC0u
#define APEFLAG_IS_HEADFOOT_MASK	  0xE0u

#define APETAG_TAGHF_PREAMBLE		 { \
	ASCII_A_UP, ASCII_P_UP, ASCII_E_UP, ASCII_T_UP, \
	ASCII_A_UP, ASCII_G_UP, ASCII_E_UP, ASCII_X_UP  \
}

/* 32u bytes */
struct ApeTag_TagHF {
	uint8_t		preamble[8u];	/* .ascii "APETAGEX"                */
	uint32_le	version;	/* 2000u for version 2.0            */
	uint32_le	size;		/* size of the items-blob + footer  */
	uint32_le	nmemb;		/* number of items in the tag       */
	uint8_t		is_readonly;	/* whether the tag is readonly      */
	uint8_t		pad_0[2u];	/*                                  */
	uint8_t		is_headfoot;	/* tag is/has header and/or footer  */
	uint8_t		pad_1[8u];	/*                                  */
};

/*  8u bytes */
struct ApeTag_ItemH {
	uint32_le	size;		/* total length of item value       */
	uint8_t		type;		/* itemtype and is_readonly flag    */
	uint8_t		pad_0[3u];	/*                                  */
};

/* ------------------------------------------------------------------------ */

#define APETAG_ITEM_TYPE(x_type_byte)	((enum ApeFlag_ItemType) (\
	(x_type_byte) & APEFLAG_ITEMTYPE_MASK \
))

CONST
ALWAYS_INLINE struct ApeTag_TagHF
apetag_taghf_make(
	const uint32_t size, const uint32_t nmemb,
	unsigned int is_readonly, unsigned int is_headfoot
)
/*@*/
{
	return (struct ApeTag_TagHF) {
		APETAG_TAGHF_PREAMBLE,
		byteswap_u32_htole(GATEPA_APETAG_VERSION),
		byteswap_u32_htole(size),
		byteswap_u32_htole(nmemb),
		(uint8_t) is_readonly,
		{0,0},
		(uint8_t) is_headfoot,
		{0,0,0,0,0,0,0,0}
	};
}

CONST
ALWAYS_INLINE struct ApeTag_ItemH
apetag_itemh_make(const uint32_t size, const enum ApeFlag_ItemType type)
/*@*/
{
	return (struct ApeTag_ItemH) {
		byteswap_u32_htole(size), (uint8_t) type, {0,0,0}
	};
}

/* ======================================================================== */

/* memory structs */

/* BINARY type items:
	- first value is the file extension (ex. ".jpg")
	- second is the binary data
*/

union Gatepa_Value {
	struct GString	single;
	/*@temp@*/
	struct GString	*multi;
};

struct Gatepa_Item {
	union Gatepa_Value	value;
	uint32_t		nmemb;
	enum ApeFlag_ItemType	type;
};

struct Gatepa_Tag {
	/*@temp@*/ /*@relnull@*/ /*@reldef@*/
	struct GString		*key;
	/*@temp@*/ /*@relnull@*/ /*@reldef@*/
	struct Gatepa_Item	*item;
	uint32_t		nmemb;
	uint32_t		nmemb_max;
	/* size is calculated/checked before writing */
};

struct Gatepa_FileInfo {
	uint32_t		items_size;
	uint32_t		items_nmemb;
	off_t			off_begin;
	off_t			off_end;
	off_t			off_items;
};

/* ------------------------------------------------------------------------ */

#define GATEPA_MEMTAG_INIT		(struct Gatepa_Tag) { \
	NULL, NULL, 0, 0 \
}

CONST
ALWAYS_INLINE struct Gatepa_Item
gatepa_memitem_init(const enum ApeFlag_ItemType type)
/*@*/
{
	struct Gatepa_Item item = {
		.value.single	= GSTRING_INIT_NULL,
		.nmemb		= 0,
		.type		= type
	};
	return item;
}

CONST
ALWAYS_INLINE struct Gatepa_FileInfo
gatepa_fileinfo_make(
	const uint32_t items_size, const uint32_t items_nmemb,
	const off_t off_begin, const off_t off_end, const off_t off_items
)
/*@*/
{
	return (struct Gatepa_FileInfo) {
		items_size, items_nmemb, off_begin, off_end, off_items
	};
}

/* //////////////////////////////////////////////////////////////////////// */

enum Print_Type {
	PRINTTYPE_SHORT,
	PRINTTYPE_LONG
};

enum Sort_TagCompar {
	TAGCOMPAR_AUDIO,
	TAGCOMPAR_ALPHA
};

enum Write_TagType {
	TAGTYPE_LONG,
	TAGTYPE_SHORT
};

/* //////////////////////////////////////////////////////////////////////// */

#undef fileinfo
#undef file
GATEPA_EXTERN enum TagCheckError apetag_file_tag_check_eof(
	/*@out@*/ struct Gatepa_FileInfo *fileinfo, nbufio_fd file
)
/*@globals	fileSystem,
		internalState
@*/
/*@modifies	internalState,
		*fileinfo
@*/
;

#undef tag_out
#undef file
GATEPA_EXTERN enum SlurpError apetag_slurp_tag(
	/*@out@*/ struct Gatepa_Tag *tag_out,
	const struct Gatepa_FileInfo *, nbufio_fd file
)
/*@globals	fileSystem,
		internalState
@*/
/*@modifies	internalState,
		*tag_out
@*/
;

/* ======================================================================== */

#undef size_out
GATEPA_EXTERN enum GatepaErr apetag_size_items(
	/*@out@*/ uint32_t *size_out, const struct Gatepa_Tag *
)
/*@modifies	*size_out@*/
;

GATEPA_EXTERN enum GatepaErr apetag_write_tag_header(
	nbufio_fd, uint32_t, uint32_t
)
/*@globals	fileSystem,
		internalState
@*/
/*@modifies	fileSystem,
		internalState
@*/
;

#undef buf
GATEPA_EXTERN size_t apetag_construct_tag(
	/*@out@*/ uint8_t *buf, size_t, const struct Gatepa_Tag *,
	enum Write_TagType
)
/*@modifies	*buf@*/
;

/* ======================================================================== */

NOINLINE PURE
GATEPA_EXTERN uint32_t apetag_memtag_find_item(
	const struct Gatepa_Tag *, const struct GString *
)
/*@*/
;

#undef tag
NOINLINE
GATEPA_EXTERN enum GatepaErr apetag_memtag_add_item(
	struct Gatepa_Tag *tag, const struct GString *,
	const struct Gatepa_Item *
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*tag
@*/
;

#undef tag
GATEPA_EXTERN void apetag_memtag_rename_item(
	struct Gatepa_Tag *tag, uint32_t, const struct GString *
)
/*@modifies	tag->key[]@*/
;

#undef tag
GATEPA_EXTERN void apetag_memtag_replace_item(
	struct Gatepa_Tag *tag, uint32_t, const struct Gatepa_Item *
)
/*@modifies	tag->item[]@*/
;

#undef tag
GATEPA_EXTERN void apetag_memtag_clear(struct Gatepa_Tag *tag)
/*@modifies	*tag@*/
;

#undef tag
NOINLINE
GATEPA_EXTERN enum GatepaErr apetag_memtag_remove_item(
	struct Gatepa_Tag *tag, uint32_t
)
/*@modifies	*tag@*/
;

/* ------------------------------------------------------------------------ */

#undef item
NOINLINE
GATEPA_EXTERN enum GatepaErr apetag_memitem_add_value(
	struct Gatepa_Item *item, const struct GString *
)
/*@globals	internalState@*/
/*@modifies	internalState,
		*item
@*/
;

#undef item
GATEPA_EXTERN void
apetag_memitem_replace_value(
	struct Gatepa_Item *item, const struct GString *,
	enum ApeFlag_ItemType
)
/*@modifies	*item@*/
;

/* ======================================================================== */

#undef file
GATEPA_EXTERN void gatepa_print_short(FILE *file, const struct Gatepa_Tag *)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		*file
@*/
;

#undef file
GATEPA_EXTERN void gatepa_print_long(FILE *file, const struct Gatepa_Tag *)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		*file
@*/
;

/* ======================================================================== */

PURE
GATEPA_EXTERN int apetag_compar_audio(const void *, const void *, void *)
/*@*/
;

PURE
GATEPA_EXTERN int apetag_compar_alpha(const void *, const void *, void *)
/*@*/
;

/* EOF //////////////////////////////////////////////////////////////////// */
#endif	/* APETAG_H */
