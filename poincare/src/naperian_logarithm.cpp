#include <poincare/naperian_logarithm.h>
#include <poincare/constant.h>
#include <poincare/logarithm.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>


namespace Poincare {

constexpr Expression::FunctionHelper NaperianLogarithm::s_functionHelper;

int NaperianLogarithmNode::numberOfChildren() const { return NaperianLogarithm::s_functionHelper.numberOfChildren(); }

Layout NaperianLogarithmNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, NaperianLogarithm::s_functionHelper.name());
}
int NaperianLogarithmNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, NaperianLogarithm::s_functionHelper.name());
}

Expression NaperianLogarithmNode::shallowReduce(ReductionContext reductionContext) {
  return NaperianLogarithm(this).shallowReduce(reductionContext);
}


Expression NaperianLogarithm::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = Expression::defaultShallowReduce();
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression c = childAtIndex(0);
  if (c.type() == ExpressionNode::Type::Matrix) {
    return mapOnMatrixChild(reductionContext);
  }
  Logarithm l = Logarithm::Builder(c, Constant::Builder(UCodePointScriptSmallE));
  replaceWithInPlace(l);
  return l.shallowReduce(reductionContext);
}

}
