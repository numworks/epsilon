#include "svcall.h"
#include <drivers/board_privileged.h>
#include <drivers/power_privileged.h>
#include <drivers/reset_privileged.h>
#include <ion/src/device/shared/usb/dfu_privileged.h>
#include <drivers/exam_mode_privileged.h>
#include <drivers/display_privileged.h>

extern "C" {
void __attribute__((interrupt, noinline)) svcall_handler(unsigned int * args) {
  unsigned int svcNumber = (( char * )args[ 6 ] )[ -2 ] ;
  switch (svcNumber) {
    case SVC_CLOCK_STANDARD_FREQUENCY:
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
    case SVC_PERSIST_BYTE:
      // TODO EMILIE:
      // We probably want to change the API here to something like:
      // case SVC_EXTERNAL_FLASH_WRITE:
      //  Ion::Device::ExamMode::externalFlashWrite((uint8_t *)args[0], (uint8_t *)args[1]);
      //  But I haven't fully understood passing args to SVChandler yet - the previous code fails with optim...
      Ion::Device::Memory::PersistByteHandler();
      return;
    case SVC_PUSH_RECT:
      Ion::Device::Display::pushRectSVC();
      return;
    case SVC_PUSH_RECT_UNIFORM:
      Ion::Device::Display::pushRectUniformSVC();
      return;
    case SVC_PULL_RECT:
      Ion::Device::Display::pullRectSVC();
      return;
    case SVC_POST_PUSH_MULTICOLOR:
      Ion::Device::Display::POSTPushMulticolorSVC();
      return;
    case SVC_READ_PERSISTED_BYTE:
      return Ion::Device::Memory::PersistedByte::Handler();
    default:
      return;
  }
}
}
