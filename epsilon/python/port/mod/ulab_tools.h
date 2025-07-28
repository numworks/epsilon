/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2022 Zoltán Vörös
*/

#ifndef _TOOLS_
#define _TOOLS_

#include "ndarray.h"

#define SWAP(t, a, b) { t tmp = a; a = b; b = tmp; }

typedef struct _shape_strides_t {
    uint8_t increment;
    uint8_t ndim;
    size_t *shape;
    int32_t *strides;
} shape_strides;

mp_float_t ndarray_get_float_uint8(void *);
mp_float_t ndarray_get_float_int8(void *);
mp_float_t ndarray_get_float_uint16(void *);
mp_float_t ndarray_get_float_int16(void *);
mp_float_t ndarray_get_float_float(void *);
void *ndarray_get_float_function(uint8_t );

uint8_t ndarray_upcast_dtype(uint8_t , uint8_t );
void *ndarray_set_float_function(uint8_t );

shape_strides tools_reduce_axes(ndarray_obj_t *, mp_obj_t );
int8_t tools_get_axis(mp_obj_t , uint8_t );
ndarray_obj_t *tools_object_is_square(mp_obj_t );

uint8_t ulab_binary_get_size(uint8_t );

#if ULAB_SUPPORTS_COMPLEX
void ulab_rescale_float_strides(int32_t *);
#endif

bool ulab_tools_mp_obj_is_scalar(mp_obj_t );
#endif
