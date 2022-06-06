#include <iostream>
#include <poincare_junior/tree_cache.h>
#include <poincare_junior/tree_sandbox.h>

using namespace Poincare;

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

void printTreePoolRec(TreePool * pool, TreeBlock * block, int deep) {
  std::cout << block->log() << std::endl;
  TreeBlock * child = pool->nextBlock(block);
  for (int i = 0; i < block->numberOfSubtrees(); i++) {
    printIndentation(deep);
    printTreePoolRec(pool, child, deep + 1);
    child = pool->nextTree(child);
  }
}

void printTreePool(TreePool * pool) {
  TreeBlock * b = pool->firstBlock();
  printTreePoolRec(pool, b, 0);
}

int main() {
  // "1 * 2 + 3";
  TreeSandbox sandbox = TreeCache::sharedCache()->sandbox();
  sandbox.pushBlock(AdditionBlock());
  sandbox.pushBlock(MultiplicationBlock());
  sandbox.pushBlock(IntegerBlock());
  sandbox.pushBlock(TreeBlock(1));
  sandbox.pushBlock(IntegerBlock());
  sandbox.pushBlock(TreeBlock(2));
  sandbox.pushBlock(IntegerBlock());
  sandbox.pushBlock(TreeBlock(3));

  printTreePool(&sandbox);

  sandbox.replaceBlock(sandbox.blockAtIndex(3), TreeBlock(4));
  sandbox.replaceBlock(sandbox.blockAtIndex(5), TreeBlock(5));
  sandbox.replaceBlock(sandbox.blockAtIndex(7), TreeBlock(6));

  printTreePool(&sandbox);
}
