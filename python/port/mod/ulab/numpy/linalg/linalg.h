
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2021 Zoltán Vörös
*/

#ifndef _LINALG_
#define _LINALG_

#include "../../ulab.h"
#include "../../ndarray.h"
#include "linalg_tools.h"

extern mp_obj_module_t ulab_linalg_module;

MP_DECLARE_CONST_FUN_OBJ_1(linalg_cholesky_obj);
MP_DECLARE_CONST_FUN_OBJ_1(linalg_det_obj);
MP_DECLARE_CONST_FUN_OBJ_1(linalg_eig_obj);
MP_DECLARE_CONST_FUN_OBJ_1(linalg_inv_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(linalg_norm_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(linalg_qr_obj);
#endif
