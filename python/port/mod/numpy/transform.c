/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2021 Zoltán Vörös
 *
*/

#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/misc.h"

#include "../ulab.h"
#include "../ulab_tools.h"
#include "carray/carray_tools.h"
#include "numerical.h"
#include "transform.h"

#if ULAB_NUMPY_HAS_COMPRESS
static mp_obj_t transform_compress(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_NONE } },
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_NONE } },
        { MP_QSTR_axis, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_NONE } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t condition = args[0].u_obj;

    if(!mp_obj_is_type(args[1].u_obj, &ulab_ndarray_type)) {
        mp_raise_TypeError(translate("wrong input type"));
    }
    ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(args[1].u_obj);
    uint8_t *array = (uint8_t *)ndarray->array;

    mp_obj_t axis = args[2].u_obj;

    size_t len = MP_OBJ_SMALL_INT_VALUE(mp_obj_len_maybe(condition));
    int8_t ax, shift_ax = 0;

    if(axis != mp_const_none) {
        ax = tools_get_axis(axis, ndarray->ndim);
        shift_ax = ULAB_MAX_DIMS - ndarray->ndim + ax;
    }

    if(((axis == mp_const_none) && (len != ndarray->len)) ||
        ((axis != mp_const_none) && (len != ndarray->shape[shift_ax]))) {
        mp_raise_ValueError(translate("wrong length of condition array"));
    }

    size_t true_count = 0;
    mp_obj_iter_buf_t iter_buf;
    mp_obj_t item, iterable = mp_getiter(condition, &iter_buf);
    while((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
        if(mp_obj_is_true(item)) {
            true_count++;
        }
    }

    iterable = mp_getiter(condition, &iter_buf);

    ndarray_obj_t *result = NULL;

    size_t *shape = m_new(size_t, ULAB_MAX_DIMS);
    memcpy(shape, ndarray->shape, ULAB_MAX_DIMS * sizeof(size_t));

    size_t *rshape = m_new(size_t, ULAB_MAX_DIMS);
    memcpy(rshape, ndarray->shape, ULAB_MAX_DIMS * sizeof(size_t));

    int32_t *strides = m_new(int32_t, ULAB_MAX_DIMS);
    memcpy(strides, ndarray->strides, ULAB_MAX_DIMS * sizeof(int32_t));

    int32_t *rstrides = m_new0(int32_t, ULAB_MAX_DIMS);

    if(axis == mp_const_none) {
        result = ndarray_new_linear_array(true_count, ndarray->dtype);

        rstrides[ULAB_MAX_DIMS - 1] = ndarray->itemsize;
        rshape[ULAB_MAX_DIMS - 1] = 0;
    } else {
        rshape[shift_ax] = true_count;

        result = ndarray_new_dense_ndarray(ndarray->ndim, rshape, ndarray->dtype);

        SWAP(size_t, shape[shift_ax], shape[ULAB_MAX_DIMS - 1]);
        SWAP(size_t, rshape[shift_ax], rshape[ULAB_MAX_DIMS - 1]);
        SWAP(int32_t, strides[shift_ax], strides[ULAB_MAX_DIMS - 1]);

        memcpy(rstrides, result->strides, ULAB_MAX_DIMS * sizeof(int32_t));
        SWAP(int32_t, rstrides[shift_ax], rstrides[ULAB_MAX_DIMS - 1]);
    }

    uint8_t *rarray = (uint8_t *)result->array;

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
                if(axis != mp_const_none) {
                    iterable = mp_getiter(condition, &iter_buf);
                }
                do {
                    item = mp_iternext(iterable);
                    if(mp_obj_is_true(item)) {
                        memcpy(rarray, array, ndarray->itemsize);
                        rarray += rstrides[ULAB_MAX_DIMS - 1];
                    }
                    array += strides[ULAB_MAX_DIMS - 1];
                    l++;
                } while(l < shape[ULAB_MAX_DIMS - 1]);
            #if ULAB_MAX_DIMS > 1
                array -= strides[ULAB_MAX_DIMS - 1] * shape[ULAB_MAX_DIMS - 1];
                array += strides[ULAB_MAX_DIMS - 2];
                rarray -= rstrides[ULAB_MAX_DIMS - 1] * rshape[ULAB_MAX_DIMS - 1];
                rarray += rstrides[ULAB_MAX_DIMS - 2];
                k++;
            } while(k < shape[ULAB_MAX_DIMS - 2]);
            #endif
        #if ULAB_MAX_DIMS > 2
            array -= strides[ULAB_MAX_DIMS - 2] * shape[ULAB_MAX_DIMS - 2];
            array += strides[ULAB_MAX_DIMS - 3];
            rarray -= rstrides[ULAB_MAX_DIMS - 2] * rshape[ULAB_MAX_DIMS - 2];
            rarray += rstrides[ULAB_MAX_DIMS - 3];
            j++;
        } while(j < shape[ULAB_MAX_DIMS - 3]);
        #endif
    #if ULAB_MAX_DIMS > 3
        array -= strides[ULAB_MAX_DIMS - 3] * shape[ULAB_MAX_DIMS - 3];
        array += strides[ULAB_MAX_DIMS - 4];
        rarray -= rstrides[ULAB_MAX_DIMS - 2] * rshape[ULAB_MAX_DIMS - 2];
        rarray += rstrides[ULAB_MAX_DIMS - 3];
        i++;
    } while(i < shape[ULAB_MAX_DIMS - 4]);
    #endif

    m_del(size_t, shape, ULAB_MAX_DIMS);
    m_del(size_t, rshape, ULAB_MAX_DIMS);
    m_del(int32_t, strides, ULAB_MAX_DIMS);
    m_del(int32_t, rstrides, ULAB_MAX_DIMS);

    return MP_OBJ_FROM_PTR(result);
}

MP_DEFINE_CONST_FUN_OBJ_KW(transform_compress_obj, 2, transform_compress);
#endif /* ULAB_NUMPY_HAS_COMPRESS */

#if ULAB_NUMPY_HAS_DELETE
static mp_obj_t transform_delete(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_NONE } },
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_NONE } },
        { MP_QSTR_axis, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_NONE } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if(!mp_obj_is_type(args[0].u_obj, &ulab_ndarray_type)) {
        mp_raise_TypeError(translate("first argument must be an ndarray"));
    }
    ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(args[0].u_obj);
    uint8_t *array = (uint8_t *)ndarray->array;

    mp_obj_t indices = args[1].u_obj;

    mp_obj_t axis = args[2].u_obj;

    int8_t shift_ax;

    size_t axis_len;

    if(axis != mp_const_none) {
        int8_t ax = tools_get_axis(axis, ndarray->ndim);
        shift_ax = ULAB_MAX_DIMS - ndarray->ndim + ax;
        axis_len = ndarray->shape[shift_ax];
    } else {
        axis_len = ndarray->len;
    }

    size_t index_len;
    if(mp_obj_is_int(indices)) {
        index_len = 1;
    } else {
        if(mp_obj_len_maybe(indices) == MP_OBJ_NULL) {
            mp_raise_TypeError(translate("wrong index type"));
        }
        index_len = MP_OBJ_SMALL_INT_VALUE(mp_obj_len_maybe(indices));
    }

    if(index_len > axis_len) {
        mp_raise_ValueError(translate("wrong length of index array"));
    }

    size_t *index_array = m_new(size_t, index_len);

    if(mp_obj_is_int(indices)) {
        ssize_t value = (ssize_t)mp_obj_get_int(indices);
        if(value < 0) {
            value += axis_len;
        }
        if((value < 0) || (value > (ssize_t)axis_len)) {
            mp_raise_ValueError(translate("index is out of bounds"));
        } else {
            *index_array++ = (size_t)value;
        }
    } else {
        mp_obj_iter_buf_t iter_buf;
        mp_obj_t item, iterable = mp_getiter(indices, &iter_buf);
        while((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
            ssize_t value = (ssize_t)mp_obj_get_int(item);
            if(value < 0) {
                value += axis_len;
            }
            if((value < 0) || (value > (ssize_t)axis_len)) {
                mp_raise_ValueError(translate("index is out of bounds"));
            } else {
                *index_array++ = (size_t)value;
            }
        }
    }

    // sort the array, since it is not guaranteed that the input is sorted
    HEAPSORT1(size_t, index_array, 1, index_len);

    size_t *shape = m_new(size_t, ULAB_MAX_DIMS);
    memcpy(shape, ndarray->shape, ULAB_MAX_DIMS * sizeof(size_t));

    size_t *rshape = m_new(size_t, ULAB_MAX_DIMS);
    memcpy(rshape, ndarray->shape, ULAB_MAX_DIMS * sizeof(size_t));

    int32_t *strides = m_new(int32_t, ULAB_MAX_DIMS);
    memcpy(strides, ndarray->strides, ULAB_MAX_DIMS * sizeof(int32_t));

    int32_t *rstrides = m_new0(int32_t, ULAB_MAX_DIMS);

    ndarray_obj_t *result = NULL;

    if(axis == mp_const_none) {
        result = ndarray_new_linear_array(ndarray->len - index_len, ndarray->dtype);
        rstrides[ULAB_MAX_DIMS - 1] = ndarray->itemsize;
        memset(rshape, 0, sizeof(size_t) * ULAB_MAX_DIMS);
    } else {
        rshape[shift_ax] = shape[shift_ax] - index_len;

        result = ndarray_new_dense_ndarray(ndarray->ndim, rshape, ndarray->dtype);

        SWAP(size_t, shape[shift_ax], shape[ULAB_MAX_DIMS - 1]);
        SWAP(size_t, rshape[shift_ax], rshape[ULAB_MAX_DIMS - 1]);
        SWAP(int32_t, strides[shift_ax], strides[ULAB_MAX_DIMS - 1]);

        memcpy(rstrides, result->strides, ULAB_MAX_DIMS * sizeof(int32_t));
        SWAP(int32_t, rstrides[shift_ax], rstrides[ULAB_MAX_DIMS - 1]);
    }

    uint8_t *rarray = (uint8_t *)result->array;
    index_array -= index_len;
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
                    if(count == *index_array) {
                        index_array++;
                    } else {
                        memcpy(rarray, array, ndarray->itemsize);
                        rarray += rstrides[ULAB_MAX_DIMS - 1];
                    }
                    array += strides[ULAB_MAX_DIMS - 1];
                    l++;
                    count++;
                } while(l < shape[ULAB_MAX_DIMS - 1]);
                if(axis != mp_const_none) {
                    index_array -= index_len;
                    count = 0;
                }
            #if ULAB_MAX_DIMS > 1
                array -= strides[ULAB_MAX_DIMS - 1] * shape[ULAB_MAX_DIMS - 1];
                array += strides[ULAB_MAX_DIMS - 2];
                rarray -= rstrides[ULAB_MAX_DIMS - 1] * rshape[ULAB_MAX_DIMS - 1];
                rarray += rstrides[ULAB_MAX_DIMS - 2];
                k++;
            } while(k < shape[ULAB_MAX_DIMS - 2]);
            #endif
        #if ULAB_MAX_DIMS > 2
            array -= strides[ULAB_MAX_DIMS - 2] * shape[ULAB_MAX_DIMS - 2];
            array += strides[ULAB_MAX_DIMS - 3];
            rarray -= rstrides[ULAB_MAX_DIMS - 2] * rshape[ULAB_MAX_DIMS - 2];
            rarray += rstrides[ULAB_MAX_DIMS - 3];
            j++;
        } while(j < shape[ULAB_MAX_DIMS - 3]);
        #endif
    #if ULAB_MAX_DIMS > 3
        array -= strides[ULAB_MAX_DIMS - 3] * shape[ULAB_MAX_DIMS - 3];
        array += strides[ULAB_MAX_DIMS - 4];
        rarray -= rstrides[ULAB_MAX_DIMS - 3] * rshape[ULAB_MAX_DIMS - 3];
        rarray += rstrides[ULAB_MAX_DIMS - 4];
        i++;
    } while(i < shape[ULAB_MAX_DIMS - 4]);
    #endif

    // TODO: deleting shape generates a seg fault
    // m_del(size_t, shape, ULAB_MAX_DIMS);
    m_del(size_t, rshape, ULAB_MAX_DIMS);
    m_del(int32_t, strides, ULAB_MAX_DIMS);
    m_del(int32_t, rstrides, ULAB_MAX_DIMS);

    return MP_OBJ_FROM_PTR(result);
}

MP_DEFINE_CONST_FUN_OBJ_KW(transform_delete_obj, 2, transform_delete);
#endif /* ULAB_NUMPY_HAS_DELETE */


#if ULAB_MAX_DIMS > 1
#if ULAB_NUMPY_HAS_DOT
//| def dot(m1: ulab.numpy.ndarray, m2: ulab.numpy.ndarray) -> Union[ulab.numpy.ndarray, _float]:
//|    """
//|    :param ~ulab.numpy.ndarray m1: a matrix, or a vector
//|    :param ~ulab.numpy.ndarray m2: a matrix, or a vector
//|
//|    Computes the product of two matrices, or two vectors. In the letter case, the inner product is returned."""
//|    ...
//|

mp_obj_t transform_dot(mp_obj_t _m1, mp_obj_t _m2) {
    // TODO: should the results be upcast?
    // This implements 2D operations only!
    if(!mp_obj_is_type(_m1, &ulab_ndarray_type) || !mp_obj_is_type(_m2, &ulab_ndarray_type)) {
        mp_raise_TypeError(translate("arguments must be ndarrays"));
    }
    ndarray_obj_t *m1 = MP_OBJ_TO_PTR(_m1);
    ndarray_obj_t *m2 = MP_OBJ_TO_PTR(_m2);
    COMPLEX_DTYPE_NOT_IMPLEMENTED(m1->dtype)
    COMPLEX_DTYPE_NOT_IMPLEMENTED(m2->dtype)

    uint8_t *array1 = (uint8_t *)m1->array;
    uint8_t *array2 = (uint8_t *)m2->array;

    mp_float_t (*func1)(void *) = ndarray_get_float_function(m1->dtype);
    mp_float_t (*func2)(void *) = ndarray_get_float_function(m2->dtype);

    if(m1->shape[ULAB_MAX_DIMS - 1] != m2->shape[ULAB_MAX_DIMS - m2->ndim]) {
        mp_raise_ValueError(translate("dimensions do not match"));
    }
    uint8_t ndim = MIN(m1->ndim, m2->ndim);
    size_t shape1 = m1->ndim == 2 ? m1->shape[ULAB_MAX_DIMS - m1->ndim] : 1;
    size_t shape2 = m2->ndim == 2 ? m2->shape[ULAB_MAX_DIMS - 1] : 1;

    size_t *shape = NULL;
    if(ndim == 2) { // matrix times matrix -> matrix
        shape = ndarray_shape_vector(0, 0, shape1, shape2);
    } else { // matrix times vector -> vector, vector times vector -> vector (size 1)
        shape = ndarray_shape_vector(0, 0, 0, shape1);
    }
    ndarray_obj_t *results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
    mp_float_t *rarray = (mp_float_t *)results->array;

    for(size_t i=0; i < shape1; i++) { // rows of m1
        for(size_t j=0; j < shape2; j++) { // columns of m2
            mp_float_t dot = 0.0;
            for(size_t k=0; k < m1->shape[ULAB_MAX_DIMS - 1]; k++) {
                // (i, k) * (k, j)
                dot += func1(array1) * func2(array2);
                array1 += m1->strides[ULAB_MAX_DIMS - 1];
                array2 += m2->strides[ULAB_MAX_DIMS - m2->ndim];
            }
            *rarray++ = dot;
            array1 -= m1->strides[ULAB_MAX_DIMS - 1] * m1->shape[ULAB_MAX_DIMS - 1];
            array2 -= m2->strides[ULAB_MAX_DIMS - m2->ndim] * m2->shape[ULAB_MAX_DIMS - m2->ndim];
            array2 += m2->strides[ULAB_MAX_DIMS - 1];
        }
        array1 += m1->strides[ULAB_MAX_DIMS - m1->ndim];
        array2 = m2->array;
    }
    if((m1->ndim * m2->ndim) == 1) { // return a scalar, if product of two vectors
        return mp_obj_new_float(*(--rarray));
    } else {
        return MP_OBJ_FROM_PTR(results);
    }
}

MP_DEFINE_CONST_FUN_OBJ_2(transform_dot_obj, transform_dot);
#endif /* ULAB_NUMPY_HAS_DOT */
#endif /* ULAB_MAX_DIMS > 1 */

#if ULAB_NUMPY_HAS_SIZE
static mp_obj_t transform_size(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_NONE } },
        { MP_QSTR_axis, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_NONE } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if(ulab_tools_mp_obj_is_scalar(args[0].u_obj)) {
        return mp_obj_new_int(1);
    }

    if(!ndarray_object_is_array_like(args[0].u_obj)) {
        mp_raise_TypeError(translate("first argument must be an ndarray"));
    }
    if(!mp_obj_is_type(args[0].u_obj, &ulab_ndarray_type)) {
        return mp_obj_len_maybe(args[0].u_obj);
    }

    // at this point, the args[0] is most certainly an ndarray
    ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(args[0].u_obj);
    mp_obj_t axis = args[1].u_obj;
    size_t len;
    if(axis != mp_const_none) {
        int8_t ax = tools_get_axis(axis, ndarray->ndim);
        len = ndarray->shape[ULAB_MAX_DIMS - ndarray->ndim + ax];
    } else {
        len = ndarray->len;
    }

    return mp_obj_new_int(len);
}
MP_DEFINE_CONST_FUN_OBJ_KW(transform_size_obj, 1, transform_size);
#endif
