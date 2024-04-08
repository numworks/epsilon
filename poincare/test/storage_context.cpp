#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/memory/storage_context.h>

#include "helper.h"

using namespace PoincareJ;

QUIZ_CASE(pcj_storage_context) {
  quiz_assert(!StorageContext::TreeForIdentifier("x"));
  quiz_assert(!StorageContext::TreeForIdentifier("y"));
  Tree* tree = KMult("x"_e, 2_e, "y"_e)->clone();
  quiz_assert(!StorageContext::DeepReplaceIdentifiersWithTrees(tree));
  quiz_assert(tree->treeIsIdenticalTo(KMult("x"_e, 2_e, "y"_e)));

  StorageContext::SetTreeForIdentifier(3_e, "y");
  quiz_assert(StorageContext::TreeForIdentifier("y")->treeIsIdenticalTo(3_e));
  quiz_assert(StorageContext::DeepReplaceIdentifiersWithTrees(tree));
  quiz_assert(tree->treeIsIdenticalTo(KMult("x"_e, 2_e, 3_e)));

  tree->cloneTreeOverTree(KMult("x"_e, 2_e, "y"_e));
  StorageContext::SetTreeForIdentifier(KAdd(1_e, π_e), "x");
  quiz_assert(StorageContext::TreeForIdentifier("x")->treeIsIdenticalTo(
      KAdd(1_e, π_e)));
  quiz_assert(StorageContext::DeepReplaceIdentifiersWithTrees(tree));
  quiz_assert(tree->treeIsIdenticalTo(KMult(KAdd(1_e, π_e), 2_e, 3_e)));

  tree->removeTree();
  StorageContext::DeleteTreeForIdentifier("x");
  quiz_assert(!StorageContext::TreeForIdentifier("x"));
  StorageContext::DeleteTreeForIdentifier("y");
  quiz_assert(!StorageContext::TreeForIdentifier("y"));
}
