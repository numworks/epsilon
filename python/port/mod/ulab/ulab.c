
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2021 Zoltán Vörös
 *               2020 Jeff Epler for Adafruit Industries
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "py/runtime.h"
#include "py/binary.h"
#include "py/obj.h"
#include "py/objarray.h"

#include "ulab.h"
#include "ulab_create.h"
#include "ndarray.h"
#include "ndarray_properties.h"
#include "numpy/ndarray/ndarray_iter.h"

#include "numpy/numpy.h"
#include "scipy/scipy.h"
// TODO: we should get rid of this; array.sort depends on it
#include "numpy/numerical.h"

#include "user/user.h"
#include "utils/utils.h"

#define ULAB_VERSION 3.3.8
#define xstr(s) str(s)
#define str(s) #s
#define ULAB_VERSION_STRING xstr(ULAB_VERSION) xstr(-) xstr(ULAB_MAX_DIMS) xstr(D)

STATIC MP_DEFINE_STR_OBJ(ulab_version_obj, ULAB_VERSION_STRING);


STATIC const mp_rom_map_elem_t ulab_ndarray_locals_dict_table[] = {
    #if ULAB_MAX_DIMS > 1
        #if NDARRAY_HAS_RESHAPE
            { MP_ROM_QSTR(MP_QSTR_reshape), MP_ROM_PTR(&ndarray_reshape_obj) },
        #endif
        #if NDARRAY_HAS_TRANSPOSE
            { MP_ROM_QSTR(MP_QSTR_transpose), MP_ROM_PTR(&ndarray_transpose_obj) },
        #endif
    #endif
    #if NDARRAY_HAS_BYTESWAP
        { MP_ROM_QSTR(MP_QSTR_byteswap), MP_ROM_PTR(&ndarray_byteswap_obj) },
    #endif
    #if NDARRAY_HAS_COPY
        { MP_ROM_QSTR(MP_QSTR_copy), MP_ROM_PTR(&ndarray_copy_obj) },
    #endif
    #if NDARRAY_HAS_FLATTEN
        { MP_ROM_QSTR(MP_QSTR_flatten), MP_ROM_PTR(&ndarray_flatten_obj) },
    #endif
    #if NDARRAY_HAS_TOBYTES
        { MP_ROM_QSTR(MP_QSTR_tobytes), MP_ROM_PTR(&ndarray_tobytes_obj) },
    #endif
    #if NDARRAY_HAS_SORT
        { MP_ROM_QSTR(MP_QSTR_sort), MP_ROM_PTR(&numerical_sort_inplace_obj) },
    #endif
    #ifdef CIRCUITPY
        #if NDARRAY_HAS_DTYPE
            { MP_ROM_QSTR(MP_QSTR_dtype), MP_ROM_PTR(&ndarray_dtype_obj) },
        #endif
        #if NDARRAY_HAS_FLATITER
            { MP_ROM_QSTR(MP_QSTR_flat), MP_ROM_PTR(&ndarray_flat_obj) },
        #endif
        #if NDARRAY_HAS_ITEMSIZE
            { MP_ROM_QSTR(MP_QSTR_itemsize), MP_ROM_PTR(&ndarray_itemsize_obj) },
        #endif
        #if NDARRAY_HAS_SHAPE
            { MP_ROM_QSTR(MP_QSTR_shape), MP_ROM_PTR(&ndarray_shape_obj) },
        #endif
        #if NDARRAY_HAS_SIZE
            { MP_ROM_QSTR(MP_QSTR_size), MP_ROM_PTR(&ndarray_size_obj) },
        #endif
        #if NDARRAY_HAS_STRIDES
            { MP_ROM_QSTR(MP_QSTR_strides), MP_ROM_PTR(&ndarray_strides_obj) },
        #endif
    #endif /* CIRCUITPY */
};

STATIC MP_DEFINE_CONST_DICT(ulab_ndarray_locals_dict, ulab_ndarray_locals_dict_table);

const mp_obj_type_t ulab_ndarray_type = {
    { &mp_type_type },
    .flags = MP_TYPE_FLAG_EXTENDED
    #if defined(MP_TYPE_FLAG_EQ_CHECKS_OTHER_TYPE) && defined(MP_TYPE_FLAG_EQ_HAS_NEQ_TEST)
        | MP_TYPE_FLAG_EQ_CHECKS_OTHER_TYPE | MP_TYPE_FLAG_EQ_HAS_NEQ_TEST,
    #endif
    .name = MP_QSTR_ndarray,
    .print = ndarray_print,
    .make_new = ndarray_make_new,
    .locals_dict = (mp_obj_dict_t*)&ulab_ndarray_locals_dict,
    MP_TYPE_EXTENDED_FIELDS(
    #if NDARRAY_IS_SLICEABLE
    .subscr = ndarray_subscr,
    #endif
    #if NDARRAY_IS_ITERABLE
    .getiter = ndarray_getiter,
    #endif
    #if NDARRAY_HAS_UNARY_OPS
    .unary_op = ndarray_unary_op,
    #endif
    #if NDARRAY_HAS_BINARY_OPS
    .binary_op = ndarray_binary_op,
    #endif
    #ifndef CIRCUITPY
    .attr = ndarray_properties_attr,
    #endif
    .buffer_p = { .get_buffer = ndarray_get_buffer, },
    )
};

#if ULAB_HAS_DTYPE_OBJECT
const mp_obj_type_t ulab_dtype_type = {
    { &mp_type_type },
    .name = MP_QSTR_dtype,
    .print = ndarray_dtype_print,
    .make_new = ndarray_dtype_make_new,
};
#endif

#if NDARRAY_HAS_FLATITER
const mp_obj_type_t ndarray_flatiter_type = {
    { &mp_type_type },
    .name = MP_QSTR_flatiter,
    MP_TYPE_EXTENDED_FIELDS(
    .getiter = ndarray_get_flatiterator,
    )
};
#endif

STATIC const mp_map_elem_t ulab_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_ulab) },
    { MP_ROM_QSTR(MP_QSTR___version__), MP_ROM_PTR(&ulab_version_obj) },
    #if ULAB_HAS_DTYPE_OBJECT
        { MP_OBJ_NEW_QSTR(MP_QSTR_dtype), (mp_obj_t)&ulab_dtype_type },
    #else
        #if NDARRAY_HAS_DTYPE
        { MP_OBJ_NEW_QSTR(MP_QSTR_dtype), (mp_obj_t)&ndarray_dtype_obj },
        #endif /* NDARRAY_HAS_DTYPE */
    #endif /* ULAB_HAS_DTYPE_OBJECT */
        { MP_ROM_QSTR(MP_QSTR_numpy), MP_ROM_PTR(&ulab_numpy_module) },
    #if ULAB_HAS_SCIPY
        { MP_ROM_QSTR(MP_QSTR_scipy), MP_ROM_PTR(&ulab_scipy_module) },
    #endif
    #if ULAB_HAS_USER_MODULE
        { MP_ROM_QSTR(MP_QSTR_user), MP_ROM_PTR(&ulab_user_module) },
    #endif
    #if ULAB_HAS_UTILS_MODULE
        { MP_ROM_QSTR(MP_QSTR_utils), MP_ROM_PTR(&ulab_utils_module) },
    #endif
};

STATIC MP_DEFINE_CONST_DICT (
    mp_module_ulab_globals,
    ulab_globals_table
);

#ifdef OPENMV
const struct _mp_obj_module_t ulab_user_cmodule = {
#else
const mp_obj_module_t ulab_user_cmodule = {
#endif
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_globals,
};

MP_REGISTER_MODULE(MP_QSTR_ulab, ulab_user_cmodule, MODULE_ULAB_ENABLED);
