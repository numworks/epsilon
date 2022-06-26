/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2021 Zoltán Vörös
 *               2020 Scott Shawcroft for Adafruit Industries
 *               2020 Roberto Colistete Jr.
 *               2020 Taku Fukada
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
#include "carray/carray_tools.h"
#include "stats.h"

#if ULAB_MAX_DIMS > 1
#if ULAB_NUMPY_HAS_TRACE

//| def trace(m: ulab.numpy.ndarray) -> _float:
//|     """
//|     :param m: a square matrix
//|
//|     Compute the trace of the matrix, the sum of its diagonal elements."""
//|     ...
//|

static mp_obj_t stats_trace(mp_obj_t oin) {
    ndarray_obj_t *ndarray = tools_object_is_square(oin);
    COMPLEX_DTYPE_NOT_IMPLEMENTED(ndarray->dtype)
    mp_float_t trace = 0.0;
    for(size_t i=0; i < ndarray->shape[ULAB_MAX_DIMS - 1]; i++) {
        int32_t pos = i * (ndarray->strides[ULAB_MAX_DIMS - 1] + ndarray->strides[ULAB_MAX_DIMS - 2]);
        trace += ndarray_get_float_index(ndarray->array, ndarray->dtype, pos/ndarray->itemsize);
    }
    if(ndarray->dtype == NDARRAY_FLOAT) {
        return mp_obj_new_float(trace);
    }
    return mp_obj_new_int_from_float(trace);
}

MP_DEFINE_CONST_FUN_OBJ_1(stats_trace_obj, stats_trace);
#endif
#endif
