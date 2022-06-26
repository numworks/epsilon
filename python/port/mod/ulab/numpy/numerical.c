
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2021 Zoltán Vörös
 *               2020 Scott Shawcroft for Adafruit Industries
 *               2020 Taku Fukada
*/

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "py/obj.h"
#include "py/objint.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/misc.h"

#include "../ulab.h"
#include "../ulab_tools.h"
#include "./carray/carray_tools.h"
#include "numerical.h"

enum NUMERICAL_FUNCTION_TYPE {
    NUMERICAL_ALL,
    NUMERICAL_ANY,
    NUMERICAL_ARGMAX,
    NUMERICAL_ARGMIN,
    NUMERICAL_MAX,
    NUMERICAL_MEAN,
    NUMERICAL_MIN,
    NUMERICAL_STD,
    NUMERICAL_SUM,
};

//| """Numerical and Statistical functions
//|
//| Most of these functions take an "axis" argument, which indicates whether to
//| operate over the flattened array (None), or a particular axis (integer)."""
//|
//| from typing import Dict
//|
//| _ArrayLike = Union[ndarray, List[_float], Tuple[_float], range]
//|
//| _DType = int
//| """`ulab.numpy.int8`, `ulab.numpy.uint8`, `ulab.numpy.int16`, `ulab.numpy.uint16`, `ulab.numpy.float` or `ulab.numpy.bool`"""
//|
//| from builtins import float as _float
//| from builtins import bool as _bool
//|
//| int8: _DType
//| """Type code for signed integers in the range -128 .. 127 inclusive, like the 'b' typecode of `array.array`"""
//|
//| int16: _DType
//| """Type code for signed integers in the range -32768 .. 32767 inclusive, like the 'h' typecode of `array.array`"""
//|
//| float: _DType
//| """Type code for floating point values, like the 'f' typecode of `array.array`"""
//|
//| uint8: _DType
//| """Type code for unsigned integers in the range 0 .. 255 inclusive, like the 'H' typecode of `array.array`"""
//|
//| uint16: _DType
//| """Type code for unsigned integers in the range 0 .. 65535 inclusive, like the 'h' typecode of `array.array`"""
//|
//| bool: _DType
//| """Type code for boolean values"""
//|

static void numerical_reduce_axes(ndarray_obj_t *ndarray, int8_t axis, size_t *shape, int32_t *strides) {
    // removes the values corresponding to a single axis from the shape and strides array
    uint8_t index = ULAB_MAX_DIMS - ndarray->ndim + axis;
    if((ndarray->ndim == 1) && (axis == 0)) {
        index = 0;
        shape[ULAB_MAX_DIMS - 1] = 1;
        return;
    }
    for(uint8_t i = ULAB_MAX_DIMS - 1; i > 0; i--) {
        if(i > index) {
            shape[i] = ndarray->shape[i];
            strides[i] = ndarray->strides[i];
        } else {
            shape[i] = ndarray->shape[i-1];
            strides[i] = ndarray->strides[i-1];
        }
    }
}

#if ULAB_NUMPY_HAS_ALL | ULAB_NUMPY_HAS_ANY
static mp_obj_t numerical_all_any(mp_obj_t oin, mp_obj_t axis, uint8_t optype) {
    bool anytype = optype == NUMERICAL_ALL ? 1 : 0;
    if(mp_obj_is_type(oin, &ulab_ndarray_type)) {
        ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(oin);
        uint8_t *array = (uint8_t *)ndarray->array;
        if(ndarray->len == 0) { // return immediately with empty arrays
        if(optype == NUMERICAL_ALL) {
                return mp_const_true;
            } else {
                return mp_const_false;
            }
        }
        // always get a float, so that we don't have to resolve the dtype later
        mp_float_t (*func)(void *) = ndarray_get_float_function(ndarray->dtype);
        ndarray_obj_t *results = NULL;
        uint8_t *rarray = NULL;
        shape_strides _shape_strides = tools_reduce_axes(ndarray, axis);
        if(axis != mp_const_none) {
            results = ndarray_new_dense_ndarray(_shape_strides.ndim, _shape_strides.shape, NDARRAY_BOOL);
            rarray = results->array;
            if(optype == NUMERICAL_ALL) {
                memset(rarray, 1, results->len);
            }
        }

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
                    if(axis == mp_const_none) {
                        do {
                            #if ULAB_SUPPORTS_COMPLEX
                            if(ndarray->dtype == NDARRAY_COMPLEX) {
                                mp_float_t real = *((mp_float_t *)array);
                                mp_float_t imag = *((mp_float_t *)(array + sizeof(mp_float_t)));
                                if(((real != MICROPY_FLOAT_CONST(0.0)) | (imag != MICROPY_FLOAT_CONST(0.0))) & !anytype) {
                                    // optype = NUMERICAL_ANY
                                    return mp_const_true;
                                } else if(((real == MICROPY_FLOAT_CONST(0.0)) & (imag == MICROPY_FLOAT_CONST(0.0))) & anytype) {
                                    // optype == NUMERICAL_ALL
                                    return mp_const_false;
                                }
                            } else {
                            #endif
                                mp_float_t value = func(array);
                                if((value != MICROPY_FLOAT_CONST(0.0)) & !anytype) {
                                    // optype = NUMERICAL_ANY
                                    return mp_const_true;
                                } else if((value == MICROPY_FLOAT_CONST(0.0)) & anytype) {
                                    // optype == NUMERICAL_ALL
                                    return mp_const_false;
                                }
                            #if ULAB_SUPPORTS_COMPLEX
                            }
                            #endif
                            array += _shape_strides.strides[0];
                            l++;
                        } while(l < _shape_strides.shape[0]);
                    } else { // a scalar axis keyword was supplied
                        do {
                            #if ULAB_SUPPORTS_COMPLEX
                            if(ndarray->dtype == NDARRAY_COMPLEX) {
                                mp_float_t real = *((mp_float_t *)array);
                                mp_float_t imag = *((mp_float_t *)(array + sizeof(mp_float_t)));
                                if(((real != MICROPY_FLOAT_CONST(0.0)) | (imag != MICROPY_FLOAT_CONST(0.0))) & !anytype) {
                                    // optype = NUMERICAL_ANY
                                    *rarray = 1;
                                    // since we are breaking out of the loop, move the pointer forward
                                    array += _shape_strides.strides[0] * (_shape_strides.shape[0] - l);
                                    break;
                                } else if(((real == MICROPY_FLOAT_CONST(0.0)) & (imag == MICROPY_FLOAT_CONST(0.0))) & anytype) {
                                    // optype == NUMERICAL_ALL
                                    *rarray = 0;
                                    // since we are breaking out of the loop, move the pointer forward
                                    array += _shape_strides.strides[0] * (_shape_strides.shape[0] - l);
                                    break;
                                }
                            } else {
                            #endif
                                mp_float_t value = func(array);
                                if((value != MICROPY_FLOAT_CONST(0.0)) & !anytype) {
                                    // optype == NUMERICAL_ANY
                                    *rarray = 1;
                                    // since we are breaking out of the loop, move the pointer forward
                                    array += _shape_strides.strides[0] * (_shape_strides.shape[0] - l);
                                    break;
                                } else if((value == MICROPY_FLOAT_CONST(0.0)) & anytype) {
                                    // optype == NUMERICAL_ALL
                                    *rarray = 0;
                                    // since we are breaking out of the loop, move the pointer forward
                                    array += _shape_strides.strides[0] * (_shape_strides.shape[0] - l);
                                    break;
                                }
                            #if ULAB_SUPPORTS_COMPLEX
                            }
                            #endif
                            array += _shape_strides.strides[0];
                            l++;
                        } while(l < _shape_strides.shape[0]);
                    }
                #if ULAB_MAX_DIMS > 1
                    rarray += _shape_strides.increment;
                    array -= _shape_strides.strides[0] * _shape_strides.shape[0];
                    array += _shape_strides.strides[ULAB_MAX_DIMS - 1];
                    k++;
                } while(k < _shape_strides.shape[ULAB_MAX_DIMS - 1]);
                #endif
            #if ULAB_MAX_DIMS > 2
                array -= _shape_strides.strides[ULAB_MAX_DIMS - 1] * _shape_strides.shape[ULAB_MAX_DIMS - 1];
                array += _shape_strides.strides[ULAB_MAX_DIMS - 2];
                j++;
            } while(j < _shape_strides.shape[ULAB_MAX_DIMS - 2]);
            #endif
        #if ULAB_MAX_DIMS > 3
            array -= _shape_strides.strides[ULAB_MAX_DIMS - 2] * _shape_strides.shape[ULAB_MAX_DIMS - 2];
            array += _shape_strides.strides[ULAB_MAX_DIMS - 3];
            i++;
        } while(i < _shape_strides.shape[ULAB_MAX_DIMS - 3]);
        #endif
        if(axis == mp_const_none) {
            // the innermost loop fell through, so return the result here
            if(!anytype) {
                return mp_const_false;
            } else {
                return mp_const_true;
            }
        }
        return results;
    } else if(mp_obj_is_int(oin) || mp_obj_is_float(oin)) {
        return mp_obj_is_true(oin) ? mp_const_true : mp_const_false;
    } else {
        mp_obj_iter_buf_t iter_buf;
        mp_obj_t item, iterable = mp_getiter(oin, &iter_buf);
        while((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
            if(!mp_obj_is_true(item) & !anytype) {
                return mp_const_false;
            } else if(mp_obj_is_true(item) & anytype) {
                return mp_const_true;
            }
        }
    }
    return anytype ? mp_const_true : mp_const_false;
}
#endif

#if ULAB_NUMPY_HAS_SUM | ULAB_NUMPY_HAS_MEAN | ULAB_NUMPY_HAS_STD
static mp_obj_t numerical_sum_mean_std_iterable(mp_obj_t oin, uint8_t optype, size_t ddof) {
    mp_float_t value = MICROPY_FLOAT_CONST(0.0);
    mp_float_t M = MICROPY_FLOAT_CONST(0.0);
    mp_float_t m = MICROPY_FLOAT_CONST(0.0);
    mp_float_t S = MICROPY_FLOAT_CONST(0.0);
    mp_float_t s = MICROPY_FLOAT_CONST(0.0);
    size_t count = 0;
    mp_obj_iter_buf_t iter_buf;
    mp_obj_t item, iterable = mp_getiter(oin, &iter_buf);
    while((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
        value = mp_obj_get_float(item);
        m = M + (value - M) / (count + 1);
        s = S + (value - M) * (value - m);
        M = m;
        S = s;
        count++;
    }
    if(optype == NUMERICAL_SUM) {
        return mp_obj_new_float(m * count);
    } else if(optype == NUMERICAL_MEAN) {
        return count > 0 ? mp_obj_new_float(m) : mp_obj_new_float(MICROPY_FLOAT_CONST(0.0));
    } else { // this should be the case of the standard deviation
        return count > ddof ? mp_obj_new_float(MICROPY_FLOAT_C_FUN(sqrt)(s / (count - ddof))) : mp_obj_new_float(MICROPY_FLOAT_CONST(0.0));
    }
}

static mp_obj_t numerical_sum_mean_std_ndarray(ndarray_obj_t *ndarray, mp_obj_t axis, uint8_t optype, size_t ddof) {
    COMPLEX_DTYPE_NOT_IMPLEMENTED(ndarray->dtype)
    uint8_t *array = (uint8_t *)ndarray->array;
    shape_strides _shape_strides = tools_reduce_axes(ndarray, axis);

    if(axis == mp_const_none) {
        // work with the flattened array
        if((optype == NUMERICAL_STD) && (ddof > ndarray->len)) {
            // if there are too many degrees of freedom, there is no point in calculating anything
            return mp_obj_new_float(MICROPY_FLOAT_CONST(0.0));
        }
        mp_float_t (*func)(void *) = ndarray_get_float_function(ndarray->dtype);
        mp_float_t M = MICROPY_FLOAT_CONST(0.0);
        mp_float_t m = MICROPY_FLOAT_CONST(0.0);
        mp_float_t S = MICROPY_FLOAT_CONST(0.0);
        mp_float_t s = MICROPY_FLOAT_CONST(0.0);
        size_t count = 0;

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
                        count++;
                        mp_float_t value = func(array);
                        m = M + (value - M) / (mp_float_t)count;
                        if(optype == NUMERICAL_STD) {
                            s = S + (value - M) * (value - m);
                            S = s;
                        }
                        M = m;
                        array += _shape_strides.strides[ULAB_MAX_DIMS - 1];
                        l++;
                    } while(l < _shape_strides.shape[ULAB_MAX_DIMS - 1]);
                #if ULAB_MAX_DIMS > 1
                    array -= _shape_strides.strides[ULAB_MAX_DIMS - 1] * _shape_strides.shape[ULAB_MAX_DIMS - 1];
                    array += _shape_strides.strides[ULAB_MAX_DIMS - 2];
                    k++;
                } while(k < _shape_strides.shape[ULAB_MAX_DIMS - 2]);
                #endif
            #if ULAB_MAX_DIMS > 2
                array -= _shape_strides.strides[ULAB_MAX_DIMS - 2] * _shape_strides.shape[ULAB_MAX_DIMS - 2];
                array += _shape_strides.strides[ULAB_MAX_DIMS - 3];
                j++;
            } while(j < _shape_strides.shape[ULAB_MAX_DIMS - 3]);
            #endif
        #if ULAB_MAX_DIMS > 3
            array -= _shape_strides.strides[ULAB_MAX_DIMS - 3] * _shape_strides.shape[ULAB_MAX_DIMS - 3];
            array += _shape_strides.strides[ULAB_MAX_DIMS - 4];
            i++;
        } while(i < _shape_strides.shape[ULAB_MAX_DIMS - 4]);
        #endif
        if(optype == NUMERICAL_SUM) {
            // numpy returns an integer for integer input types
            if(ndarray->dtype == NDARRAY_FLOAT) {
                return mp_obj_new_float(M * ndarray->len);
            } else {
                return mp_obj_new_int((int32_t)MICROPY_FLOAT_C_FUN(round)(M * ndarray->len));
            }
        } else if(optype == NUMERICAL_MEAN) {
            return mp_obj_new_float(M);
        } else { // this must be the case of the standard deviation
            // we have already made certain that ddof < ndarray->len holds
            return mp_obj_new_float(MICROPY_FLOAT_C_FUN(sqrt)(S / (ndarray->len - ddof)));
        }
    } else {
        ndarray_obj_t *results = NULL;
        uint8_t *rarray = NULL;
        mp_float_t *farray = NULL;
        if(optype == NUMERICAL_SUM) {
            results = ndarray_new_dense_ndarray(_shape_strides.ndim, _shape_strides.shape, ndarray->dtype);
            rarray = (uint8_t *)results->array;
            // TODO: numpy promotes the output to the highest integer type
            if(ndarray->dtype == NDARRAY_UINT8) {
                RUN_SUM(uint8_t, array, results, rarray, _shape_strides);
            } else if(ndarray->dtype == NDARRAY_INT8) {
                RUN_SUM(int8_t, array, results, rarray, _shape_strides);
            } else if(ndarray->dtype == NDARRAY_UINT16) {
                RUN_SUM(uint16_t, array, results, rarray, _shape_strides);
            } else if(ndarray->dtype == NDARRAY_INT16) {
                RUN_SUM(int16_t, array, results, rarray, _shape_strides);
            } else {
                // for floats, the sum might be inaccurate with the naive summation
                // call mean, and multiply with the number of samples
                farray = (mp_float_t *)results->array;
                RUN_MEAN_STD(mp_float_t, array, farray, _shape_strides, MICROPY_FLOAT_CONST(0.0), 0);
                mp_float_t norm = (mp_float_t)_shape_strides.shape[0];
                // re-wind the array here
                farray = (mp_float_t *)results->array;
                for(size_t i=0; i < results->len; i++) {
                    *farray++ *= norm;
                }
            }
        } else {
            bool isStd = optype == NUMERICAL_STD ? 1 : 0;
            results = ndarray_new_dense_ndarray(_shape_strides.ndim, _shape_strides.shape, NDARRAY_FLOAT);
            farray = (mp_float_t *)results->array;
            // we can return the 0 array here, if the degrees of freedom is larger than the length of the axis
            if((optype == NUMERICAL_STD) && (_shape_strides.shape[0] <= ddof)) {
                return MP_OBJ_FROM_PTR(results);
            }
            mp_float_t div = optype == NUMERICAL_STD ? (mp_float_t)(_shape_strides.shape[0] - ddof) : MICROPY_FLOAT_CONST(0.0);
            if(ndarray->dtype == NDARRAY_UINT8) {
                RUN_MEAN_STD(uint8_t, array, farray, _shape_strides, div, isStd);
            } else if(ndarray->dtype == NDARRAY_INT8) {
                RUN_MEAN_STD(int8_t, array, farray, _shape_strides, div, isStd);
            } else if(ndarray->dtype == NDARRAY_UINT16) {
                RUN_MEAN_STD(uint16_t, array, farray, _shape_strides, div, isStd);
            } else if(ndarray->dtype == NDARRAY_INT16) {
                RUN_MEAN_STD(int16_t, array, farray, _shape_strides, div, isStd);
            } else {
                RUN_MEAN_STD(mp_float_t, array, farray, _shape_strides, div, isStd);
            }
        }
        if(results->ndim == 0) { // return a scalar here
            return mp_binary_get_val_array(results->dtype, results->array, 0);
        }
        return MP_OBJ_FROM_PTR(results);
    }
    return mp_const_none;
}
#endif

#if ULAB_NUMPY_HAS_ARGMINMAX
static mp_obj_t numerical_argmin_argmax_iterable(mp_obj_t oin, uint8_t optype) {
    if(MP_OBJ_SMALL_INT_VALUE(mp_obj_len_maybe(oin)) == 0) {
        mp_raise_ValueError(translate("attempt to get argmin/argmax of an empty sequence"));
    }
    size_t idx = 0, best_idx = 0;
    mp_obj_iter_buf_t iter_buf;
    mp_obj_t iterable = mp_getiter(oin, &iter_buf);
    mp_obj_t item;
    uint8_t op = 0; // argmin, min
    if((optype == NUMERICAL_ARGMAX) || (optype == NUMERICAL_MAX)) op = 1;
    item = mp_iternext(iterable);
    mp_obj_t best_obj = item;
    mp_float_t value, best_value = mp_obj_get_float(item);
    value = best_value;
    while((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
        idx++;
        value = mp_obj_get_float(item);
        if((op == 0) && (value < best_value)) {
            best_obj = item;
            best_idx = idx;
            best_value = value;
        } else if((op == 1) && (value > best_value)) {
            best_obj = item;
            best_idx = idx;
            best_value = value;
        }
    }
    if((optype == NUMERICAL_ARGMIN) || (optype == NUMERICAL_ARGMAX)) {
        return MP_OBJ_NEW_SMALL_INT(best_idx);
    } else {
        return best_obj;
    }
}

static mp_obj_t numerical_argmin_argmax_ndarray(ndarray_obj_t *ndarray, mp_obj_t axis, uint8_t optype) {
    // TODO: treat the flattened array
    if(ndarray->len == 0) {
        mp_raise_ValueError(translate("attempt to get (arg)min/(arg)max of empty sequence"));
    }

    if(axis == mp_const_none) {
        // work with the flattened array
        mp_float_t (*func)(void *) = ndarray_get_float_function(ndarray->dtype);
        uint8_t *array = (uint8_t *)ndarray->array;
        mp_float_t best_value = func(array);
        mp_float_t value;
        size_t index = 0, best_index = 0;

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
                        value = func(array);
                        if((optype == NUMERICAL_ARGMAX) || (optype == NUMERICAL_MAX)) {
                            if(best_value < value) {
                                best_value = value;
                                best_index = index;
                            }
                        } else {
                            if(best_value > value) {
                                best_value = value;
                                best_index = index;
                            }
                        }
                        array += ndarray->strides[ULAB_MAX_DIMS - 1];
                        l++;
                        index++;
                    } while(l < ndarray->shape[ULAB_MAX_DIMS - 1]);
                #if ULAB_MAX_DIMS > 1
                    array -= ndarray->strides[ULAB_MAX_DIMS - 1] * ndarray->shape[ULAB_MAX_DIMS-1];
                    array += ndarray->strides[ULAB_MAX_DIMS - 2];
                    k++;
                } while(k < ndarray->shape[ULAB_MAX_DIMS - 2]);
                #endif
            #if ULAB_MAX_DIMS > 2
                array -= ndarray->strides[ULAB_MAX_DIMS - 2] * ndarray->shape[ULAB_MAX_DIMS-2];
                array += ndarray->strides[ULAB_MAX_DIMS - 3];
                j++;
            } while(j < ndarray->shape[ULAB_MAX_DIMS - 3]);
            #endif
        #if ULAB_MAX_DIMS > 3
            array -= ndarray->strides[ULAB_MAX_DIMS - 3] * ndarray->shape[ULAB_MAX_DIMS-3];
            array += ndarray->strides[ULAB_MAX_DIMS - 4];
            i++;
        } while(i < ndarray->shape[ULAB_MAX_DIMS - 4]);
        #endif

        if((optype == NUMERICAL_ARGMIN) || (optype == NUMERICAL_ARGMAX)) {
            return mp_obj_new_int(best_index);
        } else {
            if(ndarray->dtype == NDARRAY_FLOAT) {
                return mp_obj_new_float(best_value);
            } else {
                return MP_OBJ_NEW_SMALL_INT((int32_t)best_value);
            }
        }
    } else {
        int8_t ax = tools_get_axis(axis, ndarray->ndim);

        uint8_t *array = (uint8_t *)ndarray->array;
        size_t *shape = m_new0(size_t, ULAB_MAX_DIMS);
        int32_t *strides = m_new0(int32_t, ULAB_MAX_DIMS);

        numerical_reduce_axes(ndarray, ax, shape, strides);
        uint8_t index = ULAB_MAX_DIMS - ndarray->ndim + ax;

        ndarray_obj_t *results = NULL;

        if((optype == NUMERICAL_ARGMIN) || (optype == NUMERICAL_ARGMAX)) {
            results = ndarray_new_dense_ndarray(MAX(1, ndarray->ndim-1), shape, NDARRAY_INT16);
        } else {
            results = ndarray_new_dense_ndarray(MAX(1, ndarray->ndim-1), shape, ndarray->dtype);
        }

        uint8_t *rarray = (uint8_t *)results->array;

        if(ndarray->dtype == NDARRAY_UINT8) {
            RUN_ARGMIN(ndarray, uint8_t, array, results, rarray, shape, strides, index, optype);
        } else if(ndarray->dtype == NDARRAY_INT8) {
            RUN_ARGMIN(ndarray, int8_t, array, results, rarray, shape, strides, index, optype);
        } else if(ndarray->dtype == NDARRAY_UINT16) {
            RUN_ARGMIN(ndarray, uint16_t, array, results, rarray, shape, strides, index, optype);
        } else if(ndarray->dtype == NDARRAY_INT16) {
            RUN_ARGMIN(ndarray, int16_t, array, results, rarray, shape, strides, index, optype);
        } else {
            RUN_ARGMIN(ndarray, mp_float_t, array, results, rarray, shape, strides, index, optype);
        }

        m_del(int32_t, strides, ULAB_MAX_DIMS);

        if(results->len == 1) {
            return mp_binary_get_val_array(results->dtype, results->array, 0);
        }
        return MP_OBJ_FROM_PTR(results);
    }
    return mp_const_none;
}
#endif

static mp_obj_t numerical_function(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args, uint8_t optype) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_rom_obj = mp_const_none} } ,
        { MP_QSTR_axis, MP_ARG_OBJ, { .u_rom_obj = mp_const_none } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t oin = args[0].u_obj;
    mp_obj_t axis = args[1].u_obj;
    if((axis != mp_const_none) && (!mp_obj_is_int(axis))) {
        mp_raise_TypeError(translate("axis must be None, or an integer"));
    }

    if((optype == NUMERICAL_ALL) || (optype == NUMERICAL_ANY)) {
        return numerical_all_any(oin, axis, optype);
    }
    if(mp_obj_is_type(oin, &mp_type_tuple) || mp_obj_is_type(oin, &mp_type_list) ||
        mp_obj_is_type(oin, &mp_type_range)) {
        switch(optype) {
            case NUMERICAL_MIN:
            case NUMERICAL_ARGMIN:
            case NUMERICAL_MAX:
            case NUMERICAL_ARGMAX:
                return numerical_argmin_argmax_iterable(oin, optype);
            case NUMERICAL_SUM:
            case NUMERICAL_MEAN:
                return numerical_sum_mean_std_iterable(oin, optype, 0);
            default: // we should never reach this point, but whatever
                return mp_const_none;
        }
    } else if(mp_obj_is_type(oin, &ulab_ndarray_type)) {
        ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(oin);
        switch(optype) {
            case NUMERICAL_MIN:
            case NUMERICAL_MAX:
            case NUMERICAL_ARGMIN:
            case NUMERICAL_ARGMAX:
                COMPLEX_DTYPE_NOT_IMPLEMENTED(ndarray->dtype)
                return numerical_argmin_argmax_ndarray(ndarray, axis, optype);
            case NUMERICAL_SUM:
            case NUMERICAL_MEAN:
                COMPLEX_DTYPE_NOT_IMPLEMENTED(ndarray->dtype)
                return numerical_sum_mean_std_ndarray(ndarray, axis, optype, 0);
            default:
                mp_raise_NotImplementedError(translate("operation is not implemented on ndarrays"));
        }
    } else {
        mp_raise_TypeError(translate("input must be tuple, list, range, or ndarray"));
    }
    return mp_const_none;
}

#if ULAB_NUMPY_HAS_SORT | NDARRAY_HAS_SORT
static mp_obj_t numerical_sort_helper(mp_obj_t oin, mp_obj_t axis, uint8_t inplace) {
    if(!mp_obj_is_type(oin, &ulab_ndarray_type)) {
        mp_raise_TypeError(translate("sort argument must be an ndarray"));
    }

    ndarray_obj_t *ndarray;
    if(inplace == 1) {
        ndarray = MP_OBJ_TO_PTR(oin);
    } else {
        ndarray = ndarray_copy_view(MP_OBJ_TO_PTR(oin));
    }
    COMPLEX_DTYPE_NOT_IMPLEMENTED(ndarray->dtype)

    int8_t ax = 0;
    if(axis == mp_const_none) {
        // flatten the array
        #if ULAB_MAX_DIMS > 1
        for(uint8_t i=0; i < ULAB_MAX_DIMS - 1; i++) {
            ndarray->shape[i] = 0;
            ndarray->strides[i] = 0;
        }
        ndarray->shape[ULAB_MAX_DIMS - 1] = ndarray->len;
        ndarray->strides[ULAB_MAX_DIMS - 1] = ndarray->itemsize;
        ndarray->ndim = 1;
        #endif
    } else {
        ax = tools_get_axis(axis, ndarray->ndim);
    }

    size_t *shape = m_new0(size_t, ULAB_MAX_DIMS);
    int32_t *strides = m_new0(int32_t, ULAB_MAX_DIMS);

    numerical_reduce_axes(ndarray, ax, shape, strides);
    ax = ULAB_MAX_DIMS - ndarray->ndim + ax;
    // we work with the typed array, so re-scale the stride
    int32_t increment = ndarray->strides[ax] / ndarray->itemsize;

    uint8_t *array = (uint8_t *)ndarray->array;
    if(ndarray->shape[ax]) {
        if((ndarray->dtype == NDARRAY_UINT8) || (ndarray->dtype == NDARRAY_INT8)) {
            HEAPSORT(ndarray, uint8_t, array, shape, strides, ax, increment, ndarray->shape[ax]);
        } else if((ndarray->dtype == NDARRAY_INT16) || (ndarray->dtype == NDARRAY_INT16)) {
            HEAPSORT(ndarray, uint16_t, array, shape, strides, ax, increment, ndarray->shape[ax]);
        } else {
            HEAPSORT(ndarray, mp_float_t, array, shape, strides, ax, increment, ndarray->shape[ax]);
        }
    }

    m_del(int32_t, strides, ULAB_MAX_DIMS);

    if(inplace == 1) {
        return mp_const_none;
    } else {
        return MP_OBJ_FROM_PTR(ndarray);
    }
}
#endif /* ULAB_NUMERICAL_HAS_SORT | NDARRAY_HAS_SORT */

#if ULAB_NUMPY_HAS_ALL
mp_obj_t numerical_all(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return numerical_function(n_args, pos_args, kw_args, NUMERICAL_ALL);
}
MP_DEFINE_CONST_FUN_OBJ_KW(numerical_all_obj, 1, numerical_all);
#endif

#if ULAB_NUMPY_HAS_ANY
mp_obj_t numerical_any(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return numerical_function(n_args, pos_args, kw_args, NUMERICAL_ANY);
}
MP_DEFINE_CONST_FUN_OBJ_KW(numerical_any_obj, 1, numerical_any);
#endif

#if ULAB_NUMPY_HAS_ARGMINMAX
//| def argmax(array: _ArrayLike, *, axis: Optional[int] = None) -> int:
//|     """Return the index of the maximum element of the 1D array"""
//|     ...
//|

mp_obj_t numerical_argmax(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return numerical_function(n_args, pos_args, kw_args, NUMERICAL_ARGMAX);
}

MP_DEFINE_CONST_FUN_OBJ_KW(numerical_argmax_obj, 1, numerical_argmax);

//| def argmin(array: _ArrayLike, *, axis: Optional[int] = None) -> int:
//|     """Return the index of the minimum element of the 1D array"""
//|     ...
//|

static mp_obj_t numerical_argmin(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return numerical_function(n_args, pos_args, kw_args, NUMERICAL_ARGMIN);
}

MP_DEFINE_CONST_FUN_OBJ_KW(numerical_argmin_obj, 1, numerical_argmin);
#endif

#if ULAB_NUMPY_HAS_ARGSORT
//| def argsort(array: ulab.numpy.ndarray, *, axis: int = -1) -> ulab.numpy.ndarray:
//|     """Returns an array which gives indices into the input array from least to greatest."""
//|     ...
//|

mp_obj_t numerical_argsort(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_rom_obj = mp_const_none } },
        { MP_QSTR_axis, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_rom_obj = mp_const_none } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    if(!mp_obj_is_type(args[0].u_obj, &ulab_ndarray_type)) {
        mp_raise_TypeError(translate("argsort argument must be an ndarray"));
    }

    ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(args[0].u_obj);
    COMPLEX_DTYPE_NOT_IMPLEMENTED(ndarray->dtype)
    if(args[1].u_obj == mp_const_none) {
        // bail out, though dense arrays could still be sorted
        mp_raise_NotImplementedError(translate("argsort is not implemented for flattened arrays"));
    }
    // Since we are returning an NDARRAY_UINT16 array, bail out,
    // if the axis is longer than what we can hold
    for(uint8_t i=0; i < ULAB_MAX_DIMS; i++) {
        if(ndarray->shape[i] > 65535) {
            mp_raise_ValueError(translate("axis too long"));
        }
    }
    int8_t ax = tools_get_axis(args[1].u_obj, ndarray->ndim);

    size_t *shape = m_new0(size_t, ULAB_MAX_DIMS);
    int32_t *strides = m_new0(int32_t, ULAB_MAX_DIMS);
    numerical_reduce_axes(ndarray, ax, shape, strides);

    // We could return an NDARRAY_UINT8 array, if all lengths are shorter than 256
    ndarray_obj_t *indices = ndarray_new_ndarray(ndarray->ndim, ndarray->shape, NULL, NDARRAY_UINT16);
    int32_t *istrides = m_new0(int32_t, ULAB_MAX_DIMS);
    numerical_reduce_axes(indices, ax, shape, istrides);

    for(uint8_t i=0; i < ULAB_MAX_DIMS; i++) {
        istrides[i] /= sizeof(uint16_t);
    }

    ax = ULAB_MAX_DIMS - ndarray->ndim + ax;
    // we work with the typed array, so re-scale the stride
    int32_t increment = ndarray->strides[ax] / ndarray->itemsize;
    uint16_t iincrement = indices->strides[ax] / sizeof(uint16_t);

    uint8_t *array = (uint8_t *)ndarray->array;
    uint16_t *iarray = (uint16_t *)indices->array;

    // fill in the index values
    #if ULAB_MAX_DIMS > 3
    size_t j = 0;
    do {
    #endif
        #if ULAB_MAX_DIMS > 2
        size_t k = 0;
        do {
        #endif
            #if ULAB_MAX_DIMS > 1
            size_t l = 0;
            do {
            #endif
            uint16_t m = 0;
                do {
                    *iarray = m++;
                    iarray += iincrement;
                } while(m < indices->shape[ax]);
            #if ULAB_MAX_DIMS > 1
                iarray -= iincrement * indices->shape[ax];
                iarray += istrides[ULAB_MAX_DIMS - 1];
                l++;
            } while(l < shape[ULAB_MAX_DIMS - 1]);
            iarray -= istrides[ULAB_MAX_DIMS - 1] * shape[ULAB_MAX_DIMS - 1];
            iarray += istrides[ULAB_MAX_DIMS - 2];
            #endif
        #if ULAB_MAX_DIMS > 2
            k++;
        } while(k < shape[ULAB_MAX_DIMS - 2]);
        iarray -= istrides[ULAB_MAX_DIMS - 2] * shape[ULAB_MAX_DIMS - 2];
        iarray += istrides[ULAB_MAX_DIMS - 3];
        #endif
    #if ULAB_MAX_DIMS > 3
        j++;
    } while(j < shape[ULAB_MAX_DIMS - 3]);
    #endif
    // reset the array
    iarray = indices->array;

    if(ndarray->shape[ax]) {
        if((ndarray->dtype == NDARRAY_UINT8) || (ndarray->dtype == NDARRAY_INT8)) {
            HEAP_ARGSORT(ndarray, uint8_t, array, shape, strides, ax, increment, ndarray->shape[ax], iarray, istrides, iincrement);
        } else if((ndarray->dtype == NDARRAY_UINT16) || (ndarray->dtype == NDARRAY_INT16)) {
            HEAP_ARGSORT(ndarray, uint16_t, array, shape, strides, ax, increment, ndarray->shape[ax], iarray, istrides, iincrement);
        } else {
            HEAP_ARGSORT(ndarray, mp_float_t, array, shape, strides, ax, increment, ndarray->shape[ax], iarray, istrides, iincrement);
        }
    }

    m_del(size_t, shape, ULAB_MAX_DIMS);
    m_del(int32_t, strides, ULAB_MAX_DIMS);
    m_del(int32_t, istrides, ULAB_MAX_DIMS);

    return MP_OBJ_FROM_PTR(indices);
}

MP_DEFINE_CONST_FUN_OBJ_KW(numerical_argsort_obj, 1, numerical_argsort);
#endif

#if ULAB_NUMPY_HAS_CROSS
//| def cross(a: ulab.numpy.ndarray, b: ulab.numpy.ndarray) -> ulab.numpy.ndarray:
//|     """Return the cross product of two vectors of length 3"""
//|     ...
//|

static mp_obj_t numerical_cross(mp_obj_t _a, mp_obj_t _b) {
    if (!mp_obj_is_type(_a, &ulab_ndarray_type) || !mp_obj_is_type(_b, &ulab_ndarray_type)) {
        mp_raise_TypeError(translate("arguments must be ndarrays"));
    }
    ndarray_obj_t *a = MP_OBJ_TO_PTR(_a);
    ndarray_obj_t *b = MP_OBJ_TO_PTR(_b);
    COMPLEX_DTYPE_NOT_IMPLEMENTED(a->dtype)
    COMPLEX_DTYPE_NOT_IMPLEMENTED(b->dtype)
    if((a->ndim != 1) || (b->ndim != 1) || (a->len != b->len) || (a->len != 3)) {
        mp_raise_ValueError(translate("cross is defined for 1D arrays of length 3"));
    }

    mp_float_t *results = m_new(mp_float_t, 3);
    results[0] = ndarray_get_float_index(a->array, a->dtype, 1) * ndarray_get_float_index(b->array, b->dtype, 2);
    results[0] -= ndarray_get_float_index(a->array, a->dtype, 2) * ndarray_get_float_index(b->array, b->dtype, 1);
    results[1] = -ndarray_get_float_index(a->array, a->dtype, 0) * ndarray_get_float_index(b->array, b->dtype, 2);
    results[1] += ndarray_get_float_index(a->array, a->dtype, 2) * ndarray_get_float_index(b->array, b->dtype, 0);
    results[2] = ndarray_get_float_index(a->array, a->dtype, 0) * ndarray_get_float_index(b->array, b->dtype, 1);
    results[2] -= ndarray_get_float_index(a->array, a->dtype, 1) * ndarray_get_float_index(b->array, b->dtype, 0);

    /* The upcasting happens here with the rules

        - if one of the operarands is a float, the result is always float
        - operation on identical types preserves type

        uint8 + int8 => int16
        uint8 + int16 => int16
        uint8 + uint16 => uint16
        int8 + int16 => int16
        int8 + uint16 => uint16
        uint16 + int16 => float

    */

    uint8_t dtype = NDARRAY_FLOAT;
    if(a->dtype == b->dtype) {
        dtype = a->dtype;
    } else if(((a->dtype == NDARRAY_UINT8) && (b->dtype == NDARRAY_INT8)) || ((a->dtype == NDARRAY_INT8) && (b->dtype == NDARRAY_UINT8))) {
        dtype = NDARRAY_INT16;
    } else if(((a->dtype == NDARRAY_UINT8) && (b->dtype == NDARRAY_INT16)) || ((a->dtype == NDARRAY_INT16) && (b->dtype == NDARRAY_UINT8))) {
        dtype = NDARRAY_INT16;
    } else if(((a->dtype == NDARRAY_UINT8) && (b->dtype == NDARRAY_UINT16)) || ((a->dtype == NDARRAY_UINT16) && (b->dtype == NDARRAY_UINT8))) {
        dtype = NDARRAY_UINT16;
    } else if(((a->dtype == NDARRAY_INT8) && (b->dtype == NDARRAY_INT16)) || ((a->dtype == NDARRAY_INT16) && (b->dtype == NDARRAY_INT8))) {
        dtype = NDARRAY_INT16;
    } else if(((a->dtype == NDARRAY_INT8) && (b->dtype == NDARRAY_UINT16)) || ((a->dtype == NDARRAY_UINT16) && (b->dtype == NDARRAY_INT8))) {
        dtype = NDARRAY_UINT16;
    }

    ndarray_obj_t *ndarray = ndarray_new_linear_array(3, dtype);
    if(dtype == NDARRAY_UINT8) {
        uint8_t *array = (uint8_t *)ndarray->array;
        for(uint8_t i=0; i < 3; i++) array[i] = (uint8_t)results[i];
    } else if(dtype == NDARRAY_INT8) {
        int8_t *array = (int8_t *)ndarray->array;
        for(uint8_t i=0; i < 3; i++) array[i] = (int8_t)results[i];
    } else if(dtype == NDARRAY_UINT16) {
        uint16_t *array = (uint16_t *)ndarray->array;
        for(uint8_t i=0; i < 3; i++) array[i] = (uint16_t)results[i];
    } else if(dtype == NDARRAY_INT16) {
        int16_t *array = (int16_t *)ndarray->array;
        for(uint8_t i=0; i < 3; i++) array[i] = (int16_t)results[i];
    } else {
        mp_float_t *array = (mp_float_t *)ndarray->array;
        for(uint8_t i=0; i < 3; i++) array[i] = results[i];
    }
    m_del(mp_float_t, results, 3);
    return MP_OBJ_FROM_PTR(ndarray);
}

MP_DEFINE_CONST_FUN_OBJ_2(numerical_cross_obj, numerical_cross);

#endif /* ULAB_NUMERICAL_HAS_CROSS */

#if ULAB_NUMPY_HAS_DIFF
//| def diff(array: ulab.numpy.ndarray, *, n: int = 1, axis: int = -1) -> ulab.numpy.ndarray:
//|     """Return the numerical derivative of successive elements of the array, as
//|        an array.  axis=None is not supported."""
//|     ...
//|

mp_obj_t numerical_diff(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_n, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1 } },
        { MP_QSTR_axis, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1 } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if(!mp_obj_is_type(args[0].u_obj, &ulab_ndarray_type)) {
        mp_raise_TypeError(translate("diff argument must be an ndarray"));
    }

    ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(args[0].u_obj);
    COMPLEX_DTYPE_NOT_IMPLEMENTED(ndarray->dtype)
    int8_t ax = args[2].u_int;
    if(ax < 0) ax += ndarray->ndim;

    if((ax < 0) || (ax > ndarray->ndim - 1)) {
        mp_raise_ValueError(translate("index out of range"));
    }

    if((args[1].u_int < 0) || (args[1].u_int > 9)) {
        mp_raise_ValueError(translate("differentiation order out of range"));
    }
    uint8_t N = (uint8_t)args[1].u_int;
    uint8_t index = ULAB_MAX_DIMS - ndarray->ndim + ax;
    if(N > ndarray->shape[index]) {
        mp_raise_ValueError(translate("differentiation order out of range"));
    }

    int8_t *stencil = m_new(int8_t, N+1);
    stencil[0] = 1;
    for(uint8_t i = 1; i < N+1; i++) {
        stencil[i] = -stencil[i-1]*(N-i+1)/i;
    }

    size_t *shape = m_new0(size_t, ULAB_MAX_DIMS);
    for(uint8_t i = 0; i < ULAB_MAX_DIMS; i++) {
        shape[i] = ndarray->shape[i];
        if(i == index) {
            shape[i] -= N;
        }
    }
    uint8_t *array = (uint8_t *)ndarray->array;
    ndarray_obj_t *results = ndarray_new_dense_ndarray(ndarray->ndim, shape, ndarray->dtype);
    uint8_t *rarray = (uint8_t *)results->array;

    memset(shape, 0, sizeof(size_t)*ULAB_MAX_DIMS);
    int32_t *strides = m_new0(int32_t, ULAB_MAX_DIMS);
    numerical_reduce_axes(ndarray, ax, shape, strides);

    if(ndarray->dtype == NDARRAY_UINT8) {
        RUN_DIFF(ndarray, uint8_t, array, results, rarray, shape, strides, index, stencil, N);
    } else if(ndarray->dtype == NDARRAY_INT8) {
        RUN_DIFF(ndarray, int8_t, array, results, rarray, shape, strides, index, stencil, N);
    }  else if(ndarray->dtype == NDARRAY_UINT16) {
        RUN_DIFF(ndarray, uint16_t, array, results, rarray, shape, strides, index, stencil, N);
    } else if(ndarray->dtype == NDARRAY_INT16) {
        RUN_DIFF(ndarray, int16_t, array, results, rarray, shape, strides, index, stencil, N);
    } else {
        RUN_DIFF(ndarray, mp_float_t, array, results, rarray, shape, strides, index, stencil, N);
    }
    m_del(int8_t, stencil, N+1);
    m_del(size_t, shape, ULAB_MAX_DIMS);
    m_del(int32_t, strides, ULAB_MAX_DIMS);
    return MP_OBJ_FROM_PTR(results);
}

MP_DEFINE_CONST_FUN_OBJ_KW(numerical_diff_obj, 1, numerical_diff);
#endif

#if ULAB_NUMPY_HAS_FLIP
//| def flip(array: ulab.numpy.ndarray, *, axis: Optional[int] = None) -> ulab.numpy.ndarray:
//|     """Returns a new array that reverses the order of the elements along the
//|        given axis, or along all axes if axis is None."""
//|     ...
//|

mp_obj_t numerical_flip(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_axis, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if(!mp_obj_is_type(args[0].u_obj, &ulab_ndarray_type)) {
        mp_raise_TypeError(translate("flip argument must be an ndarray"));
    }

    ndarray_obj_t *results = NULL;
    ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(args[0].u_obj);
    if(args[1].u_obj == mp_const_none) { // flip the flattened array
        results = ndarray_new_linear_array(ndarray->len, ndarray->dtype);
        ndarray_copy_array(ndarray, results, 0);
        uint8_t *rarray = (uint8_t *)results->array;
        rarray += (results->len - 1) * results->itemsize;
        results->array = rarray;
        results->strides[ULAB_MAX_DIMS - 1] = -results->strides[ULAB_MAX_DIMS - 1];
    } else if(mp_obj_is_int(args[1].u_obj)){
        int8_t ax = tools_get_axis(args[1].u_obj, ndarray->ndim);

        ax = ULAB_MAX_DIMS - ndarray->ndim + ax;
        int32_t offset = (ndarray->shape[ax] - 1) * ndarray->strides[ax];
        results = ndarray_new_view(ndarray, ndarray->ndim, ndarray->shape, ndarray->strides, offset);
        results->strides[ax] = -results->strides[ax];
    } else {
        mp_raise_TypeError(translate("wrong axis index"));
    }
    return results;
}

MP_DEFINE_CONST_FUN_OBJ_KW(numerical_flip_obj, 1, numerical_flip);
#endif

#if ULAB_NUMPY_HAS_MINMAX
//| def max(array: _ArrayLike, *, axis: Optional[int] = None) -> _float:
//|     """Return the maximum element of the 1D array"""
//|     ...
//|

mp_obj_t numerical_max(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return numerical_function(n_args, pos_args, kw_args, NUMERICAL_MAX);
}

MP_DEFINE_CONST_FUN_OBJ_KW(numerical_max_obj, 1, numerical_max);
#endif

#if ULAB_NUMPY_HAS_MEAN
//| def mean(array: _ArrayLike, *, axis: Optional[int] = None) -> _float:
//|     """Return the mean element of the 1D array, as a number if axis is None, otherwise as an array."""
//|     ...
//|

mp_obj_t numerical_mean(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return numerical_function(n_args, pos_args, kw_args, NUMERICAL_MEAN);
}

MP_DEFINE_CONST_FUN_OBJ_KW(numerical_mean_obj, 1, numerical_mean);
#endif

#if ULAB_NUMPY_HAS_MEDIAN
//| def median(array: ulab.numpy.ndarray, *, axis: int = -1) -> ulab.numpy.ndarray:
//|     """Find the median value in an array along the given axis, or along all axes if axis is None."""
//|     ...
//|

mp_obj_t numerical_median(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_axis, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_rom_obj = mp_const_none } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    if(!mp_obj_is_type(args[0].u_obj, &ulab_ndarray_type)) {
        mp_raise_TypeError(translate("median argument must be an ndarray"));
    }

    ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(args[0].u_obj);
    if(ndarray->len == 0) {
        return mp_obj_new_float(MICROPY_FLOAT_C_FUN(nan)(""));
    }

    ndarray = numerical_sort_helper(args[0].u_obj, args[1].u_obj, 0);

    if((args[1].u_obj == mp_const_none) || (ndarray->ndim == 1)) {
        // at this point, the array holding the sorted values should be flat
        uint8_t *array = (uint8_t *)ndarray->array;
        size_t len = ndarray->len;
        array += (len >> 1) * ndarray->itemsize;
        mp_float_t median = ndarray_get_float_value(array, ndarray->dtype);
        if(!(len & 0x01)) { // len is an even number
            array -= ndarray->itemsize;
            median += ndarray_get_float_value(array, ndarray->dtype);
            median *= MICROPY_FLOAT_CONST(0.5);
        }
        return mp_obj_new_float(median);
    } else {
        int8_t ax = tools_get_axis(args[1].u_obj, ndarray->ndim);

        size_t *shape = m_new0(size_t, ULAB_MAX_DIMS);
        int32_t *strides = m_new0(int32_t, ULAB_MAX_DIMS);
        numerical_reduce_axes(ndarray, ax, shape, strides);

        ax = ULAB_MAX_DIMS - ndarray->ndim + ax;
        ndarray_obj_t *results = ndarray_new_dense_ndarray(ndarray->ndim-1, shape, NDARRAY_FLOAT);
        m_del(size_t, shape, ULAB_MAX_DIMS);

        mp_float_t *rarray = (mp_float_t *)results->array;

        uint8_t *array = (uint8_t *)ndarray->array;

        size_t len = ndarray->shape[ax];

        #if ULAB_MAX_DIMS > 3
        size_t i = 0;
        do {
        #endif
            #if ULAB_MAX_DIMS > 2
            size_t j = 0;
            do {
            #endif
                size_t k = 0;
                do {
                    array += ndarray->strides[ax] * (len >> 1);
                    mp_float_t median = ndarray_get_float_value(array, ndarray->dtype);
                    if(!(len & 0x01)) { // len is an even number
                        array -= ndarray->strides[ax];
                        median += ndarray_get_float_value(array, ndarray->dtype);
                        median *= MICROPY_FLOAT_CONST(0.5);
                        array += ndarray->strides[ax];
                    }
                    array -= ndarray->strides[ax] * (len >> 1);
                    array += strides[ULAB_MAX_DIMS - 1];
                    *rarray = median;
                    rarray++;
                    k++;
                } while(k < shape[ULAB_MAX_DIMS - 1]);
            #if ULAB_MAX_DIMS > 2
                array -= strides[ULAB_MAX_DIMS - 1] * shape[ULAB_MAX_DIMS - 1];
                array += strides[ULAB_MAX_DIMS - 2];
                j++;
            } while(j < shape[ULAB_MAX_DIMS - 2]);
            #endif
        #if ULAB_MAX_DIMS > 3
            array -= strides[ULAB_MAX_DIMS - 2] * shape[ULAB_MAX_DIMS-2];
            array += strides[ULAB_MAX_DIMS - 3];
            i++;
        } while(i < shape[ULAB_MAX_DIMS - 3]);
        #endif

        return MP_OBJ_FROM_PTR(results);
    }
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_KW(numerical_median_obj, 1, numerical_median);
#endif

#if ULAB_NUMPY_HAS_MINMAX
//| def min(array: _ArrayLike, *, axis: Optional[int] = None) -> _float:
//|     """Return the minimum element of the 1D array"""
//|     ...
//|

mp_obj_t numerical_min(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return numerical_function(n_args, pos_args, kw_args, NUMERICAL_MIN);
}

MP_DEFINE_CONST_FUN_OBJ_KW(numerical_min_obj, 1, numerical_min);
#endif

#if ULAB_NUMPY_HAS_ROLL
//| def roll(array: ulab.numpy.ndarray, distance: int, *, axis: Optional[int] = None) -> None:
//|     """Shift the content of a vector by the positions given as the second
//|        argument. If the ``axis`` keyword is supplied, the shift is applied to
//|        the given axis.  The array is modified in place."""
//|     ...
//|

mp_obj_t numerical_roll(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none  } },
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_axis, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if(!mp_obj_is_type(args[0].u_obj, &ulab_ndarray_type)) {
        mp_raise_TypeError(translate("roll argument must be an ndarray"));
    }
    ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(args[0].u_obj);
    uint8_t *array = ndarray->array;
    ndarray_obj_t *results = ndarray_new_dense_ndarray(ndarray->ndim, ndarray->shape, ndarray->dtype);

    int32_t shift = mp_obj_get_int(args[1].u_obj);
    int32_t _shift = shift < 0 ? -shift : shift;

    size_t counter;
    uint8_t *rarray = (uint8_t *)results->array;

    if(args[2].u_obj == mp_const_none) { // roll the flattened array
        _shift = _shift % results->len;
        if(shift > 0) { // shift to the right
            rarray += _shift * results->itemsize;
            counter = results->len - _shift;
        } else { // shift to the left
            rarray += (results->len - _shift) * results->itemsize;
            counter = _shift;
        }
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
                        memcpy(rarray, array, ndarray->itemsize);
                        rarray += results->itemsize;
                        array += ndarray->strides[ULAB_MAX_DIMS - 1];
                        l++;
                        if(--counter == 0) {
                            rarray = results->array;
                        }
                    } while(l <  ndarray->shape[ULAB_MAX_DIMS - 1]);
                #if ULAB_MAX_DIMS > 1
                    array -= ndarray->strides[ULAB_MAX_DIMS - 1] * ndarray->shape[ULAB_MAX_DIMS-1];
                    array += ndarray->strides[ULAB_MAX_DIMS - 2];
                    k++;
                } while(k <  ndarray->shape[ULAB_MAX_DIMS - 2]);
                #endif
            #if ULAB_MAX_DIMS > 2
                array -= ndarray->strides[ULAB_MAX_DIMS - 2] * ndarray->shape[ULAB_MAX_DIMS-2];
                array += ndarray->strides[ULAB_MAX_DIMS - 3];
                j++;
            } while(j <  ndarray->shape[ULAB_MAX_DIMS - 3]);
            #endif
        #if ULAB_MAX_DIMS > 3
            array -= ndarray->strides[ULAB_MAX_DIMS - 3] * ndarray->shape[ULAB_MAX_DIMS-3];
            array += ndarray->strides[ULAB_MAX_DIMS - 4];
            i++;
        } while(i <  ndarray->shape[ULAB_MAX_DIMS - 4]);
        #endif
    } else if(mp_obj_is_int(args[2].u_obj)){
        int8_t ax = tools_get_axis(args[2].u_obj, ndarray->ndim);

        size_t *shape = m_new0(size_t, ULAB_MAX_DIMS);
        int32_t *strides = m_new0(int32_t, ULAB_MAX_DIMS);
        numerical_reduce_axes(ndarray, ax, shape, strides);

        size_t *rshape = m_new0(size_t, ULAB_MAX_DIMS);
        int32_t *rstrides = m_new0(int32_t, ULAB_MAX_DIMS);
        numerical_reduce_axes(results, ax, rshape, rstrides);

        ax = ULAB_MAX_DIMS - ndarray->ndim + ax;
        uint8_t *_rarray;
        _shift = _shift % results->shape[ax];

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
                    _rarray = rarray;
                    if(shift < 0) {
                        rarray += (results->shape[ax] - _shift) * results->strides[ax];
                        counter = _shift;
                    } else {
                        rarray += _shift * results->strides[ax];
                        counter = results->shape[ax] - _shift;
                    }
                    do {
                        memcpy(rarray, array, ndarray->itemsize);
                        array += ndarray->strides[ax];
                        rarray += results->strides[ax];
                        if(--counter == 0) {
                            rarray = _rarray;
                        }
                        l++;
                    } while(l < ndarray->shape[ax]);
                #if ULAB_MAX_DIMS > 1
                    rarray = _rarray;
                    rarray += rstrides[ULAB_MAX_DIMS - 1];
                    array -= ndarray->strides[ax] * ndarray->shape[ax];
                    array += strides[ULAB_MAX_DIMS - 1];
                    k++;
                } while(k < shape[ULAB_MAX_DIMS - 1]);
                #endif
            #if ULAB_MAX_DIMS > 2
                rarray -= rstrides[ULAB_MAX_DIMS - 1] * rshape[ULAB_MAX_DIMS-1];
                rarray += rstrides[ULAB_MAX_DIMS - 2];
                array -= strides[ULAB_MAX_DIMS - 1] * shape[ULAB_MAX_DIMS-1];
                array += strides[ULAB_MAX_DIMS - 2];
                j++;
            } while(j < shape[ULAB_MAX_DIMS - 2]);
            #endif
        #if ULAB_MAX_DIMS > 3
            rarray -= rstrides[ULAB_MAX_DIMS - 2] * rshape[ULAB_MAX_DIMS-2];
            rarray += rstrides[ULAB_MAX_DIMS - 3];
            array -= strides[ULAB_MAX_DIMS - 2] * shape[ULAB_MAX_DIMS-2];
            array += strides[ULAB_MAX_DIMS - 3];
            i++;
        } while(i < shape[ULAB_MAX_DIMS - 3]);
        #endif

        m_del(size_t, shape, ULAB_MAX_DIMS);
        m_del(int32_t, strides, ULAB_MAX_DIMS);
        m_del(size_t, rshape, ULAB_MAX_DIMS);
        m_del(int32_t, rstrides, ULAB_MAX_DIMS);

    } else {
        mp_raise_TypeError(translate("wrong axis index"));
    }

    return results;
}

MP_DEFINE_CONST_FUN_OBJ_KW(numerical_roll_obj, 2, numerical_roll);
#endif

#if ULAB_NUMPY_HAS_SORT
//| def sort(array: ulab.numpy.ndarray, *, axis: int = -1) -> ulab.numpy.ndarray:
//|     """Sort the array along the given axis, or along all axes if axis is None.
//|        The array is modified in place."""
//|     ...
//|

mp_obj_t numerical_sort(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_rom_obj = mp_const_none } },
        { MP_QSTR_axis, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_rom_obj = mp_const_none } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    return numerical_sort_helper(args[0].u_obj, args[1].u_obj, 0);
}

MP_DEFINE_CONST_FUN_OBJ_KW(numerical_sort_obj, 1, numerical_sort);
#endif

#if NDARRAY_HAS_SORT
// method of an ndarray
static mp_obj_t numerical_sort_inplace(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_axis, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_int = -1 } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    return numerical_sort_helper(args[0].u_obj, args[1].u_obj, 1);
}

MP_DEFINE_CONST_FUN_OBJ_KW(numerical_sort_inplace_obj, 1, numerical_sort_inplace);
#endif /* NDARRAY_HAS_SORT */

#if ULAB_NUMPY_HAS_STD
//| def std(array: _ArrayLike, *, axis: Optional[int] = None, ddof: int = 0) -> _float:
//|     """Return the standard deviation of the array, as a number if axis is None, otherwise as an array."""
//|     ...
//|

mp_obj_t numerical_std(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } } ,
        { MP_QSTR_axis, MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_ddof, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t oin = args[0].u_obj;
    mp_obj_t axis = args[1].u_obj;
    size_t ddof = args[2].u_int;
    if((axis != mp_const_none) && (mp_obj_get_int(axis) != 0) && (mp_obj_get_int(axis) != 1)) {
        // this seems to pass with False, and True...
        mp_raise_ValueError(translate("axis must be None, or an integer"));
    }
    if(mp_obj_is_type(oin, &mp_type_tuple) || mp_obj_is_type(oin, &mp_type_list) || mp_obj_is_type(oin, &mp_type_range)) {
        return numerical_sum_mean_std_iterable(oin, NUMERICAL_STD, ddof);
    } else if(mp_obj_is_type(oin, &ulab_ndarray_type)) {
        ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(oin);
        return numerical_sum_mean_std_ndarray(ndarray, axis, NUMERICAL_STD, ddof);
    } else {
        mp_raise_TypeError(translate("input must be tuple, list, range, or ndarray"));
    }
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_KW(numerical_std_obj, 1, numerical_std);
#endif

#if ULAB_NUMPY_HAS_SUM
//| def sum(array: _ArrayLike, *, axis: Optional[int] = None) -> Union[_float, int, ulab.numpy.ndarray]:
//|     """Return the sum of the array, as a number if axis is None, otherwise as an array."""
//|     ...
//|

mp_obj_t numerical_sum(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return numerical_function(n_args, pos_args, kw_args, NUMERICAL_SUM);
}

MP_DEFINE_CONST_FUN_OBJ_KW(numerical_sum_obj, 1, numerical_sum);
#endif
