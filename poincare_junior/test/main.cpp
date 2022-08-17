#include <iostream>
#include "print.h"

int elementaryTreeManipulation(TreeCache * cache, TreeSandbox * sandbox);
int testOverflowTreeSandbox(TreeCache * cache);
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

  std::cout << "\n\n TEST CACHED TREE \n" << std::endl;
  playWithCachedTree();
  cache->reset(false);

  intermediaryPrint();
}

//StackPointer given to all arguments indicating where to play: why? The sandbox remembers its end?
#if 0

Tests to implement:
Fill the cache of with too many identifiers


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
