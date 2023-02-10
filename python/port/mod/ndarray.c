
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2022 Zoltán Vörös
 *               2020 Jeff Epler for Adafruit Industries
 *               2020 Taku Fukada
*/

#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "py/runtime.h"
#include "py/binary.h"
#include "py/obj.h"
#include "py/objtuple.h"
#include "py/objint.h"

#include "ulab_tools.h"
#include "ndarray.h"
#include "ndarray_operators.h"
#include "numpy/carray/carray.h"
#include "numpy/carray/carray_tools.h"

mp_uint_t ndarray_print_threshold = NDARRAY_PRINT_THRESHOLD;
mp_uint_t ndarray_print_edgeitems = NDARRAY_PRINT_EDGEITEMS;

//| """Manipulate numeric data similar to numpy
//|
//| `ulab` is a numpy-like module for micropython, meant to simplify and
//| speed up common mathematical operations on arrays. The primary goal was to
//| implement a small subset of numpy that might be useful in the context of a
//| microcontroller. This means low-level data processing of linear (array) and
//| two-dimensional (matrix) data.
//|
//| `ulab` is adapted from micropython-ulab, and the original project's
//| documentation can be found at
//| https://micropython-ulab.readthedocs.io/en/latest/
//|
//| `ulab` is modeled after numpy, and aims to be a compatible subset where
//| possible.  Numpy's documentation can be found at
//| https://docs.scipy.org/doc/numpy/index.html"""
//|

void ndarray_set_complex_value(void *p, size_t index, mp_obj_t value) {
    mp_float_t real, imag;
    if(mp_obj_is_type(value, &mp_type_complex)) {
        mp_obj_get_complex(value, &real, &imag);
        ((mp_float_t *)p)[2 * index] = real;
        ((mp_float_t *)p)[2 * index + 1] = imag;
    } else {
        real = mp_obj_get_float(value);
        ((mp_float_t *)p)[2 * index] = real;
        ((mp_float_t *)p)[2 * index + 1] = MICROPY_FLOAT_CONST(0.0);
    }
}

#ifdef CIRCUITPY
void ndarray_set_value(char typecode, void *p, size_t index, mp_obj_t val_in) {
    switch (typecode) {
        case NDARRAY_INT8:
            ((signed char *)p)[index] = mp_obj_get_int(val_in);
            break;
        case NDARRAY_UINT8:
            ((unsigned char *)p)[index] = mp_obj_get_int(val_in);
            break;
        case NDARRAY_INT16:
            ((short *)p)[index] = mp_obj_get_int(val_in);
            break;
        case NDARRAY_UINT16:
            ((unsigned short *)p)[index] = mp_obj_get_int(val_in);
            break;
        case NDARRAY_FLOAT:
            ((mp_float_t *)p)[index] = mp_obj_get_float(val_in);
            break;
        #if ULAB_SUPPORTS_COMPLEX
        case NDARRAY_COMPLEX:
            ndarray_set_complex_value(p, index, val_in);
            break;
        #endif
    }
}
#endif

#if defined(MICROPY_VERSION_MAJOR) && MICROPY_VERSION_MAJOR == 1 && MICROPY_VERSION_MINOR == 11

void mp_obj_slice_indices(mp_obj_t self_in, mp_int_t length, mp_bound_slice_t *result) {
    mp_obj_slice_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t start, stop, step;

    if (self->step == mp_const_none) {
        step = 1;
    } else {
        step = mp_obj_get_int(self->step);
        if (step == 0) {
            mp_raise_ValueError(translate("slice step can't be zero"));
        }
    }

    if (step > 0) {
        // Positive step
        if (self->start == mp_const_none) {
            start = 0;
        } else {
            start = mp_obj_get_int(self->start);
            if (start < 0) {
                start += length;
            }
            start = MIN(length, MAX(start, 0));
        }

        if (self->stop == mp_const_none) {
            stop = length;
        } else {
            stop = mp_obj_get_int(self->stop);
            if (stop < 0) {
                stop += length;
            }
            stop = MIN(length, MAX(stop, 0));
        }
    } else {
        // Negative step
        if (self->start == mp_const_none) {
            start = length - 1;
        } else {
            start = mp_obj_get_int(self->start);
            if (start < 0) {
                start += length;
            }
            start = MIN(length - 1, MAX(start, -1));
        }

        if (self->stop == mp_const_none) {
            stop = -1;
        } else {
            stop = mp_obj_get_int(self->stop);
            if (stop < 0) {
                stop += length;
            }
            stop = MIN(length - 1, MAX(stop, -1));
        }
    }

    result->start = start;
    result->stop = stop;
    result->step = step;
}
#endif /* MICROPY_VERSION v1.11 */

void ndarray_fill_array_iterable(mp_float_t *array, mp_obj_t iterable) {
    mp_obj_iter_buf_t x_buf;
    mp_obj_t x_item, x_iterable = mp_getiter(iterable, &x_buf);
    while ((x_item = mp_iternext(x_iterable)) != MP_OBJ_STOP_ITERATION) {
        *array++ = (mp_float_t)mp_obj_get_float(x_item);
    }
}

#if ULAB_HAS_FUNCTION_ITERATOR
size_t *ndarray_new_coords(uint8_t ndim) {
    size_t *coords = m_new0(size_t, ndim);
    return coords;
}

void ndarray_rewind_array(uint8_t ndim, uint8_t *array, size_t *shape, int32_t *strides, size_t *coords) {
    // resets the data pointer of a single array, whenever an axis is full
    // since we always iterate over the very last axis, we have to keep track of
    // the last ndim-2 axes only
    array -= shape[ULAB_MAX_DIMS - 1] * strides[ULAB_MAX_DIMS - 1];
    array += strides[ULAB_MAX_DIMS - 2];
    for(uint8_t i=1; i < ndim-1; i++) {
        coords[ULAB_MAX_DIMS - 1 - i] += 1;
        if(coords[ULAB_MAX_DIMS - 1 - i] == shape[ULAB_MAX_DIMS - 1 - i]) { // we are at a dimension boundary
            array -= shape[ULAB_MAX_DIMS - 1 - i] * strides[ULAB_MAX_DIMS - 1 - i];
            array += strides[ULAB_MAX_DIMS - 2 - i];
            coords[ULAB_MAX_DIMS - 1 - i] = 0;
            coords[ULAB_MAX_DIMS - 2 - i] += 1;
        } else { // coordinates can change only, if the last coordinate changes
            return;
        }
    }
}
#endif

static int32_t *strides_from_shape(size_t *shape, uint8_t dtype) {
    // returns a strides array that corresponds to a dense array with the prescribed shape
    int32_t *strides = m_new(int32_t, ULAB_MAX_DIMS);
    strides[ULAB_MAX_DIMS-1] = (int32_t)ulab_binary_get_size(dtype);
    for(uint8_t i=ULAB_MAX_DIMS; i > 1; i--) {
        strides[i-2] = strides[i-1] * shape[i-1];
    }
    return strides;
}

size_t *ndarray_shape_vector(size_t a, size_t b, size_t c, size_t d) {
    // returns a ULAB_MAX_DIMS-aware array of shapes
    // WARNING: this assumes that the maximum possible dimension is 4!
    size_t *shape = m_new(size_t, ULAB_MAX_DIMS);
    shape[ULAB_MAX_DIMS - 1] = d;
    #if ULAB_MAX_DIMS > 1
    shape[ULAB_MAX_DIMS - 2] = c;
    #endif
    #if ULAB_MAX_DIMS > 2
    shape[ULAB_MAX_DIMS - 3] = b;
    #endif
    #if ULAB_MAX_DIMS > 3
    shape[ULAB_MAX_DIMS - 4] = a;
    #endif
    return shape;
}

bool ndarray_object_is_array_like(mp_obj_t o_in) {
    if(mp_obj_is_type(o_in, &ulab_ndarray_type) ||
      mp_obj_is_type(o_in, &mp_type_tuple) ||
      mp_obj_is_type(o_in, &mp_type_list) ||
      mp_obj_is_type(o_in, &mp_type_range)) {
        return true;
    }
    return false;
}

void fill_array_iterable(mp_float_t *array, mp_obj_t iterable) {
    mp_obj_iter_buf_t x_buf;
    mp_obj_t x_item, x_iterable = mp_getiter(iterable, &x_buf);
    size_t i=0;
    while ((x_item = mp_iternext(x_iterable)) != MP_OBJ_STOP_ITERATION) {
        array[i] = (mp_float_t)mp_obj_get_float(x_item);
        i++;
    }
}

#if NDARRAY_HAS_DTYPE
#if ULAB_HAS_DTYPE_OBJECT
void ndarray_dtype_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    dtype_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_print_str(print, "dtype('");
    if(self->dtype == NDARRAY_BOOLEAN) {
        mp_print_str(print, "bool')");
    } else if(self->dtype == NDARRAY_UINT8) {
        mp_print_str(print, "uint8')");
    } else if(self->dtype == NDARRAY_INT8) {
        mp_print_str(print, "int8')");
    } else if(self->dtype == NDARRAY_UINT16) {
        mp_print_str(print, "uint16')");
    } else if(self->dtype == NDARRAY_INT16) {
        mp_print_str(print, "int16')");
    }
    #if ULAB_SUPPORTS_COMPLEX
    else if(self->dtype == NDARRAY_COMPLEX) {
        mp_print_str(print, "complex')");
    }
    #endif
    else {
        #if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
        mp_print_str(print, "float32')");
        #else
        mp_print_str(print, "float64')");
        #endif
    }
}

mp_obj_t ndarray_dtype_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void) type;
    mp_arg_check_num(n_args, n_kw, 0, 1, true);
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_OBJ, { .u_rom_obj = MP_ROM_NONE } },
    };
    mp_arg_val_t _args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, args, &kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, _args);

    dtype_obj_t *dtype = m_new_obj(dtype_obj_t);
    dtype->base.type = &ulab_dtype_type;

    if(mp_obj_is_type(args[0], &ulab_ndarray_type)) {
        // return the dtype of the array
        ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(args[0]);
        dtype->dtype = ndarray->dtype;
    } else {
        uint8_t _dtype;
        if(mp_obj_is_int(_args[0].u_obj)) {
            _dtype = mp_obj_get_int(_args[0].u_obj);
            if((_dtype != NDARRAY_BOOL) && (_dtype != NDARRAY_UINT8)
                && (_dtype != NDARRAY_INT8) && (_dtype != NDARRAY_UINT16)
                && (_dtype != NDARRAY_INT16) && (_dtype != NDARRAY_FLOAT)) {
                mp_raise_TypeError(translate("data type not understood"));
            }
        } else {
            GET_STR_DATA_LEN(_args[0].u_obj, _dtype_, len);
            if(memcmp(_dtype_, "uint8", 5) == 0) {
                _dtype = NDARRAY_UINT8;
            } else if(memcmp(_dtype_, "int8", 4) == 0) {
                _dtype = NDARRAY_INT8;
            } else if(memcmp(_dtype_, "uint16", 6) == 0) {
                _dtype = NDARRAY_UINT16;
            } else if(memcmp(_dtype_, "int16", 5) == 0) {
                _dtype = NDARRAY_INT16;
            } else if(memcmp(_dtype_, "float", 5) == 0) {
                _dtype = NDARRAY_FLOAT;
            }
            #if ULAB_SUPPORTS_COMPLEX
            else if(memcmp(_dtype_, "complex", 7) == 0) {
                _dtype = NDARRAY_COMPLEX;
            }
            #endif
            else {
                mp_raise_TypeError(translate("data type not understood"));
            }
        }
        dtype->dtype = _dtype;
    }
    return MP_OBJ_FROM_PTR(dtype);
}

mp_obj_t ndarray_dtype(mp_obj_t self_in) {
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
    dtype_obj_t *dtype = m_new_obj(dtype_obj_t);
    dtype->base.type = &ulab_dtype_type;
    dtype->dtype = self->dtype;
    return MP_OBJ_FROM_PTR(dtype);
}

#else
// this is the cheap implementation of tbe dtype
mp_obj_t ndarray_dtype(mp_obj_t self_in) {
    uint8_t dtype;
    if(mp_obj_is_type(self_in, &ulab_ndarray_type)) {
        ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
        dtype = self->dtype;
    } else { // we assume here that the input is a single character
        GET_STR_DATA_LEN(self_in, _dtype, len);
        if((len != 1) || ((*_dtype != NDARRAY_BOOL) && (*_dtype != NDARRAY_UINT8)
            && (*_dtype != NDARRAY_INT8) && (*_dtype != NDARRAY_UINT16)
            && (*_dtype != NDARRAY_INT16) && (*_dtype != NDARRAY_FLOAT)
            #if ULAB_SUPPORTS_COMPLEX
                && (*_dtype != NDARRAY_COMPLEX)
            #endif
        )) {
            mp_raise_TypeError(translate("data type not understood"));
        }
        dtype = *_dtype;
    }
    return mp_obj_new_int(dtype);
}
#endif /* ULAB_HAS_DTYPE_OBJECT */
#endif /* NDARRAY_HAS_DTYPE */

#if ULAB_HAS_PRINTOPTIONS
mp_obj_t ndarray_set_printoptions(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_threshold, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_edgeitems, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    if(args[0].u_obj != mp_const_none) {
        ndarray_print_threshold = mp_obj_get_int(args[0].u_obj);
    }
    if(args[1].u_obj != mp_const_none) {
        ndarray_print_edgeitems = mp_obj_get_int(args[1].u_obj);
    }
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_KW(ndarray_set_printoptions_obj, 0, ndarray_set_printoptions);

mp_obj_t ndarray_get_printoptions(void) {
    mp_obj_t dict = mp_obj_new_dict(2);
    mp_obj_dict_store(MP_OBJ_FROM_PTR(dict), MP_OBJ_NEW_QSTR(MP_QSTR_threshold), mp_obj_new_int(ndarray_print_threshold));
    mp_obj_dict_store(MP_OBJ_FROM_PTR(dict), MP_OBJ_NEW_QSTR(MP_QSTR_edgeitems), mp_obj_new_int(ndarray_print_edgeitems));
    return dict;
}

MP_DEFINE_CONST_FUN_OBJ_0(ndarray_get_printoptions_obj, ndarray_get_printoptions);
#endif

mp_obj_t ndarray_get_item(ndarray_obj_t *ndarray, void *array) {
    // returns a proper micropython object from an array
    if(!ndarray->boolean) {
        #if ULAB_SUPPORTS_COMPLEX
        if(ndarray->dtype == NDARRAY_COMPLEX) {
            mp_float_t *c = (mp_float_t *)array;
            mp_float_t real = *c++;
            mp_float_t imag = *c;
            return mp_obj_new_complex(real, imag);
        }
        #endif
        return mp_binary_get_val_array(ndarray->dtype, array, 0);
    } else {
        if(*(uint8_t *)array) {
            return mp_const_true;
        } else {
            return mp_const_false;
        }
    }
}

static void ndarray_print_element(const mp_print_t *print, ndarray_obj_t *ndarray, uint8_t *array) {
    #if ULAB_SUPPORTS_COMPLEX
        if(ndarray->dtype == NDARRAY_COMPLEX) {
            // real part first
            mp_float_t fvalue = *(mp_float_t *)array;
            mp_obj_print_helper(print, mp_obj_new_float(fvalue), PRINT_REPR);
            // imaginary part
            array += ndarray->itemsize / 2;
            fvalue = *(mp_float_t *)array;
            if(fvalue >= MICROPY_FLOAT_CONST(0.0) || isnan(fvalue)) {
                mp_print_str(print, "+");
            }
            array += ndarray->itemsize / 2;
            mp_obj_print_helper(print, mp_obj_new_float(fvalue), PRINT_REPR);
            mp_print_str(print, "j");
        } else {
            mp_obj_print_helper(print, ndarray_get_item(ndarray, array), PRINT_REPR);
        }
    #else
        mp_obj_print_helper(print, ndarray_get_item(ndarray, array), PRINT_REPR);
    #endif
}

static void ndarray_print_row(const mp_print_t *print, ndarray_obj_t *ndarray, uint8_t *array, int32_t stride, size_t n) {
    if(n == 0) {
        return;
    }
    mp_print_str(print, "[");
    if((n <= ndarray_print_threshold) || (n <= 2*ndarray_print_edgeitems)) { // if the array is short, print everything
        ndarray_print_element(print, ndarray, array);
        array += stride;
        for(size_t i=1; i < n; i++, array += stride) {
            mp_print_str(print, ", ");
            ndarray_print_element(print, ndarray, array);
        }
    } else {
        ndarray_print_element(print, ndarray, array);
        array += stride;
        for(size_t i=1; i < ndarray_print_edgeitems; i++, array += stride) {
            mp_print_str(print, ", ");
            ndarray_print_element(print, ndarray, array);
        }
        mp_printf(print, ", ..., ");
        array += stride * (n - 2 * ndarray_print_edgeitems);
        ndarray_print_element(print, ndarray, array);
        array += stride;
        for(size_t i=1; i < ndarray_print_edgeitems; i++, array += stride) {
            mp_print_str(print, ", ");
            ndarray_print_element(print, ndarray, array);
        }
    }
    mp_print_str(print, "]");
}

#if ULAB_MAX_DIMS > 1
static void ndarray_print_bracket(const mp_print_t *print, const size_t condition, const size_t shape, const char *string) {
    if(condition < shape) {
        mp_print_str(print, string);
    }
}
#endif

void ndarray_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint8_t *array = (uint8_t *)self->array;
    mp_print_str(print, "array(");
    if(self->len == 0) {
        mp_print_str(print, "[]");
        if(self->ndim > 1) {
            mp_print_str(print, ", shape=(");
            #if ULAB_MAX_DIMS > 1
            for(uint8_t ndim = self->ndim; ndim > 1; ndim--) {
                mp_printf(MP_PYTHON_PRINTER, "%d,", self->shape[ULAB_MAX_DIMS - ndim]);
            }
            #else
            mp_printf(MP_PYTHON_PRINTER, "%d,", self->shape[0]);
            #endif
            mp_printf(MP_PYTHON_PRINTER, "%d)", self->shape[ULAB_MAX_DIMS - 1]);
        }
    } else {
        #if ULAB_MAX_DIMS > 3
        size_t i=0;
        ndarray_print_bracket(print, 0, self->shape[ULAB_MAX_DIMS-4], "[");
        do {
        #endif
            #if ULAB_MAX_DIMS > 2
            size_t j = 0;
            ndarray_print_bracket(print, 0, self->shape[ULAB_MAX_DIMS-3], "[");
            do {
            #endif
                #if ULAB_MAX_DIMS > 1
                size_t k = 0;
                ndarray_print_bracket(print, 0, self->shape[ULAB_MAX_DIMS-2], "[");
                do {
                #endif
                    ndarray_print_row(print, self, array, self->strides[ULAB_MAX_DIMS-1], self->shape[ULAB_MAX_DIMS-1]);
                #if ULAB_MAX_DIMS > 1
                    array += self->strides[ULAB_MAX_DIMS-2];
                    k++;
                    ndarray_print_bracket(print, k, self->shape[ULAB_MAX_DIMS-2], ",\n       ");
                } while(k < self->shape[ULAB_MAX_DIMS-2]);
                ndarray_print_bracket(print, 0, self->shape[ULAB_MAX_DIMS-2], "]");
                #endif
            #if ULAB_MAX_DIMS > 2
                j++;
                ndarray_print_bracket(print, j, self->shape[ULAB_MAX_DIMS-3], ",\n\n       ");
                array -= self->strides[ULAB_MAX_DIMS-2] * self->shape[ULAB_MAX_DIMS-2];
                array += self->strides[ULAB_MAX_DIMS-3];
            } while(j < self->shape[ULAB_MAX_DIMS-3]);
            ndarray_print_bracket(print, 0, self->shape[ULAB_MAX_DIMS-3], "]");
            #endif
        #if ULAB_MAX_DIMS > 3
            array -= self->strides[ULAB_MAX_DIMS-3] * self->shape[ULAB_MAX_DIMS-3];
            array += self->strides[ULAB_MAX_DIMS-4];
            i++;
            ndarray_print_bracket(print, i, self->shape[ULAB_MAX_DIMS-4], ",\n\n       ");
        } while(i < self->shape[ULAB_MAX_DIMS-4]);
        ndarray_print_bracket(print, 0, self->shape[ULAB_MAX_DIMS-4], "]");
        #endif
    }
    mp_print_str(print, ", dtype=");
    if(self->boolean) {
        mp_print_str(print, "bool)");
    } else if(self->dtype == NDARRAY_UINT8) {
        mp_print_str(print, "uint8)");
    } else if(self->dtype == NDARRAY_INT8) {
        mp_print_str(print, "int8)");
    } else if(self->dtype == NDARRAY_UINT16) {
        mp_print_str(print, "uint16)");
    } else if(self->dtype == NDARRAY_INT16) {
        mp_print_str(print, "int16)");
    }
    #if ULAB_SUPPORTS_COMPLEX
    else if(self->dtype == NDARRAY_COMPLEX) {
        mp_print_str(print, "complex)");
    }
    #endif /* ULAB_SUPPORTS_COMPLEX */
    else {
        #if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
        mp_print_str(print, "float32)");
        #else
        mp_print_str(print, "float64)");
        #endif
    }
}

void ndarray_assign_elements(ndarray_obj_t *ndarray, mp_obj_t iterable, uint8_t dtype, size_t *idx) {
    // assigns a single row in the tensor
    mp_obj_t item;
    if(ndarray->boolean) {
        uint8_t *array = (uint8_t *)ndarray->array;
        array += *idx;
        while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
            if(mp_obj_is_true(item)) {
                *array = 1;
            }
            array++;
            (*idx)++;
        }
    } else {
        while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
            #if ULAB_SUPPORTS_COMPLEX
                mp_float_t real;
                mp_float_t imag;
                if(dtype == NDARRAY_COMPLEX) {
                    mp_obj_get_complex(item, &real, &imag);
                    ndarray_set_value(NDARRAY_FLOAT, ndarray->array, (*idx)++, mp_obj_new_float(real));
                    ndarray_set_value(NDARRAY_FLOAT, ndarray->array, (*idx)++, mp_obj_new_float(imag));
                } else {
                    ndarray_set_value(dtype, ndarray->array, (*idx)++, item);
                }
            #else
                ndarray_set_value(dtype, ndarray->array, (*idx)++, item);
            #endif
        }
    }
}

bool ndarray_is_dense(ndarray_obj_t *ndarray) {
    // returns true, if the array is dense, false otherwise
    // the array should be dense, if the very first stride can be calculated from shape
    int32_t stride = ndarray->itemsize;
    for(uint8_t i = ULAB_MAX_DIMS - 1; i > ULAB_MAX_DIMS-ndarray->ndim; i--) {
        stride *= ndarray->shape[i];
    }
    return stride == ndarray->strides[ULAB_MAX_DIMS-ndarray->ndim] ? true : false;
}


ndarray_obj_t *ndarray_new_ndarray(uint8_t ndim, size_t *shape, int32_t *strides, uint8_t dtype) {
    // Creates the base ndarray with shape, and initialises the values to straight 0s
    ndarray_obj_t *ndarray = m_new_obj(ndarray_obj_t);
    ndarray->base.type = &ulab_ndarray_type;
    ndarray->dtype = dtype == NDARRAY_BOOL ? NDARRAY_UINT8 : dtype;
    ndarray->boolean = dtype == NDARRAY_BOOL ? NDARRAY_BOOLEAN : NDARRAY_NUMERIC;
    ndarray->ndim = ndim;
    ndarray->len = ndim == 0 ? 0 : 1;
    ndarray->itemsize = ulab_binary_get_size(dtype);
    int32_t *_strides;
    if(strides == NULL) {
        _strides = strides_from_shape(shape, ndarray->dtype);
    } else {
        _strides = strides;
    }
    for(uint8_t i=ULAB_MAX_DIMS; i > ULAB_MAX_DIMS-ndim; i--) {
        ndarray->shape[i-1] = shape[i-1];
        ndarray->strides[i-1] = _strides[i-1];
        ndarray->len *= shape[i-1];
    }

    // if the length is 0, still allocate a single item, so that contractions can be handled
    size_t len = ndarray->itemsize * MAX(1, ndarray->len);
    uint8_t *array = m_new0(byte, len);
    // this should set all elements to 0, irrespective of the of the dtype (all bits are zero)
    // we could, perhaps, leave this step out, and initialise the array only, when needed
    ndarray->array = array;
    ndarray->origin = array;
    return ndarray;
}

ndarray_obj_t *ndarray_new_dense_ndarray(uint8_t ndim, size_t *shape, uint8_t dtype) {
    // creates a dense array, i.e., one, where the strides are derived directly from the shapes
    // the function should work in the general n-dimensional case
    int32_t *strides = m_new(int32_t, ULAB_MAX_DIMS);
    strides[ULAB_MAX_DIMS-1] = (int32_t)ulab_binary_get_size(dtype);
    for(size_t i=ULAB_MAX_DIMS; i > 1; i--) {
        strides[i-2] = strides[i-1] * MAX(1, shape[i-1]);
    }
    return ndarray_new_ndarray(ndim, shape, strides, dtype);
}

ndarray_obj_t *ndarray_new_ndarray_from_tuple(mp_obj_tuple_t *_shape, uint8_t dtype) {
    // creates a dense array from a tuple
    // the function should work in the general n-dimensional case
    size_t *shape = m_new(size_t, ULAB_MAX_DIMS);
    for(size_t i=0; i < ULAB_MAX_DIMS; i++) {
        if(i < ULAB_MAX_DIMS - _shape->len) {
            shape[i] = 0;
        } else {
            shape[i] = mp_obj_get_int(_shape->items[i]);
        }
    }
    return ndarray_new_dense_ndarray(_shape->len, shape, dtype);
}

void ndarray_copy_array(ndarray_obj_t *source, ndarray_obj_t *target, uint8_t shift) {
    // TODO: if the array is dense, the content could be copied in a single pass
    // copies the content of source->array into a new dense void pointer
    // it is assumed that the dtypes in source and target are the same
    // Since the target is a new array, it is supposed to be dense
    uint8_t *sarray = (uint8_t *)source->array;
    uint8_t *tarray = (uint8_t *)target->array;
    #if ULAB_SUPPORTS_COMPLEX
    if(source->dtype == NDARRAY_COMPLEX) {
        sarray += shift;
    }
    #endif

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
                    memcpy(tarray, sarray, target->itemsize);
                    tarray += target->itemsize;
                    sarray += source->strides[ULAB_MAX_DIMS - 1];
                    l++;
                } while(l < source->shape[ULAB_MAX_DIMS - 1]);
            #if ULAB_MAX_DIMS > 1
                sarray -= source->strides[ULAB_MAX_DIMS - 1] * source->shape[ULAB_MAX_DIMS-1];
                sarray += source->strides[ULAB_MAX_DIMS - 2];
                k++;
            } while(k < source->shape[ULAB_MAX_DIMS - 2]);
            #endif
        #if ULAB_MAX_DIMS > 2
            sarray -= source->strides[ULAB_MAX_DIMS - 2] * source->shape[ULAB_MAX_DIMS-2];
            sarray += source->strides[ULAB_MAX_DIMS - 3];
            j++;
        } while(j < source->shape[ULAB_MAX_DIMS - 3]);
        #endif
    #if ULAB_MAX_DIMS > 3
        sarray -= source->strides[ULAB_MAX_DIMS - 3] * source->shape[ULAB_MAX_DIMS-3];
        sarray += source->strides[ULAB_MAX_DIMS - 4];
        i++;
    } while(i < source->shape[ULAB_MAX_DIMS - 4]);
    #endif
}

ndarray_obj_t *ndarray_new_view(ndarray_obj_t *source, uint8_t ndim, size_t *shape, int32_t *strides, int32_t offset) {
    // creates a new view from the input arguments
    ndarray_obj_t *ndarray = m_new_obj(ndarray_obj_t);
    ndarray->base.type = &ulab_ndarray_type;
    ndarray->boolean = source->boolean;
    ndarray->dtype = source->dtype;
    ndarray->ndim = ndim;
    ndarray->itemsize = source->itemsize;
    ndarray->len = ndim == 0 ? 0 : 1;
    for(uint8_t i=ULAB_MAX_DIMS; i > ULAB_MAX_DIMS-ndim; i--) {
        ndarray->shape[i-1] = shape[i-1];
        ndarray->strides[i-1] = strides[i-1];
        ndarray->len *= shape[i-1];
    }
    uint8_t *pointer = (uint8_t *)source->array;
    pointer += offset;
    ndarray->array = pointer;
    ndarray->origin = source->origin;
    return ndarray;
}

ndarray_obj_t *ndarray_copy_view(ndarray_obj_t *source) {
    // creates a one-to-one deep copy of the input ndarray or its view
    // the function should work in the general n-dimensional case
    // In order to make it dtype-agnostic, we copy the memory content
    // instead of reading out the values

    int32_t *strides = strides_from_shape(source->shape, source->dtype);

    uint8_t dtype = source->dtype;
    if(source->boolean) {
        dtype = NDARRAY_BOOL;
    }
    ndarray_obj_t *ndarray = ndarray_new_ndarray(source->ndim, source->shape, strides, dtype);
    ndarray_copy_array(source, ndarray, 0);
    return ndarray;
}

ndarray_obj_t *ndarray_copy_view_convert_type(ndarray_obj_t *source, uint8_t dtype) {
    // creates a copy, similar to ndarray_copy_view, but it also converts the dtype, if necessary
    if(dtype == source->dtype) {
        return ndarray_copy_view(source);
    }
    ndarray_obj_t *ndarray = ndarray_new_dense_ndarray(source->ndim, source->shape, dtype);
    uint8_t *sarray = (uint8_t *)source->array;
    uint8_t *array = (uint8_t *)ndarray->array;

    #if ULAB_SUPPORTS_COMPLEX
    uint8_t complex_size = 2 * sizeof(mp_float_t);
    #endif

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
                    mp_obj_t item;
                    #if ULAB_SUPPORTS_COMPLEX
                    if(source->dtype == NDARRAY_COMPLEX) {
                        if(dtype != NDARRAY_COMPLEX) {
                            mp_raise_TypeError(translate("cannot convert complex type"));
                        } else {
                            memcpy(array, sarray, complex_size);
                        }
                    } else {
                    #endif
                        if((source->dtype == NDARRAY_FLOAT) && (dtype != NDARRAY_FLOAT)) {
                            // floats must be treated separately, because they can't directly be converted to integer types
                            mp_float_t f = ndarray_get_float_value(sarray, source->dtype);
                            item = mp_obj_new_int((int32_t)MICROPY_FLOAT_C_FUN(round)(f));
                        } else {
                            item = mp_binary_get_val_array(source->dtype, sarray, 0);
                        }
                    #if ULAB_SUPPORTS_COMPLEX
                        if(dtype == NDARRAY_COMPLEX) {
                            ndarray_set_value(NDARRAY_FLOAT, array, 0, item);
                        } else {
                            ndarray_set_value(dtype, array, 0, item);
                        }
                    }
                    #else
                    ndarray_set_value(dtype, array, 0, item);
                    #endif
                    array += ndarray->itemsize;
                    sarray += source->strides[ULAB_MAX_DIMS - 1];
                    l++;
                } while(l < source->shape[ULAB_MAX_DIMS - 1]);
            #if ULAB_MAX_DIMS > 1
                sarray -= source->strides[ULAB_MAX_DIMS - 1] * source->shape[ULAB_MAX_DIMS-1];
                sarray += source->strides[ULAB_MAX_DIMS - 2];
                k++;
            } while(k < source->shape[ULAB_MAX_DIMS - 2]);
            #endif
        #if ULAB_MAX_DIMS > 2
            sarray -= source->strides[ULAB_MAX_DIMS - 2] * source->shape[ULAB_MAX_DIMS-2];
            sarray += source->strides[ULAB_MAX_DIMS - 3];
            j++;
        } while(j < source->shape[ULAB_MAX_DIMS - 3]);
        #endif
    #if ULAB_MAX_DIMS > 3
        sarray -= source->strides[ULAB_MAX_DIMS - 3] * source->shape[ULAB_MAX_DIMS-3];
        sarray += source->strides[ULAB_MAX_DIMS - 4];
        i++;
    } while(i < source->shape[ULAB_MAX_DIMS - 4]);
    #endif
    return ndarray;
}

#if NDARRAY_HAS_BYTESWAP
mp_obj_t ndarray_byteswap(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    // changes the endiannes of an array
    // if the dtype of the input uint8/int8/bool, simply return a copy or view
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_NONE } },
        { MP_QSTR_inplace, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_FALSE } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    ndarray_obj_t *self = MP_OBJ_TO_PTR(args[0].u_obj);
    ndarray_obj_t *ndarray = NULL;
    if(args[1].u_obj == mp_const_false) {
        ndarray = ndarray_copy_view(self);
    } else {
        ndarray = ndarray_new_view(self, self->ndim, self->shape, self->strides, 0);
    }
    if((self->dtype == NDARRAY_BOOL) || (self->dtype == NDARRAY_UINT8) || (self->dtype == NDARRAY_INT8)) {
        return MP_OBJ_FROM_PTR(ndarray);
    } else {
        uint8_t *array = (uint8_t *)ndarray->array;
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
                        if(self->dtype == NDARRAY_FLOAT) {
                            #if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
                            SWAP(uint8_t, array[0], array[3]);
                            SWAP(uint8_t, array[1], array[2]);
                            #else
                            SWAP(uint8_t, array[0], array[7]);
                            SWAP(uint8_t, array[1], array[6]);
                            SWAP(uint8_t, array[2], array[5]);
                            SWAP(uint8_t, array[3], array[4]);
                            #endif
                        } else {
                            SWAP(uint8_t, array[0], array[1]);
                        }
                        array += ndarray->strides[ULAB_MAX_DIMS - 1];
                        l++;
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
    }
    return MP_OBJ_FROM_PTR(ndarray);
}

MP_DEFINE_CONST_FUN_OBJ_KW(ndarray_byteswap_obj, 1, ndarray_byteswap);
#endif

#if NDARRAY_HAS_COPY
mp_obj_t ndarray_copy(mp_obj_t self_in) {
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_FROM_PTR(ndarray_copy_view(self));
}

MP_DEFINE_CONST_FUN_OBJ_1(ndarray_copy_obj, ndarray_copy);
#endif

ndarray_obj_t *ndarray_new_linear_array(size_t len, uint8_t dtype) {
    size_t *shape = m_new(size_t, ULAB_MAX_DIMS);
    if(len == 0) {
        return ndarray_new_dense_ndarray(0, shape, dtype);
    }
    shape[ULAB_MAX_DIMS-1] = len;
    return ndarray_new_dense_ndarray(1, shape, dtype);
}

ndarray_obj_t *ndarray_from_iterable(mp_obj_t obj, uint8_t dtype) {
    // returns an ndarray from an iterable micropython object
    // if the input is an ndarray, returns the input...
    if(mp_obj_is_type(obj, &ulab_ndarray_type)) {
        return MP_OBJ_TO_PTR(obj);
    }
    // ... otherwise, takes the values from the iterable, and creates the corresponding ndarray

    // First, we have to figure out, whether the elements of the iterable are iterables themself
    uint8_t ndim = 0;
    size_t shape[ULAB_MAX_DIMS];
    mp_obj_iter_buf_t iter_buf[ULAB_MAX_DIMS];
    mp_obj_t iterable[ULAB_MAX_DIMS];
    // inspect only the very first element in each dimension; this is fast,
    // but not completely safe, e.g., length compatibility is not checked
    mp_obj_t item = obj;

    while(1) {
        if(mp_obj_len_maybe(item) == MP_OBJ_NULL) {
            break;
        }
        if(ndim == ULAB_MAX_DIMS) {
            mp_raise_ValueError(translate("too many dimensions"));
        }
        shape[ndim] = MP_OBJ_SMALL_INT_VALUE(mp_obj_len_maybe(item));
        if(shape[ndim] == 0) {
            ndim++;
            break;
        }
        iterable[ndim] = mp_getiter(item, &iter_buf[ndim]);
        item = mp_iternext(iterable[ndim]);
        ndim++;
    }
    for(uint8_t i = 0; i < ndim; i++) {
        // align all values to the right
        shape[ULAB_MAX_DIMS - i - 1] = shape[ndim - 1 - i];
    }

    ndarray_obj_t *ndarray = ndarray_new_dense_ndarray(ndim, shape, dtype);
    item = obj;
    for(uint8_t i = 0; i < ndim - 1; i++) {
        // if ndim > 1, descend into the hierarchy
        iterable[ULAB_MAX_DIMS - ndim + i] = mp_getiter(item, &iter_buf[ULAB_MAX_DIMS - ndim + i]);
        item = mp_iternext(iterable[ULAB_MAX_DIMS - ndim + i]);
    }

    size_t idx = 0;
    // TODO: this could surely be done in a more elegant way...
    #if ULAB_MAX_DIMS > 3
    do {
    #endif
        #if ULAB_MAX_DIMS > 2
        do {
        #endif
            #if ULAB_MAX_DIMS > 1
            do {
            #endif
                iterable[ULAB_MAX_DIMS - 1] = mp_getiter(item, &iter_buf[ULAB_MAX_DIMS - 1]);
                ndarray_assign_elements(ndarray, iterable[ULAB_MAX_DIMS - 1], ndarray->dtype, &idx);
            #if ULAB_MAX_DIMS > 1
                item = ndim > 1 ? mp_iternext(iterable[ULAB_MAX_DIMS - 2]) : MP_OBJ_STOP_ITERATION;
            } while(item != MP_OBJ_STOP_ITERATION);
            #endif
        #if ULAB_MAX_DIMS > 2
            item = ndim > 2 ? mp_iternext(iterable[ULAB_MAX_DIMS - 3]) : MP_OBJ_STOP_ITERATION;
            if(item != MP_OBJ_STOP_ITERATION) {
                iterable[ULAB_MAX_DIMS - 2] = mp_getiter(item, &iter_buf[ULAB_MAX_DIMS - 2]);
                item = mp_iternext(iterable[ULAB_MAX_DIMS - 2]);
            } else {
                iterable[ULAB_MAX_DIMS - 2] = MP_OBJ_STOP_ITERATION;
            }
        } while(iterable[ULAB_MAX_DIMS - 2] != MP_OBJ_STOP_ITERATION);
        #endif
    #if ULAB_MAX_DIMS > 3
        item = ndim > 3 ? mp_iternext(iterable[ULAB_MAX_DIMS - 4]) : MP_OBJ_STOP_ITERATION;
        if(item != MP_OBJ_STOP_ITERATION) {
            iterable[ULAB_MAX_DIMS - 3] = mp_getiter(item, &iter_buf[ULAB_MAX_DIMS - 3]);
            item = mp_iternext(iterable[ULAB_MAX_DIMS - 3]);
        } else {
            iterable[ULAB_MAX_DIMS - 3] = MP_OBJ_STOP_ITERATION;
        }
    } while(iterable[ULAB_MAX_DIMS - 3] != MP_OBJ_STOP_ITERATION);
    #endif

    return ndarray;
}

STATIC uint8_t ndarray_init_helper(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE } },
        { MP_QSTR_dtype, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_INT(NDARRAY_FLOAT) } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    uint8_t _dtype;
    #if ULAB_HAS_DTYPE_OBJECT
    if(mp_obj_is_type(args[1].u_obj, &ulab_dtype_type)) {
        dtype_obj_t *dtype = MP_OBJ_TO_PTR(args[1].u_obj);
        _dtype = dtype->dtype;
    } else { // this must be an integer defined as a class constant (ulab.numpy.uint8 etc.)
        _dtype = mp_obj_get_int(args[1].u_obj);
    }
    #else
    _dtype = mp_obj_get_int(args[1].u_obj);
    #endif
    return _dtype;
}

STATIC mp_obj_t ndarray_make_new_core(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args, mp_map_t *kw_args) {
    uint8_t dtype = ndarray_init_helper(n_args, args, kw_args);

    if(mp_obj_is_type(args[0], &ulab_ndarray_type)) {
        ndarray_obj_t *source = MP_OBJ_TO_PTR(args[0]);
        return MP_OBJ_FROM_PTR(ndarray_copy_view_convert_type(source, dtype));
    } else {
        // assume that the input is an iterable
        return MP_OBJ_FROM_PTR(ndarray_from_iterable(args[0], dtype));
    }
}

mp_obj_t ndarray_array_constructor(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    // array constructor for ndarray, equivalent to numpy.array(...)
    return ndarray_make_new_core(&ulab_ndarray_type, n_args, kw_args->used, pos_args, kw_args);
}
MP_DEFINE_CONST_FUN_OBJ_KW(ndarray_array_constructor_obj, 1, ndarray_array_constructor);

mp_obj_t ndarray_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void) type;
    mp_arg_check_num(n_args, n_kw, 1, 2, true);
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    return ndarray_make_new_core(type, n_args, n_kw, args, &kw_args);
}

// broadcasting is used at a number of places, always include
bool ndarray_can_broadcast(ndarray_obj_t *lhs, ndarray_obj_t *rhs, uint8_t *ndim, size_t *shape, int32_t *lstrides, int32_t *rstrides) {
    // Returns true or false, depending on, whether the two arrays can be broadcast together
    // with numpy's broadcasting rules. These are as follows:
    //
    // 1. the two shapes are either equal
    // 2. one of the shapes is 1

    lstrides[ULAB_MAX_DIMS - 1] = lhs->strides[ULAB_MAX_DIMS - 1];
    rstrides[ULAB_MAX_DIMS - 1] = rhs->strides[ULAB_MAX_DIMS - 1];
    for(uint8_t i=ULAB_MAX_DIMS; i > 0; i--) {
        if((lhs->shape[i-1] == rhs->shape[i-1]) || (lhs->shape[i-1] == 0) || (lhs->shape[i-1] == 1) ||
        (rhs->shape[i-1] == 0) || (rhs->shape[i-1] == 1)) {
            shape[i-1] = MAX(lhs->shape[i-1], rhs->shape[i-1]);
            if(shape[i-1] > 0) (*ndim)++;
            if(lhs->shape[i-1] < 2) {
                lstrides[i-1] = 0;
            } else {
                lstrides[i-1] = lhs->strides[i-1];
            }
            if(rhs->shape[i-1] < 2) {
                rstrides[i-1] = 0;
            } else {
                rstrides[i-1] = rhs->strides[i-1];
            }
        } else {
            return false;
        }
    }
    return true;
}

#if NDARRAY_HAS_INPLACE_OPS
bool ndarray_can_broadcast_inplace(ndarray_obj_t *lhs, ndarray_obj_t *rhs, int32_t *rstrides) {
    // returns true or false, depending on, whether the two arrays can be broadcast together inplace
    // this means that the right hand side always must be "smaller" than the left hand side, i.e.
    // the broadcasting rules are as follows:
    //
    // 1. the two shapes are either equal
    // 2. the shapes on the right hand side is 1

    rstrides[ULAB_MAX_DIMS - 1] = rhs->strides[ULAB_MAX_DIMS - 1];
    for(uint8_t i=ULAB_MAX_DIMS; i > 0; i--) {
        if((lhs->shape[i-1] == rhs->shape[i-1]) || (rhs->shape[i-1] == 0) || (rhs->shape[i-1] == 1)) {
            if(rhs->shape[i-1] < 2) {
                rstrides[i-1] = 0;
            } else {
                rstrides[i-1] = rhs->strides[i-1];
            }
        } else {
            return false;
        }
    }
    return true;
}
#endif

#if NDARRAY_IS_SLICEABLE
static size_t slice_length(mp_bound_slice_t slice) {
    ssize_t len, correction = 1;
    if(slice.step > 0) correction = -1;
    len = (ssize_t)(slice.stop - slice.start + (slice.step + correction)) / slice.step;
    if(len < 0) return 0;
    return (size_t)len;
}

static mp_bound_slice_t generate_slice(mp_int_t n, mp_obj_t index) {
    mp_bound_slice_t slice;
    if(mp_obj_is_type(index, &mp_type_slice)) {
        mp_obj_slice_indices(index, n, &slice);
    } else if(mp_obj_is_int(index)) {
        mp_int_t _index = mp_obj_get_int(index);
        if(_index < 0) {
            _index += n;
        }
        if((_index >= n) || (_index < 0)) {
            mp_raise_msg(&mp_type_IndexError, translate("index is out of bounds"));
        }
        slice.start = _index;
        slice.stop = _index + 1;
        slice.step = 1;
    } else {
        mp_raise_msg(&mp_type_IndexError, translate("indices must be integers, slices, or Boolean lists"));
    }
    return slice;
}

static ndarray_obj_t *ndarray_view_from_slices(ndarray_obj_t *ndarray, mp_obj_tuple_t *tuple) {
    size_t *shape = m_new0(size_t, ULAB_MAX_DIMS);
    int32_t *strides = m_new0(int32_t, ULAB_MAX_DIMS);

    uint8_t ndim = ndarray->ndim;

    for(uint8_t i=0; i < ndim; i++) {
        // copy from the end
        shape[ULAB_MAX_DIMS - 1 - i] = ndarray->shape[ULAB_MAX_DIMS  - 1 - i];
        strides[ULAB_MAX_DIMS - 1 - i] = ndarray->strides[ULAB_MAX_DIMS  - 1 - i];
    }
    int32_t offset = 0;
    for(uint8_t i=0; i  < tuple->len; i++) {
        if(mp_obj_is_int(tuple->items[i])) {
            // if item is an int, the dimension will first be reduced ...
            ndim--;
            int32_t k = mp_obj_get_int(tuple->items[i]);
            if(k < 0) {
                k += ndarray->shape[ULAB_MAX_DIMS - ndarray->ndim + i];
            }
            if((k >= (int32_t)ndarray->shape[ULAB_MAX_DIMS - ndarray->ndim + i]) || (k < 0)) {
                mp_raise_msg(&mp_type_IndexError, translate("index is out of bounds"));
            }
            offset += ndarray->strides[ULAB_MAX_DIMS - ndarray->ndim + i] * k;
            // ... and then we have to shift the shapes to the right
            for(uint8_t j=0; j < i; j++) {
                shape[ULAB_MAX_DIMS - ndarray->ndim + i - j] = shape[ULAB_MAX_DIMS - ndarray->ndim + i - j - 1];
                strides[ULAB_MAX_DIMS - ndarray->ndim + i - j] = strides[ULAB_MAX_DIMS - ndarray->ndim + i - j - 1];
            }
        } else {
            mp_bound_slice_t slice = generate_slice(shape[ULAB_MAX_DIMS - ndarray->ndim + i], tuple->items[i]);
            shape[ULAB_MAX_DIMS - ndarray->ndim + i] = slice_length(slice);
            offset += ndarray->strides[ULAB_MAX_DIMS - ndarray->ndim + i] * (int32_t)slice.start;
            strides[ULAB_MAX_DIMS - ndarray->ndim + i] = (int32_t)slice.step * ndarray->strides[ULAB_MAX_DIMS - ndarray->ndim + i];
        }
    }
    return ndarray_new_view(ndarray, ndim, shape, strides, offset);
}

void ndarray_assign_view(ndarray_obj_t *view, ndarray_obj_t *values) {
    if(values->len == 0) {
        return;
    }
    uint8_t ndim = 0;
    size_t *shape = m_new0(size_t, ULAB_MAX_DIMS);
    int32_t *lstrides = m_new0(int32_t, ULAB_MAX_DIMS);
    int32_t *rstrides = m_new0(int32_t, ULAB_MAX_DIMS);
    if(!ndarray_can_broadcast(view, values, &ndim, shape, lstrides, rstrides)) {
        mp_raise_ValueError(translate("operands could not be broadcast together"));
    } else {

        ndarray_obj_t *ndarray = ndarray_copy_view_convert_type(values, view->dtype);
        // re-calculate rstrides, since the copy operation might have changed the directions of the strides
        ndarray_can_broadcast(view, ndarray, &ndim, shape, lstrides, rstrides);
        uint8_t *rarray = (uint8_t *)ndarray->array;


        uint8_t *larray = (uint8_t *)view->array;

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
                        memcpy(larray, rarray, view->itemsize);
                        larray += lstrides[ULAB_MAX_DIMS - 1];
                        rarray += rstrides[ULAB_MAX_DIMS - 1];
                        l++;
                    } while(l <  view->shape[ULAB_MAX_DIMS - 1]);
                #if ULAB_MAX_DIMS > 1
                    larray -= lstrides[ULAB_MAX_DIMS - 1] * view->shape[ULAB_MAX_DIMS-1];
                    larray += lstrides[ULAB_MAX_DIMS - 2];
                    rarray -= rstrides[ULAB_MAX_DIMS - 1] * view->shape[ULAB_MAX_DIMS-1];
                    rarray += rstrides[ULAB_MAX_DIMS - 2];
                    k++;
                } while(k <  view->shape[ULAB_MAX_DIMS - 2]);
                #endif
            #if ULAB_MAX_DIMS > 2
                larray -= lstrides[ULAB_MAX_DIMS - 2] * view->shape[ULAB_MAX_DIMS-2];
                larray += lstrides[ULAB_MAX_DIMS - 3];
                rarray -= rstrides[ULAB_MAX_DIMS - 2] * view->shape[ULAB_MAX_DIMS-2];
                rarray += rstrides[ULAB_MAX_DIMS - 3];
                j++;
            } while(j <  view->shape[ULAB_MAX_DIMS - 3]);
            #endif
        #if ULAB_MAX_DIMS > 3
            larray -= lstrides[ULAB_MAX_DIMS - 3] * view->shape[ULAB_MAX_DIMS-3];
            larray += lstrides[ULAB_MAX_DIMS - 4];
            rarray -= rstrides[ULAB_MAX_DIMS - 3] * view->shape[ULAB_MAX_DIMS-3];
            rarray += rstrides[ULAB_MAX_DIMS - 4];
            i++;
        } while(i <  view->shape[ULAB_MAX_DIMS - 4]);
        #endif
    }

    m_del(size_t, shape, ULAB_MAX_DIMS);
    m_del(int32_t, lstrides, ULAB_MAX_DIMS);
    m_del(int32_t, rstrides, ULAB_MAX_DIMS);

    return;
}

static mp_obj_t ndarray_from_boolean_index(ndarray_obj_t *ndarray, ndarray_obj_t *index) {
    // returns a 1D array, indexed by a Boolean array
    if(ndarray->len != index->len) {
        mp_raise_ValueError(translate("array and index length must be equal"));
    }
    uint8_t *iarray = (uint8_t *)index->array;
    // first we have to find out how many trues there are
    size_t count = 0;
    for(size_t i=0; i < index->len; i++) {
        count += *iarray;
        iarray += index->strides[ULAB_MAX_DIMS - 1];
    }
    ndarray_obj_t *results = ndarray_new_linear_array(count, ndarray->dtype);
    uint8_t *rarray = (uint8_t *)results->array;
    uint8_t *array = (uint8_t *)ndarray->array;
    // re-wind the index array
    iarray = index->array;
    for(size_t i=0; i < index->len; i++) {
        if(*iarray) {
            memcpy(rarray, array, results->itemsize);
            rarray += results->itemsize;
            count++;
        }
        array += ndarray->strides[ULAB_MAX_DIMS - 1];
        iarray += index->strides[ULAB_MAX_DIMS - 1];
    }
    return MP_OBJ_FROM_PTR(results);
}

static mp_obj_t ndarray_assign_from_boolean_index(ndarray_obj_t *ndarray, ndarray_obj_t *index, ndarray_obj_t *values) {
    // assigns values to a Boolean-indexed array
    // first we have to find out how many trues there are
    uint8_t *iarray = (uint8_t *)index->array;
    size_t istride = index->strides[ULAB_MAX_DIMS - 1];
    size_t count = 0;
    for(size_t i=0; i < index->len; i++) {
        count += *iarray;
        iarray += istride;
    }
    // re-wind the index array
    iarray = index->array;
    uint8_t *varray = (uint8_t *)values->array;
    size_t vstride;

    if(count == values->len) {
        // there are as many values as true indices
        vstride = values->strides[ULAB_MAX_DIMS - 1];
    } else {
        // there is a single value
        vstride = 0;
    }

    #if ULAB_SUPPORTS_COMPLEX
    if(values->dtype == NDARRAY_COMPLEX) {
        if(ndarray->dtype != NDARRAY_COMPLEX) {
            mp_raise_TypeError(translate("cannot convert complex to dtype"));
        } else {
            uint8_t *array = (uint8_t *)ndarray->array;
            for(size_t i = 0; i < ndarray->len; i++) {
                if(*iarray) {
                    memcpy(array, varray, ndarray->itemsize);
                    varray += vstride;
                }
                array += ndarray->strides[ULAB_MAX_DIMS - 1];
                iarray += istride;
            } while(0);
            return MP_OBJ_FROM_PTR(ndarray);
        }
    }
    #endif

    int32_t lstrides = ndarray->strides[ULAB_MAX_DIMS - 1] / ndarray->itemsize;

    if(ndarray->dtype == NDARRAY_UINT8) {
        if(values->dtype == NDARRAY_UINT8) {
            BOOLEAN_ASSIGNMENT_LOOP(uint8_t, uint8_t, ndarray, lstrides, iarray, istride, varray, vstride);
        } else if(values->dtype == NDARRAY_INT8) {
            BOOLEAN_ASSIGNMENT_LOOP(uint8_t, int8_t, ndarray, lstrides, iarray, istride, varray, vstride);
        } else if(values->dtype == NDARRAY_UINT16) {
            BOOLEAN_ASSIGNMENT_LOOP(uint8_t, uint16_t, ndarray, lstrides, iarray, istride, varray, vstride);
        } else if(values->dtype == NDARRAY_INT16) {
            BOOLEAN_ASSIGNMENT_LOOP(uint8_t, int16_t, ndarray, lstrides, iarray, istride, varray, vstride);
        } else if(values->dtype == NDARRAY_FLOAT) {
            BOOLEAN_ASSIGNMENT_LOOP(uint8_t, mp_float_t, ndarray, lstrides, iarray, istride, varray, vstride);
        }
    } else if(ndarray->dtype == NDARRAY_INT8) {
        if(values->dtype == NDARRAY_UINT8) {
            BOOLEAN_ASSIGNMENT_LOOP(int8_t, uint8_t, ndarray, lstrides, iarray, istride, varray, vstride);
        } else if(values->dtype == NDARRAY_INT8) {
            BOOLEAN_ASSIGNMENT_LOOP(int8_t, int8_t, ndarray, lstrides, iarray, istride, varray, vstride);
        } else if(values->dtype == NDARRAY_UINT16) {
            BOOLEAN_ASSIGNMENT_LOOP(int8_t, uint16_t, ndarray, lstrides, iarray, istride, varray, vstride);
        } else if(values->dtype == NDARRAY_INT16) {
            BOOLEAN_ASSIGNMENT_LOOP(int8_t, int16_t, ndarray, lstrides, iarray, istride, varray, vstride);
        } else if(values->dtype == NDARRAY_FLOAT) {
            BOOLEAN_ASSIGNMENT_LOOP(int8_t, mp_float_t, ndarray, lstrides, iarray, istride, varray, vstride);
        }
    } else if(ndarray->dtype == NDARRAY_UINT16) {
        if(values->dtype == NDARRAY_UINT8) {
            BOOLEAN_ASSIGNMENT_LOOP(uint16_t, uint8_t, ndarray, lstrides, iarray, istride, varray, vstride);
        } else if(values->dtype == NDARRAY_INT8) {
            BOOLEAN_ASSIGNMENT_LOOP(uint16_t, int8_t, ndarray, lstrides, iarray, istride, varray, vstride);
        } else if(values->dtype == NDARRAY_UINT16) {
            BOOLEAN_ASSIGNMENT_LOOP(uint16_t, uint16_t, ndarray, lstrides, iarray, istride, varray, vstride);
        } else if(values->dtype == NDARRAY_INT16) {
            BOOLEAN_ASSIGNMENT_LOOP(uint16_t, int16_t, ndarray, lstrides, iarray, istride, varray, vstride);
        } else if(values->dtype == NDARRAY_FLOAT) {
            BOOLEAN_ASSIGNMENT_LOOP(uint16_t, mp_float_t, ndarray, lstrides, iarray, istride, varray, vstride);
        }
    } else if(ndarray->dtype == NDARRAY_INT16) {
        if(values->dtype == NDARRAY_UINT8) {
            BOOLEAN_ASSIGNMENT_LOOP(int16_t, uint8_t, ndarray, lstrides, iarray, istride, varray, vstride);
        } else if(values->dtype == NDARRAY_INT8) {
            BOOLEAN_ASSIGNMENT_LOOP(int16_t, int8_t, ndarray, lstrides, iarray, istride, varray, vstride);
        } else if(values->dtype == NDARRAY_UINT16) {
            BOOLEAN_ASSIGNMENT_LOOP(int16_t, uint16_t, ndarray, lstrides, iarray, istride, varray, vstride);
        } else if(values->dtype == NDARRAY_INT16) {
            BOOLEAN_ASSIGNMENT_LOOP(int16_t, int16_t, ndarray, lstrides, iarray, istride, varray, vstride);
        } else if(values->dtype == NDARRAY_FLOAT) {
            BOOLEAN_ASSIGNMENT_LOOP(int16_t, mp_float_t, ndarray, lstrides, iarray, istride, varray, vstride);
        }
    } else {
        #if ULAB_SUPPORTS_COMPLEX
        if(ndarray->dtype == NDARRAY_COMPLEX) {
            lstrides *= 2;
        }
        #endif
        if(values->dtype == NDARRAY_UINT8) {
            BOOLEAN_ASSIGNMENT_LOOP(mp_float_t, uint8_t, ndarray, lstrides, iarray, istride, varray, vstride);
        } else if(values->dtype == NDARRAY_INT8) {
            BOOLEAN_ASSIGNMENT_LOOP(mp_float_t, int8_t, ndarray, lstrides, iarray, istride, varray, vstride);
        } else if(values->dtype == NDARRAY_UINT16) {
            BOOLEAN_ASSIGNMENT_LOOP(mp_float_t, uint16_t, ndarray, lstrides, iarray, istride, varray, vstride);
        } else if(values->dtype == NDARRAY_INT16) {
            BOOLEAN_ASSIGNMENT_LOOP(mp_float_t, int16_t, ndarray, lstrides, iarray, istride, varray, vstride);
        } else if(values->dtype == NDARRAY_FLOAT) {
            BOOLEAN_ASSIGNMENT_LOOP(mp_float_t, mp_float_t, ndarray, lstrides, iarray, istride, varray, vstride);
        }
    }
    return MP_OBJ_FROM_PTR(ndarray);
}

static mp_obj_t ndarray_get_slice(ndarray_obj_t *ndarray, mp_obj_t index, ndarray_obj_t *values) {
    if(mp_obj_is_type(index, &ulab_ndarray_type)) {
        ndarray_obj_t *nindex = MP_OBJ_TO_PTR(index);
        if((nindex->ndim > 1) || (nindex->boolean == false)) {
            mp_raise_NotImplementedError(translate("operation is implemented for 1D Boolean arrays only"));
        }
        if(values == NULL) { // return value(s)
            return ndarray_from_boolean_index(ndarray, nindex);
        } else { // assign value(s)
            ndarray_assign_from_boolean_index(ndarray, nindex, values);
        }
    }
    if(mp_obj_is_type(index, &mp_type_tuple) || mp_obj_is_int(index) || mp_obj_is_type(index, &mp_type_slice)) {
        mp_obj_tuple_t *tuple;
        if(mp_obj_is_type(index, &mp_type_tuple)) {
            tuple = MP_OBJ_TO_PTR(index);
            if(tuple->len > ndarray->ndim) {
                mp_raise_msg(&mp_type_IndexError, translate("too many indices"));
            }
        } else {
            mp_obj_t *items = m_new(mp_obj_t, 1);
            items[0] = index;
            tuple = MP_OBJ_TO_PTR(mp_obj_new_tuple(1, items));
        }
        ndarray_obj_t *view = ndarray_view_from_slices(ndarray, tuple);
        if(values == NULL) { // return value(s)
            // if the view has been reduced to nothing, return a single value
            if(view->ndim == 0) {
                return ndarray_get_item(view, view->array);
            } else {
                return MP_OBJ_FROM_PTR(view);
            }
        } else { // assign value(s)
            ndarray_assign_view(view, values);
        }
    }
    return mp_const_none;
}

mp_obj_t ndarray_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value) {
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (value == MP_OBJ_SENTINEL) { // return value(s)
        return ndarray_get_slice(self, index, NULL);
    } else { // assignment to slices; the value must be an ndarray, or a scalar
        ndarray_obj_t *values = ndarray_from_mp_obj(value, 0);
        return ndarray_get_slice(self, index, values);
    }
    return mp_const_none;
}
#endif /* NDARRAY_IS_SLICEABLE */

#if NDARRAY_IS_ITERABLE

// itarray iterator
mp_obj_t ndarray_getiter(mp_obj_t o_in, mp_obj_iter_buf_t *iter_buf) {
    return ndarray_new_ndarray_iterator(o_in, iter_buf);
}

typedef struct _mp_obj_ndarray_it_t {
    mp_obj_base_t base;
    mp_fun_1_t iternext;
    mp_obj_t ndarray;
    size_t cur;
} mp_obj_ndarray_it_t;

mp_obj_t ndarray_iternext(mp_obj_t self_in) {
    mp_obj_ndarray_it_t *self = MP_OBJ_TO_PTR(self_in);
    ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(self->ndarray);
    uint8_t *array = (uint8_t *)ndarray->array;

    size_t iter_end = ndarray->shape[ULAB_MAX_DIMS-ndarray->ndim];
    if(self->cur < iter_end) {
        // separating this case out saves 50 bytes for 1D arrays
        #if ULAB_MAX_DIMS == 1
        array += self->cur * ndarray->strides[0];
        self->cur++;
        return ndarray_get_item(ndarray, array);
        #else
        if(ndarray->ndim == 1) { // we have a linear array
            array += self->cur * ndarray->strides[ULAB_MAX_DIMS - 1];
            self->cur++;
            return ndarray_get_item(ndarray, array);
        } else { // we have a tensor, return the reduced view
            size_t offset = self->cur * ndarray->strides[ULAB_MAX_DIMS - ndarray->ndim];
            self->cur++;
            return MP_OBJ_FROM_PTR(ndarray_new_view(ndarray, ndarray->ndim-1, ndarray->shape, ndarray->strides, offset));
        }
        #endif
    } else {
        return MP_OBJ_STOP_ITERATION;
    }
}

mp_obj_t ndarray_new_ndarray_iterator(mp_obj_t ndarray, mp_obj_iter_buf_t *iter_buf) {
    assert(sizeof(mp_obj_ndarray_it_t) <= sizeof(mp_obj_iter_buf_t));
    mp_obj_ndarray_it_t *iter = (mp_obj_ndarray_it_t *)iter_buf;
    iter->base.type = &mp_type_polymorph_iter;
    iter->iternext = ndarray_iternext;
    iter->ndarray = ndarray;
    iter->cur = 0;
    return MP_OBJ_FROM_PTR(iter);
}
#endif /* NDARRAY_IS_ITERABLE */

#if NDARRAY_HAS_FLATTEN
mp_obj_t ndarray_flatten(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_order, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_QSTR(MP_QSTR_C)} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    ndarray_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    GET_STR_DATA_LEN(args[0].u_obj, order, len);
    if((len != 1) || ((memcmp(order, "C", 1) != 0) && (memcmp(order, "F", 1) != 0))) {
        mp_raise_ValueError(translate("flattening order must be either 'C', or 'F'"));
    }

    uint8_t *sarray = (uint8_t *)self->array;
    ndarray_obj_t *ndarray = ndarray_new_linear_array(self->len, self->dtype);
    uint8_t *array = (uint8_t *)ndarray->array;

    if(memcmp(order, "C", 1) == 0) { // C-type ordering
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
                        memcpy(array, sarray, self->itemsize);
                        array += ndarray->strides[ULAB_MAX_DIMS - 1];
                        sarray += self->strides[ULAB_MAX_DIMS - 1];
                        l++;
                    } while(l <  self->shape[ULAB_MAX_DIMS - 1]);
                #if ULAB_MAX_DIMS > 1
                    sarray -= self->strides[ULAB_MAX_DIMS - 1] * self->shape[ULAB_MAX_DIMS-1];
                    sarray += self->strides[ULAB_MAX_DIMS - 2];
                    k++;
                } while(k <  self->shape[ULAB_MAX_DIMS - 2]);
                #endif
            #if ULAB_MAX_DIMS > 2
                sarray -= self->strides[ULAB_MAX_DIMS - 2] * self->shape[ULAB_MAX_DIMS-2];
                sarray += self->strides[ULAB_MAX_DIMS - 3];
                j++;
            } while(j <  self->shape[ULAB_MAX_DIMS - 3]);
            #endif
        #if ULAB_MAX_DIMS > 3
            sarray -= self->strides[ULAB_MAX_DIMS - 3] * self->shape[ULAB_MAX_DIMS-3];
            sarray += self->strides[ULAB_MAX_DIMS - 4];
            i++;
        } while(i <  self->shape[ULAB_MAX_DIMS - 4]);
        #endif
    } else { // 'F', Fortran-type ordering
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
                        memcpy(array, sarray, self->itemsize);
                        array += ndarray->strides[ULAB_MAX_DIMS - 1];
                        sarray += self->strides[0];
                        l++;
                    } while(l < self->shape[0]);
                #if ULAB_MAX_DIMS > 1
                    sarray -= self->strides[0] * self->shape[0];
                    sarray += self->strides[1];
                    k++;
                } while(k < self->shape[1]);
                #endif
            #if ULAB_MAX_DIMS > 2
                sarray -= self->strides[1] * self->shape[1];
                sarray += self->strides[2];
                j++;
            } while(j < self->shape[2]);
            #endif
        #if ULAB_MAX_DIMS > 3
            sarray -= self->strides[2] * self->shape[2];
            sarray += self->strides[3];
            i++;
        } while(i < self->shape[3]);
        #endif
    }
    return MP_OBJ_FROM_PTR(ndarray);
}

MP_DEFINE_CONST_FUN_OBJ_KW(ndarray_flatten_obj, 1, ndarray_flatten);
#endif

#if NDARRAY_HAS_ITEMSIZE
mp_obj_t ndarray_itemsize(mp_obj_t self_in) {
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(self->itemsize);
}
#endif

#if NDARRAY_HAS_SHAPE
mp_obj_t ndarray_shape(mp_obj_t self_in) {
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint8_t nitems = MAX(1, self->ndim);
    mp_obj_t *items = m_new(mp_obj_t, nitems);
    for(uint8_t i = 0; i < nitems; i++) {
        items[nitems - i - 1] = mp_obj_new_int(self->shape[ULAB_MAX_DIMS - i - 1]);
    }
    mp_obj_t tuple = mp_obj_new_tuple(nitems, items);
    m_del(mp_obj_t, items, nitems);
    return tuple;
}
#endif

#if NDARRAY_HAS_SIZE
mp_obj_t ndarray_size(mp_obj_t self_in) {
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(self->len);
}
#endif

#if NDARRAY_HAS_STRIDES
mp_obj_t ndarray_strides(mp_obj_t self_in) {
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t *items = m_new(mp_obj_t, self->ndim);
    for(int8_t i=0; i < self->ndim; i++) {
        items[i] = mp_obj_new_int(self->strides[ULAB_MAX_DIMS - self->ndim + i]);
    }
    mp_obj_t tuple = mp_obj_new_tuple(self->ndim, items);
    m_del(mp_obj_t, items, self->ndim);
    return tuple;
}
#endif

#if NDARRAY_HAS_TOBYTES
mp_obj_t ndarray_tobytes(mp_obj_t self_in) {
    // As opposed to numpy, this function returns a bytearray object with the data pointer (i.e., not a copy)
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
    // Piping into a bytearray makes sense for dense arrays only,
    // so bail out, if that is not the case
    if(!ndarray_is_dense(self)) {
        mp_raise_ValueError(translate("tobytes can be invoked for dense arrays only"));
    }
    return mp_obj_new_bytearray_by_ref(self->itemsize * self->len, self->array);
}

MP_DEFINE_CONST_FUN_OBJ_1(ndarray_tobytes_obj, ndarray_tobytes);
#endif

#if NDARRAY_HAS_TOLIST
static mp_obj_t ndarray_recursive_list(ndarray_obj_t *self, uint8_t *array, uint8_t dim) {
    int32_t stride = self->strides[ULAB_MAX_DIMS - dim];
    size_t len = self->shape[ULAB_MAX_DIMS - dim];

    mp_obj_list_t *list = MP_OBJ_TO_PTR(mp_obj_new_list(len, NULL));
    for(size_t i = 0; i < len; i++) {
        if(dim == 1) {
            list->items[i] = ndarray_get_item(self, array);
        } else {
            list->items[i] = ndarray_recursive_list(self, array, dim-1);
        }
        array += stride;
    }
    return MP_OBJ_FROM_PTR(list);
}

mp_obj_t ndarray_tolist(mp_obj_t self_in) {
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint8_t *array = (uint8_t *)self->array;
    return ndarray_recursive_list(self, array, self->ndim);
}

MP_DEFINE_CONST_FUN_OBJ_1(ndarray_tolist_obj, ndarray_tolist);
#endif

// Binary operations
ndarray_obj_t *ndarray_from_mp_obj(mp_obj_t obj, uint8_t other_type) {
    // creates an ndarray from a micropython int or float
    // if the input is an ndarray, it is returned
    // if other_type is 0, return the smallest type that can accommodate the object
    ndarray_obj_t *ndarray;

    if(mp_obj_is_int(obj)) {
        int32_t ivalue = mp_obj_get_int(obj);
        if((ivalue < -32767) || (ivalue > 32767)) {
            // the integer value clearly does not fit the ulab integer types, so move on to float
            ndarray = ndarray_new_linear_array(1, NDARRAY_FLOAT);
            mp_float_t *array = (mp_float_t *)ndarray->array;
            array[0] = (mp_float_t)ivalue;
        } else {
            uint8_t dtype;
            if(ivalue < 0) {
                if(ivalue > -128) {
                    dtype = NDARRAY_INT8;
                } else {
                    dtype = NDARRAY_INT16;
                }
            } else { // ivalue >= 0
                if((other_type == NDARRAY_INT8) || (other_type == NDARRAY_INT16)) {
                    if(ivalue < 128) {
                        dtype = NDARRAY_INT8;
                    } else {
                        dtype = NDARRAY_INT16;
                    }
                } else { // other_type = 0 is also included here
                    if(ivalue < 256) {
                        dtype = NDARRAY_UINT8;
                    } else {
                        dtype = NDARRAY_UINT16;
                    }
                }
            }
            ndarray = ndarray_new_linear_array(1, dtype);
            ndarray_set_value(dtype, ndarray->array, 0, obj);
        }
    } else if(mp_obj_is_float(obj)) {
        ndarray = ndarray_new_linear_array(1, NDARRAY_FLOAT);
        mp_float_t *array = (mp_float_t *)ndarray->array;
        array[0] = mp_obj_get_float(obj);
    } else if(mp_obj_is_type(obj, &ulab_ndarray_type)){
        return MP_OBJ_TO_PTR(obj);
    }
    #if ULAB_SUPPORTS_COMPLEX
    else if(mp_obj_is_type(obj, &mp_type_complex)) {
        ndarray = ndarray_new_linear_array(1, NDARRAY_COMPLEX);
        mp_float_t *array = (mp_float_t *)ndarray->array;
        mp_obj_get_complex(obj, &array[0], &array[1]);
    }
    #endif
    else {
        // assume that the input is an iterable (raises an exception, if it is not the case)
        ndarray = ndarray_from_iterable(obj, NDARRAY_FLOAT);
    }
    return ndarray;
}

#if NDARRAY_HAS_BINARY_OPS || NDARRAY_HAS_INPLACE_OPS
mp_obj_t ndarray_binary_op(mp_binary_op_t _op, mp_obj_t lobj, mp_obj_t robj) {
    // TODO: implement in-place operators
    // if the ndarray stands on the right hand side of the expression, simply swap the operands
    ndarray_obj_t *lhs, *rhs;
    mp_binary_op_t op = _op;
    if((op == MP_BINARY_OP_REVERSE_ADD) || (op == MP_BINARY_OP_REVERSE_MULTIPLY) ||
        (op == MP_BINARY_OP_REVERSE_POWER) || (op == MP_BINARY_OP_REVERSE_SUBTRACT) ||
        (op == MP_BINARY_OP_REVERSE_TRUE_DIVIDE)) {
        lhs = ndarray_from_mp_obj(robj, 0);
        rhs = ndarray_from_mp_obj(lobj, lhs->dtype);
    } else {
        lhs = ndarray_from_mp_obj(lobj, 0);
        rhs = ndarray_from_mp_obj(robj, lhs->dtype);
    }
    if(op == MP_BINARY_OP_REVERSE_ADD) {
        op = MP_BINARY_OP_ADD;
    } else if(op == MP_BINARY_OP_REVERSE_MULTIPLY) {
        op = MP_BINARY_OP_MULTIPLY;
    } else if(op == MP_BINARY_OP_REVERSE_POWER) {
        op = MP_BINARY_OP_POWER;
    } else if(op == MP_BINARY_OP_REVERSE_SUBTRACT) {
        op = MP_BINARY_OP_SUBTRACT;
    } else if(op == MP_BINARY_OP_REVERSE_TRUE_DIVIDE) {
        op = MP_BINARY_OP_TRUE_DIVIDE;
    }

    uint8_t ndim = 0;
    size_t *shape = m_new0(size_t, ULAB_MAX_DIMS);
    int32_t *lstrides = m_new0(int32_t, ULAB_MAX_DIMS);
    int32_t *rstrides = m_new0(int32_t, ULAB_MAX_DIMS);
    uint8_t broadcastable;
    if((op == MP_BINARY_OP_INPLACE_ADD) || (op == MP_BINARY_OP_INPLACE_MULTIPLY) || (op == MP_BINARY_OP_INPLACE_POWER) ||
        (op == MP_BINARY_OP_INPLACE_SUBTRACT) || (op == MP_BINARY_OP_INPLACE_TRUE_DIVIDE)) {
        broadcastable = ndarray_can_broadcast_inplace(lhs, rhs, rstrides);
    } else {
        broadcastable = ndarray_can_broadcast(lhs, rhs, &ndim, shape, lstrides, rstrides);
    }
    if(!broadcastable) {
        mp_raise_ValueError(translate("operands could not be broadcast together"));
        m_del(size_t, shape, ULAB_MAX_DIMS);
        m_del(int32_t, lstrides, ULAB_MAX_DIMS);
        m_del(int32_t, rstrides, ULAB_MAX_DIMS);
    }
    // the empty arrays have to be treated separately
    uint8_t dtype = NDARRAY_INT16;
    ndarray_obj_t *nd;
    if((lhs->len == 0) || (rhs->len == 0)) {
        switch(op) {
            case MP_BINARY_OP_INPLACE_ADD:
            case MP_BINARY_OP_INPLACE_MULTIPLY:
            case MP_BINARY_OP_INPLACE_SUBTRACT:
            case MP_BINARY_OP_ADD:
            case MP_BINARY_OP_MULTIPLY:
            case MP_BINARY_OP_SUBTRACT:
                // here we don't have to list those cases that result in an int16,
                // because dtype is initialised with that NDARRAY_INT16
                if(lhs->dtype == rhs->dtype) {
                    dtype = rhs->dtype;
                } else if((lhs->dtype == NDARRAY_FLOAT) || (rhs->dtype == NDARRAY_FLOAT)) {
                    dtype = NDARRAY_FLOAT;
                } else if(((lhs->dtype == NDARRAY_UINT8) && (rhs->dtype == NDARRAY_UINT16)) ||
                            ((lhs->dtype == NDARRAY_INT8) && (rhs->dtype == NDARRAY_UINT16)) ||
                            ((rhs->dtype == NDARRAY_UINT8) && (lhs->dtype == NDARRAY_UINT16)) ||
                            ((rhs->dtype == NDARRAY_INT8) && (lhs->dtype == NDARRAY_UINT16))) {
                    dtype = NDARRAY_UINT16;
                }
                return MP_OBJ_FROM_PTR(ndarray_new_linear_array(0, dtype));
                break;

            case MP_BINARY_OP_INPLACE_POWER:
            case MP_BINARY_OP_INPLACE_TRUE_DIVIDE:
            case MP_BINARY_OP_POWER:
            case MP_BINARY_OP_TRUE_DIVIDE:
                return MP_OBJ_FROM_PTR(ndarray_new_linear_array(0, NDARRAY_FLOAT));
                break;

            case MP_BINARY_OP_LESS:
            case MP_BINARY_OP_LESS_EQUAL:
            case MP_BINARY_OP_MORE:
            case MP_BINARY_OP_MORE_EQUAL:
            case MP_BINARY_OP_EQUAL:
            case MP_BINARY_OP_NOT_EQUAL:
                nd = ndarray_new_linear_array(0, NDARRAY_UINT8);
                nd->boolean = 1;
                return MP_OBJ_FROM_PTR(nd);

            default:
                return mp_const_none;
                break;
        }
    }

    switch(op) {
        // first the in-place operators
        #if NDARRAY_HAS_INPLACE_ADD
        case MP_BINARY_OP_INPLACE_ADD:
            COMPLEX_DTYPE_NOT_IMPLEMENTED(lhs->dtype);
            return ndarray_inplace_ams(lhs, rhs, rstrides, op);
            break;
        #endif
        #if NDARRAY_HAS_INPLACE_MULTIPLY
        case MP_BINARY_OP_INPLACE_MULTIPLY:
            COMPLEX_DTYPE_NOT_IMPLEMENTED(lhs->dtype);
            return ndarray_inplace_ams(lhs, rhs, rstrides, op);
            break;
        #endif
        #if NDARRAY_HAS_INPLACE_POWER
        case MP_BINARY_OP_INPLACE_POWER:
            COMPLEX_DTYPE_NOT_IMPLEMENTED(lhs->dtype);
            return ndarray_inplace_power(lhs, rhs, rstrides);
            break;
        #endif
        #if NDARRAY_HAS_INPLACE_SUBTRACT
        case MP_BINARY_OP_INPLACE_SUBTRACT:
            COMPLEX_DTYPE_NOT_IMPLEMENTED(lhs->dtype);
            return ndarray_inplace_ams(lhs, rhs, rstrides, op);
            break;
        #endif
        #if NDARRAY_HAS_INPLACE_TRUE_DIVIDE
        case MP_BINARY_OP_INPLACE_TRUE_DIVIDE:
            COMPLEX_DTYPE_NOT_IMPLEMENTED(lhs->dtype);
            return ndarray_inplace_divide(lhs, rhs, rstrides);
            break;
        #endif
        // end if in-place operators

        #if NDARRAY_HAS_BINARY_OP_LESS
        case MP_BINARY_OP_LESS:
            COMPLEX_DTYPE_NOT_IMPLEMENTED(lhs->dtype);
            // here we simply swap the operands
            return ndarray_binary_more(rhs, lhs, ndim, shape, rstrides, lstrides, MP_BINARY_OP_MORE);
            break;
        #endif
        #if NDARRAY_HAS_BINARY_OP_LESS_EQUAL
        case MP_BINARY_OP_LESS_EQUAL:
            COMPLEX_DTYPE_NOT_IMPLEMENTED(lhs->dtype);
            // here we simply swap the operands
            return ndarray_binary_more(rhs, lhs, ndim, shape, rstrides, lstrides, MP_BINARY_OP_MORE_EQUAL);
            break;
        #endif
        #if NDARRAY_HAS_BINARY_OP_EQUAL
        case MP_BINARY_OP_EQUAL:
            return ndarray_binary_equality(lhs, rhs, ndim, shape, lstrides, rstrides, MP_BINARY_OP_EQUAL);
            break;
        #endif
        #if NDARRAY_HAS_BINARY_OP_NOT_EQUAL
        case MP_BINARY_OP_NOT_EQUAL:
            return ndarray_binary_equality(lhs, rhs, ndim, shape, lstrides, rstrides, MP_BINARY_OP_NOT_EQUAL);
            break;
        #endif
        #if NDARRAY_HAS_BINARY_OP_ADD
        case MP_BINARY_OP_ADD:
            return ndarray_binary_add(lhs, rhs, ndim, shape, lstrides, rstrides);
            break;
        #endif
        #if NDARRAY_HAS_BINARY_OP_MULTIPLY
        case MP_BINARY_OP_MULTIPLY:
            return ndarray_binary_multiply(lhs, rhs, ndim, shape, lstrides, rstrides);
            break;
        #endif
        #if NDARRAY_HAS_BINARY_OP_MORE
        case MP_BINARY_OP_MORE:
            COMPLEX_DTYPE_NOT_IMPLEMENTED(lhs->dtype);
            return ndarray_binary_more(lhs, rhs, ndim, shape, lstrides, rstrides, MP_BINARY_OP_MORE);
            break;
        #endif
        #if NDARRAY_HAS_BINARY_OP_MORE_EQUAL
        case MP_BINARY_OP_MORE_EQUAL:
            COMPLEX_DTYPE_NOT_IMPLEMENTED(lhs->dtype);
            return ndarray_binary_more(lhs, rhs, ndim, shape, lstrides, rstrides, MP_BINARY_OP_MORE_EQUAL);
            break;
        #endif
        #if NDARRAY_HAS_BINARY_OP_SUBTRACT
        case MP_BINARY_OP_SUBTRACT:
            return ndarray_binary_subtract(lhs, rhs, ndim, shape, lstrides, rstrides);
            break;
        #endif
        #if NDARRAY_HAS_BINARY_OP_TRUE_DIVIDE
        case MP_BINARY_OP_TRUE_DIVIDE:
            return ndarray_binary_true_divide(lhs, rhs, ndim, shape, lstrides, rstrides);
            break;
        #endif
        #if NDARRAY_HAS_BINARY_OP_POWER
        case MP_BINARY_OP_POWER:
            COMPLEX_DTYPE_NOT_IMPLEMENTED(lhs->dtype);
            return ndarray_binary_power(lhs, rhs, ndim, shape, lstrides, rstrides);
            break;
        #endif
        default:
            return MP_OBJ_NULL; // op not supported
            break;
    }
    return MP_OBJ_NULL;
}
#endif /* NDARRAY_HAS_BINARY_OPS || NDARRAY_HAS_INPLACE_OPS */

#if NDARRAY_HAS_UNARY_OPS
mp_obj_t ndarray_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
    ndarray_obj_t *ndarray = NULL;

    switch (op) {
        #if NDARRAY_HAS_UNARY_OP_ABS
        case MP_UNARY_OP_ABS:
            #if ULAB_SUPPORTS_COMPLEX
            if(self->dtype == NDARRAY_COMPLEX) {
                int32_t *strides = strides_from_shape(self->shape, NDARRAY_FLOAT);
                ndarray_obj_t *target = ndarray_new_ndarray(self->ndim, self->shape, strides, NDARRAY_FLOAT);
                ndarray = MP_OBJ_TO_PTR(carray_abs(self, target));
            } else {
            #endif
                ndarray = ndarray_copy_view(self);
                // if Boolean, NDARRAY_UINT8, or NDARRAY_UINT16, there is nothing to do
                if(self->dtype == NDARRAY_INT8) {
                    int8_t *array = (int8_t *)ndarray->array;
                    for(size_t i=0; i < self->len; i++, array++) {
                        if(*array < 0) *array = -(*array);
                    }
                } else if(self->dtype == NDARRAY_INT16) {
                    int16_t *array = (int16_t *)ndarray->array;
                    for(size_t i=0; i < self->len; i++, array++) {
                        if(*array < 0) *array = -(*array);
                    }
                } else {
                    mp_float_t *array = (mp_float_t *)ndarray->array;
                    for(size_t i=0; i < self->len; i++, array++) {
                        if(*array < 0) *array = -(*array);
                    }
                }
            #if ULAB_SUPPORTS_COMPLEX
            }
            #endif
            return MP_OBJ_FROM_PTR(ndarray);
            break;
        #endif
        #if NDARRAY_HAS_UNARY_OP_INVERT
        case MP_UNARY_OP_INVERT:
            #if ULAB_SUPPORTS_COMPLEX
            if(self->dtype == NDARRAY_FLOAT || self->dtype == NDARRAY_COMPLEX) {
            #else
            if(self->dtype == NDARRAY_FLOAT) {
            #endif
                mp_raise_ValueError(translate("operation is not supported for given type"));
            }
            // we can invert the content byte by byte, no need to distinguish between different dtypes
            ndarray = ndarray_copy_view(self); // from this point, this is a dense copy
            uint8_t *array = (uint8_t *)ndarray->array;
            if(ndarray->boolean) {
                for(size_t i=0; i < ndarray->len; i++, array++) *array = *array ^ 0x01;
            } else {
                uint8_t itemsize = ulab_binary_get_size(self->dtype);
                for(size_t i=0; i < ndarray->len*itemsize; i++, array++) *array ^= 0xFF;
            }
            return MP_OBJ_FROM_PTR(ndarray);
            break;
        #endif
        #if NDARRAY_HAS_UNARY_OP_LEN
        case MP_UNARY_OP_LEN:
            return mp_obj_new_int(self->shape[ULAB_MAX_DIMS - self->ndim]);
            break;
        #endif
        #if NDARRAY_HAS_UNARY_OP_NEGATIVE
        case MP_UNARY_OP_NEGATIVE:
            ndarray = ndarray_copy_view(self); // from this point, this is a dense copy
            if(self->dtype == NDARRAY_UINT8) {
                uint8_t *array = (uint8_t *)ndarray->array;
                for(size_t i=0; i < self->len; i++, array++) *array = -(*array);
            } else if(self->dtype == NDARRAY_INT8) {
                int8_t *array = (int8_t *)ndarray->array;
                for(size_t i=0; i < self->len; i++, array++) *array = -(*array);
            } else if(self->dtype == NDARRAY_UINT16) {
                uint16_t *array = (uint16_t *)ndarray->array;
                for(size_t i=0; i < self->len; i++, array++) *array = -(*array);
            } else if(self->dtype == NDARRAY_INT16) {
                int16_t *array = (int16_t *)ndarray->array;
                for(size_t i=0; i < self->len; i++, array++) *array = -(*array);
            } else {
                mp_float_t *array = (mp_float_t *)ndarray->array;
                size_t len = self->len;
                #if ULAB_SUPPORTS_COMPLEX
                if(self->dtype == NDARRAY_COMPLEX) {
                    len *= 2;
                }
                #endif
                for(size_t i=0; i < len; i++, array++) *array = -(*array);
            }
            return MP_OBJ_FROM_PTR(ndarray);
            break;
        #endif
        #if NDARRAY_HAS_UNARY_OP_POSITIVE
        case MP_UNARY_OP_POSITIVE:
            return MP_OBJ_FROM_PTR(ndarray_copy_view(self));
        #endif

        default:
            return MP_OBJ_NULL; // operator not supported
            break;
    }
}
#endif /* NDARRAY_HAS_UNARY_OPS */

#if NDARRAY_HAS_TRANSPOSE
mp_obj_t ndarray_transpose(mp_obj_t self_in) {
    #if ULAB_MAX_DIMS == 1
        return self_in;
    #endif
    // TODO: check, what happens to the offset here, if we have a view
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if(self->ndim == 1) {
        return self_in;
    }
    size_t *shape = m_new(size_t, self->ndim);
    int32_t *strides = m_new(int32_t, self->ndim);
    for(uint8_t i=0; i < self->ndim; i++) {
        shape[ULAB_MAX_DIMS - 1 - i] = self->shape[ULAB_MAX_DIMS - self->ndim + i];
        strides[ULAB_MAX_DIMS - 1 - i] = self->strides[ULAB_MAX_DIMS - self->ndim + i];
    }
    // TODO: I am not sure ndarray_new_view is OK here...
    // should be deep copy...
    ndarray_obj_t *ndarray = ndarray_new_view(self, self->ndim, shape, strides, 0);
    return MP_OBJ_FROM_PTR(ndarray);
}

MP_DEFINE_CONST_FUN_OBJ_1(ndarray_transpose_obj, ndarray_transpose);
#endif /* NDARRAY_HAS_TRANSPOSE */

#if ULAB_MAX_DIMS > 1
#if NDARRAY_HAS_RESHAPE
mp_obj_t ndarray_reshape_core(mp_obj_t oin, mp_obj_t _shape, bool inplace) {
    ndarray_obj_t *source = MP_OBJ_TO_PTR(oin);
    if(!mp_obj_is_type(_shape, &mp_type_tuple)) {
        mp_raise_TypeError(translate("shape must be a tuple"));
    }

    mp_obj_tuple_t *shape = MP_OBJ_TO_PTR(_shape);
    if(shape->len > ULAB_MAX_DIMS) {
        mp_raise_ValueError(translate("maximum number of dimensions is " MP_STRINGIFY(ULAB_MAX_DIMS)));
    }
    size_t *new_shape = m_new0(size_t, ULAB_MAX_DIMS);

    size_t new_length = 1;
    for(uint8_t i=0; i < shape->len; i++) {
        new_shape[ULAB_MAX_DIMS - i - 1] = mp_obj_get_int(shape->items[shape->len - i - 1]);
        new_length *= new_shape[ULAB_MAX_DIMS - i - 1];
    }
    if(source->len != new_length) {
        mp_raise_ValueError(translate("input and output shapes are not compatible"));
    }
    ndarray_obj_t *ndarray;
    if(ndarray_is_dense(source)) {
        int32_t *new_strides = strides_from_shape(new_shape, source->dtype);
        if(inplace) {
            for(uint8_t i = 0; i < ULAB_MAX_DIMS; i++) {
                source->shape[i] = new_shape[i];
                source->strides[i] = new_strides[i];
            }
            return MP_OBJ_FROM_PTR(oin);
        } else {
            ndarray = ndarray_new_view(source, shape->len, new_shape, new_strides, 0);
        }
    } else {
        if(inplace) {
            mp_raise_ValueError(translate("cannot assign new shape"));
        }
        ndarray = ndarray_new_ndarray_from_tuple(shape, source->dtype);
        ndarray_copy_array(source, ndarray, 0);
    }
    return MP_OBJ_FROM_PTR(ndarray);
}

mp_obj_t ndarray_reshape(mp_obj_t oin, mp_obj_t _shape) {
    return ndarray_reshape_core(oin, _shape, 0);
}

MP_DEFINE_CONST_FUN_OBJ_2(ndarray_reshape_obj, ndarray_reshape);
#endif /* NDARRAY_HAS_RESHAPE */
#endif /* ULAB_MAX_DIMS > 1 */

#if ULAB_NUMPY_HAS_NDINFO
mp_obj_t ndarray_info(mp_obj_t obj_in) {
    if(!mp_obj_is_type(obj_in, &ulab_ndarray_type)) {
        mp_raise_TypeError(translate("function is defined for ndarrays only"));
    }
    ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(obj_in);
    mp_printf(MP_PYTHON_PRINTER, "class: ndarray\n");
    mp_printf(MP_PYTHON_PRINTER, "shape: (");
    if(ndarray->ndim == 1) {
        mp_printf(MP_PYTHON_PRINTER, "%d,", ndarray->shape[ULAB_MAX_DIMS-1]);
    } else {
        for(uint8_t i=0; i < ndarray->ndim-1; i++) mp_printf(MP_PYTHON_PRINTER, "%d, ", ndarray->shape[i]);
        mp_printf(MP_PYTHON_PRINTER, "%d", ndarray->shape[ULAB_MAX_DIMS-1]);
    }
    mp_printf(MP_PYTHON_PRINTER, ")\n");
    mp_printf(MP_PYTHON_PRINTER, "strides: (");
    if(ndarray->ndim == 1) {
        mp_printf(MP_PYTHON_PRINTER, "%d,", ndarray->strides[ULAB_MAX_DIMS-1]);
    } else {
        for(uint8_t i=0; i < ndarray->ndim-1; i++) mp_printf(MP_PYTHON_PRINTER, "%d, ", ndarray->strides[i]);
        mp_printf(MP_PYTHON_PRINTER, "%d", ndarray->strides[ULAB_MAX_DIMS-1]);
    }
    mp_printf(MP_PYTHON_PRINTER, ")\n");
    mp_printf(MP_PYTHON_PRINTER, "itemsize: %d\n", ndarray->itemsize);
    mp_printf(MP_PYTHON_PRINTER, "data pointer: 0x%p\n", ndarray->array);
    mp_printf(MP_PYTHON_PRINTER, "type: ");
    if(ndarray->boolean) {
        mp_printf(MP_PYTHON_PRINTER, "bool\n");
    } else if(ndarray->dtype == NDARRAY_UINT8) {
        mp_printf(MP_PYTHON_PRINTER, "uint8\n");
    } else if(ndarray->dtype == NDARRAY_INT8) {
        mp_printf(MP_PYTHON_PRINTER, "int8\n");
    } else if(ndarray->dtype == NDARRAY_UINT16) {
        mp_printf(MP_PYTHON_PRINTER, "uint16\n");
    } else if(ndarray->dtype == NDARRAY_INT16) {
        mp_printf(MP_PYTHON_PRINTER, "int16\n");
    } else if(ndarray->dtype == NDARRAY_FLOAT) {
        mp_printf(MP_PYTHON_PRINTER, "float\n");
    }
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(ndarray_info_obj, ndarray_info);
#endif

// (the get_buffer protocol returns 0 for success, 1 for failure)
mp_int_t ndarray_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    ndarray_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if(!ndarray_is_dense(self)) {
        return 1;
    }
    bufinfo->len = self->itemsize * self->len;
    bufinfo->buf = self->array;
    bufinfo->typecode = self->dtype;
    return 0;
}
