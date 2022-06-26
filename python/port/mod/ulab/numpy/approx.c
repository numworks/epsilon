/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Zoltán Vörös
 *               2020 Diego Elio Pettenò
 *               2020 Taku Fukada
*/

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/misc.h"

#include "../ulab.h"
#include "../ulab_tools.h"
#include "carray/carray_tools.h"
#include "approx.h"

//| """Numerical approximation methods"""
//|

const mp_obj_float_t approx_trapz_dx = {{&mp_type_float}, MICROPY_FLOAT_CONST(1.0)};

#if ULAB_NUMPY_HAS_INTERP
//| def interp(
//|     x: ulab.numpy.ndarray,
//|     xp: ulab.numpy.ndarray,
//|     fp: ulab.numpy.ndarray,
//|     *,
//|     left: Optional[_float] = None,
//|     right: Optional[_float] = None
//| ) -> ulab.numpy.ndarray:
//|     """
//|     :param ulab.numpy.ndarray x: The x-coordinates at which to evaluate the interpolated values.
//|     :param ulab.numpy.ndarray xp: The x-coordinates of the data points, must be increasing
//|     :param ulab.numpy.ndarray fp: The y-coordinates of the data points, same length as xp
//|     :param left: Value to return for ``x < xp[0]``, default is ``fp[0]``.
//|     :param right: Value to return for ``x > xp[-1]``, default is ``fp[-1]``.
//|
//|     Returns the one-dimensional piecewise linear interpolant to a function with given discrete data points (xp, fp), evaluated at x."""
//|     ...
//|

STATIC mp_obj_t approx_interp(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_left, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = mp_const_none} },
        { MP_QSTR_right, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = mp_const_none} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    ndarray_obj_t *x = ndarray_from_mp_obj(args[0].u_obj, 0);
    ndarray_obj_t *xp = ndarray_from_mp_obj(args[1].u_obj, 0); // xp must hold an increasing sequence of independent values
    ndarray_obj_t *fp = ndarray_from_mp_obj(args[2].u_obj, 0);
    COMPLEX_DTYPE_NOT_IMPLEMENTED(x->dtype)
    COMPLEX_DTYPE_NOT_IMPLEMENTED(xp->dtype)
    COMPLEX_DTYPE_NOT_IMPLEMENTED(fp->dtype)
    if((xp->ndim != 1) || (fp->ndim != 1) || (xp->len < 2) || (fp->len < 2) || (xp->len != fp->len)) {
        mp_raise_ValueError(translate("interp is defined for 1D iterables of equal length"));
    }

    ndarray_obj_t *y = ndarray_new_linear_array(x->len, NDARRAY_FLOAT);
    mp_float_t left_value, right_value;
    uint8_t *xparray = (uint8_t *)xp->array;

    mp_float_t xp_left = ndarray_get_float_value(xparray, xp->dtype);
    xparray += (xp->len-1) * xp->strides[ULAB_MAX_DIMS - 1];
    mp_float_t xp_right = ndarray_get_float_value(xparray, xp->dtype);

    uint8_t *fparray = (uint8_t *)fp->array;

    if(args[3].u_obj == mp_const_none) {
        left_value = ndarray_get_float_value(fparray, fp->dtype);
    } else {
        left_value = mp_obj_get_float(args[3].u_obj);
    }
    if(args[4].u_obj == mp_const_none) {
        fparray += (fp->len-1) * fp->strides[ULAB_MAX_DIMS - 1];
        right_value = ndarray_get_float_value(fparray, fp->dtype);
    } else {
        right_value = mp_obj_get_float(args[4].u_obj);
    }

    xparray = xp->array;
    fparray = fp->array;

    uint8_t *xarray = (uint8_t *)x->array;
    mp_float_t *yarray = (mp_float_t *)y->array;
    uint8_t *temp;

    for(size_t i=0; i < x->len; i++, yarray++) {
        mp_float_t x_value = ndarray_get_float_value(xarray, x->dtype);
        xarray += x->strides[ULAB_MAX_DIMS - 1];
        if(x_value < xp_left) {
            *yarray = left_value;
        } else if(x_value > xp_right) {
            *yarray = right_value;
        } else { // do the binary search here
            mp_float_t xp_left_, xp_right_;
            mp_float_t fp_left, fp_right;
            size_t left_index = 0, right_index = xp->len - 1, middle_index;
            while(right_index - left_index > 1) {
                middle_index = left_index + (right_index - left_index) / 2;
                temp = xparray + middle_index * xp->strides[ULAB_MAX_DIMS - 1];
                mp_float_t xp_middle = ndarray_get_float_value(temp, xp->dtype);
                if(x_value <= xp_middle) {
                    right_index = middle_index;
                } else {
                    left_index = middle_index;
                }
            }
            temp = xparray + left_index * xp->strides[ULAB_MAX_DIMS - 1];
            xp_left_ = ndarray_get_float_value(temp, xp->dtype);

            temp = xparray + right_index * xp->strides[ULAB_MAX_DIMS - 1];
            xp_right_ = ndarray_get_float_value(temp, xp->dtype);

            temp = fparray + left_index * fp->strides[ULAB_MAX_DIMS - 1];
            fp_left = ndarray_get_float_value(temp, fp->dtype);

            temp = fparray + right_index * fp->strides[ULAB_MAX_DIMS - 1];
            fp_right = ndarray_get_float_value(temp, fp->dtype);

            *yarray = fp_left + (x_value - xp_left_) * (fp_right - fp_left) / (xp_right_ - xp_left_);
        }
    }
    return MP_OBJ_FROM_PTR(y);
}

MP_DEFINE_CONST_FUN_OBJ_KW(approx_interp_obj, 2, approx_interp);
#endif

#if ULAB_NUMPY_HAS_TRAPZ
//| def trapz(y: ulab.numpy.ndarray, x: Optional[ulab.numpy.ndarray] = None, dx: _float = 1.0) -> _float:
//|     """
//|     :param 1D ulab.numpy.ndarray y: the values of the dependent variable
//|     :param 1D ulab.numpy.ndarray x: optional, the coordinates of the independent variable. Defaults to uniformly spaced values.
//|     :param float dx: the spacing between sample points, if x=None
//|
//|     Returns the integral of y(x) using the trapezoidal rule.
//|     """
//|     ...
//|

STATIC mp_obj_t approx_trapz(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_x, MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_dx, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&approx_trapz_dx)} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    ndarray_obj_t *y = ndarray_from_mp_obj(args[0].u_obj, 0);
    COMPLEX_DTYPE_NOT_IMPLEMENTED(y->dtype)
    ndarray_obj_t *x;
    mp_float_t mean = MICROPY_FLOAT_CONST(0.0);
    if(y->len < 2) {
        return mp_obj_new_float(mean);
    }
    if((y->ndim != 1)) {
        mp_raise_ValueError(translate("trapz is defined for 1D iterables"));
    }

    mp_float_t (*funcy)(void *) = ndarray_get_float_function(y->dtype);
    uint8_t *yarray = (uint8_t *)y->array;

    size_t count = 1;
    mp_float_t y1, y2, m;

    if(args[1].u_obj != mp_const_none) {
        x = ndarray_from_mp_obj(args[1].u_obj, 0); // x must hold an increasing sequence of independent values
        COMPLEX_DTYPE_NOT_IMPLEMENTED(x->dtype)
        if((x->ndim != 1) || (y->len != x->len)) {
            mp_raise_ValueError(translate("trapz is defined for 1D arrays of equal length"));
        }

        mp_float_t (*funcx)(void *) = ndarray_get_float_function(x->dtype);
        uint8_t *xarray = (uint8_t *)x->array;
        mp_float_t x1, x2;

        y1 = funcy(yarray);
        yarray += y->strides[ULAB_MAX_DIMS - 1];
        x1 = funcx(xarray);
        xarray += x->strides[ULAB_MAX_DIMS - 1];

        for(size_t i=1; i < y->len; i++) {
            y2 = funcy(yarray);
            yarray += y->strides[ULAB_MAX_DIMS - 1];
            x2 = funcx(xarray);
            xarray += x->strides[ULAB_MAX_DIMS - 1];
            mp_float_t value = (x2 - x1) * (y2 + y1);
            m = mean + (value - mean) / (mp_float_t)count;
            mean = m;
            x1 = x2;
            y1 = y2;
            count++;
        }
    } else {
        mp_float_t dx = mp_obj_get_float(args[2].u_obj);
        y1 = funcy(yarray);
        yarray += y->strides[ULAB_MAX_DIMS - 1];

        for(size_t i=1; i < y->len; i++) {
            y2 = ndarray_get_float_index(y->array, y->dtype, i);
            mp_float_t value = (y2 + y1);
            m = mean + (value - mean) / (mp_float_t)count;
            mean = m;
            y1 = y2;
            count++;
        }
        mean *= dx;
    }
    return mp_obj_new_float(MICROPY_FLOAT_CONST(0.5)*mean*(y->len-1));
}

MP_DEFINE_CONST_FUN_OBJ_KW(approx_trapz_obj, 1, approx_trapz);
#endif
