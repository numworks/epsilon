#include "function_cell.h"

namespace Graph {

constexpr KDColor FunctionCell::k_desactiveTextColor;

FunctionCell::FunctionCell() :
  EvenOddCell(),
  m_function(nullptr)
{
}

void FunctionCell::setFunction(Function * f) {
  m_function = f;
  markRectAsDirty(bounds());
}

Function * FunctionCell::function() {
  return m_function;
}

}
