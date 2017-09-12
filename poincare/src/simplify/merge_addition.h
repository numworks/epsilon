#ifndef POINCARE_SIMPLIFY_MERGE_ADDITION_H
#define POINCARE_SIMPLIFY_MERGE_ADDITION_H

#include "transform.h"

namespace Poincare {

class MergeAddition : public Transform {
public:
  constexpr MergeAddition() {};
  void apply(Expression * root, Expression * captures[]) const override;
};

}

#endif
