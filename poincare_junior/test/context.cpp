#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/memory/context.h>

#include "helper.h"

using namespace PoincareJ;

QUIZ_CASE(pcj_storage_context) {
  quiz_assert(!Context::TreeForIdentifier("x"));
  quiz_assert(!Context::TreeForIdentifier("y"));
  Tree* tree = KAdd("x"_e, 2_e, "y"_e)->clone();
  quiz_assert(!Context::DeepReplaceIdentifiersWithTrees(tree));
  quiz_assert(tree->treeIsIdenticalTo(KAdd("x"_e, 2_e, "y"_e)));

  Context::SetTreeForIdentifier(KMult("x"_e, "x"_e), "y");
  quiz_assert(
      Context::TreeForIdentifier("y")->treeIsIdenticalTo(KMult("x"_e, "x"_e)));
  quiz_assert(Context::DeepReplaceIdentifiersWithTrees(tree));
  quiz_assert(tree->treeIsIdenticalTo(KAdd("x"_e, 2_e, KMult("x"_e, "x"_e))));

  tree->cloneTreeOverTree(KAdd("x"_e, 2_e, "y"_e));
  Context::SetTreeForIdentifier(KAdd(1_e, π_e), "x");
  quiz_assert(
      Context::TreeForIdentifier("x")->treeIsIdenticalTo(KAdd(1_e, π_e)));
  quiz_assert(Context::DeepReplaceIdentifiersWithTrees(tree));
  quiz_assert(tree->treeIsIdenticalTo(
      KAdd(KAdd(1_e, π_e), 2_e, KMult(KAdd(1_e, π_e), KAdd(1_e, π_e)))));

  tree->removeTree();
  Context::DeleteTreeForIdentifier("x");
  quiz_assert(!Context::TreeForIdentifier("x"));
  Context::DeleteTreeForIdentifier("y");
  quiz_assert(!Context::TreeForIdentifier("y"));
}
