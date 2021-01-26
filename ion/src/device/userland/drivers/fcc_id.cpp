#include <ion.h>
#include <userland/drivers/svcall.h>

namespace Ion {

void SVC_ATTRIBUTES fccIdSVC(char * buffer) {
  SVC(SVC_FCC_ID);
}

void fccId(char * buffer) {
  fccIdSVC(buffer);
}

}
