#include <poincare/imaginary_part.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
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

Expression ImaginaryPartNode::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) {
  return ImaginaryPart(this).shallowReduce(context, complexFormat, angleUnit, target);
}

ImaginaryPart ImaginaryPart::Builder(Expression child) {
  void * bufferNode = TreePool::sharedPool()->alloc(sizeof(ImaginaryPartNode));
  ImaginaryPartNode * node = new (bufferNode) ImaginaryPartNode();
  TreeHandle h = TreeHandle::BuildWithBasicChildren(node, &child, 1);
  return static_cast<ImaginaryPart &>(h);
}

Expression ImaginaryPart::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  {
    Expression e = Expression::defaultShallowReduce();
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression c = childAtIndex(0);
#if MATRIX_EXACT_REDUCING
  if (c.type() == ExpressionNode::Type::Matrix) {
    return SimplificationHelper::Map(*this, context, angleUnit);
  }
#endif
  if (c.isReal(context)) {
    Expression result = Rational::Builder(0);
    replaceWithInPlace(result);
    return result;
  }
  if (c.type() == ExpressionNode::Type::ComplexCartesian) {
    ComplexCartesian complexChild = static_cast<ComplexCartesian &>(c);
    Expression i = complexChild.imag();
    replaceWithInPlace(i);
    return i.shallowReduce(context, complexFormat, angleUnit, target);
  }
  return *this;
}

}
