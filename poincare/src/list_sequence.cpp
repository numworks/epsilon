#include <poincare/list_sequence.h>
#include <poincare/list_sequence_layout.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

constexpr Expression::FunctionHelper ListSequence::s_functionHelper;

int ListSequenceNode::numberOfChildren() const {
  return ListSequence::s_functionHelper.numberOfChildren();
}

Layout ListSequenceNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return ListSequenceLayout::Builder(
      childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits),
      childAtIndex(1)->createLayout(floatDisplayMode, numberOfSignificantDigits),
      childAtIndex(2)->createLayout(floatDisplayMode, numberOfSignificantDigits));
}

int ListSequenceNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, ListSequence::s_functionHelper.name());
}

Expression ListSequence::UntypedBuilder(Expression children) {
  assert(children.type() == ExpressionNode::Type::Matrix);
  if (children.childAtIndex(1).type() != ExpressionNode::Type::Symbol) {
    // Second parameter must be a Symbol.
    return Expression();
  }
  return Builder(children.childAtIndex(0), children.childAtIndex(1).convert<Symbol>(), children.childAtIndex(2));
}

}
