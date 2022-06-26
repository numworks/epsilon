
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2021 Zoltán Vörös
 *               2020 Jeff Epler for Adafruit Industries
 *               2020 Scott Shawcroft for Adafruit Industries
 *               2020 Taku Fukada
*/

#include "py/obj.h"
#include "py/runtime.h"
#include "py/objarray.h"

#include "../ulab.h"
#include "linalg/linalg_tools.h"
#include "../ulab_tools.h"
#include "carray/carray_tools.h"
#include "poly.h"

#if ULAB_NUMPY_HAS_POLYFIT

mp_obj_t poly_polyfit(size_t n_args, const mp_obj_t *args) {
    if(!ndarray_object_is_array_like(args[0])) {
        mp_raise_ValueError(translate("input data must be an iterable"));
    }
    #if ULAB_SUPPORTS_COMPLEX
    if(mp_obj_is_type(args[0], &ulab_ndarray_type)) {
        ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(args[0]);
        COMPLEX_DTYPE_NOT_IMPLEMENTED(ndarray->dtype)
    }
    #endif
    size_t lenx = 0, leny = 0;
    uint8_t deg = 0;
    mp_float_t *x, *XT, *y, *prod;

    if(n_args == 2) { // only the y values are supplied
        // TODO: this is actually not enough: the first argument can very well be a matrix,
        // in which case we are between the rock and a hard place
        leny = (size_t)mp_obj_get_int(mp_obj_len_maybe(args[0]));
        deg = (uint8_t)mp_obj_get_int(args[1]);
        if(leny < deg) {
            mp_raise_ValueError(translate("more degrees of freedom than data points"));
        }
        lenx = leny;
        x = m_new(mp_float_t, lenx); // assume uniformly spaced data points
        for(size_t i=0; i < lenx; i++) {
            x[i] = i;
        }
        y = m_new(mp_float_t, leny);
        fill_array_iterable(y, args[0]);
    } else /* n_args == 3 */ {
        if(!ndarray_object_is_array_like(args[1])) {
            mp_raise_ValueError(translate("input data must be an iterable"));
        }
        lenx = (size_t)mp_obj_get_int(mp_obj_len_maybe(args[0]));
        leny = (size_t)mp_obj_get_int(mp_obj_len_maybe(args[1]));
        if(lenx != leny) {
            mp_raise_ValueError(translate("input vectors must be of equal length"));
        }
        deg = (uint8_t)mp_obj_get_int(args[2]);
        if(leny < deg) {
            mp_raise_ValueError(translate("more degrees of freedom than data points"));
        }
        x = m_new(mp_float_t, lenx);
        fill_array_iterable(x, args[0]);
        y = m_new(mp_float_t, leny);
        fill_array_iterable(y, args[1]);
    }

    // one could probably express X as a function of XT,
    // and thereby save RAM, because X is used only in the product
    XT = m_new(mp_float_t, (deg+1)*leny); // XT is a matrix of shape (deg+1, len) (rows, columns)
    for(size_t i=0; i < leny; i++) { // column index
        XT[i+0*lenx] = 1.0; // top row
        for(uint8_t j=1; j < deg+1; j++) { // row index
            XT[i+j*leny] = XT[i+(j-1)*leny]*x[i];
        }
    }

    prod = m_new(mp_float_t, (deg+1)*(deg+1)); // the product matrix is of shape (deg+1, deg+1)
    mp_float_t sum;
    for(uint8_t i=0; i < deg+1; i++) { // column index
        for(uint8_t j=0; j < deg+1; j++) { // row index
            sum = 0.0;
            for(size_t k=0; k < lenx; k++) {
                // (j, k) * (k, i)
                // Note that the second matrix is simply the transpose of the first:
                // X(k, i) = XT(i, k) = XT[k*lenx+i]
                sum += XT[j*lenx+k]*XT[i*lenx+k]; // X[k*(deg+1)+i];
            }
            prod[j*(deg+1)+i] = sum;
        }
    }
    if(!linalg_invert_matrix(prod, deg+1)) {
        // Although X was a Vandermonde matrix, whose inverse is guaranteed to exist,
        // we bail out here, if prod couldn't be inverted: if the values in x are not all
        // distinct, prod is singular
        m_del(mp_float_t, XT, (deg+1)*lenx);
        m_del(mp_float_t, x, lenx);
        m_del(mp_float_t, y, lenx);
        m_del(mp_float_t, prod, (deg+1)*(deg+1));
        mp_raise_ValueError(translate("could not invert Vandermonde matrix"));
    }
    // at this point, we have the inverse of X^T * X
    // y is a column vector; x is free now, we can use it for storing intermediate values
    for(uint8_t i=0; i < deg+1; i++) { // row index
        sum = 0.0;
        for(size_t j=0; j < lenx; j++) { // column index
            sum += XT[i*lenx+j]*y[j];
        }
        x[i] = sum;
    }
    // XT is no longer needed
    m_del(mp_float_t, XT, (deg+1)*leny);

    ndarray_obj_t *beta = ndarray_new_linear_array(deg+1, NDARRAY_FLOAT);
    mp_float_t *betav = (mp_float_t *)beta->array;
    // x[0..(deg+1)] contains now the product X^T * y; we can get rid of y
    m_del(float, y, leny);

    // now, we calculate beta, i.e., we apply prod = (X^T * X)^(-1) on x = X^T * y; x is a column vector now
    for(uint8_t i=0; i < deg+1; i++) {
        sum = 0.0;
        for(uint8_t j=0; j < deg+1; j++) {
            sum += prod[i*(deg+1)+j]*x[j];
        }
        betav[i] = sum;
    }
    m_del(mp_float_t, x, lenx);
    m_del(mp_float_t, prod, (deg+1)*(deg+1));
    for(uint8_t i=0; i < (deg+1)/2; i++) {
        // We have to reverse the array, for the leading coefficient comes first.
        SWAP(mp_float_t, betav[i], betav[deg-i]);
    }
    return MP_OBJ_FROM_PTR(beta);
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(poly_polyfit_obj, 2, 3, poly_polyfit);
#endif

#if ULAB_NUMPY_HAS_POLYVAL

mp_obj_t poly_polyval(mp_obj_t o_p, mp_obj_t o_x) {
    if(!ndarray_object_is_array_like(o_p) || !ndarray_object_is_array_like(o_x)) {
        mp_raise_TypeError(translate("inputs are not iterable"));
    }
    #if ULAB_SUPPORTS_COMPLEX
    ndarray_obj_t *input;
    if(mp_obj_is_type(o_p, &ulab_ndarray_type)) {
        input = MP_OBJ_TO_PTR(o_p);
        COMPLEX_DTYPE_NOT_IMPLEMENTED(input->dtype)
    }
    if(mp_obj_is_type(o_x, &ulab_ndarray_type)) {
        input = MP_OBJ_TO_PTR(o_x);
        COMPLEX_DTYPE_NOT_IMPLEMENTED(input->dtype)
    }
    #endif
    // p had better be a one-dimensional standard iterable
    uint8_t plen = mp_obj_get_int(mp_obj_len_maybe(o_p));
    mp_float_t *p = m_new(mp_float_t, plen);
    mp_obj_iter_buf_t p_buf;
    mp_obj_t p_item, p_iterable = mp_getiter(o_p, &p_buf);
    uint8_t i = 0;
    while((p_item = mp_iternext(p_iterable)) != MP_OBJ_STOP_ITERATION) {
        p[i] = mp_obj_get_float(p_item);
        i++;
    }

    // polynomials are going to be of type float, except, when both
    // the coefficients and the independent variable are integers
    ndarray_obj_t *ndarray;
    if(mp_obj_is_type(o_x, &ulab_ndarray_type)) {
        ndarray_obj_t *source = MP_OBJ_TO_PTR(o_x);
        uint8_t *sarray = (uint8_t *)source->array;
        ndarray = ndarray_new_dense_ndarray(source->ndim, source->shape, NDARRAY_FLOAT);
        mp_float_t *array = (mp_float_t *)ndarray->array;

        mp_float_t (*func)(void *) = ndarray_get_float_function(source->dtype);

        // TODO: these loops are really nothing, but the re-impplementation of
        // ITERATE_VECTOR from vectorise.c. We could pass a function pointer here
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
                        mp_float_t y = p[0];
                        mp_float_t _x = func(sarray);
                        for(uint8_t m=0; m < plen-1; m++) {
                            y *= _x;
                            y += p[m+1];
                        }
                        *array++ = y;
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
    } else {
        // o_x had better be a one-dimensional standard iterable
        ndarray = ndarray_new_linear_array(mp_obj_get_int(mp_obj_len_maybe(o_x)), NDARRAY_FLOAT);
        mp_float_t *array = (mp_float_t *)ndarray->array;
        mp_obj_iter_buf_t x_buf;
        mp_obj_t x_item, x_iterable = mp_getiter(o_x, &x_buf);
        while ((x_item = mp_iternext(x_iterable)) != MP_OBJ_STOP_ITERATION) {
            mp_float_t _x = mp_obj_get_float(x_item);
            mp_float_t y = p[0];
            for(uint8_t j=0; j < plen-1; j++) {
                y *= _x;
                y += p[j+1];
            }
            *array++ = y;
        }
    }
    m_del(mp_float_t, p, plen);
    return MP_OBJ_FROM_PTR(ndarray);
}

MP_DEFINE_CONST_FUN_OBJ_2(poly_polyval_obj, poly_polyval);
#endif
