/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Zoltán Vörös
*/

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/misc.h"
#include "utils.h"

#if ULAB_HAS_UTILS_MODULE

enum UTILS_BUFFER_TYPE {
    UTILS_INT16_BUFFER,
    UTILS_UINT16_BUFFER,
    UTILS_INT32_BUFFER,
    UTILS_UINT32_BUFFER,
};

#if ULAB_UTILS_HAS_FROM_INT16_BUFFER | ULAB_UTILS_HAS_FROM_UINT16_BUFFER | ULAB_UTILS_HAS_FROM_INT32_BUFFER | ULAB_UTILS_HAS_FROM_UINT32_BUFFER
static mp_obj_t utils_from_intbuffer_helper(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args, uint8_t buffer_type) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_rom_obj = mp_const_none } } ,
        { MP_QSTR_count, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_INT(-1) } },
        { MP_QSTR_offset, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_INT(0) } },
        { MP_QSTR_out, MP_ARG_OBJ, { .u_rom_obj = mp_const_none } },
        { MP_QSTR_byteswap, MP_ARG_OBJ, { .u_rom_obj = mp_const_false } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    ndarray_obj_t *ndarray = NULL;

    if(args[3].u_obj != mp_const_none) {
        ndarray = MP_OBJ_TO_PTR(args[3].u_obj);
        if((ndarray->dtype != NDARRAY_FLOAT) || !ndarray_is_dense(ndarray)) {
            mp_raise_TypeError(translate("out must be a float dense array"));
        }
    }

    size_t offset = mp_obj_get_int(args[2].u_obj);

    mp_buffer_info_t bufinfo;
    if(mp_get_buffer(args[0].u_obj, &bufinfo, MP_BUFFER_READ)) {
        if(bufinfo.len < offset) {
            mp_raise_ValueError(translate("offset is too large"));
        }
        uint8_t sz = sizeof(int16_t);
        #if ULAB_UTILS_HAS_FROM_INT32_BUFFER | ULAB_UTILS_HAS_FROM_UINT32_BUFFER
        if((buffer_type == UTILS_INT32_BUFFER) ||  (buffer_type == UTILS_UINT32_BUFFER)) {
            sz = sizeof(int32_t);
        }
        #endif

        size_t len = (bufinfo.len - offset) / sz;
        if((len * sz) != (bufinfo.len - offset)) {
            mp_raise_ValueError(translate("buffer size must be a multiple of element size"));
        }
        if(mp_obj_get_int(args[1].u_obj) > 0) {
            size_t count = mp_obj_get_int(args[1].u_obj);
            if(len < count) {
                mp_raise_ValueError(translate("buffer is smaller than requested size"));
            } else {
                len = count;
            }
        }
        if(args[3].u_obj == mp_const_none) {
            ndarray = ndarray_new_linear_array(len, NDARRAY_FLOAT);
        } else {
            if(ndarray->len < len) {
                mp_raise_ValueError(translate("out array is too small"));
            }
        }
        uint8_t *buffer = bufinfo.buf;

        mp_float_t *array = (mp_float_t *)ndarray->array;
        if(args[4].u_obj == mp_const_true) {
            // swap the bytes before conversion
            uint8_t *tmpbuff = m_new(uint8_t, sz);
            #if ULAB_UTILS_HAS_FROM_INT16_BUFFER | ULAB_UTILS_HAS_FROM_UINT16_BUFFER
            if((buffer_type == UTILS_INT16_BUFFER) || (buffer_type == UTILS_UINT16_BUFFER)) {
                for(size_t i = 0; i < len; i++) {
                    tmpbuff += sz;
                    for(uint8_t j = 0; j < sz; j++) {
                        memcpy(--tmpbuff, buffer++, 1);
                    }
                    if(buffer_type == UTILS_INT16_BUFFER) {
                        *array++ = (mp_float_t)(*(int16_t *)tmpbuff);
                    } else {
                        *array++ = (mp_float_t)(*(uint16_t *)tmpbuff);
                    }
                }
            }
            #endif
            #if ULAB_UTILS_HAS_FROM_INT32_BUFFER | ULAB_UTILS_HAS_FROM_UINT32_BUFFER
            if((buffer_type == UTILS_INT32_BUFFER) || (buffer_type == UTILS_UINT32_BUFFER)) {
                for(size_t i = 0; i < len; i++) {
                    tmpbuff += sz;
                    for(uint8_t j = 0; j < sz; j++) {
                        memcpy(--tmpbuff, buffer++, 1);
                    }
                    if(buffer_type == UTILS_INT32_BUFFER) {
                        *array++ = (mp_float_t)(*(int32_t *)tmpbuff);
                    } else {
                        *array++ = (mp_float_t)(*(uint32_t *)tmpbuff);
                    }
                }
            }
            #endif
        } else {
            #if ULAB_UTILS_HAS_FROM_INT16_BUFFER
            if(buffer_type == UTILS_INT16_BUFFER) {
                for(size_t i = 0; i < len; i++) {
                    *array++ = (mp_float_t)(*(int16_t *)buffer);
                    buffer += sz;
                }
            }
            #endif
            #if ULAB_UTILS_HAS_FROM_UINT16_BUFFER
            if(buffer_type == UTILS_UINT16_BUFFER) {
                for(size_t i = 0; i < len; i++) {
                    *array++ = (mp_float_t)(*(uint16_t *)buffer);
                    buffer += sz;
                }
            }
            #endif
            #if ULAB_UTILS_HAS_FROM_INT32_BUFFER
            if(buffer_type == UTILS_INT32_BUFFER) {
                for(size_t i = 0; i < len; i++) {
                    *array++ = (mp_float_t)(*(int32_t *)buffer);
                    buffer += sz;
                }
            }
            #endif
            #if ULAB_UTILS_HAS_FROM_UINT32_BUFFER
            if(buffer_type == UTILS_UINT32_BUFFER) {
                for(size_t i = 0; i < len; i++) {
                    *array++ = (mp_float_t)(*(uint32_t *)buffer);
                    buffer += sz;
                }
            }
            #endif
        }
        return MP_OBJ_FROM_PTR(ndarray);
    }
    return mp_const_none;
}

#ifdef ULAB_UTILS_HAS_FROM_INT16_BUFFER
static mp_obj_t utils_from_int16_buffer(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return utils_from_intbuffer_helper(n_args, pos_args, kw_args, UTILS_INT16_BUFFER);
}

MP_DEFINE_CONST_FUN_OBJ_KW(utils_from_int16_buffer_obj, 1, utils_from_int16_buffer);
#endif

#ifdef ULAB_UTILS_HAS_FROM_UINT16_BUFFER
static mp_obj_t utils_from_uint16_buffer(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return utils_from_intbuffer_helper(n_args, pos_args, kw_args, UTILS_UINT16_BUFFER);
}

MP_DEFINE_CONST_FUN_OBJ_KW(utils_from_uint16_buffer_obj, 1, utils_from_uint16_buffer);
#endif

#ifdef ULAB_UTILS_HAS_FROM_INT32_BUFFER
static mp_obj_t utils_from_int32_buffer(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return utils_from_intbuffer_helper(n_args, pos_args, kw_args, UTILS_INT32_BUFFER);
}

MP_DEFINE_CONST_FUN_OBJ_KW(utils_from_int32_buffer_obj, 1, utils_from_int32_buffer);
#endif

#ifdef ULAB_UTILS_HAS_FROM_UINT32_BUFFER
static mp_obj_t utils_from_uint32_buffer(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return utils_from_intbuffer_helper(n_args, pos_args, kw_args, UTILS_UINT32_BUFFER);
}

MP_DEFINE_CONST_FUN_OBJ_KW(utils_from_uint32_buffer_obj, 1, utils_from_uint32_buffer);
#endif

#endif

static const mp_rom_map_elem_t ulab_utils_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_utils) },
    #if ULAB_UTILS_HAS_FROM_INT16_BUFFER
        { MP_OBJ_NEW_QSTR(MP_QSTR_from_int16_buffer), (mp_obj_t)&utils_from_int16_buffer_obj },
    #endif
    #if ULAB_UTILS_HAS_FROM_UINT16_BUFFER
        { MP_OBJ_NEW_QSTR(MP_QSTR_from_uint16_buffer), (mp_obj_t)&utils_from_uint16_buffer_obj },
    #endif
    #if ULAB_UTILS_HAS_FROM_INT32_BUFFER
        { MP_OBJ_NEW_QSTR(MP_QSTR_from_int32_buffer), (mp_obj_t)&utils_from_int32_buffer_obj },
    #endif
    #if ULAB_UTILS_HAS_FROM_UINT32_BUFFER
        { MP_OBJ_NEW_QSTR(MP_QSTR_from_uint32_buffer), (mp_obj_t)&utils_from_uint32_buffer_obj },
    #endif
};

static MP_DEFINE_CONST_DICT(mp_module_ulab_utils_globals, ulab_utils_globals_table);

mp_obj_module_t ulab_utils_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_utils_globals,
};

#endif
