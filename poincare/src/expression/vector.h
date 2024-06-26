#ifndef POINCARE_EXPRESSION_VECTOR_H
#define POINCARE_EXPRESSION_VECTOR_H

#include "matrix.h"

namespace Poincare::Internal {

struct Vector {
  static Tree* Cross(const Tree* e1, const Tree* e2);
  static Tree* Dot(const Tree* e1, const Tree* e2);
  static Tree* Norm(const Tree* e);
};

}  // namespace Poincare::Internal

#endif
