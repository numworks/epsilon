#include <iostream>
#include "print.h"

int elementaryTreeManipulation(TreeCache * cache, TreeSandbox * sandbox);
int testOverflowTreeSandbox(TreeCache * cache);
int testOverflowCacheIdentifiers(TreeCache * cache);
void playWithCachedTree();

int main() {
  TreeCache * cache = TreeCache::sharedCache();
  TreeSandbox * sandbox = cache->sandbox();

  std::cout << "\n\n ELEMENTARY TREE MANIPULATION \n" << std::endl;
  elementaryTreeManipulation(cache, sandbox);
  cache->reset(false);

  std::cout << "\n\n TEST OVERFLOW TREE SANDBOX \n" << std::endl;
  testOverflowTreeSandbox(cache);
  cache->reset(false);

  std::cout << "\n\n TEST OVERFLOW CACHE IDENTIFIERS \n" << std::endl;
  testOverflowCacheIdentifiers(cache);
  cache->reset(false);

  std::cout << "\n\n TEST CACHED TREE \n" << std::endl;
  playWithCachedTree();
  cache->reset(false);

  intermediaryPrint();
}

#if 0

projection to internal expression (remove ln, /, -...)
basic_simplication
expand_trig
contract_trig
expand_transcendantal
contract_transcendantal
polynomial_simplification
--> expand + normalize
-->polynomial_interpretation_with_grobner_basis
#endif
