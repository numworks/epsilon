#ifndef POINCARE_SIMPLIFICATION_TRANSFORM_TRANSFORM_H
#define POINCARE_SIMPLIFICATION_TRANSFORM_TRANSFORM_H

#include <poincare/expression.h>

namespace Poincare {
namespace Simplification {

class Transform {
public:
  virtual void apply(Expression * root, Expression * captures[]) const = 0;
};

}
}

#endif
