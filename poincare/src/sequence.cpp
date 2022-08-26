#include <poincare/sequence.h>
#include <poincare/integer.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/vertical_offset_layout.h>
#include <poincare/based_integer.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
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

int SequenceNode::simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool ignoreParentheses) const {
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
    return SimplificationOrder(childAtIndex(0), e->childAtIndex(0), ascending, ignoreParentheses);
  }
  return delta;
}

Layout SequenceNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  Layout rank = childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits, context);
  return HorizontalLayout::Builder(
    CodePointLayout::Builder(name()[0]),
    VerticalOffsetLayout::Builder(rank, VerticalOffsetLayoutNode::Position::Subscript));
}

int SequenceNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  int result = SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, name(), false, -1, true);
  return result;
}

Expression SequenceNode::shallowReduce(const ReductionContext& reductionContext) {
  return Sequence(this).shallowReduce(reductionContext);
}

Evaluation<float> SequenceNode::approximate(SinglePrecision p, const ApproximationContext& approximationContext) const {
  return templatedApproximate<float>(approximationContext);
}

Evaluation<double> SequenceNode::approximate(DoublePrecision p, const ApproximationContext& approximationContext) const {
  return templatedApproximate<double>(approximationContext);
}

template<typename T>
Evaluation<T> SequenceNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  if (approximationContext.withinReduce() || childAtIndex(0)->approximate((T)1, approximationContext).isUndefined()) {
    /* If we're inside a reducing routine, we want to escape the sequence
     * approximation. Indeed, in order to know that the sequence is well defined
     * (especially for self-referencing or inter-dependently defined sequences),
     * we need to reduce the sequence definition (done by calling
     * 'expressionForSymbolAbstract'); if we're within a reduce routine, we
     * would create an infinite loop. Returning a NAN approximation for
     * sequences within reduce routine does not really matter: we just have
     * access to less information in order to simplify (abs(u(n)) might not be
     * reduced for instance). */
    return Complex<T>::Undefined();
  }
  Expression e = approximationContext.context()->expressionForSymbolAbstract(this, false);
  if (e.isUninitialized()) {
    return Complex<T>::Undefined();
  }
  return e.node()->approximate(T(), approximationContext);
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
  Expression e = SimplificationHelper::defaultShallowReduce(
      *this,
      &reductionContext,
      SimplificationHelper::UnitReduction::BanUnits
  );
  if (!e.isUninitialized()) {
    return e;
  }
  if (reductionContext.symbolicComputation() == ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithUndefined) {
    return replaceWithUndefinedInPlace();
  }
  return *this;
}

}
