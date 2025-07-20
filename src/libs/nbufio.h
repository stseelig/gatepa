#ifndef NBUFIO_H
#define NBUFIO_H
/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// nbufio.h - unbuffered i/o                                                //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <assert.h>
#include <stdint.h>

#include <fcntl.h>
#include <unistd.h>

#include <sys/file.h>

/* //////////////////////////////////////////////////////////////////////// */

#ifdef __GNUC__

#ifdef __has_attribute
#define X_NBUFIO_HAS_ATTRIBUTE_GNUC(x)		__has_attribute(x)
#else
#define X_NBUFIO_HAS_ATTRIBUTE_GNUC(x)		0
#endif

#define X_NBUFIO_ATTRIBUTE_GNUC_ALWAYS_INLINE	always_inline

#else	/* !defined(__GNUC__) */

#define X_NBUFIO_HAS_ATTRIBUTE_GNUC(x)		0

#define X_NBUFIO_ATTRIBUTE_GNUC_ALWAYS_INLINE	nil

#endif	/* __GNUC__ */

/* ======================================================================== */

#if __STDC_VERSION__ >= 199901L
#define X_NBUFIO_INLINE			/*@unused@*/ static inline
#elif defined(__GNUC__)
#define X_NBUFIO_INLINE			/*@unused@*/ static __inline__
#else
#define X_NBUFIO_INLINE			/*@unused@*/ static
#endif	/* __STDC_VERSION__ */

#if X_NBUFIO_HAS_ATTRIBUTE_GNUC(X_NBUFIO_ATTRIBUTE_GNUC_ALWAYS_INLINE)
#define X_NBUFIO_ALWAYS_INLINE		\
	X_NBUFIO_INLINE __attribute__((X_NBUFIO_ATTRIBUTE_GNUC_ALWAYS_INLINE))
#else
#define X_NBUFIO_ALWAYS_INLINE		X_NBUFIO_INLINE
#endif

/* //////////////////////////////////////////////////////////////////////// */

typedef int			nbufio_fd;

/* //////////////////////////////////////////////////////////////////////// */

#define NBUFIO_FILENO_STDIN	((nbufio_fd) STDIN_FILENO)
#define NBUFIO_FILENO_STDOUT	((nbufio_fd) STDOUT_FILENO)
#define NBUFIO_FILENO_STDERR	((nbufio_fd) STDERR_FILENO)

/* ======================================================================== */

#define NBUFIO_FD_ERROR		((nbufio_fd) -1)
#define NBUFIO_OFF_ERROR	((off_t) -1)
#define NBUFIO_RW_ERROR		SIZE_MAX

/* //////////////////////////////////////////////////////////////////////// */

/*@-globuse@*/ /*@-mustmod@*/ /*@+longintegral@*/

/* ======================================================================== */

#undef buf
/*@external@*/ /*@unused@*/
extern size_t nbufio_read(nbufio_fd, /*@out@*/ void *buf, size_t)
/*@globals	fileSystem,
		internalState
@*/
/*@modifies	internalState,
		*buf
@*/
;

/*@external@*/ /*@unused@*/
extern size_t nbufio_write(nbufio_fd, const void *, size_t)
/*@globals	fileSystem,
		internalState
@*/
/*@modifies	fileSystem,
		internalState
@*/
;

/* //////////////////////////////////////////////////////////////////////// */

/* returns NBUFIO_FD_ERROR on error */
/* NOTE: does not handle EINTR for a blocked call */
X_NBUFIO_ALWAYS_INLINE
nbufio_fd
nbufio_open(const char *const pathname, const int flags)
/*@globals	fileSystem,
		internalState
@*/
/*@modifies	internalState@*/
{
	return open(pathname, flags);
}

/* returns 0 on success */
X_NBUFIO_ALWAYS_INLINE
int
nbufio_close(const nbufio_fd fd)
/*@globals	fileSystem,
		internalState
@*/
/*@modifies	internalState@*/
{
	return close((int) fd);
}

/* ======================================================================== */

/*@-redecl@*/
extern int flock(int, int)
/*@globals	fileSystem,
		internalState
@*/
/*@modifies	fileSystem,
		internalState
@*/
;
/*@=redecl@*/

/* returns 0 on success */
X_NBUFIO_ALWAYS_INLINE
int
nbufio_lock(const nbufio_fd fd, const int operation)
/*@globals	fileSystem,
		internalState
@*/
/*@modifies	fileSystem,
		internalState
@*/
{
	return flock((int) fd, operation);
}

/* ======================================================================== */

/* returns the offset of the file on success, or NBUFIO_OFF_ERROR on error */
X_NBUFIO_ALWAYS_INLINE
off_t
nbufio_tell(const nbufio_fd fd)
/*@globals	internalState@*/
/*@modifies	nothing@*/
{
	return lseek((int) fd, 0, SEEK_CUR);
}

/* returns the offset of the file on success, or NBUFIO_OFF_ERROR on error */
X_NBUFIO_ALWAYS_INLINE
off_t
nbufio_seek(const nbufio_fd fd, const off_t offset, const int whence)
/*@globals	internalState@*/
/*@modifies	internalState@*/
{
	return lseek((int) fd, offset, whence);
}

/* ======================================================================== */

/* compiler needs prototype (system header bug) */
/*@-redecl@*/ /*@-type@*/
extern int ftruncate(int, off_t)
/*@globals	fileSystem@*/
/*@modifies	fileSystem@*/
;
/*@=redecl@*/ /*@=type@*/

/* returns 0 on success */
X_NBUFIO_ALWAYS_INLINE
int
nbufio_truncate(const nbufio_fd fd, const off_t length)
/*@globals	fileSystem@*/
/*@modifies	fileSystem@*/
{
	return ftruncate((int) fd, length);
}

/* ======================================================================== */

/*@=globuse@*/ /*@=mustmod@*/ /*@=longintegral@*/

/* EOF //////////////////////////////////////////////////////////////////// */
#endif	/* NBUFIO_H */
