#include <iostream>
#include "print.h"

void elementaryTreeManipulation();
void testChildrenIterator();
void testOverflowEditionPool();
void testOverflowCacheIdentifiers();
void testCalculation();
void testGraph();
void playWithConstexprNodes();
void testRunTimeCrashIllFormedExpression();

typedef void (*Test)();

void test(Test test, const char * title) {
  std::cout << "\n\n--------------------------------\n" << std::endl;
  std::cout << title << std::endl;
  std::cout << "\n--------------------------------\n" << std::endl;
  test();
  CachePool::sharedCachePool()->reset();
  std::cout << "\n--------------------------------\n" << std::endl;
  std::cout << "\n--------------------------------\n" << std::endl;
}

int main() {
  // Dummy call just to keep it in the executable and be able to call it from debugger
  intermediaryPrint();

  test(elementaryTreeManipulation, "ELEMENTARY TREE MANIPULATION");
  test(testChildrenIterator, "TEST NODE CHILDREN ITERATOR");
  test(testOverflowEditionPool, "TEST OVERFLOW TREE EDITION POOL");
  test(testOverflowCacheIdentifiers, "TEST OVERFLOW CACHE IDENTIFIERS");
  test(testCalculation, "TEST DUMMY CALCULATION");
  test(testGraph, "TEST DUMMY GRAPH");
  test(playWithConstexprNodes, "TEST CONSTEXPR NODES");
  test(testRunTimeCrashIllFormedExpression, "TEST ILL-FORMED EXPRESSIONS");
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
