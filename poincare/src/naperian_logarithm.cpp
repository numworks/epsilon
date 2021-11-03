#include <poincare/naperian_logarithm.h>
#include <poincare/constant.h>
#include <poincare/logarithm.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>


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
    Expression e = SimplificationHelper::defaultShallowReduce(*this);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  Logarithm l = Logarithm::Builder(childAtIndex(0), Constant::Builder("ℯ"));
  replaceWithInPlace(l);
  return l.shallowReduce(reductionContext);
}

}
