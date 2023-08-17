#ifndef POINCARE_EXPRESSION_VECTOR_H
#define POINCARE_EXPRESSION_VECTOR_H

#include "matrix.h"

namespace PoincareJ {

struct Vector {
  static Tree* Cross(const Tree* u, const Tree* v);
  static Tree* Dot(const Tree* u, const Tree* v);
  static Tree* Norm(const Tree* v);
};

}  // namespace PoincareJ

#endif
