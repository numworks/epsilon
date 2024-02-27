#include <poincare/layout_helper.h>
#include <poincare/nonreal.h>
#include <poincare/symbol.h>

#include <algorithm>

extern "C" {
#include <math.h>
#include <string.h>
}

namespace Poincare {

OLayout NonrealNode::createLayout(Preferences::PrintFloatMode floatDisplayMode,
                                  int numberOfSignificantDigits,
                                  Context* context) const {
  return LayoutHelper::String(Nonreal::Name(), Nonreal::NameSize() - 1);
}

bool NonrealNode::derivate(const ReductionContext& reductionContext,
                           Symbol symbol, Expression symbolValue) {
  return true;
}

size_t NonrealNode::serialize(char* buffer, size_t bufferSize,
                              Preferences::PrintFloatMode floatDisplayMode,
                              int numberOfSignificantDigits) const {
  return std::min<size_t>(strlcpy(buffer, Nonreal::Name(), bufferSize),
                          bufferSize - 1);
}

}  // namespace Poincare
