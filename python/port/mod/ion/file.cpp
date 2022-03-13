extern "C" {
#include <py/smallint.h>
#include <py/objint.h>
#include <py/objstr.h>
#include <py/objtype.h>
#include <py/runtime.h>
#include <py/stream.h>
#include <py/builtin.h>
#include <py/obj.h>
#include <py/misc.h>
#include "file.h"
}

#include <algorithm>
#include <string.h>
#include <ion/storage.h>

STATIC void file_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
STATIC mp_obj_t file_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args);
STATIC void file_attr(mp_obj_t self_in, qstr attribute, mp_obj_t *destination);
STATIC mp_obj_t file___iter__(mp_obj_t o_in, mp_obj_iter_buf_t *iter_buf);

STATIC mp_obj_t file_tell(mp_obj_t o_in);

const mp_obj_fun_builtin_fixed_t file_tell_obj = {
  {&mp_type_fun_builtin_1},
  {(mp_fun_0_t)file_tell}
};

STATIC mp_obj_t file_seek(size_t n_args, const mp_obj_t* args);

const mp_obj_fun_builtin_var_t file_seek_obj = {
    {&mp_type_fun_builtin_var},
    MP_OBJ_FUN_MAKE_SIG(2, 3, false),
    {(mp_fun_var_t)file_seek}
};

STATIC mp_obj_t file_readline(size_t n_args, const mp_obj_t* args);

const mp_obj_fun_builtin_var_t file_readline_obj = {
    {&mp_type_fun_builtin_var},
    MP_OBJ_FUN_MAKE_SIG(1, 2, false),
    {(mp_fun_var_t)file_readline}
};

STATIC mp_obj_t file_readlines(size_t n_args, const mp_obj_t* args);

const mp_obj_fun_builtin_var_t file_readlines_obj = {
    {&mp_type_fun_builtin_var},
    MP_OBJ_FUN_MAKE_SIG(1, 2, false),
    {(mp_fun_var_t)file_readlines}
};

STATIC mp_obj_t file_truncate(size_t n_args, const mp_obj_t* args);

const mp_obj_fun_builtin_var_t file_truncate_obj = {
    {&mp_type_fun_builtin_var},
    MP_OBJ_FUN_MAKE_SIG(1, 2, false),
    {(mp_fun_var_t)file_truncate}
};

STATIC mp_obj_t file_read(size_t n_args, const mp_obj_t* args);

const mp_obj_fun_builtin_var_t file_read_obj = {
    {&mp_type_fun_builtin_var},
    MP_OBJ_FUN_MAKE_SIG(1, 2, false),
    {(mp_fun_var_t)file_read}
};

STATIC mp_obj_t file_write(mp_obj_t o_in, mp_obj_t o_s);

const mp_obj_fun_builtin_fixed_t file_write_obj = {
  {&mp_type_fun_builtin_2},
  {(mp_fun_0_t)file_write}
};

STATIC mp_obj_t file_writelines(mp_obj_t o_in, mp_obj_t o_lines);

const mp_obj_fun_builtin_fixed_t file_writelines_obj = {
  {&mp_type_fun_builtin_2},
  {(mp_fun_0_t)file_writelines}
};

STATIC mp_obj_t file_seekable(mp_obj_t o_in);

const mp_obj_fun_builtin_fixed_t file_seekable_obj = {
  {&mp_type_fun_builtin_1},
  {(mp_fun_0_t)file_seekable}
};

STATIC mp_obj_t file_close(mp_obj_t o_in);

const mp_obj_fun_builtin_fixed_t file_close_obj = {
  {&mp_type_fun_builtin_1},
  {(mp_fun_0_t)file_close}
};

STATIC mp_obj_t file___enter__(mp_obj_t o_in);

const mp_obj_fun_builtin_fixed_t file___enter___obj = {
  {&mp_type_fun_builtin_1},
  {(mp_fun_0_t)file___enter__}
};

STATIC mp_obj_t file___exit__(size_t n_args, const mp_obj_t* args);

const mp_obj_fun_builtin_var_t file___exit___obj = {
    {&mp_type_fun_builtin_var},
    MP_OBJ_FUN_MAKE_SIG(4, 4, false),
    {(mp_fun_var_t)file___exit__}
};

STATIC mp_obj_t file_fileno(mp_obj_t o_in);

const mp_obj_fun_builtin_fixed_t file_fileno_obj = {
  {&mp_type_fun_builtin_1},
  {(mp_fun_0_t)file_fileno}
};

STATIC mp_obj_t file_flush(mp_obj_t o_in);

const mp_obj_fun_builtin_fixed_t file_flush_obj = {
  {&mp_type_fun_builtin_1},
  {(mp_fun_0_t)file_flush}
};

STATIC mp_obj_t file_isatty(mp_obj_t o_in);

const mp_obj_fun_builtin_fixed_t file_isatty_obj = {
  {&mp_type_fun_builtin_1},
  {(mp_fun_0_t)file_isatty}
};

STATIC mp_obj_t file_readable(mp_obj_t o_in);

const mp_obj_fun_builtin_fixed_t file_readable_obj = {
  {&mp_type_fun_builtin_1},
  {(mp_fun_0_t)file_readable}
};

STATIC mp_obj_t file_writable(mp_obj_t o_in);

const mp_obj_fun_builtin_fixed_t file_writable_obj = {
  {&mp_type_fun_builtin_1},
  {(mp_fun_0_t)file_writable}
};

STATIC const mp_rom_map_elem_t file_type_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_SEEK_SET), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_SEEK_CUR), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_SEEK_END), MP_ROM_INT(2) },
};

STATIC MP_DEFINE_CONST_DICT(file_type_globals, file_type_globals_table);

/*
 * File type creation.
 *
 * Constructor is set to nullptr, forcing users to use "open".
 * attr is used to return methods ands attributes.
 * file___iter__ return an iterable object.
 */
extern const mp_obj_type_t file_type = {
    { &mp_type_type },  // base
    0,                  // flags
    MP_QSTR_file,       // name
    file_print,         // __repr__, __srt__
    nullptr,            // __new__, __init__
    nullptr,            // __call__
    nullptr,            // unary operations
    nullptr,            // binary operations
    file_attr,          // load, store, delete attributes
    nullptr,            // load, store, delete subscripting
    file___iter__,      // __iter__
    nullptr,            // __next__
    nullptr,            // buffer
    nullptr,            // protocol
    nullptr,            // parent
    (mp_obj_dict_t*) &file_type_globals   // globals table
};

typedef enum _file_mode_t {
    READ = 0, WRITE = 1, APPEND = 2, CREATE = 3
} file_mode_t;

typedef enum _file_bin_t {
    TEXT = 0, BINARY = 1
} file_bin_t;


typedef struct _file_obj_t {
    mp_obj_base_t base;
    
    mp_obj_t name;
    mp_obj_t mode_repr;
    
    file_mode_t     open_mode;
    bool            edit_mode;
    file_bin_t      binary_mode;
    
    Ion::Storage::Record record;
    
    size_t position;
    
    bool closed;
    
} file_obj_t;

STATIC mp_obj_t __file_read_backend(file_obj_t* file, mp_int_t size, bool with_line_sep);

/*
 * Definition of the file iterator object.
 * iternext gets the next line of the file.
 */
typedef struct _file_it_obj_t {
    mp_obj_base_t base;
    mp_fun_1_t iternext;
    mp_obj_t file;
} file_it_obj_t;

STATIC mp_obj_t file_it___next__(mp_obj_t self_in) {
    file_it_obj_t *file_it = (file_it_obj_t*) MP_OBJ_TO_PTR(self_in);
    file_obj_t *file = (file_obj_t*) MP_OBJ_TO_PTR(file_it->file);
    
    mp_obj_t ret = __file_read_backend(file, -1, false);
    
    if (ret == mp_const_none) {
        return MP_OBJ_STOP_ITERATION;
    } else {
        return ret;
    }
}

STATIC mp_obj_t file___iter__(mp_obj_t self_in, mp_obj_iter_buf_t *iter_buf) {
    assert(sizeof(file_it_obj_t) <= sizeof(mp_obj_iter_buf_t));
    
    file_obj_t *file = (file_obj_t*) MP_OBJ_TO_PTR(self_in);
    
    file_it_obj_t *o = (file_it_obj_t*)iter_buf;
    o->base.type = &mp_type_polymorph_iter;
    o->iternext = file_it___next__;
    o->file = file;
    return MP_OBJ_FROM_PTR(o);
}

// Gets attributs and methods for file object.
// destination[0] is set to object itself, destination[1] is set to self (only for methods).
STATIC void file_attr(mp_obj_t self_in, qstr attribute, mp_obj_t *destination) {
    file_obj_t *self = (file_obj_t*) MP_OBJ_TO_PTR(self_in);
    
    switch(attribute) {
        case MP_QSTR_closed:
            destination[0] = mp_obj_new_bool(self->closed);
            break;
        case MP_QSTR_name:
            destination[0] = (mp_obj_t) self->name;
            break;
        case MP_QSTR_mode:
            destination[0] = (mp_obj_t) self->mode_repr;
            break;
        case MP_QSTR___enter__:
            destination[0] = (mp_obj_t) MP_ROM_PTR(&file___enter___obj);
            destination[1] = self_in;
            break;
        case MP_QSTR___exit__:
            destination[0] = (mp_obj_t) MP_ROM_PTR(&file___exit___obj);
            destination[1] = self_in;
            break;
        case MP_QSTR_close:
            destination[0] = (mp_obj_t) MP_ROM_PTR(&file_close_obj);
            destination[1] = self_in;
            break;
        case MP_QSTR_tell:
            destination[0] = (mp_obj_t) MP_ROM_PTR(&file_tell_obj);
            destination[1] = self_in;
            break;
        case MP_QSTR_seek:
            destination[0] = (mp_obj_t) MP_ROM_PTR(&file_seek_obj);
            destination[1] = self_in;
            break;
        case MP_QSTR_seekable:
            destination[0] = (mp_obj_t) MP_ROM_PTR(&file_seekable_obj);
            destination[1] = self_in;
            break;
        case MP_QSTR_fileno:
            destination[0] = (mp_obj_t) MP_ROM_PTR(&file_fileno_obj);
            destination[1] = self_in;
            break;
        case MP_QSTR_flush:
            destination[0] = (mp_obj_t) MP_ROM_PTR(&file_flush_obj);
            destination[1] = self_in;
            break;
        case MP_QSTR_isatty:
            destination[0] = (mp_obj_t) MP_ROM_PTR(&file_isatty_obj);
            destination[1] = self_in;
            break;
        case MP_QSTR_readable:
            destination[0] = (mp_obj_t) MP_ROM_PTR(&file_readable_obj);
            destination[1] = self_in;
            break;
        case MP_QSTR_writable:
            destination[0] = (mp_obj_t) MP_ROM_PTR(&file_writable_obj);
            destination[1] = self_in;
            break;
        case MP_QSTR_read:
            destination[0] = (mp_obj_t) MP_ROM_PTR(&file_read_obj);
            destination[1] = self_in;
            break;
        case MP_QSTR_readline:
            destination[0] = (mp_obj_t) MP_ROM_PTR(&file_readline_obj);
            destination[1] = self_in;
            break;
        case MP_QSTR_readlines:
            destination[0] = (mp_obj_t) MP_ROM_PTR(&file_readlines_obj);
            destination[1] = self_in;
            break;
        case MP_QSTR_write:
            destination[0] = (mp_obj_t) MP_ROM_PTR(&file_write_obj);
            destination[1] = self_in;
            break;
        case MP_QSTR_writelines:
            destination[0] = (mp_obj_t) MP_ROM_PTR(&file_writelines_obj);
            destination[1] = self_in;
            break;
        case MP_QSTR_truncate:
            destination[0] = (mp_obj_t) MP_ROM_PTR(&file_truncate_obj);
            destination[1] = self_in;
            break;
        default:
            break;
    }
}

STATIC void file_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    file_obj_t *self = (file_obj_t*) MP_OBJ_TO_PTR(self_in);
    
    size_t l;
    const char* file_name = mp_obj_str_get_data(self->name, &l);
    const char* file_mode = mp_obj_str_get_data(self->mode_repr, &l);
    
    
    mp_print_str(print, "<ion.file name='");
    mp_print_str(print, file_name);
    mp_print_str(print, "' mode='");
    mp_print_str(print, file_mode);
    mp_print_str(print, "'");
    if (self->closed)
        mp_print_str(print, " closed");
    mp_print_str(print, ">");
}

/*
 * File constructor takes two arguments:
 *  - name: name of the file
 *  - mode: mode of opening (optional, r by default)
 *
 */
STATIC mp_obj_t file_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 2, true);

    file_obj_t *file = m_new_obj(file_obj_t);
    
    if (!mp_obj_is_str(args[0])) {
        mp_raise_ValueError("path must be a string!");
    }
    
    // Store and parse file name
    size_t l;
    const char* file_name = mp_obj_str_get_data(args[0], &l);
    file->name = mp_obj_new_str(file_name, l);
    
    // Parses file mode
    file->open_mode   = READ;
    file->edit_mode   = false;
    file->binary_mode = TEXT;
    
    if (n_args == 2) {
        if (!mp_obj_is_str(args[1])) {
            mp_raise_ValueError("mode must be a string!");
        }
        
        const char* file_mode = mp_obj_str_get_data(args[1], &l);
        
        for(size_t i = 0; i < l; i++) {
            char c = file_mode[i];
            
            switch(c) {
                case 'r':
                    file->open_mode = READ;
                    break;
                case 'w':
                    file->open_mode = WRITE;
                    break;
                case 'a':
                    file->open_mode = APPEND;
                    break;
                case 'x':
                    file->open_mode = CREATE;
                    break;
                case '+':
                    file->edit_mode = true;
                    break;
                case 't':
                    file->binary_mode = TEXT;
                    break;
                case 'b':
                    file->binary_mode = BINARY;
                    break;
            }
        }
    }
    
    if (!Ion::Storage::FullNameCompliant(file_name)) {
        mp_raise_OSError(22);
    }
    
    Ion::Storage::Record::ErrorStatus status;

    switch(file->open_mode) {
        case READ:
            file->record = Ion::Storage::sharedStorage()->recordNamed(file_name);
            file->position = 0;
            if (file->record == Ion::Storage::Record()) {
                mp_raise_OSError(2);
            }
            break;
        case CREATE:
            file->position = 0;
            status = Ion::Storage::sharedStorage()->createRecordWithFullName(file_name, "", 0);
            switch (status) {
                case Ion::Storage::Record::ErrorStatus::NameTaken:
                    mp_raise_OSError(17);
                    break;
                case Ion::Storage::Record::ErrorStatus::NotEnoughSpaceAvailable:
                    mp_raise_OSError(28);
                    break;
                default:
                    break;
            }
            file->record = Ion::Storage::sharedStorage()->recordNamed(file_name);
            break;
        case WRITE:
            file->position = 0;
            status = Ion::Storage::sharedStorage()->createRecordWithFullName(file_name, "", 0);
            switch (status) {
                case Ion::Storage::Record::ErrorStatus::NameTaken:
                    // setValue messes with empty buffer, so we delete record and re-create it.
                    file->record = Ion::Storage::sharedStorage()->recordNamed(file_name);
                    file->record.destroy();
                    
                    status = Ion::Storage::sharedStorage()->createRecordWithFullName(file_name, "", 0);
                    
                    if (status == Ion::Storage::Record::ErrorStatus::NotEnoughSpaceAvailable) {
                        mp_raise_OSError(28);
                    }
                    break;
                case Ion::Storage::Record::ErrorStatus::NotEnoughSpaceAvailable:
                    mp_raise_OSError(28);
                    break;
                default:
                    break;
            }
            file->record = Ion::Storage::sharedStorage()->recordNamed(file_name);
            break;
        case APPEND:
            status = Ion::Storage::sharedStorage()->createRecordWithFullName(file_name, "", 0);
            switch (status) {
                case Ion::Storage::Record::ErrorStatus::NotEnoughSpaceAvailable:
                    mp_raise_OSError(28);
                    break;
                default:
                    break;
            }
            file->record = Ion::Storage::sharedStorage()->recordNamed(file_name);
            file->position = file->record.value().size;
            break;
    }
    
    file->base.type = &file_type;
    
    // Save mode representation
    uint8_t offset = 0;
    char file_mode[4] = {0, 0, 0, 0};
    
    switch(file->open_mode) {
        case READ:
            file_mode[0] = 'r';
            break;
        case WRITE:
            file_mode[0] = 'w';
            break;
        case APPEND:
            file_mode[0] = 'a';
            break;
        case CREATE:
            file_mode[0] = 'x';
            break;
    }
    
    if (file->edit_mode) {
        file_mode[1] = '+';
        offset = 1;
    }
    
    switch(file->binary_mode) {
        case TEXT:
            file_mode[1 + offset] = 't';
            break;
        case BINARY:
            file_mode[1 + offset] = 'b';
            break;
    }
    
    file->mode_repr = mp_obj_new_str(file_mode, 2 + offset);
    
    
    return MP_OBJ_FROM_PTR(file);
}

void check_closed(file_obj_t* file) {
    if (file->closed)
        mp_raise_ValueError("I/O operation on closed file");

    // This is VERY imports, as it eliminates concurrency
    // issues (eg. removing the file and then writing to it, 
    // having file move in the store, etc.)
    size_t l;
    const char* file_name = mp_obj_str_get_data(file->name, &l);
    file->record = Ion::Storage::sharedStorage()->recordNamed(file_name);

    if (file->record == Ion::Storage::Record()) {
        mp_raise_OSError(2);
    }
}


// Methods

STATIC mp_obj_t file___enter__(mp_obj_t o_in) {
    
    if(!mp_obj_is_type(o_in, &file_type)) {
        mp_raise_TypeError("self must be a file!");
    }
    
    return o_in;
}

STATIC mp_obj_t file___exit__(size_t n_args, const mp_obj_t* args) {
    mp_arg_check_num(n_args, 0, 4, 4, false);
    
    if(!mp_obj_is_type(args[0], &file_type)) {
        mp_raise_TypeError("self must be a file!");
    }
    
    return file_close(args[0]);
}

STATIC mp_obj_t file_close(mp_obj_t o_in) {
    if(!mp_obj_is_type(o_in, &file_type)) {
        mp_raise_TypeError("self must be a file!");
    }
    
    file_obj_t* file = (file_obj_t*) MP_OBJ_TO_PTR(o_in);
    
    if (!file->closed) {
        file->record = Ion::Storage::Record();
        file->closed = true;
    }
    
    return mp_const_none;
}


STATIC mp_obj_t file_tell(mp_obj_t o_in) {
    if(!mp_obj_is_type(o_in, &file_type)) {
        mp_raise_TypeError("self must be a file!");
    }
    
    file_obj_t* file = (file_obj_t*) MP_OBJ_TO_PTR(o_in);
    
    check_closed(file);
    
    return mp_obj_new_int(file->position);
}

STATIC mp_obj_t file_seek(size_t n_args, const mp_obj_t* args) {
    mp_arg_check_num(n_args, 0, 2, 3, false);
    
    if(!mp_obj_is_type(args[0], &file_type)) {
        mp_raise_TypeError("self must be a file!");
    }

    file_obj_t *file = (file_obj_t*) MP_OBJ_TO_PTR(args[0]);
    
    check_closed(file);
    
    if (!mp_obj_is_integer(args[1])) {
        mp_raise_ValueError("offset must be an int!");
    }
    
    int position = mp_obj_get_int(args[1]);
    mp_int_t whence = 0;
    
    if (n_args > 2) {
        if (!mp_obj_is_integer(args[2])) {
            mp_raise_ValueError("whence must be an int!");
        }
        
        whence = mp_obj_get_int(args[2]);
    }
    
    int new_position = file->position;
    size_t file_size = file->record.value().size;
        
    switch (whence) {
        // SEEK_SET
        case 0:
            new_position = position;
            break;
        // SEEK_CUR
        case 1:
            new_position += position;
            break;
        // SEEK_END
        case 2:
            new_position = file_size + position;
            break;
        default:
            mp_raise_ValueError("invalid whence (should be 0, 1 or 2)");
    }
    
    if (new_position < 0) {
        mp_raise_ValueError("negative seek position");
    } else {
        file->position = (size_t) new_position;
    }
    
    return mp_obj_new_int(file->position);
}

STATIC mp_obj_t file_seekable(mp_obj_t o_in) {
    if(!mp_obj_is_type(o_in, &file_type)) {
        mp_raise_TypeError("self must be a file!");
    }
    
    file_obj_t *file = (file_obj_t*) MP_OBJ_TO_PTR(o_in);
    
    check_closed(file);
    
    return mp_const_true;
}

STATIC mp_obj_t file_fileno(mp_obj_t o_in) {
    file_obj_t *file = (file_obj_t*) MP_OBJ_TO_PTR(o_in);
    check_closed(file);

    mp_raise_OSError(1);
    return mp_const_none;
}

STATIC mp_obj_t file_flush(mp_obj_t o_in) {
    file_obj_t *file = (file_obj_t*) MP_OBJ_TO_PTR(o_in);
    check_closed(file);

    return mp_const_none;
}

STATIC mp_obj_t file_isatty(mp_obj_t o_in) {
    file_obj_t *file = (file_obj_t*) MP_OBJ_TO_PTR(o_in);
    check_closed(file);

    return mp_const_false;
}

STATIC mp_obj_t file_writable(mp_obj_t o_in) {
    file_obj_t *file = (file_obj_t*) MP_OBJ_TO_PTR(o_in);
    check_closed(file);

    if (file->open_mode == READ && file->edit_mode != true) {
        return mp_const_false;
    }

    return mp_const_true;
}

STATIC mp_obj_t file_readable(mp_obj_t o_in) {
    file_obj_t *file = (file_obj_t*) MP_OBJ_TO_PTR(o_in);
    check_closed(file);

    if (file->open_mode != READ && file->edit_mode != true) {
        return mp_const_false;
    }

    return mp_const_true;
}

STATIC mp_obj_t file_write(mp_obj_t o_in, mp_obj_t o_s) {
    
    if(!mp_obj_is_type(o_in, &file_type)) {
        mp_raise_TypeError("self must be a file!");
    }

    file_obj_t *file = (file_obj_t*) MP_OBJ_TO_PTR(o_in);
    
    check_closed(file);
    
    if (file->open_mode == READ && file->edit_mode != true) {
        mp_raise_OSError(1);
    }
    
    if (file->binary_mode == TEXT) {
        if (!mp_obj_is_str(o_s)) {
            mp_raise_ValueError("s must be a str!");
        }
    } else if (file->binary_mode == BINARY) {
        if (!mp_obj_is_type(o_s, &mp_type_bytes)) {
            mp_raise_ValueError("s must be a bytes!");
        }
    }
    
    size_t len;
    const char* buffer;
    buffer = mp_obj_str_get_data(o_s, &len);

    size_t previous_size = file->record.value().size;
    
    // Claim available space.
    size_t available_size = Ion::Storage::sharedStorage()->putAvailableSpaceAtEndOfRecord(file->record);
    
    // Check if there is enough space left
    if (file->position + len > available_size) {
        Ion::Storage::sharedStorage()->getAvailableSpaceFromEndOfRecord(file->record, available_size - previous_size);
        mp_raise_OSError(28);
    }
    
    // Check if seek pos is higher than file end
    // If yes, fill space between there with 0x00
    if (file->position > previous_size) {
        memset((uint8_t*)(file->record.value().buffer) + file->position, 0x00, file->position - previous_size);
    }
    
    // Copy buffer to destination
    memcpy((uint8_t*)(file->record.value().buffer) + file->position, buffer, len);
    
    // Set size again
    Ion::Storage::sharedStorage()->getAvailableSpaceFromEndOfRecord(file->record, file->record.value().size - std::max(previous_size, file->position + len));
    
    file->position += len;
    
    return mp_obj_new_int(len);
}

STATIC mp_obj_t file_writelines(mp_obj_t o_in, mp_obj_t o_lines) {
    mp_obj_iter_buf_t iter_buf;
    mp_obj_t iterable = mp_getiter(o_lines, &iter_buf);
    mp_obj_t item;
    while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
        file_write(o_in, item);
    }
    return mp_const_none;
}

/*
 * Simpler read function used by read and readline.
 */
STATIC mp_obj_t __file_read_backend(file_obj_t* file, mp_int_t size, bool with_line_sep) {
    size_t file_size = file->record.value().size;
    size_t start = file->position;
    
    // Handle seek pos > file size
    // And size = 0
    if (start >= file_size || size == 0) {
        return mp_const_none;
    }
    
    size_t end = 0;
    
    // size == 0 handled earlier.
    if (size < 0) {
        end = file_size;
    } else {
        end = std::min(file_size, file->position + size);
    }
    
    // Handle line separator case.
    // Always use \n, because simpler.
    if (with_line_sep) {
        for(size_t i = start; i < end; i++) {
            if (*((uint8_t*)(file->record.value().buffer) + i) == '\n') {
                end = i + 1;
                break;
            }
        }
    }
    
    file->position = end;
    
    
    // Return different type based on mode.
    if (file->binary_mode == TEXT)
        return mp_obj_new_str((const char*)file->record.value().buffer + start, end - start);
    if (file->binary_mode == BINARY)
        return mp_obj_new_bytes((const byte*)file->record.value().buffer + start, end - start);
    
    return mp_const_none;
}

STATIC mp_obj_t file_read(size_t n_args, const mp_obj_t* args) {
    mp_arg_check_num(n_args, 0, 1, 2, false);
    
    // Check type of self
    if(!mp_obj_is_type(args[0], &file_type)) {
        mp_raise_TypeError("self must be a file!");
    }

    file_obj_t *file = (file_obj_t*) MP_OBJ_TO_PTR(args[0]);
    
    check_closed(file);
    
    // Check mode
    if (file->open_mode != READ && file->edit_mode != true) {
        mp_raise_OSError(1);
    }
    
    mp_int_t size = -1;
    
    // Check size arg
    if (n_args > 1) {
        if (!mp_obj_is_integer(args[1])) {
            mp_raise_ValueError("size must be an int!");
        }
        
        size = mp_obj_get_int(args[1]);
    }

    // Call the actual read function.
    mp_obj_t ret = __file_read_backend(file, size, false);
    
    // Handle none return
    if (ret == mp_const_none) {
        if (file->binary_mode == TEXT)
            return mp_obj_new_str("", 0);
        if (file->binary_mode == BINARY)
            return mp_const_empty_bytes;
    } else {
        return ret;
    }
    
    return mp_const_none;
}

STATIC mp_obj_t file_readline(size_t n_args, const mp_obj_t* args) {
    mp_arg_check_num(n_args, 0, 1, 2, false);
    
    if(!mp_obj_is_type(args[0], &file_type)) {
        mp_raise_TypeError("self must be a file!");
    }

    file_obj_t *file = (file_obj_t*) MP_OBJ_TO_PTR(args[0]);
    
    check_closed(file);
    
    if (file->open_mode != READ && file->edit_mode != true) {
        mp_raise_OSError(1);
    }
    
    mp_int_t size = -1;
    
    if (n_args > 1) {
        if (!mp_obj_is_integer(args[1])) {
            mp_raise_ValueError("size must be an int!");
        }
        
        size = mp_obj_get_int(args[1]);
    }

    mp_obj_t ret = __file_read_backend(file, size, true);
    
    if (ret == mp_const_none) {
        if (file->binary_mode == TEXT)
            return mp_obj_new_str("", 0);
        if (file->binary_mode == BINARY)
            return mp_const_empty_bytes;
    } else {
        return ret;
    }
    
    return mp_const_none;
}

STATIC mp_obj_t file_readlines(size_t n_args, const mp_obj_t* args) {
    mp_arg_check_num(n_args, 0, 1, 2, false);
    
    if(!mp_obj_is_type(args[0], &file_type)) {
        mp_raise_TypeError("self must be a file!");
    }

    file_obj_t *file = (file_obj_t*) MP_OBJ_TO_PTR(args[0]);
    
    check_closed(file);
    
    if (file->open_mode != READ && file->edit_mode != true) {
        mp_raise_OSError(1);
    }
    
    mp_int_t hint = -1;
    
    if (n_args > 1) {
        if (!mp_obj_is_integer(args[1])) {
            mp_raise_ValueError("hint must be an int!");
        }
        
        hint = mp_obj_get_int(args[1]);
    }
    
    mp_obj_t list = mp_obj_new_list(0, NULL);
    
    if (hint <= 0) {
        // Read until there is no new lines.
        mp_obj_t ret = __file_read_backend(file, -1, true);
        while(ret != mp_const_none) {
            // Append to list.
            mp_obj_list_append(list, ret);
            ret = __file_read_backend(file, -1, true);
        }
    } else {
        mp_int_t curr_len = 0;
        
        // Read until total read > hint.
        mp_obj_t ret = __file_read_backend(file, -1, true);
        while(ret != mp_const_none && curr_len <= hint) {
            mp_obj_list_append(list, ret);
            size_t l = 0;
            mp_obj_str_get_data(ret, &l);
            curr_len += l;
            ret = __file_read_backend(file, -1, true);
        }
    }

    return list;
}

STATIC mp_obj_t file_truncate(size_t n_args, const mp_obj_t* args) {
    mp_arg_check_num(n_args, 0, 1, 2, false);
    
    if(!mp_obj_is_type(args[0], &file_type)) {
        mp_raise_TypeError("self must be a file!");
    }

    file_obj_t *file = (file_obj_t*) MP_OBJ_TO_PTR(args[0]);
    
    check_closed(file);
    
    if (file->open_mode == READ && file->edit_mode != true) {
        mp_raise_OSError(1);
    }
    
    if(!mp_obj_is_integer(args[1]) && args[1] != mp_const_none) {
        mp_raise_TypeError("size must be an integer!");
    }
    
    mp_int_t temp_new_end = file->position;
    
    if (args[1] != mp_const_none) {
        temp_new_end = mp_obj_get_int(args[1]);
    }
    
    if (temp_new_end < 0) {
        mp_raise_OSError(22);
    }
    
    size_t new_end = (size_t) temp_new_end;
    
    size_t previous_size = file->record.value().size;

    // Claim available space.
    size_t available_size = Ion::Storage::sharedStorage()->putAvailableSpaceAtEndOfRecord(file->record);
    
    // Check if there is enough space left
    if (new_end > available_size) {
        Ion::Storage::sharedStorage()->getAvailableSpaceFromEndOfRecord(file->record, available_size - previous_size);
        mp_raise_OSError(28);
    }
    
    // Check if new_end is higher than file end
    // If yes, fill space between there with 0x00
    if (new_end > previous_size) {
        memset((uint8_t*)(file->record.value().buffer) + new_end, 0x00, new_end - previous_size);
    }
    
    // Set new size
    Ion::Storage::sharedStorage()->getAvailableSpaceFromEndOfRecord(file->record, file->record.value().size - new_end);
    
    return mp_obj_new_int(new_end);
    
    return mp_const_none;
}

// Open method, with only name. Calls constructor.
mp_obj_t file_open(mp_obj_t file_name) {
    mp_obj_t args[1];
    args[0] = file_name;

    return file_make_new(nullptr, 1, 0, args);
}

// Open method, with name and mode. Calls constructor.
mp_obj_t file_open_mode(mp_obj_t file_name, mp_obj_t file_mode) {
    mp_obj_t args[2];
    args[0] = file_name;
    args[1] = file_mode;

    return file_make_new(nullptr, 2, 0, args);
}

