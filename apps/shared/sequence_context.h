#ifndef APPS_SHARED_SEQUENCE_CONTEXT_H
#define APPS_SHARED_SEQUENCE_CONTEXT_H

#include <poincare/context_with_parent.h>
#include <poincare/expression.h>
#include <poincare/symbol.h>

#include "sequence_store.h"

namespace Shared {

class Sequence;

template <typename T>
class TemplatedSequenceContext {
 public:
  TemplatedSequenceContext(SequenceContext* sequenceContext);
  void resetCache();
  void stepUntilRank(int n, int sequenceIndex);
  constexpr static bool IsAcceptableRank(int n) {
    return 0 <= n && n <= k_maxRecurrentRank;
  }
  int rank(int sequenceIndex, bool independent) const {
    assert(0 <= sequenceIndex &&
           sequenceIndex < SequenceStore::k_maxNumberOfSequences);
    return independent ? m_independentRanks[sequenceIndex] : m_commonRank;
  }
  T rankSequenceValue(int sequenceIndex, int depth, bool independent) const {
    assert(0 <= sequenceIndex &&
           sequenceIndex < SequenceStore::k_maxNumberOfSequences);
    assert(0 <= depth && depth < SequenceStore::k_maxRecurrenceDepth + 1);
    return independent ? m_independentRankValues[sequenceIndex][depth]
                       : m_commonRankValues[sequenceIndex][depth];
  }

 private:
  constexpr static int k_maxRecurrentRank = 10000;
  void stepToNextRank(int sequenceIndex = -1);

  SequenceContext* m_sequenceContext;

  /* Cache:
   * We use two types of cache :
   * The first one is used to to accelerate the
   * computation of values of recurrent sequences. We memoize the last computed
   * values of the sequences and their associated ranks (n and n+1 for
   * instance). Thereby, when another evaluation at a superior rank k > n+1 is
   * called, we avoid iterating from 0 but can start from n. This cache allows
   * us to step all of the sequences at once.
   *
   * The second one used used for fixed term computation. For instance, if a
   * sequence is defined using a fixed term of another, u(3) for instance, we
   * compute its value through the second type of cache. This way, we do not
   * erase the data stored in the first type of cache and we can compute the
   * values of each sequence at independent rank. This means that
   * (u(3), v(5), w(10)) can be computed at the same time.
   * This cache is therefore used for independent steps of sequences
   */
  int m_commonRank;
  // For example if m_commonRank = 9, then m_commonRankValues = {{u9,u8,u7},
  // {v9,v8,v7}, {w9,w8,w7}}
  T m_commonRankValues[SequenceStore::k_maxNumberOfSequences]
                      [SequenceStore::k_maxRecurrenceDepth + 1];

  // Used for fixed computations
  int m_independentRanks[SequenceStore::k_maxNumberOfSequences];
  // For example if m_independentRanks = {9,5,4}, then m_independentRanks =
  // {{u9,u8,u7}, {v5,v4,v3}, {w4,w3,w2}}
  T m_independentRankValues[SequenceStore::k_maxNumberOfSequences]
                           [SequenceStore::k_maxRecurrenceDepth + 1];
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
  void stepUntilRank(int n, int sequenceIndex) {
    static_cast<TemplatedSequenceContext<T>*>(helper<T>())
        ->stepUntilRank(n, sequenceIndex);
  }

  SequenceStore* sequenceStore() { return m_sequenceStore; }

  void tidyDownstreamPoolFrom(char* treePoolCursor) override;

  template <typename T>
  int rank(int sequenceIndex, bool independent) {
    return static_cast<TemplatedSequenceContext<T>*>(helper<T>())
        ->rank(sequenceIndex, independent);
  }

  template <typename T>
  T rankSequenceValue(int sequenceIndex, int depth, bool independent) {
    return static_cast<TemplatedSequenceContext<T>*>(helper<T>())
        ->rankSequenceValue(sequenceIndex, depth, independent);
  }

 private:
  SequenceStore* m_sequenceStore;
  TemplatedSequenceContext<float> m_floatSequenceContext;
  TemplatedSequenceContext<double> m_doubleSequenceContext;
  template <typename T>
  void* helper() {
    return sizeof(T) == sizeof(float) ? (void*)&m_floatSequenceContext
                                      : (void*)&m_doubleSequenceContext;
  }
};

}  // namespace Shared

#endif
