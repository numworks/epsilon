#include <kernel/boot/isr.h>
#include <kernel/drivers/display.h>
#include <shared/drivers/svcall.h>
#include <shared/drivers/usb.h>

//https://developer.arm.com/documentation/dui0471/m/handling-processor-exceptions/svc-handlers-in-c-and-assembly-language

void svcall_handler(unsigned svcNumber, void * args[]) {
  switch (svcNumber) {
    /*case SVC_CLOCK_STANDARD_FREQUENCY:
      Ion::Device::Board::setStandardClockFrequencyHandler();
      return;
    case SVC_CLOCK_LOW_FREQUENCY:
      Ion::Device::Board::setLowClockFrequencyHandler();
      return;
    case SVC_POWER_STANDBY:
      Ion::Device::Power::standbyHandler();
      return;
    case SVC_POWER_SLEEP_OR_STOP:
      Ion::Device::Power::sleepStopHandler();
      return;
    case SVC_RESET_CORE:
      Ion::Device::Reset::coreHandler();
      return;
    case SVC_EXAM_MODE_TOGGLE:
      // TODO EMILIE:
      // We probably want to change the API here to something like:
      // case SVC_EXTERNAL_FLASH_WRITE:
      //  Ion::Device::ExamMode::externalFlashWrite((uint8_t *)args[0], (uint8_t *)args[1]);
      //  But I haven't fully understood passing args to SVChandler yet - the previous code fails with optim...
      Ion::Device::ExamMode::ToggleExamMode();
      return;*/
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
    case SVC_DISPLAY_POST_PUSH_MULTICOLOR:
      // Load rootNumberTiles and tileSize
      Ion::Device::Display::POSTPushMulticolor(
          *static_cast<int *>(args[0]),
          *static_cast<int *>(args[1])
        );
      return;
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
    default:
      return;
  }
}
