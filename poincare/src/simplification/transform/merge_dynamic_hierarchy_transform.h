#ifndef POINCARE_SIMPLIFICATION_TRANSFORM_MERGE_HIERARCHY_TRANSFORM_H
#define POINCARE_SIMPLIFICATION_TRANSFORM_MERGE_HIERARCHY_TRANSFORM_H

#include "transform.h"

namespace Poincare {
namespace Simplification {

// This transform expects two DynamicHierarchy in its captures.
// It will remove the second one, and put all its operands in the first.

class MergeDynamicHierarchyTransform : public Transform {
public:
  constexpr MergeDynamicHierarchyTransform() {};
  void apply(Expression * captures[]) const override;
};

}
}

#endif
