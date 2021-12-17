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

#include "ulab.h"
#include "ulab_create.h"

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
        size_t *shape = m_new(size_t, ULAB_MAX_DIMS);
        memset(shape, 0, ULAB_MAX_DIMS * sizeof(size_t));
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
            ndarray_set_value(dtype, ndarray->array, i, value);
        }
    }
    // if zeros calls the function, we don't have to do anything
    return MP_OBJ_FROM_PTR(ndarray);
}
#endif

#if ULAB_NUMPY_HAS_ARANGE | ULAB_NUMPY_HAS_LINSPACE
static ndarray_obj_t *create_linspace_arange(mp_float_t start, mp_float_t step, size_t len, uint8_t dtype) {
    mp_float_t value = start;

    ndarray_obj_t *ndarray = ndarray_new_linear_array(len, dtype);
    if(ndarray->boolean == NDARRAY_BOOLEAN) {
        uint8_t *array = (uint8_t *)ndarray->array;
        for(size_t i=0; i < len; i++, value += step) {
            *array++ = value == MICROPY_FLOAT_CONST(0.0) ? 0 : 1;
        }
    } else if(dtype == NDARRAY_UINT8) {
        ARANGE_LOOP(uint8_t, ndarray, len, step);
    } else if(dtype == NDARRAY_INT8) {
        ARANGE_LOOP(int8_t, ndarray, len, step);
    } else if(dtype == NDARRAY_UINT16) {
        ARANGE_LOOP(uint16_t, ndarray, len, step);
    } else if(dtype == NDARRAY_INT16) {
        ARANGE_LOOP(int16_t, ndarray, len, step);
    } else {
        ARANGE_LOOP(mp_float_t, ndarray, len, step);
    }
    return ndarray;
}
#endif

#if ULAB_NUMPY_HAS_ARANGE
mp_obj_t create_arange(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_rom_obj = mp_const_none } },
        { MP_QSTR_, MP_ARG_OBJ, { .u_rom_obj = mp_const_none } },
        { MP_QSTR_, MP_ARG_OBJ, { .u_rom_obj = mp_const_none } },
        { MP_QSTR_dtype, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_rom_obj = mp_const_none } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    uint8_t dtype = NDARRAY_FLOAT;
    mp_float_t start, stop, step;
    if(n_args == 1) {
        start = 0.0;
        stop = mp_obj_get_float(args[0].u_obj);
        step = 1.0;
        if(mp_obj_is_int(args[0].u_obj)) dtype = NDARRAY_INT16;
    } else if(n_args == 2) {
        start = mp_obj_get_float(args[0].u_obj);
        stop = mp_obj_get_float(args[1].u_obj);
        step = 1.0;
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
        size_t len = (size_t)(MICROPY_FLOAT_C_FUN(ceil)((stop - start)/step));
        ndarray = create_linspace_arange(start, step, len, dtype);
    }
    return MP_OBJ_FROM_PTR(ndarray);
}

MP_DEFINE_CONST_FUN_OBJ_KW(create_arange_obj, 1, create_arange);
#endif

#if ULAB_NUMPY_HAS_CONCATENATE
mp_obj_t create_concatenate(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_rom_obj = mp_const_none } },
        { MP_QSTR_axis, MP_ARG_KW_ONLY | MP_ARG_INT, { .u_int = 0 } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if(!mp_obj_is_type(args[0].u_obj, &mp_type_tuple)) {
        mp_raise_TypeError(translate("first argument must be a tuple of ndarrays"));
    }
    int8_t axis = (int8_t)args[1].u_int;
    size_t *shape = m_new(size_t, ULAB_MAX_DIMS);
    memset(shape, 0, sizeof(size_t)*ULAB_MAX_DIMS);
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
mp_obj_t create_diag(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_rom_obj = mp_const_none } },
        { MP_QSTR_k, MP_ARG_KW_ONLY | MP_ARG_INT, { .u_int = 0 } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if(!mp_obj_is_type(args[0].u_obj, &ulab_ndarray_type)) {
        mp_raise_TypeError(translate("input must be an ndarray"));
    }
    ndarray_obj_t *source = MP_OBJ_TO_PTR(args[0].u_obj);
    if(source->ndim == 1) { // return a rank-2 tensor with the prescribed diagonal
        ndarray_obj_t *target = ndarray_new_dense_ndarray(2, ndarray_shape_vector(0, 0, source->len, source->len), source->dtype);
        uint8_t *sarray = (uint8_t *)source->array;
        uint8_t *tarray = (uint8_t *)target->array;
        for(size_t i=0; i < source->len; i++) {
            memcpy(tarray, sarray, source->itemsize);
            sarray += source->strides[ULAB_MAX_DIMS - 1];
            tarray += (source->len + 1) * target->itemsize;
        }
        return MP_OBJ_FROM_PTR(target);
    }
    if(source->ndim > 2) {
        mp_raise_TypeError(translate("input must be a tensor of rank 2"));
    }
    int32_t k = args[1].u_int;
    size_t len = 0;
    uint8_t *sarray = (uint8_t *)source->array;
    if(k < 0) { // move the pointer "vertically"
        if(-k < (int32_t)source->shape[ULAB_MAX_DIMS - 2]) {
            sarray -= k * source->strides[ULAB_MAX_DIMS - 2];
            len = MIN(source->shape[ULAB_MAX_DIMS - 2] + k, source->shape[ULAB_MAX_DIMS - 1]);
        }
    } else { // move the pointer "horizontally"
        if(k < (int32_t)source->shape[ULAB_MAX_DIMS - 1]) {
            sarray += k * source->strides[ULAB_MAX_DIMS - 1];
            len = MIN(source->shape[ULAB_MAX_DIMS - 1] - k, source->shape[ULAB_MAX_DIMS - 2]);
        }
    }

    if(len == 0) {
        mp_raise_ValueError(translate("offset is too large"));
    }

    ndarray_obj_t *target = ndarray_new_linear_array(len, source->dtype);
    uint8_t *tarray = (uint8_t *)target->array;

    for(size_t i=0; i < len; i++) {
        memcpy(tarray, sarray, source->itemsize);
        sarray += source->strides[ULAB_MAX_DIMS - 2];
        sarray += source->strides[ULAB_MAX_DIMS - 1];
        tarray += source->itemsize;
    }
    return MP_OBJ_FROM_PTR(target);
}

MP_DEFINE_CONST_FUN_OBJ_KW(create_diag_obj, 1, create_diag);
#endif /* ULAB_NUMPY_HAS_DIAG */

#if ULAB_NUMPY_HAS_EYE
mp_obj_t create_eye(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_M, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_rom_obj = mp_const_none } },
        { MP_QSTR_k, MP_ARG_KW_ONLY | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_dtype, MP_ARG_KW_ONLY | MP_ARG_INT, { .u_int = NDARRAY_FLOAT } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    size_t n = args[0].u_int, m;
    size_t k = args[2].u_int > 0 ? (size_t)args[2].u_int : (size_t)(-args[2].u_int);
    uint8_t dtype = args[3].u_int;
    if(args[1].u_rom_obj == mp_const_none) {
        m = n;
    } else {
        m = mp_obj_get_int(args[1].u_rom_obj);
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
mp_obj_t create_linspace(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_rom_obj = mp_const_none } },
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_rom_obj = mp_const_none } },
        { MP_QSTR_num, MP_ARG_INT, { .u_int = 50 } },
        { MP_QSTR_endpoint, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_rom_obj = mp_const_true } },
        { MP_QSTR_retstep, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_rom_obj = mp_const_false } },
        { MP_QSTR_dtype, MP_ARG_KW_ONLY | MP_ARG_INT, { .u_int = NDARRAY_FLOAT } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if(args[2].u_int < 2) {
        mp_raise_ValueError(translate("number of points must be at least 2"));
    }
    size_t len = (size_t)args[2].u_int;
    mp_float_t start, step;
    start = mp_obj_get_float(args[0].u_obj);
    uint8_t typecode = args[5].u_int;
    if(args[3].u_obj == mp_const_true) step = (mp_obj_get_float(args[1].u_obj)-start)/(len-1);
    else step = (mp_obj_get_float(args[1].u_obj)-start)/len;
    ndarray_obj_t *ndarray = create_linspace_arange(start, step, len, typecode);
    if(args[4].u_obj == mp_const_false) {
        return MP_OBJ_FROM_PTR(ndarray);
    } else {
        mp_obj_t tuple[2];
        tuple[0] = ndarray;
        tuple[1] = mp_obj_new_float(step);
        return mp_obj_new_tuple(2, tuple);
    }
}

MP_DEFINE_CONST_FUN_OBJ_KW(create_linspace_obj, 2, create_linspace);
#endif

#if ULAB_NUMPY_HAS_LOGSPACE
const mp_obj_float_t create_float_const_ten = {{&mp_type_float}, MICROPY_FLOAT_CONST(10.0)};

mp_obj_t create_logspace(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_rom_obj = mp_const_none } },
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_rom_obj = mp_const_none } },
        { MP_QSTR_num, MP_ARG_INT, { .u_int = 50 } },
        { MP_QSTR_base, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_PTR(&create_float_const_ten) } },
        { MP_QSTR_endpoint, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_rom_obj = mp_const_true } },
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
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_rom_obj = mp_const_none } },
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
        size_t sz = 1;
        if(dtype != NDARRAY_BOOL) { // mp_binary_get_size doesn't work with Booleans
            sz = mp_binary_get_size('@', dtype, NULL);
        }
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
