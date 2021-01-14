#include <kernel/boot/isr.h>
#include <kernel/drivers/backlight.h>
#include <kernel/drivers/battery.h>
#include <kernel/drivers/display.h>
#include <kernel/drivers/events_keyboard_platform.h>
#include <kernel/drivers/keyboard.h>
#include <kernel/drivers/led.h>
#include <kernel/drivers/persisting_bytes.h>
#include <kernel/drivers/power.h>
#include <kernel/drivers/timing.h>
#include <shared/drivers/svcall.h>
#include <shared/drivers/usb.h>

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
    case SVC_KEYBOARD_HAS_NEXT_STATE:
      *static_cast<bool *>(args[0]) = Ion::Device::Keyboard::hasNextState();
      return;
    case SVC_KEYBOARD_NEXT_STATE:
      *static_cast<Ion::Keyboard::State *>(args[0]) = Ion::Device::Keyboard::nextState();
      return;
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
    case SVC_EVENTS_GET_PLATFORM_EVENT:
      *static_cast<Ion::Events::Event *>(args[0]) = Ion::Device::Events::getPlatformEvent();
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
    default:
      return;
  }
}
