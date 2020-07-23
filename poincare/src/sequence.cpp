#include <poincare/sequence.h>
#include <poincare/integer.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/vertical_offset_layout.h>
#include <poincare/based_integer.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/parenthesis.h>
#include <poincare/complex.h>
#include <apps/shared/sequence.h>

namespace Poincare {

SequenceNode::SequenceNode(const char * newName, int length) : SymbolAbstractNode() {
  strlcpy(const_cast<char*>(name()), newName, length+1);
}

Expression SequenceNode::replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression) {
  return Sequence(this).replaceSymbolWithExpression(symbol, expression);
}

Layout SequenceNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  assert(name()[0] >= 'u' && name()[0] <= 'w');
  Layout rank;
  for (char sequenceName = 'u'; sequenceName <= 'w'; sequenceName++) {
    if (name()[0] == sequenceName) {
      // Checking for the sequence children
      if (childAtIndex(0)->type() == Type::Symbol) {
        // u(n)
        rank = LayoutHelper::String("n", strlen("n"));
      } else if (childAtIndex(0)->type() == Type::Addition) {
        rank = LayoutHelper::String("n+1", strlen("n+1"));
      } else {
        assert(childAtIndex(0)->type() ==  Type::BasedInteger);
        rank = static_cast<BasedIntegerNode &>(*childAtIndex(0)).integer().createLayout();
      }
      return HorizontalLayout::Builder(
        CodePointLayout::Builder(sequenceName),
        VerticalOffsetLayout::Builder(rank, VerticalOffsetLayoutNode::Position::Subscript));
    }
  }
  assert(false);
  return LayoutHelper::String(name(), strlen(name()));
}

int SequenceNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, name());
}

Expression SequenceNode::shallowReduce(ReductionContext reductionContext) {
  return Sequence(this).shallowReduce(reductionContext); // This uses Symbol::shallowReduce
}

Evaluation<float> SequenceNode::approximate(SinglePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  return templatedApproximate<float>(context, complexFormat, angleUnit);
}

Evaluation<double> SequenceNode::approximate(DoublePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  return templatedApproximate<double>(context, complexFormat, angleUnit);
}

template<typename T>
Evaluation<T> SequenceNode::templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  if (childAtIndex(0)->approximate((T)1, context, complexFormat, angleUnit).isUndefined()) {
    return Complex<T>::Undefined();
  }
  Expression e = context->expressionForSymbolAbstract(this, false);
  if (e.isUninitialized()) {
    return Complex<T>::Undefined();
  }
  return e.node()->approximate(T(), context, complexFormat, angleUnit);
}

Sequence Sequence::Builder(const char * name, size_t length, Expression child) {
  Sequence seq = SymbolAbstract::Builder<Sequence, SequenceNode>(name, length);
  if (!child.isUninitialized()) {
    seq.replaceChildAtIndexInPlace(0, child);
  }
  return seq;
}

Expression Sequence::replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression) {
  // Replace the symbol in the child
  childAtIndex(0).replaceSymbolWithExpression(symbol, expression);
  if (symbol.type() == ExpressionNode::Type::Sequence && hasSameNameAs(symbol)) {
    Expression value = expression.clone();
    Expression p = parent();
    if (!p.isUninitialized() && p.node()->childAtIndexNeedsUserParentheses(value, p.indexOfChild(*this))) {
      value = Parenthesis::Builder(value);
    }
    replaceWithInPlace(value);
    return value;
  }
  return *this;
}

// Those two functions will be updated in a comming commit
Expression Sequence::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  return *this;
}

Expression Sequence::deepReplaceReplaceableSymbols(Context * context, bool * didReplace, bool replaceFunctionsOnly, int parameteredAncestorsCount) {
  return *this;
}

}
