#include "print.h"

using namespace Poincare;

int initCache(TreeCache * cache) {
  std::cout << "\n---------------- Store (1 + 2) * 3 * 4 in cache ----------------" << std::endl;
  Multiplication::PushNode(3);
  Addition::PushNode(2);
  Integer::PushNode(1);
  Integer::PushNode(2);
  Integer::PushNode(3);
  Integer::PushNode(4);

  int treeId = cache->storeLastTree();
  print();
  return treeId;
}

void testOverflowTreeSandbox(TreeCache * cache) {
  // TEST 1
  int treeId = initCache(cache);

  std::cout << "\n---------------- Fill cache with copies until cache is emptied and initial tree disappear" << std::endl;
  do {
    treeId = cache->execute(treeId, [](TypeTreeBlock *) {});
    print();
  } while (treeId >= 0);

  // TEST 2
  treeId = initCache(cache);

  std::cout << "\n---------------- Fill cache with copies until almost full" << std::endl;
  TypeTreeBlock * tree = cache->treeForIdentifier(treeId);
  TreeBlock buffer[100];
  tree->copyTo(buffer);
  int maxNumberOfTreesInCache = TreeCache::k_maxNumberOfBlocks/tree->treeSize() - 1;
  for (int i = 0; i < maxNumberOfTreesInCache; i++) {
    cache->execute(static_cast<TypeTreeBlock *>(buffer), [](TypeTreeBlock *) {});
  }
  print();


  std::cout << "\n---------------- Edit another tree triggering a cache flush" << std::endl;
  cache->execute(static_cast<TypeTreeBlock *>(buffer), [](TypeTreeBlock *) {});
  print();
}
