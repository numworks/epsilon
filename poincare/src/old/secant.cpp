#include <poincare/layout.h>
#include <poincare/old/complex.h>
#include <poincare/old/cosine.h>
#include <poincare/old/secant.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/trigonometry.h>

#include <cmath>

namespace Poincare {

int SecantNode::numberOfChildren() const {
  return Secant::s_functionHelper.numberOfChildren();
}

size_t SecantNode::serialize(char* buffer, size_t bufferSize,
                             Preferences::PrintFloatMode floatDisplayMode,
                             int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      Secant::s_functionHelper.aliasesList().mainAlias());
}

// TODO_PCJ: Delete this method
OExpression SecantNode::shallowReduce(
    const ReductionContext& reductionContext) {
  assert(false);
  return this;
  // Secant e = Secant(this);
  // return Trigonometry::ShallowReduceAdvancedFunction(e, reductionContext);
}

}  // namespace Poincare
