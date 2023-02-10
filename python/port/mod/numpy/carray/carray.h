
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021-2022 Zoltán Vörös
*/

#ifndef _CARRAY_
#define _CARRAY_

MP_DECLARE_CONST_FUN_OBJ_1(carray_real_obj);
MP_DECLARE_CONST_FUN_OBJ_1(carray_imag_obj);
MP_DECLARE_CONST_FUN_OBJ_1(carray_conjugate_obj);
MP_DECLARE_CONST_FUN_OBJ_1(carray_sort_complex_obj);


mp_obj_t carray_imag(mp_obj_t );
mp_obj_t carray_real(mp_obj_t );

mp_obj_t carray_abs(ndarray_obj_t *, ndarray_obj_t *);
mp_obj_t carray_binary_add(ndarray_obj_t *, ndarray_obj_t *, uint8_t , size_t *, int32_t *, int32_t *);
mp_obj_t carray_binary_multiply(ndarray_obj_t *, ndarray_obj_t *, uint8_t , size_t *, int32_t *, int32_t *);
mp_obj_t carray_binary_subtract(ndarray_obj_t *, ndarray_obj_t *, uint8_t , size_t *, int32_t *, int32_t *);
mp_obj_t carray_binary_divide(ndarray_obj_t *, ndarray_obj_t *, uint8_t , size_t *, int32_t *, int32_t *);
mp_obj_t carray_binary_equal_not_equal(ndarray_obj_t *, ndarray_obj_t *, uint8_t , size_t *, int32_t *, int32_t *, mp_binary_op_t );

#define BINARY_LOOP_COMPLEX1(results, resarray, type_right, larray, lstrides, rarray, rstrides, OPERATOR)\
    size_t l = 0;\
    do {\
        *(resarray) = *((mp_float_t *)(larray)) OPERATOR *((type_right *)(rarray));\
        (resarray) += 2;\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 1];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
        l++;\
    } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\

#define BINARY_LOOP_COMPLEX2(results, resarray, type_right, larray, lstrides, rarray, rstrides, OPERATOR)\
    size_t k = 0;\
    do {\
        BINARY_LOOP_COMPLEX1((results), (resarray), type_right, (larray), (lstrides), (rarray), (rstrides), OPERATOR);\
        (larray) -= (lstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS - 1];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 2];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS - 1];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
        k++;\
    } while(k < (results)->shape[ULAB_MAX_DIMS - 2]);\

#define BINARY_LOOP_COMPLEX3(results, resarray, type_right, larray, lstrides, rarray, rstrides, OPERATOR)\
    size_t j = 0;\
    do {\
        BINARY_LOOP_COMPLEX2((results), (resarray), type_right, (larray), (lstrides), (rarray), (rstrides), OPERATOR);\
        (larray) -= (lstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS - 2];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 3];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS - 2];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 3];\
        j++;\
    } while(j < (results)->shape[ULAB_MAX_DIMS - 3]);\

#define BINARY_LOOP_COMPLEX4(results, resarray, type_right, larray, lstrides, rarray, rstrides, OPERATOR)\
    size_t i = 0;\
    do {\
        BINARY_LOOP_COMPLEX3((results), (resarray), type_right, (larray), (lstrides), (rarray), (rstrides), OPERATOR);\
        (larray) -= (lstrides)[ULAB_MAX_DIMS - 3] * (results)->shape[ULAB_MAX_DIMS - 3];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 4];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 3] * (results)->shape[ULAB_MAX_DIMS - 3];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 4];\
        i++;\
    } while(i < (results)->shape[ULAB_MAX_DIMS - 4]);\

#define BINARY_LOOP_COMPLEX_REVERSED_SUBTRACT1(results, resarray, type_left, larray, lstrides, rarray, rstrides)\
    size_t l = 0;\
    do {\
        *(resarray)++ = *((type_left *)(larray)) - (rarray)[0];\
        *(resarray)++ = -(rarray)[1];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 1];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
        l++;\
    } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\

#define BINARY_LOOP_COMPLEX_REVERSED_SUBTRACT2(results, resarray, type_left, larray, lstrides, rarray, rstrides)\
    size_t k = 0;\
    do {\
        BINARY_LOOP_COMPLEX_REVERSED_SUBTRACT1((results), (resarray), type_left, (larray), (lstrides), (rarray), (rstrides));\
        (larray) -= (lstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 2];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
        k++;\
    } while(k < (results)->shape[ULAB_MAX_DIMS - 2]);\

#define BINARY_LOOP_COMPLEX_REVERSED_SUBTRACT3(results, resarray, type_left, larray, lstrides, rarray, rstrides)\
    size_t j = 0;\
    do {\
        BINARY_LOOP_COMPLEX_REVERSED_SUBTRACT2((results), (resarray), type_left, (larray), (lstrides), (rarray), (rstrides));\
        (larray) -= (lstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS - 2];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 3];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS - 2];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 3];\
        j++;\
    } while(j < (results)->shape[ULAB_MAX_DIMS - 3]);\

#define BINARY_LOOP_COMPLEX_REVERSED_SUBTRACT4(results, resarray, type_left, larray, lstrides, rarray, rstrides)\
    size_t i = 0;\
    do {\
        BINARY_LOOP_COMPLEX_REVERSED_SUBTRACT3((results), (resarray), type_left, (larray), (lstrides), (rarray), (rstrides));\
        (larray) -= (lstrides)[ULAB_MAX_DIMS - 3] * (results)->shape[ULAB_MAX_DIMS - 3];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 4];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 3] * (results)->shape[ULAB_MAX_DIMS - 3];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 4];\
        i++;\
    } while(i < (results)->shape[ULAB_MAX_DIMS - 4]);\

#define BINARY_LOOP_COMPLEX_RIGHT_DIVIDE1(results, resarray, type_left, larray, lstrides, rarray, rstrides)\
    size_t l = 0;\
    do {\
        mp_float_t *c = (mp_float_t *)(rarray);\
        mp_float_t denom = c[0] * c[0] + c[1] * c[1];\
        mp_float_t a = *((type_left *)(larray)) / denom;\
        *(resarray)++ = a * c[0];\
        *(resarray)++ = -a * c[1];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 1];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
        l++;\
    } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\

#define BINARY_LOOP_COMPLEX_RIGHT_DIVIDE2(results, resarray, type_left, larray, lstrides, rarray, rstrides)\
    size_t k = 0;\
    do {\
        BINARY_LOOP_COMPLEX_RIGHT_DIVIDE1((results), (resarray), type_left, (larray), (lstrides), (rarray), (rstrides));\
        (larray) -= (lstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS - 1];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 2];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS - 1];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
        k++;\
    } while(k < (results)->shape[ULAB_MAX_DIMS - 2]);\

#define BINARY_LOOP_COMPLEX_RIGHT_DIVIDE3(results, resarray, type_left, larray, lstrides, rarray, rstrides)\
    size_t j = 0;\
    do {\
        BINARY_LOOP_COMPLEX_RIGHT_DIVIDE2((results), (resarray), type_left, (larray), (lstrides), (rarray), (rstrides));\
        (larray) -= (lstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS - 2];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 3];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS - 2];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 3];\
        j++;\
    } while(j < (results)->shape[ULAB_MAX_DIMS - 3]);\

#define BINARY_LOOP_COMPLEX_RIGHT_DIVIDE4(results, resarray, type_left, larray, lstrides, rarray, rstrides)\
    size_t i = 0;\
    do {\
        BINARY_LOOP_COMPLEX_RIGHT_DIVIDE3((results), (resarray), type_left, (larray), (lstrides), (rarray), (rstrides));\
        (larray) -= (lstrides)[ULAB_MAX_DIMS - 3] * (results)->shape[ULAB_MAX_DIMS - 3];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 4];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 3] * (results)->shape[ULAB_MAX_DIMS - 3];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 4];\
        i++;\
    } while(i < (results)->shape[ULAB_MAX_DIMS - 4]);\


#define BINARY_LOOP_COMPLEX_EQUAL1(results, array, type_right, larray, lstrides, rarray, rstrides)\
    size_t l = 0;\
    do {\
        if((*(larray) == *((type_right *)(rarray))) && ((larray)[1] == MICROPY_FLOAT_CONST(0.0))) {\
            *(array) ^= 0x01;\
        }\
        (array)++;\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 1];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
        l++;\
    } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\

#define BINARY_LOOP_COMPLEX_EQUAL2(results, array, type_right, larray, lstrides, rarray, rstrides)\
    size_t k = 0;\
    do {\
        BINARY_LOOP_COMPLEX_EQUAL1((results), (array), type_right, (larray), (lstrides), (rarray), (rstrides));\
        (larray) -= (lstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS - 1];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 2];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS - 1];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
        k++;\
    } while(k < (results)->shape[ULAB_MAX_DIMS - 2]);\

#define BINARY_LOOP_COMPLEX_EQUAL3(results, array, type_right, larray, lstrides, rarray, rstrides)\
    size_t j = 0;\
    do {\
        BINARY_LOOP_COMPLEX_EQUAL2((results), (array), type_right, (larray), (lstrides), (rarray), (rstrides));\
        (larray) -= (lstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS - 2];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 3];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS - 2];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 3];\
        j++;\
    } while(j < (results)->shape[ULAB_MAX_DIMS - 3]);\

#define BINARY_LOOP_COMPLEX_EQUAL4(results, array, type_right, larray, lstrides, rarray, rstrides)\
    size_t i = 0;\
    do {\
        BINARY_LOOP_COMPLEX_EQUAL3((results), (array), type_right, (larray), (lstrides), (rarray), (rstrides));\
        (larray) -= (lstrides)[ULAB_MAX_DIMS - 3] * (results)->shape[ULAB_MAX_DIMS - 3];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 4];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 3] * (results)->shape[ULAB_MAX_DIMS - 3];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 4];\
        i++;\
    } while(i < (results)->shape[ULAB_MAX_DIMS - 4]);\

#if ULAB_MAX_DIMS == 1
#define BINARY_LOOP_COMPLEX BINARY_LOOP_COMPLEX1
#define BINARY_LOOP_COMPLEX_REVERSED_SUBTRACT BINARY_LOOP_COMPLEX_REVERSED_SUBTRACT1
#define BINARY_LOOP_COMPLEX_RIGHT_DIVIDE BINARY_LOOP_COMPLEX_RIGHT_DIVIDE1
#define BINARY_LOOP_COMPLEX_EQUAL BINARY_LOOP_COMPLEX_EQUAL1
#endif /* ULAB_MAX_DIMS == 1 */

#if ULAB_MAX_DIMS == 2
#define BINARY_LOOP_COMPLEX BINARY_LOOP_COMPLEX2
#define BINARY_LOOP_COMPLEX_REVERSED_SUBTRACT BINARY_LOOP_COMPLEX_REVERSED_SUBTRACT2
#define BINARY_LOOP_COMPLEX_RIGHT_DIVIDE BINARY_LOOP_COMPLEX_RIGHT_DIVIDE2
#define BINARY_LOOP_COMPLEX_EQUAL BINARY_LOOP_COMPLEX_EQUAL2
#endif /* ULAB_MAX_DIMS == 2 */

#if ULAB_MAX_DIMS == 3
#define BINARY_LOOP_COMPLEX BINARY_LOOP_COMPLEX3
#define BINARY_LOOP_COMPLEX_REVERSED_SUBTRACT BINARY_LOOP_COMPLEX_REVERSED_SUBTRACT3
#define BINARY_LOOP_COMPLEX_RIGHT_DIVIDE BINARY_LOOP_COMPLEX_RIGHT_DIVIDE3
#define BINARY_LOOP_COMPLEX_EQUAL BINARY_LOOP_COMPLEX_EQUAL3
#endif /* ULAB_MAX_DIMS == 3 */

#if ULAB_MAX_DIMS == 4
#define BINARY_LOOP_COMPLEX BINARY_LOOP_COMPLEX4
#define BINARY_LOOP_COMPLEX_REVERSED_SUBTRACT BINARY_LOOP_COMPLEX_REVERSED_SUBTRACT4
#define BINARY_LOOP_COMPLEX_RIGHT_DIVIDE BINARY_LOOP_COMPLEX_RIGHT_DIVIDE4
#define BINARY_LOOP_COMPLEX_EQUAL BINARY_LOOP_COMPLEX_EQUAL4
#endif /* ULAB_MAX_DIMS == 4 */

#endif
