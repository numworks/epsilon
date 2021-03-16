#include <ion.h>
#include <userland/drivers/svcall.h>

namespace Ion {

void SVC_ATTRIBUTES fccIdSVC(const char ** buffer) {
  SVC(SVC_FCC_ID);
}

const char * fccId() {
  const char * res;
  fccIdSVC(&res);
  return res;
}

}
