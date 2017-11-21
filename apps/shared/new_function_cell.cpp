#include "new_function_cell.h"
#include "assert.h"

namespace Shared {

NewFunctionCell::NewFunctionCell(I18n::Message text) :
  EvenOddCell(),
  m_messageTextView(KDText::FontSize::Large, text, 0.5f, 0.5f)
{
}

void NewFunctionCell::setEven(bool even) {
  EvenOddCell::setEven(even);
  m_messageTextView.setBackgroundColor(backgroundColor());
}

void NewFunctionCell::setHighlighted(bool highlight) {
  EvenOddCell::setHighlighted(highlight);
  m_messageTextView.setBackgroundColor(backgroundColor());
}

int NewFunctionCell::numberOfSubviews() const {
  return 1;
}

View * NewFunctionCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_messageTextView;
}

void NewFunctionCell::layoutSubviews() {
  m_messageTextView.setFrame(bounds());
}

}
