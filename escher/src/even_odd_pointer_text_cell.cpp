#include <escher/even_odd_pointer_text_cell.h>
#include <assert.h>

EvenOddPointerTextCell::EvenOddPointerTextCell() :
  EvenOddCell(),
  m_pointerTextView(nullptr, 0.5f, 0.5f)
{
}

void EvenOddPointerTextCell::reloadCell() {
  EvenOddCell::reloadCell();
  m_pointerTextView.setBackgroundColor(backgroundColor());
}

void EvenOddPointerTextCell::setText(const char * title, KDColor textColor) {
  m_pointerTextView.setText(title);
  m_pointerTextView.setTextColor(textColor);
}

int EvenOddPointerTextCell::numberOfSubviews() const {
  return 1;
}

View * EvenOddPointerTextCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_pointerTextView;
}

void EvenOddPointerTextCell::layoutSubviews() {
  m_pointerTextView.setFrame(bounds());
}
