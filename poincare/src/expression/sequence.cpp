#include "sequence.h"

#include <assert.h>
#include <omg/unreachable.h>
#include <string.h>

#include "integer.h"
#include "k_tree.h"
#include "symbol.h"

namespace Poincare::Internal {

Sequence::Type Sequence::GetType(const Tree* sequence) {
  switch (sequence->type()) {
    case Internal::Type::SequenceExplicit:
      return Type::Explicit;
    case Internal::Type::SequenceSingleRecurrence:
      return Type::SingleRecurrence;
    case Internal::Type::SequenceDoubleRecurrence:
      return Type::DoubleRecurrence;
    default:
      OMG::unreachable();
  }
}

int Sequence::InitialRank(const Tree* sequence) {
  assert(sequence->isSequence());
  assert(sequence->child(1)->isInteger());
  return Integer::Handler(sequence->child(1)).to<int>();
}

bool Sequence::MainExpressionContainsForbiddenTerms(
    const Tree* e, const char* name, Type type, int initialRank, bool recursion,
    bool systemSymbol, bool otherSequences) {
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
    const Tree* rank = d->child(0);
    if (rank->isInteger()) {
      // u(k) is allowed only when it is an initial condition
      int rankValue = Integer::Handler(rank).to<int>();
      if ((type != Type::Explicit && rankValue == initialRank) ||
          (type == Type::DoubleRecurrence && rankValue == initialRank + 1)) {
        continue;
      }
      return true;
    }
    // Recursion on a sequence is allowed only on u(n) (or u(n+1) if double rec)
    if (recursion &&
        ((type != Type::Explicit && rank->treeIsIdenticalTo(KUnknownSymbol)) ||
         (type == Type::DoubleRecurrence &&
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
