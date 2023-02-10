
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021-2022 Zoltán Vörös
*/

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "py/obj.h"
#include "py/objint.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/misc.h"

#include "../../ulab.h"
#include "../../ndarray.h"
#include "../../ulab_tools.h"
#include "carray.h"

#if ULAB_SUPPORTS_COMPLEX

//| import ulab.numpy

//| def real(val):
//|     """
//|     Return the real part of the complex argument, which can be
//|     either an ndarray, or a scalar."""
//|     ...
//|

mp_obj_t carray_real(mp_obj_t _source) {
    if(mp_obj_is_type(_source, &ulab_ndarray_type)) {
        ndarray_obj_t *source = MP_OBJ_TO_PTR(_source);
        if(source->dtype != NDARRAY_COMPLEX) {
            ndarray_obj_t *target = ndarray_new_dense_ndarray(source->ndim, source->shape, source->dtype);
            ndarray_copy_array(source, target, 0);
            return MP_OBJ_FROM_PTR(target);
        } else { // the input is most definitely a complex array
            ndarray_obj_t *target = ndarray_new_dense_ndarray(source->ndim, source->shape, NDARRAY_FLOAT);
            ndarray_copy_array(source, target, 0);
            return MP_OBJ_FROM_PTR(target);
        }
    } else {
        mp_raise_NotImplementedError(translate("function is implemented for ndarrays only"));
    }
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(carray_real_obj, carray_real);

//| def imag(val):
//|     """
//|     Return the imaginary part of the complex argument, which can be
//|     either an ndarray, or a scalar."""
//|     ...
//|

mp_obj_t carray_imag(mp_obj_t _source) {
    if(mp_obj_is_type(_source, &ulab_ndarray_type)) {
        ndarray_obj_t *source = MP_OBJ_TO_PTR(_source);
        if(source->dtype != NDARRAY_COMPLEX) { // if not complex, then the imaginary part is zero
            ndarray_obj_t *target = ndarray_new_dense_ndarray(source->ndim, source->shape, source->dtype);
            return MP_OBJ_FROM_PTR(target);
        } else { // the input is most definitely a complex array
            ndarray_obj_t *target = ndarray_new_dense_ndarray(source->ndim, source->shape, NDARRAY_FLOAT);
            ndarray_copy_array(source, target, source->itemsize / 2);
            return MP_OBJ_FROM_PTR(target);
        }
    } else {
        mp_raise_NotImplementedError(translate("function is implemented for ndarrays only"));
    }
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(carray_imag_obj, carray_imag);

#if ULAB_NUMPY_HAS_CONJUGATE

//| def conjugate(val):
//|     """
//|     Return the conjugate of the complex argument, which can be
//|     either an ndarray, or a scalar."""
//|     ...
//|
mp_obj_t carray_conjugate(mp_obj_t _source) {
    if(mp_obj_is_type(_source, &ulab_ndarray_type)) {
        ndarray_obj_t *source = MP_OBJ_TO_PTR(_source);
        ndarray_obj_t *ndarray = ndarray_new_dense_ndarray(source->ndim, source->shape, source->dtype);
        ndarray_copy_array(source, ndarray, 0);
        if(source->dtype == NDARRAY_COMPLEX) {
            mp_float_t *array = (mp_float_t *)ndarray->array;
            array++;
            for(size_t i = 0; i < ndarray->len; i++) {
                *array *= MICROPY_FLOAT_CONST(-1.0);
                array += 2;
            }
        }
        return MP_OBJ_FROM_PTR(ndarray);
    } else {
        if(mp_obj_is_type(_source, &mp_type_complex)) {
            mp_float_t real, imag;
            mp_obj_get_complex(_source, &real, &imag);
            imag = imag * MICROPY_FLOAT_CONST(-1.0);
            return mp_obj_new_complex(real, imag);
        } else if(mp_obj_is_int(_source) || mp_obj_is_float(_source)) {
            return _source;
        } else {
            mp_raise_TypeError(translate("input must be an ndarray, or a scalar"));
        }
    }
    // this should never happen
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(carray_conjugate_obj, carray_conjugate);
#endif

#if ULAB_NUMPY_HAS_SORT_COMPLEX
//| def sort_complex(a: ulab.numpy.ndarray) -> ulab.numpy.ndarray:
//|     """
//|     .. param: a
//|       a one-dimensional ndarray
//|
//|     Sort a complex array using the real part first, then the imaginary part.
//|     Always returns a sorted complex array, even if the input was real."""
//|     ...
//|

static void carray_sort_complex_(mp_float_t *array, size_t len) {
    // array is assumed to be a floating vector containing the real and imaginary parts
    // of a complex array at alternating positions as
    // array[0] = real[0]
    // array[1] = imag[0]
    // array[2] = real[1]
    // array[3] = imag[1]

    mp_float_t real, imag;
    size_t c, q = len, p, r = len >> 1;
    for (;;) {
        if (r > 0) {
            r--;
            real = array[2 * r];
            imag = array[2 * r + 1];
        } else {
            q--;
            if(q == 0) {
                break;
            }
            real = array[2 * q];
            imag = array[2 * q + 1];
            array[2 * q] = array[0];
            array[2 * q + 1] = array[1];
        }
        p = r;
        c = r + r + 1;
        while (c < q) {
            if(c + 1 < q) {
                if((array[2 * (c+1)] > array[2 * c]) ||
                    ((array[2 * (c+1)] == array[2 * c]) && (array[2 * (c+1) + 1] > array[2 * c + 1]))) {
                    c++;
                }
            }
            if((array[2 * c] > real) ||
                ((array[2 * c] == real) && (array[2 * c + 1] > imag))) {
                array[2 * p] = array[2 * c]; // real part
                array[2 * p + 1] = array[2 * c + 1]; // imag part
                p = c;
                c = p + p + 1;
            } else {
                break;
            }
        }
        array[2 * p] = real;
        array[2 * p + 1] = imag;
    }
}

mp_obj_t carray_sort_complex(mp_obj_t _source) {
    if(!mp_obj_is_type(_source, &ulab_ndarray_type)) {
        mp_raise_TypeError(translate("input must be a 1D ndarray"));
    }
    ndarray_obj_t *source = MP_OBJ_TO_PTR(_source);
    if(source->ndim != 1) {
        mp_raise_TypeError(translate("input must be a 1D ndarray"));
    }

    ndarray_obj_t *ndarray = ndarray_copy_view_convert_type(source, NDARRAY_COMPLEX);
    mp_float_t *array = (mp_float_t *)ndarray->array;
    carray_sort_complex_(array, ndarray->len);
    return MP_OBJ_FROM_PTR(ndarray);
}

MP_DEFINE_CONST_FUN_OBJ_1(carray_sort_complex_obj, carray_sort_complex);
#endif

//| def abs(a: ulab.numpy.ndarray) -> ulab.numpy.ndarray:
//|     """
//|     .. param: a
//|       a one-dimensional ndarray
//|
//|     Return the absolute value of complex ndarray."""
//|     ...
//|

mp_obj_t carray_abs(ndarray_obj_t *source, ndarray_obj_t *target) {
    // calculates the absolute value of a complex array and returns a dense array
    uint8_t *sarray = (uint8_t *)source->array;
    mp_float_t *tarray = (mp_float_t *)target->array;
    uint8_t itemsize = mp_binary_get_size('@', NDARRAY_FLOAT, NULL);

    #if ULAB_MAX_DIMS > 3
    size_t i = 0;
    do {
    #endif
        #if ULAB_MAX_DIMS > 2
        size_t j = 0;
        do {
        #endif
            #if ULAB_MAX_DIMS > 1
            size_t k = 0;
            do {
            #endif
                size_t l = 0;
                do {
                    mp_float_t rvalue = *(mp_float_t *)sarray;
                    mp_float_t ivalue = *(mp_float_t *)(sarray + itemsize);
                    *tarray++ = MICROPY_FLOAT_C_FUN(sqrt)(rvalue * rvalue + ivalue * ivalue);
                    sarray += source->strides[ULAB_MAX_DIMS - 1];
                    l++;
                } while(l < source->shape[ULAB_MAX_DIMS - 1]);
            #if ULAB_MAX_DIMS > 1
                sarray -= source->strides[ULAB_MAX_DIMS - 1] * source->shape[ULAB_MAX_DIMS-1];
                sarray += source->strides[ULAB_MAX_DIMS - 2];
                k++;
            } while(k < source->shape[ULAB_MAX_DIMS - 2]);
            #endif
        #if ULAB_MAX_DIMS > 2
            sarray -= source->strides[ULAB_MAX_DIMS - 2] * source->shape[ULAB_MAX_DIMS-2];
            sarray += source->strides[ULAB_MAX_DIMS - 3];
            j++;
        } while(j < source->shape[ULAB_MAX_DIMS - 3]);
        #endif
    #if ULAB_MAX_DIMS > 3
        sarray -= source->strides[ULAB_MAX_DIMS - 3] * source->shape[ULAB_MAX_DIMS-3];
        sarray += source->strides[ULAB_MAX_DIMS - 4];
        i++;
    } while(i < source->shape[ULAB_MAX_DIMS - 4]);
    #endif
    return MP_OBJ_FROM_PTR(target);
}

static void carray_copy_part(uint8_t *tarray, uint8_t *sarray, size_t *shape, int32_t *strides) {
    // copies the real or imaginary part of an array
    // into the respective part of a dense complex array
    uint8_t sz = sizeof(mp_float_t);

    #if ULAB_MAX_DIMS > 3
    size_t i = 0;
    do {
    #endif
        #if ULAB_MAX_DIMS > 2
        size_t j = 0;
        do {
        #endif
            #if ULAB_MAX_DIMS > 1
            size_t k = 0;
            do {
            #endif
                size_t l = 0;
                do {
                    memcpy(tarray, sarray, sz);
                    tarray += 2 * sz;
                    sarray += strides[ULAB_MAX_DIMS - 1];
                    l++;
                } while(l < shape[ULAB_MAX_DIMS - 1]);
            #if ULAB_MAX_DIMS > 1
                sarray -= strides[ULAB_MAX_DIMS - 1] * shape[ULAB_MAX_DIMS-1];
                sarray += strides[ULAB_MAX_DIMS - 2];
                k++;
            } while(k < shape[ULAB_MAX_DIMS - 2]);
            #endif /* ULAB_MAX_DIMS > 1 */
        #if ULAB_MAX_DIMS > 2
            sarray -= strides[ULAB_MAX_DIMS - 2] * shape[ULAB_MAX_DIMS-2];
            sarray += strides[ULAB_MAX_DIMS - 3];
            j++;
        } while(j < shape[ULAB_MAX_DIMS - 3]);
        #endif /* ULAB_MAX_DIMS > 2 */
    #if ULAB_MAX_DIMS > 3
        sarray -= strides[ULAB_MAX_DIMS - 3] * shape[ULAB_MAX_DIMS-3];
        sarray += strides[ULAB_MAX_DIMS - 4];
        i++;
    } while(i < shape[ULAB_MAX_DIMS - 4]);
    #endif /* ULAB_MAX_DIMS > 3 */
}

mp_obj_t carray_binary_equal_not_equal(ndarray_obj_t *lhs, ndarray_obj_t *rhs,
                            uint8_t ndim, size_t *shape, int32_t *lstrides, int32_t *rstrides, mp_binary_op_t op) {

    ndarray_obj_t *results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_UINT8);
    results->boolean = 1;
    uint8_t *array = (uint8_t *)results->array;

    if(op == MP_BINARY_OP_NOT_EQUAL) {
        memset(array, 1, results->len);
    }

    if((lhs->dtype == NDARRAY_COMPLEX) && (rhs->dtype == NDARRAY_COMPLEX)) {
        mp_float_t *larray = (mp_float_t *)lhs->array;
        mp_float_t *rarray = (mp_float_t *)rhs->array;

        ulab_rescale_float_strides(lstrides);
        ulab_rescale_float_strides(rstrides);

        #if ULAB_MAX_DIMS > 3
        size_t i = 0;
        do {
        #endif
            #if ULAB_MAX_DIMS > 2
            size_t j = 0;
            do {
            #endif
                #if ULAB_MAX_DIMS > 1
                size_t k = 0;
                do {
                #endif
                    size_t l = 0;
                    do {
                        if((larray[0] == rarray[0]) && (larray[1] == rarray[1])) {
                            *array ^= 0x01;
                        }
                        array++;
                        larray += lstrides[ULAB_MAX_DIMS - 1];
                        rarray += rstrides[ULAB_MAX_DIMS - 1];
                        l++;
                    } while(l < results->shape[ULAB_MAX_DIMS - 1]);
                #if ULAB_MAX_DIMS > 1
                    larray -= lstrides[ULAB_MAX_DIMS - 1] * results->shape[ULAB_MAX_DIMS-1];
                    larray += lstrides[ULAB_MAX_DIMS - 2];
                    rarray -= rstrides[ULAB_MAX_DIMS - 1] * results->shape[ULAB_MAX_DIMS-1];
                    rarray += rstrides[ULAB_MAX_DIMS - 2];
                    k++;
                } while(k < results->shape[ULAB_MAX_DIMS - 2]);
                #endif /* ULAB_MAX_DIMS > 1 */
            #if ULAB_MAX_DIMS > 2
                larray -= lstrides[ULAB_MAX_DIMS - 2] * results->shape[ULAB_MAX_DIMS-2];
                larray += lstrides[ULAB_MAX_DIMS - 3];
                rarray -= rstrides[ULAB_MAX_DIMS - 2] * results->shape[ULAB_MAX_DIMS-2];
                rarray += rstrides[ULAB_MAX_DIMS - 3];
                j++;
            } while(j < results->shape[ULAB_MAX_DIMS - 3]);
            #endif /* ULAB_MAX_DIMS > 2 */
        #if ULAB_MAX_DIMS > 3
            larray -= lstrides[ULAB_MAX_DIMS - 3] * results->shape[ULAB_MAX_DIMS-3];
            larray += lstrides[ULAB_MAX_DIMS - 4];
            rarray -= rstrides[ULAB_MAX_DIMS - 3] * results->shape[ULAB_MAX_DIMS-3];
            rarray += rstrides[ULAB_MAX_DIMS - 4];
            i++;
        } while(i < results->shape[ULAB_MAX_DIMS - 4]);
        #endif /* ULAB_MAX_DIMS > 3 */
    } else { // only one of the operands is complex
        mp_float_t *larray = (mp_float_t *)lhs->array;
        uint8_t *rarray = (uint8_t *)rhs->array;

        // align the complex array to the left
        uint8_t rdtype = rhs->dtype;
        int32_t *lstrides_ = lstrides;
        int32_t *rstrides_ = rstrides;

        if(rhs->dtype == NDARRAY_COMPLEX) {
            larray = (mp_float_t *)rhs->array;
            rarray = (uint8_t *)lhs->array;
            lstrides_ = rstrides;
            rstrides_ = lstrides;
            rdtype = lhs->dtype;
        }

        ulab_rescale_float_strides(lstrides_);

        if(rdtype == NDARRAY_UINT8) {
            BINARY_LOOP_COMPLEX_EQUAL(results, array, uint8_t, larray, lstrides_, rarray, rstrides_);
        } else if(rdtype == NDARRAY_INT8) {
            BINARY_LOOP_COMPLEX_EQUAL(results, array, int8_t, larray, lstrides_, rarray, rstrides_);
        } else if(rdtype == NDARRAY_UINT16) {
            BINARY_LOOP_COMPLEX_EQUAL(results, array, uint16_t, larray, lstrides_, rarray, rstrides_);
        } else if(rdtype == NDARRAY_INT16) {
            BINARY_LOOP_COMPLEX_EQUAL(results, array, int16_t, larray, lstrides_, rarray, rstrides_);
        } else if(rdtype == NDARRAY_FLOAT) {
            BINARY_LOOP_COMPLEX_EQUAL(results, array, mp_float_t, larray, lstrides_, rarray, rstrides_);
        }
    }
    return MP_OBJ_FROM_PTR(results);
}

mp_obj_t carray_binary_add(ndarray_obj_t *lhs, ndarray_obj_t *rhs,
                            uint8_t ndim, size_t *shape, int32_t *lstrides, int32_t *rstrides) {

    ndarray_obj_t *results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_COMPLEX);
    mp_float_t *resarray = (mp_float_t *)results->array;

    if((lhs->dtype == NDARRAY_COMPLEX) && (rhs->dtype == NDARRAY_COMPLEX)) {
        mp_float_t *larray = (mp_float_t *)lhs->array;
        mp_float_t *rarray = (mp_float_t *)rhs->array;

        ulab_rescale_float_strides(lstrides);
        ulab_rescale_float_strides(rstrides);

        #if ULAB_MAX_DIMS > 3
        size_t i = 0;
        do {
        #endif
            #if ULAB_MAX_DIMS > 2
            size_t j = 0;
            do {
            #endif
                #if ULAB_MAX_DIMS > 1
                size_t k = 0;
                do {
                #endif
                    size_t l = 0;
                    do {
                        // real part
                        *resarray++ = larray[0] + rarray[0];
                        // imaginary part
                        *resarray++ = larray[1] + rarray[1];
                        larray += lstrides[ULAB_MAX_DIMS - 1];
                        rarray += rstrides[ULAB_MAX_DIMS - 1];
                        l++;
                    } while(l < results->shape[ULAB_MAX_DIMS - 1]);
                #if ULAB_MAX_DIMS > 1
                    larray -= lstrides[ULAB_MAX_DIMS - 1] * results->shape[ULAB_MAX_DIMS-1];
                    larray += lstrides[ULAB_MAX_DIMS - 2];
                    rarray -= rstrides[ULAB_MAX_DIMS - 1] * results->shape[ULAB_MAX_DIMS-1];
                    rarray += rstrides[ULAB_MAX_DIMS - 2];
                    k++;
                } while(k < results->shape[ULAB_MAX_DIMS - 2]);
                #endif /* ULAB_MAX_DIMS > 1 */
            #if ULAB_MAX_DIMS > 2
                larray -= lstrides[ULAB_MAX_DIMS - 2] * results->shape[ULAB_MAX_DIMS-2];
                larray += lstrides[ULAB_MAX_DIMS - 3];
                rarray -= rstrides[ULAB_MAX_DIMS - 2] * results->shape[ULAB_MAX_DIMS-2];
                rarray += rstrides[ULAB_MAX_DIMS - 3];
                j++;
            } while(j < results->shape[ULAB_MAX_DIMS - 3]);
            #endif /* ULAB_MAX_DIMS > 2 */
        #if ULAB_MAX_DIMS > 3
            larray -= lstrides[ULAB_MAX_DIMS - 3] * results->shape[ULAB_MAX_DIMS-3];
            larray += lstrides[ULAB_MAX_DIMS - 4];
            rarray -= rstrides[ULAB_MAX_DIMS - 3] * results->shape[ULAB_MAX_DIMS-3];
            rarray += rstrides[ULAB_MAX_DIMS - 4];
            i++;
        } while(i < results->shape[ULAB_MAX_DIMS - 4]);
        #endif /* ULAB_MAX_DIMS > 3 */
    } else { // only one of the operands is complex
        uint8_t *larray = (uint8_t *)lhs->array;
        uint8_t *rarray = (uint8_t *)rhs->array;

        // align the complex array to the left
        uint8_t rdtype = rhs->dtype;
        int32_t *lstrides_ = lstrides;
        int32_t *rstrides_ = rstrides;

        if(rhs->dtype == NDARRAY_COMPLEX) {
            larray = (uint8_t *)rhs->array;
            rarray = (uint8_t *)lhs->array;
            lstrides_ = rstrides;
            rstrides_ = lstrides;
            rdtype = lhs->dtype;
        }

        if(rdtype == NDARRAY_UINT8) {
            BINARY_LOOP_COMPLEX(results, resarray, uint8_t, larray, lstrides_, rarray, rstrides_, +);
        } else if(rdtype == NDARRAY_INT8) {
            BINARY_LOOP_COMPLEX(results, resarray, int8_t, larray, lstrides_, rarray, rstrides_, +);
        } else if(rdtype == NDARRAY_UINT16) {
            BINARY_LOOP_COMPLEX(results, resarray, uint16_t, larray, lstrides_, rarray, rstrides_, +);
        } else if(rdtype == NDARRAY_INT16) {
            BINARY_LOOP_COMPLEX(results, resarray, int16_t, larray, lstrides_, rarray, rstrides_, +);
        } else if(rdtype == NDARRAY_FLOAT) {
            BINARY_LOOP_COMPLEX(results, resarray, mp_float_t, larray, lstrides_, rarray, rstrides_, +);
        }

        // simply copy the imaginary part
        uint8_t *tarray = (uint8_t *)results->array;
        tarray += sizeof(mp_float_t);

        if(lhs->dtype == NDARRAY_COMPLEX) {
            rarray = (uint8_t *)lhs->array;
            rstrides = lstrides;
        } else {
            rarray = (uint8_t *)rhs->array;
        }
        rarray += sizeof(mp_float_t);
        carray_copy_part(tarray, rarray, results->shape, rstrides);
    }
    return MP_OBJ_FROM_PTR(results);
}

static void carray_binary_multiply_(ndarray_obj_t *results, mp_float_t *resarray, uint8_t *larray, uint8_t *rarray,
                            int32_t *lstrides, int32_t *rstrides, uint8_t rdtype) {

    if(rdtype == NDARRAY_UINT8) {
        BINARY_LOOP_COMPLEX(results, resarray, uint8_t, larray, lstrides, rarray, rstrides, *);
    } else if(rdtype == NDARRAY_INT8) {
        BINARY_LOOP_COMPLEX(results, resarray, int8_t, larray, lstrides, rarray, rstrides, *);
    } else if(rdtype == NDARRAY_UINT16) {
        BINARY_LOOP_COMPLEX(results, resarray, uint16_t, larray, lstrides, rarray, rstrides, *);
    } else if(rdtype == NDARRAY_INT16) {
        BINARY_LOOP_COMPLEX(results, resarray, int16_t, larray, lstrides, rarray, rstrides, *);
    } else if(rdtype == NDARRAY_FLOAT) {
        BINARY_LOOP_COMPLEX(results, resarray, mp_float_t, larray, lstrides, rarray, rstrides, *);
    }
}

mp_obj_t carray_binary_multiply(ndarray_obj_t *lhs, ndarray_obj_t *rhs,
                            uint8_t ndim, size_t *shape, int32_t *lstrides, int32_t *rstrides) {

    ndarray_obj_t *results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_COMPLEX);
    mp_float_t *resarray = (mp_float_t *)results->array;

    if((lhs->dtype == NDARRAY_COMPLEX) && (rhs->dtype == NDARRAY_COMPLEX)) {
        mp_float_t *larray = (mp_float_t *)lhs->array;
        mp_float_t *rarray = (mp_float_t *)rhs->array;

        ulab_rescale_float_strides(lstrides);
        ulab_rescale_float_strides(rstrides);

        #if ULAB_MAX_DIMS > 3
        size_t i = 0;
        do {
        #endif
            #if ULAB_MAX_DIMS > 2
            size_t j = 0;
            do {
            #endif
                #if ULAB_MAX_DIMS > 1
                size_t k = 0;
                do {
                #endif
                    size_t l = 0;
                    do {
                        // real part
                        *resarray++ = larray[0] * rarray[0] - larray[1] * rarray[1];
                        // imaginary part
                        *resarray++ = larray[0] * rarray[1] + larray[1] * rarray[0];
                        larray += lstrides[ULAB_MAX_DIMS - 1];
                        rarray += rstrides[ULAB_MAX_DIMS - 1];
                        l++;
                    } while(l < results->shape[ULAB_MAX_DIMS - 1]);
                #if ULAB_MAX_DIMS > 1
                    larray -= lstrides[ULAB_MAX_DIMS - 1] * results->shape[ULAB_MAX_DIMS-1];
                    larray += lstrides[ULAB_MAX_DIMS - 2];
                    rarray -= rstrides[ULAB_MAX_DIMS - 1] * results->shape[ULAB_MAX_DIMS-1];
                    rarray += rstrides[ULAB_MAX_DIMS - 2];
                    k++;
                } while(k < results->shape[ULAB_MAX_DIMS - 2]);
                #endif /* ULAB_MAX_DIMS > 1 */
            #if ULAB_MAX_DIMS > 2
                larray -= lstrides[ULAB_MAX_DIMS - 2] * results->shape[ULAB_MAX_DIMS-2];
                larray += lstrides[ULAB_MAX_DIMS - 3];
                rarray -= rstrides[ULAB_MAX_DIMS - 2] * results->shape[ULAB_MAX_DIMS-2];
                rarray += rstrides[ULAB_MAX_DIMS - 3];
                j++;
            } while(j < results->shape[ULAB_MAX_DIMS - 3]);
            #endif /* ULAB_MAX_DIMS > 2 */
        #if ULAB_MAX_DIMS > 3
            larray -= lstrides[ULAB_MAX_DIMS - 3] * results->shape[ULAB_MAX_DIMS-3];
            larray += lstrides[ULAB_MAX_DIMS - 4];
            rarray -= rstrides[ULAB_MAX_DIMS - 3] * results->shape[ULAB_MAX_DIMS-3];
            rarray += rstrides[ULAB_MAX_DIMS - 4];
            i++;
        } while(i < results->shape[ULAB_MAX_DIMS - 4]);
        #endif /* ULAB_MAX_DIMS > 3 */
    } else { // only one of the operands is complex

        uint8_t *larray = (uint8_t *)lhs->array;
        uint8_t *rarray = (uint8_t *)rhs->array;
        uint8_t *lo = larray, *ro = rarray;
        int32_t *left_strides = lstrides;
        int32_t *right_strides = rstrides;
        uint8_t rdtype = rhs->dtype;

        // align the complex array to the left
        if(rhs->dtype == NDARRAY_COMPLEX) {
            lo = (uint8_t *)rhs->array;
            ro = (uint8_t *)lhs->array;
            rdtype = lhs->dtype;
            left_strides = rstrides;
            right_strides = lstrides;
        }

        larray = lo;
        rarray = ro;
        // real part
        carray_binary_multiply_(results, resarray, larray, rarray, left_strides, right_strides, rdtype);

        larray = lo + sizeof(mp_float_t);
        rarray = ro;
        resarray = (mp_float_t *)results->array;
        resarray++;
        // imaginary part
        carray_binary_multiply_(results, resarray, larray, rarray, left_strides, right_strides, rdtype);
    }
    return MP_OBJ_FROM_PTR(results);
}

mp_obj_t carray_binary_subtract(ndarray_obj_t *lhs, ndarray_obj_t *rhs,
                            uint8_t ndim, size_t *shape, int32_t *lstrides, int32_t *rstrides) {

    ndarray_obj_t *results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_COMPLEX);
    mp_float_t *resarray = (mp_float_t *)results->array;

    if((lhs->dtype == NDARRAY_COMPLEX) && (rhs->dtype == NDARRAY_COMPLEX)) {
        mp_float_t *larray = (mp_float_t *)lhs->array;
        mp_float_t *rarray = (mp_float_t *)rhs->array;

        ulab_rescale_float_strides(lstrides);
        ulab_rescale_float_strides(rstrides);

        #if ULAB_MAX_DIMS > 3
        size_t i = 0;
        do {
        #endif
            #if ULAB_MAX_DIMS > 2
            size_t j = 0;
            do {
            #endif
                #if ULAB_MAX_DIMS > 1
                size_t k = 0;
                do {
                #endif
                    size_t l = 0;
                    do {
                        // real part
                        *resarray++ = larray[0] - rarray[0];
                        // imaginary part
                        *resarray++ = larray[1] - rarray[1];
                        larray += lstrides[ULAB_MAX_DIMS - 1];
                        rarray += rstrides[ULAB_MAX_DIMS - 1];
                        l++;
                    } while(l < results->shape[ULAB_MAX_DIMS - 1]);
                #if ULAB_MAX_DIMS > 1
                    larray -= lstrides[ULAB_MAX_DIMS - 1] * results->shape[ULAB_MAX_DIMS-1];
                    larray += lstrides[ULAB_MAX_DIMS - 2];
                    rarray -= rstrides[ULAB_MAX_DIMS - 1] * results->shape[ULAB_MAX_DIMS-1];
                    rarray += rstrides[ULAB_MAX_DIMS - 2];
                    k++;
                } while(k < results->shape[ULAB_MAX_DIMS - 2]);
                #endif /* ULAB_MAX_DIMS > 1 */
            #if ULAB_MAX_DIMS > 2
                larray -= lstrides[ULAB_MAX_DIMS - 2] * results->shape[ULAB_MAX_DIMS-2];
                larray += lstrides[ULAB_MAX_DIMS - 3];
                rarray -= rstrides[ULAB_MAX_DIMS - 2] * results->shape[ULAB_MAX_DIMS-2];
                rarray += rstrides[ULAB_MAX_DIMS - 3];
                j++;
            } while(j < results->shape[ULAB_MAX_DIMS - 3]);
            #endif /* ULAB_MAX_DIMS > 2 */
        #if ULAB_MAX_DIMS > 3
            larray -= lstrides[ULAB_MAX_DIMS - 3] * results->shape[ULAB_MAX_DIMS-3];
            larray += lstrides[ULAB_MAX_DIMS - 4];
            rarray -= rstrides[ULAB_MAX_DIMS - 3] * results->shape[ULAB_MAX_DIMS-3];
            rarray += rstrides[ULAB_MAX_DIMS - 4];
            i++;
        } while(i < results->shape[ULAB_MAX_DIMS - 4]);
        #endif /* ULAB_MAX_DIMS > 3 */
    } else {
        uint8_t *larray = (uint8_t *)lhs->array;
        if(lhs->dtype == NDARRAY_COMPLEX) {
            uint8_t *rarray = (uint8_t *)rhs->array;
            if(rhs->dtype == NDARRAY_UINT8) {
                BINARY_LOOP_COMPLEX(results, resarray, uint8_t, larray, lstrides, rarray, rstrides, -);
            } else if(rhs->dtype == NDARRAY_INT8) {
                BINARY_LOOP_COMPLEX(results, resarray, int8_t, larray, lstrides, rarray, rstrides, -);
            } else if(rhs->dtype == NDARRAY_UINT16) {
                BINARY_LOOP_COMPLEX(results, resarray, uint16_t, larray, lstrides, rarray, rstrides, -);
            } else if(rhs->dtype == NDARRAY_INT16) {
                BINARY_LOOP_COMPLEX(results, resarray, int16_t, larray, lstrides, rarray, rstrides, -);
            } else if(rhs->dtype == NDARRAY_FLOAT) {
                BINARY_LOOP_COMPLEX(results, resarray, mp_float_t, larray, lstrides, rarray, rstrides, -);
            }
            // copy the imaginary part
            uint8_t *tarray = (uint8_t *)results->array;
            tarray += sizeof(mp_float_t);

            larray = (uint8_t *)lhs->array;
            larray += sizeof(mp_float_t);

            carray_copy_part(tarray, larray, results->shape, lstrides);
        } else if(rhs->dtype == NDARRAY_COMPLEX) {
            mp_float_t *rarray = (mp_float_t *)rhs->array;
            ulab_rescale_float_strides(rstrides);

            if(lhs->dtype == NDARRAY_UINT8) {
                BINARY_LOOP_COMPLEX_REVERSED_SUBTRACT(results, resarray, uint8_t, larray, lstrides, rarray, rstrides);
            } else if(lhs->dtype == NDARRAY_INT8) {
                BINARY_LOOP_COMPLEX_REVERSED_SUBTRACT(results, resarray, int8_t, larray, lstrides, rarray, rstrides);
            } else if(lhs->dtype == NDARRAY_UINT16) {
                BINARY_LOOP_COMPLEX_REVERSED_SUBTRACT(results, resarray, uint16_t, larray, lstrides, rarray, rstrides);
            } else if(lhs->dtype == NDARRAY_INT16) {
                BINARY_LOOP_COMPLEX_REVERSED_SUBTRACT(results, resarray, int16_t, larray, lstrides, rarray, rstrides);
            } else if(lhs->dtype == NDARRAY_FLOAT) {
                BINARY_LOOP_COMPLEX_REVERSED_SUBTRACT(results, resarray, mp_float_t, larray, lstrides, rarray, rstrides);
            }
        }
    }

    return MP_OBJ_FROM_PTR(results);
}

static void carray_binary_left_divide_(ndarray_obj_t *results, mp_float_t *resarray, uint8_t *larray, uint8_t *rarray,
                            int32_t *lstrides, int32_t *rstrides, uint8_t rdtype) {

    if(rdtype == NDARRAY_UINT8) {
        BINARY_LOOP_COMPLEX(results, resarray, uint8_t, larray, lstrides, rarray, rstrides, /);
    } else if(rdtype == NDARRAY_INT8) {
        BINARY_LOOP_COMPLEX(results, resarray, int8_t, larray, lstrides, rarray, rstrides, /);
    } else if(rdtype == NDARRAY_UINT16) {
        BINARY_LOOP_COMPLEX(results, resarray, uint16_t, larray, lstrides, rarray, rstrides, /);
    } else if(rdtype == NDARRAY_INT16) {
        BINARY_LOOP_COMPLEX(results, resarray, int16_t, larray, lstrides, rarray, rstrides, /);
    } else if(rdtype == NDARRAY_FLOAT) {
        BINARY_LOOP_COMPLEX(results, resarray, mp_float_t, larray, lstrides, rarray, rstrides, /);
    }
}

mp_obj_t carray_binary_divide(ndarray_obj_t *lhs, ndarray_obj_t *rhs,
                            uint8_t ndim, size_t *shape, int32_t *lstrides, int32_t *rstrides) {

    ndarray_obj_t *results = ndarray_new_dense_ndarray(ndim, shape, NDARRAY_COMPLEX);
    mp_float_t *resarray = (mp_float_t *)results->array;

    if((lhs->dtype == NDARRAY_COMPLEX) && (rhs->dtype == NDARRAY_COMPLEX)) {
        mp_float_t *larray = (mp_float_t *)lhs->array;
        mp_float_t *rarray = (mp_float_t *)rhs->array;

        ulab_rescale_float_strides(lstrides);
        ulab_rescale_float_strides(rstrides);

        #if ULAB_MAX_DIMS > 3
        size_t i = 0;
        do {
        #endif
            #if ULAB_MAX_DIMS > 2
            size_t j = 0;
            do {
            #endif
                #if ULAB_MAX_DIMS > 1
                size_t k = 0;
                do {
                #endif
                    size_t l = 0;
                    do {
                        // (a + bi) / (c + di) =
                        // (ac + bd) / (c^2 + d^2) + i (bc - ad) / (c^2 + d^2)
                        // denominator
                        mp_float_t denom = rarray[0] * rarray[0] + rarray[1] * rarray[1];

                        // real part
                        *resarray++ = (larray[0] * rarray[0] + larray[1] * rarray[1]) / denom;
                        // imaginary part
                        *resarray++ = (larray[1] * rarray[0] - larray[0] * rarray[1]) / denom;
                        larray += lstrides[ULAB_MAX_DIMS - 1];
                        rarray += rstrides[ULAB_MAX_DIMS - 1];
                        l++;
                    } while(l < results->shape[ULAB_MAX_DIMS - 1]);
                #if ULAB_MAX_DIMS > 1
                    larray -= lstrides[ULAB_MAX_DIMS - 1] * results->shape[ULAB_MAX_DIMS-1];
                    larray += lstrides[ULAB_MAX_DIMS - 2];
                    rarray -= rstrides[ULAB_MAX_DIMS - 1] * results->shape[ULAB_MAX_DIMS-1];
                    rarray += rstrides[ULAB_MAX_DIMS - 2];
                    k++;
                } while(k < results->shape[ULAB_MAX_DIMS - 2]);
                #endif /* ULAB_MAX_DIMS > 1 */
            #if ULAB_MAX_DIMS > 2
                larray -= lstrides[ULAB_MAX_DIMS - 2] * results->shape[ULAB_MAX_DIMS-2];
                larray += lstrides[ULAB_MAX_DIMS - 3];
                rarray -= rstrides[ULAB_MAX_DIMS - 2] * results->shape[ULAB_MAX_DIMS-2];
                rarray += rstrides[ULAB_MAX_DIMS - 3];
                j++;
            } while(j < results->shape[ULAB_MAX_DIMS - 3]);
            #endif /* ULAB_MAX_DIMS > 2 */
        #if ULAB_MAX_DIMS > 3
            larray -= lstrides[ULAB_MAX_DIMS - 3] * results->shape[ULAB_MAX_DIMS-3];
            larray += lstrides[ULAB_MAX_DIMS - 4];
            rarray -= rstrides[ULAB_MAX_DIMS - 3] * results->shape[ULAB_MAX_DIMS-3];
            rarray += rstrides[ULAB_MAX_DIMS - 4];
            i++;
        } while(i < results->shape[ULAB_MAX_DIMS - 4]);
        #endif /* ULAB_MAX_DIMS > 3 */
    } else {
        uint8_t *larray = (uint8_t *)lhs->array;
        uint8_t *rarray = (uint8_t *)rhs->array;
        if(lhs->dtype == NDARRAY_COMPLEX) {
            // real part
            carray_binary_left_divide_(results, resarray, larray, rarray, lstrides, rstrides, rhs->dtype);
            // imaginary part
            resarray = (mp_float_t *)results->array;
            resarray++;
            larray = (uint8_t *)lhs->array;
            larray += sizeof(mp_float_t);
            rarray = (uint8_t *)rhs->array;
            carray_binary_left_divide_(results, resarray, larray, rarray, lstrides, rstrides, rhs->dtype);
        } else {
            if(lhs->dtype == NDARRAY_UINT8) {
                BINARY_LOOP_COMPLEX_RIGHT_DIVIDE(results, resarray, uint8_t, larray, lstrides, rarray, rstrides);
            } else if(lhs->dtype == NDARRAY_INT8) {
                BINARY_LOOP_COMPLEX_RIGHT_DIVIDE(results, resarray, int8_t, larray, lstrides, rarray, rstrides);
            } else if(lhs->dtype == NDARRAY_UINT16) {
                BINARY_LOOP_COMPLEX_RIGHT_DIVIDE(results, resarray, uint16_t, larray, lstrides, rarray, rstrides);
            } else if(lhs->dtype == NDARRAY_INT16) {
                BINARY_LOOP_COMPLEX_RIGHT_DIVIDE(results, resarray, int16_t, larray, lstrides, rarray, rstrides);
            } else if(lhs->dtype == NDARRAY_FLOAT) {
                BINARY_LOOP_COMPLEX_RIGHT_DIVIDE(results, resarray, mp_float_t, larray, lstrides, rarray, rstrides);
            }
        }
    }

    return MP_OBJ_FROM_PTR(results);
}

#endif
