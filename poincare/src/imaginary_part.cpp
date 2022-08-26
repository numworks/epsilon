#include <poincare/imaginary_part.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/complex_cartesian.h>
#include <poincare/rational.h>
#include <cmath>

namespace Poincare {

int ImaginaryPartNode::numberOfChildren() const { return ImaginaryPart::s_functionHelper.numberOfChildren(); }

Layout ImaginaryPartNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  return LayoutHelper::Prefix(ImaginaryPart(this), floatDisplayMode, numberOfSignificantDigits, ImaginaryPart::s_functionHelper.aliasesList().mainAlias(), context);
}

int ImaginaryPartNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, ImaginaryPart::s_functionHelper.aliasesList().mainAlias());
}

Expression ImaginaryPartNode::shallowReduce(const ReductionContext& reductionContext) {
  return ImaginaryPart(this).shallowReduce(reductionContext);
}

Expression ImaginaryPart::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(
        *this,
        &reductionContext,
        SimplificationHelper::UnitReduction::BanUnits,
        SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
        SimplificationHelper::ListReduction::DistributeOverLists
    );
    if (!e.isUninitialized()) {
      return e;
    }
  }
  Expression c = childAtIndex(0);
  if (c.isReal(reductionContext.context())) {
    Expression result = Rational::Builder(0);
    replaceWithInPlace(result);
    return result;
  }
  if (c.type() == ExpressionNode::Type::ComplexCartesian) {
    ComplexCartesian complexChild = static_cast<ComplexCartesian &>(c);
    Expression i = complexChild.imag();
    replaceWithInPlace(i);
    return i.shallowReduce(reductionContext);
  }
  return *this;
}

}
