#include <kernel/boot/isr.h>
#include <ion.h>
#include <kernel/drivers/authentication.h>
#include <kernel/drivers/backlight.h>
#include <kernel/drivers/battery.h>
#include <kernel/drivers/board.h>
#include <kernel/drivers/circuit_breaker.h>
#include <kernel/drivers/crc32.h>
#include <kernel/drivers/events.h>
#include <kernel/drivers/fcc_id.h>
#include <kernel/drivers/keyboard.h>
#include <kernel/drivers/led.h>
#include <kernel/drivers/persisting_bytes.h>
#include <kernel/drivers/power.h>
#include <kernel/drivers/timing.h>
#include <kernel/drivers/random.h>
#include <shared/drivers/usb.h>
#include <shared/drivers/display.h>
#include <shared/drivers/flash_privileged.h>
#include <shared/drivers/reset.h>
#include <shared/drivers/serial_number.h>
#include <shared/drivers/svcall.h>
#include <string.h>

//https://developer.arm.com/documentation/dui0471/m/handling-processor-exceptions/svc-handlers-in-c-and-assembly-language

template <class T1, class T2> struct SameType { enum{value = false}; };
template <class T> struct SameType<T,T> { enum{value = true}; };

#define ENSURE_SVC_TYPE(svcallhandler) \
  static_assert(SameType<decltype(&svcallhandler), void (*)()>::value || \
                SameType<decltype(&svcallhandler), void (*)(uint8_t)>::value || \
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
                SameType<decltype(&svcallhandler), int (*)(KDColor)>::value || \
                SameType<decltype(&svcallhandler), Ion::Events::Event (*)(int *)>::value || \
                SameType<decltype(&svcallhandler), size_t (*)(uint8_t, char *, size_t)>::value || \
                SameType<decltype(&svcallhandler), bool (*)(uint8_t)>::value || \
                SameType<decltype(&svcallhandler), void (*)(Ion::Events::ShiftAlphaStatus)>::value || \
                SameType<decltype(&svcallhandler), Ion::Events::ShiftAlphaStatus (*)()>::value || \
                SameType<decltype(&svcallhandler), const char * (*)()>::value || \
                SameType<decltype(&svcallhandler), bool (*)(int)>::value || \
                SameType<decltype(&svcallhandler), bool (*)(uint8_t *, const uint8_t *, size_t)>::value || \
                SameType<decltype(&svcallhandler), Ion::Keyboard::State (*)()>::value || \
                SameType<decltype(&svcallhandler), KDColor (*)()>::value || \
                SameType<decltype(&svcallhandler), void (*)(KDColor)>::value || \
                SameType<decltype(&svcallhandler), void (*)(uint16_t, float)>::value || \
                SameType<decltype(&svcallhandler), void (*)(bool)>::value || \
                SameType<decltype(&svcallhandler), void (*)(char *)>::value || \
                SameType<decltype(&svcallhandler), void (*)(uint32_t)>::value || \
                SameType<decltype(&svcallhandler), uint64_t (*)()>::value || \
                SameType<decltype(&svcallhandler), uint8_t (*)()>::value \
      );

#define MAKE_SVCALL_HANDLER(svcallindex, svcallhandler) ENSURE_SVC_TYPE(svcallhandler) SVCallTable[svcallindex] = reinterpret_cast<void *>(svcallhandler);

void * svcallHandler(int svcNumber) {
  static void * SVCallTable[SVC_NUMBER_OF_CALLS] = {0};
  if (SVCallTable[0] == 0) {
    MAKE_SVCALL_HANDLER(SVC_AUTHENTICATION_TRUSTED_USERLAND, Ion::Device::Authentication::trustedUserland)
    MAKE_SVCALL_HANDLER(SVC_BACKLIGHT_BRIGHTNESS, Ion::Device::Backlight::brightness)
    MAKE_SVCALL_HANDLER(SVC_BACKLIGHT_INIT, Ion::Device::Backlight::init)
    MAKE_SVCALL_HANDLER(SVC_BACKLIGHT_IS_INITIALIZED, Ion::Device::Backlight::isInitialized)
    MAKE_SVCALL_HANDLER(SVC_BACKLIGHT_SET_BRIGHTNESS, Ion::Device::Backlight::setBrightness)
    MAKE_SVCALL_HANDLER(SVC_BACKLIGHT_SHUTDOWN, Ion::Device::Backlight::shutdown)
    MAKE_SVCALL_HANDLER(SVC_BATTERY_IS_CHARGING, Ion::Device::Battery::isCharging)
    MAKE_SVCALL_HANDLER(SVC_BATTERY_LEVEL, Ion::Device::Battery::level)
    MAKE_SVCALL_HANDLER(SVC_BATTERY_VOLTAGE, Ion::Device::Battery::voltage)
    MAKE_SVCALL_HANDLER(SVC_BOARD_DOWNGRADE_TRUST_LEVEL, Ion::Device::Board::downgradeTrustLevel)
    MAKE_SVCALL_HANDLER(SVC_BOARD_SWITCH_EXECUTABLE_SLOT, Ion::Device::Board::switchExecutableSlot)
    MAKE_SVCALL_HANDLER(SVC_CIRCUIT_BREAKER_HAS_CHECKPOINT, Ion::Device::CircuitBreaker::hasCheckpoint)
    MAKE_SVCALL_HANDLER(SVC_CIRCUIT_BREAKER_LOAD_CHECKPOINT, Ion::Device::CircuitBreaker::loadCheckpoint)
    MAKE_SVCALL_HANDLER(SVC_CIRCUIT_BREAKER_SET_CHECKPOINT, Ion::Device::CircuitBreaker::setCheckpoint)
    MAKE_SVCALL_HANDLER(SVC_CIRCUIT_BREAKER_STATUS, Ion::Device::CircuitBreaker::status)
    MAKE_SVCALL_HANDLER(SVC_CIRCUIT_BREAKER_UNSET_CHECKPOINT, Ion::Device::CircuitBreaker::unsetCheckpoint)
    MAKE_SVCALL_HANDLER(SVC_CRC32_BYTE, Ion::Device::crc32Byte)
    MAKE_SVCALL_HANDLER(SVC_CRC32_WORD, Ion::Device::crc32Word)
    MAKE_SVCALL_HANDLER(SVC_DISPLAY_COLORED_TILING_SIZE_10, Ion::Device::Display::displayColoredTilingSize10)
    MAKE_SVCALL_HANDLER(SVC_DISPLAY_POST_PUSH_MULTICOLOR, Ion::Device::Display::POSTPushMulticolor)
    MAKE_SVCALL_HANDLER(SVC_DISPLAY_PULL_RECT, Ion::Device::Display::pullRectSecure)
    MAKE_SVCALL_HANDLER(SVC_DISPLAY_PUSH_RECT, Ion::Device::Display::pushRect)
    MAKE_SVCALL_HANDLER(SVC_DISPLAY_PUSH_RECT_UNIFORM, Ion::Device::Display::pushRectUniform)
    MAKE_SVCALL_HANDLER(SVC_DISPLAY_UNIFORM_TILING_SIZE_10, Ion::Device::Display::displayUniformTilingSize10)
    MAKE_SVCALL_HANDLER(SVC_DISPLAY_WAIT_FOR_V_BLANK, Ion::Device::Display::waitForVBlank)
    MAKE_SVCALL_HANDLER(SVC_EVENTS_COPY_TEXT, Ion::Device::Events::copyTextSecure)
    MAKE_SVCALL_HANDLER(SVC_EVENTS_GET_EVENT, Ion::Device::Events::getEvent)
    MAKE_SVCALL_HANDLER(SVC_EVENTS_IS_DEFINED, Ion::Device::Events::isDefinedSecure)
    MAKE_SVCALL_HANDLER(SVC_EVENTS_REPETITION_FACTOR, Ion::Events::repetitionFactor)
    MAKE_SVCALL_HANDLER(SVC_EVENTS_SET_SHIFT_ALPHA_STATUS, Ion::Events::setShiftAlphaStatus)
    MAKE_SVCALL_HANDLER(SVC_EVENTS_SET_SPINNER, Ion::Device::Events::setSpinner)
    MAKE_SVCALL_HANDLER(SVC_EVENTS_SHIFT_ALPHA_STATUS, Ion::Events::shiftAlphaStatus)
    MAKE_SVCALL_HANDLER(SVC_FCC_ID, Ion::Device::fccId)
    MAKE_SVCALL_HANDLER(SVC_FLASH_ERASE_SECTOR, Ion::Device::Flash::EraseSector)
    MAKE_SVCALL_HANDLER(SVC_FLASH_MASS_ERASE, Ion::Device::Flash::MassErase)
    MAKE_SVCALL_HANDLER(SVC_FLASH_WRITE_MEMORY, Ion::Device::Flash::WriteMemory)
    MAKE_SVCALL_HANDLER(SVC_KEYBOARD_SCAN, Ion::Device::Keyboard::scan)
    MAKE_SVCALL_HANDLER(SVC_LED_GET_COLOR, Ion::Device::LED::getColor)
    MAKE_SVCALL_HANDLER(SVC_LED_SET_BLINKING, Ion::Device::LED::setBlinking)
    MAKE_SVCALL_HANDLER(SVC_LED_SET_COLOR, Ion::Device::LED::setColor)
    MAKE_SVCALL_HANDLER(SVC_LED_UPDATE_COLOR_WITH_PLUG_AND_CHARGE, Ion::Device::LED::updateColorWithPlugAndCharge)
    MAKE_SVCALL_HANDLER(SVC_PCB_VERSION, Ion::Device::Board::pcbVersion)
    MAKE_SVCALL_HANDLER(SVC_PERSISTING_BYTES_READ, Ion::Device::PersistingBytes::read)
    MAKE_SVCALL_HANDLER(SVC_PERSISTING_BYTES_WRITE, Ion::Device::PersistingBytes::write)
    MAKE_SVCALL_HANDLER(SVC_POWER_SELECT_STANDBY_MODE, Ion::Device::Power::selectStandbyMode)
    MAKE_SVCALL_HANDLER(SVC_POWER_STANDBY, Ion::Device::Power::standby)
    MAKE_SVCALL_HANDLER(SVC_POWER_SUSPEND, Ion::Device::Power::suspend)
    MAKE_SVCALL_HANDLER(SVC_RANDOM, Ion::Device::random)
    MAKE_SVCALL_HANDLER(SVC_RESET_CORE, Ion::Device::Reset::coreWhilePlugged)
    MAKE_SVCALL_HANDLER(SVC_SERIAL_NUMBER_COPY, Ion::Device::SerialNumber::copy)
    MAKE_SVCALL_HANDLER(SVC_TIMING_MILLIS, Ion::Timing::millis)
    MAKE_SVCALL_HANDLER(SVC_TIMING_MSLEEP, Ion::Timing::msleep)
    MAKE_SVCALL_HANDLER(SVC_TIMING_USLEEP, Ion::Timing::usleep)
    MAKE_SVCALL_HANDLER(SVC_USB_DID_EXECUTE_DFU, Ion::Device::USB::didExecuteDFU)
    MAKE_SVCALL_HANDLER(SVC_USB_IS_PLUGGED, Ion::USB::isPlugged)
    MAKE_SVCALL_HANDLER(SVC_USB_SHOULD_INTERRUPT, Ion::Device::USB::shouldInterruptDFU)
    MAKE_SVCALL_HANDLER(SVC_USB_WILL_EXECUTE_DFU, Ion::Device::USB::willExecuteDFU)
  }
  return SVCallTable[svcNumber];
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
    if (svcNumber == authentificationRequired[i] && !Ion::Device::Authentication::trustedUserland()) {
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

  // Step 5: store r4-r5, callee-saved registers to use them as scratch regs
  asm volatile ("push {r4-r5}");

  /* Step 6: find kernel function address */
  asm volatile ("mov r4, %[kernelFunction]" : : [kernelFunction] "rn" (svcallHandlerPointer));

  /* Step 7: restore r0-r4, d0-d7 registers from stack frame since they might
   * hold the arguments/return value of the kernel function. */
  asm volatile ("mov r5, %[value]" : : [value] "r" (processStackPointer));
  if (extendedStackFrame) {
    asm volatile ("vldr d0, [r5, #32]");
    asm volatile ("vldr d1, [r5, #40]");
    asm volatile ("vldr d2, [r5, #48]");
    asm volatile ("vldr d3, [r5, #56]");
    asm volatile ("vldr d4, [r5, #64]");
    asm volatile ("vldr d5, [r5, #72]");
    asm volatile ("vldr d6, [r5, #80]");
    asm volatile ("vldr d7, [r5, #88]");
  }
  asm volatile ("ldr r0, [r5]");
  asm volatile ("ldr r1, [r5, #4]");
  asm volatile ("ldr r2, [r5, #8]");
  asm volatile ("ldr r3, [r5, #12]");

  // Step 8: jump to the kernel function
  asm volatile ("blx r4");

  /* Step 9: save the new values of the argument/return values registers in
   * the stack frame. */
  asm volatile ("str r0, [r5]");
  asm volatile ("str r1, [r5, #4]");
  asm volatile ("str r2, [r5, #8]");
  asm volatile ("str r3, [r5, #12]");
  if (extendedStackFrame) {
    asm volatile ("vstr d0, [r5, #32]");
    asm volatile ("vstr d1, [r5, #40]");
    asm volatile ("vstr d2, [r5, #48]");
    asm volatile ("vstr d3, [r5, #56]");
    asm volatile ("vstr d4, [r5, #64]");
    asm volatile ("vstr d5, [r5, #72]");
    asm volatile ("vstr d6, [r5, #80]");
    asm volatile ("vstr d7, [r5, #88]");
  }

  // Step 10: restore callee-saved registers
  asm volatile ("pop {r4-r5}");
}
