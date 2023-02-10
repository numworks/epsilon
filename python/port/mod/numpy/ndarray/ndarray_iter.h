
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
 *               2020-2021 Zoltán Vörös
*/

#ifndef _NDARRAY_ITER_
#define _NDARRAY_ITER_

#include "py/runtime.h"
#include "py/binary.h"
#include "py/obj.h"
#include "py/objarray.h"

#include "../../ulab.h"
#include "../../ndarray.h"

// TODO: take simply mp_obj_ndarray_it_t from ndarray.c
typedef struct _mp_obj_ndarray_flatiter_t {
    mp_obj_base_t base;
    mp_fun_1_t iternext;
    mp_obj_t ndarray;
    size_t cur;
} ndarray_flatiter_t;

mp_obj_t ndarray_get_flatiterator(mp_obj_t , mp_obj_iter_buf_t *);
mp_obj_t ndarray_flatiter_make_new(mp_obj_t );
mp_obj_t ndarray_flatiter_next(mp_obj_t );

#endif