#include "svc.h"

void abort() {
  SVC_RETURNING_VOID(SVC_RESET_CORE);
}

namespace Ion {

namespace Backlight {

void SVC_ATTRIBUTES init() {
  SVC_RETURNING_VOID(SVC_BACKLIGHT_INIT)
}

void SVC_ATTRIBUTES shutdown() {
  SVC_RETURNING_VOID(SVC_BACKLIGHT_SHUTDOWN)
}

bool SVC_ATTRIBUTES isInitialized() {
  SVC_RETURNING_R0(SVC_BACKLIGHT_IS_INITIALIZED, bool)
}

void SVC_ATTRIBUTES setBrightness(uint8_t b) {
  SVC_RETURNING_VOID(SVC_BACKLIGHT_SET_BRIGHTNESS)
}

uint8_t SVC_ATTRIBUTES brightness() {
  SVC_RETURNING_R0(SVC_BACKLIGHT_BRIGHTNESS, uint8_t)
}

}

namespace Battery {

bool SVC_ATTRIBUTES isCharging() {
  SVC_RETURNING_R0(SVC_BATTERY_IS_CHARGING, bool)
}

Charge SVC_ATTRIBUTES level() {
  SVC_RETURNING_R0(SVC_BATTERY_LEVEL, Charge)
}

float SVC_ATTRIBUTES voltage() {
  SVC_RETURNING_S0(SVC_BATTERY_VOLTAGE, float)
}

}

namespace CircuitBreaker {

bool SVC_ATTRIBUTES hasCheckpoint(CheckpointType type) {
  SVC_RETURNING_R0(SVC_CIRCUIT_BREAKER_HAS_CHECKPOINT, bool)
}

void SVC_ATTRIBUTES loadCheckpoint(CheckpointType type) {
  SVC_RETURNING_VOID(SVC_CIRCUIT_BREAKER_LOAD_CHECKPOINT)
}

bool SVC_ATTRIBUTES kernelSetCheckpoint(CheckpointType type) {
  SVC_RETURNING_R0(SVC_CIRCUIT_BREAKER_SET_CHECKPOINT, bool)
}

Status SVC_ATTRIBUTES status() {
  SVC_RETURNING_R0(SVC_CIRCUIT_BREAKER_STATUS, Ion::CircuitBreaker::Status)
}

Status setCheckpoint(CheckpointType type) {
  bool checkpointHasBeenSet = kernelSetCheckpoint(type);
  if (!checkpointHasBeenSet) {
    return Status::Ignored;
  }
  Status s = status();
  while (s == Status::Busy) {
    s = status();
  }
  return s;

}

void SVC_ATTRIBUTES unsetCheckpoint(CheckpointType type) {
  SVC_RETURNING_VOID(SVC_CIRCUIT_BREAKER_UNSET_CHECKPOINT)
}

}

namespace Display {

void SVC_ATTRIBUTES pushRect(Rect r, const Color * pixels) {
  SVC_RETURNING_VOID(SVC_DISPLAY_PUSH_RECT)
}

void SVC_ATTRIBUTES pushRectUniform(Rect r, Color c) {
  SVC_RETURNING_VOID(SVC_DISPLAY_PUSH_RECT_UNIFORM)
}

void SVC_ATTRIBUTES pullRect(Rect r, Color * pixels) {
  SVC_RETURNING_VOID(SVC_DISPLAY_PULL_RECT)
}

bool SVC_ATTRIBUTES waitForVBlank() {
  SVC_RETURNING_R0(SVC_DISPLAY_WAIT_FOR_V_BLANK, bool)
}

int SVC_ATTRIBUTES displayUniformTilingSize10(Color c) {
  SVC_RETURNING_R0(SVC_DISPLAY_UNIFORM_TILING_SIZE_10, int)
}

int SVC_ATTRIBUTES displayColoredTilingSize10() {
  SVC_RETURNING_R0(SVC_DISPLAY_COLORED_TILING_SIZE_10, int)
}

void SVC_ATTRIBUTES POSTPushMulticolor(int rootNumberTiles, int tileSize) {
  SVC_RETURNING_VOID(SVC_DISPLAY_POST_PUSH_MULTICOLOR)
}

}

namespace Events {

size_t SVC_ATTRIBUTES copyText(uint8_t eventId, char * buffer, uint32_t bufferSize) {
  SVC_RETURNING_R0(SVC_EVENTS_COPY_TEXT, uint32_t)
}

bool SVC_ATTRIBUTES isDefined(uint8_t eventId) {
  SVC_RETURNING_R0(SVC_EVENTS_IS_DEFINED, bool)
}

void SVC_ATTRIBUTES setShiftAlphaStatus(ShiftAlphaStatus s) {
  SVC_RETURNING_VOID(SVC_EVENTS_SET_SHIFT_ALPHA_STATUS)
}

ShiftAlphaStatus SVC_ATTRIBUTES shiftAlphaStatus() {
  SVC_RETURNING_R0(SVC_EVENTS_SHIFT_ALPHA_STATUS, ShiftAlphaStatus)
}

int SVC_ATTRIBUTES repetitionFactor() {
  SVC_RETURNING_R0(SVC_EVENTS_REPETITION_FACTOR, int)
}

uint8_t SVC_ATTRIBUTES getEvent(int * timeout) {
  SVC_RETURNING_R0(SVC_EVENTS_GET_EVENT, uint8_t)
}

}

const char * SVC_ATTRIBUTES fccId() {
  SVC_RETURNING_R0(SVC_FCC_ID, const char *)
}

namespace Keyboard {

State SVC_ATTRIBUTES scan() {
  SVC_RETURNING_STASH_ADDRESS_IN_R0(SVC_KEYBOARD_SCAN, State)
}

}

namespace Power {

void SVC_ATTRIBUTES selectStandbyMode(bool standbyMode) {
  SVC_RETURNING_VOID(SVC_POWER_SELECT_STANDBY_MODE)
}

void SVC_ATTRIBUTES suspend(bool checkIfOnOffKeyReleased) {
  SVC_RETURNING_VOID(SVC_POWER_SUSPEND)
}

void SVC_ATTRIBUTES standby() {
  SVC_RETURNING_VOID(SVC_POWER_STANDBY)
}

}

uint32_t SVC_ATTRIBUTES random() {
  SVC_RETURNING_R0(SVC_RANDOM, uint32_t)
}

namespace Device {
namespace SerialNumber {

void SVC_ATTRIBUTES copy(char * buffer) {
  SVC_RETURNING_VOID(SVC_SERIAL_NUMBER_COPY)
}

}
}

namespace Timing {

void SVC_ATTRIBUTES usleep(uint32_t us) {
  SVC_RETURNING_VOID(SVC_TIMING_USLEEP)
}

void SVC_ATTRIBUTES msleep(uint32_t ms) {
  SVC_RETURNING_VOID(SVC_TIMING_MSLEEP)
}

uint64_t SVC_ATTRIBUTES millis() {
  SVC_RETURNING_R0R1(SVC_TIMING_MILLIS, uint64_t)
}

}

namespace USB {

bool SVC_ATTRIBUTES isPlugged() {
  SVC_RETURNING_R0(SVC_USB_IS_PLUGGED, bool)
}

}

namespace Device {
namespace USB {

void SVC_ATTRIBUTES willExecuteDFU() {
  SVC_RETURNING_VOID(SVC_USB_WILL_EXECUTE_DFU)
}

void SVC_ATTRIBUTES didExecuteDFU() {
  SVC_RETURNING_VOID(SVC_USB_DID_EXECUTE_DFU)
}

bool SVC_ATTRIBUTES shouldInterruptDFU() {
  SVC_RETURNING_R0(SVC_USB_SHOULD_INTERRUPT, bool)
}

}
}
}
