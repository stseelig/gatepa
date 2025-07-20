#ifndef OVERFLOW_H
#define OVERFLOW_H
/* ///////////////////////////////////////////////////////////////////////////
//                                                                          //
// overflow.h - (some) checked overflow arithmetic                          //
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
#define X_OVERFLOW_HAS_ATTRIBUTE_GNUC(x)	__has_attribute(x)
#else
#define X_OVERFLOW_HAS_ATTRIBUTE_GNUC(x)	0
#endif

#define X_OVERFLOW_ATTRIBUTE_GNUC_ALWAYS_INLINE	always_inline

#else	/* !defined(__GNUC__) */

#define X_OVERFLOW_HAS_ATTRIBUTE_GNUC(x)	0

#define X_OVERFLOW_ATTRIBUTE_GNUC_ALWAYS_INLINE	nil

#endif	/* __GNUC__ */

/* ======================================================================== */

#if __STDC_VERSION__ >= 199901L
#define X_OVERFLOW_INLINE		/*@unused@*/ static inline
#elif defined(__GNUC__)
#define X_OVERFLOW_INLINE		/*@unused@*/ static __inline__
#else
#define X_OVERFLOW_INLINE		/*@unused@*/ static
#endif	/* __STDC_VERSION__ */

#if X_OVERFLOW_HAS_ATTRIBUTE_GNUC(X_OVERFLOW_ATTRIBUTE_GNUC_ALWAYS_INLINE)
#define X_OVERFLOW_ALWAYS_INLINE	\
	X_OVERFLOW_INLINE \
	__attribute__((X_OVERFLOW_ATTRIBUTE_GNUC_ALWAYS_INLINE))
#else
#define X_OVERFLOW_ALWAYS_INLINE	X_OVERFLOW_INLINE
#endif

/* //////////////////////////////////////////////////////////////////////// */

#ifdef __GNUC__

#ifdef __has_builtin
#define X_OVERFLOW_HAS_BUILTIN_GNUC(x)		__has_builtin(x)
#else
#define X_OVERFLOW_HAS_BUILTIN_GNUC(x)		0
#endif

#define X_OVERFLOW_BUILTIN_GNUC_UADD_OVERFLOW	__builtin_uadd_overflow
#define X_OVERFLOW_BUILTIN_GNUC_UADDL_OVERFLOW	__builtin_uaddl_overflow
#define X_OVERFLOW_BUILTIN_GNUC_UADDLL_OVERFLOW	__builtin_uaddll_overflow

#define X_OVERFLOW_BUILTIN_GNUC_USUB_OVERFLOW	__builtin_usub_overflow
#define X_OVERFLOW_BUILTIN_GNUC_USUBL_OVERFLOW	__builtin_usubl_overflow
#define X_OVERFLOW_BUILTIN_GNUC_USUBLL_OVERFLOW	__builtin_usubll_overflow

#define X_OVERFLOW_BUILTIN_GNUC_UMUL_OVERFLOW	__builtin_umul_overflow
#define X_OVERFLOW_BUILTIN_GNUC_UMULL_OVERFLOW	__builtin_umull_overflow
#define X_OVERFLOW_BUILTIN_GNUC_UMULLL_OVERFLOW	__builtin_umulll_overflow

#else	/* !defined(__GNUC__) */

#define X_OVERFLOW_HAS_BUILTIN_GNUC(x)		0

#define X_OVERFLOW_BUILTIN_GNUC_UADD_OVERFLOW	nil
#define X_OVERFLOW_BUILTIN_GNUC_UADDL_OVERFLOW	nil
#define X_OVERFLOW_BUILTIN_GNUC_UADDLL_OVERFLOW	nil

#define X_OVERFLOW_BUILTIN_GNUC_USUB_OVERFLOW	nil
#define X_OVERFLOW_BUILTIN_GNUC_USUBL_OVERFLOW	nil
#define X_OVERFLOW_BUILTIN_GNUC_USUBLL_OVERFLOW	nil

#define X_OVERFLOW_BUILTIN_GNUC_UMUL_OVERFLOW	nil
#define X_OVERFLOW_BUILTIN_GNUC_UMULL_OVERFLOW	nil
#define X_OVERFLOW_BUILTIN_GNUC_UMULLL_OVERFLOW	nil

#endif	/* __GNUC__ */

/* //////////////////////////////////////////////////////////////////////// */

/*@-mustdefine@*/	/* splint bug */

/* ======================================================================== */

#if X_OVERFLOW_HAS_BUILTIN_GNUC(X_OVERFLOW_BUILTIN_GNUC_UADD_OVERFLOW)
#define X_OVERFLOW_ADD_UINT_OVERFLOW(x_res_ptr, x_a, x_b)	( \
	(int) X_OVERFLOW_BUILTIN_GNUC_UADD_OVERFLOW( \
		(x_a), (x_b), (x_res_ptr) \
	) \
)
#else
X_OVERFLOW_ALWAYS_INLINE int
X_OVERFLOW_ADD_UINT_OVERFLOW(
	/*@out@*/ unsigned int *const res,
	const unsigned int a, const unsigned int b
)
/*@modifies	*res@*/
{
	*res = a + b;
	return (int) (*res < a);
}
#endif
X_OVERFLOW_ALWAYS_INLINE int
add_uint_overflow(
	/*@out@*/ unsigned int *const res,
	const unsigned int a, const unsigned int b
)
/*@modifies	*res@*/
{
	return X_OVERFLOW_ADD_UINT_OVERFLOW(res, a, b);
}

#if X_OVERFLOW_HAS_BUILTIN_GNUC(X_OVERFLOW_BUILTIN_GNUC_UADDL_OVERFLOW)
#define X_OVERFLOW_ADD_ULONG_OVERFLOW(x_res_ptr, x_a, x_b)	( \
	(int) X_OVERFLOW_BUILTIN_GNUC_UADDL_OVERFLOW( \
		(x_a), (x_b), (x_res_ptr) \
	) \
)
#else
X_OVERFLOW_ALWAYS_INLINE int
X_OVERFLOW_ADD_ULONG_OVERFLOW(
	/*@out@*/ unsigned long *const res,
	const unsigned long a, const unsigned long b
)
/*@modifies	*res@*/
{
	*res = a + b;
	return (int) (*res < a);
}
#endif
X_OVERFLOW_ALWAYS_INLINE int
add_ulong_overflow(
	/*@out@*/ unsigned long *const res,
	const unsigned long a, const unsigned long b
)
/*@modifies	*res@*/
{
	return X_OVERFLOW_ADD_ULONG_OVERFLOW(res, a, b);
}

#if X_OVERFLOW_HAS_BUILTIN_GNUC(X_OVERFLOW_BUILTIN_GNUC_UADDLL_OVERFLOW)
#define X_OVERFLOW_ADD_ULONGLONG_OVERFLOW(x_res_ptr, x_a, x_b)	( \
	(int) X_OVERFLOW_BUILTIN_GNUC_UADDLL_OVERFLOW( \
		(x_a), (x_b), (x_res_ptr) \
	) \
)
#else
X_OVERFLOW_ALWAYS_INLINE int
X_OVERFLOW_ADD_ULONGLONG_OVERFLOW(
	/*@out@*/ unsigned long long *const res,
	const unsigned long long a, const unsigned long long b
)
/*@modifies	*res@*/
{
	*res = a + b;
	return (int) (*res < a);
}
#endif
X_OVERFLOW_ALWAYS_INLINE int
add_ulonglong_overflow(
	/*@out@*/ unsigned long long *const res,
	const unsigned long long a, const unsigned long long b
)
/*@modifies	*res@*/
{
	return X_OVERFLOW_ADD_ULONGLONG_OVERFLOW(res, a, b);
}

/* ------------------------------------------------------------------------ */

#if   SIZE_MAX == UINT_MAX
X_OVERFLOW_ALWAYS_INLINE int
add_usize_overflow(
	/*@out@*/ size_t *const res, const size_t a, const size_t b
)
/*@modifies	*res@*/
{
	return add_uint_overflow(
		(unsigned int *) res, (unsigned int) a, (unsigned int) b
	);
}
#elif SIZE_MAX == ULONG_MAX
X_OVERFLOW_ALWAYS_INLINE int
add_usize_overflow(
	/*@out@*/ size_t *const res, const size_t a, const size_t b
)
/*@modifies	*res@*/
{
	return add_ulong_overflow(
		(unsigned long *) res, (unsigned long) a, (unsigned long) b
	);
}
#elif SIZE_MAX == ULONGLONG_MAX
X_OVERFLOW_ALWAYS_INLINE int
add_usize_overflow(
	/*@out@*/ size_t *const res, const size_t a, const size_t b
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
#error "add_usize_overflow"
#endif

/* ------------------------------------------------------------------------ */

#if   UINT32_MAX == UINT_MAX
X_OVERFLOW_ALWAYS_INLINE int
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
X_OVERFLOW_ALWAYS_INLINE int
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
X_OVERFLOW_ALWAYS_INLINE int
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

#if X_OVERFLOW_HAS_BUILTIN_GNUC(X_OVERFLOW_BUILTIN_GNUC_USUB_OVERFLOW)
#define X_OVERFLOW_SUB_UINT_OVERFLOW(x_res_ptr, x_a, x_b)	( \
	(int) X_OVERFLOW_BUILTIN_GNUC_USUB_OVERFLOW( \
		(x_a), (x_b), (x_res_ptr) \
	) \
)
#else
X_OVERFLOW_ALWAYS_INLINE int
X_OVERFLOW_SUB_UINT_OVERFLOW(
	/*@out@*/ unsigned int *const res,
	const unsigned int a, const unsigned int b
)
/*@modifies	*res@*/
{
	*res = a - b;
	return (int) (b > a);
}
#endif
X_OVERFLOW_ALWAYS_INLINE int
sub_uint_overflow(
	/*@out@*/ unsigned int *const res,
	const unsigned int a, const unsigned int b
)
/*@modifies	*res@*/
{
	return X_OVERFLOW_SUB_UINT_OVERFLOW(res, a, b);
}

#if X_OVERFLOW_HAS_BUILTIN_GNUC(X_OVERFLOW_BUILTIN_GNUC_USUBL_OVERFLOW)
#define X_OVERFLOW_SUB_ULONG_OVERFLOW(x_res_ptr, x_a, x_b)	( \
	(int) X_OVERFLOW_BUILTIN_GNUC_USUBL_OVERFLOW( \
		(x_a), (x_b), (x_res_ptr) \
	) \
)
#else
X_OVERFLOW_ALWAYS_INLINE int
X_OVERFLOW_SUB_ULONG_OVERFLOW(
	/*@out@*/ unsigned long *const res,
	const unsigned long a, const unsigned long b
)
/*@modifies	*res@*/
{
	*res = a - b;
	return (int) (b > a);
}
#endif
X_OVERFLOW_ALWAYS_INLINE int
sub_ulong_overflow(
	/*@out@*/ unsigned long *const res,
	const unsigned long a, const unsigned long b
)
/*@modifies	*res@*/
{
	return X_OVERFLOW_SUB_ULONG_OVERFLOW(res, a, b);
}

#if X_OVERFLOW_HAS_BUILTIN_GNUC(X_OVERFLOW_BUILTIN_GNUC_USUBLL_OVERFLOW)
#define X_OVERFLOW_SUB_ULONGLONG_OVERFLOW(x_res_ptr, x_a, x_b)	( \
	(int) X_OVERFLOW_BUILTIN_GNUC_USUBLL_OVERFLOW( \
		(x_a), (x_b), (x_res_ptr) \
	) \
)
#else
X_OVERFLOW_ALWAYS_INLINE int
X_OVERFLOW_SUB_ULONGLONG_OVERFLOW(
	/*@out@*/ unsigned long long *const res,
	const unsigned long long a, const unsigned long long b
)
/*@modifies	*res@*/
{
	*res = a - b;
	return (int) (b > a);
}
#endif
X_OVERFLOW_ALWAYS_INLINE int
sub_ulonglong_overflow(
	/*@out@*/ unsigned long long *const res,
	const unsigned long long a, const unsigned long long b
)
/*@modifies	*res@*/
{
	return X_OVERFLOW_SUB_ULONGLONG_OVERFLOW(res, a, b);
}

/* ------------------------------------------------------------------------ */

#if   SIZE_MAX == UINT_MAX
X_OVERFLOW_ALWAYS_INLINE int
sub_usize_overflow(
	/*@out@*/ size_t *const res, const size_t a, const size_t b
)
/*@modifies	*res@*/
{
	return sub_uint_overflow(
		(unsigned int *) res, (unsigned int) a, (unsigned int) b
	);
}
#elif SIZE_MAX == ULONG_MAX
X_OVERFLOW_ALWAYS_INLINE int
sub_usize_overflow(
	/*@out@*/ size_t *const res, const size_t a, const size_t b
)
/*@modifies	*res@*/
{
	return sub_ulong_overflow(
		(unsigned long *) res, (unsigned long) a, (unsigned long) b
	);
}
#elif SIZE_MAX == ULONGLONG_MAX
X_OVERFLOW_ALWAYS_INLINE int
sub_usize_overflow(
	/*@out@*/ size_t *const res, const size_t a, const size_t b
)
/*@modifies	*res@*/
{
	return sub_ulonglong_overflow(
		(unsigned long long *) res,
		(unsigned long long) a,
		(unsigned long long) b
	);
}
#else
#error "sub_usize_overflow"
#endif

/* ------------------------------------------------------------------------ */

#if   UINT32_MAX == UINT_MAX
X_OVERFLOW_ALWAYS_INLINE int
sub_u32_overflow(
	/*@out@*/ uint32_t *const res, const uint32_t a, const uint32_t b
)
/*@modifies	*res@*/
{
	return sub_uint_overflow(
		(unsigned int *) res, (unsigned int) a, (unsigned int) b
	);
}
#elif UINT32_MAX == ULONG_MAX
X_OVERFLOW_ALWAYS_INLINE int
sub_u32_overflow(
	/*@out@*/ uint32_t *const res, const uint32_t a, const uint32_t b
)
/*@modifies	*res@*/
{
	return sub_ulong_overflow(
		(unsigned long *) res, (unsigned long) a, (unsigned long) b
	);
}
#elif UINT32_MAX == ULONGLONG_MAX
X_OVERFLOW_ALWAYS_INLINE int
sub_u32_overflow(
	/*@out@*/ uint32_t *const res, const uint32_t a, const uint32_t b
)
/*@modifies	*res@*/
{
	return sub_ulonglong_overflow(
		(unsigned long long *) res,
		(unsigned long long) a,
		(unsigned long long) b
	);
}
#else
#error "sub_u32_overflow"
#endif

/* ======================================================================== */

#if X_OVERFLOW_HAS_BUILTIN_GNUC(X_OVERFLOW_BUILTIN_GNUC_UMUL_OVERFLOW)
#define X_OVERFLOW_MUL_UINT_OVERFLOW(x_res_ptr, x_a, x_b)	( \
	(int) X_OVERFLOW_BUILTIN_GNUC_UMUL_OVERFLOW( \
		(x_a), (x_b), (x_res_ptr) \
	) \
)
#else
X_OVERFLOW_ALWAYS_INLINE int
X_OVERFLOW_MUL_UINT_OVERFLOW(
	/*@out@*/ unsigned int *const res,
	const unsigned int a, const unsigned int b
)
/*@modifies	*res@*/
{
	*res = a * b;
	return (int) (((a != 0) && (*res / a == b)) ? 0 : 1);
}
#endif
X_OVERFLOW_ALWAYS_INLINE int
mul_uint_overflow(
	/*@out@*/ unsigned int *const res,
	const unsigned int a, const unsigned int b
)
/*@modifies	*res@*/
{
	return X_OVERFLOW_MUL_UINT_OVERFLOW(res, a, b);
}

#if X_OVERFLOW_HAS_BUILTIN_GNUC(X_OVERFLOW_BUILTIN_GNUC_UMULL_OVERFLOW)
#define X_OVERFLOW_MUL_ULONG_OVERFLOW(x_res_ptr, x_a, x_b)	( \
	(int) X_OVERFLOW_BUILTIN_GNUC_UMULL_OVERFLOW( \
		(x_a), (x_b), (x_res_ptr) \
	) \
)
#else
X_OVERFLOW_ALWAYS_INLINE int
X_OVERFLOW_MUL_ULONG_OVERFLOW(
	/*@out@*/ unsigned long *const res,
	const unsigned long a, const unsigned long b
)
/*@modifies	*res@*/
{
	*res = a * b;
	return (int) (((a != 0) && (*res / a == b)) ? 0 : 1);
}
#endif
X_OVERFLOW_ALWAYS_INLINE int
mul_ulong_overflow(
	/*@out@*/ unsigned long *const res,
	const unsigned long a, const unsigned long b
)
/*@modifies	*res@*/
{
	return X_OVERFLOW_MUL_ULONG_OVERFLOW(res, a, b);
}

#if X_OVERFLOW_HAS_BUILTIN_GNUC(X_OVERFLOW_BUILTIN_GNUC_UMULLL_OVERFLOW)
#define X_OVERFLOW_MUL_ULONGLONG_OVERFLOW(x_res_ptr, x_a, x_b)	( \
	(int) X_OVERFLOW_BUILTIN_GNUC_UMULLL_OVERFLOW( \
		(x_a), (x_b), (x_res_ptr) \
	) \
)
#else
X_OVERFLOW_ALWAYS_INLINE int
X_OVERFLOW_MUL_ULONGLONG_OVERFLOW(
	/*@out@*/ unsigned long long *const res,
	const unsigned long long a, const unsigned long long b
)
/*@modifies	*res@*/
{
	*res = a * b;
	return (int) (((a != 0) && (*res / a == b)) ? 0 : 1);
}
#endif
X_OVERFLOW_ALWAYS_INLINE int
mul_ulonglong_overflow(
	/*@out@*/ unsigned long long *const res,
	const unsigned long long a, const unsigned long long b
)
/*@modifies	*res@*/
{
	return X_OVERFLOW_MUL_ULONGLONG_OVERFLOW(res, a, b);
}

/* ------------------------------------------------------------------------ */

#if   SIZE_MAX == UINT_MAX
X_OVERFLOW_ALWAYS_INLINE int
mul_usize_overflow(
	/*@out@*/ size_t *const res, const size_t a, const size_t b
)
/*@modifies	*res@*/
{
	return mul_uint_overflow(
		(unsigned int *) res, (unsigned int) a, (unsigned int) b
	);
}
#elif SIZE_MAX == ULONG_MAX
X_OVERFLOW_ALWAYS_INLINE int
mul_usize_overflow(
	/*@out@*/ size_t *const res, const size_t a, const size_t b
)
/*@modifies	*res@*/
{
	return mul_ulong_overflow(
		(unsigned long *) res, (unsigned long) a, (unsigned long) b
	);
}
#elif SIZE_MAX == ULONGLONG_MAX
X_OVERFLOW_ALWAYS_INLINE int
mul_usize_overflow(
	/*@out@*/ size_t *const res, const size_t a, const size_t b
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
#error "mul_usize_overflow"
#endif

/* ======================================================================== */

/*@=mustdefine@*/

/* EOF //////////////////////////////////////////////////////////////////// */
#endif	/* OVERFLOW_H */
