/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// help.c                                                                   //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#include "../version.h"

#include "attributes.h"
#include "errors.h"
#include "mode.h"
#include "open.h"

/* //////////////////////////////////////////////////////////////////////// */

/*@unchecked@*/ /*@observer@*/
static const char f_str_help_license[] = {
/*12345670123456701234567012345670123456701234567012345670123456701234567012*/
"\n"
"    This program is free software released under the terms of the GNU\n"
" General Public License v3. It is distributed in the hope that it will be\n"
" useful, but WITHOUT ANY WARRANTY. See the license for details.\n"
};

/*@unchecked@*/ /*@observer@*/
static const char f_str_help_usage[] = {
/*12345670123456701234567012345670123456701234567012345670123456701234567012*/
"\n"
" Usage:"
"\n\t"  "gatepa [options] [files] -- [modes]"
"\n"
"\n\t"  "gatepa --help[=mode]"
"\n\n"
" Modes:"
"\n\t"  "add, add-file, add-loc, append, append-loc, auto-track, clear, dump,"
"\n"
"     extract, print, print-long, print-short, remove, rename, sort,"
"\n"
"     sort-alpha, sort-audio, tidy-keys, tidy-keys-1up, tidy-keys-lo,"
"\n"
"     tidy-keys-up, verify, write, write-long, write-short"
"\n\n"
};

/*@unchecked@*/ /*@observer@*/
static const char f_str_help_opts[] = {
/*12345670123456701234567012345670123456701234567012345670123456701234567012*/
" Options:"
"\n\t"  "--help[=mode]"
                "\t\t\t"                "Print this help, or a mode's help."
"\n\t"  "--limit-binary-fext"
                             "\t\t"     "(read) binary file-extension limit"
"\n\t"  "--limit-binary-name"
                             "\t\t"     "(read) binary name limit"
"\n\t"  "--softlimit-items-size"
                             "\t\t"     "(verify) tag items size softlimit"
"\n\t"  "--softlimit-key-size"
                             "\t\t"     "(verify) item key size softlimit"
"\n\n"
};

/* ======================================================================== */

/*@unchecked@*/ /*@observer@*/
static const char f_str_help_mode_add[] = {
/*12345670123456701234567012345670123456701234567012345670123456701234567012*/
"\n"
" Usage:"
"\n\t"  "add$[file-range]$key$value[$]"
"\n"
"\n\t"  "add-file$[file-range]$key$path[$]"
"\n"
"\n\t"  "add-loc$[file-range]$key$value[$]"
"\n\n"
" Brief:"
"\n\t"  "'add', 'add-file', and 'add-loc' repectively add/replace text,"
"\n"
"     binary, and locator items to/in tags."
"\n\n"
};

/*@unchecked@*/ /*@observer@*/
static const char f_str_help_mode_append[] = {
/*12345670123456701234567012345670123456701234567012345670123456701234567012*/
"\n"
" Usage:"
"\n\t"  "append$[file-range]$key$value[$]"
"\n"
"\n\t"  "append-loc$[file-range]$key$value[$]"
"\n\n"
" Brief:"
"\n\t"  "'append' and 'append-loc' repectively add/append text and locator"
"\n"
"     items to tags."
"\n\n"
};

/*@unchecked@*/ /*@observer@*/
static const char f_str_help_mode_autotrack[] = {
/*12345670123456701234567012345670123456701234567012345670123456701234567012*/
"\n"
" Usage:"
"\n\t"  "auto-track$[file-range][$]"
"\n\n"
" Brief:"
"\n\t"  "Adds an item with the key 'track' to each of the selected files in"
"\n"
"     the form of 'X/Y', where X is the 1-based index of the file, with"
"\n"
"     respect to the file-range, and Y is the total number of files in the"
"\n"
"     range."
"\n\n"
};

/*@unchecked@*/ /*@observer@*/
static const char f_str_help_mode_clear[] = {
/*12345670123456701234567012345670123456701234567012345670123456701234567012*/
"\n"
" Usage:"
"\n\t"  "clear$[file-range][$]"
"\n\n"
" Brief:"
"\n\t"  "Removes all items from tags."
"\n\n"
};

/*@unchecked@*/ /*@observer@*/
static const char f_str_help_mode_dump[] = {
/*12345670123456701234567012345670123456701234567012345670123456701234567012*/
"\n"
" Usage:"
"\n\t"  "dump$[file-range][$]"
"\n\n"
" Brief:"
"\n\t"  "Outputs the raw binary tag, as it currently is on disk, to stdout."
"\n"
"\n\t"  "Only one file may be dumped at a time."
"\n\n"
};

/*@unchecked@*/ /*@observer@*/
static const char f_str_help_mode_extract[] = {
/*12345670123456701234567012345670123456701234567012345670123456701234567012*/
"\n"
" Usage:"
"\n\t"  "extract$[file-range]$key[$]"
"\n\n"
" Brief:"
"\n\t"  "Outputs the item value to stdout. If a text item has multiple"
"\n"
"     values, they will be NUL seperated."
"\n"
"\n\t"  "Only one file/item may be extracted from at a time."
"\n\n"
};

/*@unchecked@*/ /*@observer@*/
static const char f_str_help_mode_print[] = {
/*12345670123456701234567012345670123456701234567012345670123456701234567012*/
"\n"
" Usage:"
"\n\t"  "print$[file-range][$]"
"\n"
"\n\t"  "print-long$[file-range][$]"
"\n"
"\n\t"  "print-short$[file-range][$]"
"\n\n"
" Brief:"
"\n\t"  "Print the formatted contents of tags."
"\n"
"\n\t"  "'print' is the same as 'print-short'. The main difference between"
"\n"
"     '-short' and '-long' is that '-long' will not abbreviate keys nor text."
"\n\n"
};

/*@unchecked@*/ /*@observer@*/
static const char f_str_help_mode_remove[] = {
/*12345670123456701234567012345670123456701234567012345670123456701234567012*/
"\n"
" Usage:"
"\n\t"  "remove$[file-range]$key[$]"
"\n\n"
" Brief:"
"\n\t"  "Remove an item from tags."
"\n\n"
};

/*@unchecked@*/ /*@observer@*/
static const char f_str_help_mode_rename[] = {
/*12345670123456701234567012345670123456701234567012345670123456701234567012*/
"\n"
" Usage:"
"\n\t"  "remove$[file-range]$old-key$new-key[$]"
"\n\n"
" Brief:"
"\n\t"  "Rename an item key in tags."
"\n\n"
};

/*@unchecked@*/ /*@observer@*/
static const char f_str_help_mode_sort[] = {
/*12345670123456701234567012345670123456701234567012345670123456701234567012*/
"\n"
" Usage:"
"\n\t"  "sort$[file-range][$]"
"\n"
"\n\t"  "sort-alpha$[file-range][$]"
"\n"
"\n\t"  "sort-audio$[file-range][$]"
"\n\n"
" Brief:"
"\n\t"  "Sort the items within tags."
"\n"
"\n\t"  "'sort' is the same as 'sort-audio'. '-alpha' will sort tags"
"\n"
"     alphabetically, while '-audio' will sort common fields in audio files"
"\n"
"     to the beginning. The item types are also prioritized diffently."
"\n\n"
};

/*@unchecked@*/ /*@observer@*/
static const char f_str_help_mode_tidykeys[] = {
/*12345670123456701234567012345670123456701234567012345670123456701234567012*/
"\n"
" Usage:"
"\n\t"  "tidy-keys$[file-range][$]"
"\n"
"\n\t"  "tidy-keys-1up$[file-range][$]"
"\n"
"\n\t"  "tidy-keys-lo$[file-range][$]"
"\n"
"\n\t"  "tidy-keys-up$[file-range][$]"
"\n\n"
" Brief:"
"\n\t"  "Convert item keys to a consistent case style."
"\n"
"\n\t"  "'tidy-keys' is the same as 'tidy-keys-lo'."
"\n"
"\n\t"  "Example:"
"\n\t\t"         "1up: 'Replaygain_Track_Gain'"
"\n\t\t"         " lo: 'replaygain_track_gain'"
"\n\t\t"         " up: 'REPLAYGAIN_TRACK_GAIN'"
"\n\n"
};

/*@unchecked@*/ /*@observer@*/
static const char f_str_help_mode_verify[] = {
/*12345670123456701234567012345670123456701234567012345670123456701234567012*/
"\n"
" Usage:"
"\n\t"  "verify$[file-range][$]"
"\n\n"
" Brief:"
"\n\t"  "Verify that tags are within some hard and soft-limits. The program"
"\n"
"     will exit with an error if any tags are not within all of the checked"
"\n"
"     limits. (Such as tag size, key size, repeated keys, zero-length items.)"
"\n"
"\n\t"  "Some of the program options change the default soft-limits."
"\n\n"
};

/*@unchecked@*/ /*@observer@*/
static const char f_str_help_mode_write[] = {
/*12345670123456701234567012345670123456701234567012345670123456701234567012*/
"\n"
" Usage:"
"\n\t"  "write$[file-range][$]"
"\n"
"\n\t"  "write-long$[file-range][$]"
"\n"
"\n\t"  "write-short$[file-range][$]"
"\n\n"
" Brief:"
"\n\t"  "Write tags to files."
"\n"
"\n\t"  "'write' is the same as 'write-long'. '-short' will not include the"
"\n"
"     tag header (32 bytes) in the written tag."
"\n\n"
};

/* ------------------------------------------------------------------------ */

static const char *f_help_mode_array[GATEPA_NUM_MODES] = {
	f_str_help_mode_add,		/* add           */
	f_str_help_mode_add,		/* add-file      */
	f_str_help_mode_add,		/* add-loc       */
	f_str_help_mode_append,		/* append        */
	f_str_help_mode_append,		/* append-loc    */
	f_str_help_mode_autotrack,	/* auto-track    */
	f_str_help_mode_clear,		/* clear         */
	f_str_help_mode_dump,		/* dump          */
	f_str_help_mode_extract,	/* extract       */
	f_str_help_mode_print,		/* print         */
	f_str_help_mode_print,		/* print-long    */
	f_str_help_mode_print,		/* print-short   */
	f_str_help_mode_remove,		/* remove        */
	f_str_help_mode_rename,		/* rename        */
	f_str_help_mode_sort,		/* sort          */
	f_str_help_mode_sort,		/* sort-alpha    */
	f_str_help_mode_sort,		/* sort-audio    */
	f_str_help_mode_tidykeys,	/* tidy-keys     */
	f_str_help_mode_tidykeys,	/* tidy-keys-1up */
	f_str_help_mode_tidykeys,	/* tidy-keys-lo  */
	f_str_help_mode_tidykeys,	/* tidy-keys-up  */
	f_str_help_mode_verify,		/* verify        */
	f_str_help_mode_write,		/* write         */
	f_str_help_mode_write,		/* write-long    */
	f_str_help_mode_write		/* write-short   */
};

/* //////////////////////////////////////////////////////////////////////// */

COLD
GATEPA void
help_version(FILE *const file)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		file
@*/
{
	(void) fprintf(file, "\n gatepa %u.%u.%u",
		GATEPA_VERSION_NUM, GATEPA_VERSION_NUM_MAJOR,
		GATEPA_VERSION_NUM_MINOR
	);
	if ( GATEPA_VERSION_NUM_REVIS != 0 ){
		(void) fprintf(file, "-%u", GATEPA_VERSION_NUM_REVIS);
	}
	if ( GATEPA_VERSION_STR_EXTRA[0] != '\0' ){
		(void) fprintf(file, "~%s", GATEPA_VERSION_STR_EXTRA);
	}
	(void) fprintf(file, " (%s)\n" "\t%s\n",
		GATEPA_VERSION_STR_DATE, GATEPA_COPYRIGHT_STR
	);
	return;
}

COLD
GATEPA void
help_license(FILE *const file)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		file
@*/
{
	(void) fputs(f_str_help_license, file);
	return;
}

COLD
GATEPA void
help_usage(FILE *const file)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		file
@*/
{
	(void) fputs(f_str_help_usage, file);
	return;
}

COLD
GATEPA void
help_opts(FILE *const file)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		file
@*/
{
	(void) fputs(f_str_help_opts, file);
	return;
}

COLD
GATEPA void
help_mode(FILE *const file, const int mode_idx)
/*@globals	fileSystem@*/
/*@modifies	fileSystem,
		file
@*/
{
	assert(mode_idx >= 0);

	(void) fputs(f_help_mode_array[mode_idx], file);
	return;
}

/* EOF //////////////////////////////////////////////////////////////////// */
