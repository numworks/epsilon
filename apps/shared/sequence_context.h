#ifndef APPS_SHARED_SEQUENCE_CONTEXT_H
#define APPS_SHARED_SEQUENCE_CONTEXT_H

#include <poincare/context_with_parent.h>
#include <poincare/expression.h>
#include <poincare/symbol.h>

#include "sequence_store.h"

namespace Shared {

class Sequence;

template <typename T>
class TemplatedSequenceContext : public Poincare::ContextWithParent {
 public:
  TemplatedSequenceContext(SequenceContext* sequenceContext);
  void resetCacheOfSequence(int sequenceIndex, bool intermediateComputation);
  void resetDataOfCurrentComputation();
  void resetCache();
  void stepUntilRank(int sequenceIndex, int rank);
  constexpr static bool IsAcceptableRank(int rank) {
    return 0 <= rank && rank <= k_maxRecurrentRank;
  }
  int rank(int sequenceIndex, bool intermediateComputation) const {
    assert(0 <= sequenceIndex &&
           sequenceIndex < SequenceStore::k_maxNumberOfSequences);
    return intermediateComputation ? m_intermediateRanks[sequenceIndex]
                                   : m_mainRanks[sequenceIndex];
  }
  T storedValueOfSequenceAtRank(int sequenceIndex, int rank);

 private:
  constexpr static int k_maxRecurrentRank = 10000;
  void stepToNextRank(int sequenceIndex, bool intermediateComputation);
  const Poincare::Expression protectedExpressionForSymbolAbstract(
      const Poincare::SymbolAbstract& symbol, bool clone,
      ContextWithParent* lastDescendantContext) override;

  SequenceContext* m_sequenceContext;

  /* Main ranks for main computations and intermediate ranks for intermediate
   * computations (ex: computation of v(2) in u(3) = v(2) + 4). If ranks are
   * {9,5,4} then values are {{u9,u8,u7}, {v5,v4,v3}, {w4,w3,w2}}. */
  int m_mainRanks[SequenceStore::k_maxNumberOfSequences];
  T m_mainValues[SequenceStore::k_maxNumberOfSequences]
                [SequenceStore::k_maxRecurrenceDepth + 1];
  int m_intermediateRanks[SequenceStore::k_maxNumberOfSequences];
  T m_intermediateValues[SequenceStore::k_maxNumberOfSequences]
                        [SequenceStore::k_maxRecurrenceDepth + 1];

  bool m_isComputingMainResult;
  int m_smallestRankBeingComputed[SequenceStore::k_maxNumberOfSequences];
};

class SequenceContext : public Poincare::ContextWithParent {
 public:
  SequenceContext(Poincare::Context* parentContext,
                  SequenceStore* sequenceStore)
      : ContextWithParent(parentContext),
        m_sequenceStore(sequenceStore),
        m_floatSequenceContext(this),
        m_doubleSequenceContext(this) {}

  /* u{n}, v{n} and w{n} must be parsed as sequences in the sequence app
   * so that u{n} can be defined as a function of v{n} without v{n} being
   * already defined.
   * So expressionTypForIdentifier returns Type::Sequence for u, v and w,
   * and calls the parent context in other cases.
   * The other methods (setExpressionForSymbolAbstract and
   * expressionForSymbolAbstract) always call the parent context. */
  Poincare::Context::SymbolAbstractType expressionTypeForIdentifier(
      const char* identifier, int length) override;

  void resetCache() {
    m_floatSequenceContext.resetCache();
    m_doubleSequenceContext.resetCache();
  }

  template <typename T>
  void stepUntilRank(int sequenceIndex, int rank) {
    context<T>()->stepUntilRank(sequenceIndex, rank);
  }

  SequenceStore* sequenceStore() { return m_sequenceStore; }

  void tidyDownstreamPoolFrom(char* treePoolCursor) override;

  template <typename T>
  int rank(int sequenceIndex, bool intermediateComputation) {
    return context<T>()->rank(sequenceIndex, intermediateComputation);
  }

  template <typename T>
  T storedValueOfSequenceAtRank(int sequenceIndex, int rank) {
    return context<T>()->storedValueOfSequenceAtRank(sequenceIndex, rank);
  }

  template <typename T>
  TemplatedSequenceContext<T>* context();

 private:
  SequenceStore* m_sequenceStore;
  TemplatedSequenceContext<float> m_floatSequenceContext;
  TemplatedSequenceContext<double> m_doubleSequenceContext;
};

}  // namespace Shared

#endif
