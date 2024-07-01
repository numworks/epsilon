#include <poincare/layout.h>
#include <poincare/old/addition.h>
#include <poincare/old/matrix.h>
#include <poincare/old/matrix_trace.h>
#include <poincare/old/rational.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/undefined.h>

#include <cmath>
#include <utility>

namespace Poincare {

int MatrixTraceNode::numberOfChildren() const {
  return MatrixTrace::s_functionHelper.numberOfChildren();
}

OExpression MatrixTraceNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return MatrixTrace(this).shallowReduce(reductionContext);
}

size_t MatrixTraceNode::serialize(char* buffer, size_t bufferSize,
                                  Preferences::PrintFloatMode floatDisplayMode,
                                  int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      MatrixTrace::s_functionHelper.aliasesList().mainAlias());
}

OExpression MatrixTrace::shallowReduce(ReductionContext reductionContext) {
  {
    OExpression e = SimplificationHelper::defaultShallowReduce(
        *this, &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::BanUnits);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  OExpression c = childAtIndex(0);
  if (c.otype() == ExpressionNode::Type::OMatrix) {
    OMatrix matrixChild0 = static_cast<OMatrix&>(c);
    if (matrixChild0.numberOfRows() != matrixChild0.numberOfColumns()) {
      return replaceWithUndefinedInPlace();
    }
    OExpression a = matrixChild0.createTrace();
    replaceWithInPlace(a);
    return a.shallowReduce(reductionContext);
  }
  if (c.deepIsMatrix(reductionContext.context(),
                     reductionContext.shouldCheckMatrices())) {
    return *this;
  }
  replaceWithInPlace(c);
  return c;
}

}  // namespace Poincare
