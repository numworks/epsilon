#include "serialized_expression.h"

namespace Poincare {

void SerializedExpression::writeText(
    std::span<char> buffer, int numberOfSignificantDigits,
    Preferences::PrintFloatMode floatDisplayMode) const {
  [[maybe_unused]] size_t usedLength =
      expression().serialize(buffer.data(), buffer.size(), floatDisplayMode,
                             numberOfSignificantDigits);
  assert(usedLength <= buffer.size());
}

}  // namespace Poincare
