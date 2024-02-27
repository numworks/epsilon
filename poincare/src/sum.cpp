#include <poincare/addition.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/sum.h>
#include <poincare/sum_layout.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <cmath>

namespace Poincare {

constexpr OExpression::FunctionHelper Sum::s_functionHelper;

size_t SumNode::serialize(char* buffer, size_t bufferSize,
                          Preferences::PrintFloatMode floatDisplayMode,
                          int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      Sum::s_functionHelper.aliasesList().mainAlias());
}

OExpression Sum::UntypedBuilder(OExpression children) {
  assert(children.type() == ExpressionNode::Type::OList);
  if (children.childAtIndex(1).type() != ExpressionNode::Type::Symbol) {
    // Second parameter must be a Symbol.
    return OExpression();
  }
  return Builder(children.childAtIndex(0),
                 children.childAtIndex(1).convert<Symbol>(),
                 children.childAtIndex(2), children.childAtIndex(3));
}

}  // namespace Poincare
