#include <iostream>
#include <poincare_junior/tree_cache.h>
#include <poincare_junior/tree_sandbox.h>

using namespace Poincare;

/*
 * Key points:
 * - User interruptions
 * - System checkpoints
 *
 * */

void printLinearPool(TreePool * pool) {
  TreeBlock * b = pool->firstBlock();
  while (b < pool->lastBlock()) {
    std::cout << b->log() << std::endl;
    b = pool->nextBlock(b);
  }
}

void printIndentation(int deep) {
  for (int k = 0; k < deep + 1; k++) {
    std::cout << "\t";
  }
}

TreeBlock * printTreePoolRec(TreePool * pool, TreeBlock * block, int deep) {
  std::cout << block->log() << std::endl;
  TreeBlock * child = pool->nextBlock(block);
  for (int i = 0; i < block->numberOfSubtrees(); i++) {
    printIndentation(deep);
    printTreePoolRec(pool, child, deep + 1);
    child = pool->nextTree(child);
  }
  return child;
}

void printTreePool(TreePool * pool) {
  TreeBlock * b = pool->firstBlock();
  int counter = 0;
  while (b && b < pool->lastBlock()) {
    std::cout << "---------------------------------- Tree n° " << counter++ << "----------------------------------" << std::endl;
    b = printTreePoolRec(pool, b, 0);
    std::cout << "------------------------------------------------------------------------------" << std::endl;
  }
}

int main() {
  // "1 * 2 + 3";
  TreeCache * cache = TreeCache::sharedCache();
  cache->pushBlock(AdditionBlock());
  cache->pushBlock(MultiplicationBlock());
  cache->pushBlock(IntegerBlock());
  cache->pushBlock(TreeBlock(1));
  cache->pushBlock(IntegerBlock());
  cache->pushBlock(TreeBlock(2));
  cache->pushBlock(IntegerBlock());
  cache->pushBlock(TreeBlock(3));

  int treeId = cache->storeLastTree();
  printTreePool(cache);

  cache->copyTreeForEditing(treeId);
  printTreePool(cache->sandbox());

  cache->replaceBlock(cache->sandboxBlockAtIndex(3), TreeBlock(4));
  cache->replaceBlock(cache->sandboxBlockAtIndex(5), TreeBlock(5));
  cache->replaceBlock(cache->sandboxBlockAtIndex(7), TreeBlock(6));
  treeId = cache->storeLastTree();

  printTreePool(cache);
}
