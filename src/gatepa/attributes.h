#ifndef GATEPA_ATTRIBUTES_H
#define GATEPA_ATTRIBUTES_H
/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// attributes.h                                                             //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#ifdef __GNUC__

#ifdef __has_attribute
#define HAS_ATTRIBUTE_GNUC(x)		__has_attribute(x)
#else
#define HAS_ATTRIBUTE_GNUC(x)		0
#endif

#define ATTRIBUTE_GNUC_ALWAYS_INLINE	always_inline
#define ATTRIBUTE_GNUC_NOINLINE		noinline

#define ATTRIBUTE_GNUC_CONST		const
#define ATTRIBUTE_GNUC_PURE		pure

#define ATTRIBUTE_GNUC_COLD		cold

#else	/* !defined(__GNUC__) */

#define HAS_ATTRIBUTE_GNUC(x)		0

#define ATTRIBUTE_GNUC_ALWAYS_INLINE	nil
#define ATTRIBUTE_GNUC_NOINLINE	nil

#define ATTRIBUTE_GNUC_CONST		nil
#define ATTRIBUTE_GNUC_PURE		nil

#define ATTRIBUTE_GNUC_COLD		nil

#endif	/* __GNUC__ */

/* ======================================================================== */

#ifdef __GNUC__

#ifdef __has_builtin
#define HAS_BUILTIN_GNUC(x)		__has_builtin(x)
#else
#define HAS_BUILTIN_GNUC(x)		0
#endif

#define BUILTIN_GNUC_EXPECT		__builtin_expect

#else	/* !defined(__GNUC__) */

#define HAS_BUILTIN_GNUC(x)		0

#define BUILTIN_GNUC_EXPECT		nil

#endif	/* __GNUC__ */

/* //////////////////////////////////////////////////////////////////////// */

#define GATEPA			/*@unused@*/ static
#define GATEPA_EXTERN		/*@external@*/ GATEPA

/* ======================================================================== */

#define INLINE			/*@unused@*/ static inline

#if HAS_ATTRIBUTE_GNUC(ATTRIBUTE_GNUC_ALWAYS_INLINE)
#define ALWAYS_INLINE		\
	INLINE __attribute__((ATTRIBUTE_GNUC_ALWAYS_INLINE))
#else
#define ALWAYS_INLINE		INLINE
#endif

#if HAS_ATTRIBUTE_GNUC(ATTRIBUTE_GNUC_NOINLINE)
#define NOINLINE		__attribute__((ATTRIBUTE_GNUC_NOINLINE))
#else
#define NOINLINE
#endif

#if HAS_ATTRIBUTE_GNUC(ATTRIBUTE_GNUC_CONST)
#define CONST			__attribute__((ATTRIBUTE_GNUC_CONST))
#else
#define CONST
#endif

#if HAS_ATTRIBUTE_GNUC(ATTRIBUTE_GNUC_PURE)
#define PURE			__attribute__((ATTRIBUTE_GNUC_PURE))
#else
#define PURE
#endif

#if HAS_ATTRIBUTE_GNUC(ATTRIBUTE_GNUC_COLD)
#define COLD			__attribute__((ATTRIBUTE_GNUC_COLD))
#else
#define COLD
#endif

/* //////////////////////////////////////////////////////////////////////// */

#if HAS_BUILTIN_GNUC(BUILTIN_GNUC_EXPECT)
#define UNLIKELY(x_cond)	(BUILTIN_GNUC_EXPECT((x_cond), (0 == 1)))
#else
#define UNLIKELY(x_cond)	(x_cond)
#endif

/* EOF //////////////////////////////////////////////////////////////////// */
#endif	/* GATEPA_ATTRIBUTES_H */
