/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2021 Zoltán Vörös
 *               2020 Scott Shawcroft for Adafruit Industries
 *               2020 Taku Fukada
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/binary.h"
#include "py/obj.h"
#include "py/objarray.h"

#include "../carray/carray_tools.h"
#include "fft.h"

//| """Frequency-domain functions"""
//|
//| import ulab.numpy
//| import ulab.utils


//| def fft(r: ulab.numpy.ndarray, c: Optional[ulab.numpy.ndarray] = None) -> Tuple[ulab.numpy.ndarray, ulab.numpy.ndarray]:
//|     """
//|     :param ulab.numpy.ndarray r: A 1-dimension array of values whose size is a power of 2
//|     :param ulab.numpy.ndarray c: An optional 1-dimension array of values whose size is a power of 2, giving the complex part of the value
//|     :return tuple (r, c): The real and complex parts of the FFT
//|
//|     Perform a Fast Fourier Transform from the time domain into the frequency domain
//|
//|     See also `ulab.utils.spectrogram`, which computes the magnitude of the fft,
//|     rather than separately returning its real and imaginary parts."""
//|     ...
//|
#if ULAB_SUPPORTS_COMPLEX & ULAB_FFT_IS_NUMPY_COMPATIBLE
static mp_obj_t fft_fft(mp_obj_t arg) {
    return fft_fft_ifft_spectrogram(arg, FFT_FFT);
}

MP_DEFINE_CONST_FUN_OBJ_1(fft_fft_obj, fft_fft);
#else
static mp_obj_t fft_fft(size_t n_args, const mp_obj_t *args) {
    if(n_args == 2) {
        return fft_fft_ifft_spectrogram(n_args, args[0], args[1], FFT_FFT);
    } else {
        return fft_fft_ifft_spectrogram(n_args, args[0], mp_const_none, FFT_FFT);
    }
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(fft_fft_obj, 1, 2, fft_fft);
#endif

//| def ifft(r: ulab.numpy.ndarray, c: Optional[ulab.numpy.ndarray] = None) -> Tuple[ulab.numpy.ndarray, ulab.numpy.ndarray]:
//|     """
//|     :param ulab.numpy.ndarray r: A 1-dimension array of values whose size is a power of 2
//|     :param ulab.numpy.ndarray c: An optional 1-dimension array of values whose size is a power of 2, giving the complex part of the value
//|     :return tuple (r, c): The real and complex parts of the inverse FFT
//|
//|     Perform an Inverse Fast Fourier Transform from the frequeny domain into the time domain"""
//|     ...
//|

#if ULAB_SUPPORTS_COMPLEX & ULAB_FFT_IS_NUMPY_COMPATIBLE
static mp_obj_t fft_ifft(mp_obj_t arg) {
    return fft_fft_ifft_spectrogram(arg, FFT_IFFT);
}

MP_DEFINE_CONST_FUN_OBJ_1(fft_ifft_obj, fft_ifft);
#else
static mp_obj_t fft_ifft(size_t n_args, const mp_obj_t *args) {
    NOT_IMPLEMENTED_FOR_COMPLEX()
    if(n_args == 2) {
        return fft_fft_ifft_spectrogram(n_args, args[0], args[1], FFT_IFFT);
    } else {
        return fft_fft_ifft_spectrogram(n_args, args[0], mp_const_none, FFT_IFFT);
    }
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(fft_ifft_obj, 1, 2, fft_ifft);
#endif

STATIC const mp_rom_map_elem_t ulab_fft_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_fft) },
    { MP_ROM_QSTR(MP_QSTR_fft), MP_ROM_PTR(&fft_fft_obj) },
    { MP_ROM_QSTR(MP_QSTR_ifft), MP_ROM_PTR(&fft_ifft_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_ulab_fft_globals, ulab_fft_globals_table);

const mp_obj_module_t ulab_fft_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_fft_globals,
};
#if CIRCUITPY_ULAB
#if !defined(MICROPY_VERSION) || MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_ulab_dot_numpy_dot_fft, ulab_fft_module, MODULE_ULAB_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_ulab_dot_numpy_dot_fft, ulab_fft_module);
#endif
#endif
