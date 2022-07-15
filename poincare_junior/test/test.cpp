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

void print() {
  TreeCache * cache = TreeCache::sharedCache();
  TreeSandbox * sandbox = cache->sandbox();

  std::cout << "\n-------- CACHE --------" << std::endl;
  cache->treeLog(std::cout);

  std::cout << "\n-------- SANDBOX --------" << std::endl;
  sandbox->treeLog(std::cout);
}

void intermediaryPrint() {
  TreeSandbox * sandbox = TreeCache::sharedCache()->sandbox();

  std::cout << "\n-------- INCOMPLETE SANDBOX --------" << std::endl;
  sandbox->flatLog(std::cout);
}

int main() {
  TreeCache * cache = TreeCache::sharedCache();
  TreeSandbox * sandbox = cache->sandbox();

  std::cout << "-------- Create (1 + 2) * 3 * 4 --------" << std::endl;
  Multiplication::PushNode(sandbox, 3);
  Addition::PushNode(sandbox, 2);
  Integer::PushNode(sandbox, 1);
  Integer::PushNode(sandbox, 2);
  Integer::PushNode(sandbox, 3);
  Integer::PushNode(sandbox, 4);

  print();

  std::cout << "-------- BACKWARD SCAN --------" << std::endl;
  TypeTreeBlock * root = sandbox->firstBlock();
  for (TypeTreeBlock * subTree : root->backwardsDirectChildren()) {
    subTree->log(std::cout);
  }

  std::cout << "-------- Store (1+2)*3*4 --------" << std::endl;
  int treeId = cache->storeLastTree();
  print();

  std::cout << "-------- Copy (1+2)*3*4 --------" << std::endl;
  cache->copyTreeForEditing(treeId);
  print();


  std::cout << "-------- Develop (1+2)*3*4 --------" << std::endl;
  root = sandbox->firstBlock();
  assert(root->type() == BlockType::MultiplicationHead);
  Multiplication m = Handle::Create<Multiplication>(root);
  Handle h = m.distributeOverAddition(sandbox);
  print();

#if 0
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
