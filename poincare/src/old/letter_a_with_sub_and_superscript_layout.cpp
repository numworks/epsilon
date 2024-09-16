#include <poincare/layout.h>
#include <poincare/old/permute_coefficient.h>
#include <poincare/old/serialization_helper.h>

namespace Poincare {

size_t LetterAWithSubAndSuperscriptLayoutNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      PermuteCoefficient::s_functionHelper.aliasesList().mainAlias(),
      SerializationHelper::ParenthesisType::System);
}

}  // namespace Poincare
