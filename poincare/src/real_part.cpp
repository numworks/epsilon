#include <poincare/real_part.h>
#include <poincare/complex_cartesian.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>

#include <assert.h>
#include <cmath>

namespace Poincare {

constexpr Expression::FunctionHelper RealPart::s_functionHelper;

int RealPartNode::numberOfChildren() const { return RealPart::s_functionHelper.numberOfChildren(); }

Layout RealPartNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(RealPart(this), floatDisplayMode, numberOfSignificantDigits, RealPart::s_functionHelper.name());
}

int RealPartNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, RealPart::s_functionHelper.name());
}

Expression RealPartNode::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target, bool symbolicComputation) {
  return RealPart(this).shallowReduce(context, complexFormat, angleUnit, target, symbolicComputation);
}

Expression RealPart::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target, bool symbolicComputation) {
  {
    Expression e = Expression::defaultShallowReduce();
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression c = childAtIndex(0);
  if (c.type() == ExpressionNode::Type::Matrix) {
    return mapOnMatrixChild(context, complexFormat, angleUnit, target, symbolicComputation);
  }
  if (c.isReal(context)) {
    replaceWithInPlace(c);
    return c;
  }
  if (c.type() == ExpressionNode::Type::ComplexCartesian) {
    ComplexCartesian complexChild = static_cast<ComplexCartesian &>(c);
    Expression r = complexChild.real();
    replaceWithInPlace(r);
    return r.shallowReduce(context, complexFormat, angleUnit, target);
  }
  return *this;
}

}
