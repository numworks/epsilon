#include <stdbool.h>
#include <stdint.h>

#define SVC_ATTRIBUTES __attribute__((noinline,externally_visible))

#define SVC_RETURNING_VOID(code) \
  asm volatile ("svc %[immediate]" \
    : : [immediate] "I" (code));

#define SVC_RETURNING_SINGLE_REGISTER(code, returnType, instruction) \
  returnType returnValue; \
  asm volatile ( \
      instruction \
      : [returnValue] "=r" (returnValue) \
      : [immediate] "I" (code) \
      : "r0", "r1", "r2", "r3"); \
  return returnValue;

#define SVC_RETURNING_R0(code, returnType) SVC_RETURNING_SINGLE_REGISTER(code, returnType, "svc %[immediate] ; mov %[returnValue], r0")

#define SVC_RETURNING_S0(code, returnType) SVC_RETURNING_SINGLE_REGISTER(code, returnType, "svc %[immediate] ; vmov %[returnValue], s0")

#define SVC_RETURNING_MULTIPLE_REGISTERS(code, returnType, instruction) \
  returnType returnValue = 0; \
  returnType * address = &returnValue; \
  asm volatile ( \
      instruction \
      : "=g" (address)  \
      : [immediate] "I" (code), [returnValueAddress] "r" (address) \
      : "r0", "r1", "r2", "r3"); \
  return *address;

#define SVC_RETURNING_R0R1(code, returnType) SVC_RETURNING_MULTIPLE_REGISTERS(code, returnType,  "svc %[immediate] ; str r0, [%[returnValueAddress]] ; str r1, [%[returnValueAddress],#4]")

#define SVC_RETURNING_STASH_ADDRESS_IN_R0(code, returnType) SVC_RETURNING_MULTIPLE_REGISTERS(code, returnType, "mov r0, %[returnValueAddress] ; svc %[immediate]")

#define SVC_BACKLIGHT_BRIGHTNESS 1
#define SVC_BACKLIGHT_SET_BRIGHTNESS 2
#define SVC_BATTERY_IS_CHARGING 3
#define SVC_BATTERY_LEVEL 4
#define SVC_BATTERY_VOLTAGE 5
#define SVC_DISPLAY_PULL_RECT 18
#define SVC_DISPLAY_PUSH_RECT 19
#define SVC_DISPLAY_PUSH_RECT_UNIFORM 20
#define SVC_DISPLAY_WAIT_FOR_V_BLANK 21
#define SVC_KEYBOARD_POP_STATE 33
#define SVC_KEYBOARD_SCAN 34
#define SVC_RANDOM 45
#define SVC_TIMING_MILLIS 48
#define SVC_TIMING_MSLEEP 49
#define SVC_TIMING_USLEEP 50
#define SVC_USB_IS_PLUGGED 52

void SVC_ATTRIBUTES eadk_backlight_set_brightness(uint8_t b) {
  SVC_RETURNING_VOID(SVC_BACKLIGHT_SET_BRIGHTNESS)
}

uint8_t SVC_ATTRIBUTES eadk_backlight_brightness() {
  SVC_RETURNING_R0(SVC_BACKLIGHT_BRIGHTNESS, uint8_t)
}

bool SVC_ATTRIBUTES eadk_battery_is_charging() {
  SVC_RETURNING_R0(SVC_BATTERY_IS_CHARGING, bool)
}

uint8_t SVC_ATTRIBUTES eadk_battery_level() {
  SVC_RETURNING_R0(SVC_BATTERY_LEVEL, uint8_t)
}

float SVC_ATTRIBUTES eadk_battery_voltage() {
  SVC_RETURNING_S0(SVC_BATTERY_VOLTAGE, float)
}

typedef uint16_t eadk_color;
typedef struct {
  uint16_t x;
  uint16_t y;
  uint16_t width;
  uint16_t height;
} eadk_rect;

void SVC_ATTRIBUTES eadk_display_push_rect(eadk_rect rect, const eadk_color * pixels) {
  SVC_RETURNING_VOID(SVC_DISPLAY_PUSH_RECT)
}

void SVC_ATTRIBUTES eadk_display_push_rect_uniform(eadk_rect rect, eadk_color color) {
  SVC_RETURNING_VOID(SVC_DISPLAY_PUSH_RECT_UNIFORM)
}

void SVC_ATTRIBUTES eadk_display_pull_rect(eadk_rect rect, eadk_color * pixels) {
  SVC_RETURNING_VOID(SVC_DISPLAY_PULL_RECT)
}

bool SVC_ATTRIBUTES eadk_display_wait_for_vblank() {
  SVC_RETURNING_R0(SVC_DISPLAY_WAIT_FOR_V_BLANK, bool)
}

typedef uint64_t eadk_keyboard_state;

eadk_keyboard_state SVC_ATTRIBUTES eadk_keyboard_scan() {
  SVC_RETURNING_R0R1(SVC_KEYBOARD_SCAN, eadk_keyboard_state)
}

eadk_keyboard_state SVC_ATTRIBUTES eadk_keyboard_pop_state() {
  SVC_RETURNING_R0R1(SVC_KEYBOARD_POP_STATE, eadk_keyboard_state)
}

void SVC_ATTRIBUTES eadk_timing_usleep(uint32_t us) {
  SVC_RETURNING_VOID(SVC_TIMING_USLEEP)
}

void SVC_ATTRIBUTES eadk_timing_msleep(uint32_t ms) {
  SVC_RETURNING_VOID(SVC_TIMING_MSLEEP)
}

uint64_t SVC_ATTRIBUTES eadk_timing_millis() {
  SVC_RETURNING_R0R1(SVC_TIMING_MILLIS, uint64_t)
}


bool SVC_ATTRIBUTES eadk_usb_is_plugged() {
  SVC_RETURNING_R0(SVC_USB_IS_PLUGGED, bool)
}

uint32_t SVC_ATTRIBUTES eadk_random() {
  SVC_RETURNING_R0(SVC_RANDOM, uint32_t)
}
