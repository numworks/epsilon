#include <ion.h>
#include <userland/drivers/svcall.h>

namespace Ion {

const char * SVC_ATTRIBUTES fccId() {
  SVC(SVC_FCC_ID);
}

}
