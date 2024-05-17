#ifndef POINCARE_EXPRESSION_DEGREE_H
#define POINCARE_EXPRESSION_DEGREE_H

#include <poincare/src/memory/tree.h>

#include "projection.h"

namespace Poincare::Internal {

class Degree {
 public:
  // Return polynomial degree of any un projected expression on given variable.
  static int Get(const Tree* tree, const Tree* variable,
                 ProjectionContext projectionContext);
};

}  // namespace Poincare::Internal

#endif
