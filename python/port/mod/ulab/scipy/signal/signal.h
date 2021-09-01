
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Zoltán Vörös
 *
*/

#ifndef _SCIPY_SIGNAL_
#define _SCIPY_SIGNAL_

#include "../../ulab.h"
#include "../../ndarray.h"

extern mp_obj_module_t ulab_scipy_signal_module;

MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(signal_spectrogram_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(signal_sosfilt_obj);

#endif /* _SCIPY_SIGNAL_ */
