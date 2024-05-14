#ifndef APPS_SHARED_SEQUENCE_CONTEXT_H
#define APPS_SHARED_SEQUENCE_CONTEXT_H

#include <omg/troolean.h>
#include <poincare/expression.h>
#include <poincare/old/context_with_parent.h>
#include <poincare/old/symbol.h>

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
  void tidyDownstreamPoolFrom(Poincare::PoolObject* treePoolCursor) override;
  SequenceStore* sequenceStore() { return m_sequenceStore; }
  bool sequenceIsNotComputable(int sequenceIndex);

  void stepUntilRank(int sequenceIndex, int rank);
  int rank(int sequenceIndex, bool intermediateComputation) {
    return *(rankPointer(sequenceIndex, intermediateComputation));
  }
  double storedValueOfSequenceAtRank(int sequenceIndex, int rank);

 private:
  constexpr static int k_maxRecurrentRank = 10000;
  constexpr static int k_storageDepth = 6;
  constexpr static int k_numberOfSequences =
      SequenceStore::k_maxNumberOfSequences;

  int* rankPointer(int sequenceIndex, bool intermediateComputation);
  double* valuesPointer(int sequenceIndex, bool intermediateComputation);
  void shiftValuesRight(int sequenceIndex, bool intermediateComputation,
                        int delta);
  void stepRanks(int sequenceIndex, bool intermediateComputation, int step);
  void resetValuesOfSequence(int sequenceIndex, bool intermediateComputation);
  void resetRanksAndValuesOfSequence(int sequenceIndex,
                                     bool intermediateComputation);
  void resetComputationStatus();
  const Poincare::UserExpression protectedExpressionForSymbolAbstract(
      const Poincare::SymbolAbstract& symbol, bool clone,
      ContextWithParent* lastDescendantContext) override;
  Sequence* sequenceAtNameIndex(int sequenceIndex) const;
  int rankForInitialValuesStorage(int sequenceIndex) const;

  /* Main ranks for main computations and intermediate ranks for intermediate
   * computations (ex: computation of v(2) in u(3) = v(2) + 4). If ranks are
   * {9,5,4} then values are {{u9,u8,u7}, {v5,v4,v3}, {w4,w3,w2}}. */
  int m_mainRanks[k_numberOfSequences];
  double m_mainValues[k_numberOfSequences][k_storageDepth];
  int m_intermediateRanks[k_numberOfSequences];
  double m_intermediateValues[k_numberOfSequences][k_storageDepth];
  /* Save initial values to avoid stepping all values back. For example if in an
   * intermediate computation we ask for v(n) with v(n+1) = v(n)+v(0), we will
   * always step to rank n and then step back to rank 0, replacing all values
   * stored in m_intermediateValues. */
  double m_initialValues[k_numberOfSequences][k_storageDepth];

  SequenceStore* m_sequenceStore;
  bool m_isInsideComputation;
  int m_smallestRankBeingComputed[k_numberOfSequences];
  OMG::Troolean m_sequenceIsNotComputable[k_numberOfSequences];
};

}  // namespace Shared

#endif
