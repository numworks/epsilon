#include <iostream>
#include <poincare_junior/handle.h>
#include <poincare_junior/tree_cache.h>
#include <poincare_junior/tree_sandbox.h>

using namespace Poincare;

/*
 * Key points:
 * - User interruptions
 * - System checkpoints
 *
 * */

/*void printLinearPool(TreePool * pool) {
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
}*/

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
 for (TreeBlock * child : block->directChildren()) {
 deepReduce(child);
 }
 block->expression()->reduce();
 }

 
 * */

#endif

int main() {
  // "1 * 2 + 3 + 4";
  // Parsing
  TreeCache * cache = TreeCache::sharedCache();
  TreeSandbox * sandbox = cache->sandbox();
  Addition::NodeBuilder(sandbox, 3);
  Multiplication::NodeBuilder(sandbox, 2);
  Integer::NodeBuilder(sandbox, 1);
  Integer::NodeBuilder(sandbox, 2);
  Integer::NodeBuilder(sandbox, 3);
  Integer::NodeBuilder(sandbox, 4);

  std::cout << "-------- SANDBOX --------" << std::endl;
  sandbox->treeLog(std::cout);

  std::cout << "-------- BACKWARD SCAN --------" << std::endl;
  TypeTreeBlock * root = sandbox->firstBlock();
  for (TypeTreeBlock * subTree : root->backwardsDirectChildren()) {
    subTree->log(std::cout);
  }

  // Reducing
  int treeId = cache->storeLastTree();
  std::cout << "\n\n-------- CACHE --------" << std::endl;
  cache->treeLog(std::cout);

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

//StackPointer given to all arguments indicating where to play
