#include <ion.h>
#include <userland/drivers/svcall.h>

namespace Ion {

uint32_t SVC_ATTRIBUTES random() {
  SVC(SVC_RANDOM);
}

}
