#include "merge_dynamic_hierarchy_transform.h"
#include <assert.h>
#include <poincare/dynamic_hierarchy.h>

namespace Poincare {
namespace Simplification {

void MergeDynamicHierarchyTransform::apply(Expression * captures[]) const {
  DynamicHierarchy * h0 = static_cast<DynamicHierarchy *>(captures[0]);
  DynamicHierarchy * h1 = static_cast<DynamicHierarchy *>(captures[1]);
  h0->removeOperand(h1, false);
  h0->addOperands(h1->operands(), h1->numberOfOperands());
  h1->detachOperands();
  delete h1;
}

}
}
