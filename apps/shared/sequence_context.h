#ifndef APPS_SHARED_SEQUENCE_CONTEXT_H
#define APPS_SHARED_SEQUENCE_CONTEXT_H

#include <poincare/context_with_parent.h>
#include <poincare/expression.h>
#include <poincare/symbol.h>

#include "sequence_store.h"

namespace Shared {

class Sequence;

class SequenceContext : public Poincare::ContextWithParent {
 public:
  SequenceContext(Poincare::Context* parentContext,
                  SequenceStore* sequenceStore);

  /* u{n}, v{n} and w{n} must be parsed as sequences in the sequence app
   * so that u{n} can be defined as a function of v{n} without v{n} being
   * already defined.
   * So expressionTypForIdentifier returns Type::Sequence for u, v and w,
   * and calls the parent context in other cases.
   * The other methods (setExpressionForSymbolAbstract and
   * expressionForSymbolAbstract) always call the parent context. */
  Poincare::Context::SymbolAbstractType expressionTypeForIdentifier(
      const char* identifier, int length) override;

  void resetCache();
  void tidyDownstreamPoolFrom(Poincare::TreeNode* treePoolCursor) override;
  SequenceStore* sequenceStore() { return m_sequenceStore; }
  bool sequenceIsNotComputable(int sequenceIndex);

  void stepUntilRank(int sequenceIndex, int rank);
  int rank(int sequenceIndex, bool intermediateComputation) {
    return *(rankPointer(sequenceIndex, intermediateComputation));
  }
  double storedValueOfSequenceAtRank(int sequenceIndex, int rank);

 private:
  constexpr static int k_maxRecurrentRank = 10000;

  int* rankPointer(int sequenceIndex, bool intermediateComputation);
  double* valuesPointer(int sequenceIndex, bool intermediateComputation);
  void shiftValuesRight(int sequenceIndex, bool intermediateComputation,
                        int delta);
  void stepRanks(int sequenceIndex, bool intermediateComputation, int step);
  void resetValuesOfSequence(int sequenceIndex, bool intermediateComputation);
  void resetRanksAndValuesOfSequence(int sequenceIndex,
                                     bool intermediateComputation);
  void resetComputationStatus();
  const Poincare::Expression protectedExpressionForSymbolAbstract(
      const Poincare::SymbolAbstract& symbol, bool clone,
      ContextWithParent* lastDescendantContext) override;
  Sequence* sequenceAtNameIndex(int sequenceIndex) const;

  /* Main ranks for main computations and intermediate ranks for intermediate
   * computations (ex: computation of v(2) in u(3) = v(2) + 4). If ranks are
   * {9,5,4} then values are {{u9,u8,u7}, {v5,v4,v3}, {w4,w3,w2}}. */
  int m_mainRanks[SequenceStore::k_maxNumberOfSequences];
  double m_mainValues[SequenceStore::k_maxNumberOfSequences]
                     [SequenceStore::k_maxRecurrenceDepth + 1];
  int m_intermediateRanks[SequenceStore::k_maxNumberOfSequences];
  double m_intermediateValues[SequenceStore::k_maxNumberOfSequences]
                             [SequenceStore::k_maxRecurrenceDepth + 1];
  /* Save initial values to avoid stepping all values back. For example if in an
   * intermediate computation we ask for v(n) with v(n+1) = v(n)+v(0), we will
   * always step to rank n and then step back to rank 0, replacing all values
   * stored in m_intermediateValues. */
  double m_initialValues[SequenceStore::k_maxNumberOfSequences]
                        [SequenceStore::k_maxRecurrenceDepth + 1];

  SequenceStore* m_sequenceStore;
  bool m_isInsideComputation;
  int m_smallestRankBeingComputed[SequenceStore::k_maxNumberOfSequences];
  Poincare::TrinaryBoolean
      m_sequenceIsNotComputable[SequenceStore::k_maxNumberOfSequences];
};

}  // namespace Shared

#endif
