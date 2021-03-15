#include "main.h"
#include "platform.h"
#include "driver/common.h"

#include <assert.h>
#include <ion/events.h>
#include <string.h>

#include <3ds.h>

static bool was_plugged = false;

namespace Ion {
namespace Events {


Event getPlatformEvent() {
  Event result = None;
  
  if (Ion::Simulator::CommonDriver::isPlugged() && !was_plugged) {
    was_plugged = true;
    return USBPlug;
  }
  
  if (!Ion::Simulator::CommonDriver::isPlugged() && was_plugged) {
    was_plugged = false;
  }
  

  if (!aptMainLoop()) {
    result = Termination;
  }
  
  return result;
}

}
}
