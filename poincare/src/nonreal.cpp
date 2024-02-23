#include <poincare/layout_helper.h>
#include <poincare/nonreal.h>
#include <poincare/symbol.h>

#include <algorithm>

extern "C" {
#include <math.h>
#include <string.h>
}

namespace Poincare {

bool NonrealNode::derivate(const ReductionContext& reductionContext,
                           Symbol symbol, OExpression symbolValue) {
  return true;
}

size_t NonrealNode::serialize(char* buffer, size_t bufferSize,
                              Preferences::PrintFloatMode floatDisplayMode,
                              int numberOfSignificantDigits) const {
  return std::min<size_t>(strlcpy(buffer, Nonreal::Name(), bufferSize),
                          bufferSize - 1);
}

}  // namespace Poincare
