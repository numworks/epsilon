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
#define SVC_CIRCUIT_BREAKER_SET_CHECKPOINT 10
#define SVC_CIRCUIT_BREAKER_STATUS 11
#define SVC_CIRCUIT_BREAKER_UNSET_CHECKPOINT 12
#define SVC_CRC32_BYTE 13
#define SVC_CRC32_WORD 14
#define SVC_DISPLAY_POST_PUSH_MULTICOLOR 15
#define SVC_DISPLAY_PULL_RECT 16
#define SVC_DISPLAY_PUSH_RECT 17
#define SVC_DISPLAY_PUSH_RECT_UNIFORM 18
#define SVC_DISPLAY_WAIT_FOR_V_BLANK 19
#define SVC_EVENTS_COPY_TEXT 20
#define SVC_EVENTS_GET_EVENT 21
#define SVC_EVENTS_IS_DEFINED 22
#define SVC_EVENTS_REPETITION_FACTOR 23
#define SVC_EVENTS_SET_SHIFT_ALPHA_STATUS 24
#define SVC_EVENTS_SET_SPINNER 25
#define SVC_EVENTS_SHIFT_ALPHA_STATUS 26
#define SVC_FCC_ID 27
#define SVC_FLASH_ERASE_SECTOR 28
#define SVC_FLASH_MASS_ERASE 29
#define SVC_FLASH_WRITE_MEMORY 30
#define SVC_KEYBOARD_POP_STATE 31
#define SVC_KEYBOARD_SCAN 32
#define SVC_LED_GET_COLOR 33
#define SVC_LED_SET_BLINKING 34
#define SVC_LED_SET_COLOR 35
#define SVC_LED_UPDATE_COLOR_WITH_PLUG_AND_CHARGE 36
#define SVC_PCB_VERSION 37
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
