#include "serialized_expression.h"

namespace Poincare {

void SerializedExpression::writeText(
    std::span<char> buffer, int numberOfSignificantDigits,
    Preferences::PrintFloatMode floatDisplayMode) const {
  /*  Note: m_buffer is just an internal storage, but it does not have the
   * requested number of significant digits or display mode. It should thus not
   * be returned directly. The expression is re-constructed, then serialized
   * with the requested display parameters. */
  [[maybe_unused]] size_t usedLength =
      expression().serialize(buffer.data(), buffer.size(), floatDisplayMode,
                             numberOfSignificantDigits);
  assert(usedLength <= buffer.size());
}

}  // namespace Poincare
