#include <assert.h>
#include <escher/metric.h>
#include <poincare/layout.h>
#include <poincare/old/conjugate.h>
#include <poincare/old/serialization_helper.h>

namespace Poincare {

size_t ConjugateLayoutNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      Conjugate::s_functionHelper.aliasesList().mainAlias(),
      SerializationHelper::ParenthesisType::System);
}

}  // namespace Poincare
