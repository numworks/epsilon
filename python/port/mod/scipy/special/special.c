
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

#include "../../ulab.h"
#include "../../numpy/vector.h"

static const mp_rom_map_elem_t ulab_scipy_special_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_special) },
    #if ULAB_SCIPY_SPECIAL_HAS_ERF
		{ MP_ROM_QSTR(MP_QSTR_erf), MP_ROM_PTR(&vector_erf_obj) },
    #endif
	#if ULAB_SCIPY_SPECIAL_HAS_ERFC
		{ MP_ROM_QSTR(MP_QSTR_erfc), MP_ROM_PTR(&vector_erfc_obj) },
	#endif
	#if ULAB_SCIPY_SPECIAL_HAS_GAMMA
		{ MP_ROM_QSTR(MP_QSTR_gamma), MP_ROM_PTR(&vector_gamma_obj) },
	#endif
	#if ULAB_SCIPY_SPECIAL_HAS_GAMMALN
		{ MP_ROM_QSTR(MP_QSTR_gammaln), MP_ROM_PTR(&vector_lgamma_obj) },
	#endif
};

static MP_DEFINE_CONST_DICT(mp_module_ulab_scipy_special_globals, ulab_scipy_special_globals_table);

const mp_obj_module_t ulab_scipy_special_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_scipy_special_globals,
};
#if CIRCUITPY_ULAB
#if !defined(MICROPY_VERSION) || MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_ulab_dot_scipy_dot_special, ulab_scipy_special_module, MODULE_ULAB_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_ulab_dot_scipy_dot_special, ulab_scipy_special_module);
#endif
#endif
