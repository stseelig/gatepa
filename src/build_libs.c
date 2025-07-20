/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// build_libs.c                                                             //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#if __STDC_VERSION__ < 201112L
#error "__STDC_VERSION__ < 201112L"
#endif	/* __STDC_VERSION__ */

/* //////////////////////////////////////////////////////////////////////// */

#include "libs/chump/1-0_destroy.c"
#include "libs/chump/1-1_reset.c"
#include "libs/chump/2-0_alloc.c"

#include "libs/nbufio.c"

#include "libs/gstring/0-0-1_mod_fini.c"
#include "libs/gstring/0-1-0_ref_bstring.c"
#include "libs/gstring/0-2-0_copy_bstring.c"
#include "libs/gstring/2-0-0_cmp_bstring.c"
#include "libs/gstring/2-0-1_cmp_gstring.c"

#include "libs/gbitset/0-0_init.c"
#include "libs/bitset/0-1-0_set_range_0.c"
#include "libs/bitset/0-1-1_set_range_1.c"
#include "libs/bitset/2-1-1_find_1.c"
#include "libs/bitset/3-0-0_popcount.c"

/* EOF //////////////////////////////////////////////////////////////////// */
