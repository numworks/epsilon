#include <poincare/layout_helper.h>
#include <poincare/point.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

Layout PointNode::createLayout(Preferences::PrintFloatMode floatDisplayMode,
                               int significantDigits, Context* context) const {
  return LayoutHelper::Prefix(Point(this), floatDisplayMode, significantDigits,
                              k_prefix, context);
}

int PointNode::serialize(char* buffer, int bufferSize,
                         Preferences::PrintFloatMode floatDisplayMode,
                         int significantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode,
                                     significantDigits, k_prefix);
}

}  // namespace Poincare
