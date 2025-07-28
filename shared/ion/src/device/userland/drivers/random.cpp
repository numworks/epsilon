#include <ion.h>

#include "svcall.h"

namespace Ion {

uint32_t SVC_ATTRIBUTES random() { SVC_RETURNING_R0(SVC_RANDOM, uint32_t) }

}  // namespace Ion
