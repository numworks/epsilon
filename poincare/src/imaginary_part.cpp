#include <poincare/imaginary_part.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/rational.h>
#include <cmath>

namespace Poincare {

int ImaginaryPartNode::numberOfChildren() const { return ImaginaryPart::NumberOfChildren(); }

Layout ImaginaryPartNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(ImaginaryPart(this), floatDisplayMode, numberOfSignificantDigits, ImaginaryPart::Name());
}

int ImaginaryPartNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, ImaginaryPart::Name());
}

Expression ImaginaryPartNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols) {
  return ImaginaryPart(this).shallowReduce(context, angleUnit, replaceSymbols);
}

ImaginaryPart::ImaginaryPart() : Expression(TreePool::sharedPool()->createTreeNode<ImaginaryPartNode>()) {}

Expression ImaginaryPart::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols) {
  {
    Expression e = Expression::defaultShallowReduce(context, angleUnit);
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
  if (c.type() == ExpressionNode::Type::Rational) {
    Expression result = Rational(0);
    replaceWithInPlace(result);
    return result;
  }
  return *this;
}

}
