#include <ion.h>
#include <kernel/boot/isr.h>
#include <kernel/drivers/authentication.h>
#include <kernel/drivers/board.h>
#include <kernel/drivers/circuit_breaker.h>
#include <kernel/drivers/events.h>
#include <kernel/drivers/fcc_id.h>
#include <kernel/drivers/keyboard.h>
#include <kernel/drivers/led.h>
#include <kernel/drivers/persisting_bytes.h>
#include <kernel/drivers/power.h>
#include <kernel/drivers/timing.h>
#include <shared/drivers/backlight.h>
#include <shared/drivers/battery.h>
#include <shared/drivers/crc32.h>
#include <shared/drivers/display.h>
#include <shared/drivers/flash_privileged.h>
#include <shared/drivers/random.h>
#include <shared/drivers/reset.h>
#include <shared/drivers/serial_number.h>
#include <shared/drivers/svcall.h>
#include <shared/drivers/usb.h>
#include <string.h>

//https://developer.arm.com/documentation/dui0471/m/handling-processor-exceptions/svc-handlers-in-c-and-assembly-language

#define MAKE_SVCALL_HANDLER_ENTRY(svcallhandler) reinterpret_cast<void *>(svcallhandler)

void * const k_SVCallTable[SVC_NUMBER_OF_CALLS] = {
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::Authentication::clearanceLevel),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::Backlight::brightness),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::Backlight::setBrightness),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::Battery::isCharging),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::Battery::level),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::Battery::voltage),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::Board::enableExternalApps),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::Board::switchExecutableSlot),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::CircuitBreaker::hasCheckpoint),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::CircuitBreaker::loadCheckpoint),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::CircuitBreaker::lock),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::CircuitBreaker::setCheckpoint),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::CircuitBreaker::status),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::CircuitBreaker::unlock),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::CircuitBreaker::unsetCheckpoint),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::crc32Byte),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::crc32Word),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::Display::POSTPushMulticolor),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::Display::pullRectSecure),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::Display::pushRect),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::Display::pushRectUniform),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::Display::waitForVBlank),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::Events::copyTextSecure),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::Events::getEvent),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::Events::isDefinedSecure),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Events::repetitionFactor),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Events::setShiftAlphaStatus),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::Events::setSpinner),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Events::shiftAlphaStatus),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::fccId),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::Flash::EraseSector),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::Flash::MassErase),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::Flash::WriteMemory),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::Keyboard::popStateSVC),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::Keyboard::scanSVC),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::LED::getColor),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::LED::setBlinking),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::LED::setColorSecure),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::LED::updateColorWithPlugAndCharge),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::Board::pcbVersion),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::PersistingBytes::read),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::PersistingBytes::write),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::Power::selectStandbyMode),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::Power::standby),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::Power::suspend),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::random),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::Reset::core),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::SerialNumber::copy),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Timing::millis),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Timing::msleep),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Timing::usleep),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::USB::didExecuteDFU),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::USB::isPlugged),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::USB::shouldInterruptDFU),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Device::USB::willExecuteDFU),
  MAKE_SVCALL_HANDLER_ENTRY(Ion::Events::longPressFactor)
};

template <class T1, class T2> struct SameType { enum{value = false}; };
template <class T> struct SameType<T,T> { enum{value = true}; };

#define ENSURE_SVC_TYPE(svcallindex, svcallhandler) \
  static_assert(SameType<decltype(&svcallhandler), void (*)()>::value || \
                SameType<decltype(&svcallhandler), void (*)(uint8_t)>::value || \
                SameType<decltype(&svcallhandler), void (*)(uint8_t, uint8_t)>::value || \
                SameType<decltype(&svcallhandler), bool (*)()>::value || \
                SameType<decltype(&svcallhandler), float (*)()>::value || \
                SameType<decltype(&svcallhandler), uint32_t (*)()>::value || \
                SameType<decltype(&svcallhandler), bool (*)(Ion::CircuitBreaker::CheckpointType)>::value || \
                SameType<decltype(&svcallhandler), void (*)(Ion::CircuitBreaker::CheckpointType)>::value || \
                SameType<decltype(&svcallhandler), Ion::CircuitBreaker::Status (*)()>::value || \
                SameType<decltype(&svcallhandler), Ion::Battery::Charge (*)()>::value || \
                SameType<decltype(&svcallhandler), uint32_t (*)(const uint8_t *, size_t)>::value || \
                SameType<decltype(&svcallhandler), uint32_t (*)(const uint32_t *, size_t)>::value || \
                SameType<decltype(&svcallhandler), uint32_t (*)(uint32_t, uint8_t)>::value || \
                SameType<decltype(&svcallhandler), int (*)()>::value || \
                SameType<decltype(&svcallhandler), void (*)(int, int)>::value || \
                SameType<decltype(&svcallhandler), void (*)(KDRect, KDColor *)>::value || \
                SameType<decltype(&svcallhandler), void (*)(KDRect, const KDColor *)>::value || \
                SameType<decltype(&svcallhandler), void (*)(KDRect, KDColor)>::value || \
                SameType<decltype(&svcallhandler), Ion::Events::Event (*)(int *)>::value || \
                SameType<decltype(&svcallhandler), size_t (*)(uint8_t, char *, size_t)>::value || \
                SameType<decltype(&svcallhandler), bool (*)(uint8_t)>::value || \
                SameType<decltype(&svcallhandler), void (*)(Ion::Events::ShiftAlphaStatus)>::value || \
                SameType<decltype(&svcallhandler), Ion::Events::ShiftAlphaStatus (*)()>::value || \
                SameType<decltype(&svcallhandler), const char * (*)()>::value || \
                SameType<decltype(&svcallhandler), bool (*)(int)>::value || \
                SameType<decltype(&svcallhandler), bool (*)(uint8_t *, const uint8_t *, size_t)>::value || \
                SameType<decltype(&svcallhandler), KDColor (*)()>::value || \
                SameType<decltype(&svcallhandler), void (*)(KDColor)>::value || \
                SameType<decltype(&svcallhandler), void (*)(uint16_t, float)>::value || \
                SameType<decltype(&svcallhandler), void (*)(bool)>::value || \
                SameType<decltype(&svcallhandler), void (*)(char *)>::value || \
                SameType<decltype(&svcallhandler), void (*)(uint32_t)>::value || \
                SameType<decltype(&svcallhandler), uint64_t (*)()>::value || \
                SameType<decltype(&svcallhandler), Ion::Authentication::ClearanceLevel (*)()>::value || \
                SameType<decltype(&svcallhandler), uint8_t (*)()>::value || \
                SameType<decltype(&svcallhandler), uint8_t (*)(uint8_t)>::value \
      ); \
  assert(k_SVCallTable[svcallindex] == MAKE_SVCALL_HANDLER_ENTRY(svcallhandler));

void * svcallHandler(int svcNumber) {
  ENSURE_SVC_TYPE(SVC_AUTHENTICATION_CLEARANCE_LEVEL, Ion::Device::Authentication::clearanceLevel)
  ENSURE_SVC_TYPE(SVC_BACKLIGHT_BRIGHTNESS, Ion::Device::Backlight::brightness)
  ENSURE_SVC_TYPE(SVC_BACKLIGHT_SET_BRIGHTNESS, Ion::Device::Backlight::setBrightness)
  ENSURE_SVC_TYPE(SVC_BATTERY_IS_CHARGING, Ion::Device::Battery::isCharging)
  ENSURE_SVC_TYPE(SVC_BATTERY_LEVEL, Ion::Device::Battery::level)
  ENSURE_SVC_TYPE(SVC_BATTERY_VOLTAGE, Ion::Device::Battery::voltage)
  ENSURE_SVC_TYPE(SVC_BOARD_ENABLE_EXTERNAL_APPS, Ion::Device::Board::enableExternalApps)
  ENSURE_SVC_TYPE(SVC_BOARD_SWITCH_EXECUTABLE_SLOT, Ion::Device::Board::switchExecutableSlot)
  ENSURE_SVC_TYPE(SVC_CIRCUIT_BREAKER_HAS_CHECKPOINT, Ion::Device::CircuitBreaker::hasCheckpoint)
  ENSURE_SVC_TYPE(SVC_CIRCUIT_BREAKER_LOAD_CHECKPOINT, Ion::Device::CircuitBreaker::loadCheckpoint)
  ENSURE_SVC_TYPE(SVC_CIRCUIT_BREAKER_SET_CHECKPOINT, Ion::Device::CircuitBreaker::setCheckpoint)
  ENSURE_SVC_TYPE(SVC_CIRCUIT_BREAKER_STATUS, Ion::Device::CircuitBreaker::status)
  ENSURE_SVC_TYPE(SVC_CIRCUIT_BREAKER_UNSET_CHECKPOINT, Ion::Device::CircuitBreaker::unsetCheckpoint)
  ENSURE_SVC_TYPE(SVC_CRC32_BYTE, Ion::Device::crc32Byte)
  ENSURE_SVC_TYPE(SVC_CRC32_WORD, Ion::Device::crc32Word)
  ENSURE_SVC_TYPE(SVC_DISPLAY_POST_PUSH_MULTICOLOR, Ion::Device::Display::POSTPushMulticolor)
  ENSURE_SVC_TYPE(SVC_DISPLAY_PULL_RECT, Ion::Device::Display::pullRectSecure)
  ENSURE_SVC_TYPE(SVC_DISPLAY_PUSH_RECT, Ion::Device::Display::pushRect)
  ENSURE_SVC_TYPE(SVC_DISPLAY_PUSH_RECT_UNIFORM, Ion::Device::Display::pushRectUniform)
  ENSURE_SVC_TYPE(SVC_DISPLAY_WAIT_FOR_V_BLANK, Ion::Device::Display::waitForVBlank)
  ENSURE_SVC_TYPE(SVC_EVENTS_COPY_TEXT, Ion::Device::Events::copyTextSecure)
  ENSURE_SVC_TYPE(SVC_EVENTS_GET_EVENT, Ion::Device::Events::getEvent)
  ENSURE_SVC_TYPE(SVC_EVENTS_IS_DEFINED, Ion::Device::Events::isDefinedSecure)
  ENSURE_SVC_TYPE(SVC_EVENTS_REPETITION_FACTOR, Ion::Events::repetitionFactor)
  ENSURE_SVC_TYPE(SVC_EVENTS_SET_SHIFT_ALPHA_STATUS, Ion::Events::setShiftAlphaStatus)
  ENSURE_SVC_TYPE(SVC_EVENTS_SET_SPINNER, Ion::Device::Events::setSpinner)
  ENSURE_SVC_TYPE(SVC_EVENTS_SHIFT_ALPHA_STATUS, Ion::Events::shiftAlphaStatus)
  ENSURE_SVC_TYPE(SVC_FCC_ID, Ion::Device::fccId)
  ENSURE_SVC_TYPE(SVC_FLASH_ERASE_SECTOR, Ion::Device::Flash::EraseSector)
  ENSURE_SVC_TYPE(SVC_FLASH_MASS_ERASE, Ion::Device::Flash::MassErase)
  ENSURE_SVC_TYPE(SVC_FLASH_WRITE_MEMORY, Ion::Device::Flash::WriteMemory)
  ENSURE_SVC_TYPE(SVC_KEYBOARD_POP_STATE, Ion::Device::Keyboard::popStateSVC)
  ENSURE_SVC_TYPE(SVC_KEYBOARD_SCAN, Ion::Device::Keyboard::scanSVC)
  ENSURE_SVC_TYPE(SVC_LED_GET_COLOR, Ion::Device::LED::getColor)
  ENSURE_SVC_TYPE(SVC_LED_SET_BLINKING, Ion::Device::LED::setBlinking)
  ENSURE_SVC_TYPE(SVC_LED_SET_COLOR, Ion::Device::LED::setColorSecure)
  ENSURE_SVC_TYPE(SVC_LED_UPDATE_COLOR_WITH_PLUG_AND_CHARGE, Ion::Device::LED::updateColorWithPlugAndCharge)
  ENSURE_SVC_TYPE(SVC_PCB_VERSION, Ion::Device::Board::pcbVersion)
  ENSURE_SVC_TYPE(SVC_PERSISTING_BYTES_READ, Ion::Device::PersistingBytes::read)
  ENSURE_SVC_TYPE(SVC_PERSISTING_BYTES_WRITE, Ion::Device::PersistingBytes::write)
  ENSURE_SVC_TYPE(SVC_POWER_SELECT_STANDBY_MODE, Ion::Device::Power::selectStandbyMode)
  ENSURE_SVC_TYPE(SVC_POWER_STANDBY, Ion::Device::Power::standby)
  ENSURE_SVC_TYPE(SVC_POWER_SUSPEND, Ion::Device::Power::suspend)
  ENSURE_SVC_TYPE(SVC_RANDOM, Ion::Device::random)
  ENSURE_SVC_TYPE(SVC_RESET_CORE, Ion::Device::Reset::core)
  ENSURE_SVC_TYPE(SVC_SERIAL_NUMBER_COPY, Ion::Device::SerialNumber::copy)
  ENSURE_SVC_TYPE(SVC_TIMING_MILLIS, Ion::Timing::millis)
  ENSURE_SVC_TYPE(SVC_TIMING_MSLEEP, Ion::Timing::msleep)
  ENSURE_SVC_TYPE(SVC_TIMING_USLEEP, Ion::Timing::usleep)
  ENSURE_SVC_TYPE(SVC_USB_DID_EXECUTE_DFU, Ion::Device::USB::didExecuteDFU)
  ENSURE_SVC_TYPE(SVC_USB_IS_PLUGGED, Ion::USB::isPlugged)
  ENSURE_SVC_TYPE(SVC_USB_SHOULD_INTERRUPT, Ion::Device::USB::shouldInterruptDFU)
  ENSURE_SVC_TYPE(SVC_USB_WILL_EXECUTE_DFU, Ion::Device::USB::willExecuteDFU)
  ENSURE_SVC_TYPE(SVC_EVENTS_LONG_PRESS_FACTOR, Ion::Events::longPressFactor)
  return k_SVCallTable[svcNumber];
}

constexpr static uint32_t k_frameSize = 32;
constexpr static uint32_t k_extendedFrameSize = 108;

void __attribute__((externally_visible)) svcall_handler(uint32_t processStackPointer, uint32_t exceptReturn, uint32_t svcNumber) {
  /* The stack process is layout as follows:
   *
   * |                       |
   * |                       |
   * |                       |
   * +-----------------------+ <-- processStackPointer
   * | Stack Frame: r0       |
   * |              r1       |
   * |              r2       |
   * |              r3       |
   * |              r4       |
   * |              r12      |
   * |              lr       |
   * |              pc       |
   * |              ...      |
   * +-----------------------+   A
   * | Potential arguments   |   |
   * | to pass to the kernel |   |
   * | function              |   |
   * +-----------------------+   |
   * | Potential allocated   |   |
   * | space for the return  |   |
   * | value of the kernel   |   |
   * | function              |   |
   * +-----------------------+   B
   * |                       |
   * |                       |
   *
   * We ensure that AB section is empty from the SVCall function signature to
   * avoid computing dynamically its size.
   */
  // Step 1: avoid overflowing svc table
  if (svcNumber >= SVC_NUMBER_OF_CALLS) {
    return;
  }
  // Step 2: some svc calls require authentication
  constexpr unsigned authentificationRequired[] = {SVC_BOARD_SWITCH_EXECUTABLE_SLOT, SVC_LED_SET_COLOR, SVC_LED_UPDATE_COLOR_WITH_PLUG_AND_CHARGE, SVC_LED_SET_BLINKING};
  for (size_t i = 0; i < sizeof(authentificationRequired)/sizeof(unsigned); i++) {
    if (svcNumber == authentificationRequired[i] && Ion::Device::Authentication::clearanceLevel() != Ion::Authentication::ClearanceLevel::NumWorks) {
      return;
    }
  }

  /* Step 3. Assert that the SVC was called from the userland
   *
   * The EXC_RETURN value hold in lr follows the spec:
   * - EXC_RETURN have bits[31:5] set to 1
   * - bit[4] indicates if the extended FPU frame context is used (1) or not (0)
   * - bit[3] indicates the mode: thread (1) or handler (0)
   * - bit[2] indicates the stack pointer used when calling: either PSP (1) or
   *   MSP (0)
   * bit[2] and bit[3] should be 1 since svc is always called from the userland
   * (kernel should not execute any SVC instruction) so we always
   * enter svcall_handler_as from the thread mode using PSP.*/
  assert((exceptReturn & 0b1100) == 0b1100);


  /* Step 4: Get frame size: do we use the extended stack frame or standard
   * stack frame? */
  bool extendedStackFrame = !(exceptReturn & 0b10000);

  // TODO: add cache barriers?

  void * svcallHandlerPointer = svcallHandler(svcNumber);

  /* Step 5: restore r0-r4, d0-d7 registers from stack frame since they might
   * hold the arguments/return value of the kernel function. */
  asm volatile ("mov r5, %[value]" : : [value] "r" (processStackPointer));
  if (extendedStackFrame) {
    asm volatile ("vldr d0, [%[value], #32]; \
                   vldr d1, [%[value], #40]; \
                   vldr d2, [%[value], #48]; \
                   vldr d3, [%[value], #56]; \
                   vldr d4, [%[value], #64]; \
                   vldr d5, [%[value], #72]; \
                   vldr d6, [%[value], #80]; \
                   vldr d7, [%[value], #88];"
                 :
                 : [value] "r" (processStackPointer)
                 : "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7");
  }
  asm volatile ("ldr r0, [%[value]]; \
                 ldr r1, [%[value], #4]; \
                 ldr r2, [%[value], #8]; \
                 ldr r3, [%[value], #12];"
                 :
                 : [value] "r" (processStackPointer)
                 : "r0", "r1", "r2", "r3", "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7");

  // Step 6: jump to the kernel function
  asm volatile ("blx %[kernelFunction]"
      :
      : [kernelFunction] "rn" (svcallHandlerPointer)
      : "r0", "r1", "r2", "r3", "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7");

  /* Step 7: save the new values of the argument/return values registers in
   * the stack frame. */
  asm volatile ("str r0, [%[value]]; \
                 str r1, [%[value], #4]; \
                 str r2, [%[value], #8]; \
                 str r3, [%[value], #12];"
                 :
                 : [value] "r" (processStackPointer)
                 : "r0", "r1", "r2", "r3", "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7");
  if (extendedStackFrame) {
    asm volatile ("vstr d0, [%[value], #32]; \
                   vstr d1, [%[value], #40]; \
                   vstr d2, [%[value], #48]; \
                   vstr d3, [%[value], #56]; \
                   vstr d4, [%[value], #64]; \
                   vstr d5, [%[value], #72]; \
                   vstr d6, [%[value], #80]; \
                   vstr d7, [%[value], #88];"
                 :
                 : [value] "r" (processStackPointer)
                 : "r0", "r1", "r2", "r3", "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7");
  }
}
