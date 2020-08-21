#include "reset.h"
#include "svcall.h"

namespace Ion {
namespace Device {
namespace Reset {

void core() {
  svc(SVC_RESET_CORE);
}

}
}
}
