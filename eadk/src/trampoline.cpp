extern "C" {
#include "../include/eadk/eadk.h"
}

extern "C" {
extern char _userland_trampoline_address;
}

static uint32_t trampolineFunctionAddress(int index) {
  static uint32_t origin = reinterpret_cast<uint32_t>(&_userland_trampoline_address);
  return origin + index * 4;
}

void eadk_display_draw_string(const char * text, eadk_point_t point, bool large_font, eadk_color_t text_color, eadk_color_t background_color) {
  typedef void (*Trampoline)(const char *, eadk_point_t, bool, eadk_color_t, eadk_color_t);
  Trampoline * trampoline = (Trampoline *)trampolineFunctionAddress(0);
  return (*trampoline)(text, point, large_font, text_color, background_color);
}
