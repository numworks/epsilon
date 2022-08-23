#include "print.h"
#include <poincare_junior/cached_tree.h>

using namespace Poincare;

bool createTree() {
  Multiplication::PushNode(3);
  Addition::PushNode(2);
  Integer::PushNode(1);
  Integer::PushNode(2);
  Integer::PushNode(3);
  Integer::PushNode(4);
  return true;
}

bool createSmallTree() {
  Integer::PushNode(1);
  return true;
}

void createTreeInCache(CachedTree::Initializer initializer) {
  CachedTree tree(initializer);
  // Force instantiation in cache
  tree.send(
    [](const TypeTreeBlock * tree, void * resultAddress) {},
    nullptr
  );
}

void testOverflowTreeSandbox(TreeCache * cache) {
  // TEST 1
  std::cout << "\n---------------- Fill cache with copies until almost full" << std::endl;
  CachedTree tree(createTree);
  int treeSize;
  tree.send(
    [](const TypeTreeBlock * tree, void * resultAddress) {
      *static_cast<int *>(resultAddress) = tree->treeSize();
    },
    &treeSize
  );
  int maxNumberOfTreesInCache = TreeCache::k_maxNumberOfBlocks/treeSize - 1;
  for (int i = 0; i < maxNumberOfTreesInCache; i++) {
    createTreeInCache(createTree);
  }
  print();

  std::cout << "\n---------------- Edit another tree triggering a cache flush" << std::endl;
  createTreeInCache(createTree);
  print();
}

void testOverflowCacheIdentifiers(TreeCache * cache) {
  std::cout << "\n---------------- Fill cache with the maximum number of trees" << std::endl;
  for (int i = 0; i < TreeCache::k_maxNumberOfCachedTrees; i++) {
    createTreeInCache(createSmallTree);
  }
  print();

  std::cout << "\n---------------- Create another tree triggering a cache flush" << std::endl;
  createTreeInCache(createSmallTree);
  print();
}


