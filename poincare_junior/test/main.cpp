#include <iostream>
#include "print.h"

void elementaryTreeManipulation();
void testOverflowEditionPool();
void testOverflowCacheIdentifiers();
void testCalculation();
void testGraph();
void testExpressionComparison();
void testRunTimeCrashIllFormedExpression();
void testSet();
void testVariables();
void testBlock();
void testTypeBlock();
void testConstexprTreeConstructor();
void testEditionNodeConstructor();
void testNodeIterator();
void testNode();
void testNodeSize();
void testEditionPool();
void testEditionReference();
void testCachePool();
void testCachePoolLimits();

typedef void (*Test)();

void test(Test test, const char * title) {
  std::cout << "\n\n--------------------------------\n" << std::endl;
  std::cout << title << std::endl;
  std::cout << "\n--------------------------------\n" << std::endl;
  test();
  CachePool * cachePool = CachePool::sharedCachePool();
  cachePool->reset();
  std::cout << "\n--------------------------------\n" << std::endl;
  std::cout << "\n--------------------------------\n" << std::endl;
}

int main() {
  // Dummy call just to keep it in the executable and be able to call it from debugger
  intermediaryPrint();

  test(testBlock, "TEST BLOCK");
  test(testTypeBlock, "TEST TYPE BLOCK");
  test(testConstexprTreeConstructor, "TEST CONSTEXPR TREE CONSTRUCTOR");
  test(testEditionNodeConstructor, "TEST EDITION NODE CONSTRUCTOR");
  test(testNodeIterator, "TEST NODE ITERATOR");
  test(testNode, "TEST NODE");
  test(testNodeSize, "TEST NODE SIZE");
  test(testEditionPool, "TEST EDITION POOL");
  test(testEditionReference, "TEST EDITION REFERENCE");
  test(testCachePool, "TEST CACHE POOL");
  test(testCachePoolLimits, "TEST CACHE POOL LIMITS");

  test(testVariables, "TEST VARIABLES");
  test(testSet, "TEST SET");

  test(elementaryTreeManipulation, "ELEMENTARY TREE MANIPULATION");
  test(testCalculation, "TEST DUMMY CALCULATION");
  test(testGraph, "TEST DUMMY GRAPH");
  test(testExpressionComparison, "TEST EXPRESSION COMPARISON");
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
