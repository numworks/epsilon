#include <poincare/imaginary_part.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>

#include <poincare/complex_cartesian.h>
#include <poincare/rational.h>
#include <cmath>

namespace Poincare {

constexpr Expression::FunctionHelper ImaginaryPart::s_functionHelper;

int ImaginaryPartNode::numberOfChildren() const { return ImaginaryPart::s_functionHelper.numberOfChildren(); }

Layout ImaginaryPartNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(ImaginaryPart(this), floatDisplayMode, numberOfSignificantDigits, ImaginaryPart::s_functionHelper.name());
}

int ImaginaryPartNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, ImaginaryPart::s_functionHelper.name());
}

Expression ImaginaryPartNode::shallowReduce(ReductionContext reductionContext) {
  return ImaginaryPart(this).shallowReduce(reductionContext);
}

Expression ImaginaryPart::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = Expression::defaultShallowReduce();
    e = e.defaultHandleUnitsInChildren();
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression c = childAtIndex(0);
  if (c.type() == ExpressionNode::Type::Matrix) {
    return mapOnMatrixFirstChild(reductionContext);
  }
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
