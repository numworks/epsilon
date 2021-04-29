#include <ion.h>
#include <userland/drivers/svcall.h>

namespace Ion {

const char * SVC_ATTRIBUTES pcbVersion() {
  SVC_RETURNING_R0(SVC_PCB_VERSION, const char *)
}

}
