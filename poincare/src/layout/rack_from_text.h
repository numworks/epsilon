#pragma once

#include "rack.h"

namespace Poincare::Internal {

Rack* RackFromText(const char* start, const char* end = nullptr);

}  // namespace Poincare::Internal
