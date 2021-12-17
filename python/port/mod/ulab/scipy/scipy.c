
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
#include "py/runtime.h"

#include "../ulab.h"
#include "optimize/optimize.h"
#include "signal/signal.h"
#include "special/special.h"
#include "linalg/linalg.h"

#if ULAB_HAS_SCIPY

//| """Compatibility layer for scipy"""
//|

static const mp_rom_map_elem_t ulab_scipy_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_scipy) },
    #if ULAB_SCIPY_HAS_LINALG_MODULE
        { MP_ROM_QSTR(MP_QSTR_linalg), MP_ROM_PTR(&ulab_scipy_linalg_module) },
    #endif
    #if ULAB_SCIPY_HAS_OPTIMIZE_MODULE
        { MP_ROM_QSTR(MP_QSTR_optimize), MP_ROM_PTR(&ulab_scipy_optimize_module) },
    #endif
    #if ULAB_SCIPY_HAS_SIGNAL_MODULE
        { MP_ROM_QSTR(MP_QSTR_signal), MP_ROM_PTR(&ulab_scipy_signal_module) },
    #endif
    #if ULAB_SCIPY_HAS_SPECIAL_MODULE
        { MP_ROM_QSTR(MP_QSTR_special), MP_ROM_PTR(&ulab_scipy_special_module) },
    #endif
};

static MP_DEFINE_CONST_DICT(mp_module_ulab_scipy_globals, ulab_scipy_globals_table);

mp_obj_module_t ulab_scipy_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_scipy_globals,
};
#endif
