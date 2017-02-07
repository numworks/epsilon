#include <escher/even_odd_buffer_text_cell.h>
#include <assert.h>

EvenOddBufferTextCell::EvenOddBufferTextCell(KDText::FontSize size, float horizontalAlignment, float verticalAlignment) :
  EvenOddCell(),
  m_bufferTextView(BufferTextView(size, horizontalAlignment, verticalAlignment))
{
}

void EvenOddBufferTextCell::reloadCell() {
  EvenOddCell::reloadCell();
  m_bufferTextView.setBackgroundColor(backgroundColor());
}

void EvenOddBufferTextCell::setText(const char * textContent) {
  m_bufferTextView.setText(textContent);
}

void EvenOddBufferTextCell::setTextColor(KDColor textColor) {
  m_bufferTextView.setTextColor(textColor);
}

int EvenOddBufferTextCell::numberOfSubviews() const {
  return 1;
}

View * EvenOddBufferTextCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_bufferTextView;
}

void EvenOddBufferTextCell::layoutSubviews() {
  m_bufferTextView.setFrame(bounds());
}
