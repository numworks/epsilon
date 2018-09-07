// -*- C++ -*-
//===--------------------------- complex ----------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "complex.h"

_LIBCPP_BEGIN_NAMESPACE_STD

template<class _Tp>
complex<_Tp>
operator/(const complex<_Tp>& __z, const complex<_Tp>& __w)
{
    int __ilogbw = 0;
    _Tp __a = __z.real();
    _Tp __b = __z.imag();
    _Tp __c = __w.real();
    _Tp __d = __w.imag();
    _Tp __logbw = logb(fmax(fabs(__c), fabs(__d)));
    if (__libcpp_isfinite_or_builtin(__logbw))
    {
        __ilogbw = static_cast<int>(__logbw);
        __c = scalbn(__c, -__ilogbw);
        __d = scalbn(__d, -__ilogbw);
    }
    _Tp __denom = __c * __c + __d * __d;
    _Tp __x = scalbn((__a * __c + __b * __d) / __denom, -__ilogbw);
    _Tp __y = scalbn((__b * __c - __a * __d) / __denom, -__ilogbw);
    if (__libcpp_isnan_or_builtin(__x) && __libcpp_isnan_or_builtin(__y))
    {
        if ((__denom == _Tp(0)) && (!__libcpp_isnan_or_builtin(__a) || !__libcpp_isnan_or_builtin(__b)))
        {
            __x = copysign(_Tp(INFINITY), __c) * __a;
            __y = copysign(_Tp(INFINITY), __c) * __b;
        }
        else if ((__libcpp_isinf_or_builtin(__a) || __libcpp_isinf_or_builtin(__b)) && __libcpp_isfinite_or_builtin(__c) && __libcpp_isfinite_or_builtin(__d))
        {
            __a = copysign(__libcpp_isinf_or_builtin(__a) ? _Tp(1) : _Tp(0), __a);
            __b = copysign(__libcpp_isinf_or_builtin(__b) ? _Tp(1) : _Tp(0), __b);
            __x = _Tp(INFINITY) * (__a * __c + __b * __d);
            __y = _Tp(INFINITY) * (__b * __c - __a * __d);
        }
        else if (__libcpp_isinf_or_builtin(__logbw) && __logbw > _Tp(0) && __libcpp_isfinite_or_builtin(__a) && __libcpp_isfinite_or_builtin(__b))
        {
            __c = copysign(__libcpp_isinf_or_builtin(__c) ? _Tp(1) : _Tp(0), __c);
            __d = copysign(__libcpp_isinf_or_builtin(__d) ? _Tp(1) : _Tp(0), __d);
            __x = _Tp(0) * (__a * __c + __b * __d);
            __y = _Tp(0) * (__b * __c - __a * __d);
        }
    }
    return complex<_Tp>(__x, __y);
}

template complex<float> operator/(const complex<float>& __z, const complex<float>& __w);
template complex<double> operator/(const complex<double>& __z, const complex<double>& __w);

_LIBCPP_END_NAMESPACE_STD
