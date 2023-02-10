
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Zoltán Vörös
 *
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "py/obj.h"
#include "py/runtime.h"

#include "ulab.h"
#include "ndarray.h"
#include "numpy/ndarray/ndarray_iter.h"
#if ULAB_SUPPORTS_COMPLEX
#include "numpy/carray/carray.h"
#endif

#ifndef CIRCUITPY

// a somewhat hackish implementation of property getters/setters;
// this functions is hooked into the attr member of ndarray

STATIC void call_local_method(mp_obj_t obj, qstr attr, mp_obj_t *dest) {
    const mp_obj_type_t *type = mp_obj_get_type(obj);
    while (type->locals_dict != NULL) {
        assert(type->locals_dict->base.type == &mp_type_dict); // MicroPython restriction, for now
        mp_map_t *locals_map = &type->locals_dict->map;
        mp_map_elem_t *elem = mp_map_lookup(locals_map, MP_OBJ_NEW_QSTR(attr), MP_MAP_LOOKUP);
        if (elem != NULL) {
            mp_convert_member_lookup(obj, type, elem->value, dest);
            break;
        }
        if (type->parent == NULL) {
            break;
        }
        type = type->parent;
    }
}


void ndarray_properties_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    if (dest[0] == MP_OBJ_NULL) {
        switch(attr) {
            #if NDARRAY_HAS_DTYPE
            case MP_QSTR_dtype:
                dest[0] = ndarray_dtype(self_in);
                break;
            #endif
            #if NDARRAY_HAS_FLATITER
            case MP_QSTR_flat:
                dest[0] = ndarray_flatiter_make_new(self_in);
                break;
            #endif
            #if NDARRAY_HAS_ITEMSIZE
            case MP_QSTR_itemsize:
                dest[0] = ndarray_itemsize(self_in);
                break;
            #endif
            #if NDARRAY_HAS_SHAPE
            case MP_QSTR_shape:
                dest[0] = ndarray_shape(self_in);
                break;
            #endif
            #if NDARRAY_HAS_SIZE
            case MP_QSTR_size:
                dest[0] = ndarray_size(self_in);
                break;
            #endif
            #if NDARRAY_HAS_STRIDES
            case MP_QSTR_strides:
                dest[0] = ndarray_strides(self_in);
                break;
            #endif
            #if NDARRAY_HAS_TRANSPOSE
            case MP_QSTR_T:
                dest[0] = ndarray_transpose(self_in);
                break;
            #endif
            #if ULAB_SUPPORTS_COMPLEX
            #if ULAB_NUMPY_HAS_IMAG
            case MP_QSTR_imag:
                dest[0] = carray_imag(self_in);
                break;
            #endif
            #if ULAB_NUMPY_HAS_IMAG
            case MP_QSTR_real:
                dest[0] = carray_real(self_in);
                break;
            #endif
            #endif /* ULAB_SUPPORTS_COMPLEX */
            default:
                call_local_method(self_in, attr, dest);
                break;
        }
    } else {
        if(dest[1]) {
            switch(attr) {
                #if ULAB_MAX_DIMS > 1
                #if NDARRAY_HAS_RESHAPE
                case MP_QSTR_shape:
                    ndarray_reshape_core(self_in, dest[1], 1);
                    break;
                #endif
                #endif
                default:
                    return;
                    break;
            }
            dest[0] = MP_OBJ_NULL;
        }
    }
}

#endif /* CIRCUITPY */