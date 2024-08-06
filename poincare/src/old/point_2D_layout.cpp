#include <assert.h>
#include <poincare/layout.h>
#include <poincare/old/point.h>
#include <poincare/old/serialization_helper.h>

#include <algorithm>

namespace Poincare {

size_t Point2DLayoutNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode,
                                     numberOfSignificantDigits,
                                     PointNode::k_prefix);
}

}  // namespace Poincare
