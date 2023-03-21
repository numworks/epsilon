#include "intermediate_sequence_context.h"

#include <omg/signaling_nan.h>
#include <poincare/addition.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>

#include <cmath>

#include "poincare_helpers.h"
#include "sequence.h"
#include "sequence_store.h"

using namespace Poincare;

namespace Shared {

template <typename T>
IntermediateSequenceContext<T>::IntermediateSequenceContext(
    SequenceContext *sequenceContext)
    : ContextWithParent(sequenceContext), m_sequenceContext(sequenceContext) {}

template <typename T>
const Expression
IntermediateSequenceContext<T>::protectedExpressionForSymbolAbstract(
    const SymbolAbstract &symbol, bool clone,
    ContextWithParent *lastDescendantContext) {
  if (symbol.type() != ExpressionNode::Type::Sequence) {
    return ContextWithParent::protectedExpressionForSymbolAbstract(
        symbol, clone, lastDescendantContext);
  }
  T result = NAN;
  /* Do not use recordAtIndex : if the sequences have been reordered, the
   * name index and the record index may not correspond. */
  char name = static_cast<const Symbol &>(symbol).name()[0];
  int index = SequenceStore::SequenceIndexForName(name);
  Ion::Storage::Record record =
      m_sequenceContext->sequenceStore()->recordAtNameIndex(index);
  if (record.isNull()) {
    return Float<T>::Builder(result);
  }
  assert(record.fullName()[0] == symbol.name()[0]);
  Sequence *seq = m_sequenceContext->sequenceStore()->modelForRecord(record);
  if (!seq->fullName()) {
    return Float<T>::Builder(result);
  }
  Expression rankExpression = symbol.childAtIndex(0).clone();
  /* The lastDesendantContext might contain informations on variables
   * that are contained in the rank expression. */
  T rankValue = PoincareHelpers::ApproximateToScalar<T>(
      rankExpression, lastDescendantContext ? lastDescendantContext : this);
  result = m_sequenceContext->storedValueOfSequenceAtRank<T>(index, rankValue);
  /* We try to approximate the sequence independently from the others at the
   * required rank (this will solve u(n) = 5*n, v(n) = u(n+10) for instance).
   * But we avoid doing so if the sequence referencing itself to avoid an
   * infinite loop. */
  if (OMG::IsSignalingNan(result)) {
    // If the rank is not an int, return NAN
    if (std::floor(rankValue) == rankValue) {
      result = seq->approximateAtRank<T>(rankValue, m_sequenceContext);
    }
  }
  return Float<T>::Builder(result);
}

template class IntermediateSequenceContext<float>;
template class IntermediateSequenceContext<double>;

}  // namespace Shared
