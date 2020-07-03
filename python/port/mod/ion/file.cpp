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

extern const mp_obj_type_t file_type = {
    { &mp_type_type },
    0,
    MP_QSTR_file,
    file_print,
    file_make_new,
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
    mp_print_str(print, "'>");
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
        mp_raise_ValueError("First argument must be a string!");
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
            mp_raise_ValueError("Second argument must be a string!");
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
    }
    
    // mp_raise_FileNotFoundError("Test");
    
    
    file->base.type = &file_type;
    return MP_OBJ_FROM_PTR(file);
}



