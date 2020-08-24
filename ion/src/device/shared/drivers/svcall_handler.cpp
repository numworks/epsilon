#include "svcall.h"
#include <drivers/board_privileged.h>
#include <drivers/power_privileged.h>
#include <drivers/reset_privileged.h>
#include <ion/src/device/shared/usb/dfu_privileged.h>

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
    default:
      return;
  }
}
}
