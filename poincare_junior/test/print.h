#include <iostream>
#include <poincare_junior/handle.h>
#include <poincare_junior/tree_cache.h>
#include <poincare_junior/tree_sandbox.h>

using namespace Poincare;

inline void print() {
  TreeCache * cache = TreeCache::sharedCache();
  TreeSandbox * sandbox = cache->sandbox();

  std::cout << "\n========= CACHE =========" << std::endl;
  cache->treeLog(std::cout);

  std::cout << "\n========= SANDBOX =========" << std::endl;
  sandbox->treeLog(std::cout);
}

inline void intermediaryPrint() {
  TreeCache * cache = TreeCache::sharedCache();
  TreeSandbox * sandbox = cache->sandbox();

  std::cout << "\n========= CACHE =========" << std::endl;
  cache->treeLog(std::cout);

  std::cout << "\n========= INCOMPLETE SANDBOX =========" << std::endl;
  sandbox->flatLog(std::cout);
}
