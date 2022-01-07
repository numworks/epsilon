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
  uint32_t userlandHeaderSize = 4 + 8 + 4 + 4 + 4 + 4 + 4;
  uint32_t isrOffset = 4 + 4;
  uint32_t trampolineFunctionStart = slotStart + kernelSize + userlandHeaderSize + isrOffset;
  return trampolineFunctionStart + index * 4;

}

typedef uint16_t eadk_color;
typedef struct {
  uint16_t x;
  uint16_t y;
} eadk_point;

TRAMPOLINE_INTERFACE(TRAMPOLINE_DISPLAY_DRAW_STRING, draw_string, (text, point, largeFont, textColor, backgroundColor), void, const char * text, eadk_point point, bool largeFont, eadk_color textColor, eadk_color backgroundColor)

