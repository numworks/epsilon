#pragma once

#include "matrix.h"

namespace Poincare::Internal {

struct Vector {
  static bool IsVector(const Tree* e);
  static Tree* Cross(const Tree* e1, const Tree* e2);
  static Tree* Dot(const Tree* e1, const Tree* e2);
  static Tree* Norm(const Tree* e);
};

}  // namespace Poincare::Internal
