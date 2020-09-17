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

int SequenceNode::simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool canBeInterrupted, bool ignoreParentheses) const {
  /* This function ensures that terms like u(n) and u(n+1), u(n) and v(n),
   * u(a) and u(b) do not factorize.
   * We never want to factorize. The only cases where it could be useful are
   * like the following : u(n)+u(n). But thanks to the cache system, no
   * computation is needed for the second term.*/
  assert(type() == e->type());
  assert(numberOfChildren() == 1);
  assert(e->numberOfChildren() == 1);
  ExpressionNode * seq = const_cast<ExpressionNode*>(e);
  int delta = strcmp(name(), reinterpret_cast<SequenceNode *>(seq)->name());
  if (delta == 0) {
    return SimplificationOrder(childAtIndex(0), e->childAtIndex(0), ascending, canBeInterrupted, ignoreParentheses);
  }
  return delta;
}

Layout SequenceNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  assert(name()[0] >= 'u' && name()[0] <= 'w');
  Layout rank = childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits);
  return HorizontalLayout::Builder(
    CodePointLayout::Builder(name()[0]),
    VerticalOffsetLayout::Builder(rank, VerticalOffsetLayoutNode::Position::Subscript));
}

int SequenceNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, name());
}

Expression SequenceNode::shallowReduce(ReductionContext reductionContext) {
  return Sequence(this).shallowReduce(reductionContext);
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

Expression Sequence::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  Expression e = Expression::defaultShallowReduce();
  e = e.defaultHandleUnitsInChildren();
  if (e.isUndefined()) {
    return e;
  }
  if (reductionContext.symbolicComputation() == ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithUndefined) {
    return replaceWithUndefinedInPlace();
  }
  return *this;
}

Expression Sequence::deepReplaceReplaceableSymbols(Context * context, bool * didReplace, bool replaceFunctionsOnly, int parameteredAncestorsCount) {
  return *this;
}

}
