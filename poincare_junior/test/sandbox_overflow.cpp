#include "print.h"
#include <poincare_junior/cached_tree.h>

using namespace Poincare;

bool createTree() {
  std::cout << "\n---------------- Store (1 + 2) * 3 * 4 in cache ----------------" << std::endl;
  Multiplication::PushNode(3);
  Addition::PushNode(2);
  Integer::PushNode(1);
  Integer::PushNode(2);
  Integer::PushNode(3);
  Integer::PushNode(4);

  return true;
}

void createTreeInCache() {
  CachedTree tree(createTree);
  // Force instantiation in cache
  tree.send(
    [](TypeTreeBlock * tree, void * resultAddress) {},
    nullptr
  );
  print();
}

void testOverflowTreeSandbox(TreeCache * cache) {
  // TEST 1
  std::cout << "\n---------------- Fill cache with copies until almost full" << std::endl;
  CachedTree tree(createTree);
  int treeSize;
  tree.send(
    [](TypeTreeBlock * tree, void * resultAddress) {
      *static_cast<int *>(resultAddress) = tree->treeSize();
    },
    &treeSize
  );
  int maxNumberOfTreesInCache = TreeCache::k_maxNumberOfBlocks/treeSize - 1;
  for (int i = 0; i < maxNumberOfTreesInCache; i++) {
    createTreeInCache();
  }

  std::cout << "\n---------------- Edit another tree triggering a cache flush" << std::endl;
  createTreeInCache();
}
