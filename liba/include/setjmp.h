#ifndef LIBA_SETJMP_H
#define LIBA_SETJMP_H

#include <stdint.h>

#include "private/macros.h"

/* We are preseving registers:
 * - sp & lr -> 2x4 bytes
 * - General purpose registers: r4-r9, r10 = sl, r11 = fp -> 8x4 bytes
 * - Floating point registers: s16-s31 -> 8x8 bytes
 * - VFP status register: fpscr ->1x4 bytes
 * The buffer size has to include room for setjmp implementation: 4x4 bytes.
 * (See C Library ABI for the ARM architecture documentation)
 * The minimum buffer size is then (2+8+16+1+4)xsizeof(int64_t). */

LIBA_BEGIN_DECLS

typedef uintptr_t jmp_buf[31];
void longjmp(jmp_buf env, int val);
int setjmp(jmp_buf env);

LIBA_END_DECLS

#endif
