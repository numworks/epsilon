/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Zoltán Vörös
*/

#include <math.h>
#include <string.h>

#include "py/builtin.h"
#include "py/formatfloat.h"
#include "py/obj.h"
#include "py/parsenum.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "extmod/vfs.h"

#include "../../ndarray.h"
#include "../../ulab_tools.h"
#include "io.h"

#define ULAB_IO_BUFFER_SIZE         128
#define ULAB_IO_CLIPBOARD_SIZE      32
#define ULAB_IO_MAX_ROWS            65535

#define ULAB_IO_NULL_ENDIAN         0
#define ULAB_IO_LITTLE_ENDIAN       1
#define ULAB_IO_BIG_ENDIAN          2

#if ULAB_NUMPY_HAS_LOAD
static void io_read_(mp_obj_t stream, const mp_stream_p_t *stream_p, char *buffer, const char *string, uint16_t len, int *error) {
    size_t read = stream_p->read(stream, buffer, len, error);
    bool fail = false;
    if(read == len) {
        if(string != NULL) {
            if(memcmp(buffer, string, len) != 0) {
                fail = true;
            }
        }
    } else {
        fail = true;
    }
    if(fail) {
        stream_p->ioctl(stream, MP_STREAM_CLOSE, 0, error);
        mp_raise_msg(&mp_type_RuntimeError, translate("corrupted file"));
    }
}

static mp_obj_t io_load(mp_obj_t file) {
    if(!mp_obj_is_str(file)) {
        mp_raise_TypeError(translate("wrong input type"));
    }

    int error;
    char *buffer = m_new(char, ULAB_IO_BUFFER_SIZE);

    // test for endianness
    uint16_t x = 1;
    int8_t native_endianness = (x >> 8) == 1 ? ULAB_IO_BIG_ENDIAN : ULAB_IO_LITTLE_ENDIAN;

    mp_obj_t open_args[2] = {
        file,
        MP_OBJ_NEW_QSTR(MP_QSTR_rb)
    };

    mp_obj_t stream = mp_builtin_open_obj.fun.kw(2, open_args, (mp_map_t *)&mp_const_empty_map);
    const mp_stream_p_t *stream_p = mp_get_stream(stream);

    // read header
    // magic string
    io_read_(stream, stream_p, buffer, "\x93NUMPY", 6, &error);
    // simply discard the version number
    io_read_(stream, stream_p, buffer, NULL, 2, &error);
    // header length, represented as a little endian uint16 (0x76, 0x00)
    io_read_(stream, stream_p, buffer, NULL, 2, &error);

    uint16_t header_length = buffer[1];
    header_length <<= 8;
    header_length += buffer[0];

    // beginning of the dictionary describing the array
    io_read_(stream, stream_p, buffer, "{'descr': '", 11, &error);
    uint8_t dtype;

    io_read_(stream, stream_p, buffer, NULL, 1, &error);
    uint8_t endianness = ULAB_IO_NULL_ENDIAN;
    if(*buffer == '<') {
        endianness = ULAB_IO_LITTLE_ENDIAN;
    } else if(*buffer == '>') {
        endianness = ULAB_IO_BIG_ENDIAN;
    }

    io_read_(stream, stream_p, buffer, NULL, 2, &error);
    if(memcmp(buffer, "u1", 2) == 0) {
        dtype = NDARRAY_UINT8;
    } else if(memcmp(buffer, "i1", 2) == 0) {
        dtype = NDARRAY_INT8;
    } else if(memcmp(buffer, "u2", 2) == 0) {
        dtype = NDARRAY_UINT16;
    } else if(memcmp(buffer, "i2", 2) == 0) {
        dtype = NDARRAY_INT16;
    }
    #if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
    else if(memcmp(buffer, "f4", 2) == 0) {
        dtype = NDARRAY_FLOAT;
    }
    #else
    else if(memcmp(buffer, "f8", 2) == 0) {
        dtype = NDARRAY_FLOAT;
    }
    #endif
    #if ULAB_SUPPORTS_COMPLEX
    #if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
    else if(memcmp(buffer, "c8", 2) == 0) {
        dtype = NDARRAY_COMPLEX;
    }
    #else
    else if(memcmp(buffer, "c16", 3) == 0) {
        dtype = NDARRAY_COMPLEX;
    }
    #endif
    #endif /* ULAB_SUPPORT_COPMLEX */
    else {
        stream_p->ioctl(stream, MP_STREAM_CLOSE, 0, &error);
        mp_raise_TypeError(translate("wrong dtype"));
    }

    io_read_(stream, stream_p, buffer, "', 'fortran_order': False, 'shape': (", 37, &error);

    size_t *shape = m_new0(size_t, ULAB_MAX_DIMS);

    uint16_t bytes_to_read = MIN(ULAB_IO_BUFFER_SIZE, header_length - 51);
    // bytes_to_read is 128 at most. This should be enough to contain a
    // maximum of 4 size_t numbers plus the delimiters
    io_read_(stream, stream_p, buffer, NULL, bytes_to_read, &error);
    char *needle = buffer;
    uint8_t ndim = 0;

    // find out the number of dimensions by counting the commas in the string
    while(1) {
        if(*needle == ',') {
            ndim++;
            if(needle[1] == ')') {
                break;
            }
        } else if((*needle == ')') && (ndim > 0)) {
            ndim++;
            break;
        }
        needle++;
    }

    needle = buffer;
    for(uint8_t i = 0; i < ndim; i++) {
        size_t number = 0;
        // trivial number parsing here
        while(1) {
            if((*needle == ' ') || (*needle == '\t')) {
                needle++;
            }
            if((*needle > 47) && (*needle < 58)) {
                number = number * 10 + (*needle - 48);
            } else if((*needle == ',') || (*needle == ')')) {
                break;
            }
            else {
                stream_p->ioctl(stream, MP_STREAM_CLOSE, 0, &error);
                mp_raise_msg(&mp_type_RuntimeError, translate("corrupted file"));
            }
            needle++;
        }
        needle++;
        shape[ULAB_MAX_DIMS - ndim + i] = number;
    }

    // strip the rest of the header
    if((bytes_to_read + 51) < header_length) {
        io_read_(stream, stream_p, buffer, NULL, header_length - (bytes_to_read + 51), &error);
    }

    ndarray_obj_t *ndarray = ndarray_new_dense_ndarray(ndim, shape, dtype);
    char *array = (char *)ndarray->array;

    size_t read = stream_p->read(stream, array, ndarray->len * ndarray->itemsize, &error);
    if(read != ndarray->len * ndarray->itemsize) {
        stream_p->ioctl(stream, MP_STREAM_CLOSE, 0, &error);
        mp_raise_msg(&mp_type_RuntimeError, translate("corrupted file"));
    }

    stream_p->ioctl(stream, MP_STREAM_CLOSE, 0, &error);
    m_del(char, buffer, ULAB_IO_BUFFER_SIZE);

    // swap the bytes, if necessary
    if((native_endianness != endianness) && (dtype != NDARRAY_UINT8) && (dtype != NDARRAY_INT8)) {
        uint8_t sz = ndarray->itemsize;
        char *tmpbuff = NULL;

        #if ULAB_SUPPORTS_COMPLEX
        if(dtype == NDARRAY_COMPLEX) {
            // work with the floating point real and imaginary parts
            sz /= 2;
            tmpbuff = m_new(char, sz);
            for(size_t i = 0; i < ndarray->len; i++) {
                for(uint8_t k = 0; k < 2; k++) {
                    tmpbuff += sz;
                    for(uint8_t j = 0; j < sz; j++) {
                        memcpy(--tmpbuff, array++, 1);
                    }
                    memcpy(array-sz, tmpbuff, sz);
                }
            }
        } else {
        #endif
            tmpbuff = m_new(char, sz);
            for(size_t i = 0; i < ndarray->len; i++) {
                tmpbuff += sz;
                for(uint8_t j = 0; j < sz; j++) {
                    memcpy(--tmpbuff, array++, 1);
                }
                memcpy(array-sz, tmpbuff, sz);
            }
        #if ULAB_SUPPORTS_COMPLEX
        }
        #endif
        m_del(char, tmpbuff, sz);
    }

    m_del(size_t, shape, ULAB_MAX_DIMS);

    return MP_OBJ_FROM_PTR(ndarray);
}

MP_DEFINE_CONST_FUN_OBJ_1(io_load_obj, io_load);
#endif /* ULAB_NUMPY_HAS_LOAD */

#if ULAB_NUMPY_HAS_LOADTXT
static void io_assign_value(const char *clipboard, uint8_t len, ndarray_obj_t *ndarray, size_t *idx, uint8_t dtype) {
    mp_obj_t value = mp_parse_num_decimal(clipboard, len, false, false, NULL);
    if(dtype != NDARRAY_FLOAT) {
        mp_float_t _value = mp_obj_get_float(value);
        value = mp_obj_new_int((int32_t)MICROPY_FLOAT_C_FUN(round)(_value));
    }
    ndarray_set_value(dtype, ndarray->array, (*idx)++, value);
}

static mp_obj_t io_loadtxt(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_NONE } },
        { MP_QSTR_delimiter, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_NONE } },
        { MP_QSTR_comments, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_NONE } },
        { MP_QSTR_max_rows, MP_ARG_KW_ONLY | MP_ARG_INT, { .u_int = -1 } },
        { MP_QSTR_usecols, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_NONE } },
        { MP_QSTR_dtype, MP_ARG_KW_ONLY | MP_ARG_INT, { .u_int = NDARRAY_FLOAT } },
        { MP_QSTR_skiprows, MP_ARG_KW_ONLY | MP_ARG_INT, { .u_int = 0 } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t open_args[2] = {
        args[0].u_obj,
        MP_OBJ_NEW_QSTR(MP_QSTR_r)
    };

    mp_obj_t stream = mp_builtin_open_obj.fun.kw(2, open_args, (mp_map_t *)&mp_const_empty_map);
    const mp_stream_p_t *stream_p = mp_get_stream(stream);

    char *buffer = m_new(char, ULAB_IO_BUFFER_SIZE);
    int error;

    char delimiter = ' ';
    if(args[1].u_obj != mp_const_none) {
        size_t _len;
        char *_delimiter = m_new(char, 8);
        _delimiter = (char *)mp_obj_str_get_data(args[1].u_obj, &_len);
        delimiter = _delimiter[0];
    }

    char comment_char = '#';
    if(args[2].u_obj != mp_const_none) {
        size_t _len;
        char *_comment_char = m_new(char, 8);
        _comment_char = (char *)mp_obj_str_get_data(args[2].u_obj, &_len);
        comment_char = _comment_char[0];
    }

    uint16_t skiprows = args[6].u_int;
    uint16_t max_rows = ULAB_IO_MAX_ROWS;
    if((args[3].u_int > 0) && (args[3].u_int < ULAB_IO_MAX_ROWS)) {
        max_rows = args[3].u_int + skiprows;
    }

    uint16_t *cols = NULL;
    uint8_t used_columns = 0;
    if(args[4].u_obj != mp_const_none) {
        if(mp_obj_is_int(args[4].u_obj)) {
            used_columns = 1;
            cols = m_new(uint16_t, used_columns);
            cols[0] = (uint16_t)mp_obj_get_int(args[4].u_obj);
        } else {
            #if ULAB_MAX_DIMS == 1
            mp_raise_ValueError(translate("usecols keyword must be specified"));
            #else
            // assume that the argument is an iterable
            used_columns = (uint16_t)mp_obj_get_int(mp_obj_len(args[4].u_obj));
            cols = m_new(uint16_t, used_columns);
            mp_obj_iter_buf_t iter_buf;
            mp_obj_t item, iterable = mp_getiter(args[4].u_obj, &iter_buf);
            while((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
                *cols++ = (uint16_t)mp_obj_get_int(item);
            }
            cols -= used_columns;
            #endif
        }
    }

    uint8_t dtype = args[5].u_int;

    // count the columns and rows
    // we actually count only the rows and the items, and assume that
    // the number of columns can be gotten by means of a simple division,
    // i.e., that each row has the same number of columns
    char *offset;
    uint16_t rows = 0, items = 0, all_rows = 0;
    uint8_t read;
    uint8_t len = 0;

    do {
        read = (uint8_t)stream_p->read(stream, buffer, ULAB_IO_BUFFER_SIZE - 1, &error);
        buffer[read] = '\0';
        offset = buffer;
        while(*offset != '\0') {
            if(*offset == comment_char) {
                // clear the line till the end, or the buffer's end
                while((*offset != '\0')) {
                    offset++;
                    if(*offset == '\n') {
                        offset++;
                        all_rows++;
                        break;
                    }
                }
            }

            // catch whitespaces here: if these are not on a comment line, then they delimit a number
            if(*offset == '\n') {
                all_rows++;
                if(all_rows > skiprows) {
                    rows++;
                    items++;
                    len = 0;
                }
                if(all_rows == max_rows) {
                    break;
                }
            }

            if((*offset == ' ') || (*offset == '\t') || (*offset == '\v') ||
                (*offset == '\f') || (*offset == '\r') || (*offset == delimiter)) {
                offset++;
                while((*offset == ' ') || (*offset == '\t') || (*offset == '\v') || (*offset == '\f') || (*offset == '\r')) {
                    offset++;
                }
                if(len > 0) {
                    if(all_rows >= skiprows) {
                        items++;
                    }
                    len = 0;
                }
            } else {
                offset++;
                len++;
            }
        }
    } while((read > 0) && (all_rows < max_rows));

    if(rows == 0) {
        mp_raise_ValueError(translate("empty file"));
    }
    uint16_t columns = items / rows;

    if(columns < used_columns) {
        mp_raise_ValueError(translate("usecols is too high"));
    }

    size_t *shape = m_new0(size_t, ULAB_MAX_DIMS);

    #if ULAB_MAX_DIMS == 1
    shape[0] = rows;
    ndarray_obj_t *ndarray = ndarray_new_dense_ndarray(1, shape, dtype);
    #else
    if(args[4].u_obj == mp_const_none) {
        shape[ULAB_MAX_DIMS - 1] = columns;
    } else {
        shape[ULAB_MAX_DIMS - 1] = used_columns;
    }
    shape[ULAB_MAX_DIMS - 2] = rows;
    ndarray_obj_t *ndarray = ndarray_new_dense_ndarray(2, shape, dtype);
    #endif

    struct mp_stream_seek_t seek_s;
    seek_s.offset = 0;
    seek_s.whence = MP_SEEK_SET;
    stream_p->ioctl(stream, MP_STREAM_SEEK, (mp_uint_t)(uintptr_t)&seek_s, &error);

    char *clipboard = m_new(char, ULAB_IO_CLIPBOARD_SIZE);
    char *clipboard_origin = clipboard;

    rows = 0;
    columns = 0;
    len = 0;

    size_t idx = 0;
    do {
        read = stream_p->read(stream, buffer, ULAB_IO_BUFFER_SIZE - 1, &error);
        buffer[read] = '\0';
        offset = buffer;

        while(*offset != '\0') {
            if(*offset == comment_char) {
                // clear the line till the end, or the buffer's end
                while((*offset != '\0')) {
                    offset++;
                    if(*offset == '\n') {
                        rows++;
                        offset++;
                        break;
                    }
                }
            }

            if(rows == max_rows) {
                break;
            }

            if((*offset == ' ') || (*offset == '\t') || (*offset == '\v') ||
                (*offset == '\f') || (*offset == '\r') || (*offset == '\n') || (*offset == delimiter)) {
                offset++;
                while((*offset == ' ') || (*offset == '\t') || (*offset == '\v') ||
                    (*offset == '\f') || (*offset == '\r') || (*offset == '\n')) {
                    offset++;
                }
                if(len > 0) {
                    clipboard = clipboard_origin;
                    if(rows >= skiprows) {
                        #if ULAB_MAX_DIMS == 1
                        if(columns == cols[0]) {
                            io_assign_value(clipboard, len, ndarray, &idx, dtype);
                        }
                        #else
                        if(args[4].u_obj == mp_const_none) {
                            io_assign_value(clipboard, len, ndarray, &idx, dtype);
                        } else {
                            for(uint8_t c = 0; c < used_columns; c++) {
                                if(columns == cols[c]) {
                                    io_assign_value(clipboard, len, ndarray, &idx, dtype);
                                    break;
                                }
                            }
                        }
                        #endif
                    }
                    columns++;
                    len = 0;

                    if(offset[-1] == '\n') {
                        columns = 0;
                        rows++;
                    }
                }
            } else {
                *clipboard++ = *offset++;
                len++;
            }
        }
    } while((read > 0) && (rows < max_rows));

    stream_p->ioctl(stream, MP_STREAM_CLOSE, 0, &error);

    m_del(size_t, shape, ULAB_MAX_DIMS);
    m_del(char, buffer, ULAB_IO_BUFFER_SIZE);
    m_del(char, clipboard, ULAB_IO_CLIPBOARD_SIZE);
    m_del(uint16_t, cols, used_columns);

    return MP_OBJ_FROM_PTR(ndarray);
}

MP_DEFINE_CONST_FUN_OBJ_KW(io_loadtxt_obj, 1, io_loadtxt);
#endif /* ULAB_NUMPY_HAS_LOADTXT */


#if ULAB_NUMPY_HAS_SAVE
static uint8_t io_sprintf(char *buffer, const char *comma, size_t x) {
    uint8_t offset = 1;
    char *buf = buffer;
    // our own minimal implementation of sprintf for size_t types
    // this is required on systems, where sprintf is not available

    // find out, how many characters are required
    // we could call log10 here...
    for(size_t i = 10; i < 100000000; i *= 10) {
        if(x < i) {
            break;
        }
        buf++;
    }

    while(x > 0) {
        uint8_t rem = x % 10;
        *buf-- = '0' + rem;
        x /= 10;
        offset++;
    }

    buf += offset;
    while(*comma != '\0') {
        *buf++ = *comma++;
        offset++;
    }
    return offset - 1;
}

static mp_obj_t io_save(mp_obj_t file, mp_obj_t ndarray_) {
    if(!mp_obj_is_str(file) || !mp_obj_is_type(ndarray_, &ulab_ndarray_type)) {
        mp_raise_TypeError(translate("wrong input type"));
    }

    ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(ndarray_);
    int error;
    char *buffer = m_new(char, ULAB_IO_BUFFER_SIZE);
    uint8_t offset = 0;

    // test for endianness
    uint16_t x = 1;
    int8_t native_endianness = (x >> 8) == 1 ? '>' : '<';

    mp_obj_t open_args[2] = {
        file,
        MP_OBJ_NEW_QSTR(MP_QSTR_wb)
    };

    mp_obj_t stream = mp_builtin_open_obj.fun.kw(2, open_args, (mp_map_t *)&mp_const_empty_map);
    const mp_stream_p_t *stream_p = mp_get_stream(stream);

    // write header;
    // magic string + header length, which is always 128 - 10 = 118, represented as a little endian uint16 (0x76, 0x00)
    // + beginning of the dictionary describing the array
    memcpy(buffer, "\x93NUMPY\x01\x00\x76\x00{'descr': '", 21);
    offset += 21;

    buffer[offset] = native_endianness;
    if((ndarray->dtype == NDARRAY_UINT8) || (ndarray->dtype == NDARRAY_INT8)) {
        // for single-byte data, the endianness doesn't matter
        buffer[offset] = '|';
    }
    offset++;
    switch(ndarray->dtype) {
        case NDARRAY_UINT8:
            memcpy(buffer+offset, "u1", 2);
            break;
        case NDARRAY_INT8:
            memcpy(buffer+offset, "i1", 2);
            break;
        case NDARRAY_UINT16:
            memcpy(buffer+offset, "u2", 2);
            break;
        case NDARRAY_INT16:
            memcpy(buffer+offset, "i2", 2);
            break;
        case NDARRAY_FLOAT:
            #if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
            memcpy(buffer+offset, "f4", 2);
            #else
            memcpy(buffer+offset, "f8", 2);
            #endif
            break;
        #if ULAB_SUPPORTS_COMPLEX
        case NDARRAY_COMPLEX:
            #if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
            memcpy(buffer+offset, "c8", 2);
            #else
            memcpy(buffer+offset, "c16", 3);
            offset++;
            #endif
            break;
        #endif
    }

    offset += 2;
    memcpy(buffer+offset, "', 'fortran_order': False, 'shape': (", 37);
    offset += 37;

    if(ndarray->ndim == 1) {
        offset += io_sprintf(buffer+offset, ",\0", ndarray->shape[ULAB_MAX_DIMS - 1]);
    } else {
        for(uint8_t i = ndarray->ndim; i > 1; i--) {
            offset += io_sprintf(buffer+offset, ", \0", ndarray->shape[ULAB_MAX_DIMS - i]);
        }
        offset += io_sprintf(buffer+offset, "\0", ndarray->shape[ULAB_MAX_DIMS - 1]);
    }
    memcpy(buffer+offset, "), }", 4);
    offset += 4;
    // pad with space till the very end
    memset(buffer+offset, 32, ULAB_IO_BUFFER_SIZE - offset - 1);
    buffer[ULAB_IO_BUFFER_SIZE - 1] = '\n';
    stream_p->write(stream, buffer, ULAB_IO_BUFFER_SIZE, &error);

    // write the array data
    uint8_t sz = ndarray->itemsize;
    offset = 0;

    uint8_t *array = (uint8_t *)ndarray->array;

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
                    memcpy(buffer+offset, array, sz);
                    offset += sz;
                    if(offset == ULAB_IO_BUFFER_SIZE) {
                        stream_p->write(stream, buffer, offset, &error);
                        offset = 0;
                    }
                    array += ndarray->strides[ULAB_MAX_DIMS - 1];
                    l++;
                } while(l <  ndarray->shape[ULAB_MAX_DIMS - 1]);
            #if ULAB_MAX_DIMS > 1
                array -= ndarray->strides[ULAB_MAX_DIMS - 1] * ndarray->shape[ULAB_MAX_DIMS-1];
                array += ndarray->strides[ULAB_MAX_DIMS - 2];
                k++;
            } while(k <  ndarray->shape[ULAB_MAX_DIMS - 2]);
            #endif
        #if ULAB_MAX_DIMS > 2
            array -= ndarray->strides[ULAB_MAX_DIMS - 2] * ndarray->shape[ULAB_MAX_DIMS-2];
            array += ndarray->strides[ULAB_MAX_DIMS - 3];
            j++;
        } while(j <  ndarray->shape[ULAB_MAX_DIMS - 3]);
        #endif
    #if ULAB_MAX_DIMS > 3
        array -= ndarray->strides[ULAB_MAX_DIMS - 3] * ndarray->shape[ULAB_MAX_DIMS-3];
        array += ndarray->strides[ULAB_MAX_DIMS - 4];
        i++;
    } while(i <  ndarray->shape[ULAB_MAX_DIMS - 4]);
    #endif

    stream_p->write(stream, buffer, offset, &error);
    stream_p->ioctl(stream, MP_STREAM_CLOSE, 0, &error);

    m_del(char, buffer, ULAB_IO_BUFFER_SIZE);
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_2(io_save_obj, io_save);
#endif /* ULAB_NUMPY_HAS_SAVE */

#if ULAB_NUMPY_HAS_SAVETXT
static int8_t io_format_float(ndarray_obj_t *ndarray, mp_float_t (*func)(void *), uint8_t *array, char *buffer, const char *delimiter) {
    // own implementation of float formatting for platforms that don't have sprintf
    int8_t offset = 0;

    #if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
        #if MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_C
        const int precision = 6;
        #else
        const int precision = 7;
        #endif
    #else
        const int precision = 16;
    #endif

    #if ULAB_SUPPORTS_COMPLEX
    if(ndarray->dtype == NDARRAY_COMPLEX) {
        mp_float_t real = func(array);
        mp_float_t imag = func(array + ndarray->itemsize / 2);
        offset = mp_format_float(real, buffer, ULAB_IO_BUFFER_SIZE, 'f', precision, 'j');
        if(imag >= MICROPY_FLOAT_CONST(0.0)) {
            buffer[offset++] = '+';
        } else {
            buffer[offset++] = '-';
        }
        offset += mp_format_float(-imag, &buffer[offset], ULAB_IO_BUFFER_SIZE, 'f', precision, 'j');
    }
    #endif
    offset = (uint8_t)mp_format_float(func(array), buffer, ULAB_IO_BUFFER_SIZE, 'f', precision, '\0');

    #if ULAB_SUPPORTS_COMPLEX
    if(ndarray->dtype != NDARRAY_COMPLEX) {
        // complexes end with a 'j', floats with a '\0', so we have to wind back by one character
        offset--;
    }
    #endif

    while(*delimiter != '\0') {
        buffer[offset++] = *delimiter++;
    }

    return offset;
}

static mp_obj_t io_savetxt(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_NONE } },
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_NONE } },
        { MP_QSTR_delimiter, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_NONE } },
        { MP_QSTR_header, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_NONE } },
        { MP_QSTR_footer, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_NONE } },
        { MP_QSTR_comments, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_rom_obj = MP_ROM_NONE } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if(!mp_obj_is_str(args[0].u_obj) || !mp_obj_is_type(args[1].u_obj, &ulab_ndarray_type)) {
        mp_raise_TypeError(translate("wrong input type"));
    }

    ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(args[1].u_obj);

    #if ULAB_MAX_DIMS > 2
    if(ndarray->ndim > 2) {
        mp_raise_ValueError(translate("array has too many dimensions"));
    }
    #endif

    mp_obj_t open_args[2] = {
        args[0].u_obj,
        MP_OBJ_NEW_QSTR(MP_QSTR_w)
    };

    mp_obj_t stream = mp_builtin_open_obj.fun.kw(2, open_args, (mp_map_t *)&mp_const_empty_map);
    const mp_stream_p_t *stream_p = mp_get_stream(stream);

    char *buffer = m_new(char, ULAB_IO_BUFFER_SIZE);
    int error;

    if(mp_obj_is_str(args[3].u_obj)) {
        size_t _len;
        if(mp_obj_is_str(args[5].u_obj)) {
            const char *comments = mp_obj_str_get_data(args[5].u_obj, &_len);
            stream_p->write(stream, comments, _len, &error);
        } else {
            stream_p->write(stream, "# ", 2, &error);
        }
        const char *header = mp_obj_str_get_data(args[3].u_obj, &_len);
        stream_p->write(stream, header, _len, &error);
        stream_p->write(stream, "\n", 1, &error);
    }

    uint8_t *array = (uint8_t *)ndarray->array;
    mp_float_t (*func)(void *) = ndarray_get_float_function(ndarray->dtype);
    char *delimiter = m_new(char, 8);

    if(ndarray->ndim == 1) {
        delimiter[0] = '\n';
        delimiter[1] = '\0';
    } else if(args[2].u_obj == mp_const_none) {
        delimiter[0] = ' ';
        delimiter[1] = '\0';
    } else {
        size_t delimiter_len;
        delimiter = (char *)mp_obj_str_get_data(args[2].u_obj, &delimiter_len);
    }

    #if ULAB_MAX_DIMS > 1
    size_t k = 0;
    do {
    #endif
        size_t l = 0;
        do {
            int8_t chars = io_format_float(ndarray, func, array, buffer, l == ndarray->shape[ULAB_MAX_DIMS - 1] - 1 ? "\n" : delimiter);
            if(chars > 0) {
                stream_p->write(stream, buffer, chars, &error);
            }
            array += ndarray->strides[ULAB_MAX_DIMS - 1];
            l++;
        } while(l < ndarray->shape[ULAB_MAX_DIMS - 1]);
    #if ULAB_MAX_DIMS > 1
        array -= ndarray->strides[ULAB_MAX_DIMS - 1] * ndarray->shape[ULAB_MAX_DIMS-1];
        array += ndarray->strides[ULAB_MAX_DIMS - 2];
        k++;
    } while(k < ndarray->shape[ULAB_MAX_DIMS - 2]);
    #endif

    if(mp_obj_is_str(args[4].u_obj)) {
        size_t _len;
        if(mp_obj_is_str(args[5].u_obj)) {
            const char *comments = mp_obj_str_get_data(args[5].u_obj, &_len);
            stream_p->write(stream, comments, _len, &error);
        } else {
            stream_p->write(stream, "# ", 2, &error);
        }
        const char *footer = mp_obj_str_get_data(args[4].u_obj, &_len);
        stream_p->write(stream, footer, _len, &error);
        stream_p->write(stream, "\n", 1, &error);
    }

    stream_p->ioctl(stream, MP_STREAM_CLOSE, 0, &error);

    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_KW(io_savetxt_obj, 2, io_savetxt);
#endif /* ULAB_NUMPY_HAS_SAVETXT */
