#include <poincare/product.h>
#include <poincare/multiplication.h>
#include <poincare/product_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <cmath>

namespace Poincare {

constexpr Expression::FunctionHelper Product::s_functionHelper;

Layout ProductNode::createSumAndProductLayout(Layout argumentLayout, Layout symbolLayout, Layout subscriptLayout, Layout superscriptLayout) const {
  return ProductLayout::Builder(argumentLayout, symbolLayout, subscriptLayout, superscriptLayout);
}

int ProductNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Product::s_functionHelper.name());
}

Expression Product::UntypedBuilder(Expression children) {
  assert(children.type() == ExpressionNode::Type::List);
  if (children.childAtIndex(1).type() != ExpressionNode::Type::Symbol) {
    // Second parameter must be a Symbol.
    return Expression();
  }
  return Builder(children.childAtIndex(0), children.childAtIndex(1).convert<Symbol>(), children.childAtIndex(2), children.childAtIndex(3));
}

}
