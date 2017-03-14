#include <escher/even_odd_pointer_text_cell.h>
#include <assert.h>

EvenOddPointerTextCell::EvenOddPointerTextCell(KDText::FontSize size) :
  EvenOddCell(),
  m_pointerTextView(size, (I18n::Message)0, 0.5f, 0.5f)
{
}

void EvenOddPointerTextCell::setHighlighted(bool highlight) {
  EvenOddCell::setHighlighted(highlight);
  m_pointerTextView.setBackgroundColor(backgroundColor());
}

void EvenOddPointerTextCell::setEven(bool even) {
  EvenOddCell::setEven(even);
  m_pointerTextView.setBackgroundColor(backgroundColor());
}

void EvenOddPointerTextCell::setMessage(I18n::Message title, KDColor textColor) {
  m_pointerTextView.setMessage(title);
  m_pointerTextView.setTextColor(textColor);
}

void EvenOddPointerTextCell::setAlignment(float horizontalAlignment, float verticalAlignment) {
  m_pointerTextView.setAlignment(horizontalAlignment, verticalAlignment);
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
