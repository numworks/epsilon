#include "print.h"
#include <poincare_junior/cache_reference.h>

using namespace Poincare;

void createTree() {
  createSimpleExpression();
}

void createSmallTree() {
  Node::Push<Constant>(u'π');
}

CacheReference createTreeInCache(CacheReference::Initializer initializer) {
  CacheReference tree(initializer);
  // Force instantiation in cache
  tree.send(
    [](const Node tree, void * resultAddress) {},
    nullptr
  );
  return tree;
}

void testOverflowEditionPool() {
  std::cout << "---------------- Fill cache with copies until almost full" << std::endl;
  CacheReference tree(createTree);
  int treeSize;
  tree.send(
    [](const Node tree, void * resultAddress) {
      *static_cast<int *>(resultAddress) = tree.treeSize();
    },
    &treeSize
  );
  int maxNumberOfTreesInCache = CachePool::k_maxNumberOfBlocks/treeSize - 1;
  for (int i = 0; i < maxNumberOfTreesInCache; i++) {
    createTreeInCache(createTree);
  }
  print();

  std::cout << "---------------- Edit another tree triggering a cache flush" << std::endl;
  createTreeInCache(createTree);
  print();
}

void testOverflowCacheIdentifiers() {
  std::cout << "---------------- Fill cache with the maximum number of trees" << std::endl;
  for (int i = 0; i < Pool::k_maxNumberOfReferences; i++) {
    createTreeInCache(createSmallTree);
  }
  print();

  std::cout << "---------------- Create another tree triggering a cache flush" << std::endl;
  CacheReference ref = createTreeInCache(createSmallTree);
  ref.log();
}


