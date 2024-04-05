#include <apps/shared/sequence.h>
#include <poincare/based_integer.h>
#include <poincare/complex.h>
#include <poincare/integer.h>
#include <poincare/layout.h>
#include <poincare/sequence.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare_junior/src/memory/tree_stack.h>

namespace Poincare {

int SequenceNode::simplificationOrderSameType(const ExpressionNode* e,
                                              bool ascending,
                                              bool ignoreParentheses) const {
  /* This function ensures that terms like u(n) and u(n+1), u(n) and v(n),
   * u(a) and u(b) do not factorize.
   * We never want to factorize. The only cases where it could be useful are
   * like the following : u(n)+u(n). But thanks to the cache system, no
   * computation is needed for the second term.*/
  assert(otype() == e->otype());
  assert(numberOfChildren() == 1);
  assert(e->numberOfChildren() == 1);
  ExpressionNode* seq = const_cast<ExpressionNode*>(e);
  int delta = strcmp(m_name, reinterpret_cast<SequenceNode*>(seq)->name());
  if (delta == 0) {
    return SimplificationOrder(childAtIndex(0), e->childAtIndex(0), ascending,
                               ignoreParentheses);
  }
  return delta;
}

size_t SequenceNode::serialize(char* buffer, size_t bufferSize,
                               Preferences::PrintFloatMode floatDisplayMode,
                               int numberOfSignificantDigits) const {
  size_t result = SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      m_name, SerializationHelper::ParenthesisType::Braces);
  return result;
}

OExpression SequenceNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return Sequence(this).shallowReduce(reductionContext);
}

Evaluation<float> SequenceNode::approximate(
    SinglePrecision p, const ApproximationContext& approximationContext) const {
  return templatedApproximate<float>(approximationContext);
}

Evaluation<double> SequenceNode::approximate(
    DoublePrecision p, const ApproximationContext& approximationContext) const {
  return templatedApproximate<double>(approximationContext);
}

template <typename T>
Evaluation<T> SequenceNode::templatedApproximate(
    const ApproximationContext& approximationContext) const {
  if (approximationContext.withinReduce() ||
      childAtIndex(0)->approximate((T)1, approximationContext).isUndefined()) {
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
  OExpression e =
      approximationContext.context()->expressionForSymbolAbstract(this, false);
  if (e.isUninitialized()) {
    return Complex<T>::Undefined();
  }
  return e.node()->approximate(T(), approximationContext);
}

Sequence Sequence::Builder(const char* name, size_t length,
                           JuniorExpression child) {
  // If needed, handle theta like functions and symbols
  assert(!AliasesLists::k_thetaAliases.contains(name, length));
  PoincareJ::Tree* tree =
      PoincareJ::SharedTreeStack->push<PoincareJ::Type::UserSequence>(name);
  assert(!child.isUninitialized());
  child.tree()->clone();

  JuniorExpression expr = JuniorExpression::Builder(tree);
  return static_cast<Sequence&>(expr);
}

OExpression Sequence::shallowReduce(ReductionContext reductionContext) {
  if (reductionContext.symbolicComputation() ==
      SymbolicComputation::ReplaceAllSymbolsWithUndefined) {
    return replaceWithUndefinedInPlace();
  }
  OExpression e = SimplificationHelper::defaultShallowReduce(
      *this, &reductionContext,
      SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
      SimplificationHelper::UnitReduction::BanUnits);
  if (!e.isUninitialized()) {
    return e;
  }
  return *this;
}

}  // namespace Poincare
