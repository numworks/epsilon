#include "print.h"
#include <poincare_junior/include/expression.h>
#include <poincare_junior/include/layout.h>

using namespace PoincareJ;

void testLayoutCacheSharedPointer() {
  CachePool * cachePool = CachePool::sharedCachePool();
  cachePool->reset();

  Expression e = Expression::ParseFromText("-1+2*3");

  // l is created with e.m_id different from 1
  assert(e.id() != 1);
  Layout l = Layout::ToLayout(&e);

  // Forcing e.m_id change
  cachePool->needFreeBlocks(1);
  assert(e.id() == 1);

  // This test should fail if this line is uncommented
  // e = Expression::ParseFromText("2*3");

  // l should handle new e.m_id
  l.id();
}

void testLayoutCreation() {
  Layout l1 = Layout::Parse("-1+2*3");
  Expression e1 = Expression::ParseFromText("-1+2*3");
  Layout l2 = Layout::ToLayout(&e1);
  assert(l1.treeIsIdenticalTo(l2));
}

QUIZ_CASE(pcj_layout_creation) { testLayoutCreation(); }
QUIZ_CASE(pcj_layout_shared_pointer) { testLayoutCacheSharedPointer(); }
