#include <poincare/equation_solver/equation_solver_properties.h>
#include <poincare/src/memory/tree.h>

namespace Poincare::EquationSolver {

void VariableArray::fillWithList(const Internal::Tree* list) {
  assert(list->isList() && list->numberOfChildren() <= capacity());
  clear();
  for (const Internal::Tree* variable : list->children()) {
    push(SymbolHelper::GetName(variable));
  }
}

}  // namespace Poincare::EquationSolver
