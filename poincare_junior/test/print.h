#include <iostream>
#include <poincare_junior/cache_pool.h>
#include <poincare_junior/edition_pool.h>

using namespace Poincare;

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
