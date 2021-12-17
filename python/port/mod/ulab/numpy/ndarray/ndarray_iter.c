
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

#include "ndarray_iter.h"

#ifdef NDARRAY_HAS_FLATITER
mp_obj_t ndarray_flatiter_make_new(mp_obj_t self_in) {
    ndarray_flatiter_t *flatiter = m_new_obj(ndarray_flatiter_t);
    flatiter->base.type = &ndarray_flatiter_type;
    flatiter->iternext = ndarray_flatiter_next;
    flatiter->ndarray = MP_OBJ_TO_PTR(self_in);
    flatiter->cur = 0;
    return flatiter;
}

mp_obj_t ndarray_flatiter_next(mp_obj_t self_in) {
    ndarray_flatiter_t *self = MP_OBJ_TO_PTR(self_in);
    ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(self->ndarray);
    uint8_t *array = (uint8_t *)ndarray->array;

    if(self->cur < ndarray->len) {
        uint32_t remainder = self->cur;
        uint8_t i = ULAB_MAX_DIMS - 1;
        do {
            size_t div = (remainder / ndarray->shape[i]);
            array += remainder * ndarray->strides[i];
            remainder -= div * ndarray->shape[i];
            i--;
        } while(i > ULAB_MAX_DIMS - ndarray->ndim);
        self->cur++;
        return ndarray_get_item(ndarray, array);
    }
    return MP_OBJ_STOP_ITERATION;
}

mp_obj_t ndarray_new_flatiterator(mp_obj_t flatiter_in, mp_obj_iter_buf_t *iter_buf) {
    assert(sizeof(ndarray_flatiter_t) <= sizeof(mp_obj_iter_buf_t));
    ndarray_flatiter_t *iter = (ndarray_flatiter_t *)iter_buf;
    ndarray_flatiter_t *flatiter = MP_OBJ_TO_PTR(flatiter_in);
    iter->base.type = &mp_type_polymorph_iter;
    iter->iternext = ndarray_flatiter_next;
    iter->ndarray = flatiter->ndarray;
    iter->cur = 0;
    return MP_OBJ_FROM_PTR(iter);
}

mp_obj_t ndarray_get_flatiterator(mp_obj_t o_in, mp_obj_iter_buf_t *iter_buf) {
    return ndarray_new_flatiterator(o_in, iter_buf);
}
#endif /* NDARRAY_HAS_FLATITER */
