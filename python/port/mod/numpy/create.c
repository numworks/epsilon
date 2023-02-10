/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
 *               2019-2021 Zoltán Vörös
 *               2020 Taku Fukada
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "py/obj.h"
#include "py/runtime.h"

#include "create.h"
#include "../ulab.h"
#include "../ulab_tools.h"

#if ULAB_NUMPY_HAS_ONES | ULAB_NUMPY_HAS_ZEROS | ULAB_NUMPY_HAS_FULL | ULAB_NUMPY_HAS_EMPTY
static mp_obj_t create_zeros_ones_full(mp_obj_t oshape, uint8_t dtype, mp_obj_t value) {
    if(!mp_obj_is_int(oshape) && !mp_obj_is_type(oshape, &mp_type_tuple) && !mp_obj_is_type(oshape, &mp_type_list)) {
        mp_raise_TypeError(translate("input argument must be an integer, a tuple, or a list"));
    }
    ndarray_obj_t *ndarray = NULL;
    if(mp_obj_is_int(oshape)) {
        size_t n = mp_obj_get_int(oshape);
        ndarray = ndarray_new_linear_array(n, dtype);
    } else if(mp_obj_is_type(oshape, &mp_type_tuple) || mp_obj_is_type(oshape, &mp_type_list)) {
        uint8_t len = (uint8_t)mp_obj_get_int(mp_obj_len_maybe(oshape));
        if(len > ULAB_MAX_DIMS) {
            mp_raise_TypeError(translate("too many dimensions"));
        }
        size_t *shape = m_new0(size_t, ULAB_MAX_DIMS);

        size_t i = 0;
        mp_obj_iter_buf_t iter_buf;
        mp_obj_t item, iterable = mp_getiter(oshape, &iter_buf);
        while((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION){
            shape[ULAB_MAX_DIMS - len + i] = (size_t)mp_obj_get_int(item);
            i++;
        }
        ndarray = ndarray_new_dense_ndarray(len, shape, dtype);
    }
    if(value != mp_const_none) {
        if(dtype == NDARRAY_BOOL) {
            dtype = NDARRAY_UINT8;
            if(mp_obj_is_true(value)) {
                value = mp_obj_new_int(1);
            } else {
                value = mp_obj_new_int(0);
            }
        }
        for(size_t i=0; i < ndarray->len; i++) {
            #if ULAB_SUPPORTS_COMPLEX
            if(dtype == NDARRAY_COMPLEX) {
                ndarray_set_complex_value(ndarray->array, i, value);
            } else {
                ndarray_set_value(dtype, ndarray->array, i, value);
            }
            #else
            ndarray_set_value(dtype, ndarray->array, i, value);
            #endif
        }
    }
    // if zeros calls the function, we don't have to do anything
    return MP_OBJ_FROM_PTR(ndarray);
}
#endif

#if ULAB_NUMPY_HAS_ARANGE | ULAB_NUMPY_HAS_LINSPACE
static ndarray_obj_t *create_linspace_arange(mp_float_t start, mp_float_t step, mp_float_t stop, size_t len, uint8_t dtype) {
    mp_float_t value = start;

    ndarray_obj_t *ndarray = ndarray_new_linear_array(len, dtype);
    if(ndarray->boolean == NDARRAY_BOOLEAN) {
        uint8_t *array = (uint8_t *)ndarray->array;
        for(size_t i=0; i < len; i++, value += step) {
            *array++ = value == MICROPY_FLOAT_CONST(0.0) ? 0 : 1;
        }
    } else if(dtype == NDARRAY_UINT8) {
        ARANGE_LOOP(uint8_t, ndarray, len, step, stop);
    } else if(dtype == NDARRAY_INT8) {
        ARANGE_LOOP(int8_t, ndarray, len, step, stop);
    } else if(dtype == NDARRAY_UINT16) {
        ARANGE_LOOP(uint16_t, ndarray, len, step, stop);
    } else if(dtype == NDARRAY_INT16) {
        ARANGE_LOOP(int16_t, ndarray, len, step, stop);
    } else {
        ARANGE_LOOP(mp_float_t, ndarray, len, step, stop);
    }
    return ndarray;
}
#endif

#if ULAB_NUMPY_HAS_ARANGE
//| @overload
//| def arange(stop: _float, step: _float = 1, *, dtype: _DType = ulab.numpy.float) -> ulab.numpy.ndarray: ...
//| @overload
//| def arange(start: _float, stop: _float, step: _float = 1, *, dtype: _DType = ulab.numpy.float) -> ulab.numpy.ndarray:
//|     """
//|     .. param: start
//|       First value in the array, optional, defaults to 0
//|     .. param: stop
//|       Final value in the array
//|     .. param: step
//|       Difference between consecutive elements, optional, defaults to 1.0
//|     .. param: dtype
//|       Type of values in the array
//|
//|     Return a new 1-D array with elements ranging from ``start`` to ``stop``, with step size ``step``."""
//|     ...
//|

mp_obj_t create_arange(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_NONE } },
        { MP_QSTR_, MP_ARG_OBJ, { .u_rom_obj = MP_ROM_NONE } },
        { MP_QSTR_, MP_ARG_OBJ, { .u_rom_obj = MP_ROM_NONE } },
        { MP_QSTR_dtype, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_NONE } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    uint8_t dtype = NDARRAY_FLOAT;
    mp_float_t start, stop, step;
    if(n_args == 1) {
        start = MICROPY_FLOAT_CONST(0.0);
        stop = mp_obj_get_float(args[0].u_obj);
        step = MICROPY_FLOAT_CONST(1.0);
        if(mp_obj_is_int(args[0].u_obj)) dtype = NDARRAY_INT16;
    } else if(n_args == 2) {
        start = mp_obj_get_float(args[0].u_obj);
        stop = mp_obj_get_float(args[1].u_obj);
        step = MICROPY_FLOAT_CONST(1.0);
        if(mp_obj_is_int(args[0].u_obj) && mp_obj_is_int(args[1].u_obj)) dtype = NDARRAY_INT16;
    } else if(n_args == 3) {
        start = mp_obj_get_float(args[0].u_obj);
        stop = mp_obj_get_float(args[1].u_obj);
        step = mp_obj_get_float(args[2].u_obj);
        if(mp_obj_is_int(args[0].u_obj) && mp_obj_is_int(args[1].u_obj) && mp_obj_is_int(args[2].u_obj)) dtype = NDARRAY_INT16;
    } else {
        mp_raise_TypeError(translate("wrong number of arguments"));
    }
    if((MICROPY_FLOAT_C_FUN(fabs)(stop) > 32768) || (MICROPY_FLOAT_C_FUN(fabs)(start) > 32768) || (MICROPY_FLOAT_C_FUN(fabs)(step) > 32768)) {
        dtype = NDARRAY_FLOAT;
    }
    if(args[3].u_obj != mp_const_none) {
        dtype = (uint8_t)mp_obj_get_int(args[3].u_obj);
    }
    ndarray_obj_t *ndarray;
    if((stop - start)/step < 0) {
        ndarray = ndarray_new_linear_array(0, dtype);
    } else {
        size_t len = (size_t)(MICROPY_FLOAT_C_FUN(ceil)((stop - start) / step));
        stop = start + (len - 1) * step;
        ndarray = create_linspace_arange(start, step, stop, len, dtype);
    }
    return MP_OBJ_FROM_PTR(ndarray);
}

MP_DEFINE_CONST_FUN_OBJ_KW(create_arange_obj, 1, create_arange);
#endif


#if ULAB_NUMPY_HAS_ASARRAY
mp_obj_t create_asarray(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_NONE } },
        { MP_QSTR_dtype, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_NONE } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    uint8_t _dtype;
    #if ULAB_HAS_DTYPE_OBJECT
    if(mp_obj_is_type(args[1].u_obj, &ulab_dtype_type)) {
        dtype_obj_t *dtype = MP_OBJ_TO_PTR(args[1].u_obj);
        _dtype = dtype->dtype;
    } else { // this must be an integer defined as a class constant (ulab.numpy.uint8 etc.)
        if(args[1].u_obj == mp_const_none) {
            _dtype = 0;
        } else {
            _dtype = mp_obj_get_int(args[1].u_obj);
        }
    }
    #else
    if(args[1].u_obj == mp_const_none) {
        _dtype = 0;
    } else {
        _dtype = mp_obj_get_int(args[1].u_obj);
    }
    #endif

    if(ulab_tools_mp_obj_is_scalar(args[0].u_obj)) {
        return args[0].u_obj;
    } else if(mp_obj_is_type(args[0].u_obj, &ulab_ndarray_type)) {
        ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(args[0].u_obj);
        if((_dtype == ndarray->dtype) || (_dtype == 0)) {
            return args[0].u_obj;
        } else {
            return MP_OBJ_FROM_PTR(ndarray_copy_view_convert_type(ndarray, _dtype));
        }
    } else if(ndarray_object_is_array_like(args[0].u_obj)) {
        if(_dtype == 0) {
            _dtype = NDARRAY_FLOAT;
        }
        return MP_OBJ_FROM_PTR(ndarray_from_iterable(args[0].u_obj, _dtype));
    } else {
        mp_raise_TypeError(translate("wrong input type"));
    }
    return mp_const_none; // this should never happen
}

MP_DEFINE_CONST_FUN_OBJ_KW(create_asarray_obj, 1, create_asarray);
#endif

#if ULAB_NUMPY_HAS_CONCATENATE
//| def concatenate(arrays: Tuple[ulab.numpy.ndarray], *, axis: int = 0) -> ulab.numpy.ndarray:
//|     """
//|     .. param: arrays
//|       tuple of ndarrays
//|     .. param: axis
//|       axis along which the arrays will be joined
//|
//|     Join a sequence of arrays along an existing axis."""
//|     ...
//|

mp_obj_t create_concatenate(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_NONE } },
        { MP_QSTR_axis, MP_ARG_KW_ONLY | MP_ARG_INT, { .u_int = 0 } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if(!mp_obj_is_type(args[0].u_obj, &mp_type_tuple)) {
        mp_raise_TypeError(translate("first argument must be a tuple of ndarrays"));
    }
    int8_t axis = (int8_t)args[1].u_int;
    size_t *shape = m_new0(size_t, ULAB_MAX_DIMS);
    mp_obj_tuple_t *ndarrays = MP_OBJ_TO_PTR(args[0].u_obj);

    // first check, whether the arrays are compatible
    ndarray_obj_t *_ndarray = MP_OBJ_TO_PTR(ndarrays->items[0]);
    uint8_t dtype = _ndarray->dtype;
    uint8_t ndim = _ndarray->ndim;
    if(axis < 0) {
        axis += ndim;
    }
    if((axis < 0) || (axis >= ndim)) {
        mp_raise_ValueError(translate("wrong axis specified"));
    }
    // shift axis
    axis = ULAB_MAX_DIMS - ndim + axis;
    for(uint8_t j=0; j < ULAB_MAX_DIMS; j++) {
        shape[j] = _ndarray->shape[j];
    }

    for(uint8_t i=1; i < ndarrays->len; i++) {
        _ndarray = MP_OBJ_TO_PTR(ndarrays->items[i]);
        // check, whether the arrays are compatible
        if((dtype != _ndarray->dtype) || (ndim != _ndarray->ndim)) {
            mp_raise_ValueError(translate("input arrays are not compatible"));
        }
        for(uint8_t j=0; j < ULAB_MAX_DIMS; j++) {
            if(j == axis) {
                shape[j] += _ndarray->shape[j];
            } else {
                if(shape[j] != _ndarray->shape[j]) {
                    mp_raise_ValueError(translate("input arrays are not compatible"));
                }
            }
        }
    }

    ndarray_obj_t *target = ndarray_new_dense_ndarray(ndim, shape, dtype);
    uint8_t *tpos = (uint8_t *)target->array;
    uint8_t *tarray;

    for(uint8_t p=0; p < ndarrays->len; p++) {
        // reset the pointer along the axis
        ndarray_obj_t *source = MP_OBJ_TO_PTR(ndarrays->items[p]);
        uint8_t *sarray = (uint8_t *)source->array;
        tarray = tpos;

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
                        memcpy(tarray, sarray, source->itemsize);
                        tarray += target->strides[ULAB_MAX_DIMS - 1];
                        sarray += source->strides[ULAB_MAX_DIMS - 1];
                        l++;
                    } while(l < source->shape[ULAB_MAX_DIMS - 1]);
                #if ULAB_MAX_DIMS > 1
                    tarray -= target->strides[ULAB_MAX_DIMS - 1] * source->shape[ULAB_MAX_DIMS-1];
                    tarray += target->strides[ULAB_MAX_DIMS - 2];
                    sarray -= source->strides[ULAB_MAX_DIMS - 1] * source->shape[ULAB_MAX_DIMS-1];
                    sarray += source->strides[ULAB_MAX_DIMS - 2];
                    k++;
                } while(k < source->shape[ULAB_MAX_DIMS - 2]);
                #endif
            #if ULAB_MAX_DIMS > 2
                tarray -= target->strides[ULAB_MAX_DIMS - 2] * source->shape[ULAB_MAX_DIMS-2];
                tarray += target->strides[ULAB_MAX_DIMS - 3];
                sarray -= source->strides[ULAB_MAX_DIMS - 2] * source->shape[ULAB_MAX_DIMS-2];
                sarray += source->strides[ULAB_MAX_DIMS - 3];
                j++;
            } while(j < source->shape[ULAB_MAX_DIMS - 3]);
            #endif
        #if ULAB_MAX_DIMS > 3
            tarray -= target->strides[ULAB_MAX_DIMS - 3] * source->shape[ULAB_MAX_DIMS-3];
            tarray += target->strides[ULAB_MAX_DIMS - 4];
            sarray -= source->strides[ULAB_MAX_DIMS - 3] * source->shape[ULAB_MAX_DIMS-3];
            sarray += source->strides[ULAB_MAX_DIMS - 4];
            i++;
        } while(i < source->shape[ULAB_MAX_DIMS - 4]);
        #endif
        if(p < ndarrays->len - 1) {
            tpos += target->strides[axis] * source->shape[axis];
        }
    }
    return MP_OBJ_FROM_PTR(target);
}

MP_DEFINE_CONST_FUN_OBJ_KW(create_concatenate_obj, 1, create_concatenate);
#endif

#if ULAB_MAX_DIMS > 1
#if ULAB_NUMPY_HAS_DIAG
//| def diag(a: ulab.numpy.ndarray, *, k: int = 0) -> ulab.numpy.ndarray:
//|     """
//|     .. param: a
//|       an ndarray
//|     .. param: k
//|       Offset of the diagonal from the main diagonal. Can be positive or negative.
//|
//|     Return specified diagonals."""
//|     ...
//|

mp_obj_t create_diag(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_NONE } },
        { MP_QSTR_k, MP_ARG_KW_ONLY | MP_ARG_INT, { .u_int = 0 } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    ndarray_obj_t *source = ndarray_from_iterable(args[0].u_obj, NDARRAY_FLOAT);
    ndarray_obj_t *target = NULL;

    int32_t k = args[1].u_int;
    size_t k_abs = k >= 0 ? (size_t)k : (size_t)(-k);
    if(source->ndim == 2) { // return the diagonal
        size_t len;
        if(k >= 0) {
           len = (k_abs <= source->shape[ULAB_MAX_DIMS - 1]) ? source->shape[ULAB_MAX_DIMS - 1] - k_abs : 0;
        } else {
           len = (k_abs <= source->shape[ULAB_MAX_DIMS - 2]) ? source->shape[ULAB_MAX_DIMS - 2] - k_abs : 0;
        }
        target = ndarray_new_linear_array(len, source->dtype);

        if(len == 0) {
            return MP_OBJ_FROM_PTR(target);
        }

        uint8_t *sarray = (uint8_t *)source->array;
        uint8_t *tarray = (uint8_t *)target->array;
        if(k >= 0) {
            sarray += source->strides[ULAB_MAX_DIMS - 1] * k;
        } else {
            sarray += source->strides[ULAB_MAX_DIMS - 2] * k_abs;
        }
        for(size_t i=0; i < len; i++) {
            memcpy(tarray, sarray, source->itemsize);
            sarray += (source->strides[ULAB_MAX_DIMS - 1] + source->strides[ULAB_MAX_DIMS - 2]);
            tarray += target->itemsize;
        }
    } else if(source->ndim == 1) { // return a rank-2 tensor with the prescribed diagonal
        size_t len = source->len + k_abs;
        target = ndarray_new_dense_ndarray(2, ndarray_shape_vector(0, 0, len, len), source->dtype);
        uint8_t *sarray = (uint8_t *)source->array;
        uint8_t *tarray = (uint8_t *)target->array;

        if(k < 0) {
            tarray += len * k_abs * target->itemsize;
        } else {
            tarray += k_abs * target->itemsize;
        }
        for(size_t i = 0; i < source->len; i++) {
            memcpy(tarray, sarray, source->itemsize);
            sarray += source->strides[ULAB_MAX_DIMS - 1];
            tarray += (len + 1) * target->itemsize;
        }
    }
    #if ULAB_MAX_DIMS > 2
    else {
        mp_raise_ValueError(translate("input must be 1- or 2-d"));
    }
    #endif

    return MP_OBJ_FROM_PTR(target);
}

MP_DEFINE_CONST_FUN_OBJ_KW(create_diag_obj, 1, create_diag);
#endif /* ULAB_NUMPY_HAS_DIAG */

#if ULAB_NUMPY_HAS_EMPTY
// This function is bound in numpy.c to numpy.zeros(), and is simply an alias for that

//| def empty(shape: Union[int, Tuple[int, ...]], *, dtype: _DType = ulab.numpy.float) -> ulab.numpy.ndarray:
//|    """
//|    .. param: shape
//|       Shape of the array, either an integer (for a 1-D array) or a tuple of 2 integers (for a 2-D array)
//|    .. param: dtype
//|       Type of values in the array
//|
//|    Return a new array of the given shape with all elements set to 0. An alias for numpy.zeros."""
//|    ...
//|
#endif

#if ULAB_NUMPY_HAS_EYE
//| def eye(size: int, *, M: Optional[int] = None, k: int = 0, dtype: _DType = ulab.numpy.float) -> ulab.numpy.ndarray:
//|     """Return a new square array of size, with the diagonal elements set to 1
//|        and the other elements set to 0. If k is given, the diagonal is shifted by the specified amount."""
//|     ...
//|

mp_obj_t create_eye(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_M, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_NONE } },
        { MP_QSTR_k, MP_ARG_KW_ONLY | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_dtype, MP_ARG_KW_ONLY | MP_ARG_INT, { .u_int = NDARRAY_FLOAT } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    size_t n = args[0].u_int, m;
    size_t k = args[2].u_int > 0 ? (size_t)args[2].u_int : (size_t)(-args[2].u_int);
    uint8_t dtype = args[3].u_int;
    if(args[1].u_obj == mp_const_none) {
        m = n;
    } else {
        m = mp_obj_get_int(args[1].u_obj);
    }
    ndarray_obj_t *ndarray = ndarray_new_dense_ndarray(2, ndarray_shape_vector(0, 0, n, m), dtype);
    if(dtype == NDARRAY_BOOL) {
       dtype = NDARRAY_UINT8;
   }
    mp_obj_t one = mp_obj_new_int(1);
    size_t i = 0;
    if((args[2].u_int >= 0)) {
        while(k < m) {
            ndarray_set_value(dtype, ndarray->array, i*m+k, one);
            k++;
            i++;
        }
    } else {
        while(k < n) {
            ndarray_set_value(dtype, ndarray->array, k*m+i, one);
            k++;
            i++;
        }
    }
    return MP_OBJ_FROM_PTR(ndarray);
}

MP_DEFINE_CONST_FUN_OBJ_KW(create_eye_obj, 1, create_eye);
#endif /* ULAB_NUMPY_HAS_EYE */
#endif /* ULAB_MAX_DIMS > 1 */

#if ULAB_NUMPY_HAS_FULL
//| def full(shape: Union[int, Tuple[int, ...]], fill_value: Union[_float, _bool], *, dtype: _DType = ulab.numpy.float) -> ulab.numpy.ndarray:
//|    """
//|    .. param: shape
//|       Shape of the array, either an integer (for a 1-D array) or a tuple of integers (for tensors of higher rank)
//|    .. param: fill_value
//|       scalar, the value with which the array is filled
//|    .. param: dtype
//|       Type of values in the array
//|
//|    Return a new array of the given shape with all elements set to 0."""
//|    ...
//|

mp_obj_t create_full(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = MP_OBJ_NULL } },
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = MP_OBJ_NULL } },
        { MP_QSTR_dtype, MP_ARG_KW_ONLY | MP_ARG_INT, { .u_int = NDARRAY_FLOAT } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    uint8_t dtype = args[2].u_int;

    return create_zeros_ones_full(args[0].u_obj, dtype, args[1].u_obj);
}

MP_DEFINE_CONST_FUN_OBJ_KW(create_full_obj, 0, create_full);
#endif


#if ULAB_NUMPY_HAS_LINSPACE
//| def linspace(
//|     start: _float,
//|     stop: _float,
//|     *,
//|     dtype: _DType = ulab.numpy.float,
//|     num: int = 50,
//|     endpoint: _bool = True,
//|     retstep: _bool = False
//| ) -> ulab.numpy.ndarray:
//|     """
//|     .. param: start
//|       First value in the array
//|     .. param: stop
//|       Final value in the array
//|     .. param int: num
//|       Count of values in the array.
//|     .. param: dtype
//|       Type of values in the array
//|     .. param bool: endpoint
//|       Whether the ``stop`` value is included.  Note that even when
//|       endpoint=True, the exact ``stop`` value may not be included due to the
//|       inaccuracy of floating point arithmetic.
//|      .. param bool: retstep,
//|       If True, return (`samples`, `step`), where `step` is the spacing between samples.
//|
//|     Return a new 1-D array with ``num`` elements ranging from ``start`` to ``stop`` linearly."""
//|     ...
//|

mp_obj_t create_linspace(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_NONE } },
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_NONE } },
        { MP_QSTR_num, MP_ARG_INT, { .u_int = 50 } },
        { MP_QSTR_endpoint, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_TRUE } },
        { MP_QSTR_retstep, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_FALSE } },
        { MP_QSTR_dtype, MP_ARG_KW_ONLY | MP_ARG_INT, { .u_int = NDARRAY_FLOAT } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if(args[2].u_int < 2) {
        mp_raise_ValueError(translate("number of points must be at least 2"));
    }
    size_t len = (size_t)args[2].u_int;
    mp_float_t start, step, stop;

    ndarray_obj_t *ndarray = NULL;

    #if ULAB_SUPPORTS_COMPLEX
    mp_float_t step_real, step_imag;
    bool complex_out = false;

    if(mp_obj_is_type(args[0].u_obj, &mp_type_complex) || mp_obj_is_type(args[1].u_obj, &mp_type_complex)) {
        complex_out = true;
        ndarray = ndarray_new_linear_array(len, NDARRAY_COMPLEX);
        mp_float_t *array = (mp_float_t *)ndarray->array;
        mp_float_t start_real, start_imag;
        mp_float_t stop_real, stop_imag;

        mp_obj_get_complex(args[0].u_obj, &start_real, &start_imag);
        mp_obj_get_complex(args[1].u_obj, &stop_real, &stop_imag);
        if(args[3].u_obj == mp_const_true) {
            step_real = (stop_real - start_real) / (len - 1);
            step_imag = (stop_imag - start_imag) / (len - 1);
        } else {
            step_real = (stop_real - start_real) / len;
            step_imag = (stop_imag - start_imag) / len;
        }

        for(size_t i = 0; i < len; i++) {
            *array++ = start_real;
            *array++ = start_imag;
            start_real += step_real;
            start_imag += step_imag;
        }
    } else {
    #endif
        start = mp_obj_get_float(args[0].u_obj);
        stop = mp_obj_get_float(args[1].u_obj);

        uint8_t typecode = args[5].u_int;

        if(args[3].u_obj == mp_const_true) {
            step = (stop - start) / (len - 1);
        } else {
            step = (stop - start) / len;
            stop = start + step * (len - 1);
        }

        ndarray = create_linspace_arange(start, step, stop, len, typecode);
    #if ULAB_SUPPORTS_COMPLEX
    }
    #endif

    if(args[4].u_obj == mp_const_false) {
        return MP_OBJ_FROM_PTR(ndarray);
    } else {
        mp_obj_t tuple[2];
        tuple[0] = MP_OBJ_FROM_PTR(ndarray);
        #if ULAB_SUPPORTS_COMPLEX
        if(complex_out) {
            tuple[1] = mp_obj_new_complex(step_real, step_imag);
        } else {
            tuple[1] = mp_obj_new_float(step);
        }
        #else /* ULAB_SUPPORTS_COMPLEX */
        tuple[1] = mp_obj_new_float(step);
        #endif

        return mp_obj_new_tuple(2, tuple);
    }
}

MP_DEFINE_CONST_FUN_OBJ_KW(create_linspace_obj, 2, create_linspace);
#endif

#if ULAB_NUMPY_HAS_LOGSPACE
//| def logspace(
//|     start: _float,
//|     stop: _float,
//|     *,
//|     dtype: _DType = ulab.numpy.float,
//|     num: int = 50,
//|     endpoint: _bool = True,
//|     base: _float = 10.0
//| ) -> ulab.numpy.ndarray:
//|     """
//|     .. param: start
//|       First value in the array
//|     .. param: stop
//|       Final value in the array
//|     .. param int: num
//|       Count of values in the array. Defaults to 50.
//|     .. param: base
//|       The base of the log space. The step size between the elements in
//|       ``ln(samples) / ln(base)`` (or ``log_base(samples)``) is uniform. Defaults to 10.0.
//|     .. param: dtype
//|       Type of values in the array
//|     .. param bool: endpoint
//|       Whether the ``stop`` value is included.  Note that even when
//|       endpoint=True, the exact ``stop`` value may not be included due to the
//|       inaccuracy of floating point arithmetic. Defaults to True.
//|
//|     Return a new 1-D array with ``num`` evenly spaced elements on a log scale.
//|     The sequence starts at ``base ** start``, and ends with ``base ** stop``."""
//|     ...
//|

ULAB_DEFINE_FLOAT_CONST(const_ten, MICROPY_FLOAT_CONST(10.0), 0x41200000UL, 0x4024000000000000ULL);

mp_obj_t create_logspace(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_NONE } },
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_NONE } },
        { MP_QSTR_num, MP_ARG_INT, { .u_int = 50 } },
        { MP_QSTR_base, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_rom_obj = ULAB_REFERENCE_FLOAT_CONST(const_ten) } },
        { MP_QSTR_endpoint, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_TRUE } },
        { MP_QSTR_dtype, MP_ARG_KW_ONLY | MP_ARG_INT, { .u_int = NDARRAY_FLOAT } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if(args[2].u_int < 2) {
        mp_raise_ValueError(translate("number of points must be at least 2"));
    }
    size_t len = (size_t)args[2].u_int;
    mp_float_t start, step, quotient;
    start = mp_obj_get_float(args[0].u_obj);
    uint8_t dtype = args[5].u_int;
    mp_float_t base = mp_obj_get_float(args[3].u_obj);
    if(args[4].u_obj == mp_const_true) step = (mp_obj_get_float(args[1].u_obj) - start)/(len - 1);
    else step = (mp_obj_get_float(args[1].u_obj) - start) / len;
    quotient = MICROPY_FLOAT_C_FUN(pow)(base, step);
    ndarray_obj_t *ndarray = ndarray_new_linear_array(len, dtype);

    mp_float_t value = MICROPY_FLOAT_C_FUN(pow)(base, start);
    if(ndarray->dtype == NDARRAY_UINT8) {
        uint8_t *array = (uint8_t *)ndarray->array;
        if(ndarray->boolean) {
            memset(array, 1, len);
        } else {
            for(size_t i=0; i < len; i++, value *= quotient) *array++ = (uint8_t)value;
        }
    } else if(ndarray->dtype == NDARRAY_INT8) {
        int8_t *array = (int8_t *)ndarray->array;
        for(size_t i=0; i < len; i++, value *= quotient) *array++ = (int8_t)value;
    } else if(ndarray->dtype == NDARRAY_UINT16) {
        uint16_t *array = (uint16_t *)ndarray->array;
        for(size_t i=0; i < len; i++, value *= quotient) *array++ = (uint16_t)value;
    } else if(ndarray->dtype == NDARRAY_INT16) {
        int16_t *array = (int16_t *)ndarray->array;
        for(size_t i=0; i < len; i++, value *= quotient) *array++ = (int16_t)value;
    } else {
        mp_float_t *array = (mp_float_t *)ndarray->array;
        for(size_t i=0; i < len; i++, value *= quotient) *array++ = value;
    }
    return MP_OBJ_FROM_PTR(ndarray);
}

MP_DEFINE_CONST_FUN_OBJ_KW(create_logspace_obj, 2, create_logspace);
#endif

#if ULAB_NUMPY_HAS_ONES
//| def ones(shape: Union[int, Tuple[int, ...]], *, dtype: _DType = ulab.numpy.float) -> ulab.numpy.ndarray:
//|    """
//|    .. param: shape
//|       Shape of the array, either an integer (for a 1-D array) or a tuple of 2 integers (for a 2-D array)
//|    .. param: dtype
//|       Type of values in the array
//|
//|    Return a new array of the given shape with all elements set to 1."""
//|    ...
//|

mp_obj_t create_ones(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = MP_OBJ_NULL } },
        { MP_QSTR_dtype, MP_ARG_KW_ONLY | MP_ARG_INT, { .u_int = NDARRAY_FLOAT } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    uint8_t dtype = args[1].u_int;
    mp_obj_t one = mp_obj_new_int(1);
    return create_zeros_ones_full(args[0].u_obj, dtype, one);
}

MP_DEFINE_CONST_FUN_OBJ_KW(create_ones_obj, 0, create_ones);
#endif

#if ULAB_NUMPY_HAS_ZEROS
//| def zeros(shape: Union[int, Tuple[int, ...]], *, dtype: _DType = ulab.numpy.float) -> ulab.numpy.ndarray:
//|    """
//|    .. param: shape
//|       Shape of the array, either an integer (for a 1-D array) or a tuple of 2 integers (for a 2-D array)
//|    .. param: dtype
//|       Type of values in the array
//|
//|    Return a new array of the given shape with all elements set to 0."""
//|    ...
//|

mp_obj_t create_zeros(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = MP_OBJ_NULL } },
        { MP_QSTR_dtype, MP_ARG_KW_ONLY | MP_ARG_INT, { .u_int = NDARRAY_FLOAT } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    uint8_t dtype = args[1].u_int;
    return create_zeros_ones_full(args[0].u_obj, dtype, mp_const_none);
}

MP_DEFINE_CONST_FUN_OBJ_KW(create_zeros_obj, 0, create_zeros);
#endif

#if ULAB_NUMPY_HAS_FROMBUFFER
mp_obj_t create_frombuffer(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_NONE } },
        { MP_QSTR_dtype, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_INT(NDARRAY_FLOAT) } },
        { MP_QSTR_count, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_INT(-1) } },
        { MP_QSTR_offset, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_INT(0) } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    uint8_t dtype = mp_obj_get_int(args[1].u_obj);
    size_t offset = mp_obj_get_int(args[3].u_obj);

    mp_buffer_info_t bufinfo;
    if(mp_get_buffer(args[0].u_obj, &bufinfo, MP_BUFFER_READ)) {
        size_t sz = ulab_binary_get_size(dtype);

        if(bufinfo.len < offset) {
            mp_raise_ValueError(translate("offset must be non-negative and no greater than buffer length"));
        }
        size_t len = (bufinfo.len - offset) / sz;
        if((len * sz) != (bufinfo.len - offset)) {
            mp_raise_ValueError(translate("buffer size must be a multiple of element size"));
        }
        if(mp_obj_get_int(args[2].u_obj) > 0) {
            size_t count = mp_obj_get_int(args[2].u_obj);
            if(len < count) {
                mp_raise_ValueError(translate("buffer is smaller than requested size"));
            } else {
                len = count;
            }
        }
        ndarray_obj_t *ndarray = m_new_obj(ndarray_obj_t);
        ndarray->base.type = &ulab_ndarray_type;
        ndarray->dtype = dtype == NDARRAY_BOOL ? NDARRAY_UINT8 : dtype;
        ndarray->boolean = dtype == NDARRAY_BOOL ? NDARRAY_BOOLEAN : NDARRAY_NUMERIC;
        ndarray->ndim = 1;
        ndarray->len = len;
        ndarray->itemsize = sz;
        ndarray->shape[ULAB_MAX_DIMS - 1] = len;
        ndarray->strides[ULAB_MAX_DIMS - 1] = sz;

        uint8_t *buffer = bufinfo.buf;
        ndarray->array = buffer + offset;
        return MP_OBJ_FROM_PTR(ndarray);
    }
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_KW(create_frombuffer_obj, 1, create_frombuffer);
#endif
