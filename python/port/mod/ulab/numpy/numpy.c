
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
#if MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_C
#define ulab_const_float_e MP_ROM_PTR((mp_obj_t)(((0x402df854 & ~3) | 2) + 0x80800000))
#elif MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_D
#define ulab_const_float_e {((mp_obj_t)((uint64_t)0x4005bf0a8b145769 + 0x8004000000000000))}
#else
mp_obj_float_t ulab_const_float_e_obj = {{&mp_type_float}, MP_E};
#define ulab_const_float_e MP_ROM_PTR(&ulab_const_float_e_obj)
#endif
#endif

#if ULAB_NUMPY_HAS_INF
#if MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_C
#define numpy_const_float_inf MP_ROM_PTR((mp_obj_t)(0x7f800002 + 0x80800000))
#elif MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_D
#define numpy_const_float_inf {((mp_obj_t)((uint64_t)0x7ff0000000000000 + 0x8004000000000000))}
#else
mp_obj_float_t numpy_const_float_inf_obj = {{&mp_type_float}, (mp_float_t)INFINITY};
#define numpy_const_float_inf MP_ROM_PTR(&numpy_const_float_inf_obj)
#endif
#endif

#if ULAB_NUMPY_HAS_NAN
#if MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_C
#define numpy_const_float_nan MP_ROM_PTR((mp_obj_t)(0x7fc00002 + 0x80800000))
#elif MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_D
#define numpy_const_float_nan {((mp_obj_t)((uint64_t)0x7ff8000000000000 + 0x8004000000000000))}
#else
mp_obj_float_t numpy_const_float_nan_obj = {{&mp_type_float}, (mp_float_t)NAN};
#define numpy_const_float_nan MP_ROM_PTR(&numpy_const_float_nan_obj)
#endif
#endif

#if ULAB_NUMPY_HAS_PI
#if MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_C
#define ulab_const_float_pi MP_ROM_PTR((mp_obj_t)(((0x40490fdb & ~3) | 2) + 0x80800000))
#elif MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_D
#define ulab_const_float_pi {((mp_obj_t)((uint64_t)0x400921fb54442d18 + 0x8004000000000000))}
#else
mp_obj_float_t ulab_const_float_pi_obj = {{&mp_type_float}, MP_PI};
#define ulab_const_float_pi MP_ROM_PTR(&ulab_const_float_pi_obj)
#endif
#endif

static const mp_rom_map_elem_t ulab_numpy_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_numpy) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ndarray), (mp_obj_t)&ulab_ndarray_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_array), MP_ROM_PTR(&ndarray_array_constructor_obj) },
    #if ULAB_NUMPY_HAS_FROMBUFFER
        { MP_ROM_QSTR(MP_QSTR_frombuffer), MP_ROM_PTR(&create_frombuffer_obj) },
    #endif
    // math constants
    #if ULAB_NUMPY_HAS_E
        { MP_ROM_QSTR(MP_QSTR_e), ulab_const_float_e },
    #endif
    #if ULAB_NUMPY_HAS_INF
        { MP_ROM_QSTR(MP_QSTR_inf), numpy_const_float_inf },
    #endif
    #if ULAB_NUMPY_HAS_NAN
        { MP_ROM_QSTR(MP_QSTR_nan), numpy_const_float_nan },
    #endif
    #if ULAB_NUMPY_HAS_PI
        { MP_ROM_QSTR(MP_QSTR_pi), ulab_const_float_pi },
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
        { MP_ROM_QSTR(MP_QSTR_set_printoptions), (mp_obj_t)&ndarray_set_printoptions_obj },
        { MP_ROM_QSTR(MP_QSTR_get_printoptions), (mp_obj_t)&ndarray_get_printoptions_obj },
    #endif
    #if ULAB_NUMPY_HAS_NDINFO
        { MP_ROM_QSTR(MP_QSTR_ndinfo), (mp_obj_t)&ndarray_info_obj },
    #endif
    #if ULAB_NUMPY_HAS_ARANGE
        { MP_ROM_QSTR(MP_QSTR_arange), (mp_obj_t)&create_arange_obj },
    #endif
    #if ULAB_NUMPY_HAS_COMPRESS
        { MP_ROM_QSTR(MP_QSTR_compress), (mp_obj_t)&transform_compress_obj },
    #endif
    #if ULAB_NUMPY_HAS_CONCATENATE
        { MP_ROM_QSTR(MP_QSTR_concatenate), (mp_obj_t)&create_concatenate_obj },
    #endif
    #if ULAB_NUMPY_HAS_DELETE
        { MP_ROM_QSTR(MP_QSTR_delete), (mp_obj_t)&transform_delete_obj },
    #endif
    #if ULAB_NUMPY_HAS_DIAG
        #if ULAB_MAX_DIMS > 1
            { MP_ROM_QSTR(MP_QSTR_diag), (mp_obj_t)&create_diag_obj },
        #endif
    #endif
    #if ULAB_NUMPY_HAS_EMPTY
        { MP_ROM_QSTR(MP_QSTR_empty), (mp_obj_t)&create_zeros_obj },
    #endif
    #if ULAB_MAX_DIMS > 1
        #if ULAB_NUMPY_HAS_EYE
            { MP_ROM_QSTR(MP_QSTR_eye), (mp_obj_t)&create_eye_obj },
        #endif
    #endif /* ULAB_MAX_DIMS */
    // functions of the approx sub-module
    #if ULAB_NUMPY_HAS_INTERP
        { MP_OBJ_NEW_QSTR(MP_QSTR_interp), (mp_obj_t)&approx_interp_obj },
    #endif
    #if ULAB_NUMPY_HAS_TRAPZ
        { MP_OBJ_NEW_QSTR(MP_QSTR_trapz), (mp_obj_t)&approx_trapz_obj },
    #endif
    // functions of the create sub-module
    #if ULAB_NUMPY_HAS_FULL
        { MP_ROM_QSTR(MP_QSTR_full), (mp_obj_t)&create_full_obj },
    #endif
    #if ULAB_NUMPY_HAS_LINSPACE
        { MP_ROM_QSTR(MP_QSTR_linspace), (mp_obj_t)&create_linspace_obj },
    #endif
    #if ULAB_NUMPY_HAS_LOGSPACE
        { MP_ROM_QSTR(MP_QSTR_logspace), (mp_obj_t)&create_logspace_obj },
    #endif
    #if ULAB_NUMPY_HAS_ONES
        { MP_ROM_QSTR(MP_QSTR_ones), (mp_obj_t)&create_ones_obj },
    #endif
    #if ULAB_NUMPY_HAS_ZEROS
        { MP_ROM_QSTR(MP_QSTR_zeros), (mp_obj_t)&create_zeros_obj },
    #endif
    // functions of the compare sub-module
    #if ULAB_NUMPY_HAS_CLIP
        { MP_OBJ_NEW_QSTR(MP_QSTR_clip), (mp_obj_t)&compare_clip_obj },
    #endif
    #if ULAB_NUMPY_HAS_EQUAL
        { MP_OBJ_NEW_QSTR(MP_QSTR_equal), (mp_obj_t)&compare_equal_obj },
    #endif
    #if ULAB_NUMPY_HAS_NOTEQUAL
        { MP_OBJ_NEW_QSTR(MP_QSTR_not_equal), (mp_obj_t)&compare_not_equal_obj },
    #endif
    #if ULAB_NUMPY_HAS_ISFINITE
        { MP_OBJ_NEW_QSTR(MP_QSTR_isfinite), (mp_obj_t)&compare_isfinite_obj },
    #endif
    #if ULAB_NUMPY_HAS_ISINF
        { MP_OBJ_NEW_QSTR(MP_QSTR_isinf), (mp_obj_t)&compare_isinf_obj },
    #endif
    #if ULAB_NUMPY_HAS_MAXIMUM
        { MP_OBJ_NEW_QSTR(MP_QSTR_maximum), (mp_obj_t)&compare_maximum_obj },
    #endif
    #if ULAB_NUMPY_HAS_MINIMUM
        { MP_OBJ_NEW_QSTR(MP_QSTR_minimum), (mp_obj_t)&compare_minimum_obj },
    #endif
    #if ULAB_NUMPY_HAS_WHERE
        { MP_OBJ_NEW_QSTR(MP_QSTR_where), (mp_obj_t)&compare_where_obj },
    #endif
    // functions of the filter sub-module
    #if ULAB_NUMPY_HAS_CONVOLVE
        { MP_OBJ_NEW_QSTR(MP_QSTR_convolve), (mp_obj_t)&filter_convolve_obj },
    #endif
    // functions of the numerical sub-module
    #if ULAB_NUMPY_HAS_ALL
        { MP_OBJ_NEW_QSTR(MP_QSTR_all), (mp_obj_t)&numerical_all_obj },
    #endif
    #if ULAB_NUMPY_HAS_ANY
        { MP_OBJ_NEW_QSTR(MP_QSTR_any), (mp_obj_t)&numerical_any_obj },
    #endif
    #if ULAB_NUMPY_HAS_ARGMINMAX
        { MP_OBJ_NEW_QSTR(MP_QSTR_argmax), (mp_obj_t)&numerical_argmax_obj },
        { MP_OBJ_NEW_QSTR(MP_QSTR_argmin), (mp_obj_t)&numerical_argmin_obj },
    #endif
    #if ULAB_NUMPY_HAS_ARGSORT
        { MP_OBJ_NEW_QSTR(MP_QSTR_argsort), (mp_obj_t)&numerical_argsort_obj },
    #endif
    #if ULAB_NUMPY_HAS_ASARRAY
        { MP_OBJ_NEW_QSTR(MP_QSTR_asarray), (mp_obj_t)&create_asarray_obj },
    #endif
    #if ULAB_NUMPY_HAS_CROSS
        { MP_OBJ_NEW_QSTR(MP_QSTR_cross), (mp_obj_t)&numerical_cross_obj },
    #endif
    #if ULAB_NUMPY_HAS_DIFF
        { MP_OBJ_NEW_QSTR(MP_QSTR_diff), (mp_obj_t)&numerical_diff_obj },
    #endif
    #if ULAB_NUMPY_HAS_DOT
        #if ULAB_MAX_DIMS > 1
            { MP_OBJ_NEW_QSTR(MP_QSTR_dot), (mp_obj_t)&transform_dot_obj },
        #endif
    #endif
    #if ULAB_NUMPY_HAS_TRACE
        #if ULAB_MAX_DIMS > 1
            { MP_ROM_QSTR(MP_QSTR_trace), (mp_obj_t)&stats_trace_obj },
        #endif
    #endif
    #if ULAB_NUMPY_HAS_FLIP
        { MP_OBJ_NEW_QSTR(MP_QSTR_flip), (mp_obj_t)&numerical_flip_obj },
    #endif
    #if ULAB_NUMPY_HAS_LOAD
        { MP_OBJ_NEW_QSTR(MP_QSTR_load), (mp_obj_t)&io_load_obj },
    #endif
    #if ULAB_NUMPY_HAS_LOADTXT
        { MP_OBJ_NEW_QSTR(MP_QSTR_loadtxt), (mp_obj_t)&io_loadtxt_obj },
    #endif
    #if ULAB_NUMPY_HAS_MINMAX
        { MP_OBJ_NEW_QSTR(MP_QSTR_max), (mp_obj_t)&numerical_max_obj },
    #endif
    #if ULAB_NUMPY_HAS_MEAN
        { MP_OBJ_NEW_QSTR(MP_QSTR_mean), (mp_obj_t)&numerical_mean_obj },
    #endif
    #if ULAB_NUMPY_HAS_MEDIAN
        { MP_OBJ_NEW_QSTR(MP_QSTR_median), (mp_obj_t)&numerical_median_obj },
    #endif
    #if ULAB_NUMPY_HAS_MINMAX
        { MP_OBJ_NEW_QSTR(MP_QSTR_min), (mp_obj_t)&numerical_min_obj },
    #endif
    #if ULAB_NUMPY_HAS_ROLL
        { MP_OBJ_NEW_QSTR(MP_QSTR_roll), (mp_obj_t)&numerical_roll_obj },
    #endif
    #if ULAB_NUMPY_HAS_SAVE
        { MP_OBJ_NEW_QSTR(MP_QSTR_save), (mp_obj_t)&io_save_obj },
    #endif
    #if ULAB_NUMPY_HAS_SAVETXT
        { MP_OBJ_NEW_QSTR(MP_QSTR_savetxt), (mp_obj_t)&io_savetxt_obj },
    #endif
    #if ULAB_NUMPY_HAS_SIZE
        { MP_OBJ_NEW_QSTR(MP_QSTR_size), (mp_obj_t)&transform_size_obj },
    #endif
    #if ULAB_NUMPY_HAS_SORT
        { MP_OBJ_NEW_QSTR(MP_QSTR_sort), (mp_obj_t)&numerical_sort_obj },
    #endif
    #if ULAB_NUMPY_HAS_STD
        { MP_OBJ_NEW_QSTR(MP_QSTR_std), (mp_obj_t)&numerical_std_obj },
    #endif
    #if ULAB_NUMPY_HAS_SUM
        { MP_OBJ_NEW_QSTR(MP_QSTR_sum), (mp_obj_t)&numerical_sum_obj },
    #endif
    // functions of the poly sub-module
    #if ULAB_NUMPY_HAS_POLYFIT
        { MP_OBJ_NEW_QSTR(MP_QSTR_polyfit), (mp_obj_t)&poly_polyfit_obj },
    #endif
    #if ULAB_NUMPY_HAS_POLYVAL
        { MP_OBJ_NEW_QSTR(MP_QSTR_polyval), (mp_obj_t)&poly_polyval_obj },
    #endif
    // functions of the vector sub-module
    #if ULAB_NUMPY_HAS_ACOS
    { MP_OBJ_NEW_QSTR(MP_QSTR_acos), (mp_obj_t)&vector_acos_obj },
    #endif
    #if ULAB_NUMPY_HAS_ACOSH
    { MP_OBJ_NEW_QSTR(MP_QSTR_acosh), (mp_obj_t)&vector_acosh_obj },
    #endif
    #if ULAB_NUMPY_HAS_ARCTAN2
    { MP_OBJ_NEW_QSTR(MP_QSTR_arctan2), (mp_obj_t)&vector_arctan2_obj },
    #endif
    #if ULAB_NUMPY_HAS_AROUND
    { MP_OBJ_NEW_QSTR(MP_QSTR_around), (mp_obj_t)&vector_around_obj },
    #endif
    #if ULAB_NUMPY_HAS_ASIN
    { MP_OBJ_NEW_QSTR(MP_QSTR_asin), (mp_obj_t)&vector_asin_obj },
    #endif
    #if ULAB_NUMPY_HAS_ASINH
    { MP_OBJ_NEW_QSTR(MP_QSTR_asinh), (mp_obj_t)&vector_asinh_obj },
    #endif
    #if ULAB_NUMPY_HAS_ATAN
    { MP_OBJ_NEW_QSTR(MP_QSTR_atan), (mp_obj_t)&vector_atan_obj },
    #endif
    #if ULAB_NUMPY_HAS_ATANH
    { MP_OBJ_NEW_QSTR(MP_QSTR_atanh), (mp_obj_t)&vector_atanh_obj },
    #endif
    #if ULAB_NUMPY_HAS_CEIL
    { MP_OBJ_NEW_QSTR(MP_QSTR_ceil), (mp_obj_t)&vector_ceil_obj },
    #endif
    #if ULAB_NUMPY_HAS_COS
    { MP_OBJ_NEW_QSTR(MP_QSTR_cos), (mp_obj_t)&vector_cos_obj },
    #endif
    #if ULAB_NUMPY_HAS_COSH
    { MP_OBJ_NEW_QSTR(MP_QSTR_cosh), (mp_obj_t)&vector_cosh_obj },
    #endif
    #if ULAB_NUMPY_HAS_DEGREES
    { MP_OBJ_NEW_QSTR(MP_QSTR_degrees), (mp_obj_t)&vector_degrees_obj },
    #endif
    #if ULAB_NUMPY_HAS_EXP
    { MP_OBJ_NEW_QSTR(MP_QSTR_exp), (mp_obj_t)&vector_exp_obj },
    #endif
    #if ULAB_NUMPY_HAS_EXPM1
    { MP_OBJ_NEW_QSTR(MP_QSTR_expm1), (mp_obj_t)&vector_expm1_obj },
    #endif
    #if ULAB_NUMPY_HAS_FLOOR
    { MP_OBJ_NEW_QSTR(MP_QSTR_floor), (mp_obj_t)&vector_floor_obj },
    #endif
    #if ULAB_NUMPY_HAS_LOG
    { MP_OBJ_NEW_QSTR(MP_QSTR_log), (mp_obj_t)&vector_log_obj },
    #endif
    #if ULAB_NUMPY_HAS_LOG10
    { MP_OBJ_NEW_QSTR(MP_QSTR_log10), (mp_obj_t)&vector_log10_obj },
    #endif
    #if ULAB_NUMPY_HAS_LOG2
    { MP_OBJ_NEW_QSTR(MP_QSTR_log2), (mp_obj_t)&vector_log2_obj },
    #endif
    #if ULAB_NUMPY_HAS_RADIANS
    { MP_OBJ_NEW_QSTR(MP_QSTR_radians), (mp_obj_t)&vector_radians_obj },
    #endif
    #if ULAB_NUMPY_HAS_SIN
    { MP_OBJ_NEW_QSTR(MP_QSTR_sin), (mp_obj_t)&vector_sin_obj },
    #endif
    #if ULAB_NUMPY_HAS_SINH
    { MP_OBJ_NEW_QSTR(MP_QSTR_sinh), (mp_obj_t)&vector_sinh_obj },
    #endif
    #if ULAB_NUMPY_HAS_SQRT
    { MP_OBJ_NEW_QSTR(MP_QSTR_sqrt), (mp_obj_t)&vector_sqrt_obj },
    #endif
    #if ULAB_NUMPY_HAS_TAN
    { MP_OBJ_NEW_QSTR(MP_QSTR_tan), (mp_obj_t)&vector_tan_obj },
    #endif
    #if ULAB_NUMPY_HAS_TANH
    { MP_OBJ_NEW_QSTR(MP_QSTR_tanh), (mp_obj_t)&vector_tanh_obj },
    #endif
    #if ULAB_NUMPY_HAS_VECTORIZE
    { MP_OBJ_NEW_QSTR(MP_QSTR_vectorize), (mp_obj_t)&vector_vectorize_obj },
    #endif
    #if ULAB_SUPPORTS_COMPLEX
        #if ULAB_NUMPY_HAS_REAL
        { MP_OBJ_NEW_QSTR(MP_QSTR_real), (mp_obj_t)&carray_real_obj },
        #endif
        #if ULAB_NUMPY_HAS_IMAG
        { MP_OBJ_NEW_QSTR(MP_QSTR_imag), (mp_obj_t)&carray_imag_obj },
        #endif
        #if ULAB_NUMPY_HAS_CONJUGATE
            { MP_ROM_QSTR(MP_QSTR_conjugate), (mp_obj_t)&carray_conjugate_obj },
        #endif
        #if ULAB_NUMPY_HAS_SORT_COMPLEX
            { MP_ROM_QSTR(MP_QSTR_sort_complex), (mp_obj_t)&carray_sort_complex_obj },
        #endif
    #endif
};

static MP_DEFINE_CONST_DICT(mp_module_ulab_numpy_globals, ulab_numpy_globals_table);

mp_obj_module_t ulab_numpy_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_numpy_globals,
};
