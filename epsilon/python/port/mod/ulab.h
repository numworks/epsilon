
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2022 Zoltán Vörös
*/

#ifndef __ULAB__
#define __ULAB__



// The pre-processor constants in this file determine how ulab behaves:
//
// - how many dimensions ulab can handle
// - which functions are included in the compiled firmware
// - whether arrays can be sliced and iterated over
// - which binary/unary operators are supported
// - whether ulab can deal with complex numbers
//
// A considerable amount of flash space can be saved by removing (setting
// the corresponding constants to 0) the unnecessary functions and features.

// Values defined here can be overridden by your own config file as
// make -DULAB_CONFIG_FILE="my_ulab_config.h"
#if defined(ULAB_CONFIG_FILE)
#include ULAB_CONFIG_FILE
#endif

// Adds support for complex ndarrays
#ifndef ULAB_SUPPORTS_COMPLEX
#define ULAB_SUPPORTS_COMPLEX               (1)
#endif

// Determines, whether scipy is defined in ulab. The sub-modules and functions
// of scipy have to be defined separately
#ifndef ULAB_HAS_SCIPY
#define ULAB_HAS_SCIPY                      (1)
#endif

// The maximum number of dimensions the firmware should be able to support
// Possible values lie between 1, and 4, inclusive
#ifndef ULAB_MAX_DIMS
#define ULAB_MAX_DIMS                       2
#endif

// By setting this constant to 1, iteration over array dimensions will be implemented
// as a function (ndarray_rewind_array), instead of writing out the loops in macros
// This reduces firmware size at the expense of speed
#ifndef ULAB_HAS_FUNCTION_ITERATOR
#define ULAB_HAS_FUNCTION_ITERATOR          (0)
#endif

// If NDARRAY_IS_ITERABLE is 1, the ndarray object defines its own iterator function
// This option saves approx. 250 bytes of flash space
#ifndef NDARRAY_IS_ITERABLE
#define NDARRAY_IS_ITERABLE                 (1)
#endif

// Slicing can be switched off by setting this variable to 0
#ifndef NDARRAY_IS_SLICEABLE
#define NDARRAY_IS_SLICEABLE                (1)
#endif

// The default threshold for pretty printing. These variables can be overwritten
// at run-time via the set_printoptions() function
#ifndef ULAB_HAS_PRINTOPTIONS
#define ULAB_HAS_PRINTOPTIONS               (1)
#endif
#define NDARRAY_PRINT_THRESHOLD             10
#define NDARRAY_PRINT_EDGEITEMS             3

// determines, whether the dtype is an object, or simply a character
// the object implementation is numpythonic, but requires more space
#ifndef ULAB_HAS_DTYPE_OBJECT
#define ULAB_HAS_DTYPE_OBJECT               (0)
#endif

// the ndarray binary operators
#ifndef NDARRAY_HAS_BINARY_OPS
#define NDARRAY_HAS_BINARY_OPS              (1)
#endif

// Firmware size can be reduced at the expense of speed by using function
// pointers in iterations. For each operator, he function pointer saves around
// 2 kB in the two-dimensional case, and around 4 kB in the four-dimensional case.

#ifndef NDARRAY_BINARY_USES_FUN_POINTER
#define NDARRAY_BINARY_USES_FUN_POINTER     (0)
#endif

#ifndef NDARRAY_HAS_BINARY_OP_ADD
#define NDARRAY_HAS_BINARY_OP_ADD           (1)
#endif

#ifndef NDARRAY_HAS_BINARY_OP_EQUAL
#define NDARRAY_HAS_BINARY_OP_EQUAL         (1)
#endif

#ifndef NDARRAY_HAS_BINARY_OP_LESS
#define NDARRAY_HAS_BINARY_OP_LESS          (1)
#endif

#ifndef NDARRAY_HAS_BINARY_OP_LESS_EQUAL
#define NDARRAY_HAS_BINARY_OP_LESS_EQUAL    (1)
#endif

#ifndef NDARRAY_HAS_BINARY_OP_MORE
#define NDARRAY_HAS_BINARY_OP_MORE          (1)
#endif

#ifndef NDARRAY_HAS_BINARY_OP_MORE_EQUAL
#define NDARRAY_HAS_BINARY_OP_MORE_EQUAL    (1)
#endif

#ifndef NDARRAY_HAS_BINARY_OP_MULTIPLY
#define NDARRAY_HAS_BINARY_OP_MULTIPLY      (1)
#endif

#ifndef NDARRAY_HAS_BINARY_OP_NOT_EQUAL
#define NDARRAY_HAS_BINARY_OP_NOT_EQUAL     (1)
#endif

#ifndef NDARRAY_HAS_BINARY_OP_POWER
#define NDARRAY_HAS_BINARY_OP_POWER         (1)
#endif

#ifndef NDARRAY_HAS_BINARY_OP_SUBTRACT
#define NDARRAY_HAS_BINARY_OP_SUBTRACT      (1)
#endif

#ifndef NDARRAY_HAS_BINARY_OP_TRUE_DIVIDE
#define NDARRAY_HAS_BINARY_OP_TRUE_DIVIDE   (1)
#endif

#ifndef NDARRAY_HAS_INPLACE_OPS
#define NDARRAY_HAS_INPLACE_OPS             (1)
#endif

#ifndef NDARRAY_HAS_INPLACE_ADD
#define NDARRAY_HAS_INPLACE_ADD             (1)
#endif

#ifndef NDARRAY_HAS_INPLACE_MULTIPLY
#define NDARRAY_HAS_INPLACE_MULTIPLY        (1)
#endif

#ifndef NDARRAY_HAS_INPLACE_POWER
#define NDARRAY_HAS_INPLACE_POWER           (1)
#endif

#ifndef NDARRAY_HAS_INPLACE_SUBTRACT
#define NDARRAY_HAS_INPLACE_SUBTRACT        (1)
#endif

#ifndef NDARRAY_HAS_INPLACE_TRUE_DIVIDE
#define NDARRAY_HAS_INPLACE_TRUE_DIVIDE     (1)
#endif

// the ndarray unary operators
#ifndef NDARRAY_HAS_UNARY_OPS
#define NDARRAY_HAS_UNARY_OPS               (1)
#endif

#ifndef NDARRAY_HAS_UNARY_OP_ABS
#define NDARRAY_HAS_UNARY_OP_ABS            (1)
#endif

#ifndef NDARRAY_HAS_UNARY_OP_INVERT
#define NDARRAY_HAS_UNARY_OP_INVERT         (1)
#endif

#ifndef NDARRAY_HAS_UNARY_OP_LEN
#define NDARRAY_HAS_UNARY_OP_LEN            (1)
#endif

#ifndef NDARRAY_HAS_UNARY_OP_NEGATIVE
#define NDARRAY_HAS_UNARY_OP_NEGATIVE       (1)
#endif

#ifndef NDARRAY_HAS_UNARY_OP_POSITIVE
#define NDARRAY_HAS_UNARY_OP_POSITIVE       (1)
#endif


// determines, which ndarray methods are available
#ifndef NDARRAY_HAS_BYTESWAP
#define NDARRAY_HAS_BYTESWAP            (1)
#endif

#ifndef NDARRAY_HAS_COPY
#define NDARRAY_HAS_COPY                (1)
#endif

#ifndef NDARRAY_HAS_DTYPE
#define NDARRAY_HAS_DTYPE               (1)
#endif

#ifndef NDARRAY_HAS_FLATTEN
#define NDARRAY_HAS_FLATTEN             (1)
#endif

#ifndef NDARRAY_HAS_ITEMSIZE
#define NDARRAY_HAS_ITEMSIZE            (1)
#endif

#ifndef NDARRAY_HAS_RESHAPE
#define NDARRAY_HAS_RESHAPE             (1)
#endif

#ifndef NDARRAY_HAS_SHAPE
#define NDARRAY_HAS_SHAPE               (1)
#endif

#ifndef NDARRAY_HAS_SIZE
#define NDARRAY_HAS_SIZE                (1)
#endif

#ifndef NDARRAY_HAS_SORT
#define NDARRAY_HAS_SORT                (1)
#endif

#ifndef NDARRAY_HAS_STRIDES
#define NDARRAY_HAS_STRIDES             (1)
#endif

#ifndef NDARRAY_HAS_TOBYTES
#define NDARRAY_HAS_TOBYTES             (1)
#endif

#ifndef NDARRAY_HAS_TOLIST
#define NDARRAY_HAS_TOLIST              (1)
#endif

#ifndef NDARRAY_HAS_TRANSPOSE
#define NDARRAY_HAS_TRANSPOSE           (1)
#endif

// Firmware size can be reduced at the expense of speed by using a function
// pointer in iterations. Setting ULAB_VECTORISE_USES_FUNCPOINTER to 1 saves
// around 800 bytes in the four-dimensional case, and around 200 in two dimensions.
#ifndef ULAB_VECTORISE_USES_FUN_POINTER
#define ULAB_VECTORISE_USES_FUN_POINTER (1)
#endif

// determines, whether e is defined in ulab.numpy itself
#ifndef ULAB_NUMPY_HAS_E
#define ULAB_NUMPY_HAS_E                (1)
#endif

// ulab defines infinite as a class constant in ulab.numpy
#ifndef ULAB_NUMPY_HAS_INF
#define ULAB_NUMPY_HAS_INF              (1)
#endif

// ulab defines NaN as a class constant in ulab.numpy
#ifndef ULAB_NUMPY_HAS_NAN
#define ULAB_NUMPY_HAS_NAN              (1)
#endif

// determines, whether pi is defined in ulab.numpy itself
#ifndef ULAB_NUMPY_HAS_PI
#define ULAB_NUMPY_HAS_PI               (1)
#endif

// determines, whether the ndinfo function is available
#ifndef ULAB_NUMPY_HAS_NDINFO
#define ULAB_NUMPY_HAS_NDINFO           (1)
#endif

// if this constant is set to 1, the interpreter can iterate
// over the flat array without copying any data
#ifndef NDARRAY_HAS_FLATITER
#define NDARRAY_HAS_FLATITER            (1)
#endif

// frombuffer adds 600 bytes to the firmware
#ifndef ULAB_NUMPY_HAS_FROMBUFFER
#define ULAB_NUMPY_HAS_FROMBUFFER       (1)
#endif

// functions that create an array
#ifndef ULAB_NUMPY_HAS_ARANGE
#define ULAB_NUMPY_HAS_ARANGE           (1)
#endif

#ifndef ULAB_NUMPY_HAS_CONCATENATE
#define ULAB_NUMPY_HAS_CONCATENATE      (1)
#endif

#ifndef ULAB_NUMPY_HAS_DIAG
#define ULAB_NUMPY_HAS_DIAG             (1)
#endif

#ifndef ULAB_NUMPY_HAS_EMPTY
#define ULAB_NUMPY_HAS_EMPTY            (1)
#endif

#ifndef ULAB_NUMPY_HAS_EYE
#define ULAB_NUMPY_HAS_EYE              (1)
#endif

#ifndef ULAB_NUMPY_HAS_FULL
#define ULAB_NUMPY_HAS_FULL             (1)
#endif

#ifndef ULAB_NUMPY_HAS_LINSPACE
#define ULAB_NUMPY_HAS_LINSPACE         (1)
#endif

#ifndef ULAB_NUMPY_HAS_LOGSPACE
#define ULAB_NUMPY_HAS_LOGSPACE         (1)
#endif

#ifndef ULAB_NUMPY_HAS_ONES
#define ULAB_NUMPY_HAS_ONES             (1)
#endif

#ifndef ULAB_NUMPY_HAS_ZEROS
#define ULAB_NUMPY_HAS_ZEROS            (1)
#endif

// functions that compare arrays
#ifndef ULAB_NUMPY_HAS_CLIP
#define ULAB_NUMPY_HAS_CLIP             (1)
#endif

#ifndef ULAB_NUMPY_HAS_EQUAL
#define ULAB_NUMPY_HAS_EQUAL            (1)
#endif

#ifndef ULAB_NUMPY_HAS_ISFINITE
#define ULAB_NUMPY_HAS_ISFINITE         (1)
#endif

#ifndef ULAB_NUMPY_HAS_ISINF
#define ULAB_NUMPY_HAS_ISINF            (1)
#endif

#ifndef ULAB_NUMPY_HAS_MAXIMUM
#define ULAB_NUMPY_HAS_MAXIMUM          (1)
#endif

#ifndef ULAB_NUMPY_HAS_MINIMUM
#define ULAB_NUMPY_HAS_MINIMUM          (1)
#endif

#ifndef ULAB_NUMPY_HAS_NONZERO
#define ULAB_NUMPY_HAS_NONZERO          (1)
#endif

#ifndef ULAB_NUMPY_HAS_NOTEQUAL
#define ULAB_NUMPY_HAS_NOTEQUAL         (1)
#endif

#ifndef ULAB_NUMPY_HAS_WHERE
#define ULAB_NUMPY_HAS_WHERE            (1)
#endif

// the linalg module; functions of the linalg module still have
// to be defined separately
#ifndef ULAB_NUMPY_HAS_LINALG_MODULE
#define ULAB_NUMPY_HAS_LINALG_MODULE    (1)
#endif

#ifndef ULAB_LINALG_HAS_CHOLESKY
#define ULAB_LINALG_HAS_CHOLESKY        (1)
#endif

#ifndef ULAB_LINALG_HAS_DET
#define ULAB_LINALG_HAS_DET             (1)
#endif

#ifndef ULAB_LINALG_HAS_EIG
#define ULAB_LINALG_HAS_EIG             (1)
#endif

#ifndef ULAB_LINALG_HAS_INV
#define ULAB_LINALG_HAS_INV             (1)
#endif

#ifndef ULAB_LINALG_HAS_NORM
#define ULAB_LINALG_HAS_NORM            (1)
#endif

#ifndef ULAB_LINALG_HAS_QR
#define ULAB_LINALG_HAS_QR              (1)
#endif

// the FFT module; functions of the fft module still have
// to be defined separately
#ifndef ULAB_NUMPY_HAS_FFT_MODULE
#define ULAB_NUMPY_HAS_FFT_MODULE       (1)
#endif

// By setting this constant to 1, the FFT routine will behave in a
// numpy-compatible way, i.e., it will output a complex array
// This setting has no effect, if ULAB_SUPPORTS_COMPLEX is 0
// Note that in this case, the input also must be numpythonic,
// i.e., the real an imaginary parts cannot be passed as two arguments
#ifndef ULAB_FFT_IS_NUMPY_COMPATIBLE
#define ULAB_FFT_IS_NUMPY_COMPATIBLE    (0)
#endif

#ifndef ULAB_FFT_HAS_FFT
#define ULAB_FFT_HAS_FFT                (1)
#endif

#ifndef ULAB_FFT_HAS_IFFT
#define ULAB_FFT_HAS_IFFT               (1)
#endif

#ifndef ULAB_NUMPY_HAS_ALL
#define ULAB_NUMPY_HAS_ALL              (1)
#endif

#ifndef ULAB_NUMPY_HAS_ANY
#define ULAB_NUMPY_HAS_ANY              (1)
#endif

#ifndef ULAB_NUMPY_HAS_ARGMINMAX
#define ULAB_NUMPY_HAS_ARGMINMAX        (1)
#endif

#ifndef ULAB_NUMPY_HAS_ARGSORT
#define ULAB_NUMPY_HAS_ARGSORT          (1)
#endif

#ifndef ULAB_NUMPY_HAS_ASARRAY
#define ULAB_NUMPY_HAS_ASARRAY          (1)
#endif

#ifndef ULAB_NUMPY_HAS_COMPRESS
#define ULAB_NUMPY_HAS_COMPRESS         (1)
#endif

#ifndef ULAB_NUMPY_HAS_CONVOLVE
#define ULAB_NUMPY_HAS_CONVOLVE         (1)
#endif

#ifndef ULAB_NUMPY_HAS_CROSS
#define ULAB_NUMPY_HAS_CROSS            (1)
#endif

#ifndef ULAB_NUMPY_HAS_DELETE
#define ULAB_NUMPY_HAS_DELETE           (1)
#endif

#ifndef ULAB_NUMPY_HAS_DIFF
#define ULAB_NUMPY_HAS_DIFF             (1)
#endif

#ifndef ULAB_NUMPY_HAS_DOT
#define ULAB_NUMPY_HAS_DOT              (1)
#endif

#ifndef ULAB_NUMPY_HAS_FLIP
#define ULAB_NUMPY_HAS_FLIP             (1)
#endif

#ifndef ULAB_NUMPY_HAS_INTERP
#define ULAB_NUMPY_HAS_INTERP           (1)
#endif

#ifndef ULAB_NUMPY_HAS_LOAD
#define ULAB_NUMPY_HAS_LOAD             (1)
#endif

#ifndef ULAB_NUMPY_HAS_LOADTXT
#define ULAB_NUMPY_HAS_LOADTXT          (1)
#endif

#ifndef ULAB_NUMPY_HAS_MEAN
#define ULAB_NUMPY_HAS_MEAN             (1)
#endif

#ifndef ULAB_NUMPY_HAS_MEDIAN
#define ULAB_NUMPY_HAS_MEDIAN           (1)
#endif

#ifndef ULAB_NUMPY_HAS_MINMAX
#define ULAB_NUMPY_HAS_MINMAX           (1)
#endif

#ifndef ULAB_NUMPY_HAS_POLYFIT
#define ULAB_NUMPY_HAS_POLYFIT          (1)
#endif

#ifndef ULAB_NUMPY_HAS_POLYVAL
#define ULAB_NUMPY_HAS_POLYVAL          (1)
#endif

#ifndef ULAB_NUMPY_HAS_ROLL
#define ULAB_NUMPY_HAS_ROLL             (1)
#endif

#ifndef ULAB_NUMPY_HAS_SAVE
#define ULAB_NUMPY_HAS_SAVE             (1)
#endif

#ifndef ULAB_NUMPY_HAS_SAVETXT
#define ULAB_NUMPY_HAS_SAVETXT          (1)
#endif

#ifndef ULAB_NUMPY_HAS_SIZE
#define ULAB_NUMPY_HAS_SIZE             (1)
#endif

#ifndef ULAB_NUMPY_HAS_SORT
#define ULAB_NUMPY_HAS_SORT             (1)
#endif

#ifndef ULAB_NUMPY_HAS_STD
#define ULAB_NUMPY_HAS_STD              (1)
#endif

#ifndef ULAB_NUMPY_HAS_SUM
#define ULAB_NUMPY_HAS_SUM              (1)
#endif

#ifndef ULAB_NUMPY_HAS_TRACE
#define ULAB_NUMPY_HAS_TRACE            (1)
#endif

#ifndef ULAB_NUMPY_HAS_TRAPZ
#define ULAB_NUMPY_HAS_TRAPZ            (1)
#endif

// vectorised versions of the functions of the math python module, with
// the exception of the functions listed in scipy.special
#ifndef ULAB_NUMPY_HAS_ACOS
#define ULAB_NUMPY_HAS_ACOS             (1)
#endif

#ifndef ULAB_NUMPY_HAS_ACOSH
#define ULAB_NUMPY_HAS_ACOSH            (1)
#endif

#ifndef ULAB_NUMPY_HAS_ARCTAN2
#define ULAB_NUMPY_HAS_ARCTAN2          (1)
#endif

#ifndef ULAB_NUMPY_HAS_AROUND
#define ULAB_NUMPY_HAS_AROUND           (1)
#endif

#ifndef ULAB_NUMPY_HAS_ASIN
#define ULAB_NUMPY_HAS_ASIN             (1)
#endif

#ifndef ULAB_NUMPY_HAS_ASINH
#define ULAB_NUMPY_HAS_ASINH            (1)
#endif

#ifndef ULAB_NUMPY_HAS_ATAN
#define ULAB_NUMPY_HAS_ATAN             (1)
#endif

#ifndef ULAB_NUMPY_HAS_ATANH
#define ULAB_NUMPY_HAS_ATANH            (1)
#endif

#ifndef ULAB_NUMPY_HAS_CEIL
#define ULAB_NUMPY_HAS_CEIL             (1)
#endif

#ifndef ULAB_NUMPY_HAS_COS
#define ULAB_NUMPY_HAS_COS              (1)
#endif

#ifndef ULAB_NUMPY_HAS_COSH
#define ULAB_NUMPY_HAS_COSH             (1)
#endif

#ifndef ULAB_NUMPY_HAS_DEGREES
#define ULAB_NUMPY_HAS_DEGREES          (1)
#endif

#ifndef ULAB_NUMPY_HAS_EXP
#define ULAB_NUMPY_HAS_EXP              (1)
#endif

#ifndef ULAB_NUMPY_HAS_EXPM1
#define ULAB_NUMPY_HAS_EXPM1            (1)
#endif

#ifndef ULAB_NUMPY_HAS_FLOOR
#define ULAB_NUMPY_HAS_FLOOR            (1)
#endif

#ifndef ULAB_NUMPY_HAS_LOG
#define ULAB_NUMPY_HAS_LOG              (1)
#endif

#ifndef ULAB_NUMPY_HAS_LOG10
#define ULAB_NUMPY_HAS_LOG10            (1)
#endif

#ifndef ULAB_NUMPY_HAS_LOG2
#define ULAB_NUMPY_HAS_LOG2             (1)
#endif

#ifndef ULAB_NUMPY_HAS_RADIANS
#define ULAB_NUMPY_HAS_RADIANS          (1)
#endif

#ifndef ULAB_NUMPY_HAS_SIN
#define ULAB_NUMPY_HAS_SIN              (1)
#endif

#ifndef ULAB_NUMPY_HAS_SINH
#define ULAB_NUMPY_HAS_SINH             (1)
#endif

#ifndef ULAB_NUMPY_HAS_SQRT
#define ULAB_NUMPY_HAS_SQRT             (1)
#endif

#ifndef ULAB_NUMPY_HAS_TAN
#define ULAB_NUMPY_HAS_TAN              (1)
#endif

#ifndef ULAB_NUMPY_HAS_TANH
#define ULAB_NUMPY_HAS_TANH             (1)
#endif

#ifndef ULAB_NUMPY_HAS_VECTORIZE
#define ULAB_NUMPY_HAS_VECTORIZE        (1)
#endif

// Complex functions. The implementations are compiled into
// the firmware, only if ULAB_SUPPORTS_COMPLEX is set to 1
#ifndef ULAB_NUMPY_HAS_CONJUGATE
#define ULAB_NUMPY_HAS_CONJUGATE        (1)
#endif

#ifndef ULAB_NUMPY_HAS_IMAG
#define ULAB_NUMPY_HAS_IMAG             (1)
#endif

#ifndef ULAB_NUMPY_HAS_REAL
#define ULAB_NUMPY_HAS_REAL             (1)
#endif

#ifndef ULAB_NUMPY_HAS_SORT_COMPLEX
#define ULAB_NUMPY_HAS_SORT_COMPLEX     (1)
#endif

// scipy modules
#ifndef ULAB_SCIPY_HAS_LINALG_MODULE
#define ULAB_SCIPY_HAS_LINALG_MODULE        (1)
#endif

#ifndef ULAB_SCIPY_LINALG_HAS_CHO_SOLVE
#define ULAB_SCIPY_LINALG_HAS_CHO_SOLVE     (1)
#endif

#ifndef ULAB_SCIPY_LINALG_HAS_SOLVE_TRIANGULAR
#define ULAB_SCIPY_LINALG_HAS_SOLVE_TRIANGULAR  (1)
#endif

#ifndef ULAB_SCIPY_HAS_SIGNAL_MODULE
#define ULAB_SCIPY_HAS_SIGNAL_MODULE        (1)
#endif

#ifndef ULAB_SCIPY_SIGNAL_HAS_SOSFILT
#define ULAB_SCIPY_SIGNAL_HAS_SOSFILT       (1)
#endif

#ifndef ULAB_SCIPY_HAS_OPTIMIZE_MODULE
#define ULAB_SCIPY_HAS_OPTIMIZE_MODULE      (1)
#endif

#ifndef ULAB_SCIPY_OPTIMIZE_HAS_BISECT
#define ULAB_SCIPY_OPTIMIZE_HAS_BISECT      (1)
#endif

#ifndef ULAB_SCIPY_OPTIMIZE_HAS_CURVE_FIT
#define ULAB_SCIPY_OPTIMIZE_HAS_CURVE_FIT   (0) // not fully implemented
#endif

#ifndef ULAB_SCIPY_OPTIMIZE_HAS_FMIN
#define ULAB_SCIPY_OPTIMIZE_HAS_FMIN        (1)
#endif

#ifndef ULAB_SCIPY_OPTIMIZE_HAS_NEWTON
#define ULAB_SCIPY_OPTIMIZE_HAS_NEWTON      (1)
#endif

#ifndef ULAB_SCIPY_HAS_SPECIAL_MODULE
#define ULAB_SCIPY_HAS_SPECIAL_MODULE       (1)
#endif

#ifndef ULAB_SCIPY_SPECIAL_HAS_ERF
#define ULAB_SCIPY_SPECIAL_HAS_ERF          (1)
#endif

#ifndef ULAB_SCIPY_SPECIAL_HAS_ERFC
#define ULAB_SCIPY_SPECIAL_HAS_ERFC         (1)
#endif

#ifndef ULAB_SCIPY_SPECIAL_HAS_GAMMA
#define ULAB_SCIPY_SPECIAL_HAS_GAMMA        (1)
#endif

#ifndef ULAB_SCIPY_SPECIAL_HAS_GAMMALN
#define ULAB_SCIPY_SPECIAL_HAS_GAMMALN      (1)
#endif

// functions of the utils module
#ifndef ULAB_HAS_UTILS_MODULE
#define ULAB_HAS_UTILS_MODULE               (1)
#endif

#ifndef ULAB_UTILS_HAS_FROM_INT16_BUFFER
#define ULAB_UTILS_HAS_FROM_INT16_BUFFER    (1)
#endif

#ifndef ULAB_UTILS_HAS_FROM_UINT16_BUFFER
#define ULAB_UTILS_HAS_FROM_UINT16_BUFFER   (1)
#endif

#ifndef ULAB_UTILS_HAS_FROM_INT32_BUFFER
#define ULAB_UTILS_HAS_FROM_INT32_BUFFER    (1)
#endif

#ifndef ULAB_UTILS_HAS_FROM_UINT32_BUFFER
#define ULAB_UTILS_HAS_FROM_UINT32_BUFFER   (1)
#endif

#ifndef ULAB_UTILS_HAS_SPECTROGRAM
#define ULAB_UTILS_HAS_SPECTROGRAM          (1)
#endif

// user-defined module; source of the module and
// its sub-modules should be placed in code/user/
#ifndef ULAB_HAS_USER_MODULE
#define ULAB_HAS_USER_MODULE                (0)
#endif

#endif
