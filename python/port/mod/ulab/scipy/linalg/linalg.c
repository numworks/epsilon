
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

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/misc.h"

#include "../../ulab.h"
#include "../../ulab_tools.h"
#include "../../numpy/linalg/linalg_tools.h"
#include "linalg.h"

#if ULAB_SCIPY_HAS_LINALG_MODULE
//|
//| import ulab.scipy
//| import ulab.numpy
//|
//| """Linear algebra functions"""
//|

#if ULAB_MAX_DIMS > 1

//| def solve_triangular(A: ulab.numpy.ndarray, b: ulab.numpy.ndarray, lower: bool) -> ulab.numpy.ndarray:
//|    """
//|    :param ~ulab.numpy.ndarray A: a matrix
//|    :param ~ulab.numpy.ndarray b: a vector
//|    :param ~bool lower: if true, use only data contained in lower triangle of A, else use upper triangle of A
//|    :return: solution to the system A x = b. Shape of return matches b
//|    :raises TypeError: if A and b are not of type ndarray and are not dense
//|    :raises ValueError: if A is a singular matrix
//|
//|    Solve the equation A x = b for x, assuming A is a triangular matrix"""
//|    ...
//|

static mp_obj_t solve_triangular(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    size_t i, j;

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_rom_obj = mp_const_none} } ,
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_rom_obj = mp_const_none} } ,
        { MP_QSTR_lower, MP_ARG_OBJ, { .u_rom_obj = mp_const_false } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if(!mp_obj_is_type(args[0].u_obj, &ulab_ndarray_type) || !mp_obj_is_type(args[1].u_obj, &ulab_ndarray_type)) {
        mp_raise_TypeError(translate("first two arguments must be ndarrays"));
    }

    ndarray_obj_t *A = MP_OBJ_TO_PTR(args[0].u_obj);
    ndarray_obj_t *b = MP_OBJ_TO_PTR(args[1].u_obj);

    if(!ndarray_is_dense(A) || !ndarray_is_dense(b)) {
        mp_raise_TypeError(translate("input must be a dense ndarray"));
    }

    size_t A_rows = A->shape[ULAB_MAX_DIMS - 2];
    size_t A_cols = A->shape[ULAB_MAX_DIMS - 1];

    uint8_t *A_arr = (uint8_t *)A->array;
    uint8_t *b_arr = (uint8_t *)b->array;

    mp_float_t (*get_A_ele)(void *) = ndarray_get_float_function(A->dtype);
    mp_float_t (*get_b_ele)(void *) = ndarray_get_float_function(b->dtype);

    uint8_t *temp_A = A_arr;

    // check if input matrix A is singular
    for (i = 0; i < A_rows; i++) {
        if (MICROPY_FLOAT_C_FUN(fabs)(get_A_ele(A_arr)) < LINALG_EPSILON)
            mp_raise_ValueError(translate("input matrix is singular"));
        A_arr += A->strides[ULAB_MAX_DIMS - 2];
        A_arr += A->strides[ULAB_MAX_DIMS - 1];
    }

    A_arr = temp_A;

    ndarray_obj_t *x = ndarray_new_dense_ndarray(b->ndim, b->shape, NDARRAY_FLOAT);
    mp_float_t *x_arr = (mp_float_t *)x->array;

    if (mp_obj_is_true(args[2].u_obj)) {
        // Solve the lower triangular matrix by iterating each row of A.
        // Start by finding the first unknown using the first row.
        // On finding this unknown, find the second unknown using the second row.
        // Continue the same till the last unknown is found using the last row.

        for (i = 0; i < A_rows; i++) {
            mp_float_t sum = 0.0;
            for (j = 0; j < i; j++) {
                sum += (get_A_ele(A_arr) * (*x_arr++));
                A_arr += A->strides[ULAB_MAX_DIMS - 1];
            }

            sum = (get_b_ele(b_arr) - sum) / (get_A_ele(A_arr));
            *x_arr = sum;

            x_arr -= j;
            A_arr -= A->strides[ULAB_MAX_DIMS - 1] * j;
            A_arr += A->strides[ULAB_MAX_DIMS - 2];
            b_arr += b->strides[ULAB_MAX_DIMS - 1];
        }
    } else {
        // Solve the upper triangular matrix by iterating each row of A.
        // Start by finding the last unknown using the last row.
        // On finding this unknown, find the last-but-one unknown using the last-but-one row.
        // Continue the same till the first unknown is found using the first row.

        A_arr += (A->strides[ULAB_MAX_DIMS - 2] * A_rows);
        b_arr += (b->strides[ULAB_MAX_DIMS - 1] * A_cols);
        x_arr += A_cols;

        for (i = A_rows - 1; i < A_rows; i--) {
            mp_float_t sum = 0.0;
            for (j = i + 1; j < A_cols; j++) {
                sum += (get_A_ele(A_arr) * (*x_arr++));
                A_arr += A->strides[ULAB_MAX_DIMS - 1];
            }

            x_arr -= (j - i);
            A_arr -= (A->strides[ULAB_MAX_DIMS - 1] * (j - i));
            b_arr -= b->strides[ULAB_MAX_DIMS - 1];

            sum = (get_b_ele(b_arr) - sum) / get_A_ele(A_arr);
            *x_arr = sum;

            A_arr -= A->strides[ULAB_MAX_DIMS - 2];
        }
    }

    return MP_OBJ_FROM_PTR(x);
}

MP_DEFINE_CONST_FUN_OBJ_KW(linalg_solve_triangular_obj, 2, solve_triangular);

//| def cho_solve(L: ulab.numpy.ndarray, b: ulab.numpy.ndarray) -> ulab.numpy.ndarray:
//|    """
//|    :param ~ulab.numpy.ndarray L: the lower triangular, Cholesky factorization of A
//|    :param ~ulab.numpy.ndarray b: right-hand-side vector b
//|    :return: solution to the system A x = b. Shape of return matches b
//|    :raises TypeError: if L and b are not of type ndarray and are not dense
//|
//|    Solve the linear equations A x = b, given the Cholesky factorization of A as input"""
//|    ...
//|

static mp_obj_t cho_solve(mp_obj_t _L, mp_obj_t _b) {

    if(!mp_obj_is_type(_L, &ulab_ndarray_type) || !mp_obj_is_type(_b, &ulab_ndarray_type)) {
        mp_raise_TypeError(translate("first two arguments must be ndarrays"));
    }

    ndarray_obj_t *L = MP_OBJ_TO_PTR(_L);
    ndarray_obj_t *b = MP_OBJ_TO_PTR(_b);

    if(!ndarray_is_dense(L) || !ndarray_is_dense(b)) {
        mp_raise_TypeError(translate("input must be a dense ndarray"));
    }

    mp_float_t (*get_L_ele)(void *) = ndarray_get_float_function(L->dtype);
    mp_float_t (*get_b_ele)(void *) = ndarray_get_float_function(b->dtype);
    void (*set_L_ele)(void *, mp_float_t) = ndarray_set_float_function(L->dtype);

    size_t L_rows = L->shape[ULAB_MAX_DIMS - 2];
    size_t L_cols = L->shape[ULAB_MAX_DIMS - 1];

    // Obtain transpose of the input matrix L in L_t
    size_t L_t_shape[ULAB_MAX_DIMS];
    size_t L_t_rows = L_t_shape[ULAB_MAX_DIMS - 2] = L_cols;
    size_t L_t_cols = L_t_shape[ULAB_MAX_DIMS - 1] = L_rows;
    ndarray_obj_t *L_t = ndarray_new_dense_ndarray(L->ndim, L_t_shape, L->dtype);

    uint8_t *L_arr = (uint8_t *)L->array;
    uint8_t *L_t_arr = (uint8_t *)L_t->array;
    uint8_t *b_arr = (uint8_t *)b->array;

    size_t i, j;

    uint8_t *L_ptr = L_arr;
    uint8_t *L_t_ptr = L_t_arr;
    for (i = 0; i < L_rows; i++) {
        for (j = 0; j < L_cols; j++) {
            set_L_ele(L_t_ptr, get_L_ele(L_ptr));
            L_t_ptr += L_t->strides[ULAB_MAX_DIMS - 2];
            L_ptr += L->strides[ULAB_MAX_DIMS - 1];
        }

        L_t_ptr -= j * L_t->strides[ULAB_MAX_DIMS - 2];
        L_t_ptr += L_t->strides[ULAB_MAX_DIMS - 1];
        L_ptr -= j * L->strides[ULAB_MAX_DIMS - 1];
        L_ptr += L->strides[ULAB_MAX_DIMS - 2];
    }

    ndarray_obj_t *x = ndarray_new_dense_ndarray(b->ndim, b->shape, NDARRAY_FLOAT);
    mp_float_t *x_arr = (mp_float_t *)x->array;

    ndarray_obj_t *y = ndarray_new_dense_ndarray(b->ndim, b->shape, NDARRAY_FLOAT);
    mp_float_t *y_arr = (mp_float_t *)y->array;

    // solve L y = b to obtain y, where L_t x = y
    for (i = 0; i < L_rows; i++) {
        mp_float_t sum = 0.0;
        for (j = 0; j < i; j++) {
            sum += (get_L_ele(L_arr) * (*y_arr++));
            L_arr += L->strides[ULAB_MAX_DIMS - 1];
        }

        sum = (get_b_ele(b_arr) - sum) / (get_L_ele(L_arr));
        *y_arr = sum;

        y_arr -= j;
        L_arr -= L->strides[ULAB_MAX_DIMS - 1] * j;
        L_arr += L->strides[ULAB_MAX_DIMS - 2];
        b_arr += b->strides[ULAB_MAX_DIMS - 1];
    }

    // using y, solve L_t x = y to obtain x
    L_t_arr += (L_t->strides[ULAB_MAX_DIMS - 2] * L_t_rows);
    y_arr += L_t_cols;
    x_arr += L_t_cols;

    for (i = L_t_rows - 1; i < L_t_rows; i--) {
        mp_float_t sum = 0.0;
        for (j = i + 1; j < L_t_cols; j++) {
            sum += (get_L_ele(L_t_arr) * (*x_arr++));
            L_t_arr += L_t->strides[ULAB_MAX_DIMS - 1];
        }

        x_arr -= (j - i);
        L_t_arr -= (L_t->strides[ULAB_MAX_DIMS - 1] * (j - i));
        y_arr--;

        sum = ((*y_arr) - sum) / get_L_ele(L_t_arr);
        *x_arr = sum;

        L_t_arr -= L_t->strides[ULAB_MAX_DIMS - 2];
    }

    return MP_OBJ_FROM_PTR(x);
}

MP_DEFINE_CONST_FUN_OBJ_2(linalg_cho_solve_obj, cho_solve);

#endif

static const mp_rom_map_elem_t ulab_scipy_linalg_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_linalg) },
    #if ULAB_MAX_DIMS > 1
        #if ULAB_SCIPY_LINALG_HAS_SOLVE_TRIANGULAR
        { MP_ROM_QSTR(MP_QSTR_solve_triangular), (mp_obj_t)&linalg_solve_triangular_obj },
        #endif
        #if ULAB_SCIPY_LINALG_HAS_CHO_SOLVE
        { MP_ROM_QSTR(MP_QSTR_cho_solve), (mp_obj_t)&linalg_cho_solve_obj },
        #endif
    #endif
};

static MP_DEFINE_CONST_DICT(mp_module_ulab_scipy_linalg_globals, ulab_scipy_linalg_globals_table);

mp_obj_module_t ulab_scipy_linalg_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_scipy_linalg_globals,
};

#endif
