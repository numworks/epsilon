#include "sequence_context.h"

#include <omg/signaling_nan.h>

#include <array>
#include <cmath>

#include "../shared/poincare_helpers.h"
#include "sequence_store.h"

using namespace Poincare;

namespace Shared {

SequenceContext::SequenceContext(Poincare::Context *parentContext,
                                 SequenceStore *sequenceStore)
    : ContextWithParent(parentContext), m_sequenceStore(sequenceStore) {
  resetCache();
}

void SequenceContext::resetValuesOfSequence(int sequenceIndex,
                                            bool intermediateComputation) {
  double *values = valuesPointer(sequenceIndex, intermediateComputation);
  for (int depth = 0; depth < SequenceStore::k_maxRecurrenceDepth + 1;
       depth++) {
    *(values + depth) = OMG::SignalingNan<double>();
  }
}

void SequenceContext::resetRanksAndValuesOfSequence(
    int sequenceIndex, bool intermediateComputation) {
  int *rank = rankPointer(sequenceIndex, intermediateComputation);
  *rank = -1;
  resetValuesOfSequence(sequenceIndex, intermediateComputation);
}

void SequenceContext::resetDataOfCurrentComputation() {
  m_isInsideComputation = false;
  for (int i = 0; i < SequenceStore::k_maxNumberOfSequences; i++) {
    m_smallestRankBeingComputed[i] = -1;
  }
}

double SequenceContext::storedValueOfSequenceAtRank(int sequenceIndex,
                                                    int rank) {
  assert(0 <= sequenceIndex &&
         sequenceIndex < SequenceStore::k_maxNumberOfSequences);
  for (int loop = 0; loop < 2; loop++) {
    int storedRank = loop == 0 ? m_mainRanks[sequenceIndex]
                               : m_intermediateRanks[sequenceIndex];
    if (storedRank >= 0) {
      int offset = storedRank - rank;
      if (0 <= offset && offset < SequenceStore::k_maxRecurrenceDepth + 1) {
        double storedValue = loop == 0
                                 ? m_mainValues[sequenceIndex][offset]
                                 : m_intermediateValues[sequenceIndex][offset];
        if (!OMG::IsSignalingNan(storedValue)) {
          return storedValue;
        }
      }
    }
  }
  return OMG::SignalingNan<double>();
}

int *SequenceContext::rankPointer(int sequenceIndex,
                                  bool intermediateComputation) {
  assert(0 <= sequenceIndex &&
         sequenceIndex < SequenceStore::k_maxNumberOfSequences);
  int *rank = intermediateComputation ? m_intermediateRanks : m_mainRanks;
  rank += sequenceIndex;
  return rank;
}

double *SequenceContext::valuesPointer(int sequenceIndex,
                                       bool intermediateComputation) {
  assert(0 <= sequenceIndex &&
         sequenceIndex < SequenceStore::k_maxNumberOfSequences);
  double *values = intermediateComputation
                       ? reinterpret_cast<double *>(&m_intermediateValues)
                       : reinterpret_cast<double *>(&m_mainValues);
  values += sequenceIndex * (SequenceStore::k_maxRecurrenceDepth + 1);
  return values;
}

void SequenceContext::shiftValuesRight(int sequenceIndex,
                                       bool intermediateComputation,
                                       int delta) {
  assert(delta > 0);
  if (delta > SequenceStore::k_maxRecurrenceDepth) {
    resetValuesOfSequence(sequenceIndex, intermediateComputation);
    return;
  }
  double *values = valuesPointer(sequenceIndex, intermediateComputation);
  int stop = delta - 1;
  assert(0 <= stop && stop < SequenceStore::k_maxRecurrenceDepth);
  for (int depth = SequenceStore::k_maxRecurrenceDepth; depth > stop; depth--) {
    *(values + depth) = *(values + depth - delta);
  }
  for (int depth = stop; depth >= 0; depth--) {
    *(values + depth) = OMG::SignalingNan<double>();
  }
}

void SequenceContext::stepUntilRank(int sequenceIndex, int rank) {
  assert(rank >= 0);
  bool intermediateComputation = m_isInsideComputation;
  Sequence *s = sequenceAtNameIndex(sequenceIndex);
  bool explicitComputation =
      rank >= s->firstNonInitialRank() && s->canBeHandledAsExplicit(this);
  if (!explicitComputation && rank > k_maxRecurrentRank) {
    return;
  }

  double cacheValue = storedValueOfSequenceAtRank(sequenceIndex, rank);
  bool jumpToRank = explicitComputation || !OMG::IsSignalingNan(cacheValue);

  m_isInsideComputation = true;
  int *currentRank = rankPointer(sequenceIndex, intermediateComputation);
  if (*currentRank > rank) {
    resetRanksAndValuesOfSequence(sequenceIndex, intermediateComputation);
  }
  while (*currentRank < rank) {
    int step = jumpToRank ? rank - *currentRank : 1;
    stepRanks(sequenceIndex, intermediateComputation, step);
  }
  if (!intermediateComputation) {
    resetDataOfCurrentComputation();
  }
}

void SequenceContext::stepRanks(int sequenceIndex, bool intermediateComputation,
                                int step) {
  int *currentRank = rankPointer(sequenceIndex, intermediateComputation);
  double *values = valuesPointer(sequenceIndex, intermediateComputation);

  // First we increment the rank
  *currentRank += step;

  // Then we shift the values
  shiftValuesRight(sequenceIndex, intermediateComputation, step);

  // We approximate the value at new rank
  double otherCacheValue =
      storedValueOfSequenceAtRank(sequenceIndex, *currentRank);
  if (!OMG::IsSignalingNan(otherCacheValue)) {
    // If the other cache already knows this value, use it
    *values = otherCacheValue;
  } else {
    int previousSmallestRank = m_smallestRankBeingComputed[sequenceIndex];
    if (previousSmallestRank == -1 || *currentRank < previousSmallestRank) {
      m_smallestRankBeingComputed[sequenceIndex] = *currentRank;
    } else {
      // We are looping, return NAN as computed result
      *values = static_cast<double>(NAN);
      return;
    }
    *values = sequenceAtNameIndex(sequenceIndex)
                  ->approximateAtContextRank(this, intermediateComputation);
    m_smallestRankBeingComputed[sequenceIndex] = previousSmallestRank;
  }
}

const Expression SequenceContext::protectedExpressionForSymbolAbstract(
    const SymbolAbstract &symbol, bool clone,
    ContextWithParent *lastDescendantContext) {
  if (symbol.type() != ExpressionNode::Type::Sequence) {
    return ContextWithParent::protectedExpressionForSymbolAbstract(
        symbol, clone, lastDescendantContext);
  }
  double result = NAN;
  /* Do not use recordAtIndex : if the sequences have been reordered, the
   * name index and the record index may not correspond. */
  char name = static_cast<const Symbol &>(symbol).name()[0];
  int index = SequenceStore::SequenceIndexForName(name);
  Ion::Storage::Record record = sequenceStore()->recordAtNameIndex(index);
  if (record.isNull()) {
    return Float<double>::Builder(result);
  }
  assert(record.fullName()[0] == symbol.name()[0]);
  Sequence *seq = sequenceStore()->modelForRecord(record);
  if (!seq->fullName()) {
    return Float<double>::Builder(result);
  }
  Expression rankExpression = symbol.childAtIndex(0).clone();
  /* The lastDesendantContext might contain informations on variables
   * that are contained in the rank expression. */
  double rankValue = PoincareHelpers::ApproximateToScalar<double>(
      rankExpression, lastDescendantContext ? lastDescendantContext : this);
  result = storedValueOfSequenceAtRank(index, rankValue);
  /* We try to approximate the sequence independently from the others at the
   * required rank (this will solve u(n) = 5*n, v(n) = u(n+10) for instance).
   * But we avoid doing so if the sequence referencing itself to avoid an
   * infinite loop. */
  if (OMG::IsSignalingNan(result)) {
    // If the rank is not an int, return NAN
    if (std::floor(rankValue) == rankValue) {
      result = seq->approximateAtRank(rankValue, this);
    }
  }
  return Float<double>::Builder(result);
}

void SequenceContext::resetCache() {
  for (int i = 0; i < SequenceStore::k_maxNumberOfSequences; i++) {
    resetRanksAndValuesOfSequence(i, true);
    resetRanksAndValuesOfSequence(i, false);
  }
  resetDataOfCurrentComputation();
  for (int i = 0; i < SequenceStore::k_maxNumberOfSequences; i++) {
    m_sequenceIsNotComputable[i] = Poincare::TrinaryBoolean::Unknown;
  }
}

void SequenceContext::tidyDownstreamPoolFrom(TreeNode *treePoolCursor) {
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

Sequence *SequenceContext::sequenceAtNameIndex(int sequenceIndex) const {
  Ion::Storage::Record record =
      m_sequenceStore->recordAtNameIndex(sequenceIndex);
  assert(!record.isNull());
  Sequence *s = m_sequenceStore->modelForRecord(record);
  /* The case !isDefined() should have been escaped in
   * Sequence::approximateAtRank. */
  assert(s->isDefined());
  return s;
}

bool SequenceContext::sequenceIsNotComputable(int sequenceIndex) {
  if (m_sequenceIsNotComputable[sequenceIndex] ==
      Poincare::TrinaryBoolean::Unknown) {
    m_sequenceIsNotComputable[sequenceIndex] =
        sequenceAtNameIndex(sequenceIndex)->mainExpressionIsNotComputable(this)
            ? Poincare::TrinaryBoolean::True
            : Poincare::TrinaryBoolean::False;
  }
  assert(m_sequenceIsNotComputable[sequenceIndex] !=
         Poincare::TrinaryBoolean::Unknown);
  return m_sequenceIsNotComputable[sequenceIndex] ==
         Poincare::TrinaryBoolean::True;
}

int SequenceContext::rank(int sequenceIndex, bool intermediateComputation) {
  assert(0 <= sequenceIndex &&
         sequenceIndex < SequenceStore::k_maxNumberOfSequences);
  return intermediateComputation ? m_intermediateRanks[sequenceIndex]
                                 : m_mainRanks[sequenceIndex];
}

}  // namespace Shared
