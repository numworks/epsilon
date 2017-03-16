#include "new_function_cell.h"
#include "assert.h"

namespace Shared {

NewFunctionCell::NewFunctionCell(I18n::Message text) :
  EvenOddCell(),
  m_pointerTextView(PointerTextView(KDText::FontSize::Large, text, 0.5f, 0.5f))
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
