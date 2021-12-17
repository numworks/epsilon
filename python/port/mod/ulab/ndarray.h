
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2021 Zoltán Vörös
 *               2020 Jeff Epler for Adafruit Industries
*/

#ifndef _NDARRAY_
#define _NDARRAY_

#include "py/objarray.h"
#include "py/binary.h"
#include "py/objstr.h"
#include "py/objlist.h"

#include "ulab.h"

#ifndef MP_PI
#define MP_PI MICROPY_FLOAT_CONST(3.14159265358979323846)
#endif
#ifndef MP_E
#define MP_E MICROPY_FLOAT_CONST(2.71828182845904523536)
#endif

#if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
#define FLOAT_TYPECODE 'f'
#elif MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_DOUBLE
#define FLOAT_TYPECODE 'd'
#endif

// this typedef is lifted from objfloat.c, because mp_obj_float_t is not exposed
typedef struct _mp_obj_float_t {
    mp_obj_base_t base;
    mp_float_t value;
} mp_obj_float_t;

#if defined(MICROPY_VERSION_MAJOR) && MICROPY_VERSION_MAJOR == 1 && MICROPY_VERSION_MINOR == 11
typedef struct _mp_obj_slice_t {
    mp_obj_base_t base;
    mp_obj_t start;
    mp_obj_t stop;
    mp_obj_t step;
} mp_obj_slice_t;
#define MP_ERROR_TEXT(x) x
#endif

#if !defined(MP_TYPE_FLAG_EXTENDED)
#define MP_TYPE_CALL call
#define mp_type_get_call_slot(t) t->call
#define MP_TYPE_FLAG_EXTENDED (0)
#define MP_TYPE_EXTENDED_FIELDS(...) __VA_ARGS__
#endif

#if !CIRCUITPY
#define translate(x) MP_ERROR_TEXT(x)
#define ndarray_set_value(a, b, c, d) mp_binary_set_val_array(a, b, c, d)
#else
void ndarray_set_value(char , void *, size_t , mp_obj_t );
#endif

#define NDARRAY_NUMERIC   0
#define NDARRAY_BOOLEAN   1

#define NDARRAY_NDARRAY_TYPE    1
#define NDARRAY_ITERABLE_TYPE   2

extern const mp_obj_type_t ulab_ndarray_type;

enum NDARRAY_TYPE {
    NDARRAY_BOOL = '?', // this must never be assigned to the dtype!
    NDARRAY_UINT8 = 'B',
    NDARRAY_INT8 = 'b',
    NDARRAY_UINT16 = 'H',
    NDARRAY_INT16 = 'h',
    NDARRAY_FLOAT = FLOAT_TYPECODE,
};

typedef struct _ndarray_obj_t {
    mp_obj_base_t base;
    uint8_t dtype;
    uint8_t itemsize;
    uint8_t boolean;
    uint8_t ndim;
    size_t len;
    size_t shape[ULAB_MAX_DIMS];
    int32_t strides[ULAB_MAX_DIMS];
    void *array;
    void *origin;
} ndarray_obj_t;

#if ULAB_HAS_DTYPE_OBJECT
extern const mp_obj_type_t ulab_dtype_type;

typedef struct _dtype_obj_t {
    mp_obj_base_t base;
    uint8_t dtype;
} dtype_obj_t;

void ndarray_dtype_print(const mp_print_t *, mp_obj_t , mp_print_kind_t );

mp_obj_t ndarray_dtype_make_new(const mp_obj_type_t *, size_t , size_t , const mp_obj_t *);
#endif /* ULAB_HAS_DTYPE_OBJECT */

extern const mp_obj_type_t ndarray_flatiter_type;

mp_obj_t ndarray_new_ndarray_iterator(mp_obj_t , mp_obj_iter_buf_t *);

mp_obj_t ndarray_get_item(ndarray_obj_t *, void *);
mp_float_t ndarray_get_float_value(void *, uint8_t );
mp_float_t ndarray_get_float_index(void *, uint8_t , size_t );
bool ndarray_object_is_array_like(mp_obj_t );
void fill_array_iterable(mp_float_t *, mp_obj_t );
size_t *ndarray_shape_vector(size_t , size_t , size_t , size_t );

void ndarray_print(const mp_print_t *, mp_obj_t , mp_print_kind_t );

#if ULAB_HAS_PRINTOPTIONS
mp_obj_t ndarray_set_printoptions(size_t , const mp_obj_t *, mp_map_t *);
MP_DECLARE_CONST_FUN_OBJ_KW(ndarray_set_printoptions_obj);

mp_obj_t ndarray_get_printoptions(void);
MP_DECLARE_CONST_FUN_OBJ_0(ndarray_get_printoptions_obj);
#endif

void ndarray_assign_elements(ndarray_obj_t *, mp_obj_t , uint8_t , size_t *);
size_t *ndarray_contract_shape(ndarray_obj_t *, uint8_t );
int32_t *ndarray_contract_strides(ndarray_obj_t *, uint8_t );

ndarray_obj_t *ndarray_new_dense_ndarray(uint8_t , size_t *, uint8_t );
ndarray_obj_t *ndarray_new_ndarray_from_tuple(mp_obj_tuple_t *, uint8_t );
ndarray_obj_t *ndarray_new_ndarray(uint8_t , size_t *, int32_t *, uint8_t );
ndarray_obj_t *ndarray_new_linear_array(size_t , uint8_t );
ndarray_obj_t *ndarray_new_view(ndarray_obj_t *, uint8_t , size_t *, int32_t *, int32_t );
bool ndarray_is_dense(ndarray_obj_t *);
ndarray_obj_t *ndarray_copy_view(ndarray_obj_t *);
void ndarray_copy_array(ndarray_obj_t *, ndarray_obj_t *);

MP_DECLARE_CONST_FUN_OBJ_KW(ndarray_array_constructor_obj);
mp_obj_t ndarray_make_new(const mp_obj_type_t *, size_t , size_t , const mp_obj_t *);
mp_obj_t ndarray_subscr(mp_obj_t , mp_obj_t , mp_obj_t );
mp_obj_t ndarray_getiter(mp_obj_t , mp_obj_iter_buf_t *);
bool ndarray_can_broadcast(ndarray_obj_t *, ndarray_obj_t *, uint8_t *, size_t *, int32_t *, int32_t *);
bool ndarray_can_broadcast_inplace(ndarray_obj_t *, ndarray_obj_t *, int32_t *);
mp_obj_t ndarray_binary_op(mp_binary_op_t , mp_obj_t , mp_obj_t );
mp_obj_t ndarray_unary_op(mp_unary_op_t , mp_obj_t );

size_t *ndarray_new_coords(uint8_t );
void ndarray_rewind_array(uint8_t , uint8_t *, size_t *, int32_t *, size_t *);

// various ndarray methods
#if NDARRAY_HAS_BYTESWAP
mp_obj_t ndarray_byteswap(size_t , const mp_obj_t *, mp_map_t *);
MP_DECLARE_CONST_FUN_OBJ_KW(ndarray_byteswap_obj);
#endif

#if NDARRAY_HAS_COPY
mp_obj_t ndarray_copy(mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_1(ndarray_copy_obj);
#endif

#if NDARRAY_HAS_FLATTEN
mp_obj_t ndarray_flatten(size_t , const mp_obj_t *, mp_map_t *);
MP_DECLARE_CONST_FUN_OBJ_KW(ndarray_flatten_obj);
#endif

mp_obj_t ndarray_dtype(mp_obj_t );
mp_obj_t ndarray_itemsize(mp_obj_t );
mp_obj_t ndarray_size(mp_obj_t );
mp_obj_t ndarray_shape(mp_obj_t );
mp_obj_t ndarray_strides(mp_obj_t );

#if NDARRAY_HAS_RESHAPE
mp_obj_t ndarray_reshape_core(mp_obj_t , mp_obj_t , bool );
mp_obj_t ndarray_reshape(mp_obj_t , mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_2(ndarray_reshape_obj);
#endif

#if NDARRAY_HAS_TOBYTES
mp_obj_t ndarray_tobytes(mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_1(ndarray_tobytes_obj);
#endif

#if NDARRAY_HAS_TRANSPOSE
mp_obj_t ndarray_transpose(mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_1(ndarray_transpose_obj);
#endif

#if ULAB_NUMPY_HAS_NDINFO
mp_obj_t ndarray_info(mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_1(ndarray_info_obj);
#endif

mp_int_t ndarray_get_buffer(mp_obj_t , mp_buffer_info_t *, mp_uint_t );
//void ndarray_attributes(mp_obj_t , qstr , mp_obj_t *);

ndarray_obj_t *ndarray_from_mp_obj(mp_obj_t , uint8_t );


#define BOOLEAN_ASSIGNMENT_LOOP(type_left, type_right, ndarray, iarray, istride, varray, vstride)\
    type_left *array = (type_left *)(ndarray)->array;\
    for(size_t i=0; i < (ndarray)->len; i++) {\
        if(*(iarray)) {\
            *array = (type_left)(*((type_right *)(varray)));\
        }\
        array += (ndarray)->strides[ULAB_MAX_DIMS - 1] / (ndarray)->itemsize;\
        (iarray) += (istride);\
        (varray) += (vstride);\
    } while(0)

#if ULAB_HAS_FUNCTION_ITERATOR
#define BINARY_LOOP(results, type_out, type_left, type_right, larray, lstrides, rarray, rstrides, OPERATOR)\
    type_out *array = (type_out *)(results)->array;\
    size_t *lcoords = ndarray_new_coords((results)->ndim);\
    size_t *rcoords = ndarray_new_coords((results)->ndim);\
    for(size_t i=0; i < (results)->len/(results)->shape[ULAB_MAX_DIMS -1]; i++) {\
        size_t l = 0;\
        do {\
            *array++ = *((type_left *)(larray)) OPERATOR *((type_right *)(rarray));\
            (larray) += (lstrides)[ULAB_MAX_DIMS - 1];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
            l++;\
        } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\
        ndarray_rewind_array((results)->ndim, (larray), (results)->shape, (lstrides), lcoords);\
        ndarray_rewind_array((results)->ndim, (rarray), (results)->shape, (rstrides), rcoords);\
    } while(0)

#define INPLACE_LOOP(results, type_left, type_right, larray, rarray, rstrides, OPERATOR)\
    size_t *lcoords = ndarray_new_coords((results)->ndim);\
    size_t *rcoords = ndarray_new_coords((results)->ndim);\
    for(size_t i=0; i < (results)->len/(results)->shape[ULAB_MAX_DIMS -1]; i++) {\
        size_t l = 0;\
        do {\
            *((type_left *)(larray)) OPERATOR *((type_right *)(rarray));\
            (larray) += (results)->strides[ULAB_MAX_DIMS - 1];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
            l++;\
        } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\
        ndarray_rewind_array((results)->ndim, (larray), (results)->shape, (results)->strides, lcoords);\
        ndarray_rewind_array((results)->ndim, (rarray), (results)->shape, (rstrides), rcoords);\
    } while(0)

#define EQUALITY_LOOP(results, array, type_left, type_right, larray, lstrides, rarray, rstrides, OPERATOR)\
    size_t *lcoords = ndarray_new_coords((results)->ndim);\
    size_t *rcoords = ndarray_new_coords((results)->ndim);\
    for(size_t i=0; i < (results)->len/(results)->shape[ULAB_MAX_DIMS -1]; i++) {\
        size_t l = 0;\
        do {\
            *(array)++ = *((type_left *)(larray)) OPERATOR *((type_right *)(rarray)) ? 1 : 0;\
            (larray) += (lstrides)[ULAB_MAX_DIMS - 1];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
            l++;\
        } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\
        ndarray_rewind_array((results)->ndim, (larray), (results)->shape, (lstrides), lcoords);\
        ndarray_rewind_array((results)->ndim, (rarray), (results)->shape, (rstrides), rcoords);\
    } while(0)

#define POWER_LOOP(results, type_out, type_left, type_right, larray, lstrides, rarray, rstrides)\
    type_out *array = (type_out *)(results)->array;\
    size_t *lcoords = ndarray_new_coords((results)->ndim);\
    size_t *rcoords = ndarray_new_coords((results)->ndim);\
    for(size_t i=0; i < (results)->len/(results)->shape[ULAB_MAX_DIMS -1]; i++) {\
        size_t l = 0;\
        do {\
            *array++ = MICROPY_FLOAT_C_FUN(pow)(*((type_left *)(larray)), *((type_right *)(rarray)));\
            (larray) += (lstrides)[ULAB_MAX_DIMS - 1];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
            l++;\
        } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\
        ndarray_rewind_array((results)->ndim, (larray), (results)->shape, (lstrides), lcoords);\
        ndarray_rewind_array((results)->ndim, (rarray), (results)->shape, (rstrides), rcoords);\
    } while(0)

#else

#if ULAB_MAX_DIMS == 1
#define BINARY_LOOP(results, type_out, type_left, type_right, larray, lstrides, rarray, rstrides, OPERATOR)\
    type_out *array = (type_out *)results->array;\
    size_t l = 0;\
    do {\
        *array++ = *((type_left *)(larray)) OPERATOR *((type_right *)(rarray));\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 1];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
        l++;\
    } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\

#define INPLACE_LOOP(results, type_left, type_right, larray, rarray, rstrides, OPERATOR)\
    size_t l = 0;\
    do {\
        *((type_left *)(larray)) OPERATOR *((type_right *)(rarray));\
        (larray) += (results)->strides[ULAB_MAX_DIMS - 1];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
        l++;\
    } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\

#define EQUALITY_LOOP(results, array, type_left, type_right, larray, lstrides, rarray, rstrides, OPERATOR)\
    size_t l = 0;\
    do {\
        *(array)++ = *((type_left *)(larray)) OPERATOR *((type_right *)(rarray)) ? 1 : 0;\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 1];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
        l++;\
    } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\

#define POWER_LOOP(results, type_out, type_left, type_right, larray, lstrides, rarray, rstrides)\
    type_out *array = (type_out *)results->array;\
    size_t l = 0;\
    do {\
        *array++ = MICROPY_FLOAT_C_FUN(pow)(*((type_left *)(larray)), *((type_right *)(rarray)));\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 1];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
        l++;\
    } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\

#endif /* ULAB_MAX_DIMS == 1 */

#if ULAB_MAX_DIMS == 2
#define BINARY_LOOP(results, type_out, type_left, type_right, larray, lstrides, rarray, rstrides, OPERATOR)\
    type_out *array = (type_out *)(results)->array;\
    size_t k = 0;\
    do {\
        size_t l = 0;\
        do {\
            *array++ = *((type_left *)(larray)) OPERATOR *((type_right *)(rarray));\
            (larray) += (lstrides)[ULAB_MAX_DIMS - 1];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
            l++;\
        } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\
        (larray) -= (lstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 2];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
        k++;\
    } while(k < (results)->shape[ULAB_MAX_DIMS - 2]);\

#define INPLACE_LOOP(results, type_left, type_right, larray, rarray, rstrides, OPERATOR)\
    size_t k = 0;\
    do {\
        size_t l = 0;\
        do {\
            *((type_left *)(larray)) OPERATOR *((type_right *)(rarray));\
            (larray) += (results)->strides[ULAB_MAX_DIMS - 1];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
            l++;\
        } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\
        (larray) -= (results)->strides[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
        (larray) += (results)->strides[ULAB_MAX_DIMS - 2];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
        k++;\
    } while(k < (results)->shape[ULAB_MAX_DIMS - 2]);\

#define EQUALITY_LOOP(results, array, type_left, type_right, larray, lstrides, rarray, rstrides, OPERATOR)\
    size_t k = 0;\
    do {\
        size_t l = 0;\
        do {\
            *(array)++ = *((type_left *)(larray)) OPERATOR *((type_right *)(rarray)) ? 1 : 0;\
            (larray) += (lstrides)[ULAB_MAX_DIMS - 1];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
            l++;\
        } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\
        (larray) -= (lstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 2];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
        k++;\
    } while(k < (results)->shape[ULAB_MAX_DIMS - 2]);\

#define POWER_LOOP(results, type_out, type_left, type_right, larray, lstrides, rarray, rstrides)\
    type_out *array = (type_out *)(results)->array;\
    size_t k = 0;\
    do {\
        size_t l = 0;\
        do {\
            *array++ = MICROPY_FLOAT_C_FUN(pow)(*((type_left *)(larray)), *((type_right *)(rarray)));\
            (larray) += (lstrides)[ULAB_MAX_DIMS - 1];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
            l++;\
        } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\
        (larray) -= (lstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 2];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
        k++;\
    } while(k < (results)->shape[ULAB_MAX_DIMS - 2]);\

#endif /* ULAB_MAX_DIMS == 2 */

#if ULAB_MAX_DIMS == 3
#define BINARY_LOOP(results, type_out, type_left, type_right, larray, lstrides, rarray, rstrides, OPERATOR)\
    type_out *array = (type_out *)results->array;\
    size_t j = 0;\
    do {\
        size_t k = 0;\
        do {\
            size_t l = 0;\
            do {\
                *array++ = *((type_left *)(larray)) OPERATOR *((type_right *)(rarray));\
                (larray) += (lstrides)[ULAB_MAX_DIMS - 1];\
                (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
                l++;\
            } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\
            (larray) -= (lstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
            (larray) += (lstrides)[ULAB_MAX_DIMS - 2];\
            (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
            k++;\
        } while(k < (results)->shape[ULAB_MAX_DIMS - 2]);\
        (larray) -= (lstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS-2];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 3];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS-2];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 3];\
        j++;\
    } while(j < (results)->shape[ULAB_MAX_DIMS - 3]);\

#define INPLACE_LOOP(results, type_left, type_right, larray, rarray, rstrides, OPERATOR)\
    size_t j = 0;\
    do {\
        size_t k = 0;\
        do {\
            size_t l = 0;\
            do {\
                *((type_left *)(larray)) OPERATOR *((type_right *)(rarray));\
                (larray) += (results)->strides[ULAB_MAX_DIMS - 1];\
                (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
                l++;\
            } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\
            (larray) -= (results)->strides[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
            (larray) += (results)->strides[ULAB_MAX_DIMS - 2];\
            (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
            k++;\
        } while(k < (results)->shape[ULAB_MAX_DIMS - 2]);\
        (larray) -= (results)->strides[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS-2];\
        (larray) += (results)->strides[ULAB_MAX_DIMS - 3];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS-2];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 3];\
        j++;\
    } while(j < (results)->shape[ULAB_MAX_DIMS - 3]);\

#define EQUALITY_LOOP(results, array, type_left, type_right, larray, lstrides, rarray, rstrides, OPERATOR)\
    size_t j = 0;\
    do {\
        size_t k = 0;\
        do {\
            size_t l = 0;\
            do {\
                *(array)++ = *((type_left *)(larray)) OPERATOR *((type_right *)(rarray)) ? 1 : 0;\
                (larray) += (lstrides)[ULAB_MAX_DIMS - 1];\
                (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
                l++;\
            } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\
            (larray) -= (lstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
            (larray) += (lstrides)[ULAB_MAX_DIMS - 2];\
            (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
            k++;\
        } while(k < (results)->shape[ULAB_MAX_DIMS - 2]);\
        (larray) -= (lstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS-2];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 3];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS-2];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 3];\
        j++;\
    } while(j < (results)->shape[ULAB_MAX_DIMS - 3]);\

#define POWER_LOOP(results, type_out, type_left, type_right, larray, lstrides, rarray, rstrides)\
    type_out *array = (type_out *)results->array;\
    size_t j = 0;\
    do {\
        size_t k = 0;\
        do {\
            size_t l = 0;\
            do {\
                *array++ = MICROPY_FLOAT_C_FUN(pow)(*((type_left *)(larray)), *((type_right *)(rarray)));\
                (larray) += (lstrides)[ULAB_MAX_DIMS - 1];\
                (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
                l++;\
            } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\
            (larray) -= (lstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
            (larray) += (lstrides)[ULAB_MAX_DIMS - 2];\
            (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
            k++;\
        } while(k < (results)->shape[ULAB_MAX_DIMS - 2]);\
        (larray) -= (lstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS-2];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 3];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS-2];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 3];\
        j++;\
    } while(j < (results)->shape[ULAB_MAX_DIMS - 3]);\

#endif /* ULAB_MAX_DIMS == 3 */

#if ULAB_MAX_DIMS == 4
#define BINARY_LOOP(results, type_out, type_left, type_right, larray, lstrides, rarray, rstrides, OPERATOR)\
    type_out *array = (type_out *)results->array;\
    size_t i = 0;\
    do {\
        size_t j = 0;\
        do {\
            size_t k = 0;\
            do {\
                size_t l = 0;\
                do {\
                    *array++ = *((type_left *)(larray)) OPERATOR *((type_right *)(rarray));\
                    (larray) += (lstrides)[ULAB_MAX_DIMS - 1];\
                    (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
                    l++;\
                } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\
                (larray) -= (lstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
                (larray) += (lstrides)[ULAB_MAX_DIMS - 2];\
                (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
                (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
                k++;\
            } while(k < (results)->shape[ULAB_MAX_DIMS - 2]);\
            (larray) -= (lstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS-2];\
            (larray) += (lstrides)[ULAB_MAX_DIMS - 3];\
            (rarray) -= (rstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS-2];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 3];\
            j++;\
        } while(j < (results)->shape[ULAB_MAX_DIMS - 3]);\
        (larray) -= (lstrides)[ULAB_MAX_DIMS - 3] * (results)->shape[ULAB_MAX_DIMS-3];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 4];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 3] * (results)->shape[ULAB_MAX_DIMS-3];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 4];\
        i++;\
    } while(i < (results)->shape[ULAB_MAX_DIMS - 4]);\

#define INPLACE_LOOP(results, type_left, type_right, larray, rarray, rstrides, OPERATOR)\
    size_t i = 0;\
    do {\
        size_t j = 0;\
        do {\
            size_t k = 0;\
            do {\
                size_t l = 0;\
                do {\
                    *((type_left *)(larray)) OPERATOR *((type_right *)(rarray));\
                    (larray) += (results)->strides[ULAB_MAX_DIMS - 1];\
                    (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
                    l++;\
                } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\
                (larray) -= (results)->strides[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
                (larray) += (results)->strides[ULAB_MAX_DIMS - 2];\
                (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
                (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
                k++;\
            } while(k < (results)->shape[ULAB_MAX_DIMS - 2]);\
            (larray) -= (results)->strides[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS-2];\
            (larray) += (results)->strides[ULAB_MAX_DIMS - 3];\
            (rarray) -= (rstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS-2];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 3];\
            j++;\
        } while(j < (results)->shape[ULAB_MAX_DIMS - 3]);\
        (larray) -= (results)->strides[ULAB_MAX_DIMS - 3] * (results)->shape[ULAB_MAX_DIMS-3];\
        (larray) += (results)->strides[ULAB_MAX_DIMS - 4];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 3] * (results)->shape[ULAB_MAX_DIMS-3];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 4];\
        i++;\
    } while(i < (results)->shape[ULAB_MAX_DIMS - 4]);\

#define EQUALITY_LOOP(results, array, type_left, type_right, larray, lstrides, rarray, rstrides, OPERATOR)\
    size_t i = 0;\
    do {\
        size_t j = 0;\
        do {\
            size_t k = 0;\
            do {\
                size_t l = 0;\
                do {\
                    *(array)++ = *((type_left *)(larray)) OPERATOR *((type_right *)(rarray)) ? 1 : 0;\
                    (larray) += (lstrides)[ULAB_MAX_DIMS - 1];\
                    (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
                    l++;\
                } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\
                (larray) -= (lstrides)[ULAB_MAX_DIMS - 1] * results->shape[ULAB_MAX_DIMS-1];\
                (larray) += (lstrides)[ULAB_MAX_DIMS - 2];\
                (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * results->shape[ULAB_MAX_DIMS-1];\
                (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
                k++;\
            } while(k < (results)->shape[ULAB_MAX_DIMS - 2]);\
            (larray) -= (lstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS-2];\
            (larray) += (lstrides)[ULAB_MAX_DIMS - 3];\
            (rarray) -= (rstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS-2];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 3];\
            j++;\
        } while(j < (results)->shape[ULAB_MAX_DIMS - 3]);\
        (larray) -= (lstrides)[ULAB_MAX_DIMS - 3] * (results)->shape[ULAB_MAX_DIMS-3];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 4];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 3] * (results)->shape[ULAB_MAX_DIMS-3];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 4];\
        i++;\
    } while(i < (results)->shape[ULAB_MAX_DIMS - 4]);\

#define POWER_LOOP(results, type_out, type_left, type_right, larray, lstrides, rarray, rstrides)\
    type_out *array = (type_out *)results->array;\
    size_t i = 0;\
    do {\
        size_t j = 0;\
        do {\
            size_t k = 0;\
            do {\
                size_t l = 0;\
                do {\
                    *array++ = MICROPY_FLOAT_C_FUN(pow)(*((type_left *)(larray)), *((type_right *)(rarray)));\
                    (larray) += (lstrides)[ULAB_MAX_DIMS - 1];\
                    (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
                    l++;\
                } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\
                (larray) -= (lstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
                (larray) += (lstrides)[ULAB_MAX_DIMS - 2];\
                (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
                (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
                k++;\
            } while(k < (results)->shape[ULAB_MAX_DIMS - 2]);\
            (larray) -= (lstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS-2];\
            (larray) += (lstrides)[ULAB_MAX_DIMS - 3];\
            (rarray) -= (rstrides)[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS-2];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 3];\
            j++;\
        } while(j < (results)->shape[ULAB_MAX_DIMS - 3]);\
        (larray) -= (lstrides)[ULAB_MAX_DIMS - 3] * (results)->shape[ULAB_MAX_DIMS-3];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 4];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 3] * (results)->shape[ULAB_MAX_DIMS-3];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 4];\
        i++;\
    } while(i < (results)->shape[ULAB_MAX_DIMS - 4]);\

#endif /* ULAB_MAX_DIMS == 4 */
#endif /* ULAB_HAS_FUNCTION_ITERATOR */


#if ULAB_MAX_DIMS == 1
#define ASSIGNMENT_LOOP(results, type_left, type_right, lstrides, rarray, rstrides)\
    type_left *larray = (type_left *)(results)->array;\
    size_t l = 0;\
    do {\
        *larray = (type_left)(*((type_right *)(rarray)));\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 1];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
        l++;\
    } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\

#endif /* ULAB_MAX_DIMS == 1 */

#if ULAB_MAX_DIMS == 2
#define ASSIGNMENT_LOOP(results, type_left, type_right, lstrides, rarray, rstrides)\
    type_left *larray = (type_left *)(results)->array;\
    size_t k = 0;\
    do {\
        size_t l = 0;\
        do {\
            *larray = (type_left)(*((type_right *)(rarray)));\
            (larray) += (lstrides)[ULAB_MAX_DIMS - 1];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
            l++;\
        } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\
        (larray) -= (lstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 2];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
        k++;\
    } while(k < (results)->shape[ULAB_MAX_DIMS - 2]);\

#endif /* ULAB_MAX_DIMS == 2 */

#if ULAB_MAX_DIMS == 3
#define ASSIGNMENT_LOOP(results, type_left, type_right, lstrides, rarray, rstrides)\
    type_left *larray = (type_left *)(results)->array;\
    size_t j = 0;\
    do {\
        size_t k = 0;\
        do {\
            size_t l = 0;\
            do {\
                *larray = (type_left)(*((type_right *)(rarray)));\
                (larray) += (lstrides)[ULAB_MAX_DIMS - 1];\
                (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
                l++;\
            } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\
            (larray) -= (lstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
            (larray) += (lstrides)[ULAB_MAX_DIMS - 2];\
            (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
            k++;\
        } while(k < (results)->shape[ULAB_MAX_DIMS - 2]);\
        (larray) -= (lstrides)[ULAB_MAX_DIMS - 2] * results->shape[ULAB_MAX_DIMS-2];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 3];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 2] * results->shape[ULAB_MAX_DIMS-2];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 3];\
        j++;\
    } while(j < (results)->shape[ULAB_MAX_DIMS - 3]);\

#endif /* ULAB_MAX_DIMS == 3 */

#if ULAB_MAX_DIMS == 4
#define ASSIGNMENT_LOOP(results, type_left, type_right, lstrides, rarray, rstrides)\
    type_left *larray = (type_left *)(results)->array;\
    size_t i = 0;\
    do {\
        size_t j = 0;\
        do {\
            size_t k = 0;\
            do {\
                size_t l = 0;\
                do {\
                    *larray = (type_left)(*((type_right *)(rarray)));\
                    (larray) += (lstrides)[ULAB_MAX_DIMS - 1];\
                    (rarray) += (rstrides)[ULAB_MAX_DIMS - 1];\
                    l++;\
                } while(l < (results)->shape[ULAB_MAX_DIMS - 1]);\
                (larray) -= (lstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
                (larray) += (lstrides)[ULAB_MAX_DIMS - 2];\
                (rarray) -= (rstrides)[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS-1];\
                (rarray) += (rstrides)[ULAB_MAX_DIMS - 2];\
                k++;\
            } while(k < (results)->shape[ULAB_MAX_DIMS - 2]);\
            (larray) -= (lstrides)[ULAB_MAX_DIMS - 2] * results->shape[ULAB_MAX_DIMS-2];\
            (larray) += (lstrides)[ULAB_MAX_DIMS - 3];\
            (rarray) -= (rstrides)[ULAB_MAX_DIMS - 2] * results->shape[ULAB_MAX_DIMS-2];\
            (rarray) += (rstrides)[ULAB_MAX_DIMS - 3];\
            j++;\
        } while(j < (results)->shape[ULAB_MAX_DIMS - 3]);\
        (larray) -= (lstrides)[ULAB_MAX_DIMS - 3] * (results)->shape[ULAB_MAX_DIMS-3];\
        (larray) += (lstrides)[ULAB_MAX_DIMS - 4];\
        (rarray) -= (rstrides)[ULAB_MAX_DIMS - 3] * (results)->shape[ULAB_MAX_DIMS-3];\
        (rarray) += (rstrides)[ULAB_MAX_DIMS - 4];\
        i++;\
    } while(i < (results)->shape[ULAB_MAX_DIMS - 4]);\

#endif /* ULAB_MAX_DIMS == 4 */

#endif
