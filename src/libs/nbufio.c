/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// nbufio.c                                                                 //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <assert.h>
#include <errno.h>
#include <stdint.h>

#include <unistd.h>

#include "nbufio.h"

/* //////////////////////////////////////////////////////////////////////// */

/* returns the number of bytes read on success (0 indicates EOF),
     or NBUFIO_RW_ERROR on error
*/
/*@unused@*/
size_t
nbufio_read(const nbufio_fd fd, /*@out@*/ void *const buf, const size_t count)
/*@globals	fileSystem,
		internalState
@*/
/*@modifies	internalState,
		*buf
@*/
{
	uint8_t *const buf_u8 = buf;
	/* * */
	size_t  size_read = 0;
	ssize_t result;

	while ( size_read < count ){
		result = read(
			(int) fd, &buf_u8[size_read], count - size_read
		);
		if ( result > 0 ){
			size_read += (size_t) result;
		}
		else if ( result == 0 ){
			if ( errno == EAGAIN ){
				continue;
			}
			break;	/* EOF */
		}
		else {	assert(result == (ssize_t) NBUFIO_RW_ERROR);
			return (size_t) result;
		}
	}
	assert(size_read <= count);
	return size_read;
}

/* returns the number of bytes written on success,
     or NBUFIO_RW_ERROR on error
*/
/*@unused@*/
size_t
nbufio_write(const nbufio_fd fd, const void *const buf, const size_t count)
/*@globals	fileSystem,
		internalState
@*/
/*@modifies	fileSystem,
		internalState
@*/
{
	const uint8_t *const buf_u8 = buf;
	/* * */
	size_t  size_writ = 0;
	ssize_t result;

	while ( size_writ < count ){
		result = write(
			(int) fd, &buf_u8[size_writ], count - size_writ
		);
		if ( result > 0 ){
			size_writ += (size_t) result;
		}
		else if ( result == 0 ){
			if ( errno == EAGAIN ){
				continue;
			}
			break;	/* EOF */
		}
		else {	assert(result == (ssize_t) NBUFIO_RW_ERROR);
			return (size_t) result;
		}
	}
	assert(size_writ <= count);
	return size_writ;
}

/* EOF //////////////////////////////////////////////////////////////////// */
