#include "print.h"
#include <poincare_junior/include/expression.h>
#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/memory/tree_constructor.h>

using namespace PoincareJ;

void assert_equal(const Layout l1, const Layout l2) {
  constexpr static int k_bufferSize = 128;
  TypeBlock t1[k_bufferSize];
  TypeBlock t2[k_bufferSize];
  l2.dumpAt(t2);
  l1.dumpAt(t1);
  assert(Node(t1).isIdenticalTo(Node(t2)));
}

void testLayoutCacheSharedPointer() {
  CachePool * cachePool = CachePool::sharedCachePool();
  cachePool->reset();

  Expression e = Expression::CreateExpressionFromText("-1+2*3");

  // l is created with e.m_id different from 1
  assert(e.id() != 1);
  Layout l = Layout::CreateLayoutFromExpression(&e);

  // Forcing e.m_id change
  cachePool->needFreeBlocks(1);
  assert(e.id() == 1);

  // This test should fail if this line is uncommented
  // e = Expression::CreateExpressionFromText("2*3");

  // l should handle new e.m_id
  l.id();
}

void testLayoutCreation() {
  Layout l1 = Layout::CreateLayoutFromText("-1+2*3");
  Expression e1 = Expression::CreateExpressionFromText("-1+2*3");
  Layout l2 = Layout::CreateLayoutFromExpression(&e1);
  assert_equal(l1, l2);
}

void testLayoutConstructors() {
  Node(RacL("1+"_l,ParL(RacL("2*"_l,ParL(RacL("1+"_l,FraL("1"_l, "2"_l))))),VerL("2"_l),"-2"_l)).log();
}

QUIZ_CASE(pcj_layout_creation) { testLayoutCreation(); }
QUIZ_CASE(pcj_layout_constructor) { testLayoutConstructors(); }
QUIZ_CASE(pcj_layout_shared_pointer) { testLayoutCacheSharedPointer(); }
