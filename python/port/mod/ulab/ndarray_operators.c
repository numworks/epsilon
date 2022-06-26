/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Zoltán Vörös
*/


#include <math.h>

#include "py/runtime.h"
#include "py/objtuple.h"
#include "ndarray.h"
#include "ndarray_operators.h"
#include "ulab.h"
#include "ulab_tools.h"
#include "numpy/carray/carray.h"

/*
    This file contains the actual implementations of the various
    ndarray operators.

    These are the upcasting rules of the binary operators

    - if complex is supported, and if one of the operarands is a complex, the result is always complex
    - if both operarands are real one of them is a float, then the result is also a float
    - operation on identical types preserves type

    uint8 + int8 => int16
    uint8 + int16 => int16
    uint8 + uint16 => uint16
    int8 + int16 => int16
    int8 + uint16 => uint16
    uint16 + int16 => float
*/

#if NDARRAY_HAS_BINARY_OP_EQUAL | NDARRAY_HAS_BINARY_OP_NOT_EQUAL
mp_obj_t ndarray_binary_equality(ndarray_obj_t *lhs, ndarray_obj_t *rhs,
                                            uint8_t ndim, size_t *shape,  int32_t *lstrides, int32_t *rstrides, mp_binary_op_t op) {

    #if ULAB_SUPPORTS_COMPLEX
    if((lhs->dtype == NDARRAY_COMPLEX) || (rhs->dtype == NDARRAY_COMPLEX))  {
        return carray_binary_equal_not_equal(lhs, rhs, ndim, shape, lstrides, rstrides, op);
    }
    #endif

    ndarray_obj_t *results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_UINT8);
    results->boolean = 1;
    uint8_t *array = (uint8_t *)results->array;
    uint8_t *larray = (uint8_t *)lhs->array;
    uint8_t *rarray = (uint8_t *)rhs->array;

    #if NDARRAY_HAS_BINARY_OP_EQUAL
    if(op == MP_BINARY_OP_EQUAL) {
        if(lhs->dtype == NDARRAY_UINT8) {
            if(rhs->dtype == NDARRAY_UINT8) {
                EQUALITY_LOOP(results, array, uint8_t, uint8_t, larray, lstrides, rarray, rstrides, ==);
            } else if(rhs->dtype == NDARRAY_INT8) {
                EQUALITY_LOOP(results, array, uint8_t, int8_t, larray, lstrides, rarray, rstrides, ==);
            } else if(rhs->dtype == NDARRAY_UINT16) {
                EQUALITY_LOOP(results, array, uint8_t, uint16_t, larray, lstrides, rarray, rstrides, ==);
            } else if(rhs->dtype == NDARRAY_INT16) {
                EQUALITY_LOOP(results, array, uint8_t, int16_t, larray, lstrides, rarray, rstrides, ==);
            } else if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, uint8_t, mp_float_t, larray, lstrides, rarray, rstrides, ==);
            }
        } else if(lhs->dtype == NDARRAY_INT8) {
            if(rhs->dtype == NDARRAY_INT8) {
                EQUALITY_LOOP(results, array, int8_t, int8_t, larray, lstrides, rarray, rstrides, ==);
            } else if(rhs->dtype == NDARRAY_UINT16) {
                EQUALITY_LOOP(results, array, int8_t, uint16_t, larray, lstrides, rarray, rstrides, ==);
            } else if(rhs->dtype == NDARRAY_INT16) {
                EQUALITY_LOOP(results, array, int8_t, int16_t, larray, lstrides, rarray, rstrides, ==);
            } else if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, int8_t, mp_float_t, larray, lstrides, rarray, rstrides, ==);
            } else {
                return ndarray_binary_op(op, rhs, lhs);
            }
        } else if(lhs->dtype == NDARRAY_UINT16) {
            if(rhs->dtype == NDARRAY_UINT16) {
                EQUALITY_LOOP(results, array, uint16_t, uint16_t, larray, lstrides, rarray, rstrides, ==);
            } else if(rhs->dtype == NDARRAY_INT16) {
                EQUALITY_LOOP(results, array, uint16_t, int16_t, larray, lstrides, rarray, rstrides, ==);
            } else if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, uint16_t, mp_float_t, larray, lstrides, rarray, rstrides, ==);
            } else {
                return ndarray_binary_op(op, rhs, lhs);
            }
        } else if(lhs->dtype == NDARRAY_INT16) {
            if(rhs->dtype == NDARRAY_INT16) {
                EQUALITY_LOOP(results, array, int16_t, int16_t, larray, lstrides, rarray, rstrides, ==);
            } else if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, int16_t, mp_float_t, larray, lstrides, rarray, rstrides, ==);
            } else {
                return ndarray_binary_op(op, rhs, lhs);
            }
        } else if(lhs->dtype == NDARRAY_FLOAT) {
            if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, mp_float_t, mp_float_t, larray, lstrides, rarray, rstrides, ==);
            } else {
                return ndarray_binary_op(op, rhs, lhs);
            }
        }
    }
    #endif /* NDARRAY_HAS_BINARY_OP_EQUAL */

    #if NDARRAY_HAS_BINARY_OP_NOT_EQUAL
    if(op == MP_BINARY_OP_NOT_EQUAL) {
        if(lhs->dtype == NDARRAY_UINT8) {
            if(rhs->dtype == NDARRAY_UINT8) {
                EQUALITY_LOOP(results, array, uint8_t, uint8_t, larray, lstrides, rarray, rstrides, !=);
            } else if(rhs->dtype == NDARRAY_INT8) {
                EQUALITY_LOOP(results, array, uint8_t, int8_t, larray, lstrides, rarray, rstrides, !=);
            } else if(rhs->dtype == NDARRAY_UINT16) {
                EQUALITY_LOOP(results, array, uint8_t, uint16_t, larray, lstrides, rarray, rstrides, !=);
            } else if(rhs->dtype == NDARRAY_INT16) {
                EQUALITY_LOOP(results, array, uint8_t, int16_t, larray, lstrides, rarray, rstrides, !=);
            } else if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, uint8_t, mp_float_t, larray, lstrides, rarray, rstrides, !=);
            }
        } else if(lhs->dtype == NDARRAY_INT8) {
            if(rhs->dtype == NDARRAY_INT8) {
                EQUALITY_LOOP(results, array, int8_t, int8_t, larray, lstrides, rarray, rstrides, !=);
            } else if(rhs->dtype == NDARRAY_UINT16) {
                EQUALITY_LOOP(results, array, int8_t, uint16_t, larray, lstrides, rarray, rstrides, !=);
            } else if(rhs->dtype == NDARRAY_INT16) {
                EQUALITY_LOOP(results, array, int8_t, int16_t, larray, lstrides, rarray, rstrides, !=);
            } else if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, int8_t, mp_float_t, larray, lstrides, rarray, rstrides, !=);
            } else {
                return ndarray_binary_op(op, rhs, lhs);
            }
        } else if(lhs->dtype == NDARRAY_UINT16) {
            if(rhs->dtype == NDARRAY_UINT16) {
                EQUALITY_LOOP(results, array, uint16_t, uint16_t, larray, lstrides, rarray, rstrides, !=);
            } else if(rhs->dtype == NDARRAY_INT16) {
                EQUALITY_LOOP(results, array, uint16_t, int16_t, larray, lstrides, rarray, rstrides, !=);
            } else if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, uint16_t, mp_float_t, larray, lstrides, rarray, rstrides, !=);
            } else {
                return ndarray_binary_op(op, rhs, lhs);
            }
        } else if(lhs->dtype == NDARRAY_INT16) {
            if(rhs->dtype == NDARRAY_INT16) {
                EQUALITY_LOOP(results, array, int16_t, int16_t, larray, lstrides, rarray, rstrides, !=);
            } else if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, int16_t, mp_float_t, larray, lstrides, rarray, rstrides, !=);
            } else {
                return ndarray_binary_op(op, rhs, lhs);
            }
        } else if(lhs->dtype == NDARRAY_FLOAT) {
            if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, mp_float_t, mp_float_t, larray, lstrides, rarray, rstrides, !=);
            } else {
                return ndarray_binary_op(op, rhs, lhs);
            }
        }
    }
    #endif /* NDARRAY_HAS_BINARY_OP_NOT_EQUAL */

    return MP_OBJ_FROM_PTR(results);
}
#endif /* NDARRAY_HAS_BINARY_OP_EQUAL | NDARRAY_HAS_BINARY_OP_NOT_EQUAL */

#if NDARRAY_HAS_BINARY_OP_ADD
mp_obj_t ndarray_binary_add(ndarray_obj_t *lhs, ndarray_obj_t *rhs,
                                        uint8_t ndim, size_t *shape, int32_t *lstrides, int32_t *rstrides) {

    #if ULAB_SUPPORTS_COMPLEX
    if((lhs->dtype == NDARRAY_COMPLEX) || (rhs->dtype == NDARRAY_COMPLEX))  {
        return carray_binary_add(lhs, rhs, ndim, shape, lstrides, rstrides);
    }
    #endif

    ndarray_obj_t *results = NULL;
    uint8_t *larray = (uint8_t *)lhs->array;
    uint8_t *rarray = (uint8_t *)rhs->array;

    if(lhs->dtype == NDARRAY_UINT8) {
        if(rhs->dtype == NDARRAY_UINT8) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_UINT16);
            BINARY_LOOP(results, uint16_t, uint8_t, uint8_t, larray, lstrides, rarray, rstrides, +);
        } else if(rhs->dtype == NDARRAY_INT8) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT16);
            BINARY_LOOP(results, int16_t, uint8_t, int8_t, larray, lstrides, rarray, rstrides, +);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_UINT16);
            BINARY_LOOP(results, uint16_t, uint8_t, uint16_t, larray, lstrides, rarray, rstrides, +);
        } else if(rhs->dtype == NDARRAY_INT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT16);
            BINARY_LOOP(results, int16_t, uint8_t, int16_t, larray, lstrides, rarray, rstrides, +);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, uint8_t, mp_float_t, larray, lstrides, rarray, rstrides, +);
        }
    } else if(lhs->dtype == NDARRAY_INT8) {
        if(rhs->dtype == NDARRAY_INT8) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT8);
            BINARY_LOOP(results, int8_t, int8_t, int8_t, larray, lstrides, rarray, rstrides, +);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT16);
            BINARY_LOOP(results, int16_t, int8_t, uint16_t, larray, lstrides, rarray, rstrides, +);
        } else if(rhs->dtype == NDARRAY_INT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT16);
            BINARY_LOOP(results, int16_t, int8_t, int16_t, larray, lstrides, rarray, rstrides, +);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, int8_t, mp_float_t, larray, lstrides, rarray, rstrides, +);
        } else {
            return ndarray_binary_op(MP_BINARY_OP_ADD, rhs, lhs);
        }
    } else if(lhs->dtype == NDARRAY_UINT16) {
        if(rhs->dtype == NDARRAY_UINT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_UINT16);
            BINARY_LOOP(results, uint16_t, uint16_t, uint16_t, larray, lstrides, rarray, rstrides, +);
        } else if(rhs->dtype == NDARRAY_INT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, uint16_t, int16_t, larray, lstrides, rarray, rstrides, +);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, uint16_t, mp_float_t, larray, lstrides, rarray, rstrides, +);
        } else {
            return ndarray_binary_op(MP_BINARY_OP_ADD, rhs, lhs);
        }
    } else if(lhs->dtype == NDARRAY_INT16) {
        if(rhs->dtype == NDARRAY_INT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT16);
            BINARY_LOOP(results, int16_t, int16_t, int16_t, larray, lstrides, rarray, rstrides, +);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, int16_t, mp_float_t, larray, lstrides, rarray, rstrides, +);
        } else {
            return ndarray_binary_op(MP_BINARY_OP_ADD, rhs, lhs);
        }
    } else if(lhs->dtype == NDARRAY_FLOAT) {
        if(rhs->dtype == NDARRAY_FLOAT) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, mp_float_t, mp_float_t, larray, lstrides, rarray, rstrides, +);
        } else {
            return ndarray_binary_op(MP_BINARY_OP_ADD, rhs, lhs);
        }
    }

    return MP_OBJ_FROM_PTR(results);
}
#endif /* NDARRAY_HAS_BINARY_OP_ADD */

#if NDARRAY_HAS_BINARY_OP_MULTIPLY
mp_obj_t ndarray_binary_multiply(ndarray_obj_t *lhs, ndarray_obj_t *rhs,
                                            uint8_t ndim, size_t *shape, int32_t *lstrides, int32_t *rstrides) {

    #if ULAB_SUPPORTS_COMPLEX
    if((lhs->dtype == NDARRAY_COMPLEX) || (rhs->dtype == NDARRAY_COMPLEX))  {
        return carray_binary_multiply(lhs, rhs, ndim, shape, lstrides, rstrides);
    }
    #endif

    ndarray_obj_t *results = NULL;
    uint8_t *larray = (uint8_t *)lhs->array;
    uint8_t *rarray = (uint8_t *)rhs->array;

    if(lhs->dtype == NDARRAY_UINT8) {
        if(rhs->dtype == NDARRAY_UINT8) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_UINT16);
            BINARY_LOOP(results, uint16_t, uint8_t, uint8_t, larray, lstrides, rarray, rstrides, *);
        } else if(rhs->dtype == NDARRAY_INT8) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT16);
            BINARY_LOOP(results, int16_t, uint8_t, int8_t, larray, lstrides, rarray, rstrides, *);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_UINT16);
            BINARY_LOOP(results, uint16_t, uint8_t, uint16_t, larray, lstrides, rarray, rstrides, *);
        } else if(rhs->dtype == NDARRAY_INT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT16);
            BINARY_LOOP(results, int16_t, uint8_t, int16_t, larray, lstrides, rarray, rstrides, *);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, uint8_t, mp_float_t, larray, lstrides, rarray, rstrides, *);
        }
    } else if(lhs->dtype == NDARRAY_INT8) {
        if(rhs->dtype == NDARRAY_INT8) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT8);
            BINARY_LOOP(results, int8_t, int8_t, int8_t, larray, lstrides, rarray, rstrides, *);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT16);
            BINARY_LOOP(results, int16_t, int8_t, uint16_t, larray, lstrides, rarray, rstrides, *);
        } else if(rhs->dtype == NDARRAY_INT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT16);
            BINARY_LOOP(results, int16_t, int8_t, int16_t, larray, lstrides, rarray, rstrides, *);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, int8_t, mp_float_t, larray, lstrides, rarray, rstrides, *);
        } else {
            return ndarray_binary_op(MP_BINARY_OP_MULTIPLY, rhs, lhs);
        }
    } else if(lhs->dtype == NDARRAY_UINT16) {
        if(rhs->dtype == NDARRAY_UINT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_UINT16);
            BINARY_LOOP(results, uint16_t, uint16_t, uint16_t, larray, lstrides, rarray, rstrides, *);
        } else if(rhs->dtype == NDARRAY_INT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, uint16_t, int16_t, larray, lstrides, rarray, rstrides, *);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, uint16_t, mp_float_t, larray, lstrides, rarray, rstrides, *);
        } else {
            return ndarray_binary_op(MP_BINARY_OP_MULTIPLY, rhs, lhs);
        }
    } else if(lhs->dtype == NDARRAY_INT16) {
        if(rhs->dtype == NDARRAY_INT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT16);
            BINARY_LOOP(results, int16_t, int16_t, int16_t, larray, lstrides, rarray, rstrides, *);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, int16_t, mp_float_t, larray, lstrides, rarray, rstrides, *);
        } else {
            return ndarray_binary_op(MP_BINARY_OP_MULTIPLY, rhs, lhs);
        }
    } else if(lhs->dtype == NDARRAY_FLOAT) {
        if(rhs->dtype == NDARRAY_FLOAT) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, mp_float_t, mp_float_t, larray, lstrides, rarray, rstrides, *);
        } else {
            return ndarray_binary_op(MP_BINARY_OP_MULTIPLY, rhs, lhs);
        }
    }

    return MP_OBJ_FROM_PTR(results);
}
#endif /* NDARRAY_HAS_BINARY_OP_MULTIPLY */

#if NDARRAY_HAS_BINARY_OP_MORE | NDARRAY_HAS_BINARY_OP_MORE_EQUAL | NDARRAY_HAS_BINARY_OP_LESS | NDARRAY_HAS_BINARY_OP_LESS_EQUAL
mp_obj_t ndarray_binary_more(ndarray_obj_t *lhs, ndarray_obj_t *rhs,
                                            uint8_t ndim, size_t *shape, int32_t *lstrides, int32_t *rstrides, mp_binary_op_t op) {

    ndarray_obj_t *results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_UINT8);
    results->boolean = 1;
    uint8_t *array = (uint8_t *)results->array;
    uint8_t *larray = (uint8_t *)lhs->array;
    uint8_t *rarray = (uint8_t *)rhs->array;

    #if NDARRAY_HAS_BINARY_OP_MORE | NDARRAY_HAS_BINARY_OP_LESS
    if(op == MP_BINARY_OP_MORE) {
        if(lhs->dtype == NDARRAY_UINT8) {
            if(rhs->dtype == NDARRAY_UINT8) {
                EQUALITY_LOOP(results, array, uint8_t, uint8_t, larray, lstrides, rarray, rstrides, >);
            } else if(rhs->dtype == NDARRAY_INT8) {
                EQUALITY_LOOP(results, array, uint8_t, int8_t, larray, lstrides, rarray, rstrides, >);
            } else if(rhs->dtype == NDARRAY_UINT16) {
                EQUALITY_LOOP(results, array, uint8_t, uint16_t, larray, lstrides, rarray, rstrides, >);
            } else if(rhs->dtype == NDARRAY_INT16) {
                EQUALITY_LOOP(results, array, uint8_t, int16_t, larray, lstrides, rarray, rstrides, >);
            } else if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, uint8_t, mp_float_t, larray, lstrides, rarray, rstrides, >);
            }
        } else if(lhs->dtype == NDARRAY_INT8) {
            if(rhs->dtype == NDARRAY_UINT8) {
                EQUALITY_LOOP(results, array, int8_t, uint8_t, larray, lstrides, rarray, rstrides, >);
            } else if(rhs->dtype == NDARRAY_INT8) {
                EQUALITY_LOOP(results, array, int8_t, int8_t, larray, lstrides, rarray, rstrides, >);
            } else if(rhs->dtype == NDARRAY_UINT16) {
                EQUALITY_LOOP(results, array, int8_t, uint16_t, larray, lstrides, rarray, rstrides, >);
            } else if(rhs->dtype == NDARRAY_INT16) {
                EQUALITY_LOOP(results, array, int8_t, int16_t, larray, lstrides, rarray, rstrides, >);
            } else if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, int8_t, mp_float_t, larray, lstrides, rarray, rstrides, >);
            }
        } else if(lhs->dtype == NDARRAY_UINT16) {
            if(rhs->dtype == NDARRAY_UINT8) {
                EQUALITY_LOOP(results, array, uint16_t, uint8_t, larray, lstrides, rarray, rstrides, >);
            } else if(rhs->dtype == NDARRAY_INT8) {
                EQUALITY_LOOP(results, array, uint16_t, int8_t, larray, lstrides, rarray, rstrides, >);
            } else if(rhs->dtype == NDARRAY_UINT16) {
                EQUALITY_LOOP(results, array, uint16_t, uint16_t, larray, lstrides, rarray, rstrides, >);
            } else if(rhs->dtype == NDARRAY_INT16) {
                EQUALITY_LOOP(results, array, uint16_t, int16_t, larray, lstrides, rarray, rstrides, >);
            } else if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, uint16_t, mp_float_t, larray, lstrides, rarray, rstrides, >);
            }
        } else if(lhs->dtype == NDARRAY_INT16) {
            if(rhs->dtype == NDARRAY_UINT8) {
                EQUALITY_LOOP(results, array, int16_t, uint8_t, larray, lstrides, rarray, rstrides, >);
            } else if(rhs->dtype == NDARRAY_INT8) {
                EQUALITY_LOOP(results, array, int16_t, int8_t, larray, lstrides, rarray, rstrides, >);
            } else if(rhs->dtype == NDARRAY_UINT16) {
                EQUALITY_LOOP(results, array, int16_t, uint16_t, larray, lstrides, rarray, rstrides, >);
            } else if(rhs->dtype == NDARRAY_INT16) {
                EQUALITY_LOOP(results, array, int16_t, int16_t, larray, lstrides, rarray, rstrides, >);
            } else if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, uint16_t, mp_float_t, larray, lstrides, rarray, rstrides, >);
            }
        } else if(lhs->dtype == NDARRAY_FLOAT) {
            if(rhs->dtype == NDARRAY_UINT8) {
                EQUALITY_LOOP(results, array, mp_float_t, uint8_t, larray, lstrides, rarray, rstrides, >);
            } else if(rhs->dtype == NDARRAY_INT8) {
                EQUALITY_LOOP(results, array, mp_float_t, int8_t, larray, lstrides, rarray, rstrides, >);
            } else if(rhs->dtype == NDARRAY_UINT16) {
                EQUALITY_LOOP(results, array, mp_float_t, uint16_t, larray, lstrides, rarray, rstrides, >);
            } else if(rhs->dtype == NDARRAY_INT16) {
                EQUALITY_LOOP(results, array, mp_float_t, int16_t, larray, lstrides, rarray, rstrides, >);
            } else if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, mp_float_t, mp_float_t, larray, lstrides, rarray, rstrides, >);
            }
        }
    }
    #endif /* NDARRAY_HAS_BINARY_OP_MORE | NDARRAY_HAS_BINARY_OP_LESS*/
    #if NDARRAY_HAS_BINARY_OP_MORE_EQUAL | NDARRAY_HAS_BINARY_OP_LESS_EQUAL
    if(op == MP_BINARY_OP_MORE_EQUAL) {
        if(lhs->dtype == NDARRAY_UINT8) {
            if(rhs->dtype == NDARRAY_UINT8) {
                EQUALITY_LOOP(results, array, uint8_t, uint8_t, larray, lstrides, rarray, rstrides, >=);
            } else if(rhs->dtype == NDARRAY_INT8) {
                EQUALITY_LOOP(results, array, uint8_t, int8_t, larray, lstrides, rarray, rstrides, >=);
            } else if(rhs->dtype == NDARRAY_UINT16) {
                EQUALITY_LOOP(results, array, uint8_t, uint16_t, larray, lstrides, rarray, rstrides, >=);
            } else if(rhs->dtype == NDARRAY_INT16) {
                EQUALITY_LOOP(results, array, uint8_t, int16_t, larray, lstrides, rarray, rstrides, >=);
            } else if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, uint8_t, mp_float_t, larray, lstrides, rarray, rstrides, >=);
            }
        } else if(lhs->dtype == NDARRAY_INT8) {
            if(rhs->dtype == NDARRAY_UINT8) {
                EQUALITY_LOOP(results, array, int8_t, uint8_t, larray, lstrides, rarray, rstrides, >=);
            } else if(rhs->dtype == NDARRAY_INT8) {
                EQUALITY_LOOP(results, array, int8_t, int8_t, larray, lstrides, rarray, rstrides, >=);
            } else if(rhs->dtype == NDARRAY_UINT16) {
                EQUALITY_LOOP(results, array, int8_t, uint16_t, larray, lstrides, rarray, rstrides, >=);
            } else if(rhs->dtype == NDARRAY_INT16) {
                EQUALITY_LOOP(results, array, int8_t, int16_t, larray, lstrides, rarray, rstrides, >=);
            } else if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, int8_t, mp_float_t, larray, lstrides, rarray, rstrides, >=);
            }
        } else if(lhs->dtype == NDARRAY_UINT16) {
            if(rhs->dtype == NDARRAY_UINT8) {
                EQUALITY_LOOP(results, array, uint16_t, uint8_t, larray, lstrides, rarray, rstrides, >=);
            } else if(rhs->dtype == NDARRAY_INT8) {
                EQUALITY_LOOP(results, array, uint16_t, int8_t, larray, lstrides, rarray, rstrides, >=);
            } else if(rhs->dtype == NDARRAY_UINT16) {
                EQUALITY_LOOP(results, array, uint16_t, uint16_t, larray, lstrides, rarray, rstrides, >=);
            } else if(rhs->dtype == NDARRAY_INT16) {
                EQUALITY_LOOP(results, array, uint16_t, int16_t, larray, lstrides, rarray, rstrides, >=);
            } else if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, uint16_t, mp_float_t, larray, lstrides, rarray, rstrides, >=);
            }
        } else if(lhs->dtype == NDARRAY_INT16) {
            if(rhs->dtype == NDARRAY_UINT8) {
                EQUALITY_LOOP(results, array, int16_t, uint8_t, larray, lstrides, rarray, rstrides, >=);
            } else if(rhs->dtype == NDARRAY_INT8) {
                EQUALITY_LOOP(results, array, int16_t, int8_t, larray, lstrides, rarray, rstrides, >=);
            } else if(rhs->dtype == NDARRAY_UINT16) {
                EQUALITY_LOOP(results, array, int16_t, uint16_t, larray, lstrides, rarray, rstrides, >=);
            } else if(rhs->dtype == NDARRAY_INT16) {
                EQUALITY_LOOP(results, array, int16_t, int16_t, larray, lstrides, rarray, rstrides, >=);
            } else if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, uint16_t, mp_float_t, larray, lstrides, rarray, rstrides, >=);
            }
        } else if(lhs->dtype == NDARRAY_FLOAT) {
            if(rhs->dtype == NDARRAY_UINT8) {
                EQUALITY_LOOP(results, array, mp_float_t, uint8_t, larray, lstrides, rarray, rstrides, >=);
            } else if(rhs->dtype == NDARRAY_INT8) {
                EQUALITY_LOOP(results, array, mp_float_t, int8_t, larray, lstrides, rarray, rstrides, >=);
            } else if(rhs->dtype == NDARRAY_UINT16) {
                EQUALITY_LOOP(results, array, mp_float_t, uint16_t, larray, lstrides, rarray, rstrides, >=);
            } else if(rhs->dtype == NDARRAY_INT16) {
                EQUALITY_LOOP(results, array, mp_float_t, int16_t, larray, lstrides, rarray, rstrides, >=);
            } else if(rhs->dtype == NDARRAY_FLOAT) {
                EQUALITY_LOOP(results, array, mp_float_t, mp_float_t, larray, lstrides, rarray, rstrides, >=);
            }
        }
    }
    #endif /* NDARRAY_HAS_BINARY_OP_MORE_EQUAL | NDARRAY_HAS_BINARY_OP_LESS_EQUAL */

    return MP_OBJ_FROM_PTR(results);
}
#endif /* NDARRAY_HAS_BINARY_OP_MORE | NDARRAY_HAS_BINARY_OP_MORE_EQUAL | NDARRAY_HAS_BINARY_OP_LESS | NDARRAY_HAS_BINARY_OP_LESS_EQUAL */

#if NDARRAY_HAS_BINARY_OP_SUBTRACT
mp_obj_t ndarray_binary_subtract(ndarray_obj_t *lhs, ndarray_obj_t *rhs,
                                            uint8_t ndim, size_t *shape, int32_t *lstrides, int32_t *rstrides) {

    #if ULAB_SUPPORTS_COMPLEX
    if((lhs->dtype == NDARRAY_COMPLEX) || (rhs->dtype == NDARRAY_COMPLEX))  {
        return carray_binary_subtract(lhs, rhs, ndim, shape, lstrides, rstrides);
    }
    #endif

    ndarray_obj_t *results = NULL;
    uint8_t *larray = (uint8_t *)lhs->array;
    uint8_t *rarray = (uint8_t *)rhs->array;

    if(lhs->dtype == NDARRAY_UINT8) {
        if(rhs->dtype == NDARRAY_UINT8) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_UINT8);
            BINARY_LOOP(results, uint8_t, uint8_t, uint8_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_INT8) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT16);
            BINARY_LOOP(results, int16_t, uint8_t, int8_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_UINT16);
            BINARY_LOOP(results, uint16_t, uint8_t, uint16_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_INT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT16);
            BINARY_LOOP(results, int16_t, uint8_t, int16_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, uint8_t, mp_float_t, larray, lstrides, rarray, rstrides, -);
        }
    } else if(lhs->dtype == NDARRAY_INT8) {
        if(rhs->dtype == NDARRAY_UINT8) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT16);
            BINARY_LOOP(results, int16_t, int8_t, uint8_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_INT8) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT8);
            BINARY_LOOP(results, int8_t, int8_t, int8_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT16);
            BINARY_LOOP(results, int16_t, int8_t, uint16_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_INT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT16);
            BINARY_LOOP(results, int16_t, int8_t, int16_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, int8_t, mp_float_t, larray, lstrides, rarray, rstrides, -);
        }
    } else if(lhs->dtype == NDARRAY_UINT16) {
        if(rhs->dtype == NDARRAY_UINT8) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_UINT16);
            BINARY_LOOP(results, uint16_t, uint16_t, uint8_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_INT8) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_UINT16);
            BINARY_LOOP(results, uint16_t, uint16_t, int8_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_UINT16);
            BINARY_LOOP(results, uint16_t, uint16_t, uint16_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_INT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, uint16_t, int16_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, uint16_t, mp_float_t, larray, lstrides, rarray, rstrides, -);
        }
    } else if(lhs->dtype == NDARRAY_INT16) {
        if(rhs->dtype == NDARRAY_UINT8) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT16);
            BINARY_LOOP(results, int16_t, int16_t, uint8_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_INT8) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT16);
            BINARY_LOOP(results, int16_t, int16_t, int8_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, int16_t, uint16_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_INT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_INT16);
            BINARY_LOOP(results, int16_t, int16_t, int16_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, uint16_t, mp_float_t, larray, lstrides, rarray, rstrides, -);
        }
    } else if(lhs->dtype == NDARRAY_FLOAT) {
        if(rhs->dtype == NDARRAY_UINT8) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, mp_float_t, uint8_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_INT8) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, mp_float_t, int8_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, mp_float_t, uint16_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_INT16) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, mp_float_t, int16_t, larray, lstrides, rarray, rstrides, -);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
            BINARY_LOOP(results, mp_float_t, mp_float_t, mp_float_t, larray, lstrides, rarray, rstrides, -);
        }
    }

    return MP_OBJ_FROM_PTR(results);
}
#endif /* NDARRAY_HAS_BINARY_OP_SUBTRACT */

#if NDARRAY_HAS_BINARY_OP_TRUE_DIVIDE
mp_obj_t ndarray_binary_true_divide(ndarray_obj_t *lhs, ndarray_obj_t *rhs,
                                            uint8_t ndim, size_t *shape, int32_t *lstrides, int32_t *rstrides) {

    #if ULAB_SUPPORTS_COMPLEX
    if((lhs->dtype == NDARRAY_COMPLEX) || (rhs->dtype == NDARRAY_COMPLEX))  {
        return carray_binary_divide(lhs, rhs, ndim, shape, lstrides, rstrides);
    }
    #endif

    ndarray_obj_t *results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
    uint8_t *larray = (uint8_t *)lhs->array;
    uint8_t *rarray = (uint8_t *)rhs->array;

    #if NDARRAY_BINARY_USES_FUN_POINTER
    mp_float_t (*get_lhs)(void *) = ndarray_get_float_function(lhs->dtype);
    mp_float_t (*get_rhs)(void *) = ndarray_get_float_function(rhs->dtype);

    uint8_t *array = (uint8_t *)results->array;
    void (*set_result)(void *, mp_float_t ) = ndarray_set_float_function(NDARRAY_FLOAT);

    // Note that lvalue and rvalue are local variables in the macro itself
    FUNC_POINTER_LOOP(results, array, get_lhs, get_rhs, larray, lstrides, rarray, rstrides, lvalue/rvalue);

    #else
    if(lhs->dtype == NDARRAY_UINT8) {
        if(rhs->dtype == NDARRAY_UINT8) {
            BINARY_LOOP(results, mp_float_t, uint8_t, uint8_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_INT8) {
            BINARY_LOOP(results, mp_float_t, uint8_t, int8_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            BINARY_LOOP(results, mp_float_t, uint8_t, uint16_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_INT16) {
            BINARY_LOOP(results, mp_float_t, uint8_t, int16_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            BINARY_LOOP(results, mp_float_t, uint8_t, mp_float_t, larray, lstrides, rarray, rstrides, /);
        }
    } else if(lhs->dtype == NDARRAY_INT8) {
        if(rhs->dtype == NDARRAY_UINT8) {
            BINARY_LOOP(results, mp_float_t, int8_t, uint8_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_INT8) {
            BINARY_LOOP(results, mp_float_t, int8_t, int8_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            BINARY_LOOP(results, mp_float_t, int8_t, uint16_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_INT16) {
            BINARY_LOOP(results, mp_float_t, int8_t, int16_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            BINARY_LOOP(results, mp_float_t, int8_t, mp_float_t, larray, lstrides, rarray, rstrides, /);
        }
    } else if(lhs->dtype == NDARRAY_UINT16) {
        if(rhs->dtype == NDARRAY_UINT8) {
            BINARY_LOOP(results, mp_float_t, uint16_t, uint8_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_INT8) {
            BINARY_LOOP(results, mp_float_t, uint16_t, int8_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            BINARY_LOOP(results, mp_float_t, uint16_t, uint16_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_INT16) {
            BINARY_LOOP(results, mp_float_t, uint16_t, int16_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            BINARY_LOOP(results, mp_float_t, uint16_t, mp_float_t, larray, lstrides, rarray, rstrides, /);
        }
    } else if(lhs->dtype == NDARRAY_INT16) {
        if(rhs->dtype == NDARRAY_UINT8) {
            BINARY_LOOP(results, mp_float_t, int16_t, uint8_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_INT8) {
            BINARY_LOOP(results, mp_float_t, int16_t, int8_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            BINARY_LOOP(results, mp_float_t, int16_t, uint16_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_INT16) {
            BINARY_LOOP(results, mp_float_t, int16_t, int16_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            BINARY_LOOP(results, mp_float_t, uint16_t, mp_float_t, larray, lstrides, rarray, rstrides, /);
        }
    } else if(lhs->dtype == NDARRAY_FLOAT) {
        if(rhs->dtype == NDARRAY_UINT8) {
            BINARY_LOOP(results, mp_float_t, mp_float_t, uint8_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_INT8) {
            BINARY_LOOP(results, mp_float_t, mp_float_t, int8_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            BINARY_LOOP(results, mp_float_t, mp_float_t, uint16_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_INT16) {
            BINARY_LOOP(results, mp_float_t, mp_float_t, int16_t, larray, lstrides, rarray, rstrides, /);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            BINARY_LOOP(results, mp_float_t, mp_float_t, mp_float_t, larray, lstrides, rarray, rstrides, /);
        }
    }
    #endif /* NDARRAY_BINARY_USES_FUN_POINTER */

    return MP_OBJ_FROM_PTR(results);
}
#endif /* NDARRAY_HAS_BINARY_OP_TRUE_DIVIDE */

#if NDARRAY_HAS_BINARY_OP_POWER
mp_obj_t ndarray_binary_power(ndarray_obj_t *lhs, ndarray_obj_t *rhs,
                                            uint8_t ndim, size_t *shape, int32_t *lstrides, int32_t *rstrides) {

    // Note that numpy upcasts the results to int64, if the inputs are of integer type,
    // while we always return a float array.
    ndarray_obj_t *results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_FLOAT);
    uint8_t *larray = (uint8_t *)lhs->array;
    uint8_t *rarray = (uint8_t *)rhs->array;

    #if NDARRAY_BINARY_USES_FUN_POINTER
    mp_float_t (*get_lhs)(void *) = ndarray_get_float_function(lhs->dtype);
    mp_float_t (*get_rhs)(void *) = ndarray_get_float_function(rhs->dtype);

    uint8_t *array = (uint8_t *)results->array;
    void (*set_result)(void *, mp_float_t ) = ndarray_set_float_function(NDARRAY_FLOAT);

    // Note that lvalue and rvalue are local variables in the macro itself
    FUNC_POINTER_LOOP(results, array, get_lhs, get_rhs, larray, lstrides, rarray, rstrides, MICROPY_FLOAT_C_FUN(pow)(lvalue, rvalue));

    #else
    if(lhs->dtype == NDARRAY_UINT8) {
        if(rhs->dtype == NDARRAY_UINT8) {
            POWER_LOOP(results, mp_float_t, uint8_t, uint8_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_INT8) {
            POWER_LOOP(results, mp_float_t, uint8_t, int8_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            POWER_LOOP(results, mp_float_t, uint8_t, uint16_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_INT16) {
            POWER_LOOP(results, mp_float_t, uint8_t, int16_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            POWER_LOOP(results, mp_float_t, uint8_t, mp_float_t, larray, lstrides, rarray, rstrides);
        }
    } else if(lhs->dtype == NDARRAY_INT8) {
        if(rhs->dtype == NDARRAY_UINT8) {
            POWER_LOOP(results, mp_float_t, int8_t, uint8_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_INT8) {
            POWER_LOOP(results, mp_float_t, int8_t, int8_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            POWER_LOOP(results, mp_float_t, int8_t, uint16_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_INT16) {
            POWER_LOOP(results, mp_float_t, int8_t, int16_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            POWER_LOOP(results, mp_float_t, int8_t, mp_float_t, larray, lstrides, rarray, rstrides);
        }
    } else if(lhs->dtype == NDARRAY_UINT16) {
        if(rhs->dtype == NDARRAY_UINT8) {
            POWER_LOOP(results, mp_float_t, uint16_t, uint8_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_INT8) {
            POWER_LOOP(results, mp_float_t, uint16_t, int8_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            POWER_LOOP(results, mp_float_t, uint16_t, uint16_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_INT16) {
            POWER_LOOP(results, mp_float_t, uint16_t, int16_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            POWER_LOOP(results, mp_float_t, uint16_t, mp_float_t, larray, lstrides, rarray, rstrides);
        }
    } else if(lhs->dtype == NDARRAY_INT16) {
        if(rhs->dtype == NDARRAY_UINT8) {
            POWER_LOOP(results, mp_float_t, int16_t, uint8_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_INT8) {
            POWER_LOOP(results, mp_float_t, int16_t, int8_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            POWER_LOOP(results, mp_float_t, int16_t, uint16_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_INT16) {
            POWER_LOOP(results, mp_float_t, int16_t, int16_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            POWER_LOOP(results, mp_float_t, uint16_t, mp_float_t, larray, lstrides, rarray, rstrides);
        }
    } else if(lhs->dtype == NDARRAY_FLOAT) {
        if(rhs->dtype == NDARRAY_UINT8) {
            POWER_LOOP(results, mp_float_t, mp_float_t, uint8_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_INT8) {
            POWER_LOOP(results, mp_float_t, mp_float_t, int8_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_UINT16) {
            POWER_LOOP(results, mp_float_t, mp_float_t, uint16_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_INT16) {
            POWER_LOOP(results, mp_float_t, mp_float_t, int16_t, larray, lstrides, rarray, rstrides);
        } else if(rhs->dtype == NDARRAY_FLOAT) {
            POWER_LOOP(results, mp_float_t, mp_float_t, mp_float_t, larray, lstrides, rarray, rstrides);
        }
    }
    #endif /* NDARRAY_BINARY_USES_FUN_POINTER */

    return MP_OBJ_FROM_PTR(results);
}
#endif /* NDARRAY_HAS_BINARY_OP_POWER */

#if NDARRAY_HAS_INPLACE_ADD || NDARRAY_HAS_INPLACE_MULTIPLY || NDARRAY_HAS_INPLACE_SUBTRACT
mp_obj_t ndarray_inplace_ams(ndarray_obj_t *lhs, ndarray_obj_t *rhs, int32_t *rstrides, uint8_t optype) {

    if((lhs->dtype != NDARRAY_FLOAT) && (rhs->dtype == NDARRAY_FLOAT)) {
        mp_raise_TypeError(translate("cannot cast output with casting rule"));
    }
    uint8_t *larray = (uint8_t *)lhs->array;
    uint8_t *rarray = (uint8_t *)rhs->array;

    #if NDARRAY_HAS_INPLACE_ADD
    if(optype == MP_BINARY_OP_INPLACE_ADD) {
        UNWRAP_INPLACE_OPERATOR(lhs, larray, rarray, rstrides, +=);
    }
    #endif
    #if NDARRAY_HAS_INPLACE_ADD
    if(optype == MP_BINARY_OP_INPLACE_MULTIPLY) {
        UNWRAP_INPLACE_OPERATOR(lhs, larray, rarray, rstrides, *=);
    }
    #endif
    #if NDARRAY_HAS_INPLACE_SUBTRACT
    if(optype == MP_BINARY_OP_INPLACE_SUBTRACT) {
        UNWRAP_INPLACE_OPERATOR(lhs, larray, rarray, rstrides, -=);
    }
    #endif

    return MP_OBJ_FROM_PTR(lhs);
}
#endif /* NDARRAY_HAS_INPLACE_ADD || NDARRAY_HAS_INPLACE_MULTIPLY || NDARRAY_HAS_INPLACE_SUBTRACT */

#if NDARRAY_HAS_INPLACE_TRUE_DIVIDE
mp_obj_t ndarray_inplace_divide(ndarray_obj_t *lhs, ndarray_obj_t *rhs, int32_t *rstrides) {

    if((lhs->dtype != NDARRAY_FLOAT)) {
        mp_raise_TypeError(translate("results cannot be cast to specified type"));
    }
    uint8_t *larray = (uint8_t *)lhs->array;
    uint8_t *rarray = (uint8_t *)rhs->array;

    if(rhs->dtype == NDARRAY_UINT8) {
        INPLACE_LOOP(lhs, mp_float_t, uint8_t, larray, rarray, rstrides, /=);
    } else if(rhs->dtype == NDARRAY_INT8) {
        INPLACE_LOOP(lhs, mp_float_t, int8_t, larray, rarray, rstrides, /=);
    } else if(rhs->dtype == NDARRAY_UINT16) {
        INPLACE_LOOP(lhs, mp_float_t, uint16_t, larray, rarray, rstrides, /=);
    } else if(rhs->dtype == NDARRAY_INT16) {
        INPLACE_LOOP(lhs, mp_float_t, int16_t, larray, rarray, rstrides, /=);
    } else if(lhs->dtype == NDARRAY_FLOAT) {
        INPLACE_LOOP(lhs, mp_float_t, mp_float_t, larray, rarray, rstrides, /=);
    }
    return MP_OBJ_FROM_PTR(lhs);
}
#endif /* NDARRAY_HAS_INPLACE_DIVIDE */

#if NDARRAY_HAS_INPLACE_POWER
mp_obj_t ndarray_inplace_power(ndarray_obj_t *lhs, ndarray_obj_t *rhs, int32_t *rstrides) {

    if((lhs->dtype != NDARRAY_FLOAT)) {
        mp_raise_TypeError(translate("results cannot be cast to specified type"));
    }
    uint8_t *larray = (uint8_t *)lhs->array;
    uint8_t *rarray = (uint8_t *)rhs->array;

    if(rhs->dtype == NDARRAY_UINT8) {
        INPLACE_POWER(lhs, mp_float_t, uint8_t, larray, rarray, rstrides);
    } else if(rhs->dtype == NDARRAY_INT8) {
        INPLACE_POWER(lhs, mp_float_t, int8_t, larray, rarray, rstrides);
    } else if(rhs->dtype == NDARRAY_UINT16) {
        INPLACE_POWER(lhs, mp_float_t, uint16_t, larray, rarray, rstrides);
    } else if(rhs->dtype == NDARRAY_INT16) {
        INPLACE_POWER(lhs, mp_float_t, int16_t, larray, rarray, rstrides);
    } else if(lhs->dtype == NDARRAY_FLOAT) {
        INPLACE_POWER(lhs, mp_float_t, mp_float_t, larray, rarray, rstrides);
    }
    return MP_OBJ_FROM_PTR(lhs);
}
#endif /* NDARRAY_HAS_INPLACE_POWER */
