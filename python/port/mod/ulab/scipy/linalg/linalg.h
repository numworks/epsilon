
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Vikas Udupa
 * 
*/

#ifndef _SCIPY_LINALG_
#define _SCIPY_LINALG_

extern mp_obj_module_t ulab_scipy_linalg_module;

MP_DECLARE_CONST_FUN_OBJ_KW(linalg_solve_triangular_obj);
MP_DECLARE_CONST_FUN_OBJ_2(linalg_cho_solve_obj);

#endif /* _SCIPY_LINALG_ */
