#include <kernel/boot/isr.h>
#include <kernel/drivers/display.h>
#include <shared/drivers/svcall.h>

void __attribute__((interrupt, noinline)) svcall_handler(unsigned int * args) {
  unsigned int svcNumber = (( char * )args[ 6 ] )[ -2 ];
  void * arguments[2];
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
    case SVC_DFU:
      Ion::Device::USB::DFUHandler();
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
    case SVC_PUSH_RECT:
      // Load rect and pixels
      Ion::Device::SVCall::getSvcallArgs(2, arguments);
      Ion::Device::Display::pushRect(
          *static_cast<KDRect *>(arguments[0]),
          *static_cast<const KDColor **>(arguments[1])
        );
      return;
    case SVC_PUSH_RECT_UNIFORM:
      // Load rect and color
      Ion::Device::SVCall::getSvcallArgs(2, arguments);
      Ion::Device::Display::pushRectUniform(
          *static_cast<KDRect *>(arguments[0]),
          *static_cast<KDColor *>(arguments[1])
        );
      return;
    case SVC_PULL_RECT:
      // Load rect and pixels
      Ion::Device::SVCall::getSvcallArgs(2, arguments);
      Ion::Device::Display::pullRect(
          *static_cast<KDRect *>(arguments[0]),
          *static_cast<KDColor **>(arguments[1])
        );
      return;
    case SVC_POST_PUSH_MULTICOLOR:
      // Load rootNumberTiles and tileSize
      Ion::Device::SVCall::getSvcallArgs(2, arguments);
      Ion::Device::Display::POSTPushMulticolor(
          *static_cast<int *>(arguments[0]),
          *static_cast<int *>(arguments[1])
        );
      return;
    default:
      return;
  }
}
