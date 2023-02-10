/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
 *               2019-2021 Zoltán Vörös
*/

#ifndef _CREATE_
#define _CREATE_

#include "../ulab.h"
#include "../ndarray.h"

#if ULAB_NUMPY_HAS_ARANGE
mp_obj_t create_arange(size_t , const mp_obj_t *, mp_map_t *);
MP_DECLARE_CONST_FUN_OBJ_KW(create_arange_obj);
#endif

#if ULAB_NUMPY_HAS_ASARRAY
mp_obj_t create_arange(size_t , const mp_obj_t *, mp_map_t *);
MP_DECLARE_CONST_FUN_OBJ_KW(create_asarray_obj);
#endif

#if ULAB_NUMPY_HAS_CONCATENATE
mp_obj_t create_concatenate(size_t , const mp_obj_t *, mp_map_t *);
MP_DECLARE_CONST_FUN_OBJ_KW(create_concatenate_obj);
#endif

#if ULAB_NUMPY_HAS_DIAG
mp_obj_t create_diag(size_t , const mp_obj_t *, mp_map_t *);
MP_DECLARE_CONST_FUN_OBJ_KW(create_diag_obj);
#endif

#if ULAB_MAX_DIMS > 1
#if ULAB_NUMPY_HAS_EYE
mp_obj_t create_eye(size_t , const mp_obj_t *, mp_map_t *);
MP_DECLARE_CONST_FUN_OBJ_KW(create_eye_obj);
#endif
#endif

#if ULAB_NUMPY_HAS_FULL
mp_obj_t create_full(size_t , const mp_obj_t *, mp_map_t *);
MP_DECLARE_CONST_FUN_OBJ_KW(create_full_obj);
#endif

#if ULAB_NUMPY_HAS_LINSPACE
mp_obj_t create_linspace(size_t , const mp_obj_t *, mp_map_t *);
MP_DECLARE_CONST_FUN_OBJ_KW(create_linspace_obj);
#endif

#if ULAB_NUMPY_HAS_LOGSPACE
mp_obj_t create_logspace(size_t , const mp_obj_t *, mp_map_t *);
MP_DECLARE_CONST_FUN_OBJ_KW(create_logspace_obj);
#endif

#if ULAB_NUMPY_HAS_ONES
mp_obj_t create_ones(size_t , const mp_obj_t *, mp_map_t *);
MP_DECLARE_CONST_FUN_OBJ_KW(create_ones_obj);
#endif

#if ULAB_NUMPY_HAS_ZEROS
mp_obj_t create_zeros(size_t , const mp_obj_t *, mp_map_t *);
MP_DECLARE_CONST_FUN_OBJ_KW(create_zeros_obj);
#endif

#if ULAB_NUMPY_HAS_FROMBUFFER
mp_obj_t create_frombuffer(size_t , const mp_obj_t *, mp_map_t *);
MP_DECLARE_CONST_FUN_OBJ_KW(create_frombuffer_obj);
#endif

#define ARANGE_LOOP(type_, ndarray, len, step, stop) \
({\
    type_ *array = (type_ *)(ndarray)->array;\
    for (size_t i = 0; i < (len) - 1; i++, (value) += (step)) {\
        *array++ = (type_)(value);\
    }\
    *array = (type_)(stop);\
})

#endif
