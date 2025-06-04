#include "sequence.h"

#include <assert.h>
#include <omg/unreachable.h>
#include <poincare/helpers/sequence.h>
#include <poincare/helpers/symbol.h>
#include <poincare/src/memory/pattern_matching.h>
#include <string.h>

#include "dimension.h"
#include "integer.h"
#include "k_tree.h"
#include "symbol.h"

namespace Poincare::Internal {

bool Sequence::IsSequenceName(const char* name) {
  for (const char* s : SequenceHelper::k_sequenceNames) {
    if (strcmp(s, name) == 0) {
      return true;
    }
  }
  return false;
}

int Sequence::InitialRank(const Tree* sequence) {
  assert(sequence->isSequence());
  assert(sequence->child(k_firstRankIndex)->isInteger());
  return Integer::Handler(sequence->child(k_firstRankIndex)).to<int>();
}

Tree* Sequence::PushMainExpressionName(const Tree* sequence) {
  assert(sequence->isSequence());
  Tree* result = SharedTreeStack->pushUserSequence(
      Symbol::GetName(sequence->child(k_nameIndex)));
  Tree* sequenceSymbol = SharedTreeStack->pushUserSymbol("n");
  switch (sequence->type()) {
    case Type::SequenceExplicit:
      break;
    case Type::SequenceSingleRecurrence:
      sequenceSymbol->moveTreeOverTree(
          PatternMatching::Create(KAdd(KA, 1_e), {.KA = sequenceSymbol}));
      break;
    case Type::SequenceDoubleRecurrence:
      sequenceSymbol->moveTreeOverTree(
          PatternMatching::Create(KAdd(KA, 2_e), {.KA = sequenceSymbol}));
      break;
    default:
      OMG::unreachable();
  }
  return result;
}

Tree* Sequence::PushInitialConditionName(const Tree* sequence,
                                         bool isFirstCondition) {
  assert(sequence->isSequence());
  Tree* result = SharedTreeStack->pushUserSequence(
      Symbol::GetName(sequence->child(k_nameIndex)));
  Tree* firstRank = sequence->child(k_firstRankIndex)->cloneTree();
  if (!isFirstCondition) {
    firstRank->moveTreeOverTree(
        PatternMatching::CreateSimplify(KAdd(KA, 1_e), {.KA = firstRank}));
  }
  return result;
}

bool Sequence::MainExpressionContainsForbiddenTerms(
    const Tree* e, const char* name, Type type, int initialRank, bool recursion,
    bool systemSymbol, bool otherSequences) {
  if (!Dimension::DeepCheck(e) || !Dimension::IsNonListScalar(e)) {
    return true;
  }
  const Tree* skipUntil = e;
  for (const Tree* d : e->selfAndDescendants()) {
    if (d < skipUntil) {
      continue;
    }
    if (d->isRandomized()) {
      return true;
    }
    if (!systemSymbol && d->treeIsIdenticalTo(KUnknownSymbol)) {
      return true;
    }
    if (!d->isUserSequence()) {
      continue;
    }
    if (strcmp(Symbol::GetName(d), name) != 0) {
      if (!otherSequences) {
        return true;
      }
      continue;
    }
    if (type == Type::SequenceExplicit) {
      // Forbid u(k) from appearing in explicit u(n) definition
      return true;
    }
    const Tree* rank = d->child(0);
    if (rank->isInteger()) {
      /* u(k) is allowed only when it is an initial condition
       * initialRank is max 255, but rank could be 256 and still valid if it's a
       * DoubleRecurrence sequence */
      IntegerHandler rankHandler = Integer::Handler(rank);
      if (rankHandler.is<int>()) {
        int rankValue = rankHandler.to<int>();
        if (rankValue == initialRank ||
            (type == Type::SequenceDoubleRecurrence &&
             rankValue == initialRank + 1)) {
          continue;
        }
      }
      // if rank !is<int>(), then it's definitely too big to be valid
      return true;
    }
    // Recursion on a sequence is allowed only on u(n) (or u(n+1) if double rec)
    if (recursion && (rank->treeIsIdenticalTo(KUnknownSymbol) ||
                      (type == Type::SequenceDoubleRecurrence &&
                       rank->treeIsIdenticalTo(KAdd(KUnknownSymbol, 1_e))))) {
      // Ignore the child content which has been checked already
      skipUntil = d->nextTree();
      continue;
    }
    return true;
  }
  return false;
}

}  // namespace Poincare::Internal
