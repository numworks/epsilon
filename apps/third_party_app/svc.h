#ifndef EXTERNAL_APPS_SVCALL_H
#define EXTERNAL_APPS_SVCALL_H

#include "color.h"
#include "keyboard.h"
#include "rect.h"
#include "stddef.h"
#include "stdint.h"

// Usefull macros for SVC calls instruction

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
      : : [immediate] "I" (code), [returnValueAddress] "r" (address) \
      : "r0", "r1", "r2", "r3"); \
  return returnValue;

#define SVC_RETURNING_R0R1(code, returnType) SVC_RETURNING_MULTIPLE_REGISTERS(code, returnType,  "svc %[immediate] ; str r0, [%[returnValueAddress]] ; str r1, [%[returnValueAddress],#4]")

#define SVC_RETURNING_STASH_ADDRESS_IN_R0(code, returnType) SVC_RETURNING_MULTIPLE_REGISTERS(code, returnType, "mov r0, %[returnValueAddress] ; svc %[immediate]")

#define SVC_BACKLIGHT_BRIGHTNESS 1
#define SVC_BACKLIGHT_INIT 2
#define SVC_BACKLIGHT_IS_INITIALIZED 3
#define SVC_BACKLIGHT_SET_BRIGHTNESS 4
#define SVC_BACKLIGHT_SHUTDOWN 5
#define SVC_BATTERY_IS_CHARGING 6
#define SVC_BATTERY_LEVEL 7
#define SVC_BATTERY_VOLTAGE 8
#define SVC_CIRCUIT_BREAKER_HAS_CHECKPOINT 11
#define SVC_CIRCUIT_BREAKER_LOAD_CHECKPOINT 12
#define SVC_CIRCUIT_BREAKER_SET_CHECKPOINT 13
#define SVC_CIRCUIT_BREAKER_STATUS 14
#define SVC_CIRCUIT_BREAKER_UNSET_CHECKPOINT 15
#define SVC_CRC32_BYTE 16
#define SVC_CRC32_WORD 17
#define SVC_DISPLAY_COLORED_TILING_SIZE_10 18
#define SVC_DISPLAY_POST_PUSH_MULTICOLOR 19
#define SVC_DISPLAY_PULL_RECT 20
#define SVC_DISPLAY_PUSH_RECT 21
#define SVC_DISPLAY_PUSH_RECT_UNIFORM 22
#define SVC_DISPLAY_UNIFORM_TILING_SIZE_10 23
#define SVC_DISPLAY_WAIT_FOR_V_BLANK 24
#define SVC_EVENTS_COPY_TEXT 25
#define SVC_EVENTS_GET_EVENT 26
#define SVC_EVENTS_IS_DEFINED 27
#define SVC_EVENTS_REPETITION_FACTOR 28
#define SVC_EVENTS_SET_SHIFT_ALPHA_STATUS 29
#define SVC_EVENTS_SET_SPINNER 30
#define SVC_EVENTS_SHIFT_ALPHA_STATUS 31
#define SVC_FCC_ID 32
#define SVC_KEYBOARD_SCAN 36
#define SVC_POWER_SELECT_STANDBY_MODE 44
#define SVC_POWER_STANDBY 45
#define SVC_POWER_SUSPEND 46
#define SVC_RANDOM 47
#define SVC_RESET_CORE 48
#define SVC_SERIAL_NUMBER_COPY 49
#define SVC_TIMING_MILLIS 50
#define SVC_TIMING_MSLEEP 51
#define SVC_TIMING_USLEEP 52
#define SVC_USB_IS_PLUGGED 54
#define SVC_USB_SHOULD_INTERRUPT 55

void abort();

namespace Ion {

namespace Backlight {

void init();
void shutdown();
bool isInitialized();
void setBrightness(uint8_t b);
uint8_t brightness();

}

namespace Battery {

enum class Charge : uint8_t {
  EMPTY = 0,
  LOW = 1,
  SOMEWHERE_INBETWEEN = 2,
  FULL = 3
};


bool isCharging();
Charge level();
float voltage();

}

namespace CircuitBreaker {

enum class CheckpointType : uint8_t {
  Home, // Checkpoint where the code jumps on Home Event
  User, // Checkpoint where the code jumps on Back Event if the device is stalling
  System // Checkpoint used by system to try different paths at run-time
};

enum class Status : uint8_t {
  Busy = 0, // Waiting after a pendSV
  Interrupted = 1, // An interruption was encountered and the execution jumped to the checkpoint
  Set = 2, // The checkpoint has just been set
  Ignored = 3 // The checkpoint was ignored because it's already set at a deeper checkpoint
};

bool hasCheckpoint(CheckpointType type);
void loadCheckpoint(CheckpointType type);
Status status();
Status setCheckpoint(CheckpointType type);
void unsetCheckpoint(CheckpointType type);

}

namespace Display {

void pushRect(Rect r, const Color * pixels);
void pushRectUniform(Rect r, Color c);
void pullRect(Rect r, Color * pixels);
bool waitForVBlank();
int displayUniformTilingSize10(Color c);
int displayColoredTilingSize10();
void POSTPushMulticolor(int rootNumberTiles, int tileSize);

}

namespace Events {

enum class ShiftAlphaStatus : uint8_t {
  Default,
  Shift,
  Alpha,
  ShiftAlpha,
  AlphaLock,
  ShiftAlphaLock,
};

size_t copyText(uint8_t eventId, char * buffer, uint32_t bufferSize);
bool isDefined(uint8_t eventId);
void setShiftAlphaStatus(ShiftAlphaStatus s);
ShiftAlphaStatus shiftAlphaStatus();
int repetitionFactor();
uint8_t getEvent(int * timeout);
void setSpinner(bool spinner);

}

const char * fccId();

namespace Keyboard {

State scan();

}

namespace Power {

void selectStandbyMode(bool standbyMode);
void suspend(bool checkIfOnOffKeyReleased);
void standby();

}

uint32_t random();

namespace Device {
namespace SerialNumber {

void copy(char * buffer);

}
}

namespace Timing {

void usleep(uint32_t us);
void msleep(uint32_t ms);
uint64_t millis();

}

namespace USB {

bool isPlugged();

}

namespace Device {
namespace USB {

void willExecuteDFU();
void didExecuteDFU();
bool shouldInterruptDFU();

}
}

}

#endif
