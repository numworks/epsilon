#include <kernel/boot/isr.h>
#include <ion.h>
#include <kernel/drivers/backlight.h>
#include <kernel/drivers/battery.h>
#include <kernel/drivers/circuit_breaker.h>
#include <kernel/drivers/crc32.h>
#include <kernel/drivers/display.h>
#include <kernel/drivers/events.h>
#include <kernel/drivers/fcc_id.h>
#include <kernel/drivers/keyboard.h>
#include <kernel/drivers/led.h>
#include <kernel/drivers/persisting_bytes.h>
#include <kernel/drivers/power.h>
#include <kernel/drivers/timing.h>
#include <kernel/drivers/random.h>
#include <shared/drivers/reset.h>
#include <shared/drivers/serial_number.h>
#include <shared/drivers/svcall.h>
#include <shared/drivers/usb.h>
#include <string.h>

//https://developer.arm.com/documentation/dui0471/m/handling-processor-exceptions/svc-handlers-in-c-and-assembly-language

void svcall_handler(unsigned svcNumber, void * args[]) {
  switch (svcNumber) {
    case SVC_DISPLAY_PUSH_RECT:
      // Load rect and pixels
      Ion::Device::Display::pushRect(
          *static_cast<KDRect *>(args[0]),
          static_cast<const KDColor *>(args[1])
        );
      return;
    case SVC_DISPLAY_PUSH_RECT_UNIFORM:
    {
      // Load rect and color
      Ion::Device::Display::pushRectUniform(
          *static_cast<KDRect *>(args[0]),
          *static_cast<KDColor *>(args[1])
        );
      return;
    }
    case SVC_DISPLAY_PULL_RECT:
      // Load rect and pixels
      Ion::Device::Display::pullRect(
          *static_cast<KDRect *>(args[0]),
          static_cast<KDColor *>(args[1])
        );
      return;
    case SVC_DISPLAY_WAIT_FOR_V_BLANK:
      *static_cast<bool *>(args[0]) = Ion::Device::Display::waitForVBlank();
      return;
    case SVC_DISPLAY_UNIFORM_TILING_SIZE_10:
      *static_cast<int *>(args[1]) = Ion::Device::Display::displayUniformTilingSize10(*static_cast<KDColor *>(args[0]));
      return;
    case SVC_DISPLAY_COLORED_TILING_SIZE_10:
      *static_cast<int *>(args[0]) = Ion::Device::Display::displayColoredTilingSize10();
      return;
    case SVC_DISPLAY_POST_PUSH_MULTICOLOR:
      // Load rootNumberTiles and tileSize
      Ion::Device::Display::POSTPushMulticolor(
          *static_cast<int *>(args[0]),
          *static_cast<int *>(args[1])
        );
      return;
    // USB
    case SVC_USB_IS_PLUGGED:
      *static_cast<bool *>(args[0]) = Ion::Device::USB::isPlugged();
      return;
    case SVC_USB_IS_ENUMERATED:
      *static_cast<bool *>(args[0]) = Ion::Device::USB::isEnumerated();
      return;
    case SVC_USB_CLEAR_ENUMERATION_INTERRUPT:
      Ion::Device::USB::clearEnumerationInterrupt();
      return;
    case SVC_USB_ENABLE:
      Ion::Device::USB::enable();
      return;
    case SVC_USB_DISABLE:
      Ion::Device::USB::disable();
      return;
    case SVC_USB_DFU:
      Ion::Device::USB::DFU();
      return;
    // TIMING
    case SVC_TIMING_USLEEP:
      Ion::Device::Timing::usleep(*static_cast<uint32_t *>(args[0]));
      return;
    case SVC_TIMING_MSLEEP:
      Ion::Device::Timing::msleep(*static_cast<uint32_t *>(args[0]));
      return;
    case SVC_TIMING_MILLIS:
      *static_cast<uint64_t *>(args[0]) = Ion::Device::Timing::millis();
      return;
    // KEYBOARD
    case SVC_KEYBOARD_SCAN:
      *static_cast<Ion::Keyboard::State *>(args[0]) = Ion::Device::Keyboard::scan();
      return;
    // BATTERY
    case SVC_BATTERY_IS_CHARGING:
      *static_cast<bool *>(args[0]) = Ion::Device::Battery::isCharging();
      return;
    case SVC_BATTERY_LEVEL:
      *static_cast<Ion::Battery::Charge *>(args[0]) = Ion::Device::Battery::level();
      return;
    case SVC_BATTERY_VOLTAGE:
      *static_cast<float *>(args[0]) = Ion::Device::Battery::voltage();
      return;
    // BACKLIGHT
    case SVC_BACKLIGHT_INIT:
      Ion::Device::Backlight::init();
      return;
    case SVC_BACKLIGHT_SHUTDOWN:
      Ion::Device::Backlight::shutdown();
    case SVC_BACKLIGHT_IS_INITIALIZED:
      *static_cast<bool *>(args[0]) = Ion::Device::Backlight::isInitialized();
      return;
    case SVC_BACKLIGHT_SET_BRIGHTNESS:
      Ion::Device::Backlight::setBrightness(*static_cast<uint8_t *>(args[0]));
      return;
    case SVC_BACKLIGHT_BRIGHTNESS:
      *static_cast<uint8_t *>(args[0]) = Ion::Device::Backlight::brightness();
      return;
    // PERSISTING BYTES
    case SVC_PERSISTING_BYTES_WRITE:
      Ion::Device::PersistingBytes::write(*static_cast<uint8_t *>(args[0]));
      return;
    case SVC_PERSISTING_BYTES_READ:
      *static_cast<uint8_t *>(args[0]) = Ion::Device::PersistingBytes::read();
      return;
    // EVENTS
    case SVC_EVENTS_COPY_TEXT:
    {
      const char * text = static_cast<const Ion::Events::Event *>(args[0])->text();
      if (text) {
        *static_cast<size_t *>(args[2]) = strlcpy(static_cast<char *>(args[1]), text, *static_cast<size_t *>(args[2]));
      } else {
        *static_cast<size_t *>(args[2]) = 0;
      }
      return;
    }
    case SVC_EVENTS_GET_EVENT:
      *static_cast<Ion::Events::Event *>(args[1]) = Ion::Device::Events::getEvent(static_cast<int *>(args[0]));
      return;
    case SVC_EVENTS_IS_DEFINED:
      *static_cast<bool *>(args[1]) = static_cast<const Ion::Events::Event *>(args[0])->isDefined();
      return;
    case SVC_EVENTS_REPETITION_FACTOR:
      *static_cast<int *>(args[0]) = Ion::Events::repetitionFactor();
      return;
    case SVC_EVENTS_SET_SHIFT_ALPHA_STATUS:
      Ion::Events::setShiftAlphaStatus(*static_cast<Ion::Events::ShiftAlphaStatus *>(args[0]));
      return;
    case SVC_EVENTS_SHIFT_ALPHA_STATUS:
      *static_cast<Ion::Events::ShiftAlphaStatus *>(args[0]) = Ion::Events::shiftAlphaStatus();
      return;
    // POWER
    case SVC_POWER_STANDBY:
      Ion::Device::Power::standby();
      return;
    case SVC_POWER_SUSPEND:
      Ion::Device::Power::suspend(*static_cast<bool *>(args[0]));
      return;
    case SVC_LED_GET_COLOR:
      *static_cast<KDColor *>(args[0]) = Ion::Device::LED::getColor();
      return;
    case SVC_LED_SET_COLOR:
      Ion::Device::LED::setColor(*static_cast<KDColor *>(args[0]));
      return;
    case SVC_LED_SET_BLINKING:
      Ion::Device::LED::setBlinking(*static_cast<uint16_t *>(args[0]), *static_cast<float *>(args[1]));
      return;
    case SVC_LED_UPDATE_COLOR_WITH_PLUG_AND_CHARGE:
      *static_cast<KDColor *>(args[0]) = Ion::Device::LED::updateColorWithPlugAndCharge();
      return;
    // CRC32
    case SVC_CRC32_WORD:
      *static_cast<uint32_t *>(args[2]) = Ion::Device::crc32Word(static_cast<const uint32_t *>(args[0]), *static_cast<size_t *>(args[1]));
      return;
    case SVC_CRC32_BYTE:
      *static_cast<uint32_t *>(args[2]) = Ion::Device::crc32Byte(static_cast<const uint8_t *>(args[0]), *static_cast<size_t *>(args[1]));
      return;
    // SERIAL NUMBER
    case SVC_SERIAL_NUMBER:
      Ion::Device::SerialNumber::copy(static_cast<char *>(args[0]));
      return;
    // FCC_ID
    case SVC_FCC_ID:
      Ion::Device::fccId(static_cast<char *>(args[0]));
      return;
    // RANDOM
    case SVC_RANDOM:
      *static_cast<uint32_t *>(args[0]) = Ion::Device::random();
      return;
    // RESET
    case SVC_RESET_CORE:
      Ion::Device::Reset::coreWhilePlugged();
      return;
    // CIRCUIT BREAKER
    case SVC_CIRCUIT_BREAKER_HAS_CUSTOM_CHECKPOINT:
    {
      *static_cast<bool *>(args[0]) = Ion::Device::CircuitBreaker::hasCheckpoint(Ion::Device::CircuitBreaker::Checkpoint::Custom);
      return;
    }
    case SVC_CIRCUIT_BREAKER_SET_HOME_CHECKPOINT:
      Ion::Device::CircuitBreaker::setCheckpoint(Ion::Device::CircuitBreaker::Checkpoint::Home, false);
      return;
    case SVC_CIRCUIT_BREAKER_LOAD_CUSTOM_CHECKPOINT:
      Ion::Device::CircuitBreaker::loadCheckpoint(Ion::Device::CircuitBreaker::Checkpoint::Custom);
      return;
    case SVC_CIRCUIT_BREAKER_SET_CUSTOM_CHECKPOINT:
      *static_cast<bool *>(args[1]) = Ion::Device::CircuitBreaker::setCheckpoint(Ion::Device::CircuitBreaker::Checkpoint::Custom, *static_cast<bool *>(args[0]));
      return;
    case SVC_CIRCUIT_BREAKER_UNSET_CUSTOM_CHECKPOINT:
      Ion::Device::CircuitBreaker::unsetCustomCheckpoint();
      return;
    case SVC_CIRCUIT_BREAKER_RESET_CUSTOM_CHECKPOINT:
      Ion::Device::CircuitBreaker::resetCustomCheckpoint(static_cast<uint8_t * >(args[0]));
      return;
    case SVC_CIRCUIT_BREAKER_STORE_CUSTOM_CHECKPOINT:
      Ion::Device::CircuitBreaker::storeCustomCheckpoint(static_cast<uint8_t * >(args[0]));
      return;
    case SVC_CIRCUIT_BREAKER_STATUS:
      *static_cast<Ion::CircuitBreaker::Status *>(args[0]) = Ion::Device::CircuitBreaker::status();
      return;
    default:
      return;
  }
}
