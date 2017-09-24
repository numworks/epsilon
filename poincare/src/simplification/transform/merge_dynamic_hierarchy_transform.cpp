#include "transform.h"
#include <assert.h>
#include <poincare/dynamic_hierarchy.h>

bool Poincare::Simplification::MergeDynamicHierarchyTransform(Expression * captures[]) {
  DynamicHierarchy * h0 = static_cast<DynamicHierarchy *>(captures[0]);
  DynamicHierarchy * h1 = static_cast<DynamicHierarchy *>(captures[1]);
  h0->removeOperand(h1, false);
  h0->addOperands(h1->operands(), h1->numberOfOperands());
  h1->detachOperands();
  delete h1;
  return true;
}
