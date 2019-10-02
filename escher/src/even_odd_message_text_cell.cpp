#include <escher/even_odd_message_text_cell.h>
#include <assert.h>

EvenOddMessageTextCell::EvenOddMessageTextCell(const KDFont * font) :
  EvenOddCell(),
  m_messageTextView(font, (I18n::Message)0, 0.5f, 0.5f)
{
}

void EvenOddMessageTextCell::setHighlighted(bool highlight) {
  EvenOddCell::setHighlighted(highlight);
  m_messageTextView.setBackgroundColor(backgroundColor());
}

void EvenOddMessageTextCell::setEven(bool even) {
  EvenOddCell::setEven(even);
  m_messageTextView.setBackgroundColor(backgroundColor());
}

void EvenOddMessageTextCell::setMessage(I18n::Message title, KDColor textColor) {
  m_messageTextView.setMessage(title);
  m_messageTextView.setTextColor(textColor);
}

void EvenOddMessageTextCell::setAlignment(float horizontalAlignment, float verticalAlignment) {
  m_messageTextView.setAlignment(horizontalAlignment, verticalAlignment);
}

int EvenOddMessageTextCell::numberOfSubviews() const {
  return 1;
}

View * EvenOddMessageTextCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_messageTextView;
}

void EvenOddMessageTextCell::layoutSubviews(bool force) {
  KDRect boundsThis = bounds();
  m_messageTextView.setFrame(KDRect(k_horizontalMargin, 0, boundsThis.width() - 2*k_horizontalMargin, boundsThis.height()), force);
}
