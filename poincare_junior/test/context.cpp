#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/memory/context.h>

#include "helper.h"

using namespace PoincareJ;

QUIZ_CASE(pcj_storage_context) {
  quiz_assert(!Context::TreeForIdentifier("x"));
  Context::SetTreeForIdentifier(KAdd(1_e, "x"_e), "x");
  quiz_assert(
      Context::TreeForIdentifier("x")->treeIsIdenticalTo(KAdd(1_e, "x"_e)));
  Context::DeleteTreeForIdentifier("x");
  quiz_assert(!Context::TreeForIdentifier("x"));
}
