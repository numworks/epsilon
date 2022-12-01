#include <iostream>
#include <poincare_junior/src/memory/cache_pool.h>
#include <poincare_junior/src/memory/edition_pool.h>

using namespace Poincare;

inline Node createSimpleExpression() {
  std::cout << "\n---------------- Create (1 + 2) * 3 * 4 ----------------" << std::endl;
  Node multiplication = Node::Push<BlockType::Multiplication>(3);
  Node::Push<BlockType::Addition>(2);
  Node::Push<BlockType::IntegerShort>(1);
  Node::Push<BlockType::IntegerShort>(2);
  Node::Push<BlockType::IntegerShort>(3);
  Node::Push<BlockType::Addition>(2);
  Node::Push<BlockType::IntegerShort>(4);
  Node::Push<BlockType::IntegerShort>(5);
  return multiplication;
}

inline void print() {
  CachePool * cachePool = CachePool::sharedCachePool();
  EditionPool * editionPool = cachePool->editionPool();

  std::cout << "\n\n\n\nxxxxxxxxxxx MEMORY DUMP xxxxxxxxxxxx" << std::endl;
  std::cout << "\n========= CACHE POOL =========" << std::endl;
  cachePool->treeLog(std::cout);

  std::cout << "\n========= EDITION POOL =========" << std::endl;
  editionPool->treeLog(std::cout);
  std::cout << "\nxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << std::endl;
}

inline void intermediaryPrint() {
  CachePool * cachePool = CachePool::sharedCachePool();
  EditionPool * editionPool = cachePool->editionPool();

  std::cout << "\n========= CACHE POOL =========" << std::endl;
  cachePool->treeLog(std::cout);

  std::cout << "\n========= INCOMPLETE EDITION POOL =========" << std::endl;
  editionPool->flatLog(std::cout);
  std::cout << "\nxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << std::endl;
}
