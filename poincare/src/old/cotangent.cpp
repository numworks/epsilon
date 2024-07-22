#include <poincare/layout.h>
#include <poincare/old/complex.h>
#include <poincare/old/cosine.h>
#include <poincare/old/cotangent.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/sine.h>
#include <poincare/old/trigonometry.h>

#include <cmath>

namespace Poincare {

int CotangentNode::numberOfChildren() const {
  return Cotangent::s_functionHelper.numberOfChildren();
}

size_t CotangentNode::serialize(char* buffer, size_t bufferSize,
                                Preferences::PrintFloatMode floatDisplayMode,
                                int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      Cotangent::s_functionHelper.aliasesList().mainAlias());
}

// TODO_PCJ: Delete this method
OExpression CotangentNode::shallowReduce(
    const ReductionContext& reductionContext) {
  assert(false);
  return this;
  // Cotangent e = Cotangent(this);
  // return Trigonometry::ShallowReduceAdvancedFunction(e, reductionContext);
}

}  // namespace Poincare
