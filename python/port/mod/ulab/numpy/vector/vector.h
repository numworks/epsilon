
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2021 Zoltán Vörös
*/

#ifndef _VECTOR_
#define _VECTOR_

#include "../../ulab.h"
#include "../../ndarray.h"

MP_DECLARE_CONST_FUN_OBJ_1(vectorise_acos_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_acosh_obj);
MP_DECLARE_CONST_FUN_OBJ_2(vectorise_arctan2_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(vectorise_around_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_asin_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_asinh_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_atan_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_atanh_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_ceil_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_cos_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_cosh_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_degrees_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_erf_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_erfc_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_exp_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_expm1_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_floor_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_gamma_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_lgamma_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_log_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_log10_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_log2_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_radians_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_sin_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_sinh_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_sqrt_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_tan_obj);
MP_DECLARE_CONST_FUN_OBJ_1(vectorise_tanh_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(vectorise_vectorize_obj);

typedef struct _vectorized_function_obj_t {
    mp_obj_base_t base;
    uint8_t otypes;
    mp_obj_t fun;
    const mp_obj_type_t *type;
} vectorized_function_obj_t;

#if ULAB_HAS_FUNCTION_ITERATOR
#define ITERATE_VECTOR(type, array, source, sarray)\
({\
    size_t *scoords = ndarray_new_coords((source)->ndim);\
    for(size_t i=0; i < (source)->len/(source)->shape[ULAB_MAX_DIMS -1]; i++) {\
        for(size_t l=0; l < (source)->shape[ULAB_MAX_DIMS - 1]; l++) {\
            *(array)++ = f(*((type *)(sarray)));\
            (sarray) += (source)->strides[ULAB_MAX_DIMS - 1];\
        }\
        ndarray_rewind_array((source)->ndim, sarray, (source)->shape, (source)->strides, scoords);\
    }\
})

#else

#if ULAB_MAX_DIMS == 4
#define ITERATE_VECTOR(type, array, source, sarray) do {\
    size_t i=0;\
    do {\
        size_t j = 0;\
        do {\
            size_t k = 0;\
            do {\
                size_t l = 0;\
                do {\
                    *(array)++ = f(*((type *)(sarray)));\
                    (sarray) += (source)->strides[ULAB_MAX_DIMS - 1];\
                    l++;\
                } while(l < (source)->shape[ULAB_MAX_DIMS-1]);\
                (sarray) -= (source)->strides[ULAB_MAX_DIMS - 1] * (source)->shape[ULAB_MAX_DIMS-1];\
                (sarray) += (source)->strides[ULAB_MAX_DIMS - 2];\
                k++;\
            } while(k < (source)->shape[ULAB_MAX_DIMS-2]);\
            (sarray) -= (source)->strides[ULAB_MAX_DIMS - 2] * (source)->shape[ULAB_MAX_DIMS-2];\
            (sarray) += (source)->strides[ULAB_MAX_DIMS - 3];\
            j++;\
        } while(j < (source)->shape[ULAB_MAX_DIMS-3]);\
        (sarray) -= (source)->strides[ULAB_MAX_DIMS - 3] * (source)->shape[ULAB_MAX_DIMS-3];\
        (sarray) += (source)->strides[ULAB_MAX_DIMS - 4];\
        i++;\
    } while(i < (source)->shape[ULAB_MAX_DIMS-4]);\
} while(0)
#endif /* ULAB_MAX_DIMS == 4 */

#if ULAB_MAX_DIMS == 3
#define ITERATE_VECTOR(type, array, source, sarray) do {\
    size_t j = 0;\
    do {\
        size_t k = 0;\
        do {\
            size_t l = 0;\
            do {\
                *(array)++ = f(*((type *)(sarray)));\
                (sarray) += (source)->strides[ULAB_MAX_DIMS - 1];\
                l++;\
            } while(l < (source)->shape[ULAB_MAX_DIMS-1]);\
            (sarray) -= (source)->strides[ULAB_MAX_DIMS - 1] * (source)->shape[ULAB_MAX_DIMS-1];\
            (sarray) += (source)->strides[ULAB_MAX_DIMS - 2];\
            k++;\
        } while(k < (source)->shape[ULAB_MAX_DIMS-2]);\
        (sarray) -= (source)->strides[ULAB_MAX_DIMS - 2] * (source)->shape[ULAB_MAX_DIMS-2];\
        (sarray) += (source)->strides[ULAB_MAX_DIMS - 3];\
        j++;\
    } while(j < (source)->shape[ULAB_MAX_DIMS-3]);\
} while(0)
#endif /* ULAB_MAX_DIMS == 3 */

#if ULAB_MAX_DIMS == 2
#define ITERATE_VECTOR(type, array, source, sarray) do {\
    size_t k = 0;\
    do {\
        size_t l = 0;\
        do {\
            *(array)++ = f(*((type *)(sarray)));\
            (sarray) += (source)->strides[ULAB_MAX_DIMS - 1];\
            l++;\
        } while(l < (source)->shape[ULAB_MAX_DIMS-1]);\
        (sarray) -= (source)->strides[ULAB_MAX_DIMS - 1] * (source)->shape[ULAB_MAX_DIMS-1];\
        (sarray) += (source)->strides[ULAB_MAX_DIMS - 2];\
        k++;\
    } while(k < (source)->shape[ULAB_MAX_DIMS-2]);\
} while(0)
#endif /* ULAB_MAX_DIMS == 2 */

#if ULAB_MAX_DIMS == 1
#define ITERATE_VECTOR(type, array, source, sarray) do {\
    size_t l = 0;\
    do {\
        *(array)++ = f(*((type *)(sarray)));\
        (sarray) += (source)->strides[ULAB_MAX_DIMS - 1];\
        l++;\
    } while(l < (source)->shape[ULAB_MAX_DIMS-1]);\
} while(0)
#endif /* ULAB_MAX_DIMS == 1 */
#endif /* ULAB_HAS_FUNCTION_ITERATOR */

#define MATH_FUN_1(py_name, c_name) \
    static mp_obj_t vectorise_ ## py_name(mp_obj_t x_obj) { \
        return vectorise_generic_vector(x_obj, MICROPY_FLOAT_C_FUN(c_name)); \
}

#endif /* _VECTOR_ */
