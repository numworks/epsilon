
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
 *               2020 Scott Shawcroft for Adafruit Industries
 *               2020-2021 Zoltán Vörös
 *               2020 Taku Fukada
*/

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/misc.h"

#include "../ulab.h"
#include "../scipy/signal/signal.h"
#include "filter.h"

#if ULAB_NUMPY_HAS_CONVOLVE

mp_obj_t filter_convolve(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_a, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_v, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if(!mp_obj_is_type(args[0].u_obj, &ulab_ndarray_type) || !mp_obj_is_type(args[1].u_obj, &ulab_ndarray_type)) {
        mp_raise_TypeError(translate("convolve arguments must be ndarrays"));
    }

    ndarray_obj_t *a = MP_OBJ_TO_PTR(args[0].u_obj);
    ndarray_obj_t *c = MP_OBJ_TO_PTR(args[1].u_obj);
    // deal with linear arrays only
    #if ULAB_MAX_DIMS > 1
    if((a->ndim != 1) || (c->ndim != 1)) {
        mp_raise_TypeError(translate("convolve arguments must be linear arrays"));
    }
    #endif
    size_t len_a = a->len;
    size_t len_c = c->len;
    if(len_a == 0 || len_c == 0) {
        mp_raise_TypeError(translate("convolve arguments must not be empty"));
    }

    int len = len_a + len_c - 1; // convolve mode "full"
    ndarray_obj_t *out = ndarray_new_linear_array(len, NDARRAY_FLOAT);
    mp_float_t *outptr = (mp_float_t *)out->array;
    uint8_t *aarray = (uint8_t *)a->array;
    uint8_t *carray = (uint8_t *)c->array;

    int32_t off = len_c - 1;
    int32_t as = a->strides[ULAB_MAX_DIMS - 1] / a->itemsize;
    int32_t cs = c->strides[ULAB_MAX_DIMS - 1] / c->itemsize;

    for(int32_t k=-off; k < len-off; k++) {
        mp_float_t accum = (mp_float_t)0.0;
        int32_t top_n = MIN(len_c, len_a - k);
        int32_t bot_n = MAX(-k, 0);
        for(int32_t n=bot_n; n < top_n; n++) {
            int32_t idx_c = (len_c - n - 1) * cs;
            int32_t idx_a = (n + k) * as;
            mp_float_t ai = ndarray_get_float_index(aarray, a->dtype, idx_a);
            mp_float_t ci = ndarray_get_float_index(carray, c->dtype, idx_c);
            accum += ai * ci;
        }
        *outptr++ = accum;
    }

    return out;
}

MP_DEFINE_CONST_FUN_OBJ_KW(filter_convolve_obj, 2, filter_convolve);

#endif
