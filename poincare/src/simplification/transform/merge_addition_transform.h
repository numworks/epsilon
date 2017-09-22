#ifndef POINCARE_SIMPLIFICATION_TRANSFORM_MERGE_ADDITION_TRANSFORM_H
#define POINCARE_SIMPLIFICATION_TRANSFORM_MERGE_ADDITION_TRANSFORM_H

#include "transform.h"

namespace Poincare {
namespace Simplification {

class MergeAdditionTransform : public Transform {
public:
  constexpr MergeAdditionTransform() {};
  void apply(Expression * root, Expression * captures[]) const override;
};

}
}

#endif
