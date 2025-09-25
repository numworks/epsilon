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

/* Returns true if the main expression of a sequence is invalid.
 * Parameters are:
 * - e -> sequence's main expression
 * - symbolContext -> used to check dimension
 * - name, type, initialRank and shiftedNotation -> attributes of the sequence
 * - recursion -> indicates whether recursion is allowed
 * - systemSymbol -> indicates whether system nodes are allowed
 * - otherSequences -> indicates whether other sequences are allowed
 */
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
    if (!Symbol::IsNamed(d, name)) {
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
    if (!recursion) {
      return true;
    } else if (!((rank->treeIsIdenticalTo(shiftedNotation ? k_firstPreviousRank
                                                          : k_defaultRank)) ||
                 (type == Type::SequenceDoubleRecurrence &&
                  rank->treeIsIdenticalTo(shiftedNotation
                                              ? k_secondPreviousRank
                                              : k_firstFollowingRank)))) {
      /* Recursion on a sequence is allowed only on
       * - u(n) for single recurrence (or u(n-1) with shifted notation)
       * - u(n) and u(n+1) for double recurrence (or u(n-1) and u(n-2) with
       *   shifted notation) */
      return true;
    } else {
      // Ignore the child content which has been checked already
      skipUntil = d->nextTree();
    }
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

/* Update a sequence expression when changing from default to shifted notation
 * or back, so that it stays mathematically correct.
 * Example: u(n+1)=v(n+1)+3u(n)*n <-> u(n)=v(n)+3u(n-1)*(n-1) */
void Sequence::UpdateMainExpressionForNotation(Tree* e, Type type,
                                               bool toShiftedNotation) {
  assert(type != Type::SequenceExplicit);
  bool changed = false;
  if (toShiftedNotation) {
    /* Replace n and common default notation ranks n+1 and n+2.
     * Other ranks will be changed when n is identified in the children.
     * Example: u(n+1) -> u(n) and u(n+3) -> u((n-1)+3) for single rec. */
    changed =
        /* Replace n with:
         * - n-1 if single rec
         * - n-2 if double rec */
        PatternMatching::MatchReplace(e, k_defaultRank,
                                      type == Type::SequenceSingleRecurrence
                                          ? k_firstPreviousRank
                                          : k_secondPreviousRank) ||
        /* Replace n+1 with:
         * - n if single rec
         * - n-1 if double rec */
        PatternMatching::MatchReplace(e, k_firstFollowingRank,
                                      type == Type::SequenceSingleRecurrence
                                          ? k_defaultRank
                                          : k_firstPreviousRank) ||
        /* Replace n+2 with:
         * - n if double rec */
        (type == Type::SequenceDoubleRecurrence &&
         PatternMatching::MatchReplace(e, k_secondFollowingRank,
                                       k_defaultRank)) ||
        changed;
  } else {
    /* Replace n and common shifted notation ranks n-1 and n-2.
     * Other ranks will be changed when n is identified in the children.
     * Example: u(n-1) -> u(n+1) and u(n+1) -> u((n+2)+1) for double rec. */
    changed =
        /* Replace n with:
         * - n+1 if single rec
         * - n+2 if double rec */
        PatternMatching::MatchReplace(e, k_defaultRank,
                                      type == Type::SequenceSingleRecurrence
                                          ? k_firstFollowingRank
                                          : k_secondFollowingRank) ||
        /* Replace n-1 with:
         * - n if single rec
         * - n+1 if double rec */
        PatternMatching::MatchReplace(e, k_firstPreviousRank,
                                      type == Type::SequenceSingleRecurrence
                                          ? k_defaultRank
                                          : k_firstFollowingRank) ||
        /* Replace n-2 with:
         * - n if double rec */
        (type == Type::SequenceDoubleRecurrence &&
         PatternMatching::MatchReplace(e, k_secondPreviousRank,
                                       k_defaultRank)) ||
        changed;
  }
  if (!changed) {
    /* Replace in children if the expression was not already replaced (because
     * it was an identifiable rank). */
    for (Tree* child : e->children()) {
      UpdateMainExpressionForNotation(child, type, toShiftedNotation);
    }
  }
}

}  // namespace Poincare::Internal
