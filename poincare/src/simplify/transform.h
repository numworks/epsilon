#ifndef POINCARE_SIMPLIFY_TRANSFORM_H
#define POINCARE_SIMPLIFY_TRANSFORM_H

#include <poincare/expression.h>

namespace Poincare {

class Transform {
public:
  virtual void apply(Expression * root, Expression * captures[]) const = 0;
};

}

#endif
