
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Zoltán Vörös
*/

#ifndef _COMPARE_
#define _COMPARE_

#include "../ulab.h"
#include "../ndarray.h"

enum COMPARE_FUNCTION_TYPE {
    COMPARE_EQUAL,
    COMPARE_NOT_EQUAL,
    COMPARE_MINIMUM,
    COMPARE_MAXIMUM,
    COMPARE_CLIP,
};

MP_DECLARE_CONST_FUN_OBJ_3(compare_clip_obj);
MP_DECLARE_CONST_FUN_OBJ_2(compare_equal_obj);
MP_DECLARE_CONST_FUN_OBJ_2(compare_isfinite_obj);
MP_DECLARE_CONST_FUN_OBJ_2(compare_isinf_obj);
MP_DECLARE_CONST_FUN_OBJ_2(compare_minimum_obj);
MP_DECLARE_CONST_FUN_OBJ_2(compare_maximum_obj);
MP_DECLARE_CONST_FUN_OBJ_2(compare_not_equal_obj);
MP_DECLARE_CONST_FUN_OBJ_3(compare_where_obj);

#if ULAB_MAX_DIMS == 1
#define COMPARE_LOOP(results, array, type_out, type_left, type_right, larray, lstrides, rarray, rstrides, OPERATOR)\
    size_t l = 0;\
    do {\
        *((type_out *)(array)) = *((type_left *)(larray)) OPERATOR *((type_right *)(rarray)) ? (type_out)(*((type_left *)(larray))) : (type_out)(*((type_right *)(rarray)));\
        (array) += (results)->strides[ULAB_MAX_DIMS - 1];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 1];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
        l++;\
    } while(l <  results->shape[ULAB_MAX_DIMS - 1]);\
    return MP_OBJ_FROM_PTR(results);\

#endif // ULAB_MAX_DIMS == 1

#if ULAB_MAX_DIMS == 2
#define COMPARE_LOOP(results, array, type_out, type_left, type_right, larray, lstrides, rarray, rstrides, OPERATOR)\
    size_t k = 0;\
    do {\
        size_t l = 0;\
        do {\
            *((type_out *)(array)) = *((type_left *)(larray)) OPERATOR *((type_right *)(rarray)) ? (type_out)(*((type_left *)(larray))) : (type_out)(*((type_right *)(rarray)));\
            (array) += (results)->strides[ULAB_MAX_DIMS - 1];\
            (larray) += (lstrides)[ULAB_MAX_DIMS - 1];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
            l++;\
        } while(l <  results->shape[ULAB_MAX_DIMS - 1]);\
        (larray) -= (lstrides)[ULAB_MAX_DIMS - 1] * results->shape[ULAB_MAX_DIMS-1];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 2];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * results->shape[ULAB_MAX_DIMS-1];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
        k++;\
    } while(k <  results->shape[ULAB_MAX_DIMS - 2]);\
    return MP_OBJ_FROM_PTR(results);\

#endif // ULAB_MAX_DIMS == 2

#if ULAB_MAX_DIMS == 3
#define COMPARE_LOOP(results, array, type_out, type_left, type_right, larray, lstrides, rarray, rstrides, OPERATOR)\
    size_t j = 0;\
    do {\
        size_t k = 0;\
        do {\
            size_t l = 0;\
            do {\
                *((type_out *)(array)) = *((type_left *)(larray)) OPERATOR *((type_right *)(rarray)) ? (type_out)(*((type_left *)(larray))) : (type_out)(*((type_right *)(rarray)));\
                (array) += (results)->strides[ULAB_MAX_DIMS - 1];\
                (larray) += (lstrides)[ULAB_MAX_DIMS - 1];\
                (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
                l++;\
            } while(l <  results->shape[ULAB_MAX_DIMS - 1]);\
            (larray) -= (lstrides)[ULAB_MAX_DIMS - 1] * results->shape[ULAB_MAX_DIMS-1];\
            (larray) += (lstrides)[ULAB_MAX_DIMS - 2];\
            (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * results->shape[ULAB_MAX_DIMS-1];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
            k++;\
        } while(k <  results->shape[ULAB_MAX_DIMS - 2]);\
        (larray) -= (lstrides)[ULAB_MAX_DIMS - 2] * results->shape[ULAB_MAX_DIMS-2];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 3];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 2] * results->shape[ULAB_MAX_DIMS-2];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 3];\
        j++;\
    } while(j <  results->shape[ULAB_MAX_DIMS - 3]);\
    return MP_OBJ_FROM_PTR(results);\

#endif // ULAB_MAX_DIMS == 3

#if ULAB_MAX_DIMS == 4
#define COMPARE_LOOP(results, array, type_out, type_left, type_right, larray, lstrides, rarray, rstrides, OPERATOR)\
    size_t i = 0;\
    do {\
        size_t j = 0;\
        do {\
            size_t k = 0;\
            do {\
                size_t l = 0;\
                do {\
                    *((type_out *)(array)) = *((type_left *)(larray)) OPERATOR *((type_right *)(rarray)) ? (type_out)(*((type_left *)(larray))) : (type_out)(*((type_right *)(rarray)));\
                    (array) += (results)->strides[ULAB_MAX_DIMS - 1];\
                    (larray) += (lstrides)[ULAB_MAX_DIMS - 1];\
                    (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
                    l++;\
                } while(l <  results->shape[ULAB_MAX_DIMS - 1]);\
                (larray) -= (lstrides)[ULAB_MAX_DIMS - 1] * results->shape[ULAB_MAX_DIMS-1];\
                (larray) += (lstrides)[ULAB_MAX_DIMS - 2];\
                (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * results->shape[ULAB_MAX_DIMS-1];\
                (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
                k++;\
            } while(k <  results->shape[ULAB_MAX_DIMS - 2]);\
            (larray) -= (lstrides)[ULAB_MAX_DIMS - 2] * results->shape[ULAB_MAX_DIMS-2];\
            (larray) += (lstrides)[ULAB_MAX_DIMS - 3];\
            (rarray) -= (rstrides)[ULAB_MAX_DIMS - 2] * results->shape[ULAB_MAX_DIMS-2];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 3];\
            j++;\
        } while(j <  results->shape[ULAB_MAX_DIMS - 3]);\
        (larray) -= (lstrides)[ULAB_MAX_DIMS - 3] * results->shape[ULAB_MAX_DIMS-3];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 4];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 3] * results->shape[ULAB_MAX_DIMS-3];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 4];\
        i++;\
    } while(i <  results->shape[ULAB_MAX_DIMS - 4]);\
    return MP_OBJ_FROM_PTR(results);\

#endif // ULAB_MAX_DIMS == 4

#define RUN_COMPARE_LOOP(dtype, type_out, type_left, type_right, larray, lstrides, rarray, rstrides, ndim, shape, op) do {\
    ndarray_obj_t *results = ndarray_new_dense_ndarray((ndim), (shape), (dtype));\
    uint8_t *array = (uint8_t *)results->array;\
    if((op) == COMPARE_MINIMUM) {\
        COMPARE_LOOP(results, array, type_out, type_left, type_right, larray, lstrides, rarray, rstrides, <);\
    }\
    if((op) == COMPARE_MAXIMUM) {\
        COMPARE_LOOP(results, array, type_out, type_left, type_right, larray, lstrides, rarray, rstrides, >);\
    }\
} while(0)

#endif
