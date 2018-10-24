#include <poincare/real_part.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <assert.h>
#include <cmath>

namespace Poincare {

int RealPartNode::numberOfChildren() const { return RealPart::FunctionHelper()->numberOfChildren(); }

Layout RealPartNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(RealPart(this), floatDisplayMode, numberOfSignificantDigits, RealPart::FunctionHelper()->name());
}

int RealPartNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, RealPart::FunctionHelper()->name());
}

Expression RealPartNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols) {
  return RealPart(this).shallowReduce(context, angleUnit, replaceSymbols);
}

Expression RealPart::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols) {
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
    replaceWithInPlace(c);
    return c;
  }
  return *this;
}

constexpr Expression::FunctionHelper RealPart::m_functionHelper = Expression::FunctionHelper("re", 1, &RealPart::UntypedBuilder);

}
