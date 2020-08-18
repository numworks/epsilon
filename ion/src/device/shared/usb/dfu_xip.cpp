#include "calculator.h"
#include <drivers/svcall_handler.h>

namespace Ion {
namespace USB {

void DFU() {
  svc(SVC_DFU);
}

}
}
