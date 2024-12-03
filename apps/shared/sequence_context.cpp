#include "sequence_context.h"

#include <apps/shared/global_context.h>
#include <apps/shared/poincare_helpers.h>
#include <omg/signaling_nan.h>
#include <poincare/expression.h>
#include <poincare/src/expression/symbol.h>

#include <array>
#include <cmath>

#include "sequence_store.h"

using namespace Poincare;

namespace Shared {

SequenceContext::SequenceContext(Context* parentContext,
                                 SequenceStore* sequenceStore)
    : ContextWithParent(parentContext), m_sequenceStore(sequenceStore) {}

const Poincare::Internal::Tree*
SequenceContext::protectedExpressionForSymbolAbstract(
    const Poincare::Internal::Tree* symbol,
    ContextWithParent* lastDescendantContext) {
  assert(symbol->isUserNamed());
  if (!symbol->isSequence()) {
    return ContextWithParent::protectedExpressionForSymbolAbstract(
        symbol, lastDescendantContext);
  }
  double result = NAN;
  /* Do not use recordAtIndex : if the sequences have been reordered, the
   * name index and the record index may not correspond. */
  char name = Internal::Symbol::GetName(symbol)[0];
  int index = SequenceStore::SequenceIndexForName(name);
  Ion::Storage::Record record = sequenceStore()->recordAtNameIndex(index);
  if (record.isNull()) {
    return NewExpression::Builder<double>(result);
  }
  assert(record.fullName()[0] == Internal::Symbol::GetName(symbol)[0]);
  Sequence* seq = sequenceStore()->modelForRecord(record);
  if (!seq->fullName()) {
    return NewExpression::Builder<double>(result);
  }
  UserExpression rankExpression = UserExpression::Builder(symbol->child(0));
  /* The lastDesendantContext might contain informations on variables
   * that are contained in the rank expression. */
  double rankValue = rankExpression.approximateToScalar<double>(
      Preferences::SharedPreferences()->angleUnit(),
      Preferences::SharedPreferences()->complexFormat(),
      lastDescendantContext ? lastDescendantContext : this);
  result = cache()->storedValueOfSequenceAtRank(index, rankValue);
  /* We try to approximate the sequence independently from the others at the
   * required rank (this will solve u(n) = 5*n, v(n) = u(n+10) for instance).
   * But we avoid doing so if the sequence referencing itself to avoid an
   * infinite loop. */
  if (OMG::IsSignalingNan(result)) {
    // If the rank is not an int, return NAN
    if (std::floor(rankValue) == rankValue) {
      result =
          seq->approximateAtRank(rankValue, cache(), lastDescendantContext);
    }
  }
  return NewExpression::Builder<double>(result);
}

void SequenceContext::tidyDownstreamPoolFrom(PoolObject* treePoolCursor) {
  m_sequenceStore->tidyDownstreamPoolFrom(treePoolCursor);
}

Context::SymbolAbstractType SequenceContext::expressionTypeForIdentifier(
    const char* identifier, int length) {
  constexpr int numberOfSequencesNames =
      std::size(SequenceStore::k_sequenceNames);
  for (int i = 0; i < numberOfSequencesNames; i++) {
    if (strncmp(identifier, SequenceStore::k_sequenceNames[i], length) == 0) {
      return Context::SymbolAbstractType::Sequence;
    }
  }
  return ContextWithParent::expressionTypeForIdentifier(identifier, length);
}

Sequence* SequenceContext::sequenceAtNameIndex(int sequenceIndex) const {
  assert(0 <= sequenceIndex && sequenceIndex < k_numberOfSequences);
  Ion::Storage::Record record =
      m_sequenceStore->recordAtNameIndex(sequenceIndex);
  if (record.isNull()) {
    return nullptr;
  }
  Sequence* s = m_sequenceStore->modelForRecord(record);
  return s;
}

bool SequenceContext::sequenceIsNotComputable(int sequenceIndex) {
  return cache()->sequenceIsNotComputable(sequenceIndex);
}

Poincare::Internal::SequenceCache* SequenceContext::cache() {
  return GlobalContext::s_sequenceCache;
}

}  // namespace Shared
