// x64 specific stuff

#include "py/mpconfig.h"

#if MICROPY_EMIT_X64

// This is defined so that the assembler exports generic assembler API macros
#define GENERIC_ASM_API (1)
#include "py/asmx64.h"

#define N_X64 (1)
#define EXPORT_FUN(name) emit_native_x64_##name
#include "py/emitnative.c"

#endif
