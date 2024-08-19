#include <poincare/layout.h>
#include <poincare/old/arc_cosine.h>
#include <poincare/old/arc_secant.h>
#include <poincare/old/complex.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/trigonometry.h>

#include <cmath>

namespace Poincare {

int ArcSecantNode::numberOfChildren() const {
  return ArcSecant::s_functionHelper.numberOfChildren();
}

size_t ArcSecantNode::serialize(char* buffer, size_t bufferSize,
                                Preferences::PrintFloatMode floatDisplayMode,
                                int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      ArcSecant::s_functionHelper.aliasesList().mainAlias());
}

}  // namespace Poincare
