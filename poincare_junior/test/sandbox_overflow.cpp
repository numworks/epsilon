#include "print.h"

using namespace Poincare;

int initCache(TreeCache * cache, TreeSandbox * sandbox) {
  std::cout << "\n---------------- Store (1 + 2) * 3 * 4 in cache ----------------" << std::endl;
  Multiplication::PushNode(sandbox, 3);
  Addition::PushNode(sandbox, 2);
  Integer::PushNode(sandbox, 1);
  Integer::PushNode(sandbox, 2);
  Integer::PushNode(sandbox, 3);
  Integer::PushNode(sandbox, 4);

  int treeId = cache->storeLastTree();
  print();
  return treeId;
}

void testOverflowTreeSandbox(TreeCache * cache, TreeSandbox * sandbox) {
  // TEST 1
  int treeId = initCache(cache, sandbox);

  std::cout << "\n---------------- Fill cache with copies until cache is emptied and initial tree disappear" << std::endl;
  bool executed;
  do {
    executed = sandbox->execute(treeId, [](TypeTreeBlock *, TreeSandbox * sandbox) {});
    cache->storeLastTree();
    print();
  } while (executed);

  // TEST 2
  treeId = initCache(cache, sandbox);

  std::cout << "\n---------------- Fill cache with copies until almost full" << std::endl;
  TypeTreeBlock * tree = cache->treeForIdentifier(treeId);
  TreeBlock buffer[100];
  tree->copyTo(buffer);
  int maxNumberOfTreesInCache = TreeCache::k_maxNumberOfBlocks/tree->treeSize() - 1;
  for (int i = 0; i < maxNumberOfTreesInCache; i++) {
    sandbox->execute(static_cast<TypeTreeBlock *>(buffer), [](TypeTreeBlock *, TreeSandbox * sandbox) {});
    cache->storeLastTree();
  }
  print();


  std::cout << "\n---------------- Edit another tree triggering a cache flush" << std::endl;
  sandbox->execute(static_cast<TypeTreeBlock *>(buffer), [](TypeTreeBlock *, TreeSandbox * sandbox) {});
  print();
}
