#include <poincare/sum.h>
#include <poincare/addition.h>
#include <poincare/sum_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <cmath>

namespace Poincare {

constexpr Expression::FunctionHelper Sum::s_functionHelper;

Layout SumNode::createSumAndProductLayout(Layout argumentLayout, Layout symbolLayout, Layout subscriptLayout, Layout superscriptLayout) const {
  return SumLayout::Builder(argumentLayout, symbolLayout, subscriptLayout, superscriptLayout);
}

int SumNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Sum::s_functionHelper.name());
}

Expression Sum::UntypedBuilder(Expression children) {
  assert(children.type() == ExpressionNode::Type::List);
  if (children.childAtIndex(1).type() != ExpressionNode::Type::Symbol) {
    // Second parameter must be a Symbol.
    return Expression();
  }
  return Builder(children.childAtIndex(0), children.childAtIndex(1).convert<Symbol>(), children.childAtIndex(2), children.childAtIndex(3));
}

}
