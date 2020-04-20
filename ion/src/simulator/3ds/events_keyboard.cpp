#include "main.h"
#include "platform.h"

#include <assert.h>
#include <ion/events.h>
#include <string.h>

#include <3ds.h>


namespace Ion {
namespace Events {


Event getPlatformEvent() {
  Event result = None;
  

  if (!aptMainLoop()) {
    result = Termination;
  }
  
  return result;
}

}
}
