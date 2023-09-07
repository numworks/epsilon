#ifndef POINCARE_EXPRESSION_VARIABLES_H
#define POINCARE_EXPRESSION_VARIABLES_H

#include <poincare_junior/src/memory/tree.h>

namespace PoincareJ {

struct Variables {
  // Push a Set with the variables of the expression
  static Tree* GetVariables(const Tree* t);
};

}  // namespace PoincareJ
#endif
