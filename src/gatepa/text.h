#ifndef GATEPA_TEXT_H
#define GATEPA_TEXT_H
/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// text.h                                                                   //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <stdint.h>
#include <stddef.h>

#include "attributes.h"

/* //////////////////////////////////////////////////////////////////////// */

CONST
GATEPA uint8_t ascii_toupper(uint8_t) /*@*/;

CONST
GATEPA uint8_t ascii_tolower(uint8_t) /*@*/;

NOINLINE PURE
GATEPA_EXTERN int ascii_isprintables(const uint8_t *, size_t) /*@*/;

NOINLINE PURE
GATEPA_EXTERN int ascii_casecmp(const uint8_t *, const uint8_t *, size_t)
/*@*/
;

NOINLINE PURE
GATEPA_EXTERN int utf8_verify(const uint8_t *, size_t) /*@*/;

/* EOF //////////////////////////////////////////////////////////////////// */
#endif	/* GATEPA_TEXT_H */
