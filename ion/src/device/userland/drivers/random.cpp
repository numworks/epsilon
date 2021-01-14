#include <ion.h>
#include <userland/drivers/svcall.h>

namespace Ion {

void randomSVC(uint32_t * res) {
  SVC(SVC_RANDOM);
}

uint32_t random() {
  uint32_t res;
  randomSVC(&res);
  return res;
}

}
