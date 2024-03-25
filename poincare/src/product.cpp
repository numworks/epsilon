#include <poincare/layout.h>
#include <poincare/multiplication.h>
#include <poincare/product.h>
#include <poincare/serialization_helper.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <cmath>

namespace Poincare {

constexpr OExpression::FunctionHelper Product::s_functionHelper;

size_t ProductNode::serialize(char* buffer, size_t bufferSize,
                              Preferences::PrintFloatMode floatDisplayMode,
                              int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      Product::s_functionHelper.aliasesList().mainAlias());
}

OExpression Product::UntypedBuilder(OExpression children) {
  assert(children.otype() == ExpressionNode::Type::OList);
  if (children.childAtIndex(1).otype() != ExpressionNode::Type::Symbol) {
    // Second parameter must be a Symbol.
    return OExpression();
  }
  return Builder(children.childAtIndex(0),
                 children.childAtIndex(1).convert<Symbol>(),
                 children.childAtIndex(2), children.childAtIndex(3));
}

}  // namespace Poincare
