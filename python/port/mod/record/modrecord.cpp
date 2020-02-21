extern "C" {
    #include "modrecord.h"
    #include <py/objtuple.h>
    #include <py/runtime.h>
}
#include <ion.h>
#include "port.h"

#define extension "py"

Ion::Storage::Record GetRecordFromName(const char* name){
    return Ion::Storage::sharedStorage()->recordBaseNamedWithExtension(name, extension);
}

mp_obj_t modrecord_istaken(mp_obj_t name){
    if(Ion::Storage::sharedStorage()->hasRecord(Ion::Storage::Record(mp_obj_str_get_str(name), extension))){
        return mp_const_true;
    } else {
        return mp_const_false;
    }
}

mp_obj_t modrecord_save(mp_obj_t name, mp_obj_t data){
    // 
    if(Ion::Storage::sharedStorage()->createRecordWithExtension(mp_obj_str_get_str(name),extension, mp_obj_str_get_str(data), sizeof(mp_obj_str_get_str(data))) == Ion::Storage::Record::ErrorStatus::None){
        return mp_const_true;
    } else {
        return mp_const_false;
    }
}

mp_obj_t modrecord_getdata(mp_obj_t name){
    Ion::Storage::Record r = GetRecordFromName(mp_obj_str_get_str(name));
    if(r != NULL){
        Ion::Storage::Record::Data d = r.value();
        const char* content = (const char *)d.buffer;
        return mp_obj_new_str_via_qstr(content, sizeof(content));   
    } else {
        return mp_const_none;
    }
}

mp_obj_t modrecord_destroy(mp_obj_t name){
    Ion::Storage::Record r = GetRecordFromName(mp_obj_str_get_str(name));
    if(r != NULL){
        r.destroy();
        return mp_const_true;
    } else {
        return mp_const_false;
    }
}

mp_obj_t modrecord_listrecords(){
    uint8_t NRecords =Ion::Storage::sharedStorage()->numberOfRecordsWithExtension(extension);
    mp_obj_tuple_t * t = static_cast<mp_obj_tuple_t *>(MP_OBJ_TO_PTR(mp_obj_new_tuple(NRecords, NULL)));
    for(int i = NRecords - 1; i>=0; i--){
        const char* name = Ion::Storage::sharedStorage()->recordWithExtensionAtIndex(extension, i).fullName();
        t->items[i] = mp_obj_new_str(name, sizeof(name));
    }
    return t;
}