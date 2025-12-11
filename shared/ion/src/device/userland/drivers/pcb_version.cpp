#include <ion.h>

#include "svcall.h"

namespace Ion {

uint32_t SVC_ATTRIBUTES devicePcbVersion() {
  SVC_RETURNING_R0(SVC_PCB_VERSION, uint32_t)
}

}  // namespace Ion
