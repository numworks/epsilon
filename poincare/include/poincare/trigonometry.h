#ifndef POINCARE_TRIGONOMETRY_H
#define POINCARE_TRIGONOMETRY_H

#include <poincare/expression.h>

namespace Poincare {

class Trigonometry {
public:
  enum class Function {
    Cosine = 0,
    Sine = 1,
  };
  static Expression * table(const Expression * e, Function f, bool inverse); // , Function f, bool inverse
  constexpr static int k_numberOfEntries = 3;
};

}

#endif
