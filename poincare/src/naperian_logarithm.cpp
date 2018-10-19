#include <poincare/naperian_logarithm.h>
#include <poincare/constant.h>
#include <poincare/logarithm.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>

namespace Poincare {

int NaperianLogarithmNode::numberOfChildren() const { return NaperianLogarithm::NumberOfChildren(); }

Layout NaperianLogarithmNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, NaperianLogarithm::Name());
}
int NaperianLogarithmNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, NaperianLogarithm::Name());
}

Expression NaperianLogarithmNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols) {
  return NaperianLogarithm(this).shallowReduce(context, angleUnit, replaceSymbols);
}

NaperianLogarithm::NaperianLogarithm() : Expression(TreePool::sharedPool()->createTreeNode<NaperianLogarithmNode>()) {}

Expression NaperianLogarithm::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols) {
  {
    Expression e = Expression::defaultShallowReduce(context, angleUnit);
    if (e.isUndefined()) {
      return e;
    }
  }
#if MATRIX_EXACT_REDUCING
  if (childAtIndex(0).type() == ExpressionNode::Type::Matrix) {
    return SimplificationHelper::Map(*this, context, angleUnit);
  }
#endif
  Logarithm l = Logarithm(childAtIndex(0), Constant(Ion::Charset::Exponential));
  replaceWithInPlace(l);
  return l.shallowReduce(context, angleUnit);
}

}
