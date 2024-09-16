#include <poincare/layout.h>

#include <algorithm>

namespace Poincare {

size_t ProductLayoutNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SequenceLayoutNode::writeDerivedClassInBuffer(
      "product", buffer, bufferSize, floatDisplayMode,
      numberOfSignificantDigits);
}

}  // namespace Poincare
