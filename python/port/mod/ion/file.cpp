extern "C" {
#include <py/smallint.h>
#include <py/objint.h>
#include <py/objstr.h>
#include <py/objtype.h>
#include <py/runtime.h>
#include <py/stream.h>
#include <py/builtin.h>
#include <py/obj.h>
}

#include <string.h>
#include <ion/storage.h>

STATIC void file_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
STATIC mp_obj_t file_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args);

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

STATIC const mp_rom_map_elem_t file_type_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_SEEK_SET), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_SEEK_CUR), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_SEEK_END), MP_ROM_INT(2) },
    
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&file_close_obj)},
    { MP_ROM_QSTR(MP_QSTR_tell), MP_ROM_PTR(&file_tell_obj)},
    { MP_ROM_QSTR(MP_QSTR_seek), MP_ROM_PTR(&file_seek_obj)},
    { MP_ROM_QSTR(MP_QSTR_seekable), MP_ROM_PTR(&file_seekable_obj)},
};

STATIC MP_DEFINE_CONST_DICT(file_type_globals, file_type_globals_table);

extern const mp_obj_type_t file_type = {
    { &mp_type_type },  // base
    0,                  // flags
    MP_QSTR_file,       // name
    file_print,         // __repr__, __srt__
    file_make_new,      // __new__, __init__
    nullptr,            // __call__
    nullptr,            // unary operations
    nullptr,            // binary operations
    nullptr,            // load, store, delete attributes
    nullptr,            // load, store, delete subscripting
    nullptr,            // __iter__  -> TODO!
    nullptr,            // __next__
    nullptr,            // buffer
    nullptr,            // protocol
    nullptr,            // parent
    (mp_obj_dict_t*) &file_type_globals   // globals table
};

typedef enum _file_location_t {
    RAM = 0, FLASH = 1
} file_location_t;

typedef enum _file_mode_t {
    READ = 0, WRITE = 1, APPEND = 2, CREATE = 3
} file_mode_t;

typedef enum _file_bin_t {
    TEXT = 0, BINARY = 1
} file_bin_t;


typedef struct _file_obj_t {
    mp_obj_base_t base;
    
    mp_obj_t name;
    
    file_mode_t     open_mode;
    bool            edit_mode;
    file_bin_t      binary_mode;
    
    file_location_t location;
    // If location is set to RAM, record is used.
    Ion::Storage::Record record;
    
    uint16_t position;
    
    bool closed;
    
} file_obj_t;

STATIC void file_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    file_obj_t *self = (file_obj_t*) MP_OBJ_TO_PTR(self_in);
    
    size_t l;
    const char* file_name = mp_obj_str_get_data(self->name, &l);
    
    uint8_t offset = 0;
    char file_mode[4] = {0, 0, 0, 0};
    
    switch(self->open_mode) {
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
    
    if (self->edit_mode) {
        file_mode[1] = '+';
        offset = 1;
    }
    
    switch(self->binary_mode) {
        case TEXT:
            file_mode[1 + offset] = 't';
            break;
        case BINARY:
            file_mode[1 + offset] = 'b';
            break;
    }
    
    
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
 * File system organisation:
 *  - /ram/
 *     Contains all files in RAM (python, poincare stuff, etc..)
 *  - /flash/
 *     Contains all files in external's TAR archive
 *
 *  When nme doesn't start with "/", we use RAM.
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
    
    // If "", throw file not found
    if (l > 0) {
        file->location = RAM;
        // Check location (RAM/FLASH)
        if (file_name[0] == '/') {
            if (strncmp(file_name, "/ram/", 5) == 0) {
                file->location = RAM;
                file_name = file_name + 5;
            // } else if (strncmp(file_name, "/flash/", 7)) {
            //     file->location = FLASH;
            //     file_name = file_name + 7;
            } else {
                mp_raise_OSError(2);
            }
        }
    } else {
        mp_raise_OSError(2);
    }
    
    if (!Ion::Storage::FullNameCompliant(file_name)) {
        mp_raise_OSError(22);
    }
    
    if(file->location == RAM) {
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
                file->record = Ion::Storage::sharedStorage()->recordNamed(file_name);
                file->position = 0;
                if (file->record == Ion::Storage::Record()) {
                    status = Ion::Storage::sharedStorage()->createRecordWithFullName(file_name, "", 0);
                    if (status == Ion::Storage::Record::ErrorStatus::NotEnoughSpaceAvailable) {
                        mp_raise_OSError(28);
                    }
                }
                file->position = file->record.value().size;
                break;
        }
    } else {
        mp_raise_OSError(2);
    }
    
    file->base.type = &file_type;
    return MP_OBJ_FROM_PTR(file);
}

void check_closed(file_obj_t* file) {
    if (file->closed)
        mp_raise_ValueError("I/O operation on closed file");
}


// Methods

STATIC mp_obj_t file_close(mp_obj_t o_in) {
    if(!mp_obj_is_type(o_in, &file_type)) {
        mp_raise_TypeError("self must be a file!");
    }
    
    file_obj_t* file = (file_obj_t*) MP_OBJ_TO_PTR(o_in);
    
    if (!file->closed) {
        if(file->location == RAM) {
            file->record = Ion::Storage::Record();
        }
    
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
    
    mp_int_t position = mp_obj_get_int(args[1]);
    mp_int_t whence = 0;
    
    if (n_args > 2) {
        if (!mp_obj_is_integer(args[2])) {
            mp_raise_ValueError("whence must be an int!");
        }
        
        whence = mp_obj_get_int(args[2]);
    }
    
    mp_int_t new_position = file->position;
    size_t file_size = 0;
    
    if(file->location == RAM) {
        file_size = file->record.value().size;
    }
        
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

