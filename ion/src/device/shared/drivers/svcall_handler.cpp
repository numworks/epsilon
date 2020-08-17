#include "svcall_handler.h"
#include <drivers/timing.h>
#include <drivers/power.h>
#include <drivers/config/clocks.h>

extern "C" {
void __attribute__((interrupt, noinline)) svcall_handler(unsigned int * args) {
  unsigned int svcNumber = (( char * )args[ 6 ] )[ -2 ] ;
  switch (svcNumber) {
    case SVC_SYSTICK_HIGH_FREQUENCY:
      Ion::Device::Timing::sysTickFrequencyHandler(Ion::Device::Clocks::Config::HCLKFrequency);
      return;
    case SVC_SYSTICK_LOW_FREQUENCY:
      Ion::Device::Timing::sysTickFrequencyHandler(Ion::Device::Clocks::Config::HCLKLowFrequency);
      return;
    case SVC_POWER_STANDBY:
      Ion::Device::Power::standbyHandler();
      return;
    case SVC_POWER_SLEEP_OR_STOP:
      Ion::Device::Power::standbyHandler();
      return;
    default:
      return;
  }
}
}
