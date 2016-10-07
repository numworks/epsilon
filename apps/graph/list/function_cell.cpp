#include "function_cell.h"

constexpr KDColor FunctionCell::k_desactiveTextColor;

FunctionCell::FunctionCell() :
  EvenOddCell(),
  m_function(nullptr)
{
}

void FunctionCell::setFunction(Graph::Function * f) {
  m_function = f;
  markRectAsDirty(bounds());
}

Graph::Function * FunctionCell::function() {
  return m_function;
}
