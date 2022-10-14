extern "C" {
#include "../include/eadk/eadk.h"
}

static uint32_t trampolineFunctionsOrigin() {
  static uint32_t origin = 0;
  if (origin == 0) {
    void * programCounter;
    asm volatile ("mov %[pc], pc" : [pc] "=r" (programCounter) :);
    // Find the running slot from the PC
    uint32_t slotStart = (uint32_t)programCounter & (~(0x400000 - 1));
    // Find the trampoline address start
    uint32_t kernelSize = 0x10000;
    uint32_t userlandHeaderSize = 4 + 8 + 4 + 4 + 4 + 4 + 4 + 4 + 4;
    uint32_t isrOffset = 4 + 4;
    origin = slotStart + kernelSize + userlandHeaderSize + isrOffset;
  }
  return origin;
}

static inline uint32_t trampolineFunctionAddress(int index) {
  return trampolineFunctionsOrigin() + index * 4;
}

void eadk_display_draw_string(const char * text, eadk_point_t point, bool large_font, eadk_color_t text_color, eadk_color_t background_color) {
  typedef void (*Trampoline)(const char *, eadk_point_t, bool, eadk_color_t, eadk_color_t);
  Trampoline trampoline = (Trampoline)trampolineFunctionAddress(0);
  return trampoline(text, point, large_font, text_color, background_color);
}
