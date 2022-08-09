#include <poincare/real_part.h>
#include <poincare/complex_cartesian.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>

#include <assert.h>
#include <cmath>

namespace Poincare {

int RealPartNode::numberOfChildren() const { return RealPart::s_functionHelper.numberOfChildren(); }

Layout RealPartNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  return LayoutHelper::Prefix(RealPart(this), floatDisplayMode, numberOfSignificantDigits, RealPart::s_functionHelper.aliasesList().mainAlias(), context);
}

int RealPartNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, RealPart::s_functionHelper.aliasesList().mainAlias());
}

Expression RealPartNode::shallowReduce(const ReductionContext& reductionContext) {
  return RealPart(this).shallowReduce(reductionContext);
}

Expression RealPart::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(
        *this,
        &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::BanUnits,
        SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
        SimplificationHelper::ListReduction::DistributeOverLists
    );
    if (!e.isUninitialized()) {
      return e;
    }
  }
  Expression c = childAtIndex(0);
  if (c.isReal(reductionContext.context(), reductionContext.shouldCheckMatrices())) {
    replaceWithInPlace(c);
    return c;
  }
  if (c.type() == ExpressionNode::Type::ComplexCartesian) {
    ComplexCartesian complexChild = static_cast<ComplexCartesian &>(c);
    Expression r = complexChild.real();
    replaceWithInPlace(r);
    return r.shallowReduce(reductionContext);
  }
  return *this;
}

}
