#include <escher/metric.h>
#include <poincare/layout.h>
#include <poincare/old/derivative.h>
#include <poincare/old/serialization_helper.h>

#include <algorithm>

namespace Poincare {

size_t DerivativeLayoutNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      Derivative::s_functionHelper.aliasesList().mainAlias(),
      SerializationHelper::ParenthesisType::System);
}

}  // namespace Poincare
