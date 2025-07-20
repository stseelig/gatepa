#ifndef BYTESWAP_H
#define BYTESWAP_H
/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// byteswap.h - little to big endian etvv.                                  //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2023-2025, Shane Seelig                                    //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <stdint.h>

/* //////////////////////////////////////////////////////////////////////// */

#ifndef S_SPLINT_S

#ifndef __BYTE_ORDER__
#error "__BYTE_ORDER__"
#endif
#ifndef __ORDER_BIG_ENDIAN__
#error "__ORDER_BIG_ENDIAN__"
#endif
#ifndef __ORDER_LITTLE_ENDIAN__
#error "__ORDER_LITTLE_ENDIAN__"
#endif

#endif	/* S_SPLINT_S */

/* //////////////////////////////////////////////////////////////////////// */

#ifdef __GNUC__

#ifdef __has_attribute
#define X_BYTESWAP_HAS_ATTRIBUTE_GNUC(x_x)	__has_attribute(x_x)
#else
#define X_BYTESWAP_HAS_ATTRIBUTE_GNUC(x_x)	0
#endif

#define X_BYTESWAP_ATTRIBUTE_GNUC_ALWAYS_INLINE	always_inline
#define X_BYTESWAP_ATTRIBUTE_GNUC_CONST		const

#else	/* !defined(__GNUC__) */

#define X_BYTESWAP_HAS_ATTRIBUTE_GNUC(x_x)	0

#define X_BYTESWAP_ATTRIBUTE_GNUC_ALWAYS_INLINE	nil
#define X_BYTESWAP_ATTRIBUTE_GNUC_CONST		nil

#endif	/* __GNUC__ */

/* ======================================================================== */

#if __STDC_VERSION__ >= 199901L
#define X_BYTESWAP_INLINE		/*@unused@*/ static inline
#elif defined(__GNUC__)
#define X_BYTESWAP_INLINE		/*@unused@*/ static __inline__
#else
#define X_BYTESWAP_INLINE		/*@unused@*/ static
#endif	/* __STDC_VERSION__ */

#if X_BYTESWAP_HAS_ATTRIBUTE_GNUC(X_BYTESWAP_ATTRIBUTE_GNUC_ALWAYS_INLINE)
#define X_BYTESWAP_ALWAYS_INLINE	\
	X_BYTESWAP_INLINE \
	__attribute__((X_BYTESWAP_ATTRIBUTE_GNUC_ALWAYS_INLINE))
#else
#define X_BYTESWAP_ALWAYS_INLINE	X_BYTESWAP_INLINE
#endif

#if X_BYTESWAP_HAS_ATTRIBUTE_GNUC(X_BYTESWAP_ATTRIBUTE_GNUC_ALWAYS_INLINE)
#define X_BYTESWAP_CONST		\
	__attribute__((X_BYTESWAP_ATTRIBUTE_GNUC_CONST))
#else
#define X_BYTESWAP_CONST
#endif

/* //////////////////////////////////////////////////////////////////////// */

#ifdef __GNUC__

#ifdef __has_builtin
#define X_BYTESWAP_HAS_BUILTIN_GNUC(x_x)	__has_builtin(x_x)
#else
#define X_BYTESWAP_HAS_BUILTIN_GNUC(x_x)	0
#endif

#define X_BYTESWAP_BUILTIN_GNUC_BSWAP16		__builtin_bswap16
#define X_BYTESWAP_BUILTIN_GNUC_BSWAP32		__builtin_bswap32
#define X_BYTESWAP_BUILTIN_GNUC_BSWAP64		__builtin_bswap64

#else	/* !defined(__GNUC__) */

#define X_BYTESWAP_HAS_BUILTIN_GNUC(x_x)	0

#define X_BYTESWAP_BUILTIN_GNUC_BSWAP16		nil
#define X_BYTESWAP_BUILTIN_GNUC_BSWAP32		nil
#define X_BYTESWAP_BUILTIN_GNUC_BSWAP64		nil

#endif	/* __GNUC__ */

/* //////////////////////////////////////////////////////////////////////// */

typedef uint16_t	uint16_le;
typedef uint32_t	uint32_le;
typedef uint64_t	uint64_le;

typedef uint16_t	uint16_be;
typedef uint32_t	uint32_be;
typedef uint64_t	uint64_be;

/* //////////////////////////////////////////////////////////////////////// */

X_BYTESWAP_ALWAYS_INLINE X_BYTESWAP_CONST
uint16_t
byteswap_u16(const uint16_t x)
/*@*/
{
#if X_BYTESWAP_HAS_BUILTIN_GNUC(X_BYTESWAP_BUILTIN_GNUC_BSWAP16)
	return (uint16_t) X_BYTESWAP_BUILTIN_GNUC_BSWAP16(x);
#elif X_BYTESWAP_HAS_BUILTIN_GNUC(X_BYTESWAP_BUILTIN_GNUC_BSWAP32)
	return (uint16_t) X_BYTESWAP_BUILTIN_GNUC_BSWAP32(
		((uint32_t) x) << 16u
	);
#elif X_BYTESWAP_HAS_BUILTIN_GNUC(X_BYTESWAP_BUILTIN_GNUC_BSWAP64)
	return (uint16_t) X_BYTESWAP_BUILTIN_GNUC_BSWAP64(
		((uint64_t) x) << 48u
	);
#else
	uint16_t retval = 0;
	retval |= (x & 0xFF00u) >> 8u;
	retval |= (x & 0x00FFu) << 8u;
	return retval;
#endif
}

X_BYTESWAP_ALWAYS_INLINE X_BYTESWAP_CONST
uint32_t
byteswap_u32(const uint32_t x)
/*@*/
{
#if X_BYTESWAP_HAS_BUILTIN_GNUC(X_BYTESWAP_BUILTIN_GNUC_BSWAP32)
	return (uint32_t) X_BYTESWAP_BUILTIN_GNUC_BSWAP32(x);
#elif X_BYTESWAP_HAS_BUILTIN_GNUC(X_BYTESWAP_BUILTIN_GNUC_BSWAP64)
	return (uint32_t) X_BYTESWAP_BUILTIN_GNUC_BSWAP64(
		((uint64_t) x) << 32u
	);
#else
	uint32_t retval = 0;
	retval |= (x & 0xFF000000u) >> 24u;
	retval |= (x & 0x00FF0000u) >>  8u;
	retval |= (x & 0x0000FF00u) <<  8u;
	retval |= (x & 0x000000FFu) << 24u;
	return retval;
#endif
}

X_BYTESWAP_ALWAYS_INLINE X_BYTESWAP_CONST
uint64_t
byteswap_u64(const uint64_t x)
/*@*/
{
#if X_BYTESWAP_HAS_BUILTIN_GNUC(X_BYTESWAP_BUILTIN_GNUC_BSWAP64)
	return (uint64_t) X_BYTESWAP_BUILTIN_GNUC_BSWAP64(x);
#else
	uint64_t retval = 0;
	retval |= (x & 0xFF00000000000000u) >> 56u;
	retval |= (x & 0x00FF000000000000u) >> 40u;
	retval |= (x & 0x0000FF0000000000u) >> 24u;
	retval |= (x & 0x000000FF00000000u) >>  8u;
	retval |= (x & 0x00000000FF000000u) <<  8u;
	retval |= (x & 0x0000000000FF0000u) << 24u;
	retval |= (x & 0x000000000000FF00u) << 40u;
	retval |= (x & 0x00000000000000FFu) << 56u;
	return retval;
#endif
}

/* ======================================================================== */

X_BYTESWAP_ALWAYS_INLINE X_BYTESWAP_CONST
uint16_le
byteswap_u16_htole(const uint16_t x)
/*@*/
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	return (uint16_le) byteswap_u16(x);
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	return (uint16_le) x;
#else
#error "__BYTE_ORDER__"
#endif
}

X_BYTESWAP_ALWAYS_INLINE X_BYTESWAP_CONST
uint16_t
byteswap_u16_letoh(const uint16_le x)
/*@*/
{
	return byteswap_u16_htole((uint16_t) x);
}

X_BYTESWAP_ALWAYS_INLINE X_BYTESWAP_CONST
uint32_le
byteswap_u32_htole(const uint32_t x)
/*@*/
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	return (uint32_le) byteswap_u32(x);
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	return (uint32_le) x;
#else
#error "__BYTE_ORDER__"
#endif
}

X_BYTESWAP_ALWAYS_INLINE X_BYTESWAP_CONST
uint32_t
byteswap_u32_letoh(const uint32_le x)
/*@*/
{
	return byteswap_u32_htole((uint32_t) x);
}

X_BYTESWAP_ALWAYS_INLINE X_BYTESWAP_CONST
uint64_le
byteswap_u64_htole(const uint64_t x)
/*@*/
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	return (uint64_le) byteswap_u64(x);
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	return (uint64_le) x;
#else
#error "__BYTE_ORDER__"
#endif
}

X_BYTESWAP_ALWAYS_INLINE X_BYTESWAP_CONST
uint64_t
byteswap_u64_letoh(const uint64_le x)
/*@*/
{
	return byteswap_u64_htole((uint64_t) x);
}

/* ------------------------------------------------------------------------ */

X_BYTESWAP_ALWAYS_INLINE X_BYTESWAP_CONST
uint16_be
byteswap_u16_htobe(const uint16_t x)
/*@*/
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	return (uint16_be) byteswap_u16(x);
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	return (uint16_be) x;
#else
#error "__BYTE_ORDER__"
#endif
}

X_BYTESWAP_ALWAYS_INLINE X_BYTESWAP_CONST
uint16_t
byteswap_u16_betoh(const uint16_be x)
/*@*/
{
	return byteswap_u16_htobe((uint16_t) x);
}

X_BYTESWAP_ALWAYS_INLINE X_BYTESWAP_CONST
uint32_be
byteswap_u32_htobe(const uint32_t x)
/*@*/
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	return (uint32_be) byteswap_u32(x);
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	return (uint32_be) x;
#else
#error "__BYTE_ORDER__"
#endif
}

X_BYTESWAP_ALWAYS_INLINE X_BYTESWAP_CONST
uint32_t
byteswap_u32_betoh(const uint32_be x)
/*@*/
{
	return byteswap_u32_htobe((uint32_t) x);
}

X_BYTESWAP_ALWAYS_INLINE X_BYTESWAP_CONST
uint64_be
byteswap_u64_htobe(const uint64_t x)
/*@*/
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	return (uint64_be) byteswap_u64(x);
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	return (uint64_be) x;
#else
#error "__BYTE_ORDER__"
#endif
}

X_BYTESWAP_ALWAYS_INLINE X_BYTESWAP_CONST
uint64_t
byteswap_u64_betoh(const uint64_be x)
/*@*/
{
	return byteswap_u64_htobe((uint64_t) x);
}

/* EOF //////////////////////////////////////////////////////////////////// */
#endif	/* BYTESWAP_H */
