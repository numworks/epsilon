#ifndef ION_DEVICE_SHARED_DRIVERS_TRAMPOLINE_H
#define ION_DEVICE_SHARED_DRIVERS_TRAMPOLINE_H

#include <stdint.h>

namespace Ion {
namespace Device {
namespace Trampoline {

#define TRAMPOLINE_ATTRIBUTES __attribute__((externally_visible))

#define TRAMPOLINE_SUSPEND 0
#define TRAMPOLINE_EXTERNAL_FLASH_ERASE_SECTOR (1+TRAMPOLINE_SUSPEND)
#define TRAMPOLINE_EXTERNAL_FLASH_WRITE_MEMORY (1+TRAMPOLINE_EXTERNAL_FLASH_ERASE_SECTOR)
#define TRAMPOLINE_MEMCMP (1+TRAMPOLINE_EXTERNAL_FLASH_WRITE_MEMORY)
#define TRAMPOLINE_MEMCPY (1+TRAMPOLINE_MEMCMP)
#define TRAMPOLINE_MEMMOVE (1+TRAMPOLINE_MEMCPY)
#define TRAMPOLINE_MEMSET (1+TRAMPOLINE_MEMMOVE)
#define TRAMPOLINE_STRCHR (1+TRAMPOLINE_MEMSET)
#define TRAMPOLINE_STRCMP (1+TRAMPOLINE_STRCHR)
#define TRAMPOLINE_STRLCAT (1+TRAMPOLINE_STRCMP)
#define TRAMPOLINE_STRLCPY (1+TRAMPOLINE_STRLCAT)
#define TRAMPOLINE_STRLEN (1+TRAMPOLINE_STRLCPY)
#define TRAMPOLINE_STRNCMP (1+TRAMPOLINE_STRLEN)
#define NUMBER_OF_TRAMPOLINE_FUNCTIONS (TRAMPOLINE_STRNCMP+1)

#define TRAMPOLINE_INTERFACE(index, function, argsList, returnType, args...) \
  typedef returnType (*FunctionType##_##function)(args); \
  returnType function(args) { \
    FunctionType##_##function * trampolineFunction = reinterpret_cast<FunctionType##_##function *>(Ion::Device::Trampoline::addressOfFunction(index)); \
    return (*trampolineFunction)argsList; \
  } \

uint32_t addressOfFunction(int index);

}
}
}

#endif
