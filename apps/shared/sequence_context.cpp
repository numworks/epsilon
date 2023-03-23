#include "sequence_context.h"

#include <omg/signaling_nan.h>

#include <array>
#include <cmath>

#include "../shared/poincare_helpers.h"
#include "sequence_store.h"

using namespace Poincare;

namespace Shared {

template <typename T>
TemplatedSequenceContext<T>::TemplatedSequenceContext(
    SequenceContext *sequenceContext)
    : ContextWithParent(sequenceContext), m_sequenceContext(sequenceContext) {
  resetCache();
}

template <typename T>
void TemplatedSequenceContext<T>::resetValuesOfSequence(
    int sequenceIndex, bool intermediateComputation) {
  T *values = valuesPointer(sequenceIndex, intermediateComputation);
  for (int depth = 0; depth < SequenceStore::k_maxRecurrenceDepth + 1;
       depth++) {
    *(values + depth) = OMG::SignalingNan<T>();
  }
}

template <typename T>
void TemplatedSequenceContext<T>::resetRanksAndValuesOfSequence(
    int sequenceIndex, bool intermediateComputation) {
  int *rank = rankPointer(sequenceIndex, intermediateComputation);
  *rank = -1;
  resetValuesOfSequence(sequenceIndex, intermediateComputation);
}

template <typename T>
void TemplatedSequenceContext<T>::resetDataOfCurrentComputation() {
  m_isComputingMainResult = false;
  for (int i = 0; i < SequenceStore::k_maxNumberOfSequences; i++) {
    m_smallestRankBeingComputed[i] = -1;
  }
}

template <typename T>
void TemplatedSequenceContext<T>::resetCache() {
  for (int i = 0; i < SequenceStore::k_maxNumberOfSequences; i++) {
    resetRanksAndValuesOfSequence(i, true);
    resetRanksAndValuesOfSequence(i, false);
  }
  resetDataOfCurrentComputation();
}

template <typename T>
T TemplatedSequenceContext<T>::storedValueOfSequenceAtRank(int sequenceIndex,
                                                           int rank) {
  for (int loop = 0; loop < 2; loop++) {
    int storedRank = loop == 0 ? m_mainRanks[sequenceIndex]
                               : m_intermediateRanks[sequenceIndex];
    if (storedRank >= 0) {
      int offset = storedRank - rank;
      if (0 <= offset && offset < SequenceStore::k_maxRecurrenceDepth + 1) {
        T storedValue = loop == 0 ? m_mainValues[sequenceIndex][offset]
                                  : m_intermediateValues[sequenceIndex][offset];
        if (!OMG::IsSignalingNan(storedValue)) {
          return storedValue;
        }
      }
    }
  }
  return OMG::SignalingNan<T>();
}

template <typename T>
int *TemplatedSequenceContext<T>::rankPointer(int sequenceIndex,
                                              bool intermediateComputation) {
  assert(0 <= sequenceIndex &&
         sequenceIndex < SequenceStore::k_maxNumberOfSequences);
  int *rank = intermediateComputation ? m_intermediateRanks : m_mainRanks;
  rank += sequenceIndex;
  return rank;
}

template <typename T>
T *TemplatedSequenceContext<T>::valuesPointer(int sequenceIndex,
                                              bool intermediateComputation) {
  assert(0 <= sequenceIndex &&
         sequenceIndex < SequenceStore::k_maxNumberOfSequences);
  T *values;
  if (intermediateComputation) {
    values = reinterpret_cast<T *>(&m_intermediateValues);
  } else {
    values = reinterpret_cast<T *>(&m_mainValues);
  }
  values += sequenceIndex * (SequenceStore::k_maxRecurrenceDepth + 1);
  return values;
}

template <typename T>
void TemplatedSequenceContext<T>::shiftValuesRight(int sequenceIndex,
                                                   bool intermediateComputation,
                                                   int delta) {
  assert(delta > 0);
  if (delta > SequenceStore::k_maxNumberOfSequences) {
    resetValuesOfSequence(sequenceIndex, intermediateComputation);
    return;
  }
  T *values = valuesPointer(sequenceIndex, intermediateComputation);
  int stop = delta - 1;
  assert(0 <= stop && stop < SequenceStore::k_maxNumberOfSequences);
  for (int depth = SequenceStore::k_maxRecurrenceDepth; depth > stop; depth--) {
    *(values + depth) = *(values + depth - delta);
  }
  for (int depth = stop; depth >= 0; depth--) {
    *(values + depth) = OMG::SignalingNan<T>();
  }
}

template <typename T>
void TemplatedSequenceContext<T>::stepUntilRank(int sequenceIndex, int rank) {
  assert(rank >= 0);
  Sequence *s = m_sequenceContext->sequenceAtNameIndex(sequenceIndex);
  bool explicitComputation =
      rank >= s->firstNonInitialRank() && s->canBeHandleAsExplicit(this);

  if (!explicitComputation && rank > k_maxRecurrentRank) {
    return;
  }
  T cacheValue = storedValueOfSequenceAtRank(sequenceIndex, rank);
  if (!OMG::IsSignalingNan(cacheValue)) {
    return;
  }

  bool intermediateComputation = m_isComputingMainResult;
  m_isComputingMainResult = true;

  int *currentRank = rankPointer(sequenceIndex, intermediateComputation);
  if (*currentRank > rank) {
    resetRanksAndValuesOfSequence(sequenceIndex, intermediateComputation);
  }
  while (*currentRank < rank) {
    int step = explicitComputation ? rank - *currentRank : 1;
    stepRanks(sequenceIndex, intermediateComputation, step);
  }
  if (!intermediateComputation) {
    resetDataOfCurrentComputation();
  }
}

template <typename T>
void TemplatedSequenceContext<T>::stepRanks(int sequenceIndex,
                                            bool intermediateComputation,
                                            int step) {
  int *currentRank = rankPointer(sequenceIndex, intermediateComputation);
  T *values = valuesPointer(sequenceIndex, intermediateComputation);

  // First we increment the rank
  *currentRank += step;

  // Then we shift the values
  shiftValuesRight(sequenceIndex, intermediateComputation, step);

  // We approximate the value at new rank
  T otherCacheValue = storedValueOfSequenceAtRank(sequenceIndex, *currentRank);
  if (!OMG::IsSignalingNan(otherCacheValue)) {
    // If the other cache already knows this value, use it
    *values = otherCacheValue;
  } else {
    int previousSmallestRank = m_smallestRankBeingComputed[sequenceIndex];
    if (previousSmallestRank == -1 || *currentRank < previousSmallestRank) {
      m_smallestRankBeingComputed[sequenceIndex] = *currentRank;
    } else {
      // We are looping, return NAN as computed result
      *values = static_cast<T>(NAN);
      return;
    }
    *values = m_sequenceContext->sequenceAtNameIndex(sequenceIndex)
                  ->approximateAtContextRank<T>(m_sequenceContext,
                                                intermediateComputation);
    m_smallestRankBeingComputed[sequenceIndex] = previousSmallestRank;
  }
}

template <typename T>
const Expression
TemplatedSequenceContext<T>::protectedExpressionForSymbolAbstract(
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

void SequenceContext::tidyDownstreamPoolFrom(char *treePoolCursor) {
  m_sequenceStore->tidyDownstreamPoolFrom(treePoolCursor);
}

Poincare::Context::SymbolAbstractType
SequenceContext::expressionTypeForIdentifier(const char *identifier,
                                             int length) {
  constexpr int numberOfSequencesNames =
      std::size(SequenceStore::k_sequenceNames);
  for (int i = 0; i < numberOfSequencesNames; i++) {
    if (strncmp(identifier, SequenceStore::k_sequenceNames[i], length) == 0) {
      return Poincare::Context::SymbolAbstractType::Sequence;
    }
  }
  return Poincare::ContextWithParent::expressionTypeForIdentifier(identifier,
                                                                  length);
}

template <typename T>
TemplatedSequenceContext<T> *SequenceContext::context() {
  void *helper = sizeof(T) == sizeof(float) ? (void *)&m_floatSequenceContext
                                            : (void *)&m_doubleSequenceContext;
  return static_cast<TemplatedSequenceContext<T> *>(helper);
}

Sequence *SequenceContext::sequenceAtNameIndex(int sequenceIndex) const {
  Ion::Storage::Record record =
      m_sequenceStore->recordAtNameIndex(sequenceIndex);
  assert(!record.isNull());
  Sequence *s = m_sequenceStore->modelForRecord(record);
  assert(s->isDefined());
  return s;
}

template class TemplatedSequenceContext<float>;
template class TemplatedSequenceContext<double>;
template TemplatedSequenceContext<float> *SequenceContext::context<float>();
template TemplatedSequenceContext<double> *SequenceContext::context<double>();

}  // namespace Shared
