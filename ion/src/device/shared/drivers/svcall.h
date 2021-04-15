#ifndef ION_DEVICE_SHARED_DRIVERS_SVCALL_H
#define ION_DEVICE_SHARED_DRIVERS_SVCALL_H

namespace Ion {
namespace Device {
namespace SVCall {

#define ASM(code, returnType, instruction) \
  returnType returnValue; \
  asm volatile (instruction \
    : [returnValue] "=r" (returnValue) \
    : [immediate] "I" (code) \
    : "r0", "r1", "r2", "r3"); \
  return returnValue;

#define SVC_RETURNING_VOID(code) \
  asm volatile ("svc %[immediate]" \
    : : [immediate] "I" (code));

#define SVC_RETURNING_R0(code, returnType) ASM(code, returnType, "svc %[immediate] ; mov %[returnValue], r0")

#define SVC_RETURNING_S0(code, returnType) ASM(code, returnType, "svc %[immediate] ; vmov %[returnValue], s0")

#define SVC_RETURNING_R0R1(code, returnType) \
  returnType returnValue; \
  returnType * address = &returnValue; \
  asm volatile ( \
      "svc %[immediate] \n" \
      "str r0, [%[returnValueAddress]] \n" \
      "str r1, [%[returnValueAddress],#4] \n" \
    : : [immediate] "I" (code), [returnValueAddress] "r" (address) \
    : "r0", "r1", "r2", "r3"); \
  return returnValue;

#define SVC_RETURNING_STASH_ADDRESS_IN_R0(code, returnType) \
  returnType returnValue; \
  returnType * address = &returnValue; \
  asm volatile ( \
      "mov r0, %[returnValueAddress] \n" \
      "svc %[immediate] \n" \
    : : [immediate] "I" (code), [returnValueAddress] "r" (address) \
    : "r0", "r1", "r2", "r3"); \
  return returnValue;

#define SVC_BACKLIGHT_BRIGHTNESS 0
#define SVC_BACKLIGHT_INIT 1
#define SVC_BACKLIGHT_IS_INITIALIZED 2
#define SVC_BACKLIGHT_SET_BRIGHTNESS 3
#define SVC_BACKLIGHT_SHUTDOWN 4
#define SVC_BATTERY_IS_CHARGING 5
#define SVC_BATTERY_LEVEL 6
#define SVC_BATTERY_VOLTAGE 7
#define SVC_BOARD_SWITCH_EXECUTABLE_SLOT 8
#define SVC_CIRCUIT_BREAKER_HAS_CHECKPOINT 9
#define SVC_CIRCUIT_BREAKER_LOAD_CHECKPOINT 10
#define SVC_CIRCUIT_BREAKER_SET_CHECKPOINT 11
#define SVC_CIRCUIT_BREAKER_STATUS 12
#define SVC_CIRCUIT_BREAKER_UNSET_CHECKPOINT 13
#define SVC_CRC32_BYTE 14
#define SVC_CRC32_WORD 15
#define SVC_DISPLAY_COLORED_TILING_SIZE_10 16
#define SVC_DISPLAY_POST_PUSH_MULTICOLOR 17
#define SVC_DISPLAY_PULL_RECT 18
#define SVC_DISPLAY_PUSH_RECT 19
#define SVC_DISPLAY_PUSH_RECT_UNIFORM 20
#define SVC_DISPLAY_UNIFORM_TILING_SIZE_10 21
#define SVC_DISPLAY_WAIT_FOR_V_BLANK 22
#define SVC_EVENTS_COPY_TEXT 23
#define SVC_EVENTS_GET_EVENT 24
#define SVC_EVENTS_IS_DEFINED 25
#define SVC_EVENTS_REPETITION_FACTOR 26
#define SVC_EVENTS_SET_SHIFT_ALPHA_STATUS 27
#define SVC_EVENTS_SHIFT_ALPHA_STATUS 28
#define SVC_FCC_ID 29
#define SVC_FLASH_ERASE_SECTOR 30
#define SVC_FLASH_MASS_ERASE 31
#define SVC_FLASH_WRITE_MEMORY 32
#define SVC_KEYBOARD_SCAN 33
#define SVC_LED_GET_COLOR 34
#define SVC_LED_SET_BLINKING 35
#define SVC_LED_SET_COLOR 36
#define SVC_LED_UPDATE_COLOR_WITH_PLUG_AND_CHARGE 37
#define SVC_PERSISTING_BYTES_READ 38
#define SVC_PERSISTING_BYTES_WRITE 39
#define SVC_POWER_SELECT_STANDBY_MODE 40
#define SVC_POWER_STANDBY 41
#define SVC_POWER_SUSPEND 42
#define SVC_RANDOM 43
#define SVC_RESET_CORE 44
#define SVC_SERIAL_NUMBER_COPY 45
#define SVC_TIMING_MILLIS 46
#define SVC_TIMING_MSLEEP 47
#define SVC_TIMING_USLEEP 48
#define SVC_USB_DID_EXECUTE_DFU 49
#define SVC_USB_IS_PLUGGED 50
#define SVC_USB_SHOULD_INTERRUPT 51
#define SVC_USB_WILL_EXECUTE_DFU 52

#define SVC_NUMBER_OF_CALLS 53

}
}
}

#endif
