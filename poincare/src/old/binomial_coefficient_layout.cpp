#include <assert.h>
#include <poincare/layout.h>
#include <poincare/old/binomial_coefficient.h>
#include <poincare/old/serialization_helper.h>

#include <algorithm>

namespace Poincare {

size_t BinomialCoefficientLayoutNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      BinomialCoefficient::s_functionHelper.aliasesList().mainAlias(),
      SerializationHelper::ParenthesisType::System);
}

}  // namespace Poincare
