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
  for (TreeBlock * child : block->directSubtrees()) {
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

#if 0
void deepReduce(TreeBlock * block) {
  BlockType blockType = block->type();
  if (blockType == BlockType::Integer) {
    return;
  }
  for (TreeBlock * child : block->directChildren()) {
    deepReduce(child);
  }
  uint8_t valueA = block->nextNthBlock(2)->value();
  uint8_t valueB = block->nextNthBlock(4)->value();
  uint8_t result;
  if (blockType == BlockType::Addition) {
    result = valueA + valueB;
  } else {
    assert(blockType == BlockType::Multiplication);
    result = valueA * valueB;
  } 
  for (TreeBlock * child : block->directChildren()) {
    child
  }
}

/*
 * void deepReduce(TreeBlock * block) {
 BlockType blockType = block->type();
 if (blockType == BlockType::Integer) {
 return;
 }
 for (TreeBlock * child : block->directChildren()) {
 deepReduce(child);
 }
 block->expression()->reduce();
 }
 * */

#endif

int main() {
  // "1 * 2 + 3";
  // Parsing
  TreeCache * cache = TreeCache::sharedCache();
  TreeSandbox * sandbox = cache->sandbox();
  sandbox->pushBlock(AdditionBlock());
  sandbox->pushBlock(MultiplicationBlock());
  sandbox->pushBlock(IntegerBlock());
  sandbox->pushBlock(TreeBlock(1));
  sandbox->pushBlock(IntegerBlock());
  sandbox->pushBlock(TreeBlock(2));
  sandbox->pushBlock(IntegerBlock());
  sandbox->pushBlock(TreeBlock(3));

  std::cout << "-------- SANDBOX --------" << std::endl;
  printTreePool(sandbox);

  std::cout << "-------- BACKWARD SCAN --------" << std::endl;
  TreeBlock * root = sandbox->firstBlock();
  for (TreeBlock * subTree : root->backwardsDirectSubtrees()) {
    std::cout << subTree->log() << std::endl;
  }

  // Reducing
  int treeId = cache->storeLastTree();
  std::cout << "-------- CACHE --------" << std::endl;
  printTreePool(cache);

#if 0
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
