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

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/misc.h"

#include "../ulab.h"
#include "../ulab_tools.h"
#include "transform.h"

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
#endif
#endif