/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2021 Zoltán Vörös
*/

#include <math.h>
#include <string.h>
#include "py/runtime.h"

#include "../../ndarray.h"
#include "../../ulab_tools.h"
#include "../carray/carray_tools.h"
#include "fft_tools.h"

#ifndef MP_PI
#define MP_PI MICROPY_FLOAT_CONST(3.14159265358979323846)
#endif
#ifndef MP_E
#define MP_E MICROPY_FLOAT_CONST(2.71828182845904523536)
#endif

/* Kernel implementation for the case, when ulab has no complex support

 * The following function takes two arrays, namely, the real and imaginary
 * parts of a complex array, and calculates the Fourier transform in place.
 *
 * The function is basically a modification of four1 from Numerical Recipes,
 * has no dependencies beyond micropython itself (for the definition of mp_float_t),
 * and can be used independent of ulab.
 */

#if ULAB_SUPPORTS_COMPLEX & ULAB_FFT_IS_NUMPY_COMPATIBLE
/* Kernel implementation for the complex case. Data are contained in data as

    data[0], data[1], data[2], data[3], .... , data[2n - 2], data[2n-1]
    real[0], imag[0], real[1], imag[1], .... , real[n-1],    imag[n-1]

    In general
    real[i] = data[2i]
    imag[i] = data[2i+1]

*/
void fft_kernel_complex(mp_float_t *data, size_t n, int isign) {
    size_t j, m, mmax, istep;
    mp_float_t tempr, tempi;
    mp_float_t wtemp, wr, wpr, wpi, wi, theta;

    j = 0;
    for(size_t i = 0; i < n; i++) {
        if (j > i) {
            SWAP(mp_float_t, data[2*i], data[2*j]);
            SWAP(mp_float_t, data[2*i+1], data[2*j+1]);
        }
        m = n >> 1;
        while (j >= m && m > 0) {
            j -= m;
            m >>= 1;
        }
        j += m;
    }

    mmax = 1;
    while (n > mmax) {
        istep = mmax << 1;
        theta = MICROPY_FLOAT_CONST(-2.0)*isign*MP_PI/istep;
        wtemp = MICROPY_FLOAT_C_FUN(sin)(MICROPY_FLOAT_CONST(0.5) * theta);
        wpr = MICROPY_FLOAT_CONST(-2.0) * wtemp * wtemp;
        wpi = MICROPY_FLOAT_C_FUN(sin)(theta);
        wr = MICROPY_FLOAT_CONST(1.0);
        wi = MICROPY_FLOAT_CONST(0.0);
        for(m = 0; m < mmax; m++) {
            for(size_t i = m; i < n; i += istep) {
                j = i + mmax;
                tempr = wr * data[2*j] - wi * data[2*j+1];
                tempi = wr * data[2*j+1] + wi * data[2*j];
                data[2*j] = data[2*i] - tempr;
                data[2*j+1] = data[2*i+1] - tempi;
                data[2*i] += tempr;
                data[2*i+1] += tempi;
            }
            wtemp = wr;
            wr = wr*wpr - wi*wpi + wr;
            wi = wi*wpr + wtemp*wpi + wi;
        }
        mmax = istep;
    }
}

/*
 * The following function is a helper interface to the python side.
 * It has been factored out from fft.c, so that the same argument parsing
 * routine can be called from scipy.signal.spectrogram.
 */
mp_obj_t fft_fft_ifft_spectrogram(mp_obj_t data_in, uint8_t type) {
    if(!mp_obj_is_type(data_in, &ulab_ndarray_type)) {
        mp_raise_NotImplementedError(translate("FFT is defined for ndarrays only"));
    }
    ndarray_obj_t *in = MP_OBJ_TO_PTR(data_in);
    #if ULAB_MAX_DIMS > 1
    if(in->ndim != 1) {
        mp_raise_TypeError(translate("FFT is implemented for linear arrays only"));
    }
    #endif
    size_t len = in->len;
    // Check if input is of length of power of 2
    if((len & (len-1)) != 0) {
        mp_raise_ValueError(translate("input array length must be power of 2"));
    }

    ndarray_obj_t *out = ndarray_new_linear_array(len, NDARRAY_COMPLEX);
    mp_float_t *data = (mp_float_t *)out->array;
    uint8_t *array = (uint8_t *)in->array;

    if(in->dtype == NDARRAY_COMPLEX) {
        uint8_t sz = 2 * sizeof(mp_float_t);
        uint8_t *data_ = (uint8_t *)out->array;
        for(size_t i = 0; i < len; i++) {
            memcpy(data_, array, sz);
            array += in->strides[ULAB_MAX_DIMS - 1];
        }
    } else {
        mp_float_t (*func)(void *) = ndarray_get_float_function(in->dtype);
        for(size_t i = 0; i < len; i++) {
            // real part; the imaginary part is 0, no need to assign
            *data = func(array);
            data += 2;
            array += in->strides[ULAB_MAX_DIMS - 1];
        }
    }
    data -= 2 * len;

    if((type == FFT_FFT) || (type == FFT_SPECTROGRAM)) {
        fft_kernel_complex(data, len, 1);
        if(type == FFT_SPECTROGRAM) {
            ndarray_obj_t *spectrum = ndarray_new_linear_array(len, NDARRAY_FLOAT);
            mp_float_t *sarray = (mp_float_t *)spectrum->array;
            for(size_t i = 0; i < len; i++) {
                *sarray++ = MICROPY_FLOAT_C_FUN(sqrt)(data[0] * data[0] + data[1] * data[1]);
                data += 2;
            }
            m_del(mp_float_t, data, 2 * len);
            return MP_OBJ_FROM_PTR(spectrum);
        }
    } else { // inverse transform
        fft_kernel_complex(data, len, -1);
        // TODO: numpy accepts the norm keyword argument
        for(size_t i = 0; i < len; i++) {
            *data++ /= len;
        }
    }
    return MP_OBJ_FROM_PTR(out);
}
#else /* ULAB_SUPPORTS_COMPLEX & ULAB_FFT_IS_NUMPY_COMPATIBLE */
void fft_kernel(mp_float_t *real, mp_float_t *imag, size_t n, int isign) {
    size_t j, m, mmax, istep;
    mp_float_t tempr, tempi;
    mp_float_t wtemp, wr, wpr, wpi, wi, theta;

    j = 0;
    for(size_t i = 0; i < n; i++) {
        if (j > i) {
            SWAP(mp_float_t, real[i], real[j]);
            SWAP(mp_float_t, imag[i], imag[j]);
        }
        m = n >> 1;
        while (j >= m && m > 0) {
            j -= m;
            m >>= 1;
        }
        j += m;
    }

    mmax = 1;
    while (n > mmax) {
        istep = mmax << 1;
        theta = MICROPY_FLOAT_CONST(-2.0)*isign*MP_PI/istep;
        wtemp = MICROPY_FLOAT_C_FUN(sin)(MICROPY_FLOAT_CONST(0.5) * theta);
        wpr = MICROPY_FLOAT_CONST(-2.0) * wtemp * wtemp;
        wpi = MICROPY_FLOAT_C_FUN(sin)(theta);
        wr = MICROPY_FLOAT_CONST(1.0);
        wi = MICROPY_FLOAT_CONST(0.0);
        for(m = 0; m < mmax; m++) {
            for(size_t i = m; i < n; i += istep) {
                j = i + mmax;
                tempr = wr * real[j] - wi * imag[j];
                tempi = wr * imag[j] + wi * real[j];
                real[j] = real[i] - tempr;
                imag[j] = imag[i] - tempi;
                real[i] += tempr;
                imag[i] += tempi;
            }
            wtemp = wr;
            wr = wr*wpr - wi*wpi + wr;
            wi = wi*wpr + wtemp*wpi + wi;
        }
        mmax = istep;
    }
}

mp_obj_t fft_fft_ifft_spectrogram(size_t n_args, mp_obj_t arg_re, mp_obj_t arg_im, uint8_t type) {
    if(!mp_obj_is_type(arg_re, &ulab_ndarray_type)) {
        mp_raise_NotImplementedError(translate("FFT is defined for ndarrays only"));
    }
    if(n_args == 2) {
        if(!mp_obj_is_type(arg_im, &ulab_ndarray_type)) {
            mp_raise_NotImplementedError(translate("FFT is defined for ndarrays only"));
        }
    }
    ndarray_obj_t *re = MP_OBJ_TO_PTR(arg_re);
    #if ULAB_MAX_DIMS > 1
    if(re->ndim != 1) {
        COMPLEX_DTYPE_NOT_IMPLEMENTED(re->dtype)
        mp_raise_TypeError(translate("FFT is implemented for linear arrays only"));
    }
    #endif
    size_t len = re->len;
    // Check if input is of length of power of 2
    if((len & (len-1)) != 0) {
        mp_raise_ValueError(translate("input array length must be power of 2"));
    }

    ndarray_obj_t *out_re = ndarray_new_linear_array(len, NDARRAY_FLOAT);
    mp_float_t *data_re = (mp_float_t *)out_re->array;

    uint8_t *array = (uint8_t *)re->array;
    mp_float_t (*func)(void *) = ndarray_get_float_function(re->dtype);

    for(size_t i=0; i < len; i++) {
        *data_re++ = func(array);
        array += re->strides[ULAB_MAX_DIMS - 1];
    }
    data_re -= len;
    ndarray_obj_t *out_im = ndarray_new_linear_array(len, NDARRAY_FLOAT);
    mp_float_t *data_im = (mp_float_t *)out_im->array;

    if(n_args == 2) {
        ndarray_obj_t *im = MP_OBJ_TO_PTR(arg_im);
        #if ULAB_MAX_DIMS > 1
        if(im->ndim != 1) {
            COMPLEX_DTYPE_NOT_IMPLEMENTED(im->dtype)
            mp_raise_TypeError(translate("FFT is implemented for linear arrays only"));
        }
        #endif
        if (re->len != im->len) {
            mp_raise_ValueError(translate("real and imaginary parts must be of equal length"));
        }
        array = (uint8_t *)im->array;
        func = ndarray_get_float_function(im->dtype);
        for(size_t i=0; i < len; i++) {
           *data_im++ = func(array);
           array += im->strides[ULAB_MAX_DIMS - 1];
        }
        data_im -= len;
    }

    if((type == FFT_FFT) || (type == FFT_SPECTROGRAM)) {
        fft_kernel(data_re, data_im, len, 1);
        if(type == FFT_SPECTROGRAM) {
            for(size_t i=0; i < len; i++) {
                *data_re = MICROPY_FLOAT_C_FUN(sqrt)(*data_re * *data_re + *data_im * *data_im);
                data_re++;
                data_im++;
            }
        }
    } else { // inverse transform
        fft_kernel(data_re, data_im, len, -1);
        // TODO: numpy accepts the norm keyword argument
        for(size_t i=0; i < len; i++) {
            *data_re++ /= len;
            *data_im++ /= len;
        }
    }
    if(type == FFT_SPECTROGRAM) {
        return MP_OBJ_TO_PTR(out_re);
    } else {
        mp_obj_t tuple[2];
        tuple[0] = out_re;
        tuple[1] = out_im;
        return mp_obj_new_tuple(2, tuple);
    }
}
#endif  /* ULAB_SUPPORTS_COMPLEX & ULAB_FFT_IS_NUMPY_COMPATIBLE */
