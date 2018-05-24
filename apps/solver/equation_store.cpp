#include "equation_store.h"

namespace Solver {

Equation * EquationStore::emptyModel() {
  static Equation e;
  return &e;
}

void EquationStore::setModelAtIndex(Shared::ExpressionModel * e, int i) {
  m_equations[i] = *(static_cast<Equation *>(e));;
}

}
