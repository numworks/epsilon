#include <kernel/boot/isr.h>
#include <ion.h>
#include <kernel/drivers/authentication.h>
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
#include <shared/drivers/backlight.h>
#include <shared/drivers/display.h>
#include <shared/drivers/flash_privileged.h>
#include <shared/drivers/reset.h>
#include <shared/drivers/serial_number.h>
#include <shared/drivers/svcall.h>
#include <string.h>

//https://developer.arm.com/documentation/dui0471/m/handling-processor-exceptions/svc-handlers-in-c-and-assembly-language

// TODO: static assertion on types and position of svcall
// static_assert(SVCallTable[SVC_BACKLIGHT_BRIGHTNESS] == (void *)Ion::Device::Backlight::brightness);

void * SVCallTable[SVC_NUMBER_OF_CALLS] = {
  // SVC_BACKLIGHT_BRIGHTNESS
  (void *)Ion::Device::Backlight::brightness,
  // SVC_BACKLIGHT_INIT
  (void *)Ion::Device::Backlight::init,
  // SVC_BACKLIGHT_IS_INITIALIZED
  (void *)Ion::Device::Backlight::isInitialized,
  // SVC_BACKLIGHT_SET_BRIGHTNESS
  (void *)Ion::Device::Backlight::setBrightness,
  // SVC_BACKLIGHT_SHUTDOWN
  (void *)Ion::Device::Backlight::shutdown,
  // SVC_BATTERY_IS_CHARGING
  (void *)Ion::Device::Battery::isCharging,
  // SVC_BATTERY_LEVEL
  (void *)Ion::Device::Battery::level,
  // SVC_BATTERY_VOLTAGE
  (void *)Ion::Device::Battery::voltage,
  // SVC_BOARD_SWITCH_EXECUTABLE_SLOT
  (void *)Ion::Device::Board::switchExecutableSlot,
  // SVC_CIRCUIT_BREAKER_HAS_CHECKPOINT
  (void *)Ion::Device::CircuitBreaker::hasCheckpoint,
  // SVC_CIRCUIT_BREAKER_LOAD_CHECKPOINT
  (void *)Ion::Device::CircuitBreaker::loadCheckpoint,
  // SVC_CIRCUIT_BREAKER_SET_CHECKPOINT
  (void *)Ion::Device::CircuitBreaker::setCheckpoint,
  // SVC_CIRCUIT_BREAKER_STATUS
  (void *)Ion::Device::CircuitBreaker::status,
  // SVC_CIRCUIT_BREAKER_UNSET_CHECKPOINT
  (void *)Ion::Device::CircuitBreaker::unsetCheckpoint,
  // SVC_CRC32_BYTE
  (void *)Ion::Device::crc32Byte,
  // SVC_CRC32_WORD
  (void *)Ion::Device::crc32Word,
  // SVC_DISPLAY_COLORED_TILING_SIZE_10
  (void *)Ion::Device::Display::displayColoredTilingSize10,
  // SVC_DISPLAY_POST_PUSH_MULTICOLOR
  (void *)Ion::Device::Display::POSTPushMulticolor,
  // SVC_DISPLAY_PULL_RECT
  (void *)Ion::Device::Display::pullRect,
  // SVC_DISPLAY_PUSH_RECT
  (void *)Ion::Device::Display::pushRect,
  // SVC_DISPLAY_PUSH_RECT_UNIFORM
  (void *)Ion::Device::Display::pushRectUniform,
  // SVC_DISPLAY_UNIFORM_TILING_SIZE_10
  (void *)Ion::Device::Display::displayUniformTilingSize10,
  // SVC_DISPLAY_WAIT_FOR_V_BLANK
  (void *)Ion::Device::Display::waitForVBlank,
  // SVC_EVENTS_COPY_TEXT
  (void *)&Ion::Events::Event::copyText,
  // SVC_EVENTS_GET_EVENT
  (void *)Ion::Device::Events::getEvent,
  // SVC_EVENTS_IS_DEFINED
  (void *)&Ion::Events::Event::isDefined,
  // SVC_EVENTS_REPETITION_FACTOR
  (void *)Ion::Events::repetitionFactor,
  // SVC_EVENTS_SET_SHIFT_ALPHA_STATUS
  (void *)Ion::Events::setShiftAlphaStatus,
  // SVC_EVENTS_SHIFT_ALPHA_STATUS
  (void *)Ion::Events::shiftAlphaStatus,
  // SVC_FCC_ID
  (void *)Ion::Device::fccId,
  // SVC_FLASH_ERASE_SECTOR
  (void *)Ion::Device::Flash::EraseSector,
  // SVC_FLASH_MASS_ERASE
  (void *)Ion::Device::Flash::MassErase,
  // SVC_FLASH_WRITE_MEMORY
  (void *)Ion::Device::Flash::WriteMemory,
  // SVC_KEYBOARD_SCAN
  (void *)Ion::Device::Keyboard::scan,
  // SVC_LED_GET_COLOR
  (void *)Ion::Device::LED::getColor,
  // SVC_LED_SET_BLINKING
  (void *)Ion::Device::LED::setBlinking,
  // SVC_LED_SET_COLOR
  (void *)Ion::Device::LED::setColor,
  // SVC_LED_UPDATE_COLOR_WITH_PLUG_AND_CHARGE
  (void *)Ion::Device::LED::updateColorWithPlugAndCharge,
  // SVC_PERSISTING_BYTES_READ
  (void *)Ion::Device::PersistingBytes::read,
  // SVC_PERSISTING_BYTES_WRITE
  (void *)Ion::Device::PersistingBytes::write,
  // SVC_POWER_SELECT_STANDBY_MODE
  (void *)Ion::Device::Power::selectStandbyMode,
  // SVC_POWER_STANDBY
  (void *)Ion::Device::Power::standby,
  // SVC_POWER_SUSPEND
  (void *)Ion::Device::Power::suspend,
  // SVC_RANDOM
  (void *)Ion::Device::random,
  // SVC_RESET_CORE
  (void *)Ion::Device::Reset::coreWhilePlugged,
  // SVC_SERIAL_NUMBER
  (void *)Ion::Device::SerialNumber::get,
  // SVC_SERIAL_NUMBER_COPY
  (void *)Ion::Device::SerialNumber::copy,
  // SVC_TIMING_MILLIS
  (void *)Ion::Timing::millis,
  // SVC_TIMING_MSLEEP
  (void *)Ion::Timing::msleep,
  // SVC_TIMING_USLEEP
  (void *)Ion::Timing::usleep,
  // SVC_USB_DID_EXECUTE_DFU
  (void *)Ion::Device::USB::didExecuteDFU,
  // SVC_USB_IS_PLUGGED
  (void *)Ion::USB::isPlugged,
  // SVC_USB_SHOULD_INTERRUPT
  (void *)Ion::Device::USB::shouldInterruptDFU,
  // SVC_USB_WILL_EXECUTE_DFU
  (void *)Ion::Device::USB::willExecuteDFU
};

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
  constexpr unsigned authentificationRequired[] = {SVC_USB_WILL_EXECUTE_DFU, SVC_USB_DID_EXECUTE_DFU, SVC_BOARD_SWITCH_EXECUTABLE_SLOT, SVC_FLASH_MASS_ERASE, SVC_FLASH_ERASE_SECTOR, SVC_FLASH_WRITE_MEMORY};
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

  // Step 5: store r4-r5, callee-saved registers to use them as scratch regs
  asm volatile ("push {r4-r5}");

  /* Step 6: find kernel function address */
  asm volatile ("mov r4, %[kernelFunction]" : : [kernelFunction] "rn" (SVCallTable[svcNumber]));

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
