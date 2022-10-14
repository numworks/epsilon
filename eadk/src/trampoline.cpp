extern "C" {
#include "../include/eadk/eadk.h"
}

#if 0

static inline uint32_t trampolines() {
  void * programCounter;
  asm volatile ("mov %[pc], pc" : [pc] "=r" (programCounter) :);
  // Find the running slot from the PC
  uint32_t slotStart = (uint32_t)programCounter & (~(0x400000 - 1));
  // Find the trampoline address start
  uint32_t kernelSize = 0x10000;
  uint32_t userlandHeaderSize = 4 + 8 + 4 + 4 + 4 + 4 + 4 + 4 + 4;
  uint32_t isrOffset = 4 + 4;
  uint32_t trampolineFunctionStart = slotStart + kernelSize + userlandHeaderSize + isrOffset;
  return trampolineFunctionStart;
}

static inline uint32_t trampolineFunction(int index) {
  static void * trampolines = trampolines();
  return trampolines + index*4;
}

/*
void eadk_display_draw_string(const char * text, eadk_point_t point, bool large_font, eadk_color_t text_color, eadk_color_t background_color) {
}
*/
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define TRAMPOLINE_DISPLAY_DRAW_STRING 0

#define TRAMPOLINE_INTERFACE(index, function, argsList, returnType, args...) \
  typedef returnType (*FunctionType##_##function)(args); \
  returnType function(args) { \
    FunctionType##_##function * trampolineFunction = (FunctionType##_##function *)(addressOfFunction(index)); \
    return (*trampolineFunction)argsList; \
  } \

static inline uint32_t addressOfFunction(int index) {
  void * programCounter;
  asm volatile ("mov %[pc], pc" : [pc] "=r" (programCounter) :);
  // Find the running slot from the PC
  uint32_t slotStart = (uint32_t)programCounter & (~(0x400000 - 1));
  // Find the trampoline address start
  uint32_t kernelSize = 0x10000;
  uint32_t userlandHeaderSize = 4 + 8 + 4 + 4 + 4 + 4 + 4 + 4 + 4;
  uint32_t isrOffset = 4 + 4;
  uint32_t trampolineFunctionStart = slotStart + kernelSize + userlandHeaderSize + isrOffset;
  return trampolineFunctionStart + index * 4;

}

typedef uint16_t eadk_color;
typedef struct {
  uint16_t x;
  uint16_t y;
} eadk_point;

TRAMPOLINE_INTERFACE(TRAMPOLINE_DISPLAY_DRAW_STRING, eadk_display_draw_string, (text, point, large_font, text_color, background_color), void, const char * text, eadk_point_t point, bool large_font, eadk_color_t text_color, eadk_color_t background_color)
