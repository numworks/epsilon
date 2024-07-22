#include <poincare/layout.h>
#include <poincare/old/complex.h>
#include <poincare/old/cosecant.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/sine.h>
#include <poincare/old/trigonometry.h>

#include <cmath>

namespace Poincare {

int CosecantNode::numberOfChildren() const {
  return Cosecant::s_functionHelper.numberOfChildren();
}

size_t CosecantNode::serialize(char* buffer, size_t bufferSize,
                               Preferences::PrintFloatMode floatDisplayMode,
                               int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      Cosecant::s_functionHelper.aliasesList().mainAlias());
}

// TODO_PCJ: Delete this method
OExpression CosecantNode::shallowReduce(
    const ReductionContext& reductionContext) {
  assert(false);
  return this;
  // Cosecant e = Cosecant(this);
  // return Trigonometry::ShallowReduceAdvancedFunction(e, reductionContext);
}

}  // namespace Poincare
