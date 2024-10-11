#include <assert.h>
#include <ion.h>
#include <omg/ieee754.h>
#include <poincare/layout.h>
#include <poincare/old/complex.h>
#include <poincare/old/random.h>
#include <poincare/old/serialization_helper.h>

#include <cmath>

namespace Poincare {

int RandomNode::numberOfChildren() const {
  return Random::s_functionHelper.numberOfChildren();
}

size_t RandomNode::serialize(char *buffer, size_t bufferSize,
                             Preferences::PrintFloatMode floatDisplayMode,
                             int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      Random::s_functionHelper.aliasesList().mainAlias());
}

}  // namespace Poincare
