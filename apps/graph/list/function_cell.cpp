#include "function_cell.h"

constexpr KDColor FunctionCell::k_evenLineBackgroundColor;
constexpr KDColor FunctionCell::k_oddLineBackgroundColor;
constexpr KDColor FunctionCell::k_selectedLineBackgroundColor;
constexpr KDColor FunctionCell::k_desactiveTextColor;

FunctionCell::FunctionCell() :
  ChildlessView(),
  m_highlighted(false),
  m_even(false),
  m_function(nullptr)
{
}

void FunctionCell::setEven(bool even) {
  m_even = even;
  markRectAsDirty(bounds());
}

void FunctionCell::setFunction(Graph::Function * f) {
  m_function = f;
  markRectAsDirty(bounds());
}

void FunctionCell::setHighlighted(bool highlight) {
  m_highlighted = highlight;
  markRectAsDirty(bounds());
}

Graph::Function * FunctionCell::function() {
  return m_function;
}
