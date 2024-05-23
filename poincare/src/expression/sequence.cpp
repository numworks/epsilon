#include "sequence.h"

#include <assert.h>
#include <string.h>

#include "integer.h"
#include "k_tree.h"
#include "symbol.h"

namespace Poincare::Internal {

bool Sequence::MainExpressionContainsForbiddenTerms(
    const Tree* e, const char* name, Type type, int initialRank, bool recursion,
    bool systemSymbol, bool otherSequences) {
  for (const Tree* d : e->selfAndDescendants()) {
    if (d->isRandomNode()) {
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
      int rankValue = Integer::Handler(rank).to<int>();
      if ((type != Type::Explicit && rankValue == initialRank) ||
          (type == Type::DoubleRecurrence && rankValue == initialRank + 1)) {
        continue;
      }
      return true;
    }
    if (recursion &&
        ((type != Type::Explicit && rank->treeIsIdenticalTo(KUnknownSymbol)) ||
         (type == Type::DoubleRecurrence &&
          rank->treeIsIdenticalTo(KAdd(KUnknownSymbol, 1_e))))) {
      // TODO this assert was not there but return TrinaryFalse was replaced by
      // continue
      assert(systemSymbol);
      continue;
    }
    return true;
  }
  return false;
}

}  // namespace Poincare::Internal
