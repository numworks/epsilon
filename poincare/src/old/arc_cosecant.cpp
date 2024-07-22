#include <poincare/layout.h>
#include <poincare/old/arc_cosecant.h>
#include <poincare/old/arc_sine.h>
#include <poincare/old/complex.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/trigonometry.h>

#include <cmath>

namespace Poincare {

int ArcCosecantNode::numberOfChildren() const {
  return ArcCosecant::s_functionHelper.numberOfChildren();
}

size_t ArcCosecantNode::serialize(char* buffer, size_t bufferSize,
                                  Preferences::PrintFloatMode floatDisplayMode,
                                  int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      ArcCosecant::s_functionHelper.aliasesList().mainAlias());
}

// TODO_PCJ: Delete this method
OExpression ArcCosecantNode::shallowReduce(
    const ReductionContext& reductionContext) {
  assert(false);
  return this;
  // ArcCosecant e = ArcCosecant(this);
  // return Trigonometry::ShallowReduceInverseAdvancedFunction(e,
  //                                                           reductionContext);
}

}  // namespace Poincare
