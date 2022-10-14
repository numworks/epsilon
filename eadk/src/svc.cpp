extern "C" {
#include "../include/eadk/eadk.h"
}

// TODO: No namespace when just defining macros
#include "ion/src/device/shared/drivers/svcall.h"
#include "ion/src/device/userland/drivers/svcall.h"

void SVC_ATTRIBUTES eadk_backlight_set_brightness(uint8_t brightness) {
  SVC_RETURNING_VOID(SVC_BACKLIGHT_SET_BRIGHTNESS)
}

uint8_t SVC_ATTRIBUTES eadk_backlight_brightness() {
  SVC_RETURNING_R0(SVC_BACKLIGHT_BRIGHTNESS, uint8_t)
}

uint32_t SVC_ATTRIBUTES eadk_random() {
  SVC_RETURNING_R0(SVC_RANDOM, uint32_t)
}

// Display

void SVC_ATTRIBUTES eadk_display_push_rect(eadk_rect_t rect, const eadk_color_t * pixels) {
  SVC_RETURNING_VOID(SVC_DISPLAY_PUSH_RECT)
}

void SVC_ATTRIBUTES eadk_display_push_rect_uniform(eadk_rect_t rect, eadk_color_t color) {
  SVC_RETURNING_VOID(SVC_DISPLAY_PUSH_RECT_UNIFORM)
}

void SVC_ATTRIBUTES eadk_display_pull_rect(eadk_rect_t rect, eadk_color_t * pixels) {
  SVC_RETURNING_VOID(SVC_DISPLAY_PULL_RECT)
}

bool SVC_ATTRIBUTES eadk_display_wait_for_vblank() {
  SVC_RETURNING_R0(SVC_DISPLAY_WAIT_FOR_V_BLANK, bool)
}

// Keyboard
eadk_keyboard_state_t SVC_ATTRIBUTES eadk_keyboard_scan() {
  SVC_RETURNING_R0R1(SVC_KEYBOARD_SCAN, eadk_keyboard_state_t)
}

eadk_keyboard_state_t SVC_ATTRIBUTES eadk_keyboard_pop_state() {
  SVC_RETURNING_R0R1(SVC_KEYBOARD_POP_STATE, eadk_keyboard_state_t)
}

// Timing

void SVC_ATTRIBUTES eadk_timing_usleep(uint32_t us) {
  SVC_RETURNING_VOID(SVC_TIMING_USLEEP)
}

void SVC_ATTRIBUTES eadk_timing_msleep(uint32_t ms) {
  SVC_RETURNING_VOID(SVC_TIMING_MSLEEP)
}

uint64_t SVC_ATTRIBUTES eadk_timing_millis() {
  SVC_RETURNING_R0R1(SVC_TIMING_MILLIS, uint64_t)
}
