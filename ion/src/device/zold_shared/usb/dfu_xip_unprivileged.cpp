#include <ion/usb.h>
#include <drivers/svcall.h>

namespace Ion {
namespace USB {

void DFU() {
  svc(SVC_DFU);
}

}
}
