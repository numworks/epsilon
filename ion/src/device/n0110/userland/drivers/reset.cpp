#include <userland/drivers/reset.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace Reset {

void SVC_ATTRIBUTES core() {
  SVC_RETURNING_VOID(SVC_RESET_CORE)
}

}
}
