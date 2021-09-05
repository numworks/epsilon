
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Zoltán Vörös
*/

#ifndef _APPROX_
#define _APPROX_

#include "../../ulab.h"
#include "../../ndarray.h"

#define     APPROX_EPS          MICROPY_FLOAT_CONST(1.0e-4)
#define     APPROX_NONZDELTA    MICROPY_FLOAT_CONST(0.05)
#define     APPROX_ZDELTA       MICROPY_FLOAT_CONST(0.00025)
#define     APPROX_ALPHA        MICROPY_FLOAT_CONST(1.0)
#define     APPROX_BETA         MICROPY_FLOAT_CONST(2.0)
#define     APPROX_GAMMA        MICROPY_FLOAT_CONST(0.5)
#define     APPROX_DELTA        MICROPY_FLOAT_CONST(0.5)

MP_DECLARE_CONST_FUN_OBJ_KW(approx_interp_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(approx_trapz_obj);

#endif  /* _APPROX_ */
