#include <poincare/layout.h>
#include <poincare/old/complex.h>
#include <poincare/old/hyperbolic_arc_sine.h>
#include <poincare/old/serialization_helper.h>

#include <cmath>

namespace Poincare {

size_t HyperbolicArcSineNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      HyperbolicArcSine::s_functionHelper.aliasesList().mainAlias());
}

}  // namespace Poincare
