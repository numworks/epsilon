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
    b = b->nextBlock();
  }
}

void printIndentation(int deep) {
  for (int k = 0; k < deep + 1; k++) {
    std::cout << "\t";
  }
}

TreeBlock * printTreePoolRec(TreeBlock * block, int deep) {
  std::cout << block->log() << std::endl;
  for (TreeBlock * child : block->directChildren()) {
    printIndentation(deep);
    printTreePoolRec(child, deep + 1);
  }
  return block->nextTree();
}

void printTreePool(TreePool * pool) {
  TreeBlock * b = pool->firstBlock();
  int counter = 0;
  while (b && b < pool->lastBlock()) {
    std::cout << "---------------------------------- Tree n° " << counter++ << "----------------------------------" << std::endl;
    b = printTreePoolRec(b, 0);
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
  std::cout << "Cache Tree" << std::endl;
  printTreePool(cache);

  cache->copyTreeForEditing(treeId);
  std::cout << "Edited Tree which has overflowed" << std::endl;
  printTreePool(cache->sandbox());
  std::cout << "Cache Tree" << std::endl;
  printTreePool(cache);

  cache->replaceBlock(cache->sandboxBlockAtIndex(6), AdditionBlock());
  cache->replaceBlock(cache->sandboxBlockAtIndex(7), MultiplicationBlock());
  cache->pushBlock(TreeBlock(4));
  cache->pushBlock(TreeBlock(5));
  cache->pushBlock(TreeBlock(6));
  std::cout << "Edited Tree which has overflowed" << std::endl;
  printTreePool(cache->sandbox());

#if 0
  cache->replaceBlock(cache->sandboxBlockAtIndex(7), AdditionBlock());
  cache->pushBlock(TreeBlock(4));
  cache->pushBlock(TreeBlock(5));

  std << "Edited Tree which has overflowed" << std::endl;
  printTreePool(cache->sandbox());
  std << "Cache Tree" << std::endl;
  printTreePool(cache);
  cache->replaceBlock(cache->sandboxBlockAtIndex(3), TreeBlock(4));
  cache->replaceBlock(cache->sandboxBlockAtIndex(5), TreeBlock(5));
  cache->replaceBlock(cache->sandboxBlockAtIndex(7), TreeBlock(6));
  treeId = cache->storeLastTree();

  printTreePool(cache);
#endif
}
