#ifndef POINCARE_LAYOUT_RACK_FROM_TEXT_H
#define POINCARE_LAYOUT_RACK_FROM_TEXT_H

#include "rack.h"

namespace Poincare::Internal {

Rack* RackFromText(const char* start, const char* end = nullptr);

}  // namespace Poincare::Internal

#endif
