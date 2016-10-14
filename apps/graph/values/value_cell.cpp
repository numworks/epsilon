#include "value_cell.h"
#include "assert.h"

namespace Graph {

ValueCell::ValueCell() :
  EvenOddCell(),
  m_bufferTextView(BufferTextView(1.0f, 0.5f))
{
}

void ValueCell::reloadCell() {
  EvenOddCell::reloadCell();
  m_bufferTextView.setBackgroundColor(backgroundColor());
}

void ValueCell::setText(const char * textContent) {
  m_bufferTextView.setText(textContent);
}


int ValueCell::numberOfSubviews() const {
  return 1;
}

View * ValueCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_bufferTextView;
}

void ValueCell::layoutSubviews() {
  m_bufferTextView.setFrame(bounds());
}

}
