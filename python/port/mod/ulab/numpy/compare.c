
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Zoltán Vörös
 *               2020 Jeff Epler for Adafruit Industries
*/

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/misc.h"

#include "../ulab.h"
#include "../ndarray_operators.h"
#include "../ulab_tools.h"
#include "carray/carray_tools.h"
#include "compare.h"

static mp_obj_t compare_function(mp_obj_t x1, mp_obj_t x2, uint8_t op) {
    ndarray_obj_t *lhs = ndarray_from_mp_obj(x1, 0);
    ndarray_obj_t *rhs = ndarray_from_mp_obj(x2, 0);
    #if ULAB_SUPPORTS_COMPLEX
    if((lhs->dtype == NDARRAY_COMPLEX) || (rhs->dtype == NDARRAY_COMPLEX)) {
        NOT_IMPLEMENTED_FOR_COMPLEX()
    }
    #endif
    uint8_t ndim = 0;
    size_t *shape = m_new(size_t, ULAB_MAX_DIMS);
    int32_t *lstrides = m_new(int32_t, ULAB_MAX_DIMS);
    int32_t *rstrides = m_new(int32_t, ULAB_MAX_DIMS);
    if(!ndarray_can_broadcast(lhs, rhs, &ndim, shape, lstrides, rstrides)) {
        mp_raise_ValueError(translate("operands could not be broadcast together"));
        m_del(size_t, shape, ULAB_MAX_DIMS);
        m_del(int32_t, lstrides, ULAB_MAX_DIMS);
        m_del(int32_t, rstrides, ULAB_MAX_DIMS);
    }

    uint8_t *larray = (uint8_t *)lhs->array;
    uint8_t *rarray = (uint8_t *)rhs->array;

    if(op == COMPARE_EQUAL) {
        return ndarray_binary_equality(lhs, rhs, ndim, shape, lstrides, rstrides, MP_BINARY_OP_EQUAL);
    } else if(op == COMPARE_NOT_EQUAL) {
        return ndarray_binary_equality(lhs, rhs, ndim, shape, lstrides, rstrides, MP_BINARY_OP_NOT_EQUAL);
    }
    // These are the upcasting rules
    // float always becomes float
    // operation on identical types preserves type
    // uint8 + int8 => int16
    // uint8 + int16 => int16
    // uint8 + uint16 => uint16
    // int8 + int16 => int16
    // int8 + uint16 => uint16
    // uint16 + int16 => float
    // The parameters of RUN_COMPARE_LOOP are
    // typecode of result, type_out, type_left, type_right, lhs operand, rhs operand, operator
    if(lhs->dtype == NDARRAY_UINT8) {
        if(rhs->dtype == NDARRAY_UINT8) {
            RUN_COMPARE_LOOP(NDARRAY_UINT8, uint8_t, uint8_t, uint8_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
        } else if(rhs->dtype == NDARRAY_INT8) {
            RUN_COMPARE_LOOP(NDARRAY_INT16, int16_t, uint8_t, int8_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            RUN_COMPARE_LOOP(NDARRAY_UINT16, uint16_t, uint8_t, uint16_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
        } else if(rhs->dtype == NDARRAY_INT16) {
            RUN_COMPARE_LOOP(NDARRAY_INT16, int16_t, uint8_t, int16_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            RUN_COMPARE_LOOP(NDARRAY_FLOAT, mp_float_t, uint8_t, mp_float_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
        }
    } else if(lhs->dtype == NDARRAY_INT8) {
        if(rhs->dtype == NDARRAY_UINT8) {
            RUN_COMPARE_LOOP(NDARRAY_INT16, int16_t, int8_t, uint8_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
        } else if(rhs->dtype == NDARRAY_INT8) {
            RUN_COMPARE_LOOP(NDARRAY_INT8, int8_t, int8_t, int8_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            RUN_COMPARE_LOOP(NDARRAY_INT16, int16_t, int8_t, uint16_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
        } else if(rhs->dtype == NDARRAY_INT16) {
            RUN_COMPARE_LOOP(NDARRAY_INT16, int16_t, int8_t, int16_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            RUN_COMPARE_LOOP(NDARRAY_FLOAT, mp_float_t, int8_t, mp_float_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
        }
    } else if(lhs->dtype == NDARRAY_UINT16) {
        if(rhs->dtype == NDARRAY_UINT8) {
            RUN_COMPARE_LOOP(NDARRAY_UINT16, uint16_t, uint16_t, uint8_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
        } else if(rhs->dtype == NDARRAY_INT8) {
            RUN_COMPARE_LOOP(NDARRAY_UINT16, uint16_t, uint16_t, int8_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            RUN_COMPARE_LOOP(NDARRAY_UINT16, uint16_t, uint16_t, uint16_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
        } else if(rhs->dtype == NDARRAY_INT16) {
            RUN_COMPARE_LOOP(NDARRAY_FLOAT, mp_float_t, uint16_t, int16_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            RUN_COMPARE_LOOP(NDARRAY_FLOAT, mp_float_t, uint16_t, mp_float_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
        }
    } else if(lhs->dtype == NDARRAY_INT16) {
        if(rhs->dtype == NDARRAY_UINT8) {
            RUN_COMPARE_LOOP(NDARRAY_INT16, int16_t, int16_t, uint8_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
        } else if(rhs->dtype == NDARRAY_INT8) {
            RUN_COMPARE_LOOP(NDARRAY_INT16, int16_t, int16_t, int8_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            RUN_COMPARE_LOOP(NDARRAY_FLOAT, mp_float_t, int16_t, uint16_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
        } else if(rhs->dtype == NDARRAY_INT16) {
            RUN_COMPARE_LOOP(NDARRAY_INT16, int16_t, int16_t, int16_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            RUN_COMPARE_LOOP(NDARRAY_FLOAT, mp_float_t, int16_t, mp_float_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
        }
    } else if(lhs->dtype == NDARRAY_FLOAT) {
        if(rhs->dtype == NDARRAY_UINT8) {
            RUN_COMPARE_LOOP(NDARRAY_FLOAT, mp_float_t, mp_float_t, uint8_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
        } else if(rhs->dtype == NDARRAY_INT8) {
            RUN_COMPARE_LOOP(NDARRAY_FLOAT, mp_float_t, mp_float_t, int8_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            RUN_COMPARE_LOOP(NDARRAY_FLOAT, mp_float_t, mp_float_t, uint16_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
        } else if(rhs->dtype == NDARRAY_INT16) {
            RUN_COMPARE_LOOP(NDARRAY_FLOAT, mp_float_t, mp_float_t, int16_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            RUN_COMPARE_LOOP(NDARRAY_FLOAT, mp_float_t, mp_float_t, mp_float_t, larray, lstrides, rarray, rstrides, ndim, shape, op);
        }
    }
    return mp_const_none; // we should never reach this point
}

static mp_obj_t compare_equal_helper(mp_obj_t x1, mp_obj_t x2, uint8_t comptype) {
    // scalar comparisons should return a single object of mp_obj_t type
    mp_obj_t result = compare_function(x1, x2, comptype);
    if((mp_obj_is_int(x1) || mp_obj_is_float(x1)) && (mp_obj_is_int(x2) || mp_obj_is_float(x2))) {
        mp_obj_iter_buf_t iter_buf;
        mp_obj_t iterable = mp_getiter(result, &iter_buf);
        mp_obj_t item = mp_iternext(iterable);
        return item;
    }
    return result;
}

#if ULAB_NUMPY_HAS_CLIP

mp_obj_t compare_clip(mp_obj_t x1, mp_obj_t x2, mp_obj_t x3) {
    // Note: this function could be made faster by implementing a single-loop comparison in
    // RUN_COMPARE_LOOP. However, that would add around 2 kB of compile size, while we
    // would not gain a factor of two in speed, since the two comparisons should still be
    // evaluated. In contrast, calling the function twice adds only 140 bytes to the firmware
    if(mp_obj_is_int(x1) || mp_obj_is_float(x1)) {
        mp_float_t v1 = mp_obj_get_float(x1);
        mp_float_t v2 = mp_obj_get_float(x2);
        mp_float_t v3 = mp_obj_get_float(x3);
        if(v1 < v2) {
            return x2;
        } else if(v1 > v3) {
            return x3;
        } else {
            return x1;
        }
    } else { // assume ndarrays
        return compare_function(x2, compare_function(x1, x3, COMPARE_MINIMUM), COMPARE_MAXIMUM);
    }
}

MP_DEFINE_CONST_FUN_OBJ_3(compare_clip_obj, compare_clip);
#endif

#if ULAB_NUMPY_HAS_EQUAL

mp_obj_t compare_equal(mp_obj_t x1, mp_obj_t x2) {
    return compare_equal_helper(x1, x2, COMPARE_EQUAL);
}

MP_DEFINE_CONST_FUN_OBJ_2(compare_equal_obj, compare_equal);
#endif

#if ULAB_NUMPY_HAS_NOTEQUAL

mp_obj_t compare_not_equal(mp_obj_t x1, mp_obj_t x2) {
    return compare_equal_helper(x1, x2, COMPARE_NOT_EQUAL);
}

MP_DEFINE_CONST_FUN_OBJ_2(compare_not_equal_obj, compare_not_equal);
#endif

#if ULAB_NUMPY_HAS_ISFINITE | ULAB_NUMPY_HAS_ISINF
static mp_obj_t compare_isinf_isfinite(mp_obj_t _x, uint8_t mask) {
    // mask should signify, whether the function is called from isinf (mask = 1),
    // or from isfinite (mask = 0)
    if(mp_obj_is_int(_x)) {
        if(mask) {
            return mp_const_false;
        } else {
            return mp_const_true;
        }
    } else if(mp_obj_is_float(_x)) {
        mp_float_t x = mp_obj_get_float(_x);
        if(isnan(x)) {
            return mp_const_false;
        }
        if(mask) { // called from isinf
            return isinf(x) ? mp_const_true : mp_const_false;
        } else { // called from isfinite
            return isinf(x) ? mp_const_false : mp_const_true;
        }
    } else if(mp_obj_is_type(_x, &ulab_ndarray_type)) {
        ndarray_obj_t *x = MP_OBJ_TO_PTR(_x);
        COMPLEX_DTYPE_NOT_IMPLEMENTED(x->dtype)
        ndarray_obj_t *results = ndarray_new_dense_ndarray(x->ndim, x->shape, NDARRAY_BOOL);
        // At this point, results is all False
        uint8_t *rarray = (uint8_t *)results->array;
        if(x->dtype != NDARRAY_FLOAT) {
            // int types can never be infinite...
            if(!mask) {
                // ...so flip all values in the array, if the function was called from isfinite
                memset(rarray, 1, results->len);
            }
            return results;
        }
        uint8_t *xarray = (uint8_t *)x->array;

        #if ULAB_MAX_DIMS > 3
        size_t i = 0;
        do {
        #endif
            #if ULAB_MAX_DIMS > 2
            size_t j = 0;
            do {
            #endif
                #if ULAB_MAX_DIMS > 1
                size_t k = 0;
                do {
                #endif
                    size_t l = 0;
                    do {
                        mp_float_t value = *(mp_float_t *)xarray;
                        if(isnan(value)) {
                            *rarray++ = 0;
                        } else {
                            *rarray++ = isinf(value) ? mask : 1 - mask;
                        }
                        xarray += x->strides[ULAB_MAX_DIMS - 1];
                        l++;
                    } while(l < x->shape[ULAB_MAX_DIMS - 1]);
                #if ULAB_MAX_DIMS > 1
                    xarray -= x->strides[ULAB_MAX_DIMS - 1] * x->shape[ULAB_MAX_DIMS-1];
                    xarray += x->strides[ULAB_MAX_DIMS - 2];
                    k++;
                } while(k < x->shape[ULAB_MAX_DIMS - 2]);
                #endif
            #if ULAB_MAX_DIMS > 2
                xarray -= x->strides[ULAB_MAX_DIMS - 2] * x->shape[ULAB_MAX_DIMS-2];
                xarray += x->strides[ULAB_MAX_DIMS - 3];
                j++;
            } while(j < x->shape[ULAB_MAX_DIMS - 3]);
            #endif
        #if ULAB_MAX_DIMS > 3
            xarray -= x->strides[ULAB_MAX_DIMS - 3] * x->shape[ULAB_MAX_DIMS-3];
            xarray += x->strides[ULAB_MAX_DIMS - 4];
            i++;
        } while(i < x->shape[ULAB_MAX_DIMS - 4]);
        #endif

        return results;
    } else {
        mp_raise_TypeError(translate("wrong input type"));
    }
    return mp_const_none;
}
#endif

#if ULAB_NUMPY_HAS_ISFINITE
mp_obj_t compare_isfinite(mp_obj_t _x) {
    return compare_isinf_isfinite(_x, 0);
}

MP_DEFINE_CONST_FUN_OBJ_1(compare_isfinite_obj, compare_isfinite);
#endif

#if ULAB_NUMPY_HAS_ISINF
mp_obj_t compare_isinf(mp_obj_t _x) {
    return compare_isinf_isfinite(_x, 1);
}

MP_DEFINE_CONST_FUN_OBJ_1(compare_isinf_obj, compare_isinf);
#endif

#if ULAB_NUMPY_HAS_MAXIMUM
mp_obj_t compare_maximum(mp_obj_t x1, mp_obj_t x2) {
    // extra round, so that we can return maximum(3, 4) properly
    mp_obj_t result = compare_function(x1, x2, COMPARE_MAXIMUM);
    if((mp_obj_is_int(x1) || mp_obj_is_float(x1)) && (mp_obj_is_int(x2) || mp_obj_is_float(x2))) {
        ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(result);
        return mp_binary_get_val_array(ndarray->dtype, ndarray->array, 0);
    }
    return result;
}

MP_DEFINE_CONST_FUN_OBJ_2(compare_maximum_obj, compare_maximum);
#endif

#if ULAB_NUMPY_HAS_MINIMUM

mp_obj_t compare_minimum(mp_obj_t x1, mp_obj_t x2) {
    // extra round, so that we can return minimum(3, 4) properly
    mp_obj_t result = compare_function(x1, x2, COMPARE_MINIMUM);
    if((mp_obj_is_int(x1) || mp_obj_is_float(x1)) && (mp_obj_is_int(x2) || mp_obj_is_float(x2))) {
        ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(result);
        return mp_binary_get_val_array(ndarray->dtype, ndarray->array, 0);
    }
    return result;
}

MP_DEFINE_CONST_FUN_OBJ_2(compare_minimum_obj, compare_minimum);
#endif

#if ULAB_NUMPY_HAS_WHERE

mp_obj_t compare_where(mp_obj_t _condition, mp_obj_t _x, mp_obj_t _y) {
    // this implementation will work with ndarrays, and scalars only
    ndarray_obj_t *c = ndarray_from_mp_obj(_condition, 0);
    ndarray_obj_t *x = ndarray_from_mp_obj(_x, 0);
    ndarray_obj_t *y = ndarray_from_mp_obj(_y, 0);

    COMPLEX_DTYPE_NOT_IMPLEMENTED(c->dtype)
    COMPLEX_DTYPE_NOT_IMPLEMENTED(x->dtype)
    COMPLEX_DTYPE_NOT_IMPLEMENTED(y->dtype)

    int32_t *cstrides = m_new(int32_t, ULAB_MAX_DIMS);
    int32_t *xstrides = m_new(int32_t, ULAB_MAX_DIMS);
    int32_t *ystrides = m_new(int32_t, ULAB_MAX_DIMS);

    size_t *oshape = m_new(size_t, ULAB_MAX_DIMS);

    uint8_t ndim;

    // establish the broadcasting conditions first
    // if any two of the arrays can be broadcast together, then
    // the three arrays can also be broadcast together
    if(!ndarray_can_broadcast(c, x, &ndim, oshape, cstrides, ystrides) ||
        !ndarray_can_broadcast(c, y, &ndim, oshape, cstrides, ystrides) ||
        !ndarray_can_broadcast(x, y, &ndim, oshape, xstrides, ystrides)) {
        mp_raise_ValueError(translate("operands could not be broadcast together"));
    }

    ndim = MAX(MAX(c->ndim, x->ndim), y->ndim);

    for(uint8_t i = 1; i <= ndim; i++) {
        cstrides[ULAB_MAX_DIMS - i] = c->shape[ULAB_MAX_DIMS - i] < 2 ? 0 : c->strides[ULAB_MAX_DIMS - i];
        xstrides[ULAB_MAX_DIMS - i] = x->shape[ULAB_MAX_DIMS - i] < 2 ? 0 : x->strides[ULAB_MAX_DIMS - i];
        ystrides[ULAB_MAX_DIMS - i] = y->shape[ULAB_MAX_DIMS - i] < 2 ? 0 : y->strides[ULAB_MAX_DIMS - i];
        oshape[ULAB_MAX_DIMS - i] = MAX(MAX(c->shape[ULAB_MAX_DIMS - i], x->shape[ULAB_MAX_DIMS - i]), y->shape[ULAB_MAX_DIMS - i]);
    }

    uint8_t out_dtype = ndarray_upcast_dtype(x->dtype, y->dtype);
    ndarray_obj_t *out = ndarray_new_dense_ndarray(ndim, oshape, out_dtype);

    mp_float_t (*cfunc)(void *) = ndarray_get_float_function(c->dtype);
    mp_float_t (*xfunc)(void *) = ndarray_get_float_function(x->dtype);
    mp_float_t (*yfunc)(void *) = ndarray_get_float_function(y->dtype);
    mp_float_t (*ofunc)(void *, mp_float_t ) = ndarray_set_float_function(out->dtype);

    uint8_t *oarray = (uint8_t *)out->array;
    uint8_t *carray = (uint8_t *)c->array;
    uint8_t *xarray = (uint8_t *)x->array;
    uint8_t *yarray = (uint8_t *)y->array;

    #if ULAB_MAX_DIMS > 3
    size_t i = 0;
    do {
    #endif
        #if ULAB_MAX_DIMS > 2
        size_t j = 0;
        do {
        #endif
            #if ULAB_MAX_DIMS > 1
            size_t k = 0;
            do {
            #endif
                size_t l = 0;
                do {
                    mp_float_t value;
                    mp_float_t cvalue = cfunc(carray);
                    if(cvalue != MICROPY_FLOAT_CONST(0.0)) {
                        value = xfunc(xarray);
                    } else {
                        value = yfunc(yarray);
                    }
                    ofunc(oarray, value);
                    oarray += out->itemsize;
                    carray += cstrides[ULAB_MAX_DIMS - 1];
                    xarray += xstrides[ULAB_MAX_DIMS - 1];
                    yarray += ystrides[ULAB_MAX_DIMS - 1];
                    l++;
                } while(l < out->shape[ULAB_MAX_DIMS - 1]);
            #if ULAB_MAX_DIMS > 1
                carray -= cstrides[ULAB_MAX_DIMS - 1] * c->shape[ULAB_MAX_DIMS-1];
                carray += cstrides[ULAB_MAX_DIMS - 2];
                xarray -= xstrides[ULAB_MAX_DIMS - 1] * x->shape[ULAB_MAX_DIMS-1];
                xarray += xstrides[ULAB_MAX_DIMS - 2];
                yarray -= ystrides[ULAB_MAX_DIMS - 1] * y->shape[ULAB_MAX_DIMS-1];
                yarray += ystrides[ULAB_MAX_DIMS - 2];
                k++;
            } while(k < out->shape[ULAB_MAX_DIMS - 2]);
            #endif
        #if ULAB_MAX_DIMS > 2
            carray -= cstrides[ULAB_MAX_DIMS - 2] * c->shape[ULAB_MAX_DIMS-2];
            carray += cstrides[ULAB_MAX_DIMS - 3];
            xarray -= xstrides[ULAB_MAX_DIMS - 2] * x->shape[ULAB_MAX_DIMS-2];
            xarray += xstrides[ULAB_MAX_DIMS - 3];
            yarray -= ystrides[ULAB_MAX_DIMS - 2] * y->shape[ULAB_MAX_DIMS-2];
            yarray += ystrides[ULAB_MAX_DIMS - 3];
            j++;
        } while(j < out->shape[ULAB_MAX_DIMS - 3]);
        #endif
    #if ULAB_MAX_DIMS > 3
        carray -= cstrides[ULAB_MAX_DIMS - 3] * c->shape[ULAB_MAX_DIMS-3];
        carray += cstrides[ULAB_MAX_DIMS - 4];
        xarray -= xstrides[ULAB_MAX_DIMS - 3] * x->shape[ULAB_MAX_DIMS-3];
        xarray += xstrides[ULAB_MAX_DIMS - 4];
        yarray -= ystrides[ULAB_MAX_DIMS - 3] * y->shape[ULAB_MAX_DIMS-3];
        yarray += ystrides[ULAB_MAX_DIMS - 4];
        i++;
    } while(i < out->shape[ULAB_MAX_DIMS - 4]);
    #endif
    return MP_OBJ_FROM_PTR(out);
}

MP_DEFINE_CONST_FUN_OBJ_3(compare_where_obj, compare_where);
#endif
