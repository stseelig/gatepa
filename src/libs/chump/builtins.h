#ifndef CHUMP_BUILTINS_H
#define CHUMP_BUILTINS_H
/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// chump/builtins.h                                                         //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2025, Shane Seelig                                         //
// SPDX-License-Identifier: GPL-3.0-or-later                                //
//                                                                          //
/////////////////////////////////////////////////////////////////////////// */

#include <limits.h>
#include <stdint.h>

/* //////////////////////////////////////////////////////////////////////// */

#ifdef __GNUC__

#ifdef __has_attribute
#define X_CHUMP_HAS_ATTRIBUTE_GNUC(x)		__has_attribute(x)
#else
#define X_CHUMP_HAS_ATTRIBUTE_GNUC(x)		0
#endif

#define X_CHUMP_ATTRIBUTE_GNUC_ALWAYS_INLINE	always_inline
#define X_CHUMP_ATTRIBUTE_GNUC_CONST		const

#else	/* !defined(__GNUC__) */

#define X_CHUMP_HAS_ATTRIBUTE_GNUC(x)		0

#define X_CHUMP_ATTRIBUTE_GNUC_ALWAYS_INLINE	nil
#define X_CHUMP_ATTRIBUTE_GNUC_CONST		nil

#endif	/* __GNUC__ */

/* ======================================================================== */

#if __STDC_VERSION__ >= 199901L
#define INLINE			/*@unused@*/ static inline
#elif defined(__GNUC__)
#define INLINE			/*@unused@*/ static __inline__
#else
#define INLINE			/*@unused@*/ static
#endif	/* __STDC_VERSION__ */

#if X_CHUMP_HAS_ATTRIBUTE_GNUC(X_CHUMP_ATTRIBUTE_GNUC_ALWAYS_INLINE)
#define ALWAYS_INLINE		\
	INLINE __attribute__((X_CHUMP_ATTRIBUTE_GNUC_ALWAYS_INLINE))
#else
#define ALWAYS_INLINE		INLINE
#endif

#if X_CHUMP_HAS_ATTRIBUTE_GNUC(X_CHUMP_ATTRIBUTE_GNUC_ALWAYS_INLINE)
#define CONST			__attribute__((X_CHUMP_ATTRIBUTE_GNUC_CONST))
#else
#define CONST
#endif

/* //////////////////////////////////////////////////////////////////////// */

#ifdef __GNUC__

#ifdef __has_builtin
#define X_CHUMP_HAS_BUILTIN_GNUC(x)		__has_builtin(x)
#else
#define X_CHUMP_HAS_BUILTIN_GNUC(x)		0
#endif

#define X_CHUMP_BUILTIN_GNUC_UADD_OVERFLOW	__builtin_uadd_overflow
#define X_CHUMP_BUILTIN_GNUC_UADDL_OVERFLOW	__builtin_uaddl_overflow
#define X_CHUMP_BUILTIN_GNUC_UADDLL_OVERFLOW	__builtin_uaddll_overflow

#define X_CHUMP_BUILTIN_GNUC_UMUL_OVERFLOW	__builtin_umul_overflow
#define X_CHUMP_BUILTIN_GNUC_UMULL_OVERFLOW	__builtin_umull_overflow
#define X_CHUMP_BUILTIN_GNUC_UMULLL_OVERFLOW	__builtin_umulll_overflow

#define X_CHUMP_BUILTIN_GNUC_POPCOUNT		__builtin_popcount
#define X_CHUMP_BUILTIN_GNUC_POPCOUNTL		__builtin_popcountl
#define X_CHUMP_BUILTIN_GNUC_POPCOUNTLL		__builtin_popcountll

#else	/* !defined(__GNUC__) */

#define X_CHUMP_HAS_BUILTIN_GNUC(x)		0

#define X_CHUMP_BUILTIN_GNUC_UADD_OVERFLOW	nil
#define X_CHUMP_BUILTIN_GNUC_UADDL_OVERFLOW	nil
#define X_CHUMP_BUILTIN_GNUC_UADDLL_OVERFLOW	nil

#define X_CHUMP_BUILTIN_GNUC_UMUL_OVERFLOW	nil
#define X_CHUMP_BUILTIN_GNUC_UMULL_OVERFLOW	nil
#define X_CHUMP_BUILTIN_GNUC_UMULLL_OVERFLOW	nil

#define X_CHUMP_BUILTIN_GNUC_POPCOUNT		nil
#define X_CHUMP_BUILTIN_GNUC_POPCOUNTL	nil
#define X_CHUMP_BUILTIN_GNUC_POPCOUNTLL	nil

#endif	/* __GNUC__ */

/* //////////////////////////////////////////////////////////////////////// */

/*@-mustdefine@*/	/* splint bug */

/* ======================================================================== */

#if X_CHUMP_HAS_BUILTIN_GNUC(X_CHUMP_BUILTIN_GNUC_UADD_OVERFLOW)
#define X_CHUMP_ADD_UINT_OVERFLOW(x_res_ptr, x_a, x_b)	( \
	(int) X_CHUMP_BUILTIN_GNUC_UADD_OVERFLOW((x_a), (x_b), (x_res_ptr)) \
)
#else
ALWAYS_INLINE int
X_CHUMP_ADD_UINT_OVERFLOW(
	/*@out@*/ unsigned int *const res,
	const unsigned int a, const unsigned int b
)
/*@modifies	*res@*/
{
	*res = a + b;
	return (int) (*res < a);
}
#endif
ALWAYS_INLINE int
add_uint_overflow(
	/*@out@*/ unsigned int *const res,
	const unsigned int a, const unsigned int b
)
/*@modifies	*res@*/
{
	return X_CHUMP_ADD_UINT_OVERFLOW(res, a, b);
}

#if X_CHUMP_HAS_BUILTIN_GNUC(X_CHUMP_BUILTIN_GNUC_UADDL_OVERFLOW)
#define X_CHUMP_ADD_ULONG_OVERFLOW(x_res_ptr, x_a, x_b)	( \
	(int) X_CHUMP_BUILTIN_GNUC_UADDL_OVERFLOW((x_a), (x_b), (x_res_ptr)) \
)
#else
ALWAYS_INLINE int
X_CHUMP_ADD_ULONG_OVERFLOW(
	/*@out@*/ unsigned long *const res,
	const unsigned long a, const unsigned long b
)
/*@modifies	*res@*/
{
	*res = a + b;
	return (int) (*res < a);
}
#endif
ALWAYS_INLINE int
add_ulong_overflow(
	/*@out@*/ unsigned long *const res,
	const unsigned long a, const unsigned long b
)
/*@modifies	*res@*/
{
	return X_CHUMP_ADD_ULONG_OVERFLOW(res, a, b);
}

#if X_CHUMP_HAS_BUILTIN_GNUC(X_CHUMP_BUILTIN_GNUC_UADDLL_OVERFLOW)
#define X_CHUMP_ADD_ULONGLONG_OVERFLOW(x_res_ptr, x_a, x_b)	( \
	(int) X_CHUMP_BUILTIN_GNUC_UADDLL_OVERFLOW((x_a), (x_b), (x_res_ptr)) \
)
#else
ALWAYS_INLINE int
X_CHUMP_ADD_ULONGLONG_OVERFLOW(
	/*@out@*/ unsigned long long *const res,
	const unsigned long long a, const unsigned long long b
)
/*@modifies	*res@*/
{
	*res = a + b;
	return (int) (*res < a);
}
#endif
ALWAYS_INLINE int
add_ulonglong_overflow(
	/*@out@*/ unsigned long long *const res,
	const unsigned long long a, const unsigned long long b
)
/*@modifies	*res@*/
{
	return X_CHUMP_ADD_ULONGLONG_OVERFLOW(res, a, b);
}

/* ------------------------------------------------------------------------ */

#if   UINT32_MAX == UINT_MAX
ALWAYS_INLINE int
add_u32_overflow(
	/*@out@*/ uint32_t *const res, const uint32_t a, const uint32_t b
)
/*@modifies	*res@*/
{
	return add_uint_overflow(
		(unsigned int *) res, (unsigned int) a, (unsigned int) b
	);
}
#elif UINT32_MAX == ULONG_MAX
ALWAYS_INLINE int
add_u32_overflow(
	/*@out@*/ uint32_t *const res, const uint32_t a, const uint32_t b
)
/*@modifies	*res@*/
{
	return add_ulong_overflow(
		(unsigned long *) res, (unsigned long) a, (unsigned long) b
	);
}
#elif UINT32_MAX == ULONGLONG_MAX
ALWAYS_INLINE int
add_u32_overflow(
	/*@out@*/ uint32_t *const res, const uint32_t a, const uint32_t b
)
/*@modifies	*res@*/
{
	return add_ulonglong_overflow(
		(unsigned long long *) res,
		(unsigned long long) a,
		(unsigned long long) b
	);
}
#else
#error "add_u32_overflow"
#endif

/* ======================================================================== */

#if X_CHUMP_HAS_BUILTIN_GNUC(X_CHUMP_BUILTIN_GNUC_UMUL_OVERFLOW)
#define X_CHUMP_MUL_UINT_OVERFLOW(x_res_ptr, x_a, x_b)	( \
	(int) X_CHUMP_BUILTIN_GNUC_UMUL_OVERFLOW((x_a), (x_b), (x_res_ptr)) \
)
#else
ALWAYS_INLINE int
X_CHUMP_MUL_UINT_OVERFLOW(
	/*@out@*/ unsigned int *const res,
	const unsigned int a, const unsigned int b
)
/*@modifies	*res@*/
{
	*res = a * b;
	return (int) (((a != 0) && (*res / a == b)) ? 0 : 1);
}
#endif
ALWAYS_INLINE int
mul_uint_overflow(
	/*@out@*/ unsigned int *const res,
	const unsigned int a, const unsigned int b
)
/*@modifies	*res@*/
{
	return X_CHUMP_MUL_UINT_OVERFLOW(res, a, b);
}

#if X_CHUMP_HAS_BUILTIN_GNUC(X_CHUMP_BUILTIN_GNUC_UMULL_OVERFLOW)
#define X_CHUMP_MUL_ULONG_OVERFLOW(x_res_ptr, x_a, x_b)	( \
	(int) X_CHUMP_BUILTIN_GNUC_UMULL_OVERFLOW((x_a), (x_b), (x_res_ptr)) \
)
#else
ALWAYS_INLINE int
X_CHUMP_MUL_ULONG_OVERFLOW(
	/*@out@*/ unsigned long *const res,
	const unsigned long a, const unsigned long b
)
/*@modifies	*res@*/
{
	*res = a * b;
	return (int) (((a != 0) && (*res / a == b)) ? 0 : 1);
}
#endif
ALWAYS_INLINE int
mul_ulong_overflow(
	/*@out@*/ unsigned long *const res,
	const unsigned long a, const unsigned long b
)
/*@modifies	*res@*/
{
	return X_CHUMP_MUL_ULONG_OVERFLOW(res, a, b);
}

#if X_CHUMP_HAS_BUILTIN_GNUC(X_CHUMP_BUILTIN_GNUC_UMULLL_OVERFLOW)
#define X_CHUMP_MUL_ULONGLONG_OVERFLOW(x_res_ptr, x_a, x_b)	( \
	(int) X_CHUMP_BUILTIN_GNUC_UMULLL_OVERFLOW((x_a), (x_b), (x_res_ptr)) \
)
#else
ALWAYS_INLINE int
X_CHUMP_MUL_ULONGLONG_OVERFLOW(
	/*@out@*/ unsigned long long *const res,
	const unsigned long long a, const unsigned long long b
)
/*@modifies	*res@*/
{
	*res = a * b;
	return (int) (((a != 0) && (*res / a == b)) ? 0 : 1);
}
#endif
ALWAYS_INLINE int
mul_ulonglong_overflow(
	/*@out@*/ unsigned long long *const res,
	const unsigned long long a, const unsigned long long b
)
/*@modifies	*res@*/
{
	return X_CHUMP_MUL_ULONGLONG_OVERFLOW(res, a, b);
}

/* ------------------------------------------------------------------------ */

#if   UINT32_MAX == UINT_MAX
ALWAYS_INLINE int
mul_u32_overflow(
	/*@out@*/ uint32_t *const res, const uint32_t a, const uint32_t b
)
/*@modifies	*res@*/
{
	return mul_uint_overflow(
		(unsigned int *) res, (unsigned int) a, (unsigned int) b
	);
}
#elif UINT32_MAX == ULONG_MAX
ALWAYS_INLINE int
mul_u32_overflow(
	/*@out@*/ uint32_t *const res, const uint32_t a, const uint32_t b
)
/*@modifies	*res@*/
{
	return mul_ulong_overflow(
		(unsigned long *) res, (unsigned long) a, (unsigned long) b
	);
}
#elif UINT32_MAX == ULONGLONG_MAX
ALWAYS_INLINE int
mul_u32_overflow(
	/*@out@*/ uint32_t *const res, const uint32_t a, const uint32_t b
)
/*@modifies	*res@*/
{
	return mul_ulonglong_overflow(
		(unsigned long long *) res,
		(unsigned long long) a,
		(unsigned long long) b
	);
}
#else
#error "mul_u32_overflow"
#endif

/* ======================================================================== */

/*@=mustdefine@*/

/* ======================================================================== */

#if   UINT32_MAX == UINT_MAX
#if X_CHUMP_HAS_BUILTIN_GNUC(X_CHUMP_BUILTIN_GNUC_POPCOUNT)
#define X_CHUMP_POPCOUNT_U32(x_x)	X_CHUMP_BUILTIN_GNUC_POPCOUNT(x_x)
#endif
#elif UINT32_MAX == ULONG_MAX
#if X_CHUMP_HAS_BUILTIN_GNUC(X_CHUMP_BUILTIN_GNUC_POPCOUNTL)
#define X_CHUMP_POPCOUNT_U32(x_x)	X_CHUMP_BUILTIN_GNUC_POPCOUNTL(x_x)
#endif
#elif UINT32_MAX == ULONGLONG_MAX
#if X_CHUMP_HAS_BUILTIN_GNUC(X_CHUMP_BUILTIN_GNUC_POPCOUNTLL)
#define X_CHUMP_POPCOUNT_U32(x_x)	X_CHUMP_BUILTIN_GNUC_POPCOUNTLL(x_x)
#endif
#else
#error "X_CHUMP_POPCOUNT_U32"
#endif
#ifndef X_CHUMP_POPCOUNT_U32
ALWAYS_INLINE CONST unsigned int
X_CHUMP_POPCOUNT_U32(uint32_t x)
/*@*/
{
	const uint32_t m1 = (uint32_t) 0x55555555u;
	const uint32_t m2 = (uint32_t) 0x33333333u;
	const uint32_t m3 = (uint32_t) 0x0F0F0F0Fu;
	const uint32_t f  = (uint32_t) 0x01010101u;

	x -= ((x >> 1u) & m1);
	x  = (x & m2) + ((x >> 2u) & m2);
	x  = (x + (x >> 4u)) & m3;
	x  = (x * f) >> 24u;
	return (unsigned int) (x & 0xFFu);
}
#endif
ALWAYS_INLINE CONST unsigned int
popcount_u32(uint32_t x)
/*@*/
{
	return X_CHUMP_POPCOUNT_U32(x);
}

/* EOF //////////////////////////////////////////////////////////////////// */
#endif	/* CHUMP_BUILTINS_H */
