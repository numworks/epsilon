#ifndef ION_DEVICE_SHARED_DRIVERS_SVCALL_H
#define ION_DEVICE_SHARED_DRIVERS_SVCALL_H

namespace Ion {
namespace Device {
namespace SVCall {

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

#define SVC_AUTHENTICATION_CLEARANCE_LEVEL 0
#define SVC_BACKLIGHT_BRIGHTNESS 1
#define SVC_BACKLIGHT_SET_BRIGHTNESS 2
#define SVC_BATTERY_IS_CHARGING 3
#define SVC_BATTERY_LEVEL 4
#define SVC_BATTERY_VOLTAGE 5
#define SVC_BOARD_ENABLE_EXTERNAL_APPS 6
#define SVC_BOARD_SWITCH_EXECUTABLE_SLOT 7
#define SVC_CIRCUIT_BREAKER_HAS_CHECKPOINT 8
#define SVC_CIRCUIT_BREAKER_LOAD_CHECKPOINT 9
#define SVC_CIRCUIT_BREAKER_LOCK 10
#define SVC_CIRCUIT_BREAKER_SET_CHECKPOINT 11
#define SVC_CIRCUIT_BREAKER_STATUS 12
#define SVC_CIRCUIT_BREAKER_UNLOCK 13
#define SVC_CIRCUIT_BREAKER_UNSET_CHECKPOINT 14
#define SVC_CRC32_BYTE 15
#define SVC_CRC32_WORD 16
#define SVC_DISPLAY_POST_PUSH_MULTICOLOR 17
#define SVC_DISPLAY_PULL_RECT 18
#define SVC_DISPLAY_PUSH_RECT 19
#define SVC_DISPLAY_PUSH_RECT_UNIFORM 20
#define SVC_DISPLAY_WAIT_FOR_V_BLANK 21
#define SVC_EVENTS_COPY_TEXT 22
#define SVC_EVENTS_GET_EVENT 23
#define SVC_EVENTS_IS_DEFINED 24
#define SVC_EVENTS_REPETITION_FACTOR 25
#define SVC_EVENTS_SET_SHIFT_ALPHA_STATUS 26
#define SVC_EVENTS_SET_SPINNER 27
#define SVC_EVENTS_SHIFT_ALPHA_STATUS 28
#define SVC_FCC_ID 29
#define SVC_FLASH_ERASE_SECTOR_WITH_INTERRUPTIONS 30
#define SVC_FLASH_MASS_ERASE_WITH_INTERRUPTIONS 31
#define SVC_FLASH_WRITE_MEMORY_WITH_INTERRUPTIONS 32
#define SVC_KEYBOARD_POP_STATE 33
#define SVC_KEYBOARD_SCAN 34
#define SVC_LED_GET_COLOR 35
#define SVC_LED_SET_BLINKING 36
#define SVC_LED_SET_COLOR 37
#define SVC_LED_UPDATE_COLOR_WITH_PLUG_AND_CHARGE 38
#define SVC_PCB_VERSION 39
#define SVC_PERSISTING_BYTES_READ 40
#define SVC_PERSISTING_BYTES_WRITE 41
#define SVC_POWER_SELECT_STANDBY_MODE 42
#define SVC_POWER_STANDBY 43
#define SVC_POWER_SUSPEND 44
#define SVC_RANDOM 45
#define SVC_RESET_CORE 46
#define SVC_SERIAL_NUMBER_COPY 47
#define SVC_TIMING_MILLIS 48
#define SVC_TIMING_MSLEEP 49
#define SVC_TIMING_USLEEP 50
#define SVC_USB_DID_EXECUTE_DFU 51
#define SVC_USB_IS_PLUGGED 52
#define SVC_USB_SHOULD_INTERRUPT 53
#define SVC_USB_WILL_EXECUTE_DFU 54
#define SVC_EVENTS_LONG_PRESS_COUNTER 55

#define SVC_NUMBER_OF_CALLS 56

}
}
}

#endif
