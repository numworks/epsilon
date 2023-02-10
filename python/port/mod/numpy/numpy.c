
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
 *               2020 Scott Shawcroft for Adafruit Industries
 *               2020-2022 Zoltán Vörös
 *               2020 Taku Fukada
*/

#include <math.h>
#include <string.h>
#include "py/runtime.h"

#include "numpy.h"
#include "approx.h"
#include "carray/carray.h"
#include "compare.h"
#include "create.h"
#include "fft/fft.h"
#include "filter.h"
#include "io/io.h"
#include "linalg/linalg.h"
#include "numerical.h"
#include "stats.h"
#include "transform.h"
#include "poly.h"
#include "vector.h"

//| """Compatibility layer for numpy"""
//|

//| class ndarray: ...

//| def get_printoptions() -> Dict[str, int]:
//|     """Get printing options"""
//|     ...
//|
//| def set_printoptions(threshold: Optional[int] = None, edgeitems: Optional[int] = None) -> None:
//|     """Set printing options"""
//|     ...
//|
//| def ndinfo(array: ulab.numpy.ndarray) -> None:
//|     ...
//|
//| def array(
//|     values: Union[ndarray, Iterable[Union[_float, _bool, Iterable[Any]]]],
//|     *,
//|     dtype: _DType = ulab.numpy.float
//| ) -> ulab.numpy.ndarray:
//|     """alternate constructor function for `ulab.numpy.ndarray`. Mirrors numpy.array"""
//|     ...

// math constants
#if ULAB_NUMPY_HAS_E
ULAB_DEFINE_FLOAT_CONST(ulab_const_float_e, MP_E, 0x402df854UL, 0x4005bf0a8b145769ULL);
#endif

#if ULAB_NUMPY_HAS_INF
ULAB_DEFINE_FLOAT_CONST(numpy_const_float_inf, (mp_float_t)INFINITY, 0x7f800000UL, 0x7ff0000000000000ULL);
#endif

#if ULAB_NUMPY_HAS_NAN
ULAB_DEFINE_FLOAT_CONST(numpy_const_float_nan, (mp_float_t)NAN, 0x7fc00000UL, 0x7ff8000000000000ULL);
#endif

#if ULAB_NUMPY_HAS_PI
ULAB_DEFINE_FLOAT_CONST(ulab_const_float_pi, MP_PI, 0x40490fdbUL, 0x400921fb54442d18ULL);
#endif

static const mp_rom_map_elem_t ulab_numpy_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_numpy) },
    { MP_ROM_QSTR(MP_QSTR_ndarray), MP_ROM_PTR(&ulab_ndarray_type) },
    { MP_ROM_QSTR(MP_QSTR_array), MP_ROM_PTR(&ndarray_array_constructor_obj) },
    #if ULAB_NUMPY_HAS_FROMBUFFER
        { MP_ROM_QSTR(MP_QSTR_frombuffer), MP_ROM_PTR(&create_frombuffer_obj) },
    #endif
    // math constants
    #if ULAB_NUMPY_HAS_E
        { MP_ROM_QSTR(MP_QSTR_e), ULAB_REFERENCE_FLOAT_CONST(ulab_const_float_e) },
    #endif
    #if ULAB_NUMPY_HAS_INF
        { MP_ROM_QSTR(MP_QSTR_inf), ULAB_REFERENCE_FLOAT_CONST(numpy_const_float_inf) },
    #endif
    #if ULAB_NUMPY_HAS_NAN
        { MP_ROM_QSTR(MP_QSTR_nan), ULAB_REFERENCE_FLOAT_CONST(numpy_const_float_nan) },
    #endif
    #if ULAB_NUMPY_HAS_PI
        { MP_ROM_QSTR(MP_QSTR_pi), ULAB_REFERENCE_FLOAT_CONST(ulab_const_float_pi) },
    #endif
    // class constants, always included
    { MP_ROM_QSTR(MP_QSTR_bool), MP_ROM_INT(NDARRAY_BOOL) },
    { MP_ROM_QSTR(MP_QSTR_uint8), MP_ROM_INT(NDARRAY_UINT8) },
    { MP_ROM_QSTR(MP_QSTR_int8), MP_ROM_INT(NDARRAY_INT8) },
    { MP_ROM_QSTR(MP_QSTR_uint16), MP_ROM_INT(NDARRAY_UINT16) },
    { MP_ROM_QSTR(MP_QSTR_int16), MP_ROM_INT(NDARRAY_INT16) },
    { MP_ROM_QSTR(MP_QSTR_float), MP_ROM_INT(NDARRAY_FLOAT) },
    #if ULAB_SUPPORTS_COMPLEX
        { MP_ROM_QSTR(MP_QSTR_complex), MP_ROM_INT(NDARRAY_COMPLEX) },
    #endif
    // modules of numpy
    #if ULAB_NUMPY_HAS_FFT_MODULE
        { MP_ROM_QSTR(MP_QSTR_fft), MP_ROM_PTR(&ulab_fft_module) },
    #endif
    #if ULAB_NUMPY_HAS_LINALG_MODULE
        { MP_ROM_QSTR(MP_QSTR_linalg), MP_ROM_PTR(&ulab_linalg_module) },
    #endif
    #if ULAB_HAS_PRINTOPTIONS
        { MP_ROM_QSTR(MP_QSTR_set_printoptions), MP_ROM_PTR(&ndarray_set_printoptions_obj) },
        { MP_ROM_QSTR(MP_QSTR_get_printoptions), MP_ROM_PTR(&ndarray_get_printoptions_obj) },
    #endif
    #if ULAB_NUMPY_HAS_NDINFO
        { MP_ROM_QSTR(MP_QSTR_ndinfo), MP_ROM_PTR(&ndarray_info_obj) },
    #endif
    #if ULAB_NUMPY_HAS_ARANGE
        { MP_ROM_QSTR(MP_QSTR_arange), MP_ROM_PTR(&create_arange_obj) },
    #endif
    #if ULAB_NUMPY_HAS_COMPRESS
        { MP_ROM_QSTR(MP_QSTR_compress), MP_ROM_PTR(&transform_compress_obj) },
    #endif
    #if ULAB_NUMPY_HAS_CONCATENATE
        { MP_ROM_QSTR(MP_QSTR_concatenate), MP_ROM_PTR(&create_concatenate_obj) },
    #endif
    #if ULAB_NUMPY_HAS_DELETE
        { MP_ROM_QSTR(MP_QSTR_delete), MP_ROM_PTR(&transform_delete_obj) },
    #endif
    #if ULAB_NUMPY_HAS_DIAG
        #if ULAB_MAX_DIMS > 1
            { MP_ROM_QSTR(MP_QSTR_diag), MP_ROM_PTR(&create_diag_obj) },
        #endif
    #endif
    #if ULAB_NUMPY_HAS_EMPTY
        { MP_ROM_QSTR(MP_QSTR_empty), MP_ROM_PTR(&create_zeros_obj) },
    #endif
    #if ULAB_MAX_DIMS > 1
        #if ULAB_NUMPY_HAS_EYE
            { MP_ROM_QSTR(MP_QSTR_eye), MP_ROM_PTR(&create_eye_obj) },
        #endif
    #endif /* ULAB_MAX_DIMS */
    // functions of the approx sub-module
    #if ULAB_NUMPY_HAS_INTERP
        { MP_ROM_QSTR(MP_QSTR_interp), MP_ROM_PTR(&approx_interp_obj) },
    #endif
    #if ULAB_NUMPY_HAS_TRAPZ
        { MP_ROM_QSTR(MP_QSTR_trapz), MP_ROM_PTR(&approx_trapz_obj) },
    #endif
    // functions of the create sub-module
    #if ULAB_NUMPY_HAS_FULL
        { MP_ROM_QSTR(MP_QSTR_full), MP_ROM_PTR(&create_full_obj) },
    #endif
    #if ULAB_NUMPY_HAS_LINSPACE
        { MP_ROM_QSTR(MP_QSTR_linspace), MP_ROM_PTR(&create_linspace_obj) },
    #endif
    #if ULAB_NUMPY_HAS_LOGSPACE
        { MP_ROM_QSTR(MP_QSTR_logspace), MP_ROM_PTR(&create_logspace_obj) },
    #endif
    #if ULAB_NUMPY_HAS_ONES
        { MP_ROM_QSTR(MP_QSTR_ones), MP_ROM_PTR(&create_ones_obj) },
    #endif
    #if ULAB_NUMPY_HAS_ZEROS
        { MP_ROM_QSTR(MP_QSTR_zeros), MP_ROM_PTR(&create_zeros_obj) },
    #endif
    // functions of the compare sub-module
    #if ULAB_NUMPY_HAS_CLIP
        { MP_ROM_QSTR(MP_QSTR_clip), MP_ROM_PTR(&compare_clip_obj) },
    #endif
    #if ULAB_NUMPY_HAS_EQUAL
        { MP_ROM_QSTR(MP_QSTR_equal), MP_ROM_PTR(&compare_equal_obj) },
    #endif
    #if ULAB_NUMPY_HAS_NOTEQUAL
        { MP_ROM_QSTR(MP_QSTR_not_equal), MP_ROM_PTR(&compare_not_equal_obj) },
    #endif
    #if ULAB_NUMPY_HAS_ISFINITE
        { MP_ROM_QSTR(MP_QSTR_isfinite), MP_ROM_PTR(&compare_isfinite_obj) },
    #endif
    #if ULAB_NUMPY_HAS_ISINF
        { MP_ROM_QSTR(MP_QSTR_isinf), MP_ROM_PTR(&compare_isinf_obj) },
    #endif
    #if ULAB_NUMPY_HAS_MAXIMUM
        { MP_ROM_QSTR(MP_QSTR_maximum), MP_ROM_PTR(&compare_maximum_obj) },
    #endif
    #if ULAB_NUMPY_HAS_MINIMUM
        { MP_ROM_QSTR(MP_QSTR_minimum), MP_ROM_PTR(&compare_minimum_obj) },
    #endif
    #if ULAB_NUMPY_HAS_NONZERO
        { MP_ROM_QSTR(MP_QSTR_nonzero), MP_ROM_PTR(&compare_nonzero_obj) },
    #endif

    #if ULAB_NUMPY_HAS_WHERE
        { MP_ROM_QSTR(MP_QSTR_where), MP_ROM_PTR(&compare_where_obj) },
    #endif
    // functions of the filter sub-module
    #if ULAB_NUMPY_HAS_CONVOLVE
        { MP_ROM_QSTR(MP_QSTR_convolve), MP_ROM_PTR(&filter_convolve_obj) },
    #endif
    // functions of the numerical sub-module
    #if ULAB_NUMPY_HAS_ALL
        { MP_ROM_QSTR(MP_QSTR_all), MP_ROM_PTR(&numerical_all_obj) },
    #endif
    #if ULAB_NUMPY_HAS_ANY
        { MP_ROM_QSTR(MP_QSTR_any), MP_ROM_PTR(&numerical_any_obj) },
    #endif
    #if ULAB_NUMPY_HAS_ARGMINMAX
        { MP_ROM_QSTR(MP_QSTR_argmax), MP_ROM_PTR(&numerical_argmax_obj) },
        { MP_ROM_QSTR(MP_QSTR_argmin), MP_ROM_PTR(&numerical_argmin_obj) },
    #endif
    #if ULAB_NUMPY_HAS_ARGSORT
        { MP_ROM_QSTR(MP_QSTR_argsort), MP_ROM_PTR(&numerical_argsort_obj) },
    #endif
    #if ULAB_NUMPY_HAS_ASARRAY
        { MP_ROM_QSTR(MP_QSTR_asarray), MP_ROM_PTR(&create_asarray_obj) },
    #endif
    #if ULAB_NUMPY_HAS_CROSS
        { MP_ROM_QSTR(MP_QSTR_cross), MP_ROM_PTR(&numerical_cross_obj) },
    #endif
    #if ULAB_NUMPY_HAS_DIFF
        { MP_ROM_QSTR(MP_QSTR_diff), MP_ROM_PTR(&numerical_diff_obj) },
    #endif
    #if ULAB_NUMPY_HAS_DOT
        #if ULAB_MAX_DIMS > 1
            { MP_ROM_QSTR(MP_QSTR_dot), MP_ROM_PTR(&transform_dot_obj) },
        #endif
    #endif
    #if ULAB_NUMPY_HAS_TRACE
        #if ULAB_MAX_DIMS > 1
            { MP_ROM_QSTR(MP_QSTR_trace), MP_ROM_PTR(&stats_trace_obj) },
        #endif
    #endif
    #if ULAB_NUMPY_HAS_FLIP
        { MP_ROM_QSTR(MP_QSTR_flip), MP_ROM_PTR(&numerical_flip_obj) },
    #endif
    #if ULAB_NUMPY_HAS_LOAD
        { MP_ROM_QSTR(MP_QSTR_load), MP_ROM_PTR(&io_load_obj) },
    #endif
    #if ULAB_NUMPY_HAS_LOADTXT
        { MP_ROM_QSTR(MP_QSTR_loadtxt), MP_ROM_PTR(&io_loadtxt_obj) },
    #endif
    #if ULAB_NUMPY_HAS_MINMAX
        { MP_ROM_QSTR(MP_QSTR_max), MP_ROM_PTR(&numerical_max_obj) },
    #endif
    #if ULAB_NUMPY_HAS_MEAN
        { MP_ROM_QSTR(MP_QSTR_mean), MP_ROM_PTR(&numerical_mean_obj) },
    #endif
    #if ULAB_NUMPY_HAS_MEDIAN
        { MP_ROM_QSTR(MP_QSTR_median), MP_ROM_PTR(&numerical_median_obj) },
    #endif
    #if ULAB_NUMPY_HAS_MINMAX
        { MP_ROM_QSTR(MP_QSTR_min), MP_ROM_PTR(&numerical_min_obj) },
    #endif
    #if ULAB_NUMPY_HAS_ROLL
        { MP_ROM_QSTR(MP_QSTR_roll), MP_ROM_PTR(&numerical_roll_obj) },
    #endif
    #if ULAB_NUMPY_HAS_SAVE
        { MP_ROM_QSTR(MP_QSTR_save), MP_ROM_PTR(&io_save_obj) },
    #endif
    #if ULAB_NUMPY_HAS_SAVETXT
        { MP_ROM_QSTR(MP_QSTR_savetxt), MP_ROM_PTR(&io_savetxt_obj) },
    #endif
    #if ULAB_NUMPY_HAS_SIZE
        { MP_ROM_QSTR(MP_QSTR_size), MP_ROM_PTR(&transform_size_obj) },
    #endif
    #if ULAB_NUMPY_HAS_SORT
        { MP_ROM_QSTR(MP_QSTR_sort), MP_ROM_PTR(&numerical_sort_obj) },
    #endif
    #if ULAB_NUMPY_HAS_STD
        { MP_ROM_QSTR(MP_QSTR_std), MP_ROM_PTR(&numerical_std_obj) },
    #endif
    #if ULAB_NUMPY_HAS_SUM
        { MP_ROM_QSTR(MP_QSTR_sum), MP_ROM_PTR(&numerical_sum_obj) },
    #endif
    // functions of the poly sub-module
    #if ULAB_NUMPY_HAS_POLYFIT
        { MP_ROM_QSTR(MP_QSTR_polyfit), MP_ROM_PTR(&poly_polyfit_obj) },
    #endif
    #if ULAB_NUMPY_HAS_POLYVAL
        { MP_ROM_QSTR(MP_QSTR_polyval), MP_ROM_PTR(&poly_polyval_obj) },
    #endif
    // functions of the vector sub-module
    #if ULAB_NUMPY_HAS_ACOS
    { MP_ROM_QSTR(MP_QSTR_acos), MP_ROM_PTR(&vector_acos_obj) },
    #endif
    #if ULAB_NUMPY_HAS_ACOSH
    { MP_ROM_QSTR(MP_QSTR_acosh), MP_ROM_PTR(&vector_acosh_obj) },
    #endif
    #if ULAB_NUMPY_HAS_ARCTAN2
    { MP_ROM_QSTR(MP_QSTR_arctan2), MP_ROM_PTR(&vector_arctan2_obj) },
    #endif
    #if ULAB_NUMPY_HAS_AROUND
    { MP_ROM_QSTR(MP_QSTR_around), MP_ROM_PTR(&vector_around_obj) },
    #endif
    #if ULAB_NUMPY_HAS_ASIN
    { MP_ROM_QSTR(MP_QSTR_asin), MP_ROM_PTR(&vector_asin_obj) },
    #endif
    #if ULAB_NUMPY_HAS_ASINH
    { MP_ROM_QSTR(MP_QSTR_asinh), MP_ROM_PTR(&vector_asinh_obj) },
    #endif
    #if ULAB_NUMPY_HAS_ATAN
    { MP_ROM_QSTR(MP_QSTR_atan), MP_ROM_PTR(&vector_atan_obj) },
    #endif
    #if ULAB_NUMPY_HAS_ATANH
    { MP_ROM_QSTR(MP_QSTR_atanh), MP_ROM_PTR(&vector_atanh_obj) },
    #endif
    #if ULAB_NUMPY_HAS_CEIL
    { MP_ROM_QSTR(MP_QSTR_ceil), MP_ROM_PTR(&vector_ceil_obj) },
    #endif
    #if ULAB_NUMPY_HAS_COS
    { MP_ROM_QSTR(MP_QSTR_cos), MP_ROM_PTR(&vector_cos_obj) },
    #endif
    #if ULAB_NUMPY_HAS_COSH
    { MP_ROM_QSTR(MP_QSTR_cosh), MP_ROM_PTR(&vector_cosh_obj) },
    #endif
    #if ULAB_NUMPY_HAS_DEGREES
    { MP_ROM_QSTR(MP_QSTR_degrees), MP_ROM_PTR(&vector_degrees_obj) },
    #endif
    #if ULAB_NUMPY_HAS_EXP
    { MP_ROM_QSTR(MP_QSTR_exp), MP_ROM_PTR(&vector_exp_obj) },
    #endif
    #if ULAB_NUMPY_HAS_EXPM1
    { MP_ROM_QSTR(MP_QSTR_expm1), MP_ROM_PTR(&vector_expm1_obj) },
    #endif
    #if ULAB_NUMPY_HAS_FLOOR
    { MP_ROM_QSTR(MP_QSTR_floor), MP_ROM_PTR(&vector_floor_obj) },
    #endif
    #if ULAB_NUMPY_HAS_LOG
    { MP_ROM_QSTR(MP_QSTR_log), MP_ROM_PTR(&vector_log_obj) },
    #endif
    #if ULAB_NUMPY_HAS_LOG10
    { MP_ROM_QSTR(MP_QSTR_log10), MP_ROM_PTR(&vector_log10_obj) },
    #endif
    #if ULAB_NUMPY_HAS_LOG2
    { MP_ROM_QSTR(MP_QSTR_log2), MP_ROM_PTR(&vector_log2_obj) },
    #endif
    #if ULAB_NUMPY_HAS_RADIANS
    { MP_ROM_QSTR(MP_QSTR_radians), MP_ROM_PTR(&vector_radians_obj) },
    #endif
    #if ULAB_NUMPY_HAS_SIN
    { MP_ROM_QSTR(MP_QSTR_sin), MP_ROM_PTR(&vector_sin_obj) },
    #endif
    #if ULAB_NUMPY_HAS_SINH
    { MP_ROM_QSTR(MP_QSTR_sinh), MP_ROM_PTR(&vector_sinh_obj) },
    #endif
    #if ULAB_NUMPY_HAS_SQRT
    { MP_ROM_QSTR(MP_QSTR_sqrt), MP_ROM_PTR(&vector_sqrt_obj) },
    #endif
    #if ULAB_NUMPY_HAS_TAN
    { MP_ROM_QSTR(MP_QSTR_tan), MP_ROM_PTR(&vector_tan_obj) },
    #endif
    #if ULAB_NUMPY_HAS_TANH
    { MP_ROM_QSTR(MP_QSTR_tanh), MP_ROM_PTR(&vector_tanh_obj) },
    #endif
    #if ULAB_NUMPY_HAS_VECTORIZE
    { MP_ROM_QSTR(MP_QSTR_vectorize), MP_ROM_PTR(&vector_vectorize_obj) },
    #endif
    #if ULAB_SUPPORTS_COMPLEX
        #if ULAB_NUMPY_HAS_REAL
        { MP_ROM_QSTR(MP_QSTR_real), MP_ROM_PTR(&carray_real_obj) },
        #endif
        #if ULAB_NUMPY_HAS_IMAG
        { MP_ROM_QSTR(MP_QSTR_imag), MP_ROM_PTR(&carray_imag_obj) },
        #endif
        #if ULAB_NUMPY_HAS_CONJUGATE
            { MP_ROM_QSTR(MP_QSTR_conjugate), MP_ROM_PTR(&carray_conjugate_obj) },
        #endif
        #if ULAB_NUMPY_HAS_SORT_COMPLEX
            { MP_ROM_QSTR(MP_QSTR_sort_complex), MP_ROM_PTR(&carray_sort_complex_obj) },
        #endif
    #endif
};

static MP_DEFINE_CONST_DICT(mp_module_ulab_numpy_globals, ulab_numpy_globals_table);

const mp_obj_module_t ulab_numpy_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_numpy_globals,
};

#if CIRCUITPY_ULAB
#if !defined(MICROPY_VERSION) || MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_ulab_dot_numpy, ulab_numpy_module, MODULE_ULAB_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_ulab_dot_numpy, ulab_numpy_module);
#endif
#endif
