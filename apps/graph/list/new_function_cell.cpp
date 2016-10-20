#include "new_function_cell.h"
#include "assert.h"

namespace Graph {

NewFunctionCell::NewFunctionCell() :
  EvenOddCell(),
  m_pointerTextView(PointerTextView("Ajouter une fonction", 0.5f, 0.5f))
{
}

void NewFunctionCell::reloadCell() {
  EvenOddCell::reloadCell();
  m_pointerTextView.setBackgroundColor(backgroundColor());
}

int NewFunctionCell::numberOfSubviews() const {
  return 1;
}

View * NewFunctionCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_pointerTextView;
}

void NewFunctionCell::layoutSubviews() {
  m_pointerTextView.setFrame(bounds());
}

}
