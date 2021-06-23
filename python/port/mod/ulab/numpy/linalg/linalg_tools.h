/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2021 Zoltán Vörös
*/

#ifndef _TOOLS_TOOLS_
#define _TOOLS_TOOLS_

#ifndef LINALG_EPSILON
#if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
#define LINALG_EPSILON      MICROPY_FLOAT_CONST(1.2e-7)
#elif MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_DOUBLE
#define LINALG_EPSILON      MICROPY_FLOAT_CONST(2.3e-16)
#endif
#endif /* LINALG_EPSILON */

#define JACOBI_MAX     20

bool linalg_invert_matrix(mp_float_t *, size_t );
size_t linalg_jacobi_rotations(mp_float_t *, mp_float_t *, size_t );

#endif /* _TOOLS_TOOLS_ */

