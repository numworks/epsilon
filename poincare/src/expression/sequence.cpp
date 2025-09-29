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

int Sequence::InitialRank(const Tree* sequence) {
  assert(sequence->isSequence());
  assert(sequence->child(k_firstRankIndex)->isInteger());
  return Integer::Handler(sequence->child(k_firstRankIndex)).to<int>();
}

Tree* Sequence::PushInitialConditionName(const Tree* sequence,
                                         bool isFirstCondition) {
  assert(sequence->isSequence());
  Tree* result = SharedTreeStack->pushUserSequence(
      Symbol::GetName(sequence->child(k_nameIndex)));
  Tree* firstRank = sequence->child(k_firstRankIndex)->cloneTree();
  if (!isFirstCondition) {
    firstRank->moveTreeOverTree(
        PatternMatching::CreateReduce(KAdd(KA, 1_e), {.KA = firstRank}));
  }
  return result;
}

bool Sequence::MainExpressionContainsForbiddenTerms(
    const Tree* e, const SymbolContext& symbolContext, const char* name,
    Type type, int initialRank, bool shiftedNotation, bool recursion,
    bool systemSymbol, bool otherSequences) {
  if (!Dimension::DeepCheck(e, symbolContext) ||
      !Dimension::IsNonListScalar(e, symbolContext)) {
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
    /* Recursion on a sequence is allowed only on
     * - u(n) for single recurrence (or u(n-1) with shifted notation)
     * - u(n) and u(n+1) for double recurrence (or u(n-1) and u(n-2) with
     *   shifted notation) */
    if (recursion &&
        (rank->treeIsIdenticalTo(shiftedNotation ? k_firstPreviousRank
                                                 : k_defaultRank) ||
         (type == Type::SequenceDoubleRecurrence &&
          rank->treeIsIdenticalTo(shiftedNotation ? k_secondPreviousRank
                                                  : k_firstFollowingRank)))) {
      // Ignore the child content which has been checked already
      skipUntil = d->nextTree();
      continue;
    }
    return true;
  }
  return false;
}

Tree* Sequence::InitialExpression(const char* name, Type type,
                                  bool shiftedNotation) {
  Tree* result = nullptr;
  switch (type) {
    case Type::SequenceExplicit:
      // No initial expression
      break;
    case Type::SequenceSingleRecurrence: {
      // u(n) (or u(n-1) for shifted notation)
      result = SharedTreeStack->pushUserSequence(name);
      (shiftedNotation ? k_firstPreviousRank : k_defaultRank)->cloneTree();
      break;
    }
    case Type::SequenceDoubleRecurrence: {
      // u(n+1)+u(n) (or u(n-1)+u(n-2) for shifted notation)
      result = SharedTreeStack->pushAdd(2);
      SharedTreeStack->pushUserSequence(name);
      (shiftedNotation ? k_firstPreviousRank : k_firstFollowingRank)
          ->cloneTree();
      SharedTreeStack->pushUserSequence(name);
      (shiftedNotation ? k_secondPreviousRank : k_defaultRank)->cloneTree();
      break;
    }
  }
  return result;
}

void Sequence::UpdateMainExpressionForNotation(Tree* e, Type type,
                                               bool shiftedNotation) {
  assert(type != Type::SequenceExplicit);
  for (Tree* t : e->selfAndDescendants()) {
    if (t->isUserSequence()) {
      Tree* rank = t->child(0);
      if (shiftedNotation) {
        /* Replace u(n) with:
         * - u(n-1) if single rec
         * - u(n-2) if double rec */
        PatternMatching::MatchReplace(rank, k_defaultRank,
                                      type == Type::SequenceSingleRecurrence
                                          ? k_firstPreviousRank
                                          : k_secondPreviousRank);
        /* Replace u(n+1) with:
         * - u(n) if single rec
         * - u(n-1) if double rec */
        PatternMatching::MatchReplace(rank, k_firstFollowingRank,
                                      type == Type::SequenceSingleRecurrence
                                          ? k_defaultRank
                                          : k_firstPreviousRank);
        /* Replace u(n+2) with:
         * - u(n) if double rec */
        if (type == Type::SequenceDoubleRecurrence) {
          PatternMatching::MatchReplace(rank, k_secondFollowingRank,
                                        k_defaultRank);
        }
      } else {
        /* Replace u(n) with:
         * - u(n+1) if single rec
         * - u(n+2) if double rec */
        PatternMatching::MatchReplace(rank, k_defaultRank,
                                      type == Type::SequenceSingleRecurrence
                                          ? k_firstFollowingRank
                                          : k_secondFollowingRank);
        /* Replace u(n-1) with:
         * - u(n) if single rec
         * - u(n+1) if double rec */
        PatternMatching::MatchReplace(rank, k_firstPreviousRank,
                                      type == Type::SequenceSingleRecurrence
                                          ? k_defaultRank
                                          : k_firstFollowingRank);
        /* Replace u(n-2) with:
         * - u(n) if double rec */
        if (type == Type::SequenceDoubleRecurrence) {
          PatternMatching::MatchReplace(rank, k_secondPreviousRank,
                                        k_defaultRank);
        }
      }
    }
  }
}

}  // namespace Poincare::Internal
