#include "new_function_cell.h"
#include "assert.h"

namespace Shared {

NewFunctionCell::NewFunctionCell() :
  EvenOddCell(),
  m_pointerTextView(PointerTextView(KDText::FontSize::Large, nullptr, 0.5f, 0.5f))
{
  m_pointerTextView.setText(text());
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

const char * NewFunctionCell::text() {
  return "Ajouter une fonction";
}

}
